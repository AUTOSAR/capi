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
/// @brief      Utility functions
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/UCM Master/Utils
/// @module_path=/UCM Master/Utils
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=Utils
/// @unit_description=Utility functions
/// @endcode
///
/// ================================================================

#ifndef _ARA_UCM_VPKGMGR_COMMON_UTILS_H_
#define _ARA_UCM_VPKGMGR_COMMON_UTILS_H_

#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <ara/ucm/internal/transfer/helper.h>
#include <nai/os/nai_dirent.h>
#include <nai/os/nai_file.h>
#include <nai/os/nai_stat.h>
#include <nai/runtime/nai_errno.h>

#include <chrono>
#include <utility>

#include "alias.h"
#include "consts.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Utils
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00412
/// @trace_id_dd=DD_UCM_Master_00821
/// @needwork = ad
/// @endcode
class Utils
{
public:
    /// @brief GetTime
    /// @return milliseconds
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00413
    /// @trace_id_dd=DD_UCM_Master_00822
    /// @needwork = ad
    /// @endcode
    static uint64_t GetTime()
    {
        return static_cast< uint64_t >(
            std::chrono::time_point_cast< std::chrono::milliseconds >(std::chrono::system_clock::now())
                .time_since_epoch()
                .count());
    }

    /// @brief ConcatenateStrings
    /// @param strVec
    /// @return str
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00414
    /// @trace_id_dd=DD_UCM_Master_00823
    /// @needwork = ad
    /// @endcode
    static ara::core::String ConcatenateStrings(ara::core::Vector< ara::core::String > const& strVec)
    {
        ara::core::String strRet;
        for (ara::core::String const& it : strVec) {  // PRQA S 2961
            std::ignore = strRet.append(it);
            if (&it != &strVec.back()) {
                std::ignore = strRet.append(",");
            }
        }
        return strRet;
    }

    /// @brief ConcatenateStrings
    /// @param strVec
    /// @return str
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00415
    /// @trace_id_dd=DD_UCM_Master_00824
    /// @needwork = ad
    /// @endcode
    static ara::core::String ConcatenateStrings(AraList< ara::core::String > const& strList)
    {
        ara::core::String strRet;
        for (ara::core::String const& it : strList) {  // PRQA S 2961
            std::ignore = strRet.append(it);
            if (&it != &strList.back()) {
                std::ignore = strRet.append(",");
            }
        }
        return strRet;
    }

    /// @brief ConcatenateStrings
    /// @param strSet
    /// @return str
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00416
    /// @trace_id_dd=DD_UCM_Master_00825
    /// @needwork = ad
    /// @endcode
    static ara::core::String ConcatenateStrings(std::set< ara::core::String > const& strSet)
    {
        ara::core::String strRet;
        for (ara::core::String const& it : strSet) {
            std::ignore = strRet.append(it);
            std::ignore = strRet.append(",");
        }
        if (!strRet.empty()) {  // Remove the trailing comma
            strRet.pop_back();
        }
        return strRet;
    }

    /// @brief ConcatenateStrings
    /// @param swpkFQN2TransferIDMap
    /// @return str
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00417
    /// @trace_id_dd=DD_UCM_Master_00826
    /// @needwork = ad
    /// @endcode
    static ara::core::String ConcatenateStrings(
        ara::core::Map< ara::core::String, pkgmgr::TransferIdType > const& swpkFQN2TransferIDMap)
    {
        ara::core::String strRet;
        for (auto const& it : swpkFQN2TransferIDMap) {
            std::ignore = strRet.append(it.first);
            std::ignore = strRet.append(",");
            std::ignore = strRet.append(pkgmgr::helper::ToHexString(it.second).c_str());
        }
        return strRet;
    }

    /// @brief Split
    /// @param src
    /// @param delimiters
    /// @return vector of str
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00418
    /// @trace_id_dd=DD_UCM_Master_00827
    /// @needwork = ad
    /// @endcode
    static ara::core::Vector< ara::core::String > Split(ara::core::String const& src,  // Currently not effectively used
                                                        ara::core::String const& delimiters)
    {
        ///ara::core::String const& delimiters = " "
        ara::core::Vector< ara::core::String > tokens;
        ara::core::String::size_type lastPos{src.find_first_not_of(delimiters, 0U)};
        ara::core::String::size_type pos{src.find_first_of(delimiters, lastPos)};
        ///while ((ara::core::String::npos != pos) || (ara::core::String::npos != lastPos)) {
        while (true) {
            if ((ara::core::String::npos == pos) && (ara::core::String::npos == lastPos)) {
                break;
            }
            tokens.push_back(src.substr(lastPos, pos - lastPos));
            lastPos = src.find_first_not_of(delimiters, pos);
            pos     = src.find_first_of(delimiters, lastPos);
        }
        return tokens;
    }
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // !_ARA_UCM_VPKGMGR_COMMON_UTILS_H_