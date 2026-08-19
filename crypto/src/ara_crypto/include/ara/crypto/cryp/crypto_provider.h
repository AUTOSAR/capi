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
/// @file       crypto_provider.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Factory for cryptographic primitives (Crypto Provider)
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Crypto Provider
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_02001
/// @unit_name=CryptoProvider
/// @unit_description=Crypto Provider Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_CRYPTO_PROVIDER_H_
#define ARA_CRYPTO_CRYP_CRYPTO_PROVIDER_H_

#include "ara/core/map.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/crypto/common/io_interface.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/common/security_error_domain.h"
#include "ara/crypto/common/serializable.h"
#include "ara/crypto/common/volatile_trusted_container.h"
#include "ara/crypto/cryp/auth_cipher_ctx.h"
#include "ara/crypto/cryp/cryobj/crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/signature.h"
#include "ara/crypto/cryp/decryptor_private_ctx.h"
#include "ara/crypto/cryp/encryptor_public_ctx.h"
#include "ara/crypto/cryp/hash_function_ctx.h"
#include "ara/crypto/cryp/key_agreement_private_ctx.h"
#include "ara/crypto/cryp/key_decapsulator_private_ctx.h"
#include "ara/crypto/cryp/key_derivation_function_ctx.h"
#include "ara/crypto/cryp/key_encapsulator_public_ctx.h"
#include "ara/crypto/cryp/message_authn_code_ctx.h"
#include "ara/crypto/cryp/msg_recovery_public_ctx.h"
#include "ara/crypto/cryp/random_generator_ctx.h"
#include "ara/crypto/cryp/sig_encode_private_ctx.h"
#include "ara/crypto/cryp/signer_private_ctx.h"
#include "ara/crypto/cryp/stream_cipher_ctx.h"
#include "ara/crypto/cryp/symmetric_block_cipher_ctx.h"
#include "ara/crypto/cryp/symmetric_key_wrapper_ctx.h"
#include "ara/crypto/cryp/verifier_public_ctx.h"

// PRQA S 2024 QAC /// @qac: False positive
namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Crypto Provider is the "factory" interface for all supported Crypto Primitives, and the "trusted environment" for internal communication between them.
///         All crypto primitives should have an actual reference to their parent crypto provider. The crypto provider can only be destroyed after all its child crypto primitives have been destroyed.
///         Each method in this interface that creates a Crypto Primitive instance is non-const, because any such creation will increment the reference counter of the Crypto Primitive.
///         Reference counter of Primitive.
/// @brief Crypto Provider is a "factory" interface of all supported Crypto Primitives and a "trusted environmet" for
/// internal communications between them.
///     All Crypto Primitives should have an actual reference to their parent Crypto Provider.
///     A Crypto Provider can be destroyed only after destroying of all its daughterly Crypto Primitives.
///     Each method of this interface that creates a Crypto Primitive instance is non-constant, because any such
///     creation increases a references counter of the Crypto Primitive.
/// @code{.isoft}
/// @interface CryptoProvider
/// @export_level=/crypto
/// @AUTOSAR_SWS {SWS_CRYPT_20700}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02305}
/// @uptrace={RS_CRYPTO_02307}
/// @uptrace={RS_CRYPTO_02401}
/// @endcode
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02203
/// @trace_id_dd=DD_CRYPTO_04974
/// @needwork = ad
/// @endcode
class CryptoProvider
{
public:
    /// @brief Short alias for the algorithm ID type.
    /// @brief A short alias for Algorithm ID type definition.
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20703}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @uptrace={RS_CRYPTO_02006}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03192
    /// @trace_id_dd=DD_CRYPTO_06403
    /// @needwork = ad
    /// @endcode
    using AlgId = CryptoPrimitiveId::AlgId;
    /// @brief Shared smart pointer for the interface. // This indicates a conflict with the definition, which one to follow?
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20701}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02109}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03193
    /// @trace_id_dd=DD_CRYPTO_06404
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< CryptoProvider >;

public:
    /// @brief Allocate a Volatile (virtual) Trusted Container based on a directly specified capacity.
    ///         Volatile Trusted Container can be used to perform import operations. The current process acquires "owner" rights to the allocated container.
    ///         If capacity
    ///         is 0), the container's capacity will be automatically selected based on the maximum size of the supported crypto objects. Several volatile (temporary) containers can coexist without affecting each other.
    /// @brief Allocate a Volatile (virtual) Trusted Container according to directly specified capacity.
    ///         The Volatile Trusted Container can be used for execution of the import operations. Current process
    ///         obtains the "Owner" rights for allocated Container. If (capacity == 0) then the capacity of the
    ///         container will be selected automatically according to a maximal size of supported crypto objects. A few
    ///         volatile (temporary) containers can coexist at same time without any affecting each-other.
    /// @param capacity the capacity required for this volatile trusted container in bytes
    /// @return unique smart pointer to an allocated volatile trusted container
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20726}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02204
    /// @trace_id_dd=DD_CRYPTO_04975
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< VolatileTrustedContainer::Uptr > AllocVolatileContainer(std::size_t capacity
                                                                                       = 0U) noexcept = 0;
    /// @brief Allocate a Volatile (virtual) Trusted Container indirectly specified by the minimum capacity required to host any of the listed objects.
    ///         Volatile Trusted
    ///         Container can be used to perform import operations. The current process acquires "owner" rights to the allocated container. The actual container capacity is calculated as the maximum storage size of all listed objects.
    /// @brief Allocate a Volatile (virtual) Trusted Container according to indirect specification of a minimal
    ///           required capacity for hosting of any listed object.
    ///      The Volatile Trusted Container can be used for execution of the import operations.
    ///      Current process obtains the "Owner" rights for allocated Container.
    ///      Real container capacity is calculated as a maximal storage size of all listed objects.
    /// @param theObjectDef  the list of objects that can be stored to this volatile trusted container
    /// @return unique smart pointer to an allocated volatile trusted container
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if unsupported combination of object type and algorithm ID
    /// presents in the list
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20727}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02205
    /// @trace_id_dd=DD_CRYPTO_04976
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< VolatileTrustedContainer::Uptr > AllocVolatileContainer(
        std::pair< AlgId, CryptoObjectType > const &theObjectDef) noexcept = 0;
    /// @brief Convert the generic name of a cryptographic algorithm to the corresponding vendor-specific binary algorithm ID.
    /// @brief Convert a common name of crypto algorithm to a correspondent vendor specific binary algorithm ID.
    /// @param primitiveName  the unified name of the crypto primitive (see "Crypto Primitives Naming Convention" for
    /// more details)
    /// @return vendor specific binary algorithm ID or @c kAlgIdUndefined if a primitive with provided name is not
    /// supported
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20711}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02308}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02206
    /// @trace_id_dd=DD_CRYPTO_04977
    /// @needwork = ad
    /// @endcode
    virtual AlgId ConvertToAlgId(ara::core::StringView const &primitiveName) const noexcept = 0;
    /// @brief Convert a vendor-specific binary algorithm ID to the corresponding generic name of the cryptographic algorithm.
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20712}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02308}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02207
    /// @trace_id_dd=DD_CRYPTO_04978
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::String >
    /// @brief Convert a vendor specific binary algorithm ID to a correspondent common name of the crypto algorithm.
    /// @param algId  the vendor specific binary algorithm ID
    /// @return the common name of the crypto algorithm see "Crypto Primitives Naming Convention" for more details
    ConvertToAlgName(AlgId algId) const noexcept = 0;

