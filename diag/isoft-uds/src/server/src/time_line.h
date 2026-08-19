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
/// @file       time_line.h
/// @brief      This file provides time_line functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds
/// @interface_level=software
/// @trace_id_sr=a9db15c8-1930-4a67-a0bb-fa170bbfac09
/// @unit_name=TimeLine
/// @unit_description=This file provides time_line functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_TIMELINE_H_
#define ISOFT_UDS_SERVER_TIMELINE_H_

#include <memory>

#include "common/request_context.h"
namespace isoft {
namespace uds {
namespace server {
class Timer;
using NotifyOfSessionChangeCallback   = std::function< void(SessionId, SessionId) >;
using NotifyOfSessionChangeCallbackId = std::uint32_t;
using P2TimerId                       = std::uint32_t;
using TimerPtr                        = std::shared_ptr< Timer >;
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00579
/// @needwork = dd
/// @endcode
class TimeLine final
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00580
    /// @needwork = dda
    /// @endcode
    TimeLine(TimeLine const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00581
    /// @needwork = dda
    /// @endcode
    TimeLine& operator=(TimeLine const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00582
    /// @needwork = dda
    /// @endcode
    TimeLine(TimeLine&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00583
    /// @needwork = dda
    /// @endcode
    TimeLine& operator=(TimeLine&&) = delete;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00584
    /// @needwork = dda
    /// @endcode
    TimeLine() noexcept;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00585
    /// @needwork = dda
    /// @endcode
    ~TimeLine() noexcept;


    /// @brief StartP2Timer function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00586
    /// @needwork = dda
    /// @endcode
    P2TimerId StartP2Timer(std::shared_ptr< RequestContext > requestContextPtr) noexcept;

    /// @brief StopP2Timer function
    /// @param id Identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00587
    /// @needwork = dda
    /// @endcode
    void StopP2Timer(P2TimerId id) noexcept;

    /// @brief StartS3Timer function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00588
    /// @needwork = dda
    /// @endcode
    void StartS3Timer() noexcept;

    /// @brief StopS3Timer function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00589
    /// @needwork = dda
    /// @endcode
    void StopS3Timer() noexcept;

private:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_
    /// @trace_id_dd=DD_UDS_00590
    /// @needwork = dd
    /// @endcode
    struct P2TimerParamter
    {
        std::shared_ptr< RequestContext > requestContextPtr;
        std::uint32_t countOfTimeout;
    };
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00591
    /// @needwork = dda
    /// @endcode
    std::map< NotifyOfSessionChangeCallbackId, NotifyOfSessionChangeCallback > notifyOfSessionChangeCallbackTable_;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00592
    /// @needwork = dda
    /// @endcode
    std::map< P2TimerId, TimerPtr > p2TimerTable_;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00593
    /// @needwork = dda
    /// @endcode
    SessionId currentSessionId_;
    //DiagnosticSessionPtrTable& table;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_TIMELINE_H_