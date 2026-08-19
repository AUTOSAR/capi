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
/// @file       ipc_key_provider.cpp
/// @brief      AutoSar-Crypto key storage module
/// @details
/// @date       2022-01-13
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/DefaultKeyComponent/KeyProvider
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_05001
/// @unit_name=IpcKeyProvider
/// @unit_description=Key provider
/// @endcode
///
/// ================================================================

#include "ara/crypto/common/keys/ipc_key_provider.h"

#include <isoft/ipccpp/client.h>

#include "ara/crypto/common/entry_point.h"
#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/ipc/isoft_ipc_auto_message.h"
#include "ara/crypto/ipc/isoft_ipc_protocol.h"
#include "ara/crypto/keys/isoft_ipc_key_slot.h"
#include "ara/crypto/keys/isoft_updates_observer.h"
#include "ara/crypto/manifest/manifest_instance.h"

namespace ara {
namespace crypto {
namespace keys {
/// @brief Extend the logging system implemented using isoft_def
using ara::crypto::isoft_def::LogInfo;
//********************************/
template class IpcKeyProvider< isoft_def::PIpcKeyProvider, cryp::isoft_def::PCryptoProvider >;
//********************************/
}  // namespace keys
}  // namespace crypto
}  // namespace ara
