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
/// @file       isoft_crypto_provider.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    Factory of cryptographic primitives (crypto provider)
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Crypto Provider
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_02001
/// @unit_name=PCryptoProvider
/// @unit_description=Crypto provider
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CRYPTO_PROVIDER_H_
#define ARA_CRYPTO_CRYP_PUHUA_CRYPTO_PROVIDER_H_

#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/crypto_provider.h"
#include "ara/crypto/cryp/isoft_convert_alg_id.h"
#include "ara/crypto/keys/isoft_ipc_key_slot.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Crypto Provider is the "factory" interface for all supported Crypto Primitives, as well as the "trusted environment" for internal communication between them.
///         All cryptographic primitives should have an actual reference to their parent crypto provider. A crypto provider can only be destroyed after all its child cryptographic primitives have been destroyed.
///         Each method in this interface that creates a Crypto Primitive instance is non-constant, because any such creation increases the reference counter of the Crypto
///         Primitive.
/// @brief Crypto Provider is a "factory" interface of all supported Crypto Primitives and a "trusted environmet" for
///         internal communications between them.
///         All Crypto Primitives should have an actual reference to their parent Crypto Provider.
///         A Crypto Provider can be destroyed only after destroying of all its daughterly Crypto Primitives.
///         Each method of this interface that creates a Crypto Primitive instance is non-constant, because any such
///         creation increases a references counter of the Crypto Primitive.
/// @code{.isoft}
/// @interface PCryptoProvider
/// @export_level=/Crypto
/// @trace_id_sws= {SWS_CRYPT_20700}
/// @uptrace={RS_CRYPTO_02305}
/// @uptrace={RS_CRYPTO_02307}
/// @uptrace={RS_CRYPTO_02401}
/// @tracestatus={draft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00071
/// @trace_id_dd=DD_CRYPTO_00668
/// @needwork = ad
/// @endcode
class PCryptoProvider : public ara::crypto::cryp::CryptoProvider
{
public:
    /// @brief Short alias for algorithm ID type definition.
    /// @brief Interface shared smart pointer.  // This conflicts with the definition; which one to follow?
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20701}
    /// @uptrace={RS_CRYPTO_02109}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00071
    /// @trace_id_dd=DD_CRYPTO_06244
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PCryptoProvider >;

private:
    /// @brief Convert encryption algorithm string to AlgID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00669
    /// @needwork = dda
    /// @endcode
    PConvertAlgID convertAlgID_;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00670
    /// @needwork = dda
    /// @endcode
    PCryptoProvider() noexcept = default;
    /// @brief Default virtual destructor
    /// @brief Destructor.
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20710}
    /// @uptrace={RS_CRYPTO_02107}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00072
    /// @trace_id_dd=DD_CRYPTO_00671
    /// @needwork = ad
    /// @endcode
    ~PCryptoProvider() noexcept override = default;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another CryptoProvider to this instance.
    /// @param other another object instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30216}
    /// @uptrace={RS_CRYPTO_02004}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00073
    /// @trace_id_dd=DD_CRYPTO_00672
    /// @needwork = ad
    /// @endcode
    PCryptoProvider &operator=(PCryptoProvider const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another CryptoProvider to this instance.
    /// @param other another object instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30217}
    /// @uptrace={RS_CRYPTO_02004}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00074
    /// @trace_id_dd=DD_CRYPTO_00673
    /// @needwork = ad
    /// @endcode
    PCryptoProvider &operator=(PCryptoProvider &&other) = delete;
    /// @brief Default copy constructor
    /// @brief Copy-assign another CryptoProvider to this instance.
    /// @param other another object instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00674
    /// @needwork = dda
    /// @endcode
    PCryptoProvider(PCryptoProvider const &other) = delete;
    /// @brief Default move constructor
    /// @brief Move-assign another CryptoProvider to this instance.
    /// @param other another object instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00675
    /// @needwork = dda
    /// @endcode
    PCryptoProvider(PCryptoProvider &&other) = delete;

public:
    /// @brief Convert the common name of an encryption algorithm to the corresponding vendor-specific binary algorithm ID.
    /// @brief Convert a common name of crypto algorithm to a correspondent vendor specific binary algorithm ID.
    /// @param primitiveName  the unified name of the crypto primitive (see "Crypto Primitives Naming Convention"
    ///                       for more details)
    /// @returns  vendor specific binary algorithm ID or @c kAlgIdUndefined if a primitive with provided name is not
    ///           supported
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20711}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02308}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00075
    /// @trace_id_dd=DD_CRYPTO_00676
    /// @needwork = ad
    /// @endcode
    AlgId ConvertToAlgId(ara::core::StringView const &primitiveName) const noexcept override;
    /// @brief Convert a vendor-specific binary algorithm ID to the corresponding common name of the encryption algorithm.
    /// @brief Convert a vendor specific binary algorithm ID to a correspondent common name of the crypto algorithm.
    /// @param algId  the vendor specific binary algorithm ID
    /// @returns the common name of the crypto algorithm (see "Crypto Primitives Naming Convention" for more details)
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20712}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02308}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00076
    /// @trace_id_dd=DD_CRYPTO_00677
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::String > ConvertToAlgName(AlgId algId) const noexcept override;

public:
    /// @brief Create a decryption private key context.
    /// @brief Create a decryption private key context.
    /// @param algId  identifier of the target asymmetric encryption/decryption algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from asymmetric encryption/decryption
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20751}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02003
    /// @trace_id_ad=AD_CRYPTO_00077
    /// @trace_id_dd=DD_CRYPTO_00678
    /// @needwork = ad
    /// @endcode
    ara::core::Result< DecryptorPrivateCtx::Uptr > CreateDecryptorPrivateCtx(AlgId algId) noexcept override;
    /// @brief Create an encryption public key context.
    /// @brief Create an encryption public key context.
    /// @param algId  identifier of the target asymmetric encryption/decryption algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from asymmetric encryption/decryption
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20750}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02003
    /// @trace_id_ad=AD_CRYPTO_00078
    /// @trace_id_dd=DD_CRYPTO_00679
    /// @needwork = ad
    /// @endcode
    ara::core::Result< EncryptorPublicCtx::Uptr > CreateEncryptorPublicCtx(AlgId algId) noexcept override;

public:
    /// @brief Create a signature private key context.
    /// @brief Create a signature private key context.
    /// @param algId  identifier of the target signature crypto algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from
    ///        private key signature
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20756}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02004
    /// @trace_id_ad=AD_CRYPTO_00079
    /// @trace_id_dd=DD_CRYPTO_00680
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SignerPrivateCtx::Uptr > CreateSignerPrivateCtx(AlgId algId) noexcept override;
    /// @brief Create a signature verification public key context.
    /// @brief Create a signature verification public key context.
    /// @param algId  identifier of the target signature crypto algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from
    ///        public key signature verification
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20757}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02004
    /// @trace_id_ad=AD_CRYPTO_00080
    /// @trace_id_dd=DD_CRYPTO_00681
    /// @needwork = ad
    /// @endcode
    ara::core::Result< VerifierPublicCtx::Uptr > CreateVerifierPublicCtx(AlgId algId) noexcept override;

public:
    /// @brief Create a signature encoding private key context.
    /// @brief Create a signature encoding private key context.
    /// @param algId  identifier of the target asymmetric crypto algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from
    ///        asymmetric signature encoding with message recovery
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20754}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @uptrace={RS_CRYPTO_02204}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02004
    /// @trace_id_ad=AD_CRYPTO_00081
    /// @trace_id_dd=DD_CRYPTO_00682
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SigEncodePrivateCtx::Uptr > CreateSigEncodePrivateCtx(AlgId algId) noexcept override;
    /// @brief Create a message recovery public key context.
    /// @brief Create a message recovery public key context.
    /// @param algId  identifier of the target asymmetric crypto algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from
    ///        asymmetric signature encoding with message recovery
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20755}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @uptrace={RS_CRYPTO_02204}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02004
    /// @trace_id_ad=AD_CRYPTO_00082
    /// @trace_id_dd=DD_CRYPTO_00683
    /// @needwork = ad
    /// @endcode
    ara::core::Result< MsgRecoveryPublicCtx::Uptr > CreateMsgRecoveryPublicCtx(AlgId algId) noexcept override;

public:
    /// @brief Create a symmetric stream cipher context.
    /// @brief Create a symmetric stream cipher context.
    /// @param algId  identifier of the target crypto algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from
    ///        symmetric stream cipher
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20744}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02002
    /// @trace_id_ad=AD_CRYPTO_00083
    /// @trace_id_dd=DD_CRYPTO_00684
    /// @needwork = ad
    /// @endcode
    ara::core::Result< StreamCipherCtx::Uptr > CreateStreamCipherCtx(AlgId algId) noexcept override;
    /// @brief Create a symmetric block cipher context.
    /// @brief Create a symmetric block cipher context.
    /// @param algId  identifier of the target crypto algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20742}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02002
    /// @trace_id_ad=AD_CRYPTO_00084
    /// @trace_id_dd=DD_CRYPTO_00685
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SymmetricBlockCipherCtx::Uptr > CreateSymmetricBlockCipherCtx(AlgId algId) noexcept override;

public:
    /// @brief Create a key encapsulator public key context for Key Encapsulation Mechanism (KEM).
    /// @brief Create a key-encapsulator public key context of a Key Encapsulation Mechanism (KEM).
    /// @param algId  identifier of the target KEM crypto algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from
    ///        asymmetric KEM
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20752}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02104}
    /// @uptrace={RS_CRYPTO_02209}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02007
    /// @trace_id_ad=AD_CRYPTO_00085
    /// @trace_id_dd=DD_CRYPTO_00686
    /// @needwork = ad
    /// @endcode
    ara::core::Result< KeyEncapsulatorPublicCtx::Uptr > CreateKeyEncapsulatorPublicCtx(AlgId algId) noexcept override;
    /// @brief Create a symmetric key-wrap algorithm context.
    /// @brief Create a symmetric key-wrap algorithm context.
    /// @param algId  identifier of the target crypto algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from
    ///        symmetric key-wrapping
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20743}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02104}
    /// @uptrace={RS_CRYPTO_02208}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02006
    /// @trace_id_ad=AD_CRYPTO_00086
    /// @trace_id_dd=DD_CRYPTO_00687
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SymmetricKeyWrapperCtx::Uptr > CreateSymmetricKeyWrapperCtx(AlgId algId) noexcept override;
    /// @brief Create a key agreement private key context.
    /// @brief Create a key-agreement private key context.
    /// @param algId  identifier of the target key-agreement crypto algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from
    ///        key-agreement
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20758}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02104}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02012
    /// @trace_id_ad=AD_CRYPTO_00087
    /// @trace_id_dd=DD_CRYPTO_00688
    /// @needwork = ad
    /// @endcode
    ara::core::Result< KeyAgreementPrivateCtx::Uptr > CreateKeyAgreementPrivateCtx(AlgId algId) noexcept override;
    /// @brief Create a key decapsulator private key context for KEM (Key Encapsulation Mechanism).
    /// @brief Create a key-decapsulator private key context of a Key Encapsulation Mechanism (KEM).
    /// @param algId  identifier of the target KEM crypto algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from
    ///        asymmetric KEM
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20753}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02104}
    /// @uptrace={RS_CRYPTO_02209}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02007
    /// @trace_id_ad=AD_CRYPTO_00088
    /// @trace_id_dd=DD_CRYPTO_00689
    /// @needwork = ad
    /// @endcode
    ara::core::Result< KeyDecapsulatorPrivateCtx::Uptr > CreateKeyDecapsulatorPrivateCtx(AlgId algId) noexcept override;
    /// @brief Create a key derivation function context.
    /// @brief Create a key derivation function context.
    /// @param algId  identifier of the target crypto algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from
    ///        key derivation function
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20748}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02103}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02009
    /// @trace_id_ad=AD_CRYPTO_00089
    /// @trace_id_dd=DD_CRYPTO_00690
    /// @needwork = ad
    /// @endcode
    ara::core::Result< KeyDerivationFunctionCtx::Uptr > CreateKeyDerivationFunctionCtx(AlgId algId) noexcept override;

public:
    /// @brief Create a symmetric authenticated cipher context.
    /// @brief Create a symmetric authenticated cipher context.
    /// @param algId  identifier of the target crypto algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from
    ///        symmetric authenticated stream cipher
    /// @error: SecurityErrorDomain::kInvalidArgument
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20745}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02207}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02010
    /// @trace_id_ad=AD_CRYPTO_00090
    /// @trace_id_dd=DD_CRYPTO_00691
    /// @needwork = ad
    /// @endcode
    ara::core::Result< AuthCipherCtx::Uptr > CreateAuthCipherCtx(AlgId algId) noexcept override;
    /// @brief Create a symmetric message authentication code context.
    /// @brief Create a symmetric message authentication code context.
    /// @param algId  identifier of the target crypto algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from
    ///        symmetric message authentication code
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20746}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02203}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02005
    /// @trace_id_ad=AD_CRYPTO_00091
    /// @trace_id_dd=DD_CRYPTO_00692
    /// @needwork = ad
    /// @endcode
    ara::core::Result< MessageAuthnCodeCtx::Uptr > CreateMessageAuthCodeCtx(AlgId algId) noexcept override;
    /// @brief Construct a signature object from directly provided digital signature/MAC or authenticated encryption (AE/AEAD) components.
    /// All integers in the digital signature BLOB value are always represented in big-endian byte order (i.e., MSF most significant byte first).
    /// @brief Construct @c Signature object from directly provided components of a digital signature/MAC or
    /// authenticated encryption (AE/AEAD).
    ///         All integers inside a digital signature BLOB value are always presented in Big Endian bytes order (i.e.
    ///         MSF - Most Significant byte First).
    /// @param signAlgId  identifier of an applied signature/MAC/AE/AEAD crypto algorithm
    /// @param value  raw BLOB value of the signature/MAC
    /// @param key  symmetric or asymmetric key (according to @c signAlgId) applied for the sign or MAC/AE/AEAD
    ///        operation
    /// @param hashAlgId  identifier of a hash function algorithm applied together with the signature algorithm
    /// @returns unique smart pointer to the created @c Signature object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if @c signAlgId or @c hashAlgId arguments have unsupported
    ///        values
    /// @error: SecurityErrorDomain::kInvalidArgument        if @c signAlgId or @c hashAlgId arguments specify crypto
    ///        algorithms different from the signature/MAC/AE/AEAD and message digest respectively
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if @c signAlgId and @c hashAlgId arguments specify
    ///        incompatible algorithms (if @c signAlgId includes hash function specification) or if a crypto primitive
    ///        associated with the @c key argument is incompatible with provided @c signAlgId or @c hashAlgId arguments
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the @c value argument has invalid size (i.e.
    /// incompatible
    ///        with the @c signAlgId argument)
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20760}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @uptrace={RS_CRYPTO_02006}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00092
    /// @trace_id_dd=DD_CRYPTO_00693
    /// @needwork = ad
    /// @endcode
    ara::core::Result< Signature::Uptrc > CreateSignature(AlgId signAlgId,
                                                          ReadOnlyMemRegion const &value,
                                                          RestrictedUseObject const &key,
                                                          AlgId hashAlgId = kAlgIdNone) noexcept override;

public:
    /// @brief Construct a signature object from directly provided hash digest components.
    /// @brief Construct @c Signature object from directly provided components of a hash digest.
    /// @param hashAlgId  identifier of an applied hash function crypto algorithm
    /// @param value  raw BLOB value of the hash digest
    /// @returns unique smart pointer to the created @c Signature object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c hashAlgId argument has unsupported value
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c hashAlgId argument specifies crypto algorithm different
    ///        from a hash function
    /// @error: SecurityErrorDomain::kInvalidInputSize   if the @c value argument has invalid size (i.e. incompatible
    ///        with the @c hashAlgId argument)
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20761}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @uptrace={RS_CRYPTO_02006}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00093
    /// @trace_id_dd=DD_CRYPTO_00694
    /// @needwork = ad
    /// @endcode
    ara::core::Result< Signature::Uptrc > CreateHashDigest(AlgId hashAlgId,
                                                           ReadOnlyMemRegion const &value) noexcept override;
    /// @brief Create a hash function context.
    /// @brief Create a hash function context.
    /// @param algId  identifier of the target crypto algorithm
    /// @returns unique smart pointer to the created context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if @c algId argument specifies a crypto algorithm different
    /// from
    ///        hash function
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20747}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02205}
    /// @uptrace={RS_AP_00144}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02001
    /// @trace_id_ad=AD_CRYPTO_00094
    /// @trace_id_dd=DD_CRYPTO_00695
    /// @needwork = ad
    /// @endcode
    ara::core::Result< HashFunctionCtx::Uptr > CreateHashFunctionCtx(AlgId algId) noexcept override;
    /// @brief Create a random number generator (RNG) context.
    /// @brief Create a Random Number Generator (RNG) context.
    /// @param algId  identifier of target RNG algorithm. If no algId is given, the default RNG is returned
    /// @param initialize  indicates whether the returned context shall be initialized (i.e., seeded) by the stack
    /// @returns unique smart pointer to the created RNG context
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if @c algId argument has an unsupported value
    ///        or if <tt>(algId == kAlgIdDefault)</tt> and the CryptoProvider does not provide any RandomGeneratorCtx
    /// @error: SecurityErrorDomain::kBusyResource       if <tt>(initialize == true)</tt> but the context currently
    ///        cannot be seeded (e.g., due to a lack of entropy)
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20741}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02008
    /// @trace_id_ad=AD_CRYPTO_00095
    /// @trace_id_dd=DD_CRYPTO_00696
    /// @needwork = ad
    /// @endcode
    ara::core::Result< RandomGeneratorCtx::Uptr > CreateRandomGeneratorCtx(AlgId algId     = kAlgIdDefault,
                                                                           bool initialize = true) noexcept override;

public:
    /// @brief Allocate a volatile (virtual) trusted container based on a directly specified capacity.
    /// @brief Allocate a Volatile (virtual) Trusted Container according to directly specified capacity.
    ///         The Volatile Trusted Container can be used for execution of the import operations. Current process
    ///         obtains the "Owner" rights for allocated Container. If (capacity == 0) then the capacity of the
    ///         container will be selected automatically according to a maximal size of supported crypto objects. A few
    ///         volatile (temporary) containers can coexist at same time without any affecting each-other.
    /// @param capacity  the capacity required for this volatile trusted container (in bytes)
    /// @returns unique smart pointer to an allocated volatile trusted container
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20726}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00096
    /// @trace_id_dd=DD_CRYPTO_00697
    /// @needwork = ad
    /// @endcode
    ara::core::Result< VolatileTrustedContainer::Uptr > AllocVolatileContainer(std::size_t capacity
                                                                               = 0U) noexcept override;
    /// @brief Allocate a volatile (virtual) trusted container indirectly specified by the minimum capacity required to host any of the listed objects.
    ///         Volatile Trusted
    ///         The Container can be used to perform import operations. The current process acquires "owner" permission for the allocated container. The actual container capacity is calculated as the maximum storage size of all listed objects.
    /// @brief Allocate a Volatile (virtual) Trusted Container according to indirect specification of a minimal
    ///        required capacity for hosting of any listed object.
    ///        The Volatile Trusted Container can be used for execution of the import operations.
    ///        Current process obtains the "Owner" rights for allocated Container.
    ///        Real container capacity is calculated as a maximal storage size of all listed objects.
    /// @param theObjectDef  the list of objects that can be stored to this volatile trusted container
    /// @returns unique smart pointer to an allocated volatile trusted container
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if unsupported combination of object type and algorithm ID
    ///        presents in the list
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20727}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00097
    /// @trace_id_dd=DD_CRYPTO_00698
    /// @needwork = ad
    /// @endcode
    ara::core::Result< VolatileTrustedContainer::Uptr > AllocVolatileContainer(
        std::pair< AlgId, CryptoObjectType > const &theObjectDef) noexcept override;

public:
    /// @brief Securely export a cryptographic object.
    ///         If (serialization.empty() == true), this method only returns the required size, but the content of transportContext remains unchanged!
    ///         Only an exportable and completed object (i.e., an object with a UUID) can be exported!
    /// @brief Export a crypto object in a secure manner.
    ///         if (serialized.empty() == true) then the method returns required size only, but content of the @c
    ///         transportContext stays unchanged! Only an exportable and completed object (i.e. that have a UUID) can be
    ///         exported!
    /// @param object  the crypto object for export
    /// @param transportContext  the symmetric key wrap context initialized by a transport key
    /// @returns  the wrapped crypto object data
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject  if the object cannot be exported due to IsExportable()
    ///        returning flase
    /// @error: SecurityErrorDomain::kIncompleteArgState  if the @c transportContext is not initialized
    /// @error: SecurityErrorDomain::kIncompatibleObject  if a key loaded to the @c transportContext doesn't have
    ///        required attributes (note: it is an optional error condition for this method)
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20728}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02105}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00098
    /// @trace_id_dd=DD_CRYPTO_00699
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportSecuredObject(
        CryptoObject const &object, SymmetricKeyWrapperCtx &transportContext) noexcept override;
    /// @brief Securely export an object directly from IOInterface (i.e., without intermediate creation of a crypto object).
    /// @brief Export securely an object directly from an IOInterface (i.e. without an intermediate creation of a
    ///           crypto object). if (serialized == nullptr) then the method returns required size only, but content of
    ///           the @c transportContext stays unchanged. This method can be used for re-exporting of just imported
    ///           object but on another transport key.
    /// @param container  the IOInterface that refers an object for export
    /// @param transportContext  the symmetric key wrap context initialized by a transport key
    /// @returns actual capacity required for the serialized data
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kEmptyContainer         if the @c container is empty
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if size of the @c serialized buffer is not enough for
    /// saving
    ///        the output data
    /// @error: SecurityErrorDomain::kIncompleteArgState     if the @c transportContext is not initialized
    /// @error: SecurityErrorDomain::kIncompatibleObject     if a key loaded to the @c transportContext doesn't have
    ///        required attributes (note: it is an optional error condition for this method)
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after the
    ///        IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20729}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02105}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00099
    /// @trace_id_dd=DD_CRYPTO_00700
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportSecuredObject(
        IOInterface const &container, SymmetricKeyWrapperCtx &transportContext) noexcept override;
    /// @brief Publicly export an object from IOInterface (i.e., without intermediate creation of a crypto object).
    /// @brief Export publicly an object from a IOInterface (i.e. without an intermediate creation of a crypto object).
    /// @param container  the IOInterface that contains an object for export
    /// @param formatId  the CryptoProvider specific identifier of the output format
    /// @returns actual capacity required for the serialized data
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kEmptyContainer         if the container is empty
    /// @error: SecurityErrorDomain::kUnexpectedValue        if the container contains a secret crypto object
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if <tt>(serialized.empty() == false)</tt>, but its capacity
    ///        is not enough for storing result
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after the
    ///        IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20731}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02105}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02015
    /// @trace_id_ad=AD_CRYPTO_00100
    /// @trace_id_dd=DD_CRYPTO_00701
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicObject(
        IOInterface const &container, Serializable::FormatId formatId = Serializable::kFormatDefault) noexcept override;

public:
    /// @brief : Import a securely serialized object into persistent or volatile storage represented by IOInterface for subsequent processing.
    /// @brief Import securely serialized object to the persistent or volatile storage represented by an IOInterface
    ///        for following processing.
    /// @param container  the IOInterface for storing of the imported object
    /// @param serialized  the memory region that contains a securely serialized object that should be imported to
    ///        the IOInterface
    /// @param transportContext  the symmetric key wrap context initialized by a transport key
    /// @param isExportable  the exportability attribute of the target object
    /// @param expectedObject  the expected object type
    /// @returns has value if ImportSecuredObject false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnexpectedValue        if the @c serialized contains incorrect data
    /// @error: SecurityErrorDomain::kBadCryptoObjectType    if <tt>(expectedObject != CryptoObjectType::kUnknown)</tt>,
    /// but the actual object type differs from the expected one
    /// @error: SecurityErrorDomain::kIncompleteArgState     if the @c transportContext is not initialized
    /// @error: SecurityErrorDomain::kIncompatibleObject     if a key loaded to the @c transportContext doesn't have
    /// required attributes (note: it is an optional error condition for this method)
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the @c container is not enough to save the
    /// deserialized object
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20730}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02105}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02014
    /// @trace_id_ad=AD_CRYPTO_00101
    /// @trace_id_dd=DD_CRYPTO_00702
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > ImportSecuredObject(IOInterface &container,
                                                  ReadOnlyMemRegion const &serialized,
                                                  SymmetricKeyWrapperCtx &transportContext,
                                                  bool isExportable = false,
                                                  CryptoObjectType expectedObject
                                                  = CryptoObjectType::kUndefined) noexcept override;
    /// @brief Import a publicly serialized object into a storage location pointed to by IOInterface for subsequent processing (without allocating a crypto object).
    ///         If (expectedObject != CryptoObjectType::kUnknown) and the actual object type differs from the expected one, this method fails.
    ///         If the serialized object contains incorrect data, this method fails.
    /// @brief Import publicly serialized object to a storage location pointed to by an IOInterface for following
    ///        processing (without allocation of a crypto object).
    ///        If (expectedObject != CryptoObjectType::kUnknown) and an actual object type differs from the expected
    ///        one then this method fails. If the @c serialized contains incorrect data then this method fails.
    /// @param container  the IOInterface for storing of the imported object
    /// @param serialized  the memory region that contains a securely serialized object that should be imported to
    ///        the IOInterface
    /// @param expectedObject  the expected object type
    /// @returns has vlaue if ImportPublicObject sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnexpectedValue        if the @c serialized contains incorrect data
    /// @error: SecurityErrorDomain::kBadCryptoObjectType    if <tt>(expectedObject != CryptoObjectType::kUnknown)</tt>,
    ///        but the actual object type differs from the expected one
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the @c container is not enough to save the
    ///        de-serialized object
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after the
    ///        IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writable.
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20732}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02105}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02016
    /// @trace_id_ad=AD_CRYPTO_00102
    /// @trace_id_dd=DD_CRYPTO_00703
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > ImportPublicObject(IOInterface &container,
                                                 ReadOnlyMemRegion const &serialized,
                                                 CryptoObjectType expectedObject
                                                 = CryptoObjectType::kUndefined) noexcept override;

public:
    /// @brief Generate a random Secret Seed object for the requested algorithm.
    /// @brief Generate a random Secret Seed object of requested algorithm.
    /// @param algId  the identifier of target crypto algorithm
    /// @param allowedUsage  the lags that define a list of allowed transformations' types in which the target seed
    ///        can be used (see constants in scope of @c RestrictedUseObject)
    /// @param isSession  the "session" (or "temporary") attribute of the target seed (if @c true)
    /// @param isExportable  the exportability attribute of the target seed (if @c true)
    /// @returns unique smart pointer to generated @c SecretSeed object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if @c algId has an unsupported value
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if @c allowedUsage argument is incompatible with target
    ///        algorithm @c algId (note: it is an optional error condition for this method)
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20723}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00103
    /// @trace_id_dd=DD_CRYPTO_00704
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SecretSeed::Uptrc > GenerateSeed(AlgId algId,
                                                        SecretSeed::Usage allowedUsage,
                                                        bool isSession    = true,
                                                        bool isExportable = false) noexcept override;
    /// @brief Allocate a new private key context of the corresponding type and randomly generate the key value.
    ///         The private key and public key should share a common COUID. Any serializable (i.e., storable/non-session or exportable) key must generate its own COUID!
    /// @brief Allocate a new private key context of correspondent type and generates the key value randomly.
    ///         A common COUID should be shared for both private and public keys. Any serializable (i.e.
    ///         savable/non-session or exportable) key must generate own COUID!
    /// @param algId  the identifier of target public-private key crypto algorithm
    /// @param allowedUsage  the flags that define a list of allowed transformations' types in which the target key
    ///        can be used (see constants in scope of @c RestrictedUseObject)
    /// @param isSession  the "session" (or "temporary") attribute for the target key (if @c true)
    /// @param isExportable  the exportability attribute of the target key (if @c true)
    /// @returns smart unique pointer to the created private key object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if @c algId has an unsupported value
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if @c allowedUsage argument is incompatible with target
    ///        algorithm @c algId (note: it is an optional error condition for this method)
    /// @export_level=/Crypto
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
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02013
    /// @trace_id_ad=AD_CRYPTO_00104
    /// @trace_id_dd=DD_CRYPTO_00705
    /// @needwork = ad
    /// @endcode
    ara::core::Result< PrivateKey::Uptrc > GeneratePrivateKey(AlgId algId,
                                                              AllowedUsageFlags allowedUsage,
                                                              bool isSession    = false,
                                                              bool isExportable = false) noexcept override;
    /// @brief Allocate a new symmetric key object and fill it with a new randomly generated value.
    ///         Any serializable (i.e., storable/non-session or exportable) key must generate its own COUID! By default, the crypto provider should use the best internal instance of all supported RNGs (ideally TRNG).
    /// @brief Allocate a new symmetric key object and fill it by a new randomly generated value.
    ///         Any serializable (i.e. savable/non-session or exportable) key must generate own COUID!
    ///         By default Crypto Provider should use an internal instance of a best from all supported RNG (ideally
    ///         TRNG).
    /// @param algId  the identifier of target symmetric crypto algorithm
    /// @param allowedUsage  the flags that define a list of allowed transformations' types in which the target key
    ///        can be used (see constants in scope of @c RestrictedUseObject)
    /// @param isSession  the "session" (or "temporary") attribute of the target key (if @c true)
    /// @param isExportable  the exportability attribute of the target key (if @c true)
    /// @returns smart unique pointer to the created symmetric key object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if @c algId has an unsupported value
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if @c allowedUsage argument is incompatible with target
    ///        algorithm @c algId (note: it is an optional error condition for this method)
    /// @export_level=/Crypto
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
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_02012
    /// @trace_id_ad=AD_CRYPTO_00105
    /// @trace_id_dd=DD_CRYPTO_00706
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SymmetricKey::Uptrc > GenerateSymmetricKey(AlgId algId,
                                                                  AllowedUsageFlags allowedUsage,
                                                                  bool isSession    = true,
                                                                  bool isExportable = false) noexcept override;

public:
    /// @brief Load any cryptographic object from the provided IOInterface.
    /// @brief Load any crypto object from the IOInterface provided.
    /// @param container  the IOInterface that contains the crypto object for loading
    /// @returns unique smart pointer to the created object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kEmptyContainer     if the container is empty
    /// @error: SecurityErrorDomain::kResourceFault      if the container content is damaged
    /// @error: SecurityErrorDomain::kModifiedResource   if the underlying resource has been modified after the
    ///        IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kIncompatibleObject if the underlying resource belongs to another, incompatible
    ///        CryptoProvider
    /// @note This method is one of the "binding" methods between a CryptoProvider and the %Key Storage Provider.
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20733}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02002}
    /// @uptrace={RS_CRYPTO_02005}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00106
    /// @trace_id_dd=DD_CRYPTO_00707
    /// @needwork = ad
    /// @endcode
    ara::core::Result< CryptoObject::Uptrc > LoadObject(IOInterface const &container) noexcept override;
    /// @brief Load a private key from the provided IOInterface.
    /// @brief Load a private key from the IOInterface provided.
    /// @param container  the IOInterface that contains the crypto object for loading
    /// @returns unique smart pointer to the PrivateKey
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kEmptyContainer     if the container is empty
    /// @error: SecurityErrorDomain::kResourceFault      if the container content is damaged
    /// @error: SecurityErrorDomain::kModifiedResource   if the underlying resource has been modified after the
    ///        IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kIncompatibleObject if the underlying resource belongs to another, incompatible
    ///        CryptoProvider
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20764}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02002}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00107
    /// @trace_id_dd=DD_CRYPTO_00708
    /// @needwork = ad
    /// @endcode
    ara::core::Result< PrivateKey::Uptrc > LoadPrivateKey(IOInterface const &container) noexcept override;
    /// @brief Load a public key from the provided IOInterface.
    /// @brief Load a public key from the IOInterface provided.
    /// @param container  the IOInterface that contains the crypto object for loading
    /// @returns unique smart pointer to the PublicKey
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kEmptyContainer     if the container is empty
    /// @error: SecurityErrorDomain::kResourceFault      if the container content is damaged
    /// @error: SecurityErrorDomain::kModifiedResource   if the underlying resource has been modified after the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kIncompatibleObject if the underlying resource belongs to another, incompatible
    ///        CryptoProvider
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20763}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02002}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00108
    /// @trace_id_dd=DD_CRYPTO_00709
    /// @needwork = ad
    /// @endcode
    ara::core::Result< PublicKey::Uptrc > LoadPublicKey(IOInterface const &container) noexcept override;
    /// @brief Load a secret seed from the provided IOInterface.
    /// @brief Load secret seed from the IOInterface provided.
    /// @param container  the IOInterface that contains the crypto object for loading
    /// @returns unique smart pointer to the SecretSeed
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kEmptyContainer     if the container is empty
    /// @error: SecurityErrorDomain::kResourceFault      if the container content is damaged
    /// @error: SecurityErrorDomain::kModifiedResource   if the underlying resource has been modified after the
    ///        IOInterface has been opened, i.e., the IOInterface has been invalidated.
    ///        If the underlying resource is modified after the IOInterface is opened, i.e., the IOInterface is invalid.
    /// @error: SecurityErrorDomain::kIncompatibleObject if the underlying resource belongs to another, incompatible
    ///        CryptoProvider  // If the underlying resource belongs to another incompatible crypto provider
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20765}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02002}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00109
    /// @trace_id_dd=DD_CRYPTO_00710
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SecretSeed::Uptrc > LoadSecretSeed(IOInterface const &container) noexcept override;
    /// @brief Load a symmetric key from the provided IOInterface.
    /// @brief Load a symmetric key from the IOInterface provided.
    /// @param container  the IOInterface that contains the crypto object for loading
    /// @returns unique smart pointer to the SymmetricKey
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kEmptyContainer     if the container is empty
    /// @error: SecurityErrorDomain::kResourceFault      if the container content is damaged
    /// @error: SecurityErrorDomain::kModifiedResource   if the underlying resource has been modified after the
    ///        IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kIncompatibleObject if the underlying resource belongs to another, incompatible
    ///        CryptoProvider
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20762}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02002}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00110
    /// @trace_id_dd=DD_CRYPTO_00711
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SymmetricKey::Uptrc > LoadSymmetricKey(IOInterface const &container) noexcept override;

public:
    /// @brief Return the minimum capacity of the key slot required to store the object payload.
    ///        The return value does not consider the object's meta-information attributes, but they have a fixed size and are common to all crypto objects independent of their actual type.
    ///        During the allocation of a TrustedContainer, the crypto provider (and key storage provider) automatically reserves space for the object's meta-information according to its implementation details.
    /// @brief Return minimally required capacity of a key slot for saving of the object’s payload.
    ///         Returned value does not take into account the object’s meta-information properties, but their size is
    ///         fixed and common for all crypto objects independently from their actual type. During an allocation of a
    ///         TrustedContainer, Crypto Providers (and Key Storage Providers) reserve space for an object’s
    ///         meta-information automatically, according to their implementation details.
    /// @param cryptoObjectType  the type of the target object
    /// @param algId  a CryptoProvider algorithm ID of the target object
    /// @returns minimal size required for storing of the object in a @c TrustedContainer (persistent or volatile)
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if any argument has an unsupported value
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if the arguments are incompatible
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20725}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00111
    /// @trace_id_dd=DD_CRYPTO_00712
    /// @needwork = ad
    /// @endcode
    ara::core::Result< std::size_t > GetPayloadStorageSize(CryptoObjectType cryptoObjectType,
                                                           AlgId algId) const noexcept override;
    /// @brief Return the buffer size required to serialize the object in a specific format.
    /// @brief Return required buffer size for serialization of an object in specific format.
    /// @param cryptoObjectType  the type of the target object
    /// @param algId  the Crypto Provider algorithm ID of the target object
    /// @param formatId  the Crypto Provider specific identifier of the output format
    /// @returns size required for storing of the object serialized in the specified format
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if any argument has an unsupported value
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if any pair of the arguments are incompatible
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20724}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02005}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00112
    /// @trace_id_dd=DD_CRYPTO_00713
    /// @needwork = ad
    /// @endcode
    ara::core::Result< std::size_t > GetSerializedSize(CryptoObjectType cryptoObjectType,
                                                       AlgId algId,
                                                       Serializable::FormatId formatId
                                                       = Serializable::kFormatDefault) const noexcept override;

public:
    /// @brief Allocate a new symmetric key object
    /// @name GenerateSymmetricKeyEx
    /// @param algId encryption algorithm cryptographic primitive ID
    /// @param memKeyData key data in memory
    /// @param allowedUsage usage scope
    /// @param isSession whether it is a temporary session
    /// @param isExportable whether export is allowed
    /// @return SymmetricKey instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00714
    /// @needwork = dda
    /// @endcode
    static ara::core::Result< SymmetricKey::Uptrc > GenerateSymmetricKeyEx(AlgId const algId,
                                                                           ReadOnlyMemRegion const &memKeyData,
                                                                           AllowedUsageFlags const allowedUsage,
                                                                           bool const isSession    = true,
                                                                           bool const isExportable = false) noexcept;
    /// @brief Allocate a new secret seed
    /// @name GenerateSeedEx
    /// @param algId encryption algorithm cryptographic primitive ID
    /// @param memKeyData key data in memory
    /// @param allowedUsage usage scope
    /// @param isSession whether it is a temporary session
    /// @param isExportable whether export is allowed
    /// @return SecretSeed instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00715
    /// @needwork = dda
    /// @endcode
    ara::core::Result< SecretSeed::Uptrc > GenerateSeedEx(AlgId const algId,
                                                          ReadOnlyMemRegion const &memKeyData,
                                                          AllowedUsageFlags const allowedUsage,
                                                          bool const isSession    = true,
                                                          bool const isExportable = false) const noexcept;
    /// @brief Get the public key information from the certificate by certificate serial number (non-standard interface)
    /// @param certSerialNumber certificate serial number
    /// @return public key instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00716
    /// @needwork = dda
    /// @endcode
    ara::core::Result< PublicKey::Uptrc > FindSubjectPubKey(
        ara::core::String const &certSerialNumber) noexcept override;
    /// @brief Save the public key serial number and certificate public key information (non-standard interface)
    /// @param certSerialNumber certificate serial number
    /// @param pubData public key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00717
    /// @needwork = dda
    /// @endcode
    void InsertSubjectPubKey(ara::core::String const &certSerialNumber,
                             ara::core::String const &pubData) noexcept override;

protected:
    /// @brief Import key slot
    /// @param pKeySlot pointer to key slot object
    /// @param ioInterfaceBase IO interface: base class object
    /// @param nSlotName key slot name
    /// @param securedObject secured object
    /// @return true if import slot sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00718
    /// @needwork = dda
    /// @endcode
    static bool ImportSlot(keys::isoft_def::PIpcKeySlot *const pKeySlot,
                           IOInterface &ioInterfaceBase,
                           ara::core::String const &nSlotName,
                           ReadOnlyMemRegion const &securedObject) noexcept;
    /// @brief Determine whether asymmetric key lengths are the same
    /// @param cryptoObjectType cryptographic material type
    /// @param algId encryption algorithm cryptographic primitive ID
    /// @param otherKeyLen length of the other key
    /// @return true if length id equal false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00719
    /// @needwork = dda
    /// @endcode
    bool _IsAsymmetricKeyEqualLen(CryptoObjectType const cryptoObjectType,
                                  AlgId const algId,
                                  std::size_t const otherKeyLen) const noexcept;

private:
    /// @brief Import Secured Object
    /// @param container the IOInterface for storing of the imported object
    /// @param securedObject secured object
    /// @return true means import success, or it returns false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    bool _importSecuredObjectEx(IOInterface const &container, ReadOnlyMemRegion const securedObject) noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CRYPTO_PROVIDER_H_