public:
    /// @brief Create a symmetric authenticated cipher context.
    /// @brief Create a symmetric authenticated cipher context.
    /// @param algId  identifier of the target crypto algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from symmetric authenticated stream cipher
    /// @error: SecurityErrorDomain::kInvalidArgument
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20745}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02207}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02208
    /// @trace_id_dd=DD_CRYPTO_04979
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< AuthCipherCtx::Uptr > CreateAuthCipherCtx(AlgId algId) noexcept = 0;
    /// @brief Create a decryption private key context.
    /// @brief Create a decryption private key context.
    /// @param algId  identifier of the target asymmetric encryption/decryption algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different from asymmetric encryption/decryption
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20751}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02209
    /// @trace_id_dd=DD_CRYPTO_04980
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< DecryptorPrivateCtx::Uptr > CreateDecryptorPrivateCtx(AlgId algId) noexcept = 0;
    /// @brief Create an encryption public key context.
    /// @brief Create an encryption public key context.
    /// @param algId  identifier of the target asymmetric encryption/decryption algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from asymmetric encryption/decryption
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20750}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02210
    /// @trace_id_dd=DD_CRYPTO_04981
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< EncryptorPublicCtx::Uptr > CreateEncryptorPublicCtx(AlgId algId) noexcept = 0;
    /// @brief Construct a signature object from a directly provided hash digest component.
    /// @brief Construct @c Signature object from directly provided components of a hash digest.
    /// @param hashAlgId  identifier of an applied hash function crypto algorithm
    /// @param value  raw BLOB value of the hash digest
    /// @return unique smart pointer to the created @c Signature object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c hashAlgId argument has unsupported value
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c hashAlgId argument specifies crypto algorithm different
    /// from a hash function
    /// @error: SecurityErrorDomain::kInvalidInputSize   if the @c value argument has invalid size (i.e. incompatible
    /// with the @c hashAlgId argument)
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20761}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @uptrace={RS_CRYPTO_02006}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02211
    /// @trace_id_dd=DD_CRYPTO_04982
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< Signature::Uptrc > CreateHashDigest(AlgId hashAlgId,
                                                                   ReadOnlyMemRegion const &value) noexcept = 0;
    /// @brief Create a hash function context.
    /// @brief Create a hash function context.
    /// @param algId  identifier of the target crypto algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from hash function
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20747}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02205}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02212
    /// @trace_id_dd=DD_CRYPTO_04983
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< HashFunctionCtx::Uptr > CreateHashFunctionCtx(AlgId algId) noexcept = 0;
    /// @brief Create a key agreement private key context.
    /// @brief Create a key-agreement private key context.
    /// @param algId  identifier of the target key-agreement crypto algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from key-agreement
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20758}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02104}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02213
    /// @trace_id_dd=DD_CRYPTO_04984
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< KeyAgreementPrivateCtx::Uptr > CreateKeyAgreementPrivateCtx(AlgId algId) noexcept = 0;
    /// @brief Create a key-decapsulator private key context for KEM (Key Encapsulation Mechanism).
    /// @brief Create a key-decapsulator private key context of a Key Encapsulation Mechanism (KEM).
    /// @param algId  identifier of the target KEM crypto algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from asymmetric KEM
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20753}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02104}
    /// @uptrace={RS_CRYPTO_02209}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02214
    /// @trace_id_dd=DD_CRYPTO_04985
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< KeyDecapsulatorPrivateCtx::Uptr > CreateKeyDecapsulatorPrivateCtx(
        AlgId algId) noexcept = 0;
    /// @brief Create a key derivation function context.
    /// @brief Create a key derivation function context.
    /// @param algId  identifier of the target crypto algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from key derivation function
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20748}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02103}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02215
    /// @trace_id_dd=DD_CRYPTO_04986
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< KeyDerivationFunctionCtx::Uptr > CreateKeyDerivationFunctionCtx(
        AlgId algId) noexcept = 0;
    /// @brief Create a key encapsulator public key context for the Key Encapsulation Mechanism (KEM).
    /// @brief Create a key-encapsulator public key context of a Key Encapsulation Mechanism (KEM).
    /// @param algId  identifier of the target KEM crypto algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from asymmetric KEM
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20752}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02104}
    /// @uptrace={RS_CRYPTO_02209}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02216
    /// @trace_id_dd=DD_CRYPTO_04987
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< KeyEncapsulatorPublicCtx::Uptr > CreateKeyEncapsulatorPublicCtx(
        AlgId algId) noexcept = 0;
    /// @brief Create a symmetric message authentication code context.
    /// @brief Create a symmetric message authentication code context.
    /// @param algId  identifier of the target crypto algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from symmetric message authentication code
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20746}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02203}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02217
    /// @trace_id_dd=DD_CRYPTO_04988
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< MessageAuthnCodeCtx::Uptr > CreateMessageAuthCodeCtx(AlgId algId) noexcept = 0;
    /// @brief Create a message recovery public key context.
    /// @brief Create a message recovery public key context.
    /// @param algId  identifier of the target asymmetric crypto algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from asymmetric signature encoding with message recovery
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20755}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @uptrace={RS_CRYPTO_02204}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02218
    /// @trace_id_dd=DD_CRYPTO_04989
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< MsgRecoveryPublicCtx::Uptr > CreateMsgRecoveryPublicCtx(AlgId algId) noexcept = 0;
    /// @brief Create a random number generator (RNG) context.
    /// @brief Create a Random Number Generator (RNG) context.
    /// @param algId  identifier of target RNG algorithm. If no algId is given, the default RNG is returned
    /// @param initialize  indicates whether the returned context shall be initialized (i.e., seeded) by the stack
    /// @return unique smart pointer to the created RNG context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value or if <tt>(algId
    /// == kAlgIdDefault)</tt> and
    ///                                                 the CryptoProvider does not provide any RandomGeneratorCtx
    /// @error: SecurityErrorDomain::kBusyResource       if <tt>(initialize == true)</tt> but the context currently
    /// cannot be seeded (e.g., due to a lack of entropy)
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20741}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02219
    /// @trace_id_dd=DD_CRYPTO_04990
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< RandomGeneratorCtx::Uptr > CreateRandomGeneratorCtx(AlgId algId     = kAlgIdDefault,
                                                                                   bool initialize = true) noexcept = 0;
    /// @brief Create a signature encoding private key context.
    /// @brief Create a signature encoding private key context.
    /// @param algId  identifier of the target asymmetric crypto algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from asymmetric signature encoding with message recovery
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20754}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @uptrace={RS_CRYPTO_02204}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02220
    /// @trace_id_dd=DD_CRYPTO_04991
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< SigEncodePrivateCtx::Uptr > CreateSigEncodePrivateCtx(AlgId algId) noexcept = 0;
    /// @brief Construct a signature object from a directly provided digital signature/MAC or authenticated encryption (AE/AEAD) component.
    ///         All integers in the digital signature BLOB value are represented in big-endian byte order (i.e., MSF most significant byte first).
    /// @brief Construct @c Signature object from directly provided components of a digital signature/MAC or
    /// authenticated encryption (AE/AEAD).
    ///         All integers inside a digital signature BLOB value are always presented in Big Endian bytes order (i.e.
    ///         MSF - Most Significant byte First).
    /// @param signAlgId  identifier of an applied signature/MAC/AE/AEAD crypto algorithm
    /// @param value  raw BLOB value of the signature/MAC
    /// @param key  symmetric or asymmetric key (according to @c signAlgId) applied for the sign or MAC/AE/AEAD
    /// operation
    /// @param hashAlgId  identifier of a hash function algorithm applied together with the signature algorithm
    /// @return unique smart pointer to the created @c Signature object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if @c signAlgId or @c hashAlgId arguments have unsupported
    /// values
    /// @error: SecurityErrorDomain::kInvalidArgument        if @c signAlgId or @c hashAlgId arguments specify crypto
    /// algorithms different from the signature/MAC/AE/AEAD and message digest respectively
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if @c signAlgId and @c hashAlgId arguments specify
    /// incompatible algorithms (if @c signAlgId includes hash function specification)
    ///                                                     or if a crypto primitive associated with the @c key argument
    ///                                                     is incompatible with provided @c signAlgId or @c hashAlgId
    ///                                                     arguments
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the @c value argument has invalid size (i.e.
    /// incompatible with the @c signAlgId argument)
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20760}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @uptrace={RS_CRYPTO_02006}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02221
    /// @trace_id_dd=DD_CRYPTO_04992
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< Signature::Uptrc > CreateSignature(AlgId signAlgId,
                                                                  ReadOnlyMemRegion const &value,
                                                                  RestrictedUseObject const &key,
                                                                  AlgId hashAlgId = kAlgIdNone) noexcept = 0;
    /// @brief Create a signature private key context.
    /// @brief Create a signature private key context.
    /// @param algId  identifier of the target signature crypto algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from private key signature
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20756}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02222
    /// @trace_id_dd=DD_CRYPTO_04993
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< SignerPrivateCtx::Uptr > CreateSignerPrivateCtx(AlgId algId) noexcept = 0;
    /// @brief Create a symmetric stream cipher context.
    /// @brief Create a symmetric stream cipher context.
    /// @param algId  identifier of the target crypto algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from symmetric stream cipher
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20744}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02223
    /// @trace_id_dd=DD_CRYPTO_04994
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< StreamCipherCtx::Uptr > CreateStreamCipherCtx(AlgId algId) noexcept = 0;
    /// @brief Create a symmetric block cipher context.
    /// @brief Create a symmetric block cipher context.
    /// @param algId  identifier of the target crypto algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20742}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02224
    /// @trace_id_dd=DD_CRYPTO_04995
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< SymmetricBlockCipherCtx::Uptr > CreateSymmetricBlockCipherCtx(AlgId algId) noexcept = 0;
    /// @brief Create a symmetric key-wrap algorithm context.
    /// @brief Create a symmetric key-wrap algorithm context.
    /// @param algId  identifier of the target crypto algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from symmetric key-wrapping
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20743}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02104}
    /// @uptrace={RS_CRYPTO_02208}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02225
    /// @trace_id_dd=DD_CRYPTO_04996
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< SymmetricKeyWrapperCtx::Uptr > CreateSymmetricKeyWrapperCtx(AlgId algId) noexcept = 0;
    /// @brief Create a signature verification public key context.
    /// @brief Create a signature verification public key context.
    /// @param algId  identifier of the target signature crypto algorithm
    /// @return unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from public key signature verification
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20757}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02226
    /// @trace_id_dd=DD_CRYPTO_04997
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< VerifierPublicCtx::Uptr > CreateVerifierPublicCtx(AlgId algId) noexcept = 0;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02227
    /// @trace_id_dd=DD_CRYPTO_04998
    /// @needwork = ad
    /// @endcode
    CryptoProvider() = default;
    /// @brief Default virtual destructor
    /// @brief Destructor.
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20710}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02107}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02228
    /// @trace_id_dd=DD_CRYPTO_04999
    /// @needwork = ad
    /// @endcode
    virtual ~CryptoProvider() noexcept = default;

