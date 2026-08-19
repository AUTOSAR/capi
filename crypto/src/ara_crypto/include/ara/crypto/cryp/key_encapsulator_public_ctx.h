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
/// @file       key_encapsulator_public_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Asymmetric Key Encapsulation Mechanism (KEM) public key context interface. (Encryption)
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </td> <td>2021-12-21 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Decryption/Key Encapsulation
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_01015
/// @unit_name=KeyEncapsulatorPublicCtx
/// @unit_description=Asymmetric Key Encapsulation Mechanism (KEM) Public Key Context Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_KEY_ENCAPSULATOR_PUBLIC_CTX_H_
#define ARA_CRYPTO_CRYP_KEY_ENCAPSULATOR_PUBLIC_CTX_H_

#include "ara/core/result.h"
#include "ara/crypto/cryp/cryobj/public_key.h"
#include "ara/crypto/cryp/cryobj/secret_seed.h"
#include "ara/crypto/cryp/cryobj/symmetric_key.h"
#include "ara/crypto/cryp/extension_service.h"
#include "ara/crypto/cryp/key_derivation_function_ctx.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Asymmetric Key Encapsulation Mechanism (KEM) public key context interface.
/// @brief Asymmetric Key Encapsulation Mechanism (KEM) Public key Context interface.
/// @interface KeyEncapsulatorPublicCtx
/// @AUTOSAR_SWS {SWS_CRYPT_21800}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02104}
/// @uptrace={RS_CRYPTO_02209}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02248
/// @trace_id_dd=DD_CRYPTO_05024
/// @needwork = ad
/// @endcode
class KeyEncapsulatorPublicCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21801}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02209}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03194
    /// @trace_id_dd=DD_CRYPTO_06405
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< KeyEncapsulatorPublicCtx >;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork = no
    /// @endcode
    KeyEncapsulatorPublicCtx() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02249
    /// @trace_id_dd=DD_CRYPTO_05025
    /// @needwork = ad
    /// @endcode
    ~KeyEncapsulatorPublicCtx() override = default;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another KeyEncapsulatorPublicCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02250
    /// @trace_id_dd=DD_CRYPTO_05026
    /// @needwork = ad
    /// @endcode
    KeyEncapsulatorPublicCtx &operator=(KeyEncapsulatorPublicCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another KeyEncapsulatorPublicCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02251
    /// @trace_id_dd=DD_CRYPTO_05027
    /// @needwork = ad
    /// @endcode
    KeyEncapsulatorPublicCtx &operator=(KeyEncapsulatorPublicCtx &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02252
    /// @trace_id_dd=DD_CRYPTO_05028
    /// @needwork = ad
    /// @endcode
    KeyEncapsulatorPublicCtx(KeyEncapsulatorPublicCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02253
    /// @trace_id_dd=DD_CRYPTO_05029
    /// @needwork = ad
    /// @endcode
    KeyEncapsulatorPublicCtx(KeyEncapsulatorPublicCtx &&other) noexcept = delete;

public:
    /// @brief Get the fixed size of the encapsulated data block.
    /// @brief Get fixed size of the encapsulated data block.
    /// @return size of the encapsulated data block in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21818}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02254
    /// @trace_id_dd=DD_CRYPTO_05030
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetEncapsulatedSize() const noexcept = 0;
    /// @brief Get the ExtensionService instance.
    /// @brief Get ExtensionService instance.
    /// @return ExtensionService instance
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21802}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02255
    /// @trace_id_dd=DD_CRYPTO_05031
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
    /// @trace_id_sws={SWS_CRYPT_21817}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02256
    /// @trace_id_dd=DD_CRYPTO_05032
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetKekEntropy() const noexcept = 0;
    /// @brief Add content (payload) to be encapsulated according to RFC 5990 ("keying data"). Currently only SymmetricKey and SecretSeed objects are supported.
    /// @brief Add the content to be encapsulated (payload) according to RFC 5990 ("keying data").
    ///         At the moment only SymmetricKey and SecretSeed objects are supported.
    /// @param keyingData  the payload to be protected
    /// @return  has value if AddKeyingData sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21810}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02257
    /// @trace_id_dd=DD_CRYPTO_05033
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > AddKeyingData(RestrictedUseObject &keyingData) noexcept = 0;
    /// @brief Encapsulate the Key Encryption Key (KEK).
    /// @brief Encapsulate Key Encryption Key (KEK).
    /// @param kdf  a context of a key derivation function, which should be used for the target KEK production
    /// @param kekAlgId  an algorithm ID of the target KEK
    /// @param salt  an optional salt value (if used, it should be unique for each instance of the target key)
    /// @param ctxLabel  an optional application specific "context label" (it can identify purpose of the target key
    /// and/or communication parties)
    /// @return    unique smart pointer to a symmetric key object derived from a randomly generated material
    /// encapsulated to the output buffer
    ///       Only first @c GetEncapsulatedSize() bytes of the output buffer should be updated by this method.
    ///       Produced @c SymmetricKey object has following attributes: session, non-exportable, Allowed Key Usage: @c
    ///       kAllowKeyExporting. This method can be used for direct production of the target key, without creation of
    ///       the intermediate @c SecretSeed object.
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a public key value
    /// @error: SecurityErrorDomain::kInvalidArgument        if @c kekAlgId specifies incorrect algorithm
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the @c output.size() is not enough to save the
    /// encapsulation result
    /// @trace_id_sws={SWS_CRYPT_21813}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02102}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02115}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02258
    /// @trace_id_dd=DD_CRYPTO_05034
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > Encapsulate(
        KeyDerivationFunctionCtx &kdf,
        AlgId kekAlgId,
        ReadOnlyMemRegion const &salt     = ReadOnlyMemRegion(),
        ReadOnlyMemRegion const &ctxLabel = ReadOnlyMemRegion()) const noexcept = 0;

public:
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return  has value if Reset sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21816}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02259
    /// @trace_id_dd=DD_CRYPTO_05035
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Reset() noexcept = 0;
    /// @brief Set (deploy) a key to the key encapsulator public algorithm context.
    /// @brief Set (deploy) a key to the key encapsulator public algorithm context.
    /// @param key  the source key object
    /// @return  has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context is
    /// prohibited by the "allowed usage" restrictions of provided key object
    /// @trace_id_sws={SWS_CRYPT_21815}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02260
    /// @trace_id_dd=DD_CRYPTO_05036
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetKey(PublicKey const &key) noexcept = 0;

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_KEY_ENCAPSULATOR_PUBLIC_CTX_H_
