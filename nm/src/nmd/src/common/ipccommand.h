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
/// @file       ipccommand.h
/// @brief      IPC internal command definitions
/// @details
/// @date       2024-05-21
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
/// @unit_description=IPC internal command definitions
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_IPCCOMMAND_H_
#define _ARA_NM_IPCCOMMAND_H_

#include <ara/core/map.h>

#include <cmath>
#include <cstdint>

namespace ara {
namespace nm {
namespace internal {

/// @brief IPC command definitions
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100000
/// @trace_id_dd=DD_NM_00738
/// @needwork = ad
/// @endcode
enum class IpcCommand : std::int32_t
{
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00928
    /// @needwork = dda
    /// @endcode
    kUnknown = -1,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00929
    /// @needwork = dda
    /// @endcode
    kGetNetworkRequestState = 0,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00930
    /// @needwork = dda
    /// @endcode
    kSetNetworkRequestState = 1,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00931
    /// @needwork = dda
    /// @endcode
    kGetNetworkCurrentState = 2,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00932
    /// @needwork = dda
    /// @endcode
    kRegisterNetworkStateChangeNotifier = 3,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00933
    /// @needwork = dda
    /// @endcode
    kUnregisterNetworkStateChangeNotifier = 4,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00934
    /// @needwork = dda
    /// @endcode
    kRegisterNetworkRequestedStateChangeNotifier = 5,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00935
    /// @needwork = dda
    /// @endcode
    kUnregisterNetworkRequestedStateChangeNotifier = 6,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00936
    /// @needwork = dda
    /// @endcode
    kUpdateNetworkStateChange = 7,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00937
    /// @needwork = dda
    /// @endcode
    kUpdateNetworkRequestedStateChange = 8,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00938
    /// @needwork = dda
    /// @endcode
    kSetEtherState = 9,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00939
    /// @needwork = dda
    /// @endcode
    kGetEtherState = 10,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00940
    /// @needwork = dda
    /// @endcode
    kRequestDetectNode = 11,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00941
    /// @needwork = dda
    /// @endcode
    kNotifyWakeup = 12,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00942
    /// @needwork = dda
    /// @endcode
    kSetMessageCtrlType = 13,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00943
    /// @needwork = dda
    /// @endcode
    kGetMessageCtrlType = 14,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00944
    /// @needwork = dda
    /// @endcode
    kRegisterEtherStateChanged = 15,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00945
    /// @needwork = dda
    /// @endcode
    kUnregisterEtherStateChanged = 16,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00946
    /// @needwork = dda
    /// @endcode
    kGetInitState = 17,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00947
    /// @needwork = dda
    /// @endcode
    kGetExternalPnRequestList = 18,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00948
    /// @needwork = dda
    /// @endcode
    kNotifyPresentNodeList = 19,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00949
    /// @needwork = dda
    /// @endcode
    kNotifyEtherStateChanged = 20,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00950
    /// @needwork = dda
    /// @endcode
    kNotifyExternalPnRequest = 21,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00951
    /// @needwork = dda
    /// @endcode
    kNotifyExternalPnRelease = 22,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00952
    /// @needwork = dda
    /// @endcode
    kRegisterExternalPnRequest = 23,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00953
    /// @needwork = dda
    /// @endcode
    kUnregisterExternalPnRequest = 24,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00954
    /// @needwork = dda
    /// @endcode
    kRegisterExternalPnRelease = 25,
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00955
    /// @needwork = dda
    /// @endcode
    kUnregisterExternalPnRelease = 26,
};

}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  // _ARA_NM_IPCCOMMAND_H_
