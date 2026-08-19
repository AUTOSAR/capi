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
/// @file       daemon.h
/// @brief      NM service process management
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
/// @unit_name=Daemon
/// @unit_description=NM service process management
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_DAEMON_H_
#define _ARA_NM_DAEMON_H_

#include <ara/core/string.h>
#include <ara/core/vector.h>

#include "common/common.h"
#include "config/include/configure.h"
#include "nmm/include/udpnm.h"
#include "utils/include/utils.h"
namespace ara {
namespace nm {
namespace internal {

/// @brief Daemon class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100006
/// @trace_id_dd=DD_NM_00898
/// @needwork = ad
/// @endcode
class Daemon final
{
public:
    /// @brief constructor of Daemon.
    /// @throws  Throws exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00729
    /// @needwork = dda
    /// @endcode
    Daemon() noexcept : udpNm_{}, mainLoop_{nullptr} {};

    /// @brief copy constructor of Daemon.
    ///
    /// @param other reference of object.
    /// @code{.isoft}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00730
    /// @needwork = dda
    /// @endcode
    Daemon(Daemon &other) = delete;

    /// @brief move constructor of Daemon.
    ///
    /// @param other reference of object.
    /// @code{.isoft}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00731
    /// @needwork = dda
    /// @endcode
    Daemon(Daemon &&other) = delete;

    /// @brief assignment operator of Daemon.
    ///
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00732
    /// @needwork = dda
    /// @endcode
    Daemon &operator=(Daemon const &other) = delete;

    /// @brief move assignment operator of Daemon.
    ///
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00733
    /// @needwork = dda
    /// @endcode
    Daemon &operator=(Daemon &&other) = delete;

    /// @brief Destructor of Daemon.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00734
    /// @needwork = dda
    /// @endcode
    ~Daemon() = default;

    /// @brief run of an Daemon.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00735
    /// @needwork = dda
    /// @endcode
    void Run() noexcept;

private:
    /// @brief udpNm_
    /// udpnm member object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00736
    /// @needwork = dda
    /// @endcode
    UdpNm udpNm_;

    /// @brief mainLoop_
    /// event loop handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00737
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_;

};  // class Daemon
}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  // _ARA_NM_DAEMON_H_
