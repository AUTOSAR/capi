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
/// @file       event.h
/// @brief      Event class definition
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/UCM Master/Fsm
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=Event
/// @unit_description=Event class definition
/// @endcode
///
/// ================================================================

#ifndef _VPKGMGR_COMMON_EVENT_H_
#define _VPKGMGR_COMMON_EVENT_H_

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Event trigger wait.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00479
/// @trace_id_dd=DD_UCM_Master_00218
/// @needwork = ad
/// @endcode
class Event final
{
public:
    /// @brief ResultType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00479
    /// @trace_id_dd=DD_UCM_Master_00219
    /// @needwork = dd
    /// @endcode
    enum class ResultType : uint8_t
    {
        kSuccess = 0U,
        kFail,
        kTimeout
    };

public:
    /// @brief constructor
    /// @param sigs Initial number of signals, default 0
    /// @throws no no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00220
    /// @needwork = dda
    /// @endcode
    explicit Event(int32_t const sigs = 0) noexcept : mtx_{}, cv_{}, signals_{sigs} {}
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00221
    /// @needwork = dda
    /// @endcode
    ~Event() = default;

    /// @brief Copy constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00222
    /// @needwork = dda
    /// @endcode
    Event(Event const& other) = delete;
    /// @brief Move constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00223
    /// @needwork = dda
    /// @endcode
    Event(Event&& other) = delete;
    /// @brief Copy assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00224
    /// @needwork = dda
    /// @endcode
    Event& operator=(Event const& other) = delete;
    /// @brief Move assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00225
    /// @needwork = dda
    /// @endcode
    Event& operator=(Event&& other) = delete;

    /// @brief Wait for signal trigger
    /// @param timeoutMs Maximum wait time in milliseconds
    /// @return
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00226
    /// @needwork = dda
    /// @endcode
    ResultType Wait(int32_t const timeoutMs = (-1));

    /// @brief Trigger signal
    /// @param count Number of signals to trigger, default 1
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00227
    /// @needwork = dda
    /// @endcode
    void Signal(int32_t const count = 1) noexcept;

    /// @brief Increase the number of signals to wait for
    /// @param count Number of signals to increase by
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00228
    /// @needwork = dda
    /// @endcode
    void Add(int32_t const count = 1) noexcept;

    /// @brief Reset
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00229
    /// @needwork = dda
    /// @endcode
    void Reset() noexcept;

    /// @brief get count of signals
    /// @return count of signals
    /// @throws no no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00230
    /// @needwork = dda
    /// @endcode
    inline int32_t GetCount() const noexcept { return signals_.load(); }

private:
    /// @brief mutex
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00231
    /// @needwork = dda
    /// @endcode
    std::mutex mtx_;

    /// @brief condition variable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00232
    /// @needwork = dda
    /// @endcode
    std::condition_variable cv_;

    /// @brief count of signals
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00233
    /// @needwork = dda
    /// @endcode
    std::atomic< int32_t > signals_;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // _VPKGMGR_COMMON_EVENT_H_