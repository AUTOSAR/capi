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
/// @file       secoc_protocol.h
/// @brief      secoc protocol header file
/// @details
/// @date       2023-09-04
/// @author     shigang.zan
/// @version    1.2.0
///
/// ================================================================

#ifndef __SECOC_PROTOCOL_H__
#define __SECOC_PROTOCOL_H__

#include "ara/com/internal/secoc/secoc_config.h"
#include "ara/com/internal/secoc/secoc_crypto.h"
#include "ara/com/internal/secoc/secoc_helper.h"
#include "ara/com/internal/secoc/secoc_statistics.h"
#include "ara/com/internal/secoc/secoc_type.h"
#include "ara/com/secoc/fvm.h"
#include "ara/core/vector.h"
#include "nsomeip/net/nsi_message.h"

namespace ara {
namespace com {
namespace internal {
namespace secoc {

/// @brief short name FVContainer
using FVContainer = ara::com::secoc::FVContainer;
/// @brief short name FVM
using SecOCFVM = ara::com::secoc::FVM;
/// @brief short name SecOcFvmErrc
using SecOcFvmErrc = ara::com::secoc::SecOcFvmErrc;

/// @brief SecOC protocol class
class SecOCProtocol
{
public:
    /// @brief set verification status override
    /// @param[in] freshnessId
    /// @param[in] overrideStatus
    /// @param[in] numberOfMessagesToOverride
    /// @return true / false
    static bool SetVerifyStatusOverrideByFreshnessId(uint16_t const freshnessId,
                                                     OverrideStatusEnum const overrideStatus,
                                                     uint8_t const numberOfMessagesToOverride) noexcept
    {
        return SecOCProfileManager::SetVerifyStatusOverrideByFreshnessId(freshnessId, overrideStatus,
                                                                         numberOfMessagesToOverride);
    }

    /// @brief set verification status override
    /// @param[in] dataId
    /// @param[in] overrideStatus
    /// @param[in] numberOfMessagesToOverride
    /// @return true / false
    static bool SetVerifyStatusOverrideByDataId(uint16_t const dataId,
                                                OverrideStatusEnum const overrideStatus,
                                                uint8_t const numberOfMessagesToOverride) noexcept
    {
        return SecOCProfileManager::SetVerifyStatusOverrideByDataId(dataId, overrideStatus, numberOfMessagesToOverride);
    }

    /// @brief SecOC initialization
    /// @param[in] path config file path
    /// @return true / false
    static bool SecOCInitialize(ara::core::StringView const& path) noexcept
    {
        bool const loadResult{SecOCProfileManager::LoadConfig(path)};
        if (!loadResult) {
            return false;
        }
        ara::core::Result< void > const fvmResult{SecOCFVM::Initialize()};
        if (!fvmResult) {
            return false;
        }
        SecOCStatisticsManager::Initialize();
        return true;
    }

    /// @brief calc secoc additional reserve buffer size for data tx
    /// @param[in] dataId data id
    /// @param[in] headerSize header size
    /// @param[in] authSize auth size
    /// @return true / false
    static bool CalcAdditionalReserveBufferSize(uint16_t const dataId,
                                                uint16_t& headerSize,
                                                uint16_t& authSize) noexcept
    {
        std::shared_ptr< SecOCProfile > profile{SecOCProfileManager::GetSecOCProfileByDataId(dataId)};
        if (profile == nullptr) {
            ComLogFatal("No any secoc config for dataId ", dataId);
            return false;
        }
        headerSize = 0;
        switch (profile->useSecuredPduHeader) {
            case isoft::manifestreader::tps::SecuredPduHeaderEnum::kSecuredPduHeader08Bit: {
                headerSize = sizeof(uint8_t);
                break;
            }
            case isoft::manifestreader::tps::SecuredPduHeaderEnum::kSecuredPduHeader16Bit: {
                headerSize = sizeof(uint16_t);
                break;
            }
            case isoft::manifestreader::tps::SecuredPduHeaderEnum::kSecuredPduHeader32Bit: {
                headerSize = sizeof(uint32_t);
                break;
            }
            default: {
                break;
            }
        }
        authSize = (profile->authInfoTxLength + profile->freshnessValueTxLength) / ara::com::secoc::internal::kCuc_8;
        return true;
    }

