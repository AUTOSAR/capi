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
/// @file       utils.h
/// @brief      Utility management: common methods
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/utils
/// @interface_level=Module
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=utils
/// @unit_description=Utility management: common methods
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_UTILS_H_
#define _ARA_NM_UTILS_H_
#include <ara/core/map.h>
#include <ara/log/logger.h>
#include <isoft/naicpp/evnode_timer.h>
#include <isoft/naicpp/global_evloop.h>

#include <cmath>
#include <cstdint>

#include "../../common/common.h"
#ifdef ARA_NM_WITH_COM
    #include "ara/nm/networkstate_skeleton.h"
#endif

namespace ara {
namespace nm {
namespace internal {
/// @brief
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100061
/// @trace_id_dd=DD_NM_00805
/// @needwork = ad
/// @endcode
double const kSecond2MicroSecond{1000.0};

/// @brief
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100062
/// @trace_id_dd=DD_NM_00806
/// @needwork = ad
/// @endcode
std::uint8_t const kCharBitsCount{8U};

/// @brief get millisecond
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100063
/// @trace_id_dd=DD_NM_00807
/// @needwork = ad
/// @endcode
inline std::int32_t TimeS2MS(double const s) noexcept { return static_cast< std::int32_t >(s * kSecond2MicroSecond); }

/// @brief Log output interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100064
/// @trace_id_dd=DD_NM_00808
/// @needwork = ad
/// @endcode
static inline ara::log::Logger &NmLogger() noexcept
{
    static ara::log::Logger &s_Logger{ara::log::CreateLogger(std::move(ara::core::StringView("NMDM")),
                                                             std::move(ara::core::StringView("Network management")),
                                                             ara::log::LogLevel::kVerbose)};
    return s_Logger;
}

/// @brief Timer creation interface
/// @param pEventLoop eventloop pointer.
/// @param timer timer pointer reference.
/// @param msecond millisecond.
/// @param onTimerCallback callback.
/// @returns millisecond
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100065
/// @trace_id_dd=DD_NM_00809
/// @needwork = ad
/// @endcode
inline std::int32_t NmMakeTimer(std::shared_ptr< isoft::naicpp::EvLoop > const &pEventLoop,
                                isoft::naicpp::EvLoop::TimerPtr &timer,
                                std::int32_t const msecond,
                                std::function< void() > &&onTimerCallback) noexcept
{
    std::int32_t ret{0};
    if (0 < msecond) {
        ret = pEventLoop->MakeTimer(timer, msecond, std::move(onTimerCallback));
    } else if (0 == msecond) {
        std::function< void() > const callbak{std::move(onTimerCallback)};
        callbak();
    } else {
        std::ignore = std::move(onTimerCallback);  /// just for qac
    }
    return ret;
}

/// @brief check double second limit,  max int
/// @param seconds time value.
/// @returns true valid, false invalid
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100066
/// @trace_id_dd=DD_NM_00810
/// @needwork = ad
/// @endcode
inline bool CheckTimerMaxium(double const seconds) noexcept
{
    std::int32_t const bits{static_cast< std::int32_t >(sizeof(std::int32_t) * kCharBitsCount - 1U)};
    double const maxint{exp2(static_cast< double >(bits)) - 1.0};
    bool bValid{false};
    if ((seconds * kSecond2MicroSecond) > maxint) {
    } else {
        bValid = true;
    }
    return bValid;
}

/// @brief Convert internal state machine state to COM communication state
/// @param ethState Internal state machine state.
/// @returns COM communication state
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100067
/// @trace_id_dd=DD_NM_00811
/// @needwork = ad
/// @endcode
inline nm::NetworkStateType GetComSMState(EthernetStateMachineStateType const &ethState) noexcept
{
    ara::core::Map< EthernetStateMachineStateType, nm::NetworkStateType > stateMap;
    stateMap[EthernetStateMachineStateType::kStateBusSleep]        = nm::NetworkStateType::kBusSleep;
    stateMap[EthernetStateMachineStateType::kStatePrepareBusSleep] = nm::NetworkStateType::kPrepareBusSleep;
    stateMap[EthernetStateMachineStateType::kStateRepeatMessage]   = nm::NetworkStateType::kRepeatMessageState;
    stateMap[EthernetStateMachineStateType::kStateNormalOperation] = nm::NetworkStateType::kNormalOperation;
    stateMap[EthernetStateMachineStateType::kStateReadySleep]      = nm::NetworkStateType::kReadySleep;
    return stateMap[ethState];
}

/// @brief Convert COM communication state to internal state machine state
/// @param comState COM communication state
/// @returns Internal state machine state.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100068
/// @trace_id_dd=DD_NM_00812
/// @needwork = ad
/// @endcode
inline EthernetStateMachineStateType GetInnerSMState(nm::NetworkStateType const &comState) noexcept
{
    ara::core::Map< nm::NetworkStateType, EthernetStateMachineStateType > stateMap;
    stateMap[nm::NetworkStateType::kNoCom]              = EthernetStateMachineStateType::kStateBusSleep;
    stateMap[nm::NetworkStateType::kFullCom]            = EthernetStateMachineStateType::kStateBusSleep;
    stateMap[nm::NetworkStateType::kBusSleep]           = EthernetStateMachineStateType::kStateBusSleep;
    stateMap[nm::NetworkStateType::kPrepareBusSleep]    = EthernetStateMachineStateType::kStatePrepareBusSleep;
    stateMap[nm::NetworkStateType::kRepeatMessageState] = EthernetStateMachineStateType::kStateRepeatMessage;
    stateMap[nm::NetworkStateType::kNormalOperation]    = EthernetStateMachineStateType::kStateNormalOperation;
    stateMap[nm::NetworkStateType::kReadySleep]         = EthernetStateMachineStateType::kStateReadySleep;
    return stateMap[comState];
}

}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  // _ARA_NM_UTILS_H_
