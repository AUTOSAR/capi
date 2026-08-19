// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

/// ================================================================
///
/// File description:
/// ----------------
/// @file       fvm.h
/// @brief      SecOcFvm header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef __FVM_H__
#define __FVM_H__

#include "ara/com/internal/secoc/secoc_config.h"
#include "ara/com/internal/secoc/secoc_helper.h"
#include "ara/com/internal/secoc/secoc_type.h"
#include "ara/com/secoc/fvm_error_domain.h"
#include "ara/core/array.h"
#include "ara/core/map.h"
#include "ara/core/result.h"
#include "ara/core/steady_clock.h"

namespace ara {
namespace com {
namespace secoc {

/// @brief short name SecOCHelper
using SecOCHelper = ara::com::internal::secoc::SecOCHelper;

/// @brief endian function
/// @return true / false
inline bool IsLittleEndian() noexcept
{
    uint32_t _{1};
    return reinterpret_cast< uint8_t* >(&_)[0] == 1;
}

/// @brief FVContainer
/// @ref [SWS_CM_11286] -- A freshness value container to hold the length of freshness value in bits and the freshness
/// value itself as an ara::core::Array
struct FVContainer
{
    /// @ref [SWS_CM_11344] -- length in bits of the freshness value passed in FVContainer
    uint64_t length;
    /// @ref [SWS_CM_11345] -- array of bytes containing the freshness value
    /// @note depends if the container is used as an input or returning value by the method it will contain either the
    /// full freshness or truncated values
    ara::core::Array< uint8_t, internal::kCuc_8 > value;
};

/// @brief FVM
/// @ref [SWS_CM_10481] -- A freshness value management interface to be implmented by the OEM/stack vendor
/// @note To be used by the freshness value management library implementer either OEM or stack vendor. The class will
/// have a single instance in the CM
class FVM
{
public:
    /// @brief Get the Rx Freshness object
    /// @param[in] secOCFreshnessValueID the identifier of the freshness value
    /// @param[in] secOCTruncatedFreshnessValue the freshness value container with the values from the received Secured
    /// I-PDU/ message
    /// @param[in] secOCAuthVerifyAttempts the number of authentication verify attempts of this I-PDU/message since the
    /// last reception. The value is 0 for the first attempt and incremented on every unsuccessful verification attempt
    /// @return ara::core::Result<FVContainer, SecOcFvmErrc>
    /// freshness value container that holds the freshness value to be used for the calculation of the authenticator by
    /// the SecOC or recoverable error
    /// @ref [SWS_CM_11288] -- This method is used by the SecOC to obtain the current freshness value
    /// @note synchronous, reentrant
    static ara::core::Result< FVContainer, SecOcFvmErrc > GetRxFreshness(
        uint16_t secOCFreshnessValueID,
        const FVContainer& secOCTruncatedFreshnessValue,
        uint16_t secOCAuthVerifyAttempts) noexcept;
    /// @brief Get the Tx Freshness object
    /// @param[in] secOCFreshnessValueID the identifier of the freshness value
    /// @return ara::core::Result<FVContainer, SecOcFvmErrc>
    /// freshness value container that holds the freshness value to be used for the calculation of the authenticator by
    /// the SecOC or recoverable error
    /// @ref [SWS_CM_11289] -- This method is used by the SecOC to obtain the current freshness value
    /// @note synchronous, reentrant
    static ara::core::Result< FVContainer, SecOcFvmErrc > GetTxFreshness(uint16_t secOCFreshnessValueID) noexcept;
    /// @brief Initialize
    /// @return Result object -- empty/value or error
    /// no return value in case of success, kFVInitialize Failed otherwise
    /// @ref [SWS_CM_11290] -- This method initializes FVM plugin implementation
    /// @note synchronous, non-reentrant
    static ara::core::Result< void > Initialize() noexcept;

