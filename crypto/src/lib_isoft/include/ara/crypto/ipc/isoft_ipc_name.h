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
/// @file       isoft_ipc_name.h
/// @brief      AutoSar-Crypto Key Storage Module
/// @details    IPC communication names for KeyProvider providers
/// @date       2022-08-01
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-08-01  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Function Modules
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Common_api
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_IPC_NAME_H_
#define ARA_CRYPTO_KEYS_PUHUA_IPC_NAME_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include <cstdint>

#include "ara/crypto/common/isoft_data_type.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Externally provided IPC name
//********************************/
/// @brief Gets IPC name: cryptod
/// @return Socket name used for IPC communication
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01979
/// @trace_id_dd=DD_CRYPTO_04673
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkIpcFile_Server() noexcept
{
    return "cryptod";
}  // Name bound to user, each defines their own: specific name synthesized by IPC based on process and this name
/// @brief Gets IPC name: cryp
/// @return IPC name
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01980
/// @trace_id_dd=DD_CRYPTO_04674
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkIpcName_Server() noexcept { return "cryp"; }

/// @brief Gets IPC name: cryptox509d
/// @return Socket name used for IPC communication
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01981
/// @trace_id_dd=DD_CRYPTO_04675
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkIpcFile_Server_x509() noexcept
{
    return "cryptox509d";
}  // Name bound to user, each defines their own: specific name synthesized by IPC based on process and this name
/// @brief Gets IPC name: x509
/// @return IPC name
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01982
/// @trace_id_dd=DD_CRYPTO_04676
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkIpcName_Server_x509() noexcept { return "x509"; }

/// @brief Gets IPC name: cryptohsmd
/// @return Socket name used for IPC communication
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01983
/// @trace_id_dd=DD_CRYPTO_04677
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkIpcFile_Server_hsm() noexcept
{
    return "cryptohsmd";
}  // Name bound to user, each defines their own: specific name synthesized by IPC based on process and this name
/// @brief Gets IPC name: hsmd
/// @return IPC name
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01984
/// @trace_id_dd=DD_CRYPTO_04678
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkIpcName_Server_hsm() noexcept { return "hsmd"; }
//********************************/
/// @brief Gets IPC name: cryptohsmbstd hsm-bst // Cannot exceed four characters
/// @return Socket name used for IPC communication
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01985
/// @trace_id_dd=DD_CRYPTO_04679
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkIpcFile_Server_hsmbst() noexcept
{
    return "cryptohsmbstd";
}  // Name bound to user, each defines their own: specific name synthesized by IPC based on process and this name
/// @brief Gets IPC name: bstd
/// @return IPC name
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01986
/// @trace_id_dd=DD_CRYPTO_04680
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkIpcName_Server_hsmbst() noexcept { return "bstd"; }
//****************************************************************************************************************************************/
/// @brief Gets string
/// @return "cryptod"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01987
/// @trace_id_dd=DD_CRYPTO_04681
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::String GetIpcFile_Server_Keys() noexcept
{
    ara::core::String stReturn{GetkIpcFile_Server()};
    return stReturn;
}
/// @brief Gets string
/// @return "cryp"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01988
/// @trace_id_dd=DD_CRYPTO_04682
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::String GetIpcName_Server_Keys() noexcept
{
    ara::core::String stReturn{GetkIpcName_Server()};
    return stReturn;
}
/// @brief Assembled by the above two functions, returns IPC communication name
/// @return "cryptod/cryp"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01989
/// @trace_id_dd=DD_CRYPTO_04683
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::String GetIpcName_Remote_Keys() noexcept
{
    ara::core::String stReturn(GetIpcFile_Server_Keys());
    stReturn += "/";
    stReturn += GetIpcName_Server_Keys();
    return stReturn;
}

//****************************************************************************************************************************************/
/// @brief Gets string
/// @return "cryptox509d"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01990
/// @trace_id_dd=DD_CRYPTO_04684
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::String GetIpcFile_Server_X509() noexcept
{
    ara::core::String stReturn{GetkIpcFile_Server_x509()};
    return stReturn;
}
/// @brief Gets string
/// @return "x509"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01991
/// @trace_id_dd=DD_CRYPTO_04685
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::String GetIpcName_Server_X509() noexcept
{
    ara::core::String stReturn{GetkIpcName_Server_x509()};
    return stReturn;
}
/// @brief Assembled by the above two functions, returns IPC communication name
/// @return "cryptox509d/x509"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01992
/// @trace_id_dd=DD_CRYPTO_04686
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::String GetIpcName_Remote_X509() noexcept
{
    ara::core::String stReturn(GetIpcFile_Server_X509());
    stReturn += "/";
    stReturn += GetIpcName_Server_X509();
    return stReturn;
}

//****************************************************************************************************************************************/
/// @brief Gets string
/// @return "cryptohsmd"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01993
/// @trace_id_dd=DD_CRYPTO_04687
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::String GetIpcFile_Server_Hsm() noexcept
{
    ara::core::String stReturn{GetkIpcFile_Server_hsm()};
    return stReturn;
}
/// @brief Gets string
/// @return "hsmd"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01994
/// @trace_id_dd=DD_CRYPTO_04688
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::String GetIpcName_Server_Hsm() noexcept
{
    ara::core::String stReturn{GetkIpcName_Server_hsm()};
    return stReturn;
}
/// @brief Assembled by the above two functions, returns IPC communication name
/// @return "cryptohsmd/hsmd"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01995
/// @trace_id_dd=DD_CRYPTO_04689
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::String GetIpcName_Remote_Hsm() noexcept
{
    ara::core::String stReturn(GetIpcFile_Server_Hsm());
    stReturn += "/";
    stReturn += GetIpcName_Server_Hsm();
    return stReturn;
}

//****************************************************************************************************************************************/
/// @brief Gets string
/// @return "cryptohsmbstd"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01996
/// @trace_id_dd=DD_CRYPTO_04690
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::String GetIpcFile_Server_Hsmbst() noexcept
{
    ara::core::String stReturn{GetkIpcFile_Server_hsmbst()};
    return stReturn;
}
/// @brief Gets string
/// @return "hsmbstd"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01997
/// @trace_id_dd=DD_CRYPTO_04691
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::String GetIpcName_Server_Hsmbst() noexcept
{
    ara::core::String stReturn{GetkIpcName_Server_hsmbst()};
    return stReturn;
}
/// @brief Assembled by the above two functions, returns IPC communication name
/// @return "cryptohsmbstd/hsmbstd"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01998
/// @trace_id_dd=DD_CRYPTO_04692
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::String GetIpcName_Remote_Hsmbst() noexcept
{
    ara::core::String stReturn(GetIpcFile_Server_Hsmbst());
    stReturn += "/";
    stReturn += GetIpcName_Server_Hsmbst();
    return stReturn;
}
/// @brief Gets version string
/// @return Version number
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01999
/// @trace_id_dd=DD_CRYPTO_04693
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const* GetstVersion() noexcept { return "2025-05-12"; }
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IPC_NAME_H_
