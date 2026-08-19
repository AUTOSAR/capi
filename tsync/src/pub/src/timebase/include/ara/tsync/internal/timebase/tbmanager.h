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
/// @file       tbmanager.h
/// @brief      time base management class
/// @details
/// @date       2023-01-12
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/TimeBase
/// module_path=/TimeSync/TimeBase
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEBASE_MANAGER_H_
#define ARA_TSYNC_INTERNAL_TIMEBASE_MANAGER_H_

#include <ara/core/map.h>
#include <isoft/naicpp/global_evloop.h>

#include <chrono>
#include <cstdint>
#include <memory>

#include "ara/tsync/internal/config/configmanager.h"
#include "ara/tsync/internal/timebase/eventtype.h"
#include "ara/tsync/internal/timebase/proxy/skeleton.h"
#include "ara/tsync/internal/timebase/resource/tbresmanager.h"
#include "ara/tsync/internal/timebase/statuschecker.h"
#include "ara/tsync/internal/timedomain/manager.h"

namespace ara {
namespace tsync {
namespace internal {

/// @brief callback function type for retrieving time base content from persistent storage
/// @param timebaseName time base name
/// @param perInstanceSpecifier persistence descriptor
/// @param fileSpecifier file descriptor
/// @param userData - user data
/// @param rateDeviation - rate deviation
/// @param lastGlobalNano - last set global time value
/// @return true, success; false, failure;
using GetStorageTimeHandler = std::function< bool(ara::core::String const &timebaseName,
                                                  ara::core::StringView kvSpecifier,
                                                  ara::core::StringView perInstanceSpecifier,
                                                  ara::core::StringView fileSpecifier,
                                                  ara::core::String &userData,
                                                  double &rateDeviation,
                                                  std::uint64_t &lastGlobalNano) >;

/// @brief persistence time event callback function type
/// @param type type
/// @param timebaseName time base name
/// @param keyStr kv storage key
/// @param userData user data
/// @param rateDeviation rate deviation
/// @param lastGlobalNano time
using SetStorageTimeHandler = std::function< void(StorageType type,
                                                  ara::core::String const &timebaseName,
                                                  ara::core::StringView keyStrView,
                                                  ara::core::String const &userData,
                                                  double const rateDeviation,
                                                  std::uint64_t const lastGlobalNano) >;

/// @brief persistence iteration
using Storageiterator = ara::core::Vector< ara::core::String >::iterator;
namespace timebase {

/// @brief time base management class
class TBManager final
{
public:
    /// @brief constructor
    TBManager() = default;

    /// @brief destructor
    ~TBManager() = default;

    /// @brief copy constructor
    /// @param    other
    /// @returns object reference
    /// TODO(zhoubo): copy assignment operator is explicitly defaulted but
    /// implicitly deleted
    TBManager &operator=(TBManager const &other) = delete;

    /// @brief move constructor
    /// @param    other
    /// @returns object reference
    TBManager &operator=(TBManager &&other) = default;

