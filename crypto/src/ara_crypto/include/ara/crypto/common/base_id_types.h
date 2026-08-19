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
/// @file       base_id_types.h
/// @brief      AutoSar-Crypto Encryption/Decryption common module
/// @details    Defines some enums and constants
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
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Common_api
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_BASE_ID_TYPES_H_
#define ARA_CRYPTO_BASE_ID_TYPES_H_

#include <cstdint>
#include <memory>

#include "ara/core/vector.h"

/// @brief Alias for a byte vector template with a custom allocator.
/// @brief Alias of a bytes' vector template with customizable allocator
/// @trace_id_sws={SWS_CRYPT_10042}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02201}
/// @uptrace={RS_CRYPTO_02202}
/// @uptrace={RS_CRYPTO_02203}
/// @uptrace={RS_CRYPTO_02204}
/// @uptrace={RS_CRYPTO_02205}
/// @uptrace={RS_CRYPTO_02206}
/// @uptrace={RS_CRYPTO_02207}
/// @uptrace={RS_CRYPTO_02208}
/// @uptrace={RS_CRYPTO_02209}
/// @tparam Alloc  custom allocator of bytes sequences
// just make it compile
// DO NOT MAKE A SWS_ID!!
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_03260
/// @trace_id_dd=DD_CRYPTO_06502
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=VENDOR_IMPLEMENTATION_DEFINED
/// @needwork = ad
/// @endcode
#define VENDOR_IMPLEMENTATION_DEFINED std::allocator< std::uint8_t >
namespace ara {
namespace crypto {
//********************************/
/// @brief Container type and constant bit flags for keys or secret seed objects.
///         Only direct specified usage of a single key is allowed, others are prohibited! Similar flag sets are defined for usage restrictions of raw keys/seeds and symmetric keys or seeds derivable from raw keys.
///         A symmetric key or secret seed can be derived from a raw key only if it supports kAllowKeyAgreement or kAllowKeyDiversification or kAllowKeyDerivation!
/// @brief A container type and constant bit-flags of allowed usages of a key or a secret seed object.
///   Only directly specified usages of a key are allowed, all other are prohibited!
///   Similar set of flags are defined for the usage restrictions of original key/seed and for a symmetric key or
///       seed that potentially can be derived from the original one.
///   A symmetric key or secret seed can be derived from the original one, only if it supports @c kAllowKeyAgreement
///       or @c kAllowKeyDiversify or @c kAllowKeyDerivation!
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10015}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_03181
/// @trace_id_dd=DD_CRYPTO_06386
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using AllowedUsageFlags = std::uint32_t;

/// @brief Byte array alias
/// @code{.isoft}
/// @tparam Alloc
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03182
/// @trace_id_dd=DD_CRYPTO_06387
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=ByteVector
/// @needwork = ad
/// @endcode
template < class Alloc = VENDOR_IMPLEMENTATION_DEFINED >
/// @brief Result type alias
using ByteVector = ara::core::Vector< std::uint8_t, Alloc >;
/// @brief Container type for encryption algorithm identifiers.
/// @brief Container type of the Crypto Algorithm Identifier.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10014}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02102}
/// @uptrace={RS_CRYPTO_02107}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_03183
/// @trace_id_dd=DD_CRYPTO_06388
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using CryptoAlgId = std::uint64_t;
/// @brief Enumeration of all crypto object types, i.e., the type of content that can be stored in a key slot.
/// @brief Enumeration of all types of crypto objects, i.e. types of content that can be stored to a key slot.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10016}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02004}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02075
/// @trace_id_dd=DD_CRYPTO_04802
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
enum class CryptoObjectType : std::uint32_t
{
    /// Object type is currently not defined (empty container)
    kUndefined = 0,
    /// @c cryp::SymmetricKey object
    kSymmetricKey = 1,
    /// @c cryp::PrivateKey object
    kPrivateKey = 2,
    /// @c cryp::PublicKey object
    kPublicKey = 3,
    /// @c cryp::Signature object (asymmetric digital signature or symmetric MAC/HMAC or hash digest)
    kSignature = 4,
    /// @c cryp::SecretSeed object. @b Note: the seed cannot have an associated crypto algorithm!
    kSecretSeed = 5,
};
/// @brief Enumeration of all known provider types.
/// @brief Enumeration of all known Provider types.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10017}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02401}
/// @uptrace={RS_CRYPTO_02109}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02076
/// @trace_id_dd=DD_CRYPTO_04803
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
enum class ProviderType : std::uint32_t
{
    /// Undefined/Unknown Provider type (or applicable for the whole Crypto Stack)
    kUndefinedProvider = 0,
    /// Cryptography Provider
    kCryptoProvider = 1,
    /// Key Storage Provider
    kKeyStorageProvider = 2,
    /// X.509 Provider
    kX509Provider = 3,
};
/// @brief Enumeration of cryptographic transformations.
/// @brief Enumeration of cryptographic transformations.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10019}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02004}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02077
/// @trace_id_dd=DD_CRYPTO_04804
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
enum class CryptoTransform : std::uint32_t
{
    kUnknown = 0,  // Unknown behavior
    /// encryption          //Encryption
    kEncrypt = 1,
    /// decryption          //Decryption
    kDecrypt = 2,
    /// MAC verification    //MAC verification
    kMacVerify = 3,
    /// MAC generation      //MAC generation
    kMacGenerate = 4,
    /// key wrapping        //Key wrapping
    kWrap = 5,
    /// key unwrapping      //Key unwrapping
    kUnwrap = 6,
    /// signature verification  //Signature verification
    kSigVerify = 7,
    /// signature generation    //Signature generation
    kSigGenerate = 8,
};
/// @brief Enumeration of key slot types; currently only machine and application key slots are defined.
/// @brief Enumeration of key-slot types; currently only machine and applicaiton key-slots are defined.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_10018}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02004}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02078
/// @trace_id_dd=DD_CRYPTO_04805
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
enum class KeySlotType : std::uint32_t
{
    /// Empty slot
    kUnDefined = 0,
    /// machine type key-slot - can be managed by application
    kMachine = 1,
    /// application exclusive type key-slot
    kApplication = 2
};
// Common grouped constants should be numbered in the way similar to interfaces:
// SWS_CRYPT_1YYXX:
//   - ZZ - constants group ID (30-59)
//   - XX - constant definition ID (00-99)
//********************************/
/// @brief Undefined algorithm ID. This value can also be used for the following meanings: Any or Default algorithm, None of algorithm.
///         The valid values for encryption algorithm IDs are specific to the concrete crypto stack implementation. However, the value 0 is reserved for special purposes, depending on the usage context.
///         This group defines several constant names for the single zero value, but they have different semantic meanings, specific to the concrete application of the constant.
/// @brief Algorithm ID is undefined. Also this value may be used in meanings: Any or Default algorithm, None of
/// algorithms.
///         Effective values of Crypto Algorithm IDs are specific for concrete Crypto Stack implementation.
///         But the zero value is reserved for especial purposes, that can differ depending from a usage context.
///         This group defines a few constant names of the single zero value, but semantically they have different
///         meaning specific for concrete application of the constant.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13000}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02107}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04806
/// @needwork = dd
/// @endcode
CryptoAlgId const kAlgIdUndefined{0U};
/// @brief Allows any algorithm ID.
/// @brief Any Algorithm ID is allowed.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13001}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02107}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04807
/// @needwork = dd
/// @endcode
CryptoAlgId const kAlgIdAny{kAlgIdUndefined};
/// @brief Default algorithm ID (in the current context/primitive).
/// @brief Default Algorithm ID (in current context/primitive).
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13002}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02107}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04808
/// @needwork = dd
/// @endcode
CryptoAlgId const kAlgIdDefault{kAlgIdUndefined};
/// @brief No algorithm ID (i.e., algorithm definition not applicable).
/// @brief None of Algorithm ID (i.e. an algorithm definition is not applicable).
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13003}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02107}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04809
/// @needwork = dd
/// @endcode
CryptoAlgId const kAlgIdNone{kAlgIdUndefined};
//********************************/
/// @brief This group contains a list of predefined 1-bit constant values for "allowed usage" flags.
///         Key/seed usage will be fully specified by the key slot prototype (objects can only be used after reloading from the key slot).
/// @brief This group contains list of constant 1-bit values predefined for Allowed Usage flags.
///         The key/seed usage will be fully specified by a key slot prototype (the object can be used only after
///         reloading from the slot).
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13100}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04810
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowPrototypedOnly{0U};
/// @brief Key/seed can be used for data encryption initialization (applicable to symmetric and asymmetric algorithms).
/// @brief The key/seed can be used for data encryption initialization (applicable to symmetric and asymmetric
/// algorithms).
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13101}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04811
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowDataEncryption{0x0001U};
/// @brief Key/seed can be used for data decryption initialization (applicable to symmetric and asymmetric algorithms).
/// @brief The key/seed can be used for data decryption initialization (applicable to symmetric and asymmetric
/// algorithms).
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13102}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04812
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowDataDecryption{0x0002U};
/// @brief Key/seed can be used for digital signature or MAC/HMAC generation (applicable to symmetric and asymmetric algorithms).
/// @brief The key/seed can be used for digital signature or MAC/HMAC production (applicable to symmetric and asymmetric
/// algorithms).
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13103}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04813
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowSignature{0x0004U};
/// @brief Key/seed can be used for digital signature or MAC/HMAC verification (applicable to symmetric and asymmetric algorithms).
/// @brief The key/seed can be used for digital signature or MAC/HMAC verification (applicable to symmetric and
/// asymmetric algorithms).
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13104}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04814
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowVerification{0x0008U};
/// @brief Seed key or asymmetric key can be used for key agreement execution.
/// @brief The seed or asymmetric key can be used for key-agreement protocol execution.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13105}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04815
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowKeyAgreement{0x0010U};
/// @brief Seed key or symmetric key can be used for key encryption (diversification?).
/// @brief The seed or symmetric key can be used for slave-keys diversification.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13106}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04816
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowKeyDiversify{0x0020U};
/// @brief Seed or symmetric key can be used for seeding RandomGeneratorCtx.
/// @brief The seed or symmetric key can be used for seeding of a RandomGeneratorCtx
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13107}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04817
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowRngInit{0x0040U};
/// @brief This object can be used as input key material for KDF.
///         Seed key or symmetric key can be used as a RestrictedUseObject for derived keys via a Key Derivation Function (KDF).
/// @brief The object can be used as an input key material to KDF.
///           The seed or symmetric key can be used as a @c RestrictedUseObject for slave-keys derivation via a Key
///           Derivation Function (KDF).
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13108}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04818
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowKdfMaterial{0x0080U};
/// @brief Key can be used as a "transport" key for key-wrap or encapsulation transformations (applicable to symmetric and asymmetric keys).
/// @brief The key can be used as "transport" one for Key-Wrap or Encapsulate transformations (applicable to symmetric
/// and asymmetric keys).
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13109}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04819
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowKeyExporting{0x0100U};
/// @brief Key can be used as a "transport" key for key-unwrap or decapsulation transformations (applicable to symmetric and asymmetric keys).
/// @brief The key can be used as "transport" one for Key-Unwrap or Decapsulate transformations (applicable to symmetric
/// and asymmetric keys).
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13110}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04820
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowKeyImporting{0x0200U};
/// @brief  Key can be exported (if not set, the key cannot be exported)
/// @brief  The key can be exported(if not set, export is not possible)
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_40991}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_08806
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowExport{0x0400U};
/// @brief This key can only be used in the mode directly specified by key::AlgId.
/// @brief The key can be used only for the mode directly specified by @c Key::AlgId.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13111}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04821
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowExactModeOnly{0x8000U};
//***************/
/// @brief Derived seed or symmetric key can be used for data decryption.
/// @brief A derived seed or symmetric key can be used for data decryption.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13113}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04822
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowDerivedDataDecryption{kAllowDataDecryption << 16U};
/// @brief Derived seed or symmetric key can be used for data encryption.
/// @brief A derived seed or symmetric key can be used for data encryption.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13112}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04823
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowDerivedDataEncryption{kAllowDataEncryption << 16U};
/// @brief Derived seed or symmetric key can be used for seeding random generator contexts.
/// @brief A derived seed or symmetric key can be used for seeding of a RandomGeneratorContext
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13117}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04824
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowDerivedRngInit{kAllowRngInit << 16U};
/// @brief Restrict the usage of the derived object only to the specified operation mode. The derived seed key or symmetric key can only be used in the mode directly specified by key::AlgId.
/// @brief Restrict usage of derived objects to specified operation mode only.
///           A derived seed or symmetric key can be used only for the mode directly specified by @c Key::AlgId.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13121}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04825
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowDerivedExactModeOnly{kAllowExactModeOnly << 16U};
/// @brief Derived seed or symmetric key can be used as a RestrictedUseObject for deriving subkeys via a Key Derivation Function (KDF).
/// @brief A derived seed or symmetric key can be used as a @c RestrictedUseObject for slave-keys derivation via a Key
/// Derivation Function (KDF).
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13118}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04826
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowDerivedKdfMaterial{kAllowKdfMaterial << 16U};
/// @brief Derived seed key or symmetric key can be used for key encryption (diversification?) of subkeys.
/// @brief A derived seed or symmetric key can be used for slave-keys diversification.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13116}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04827
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowDerivedKeyDiversify{kAllowKeyDiversify << 16U};
/// @brief Derived seed or symmetric key can be used as a "transport" key for key-wrap transformations.
/// @brief A derived seed or symmetric key can be used as a "transport" one for Key-Wrap transformation.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13119}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04828
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowDerivedKeyExporting{kAllowKeyExporting << 16U};
/// @brief Derived seed or symmetric key can be used as a "transport" key for key-unwrap transformations.
/// @brief A derived seed or symmetric key can be used as a "transport" one for Key-Unwrap transformation.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13120}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04829
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowDerivedKeyImporting{kAllowKeyImporting << 16U};
/// @brief Derived seed or symmetric key can be used for MAC/HMAC generation.
/// @brief A derived seed or symmetric key can be used for MAC/HMAC production.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13114}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04830
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowDerivedSignature{kAllowSignature << 16U};
/// @brief Derived seed or symmetric key can be used for MAC/HMAC verification.
/// @brief A derived seed or symmetric key can be used for MAC/HMAC verification.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13115}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04831
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowDerivedVerification{kAllowVerification << 16U};
/// @brief Allow using this object as the key material for KDF and any usage of the derived object.
///         Seed key or symmetric key can be used as a RestrictedUseObject for the Key Derivation Function (KDF), and the derived "sub" key can be used without restrictions.
/// @brief Allow usage of the object as a key material for KDF and any usage of derived objects.
///           The seed or symmetric key can be used as a @c RestrictedUseObject for a Key Derivation Function (KDF) and
///           the derived "slave" keys can be used without limitations.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_13122}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02543
/// @trace_id_dd=DD_CRYPTO_04832
/// @needwork = dd
/// @endcode
AllowedUsageFlags const kAllowKdfMaterialAnyUsage{
    kAllowKdfMaterial | kAllowDerivedDataEncryption | kAllowDerivedDataDecryption | kAllowDerivedSignature
    | kAllowDerivedVerification | kAllowDerivedKeyDiversify | kAllowDerivedRngInit | kAllowDerivedKdfMaterial
    | kAllowDerivedKeyExporting | kAllowDerivedKeyImporting};
//********************************/
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_BASE_ID_TYPES_H_
