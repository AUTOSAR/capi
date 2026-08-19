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
/// @file       isoft_ctx_key_encapsulator_public.cpp
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details
/// @date       2022-04-24
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/DefaultEncryptionDecryption/KeyEncapsulation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01015
/// @unit_name=PCtxKeyEncapsulatorPublic
/// @unit_description=Public key encapsulation context base class
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/kem/isoft_ctx_key_encapsulator_public.h"

#include "ara/crypto/cryp/isoft_crypto_provider.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {

/// @brief Parameterized constructor
/// @param cryptoProvider crypto provider
/// @returns
/// @throws
PCtxKeyEncapsulatorPublic::PCtxKeyEncapsulatorPublic(PCryptoProvider &cryptoProvider) noexcept
    : KeyEncapsulatorPublicCtx{}  // NOLINT
    , cryptoProvider_{cryptoProvider}
{
}

/// @brief Get the reference to the Crypto Provider for this context.
/// @returns reference to crypto provider
// PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
inline CryptoProvider &PCtxKeyEncapsulatorPublic::MyProvider() const noexcept { return cryptoProvider_; }
// PRQA L:QAC

//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
