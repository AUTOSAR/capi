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
/// @file       types.h
/// @brief      Definitions of some data types.
/// @details
/// @date       2024-06-06
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/Util
/// @unit_description=Definitions of some data types.
/// @trace_id_sr=SR_PHM_00002,SR_PHM_00003,SR_PHM_00004,SR_PHM_00005,SR_PHM_00006,
/// SR_PHM_00007,SR_PHM_00008,SR_PHM_00009,SR_PHM_00010,SR_PHM_00011,SR_PHM_00012
/// @unit_name=Type
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_UTIL_TYPE_H_
#define ARA_PHM_INTERNAL_UTIL_TYPE_H_

#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <nai/os/nai_proc.h>

#include <chrono>
#include <cmath>

/// @brief second to ms
/// @trace_id_sr=SR_PHM_00002,SR_PHM_00003,SR_PHM_00004,SR_PHM_00005,SR_PHM_00006,
///              SR_PHM_00007,SR_PHM_00008,SR_PHM_00009,SR_PHM_00010,SR_PHM_00011,SR_PHM_00012
/// @needwork = no
#define SECOND_TO_MS(second) (static_cast< int32_t >(std::round(((second)*1000.0))))

namespace ara {
namespace phm {
namespace internal {

/// @brief definition of specifier definition.
/// @trace_id_sr=SR_PHM_00002,SR_PHM_00003,SR_PHM_00004,SR_PHM_00005,SR_PHM_00006,
///              SR_PHM_00007,SR_PHM_00008,SR_PHM_00009,SR_PHM_00010,SR_PHM_00011,SR_PHM_00012
/// @needwork = no
using Specifier = ara::core::String;

/// @brief definition of checkpoint id.
/// @trace_id_sr=SR_PHM_00002,SR_PHM_00003,SR_PHM_00004,SR_PHM_00005,SR_PHM_00006,
///              SR_PHM_00007,SR_PHM_00008,SR_PHM_00009,SR_PHM_00010,SR_PHM_00011,SR_PHM_00012
/// @needwork = no
using CheckpointId = uint32_t;

/// @brief definition of process id.
/// @trace_id_sr=SR_PHM_00002,SR_PHM_00003,SR_PHM_00004,SR_PHM_00005,SR_PHM_00006,
///              SR_PHM_00007,SR_PHM_00008,SR_PHM_00009,SR_PHM_00010,SR_PHM_00011,SR_PHM_00012
/// @needwork = no
using ProcessId = nai_pid_t;

/// @brief Data type of LocalSupervisionStatus and GlobalSupervisionStatus.
/// @trace_id_sr=SR_PHM_00002,SR_PHM_00003,SR_PHM_00004,SR_PHM_00005,SR_PHM_00006,
///              SR_PHM_00007,SR_PHM_00008,SR_PHM_00009,SR_PHM_00010,SR_PHM_00011,SR_PHM_00012
/// @needwork = no
using SupervisionStatus = uint32_t;

/// @brief invalid supervision status
/// @trace_id_sr=SR_PHM_00002,SR_PHM_00003,SR_PHM_00004,SR_PHM_00005,SR_PHM_00006,
///              SR_PHM_00007,SR_PHM_00008,SR_PHM_00009,SR_PHM_00010,SR_PHM_00011,SR_PHM_00012
/// @needwork = no
SupervisionStatus const kInvalidSupervisionStatus{0xFFFFU};

/// @brief serialized data used in communication.
/// @trace_id_sr=SR_PHM_00002,SR_PHM_00003,SR_PHM_00004,SR_PHM_00005,SR_PHM_00006,
///              SR_PHM_00007,SR_PHM_00008,SR_PHM_00009,SR_PHM_00010,SR_PHM_00011,SR_PHM_00012
/// @needwork = no
using Chunk = ara::core::Vector< std::uint8_t >;

/// @brief type of base supervision.
/// @trace_id_sr=SR_PHM_00002,SR_PHM_00003,SR_PHM_00004,SR_PHM_00005,SR_PHM_00006,
///              SR_PHM_00007,SR_PHM_00008,SR_PHM_00009,SR_PHM_00010,SR_PHM_00011,SR_PHM_00012
/// @needwork = no
enum class BaseSupervisionType : uint32_t
{
    /// @brief Supervision is of type AliveSupervision.
    kAlive = 0,

    /// @brief Supervision is of type DeadlineSupervision.
    kDeadline = 1,

    /// @brief Supervision is of type LogicalSupervision.
    kLogical = 2
};

/// @brief A class used to get time stamp.
/// @trace_id_sr=SR_PHM_00002,SR_PHM_00003,SR_PHM_00004,SR_PHM_00005,SR_PHM_00006,
/// @needwork = no
class TimeStamp
{
public:
    /// @brief Returns the timestamp in ms.
    /// @return timestamp in ms.
    /// @trace_id_sr=SR_PHM_00002,SR_PHM_00003,SR_PHM_00004,SR_PHM_00005,SR_PHM_00006,
    /// @needwork = no
    static int64_t GetMs() noexcept
    {
        int64_t const unit{1000 * 1000};
        return RawTimestampNs() / unit;
    }

private:
    /// @brief Returns the timestamp in ns.
    /// @return timestamp in ns.
    /// @trace_id_sr=SR_PHM_00002,SR_PHM_00003,SR_PHM_00004,SR_PHM_00005,SR_PHM_00006,
    ///              SR_PHM_00007,SR_PHM_00008,SR_PHM_00009,SR_PHM_00010,SR_PHM_00011,SR_PHM_00012
    /// @needwork = no
    static inline int64_t RawTimestampNs() noexcept
    {
        std::chrono::high_resolution_clock::time_point const now{std::chrono::high_resolution_clock::now()};
        std::chrono::high_resolution_clock::time_point::duration const dr{std::move(now.time_since_epoch())};
        std::chrono::nanoseconds const cs{std::move(std::chrono::duration_cast< std::chrono::nanoseconds >(dr))};
        return std::move(cs.count());
    }
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

namespace ara {
namespace phm {

/// @todo Used in SM2.1, so it is placed here separately, needs to be modified later
/// @brief definition of health status id.
/// @trace_id_sr=SR_PHM_00002,SR_PHM_00003,SR_PHM_00004,SR_PHM_00005,SR_PHM_00006,
///              SR_PHM_00007,SR_PHM_00008,SR_PHM_00009,SR_PHM_00010,SR_PHM_00011,SR_PHM_00012
/// @needwork = no
using HealthStatus = uint32_t;

}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_UTIL_TYPE_H_