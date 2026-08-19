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
/// @file       secoc_config.h
/// @brief      secoc config header file
/// @details
/// @date       2023-09-04
/// @author     shigang.zan
/// @version    1.2.0
///
/// ================================================================

#ifndef __SECOC_CONFIG_H__
#define __SECOC_CONFIG_H__

#include <mutex>

#include "ara/com/internal/log/log.h"
#include "ara/com/internal/secoc/secoc_crypto.h"
#include "ara/com/internal/secoc/secoc_type.h"
#include "ara/core/array.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/map.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/vector.h"
#include "isoft/manifestreader/manifest.h"
#include "isoft/manifestreader/manifest_node.h"
#include "isoft/manifestreader/manifest_reader.h"
#include "isoft/manifestreader/tps_enumeration.h"

namespace ara {
namespace com {
namespace internal {
namespace secoc {

/// @brief byte endian func
/// @return true/false
inline bool IsLittleEndian() noexcept
{
    uint32_t _{1};
    return reinterpret_cast< uint8_t* >(&_)[0] == 1;
}

/// @brief Auth algorithm type. mac or signature.
enum class SecOcJobSemanticEnum : uint8_t
{
    // Authentication algorithm for Authenticator generation/verification.
    kAuthenticate = 0,
    // Asymmetric cryptographic algorithm to generate/verify a signature
    kVerify = 1
};

/// @brief SecOC override status enum
enum class OverrideStatusEnum : uint8_t
{
    // Until further notice, authenticator verification is not performed, PDU is dropped, verification result is set to
    // kSecOcNoVerification.
    kSecOcOverrideDropUntilNotice = 0x00,
    // Until NumberOfMessagesToOverride is reached, authenticator verification is not performed, PDU is dropped,
    // verification result is set to kSecOcNoVerification.
    kSecOcOverrideDropUntilLimit = 0x01,
    // Cancel Override of VerifyStatus.
    kSecOcOverrideCancel = 0x02,
    // Until further notice, authenticator verification is performed, PDU is forwarded to the application independent of
    // verification result, verification result is set to kSecOcVerificationFailureOverwritten in case of failed
    // verification.
    kSecOcOverridePassUntilNotice = 0x40,
    // Until NumberOfMessagesToOverride is reached, authenticator verification is not performed, PDU is sent to the
    // application, verification result is set to kSecOcNoVerification.
    kSecOcOverrideSkipUntilLimit = 0x41,
    // Until NumberOfMessagesToOverride is reached, authenticator verification is performed, PDU is sent to the
    // application independent of verification result, verification result is set to kSecOcVerificationFailure
    // Overwritten in case of failed verification.
    kSecOcOverridePassUntilLimit = 0x42,
    // Until further notice, authenticator verification is not performed, PDU is sent to the application, verification
    // result is set to kSecOcNoVerification.
    kSecOcOverrideSkipUntilNotice = 0x43
};

/// @brief SecOC profile class
class SecOCProfile final
{
public:
    /// @brief data ID
    /// @details This attribute defines a numerical identifier for the
    ///    Secured I-PDU.
    uint16_t dataId{ara::com::secoc::internal::kDefaultValue_0};

    /// @brief freshness ID
    /// @details This attribute defines the Id of the Freshness Value. The
    ///    Freshness Value might be a normal counter or a time
    ///    value.
    uint16_t freshnessId{ara::com::secoc::internal::kDefaultValue_0};

    /// @brief complete freshness value length
    /// @details This attribute defines the complete length in bits of the
    ///    Freshness Value. As long as the key doesn’t change the
    ///    counter shall not overflow. The length of the counter shall
    ///    be determined based on the expected life time of the
    ///    corresponding key and frequency of usage of the counter.
    uint16_t freshnessValueLength{ara::com::secoc::internal::kDefaultValue_64};

    /// @brief freshness value truncate length
    /// @details This attribute defines the length in bits of the Freshness
    ///    Value to be included in the payload of the Secured I-PDU.
    ///    This length is specific to the least significant bits of the
    ///    complete Freshness Counter. If the attribute is 0 no
    ///    Freshness Value is included in the Secured I-PDU
    uint16_t freshnessValueTxLength{ara::com::secoc::internal::kDefaultValue_0};

