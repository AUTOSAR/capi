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
/// @file       udpnm.h
/// @brief      Internal object organization management
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/nmm
/// @interface_level=unit
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=UdpNm
/// @unit_description=Internal object organization management
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_UDPNM_H_
#define _ARA_NM_UDPNM_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include "logicnetworkoper.h"
#if ARA_NM_WITH_DIAG
    #include "diagdtcserver.h"
#endif

namespace ara {
namespace nm {
namespace internal {

/// @brief UdpNm, the top management class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100056
/// @trace_id_dd=DD_NM_00795
/// @needwork = ad
/// @endcode
class UdpNm final
{
public:
    /// @brief constructor of UdpNm.
    /// @throws  Throws exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00358
    /// @needwork = dda
    /// @endcode
    UdpNm() = default;

    /// @brief copy constructor is forbidden.
    ///
    /// @param other class instance.
    /// @code{.isoft}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00359
    /// @needwork = dda
    /// @endcode
    UdpNm(UdpNm const &other) = delete;

    /// @brief copy operator is forbidden.
    ///
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00360
    /// @needwork = dda
    /// @endcode
    UdpNm &operator=(UdpNm const &other) = delete;

    /// @brief move constructor is default
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00361
    /// @needwork = dda
    /// @endcode
    UdpNm(UdpNm &&other) = default;

    /// @brief move operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00362
    /// @needwork = dda
    /// @endcode
    UdpNm &operator=(UdpNm &&other) = default;

    /// @brief destructor of UdpNm.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00363
    /// @needwork = dda
    /// @endcode
    ~UdpNm() = default;

    /// @brief udpnm initialization
    /// @returns 0 ok
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00062}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00364
    /// @needwork = dda
    /// @endcode
    std::int32_t Open() noexcept;

    /// @brief udpnm close
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00365
    /// @needwork = dda
    /// @endcode
    void Close() noexcept;

#if ARA_NM_DEBUG
    /// @brief request logic network, for test
    /// @param logicHandle logic network handle
    /// @returns kNmOperOK ok
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00366
    /// @needwork = dda
    /// @endcode
    NmOperCode LogicNetworkRequest(ara::core::String const &logicHandle) noexcept;

    /// @brief release logic network, for test
    /// @param logicHandle logic network handle
    /// @returns kNmOperOK ok
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00367
    /// @needwork = dda
    /// @endcode
    NmOperCode LogicNetworkRelease(ara::core::String const &logicHandle) noexcept;
#endif

#if ARA_NM_WITH_DIAG
    /// @brief Whether diagnostic events need to be recorded
    /// @returns true if diagnostic events need to be recorded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00368
    /// @needwork = dda
    /// @endcode
    bool HasDtcRecord() const noexcept
    {
        bool const bHaveDid{false == configure_->GetDidInstance().empty()};
        bool const bHaveMonitor{false == configure_->GetDiagMonitorId().empty()};
        return (bInitEtherError_ && (true == bHaveDid) && (true == bHaveMonitor));
    };
#endif

private:
    /// @brief If initialization problems occur, record diagnostic events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00369
    /// @needwork = dda
    /// @endcode
    void _writeDtcRecord() noexcept;

private:
    /// @brief logicHandleMap_
    /// collection of logic network
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00370
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, std::shared_ptr< LogicNetworkOper > > logicHandleMap_{};

    /// @brief partialNetworkMap_
    /// collection of partial network
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00371
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, std::shared_ptr< PartialNetworkOper > > partialNetworkMap_{};

    /// @brief stateMachineRes_
    /// collection of ethernet communicator state machine
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00372
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, std::shared_ptr< NMEtherStateMachine > > stateMachineRes_{};

    /// @brief configure_
    /// json configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00373
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< Configure > configure_{nullptr};

#if ARA_NM_WITH_DIAG
    /// @brief pDiagDtcServer_
    /// Related to recording diagnostic events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00374
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< DiagDtcServer > pDiagDtcServer_{nullptr};
#endif

    /// @brief bInitEtherError_
    /// Whether initialization error occurred
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00375
    /// @needwork = dda
    /// @endcode
    bool bInitEtherError_{true};

    /// @brief dtcError_
    /// Diagnostic error string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00376
    /// @needwork = dda
    /// @endcode
    ara::core::String dtcError_{};

    /// @brief pthread_
    /// Recording diagnostic events requires a new thread, must be in a new thread, otherwise the main thread will be blocked
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00377
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< std::thread > pthread_{nullptr};

    /// @brief dtctimer_
    /// Diagnostic event recording delay timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00378
    /// @needwork = dda
    /// @endcode
    isoft::naicpp::EvLoop::TimerPtr dtctimer_{nullptr};
};  // class UdpNm

}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  //_ARA_NM_UDPNM_H_
