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
/// @file       elementary_types.h
/// @brief      AutoSar-Crypto Key Storage Module
/// @details    Definition of common types.
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
/// @module_path=/CRYPTO/Default Key Components/Key Slot
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_05002
/// @unit_name=PIpcKeySlot
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_ELEMENTARY_TYPES_H_
#define ARA_CRYPTO_KEYS_ELEMENTARY_TYPES_H_

#include "ara/core/vector.h"
#include "ara/crypto/keys/keyslot.h"

namespace ara {
namespace crypto {
namespace keys {
//********************************/
/// @brief Definition of transaction identifier type. The zero value should be reserved for special cases.
/// @brief Definition of a transaction identifier type. The zero value should be reserved for especial cases.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_30010}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02004}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_03243
/// @trace_id_dd=DD_CRYPTO_06455
/// @trace_id_sr=SR_CRYPTO_05002
/// @needwork = ad
/// @endcode
using TransactionId = std::uint64_t;
/// @brief Definition of "transaction scope" type. "Transaction scope" defines the list of key slots to be updated in a transaction.
/// @brief Definition of a "transaction scope" type. The "transaction scope" defines a list of key slots that are target
/// for update in a transaction.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_30011}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02004}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_03244
/// @trace_id_dd=DD_CRYPTO_06456
/// @trace_id_sr=SR_CRYPTO_05002
/// @needwork = ad
/// @endcode
using TransactionScope = ara::core::Vector< KeySlot* >;
//********************************/
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_ELEMENTARY_TYPES_H_
