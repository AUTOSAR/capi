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
/// @file       key_agreement_private_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Key agreement private key context interface
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-21  <td>1.0.0    <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Decryption/Key Exchange
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_01013
/// @unit_name=KeyAgreementPrivateCtx
/// @unit_description=Key Agreement Private Key Context Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_KEY_AGREEMENT_PRIVATE_CTX_H_
#define ARA_CRYPTO_CRYP_KEY_AGREEMENT_PRIVATE_CTX_H_

#include "ara/core/result.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/cryp/cryobj/private_key.h"
#include "ara/crypto/cryp/cryobj/public_key.h"
#include "ara/crypto/cryp/cryobj/secret_seed.h"
#include "ara/crypto/cryp/cryobj/symmetric_key.h"
#include "ara/crypto/cryp/crypto_context.h"
#include "ara/crypto/cryp/extension_service.h"
#include "ara/crypto/cryp/key_derivation_function_ctx.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Key agreement private key context interface (Diffie Hellman or similar in concept).
/// @brief Key Agreement Private key Context interface (Diffie Hellman or conceptually similar).
/// @interface KeyAgreementPrivateCtx
/// @AUTOSAR_SWS {SWS_CRYPT_21300}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02104}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02306
/// @trace_id_dd=DD_CRYPTO_05082
/// @needwork = ad
/// @endcode
class KeyAgreementPrivateCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of this interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21301}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02104}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03321
    /// @trace_id_dd=DD_CRYPTO_06643
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< KeyAgreementPrivateCtx >;
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork = no
    /// @endcode
    KeyAgreementPrivateCtx() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02307
    /// @trace_id_dd=DD_CRYPTO_05083
    /// @needwork = ad
    /// @endcode
    ~KeyAgreementPrivateCtx() override = default;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another KeyAgreementPrivateCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02308
    /// @trace_id_dd=DD_CRYPTO_05084
    /// @needwork = ad
    /// @endcode
    KeyAgreementPrivateCtx &operator=(KeyAgreementPrivateCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another KeyAgreementPrivateCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02309
    /// @trace_id_dd=DD_CRYPTO_05085
    /// @needwork = ad
    /// @endcode
    KeyAgreementPrivateCtx &operator=(KeyAgreementPrivateCtx &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02310
    /// @trace_id_dd=DD_CRYPTO_05086
    /// @needwork = ad
    /// @endcode
    KeyAgreementPrivateCtx(KeyAgreementPrivateCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02311
    /// @trace_id_dd=DD_CRYPTO_05087
    /// @needwork = ad
    /// @endcode
    KeyAgreementPrivateCtx(KeyAgreementPrivateCtx &&other) noexcept = delete;

public:
    /// @brief Generate a common symmetric key by executing the key agreement algorithm between this private key and the other party's public key.
    ///         The generated SymmetricKey object has the following properties: session, non-exportable. This method can be used directly to generate the target key without creating an intermediate SecretSeed object.
    /// @brief Produce a common symmetric key via execution of the key-agreement algorithm between this private key and
    /// a public key of another side.
    ///         Produced @c SymmetricKey object has following attributes: session, non-exportable.
    ///         This method can be used for direct production of the target key, without creation of the intermediate @c
    ///         SecretSeed object.
    /// @param otherSideKey Another instance of this class SideKey  the public key of the other side of the Key-Agreement
    /// @param kdf  the Context of a Key Derivation Function, which should be used for the target key production
    /// @param targetAlgId  identifier of the target symmetric algorithm (also defines a target key-length)
    /// @param allowedUsage  the allowed usage scope of the target key
    /// @param salt  an optional salt value (if used, it should be unique for each instance of the target key)
    /// @param ctxLabel  an optional application specific "context label" (it can identify purpose of the target
    ///            key and/or communication parties)
    /// @return a unique pointer to @c SecretSeed object, which contains the key material produced by the Key-Agreement
    /// algorithm
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the public and private keys correspond to different
    /// algorithms
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21312}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02115}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02312
    /// @trace_id_dd=DD_CRYPTO_05088
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< SymmetricKey::Uptrc > AgreeKey(PublicKey const &otherSideKey,
                                                              KeyDerivationFunctionCtx &kdf,
                                                              AlgId targetAlgId,
                                                              AllowedUsageFlags allowedUsage,
                                                              ReadOnlyMemRegion const &salt = ReadOnlyMemRegion(),
                                                              ReadOnlyMemRegion const &ctxLabel
                                                              = ReadOnlyMemRegion()) const noexcept = 0;
    /// @brief Generate a common secret seed by executing the key agreement algorithm between this private key and the other party's public key.
    /// The generated SecretSeed object has the following properties: session, non-exportable, AlgID
    /// (Key-Agreement Algorithm ID)。
    /// @brief Produce a common secret seed via execution of the key-agreement algorithm between this private key and a
    /// public key of another side.
    ///         Produced @c SecretSeed object has following attributes: session, non-exportable, AlgID (this
    ///         Key-Agreement Algorithm ID).
    /// @param otherSideKey Another instance of this class SideKey  the public key of the other side of the Key-Agreement
    /// @param allowedUsage  the allowed usage scope of the target seed
    /// @return unique pointer to @c SecretSeed object, which contains the key material produced by the Key-Agreement
    /// algorithm
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the public and private keys correspond to different
    /// algorithms
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21311}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02313
    /// @trace_id_dd=DD_CRYPTO_05089
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< SecretSeed::Uptrc > AgreeSeed(PublicKey const &otherSideKey,
                                                             SecretSeed::Usage allowedUsage
                                                             = kAllowKdfMaterialAnyUsage) const noexcept = 0;
    /// @brief Get the ExtensionService instance.
    /// @brief Get ExtensionService instance.
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21302}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02314
    /// @trace_id_dd=DD_CRYPTO_05090
    /// @needwork = ad
    /// @endcode
    virtual ExtensionService::Uptr GetExtensionService() const noexcept = 0;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21314}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02315
    /// @trace_id_dd=DD_CRYPTO_05091
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Reset() noexcept = 0;
    /// @brief Set (deploy) a key to the key agreement private algorithm context.
    /// @brief Set (deploy) a key to the key agreement private algorithm context.
    /// @param key  the source key object
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject if the provided key object is incompatible with this private key context
    /// @error: SecurityErrorDomain::kUsageViolation     if the transformation type associated with this context is
    /// prohibited by the "allowed usage" restrictions of provided key object
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21313}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02316
    /// @trace_id_dd=DD_CRYPTO_05092
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetKey(PrivateKey const &key) noexcept = 0;

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_KEY_AGREEMENT_PRIVATE_CTX_H_
