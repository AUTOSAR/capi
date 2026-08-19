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
/// @file       isoft_key_symmetric.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption decryption/Symmetric key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeySymmetric_Base
/// @unit_description=Symmetric key base class
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/cryobj/isoft_key_symmetric.h"

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/common/security_error_domain.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/

/// @brief Default constructor
PKeySymmetric_Base::PKeySymmetric_Base() noexcept : SymmetricKey{}  // NOLINT
{
    keyId_.mCOType              = CryptoObjectType::kSymmetricKey;
    keyId_.mCouid.mGeneratorUid = GetUuidByTime();
    keyId_.mCouid.mVersionStamp = 0U;
}
/// @brief: Save the key
/// @param container IO interface
/// @returns true if save to key false otherwise
bool PKeySymmetric_Base::_SaveToKeySlot(IOInterface &container) const noexcept
{
    keys::isoft_def::PIoInterface *const pIoInterface{dynamic_cast< keys::isoft_def::PIoInterface * >(&container)};
    if (nullptr == pIoInterface) {
        return false;
    }
    ReadOnlyMemRegion const memKeyData{GetKeySymmetric(), GetPayloadSize()};
    return pIoInterface->SaveKeyToSlot< PKeySymmetric_Base >(this, memKeyData);
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
