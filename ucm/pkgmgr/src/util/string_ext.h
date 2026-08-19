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
/// @file       string_ext.h
/// @brief      string extension utility
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
/// @unit_name=StringExt
/// @unit_description=string extension utility
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_UTIL_STRING_EXT_H_
#define ARA_UCM_PKGMGR_UTIL_STRING_EXT_H_

#include <list>

#include "common/alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {
namespace strutil {

/// @brief convert string with format of bigcase
/// @param data source string
/// @returns string in format of bigcase
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10404
/// @trace_id_dd=DD_UCM_10788
/// @needwork = ad
/// @endcode
AraString Upper(AraString const& data);

/// @brief join vector as string
/// @param items a vector
/// @returns string
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10404
/// @trace_id_dd=DD_UCM_10789
/// @needwork = dd
/// @endcode
AraString JoinVector(AraVectorString const& items);

/// @brief join map as string
/// @param items a map
/// @returns string
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10404
/// @trace_id_dd=DD_UCM_10790
/// @needwork = dd
/// @endcode
AraString JoinMap(AraMap< AraString, AraString > const& items);

/// @brief compare between first and second, ignore bigcase and lowcase
/// @param first
/// @param second
/// @returns bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10404
/// @trace_id_dd=DD_UCM_10791
/// @needwork = dd
/// @endcode
bool CaseCompare(AraString const& first, AraString const& second) noexcept;

/// @brief hex string convert to byte vector
/// @param hexStr source hex string
/// @returns byte vector
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10404
/// @trace_id_dd=DD_UCM_10792
/// @needwork = dd
/// @endcode
AraVector< AraByte > HexStrToBytes(AraString const& hexStr);

/// @brief replace the first 'from' to 'to'
/// @param str source string
/// @param from
/// @param to
/// @returns bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10404
/// @trace_id_dd=DD_UCM_10793
/// @needwork = dd
/// @endcode
bool ReplaceFirst(AraString& str, AraString const& from, AraString const& to) noexcept;

/// @brief replace the last 'from' to 'to'
/// @param str source string
/// @param from
/// @param to
/// @returns bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10404
/// @trace_id_dd=DD_UCM_10794
/// @needwork = dd
/// @endcode
bool ReplaceLast(AraString& str, AraString const& from, AraString const& to) noexcept;

/// @brief StrGetOption
/// @param args
/// @param optionName
/// @return option value
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10404
/// @trace_id_dd=DD_UCM_10795
/// @needwork = dd
/// @endcode
AraString GetOption(std::list< AraStringView > const& args, AraStringView const& optionName);

// Get the content within the last pair of [] in the FQN string, such as 'UpdateDemo' in package[UpdateDemoApp].package[exe].executable[UpdateDemo]
AraString GetLastComponent(AraString const& fqn);

}  // namespace strutil
}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_UTIL_STRING_EXT_H_