public:
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another CryptoProvider to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_30216}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05000
    /// @needwork = dda
    /// @endcode
    CryptoProvider &operator=(CryptoProvider const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another CryptoProvider to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_30217}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05001
    /// @needwork = dda
    /// @endcode
    CryptoProvider &operator=(CryptoProvider &&other) = delete;

public:
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05002
    /// @needwork = dda
    /// @endcode
    CryptoProvider(CryptoProvider const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05003
    /// @needwork = dda
    /// @endcode
    CryptoProvider(CryptoProvider &&other) = delete;

public:
    /// @brief Publicly export an object from an IOInterface (i.e., without the need to create an intermediate crypto object).
    /// @brief Export publicly an object from a IOInterface (i.e. without an intermediate creation of a crypto object).
    /// @param container  the IOInterface that contains an object for export
    /// @param formatId  the CryptoProvider specific identifier of the output format
    /// @return actual capacity required for the serialized data
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kEmptyContainer         if the container is empty
    /// @error: SecurityErrorDomain::kUnexpectedValue        if the container contains a secret crypto object
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if <tt>(serialized.empty() == false)</tt>, but its capacity
    /// is not enough for storing result
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20731}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02105}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02229
    /// @trace_id_dd=DD_CRYPTO_05004
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicObject(
        IOInterface const &container, Serializable::FormatId formatId = Serializable::kFormatDefault) noexcept = 0;
    /// @brief Export a crypto object securely.
    ///         If (serialization .empty() == true), this method only returns the required size, but the content of transportContext remains unchanged!
    ///         Only an exportable and completed object (i.e., an object with a UUID) can be exported!
    /// @brief Export a crypto object in a secure manner.
    ///         if (serialized.empty() == true) then the method returns required size only, but content of the @c
    ///         transportContext stays unchanged! Only an exportable and completed object (i.e. that have a UUID) can be
    ///         exported!
    /// @param object  the crypto object for export
    /// @param transportContext  the symmetric key wrap context initialized by a transport key (allowed usage: @c kAllowKeyExporting)
    /// @return the wrapped crypto object data
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject  if the object cannot be exported due to IsExportable()
    /// returning flase
    /// @error: SecurityErrorDomain::kIncompleteArgState  if the @c transportContext is not initialized
    /// @error: SecurityErrorDomain::kIncompatibleObject  if a key loaded to the @c transportContext doesn't have
    /// required attributes (note: it is an optional error condition for this method)
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20728}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02105}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02230
    /// @trace_id_dd=DD_CRYPTO_05005
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportSecuredObject(
        CryptoObject const &object, SymmetricKeyWrapperCtx &transportContext) noexcept = 0;
    /// @brief Securely export an object directly from an IOInterface (i.e., without the need to create an intermediate crypto object).
    /// If (serialized == nullptr), this method only returns the required size, but the content of transportContext remains unchanged.
    /// This method can be used to re-export an object just imported, but on another transport key.
    /// @brief Export securely an object directly from an IOInterface (i.e. without an intermediate creation of a crypto object).
    ///     if (serialized == nullptr) then the method returns required size only, but content of the @c
    ///     transportContext stays unchanged. This method can be used for re-exporting of just imported object but
    ///     on another transport key.
    /// @param container  the IOInterface that refers an object for export
    /// @param transportContext  the symmetric key wrap context initialized by a transport key (allowed usage: @c kAllowKeyExporting)
    /// @return actual capacity required for the serialized data
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kEmptyContainer         if the @c container is empty
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if size of the @c serialized buffer is not enough for
    /// saving the output data
    /// @error: SecurityErrorDomain::kIncompleteArgState     if the @c transportContext is not initialized
    /// @error: SecurityErrorDomain::kIncompatibleObject     if a key loaded to the @c transportContext doesn't have
    /// required attributes (note: it is an optional error condition for this method)
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20729}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02105}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02231
    /// @trace_id_dd=DD_CRYPTO_05006
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportSecuredObject(
        IOInterface const &container, SymmetricKeyWrapperCtx &transportContext) noexcept = 0;
    /// @brief Allocate a new private key context of the corresponding type and randomly generate the key value.
    ///         The private and public keys should share a common COUID. Any serializable (i.e., saveable/non-session or exportable) key must generate its own COUID!
    /// @brief Allocate a new private key context of correspondent type and generates the key value randomly.
    ///         A common COUID should be shared for both private and public keys. Any serializable (i.e.
    ///         savable/non-session or exportable) key must generate own COUID!
    /// @param algId  the identifier of target public-private key crypto algorithm
    /// @param allowedUsage  the flags that define a list of allowed transformations' types in which the target key
    /// can be used (see constants in scope of @c RestrictedUseObject)
    /// @param isSession  the "session" (or "temporary") attribute for the target key (if @c true)
    /// @return smart unique pointer to the created private key object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if @c algId has an unsupported value
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if @c allowedUsage argument is incompatible with target
    /// algorithm @c algId (note: it is an optional error condition for this method)
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20722}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02003}
    /// @uptrace={RS_CRYPTO_02101}
    /// @uptrace={RS_CRYPTO_02102}
    /// @uptrace={RS_CRYPTO_02107}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02111}
    /// @uptrace={RS_CRYPTO_02115}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02232
    /// @trace_id_dd=DD_CRYPTO_05007
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< PrivateKey::Uptrc > GeneratePrivateKey(AlgId algId,
                                                                      AllowedUsageFlags allowedUsage,
                                                                      bool isSession    = false,
                                                                      bool isExportable = false) noexcept = 0;
    /// @brief Generate a random Secret Seed object for the requested algorithm.
    /// @brief Generate a random Secret Seed object of requested algorithm.
    /// @param algId  the identifier of target crypto algorithm
    /// @param allowedUsage  the lags that define a list of allowed transformations' types in which the target seed
    /// can be used (see constants in scope of @c RestrictedUseObject)
    /// @param isSession  the "session" (or "temporary") attribute of the target seed (if @c true)
    /// @param isExportable  the exportability attribute of the target seed (if @c true)
    /// @return unique smart pointer to generated @c SecretSeed object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if @c algId has an unsupported value
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if @c allowedUsage argument is incompatible with target
    /// algorithm @c algId (note: it is an optional error condition for this method)
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20723}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02233
    /// @trace_id_dd=DD_CRYPTO_05008
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< SecretSeed::Uptrc > GenerateSeed(AlgId algId,
                                                                SecretSeed::Usage allowedUsage,
                                                                bool isSession    = true,
                                                                bool isExportable = false) noexcept = 0;
    /// @brief Allocate a new symmetric key object and fill it with a newly randomly generated value.
    ///         Any serializable (i.e., saveable/non-session or exportable) key must generate its own COUID! By default, the crypto provider should use the best internal instance of all supported RNGs (ideally TRNG).
    /// @brief Allocate a new symmetric key object and fill it by a new randomly generated value.
    ///         Any serializable (i.e. savable/non-session or exportable) key must generate own COUID!
    ///         By default Crypto Provider should use an internal instance of a best from all supported RNG (ideally
    ///         TRNG).
    /// @param algId  the identifier of target symmetric crypto algorithm
    /// @param allowedUsage  the flags that define a list of allowed transformations' types in which the target key
    /// can be used (see constants in scope of @c RestrictedUseObject)
    /// @param isSession  the "session" (or "temporary") attribute of the target key (if @c true)
    /// @param isExportable  the exportability attribute of the target key (if @c true)
    /// @returns smart unique pointer to the created symmetric key object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if @c algId has an unsupported value
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if @c allowedUsage argument is incompatible with target
    /// algorithm @c algId (note: it is an optional error condition for this method)
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20721}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02003}
    /// @uptrace={RS_CRYPTO_02101}
    /// @uptrace={RS_CRYPTO_02102}
    /// @uptrace={RS_CRYPTO_02107}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02111}
    /// @uptrace={RS_CRYPTO_02115}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02234
    /// @trace_id_dd=DD_CRYPTO_05009
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< SymmetricKey::Uptrc > GenerateSymmetricKey(AlgId algId,
                                                                          AllowedUsageFlags allowedUsage,
                                                                          bool isSession    = true,
                                                                          bool isExportable = false) noexcept = 0;
    /// @brief Returns the minimum key slot capacity required to save the object payload.
    ///         The return value does not consider the object's meta-information attributes, but their size is fixed and common for all crypto objects independent of their actual type.
    ///         During the allocation of TrustedContainer, the crypto provider (and key storage provider) automatically reserves space for the object's meta-information according to its implementation details.
    /// @brief Return minimally required capacity of a key slot for saving of the object’s payload.
    ///         Returned value does not take into account the object’s meta-information properties, but their size is
    ///         fixed and common for all crypto objects independently from their actual type. During an allocation of a
    ///         TrustedContainer, Crypto Providers (and Key Storage Providers) reserve space for an object’s
    ///         meta-information automatically, according to their implementation details.
    /// @param cryptoObjectType  the type of the target object
    /// @param algId  a CryptoProvider algorithm ID of the target object
    /// @return minimal size required for storing of the object in a @c TrustedContainer (persistent or volatile)
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if any argument has an unsupported value
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if the arguments are incompatible
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20725}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02235
    /// @trace_id_dd=DD_CRYPTO_05010
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< std::size_t > GetPayloadStorageSize(CryptoObjectType cryptoObjectType,
                                                                   AlgId algId) const noexcept = 0;
    /// @brief Returns the buffer size required to serialize the object in a specific format.
    /// @brief Return required buffer size for serialization of an object in specific format.
    /// @param cryptoObjectType  the type of the target object
    /// @param algId  the Crypto Provider algorithm ID of the target object
    /// @param formatId  the Crypto Provider specific identifier of the output format
    /// @return size required for storing of the object serialized in the specified format
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if any argument has an unsupported value
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if any pair of the arguments are incompatible
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20724}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02236
    /// @trace_id_dd=DD_CRYPTO_05011
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< std::size_t > GetSerializedSize(CryptoObjectType cryptoObjectType,
                                                               AlgId algId,
                                                               Serializable::FormatId formatId
                                                               = Serializable::kFormatDefault) const noexcept = 0;
    /// @brief Import a publicly serialized object into the storage location pointed to by the IOInterface for subsequent processing (without allocating a crypto object).
    ///         If (expectedObject != CryptoObjectType::kUnknown) and the actual object type differs from the expected, this method fails.
    ///         If the serialized object contains incorrect data, this method fails.
    /// @brief Import publicly serialized object to a storage location pointed to by an IOInterface for following
    /// processing (without allocation of a crypto object).
    ///         If (expectedObject != CryptoObjectType::kUnknown) and an actual object type differs from the expected
    ///         one then this method fails. If the @c serialized contains incorrect data then this method fails.
    /// @param container  the IOInterface for storing of the imported object
    /// @param serialized  the memory region that contains a securely serialized object that should be imported to
    /// the IOInterface
    /// @param expectedObject  the expected object type (default value @c CryptoObjectType::kUnknown means without check)
    /// @return has value if ImportPublicObject sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnexpectedValue        if the @c serialized contains incorrect data
    /// @error: SecurityErrorDomain::kBadCryptoObjectType    if <tt>(expectedObject != CryptoObjectType::kUnknown)</tt>,
    /// but the actual object type differs from the expected one
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the @c container is not enough to save the
    /// de-serialized object
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writable.
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20732}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02105}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02237
    /// @trace_id_dd=DD_CRYPTO_05012
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > ImportPublicObject(IOInterface &container,
                                                         ReadOnlyMemRegion const &serialized,
                                                         CryptoObjectType expectedObject
                                                         = CryptoObjectType::kUndefined) noexcept = 0;
    /// @brief Import a securely serialized object into persistent or volatile storage represented by the IOInterface for subsequent processing.
    /// @brief Import securely serialized object to the persistent or volatile storage represented by an IOInterface
    /// for following processing.
    /// @param container  the IOInterface for storing of the imported object
    /// @param serialized  the memory region that contains a securely serialized object that should be imported to
    /// the IOInterface
    /// @param transportContext  the symmetric key wrap context initialized by a transport key (allowed usage: @c kAllowKeyImporting)
    /// @param isExportable  the exportability attribute of the target object
    /// @param expectedObject  the expected object type (default value @c CryptoObjectType::kUnknown means without check)
    /// @return has value if ImportPublicObject sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnexpectedValue        if the @c serialized contains incorrect data
    /// @error: SecurityErrorDomain::kBadCryptoObjectType    if <tt>(expectedObject != CryptoObjectType::kUnknown)</tt>, but the actual object type differs from the expected one
    /// @error: SecurityErrorDomain::kIncompleteArgState     if the @c transportContext is not initialized
    /// @error: SecurityErrorDomain::kIncompatibleObject     if a key loaded to the @c transportContext doesn't have required attributes (note: it is an optional error condition for this method)
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the @c container is not enough to save the deserialized object
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20730}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02105}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02238
    /// @trace_id_dd=DD_CRYPTO_05013
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > ImportSecuredObject(IOInterface &container,
                                                          ReadOnlyMemRegion const &serialized,
                                                          SymmetricKeyWrapperCtx &transportContext,
                                                          bool isExportable = false,
                                                          CryptoObjectType expectedObject
                                                          = CryptoObjectType::kUndefined) noexcept = 0;
    /// @brief Load any crypto object from the provided IOInterface.
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kEmptyContainer     if the container is empty
    /// @error: SecurityErrorDomain::kResourceFault      if the container content is damaged
    /// @error: SecurityErrorDomain::kModifiedResource   if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kIncompatibleObject if the underlying resource belongs to another, incompatible CryptoProvider
    /// @note This method is one of the "binding" methods between a CryptoProvider and the %Key Storage Provider.
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20733}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02002}
    /// @uptrace={RS_CRYPTO_02005}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02239
    /// @trace_id_dd=DD_CRYPTO_05014
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< CryptoObject::Uptrc >
    /// @brief Load any crypto object from the IOInterface provided.
    /// @param container  the IOInterface that contains the crypto object for loading
    /// @return unique smart pointer to the created object
    LoadObject(IOInterface const &container) noexcept = 0;
    /// @brief Load a private key from the provided IOInterface.
    /// @brief Load a private key from the IOInterface provided.
    /// @param container  the IOInterface that contains the crypto object for loading
    /// @return nunique smart pointer to the PrivateKey
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kEmptyContainer     if the container is empty
    /// @error: SecurityErrorDomain::kResourceFault      if the container content is damaged
    /// @error: SecurityErrorDomain::kModifiedResource   if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kIncompatibleObject if the underlying resource belongs to another, incompatible
    /// CryptoProvider
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20764}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02002}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02240
    /// @trace_id_dd=DD_CRYPTO_05015
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< PrivateKey::Uptrc > LoadPrivateKey(IOInterface const &container) noexcept = 0;
    /// @brief Load a public key from the provided IOInterface.
    /// @brief Load a public key from the IOInterface provided.
    /// @param container  the IOInterface that contains the crypto object for loading
    /// @return unique smart pointer to the PublicKey
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kEmptyContainer     if the container is empty
    /// @error: SecurityErrorDomain::kResourceFault      if the container content is damaged
    /// @error: SecurityErrorDomain::kModifiedResource   if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kIncompatibleObject if the underlying resource belongs to another, incompatible
    /// CryptoProvider
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20763}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02002}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02241
    /// @trace_id_dd=DD_CRYPTO_05016
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< PublicKey::Uptrc > LoadPublicKey(IOInterface const &container) noexcept = 0;
    /// @brief Load a secret seed from the provided IOInterface.
    /// @brief Load secret seed from the IOInterface provided.
    /// @param container  the IOInterface that contains the crypto object for loading
    /// @return unique smart pointer to the SecretSeed
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kEmptyContainer     if the container is empty
    /// @error: SecurityErrorDomain::kResourceFault      if the container content is damaged
    /// @error: SecurityErrorDomain::kModifiedResource   if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kIncompatibleObject if the underlying resource belongs to another, incompatible
    /// CryptoProvider
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20765}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02002}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02242
    /// @trace_id_dd=DD_CRYPTO_05017
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< SecretSeed::Uptrc > LoadSecretSeed(IOInterface const &container) noexcept = 0;
    /// @brief Load a symmetric key from the provided IOInterface.
    /// @brief Load a symmetric key from the IOInterface provided.
    /// @param container  the IOInterface that contains the crypto object for loading
    /// @return unique smart pointer to the SymmetricKey
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kEmptyContainer     if the container is empty
    /// @error: SecurityErrorDomain::kResourceFault      if the container content is damaged
    /// @error: SecurityErrorDomain::kModifiedResource   if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kIncompatibleObject if the underlying resource belongs to another, incompatible
    /// CryptoProvider
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20762}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02002}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02243
    /// @trace_id_dd=DD_CRYPTO_05018
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< SymmetricKey::Uptrc > LoadSymmetricKey(IOInterface const &container) noexcept = 0;

public:
    /// @brief Get the public key information in the certificate via the certificate serial number (non-standard interface)
    /// @param certSerialNumber Certificate serial number
    /// @return  has value if FindSubjectPubKey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02244
    /// @trace_id_dd=DD_CRYPTO_05019
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< PublicKey::Uptrc > FindSubjectPubKey(
        ara::core::String const &certSerialNumber) noexcept = 0;
    /// @brief Save the public key serial number and the iointerface corresponding to the certificate public key information (non-standard interface)
    /// @param certSerialNumber Certificate serial number
    /// @param pubData Public key data
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02245
    /// @trace_id_dd=DD_CRYPTO_05020
    /// @needwork = ad
    /// @endcode
    virtual void InsertSubjectPubKey(ara::core::String const &certSerialNumber,
                                     ara::core::String const &pubData) noexcept = 0;

private:
    /// @brief Public key MAP list
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05021
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, ara::core::String > mapSubjectPubKey_;

protected:
    /// @brief Get the public key MAP list
    /// @param certSerialNumber Certificate serial number
    /// @return Public key MAP list
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02246
    /// @trace_id_dd=DD_CRYPTO_05022
    /// @needwork = ad
    /// @endcode
    ara::core::String _FindSubjectPubKey(ara::core::String const &certSerialNumber) const noexcept
    {
        ara::core::Map< ara::core::String, ara::core::String >::const_iterator const itFind{
            mapSubjectPubKey_.find(certSerialNumber)};
        if (itFind == mapSubjectPubKey_.end()) {
            return ara::core::String{};
        }
        return itFind->second;
    }
    /// @brief Insert a public key into the public key MAP list
    /// @param certSerialNumber Certificate serial number
    /// @param pubData Public key data
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02247
    /// @trace_id_dd=DD_CRYPTO_05023
    /// @needwork = ad
    /// @endcode
    void _InsertSubjectPubKey(ara::core::String const &certSerialNumber, ara::core::String const &pubData) noexcept
    {
        std::ignore = mapSubjectPubKey_.insert(std::make_pair(certSerialNumber, pubData));
    }
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

// PRQA L:QAC

#endif  // ARA_CRYPTO_CRYP_CRYPTO_PROVIDER_H_