    /// @brief auth info truncate length
    /// @details This attribute defines the length in bits of the
    ///    authentication code to be included in the payload of the
    ///    authenticated Pdu.
    uint16_t authInfoTxLength{ara::com::secoc::internal::kDefaultValue_0};

    /// @brief freshness value length from auth data
    /// @details This attribute defines the length in bits of the authentic
    ///    PDU data that is passed to the SWC that verifies and generates
    ///    the Freshness.
    uint16_t authDataFreshnessLength{ara::com::secoc::internal::kDefaultValue_0};

    /// @brief freshness value start position of auth data
    /// @details This value determines the start position in bits of the
    ///    Authentic PDU that shall be passed on to the SWC that
    ///    verifies and generates the Freshness. The bit counting is
    ///    done according to TPS_SYST_01068.
    uint16_t authDataFreshnessStartPosition{ara::com::secoc::internal::kDefaultValue_0};

    /// @brief secured area length in payload
    /// @details This attribute defines the length in bytes of the area within
    ///    the payload Pdu which will be secured.
    uint16_t securedAreaLength{ara::com::secoc::internal::kDefaultValue_0};

    /// @brief secured area offset in payload
    /// @details This attribute defines the start position (offset in byte) of
    ///    the area within the payload Pdu which will be secured.
    uint16_t securedAreaOffset{ara::com::secoc::internal::kDefaultValue_0};

    /// @brief authentication build attemp times
    /// @details This attribute specifies the number of authentication build
    ///    attempts.
    uint16_t authenticationBuildAttempts{ara::com::secoc::internal::kDefaultValue_1};

    /// @brief authentication verification retry times
    /// @details This attribute defines the additional number of
    ///    authentication attempts that are to be carried out when
    ///    the generation of the authentication information failed for
    ///    a given SecuredIPdu. If zero is set than only one
    ///    authentication attempt is done.
    uint16_t authenticationRetries{ara::com::secoc::internal::kDefaultValue_1};

    /// @brief Set by method, not from configuration file
    uint16_t numberOfMessagesToOverride{ara::com::secoc::internal::kDefaultValue_0};

    /// @brief internal var
    uint16_t currentNumberOfMessagesToOverride{ara::com::secoc::internal::kDefaultValue_0};

    /// @brief internal Mac result length
    uint16_t expectedAuthValueLength{ara::com::secoc::internal::kDefaultValue_128};

    /// @brief use timestamp or counter as FV
    /// @details This attribute specifies whether the Freshness Value is
    ///    generated through individual Freshness Counters or by a
    ///    Timestamps. The value is set to TRUE when Timestamps
    ///    are used.
    bool useFreshnessTimestamp{true};

    /// @brief use auth data as fv
    /// @details This attribute describes whether a part of AuthenticPdu
    ///    contained in a SecuredIPdu shall be passed on to the
    ///    SWC that verifies and generates the Freshness. The part
    ///    of the Authentic-PDU is defined by the authData
    ///    FreshnessStartPosition and authDataFreshnessLength.
    bool useAuthDataFreshness{false};

    /// @brief optional secoc header
    /// @details This attribute defines the size of the header which is
    ///    inserted into the SecuredIPdu. If this attribute is set to
    ///    anything but noHeader, the SecuredIPdu contains the
    ///    Secured I-PDU Header to indicate the length of the
    ///    AuthenticIPdu. The AuthenticIPdu contains the original
    ///    payload, i.e. the secured data.
    isoft::manifestreader::tps::SecuredPduHeaderEnum useSecuredPduHeader{
        isoft::manifestreader::tps::SecuredPduHeaderEnum::kNoHeader};

    /// @brief Set by method, not from configuration file
    OverrideStatusEnum overrideStatus{OverrideStatusEnum::kSecOcOverrideCancel};

    /// @brief auth algorithm mac or signature
    SecOcJobSemanticEnum jobSemantic{SecOcJobSemanticEnum::kAuthenticate};

