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
/// @file       isoft_crl.cpp
/// @brief      AutoSar-Crypto Certificate management module
/// @details
/// @date       2023-11-20
/// @author     Che Jinzhao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate component/Certificate revocation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=CRL
/// @unit_description=Certificate revocation list
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/isoft_crl.h"

namespace ara {
namespace crypto {
namespace x509 {
namespace isoft_def {
/// @brief Constructor
/// @param x509Provider Certificate provider
CRL::CRL(X509Provider &x509Provider) noexcept : X509Object{x509Provider}, x509Provider_{x509Provider} {}

/// @brief Publicly serialize itself.
/// @brief Serialize itself publicly.
/// @param formatId  the Crypto Provider specific identifier of the output format
/// @returns a buffer with the serialized object
/// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less than
/// required
/// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
/// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
/// type
/// @trace_id_sws={SWS_CRYPT_10711}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02112}
/// @threadsafety={Thread-safe}
ara::core::Result< ara::core::Vector< ara::core::Byte > > CRL::ExportPublicly(FormatId formatId) const noexcept
{
    std::ignore = formatId;
    ara::core::Vector< ara::core::Byte > const data;
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(data);
}

}  // namespace  isoft_def
}  // namespace x509
}  // namespace crypto
}  // namespace ara