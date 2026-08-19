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
/// @file       tbresmanager.h
/// @brief      time base resource management class
/// @details
/// @date       2023-02-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_TBRESMANAGER_H_
#define ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_TBRESMANAGER_H_

#include <ara/core/vector.h>
#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include <cstdint>

#include "ara/tsync/internal/config/configmanager.h"
#include "ara/tsync/internal/timebase/resource/tbcontext.h"
#include "isoft/osi/ipc/sharedmemory.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace resource {

/// @brief time base resource
class TBResManager final
{
public:
    /// @brief constructor with parameters
    TBResManager() = default;

    /// @brief copy constructor
    /// @param st - reference to other object
    TBResManager(TBResManager const &st) = delete;

    /// @brief move constructor
    /// @param st - reference to other object
    TBResManager(TBResManager &&st) = default;

    /// @brief operator overload
    /// @param st - other object
    /// @return reference to this object
    TBResManager &operator=(TBResManager const &st) & = delete;

    /// @brief operator overload
    /// @param st - other object
    /// @return object reference
    TBResManager &operator=(TBResManager &&st) & = default;

    /// @brief destructor
    ~TBResManager() noexcept;

    /// @brief create time base resource manager based on configuration
    /// @param configMan - time base configuration set
    /// @return time base resource manager handle
    static std::shared_ptr< TBResManager > CreateManager(
        std::shared_ptr< config::ConfigManager const > const &configMan) noexcept;

    /// @brief open an existing time base resource manager
    /// @return time base resource manager handle
    static std::shared_ptr< TBResManager > OpenManager() noexcept;

    /// @brief get time base resource context based on time base name
    /// @param tbName - time base name
    /// @return time base resource context
    TBContext *GetContext(ara::core::String const &tbName) const noexcept;

    /// @brief get time base resource context based on time base ID
    /// @param tbId - time base ID
    /// @return time base resource context
    TBContext *GetContext(TimeBaseId const &tbId) const noexcept
    {
        if (true != tbId.IsValid()) {
            return nullptr;
        }
        std::uint8_t const tbIndex{tbId.GetIndex()};
        if (true != _isValidIndex(tbIndex)) {
            return nullptr;
        }
        return contextSet_ + tbIndex;
    }

    /// @brief get the list of offset time base IDs associated with the specified time domain ID
    /// @param domainId - time domain ID
    /// @return list of time base IDs
    ara::core::Vector< TimeBaseId > GetOffsetTimeBaseIds(internal::TimeDomainId const &domainId) const noexcept;

    /// @brief get the list of time base IDs associated with the specified time domain ID
    /// @param domainId - time domain ID
    /// @return list of time base IDs
    ara::core::Vector< TimeBaseId > GetTimeBaseIds(internal::TimeDomainId const &domainId) const noexcept;

    /// @brief get time base ID based on time base name
    /// @param tbName - time base name
    /// @return time base ID
    TimeBaseId GetIdbyName(ara::core::String const &tbName) const noexcept
    {
        TBContext const *const ctx{GetContext(tbName)};
        if (nullptr == ctx) {
            return internal::timebase::resource::TimeBaseId();
        }
        return ctx->GetId();
    }

    /// @brief when the synchronization time base sets the time, update the corresponding offset time base time
    /// @param tbId - time base ID
    /// @return none
    void UpdateOffsetTimeBase(timebase::resource::TimeBaseId const &tbId) noexcept;

    /// @brief when the offset time base sets the rate offset, update the corresponding synchronization time base
    /// @param tbId - time base ID
    void UpdateSyncTimeBase(timebase::resource::TimeBaseId const &tbId) noexcept;

private:
    /// @brief create and open time base resource manager, then other processes can connect and use it
    /// @param configMan - time base configuration set
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _create(std::shared_ptr< config::ConfigManager const > const &configMan) noexcept;

    /// @brief close and destroy time base resource manager, after which no process can use it
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _destroy() noexcept;

    /// @brief open time base resource manager for current process to use
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _open() noexcept;

    /// @brief close time base resource manager, after which current process cannot use it
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _close() noexcept;

    /// @brief generate shared memory path
    void _makeSharedMemPath() noexcept
    {
        isoft::ara_fsh::Platform const fsh;
        shareMemPath_ = kSharedMemName_ + ara::core::String{"_"};
        shareMemPath_ += fsh.GetSysrootMd5Str().c_str();
    }

    /// @brief generate time base ID based on the time domain ID where the time base is located and the storage location of the time base
    /// @param tbIndex - location of the time base
    /// @param timeDomainId - time domain ID
    /// @return time base ID
    TimeBaseId _makeId(std::uint8_t const tbIndex, internal::TimeDomainId const timeDomainId) const noexcept
    {
        /// 31        23           15        7      0
        /// | domainId | isprovider |      index    |
        if (!_isValidIndex(tbIndex)) {
            return internal::timebase::resource::TimeBaseId();
        }
        return internal::timebase::resource::TimeBaseId{timeDomainId, tbIndex};
    }

    /// @brief check whether the index is valid
    /// @param tbIndex - index number
    /// @return true - valid
    /// @return false - invalid
    bool _isValidIndex(std::uint8_t const tbIndex) const noexcept
    {
        if (tbIndex >= (shm_.Size() / sizeof(TBContext))) {
            return false;
        }
        return true;
    }

private:
    /// @name configMan_ - configuration manager handle
    std::shared_ptr< config::ConfigManager const > configMan_{nullptr};

    /// @name timeBaseConfigSet_ - time base configuration set handle
    std::shared_ptr< config::TimeBaseSet const > timeBaseConfigSet_{nullptr};

    /// @name contextSet_ - time base resource set
    TBContext *contextSet_{nullptr};

    /// @name shm_ - shared memory handle
    isoft::osi::tsipc::SharedMemory shm_{};

    /// @name shareMemPath_ - shared memory file name
    ara::core::String shareMemPath_{};
    /// @name kSharedMemName
    ara::core::String kSharedMemName_{"ara_tsync_time_base_shm_name"};

};  /// class TBResManager

}  // namespace resource
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  // ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_TBRESMANAGER_H_
