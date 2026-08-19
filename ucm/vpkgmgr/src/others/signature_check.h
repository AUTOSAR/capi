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
/// @file       signature_check.h
/// @brief
/// @details
/// @date       2023-09-07
/// @author     zhaoyunfei
/// @version    1.2.0
///
/// ================================================================

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <ara/crypto/common/entry_point.h>
#include <ara/crypto/common/mem_region.h>
#include <ara/crypto/cryp/crypto_provider.h>
#include <ara/crypto/cryp/decryptor_private_ctx.h>
#include <ara/crypto/cryp/encryptor_public_ctx.h>
#include <ara/crypto/x509/certificate.h>
#include <ara/crypto/x509/x509_provider.h>

namespace ara {
namespace ucm {
namespace vpkgmgr {

class SignatureCheck  // NOLINT
{
public:
    SignatureCheck()          = default;
    virtual ~SignatureCheck() = default;

    bool CheckSignature(ara::core::InstanceSpecifier &certPortId,
                        ara::core::String const &filePath,
                        ara::core::String const &signaturePath);

    /// @brief
    /// @param certPortId  Certificate serial number
    /// @param issuer Issuer DN
    /// @param filePath  Vehicle package manifest, step manifest, software package manifest, software cluster manifest to be verified
    /// @param signaturePath Encrypted signature
    /// @return
    ara::core::Result< void > CheckSignature(ara::core::String const &certSerialNumber,
                                             ara::core::String const &issuer,
                                             ara::core::String const &filePath,
                                             ara::core::String const &signaturePath);

private:
    bool _RsaDecode(ara::core::StringView stCryptoName,
                    ara::crypto::cryp::PublicKey const &key,
                    ara::crypto::ReadOnlyMemRegion value,
                    ara::crypto::ReadOnlyMemRegion signature);
    ara::core::Vector< ara::core::Byte > _hexToBytes(core::String const &hex) const;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
