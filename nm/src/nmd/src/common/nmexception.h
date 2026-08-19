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
/// @file       nmexception.h
/// @brief      Network management exception management
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
/// @unit_description=Network management exception management
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_EXCEPTION_H_
#define _ARA_NM_EXCEPTION_H_
#include <ara/core/error_code.h>
#include <ara/core/error_domain.h>
#include <ara/core/exception.h>

#include "common.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief Type simplification
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100000
/// @trace_id_dd=DD_NM_00969
/// @needwork = ad
/// @endcode
using Exception = core::Exception;

/// @brief Type simplification
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100001
/// @trace_id_dd=DD_NM_00970
/// @needwork = ad
/// @endcode
using ErrorCode = core::ErrorCode;

/// @brief Exception type thrown for CORE errors.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100048
/// @trace_id_dd=DD_NM_00787
/// @needwork = ad
/// @endcode
class NmException : public Exception
{
public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00971
    /// @needwork = dda
    /// @endcode
    using Exception::Exception;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00174
    /// @needwork = dda
    /// @endcode
    ~NmException() final = default;
    /// @brief Copy constructor
    /// @param other The FGInfoConfig instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00175
    /// @needwork = dda
    /// @endcode
    NmException(NmException const &other) = default;
    /// @brief Move copy
    /// @param other The FGInfoConfig instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00176
    /// @needwork = dda
    /// @endcode
    NmException(NmException &&other) = default;
    /// @brief Assignment function
    /// @param other The FGInfoConfig instance to be moved
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00177
    /// @needwork = dda
    /// @endcode
    NmException &operator=(NmException const &other) = delete;
    /// @brief Move assignment function
    /// @param other The FGInfoConfig instance to be moved
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00178
    /// @needwork = dda
    /// @endcode
    NmException &operator=(NmException &&other) = delete;
};
}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  //_ARA_NM_EXCEPTION_H_