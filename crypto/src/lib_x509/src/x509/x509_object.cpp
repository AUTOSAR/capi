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
/// @file       x509_object.cpp
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details
/// @date       2022-05-18
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Component/X.509 Certificate
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=X509Object
/// @unit_description=Certificate Object Common Interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/x509_object.h"

#include "ara/crypto/x509/x509_provider.h"

namespace ara {
namespace crypto {
namespace x509 {
//********************************/
/// @brief Certificate object class
/// @param x509Provider Certificate provider
X509Object::X509Object(X509Provider& x509Provider) noexcept
    : Serializable{}  // NOLINT
    , x509Provider_{x509Provider}
{
}
//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara
