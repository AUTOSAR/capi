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
/// @file       cert_sign_request.cpp
/// @brief      AutoSar-Crypto Certificate management module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate component/Certificate request
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03005
/// @unit_name=CertSignRequest
/// @unit_description=Certificate signing request base class
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/cert_sign_request.h"

namespace ara {
namespace crypto {
namespace x509 {
//********************************/
/// @brief Certificate signing request (CSR) object interface. This interface is specifically used for complete parsing of the request content.
/// @param x509Provider Certificate provider
/// @returns
/// @throws
CertSignRequest::CertSignRequest(X509Provider& x509Provider) noexcept : BasicCertInfo{x509Provider} {}
//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara
