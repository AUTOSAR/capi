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
/// @file       manifest_fw_common.h
/// @brief      AutoSar-fw configuration
/// @details    Code for reading arrays in fw's Json format configuration file
/// @date       2024-12-27
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/FW/configuration management
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0003
/// @unit_name=Manifest_Common
/// @unit_description=Configuration file parsing processing function
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_MANIFEST_FW_COMMON_H_
#define ARA_FW_MANIFEST_FW_COMMON_H_

#include <ara/core/string.h>
#include <isoft/manifestreader/manifest.h>

#include <memory>

#include "ara/fw/internal/log_api.h"

namespace ara {
namespace fw {
namespace internal {

/// @brief Template for reading Json arrays
/// @param manifests Configuration read object
/// @param stReadKey Key of the data to read
/// @param pFunc Callback function
/// @return Success/Failure
/// @throws
/// @code{.isoft}
/// @tparam T_MapData
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00175
/// @trace_id_dd=DD_FW_00258
/// @needwork = ad
/// @endcode
template < typename T_MapData >
bool ReadMapData(std::unique_ptr< isoft::manifestreader::Manifest > const &manifests,
                 ara::core::String const &stReadKey,
                 std::function< void(T_MapData const &data) > const &pFunc)
{
    ara::core::Vector< T_MapData > vecData;
    if (isoft::kSuccess != manifests->Load({stReadKey.data(), stReadKey.size()}, vecData)) {
        LogWarn() << "ReadMapData::LoadData No Find ConfigName = " << stReadKey.data();
        return false;
    }
    for (auto &itData : vecData) {
        pFunc(itData);
    }

    return true;
}
/// @brief Save the read Json data to the Map list
/// @param tMap MAP template type
/// @param stName KEY of the data saved to MAP
/// @param data Data
/// @throws
/// @code{.isoft}
/// @tparam T_Map
/// @tparam T_Data
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00176
/// @trace_id_dd=DD_FW_00259
/// @needwork = ad
/// @endcode
template < typename T_Map, typename T_Data >
void SaveReadData(T_Map &tMap, ara::core::String const &stName, T_Data const &data)
{
    auto pFind = tMap.find(stName);
    if (pFind != tMap.end() && nullptr != pFind->second) {
        *(pFind->second) = data;
    } else {
        T_Data *pNewData = new T_Data;
        *pNewData        = data;
        tMap[stName]     = pNewData;
    }
}

}  // namespace internal
}  // namespace fw
}  // namespace ara

#endif  // ARA_FW_MANIFEST_FW_COMMON_H_
