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
/// @file       secoc_statistics.h
/// @brief      secoc statistics header file
/// @details
/// @date       2023-09-04
/// @author     shigang.zan
/// @version    1.2.0
///
/// ================================================================

#ifndef __SECOC_STATISTICS_H__
#define __SECOC_STATISTICS_H__

#include <mutex>

#include "ara/com/internal/secoc/secoc_config.h"
#include "ara/core/string.h"

namespace ara {
namespace com {
namespace internal {
namespace secoc {

/// @brief verification status result enum
enum class VerificationStatusResultEnum : uint8_t
{
    // Verification successful
    kSecOcVerificationSuccess = 0x00,
    // Verification not successful
    kSecOcVerificationFailure = 0x01,
    // Verification not successful because of wrong freshness value.
    kSecOcFreshnessFailure = 0x02,
    // Verification not successful because of wrong build authentication codes
    kSecOcAuthenticationBuildFailure = 0x03,
    // Verification has been skipped and the data has been provided to the application as is.
    kSecOcNoVerification = 0x04,
    // Verification failed, but the PDU was passed on to the application due to the override status for this PDU.
    kSecOcVerificationFailureOverwritten = 0x05
};

/// @brief Statistics class
struct SecOCStatistics final
{
    /// @brief success num
    int16_t secOCVerificationSuccessNum;
    /// @brief failure num
    int16_t secOCVerificationFailureNum;
    /// @brief freshness failure num
    int16_t secOCFreshnessFailureNum;
    /// @brief authentication build failure num
    int16_t secOCAuthenticaitonBuildFailureNum;
    /// @brief no verification num
    int16_t secOCNoVerificationNum;
    /// @brief overwritten num
    int16_t secOCVerificationFailureOverwrittenNum;
};

/// @brief event snapshot class
struct SecOCVerificationEventSnapshot final
{
    /// @brief freshness Id
    uint16_t freshnessId;
    /// @brief last verification result
    VerificationStatusResultEnum verificationStatus;
    /// @brief data Id
    uint16_t dataId;
};

/// @brief Statistics manager class
class SecOCStatisticsManager
{
public:
    /// @brief get statistics by data id
    /// @param[in] dataId
    /// @return statistics
    static std::shared_ptr< SecOCStatistics > GetSecOCStatisticsByDataId(uint16_t const dataId) noexcept
    {
        return Impl::Instance().ImplSecOCStatisticsByDataId(dataId);
    }

    /// @brief get statistics by freshness id
    /// @param[in] freshnessId
    /// @return statistics
    static std::shared_ptr< SecOCStatistics > GetSecOCStatisticsByFreshnessId(uint16_t const freshnessId) noexcept
    {
        return Impl::Instance().ImplSecOCStatisticsByFreshnessId(freshnessId);
    }

    /// @brief initialize depends on SecOCProfile
    /// @code{.isoft}
    /// export_level=/COM/Security/SecOC
    /// @endcode
    static void Initialize() noexcept { return Impl::Instance().ImplInitialize(); }

    /// @brief set verification snapshot value
    /// @param[in] dataId
    /// @param[in] freshnessId
    /// @param[in] verificationStatus
    static void SetVerificationEventSnapshot(uint16_t const dataId,
                                             uint16_t const freshnessId,
                                             VerificationStatusResultEnum const verificationStatus) noexcept
    {
        return Impl::Instance().ImplSetVerificationEventSnapshot(dataId, freshnessId, verificationStatus);
    }

    /// @brief get verification snapshot value
    /// @param[in] verificationSnapshot
    /// @return true / false
    static bool GetVerificationEventSnapshot(SecOCVerificationEventSnapshot& verificationSnapshot) noexcept
    {
        return Impl::Instance().ImplGetVerificationEventSnapshot(verificationSnapshot);
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

        /// @brief get statistics by data id
        /// @param[in] dataId
        /// @return statistics
        std::shared_ptr< SecOCStatistics > ImplSecOCStatisticsByDataId(uint16_t const dataId) noexcept
        {
            ara::core::Map< uint16_t, std::shared_ptr< SecOCStatistics > >::iterator const it{
                secocStatisticsByDataIdMappings_.find(dataId)};
            if (it != secocStatisticsByDataIdMappings_.end()) {
                return it->second;
            }
            return std::shared_ptr< SecOCStatistics >(nullptr);
        }
        /// @brief get statistics by freshness id
        /// @param[in] freshnessId
        /// @return statistics
        std::shared_ptr< SecOCStatistics > ImplSecOCStatisticsByFreshnessId(uint16_t const freshnessId) noexcept
        {
            ara::core::Map< uint16_t, std::shared_ptr< SecOCStatistics > >::iterator const it{
                secocStatisticsByFreshnessIdMappings_.find(freshnessId)};
            if (it != secocStatisticsByFreshnessIdMappings_.end()) {
                return it->second;
            }
            return std::shared_ptr< SecOCStatistics >(nullptr);
        }

        /// @brief impl initialize
        void ImplInitialize() noexcept
        {
            if (configured_) {
                return;
            }
            ara::core::Vector< uint16_t > const& dataIdSet{SecOCProfileManager::GetDataIdSet()};
            if (dataIdSet.empty()) {
                ComLogError("SecOCStatistics:Init No any secoc config found");
                return;
            }
            for (uint16_t const dataId : dataIdSet) {
                std::shared_ptr< SecOCProfile > profile{SecOCProfileManager::GetSecOCProfileByDataId(dataId)};
                if (profile != nullptr) {
                    std::shared_ptr< SecOCStatistics > const p{std::make_shared< SecOCStatistics >()};
                    secocStatisticsByDataIdMappings_[profile->dataId]           = p;
                    secocStatisticsByFreshnessIdMappings_[profile->freshnessId] = p;
                }
            }
            configured_ = true;
        }

        /// @brief set verification snapshot value
        /// @param[in] dataId
        /// @param[in] freshnessId
        /// @param[in] verificationStatus
        void ImplSetVerificationEventSnapshot(uint16_t const dataId,
                                              uint16_t const freshnessId,
                                              VerificationStatusResultEnum const verificationStatus) noexcept
        {
            std::lock_guard< std::mutex > const localLock{snapshotMutex_};
            snapshot_.dataId             = dataId;
            snapshot_.freshnessId        = freshnessId;
            snapshot_.verificationStatus = verificationStatus;
            snapshotUpdateFlag_          = true;
        }

        /// @brief get verification snapshot value
        /// @param[in] verificationSnapshot
        /// @return true / false
        bool ImplGetVerificationEventSnapshot(SecOCVerificationEventSnapshot& verificationSnapshot) noexcept
        {
            std::lock_guard< std::mutex > const localLock{snapshotMutex_};
            if (snapshotUpdateFlag_) {
                verificationSnapshot = snapshot_;
                snapshotUpdateFlag_  = false;
                return true;
            }
            return false;
        }

        /// @brief impl()
        Impl() noexcept = default;
        /// @brief ~Impl()
        virtual ~Impl() noexcept = default;
        /// @brief Impl()
        /// @param[in] other
        Impl(Impl const& other) noexcept = delete;
        /// @brief Impl operator =
        /// @param[in] other
        /// @return Impl &
        Impl& operator=(Impl const& other) noexcept = delete;
        /// @brief Impl()
        /// @param[in] other
        Impl(Impl&& other) noexcept = delete;
        /// @brief Impl operartor =
        /// @param[in] other
        /// @return Impl &
        Impl& operator=(Impl&& other) noexcept = delete;

    private:
        /// @brief data id statistics map
        ara::core::Map< uint16_t, std::shared_ptr< SecOCStatistics > > secocStatisticsByDataIdMappings_;
        /// @brief freshness id statistics map
        ara::core::Map< uint16_t, std::shared_ptr< SecOCStatistics > > secocStatisticsByFreshnessIdMappings_;
        /// @brief secoc verification event snapshot
        SecOCVerificationEventSnapshot snapshot_{};
        /// @brief snapshot update flag
        bool snapshotUpdateFlag_{false};
        /// @brief snapshot mutex
        std::mutex snapshotMutex_;
        /// @brief configured or not
        bool configured_{false};
    };

public:
    /// @brief disable
    ~SecOCStatisticsManager() noexcept = delete;
    /// @brief disable
    SecOCStatisticsManager() noexcept = delete;
    /// @brief disable
    /// @param[in] other
    SecOCStatisticsManager(SecOCStatisticsManager const& other) noexcept = delete;
    /// @brief disable
    /// @param[in] other
    /// @return none
    SecOCStatisticsManager& operator=(SecOCStatisticsManager const& other) noexcept = delete;
    /// @brief disable
    /// @param[in] other
    SecOCStatisticsManager(SecOCStatisticsManager const&& other) noexcept = delete;
    /// @brief disable
    /// @param[in] other
    /// @return none
    SecOCStatisticsManager& operator=(SecOCStatisticsManager const&& other) noexcept = delete;
};

}  // namespace secoc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif