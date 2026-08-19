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
/// @file       errc.h
/// @brief      ucm error domain error code
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/Utils
/// @module_path=/UCM/Utils
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=Errc
/// @unit_description=UCM error domain error code
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_COMMON_ERRC_H_
#define ARA_UCM_PKGMGR_COMMON_ERRC_H_

#include "alias.h"
#include "ara/ucm/pkgmgr/error_domain_ucmerrordomain.h"

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define GetAraResultErrc(ret) static_cast< UCMErrorDomainErrc >((ret).Error().Value())

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnErrcWithLongLog(msg, rettype, errc)                                                                      \
    LOGE << msg << ". return errc:" /* NOLINT */                                                                       \
         << GetUCMErrorDomainErrorDomain().Message(static_cast< ara::core::ErrorDomain::CodeType >(errc));             \
    return AraResult< rettype >::FromError(errc)
/// #define ReturnErrcWithShortLog(rettype, errc)  ReturnErrcWithLongLog("", rettype, errc)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnVoidErrcWithLongLog(msg, errc) ReturnErrcWithLongLog(msg, void, errc)
///#define ReturnVoidErrcWithShortLog(errc)  ReturnErrcWithShortLog(void, errc) //or//
///#define ReturnVoidErrcWithShortLog(errc)  ReturnVoidErrcWithLongLog("", errc)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnErrcEnumWithLongLog(msg, rettype, errcEnum)                                                              \
    ReturnErrcWithLongLog(msg, rettype, UCMErrorDomainErrc::errcEnum)
///#define ReturnErrcEnumWithShortLog(rettype, errcEnum)  ReturnErrcEnumWithLongLog("", rettype, errcEnum)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnVoidErrcEnumWithLongLog(msg, errcEnum) ReturnErrcEnumWithLongLog(msg, void, errcEnum)
///#define ReturnVoidErrcEnumWithShortLog(errcEnum)  ReturnVoidErrcEnumWithLongLog("", errcEnum)

#endif  // ARA_UCM_PKGMGR_COMMON_ERRC_H_