    // Authentication algorithm (e.g. MacAescbc_256). Set by string type authAlgorithm or by meta-model
    // CryptoServicePrimitive (algorithmFamily,algorithmMode,algorithmSecondaryFamily).
    /// @brief auth algorithm string name
    ara::core::String authAlgorithm{};
    /// @brief algorithm family
    ara::core::String algorithmFamily{};
    /// @brief algorithm mode
    ara::core::String algorithmMode{};
    /// @brief algorithm second family
    ara::core::String algorithmSecondaryFamily{};
    // Default category settings by AUTOSAR. PROFILE_01, PROFILE_02 and PROFILE_03.
    // PROFILE_01: algorithmFamily="CRYPTO_ALGOFAM_AES"
    //            algorithmMode="CRYPTO_ALGOMODE_CMAC"
    //            algorithmSecondaryFamily="CRYPTO_ALGOFAM_NOT_SET"
    //            freshnessValueLength= NA
    //            freshnessValueTxLength= 8
    //            authInfoTxLength= 24
    // PROFILE_02: algorithmFamily="CRYPTO_ALGOFAM_AES"
    //            algorithmMode="CRYPTO_ALGOMODE_CMAC"
    //            algorithmSecondaryFamily="CRYPTO_ALGOFAM_NOT_SET"
    //            freshnessValueLength= 0
    //            freshnessValueTxLength= 0
    //            authInfoTxLength= 24
    // PROFILE_03: algorithmFamily="CRYPTO_ALGOFAM_AES"
    //            algorithmMode="CRYPTO_ALGOMODE_CMAC"
    //            algorithmSecondaryFamily="CRYPTO_ALGOFAM_NOT_SET"
    //            freshnessValueLength= 64
    //            freshnessValueTxLength= 4
    //            authInfoTxLength= 28
    /// @brief PROFILE_01 02 03
    ara::core::String profileCategory{};

    /// @brief crypto key slot name

    ara::core::String cryptoKeySlot{};

    /// @brief internal Crypto algorithm name
    ara::core::String cryptoAlgorithm{};

public:
    /// @brief Set default value for profile items.
    SecOCProfile() = default;

    /// @brief load profile from configuration file
    /// @param[in] node manifest node
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
    {
        ara::core::StringView const dataIdStr{"dataId"};
        std::ignore = node.Load(dataIdStr, dataId);
        ara::core::StringView const freshnessIdStr{"freshnessId"};
        std::ignore = node.Load(freshnessIdStr, freshnessId);
        ara::core::StringView const freshnessValueLengthStr{"freshnessValueLength"};
        std::ignore = node.Load(freshnessValueLengthStr, freshnessValueLength);
        ara::core::StringView const freshnessValueTxLengthStr{"freshnessValueTxLength"};
        std::ignore = node.Load(freshnessValueTxLengthStr, freshnessValueTxLength);
        ara::core::StringView const authInfoTxLengthStr{"authInfoTxLength"};
        std::ignore = node.Load(authInfoTxLengthStr, authInfoTxLength);
        ara::core::StringView const authDataFreshnessLengthStr{"authDataFreshnessLength"};
        std::ignore = node.Load(authDataFreshnessLengthStr, authDataFreshnessLength);
        ara::core::StringView const authDataFreshnessStartPositionStr{"authDataFreshnessStartPosition"};
        std::ignore = node.Load(authDataFreshnessStartPositionStr, authDataFreshnessStartPosition);
        ara::core::StringView const securedAreaLengthStr{"securedAreaLength"};
        std::ignore = node.Load(securedAreaLengthStr, securedAreaLength);
        ara::core::StringView const securedAreaOffsetStr{"securedAreaOffset"};
        std::ignore = node.Load(securedAreaOffsetStr, securedAreaOffset);
        ara::core::StringView const authenticationBuildAttemptsStr{"authenticationBuildAttempts"};
        std::ignore = node.Load(authenticationBuildAttemptsStr, authenticationBuildAttempts);
        ara::core::StringView const authenticationRetriesStr{"authenticationRetries"};
        std::ignore = node.Load(authenticationRetriesStr, authenticationRetries);
        ara::core::StringView const useFreshnessTimestampStr{"useFreshnessTimestamp"};
        std::ignore = node.Load(useFreshnessTimestampStr, useFreshnessTimestamp);
        ara::core::StringView const useAuthDataFreshnessStr{"useAuthDataFreshness"};
        std::ignore = node.Load(useAuthDataFreshnessStr, useAuthDataFreshness);
        ara::core::StringView const useSecuredPduHeaderStr{"useSecuredPduHeader"};
        std::ignore = node.Load(useSecuredPduHeaderStr, useSecuredPduHeader);
        ara::core::StringView const authAlgorithmStr{"authAlgorithm"};
        std::ignore = node.Load(authAlgorithmStr, authAlgorithm);
        ara::core::StringView const algorithmFamilyStr{"algorithmFamily"};
        std::ignore = node.Load(algorithmFamilyStr, algorithmFamily);
        ara::core::StringView const algorithmModeStr{"algorithmMode"};
        std::ignore = node.Load(algorithmModeStr, algorithmMode);
        ara::core::StringView const algorithmSecondaryFamilyStr{"algorithmSecondaryFamily"};
        std::ignore = node.Load(algorithmSecondaryFamilyStr, algorithmSecondaryFamily);
        ara::core::StringView const profileCategoryStr{"profileCategory"};
        std::ignore = node.Load(profileCategoryStr, profileCategory);
        ara::core::StringView const cryptoKeySlotStr{"cryptoKeySlot"};
        std::ignore = node.Load(cryptoKeySlotStr, cryptoKeySlot);
        return isoft::kSuccess;
    }

