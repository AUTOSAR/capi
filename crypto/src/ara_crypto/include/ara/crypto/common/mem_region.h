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
/// @file       mem_region.h
/// @brief      AutoSar-Crypto Encryption/Decryption common module
/// @details    Memory region alias
/// @date       2021-12-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr> <td>2021-12-29 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Common_api
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_MEM_REGION_H_
#define ARA_CRYPTO_MEM_REGION_H_

#include <cstdint>
#include <functional>
#include <memory>

#include "ara/core/span.h"
#include "ara/core/utility.h"
#include "ara/core/vector.h"

namespace ara {
namespace crypto {
//********************************/
/// @brief Read-only memory region (for [in]parameters)
/// @brief Read-Only Memory Region (intended for [in] arguments)
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10033}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02004}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_03185
/// @trace_id_dd=DD_CRYPTO_06392
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using ReadOnlyMemRegion = ara::core::Span< std::uint8_t const >;
/// @brief Read-write memory region (for [in/out]parameters)
/// @brief Read-Write Memory Region (intended for [in/out] arguments)
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10031}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02004}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_03186
/// @trace_id_dd=DD_CRYPTO_06393
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using ReadWriteMemRegion = ara::core::Span< std::uint8_t >;
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_MEM_REGION_H_