    /// @brief copy constructor
    /// @param other - other object
    /// TODO(zhoubo): copy constructor is explicitly defaulted but implicitly
    /// deleted
    TBManager(TBManager const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    TBManager(TBManager &&) noexcept = default;

    /// @brief create manager
    /// @param pConfig configuration manager
    /// @param timeDomain time domain manager
    /// @return manager handle
    static std::unique_ptr< TBManager > CreateManager(
        std::shared_ptr< config::ConfigManager > const &pConfig,
        std::shared_ptr< timedomain::TDManager > const &timeDomain) noexcept
    {
        std::unique_ptr< TBManager > man{nullptr};
        if (nullptr == pConfig) {
            return man;
        }
        if (nullptr == timeDomain) {
            return man;
        }
        man = std::make_unique< TBManager >();
        if (nullptr != man) {
            if (0 != man->_init(pConfig, timeDomain)) {
                man = nullptr;
            }
        }
        return man;
    }

    /// @brief notify an event on the specified time domain
    /// @param domainId - time domain ID
    /// @param event - event type
    void Notify(internal::TimeDomainId const &domainId, EventType const &event) noexcept;

    /// @brief
    /// Register the set time callback function, which will be called when the time base receives this event. Subsequent calls will overwrite the previous registration.
    /// @param cb - callback function
    void OnSetTime(EventHandler const &cb) noexcept { setTimeCb_ = cb; }

    /// @brief store time value to persistent storage when process exits
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t StoreTimeToStorage() noexcept;

    /// @brief initialize storage related, get storage time, periodic storage
    /// @param getStorageTimeCb - get time callback
    /// @param setStorageTimeCb - store time callback
    /// @return 0 - success
    /// @return <0 - failure
    void InitStorage(GetStorageTimeHandler const &getStorageTimeCb,
                     SetStorageTimeHandler const &setStorageTimeCb) noexcept;

private:
    /// @brief initialize
    /// @param pConfig configuration manager
    /// @param timeDomain time domain manager
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _init(std::shared_ptr< config::ConfigManager > const &pConfig,
                       std::shared_ptr< timedomain::TDManager > const &timeDomain) noexcept;

    /// @brief update time base resource status based on time domain configuration
    /// @param tbId - time base ID
    /// @param FromDomainId - time domain that triggered this update
    /// @param isSync - whether triggered by a synchronization domain; only synchronization domain triggers rrc calculation
    /// @return check result, indicating which statuses have changed
    StatusChecker _updateTimeBaseResource(timebase::resource::TimeBaseId const tbId,
                                          internal::TimeDomainId const &fromDomainId,
                                          bool const isSync) noexcept;

    /// @brief time base proxy skeleton event callback function
    /// @param tbId - time base resource context
    /// @param event - event
    void _proxySkeletonEventHandler(resource::TimeBaseId const tbId, proxy::ProxyEventType const event) noexcept;

    /// @brief recover time from persistent storage during initialization
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _loadTimeFromStorage() noexcept;

    /// @brief create periodic backup timer
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _createCyclicBackupTimer() noexcept;

    /// @brief perform periodic backup
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _doCyclicBackup(ara::core::String const &timebaseName) noexcept;

    /// @brief when setting rate offset and time for synchronization time base, need to update the corresponding offset time base and time domain
    /// @param tbId - time base resource context
    void _updateOffset(timebase::resource::TimeBaseId const &tbId) noexcept;

    /// @brief when setting rate offset for offset time base, need to update the corresponding synchronization time base and time domain
    /// @param tbId - time base resource context
    void _updateSync(timebase::resource::TimeBaseId const &tbId) noexcept;

private:
    /// @name configManager_ - configuration manager handle
    std::shared_ptr< config::ConfigManager > configManager_{nullptr};

    /// @name timeDomainManager_ - time domain manager handle
    std::shared_ptr< timedomain::TDManager > timeDomainManager_{nullptr};

    /// @name setTimeCb_ - SetTime event callback function
    EventHandler setTimeCb_{nullptr};

    /// @name resourceMan_ - time base resource manager handle
    std::shared_ptr< resource::TBResManager > resourceMan_{nullptr};

    /// @name proxySkeleton_ - time base proxy skeleton handle
    std::unique_ptr< proxy::Skeleton > proxySkeleton_{nullptr};

    /// @name getStorageTimeCb_ get persistent time during initialization
    GetStorageTimeHandler getStorageTimeCb_{nullptr};

    /// @name setStorageTimeCb_ persist time
    SetStorageTimeHandler setStorageTimeCb_{nullptr};

    /// @name kvVector_ kv set
    ara::core::Vector< ara::core::String > kvVector_{};

    /// @name fileVector_ file set
    ara::core::Vector< ara::core::String > fileVector_{};
    /// @name storageKVTimerMap_ -
    /// Map of time base providers and periodic kv persistence, used only on daemon side, only kv, 2311
    ara::core::Map< ara::core::String, isoft::naicpp::EvLoop::TimerPtr > storageKVTimerMap_{};
};  /// class TBManager

}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  // ARA_TSYNC_INTERNAL_TIMEBASE_MANAGER_H_
