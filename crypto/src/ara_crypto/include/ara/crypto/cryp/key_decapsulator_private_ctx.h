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
/// @file       key_decapsulator_private_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Asymmetric Key Encapsulation Mechanism (KEM) private key context interface. (Decryption)
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Key Encapsulation
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_01015
/// @unit_name=KeyDecapsulatorPrivateCtx
/// @unit_description=Asymmetric Key Encapsulation Mechanism (KEM) Private Key Context Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_KEY_DECAPSULATOR_PRIVATE_CTX_H_
#define ARA_CRYPTO_CRYP_KEY_DECAPSULATOR_PRIVATE_CTX_H_

#include "ara/core/result.h"
#include "ara/crypto/cryp/cryobj/private_key.h"
#include "ara/crypto/cryp/cryobj/secret_seed.h"
#include "ara/crypto/cryp/cryobj/symmetric_key.h"
#include "ara/crypto/cryp/crypto_context.h"
#include "ara/crypto/cryp/extension_service.h"
#include "ara/crypto/cryp/key_derivation_function_ctx.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Asymmetric Key Encapsulation Mechanism (KEM) private key context interface.
/// @brief Asymmetric Key Encapsulation Mechanism (KEM) Private key Context interface.
/// @interface KeyDecapsulatorPrivateCtx
/// @AUTOSAR_SWS {SWS_CRYPT_21400}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02104}
/// @uptrace={RS_CRYPTO_02209}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02354
/// @trace_id_dd=DD_CRYPTO_05134
/// @needwork = ad
/// @endcode
class KeyDecapsulatorPrivateCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21401}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02104}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03201
    /// @trace_id_dd=DD_CRYPTO_06413
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< KeyDecapsulatorPrivateCtx >;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork = no
    /// @endcode
    KeyDecapsulatorPrivateCtx() noexcept = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02355
    /// @trace_id_dd=DD_CRYPTO_05135
    /// @needwork = ad
    /// @endcode
    ~KeyDecapsulatorPrivateCtx() override = default;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another KeyDecapsulatorPrivateCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02356
    /// @trace_id_dd=DD_CRYPTO_05136
    /// @needwork = ad
    /// @endcode
    KeyDecapsulatorPrivateCtx &operator=(KeyDecapsulatorPrivateCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another KeyDecapsulatorPrivateCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02357
    /// @trace_id_dd=DD_CRYPTO_05137
    /// @needwork = ad
    /// @endcode
    KeyDecapsulatorPrivateCtx &operator=(KeyDecapsulatorPrivateCtx &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02358
    /// @trace_id_dd=DD_CRYPTO_05138
    /// @needwork = ad
    /// @endcode
    KeyDecapsulatorPrivateCtx(KeyDecapsulatorPrivateCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02359
    /// @trace_id_dd=DD_CRYPTO_05139
    /// @needwork = ad
    /// @endcode
    KeyDecapsulatorPrivateCtx(KeyDecapsulatorPrivateCtx &&other) noexcept = delete;

public:
    /// @brief Decapsulate the Key Encryption Key (KEK). The resulting SymmetricKey object has the following properties: session, non-exportable, Key
    /// Usage:kAllowKeyImporting。
    ///         This method can be used directly to generate the target key without creating an intermediate SecretSeed object.

    /// @brief Decapsulate Key Encryption Key (KEK).
    ///         Produced @c SymmetricKey object has following attributes: session, non-exportable, Key Usage: @c
    ///         kAllowKeyImporting. This method can be used for direct production of the target key, without creation of
    ///         the intermediate @c SecretSeed object.
    /// @param input  an input buffer (its size should be equal @c GetEncapsulatedSize() bytes)
    /// @param kdf  a context of a key derivation function, which should be used for the target KEK production
    /// @param kekAlgId  an algorithm ID of the target KEK
    /// @param salt  an optional salt value (if used, it should be unique for each instance of the target key)
    /// @param ctxLabel  an pptional application specific "context label" (it can identify purpose of the target key
    /// and/or communication parties)
    /// @return unique smart pointer to a symmetric key object derived from a key material decapsulated from the input
    /// block
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a private key value
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if @c kekAlgId specifies incorrect algorithm
    /// @error: SecurityErrorDomain::kInvalidInputSize       if (input.size() <> this->GetEncapsulatedSize())
    /// @trace_id_sws={SWS_CRYPT_21412}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02102}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02115}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02360
    /// @trace_id_dd=DD_CRYPTO_05140
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< SymmetricKey::Uptrc > DecapsulateKey(ReadOnlyMemRegion const &input,
                                                                    KeyDerivationFunctionCtx &kdf,
                                                                    AlgId kekAlgId,
                                                                    ReadOnlyMemRegion const &salt = ReadOnlyMemRegion(),
                                                                    ReadOnlyMemRegion const &ctxLabel
                                                                    = ReadOnlyMemRegion()) const noexcept = 0;
    /// @brief Decapsulate key material.
    /// The returned key Material object should be used to derive a symmetric key. The resulting SecretSeed object has the following properties: session, non-exportable, AlgID
    /// = this KEM AlgID.
    /// @brief Decapsulate key material.
    ///         Returned Key Material object should be used for derivation of a symmetric key.
    ///         Produced @c SecretSeed object has following attributes: session, non-exportable, AlgID = this KEM AlgID.
    /// @param input  a buffer with the encapsulated seed (its size should be equal @c GetEncapsulatedSize() bytes)
    /// @param allowedUsage  the allowed usage scope of the target seed
    /// @return unique smart pointer to @c SecretSeed object, which keeps the key material decapsulated from the input
    /// buffer
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a private key value
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the @c output.size() is not enough to save the
    /// decapsulation result
    /// @trace_id_sws={SWS_CRYPT_21411}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02361
    /// @trace_id_dd=DD_CRYPTO_05141
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< SecretSeed::Uptrc > DecapsulateSeed(ReadOnlyMemRegion const &input,
                                                                   SecretSeed::Usage allowedUsage
                                                                   = kAllowKdfMaterialAnyUsage) const noexcept = 0;
    /// @brief Get the fixed size of the encapsulated data block.
    /// @brief Get fixed size of the encapsulated data block.
    /// @return size of the encapsulated data block in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21416}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02362
    /// @trace_id_dd=DD_CRYPTO_05142
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetEncapsulatedSize() const noexcept = 0;
    /// @brief Get the ExtensionService instance.
    /// @brief Get ExtensionService instance.
    /// @return ExtensionService instance
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21402}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02363
    /// @trace_id_dd=DD_CRYPTO_05143
    /// @needwork = ad
    /// @endcode
    virtual ExtensionService::Uptr GetExtensionService() const noexcept = 0;
    /// @brief Get the key entropy (bit length) of the key (KEK) material.
    /// For RSA systems, the return value corresponds to the module length N (-1), for DH-like systems, the return value corresponds to the module length q (-1).
    /// @brief Get entropy (bit-length) of the key encryption key (KEK) material.
    ///         For RSA system the returned value corresponds to the length of module N (minus 1).
    ///         For DH-like system the returned value corresponds to the length of module q (minus 1).
    /// @return entropy of the KEK material in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21415}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02364
    /// @trace_id_dd=DD_CRYPTO_05144
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetKekEntropy() const noexcept = 0;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return  has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21414}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02365
    /// @trace_id_dd=DD_CRYPTO_05145
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Reset() noexcept = 0;
    /// @brief Set (deploy) a key to the key decapsulator private algorithm context.
    /// @brief Set (deploy) a key to the key decapsulator private algorithm context.
    /// @param key  the source key object
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// private key context
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context is
    /// prohibited by the "allowed usage" restrictions of provided key object
    /// @trace_id_sws={SWS_CRYPT_21413}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02366
    /// @trace_id_dd=DD_CRYPTO_05146
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetKey(PrivateKey const &key) noexcept = 0;

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_KEY_DECAPSULATOR_PRIVATE_CTX_H_