    /// @brief Verify authentication info for Signal msg
    /// @param[in] dataId
    /// @param[in] message
    /// @param[in] shouldDrop drop or not
    /// @param[in] verificationResult verification result
    /// @param[in] authLength
    /// @return true/false
    static bool VerifyAuthInfo4SignalMsg(uint16_t dataId,
                                         nsi_message_t* const message,
                                         bool& shouldDrop,
                                         VerificationStatusResultEnum& verificationResult,
                                         uint16_t& authLength) noexcept
    {
        std::shared_ptr< SecOCProfile > profile{SecOCProfileManager::GetSecOCProfileByDataId(dataId)};
        if (profile == nullptr) {
            ComLogFatal("No any secoc config for dataId ", dataId);
            return false;
        }

        uint16_t buildCounter{};
        uint16_t verifyAttemp{};

        // check point of verification override
        if (profile->overrideStatus != OverrideStatusEnum::kSecOcOverrideCancel) {
            if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverrideDropUntilNotice) {
                shouldDrop         = true;
                verificationResult = VerificationStatusResultEnum::kSecOcNoVerification;
                ComLogInfo("kSecOcOverrideDropUntilNotice is set for dataId: ", dataId);
                SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId, verificationResult);
                return true;
            }
            if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverrideDropUntilLimit) {
                if (profile->currentNumberOfMessagesToOverride < profile->numberOfMessagesToOverride) {
                    profile->currentNumberOfMessagesToOverride++;
                    shouldDrop         = true;
                    verificationResult = VerificationStatusResultEnum::kSecOcNoVerification;
                    ComLogInfo("kSecOcOverrideDropUntilLimit(within range) is set for dataId: ", dataId);
                    SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                         verificationResult);
                    return true;
                }
            }
            if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverrideSkipUntilNotice) {
                shouldDrop         = false;
                verificationResult = VerificationStatusResultEnum::kSecOcNoVerification;
                ComLogInfo("kSecOcOverrideSkipUntilNotice is set for dataId: ", dataId);
                SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId, verificationResult);
                return true;
            }
            if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverrideSkipUntilLimit) {
                if (profile->currentNumberOfMessagesToOverride < profile->numberOfMessagesToOverride) {
                    profile->currentNumberOfMessagesToOverride++;
                    shouldDrop         = false;
                    verificationResult = VerificationStatusResultEnum::kSecOcNoVerification;
                    ComLogInfo("kSecOcOverrideSkipUntilLimit(within range) is set for dataId: ", dataId);
                    SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                         verificationResult);
                    return true;
                }
            }
        }
        ara::core::Vector< uint8_t > securityPayload4Auth;
        // dataId length: 2 Bytes
        uint32_t securityPayload4AuthLength{2};
        // signal payload length
        uint16_t const authInfoLength{
            static_cast< uint16_t >((profile->freshnessValueTxLength + profile->authInfoTxLength) / kBitNuMinByte)};
        authLength = authInfoLength;
        securityPayload4AuthLength += message->hdr.len - 8;  // NOLINT -- TODO[magic-numbers]
        // FV length
        // freshness value length: from authDataFreshnessLength / freshnessValueLength / none
        if (profile->useAuthDataFreshness) {
            securityPayload4AuthLength += SecOCHelper::CalcByteNum(profile->authDataFreshnessLength);
        } else {
            // freshnessValueLength may be 0
            securityPayload4AuthLength += SecOCHelper::CalcByteNum(profile->freshnessValueLength);
        }
        // malloc buffer
        securityPayload4Auth.resize(static_cast< std::size_t >(securityPayload4AuthLength));
        uint32_t position{};
        // copy dataId
        uint8_t* const dataIdPtr{reinterpret_cast< uint8_t* >(&dataId)};
        if (IsLittleEndian()) {
            std::ignore = std::reverse_copy(dataIdPtr, dataIdPtr + ara::com::secoc::internal::kCus_2,
                                            securityPayload4Auth.data());
        } else {
            std::ignore
                = std::copy(dataIdPtr, dataIdPtr + ara::com::secoc::internal::kCus_2, securityPayload4Auth.data());
        }
        position += sizeof(dataId);
        // copy someip payload
        nsi_message_io_t io;
        std::ignore = nsi_message_read_start(&io, message, 0);
        std::ignore = nsi_message_read(&io, securityPayload4Auth.data() + position,
                                       message->hdr.len - 8);  // NOLINT -- TODO[magic-numbers]
        std::ignore = nsi_message_read_end(&io);
        // adjust buffer position
        position += (message->hdr.len - 8 - authInfoLength);  // NOLINT -- TODO[magic-numbers]
        uint8_t* const authInfoPtr{securityPayload4Auth.data() + position};

        // Get mac info from payload
        ara::core::Vector< uint8_t > authTxValue;
        uint8_t macStartBit{};
        uint16_t macStartByte{};
        SecOCHelper::NewStartPosition4Stream(0, 0, profile->freshnessValueTxLength, macStartByte, macStartBit);
        SecOCHelper::CopyBitStream2ByteBuffer(authInfoPtr, macStartByte, macStartBit, profile->authInfoTxLength,
                                              authTxValue);
        // copy freshness value
        FVContainer fvContainer{};
        // store return freshness value for build auth info
        if (profile->useAuthDataFreshness) {
            // byte index of PDU      ----------0----  ------------1--------
            // bit schema             7 6 5 4 3 2 1 0  15 14 13 12 11 10 9 8
            // SecOCAuthDataFreshnessStartPosition = 11 and SecOCAuthDataFreshnessLen == 4
            // data freshness would be 11 10 9 8

            // Example:
            // The 10-bit freshness “0011010110” (bin) can be located in a 2 byte array and corresponds to the value:
            // “35 80” (hex). The length value is 10.

            uint8_t const authFvStartBit{
                static_cast< uint8_t >(profile->authDataFreshnessStartPosition % kBitNuMinByte)};
            uint16_t const authFvStartByte{
                static_cast< uint16_t >(profile->authDataFreshnessStartPosition / kBitNuMinByte)};
            if (static_cast< uint32_t >(authFvStartByte + profile->authDataFreshnessLength / kBitNuMinByte + 1)
                >= static_cast< uint32_t >(message->hdr.len - 8)) {  // NOLINT -- TODO[magic-numbers]
                ComLogError("authDataFreshnessLength/authDataFreshnessStartPosition config error");
                return false;
            }
            SecOCHelper::CopyBitStream2BytePtr(
                securityPayload4Auth.data() + 2, authFvStartByte, ara::com::secoc::internal::kCuc_7 - authFvStartBit,
                profile->authDataFreshnessLength, securityPayload4Auth.data() + position);

        } else if (profile->freshnessValueLength != 0) {
            // Call FVM to get fv according to tx fv
            ara::com::secoc::FVContainer fvTxContainer{};
            fvTxContainer.length = profile->freshnessValueTxLength;

            // Get tx fv freshness information immediately following secured payload end
            uint8_t* const fvTxPtr{authInfoPtr};
            SecOCHelper::CopyBitStream2BytePtr(fvTxPtr, 0, 0, profile->freshnessValueTxLength,
                                               fvTxContainer.value.data());

            // Call FVM to get possible fv used by sender and check fv
            while (buildCounter <= profile->authenticationBuildAttempts) {
                ara::core::Result< FVContainer, SecOcFvmErrc > const fvmResult{
                    SecOCFVM::GetRxFreshness(profile->freshnessId, fvTxContainer, 0)};
                if (!fvmResult) {
                    if (buildCounter < profile->authenticationBuildAttempts) {
                        buildCounter++;
                        continue;
                    }
                    shouldDrop         = true;
                    verificationResult = VerificationStatusResultEnum::kSecOcFreshnessFailure;
                    ComLogInfo("kSecOcFreshnessFailure for dataId: ", dataId);
                    SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                         verificationResult);
                    return true;
                }
                fvContainer = std::move(std::move(fvmResult).Value());
                break;
            }
            if (fvContainer.length != profile->freshnessValueLength) {
                ComLogError("Mismatch between profile and FVM freshness value length, dataid: ", dataId);
                return false;
            }
            // Copy fv
            uint8_t const fvByteLength{static_cast< uint8_t >(SecOCHelper::CalcByteNum(profile->freshnessValueLength))};
            std::ignore = std::copy(fvContainer.value.data(), fvContainer.value.data() + fvByteLength,
                                    securityPayload4Auth.data() + position);
        }
        // auth calculation and compare
        if (profile->jobSemantic == SecOcJobSemanticEnum::kAuthenticate) {
            ara::core::Vector< uint8_t > macOutput;
            while (buildCounter <= profile->authenticationBuildAttempts) {
#ifdef SECOC_UT_MOCK_CMAC_BY_OPENSSL
                if (!SecOCHelper::DOMacAuthenticateByOpenssl(
                        securityPayload4Auth.data(), securityPayload4AuthLength - authInfoLength,
                        profile->cryptoAlgorithm, profile->cryptoKeySlot, macOutput)) {
#else
                if (!SecOCCryptoManager::DoCryptoAuth(securityPayload4Auth.data(),
                                                      securityPayload4AuthLength - authInfoLength, macOutput, dataId)) {
#endif
                    if (buildCounter < profile->authenticationBuildAttempts) {
                        buildCounter++;
                        continue;
                    }
                    shouldDrop         = true;
                    verificationResult = VerificationStatusResultEnum::kSecOcAuthenticationBuildFailure;
                    ComLogInfo("kSecOcAuthenticationBuildFailure for dataId: ", dataId);
                    SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                         verificationResult);
                    return true;
                }
                break;
            }
            if ((macOutput.size() * kBitNuMinByte) != profile->expectedAuthValueLength) {
                ComLogError("DoMacAuthenticate mac length not expected");
                return false;
            }
            // compare tx auth and calculated auth
            bool const matched{
                SecOCHelper::MemBlockCompare(authTxValue.data(), macOutput.data(), profile->authInfoTxLength)};

            if (matched) {
                shouldDrop         = false;
                verificationResult = VerificationStatusResultEnum::kSecOcVerificationSuccess;
                ComLogInfo("kSecOcVerificationSuccess for dataId: ", dataId);
                SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId, verificationResult);
                if (profile->useAuthDataFreshness == false) {
                    if (profile->freshnessValueLength != 0) {
                        std::ignore = SecOCFVM::VerificationSuccessCallout(profile->freshnessId, fvContainer);
                    }
                }
                buildCounter = 0;
                verifyAttemp = 0;
                return true;
            }
            if (verifyAttemp < profile->authenticationRetries) {
                verifyAttemp++;
            }
            // check point of verification override
            if (profile->overrideStatus != OverrideStatusEnum::kSecOcOverrideCancel) {
                if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverridePassUntilNotice) {
                    shouldDrop         = false;
                    verificationResult = VerificationStatusResultEnum::kSecOcVerificationFailureOverwritten;
                    ComLogInfo("kSecOcOverridePassUntilNotice is set for dataId: ", dataId);
                    SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                         verificationResult);
                    return true;
                }
                if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverridePassUntilLimit) {
                    if (profile->currentNumberOfMessagesToOverride < profile->numberOfMessagesToOverride) {
                        profile->currentNumberOfMessagesToOverride++;
                        shouldDrop         = false;
                        verificationResult = VerificationStatusResultEnum::kSecOcNoVerification;
                        ComLogInfo("kSecOcOverridePassUntilLimit(within range) is set for dataId: ", dataId);
                        SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                             verificationResult);
                        return true;
                    }
                }
            }
            shouldDrop         = true;
            verificationResult = VerificationStatusResultEnum::kSecOcVerificationFailure;
            ComLogInfo("kSecOcVerificationFailure for dataId: ", dataId);
            SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId, verificationResult);
            return true;
        }
        return false;
    }

    /// @brief generate authinfo for someip msg
    /// @param[in] dataId
    /// @param[in] message
    /// @param[in] authInfo
    /// @return true/false
    static bool GenerateAuthInfo4SignalMsg(uint16_t dataId,
                                           nsi_message_t* const message,
                                           ara::core::Vector< uint8_t >& authInfo) noexcept
    {
        std::shared_ptr< SecOCProfile > profile{SecOCProfileManager::GetSecOCProfileByDataId(dataId)};
        if (profile == nullptr) {
            ComLogFatal("No any secoc config for dataId ", dataId);
            return false;
        }

        uint16_t buildCounter{};

        ara::core::Vector< uint8_t > securityPayload4Auth;
        // dataId length: 2 Bytes
        uint32_t securityPayload4AuthLength{2};
        // signal payload length
        securityPayload4AuthLength += message->hdr.len - 8;  // NOLINT -- TODO[magic-numbers]
        // FV length
        // freshness value length: from authDataFreshnessLength / freshnessValueLength / none
        if (profile->useAuthDataFreshness) {
            securityPayload4AuthLength += SecOCHelper::CalcByteNum(profile->authDataFreshnessLength);
        } else {
            // freshnessValueLength may be 0
            securityPayload4AuthLength += SecOCHelper::CalcByteNum(profile->freshnessValueLength);
        }
        // malloc buffer
        securityPayload4Auth.resize(static_cast< std::size_t >(securityPayload4AuthLength));
        uint32_t position{};
        // copy dataId
        uint8_t* const dataIdPtr{reinterpret_cast< uint8_t* >(&dataId)};
        if (IsLittleEndian()) {
            std::ignore = std::reverse_copy(dataIdPtr, dataIdPtr + ara::com::secoc::internal::kCus_2,
                                            securityPayload4Auth.data());
        } else {
            std::ignore
                = std::copy(dataIdPtr, dataIdPtr + ara::com::secoc::internal::kCus_2, securityPayload4Auth.data());
        }
        position += sizeof(dataId);
        // copy someip payload
        nsi_message_io_t io;
        std::ignore = nsi_message_read_start(&io, message, 0);
        std::ignore = nsi_message_read(&io, securityPayload4Auth.data() + position,
                                       message->hdr.len - 8);  // NOLINT -- TODO[magic-numbers]
        std::ignore = nsi_message_read_end(&io);
        position += message->hdr.len - 8;  // NOLINT -- TODO[magic-numbers]
        // copy and get FV. At the same time, store return freshness value for build auth info.
        ara::core::Vector< uint8_t > freshnessValue;
        if (profile->useAuthDataFreshness) {
            // byte index of PDU      ----------0----  ------------1--------
            // bit schema             7 6 5 4 3 2 1 0  15 14 13 12 11 10 9 8
            // SecOCAuthDataFreshnessStartPosition = 11 and SecOCAuthDataFreshnessLen == 4
            // data freshness would be 11 10 9 8

            // Example:
            // The 10-bit freshness “0011010110” (bin) can be located in a 2 byte array and corresponds to the value:
            // “35 80” (hex). The length value is 10.

            uint8_t const authFvStartBit{
                static_cast< uint8_t >(profile->authDataFreshnessStartPosition % kBitNuMinByte)};
            uint16_t const authFvStartByte{
                static_cast< uint16_t >(profile->authDataFreshnessStartPosition / kBitNuMinByte)};
            if ((authFvStartByte + SecOCHelper::CalcByteNum(profile->authDataFreshnessLength))
                >= (message->hdr.len - 8)) {  // NOLINT -- TODO[magic-numbers]
                ComLogError("authDataFreshnessLength/authDataFreshnessStartPosition config error");
                return false;
            }

            SecOCHelper::CopyBitStream2ByteBuffer(securityPayload4Auth.data() + 2, authFvStartByte,
                                                  ara::com::secoc::internal::kCuc_7 - authFvStartBit,
                                                  profile->authDataFreshnessLength, freshnessValue);

            std::ignore = std::copy(freshnessValue.data(), freshnessValue.data() + freshnessValue.size(),
                                    securityPayload4Auth.data() + position);

#ifdef SECOC_UT_DUMP_BIT
            ComLogDebug("Dump auth data FV info as below:");
            std::string dataFvOut;
            SecOCHelper::PrintBit4Vector(freshnessValue, dataFvOut);
            ComLogDebug(dataFvOut.c_str());
#endif
        } else if (profile->freshnessValueLength != 0) {
            // Call FVM to get fv
            FVContainer fvContainer;
            while (buildCounter <= profile->authenticationBuildAttempts) {
                ara::core::Result< FVContainer, SecOcFvmErrc > fvmResult{
                    SecOCFVM::GetTxFreshness(profile->freshnessId)};
                if (!fvmResult) {
                    if (buildCounter < profile->authenticationBuildAttempts) {
                        buildCounter++;
                        continue;
                    }
                    ComLogError("Meet FVM error for freshnessId %u", profile->freshnessId);
                    return false;
                }
                fvContainer = std::move(std::move(fvmResult).Value());
                break;
            }
            if (fvContainer.length != profile->freshnessValueLength) {
                ComLogError("Mismatch between profile and FVM freshness value length");
                return false;
            }
            uint16_t const fvByteLength{SecOCHelper::CalcByteNum(profile->freshnessValueLength)};
            freshnessValue.resize(static_cast< std::size_t >(fvByteLength));
            std::ignore = std::copy(fvContainer.value.data(), fvContainer.value.data() + fvByteLength,
                                    securityPayload4Auth.data() + position);
            std::ignore
                = std::copy(fvContainer.value.data(), fvContainer.value.data() + fvByteLength, freshnessValue.data());

#ifdef SECOC_UT_DUMP_BIT
            ComLogDebug("Dump FV info as below:");
            std::string fvOut;
            SecOCHelper::PrintBit4Vector(freshnessValue, fvOut);
            ComLogDebug(fvOut.c_str());
#endif
        }
        // authentication calculation
        if (profile->jobSemantic == SecOcJobSemanticEnum::kAuthenticate) {
            ara::core::Vector< uint8_t > macOutput;
#ifdef SECOC_UT_DUMP_BIT
            ComLogDebug("Dump payload info as below:");
            std::string payloadOut;
            SecOCHelper::PrintBit4Vector(securityPayload4Auth, payloadOut);
            ComLogDebug(payloadOut.c_str());
#endif
            while (buildCounter <= profile->authenticationBuildAttempts) {
#ifdef SECOC_UT_MOCK_CMAC_BY_OPENSSL
                if (!SecOCHelper::DOMacAuthenticateByOpenssl(securityPayload4Auth.data(), securityPayload4AuthLength,
                                                             profile->cryptoAlgorithm, profile->cryptoKeySlot,
                                                             macOutput)) {
#else
                if (!SecOCCryptoManager::DoCryptoAuth(securityPayload4Auth.data(), securityPayload4AuthLength,
                                                      macOutput, dataId)) {
#endif
                    if (buildCounter < profile->authenticationBuildAttempts) {
                        buildCounter++;
                        continue;
                    }
                    ComLogError("DoMacAuthenticate return error");
                    return false;
                }
                break;
            }
#ifdef SECOC_UT_DUMP_BIT
            ComLogDebug("Dump mac info as below:");
            std::string macOut;
            SecOCHelper::PrintBit4Vector(macOutput, macOut);
            ComLogDebug(macOut.c_str());
#endif
            if ((macOutput.size() * kBitNuMinByte) != profile->expectedAuthValueLength) {
                ComLogError("DoMacAuthenticate mac length not expected");
                return false;
            }
            // prepare auth info
            uint16_t const authInfoLength{
                static_cast< uint16_t >((profile->freshnessValueTxLength + profile->authInfoTxLength) / kBitNuMinByte)};
            authInfo.resize(static_cast< std::size_t >(authInfoLength));
            // LSB of fv and MSB of mac
            if (profile->freshnessValueTxLength == 0) {
                // only copy mac
                std::ignore = std::copy(macOutput.data(), macOutput.data() + authInfoLength, authInfo.data());
            } else {
                //-----------------------fv------------- -----------------------mac--------------
                // 7 6 5 4 3 2 1 0  15 14 13 12 11 10 9 8  7 6 5 4 3 2 1 0   15 14 13 12 11 10 9 8
                //     ---------- LSB(10bit)------------  -----MSB(14bit)------------------------
                //            1 0 15 14 13 12 11 10  9 8 7 6 5 4 3 2  1 0 15 14 13 12 11 10
                // copy fv and mac

                // copy fv - freshnessVlaue data  MSB byte1 byte2 ...byteN(maybe fill some 0s).
                uint16_t fvBitLength{profile->freshnessValueLength};
                uint8_t fvStartBit{};
                uint16_t fvStartByte{};
                if (profile->useAuthDataFreshness) {
                    fvBitLength = profile->authDataFreshnessLength;
                }
                SecOCHelper::NewStartPosition4Stream(0, 0, fvBitLength - profile->freshnessValueTxLength, fvStartByte,
                                                     fvStartBit);

                SecOCHelper::CopyBitStream2BitStream(freshnessValue.data(), fvStartByte, fvStartBit,
                                                     profile->freshnessValueTxLength, authInfo.data(), 0, 0);
                // copy mac
                uint16_t macStartByte{};
                uint8_t macStartBit{};
                SecOCHelper::NewStartPosition4Stream(0, 0, profile->freshnessValueTxLength, macStartByte, macStartBit);

                SecOCHelper::CopyBitStream2BitStream(macOutput.data(), 0, 0, profile->authInfoTxLength, authInfo.data(),
                                                     macStartByte, macStartBit);
            }
        } else {
            ComLogError("So far only support MAC");
            return false;
        }
        buildCounter = 0;
        return true;
    }

    /// @brief Verify authentication info for SomeIp msg
    /// @param[in] dataId
    /// @param[in] message
    /// @param[in] shouldDrop drop or not
    /// @param[in] verificationResult verification result
    /// @param[in] authLength
    /// @return true/false
    static bool VerifyAuthInfo4SomeIpMsg(uint16_t dataId,
                                         nsi_message_t* const message,
                                         bool& shouldDrop,
                                         VerificationStatusResultEnum& verificationResult,
                                         uint16_t& authLength) noexcept
    {
        std::shared_ptr< SecOCProfile > profile{SecOCProfileManager::GetSecOCProfileByDataId(dataId)};
        if (profile == nullptr) {
            ComLogFatal("No any secoc config for dataId ", dataId);
            return false;
        }
        uint16_t buildCounter{};
        uint16_t verifyAttemp{};
        // check point of verification override
        if (profile->overrideStatus != OverrideStatusEnum::kSecOcOverrideCancel) {
            if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverrideDropUntilNotice) {
                shouldDrop         = true;
                verificationResult = VerificationStatusResultEnum::kSecOcNoVerification;
                ComLogInfo("kSecOcOverrideDropUntilNotice is set for dataId: ", dataId);
                SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId, verificationResult);
                return true;
            }
            if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverrideDropUntilLimit) {
                if (profile->currentNumberOfMessagesToOverride < profile->numberOfMessagesToOverride) {
                    profile->currentNumberOfMessagesToOverride++;
                    shouldDrop         = true;
                    verificationResult = VerificationStatusResultEnum::kSecOcNoVerification;
                    ComLogInfo("kSecOcOverrideDropUntilLimit(within range) is set for dataId: ", dataId);
                    SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                         verificationResult);
                    return true;
                }
            }
            if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverrideSkipUntilNotice) {
                shouldDrop         = false;
                verificationResult = VerificationStatusResultEnum::kSecOcNoVerification;
                ComLogInfo("kSecOcOverrideSkipUntilNotice is set for dataId: ", dataId);
                SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId, verificationResult);
                return true;
            }
            if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverrideSkipUntilLimit) {
                if (profile->currentNumberOfMessagesToOverride < profile->numberOfMessagesToOverride) {
                    profile->currentNumberOfMessagesToOverride++;
                    shouldDrop         = false;
                    verificationResult = VerificationStatusResultEnum::kSecOcNoVerification;
                    ComLogInfo("kSecOcOverrideSkipUntilLimit(within range) is set for dataId: ", dataId);
                    SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                         verificationResult);
                    return true;
                }
            }
        }

        ara::core::Vector< uint8_t > securityPayload4Auth;
        // dataId length: 2 Bytes
        uint32_t securityPayload4AuthLength{2};
        // someip header partII length
        securityPayload4AuthLength += ara::com::secoc::internal::kCus_8;
        // someip payload length
        uint16_t const authInfoLength{
            static_cast< uint16_t >((profile->freshnessValueTxLength + profile->authInfoTxLength) / kBitNuMinByte)};
        authLength = authInfoLength;
        securityPayload4AuthLength += message->hdr.len - 8;  // NOLINT -- TODO[magic-numbers]
        // FV length
        securityPayload4AuthLength += SecOCHelper::CalcByteNum(profile->freshnessValueLength);
        // malloc buffer
        securityPayload4Auth.resize(static_cast< std::size_t >(securityPayload4AuthLength));
        uint32_t position{};
        // copy dataId
        uint8_t* const dataIdPtr{reinterpret_cast< uint8_t* >(&dataId)};
        if (IsLittleEndian()) {
            std::ignore = std::reverse_copy(dataIdPtr, dataIdPtr + ara::com::secoc::internal::kCus_2,
                                            securityPayload4Auth.data());
        } else {
            std::ignore
                = std::copy(dataIdPtr, dataIdPtr + ara::com::secoc::internal::kCus_2, securityPayload4Auth.data());
        }
        position += sizeof(dataId);
        // copy someip haeader part II
        // Request Id 32bit, Protocol Version 8bit, Interface Version 8bit, Message Type 8bit, Return Code 8bit.
        if (message->hdr.type == 2) {
            uint16_t clientIdZero{};
            uint8_t* const clientIdPtr{reinterpret_cast< uint8_t* >(&clientIdZero)};
            std::ignore = std::copy(clientIdPtr, clientIdPtr + ara::com::secoc::internal::kCus_2,
                                    securityPayload4Auth.data() + position);
        } else {
            uint8_t* const clientIdPtr{reinterpret_cast< uint8_t* >(&message->hdr.client)};
            if (IsLittleEndian()) {
                std::ignore = std::reverse_copy(clientIdPtr, clientIdPtr + ara::com::secoc::internal::kCus_2,
                                                securityPayload4Auth.data() + position);
            } else {
                std::ignore = std::copy(clientIdPtr, clientIdPtr + ara::com::secoc::internal::kCus_2,
                                        securityPayload4Auth.data() + position);
            }
        }
        position += sizeof(uint16_t);
        uint8_t* const sessionIdPtr{reinterpret_cast< uint8_t* >(&message->hdr.session)};
        if (IsLittleEndian()) {
            std::ignore = std::reverse_copy(sessionIdPtr, sessionIdPtr + ara::com::secoc::internal::kCus_2,
                                            securityPayload4Auth.data() + position);
        } else {
            std::ignore = std::copy(sessionIdPtr, sessionIdPtr + ara::com::secoc::internal::kCus_2,
                                    securityPayload4Auth.data() + position);
        }
        position += sizeof(uint16_t);
        *(securityPayload4Auth.data() + position) = message->hdr.protocol;
        position++;
        *(securityPayload4Auth.data() + position) = message->hdr.interface;
        position++;
        *(securityPayload4Auth.data() + position) = message->hdr.type;
        position++;
        *(securityPayload4Auth.data() + position) = message->hdr.code;
        position++;
        // copy someip payload
        nsi_message_io_t io;
        std::ignore = nsi_message_read_start(&io, message, 0);
        std::ignore = nsi_message_read(&io, securityPayload4Auth.data() + position,
                                       message->hdr.len - 8);  // NOLINT -- TODO[magic-numbers]
        std::ignore = nsi_message_read_end(&io);
        // adjust buffer position
        position += (message->hdr.len - 8 - authInfoLength);  // NOLINT -- TODO[magic-numbers]
        uint8_t* const authInfoPtr{securityPayload4Auth.data() + position};

        // Get mac info from payload
        ara::core::Vector< uint8_t > authTxValue;
        uint8_t macStartBit{};
        uint16_t macStartByte{};
        SecOCHelper::NewStartPosition4Stream(0, 0, profile->freshnessValueTxLength, macStartByte, macStartBit);
        SecOCHelper::CopyBitStream2ByteBuffer(authInfoPtr, macStartByte, macStartBit, profile->authInfoTxLength,
                                              authTxValue);
        // copy freshness value
        FVContainer fvContainer{};
        // store return freshness value for build auth info
        if (profile->freshnessValueLength != 0) {
            // Call FVM to get fv according to tx fv
            ara::com::secoc::FVContainer fvTxContainer{};
            fvTxContainer.length = profile->freshnessValueTxLength;

            // Get tx fv freshness information immediately following secured payload end
            uint8_t* const fvTxPtr{authInfoPtr};
            SecOCHelper::CopyBitStream2BytePtr(fvTxPtr, 0, 0, profile->freshnessValueTxLength,
                                               fvTxContainer.value.data());

            // Call FVM to get possible fv used by sender and check fv
            while (buildCounter <= profile->authenticationBuildAttempts) {
                ara::core::Result< FVContainer, SecOcFvmErrc > const fvmResult{
                    SecOCFVM::GetRxFreshness(profile->freshnessId, fvTxContainer, 0)};
                if (!fvmResult) {
                    if (buildCounter < profile->authenticationBuildAttempts) {
                        buildCounter++;
                        continue;
                    }
                    shouldDrop         = true;
                    verificationResult = VerificationStatusResultEnum::kSecOcFreshnessFailure;
                    ComLogInfo("kSecOcFreshnessFailure for dataId: ", dataId);
                    SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                         verificationResult);
                    return true;
                }
                fvContainer = std::move(std::move(fvmResult).Value());
                break;
            }
            if (fvContainer.length != profile->freshnessValueLength) {
                ComLogError("Mismatch between profile and FVM freshness value length, dataid: ", dataId);
                return false;
            }
            // Copy fv
            uint8_t const fvByteLength{static_cast< uint8_t >(SecOCHelper::CalcByteNum(profile->freshnessValueLength))};
            std::ignore = std::copy(fvContainer.value.data(), fvContainer.value.data() + fvByteLength,
                                    securityPayload4Auth.data() + position);
        }
        // auth calculation and compare
        if (profile->jobSemantic == SecOcJobSemanticEnum::kAuthenticate) {
            ara::core::Vector< uint8_t > macOutput;
            while (buildCounter <= profile->authenticationBuildAttempts) {
#ifdef SECOC_UT_MOCK_CMAC_BY_OPENSSL
                if (!SecOCHelper::DOMacAuthenticateByOpenssl(
                        securityPayload4Auth.data(), securityPayload4AuthLength - authInfoLength,
                        profile->cryptoAlgorithm, profile->cryptoKeySlot, macOutput)) {
#else
                if (!SecOCCryptoManager::DoCryptoAuth(securityPayload4Auth.data(),
                                                      securityPayload4AuthLength - authInfoLength, macOutput, dataId)) {
#endif
                    if (buildCounter < profile->authenticationBuildAttempts) {
                        buildCounter++;
                        continue;
                    }
                    shouldDrop         = true;
                    verificationResult = VerificationStatusResultEnum::kSecOcAuthenticationBuildFailure;
                    ComLogInfo("kSecOcAuthenticationBuildFailure for dataId: ", dataId);
                    SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                         verificationResult);
                    return true;
                }
                break;
            }
            if ((macOutput.size() * kBitNuMinByte) != profile->expectedAuthValueLength) {
                ComLogError("DoMacAuthenticate mac length not expected");
                return false;
            }
            // compare tx auth and calculated auth
            bool const matched{
                SecOCHelper::MemBlockCompare(authTxValue.data(), macOutput.data(), profile->authInfoTxLength)};

            if (matched) {
                shouldDrop         = false;
                verificationResult = VerificationStatusResultEnum::kSecOcVerificationSuccess;
                ComLogInfo("kSecOcVerificationSuccess for dataId: ", dataId);
                SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId, verificationResult);
                if (profile->useAuthDataFreshness == false) {
                    if (profile->freshnessValueLength != 0) {
                        std::ignore = SecOCFVM::VerificationSuccessCallout(profile->freshnessId, fvContainer);
                    }
                }
                buildCounter = 0;
                verifyAttemp = 0;
                return true;
            }
            if (verifyAttemp < profile->authenticationRetries) {
                verifyAttemp++;
            }
            // check point of verification override
            if (profile->overrideStatus != OverrideStatusEnum::kSecOcOverrideCancel) {
                if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverridePassUntilNotice) {
                    shouldDrop         = false;
                    verificationResult = VerificationStatusResultEnum::kSecOcVerificationFailureOverwritten;
                    ComLogInfo("kSecOcOverridePassUntilNotice is set for dataId: ", dataId);
                    SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                         verificationResult);
                    return true;
                }
                if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverridePassUntilLimit) {
                    if (profile->currentNumberOfMessagesToOverride < profile->numberOfMessagesToOverride) {
                        profile->currentNumberOfMessagesToOverride++;
                        shouldDrop         = false;
                        verificationResult = VerificationStatusResultEnum::kSecOcNoVerification;
                        ComLogInfo("kSecOcOverridePassUntilLimit(within range) is set for dataId: ", dataId);
                        SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                             verificationResult);
                        return true;
                    }
                }
            }
            shouldDrop         = true;
            verificationResult = VerificationStatusResultEnum::kSecOcVerificationFailure;
            ComLogInfo("kSecOcVerificationFailure for dataId: ", dataId);
            SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId, verificationResult);
            return true;
        }
        return false;
    }

    /// @brief generate authinfo for someip msg
    /// @param[in] dataId
    /// @param[in] message
    /// @param[in] authInfo
    /// @return true/false
    static bool GenerateAuthInfo4SomeIpMsg(uint16_t dataId,
                                           nsi_message_t* const message,
                                           ara::core::Vector< uint8_t >& authInfo) noexcept
    {
        std::shared_ptr< SecOCProfile > profile{SecOCProfileManager::GetSecOCProfileByDataId(dataId)};
        if (profile == nullptr) {
            ComLogFatal("No any secoc config for dataId ", dataId);
            return false;
        }
        uint16_t buildCounter{};
        ara::core::Vector< uint8_t > securityPayload4Auth;
        // dataId length: 2 Bytes
        uint32_t securityPayload4AuthLength{2};
        // someip header partII length
        securityPayload4AuthLength += ara::com::secoc::internal::kCus_8;
        // someip payload length
        securityPayload4AuthLength += message->hdr.len - 8;  // NOLINT -- TODO[magic-numbers]
        // FV length
        securityPayload4AuthLength += SecOCHelper::CalcByteNum(profile->freshnessValueLength);
        // malloc buffer
        securityPayload4Auth.resize(static_cast< std::size_t >(securityPayload4AuthLength));
        uint32_t position{};
        // copy dataId
        uint8_t* const dataIdPtr{reinterpret_cast< uint8_t* >(&dataId)};
        if (IsLittleEndian()) {
            std::ignore = std::reverse_copy(dataIdPtr, dataIdPtr + ara::com::secoc::internal::kCus_2,
                                            securityPayload4Auth.data());
        } else {
            std::ignore
                = std::copy(dataIdPtr, dataIdPtr + ara::com::secoc::internal::kCus_2, securityPayload4Auth.data());
        }
        position += sizeof(dataId);
        // copy someip haeader part II
        // Request Id 32bit, Protocol Version 8bit, Interface Version 8bit, Message Type 8bit, Return Code 8bit.
        if (message->hdr.type == 2) {
            uint16_t clientIdZero{};
            uint8_t* const clientIdPtr{reinterpret_cast< uint8_t* >(&clientIdZero)};
            std::ignore = std::copy(clientIdPtr, clientIdPtr + ara::com::secoc::internal::kCus_2,
                                    securityPayload4Auth.data() + position);
        } else {
            uint8_t* const clientIdPtr{reinterpret_cast< uint8_t* >(&message->hdr.client)};
            if (IsLittleEndian()) {
                std::ignore = std::reverse_copy(clientIdPtr, clientIdPtr + ara::com::secoc::internal::kCus_2,
                                                securityPayload4Auth.data() + position);
            } else {
                std::ignore = std::copy(clientIdPtr, clientIdPtr + ara::com::secoc::internal::kCus_2,
                                        securityPayload4Auth.data() + position);
            }
        }
        position += sizeof(uint16_t);
        uint8_t* const sessionIdPtr{reinterpret_cast< uint8_t* >(&message->hdr.session)};
        if (IsLittleEndian()) {
            std::ignore = std::reverse_copy(sessionIdPtr, sessionIdPtr + ara::com::secoc::internal::kCus_2,
                                            securityPayload4Auth.data() + position);
        } else {
            std::ignore = std::copy(sessionIdPtr, sessionIdPtr + ara::com::secoc::internal::kCus_2,
                                    securityPayload4Auth.data() + position);
        }
        position += sizeof(uint16_t);
        *(securityPayload4Auth.data() + position) = message->hdr.protocol;
        position++;
        *(securityPayload4Auth.data() + position) = message->hdr.interface;
        position++;
        *(securityPayload4Auth.data() + position) = message->hdr.type;
        position++;
        *(securityPayload4Auth.data() + position) = message->hdr.code;
        position++;
        // copy someip payload
        nsi_message_io_t io;
        std::ignore = nsi_message_read_start(&io, message, 0);
        std::ignore = nsi_message_read(&io, securityPayload4Auth.data() + position,
                                       message->hdr.len - 8);  // NOLINT -- TODO[magic-numbers]
        std::ignore = nsi_message_read_end(&io);
        position += message->hdr.len - 8;  // NOLINT -- TODO[magic-numbers]
        // copy and get FV
        ara::core::Vector< uint8_t > freshnessValue;
        if (profile->freshnessValueLength != 0) {
            // Call FVM to get fv
            FVContainer fvContainer;
            while (buildCounter <= profile->authenticationBuildAttempts) {
                ara::core::Result< FVContainer, SecOcFvmErrc > fvmResult{
                    SecOCFVM::GetTxFreshness(profile->freshnessId)};
                if (!fvmResult) {
                    if (buildCounter < profile->authenticationBuildAttempts) {
                        buildCounter++;
                        continue;
                    }
                    ComLogError("Meet FVM error for freshnessId %u", profile->freshnessId);
                    return false;
                }
                fvContainer = std::move(std::move(fvmResult).Value());
                break;
            }
            if (fvContainer.length != profile->freshnessValueLength) {
                ComLogError("Mismatch between profile and FVM freshness value length");
                return false;
            }
            uint16_t const fvByteLength{SecOCHelper::CalcByteNum(profile->freshnessValueLength)};
            freshnessValue.resize(static_cast< std::size_t >(fvByteLength));
            std::ignore = std::copy(fvContainer.value.data(), fvContainer.value.data() + fvByteLength,
                                    securityPayload4Auth.data() + position);
            std::ignore
                = std::copy(fvContainer.value.data(), fvContainer.value.data() + fvByteLength, freshnessValue.data());

#ifdef SECOC_UT_DUMP_BIT
            ComLogDebug("Dump FV info as below:");
            std::string fvOut;
            SecOCHelper::PrintBit4Vector(freshnessValue, fvOut);
            ComLogDebug(fvOut.c_str());
#endif
        }
        // authentication calculation
        if (profile->jobSemantic == SecOcJobSemanticEnum::kAuthenticate) {
            ara::core::Vector< uint8_t > macOutput;
#ifdef SECOC_UT_DUMP_BIT
            ComLogDebug("Dump payload info as below:");
            std::string payloadOut;
            SecOCHelper::PrintBit4Vector(securityPayload4Auth, payloadOut);
            ComLogDebug(payloadOut.c_str());
#endif
            while (buildCounter <= profile->authenticationBuildAttempts) {
#ifdef SECOC_UT_MOCK_CMAC_BY_OPENSSL
                if (!SecOCHelper::DOMacAuthenticateByOpenssl(securityPayload4Auth.data(), securityPayload4AuthLength,
                                                             profile->cryptoAlgorithm, profile->cryptoKeySlot,
                                                             macOutput)) {
#else
                if (!SecOCCryptoManager::DoCryptoAuth(securityPayload4Auth.data(), securityPayload4AuthLength,
                                                      macOutput, dataId)) {
#endif
                    if (buildCounter < profile->authenticationBuildAttempts) {
                        buildCounter++;
                        continue;
                    }
                    ComLogError("DoMacAuthenticate return error");
                    return false;
                }
                break;
            }

#ifdef SECOC_UT_DUMP_BIT
            ComLogDebug("Dump mac info as below:");
            std::string macOut;
            SecOCHelper::PrintBit4Vector(macOutput, macOut);
            ComLogDebug(macOut.c_str());
#endif
            if ((macOutput.size() * kBitNuMinByte) != profile->expectedAuthValueLength) {
                ComLogError("DoMacAuthenticate mac length not expected");
                return false;
            }
            // prepare auth info
            uint16_t const authInfoLength{
                static_cast< uint16_t >((profile->freshnessValueTxLength + profile->authInfoTxLength) / kBitNuMinByte)};
            authInfo.resize(static_cast< std::size_t >(authInfoLength));
            // LSB of fv and MSB of mac
            if (profile->freshnessValueTxLength == 0) {
                // only copy mac
                std::ignore = std::copy(macOutput.data(), macOutput.data() + authInfoLength, authInfo.data());
            } else {
                //-----------------------fv------------- -----------------------mac--------------
                // 7 6 5 4 3 2 1 0  15 14 13 12 11 10 9 8  7 6 5 4 3 2 1 0   15 14 13 12 11 10 9 8
                //     ---------- LSB(10bit)------------  -----MSB(14bit)------------------------
                //            1 0 15 14 13 12 11 10  9 8 7 6 5 4 3 2  1 0 15 14 13 12 11 10
                // copy fv and mac

                // copy fv - freshnessVlaue data  MSB byte1 byte2 ...byteN(maybe fill some 0s).
                uint16_t fvBitLength{profile->freshnessValueLength};
                uint8_t fvStartBit{};
                uint16_t fvStartByte{};
                if (profile->useAuthDataFreshness) {
                    fvBitLength = profile->authDataFreshnessLength;
                }
                SecOCHelper::NewStartPosition4Stream(0, 0, fvBitLength - profile->freshnessValueTxLength, fvStartByte,
                                                     fvStartBit);

                SecOCHelper::CopyBitStream2BitStream(freshnessValue.data(), fvStartByte, fvStartBit,
                                                     profile->freshnessValueTxLength, authInfo.data(), 0, 0);
                // copy mac
                uint16_t macStartByte{};
                uint8_t macStartBit{};
                SecOCHelper::NewStartPosition4Stream(0, 0, profile->freshnessValueTxLength, macStartByte, macStartBit);

                SecOCHelper::CopyBitStream2BitStream(macOutput.data(), 0, 0, profile->authInfoTxLength, authInfo.data(),
                                                     macStartByte, macStartBit);
            }
        } else {
            ComLogError("So far only support MAC");
            return false;
        }
        buildCounter = 0;
        return true;
    }

    ///        length/in     optional/out       tx length/out
    ///    ----------------    --------       -----------------
    ///    |    payload   |    |header|       |LSB FV|MSB Auth|
    ///    ----------------    --------       -----------------
    /// @brief Generate necessary info for SecOC TX process
    /// @param[in] dataId SecOC data Id
    /// @param[in] payload Secured PDU content
    /// @param[in] length Secured PDU content length
    /// @param[in] header Optional: SecOC header part
    /// @param[in] authInfo SecOC authentication info
    /// @return true/false
    static bool GenerateAuthInfo(uint16_t dataId,
                                 uint8_t* const payload,
                                 uint16_t length,
                                 ara::core::Vector< uint8_t >& header,
                                 ara::core::Vector< uint8_t >& authInfo) noexcept
    {
        std::shared_ptr< SecOCProfile > profile{SecOCProfileManager::GetSecOCProfileByDataId(dataId)};
        if (profile == nullptr) {
            ComLogFatal("No any secoc config for dataId ", dataId);
            return false;
        }

        uint16_t buildCounter{};

        ara::core::Vector< uint8_t > securityPayload4Auth;
        // dataId length: 2 Bytes
        uint16_t securityPayload4AuthLength{2};
        // freshness value length: from authDataFreshnessLength / freshnessValueLength / none
        if (profile->useAuthDataFreshness) {
            securityPayload4AuthLength += SecOCHelper::CalcByteNum(profile->authDataFreshnessLength);
        } else {
            // freshnessValueLength may be 0
            securityPayload4AuthLength += SecOCHelper::CalcByteNum(profile->freshnessValueLength);
        }
        // payload length: from securedAreaLength or from total length
        if (profile->securedAreaLength != 0) {
            if (profile->securedAreaLength > length) {
                ComLogError("Profile securedAreaLength/securedAreaOffset parameters error");
                return false;
            }
            if ((profile->securedAreaOffset + profile->securedAreaLength) > length) {
                ComLogError("Profile securedAreaLength/securedAreaOffset parameters error");
                return false;
            }
            securityPayload4AuthLength += profile->securedAreaLength;
        } else {
            securityPayload4AuthLength += length;
        }
        securityPayload4Auth.resize(static_cast< std::size_t >(securityPayload4AuthLength));

        // copy dataId
        uint16_t position{};
        uint8_t* const dataIdPtr{reinterpret_cast< uint8_t* >(&dataId)};
        if (IsLittleEndian()) {
            std::ignore = std::reverse_copy(dataIdPtr, dataIdPtr + ara::com::secoc::internal::kCus_2,
                                            securityPayload4Auth.data());
        } else {
            std::ignore
                = std::copy(dataIdPtr, dataIdPtr + ara::com::secoc::internal::kCus_2, securityPayload4Auth.data());
        }
        position += sizeof(dataId);

        // copy payload
        if (profile->securedAreaLength != 0) {
            std::ignore = std::copy(&payload[profile->securedAreaOffset],
                                    &payload[profile->securedAreaOffset + profile->securedAreaLength],
                                    securityPayload4Auth.data() + position);
            position += profile->securedAreaLength;
        } else {
            std::ignore = std::copy(payload, payload + length, securityPayload4Auth.data() + position);
            position += length;
        }

        // copy freshness value
        // at the same time, store return freshness value for build auth info.
        ara::core::Vector< uint8_t > freshnessValue;
        if (profile->useAuthDataFreshness) {
            // byte index of PDU      ----------0----  ------------1--------
            // bit schema             7 6 5 4 3 2 1 0  15 14 13 12 11 10 9 8
            // SecOCAuthDataFreshnessStartPosition = 11 and SecOCAuthDataFreshnessLen == 4
            // data freshness would be 11 10 9 8

            // Example:
            // The 10-bit freshness “0011010110” (bin) can be located in a 2 byte array and corresponds to the value:
            // “35 80” (hex). The length value is 10.

            uint8_t const authFvStartBit{
                static_cast< uint8_t >(profile->authDataFreshnessStartPosition % kBitNuMinByte)};
            uint16_t const authFvStartByte{
                static_cast< uint16_t >(profile->authDataFreshnessStartPosition / kBitNuMinByte)};
            if ((authFvStartByte + SecOCHelper::CalcByteNum(profile->authDataFreshnessLength)) >= length) {
                ComLogError("authDataFreshnessLength/authDataFreshnessStartPosition config error");
                return false;
            }

            SecOCHelper::CopyBitStream2ByteBuffer(payload, authFvStartByte,
                                                  ara::com::secoc::internal::kCuc_7 - authFvStartBit,
                                                  profile->authDataFreshnessLength, freshnessValue);

            std::ignore = std::copy(freshnessValue.data(), freshnessValue.data() + freshnessValue.size(),
                                    securityPayload4Auth.data() + position);

#ifdef SECOC_UT_DUMP_BIT
            ComLogDebug("Dump auth data FV info as below:");
            std::string dataFvOut;
            SecOCHelper::PrintBit4Vector(freshnessValue, dataFvOut);
            ComLogDebug(dataFvOut.c_str());
#endif

        } else if (profile->freshnessValueLength != 0) {
            // Call FVM to get fv
            FVContainer fvContainer;
            while (buildCounter <= profile->authenticationBuildAttempts) {
                ara::core::Result< FVContainer, SecOcFvmErrc > fvmResult{
                    SecOCFVM::GetTxFreshness(profile->freshnessId)};
                if (!fvmResult) {
                    if (buildCounter < profile->authenticationBuildAttempts) {
                        buildCounter++;
                        continue;
                    }
                    ComLogError("Meet FVM error for freshnessId %u", profile->freshnessId);
                    return false;
                }
                fvContainer = std::move(std::move(fvmResult).Value());
                break;
            }

            if (fvContainer.length != profile->freshnessValueLength) {
                ComLogError("Mismatch between profile and FVM freshness value length");
                return false;
            }
            uint16_t const fvByteLength{SecOCHelper::CalcByteNum(profile->freshnessValueLength)};
            freshnessValue.resize(static_cast< std::size_t >(fvByteLength));
            std::ignore = std::copy(fvContainer.value.data(), fvContainer.value.data() + fvByteLength,
                                    securityPayload4Auth.data() + position);
            std::ignore
                = std::copy(fvContainer.value.data(), fvContainer.value.data() + fvByteLength, freshnessValue.data());

#ifdef SECOC_UT_DUMP_BIT
            ComLogDebug("Dump FV info as below:");
            std::string fvOut;
            SecOCHelper::PrintBit4Vector(freshnessValue, fvOut);
            ComLogDebug(fvOut.c_str());
#endif
        } else {
            // FV is not used.
            ;
        }
        // authentication calculation
        if (profile->jobSemantic == SecOcJobSemanticEnum::kAuthenticate) {
            ara::core::Vector< uint8_t > macOutput;
#ifdef SECOC_UT_DUMP_BIT
            ComLogDebug("Dump payload info as below:");
            std::string payloadOut;
            SecOCHelper::PrintBit4Vector(securityPayload4Auth, payloadOut);
            ComLogDebug(payloadOut.c_str());
#endif
            while (buildCounter <= profile->authenticationBuildAttempts) {
#ifdef SECOC_UT_MOCK_CMAC_BY_OPENSSL
                if (!SecOCHelper::DOMacAuthenticateByOpenssl(securityPayload4Auth.data(), securityPayload4AuthLength,
                                                             profile->cryptoAlgorithm, profile->cryptoKeySlot,
                                                             macOutput)) {
#else
                if (!SecOCCryptoManager::DoCryptoAuth(securityPayload4Auth.data(), securityPayload4AuthLength,
                                                      macOutput, dataId)) {
#endif
                    if (buildCounter < profile->authenticationBuildAttempts) {
                        buildCounter++;
                        continue;
                    }
                    ComLogError("DoMacAuthenticate return error");
                    return false;
                }
                break;
            }
#ifdef SECOC_UT_DUMP_BIT
            ComLogDebug("Dump mac info as below:");
            std::string macOut;
            SecOCHelper::PrintBit4Vector(macOutput, macOut);
            ComLogDebug(macOut.c_str());
#endif
            if ((macOutput.size() * kBitNuMinByte) != profile->expectedAuthValueLength) {
                ComLogError("DoMacAuthenticate mac length not expected");
                return false;
            }
            // prepare auth info
            uint16_t const authInfoLength{
                static_cast< uint16_t >((profile->freshnessValueTxLength + profile->authInfoTxLength) / kBitNuMinByte)};
            authInfo.resize(static_cast< std::size_t >(authInfoLength));
            // LSB of fv and MSB of mac
            if (profile->freshnessValueTxLength == 0) {
                // only copy mac
                std::ignore = std::copy(macOutput.data(), macOutput.data() + authInfoLength, authInfo.data());
            } else {
                //-----------------------fv------------- -----------------------mac--------------
                // 7 6 5 4 3 2 1 0  15 14 13 12 11 10 9 8  7 6 5 4 3 2 1 0   15 14 13 12 11 10 9 8
                //     ---------- LSB(10bit)------------  -----MSB(14bit)------------------------
                //            1 0 15 14 13 12 11 10  9 8 7 6 5 4 3 2  1 0 15 14 13 12 11 10
                // copy fv and mac

                // copy fv - freshnessVlaue data  MSB byte1 byte2 ...byteN(maybe fill some 0s).
                uint16_t fvBitLength{profile->freshnessValueLength};
                uint8_t fvStartBit{};
                uint16_t fvStartByte{};
                if (profile->useAuthDataFreshness) {
                    fvBitLength = profile->authDataFreshnessLength;
                }
                SecOCHelper::NewStartPosition4Stream(0, 0, fvBitLength - profile->freshnessValueTxLength, fvStartByte,
                                                     fvStartBit);

                SecOCHelper::CopyBitStream2BitStream(freshnessValue.data(), fvStartByte, fvStartBit,
                                                     profile->freshnessValueTxLength, authInfo.data(), 0, 0);
                // copy mac
                uint16_t macStartByte{};
                uint8_t macStartBit{};
                SecOCHelper::NewStartPosition4Stream(0, 0, profile->freshnessValueTxLength, macStartByte, macStartBit);

                SecOCHelper::CopyBitStream2BitStream(macOutput.data(), 0, 0, profile->authInfoTxLength, authInfo.data(),
                                                     macStartByte, macStartBit);
            }
        } else {
            ComLogError("So far only support MAC");
            return false;
        }
        // ship necessary data - optional header
        if (profile->useSecuredPduHeader != isoft::manifestreader::tps::SecuredPduHeaderEnum::kNoHeader) {
            switch (profile->useSecuredPduHeader) {
                case isoft::manifestreader::tps::SecuredPduHeaderEnum::kSecuredPduHeader08Bit: {
                    header.resize(sizeof(uint8_t));
                    *header.data() = static_cast< uint8_t >(length);
                    break;
                }
                case isoft::manifestreader::tps::SecuredPduHeaderEnum::kSecuredPduHeader16Bit: {
                    header.resize(sizeof(uint16_t));
                    uint8_t* const data{reinterpret_cast< uint8_t* >(&length)};
                    if (IsLittleEndian()) {
                        std::ignore = std::reverse_copy(data, data + sizeof(uint16_t), header.data());
                    } else {
                        std::ignore = std::copy(data, data + sizeof(uint16_t), header.data());
                    }
                    break;
                }
                case isoft::manifestreader::tps::SecuredPduHeaderEnum::kSecuredPduHeader32Bit: {
                    header.resize(sizeof(uint32_t));
                    uint32_t tmpLength{length};
                    uint8_t* const data{reinterpret_cast< uint8_t* >(&tmpLength)};
                    if (IsLittleEndian()) {
                        std::ignore = std::reverse_copy(data, data + sizeof(uint16_t), header.data());
                    } else {
                        std::ignore = std::copy(data, data + sizeof(uint32_t), header.data());
                    }
                    break;
                }
                case isoft::manifestreader::tps::SecuredPduHeaderEnum::kNoHeader:
                default: {
                    break;
                }
            }
        }
        buildCounter = 0;
        return true;
    };

    /// Scenario 1:
    ///                  length
    ///    ----------------------------------
    ///    |    payload     |LSB FV|MSB Auth|    FV may be empty
    ///    ----------------------------------
    /// Scenario 2:
    ///                  length
    ///    ----------------------------------
    ///    |header| payload |LSB FV|MSB Auth|    FV may be empty
    ///    ----------------------------------

    /// @brief Verify authentication info for SecOC Rx process
    /// @param[in] dataId SecOC data Id
    /// @param[in] payload Complete PDU content to verify  optional header | secured payload | auth info
    /// @param[in] length  Complete PDU content length
    /// @param[in] securedPayload Secured payload pointer
    /// @param[in] securedPayloadLength Secured payload length
    /// @param[in] shouldDrop drop or not
    /// @param[in] verificationResult verification result
    /// @return true/false
    static bool VerifyAuthInfo(uint16_t dataId,
                               uint8_t* const payload,
                               uint16_t const length,
                               uint8_t*& securedPayload,
                               uint16_t& securedPayloadLength,
                               bool& shouldDrop,
                               VerificationStatusResultEnum& verificationResult) noexcept
    {
        std::shared_ptr< SecOCProfile > profile{SecOCProfileManager::GetSecOCProfileByDataId(dataId)};
        if (profile == nullptr) {
            ComLogFatal("No any secoc config for dataId ", dataId);
            return false;
        }

        uint16_t buildCounter{};
        uint16_t verifyAttemp{};

        uint8_t* payloadPtr{payload};
        uint8_t* authInfoPtr{};
        uint16_t const authInfoLength{
            static_cast< uint16_t >((profile->authInfoTxLength + profile->freshnessValueTxLength) / kBitNuMinByte)};
        uint16_t payloadLength{static_cast< uint16_t >(length - authInfoLength)};
        // process optional header
        if (profile->useSecuredPduHeader != isoft::manifestreader::tps::SecuredPduHeaderEnum::kNoHeader) {
            uint8_t headerLength{};
            switch (profile->useSecuredPduHeader) {
                case isoft::manifestreader::tps::SecuredPduHeaderEnum::kSecuredPduHeader08Bit: {
                    payloadLength = *payload;
                    headerLength  = sizeof(uint8_t);
                    break;
                }
                case isoft::manifestreader::tps::SecuredPduHeaderEnum::kSecuredPduHeader16Bit: {
                    uint8_t* const data{reinterpret_cast< uint8_t* >(&payloadLength)};
                    if (IsLittleEndian()) {
                        std::ignore = std::reverse_copy(payload, payload + sizeof(uint16_t), data);
                    } else {
                        std::ignore = std::copy(payload, payload + sizeof(uint16_t), data);
                    }
                    headerLength = sizeof(uint16_t);
                    break;
                }
                case isoft::manifestreader::tps::SecuredPduHeaderEnum::kSecuredPduHeader32Bit: {
                    uint32_t tmpLength{};
                    uint8_t* const data{reinterpret_cast< uint8_t* >(&tmpLength)};
                    if (IsLittleEndian()) {
                        std::ignore = std::reverse_copy(payload, payload + sizeof(uint32_t), data);
                    } else {
                        std::ignore = std::copy(payload, payload + sizeof(uint32_t), data);
                    }
                    payloadLength = static_cast< uint16_t >(tmpLength);
                    headerLength  = sizeof(uint32_t);
                    break;
                }
                case isoft::manifestreader::tps::SecuredPduHeaderEnum::kNoHeader:
                default: {
                    break;
                }
            }
            payloadPtr += headerLength;
            // Not yet sure if there is any situation where the content within payload is not aligned
            if ((payloadLength + headerLength + authInfoLength) != length) {
                ComLogError("Payload content not correct");
                return false;
            }
        } else {
            payloadLength = length - authInfoLength;
        }
        authInfoPtr          = payloadPtr + payloadLength;
        securedPayload       = payloadPtr;
        securedPayloadLength = payloadLength;

        // check point of verification override
        if (profile->overrideStatus != OverrideStatusEnum::kSecOcOverrideCancel) {
            if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverrideDropUntilNotice) {
                shouldDrop         = true;
                verificationResult = VerificationStatusResultEnum::kSecOcNoVerification;
                ComLogInfo("kSecOcOverrideDropUntilNotice is set for dataId: ", dataId);
                SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId, verificationResult);
                return true;
            }
            if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverrideDropUntilLimit) {
                if (profile->currentNumberOfMessagesToOverride < profile->numberOfMessagesToOverride) {
                    profile->currentNumberOfMessagesToOverride++;
                    shouldDrop         = true;
                    verificationResult = VerificationStatusResultEnum::kSecOcNoVerification;
                    ComLogInfo("kSecOcOverrideDropUntilLimit(within range) is set for dataId: ", dataId);
                    SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                         verificationResult);
                    return true;
                }
            }
            if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverrideSkipUntilNotice) {
                shouldDrop         = false;
                verificationResult = VerificationStatusResultEnum::kSecOcNoVerification;
                ComLogInfo("kSecOcOverrideSkipUntilNotice is set for dataId: ", dataId);
                SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId, verificationResult);
                return true;
            }
            if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverrideSkipUntilLimit) {
                if (profile->currentNumberOfMessagesToOverride < profile->numberOfMessagesToOverride) {
                    profile->currentNumberOfMessagesToOverride++;
                    shouldDrop         = false;
                    verificationResult = VerificationStatusResultEnum::kSecOcNoVerification;
                    ComLogInfo("kSecOcOverrideSkipUntilLimit(within range) is set for dataId: ", dataId);
                    SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                         verificationResult);
                    return true;
                }
            }
        }

        // Construct same security context to generate mac info
        ara::core::Vector< uint8_t > securityPayload4Auth;
        // dataId length
        uint16_t securityPayload4AuthLength{sizeof(uint16_t)};
        // freshness value length
        if (profile->useAuthDataFreshness) {
            securityPayload4AuthLength += SecOCHelper::CalcByteNum(profile->authDataFreshnessLength);
        } else {
            securityPayload4AuthLength += SecOCHelper::CalcByteNum(profile->freshnessValueLength);
        }
        // payload length
        if (profile->securedAreaLength != 0) {
            if (profile->securedAreaLength > payloadLength) {
                ComLogError("Profile securedAreaLength/securedAreaOffset parameters error");
                return false;
            }
            if ((profile->securedAreaOffset + profile->securedAreaLength) > payloadLength) {
                ComLogError("Profile securedAreaLength/securedAreaOffset parameters error");
                return false;
            }
            securityPayload4AuthLength += profile->securedAreaLength;
        } else {
            securityPayload4AuthLength += payloadLength;
        }
        securityPayload4Auth.resize(static_cast< std::size_t >(securityPayload4AuthLength));

        // copy dataId
        uint16_t position{};
        uint8_t* const dataIdPtr{reinterpret_cast< uint8_t* >(&dataId)};
        if (IsLittleEndian()) {
            std::ignore = std::reverse_copy(dataIdPtr, dataIdPtr + sizeof(dataId), securityPayload4Auth.data());
        } else {
            std::ignore = std::copy(dataIdPtr, dataIdPtr + sizeof(dataId), securityPayload4Auth.data());
        }
        position += sizeof(dataId);

        // copy payload
        if (profile->securedAreaLength != 0) {
            std::ignore = std::copy(&payloadPtr[profile->securedAreaOffset],
                                    &payloadPtr[profile->securedAreaOffset + profile->securedAreaLength],
                                    securityPayload4Auth.data() + position);
            position += profile->securedAreaLength;
        } else {
            std::ignore = std::copy(payloadPtr, payloadPtr + payloadLength, securityPayload4Auth.data() + position);
            position += payloadLength;
        }

        // copy freshness value
        FVContainer fvContainer{};
        // store return freshness value for build auth info
        if (profile->useAuthDataFreshness) {
            // byte index of PDU      ----------0----  ------------1--------
            // bit schema             7 6 5 4 3 2 1 0  15 14 13 12 11 10 9 8
            // SecOCAuthDataFreshnessStartPosition = 11 and SecOCAuthDataFreshnessLen == 4
            // data freshness would be 11 10 9 8

            // Example:
            // The 10-bit freshness “0011010110” (bin) can be located in a 2 byte array and corresponds to the value:
            // “35 80” (hex). The length value is 10.

            uint8_t const authFvStartBit{
                static_cast< uint8_t >(profile->authDataFreshnessStartPosition % kBitNuMinByte)};
            uint16_t const authFvStartByte{
                static_cast< uint16_t >(profile->authDataFreshnessStartPosition / kBitNuMinByte)};
            if ((authFvStartByte + profile->authDataFreshnessLength / kBitNuMinByte + 1) >= payloadLength) {
                ComLogError("authDataFreshnessLength/authDataFreshnessStartPosition config error");
                return false;
            }
            SecOCHelper::CopyBitStream2BytePtr(
                payloadPtr, authFvStartByte, ara::com::secoc::internal::kCuc_7 - authFvStartBit,
                profile->authDataFreshnessLength, securityPayload4Auth.data() + position);

        } else if (profile->freshnessValueLength != 0) {
            // Call FVM to get fv according to tx fv
            ara::com::secoc::FVContainer fvTxContainer{};
            fvTxContainer.length = profile->freshnessValueTxLength;

            // Get tx fv freshness information immediately following secured payload end
            uint8_t* const fvTxPtr{authInfoPtr};
            SecOCHelper::CopyBitStream2BytePtr(fvTxPtr, 0, 0, profile->freshnessValueTxLength,
                                               fvTxContainer.value.data());

            // Call FVM to get possible fv used by sender and check fv
            while (buildCounter <= profile->authenticationBuildAttempts) {
                ara::core::Result< FVContainer, SecOcFvmErrc > const fvmResult{
                    SecOCFVM::GetRxFreshness(profile->freshnessId, fvTxContainer, 0)};
                if (!fvmResult) {
                    if (buildCounter < profile->authenticationBuildAttempts) {
                        buildCounter++;
                        continue;
                    }
                    shouldDrop         = true;
                    verificationResult = VerificationStatusResultEnum::kSecOcFreshnessFailure;
                    ComLogInfo("kSecOcFreshnessFailure for dataId: ", dataId);
                    SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                         verificationResult);
                    return true;
                }
                fvContainer = std::move(std::move(fvmResult).Value());
                break;
            }
            if (fvContainer.length != profile->freshnessValueLength) {
                ComLogError("Mismatch between profile and FVM freshness value length, dataid: ", dataId);
                return false;
            }
            // Copy fv
            uint8_t const fvByteLength{static_cast< uint8_t >(SecOCHelper::CalcByteNum(profile->freshnessValueLength))};
            std::ignore = std::copy(fvContainer.value.data(), fvContainer.value.data() + fvByteLength,
                                    securityPayload4Auth.data() + position);
        } else {
            // fv is not used
            ;
        }
        // authentication calculation
        if (profile->jobSemantic == SecOcJobSemanticEnum::kAuthenticate) {
            ara::core::Vector< uint8_t > macOutput;
            while (buildCounter <= profile->authenticationBuildAttempts) {
#ifdef SECOC_UT_MOCK_CMAC_BY_OPENSSL
                if (!SecOCHelper::DOMacAuthenticateByOpenssl(securityPayload4Auth.data(), securityPayload4AuthLength,
                                                             profile->cryptoAlgorithm, profile->cryptoKeySlot,
                                                             macOutput)) {
#else
                if (!SecOCCryptoManager::DoCryptoAuth(securityPayload4Auth.data(), securityPayload4AuthLength,
                                                      macOutput, dataId)) {
#endif
                    if (buildCounter < profile->authenticationBuildAttempts) {
                        buildCounter++;
                        continue;
                    }
                    shouldDrop         = true;
                    verificationResult = VerificationStatusResultEnum::kSecOcAuthenticationBuildFailure;
                    ComLogInfo("kSecOcAuthenticationBuildFailure for dataId: ", dataId);
                    SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                         verificationResult);
                    return true;
                }
                break;
            }

            if ((macOutput.size() * kBitNuMinByte) != profile->expectedAuthValueLength) {
                ComLogError("DoMacAuthenticate mac length not expected");
                return false;
            }
            // Get mac info from payload
            uint8_t macStartBit{};
            uint16_t macStartByte{};
            SecOCHelper::NewStartPosition4Stream(0, 0, profile->freshnessValueTxLength, macStartByte, macStartBit);
            ara::core::Vector< uint8_t > authTxValue;
            SecOCHelper::CopyBitStream2ByteBuffer(authInfoPtr, macStartByte, macStartBit, profile->authInfoTxLength,
                                                  authTxValue);
            // compare tx auth and calculated auth
            bool const matched{
                SecOCHelper::MemBlockCompare(authTxValue.data(), macOutput.data(), profile->authInfoTxLength)};

            if (matched) {
                shouldDrop         = false;
                verificationResult = VerificationStatusResultEnum::kSecOcVerificationSuccess;
                ComLogInfo("kSecOcVerificationSuccess for dataId: ", dataId);
                SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId, verificationResult);
                if (profile->useAuthDataFreshness == false) {
                    if (profile->freshnessValueLength != 0) {
                        std::ignore = SecOCFVM::VerificationSuccessCallout(profile->freshnessId, fvContainer);
                    }
                }
                buildCounter = 0;
                return true;
            }
            buildCounter = 0;
            if (verifyAttemp < profile->authenticationRetries) {
                verifyAttemp++;
            }
            // check point of verification override
            if (profile->overrideStatus != OverrideStatusEnum::kSecOcOverrideCancel) {
                if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverridePassUntilNotice) {
                    shouldDrop         = false;
                    verificationResult = VerificationStatusResultEnum::kSecOcVerificationFailureOverwritten;
                    ComLogInfo("kSecOcOverridePassUntilNotice is set for dataId: ", dataId);
                    SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                         verificationResult);
                    return true;
                }
                if (profile->overrideStatus == OverrideStatusEnum::kSecOcOverridePassUntilLimit) {
                    if (profile->currentNumberOfMessagesToOverride < profile->numberOfMessagesToOverride) {
                        profile->currentNumberOfMessagesToOverride++;
                        shouldDrop         = false;
                        verificationResult = VerificationStatusResultEnum::kSecOcNoVerification;
                        ComLogInfo("kSecOcOverridePassUntilLimit(within range) is set for dataId: ", dataId);
                        SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId,
                                                                             verificationResult);
                        return true;
                    }
                }
            }
            shouldDrop         = true;
            verificationResult = VerificationStatusResultEnum::kSecOcVerificationFailure;
            ComLogInfo("kSecOcVerificationFailure for dataId: ", dataId);
            SecOCStatisticsManager::SetVerificationEventSnapshot(dataId, profile->freshnessId, verificationResult);
            return true;
        }
        return false;
    }

public:
    /// @brief disable
    ~SecOCProtocol() noexcept = delete;
    /// @brief disable
    SecOCProtocol() noexcept = delete;
    /// @brief disable
    /// @param[in] other
    SecOCProtocol(SecOCProtocol const& other) noexcept = delete;
    /// @brief disable
    /// @param[in] other
    /// @return none
    SecOCProtocol& operator=(SecOCProtocol const& other) noexcept = delete;
    /// @brief disable
    /// @param[in] other
    SecOCProtocol(SecOCProtocol const&& other) noexcept = delete;
    /// @brief disable
    /// @param[in] other
    /// @return none
    SecOCProtocol& operator=(SecOCProtocol const&& other) noexcept = delete;
};

}  // namespace secoc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif