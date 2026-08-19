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
/// @file       crypto.h
/// @brief      The Crypto definition for ucm.
/// @details
/// @date       2023-10-26
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/UCMLib
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00006
/// @unit_name=Crypto
/// @unit_description=Crypto definitions provided for UCM
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_CRYPTO_H_
#define ARA_UCM_PKGMGR_CRYPTO_H_

#include <tuple>

#include "ara/crypto/cryp/crypto_provider.h"
#include "ara/crypto/x509/certificate.h"
#include "ara/crypto/x509/x509_provider.h"
#include "ara/ucm/internal/extraction/alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief AlgId
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00001
/// @trace_id_dd=DD_UCM_00144
/// @needwork = no
/// @endcode
using AlgId = ara::crypto::cryp::CryptoContext::AlgId;
/// @brief PublicKey
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00001
/// @trace_id_dd=DD_UCM_00144
/// @needwork = no
/// @endcode
using PublicKey = ara::crypto::cryp::PublicKey;
/// @brief VerifierPublicCtx
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00001
/// @trace_id_dd=DD_UCM_00144
/// @needwork = no
/// @endcode
using VerifierPublicCtx = ara::crypto::cryp::VerifierPublicCtx;
/// @brief HashFunctionCtx
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00001
/// @trace_id_dd=DD_UCM_00144
/// @needwork = no
/// @endcode
using HashFunctionCtx = ara::crypto::cryp::HashFunctionCtx;

/// @brief ResultCreateHashFunctionCtx
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00001
/// @trace_id_dd=DD_UCM_00144
/// @needwork = no
/// @endcode
using ResultCreateHashFunctionCtx = AraResult< HashFunctionCtx::Uptr >;
/// @brief Result_getVerifierPublicCtx
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00001
/// @trace_id_dd=DD_UCM_00144
/// @needwork = no
/// @endcode
using Result_getVerifierPublicCtx = AraResult< std::tuple< VerifierPublicCtx::Uptr, PublicKey::Uptrc > >;

/// @brief This class contains UCM crypto functions
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00001
/// @trace_id_dd=DD_UCM_00144
/// @needwork = ad
/// @endcode
class Crypto
{
public:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00001
    /// @trace_id_dd=DD_UCM_00145
    /// @needwork = dda
    /// @endcode
    Crypto();

    /// @brief Checks validity of a signature
    /// @param certificate certificate that contains a public key
    /// @param filePath The absolute path to a file to check
    /// @param signaturePath The absolute path to a signature
    ///
    /// @return true if signature is valid, false otherwise
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00001
    /// @trace_id_dd=DD_UCM_00146
    /// @needwork = dda
    /// @endcode
    bool CheckSignature(AraVector< uint8_t > const& certificate,
                        AraString const& filePath,
                        AraString const& signaturePath) const;

    /// @brief Returns the SHA-256 hash of the file
    ///
    /// @param filePath The absolute path to a file.
    ///
    /// @return SHA-256 hash in AraString
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00001
    /// @trace_id_dd=DD_UCM_00147
    /// @needwork = dda
    /// @endcode
    AraString GetFileHash(AraString const& filePath) const;

    /// @brief Get the MD5 value of a file content (a 32-character lowercase hexadecimal string); if the file does not exist, return an empty string
    /// @param fileName File name
    /// @return md5 string
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00001
    /// @trace_id_dd=DD_UCM_00148
    /// @needwork = dda
    /// @endcode
    AraString MD5ForFile(AraString const& fileName) const;

private:
    /// @brief LoadFile
    /// @param filePath File path
    /// @return file body File content
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00001
    /// @trace_id_dd=DD_UCM_00149
    /// @needwork = dda
    /// @endcode
    static AraVector< uint8_t > LoadFile(AraString const& filePath);

    /// @brief _getVerifierPublicCtx
    /// @param certificate Certificate
    /// @return result Result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00001
    /// @trace_id_dd=DD_UCM_00150
    /// @needwork = dda
    /// @endcode
    Result_getVerifierPublicCtx _getVerifierPublicCtx(AraVector< uint8_t > const& certificate) const;

    /// @brief _sHA256HashFile
    /// @param filePath File path
    /// @return result Result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00001
    /// @trace_id_dd=DD_UCM_00151
    /// @needwork = dda
    /// @endcode
    ResultCreateHashFunctionCtx _sHA256HashFile(AraString const& filePath) const;

private:
    /// @brief pProviderX509_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00001
    /// @trace_id_dd=DD_UCM_00152
    /// @needwork = dda
    /// @endcode
    ara::crypto::x509::X509Provider::Uptr pProviderX509_;
    /// @brief pCryptoProvider_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00001
    /// @trace_id_dd=DD_UCM_00153
    /// @needwork = dda
    /// @endcode
    ara::crypto::cryp::CryptoProvider::Uptr pCryptoProvider_;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_CRYPTO_H_
