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
/// @file       manifest_crypto_common.h
/// @brief      AutoSar-Crypto Configuration
/// @details    Code for reading arrays from Json format configuration files for Crypto.
/// @date       2021-09-13
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-09-13  <td>1.0.0    <td>      <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Parse Configuration Files
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06004
/// @unit_name=PManifestKspConfig
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_MANIFEST_CRYPTO_COMMON_H_
#define ARA_CRYPTO_MANIFEST_CRYPTO_COMMON_H_

#include <ara/core/string.h>
#include <isoft/manifestreader/manifest.h>

#include <memory>

#include "ara/crypto/common/isoft_log_api.h"

namespace ara {
namespace crypto {
namespace manifest {
//********************************/
/// @brief Template for reading Json arrays
/// @param pManifests Configuration reading object
/// @param stReadKey Key of the data to be read
/// @param pFunc Callback function
/// @return true if read sucess false otherwise
/// @throw ???
/// @code{.isoft}
/// @tparam T_MapData
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01526
/// @trace_id_dd=DD_CRYPTO_03962
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
template < typename T_MapData >
bool ReadMapData(isoft::manifestreader::Manifest *const pManifests,
                 ara::core::String const &stReadKey,
                 std::function< void(T_MapData const &data) > const &pFunc) noexcept(false)
{
    ara::core::Vector< T_MapData > vecData;
    if (isoft::kSuccess != pManifests->Load({stReadKey.data(), stReadKey.size()}, vecData)) {
        ara::crypto::isoft_def::LogWarn() << "PManifestKspConfig::LoadData No Find ConfigName = " << stReadKey.data();
        return false;
    }
    for (auto &itData : vecData) {
        pFunc(itData);
    }

    return true;
}
/// @brief Save read Json data into Map list
/// @param mapT MAP template type
/// @param stName Key for data saved into MAP
/// @param data Data
/// @code{.isoft}
/// @tparam T_Map
/// @tparam T_Data
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01527
/// @trace_id_dd=DD_CRYPTO_03963
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
template < typename T_Map, typename T_Data >
void SaveReadData(T_Map &mapT, ara::core::String const &stName, T_Data const &data) noexcept
{
    auto pFind = mapT.find(stName);
    if (pFind != mapT.end() && nullptr != pFind->second) {
        *(pFind->second) = data;
    } else {
        auto *pNewData = new T_Data;
        *pNewData      = data;
        mapT[stName]   = pNewData;
    }
}
//********************************/
}  // namespace manifest
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_MANIFEST_CRYPTO_COMMON_H_