    /// @brief validate profile items
    /// @return true of false
    bool ValidateAndTransform() noexcept
    {
        // dataId and freshnessId must be set
        if ((dataId == 0) || (freshnessId == 0)) {
            ComLogError("dataId or freshnessId error");
            return false;
        }
        // Key slot information must be set
        if (cryptoKeySlot.empty()) {
            ComLogError("cryptoKeySlot error");
            return false;
        }
        // For Profile_01 02 03 handling
        if (profileCategory.compare("PROFILE_01") == 0) {
            algorithmFamily          = "CRYPTO_ALGOFAM_AES";
            algorithmMode            = "CRYPTO_ALGOMODE_CMAC";
            algorithmSecondaryFamily = "CRYPTO_ALGOFAM_NOT_SET";
            freshnessValueTxLength   = ara::com::secoc::internal::kDefaultValue_8;
            authInfoTxLength         = ara::com::secoc::internal::kDefaultValue_24;
            cryptoAlgorithm          = "macaescbc-128";
            expectedAuthValueLength  = ara::com::secoc::internal::kDefaultValue_128;
            jobSemantic              = SecOcJobSemanticEnum::kAuthenticate;
        }
        if (profileCategory.compare("PROFILE_02") == 0) {
            algorithmFamily          = "CRYPTO_ALGOFAM_AES";
            algorithmMode            = "CRYPTO_ALGOMODE_CMAC";
            algorithmSecondaryFamily = "CRYPTO_ALGOFAM_NOT_SET";
            freshnessValueLength     = ara::com::secoc::internal::kDefaultValue_0;
            freshnessValueTxLength   = ara::com::secoc::internal::kDefaultValue_0;
            authInfoTxLength         = ara::com::secoc::internal::kDefaultValue_24;
            cryptoAlgorithm          = "macaescbc-128";
            expectedAuthValueLength  = ara::com::secoc::internal::kDefaultValue_128;
            jobSemantic              = SecOcJobSemanticEnum::kAuthenticate;
        }
        if (profileCategory.compare("PROFILE_03") == 0) {
            algorithmFamily          = "CRYPTO_ALGOFAM_AES";
            algorithmMode            = "CRYPTO_ALGOMODE_CMAC";
            algorithmSecondaryFamily = "CRYPTO_ALGOFAM_NOT_SET";
            freshnessValueLength     = ara::com::secoc::internal::kDefaultValue_64;
            freshnessValueTxLength   = ara::com::secoc::internal::kDefaultValue_4;
            authInfoTxLength         = ara::com::secoc::internal::kDefaultValue_28;
            cryptoAlgorithm          = "macaescbc-128";
            expectedAuthValueLength  = ara::com::secoc::internal::kDefaultValue_128;
            jobSemantic              = SecOcJobSemanticEnum::kAuthenticate;
        }
        // Authentication information transmission length must be set
        if (authInfoTxLength == 0) {
            ComLogError("authInfoTxLength error");
            return false;
        }
        // Freshness value judgment, prioritize using authData, then FVM
        // Freshness transmission length of 0 case, can only be not using freshness or using authData as freshness, will not use FVM
        if (useAuthDataFreshness) {
            // In this case, theoretically freshness value may not be transmitted
            if (freshnessValueTxLength > authDataFreshnessLength) {
                ComLogError("freshnessValueTxLength > authDataFreshnessLength error");
                return false;
            }
            if (authDataFreshnessLength == 0) {
                ComLogError("authDataFreshnessLength error");
                return false;
            }
        } else {
            // FV length not allowed to be non-zero while transmission length is 0
            if ((freshnessValueTxLength == 0) && (freshnessValueLength > 0)) {
                ComLogError("freshnessValueTxLength/freshnessValueLength error");
                return false;
            }
            // Transmission length longer than actual length not allowed
            if (freshnessValueTxLength > freshnessValueLength) {
                ComLogError("freshnessValueTxLength>freshnessValueLengt error");
                return false;
            }
        }
        // Authentication information transmission length should not be longer than expected length
        if (authInfoTxLength > expectedAuthValueLength) {
            ComLogError("authInfoTxLength > expectedAuthValueLength error");
            return false;
        }
        // Freshness transmission length and authentication information transmission length sum needs to be an integer number of bytes
        if (((freshnessValueTxLength + authInfoTxLength) % 8) != 0) {  // NOLINT -- TODO[magic-numbers]
            ComLogError("(freshnessValueTxLength+authInfoTxLength)%8 error");
            return false;
        }
        // Eliminate unreasonable settings
        if ((freshnessValueLength > ara::com::secoc::internal::kDefaultValue_64)
            || (freshnessValueTxLength > ara::com::secoc::internal::kDefaultValue_64)
            || (authenticationBuildAttempts >= ara::com::secoc::internal::kDefaultValue_4)
            || (authenticationRetries >= ara::com::secoc::internal::kDefaultValue_4)) {
            ComLogError("Value too large error");
            return false;
        }
        // algorithm process
        /* The source of algorithms may have several types, one is PROFILE_01 to 03, which are standard-defined, available for both SOME/IP and
            Signal-based binding. Additionally, there is the string type authAlgorithm for SOME/IP models, and the CryptoServicePrimitive type for Signal-based
            binding, containing algorithmFamily, algorithmMode, and algorithmSecondaryFamily. The current standard does not have a clear definition of 
            algorithm standards, only from a few examples in the standard the string type includes AES-128, and PROFILE defines several. The implementation 
            will initially support only one, can add more later. */
        if (authAlgorithm.compare("AES-128") == 0) {
            cryptoAlgorithm = "macaescbc-128";
        }
        if ((algorithmFamily.compare("CRYPTO_ALGOFAM_AES") == 0)
            && (algorithmMode.compare("CRYPTO_ALGOMODE_CMAC") == 0)) {
            cryptoAlgorithm = "macaescbc-128";
        }
        // Algorithm field must be set
        if (cryptoAlgorithm.empty()) {
            ComLogError("cryptoAlgorithm error");
            return false;
        }
        return true;
    }
};

/// @brief Profile manager class
class SecOCProfileManager
{
public:
    /// @brief get profile by data id
    /// @param[in] dataId
    /// @return
    static std::shared_ptr< SecOCProfile > GetSecOCProfileByDataId(uint16_t const dataId) noexcept
    {
        return Impl::Instance().ImplSecOCProfileByDataId(dataId);
    }

