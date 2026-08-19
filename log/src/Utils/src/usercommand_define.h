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
/// @file       usercommand_define.h
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/Utils
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00006,LOG_SR_00015
/// @unit_name = usercommand_define
/// @unit_description=Internal support function of Dlt lib, utility class, used for type definitions
/// @endcode
///
/// ================================================================

#ifndef __LOG_INTERNAL_COMMONDEF__
#define __LOG_INTERNAL_COMMONDEF__

namespace ara {
namespace log {
namespace internal {

/// @brief UserCommand - Enumeration value definition of commands sent by user-side AA Client
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00133
/// @trace_id_dd=DD_LOG_00187
/// @needwork = ad
/// @endcode
enum UserCommand
{
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02408
    /// @trace_id_dd=DD_TSYNC_02807
    /// @needwork = ad
    /// @endcode
    kUnknown = -1,

    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02409
    /// @trace_id_dd=DD_TSYNC_02808
    /// @needwork = ad
    /// @endcode
    kRegApp = 0xFF01,

    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02410
    /// @trace_id_dd=DD_TSYNC_02809
    /// @needwork = ad
    /// @endcode
    kUnRegApp = 0xFF02,

    /// @brief  Registration status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02411
    /// @trace_id_dd=DD_TSYNC_02810
    /// @needwork = ad
    /// @endcode
    kRegAppStatusOK = 0xFF03,

    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02412
    /// @trace_id_dd=DD_TSYNC_02811
    /// @needwork = ad
    /// @endcode
    kRegAppStatusFailed = 0xFF04,

    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02413
    /// @trace_id_dd=DD_TSYNC_02812
    /// @needwork = ad
    /// @endcode
    kClientConncted = 0xFF05,

    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02414
    /// @trace_id_dd=DD_TSYNC_02813
    /// @needwork = ad
    /// @endcode
    kClientDisConncted = 0xFF06,

    /// @code{.isoft}
    ///@brief
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02415
    /// @trace_id_dd=DD_TSYNC_02814
    /// @needwork = ad
    /// @endcode
    kUpdateLogChannel = 0xFF07,

    /// @code{.isoft}
    ///@brief
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_03415
    /// @trace_id_dd=DD_TSYNC_03814
    /// @needwork = ad
    /// @endcode
    kUpdateHeartBeat = 0xFF08,

    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02416
    /// @trace_id_dd=DD_TSYNC_02815
    /// @needwork = ad
    /// @endcode
    kLibCommandEnd = 0xFFFF
};

}  // namespace internal
}  // namespace log
}  // namespace ara
#endif