    /// @brief update fv after verification success
    /// @param[in] secOCFreshnessValueID freshness Id
    /// @param[in] secOCFreshnessValue freshness value which is used to verify pdu
    /// @return true / false
    /// @details need meet some conditions: 1.Counter mode 2.Use FVM 3.larger than before
    static bool VerificationSuccessCallout(uint16_t secOCFreshnessValueID,
                                           const FVContainer& secOCFreshnessValue) noexcept;

private:
    /// @brief FVM Impl class
    class Impl
    {
    public:
        /// @brief Impl instance func
        /// @return Impl &
        static Impl& Instance() noexcept
        {
            static Impl s_Instance;
            return s_Instance;
        }
        /// @brief FVM Impl initialize func
        /// @return Result<void>
        ara::core::Result< void > ImplInitialize() noexcept
        {
            if (configured_) {
                return ara::core::Result< void >::FromValue();
            }
            ara::core::Vector< uint16_t > const& freshnessIdSet{
                ara::com::internal::secoc::SecOCProfileManager::GetFreshnessIdSet()};
            if (freshnessIdSet.empty()) {
                ComLogError("FVM::Init No any secoc config found");
                return ara::core::Result< void >::FromError(SecOcFvmErrc::kFVInitializeFailed);
            }
            for (uint16_t const freshnessId : freshnessIdSet) {
                std::shared_ptr< ara::com::internal::secoc::SecOCProfile > profile{
                    ara::com::internal::secoc::SecOCProfileManager::GetSecOCProfileByFreshnessId(freshnessId)};
                if (profile->useFreshnessTimestamp) {
                    // Timestamp mode
                    freshnessIdToFVModeAndFreshnessLengthMappings_[freshnessId]
                        = (static_cast< uint8_t >(kTimestampMode) << internal::kCuc_8)
                          + static_cast< uint8_t >(profile->freshnessValueLength);
                } else {
                    freshnessIdToCounterRxMappings_[freshnessId] = 0;
                    // set Counter initial value to 0 for freshnessId it
                    freshnessIdToCounterMappings_[freshnessId] = 0;
                    // Counter mode
                    freshnessIdToFVModeAndFreshnessLengthMappings_[freshnessId]
                        = (static_cast< uint8_t >(kCounterMode) << internal::kCuc_8)
                          + static_cast< uint8_t >(profile->freshnessValueLength);
                }
            }
            configured_ = true;
            return ara::core::Result< void >::FromValue();
        }
        // The 10-bit freshness “0011010110” (bin) can be located in a 2 byte array and corresponds to the value:
        // “35 80” (hex). The length value is 10.
        /// @brief get tx freshness value
        /// @param[in] secOCFreshnessValueID
        /// @return ara::core::Result<FVContainer, SecOcFvmErrc>
        ara::core::Result< FVContainer, SecOcFvmErrc > ImplGetTxFreshness(uint16_t const secOCFreshnessValueID) noexcept
        {
            ara::core::Map< uint16_t, uint16_t >::iterator const it{
                freshnessIdToFVModeAndFreshnessLengthMappings_.find(secOCFreshnessValueID)};
            if (it != freshnessIdToFVModeAndFreshnessLengthMappings_.end()) {
                // 1. Get freshness value byte length and bit length, bit may not be an integer number of bytes, MSB aligned and padded with 0 at the end
                uint16_t const fvBitLength{static_cast< uint16_t >(it->second & internal::kCuc_F)};
                uint16_t const fvByteLength{SecOCHelper::CalcByteNum(fvBitLength)};
                if ((it->second >> internal::kCus_8) == kCounterMode) {
                    ara::core::Map< uint16_t, uint64_t >::iterator const it2{
                        freshnessIdToCounterMappings_.find(secOCFreshnessValueID)};
                    if (it2 != freshnessIdToCounterMappings_.end()) {
                        // 2. Get freshness value. The freshness value has already been truncated to the corresponding length when stored, first +1 count
                        uint64_t counter{static_cast< uint64_t >(it2->second + 1)};
                        uint64_t fv{counter};
                        // 3. Shift and pad with 0
                        uint8_t const fill0BitLength{
                            static_cast< uint8_t >(fvByteLength * internal::kCus_8 - fvBitLength)};
                        fv <<= fill0BitLength;
                        FVContainer c{};
                        c.length = fvBitLength;
                        uint8_t* const data{reinterpret_cast< uint8_t* >(&fv)};
                        if (c.length > internal::kCus_64) {
                            return ara::core::Result< FVContainer, SecOcFvmErrc >::FromError(
                                SecOcFvmErrc::kFVInitializeFailed);
                        }
                        // 4. Encapsulate freshness value
                        if (IsLittleEndian()) {
                            std::ignore = std::reverse_copy(data, data + fvByteLength, c.value.data());
                        } else {
                            std::ignore = std::copy(data + internal::kCus_8 - fvByteLength, data + internal::kCus_8,
                                                    c.value.data());
                        }
                        // Counter ++ regardless of sending success or failure, simplified processing. Keep the part of freshness length.
                        counter <<= (internal::kCus_64 - fvBitLength);
                        counter >>= (internal::kCus_64 - fvBitLength);
                        freshnessIdToCounterMappings_[secOCFreshnessValueID] = counter;
                        ComLogDebug("Get Tx FV for freshnessId: ", secOCFreshnessValueID, " Counter FV: ", counter);
                        return ara::core::Result< FVContainer, SecOcFvmErrc >::FromValue(c);
                    }
                    return ara::core::Result< FVContainer, SecOcFvmErrc >::FromError(SecOcFvmErrc::kFVInitializeFailed);
                }
                if ((it->second >> internal::kCus_8) == kTimestampMode) {
                    uint64_t timestamp{SecOCHelper::GetSecondValueFrom1970()};
                    // Take the low byte part of timestamp
                    timestamp <<= (internal::kCus_64 - fvBitLength);
                    timestamp >>= (internal::kCus_64 - fvBitLength);
                    ComLogDebug("Get Tx FV for freshnessId: ", secOCFreshnessValueID, " Timestamp FV: ", timestamp);
                    uint16_t const fill0BitLength{
                        static_cast< uint16_t >(fvByteLength * internal::kCus_8 - fvBitLength)};
                    timestamp <<= fill0BitLength;
                    FVContainer c{};
                    c.length = fvBitLength;
                    uint8_t* const data{reinterpret_cast< uint8_t* >(&timestamp)};
                    if (c.length > internal::kCus_64) {
                        return ara::core::Result< FVContainer, SecOcFvmErrc >::FromError(
                            SecOcFvmErrc::kFVInitializeFailed);
                    }
                    if (IsLittleEndian()) {
                        std::ignore = std::reverse_copy(data, data + fvByteLength, c.value.data());
                    } else {
                        std::ignore = std::copy(data + internal::kCus_8 - fvByteLength, data + internal::kCus_8,
                                                c.value.data());
                    }
                    return ara::core::Result< FVContainer, SecOcFvmErrc >::FromValue(c);
                }
            }
            return ara::core::Result< FVContainer, SecOcFvmErrc >::FromError(SecOcFvmErrc::kFVInitializeFailed);
        }

        /// @brief update fv after verification success
        /// @param[in] secOCFreshnessValueID freshness Id
        /// @param[in] secOCFreshnessValue freshness value which is used to verify pdu
        /// @return true / false
        /// @details need meet some conditions: 1.Counter mode 2.Use FVM 3.larger than before
        bool ImplVerificationSuccessCallout(uint16_t secOCFreshnessValueID,
                                            FVContainer const& secOCFreshnessValue) noexcept
        {
            ara::core::Map< uint16_t, uint16_t >::iterator const it{
                freshnessIdToFVModeAndFreshnessLengthMappings_.find(secOCFreshnessValueID)};
            if (it != freshnessIdToFVModeAndFreshnessLengthMappings_.end()) {
                if ((it->second >> internal::kCus_8) == kCounterMode) {
                    uint16_t const fvBitLength{static_cast< uint16_t >(it->second & internal::kCuc_F)};
                    if (secOCFreshnessValue.length == fvBitLength) {
                        ara::core::Map< uint16_t, uint64_t >::iterator const it2{
                            freshnessIdToCounterRxMappings_.find(secOCFreshnessValueID)};
                        if (it2 != freshnessIdToCounterRxMappings_.end()) {
                            // Stored verified counter or initial counter
                            uint64_t const oldCounter{static_cast< uint64_t >(it2->second)};
                            uint16_t const fvByteLength{SecOCHelper::CalcByteNum(fvBitLength)};
                            uint64_t fvValue{};
                            if (secOCFreshnessValue.length > internal::kCus_64) {
                                return false;
                            }
                            uint8_t* const ptr{reinterpret_cast< uint8_t* >(&fvValue)};
                            if (IsLittleEndian()) {
                                std::ignore = std::reverse_copy(secOCFreshnessValue.value.data(),
                                                                secOCFreshnessValue.value.data() + fvByteLength, ptr);
                            } else {
                                std::ignore = std::copy(secOCFreshnessValue.value.data(),
                                                        secOCFreshnessValue.value.data() + fvByteLength,
                                                        ptr + internal::kCus_8 - fvByteLength);
                            }
                            uint16_t const fill0BitLengthTx{
                                static_cast< uint16_t >(fvByteLength * internal::kCus_8 - fvBitLength)};
                            fvValue >>= fill0BitLengthTx;
                            ComLogDebug("Update FV for freshnessId: ", secOCFreshnessValueID,
                                        " Old couter: ", oldCounter, " New counter: ", fvValue);
                            it2->second = fvValue;
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        /// @brief get rx freshness value
        /// @param[in] secOCFreshnessValueID
        /// @param[in] secOCTruncatedFreshnessValue
        /// @param[in] secOCAuthVerifyAttempts
        /// @return ara::core::Result<FVContainer, SecOcFvmErrc>
        ara::core::Result< FVContainer, SecOcFvmErrc > ImplGetRxFreshness(
            uint16_t secOCFreshnessValueID,
            FVContainer const& secOCTruncatedFreshnessValue,
            uint16_t const secOCAuthVerifyAttempts) noexcept
        {
            std::ignore = secOCAuthVerifyAttempts;
            // Send freshness bit and byte length, may not be an integer multiple
            uint64_t const fvTxBitLength{secOCTruncatedFreshnessValue.length};
            if (fvTxBitLength > static_cast< uint64_t >(internal::kCus_64)) {
                return ara::core::Result< FVContainer, SecOcFvmErrc >::FromError(SecOcFvmErrc::kFVInitializeFailed);
            }
            uint16_t const fvTxByteLength{std::move(SecOCHelper::CalcByteNum(static_cast< int16_t >(fvTxBitLength)))};
            ara::core::Map< uint16_t, uint16_t >::iterator const it{
                freshnessIdToFVModeAndFreshnessLengthMappings_.find(secOCFreshnessValueID)};
            if (it != freshnessIdToFVModeAndFreshnessLengthMappings_.end()) {
                // Freshness bit and byte length may not be an integer multiple, need MSB alignment and pad with 0
                uint16_t const fvBitLength{static_cast< uint16_t >(it->second & internal::kCuc_F)};
                uint16_t const fvByteLength{SecOCHelper::CalcByteNum(fvBitLength)};

                // Extract the value of the sent freshness
                uint64_t fvTxValue{};
                uint8_t* const ptr{reinterpret_cast< uint8_t* >(&fvTxValue)};
                if (IsLittleEndian()) {
                    std::ignore = std::reverse_copy(secOCTruncatedFreshnessValue.value.data(),
                                                    secOCTruncatedFreshnessValue.value.data() + fvTxByteLength, ptr);
                } else {
                    std::ignore = std::copy(secOCTruncatedFreshnessValue.value.data(),
                                            secOCTruncatedFreshnessValue.value.data() + fvTxByteLength,
                                            ptr + internal::kCus_8 - fvTxByteLength);
                }
                uint16_t const fill0BitLengthTx{
                    static_cast< uint16_t >(fvTxByteLength * internal::kCus_8 - fvTxBitLength)};
                fvTxValue >>= fill0BitLengthTx;

                if ((it->second >> internal::kCus_8) == kCounterMode) {
                    ara::core::Map< uint16_t, uint64_t >::iterator const it2{
                        freshnessIdToCounterRxMappings_.find(secOCFreshnessValueID)};
                    if (it2 != freshnessIdToCounterRxMappings_.end()) {
                        // Stored verified counter or initial counter
                        uint64_t const oldCounter{static_cast< uint64_t >(it2->second)};

                        // Get the part corresponding to fvTxLength
                        uint64_t leastSignificantCounter{oldCounter << (internal::kCus_64 - fvTxBitLength)};
                        leastSignificantCounter >>= (internal::kCus_64 - fvTxBitLength);
                        uint64_t mostSignificantCounter{oldCounter << (internal::kCus_64 - fvBitLength)};
                        mostSignificantCounter >>= (internal::kCus_64 - fvBitLength + fvTxBitLength);
                        uint64_t verifyFreshnessValue{};

                        // If transmission length equals freshness length, directly use freshness value in message
                        if (fvBitLength == fvTxBitLength) {
                            // Check that freshness value is greater than last verified value TODO: Overflow handling considerations
                            if (fvTxValue <= leastSignificantCounter) {
                                ComLogError("Rx freshness value: tx value is not greater than old value.");
                                return ara::core::Result< FVContainer, SecOcFvmErrc >::FromError(
                                    SecOcFvmErrc::kUnknownError);
                            }
                            ComLogDebug("Rx freshness value: return full counter freshness value directly.");
                            return ara::core::Result< FVContainer, SecOcFvmErrc >::FromValue(
                                secOCTruncatedFreshnessValue);
                        }
                        // If transmission length is smaller than freshness length, try to calculate freshness value. Whether this logic is rigorous in the standard, no replay judgment
                        // For Counter, expect PDU's fv to be larger than saved fv
                        if (fvTxValue > leastSignificantCounter) {
                            verifyFreshnessValue = (mostSignificantCounter << fvTxBitLength) | fvTxValue;
                        } else {
                            verifyFreshnessValue = ((mostSignificantCounter + 1) << fvTxBitLength) | fvTxValue;
                        }
                        FVContainer c{};
                        c.length = fvBitLength;
                        // Shift and pad with 0
                        uint8_t const fill0BitLength{
                            static_cast< uint8_t >(fvByteLength * internal::kCus_8 - fvBitLength)};
                        verifyFreshnessValue <<= fill0BitLength;
                        ComLogDebug("Get Rx guess FV for freshnessId: ", secOCFreshnessValueID,
                                    " Counter FV: ", verifyFreshnessValue >> fill0BitLength);
                        uint8_t* const data{reinterpret_cast< uint8_t* >(&verifyFreshnessValue)};
                        if (IsLittleEndian()) {
                            std::ignore = std::reverse_copy(data, data + fvByteLength, c.value.data());
                        } else {
                            std::ignore = std::copy(data + internal::kCus_8 - fvByteLength, data + internal::kCus_8,
                                                    c.value.data());
                        }
                        return ara::core::Result< FVContainer, SecOcFvmErrc >::FromValue(c);
                    }
                    return ara::core::Result< FVContainer, SecOcFvmErrc >::FromError(SecOcFvmErrc::kFVInitializeFailed);
                }
                if (static_cast< uint8_t >(it->second >> internal::kCus_8) == kTimestampMode) {
                    uint64_t const timestamp{SecOCHelper::GetSecondValueFrom1970()};
                    // Time window set to 3s
                    uint8_t const secOCRxAcceptanceWindow{3};
                    // Get the part corresponding to fvTxLength
                    uint64_t leastSignificantTimestamp{timestamp << (internal::kCus_64 - fvTxBitLength)};
                    leastSignificantTimestamp >>= (internal::kCus_64 - fvTxBitLength);
                    uint64_t mostSignificantTimestamp{timestamp << (internal::kCus_64 - fvBitLength)};
                    mostSignificantTimestamp >>= (internal::kCus_64 - fvBitLength + fvTxBitLength);
                    uint64_t verifyFreshnessValue{};
                    // If transmission length equals freshness length, directly use freshness value in message
                    if (fvBitLength == fvTxBitLength) {
                        // Check that freshness value is greater than last verified value TODO: Overflow considerations
                        uint64_t const interval{leastSignificantTimestamp - fvTxValue};
                        if (interval >= secOCRxAcceptanceWindow) {
                            ComLogError("Rx freshness value: tx value is not in suitable window.");
                            return ara::core::Result< FVContainer, SecOcFvmErrc >::FromError(
                                SecOcFvmErrc::kFVInitializeFailed);
                        }
                        ComLogDebug("Rx freshness value: return full timestamp freshness value directly.");
                        return ara::core::Result< FVContainer, SecOcFvmErrc >::FromValue(secOCTruncatedFreshnessValue);
                    }
                    // If transmission length is smaller than freshness length, try to calculate freshness value. Whether this logic is rigorous in the standard, no replay judgment
                    // For Timestamp, expect PDU's fv to be smaller than or equal to current fv and within a time window
                    // If fvBitLength fvTxBitLength is too short, logic may have problems, easily overflow in loop
                    uint64_t windowCompare{};
                    if ((mostSignificantTimestamp | leastSignificantTimestamp) > secOCRxAcceptanceWindow) {
                        windowCompare
                            = (mostSignificantTimestamp | leastSignificantTimestamp) - secOCRxAcceptanceWindow;
                    }
                    if (((mostSignificantTimestamp | fvTxValue) >= windowCompare)
                        && ((mostSignificantTimestamp | fvTxValue)
                            <= (mostSignificantTimestamp | leastSignificantTimestamp))) {
                        verifyFreshnessValue = (mostSignificantTimestamp << fvTxBitLength) | fvTxValue;
                    } else {
                        // -1 operation may cause count overflow, resulting in high bits being all 1
                        uint64_t mostTmp{mostSignificantTimestamp - 1};
                        mostTmp <<= (internal::kCus_64 - (fvBitLength - fvTxBitLength));
                        mostTmp >>= (internal::kCus_64 - (fvBitLength - fvTxBitLength));
                        verifyFreshnessValue = (mostTmp << fvTxBitLength) | fvTxValue;
                    }
                    ComLogDebug("Get Rx guess FV for freshnessId: ", secOCFreshnessValueID,
                                " Timestamp FV: ", verifyFreshnessValue);
                    FVContainer c{};
                    c.length = fvBitLength;
                    // Shift and pad with 0
                    uint16_t const fill0BitLength{
                        static_cast< uint16_t >(fvByteLength * internal::kDefaultValue_8 - fvBitLength)};
                    verifyFreshnessValue <<= fill0BitLength;
                    uint8_t* const data{reinterpret_cast< uint8_t* >(&verifyFreshnessValue)};
                    if (IsLittleEndian()) {
                        std::ignore = std::reverse_copy(data, data + fvByteLength, c.value.data());
                    } else {
                        std::ignore = std::copy(data + internal::kCus_8 - fvByteLength, data + internal::kCus_8,
                                                c.value.data());
                    }
                    return ara::core::Result< FVContainer, SecOcFvmErrc >::FromValue(c);
                }
            }
            return ara::core::Result< FVContainer, SecOcFvmErrc >::FromError(SecOcFvmErrc::kFVInitializeFailed);
        }

    private:
        /// @brief freshness id to counter map
        ara::core::Map< uint16_t, uint64_t > freshnessIdToCounterMappings_;
        // high 8 bit: Mode 0: Counter mode 1: Timestamp mode
        // low 8 bit: freshness value length
        // freshness value length: preferably an integer multiple of 8 bits. When using Counter or Timestamp mode, do not exceed 64 bits
        // Also do not set too short, easily overflow in loop, preferably 64 bits
        /// @brief freshness id to mode nad fv length map
        ara::core::Map< uint16_t, uint16_t > freshnessIdToFVModeAndFreshnessLengthMappings_;
        /// @brief counter mode
        uint8_t const kCounterMode{};
        /// @brief timestamp mode
        uint8_t const kTimestampMode{1};
        /// @brief configured or not
        bool configured_{false};
        /// @brief For ease of testing, actually one ID can only be one end of a stream
        ara::core::Map< uint16_t, uint64_t > freshnessIdToCounterRxMappings_;

    public:
        /// @brief impl()
        Impl() noexcept = default;
        /// @brief ~Impl()
        virtual ~Impl() noexcept = default;
        /// @brief Impl()
        /// @param[in] other
        Impl(Impl const& other) noexcept = default;
        /// @brief Impl operator =
        /// @param[in] other
        /// @return Impl &
        Impl& operator=(Impl const& other) noexcept = delete;
        /// @brief Impl()
        /// @param[in] other
        Impl(Impl&& other) noexcept = default;
        /// @brief Impl operartor =
        /// @param[in] other
        /// @return Impl &
        Impl& operator=(Impl&& other) noexcept = delete;
    };
};
}  // namespace secoc
}  // namespace com
}  // namespace ara
#endif