    /// @brief get profile by freshness id
    /// @param[in] freshnessId
    /// @return profile
    static std::shared_ptr< SecOCProfile > GetSecOCProfileByFreshnessId(uint16_t const freshnessId) noexcept
    {
        return Impl::Instance().ImplSecOCProfileByFreshnessId(freshnessId);
    }

    /// @brief get all freshness id
    /// @return freshness id set
    static ara::core::Vector< uint16_t > const& GetFreshnessIdSet() noexcept
    {
        return Impl::Instance().ImplFreshnessIdSet();
    }

    /// @brief get all data id
    /// @return data id set
    static ara::core::Vector< uint16_t > const& GetDataIdSet() noexcept { return Impl::Instance().ImplDataIdSet(); }

    /// @brief load config
    /// @code{.isoft}
    /// export_level=/COM/Security/SecOC
    /// @endcode
    /// @param[in] path
    /// @return true / false
    static bool LoadConfig(ara::core::StringView const& path) noexcept { return Impl::Instance().ImplLoadConfig(path); }

    /// @brief set verification status override
    /// @param[in] freshnessId
    /// @param[in] overrideStatus
    /// @param[in] numberOfMessagesToOverride
    /// @return true / false
    static bool SetVerifyStatusOverrideByFreshnessId(uint16_t const freshnessId,
                                                     OverrideStatusEnum const overrideStatus,
                                                     uint8_t const numberOfMessagesToOverride) noexcept
    {
        return Impl::Instance().ImplSetVerifyStatusOverrideByFreshnessId(freshnessId, overrideStatus,
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
        return Impl::Instance().ImplSetVerifyStatusOverrideByDataId(dataId, overrideStatus, numberOfMessagesToOverride);
    }

private:
    /// @brief impl class
    class Impl
    {
    public:
        /// @brief get instance
        /// @return impl instance
        static Impl& Instance() noexcept
        {
            static Impl s_Instance;
            return s_Instance;
        }

        /// @brief load config
        /// @param[in] path
        /// @return true / false
        bool ImplLoadConfig(ara::core::StringView const& path) noexcept
        {
            if (configured_) {
                ComLogInfo("secoc was already configured, skipping");
                return false;
            }
            ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > const manifestRes{
                isoft::manifestreader::OpenManifest(path)};
            if (!manifestRes) {
                ComLogInfo("Can not load secoc manifest profile");
                return false;
            }
            ara::core::Vector< SecOCProfile > cnf{};
            ara::core::StringView const secocProfile{"secoc_profiles"};
            int32_t const ret{manifestRes.Value()->Load(secocProfile, cnf)};
            if (ret == isoft::kSuccess) {
                if (cnf.empty()) {
                    ComLogDebug("No secoc dataId confiured");
                    return false;
                }
                for (auto& it : cnf) {
                    if (!(it.ValidateAndTransform())) {
                        ComLogError("secoc profile of dataid incorrect ", it.dataId);
                        continue;
                    }
#ifndef SECOC_UT_MOCK_CMAC_BY_OPENSSL
                    if (!SecOCCryptoManager::Initialize(it.cryptoKeySlot, it.cryptoAlgorithm, it.dataId)) {
                        ComLogError("Crypto initialization error of data id ", it.dataId);
                        continue;
                    }
#endif
                    std::shared_ptr< SecOCProfile > const p{std::make_shared< SecOCProfile >(it)};
                    secocProfileByDataIdMappings_[it.dataId]           = p;
                    secocProfileByFreshnessIdMappings_[it.freshnessId] = p;
                    freshnessIdSet_.push_back(it.freshnessId);
                    dataIdSet_.push_back(it.dataId);
                }
            } else {
                return false;
            }
            configured_ = true;
            return true;
        }
        /// @brief get profile by data id
        /// @param[in] dataId
        /// @return profile
        std::shared_ptr< SecOCProfile > ImplSecOCProfileByDataId(uint16_t const dataId) noexcept
        {
            ara::core::Map< uint16_t, std::shared_ptr< SecOCProfile > >::iterator const it{
                secocProfileByDataIdMappings_.find(dataId)};
            if (it != secocProfileByDataIdMappings_.end()) {
                return it->second;
            }
            return std::shared_ptr< SecOCProfile >(nullptr);
        }
        /// @brief get profile by freshness id
        /// @param[in] freshnessId
        /// @return profile
        std::shared_ptr< SecOCProfile > ImplSecOCProfileByFreshnessId(uint16_t const freshnessId) noexcept
        {
            ara::core::Map< uint16_t, std::shared_ptr< SecOCProfile > >::iterator const it{
                secocProfileByFreshnessIdMappings_.find(freshnessId)};
            if (it != secocProfileByFreshnessIdMappings_.end()) {
                return it->second;
            }
            return std::shared_ptr< SecOCProfile >(nullptr);
        }
        /// @brief get freshness id set
        /// @return freshness id set
        ara::core::Vector< uint16_t > const& ImplFreshnessIdSet() const noexcept { return freshnessIdSet_; }
        /// @brief get data id set
        /// @return data id set
        ara::core::Vector< uint16_t > const& ImplDataIdSet() const noexcept { return dataIdSet_; }

        /// @brief set verification status override
        /// @param[in] freshnessId
        /// @param[in] overrideStatus
        /// @param[in] numberOfMessagesToOverride
        /// @return true / false
        bool ImplSetVerifyStatusOverrideByFreshnessId(uint16_t const freshnessId,
                                                      OverrideStatusEnum const overrideStatus,
                                                      uint8_t const numberOfMessagesToOverride) noexcept
        {
            std::shared_ptr< SecOCProfile > profile{ImplSecOCProfileByFreshnessId(freshnessId)};
            if (profile == nullptr) {
                return false;
            }
            profile->overrideStatus                    = overrideStatus;
            profile->numberOfMessagesToOverride        = numberOfMessagesToOverride;
            profile->currentNumberOfMessagesToOverride = 0;
            return true;
        }

        /// @brief set verification status override
        /// @param[in] dataId
        /// @param[in] overrideStatus
        /// @param[in] numberOfMessagesToOverride
        /// @return true / false
        bool ImplSetVerifyStatusOverrideByDataId(uint16_t const dataId,
                                                 OverrideStatusEnum const overrideStatus,
                                                 uint8_t const numberOfMessagesToOverride) noexcept
        {
            std::shared_ptr< SecOCProfile > profile{ImplSecOCProfileByDataId(dataId)};
            if (profile == nullptr) {
                return false;
            }
            profile->overrideStatus                    = overrideStatus;
            profile->numberOfMessagesToOverride        = numberOfMessagesToOverride;
            profile->currentNumberOfMessagesToOverride = 0;
            return true;
        }

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
        Impl& operator=(Impl const& other) noexcept = default;
        /// @brief Impl()
        /// @param[in] other
        Impl(Impl&& other) noexcept = default;
        /// @brief Impl operartor =
        /// @param[in] other
        /// @return Impl &
        Impl& operator=(Impl&& other) noexcept = default;

    private:
        /// @brief configured or not
        bool configured_{false};
        /// @brief data id profile map
        ara::core::Map< uint16_t, std::shared_ptr< SecOCProfile > > secocProfileByDataIdMappings_;
        /// @brief freshness id profile map
        ara::core::Map< uint16_t, std::shared_ptr< SecOCProfile > > secocProfileByFreshnessIdMappings_;
        /// @brief freshness id set
        ara::core::Vector< uint16_t > freshnessIdSet_;
        /// @brief data id set
        ara::core::Vector< uint16_t > dataIdSet_;
    };

public:
    /// @brief disable
    ~SecOCProfileManager() noexcept = delete;
    /// @brief disable
    SecOCProfileManager() noexcept = delete;
    /// @brief disable
    /// @param[in] other
    SecOCProfileManager(SecOCProfileManager const& other) noexcept = delete;
    /// @brief disable
    /// @param[in] other
    /// @return none
    SecOCProfileManager& operator=(SecOCProfileManager const& other) noexcept = delete;
    /// @brief disable
    /// @param[in] other
    SecOCProfileManager(SecOCProfileManager const&& other) noexcept = delete;
    /// @brief disable
    /// @param[in] other
    /// @return none
    SecOCProfileManager& operator=(SecOCProfileManager const&& other) noexcept = delete;
};

}  // namespace secoc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif