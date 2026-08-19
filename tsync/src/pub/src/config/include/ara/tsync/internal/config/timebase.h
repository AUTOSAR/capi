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
/// @file       timebase.h
/// @brief      configuration module time base set class
/// @details
/// @date       2023-01-11
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/Config
/// module_path=/TimeSync/Config
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_CONFIG_TIMEBASE_H_
#define ARA_TSYNC_INTERNAL_CONFIG_TIMEBASE_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include <cstdint>

#include "ara/tsync/internal/TimeDomainId.h"
#include "ara/tsync/internal/config/common.h"
#include "ara/tsync/internal/config/timesynccorrection.h"
#include "ara/tsync/internal/log/logger.h"
namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief time base configuration
class TimeBase final
{
public:
    /// @brief default constructor
    TimeBase() = default;

    /// @brief copy constructor
    /// @param tb time base object
    TimeBase(TimeBase const &tb) = default;

    /// @brief default destructor
    ~TimeBase() = default;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    TimeBase &operator=(TimeBase const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    TimeBase(TimeBase &&other) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    TimeBase &operator=(TimeBase &&other) &noexcept = default;

    /// @brief get time base name
    /// @return name
    ara::core::String const &GetName() const noexcept { return name_; }

    /// @brief get network time provider name
    /// @return name
    ara::core::String const &GetNetworkTimeProvider() const noexcept { return networkTimeProvider_; }

    /// @brief get network time consumer name
    /// @return name
    ara::core::String const &GetNetworkTimeConsumer() const noexcept { return networkTimeConsumer_; }

    /// @brief get time correction configuration
    /// @return time correction configuration
    TimeSyncCorrection const &GetTimeSyncCorrection() const noexcept { return timeSyncCorrection_; }

    /// @brief get time domain ID
    /// @return time domain ID
    TimeDomainId const &GetTimeDomainId() const noexcept { return timeDomainId_; }

    /// @brief set time domain ID
    /// @param id - time domain ID
    void SetTimeDomainId(TimeDomainId const &id) noexcept { timeDomainId_ = id; }

    /// @brief get time domain name
    /// @return time domain name
    ara::core::StringView GetTimeDomainName() const noexcept
    {
        ara::core::StringView networkName;
        ara::core::StringView timeDomainName;
        if (true != GetNetworkTimeProvider().empty()) {
            networkName = ara::core::StringView{GetNetworkTimeProvider().data(), GetNetworkTimeProvider().size()};
        } else if (true != GetNetworkTimeConsumer().empty()) {
            networkName = ara::core::StringView{GetNetworkTimeConsumer().data(), GetNetworkTimeConsumer().size()};
        } else {
        }
        char8_t const letter{'/'};
        ara::core::StringView::size_type const pos{networkName.find_last_of(letter)};
        char8_t const letterV3{'.'};
        ara::core::StringView::size_type const posV3{networkName.find_last_of(letterV3)};
        if (ara::core::StringView::npos != pos) {
            timeDomainName = BaseName(networkName.substr(0U, pos));
        } else if (ara::core::StringView::npos != posV3) {
            timeDomainName = BaseName(networkName.substr(0U, posV3));
        } else {
            LOG().Error() << "Invalid networkName{" << networkName << "}: must be a FQN.";
        }
        return timeDomainName;
    };

    /// @brief manifestReader standard node load function
    /// @param node - node
    /// @return kSucess/ErrorCode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
    {
        /// @name kName
        static constexpr ara::core::StringView kName{"name"};
        /// @name kNetworkTimeProvider
        static constexpr ara::core::StringView kNetworkTimeProvider{"networkTimeProvider"};
        /// @name kNetworkTimeConsumer
        static constexpr ara::core::StringView kNetworkTimeConsumer{"networkTimeConsumer"};
        /// @name kTimeSyncCorrection
        static constexpr ara::core::StringView kTimeSyncCorrection{"timeSyncCorrection"};
        /// @name kStorageInstanceSpecifier
        static constexpr ara::core::StringView kStorageInstanceSpecifier{"storageInstanceSpecifier"};
        /// @name kStorageKVKey
        static constexpr ara::core::StringView kStorageKVKey{"storageKVKey"};
        /// @name kStorageFile
        static constexpr ara::core::StringView kStorageFile{"storageFile"};  /// @name kStorageFile
        /// @name kCyclicBackupInterval
        static constexpr ara::core::StringView kCyclicBackupInterval{"cyclicBackupInterval"};
        this->name_                     = node.GetValue(kName, ara::core::String());
        this->networkTimeProvider_      = node.GetValue(kNetworkTimeProvider, ara::core::String());
        this->networkTimeConsumer_      = node.GetValue(kNetworkTimeConsumer, ara::core::String());
        this->storageInstanceSpecifier_ = node.GetValue(kStorageInstanceSpecifier, ara::core::String());
        this->storageKVKey_             = node.GetValue(kStorageKVKey, ara::core::String());

        this->storageFile_          = node.GetValue(kStorageFile, ara::core::String());
        this->cyclicBackupInterval_ = node.GetValue(kCyclicBackupInterval, 0.0);
        static_cast< void >(node.Load(kTimeSyncCorrection, this->timeSyncCorrection_));
        if (0 > this->cyclicBackupInterval_) {
            LOG().Error() << "cyclicBackupInterval should be bigger than 0.";
            return -1;
        }
        return 0;
    };

    /// @brief print debug information
    void Debug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        std::cout << "name: " << GetName() << std::endl;
        std::cout << "networkTimeProvider: " << GetNetworkTimeProvider() << std::endl;
        std::cout << "networkTimeConsumer: " << GetNetworkTimeConsumer() << std::endl;
        std::cout << "timeSyncCorrection: " << std::endl;
        GetTimeSyncCorrection().Debug();
#endif
    };

    /// @brief whether it is a time base provider
    /// @return yes/no
    bool IsProvider() const noexcept
    {
        if (true != GetNetworkTimeProvider().empty()) {
            return true;
        }
        return false;
    }
    /// @brief get persistent instance descriptor
    /// @return persistent instance descriptor
    ara::core::String const &GetStorageInstanceSpecifier() const noexcept { return storageInstanceSpecifier_; }

    /// @brief get persistent kv storage key
    /// @return persistent kv storage key
    ara::core::String const &GetStorageKVKey() const noexcept { return storageKVKey_; }

    /// @brief get persistent file storage file name
    /// @return persistent file storage file name
    ara::core::String const &GetStorageFile() const noexcept { return storageFile_; }

    /// @brief get periodic backup configuration
    /// @return periodic backup duration
    double GetCyclicBackupInterval() const noexcept { return cyclicBackupInterval_; }

private:
    /// @name name_ - time base name
    ara::core::String name_;
    /// @name networkTimeProvider_ - network time provider
    ara::core::String networkTimeProvider_;
    /// @name networkTimeConsumer_ - network time consumer
    ara::core::String networkTimeConsumer_;
    /// @name timeSyncCorrection_ - time correction configuration
    TimeSyncCorrection timeSyncCorrection_;
    /// @name timeDomainId_ - bound underlying time domain ID
    TimeDomainId timeDomainId_;
    /// @name storageInstanceSpecifier_ - persistent instance descriptor
    ara::core::String storageInstanceSpecifier_;
    /// @name storageKVKey_ - persistent kv key
    ara::core::String storageKVKey_;
    /// @name storageFile_ - file name for file persistence
    ara::core::String storageFile_;
    /// @name cyclicBackupInterval_ - periodic backup interval (R2311)
    double cyclicBackupInterval_{0.0};
};

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_CONFIG_TIMEBASE_H_
