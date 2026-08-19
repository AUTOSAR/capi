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
/// @file       timer.h
/// @brief      This file provides timer functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/common
/// @interface_level=software
/// @trace_id_sr=6093955b-6ee7-4430-a7c8-8a28597be63b
/// @unit_name=timer
/// @unit_description=This file provides timer functionality
/// @endcode
///
/// ================================================================

#ifndef ARA_DIAG_COMMON_TIMER_H_
#define ARA_DIAG_COMMON_TIMER_H_

#include <ara/core/result.h>
#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>

#include <atomic>
#include <functional>

namespace isoft {
namespace uds {
namespace server {

/// @brief Timer
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
class Timer
{
public:
    /// @param[in] object Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00827
    /// @needwork = dda
    /// @endcode
    Timer(Timer const& object) = delete;

    /// @param[in] object Reference
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00828
    /// @needwork = dda
    /// @endcode
    Timer& operator=(Timer const& object) = delete;

    /// @param[in, out] object Rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00829
    /// @needwork = dda
    /// @endcode
    Timer(Timer&& object) = delete;

    /// @param[in, out] object Rvalue reference
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00830
    /// @needwork = dda
    /// @endcode
    Timer& operator=(Timer&& object) = delete;

    /// @brief Constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    Timer() noexcept = default;

    /// @brief Destructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    virtual ~Timer() noexcept;

    /// @brief Initialize timer
    /// @param[in] handler Timeout handler function
    /// @param[in] duration Timer duration
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(std::function< void() >&& handler, std::uint32_t const duration) noexcept;

    /// @brief Initialize timer
    /// @param[in] handler Timeout handler function
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(std::function< void() >&& handler) noexcept;

    /// @brief Start timer
    /// @param[in] duration Timer duration, unit s
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t Start(std::uint32_t const duration) noexcept;

    /// @brief Start timer
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t Start() noexcept;

    /// @brief Stop timer
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t Stop() noexcept;

    /// @brief Whether running
    /// @return Whether running
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    inline bool IsRunning() const noexcept { return running_.load(); }

private:
    /// @brief Define an invalid constant value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    static std::int32_t const kInvalidDuration{-1};

    /// @brief Timer pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    isoft::naicpp::EvLoop::TimerPtr timer_{};

    /// @brief Timer duration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t duration_{kInvalidDuration};

    /// @brief Whether running
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::atomic_bool running_{false};

    /// @brief Lifetime pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< uint32_t > lifePtr_{nullptr};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ARA_DIAG_COMMON_TIMER_H_