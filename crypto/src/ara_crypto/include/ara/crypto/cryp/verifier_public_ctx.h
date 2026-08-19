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
/// @file       verifier_public_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Signature verification public key context interface.
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Signature and Verification
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=VerifierPublicCtx
/// @unit_description=Signature Verification Public Key Context Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_VERIFIER_PUBLIC_CTX_H_
#define ARA_CRYPTO_CRYP_VERIFIER_PUBLIC_CTX_H_

#include "ara/core/result.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/cryp/cryobj/public_key.h"
#include "ara/crypto/cryp/cryobj/signature.h"
#include "ara/crypto/cryp/crypto_context.h"
#include "ara/crypto/cryp/signature_service.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Signature verification public key context interface.
/// @brief Signature Verification Public key Context interface.
/// @interface VerifierPublicCtx
/// @AUTOSAR_SWS {SWS_CRYPT_24100}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02204}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02341
/// @trace_id_dd=DD_CRYPTO_05121
/// @needwork = ad
/// @endcode
class VerifierPublicCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24101}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03200
    /// @trace_id_dd=DD_CRYPTO_06412
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< VerifierPublicCtx >;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork = no
    /// @endcode
    VerifierPublicCtx() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02342
    /// @trace_id_dd=DD_CRYPTO_05122
    /// @needwork = ad
    /// @endcode
    ~VerifierPublicCtx() override = default;

    /// @brief Default copy assignment operator
    /// @brief Copy-assign another VerifierPublicCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02343
    /// @trace_id_dd=DD_CRYPTO_05123
    /// @needwork = ad
    /// @endcode
    VerifierPublicCtx &operator=(VerifierPublicCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another VerifierPublicCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02344
    /// @trace_id_dd=DD_CRYPTO_05124
    /// @needwork = ad
    /// @endcode
    VerifierPublicCtx &operator=(VerifierPublicCtx &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02345
    /// @trace_id_dd=DD_CRYPTO_05125
    /// @needwork = ad
    /// @endcode
    VerifierPublicCtx(VerifierPublicCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02346
    /// @trace_id_dd=DD_CRYPTO_05126
    /// @needwork = ad
    /// @endcode
    VerifierPublicCtx(VerifierPublicCtx &&other) noexcept = delete;

public:
    /// @brief Extension service member class.
    /// @brief Extension service member class
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24102}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02347
    /// @trace_id_dd=DD_CRYPTO_05127
    /// @needwork = ad
    /// @endcode
    virtual SignatureService::Uptr GetSignatureService() const noexcept = 0;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return has value if Reset sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24116}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02348
    /// @trace_id_dd=DD_CRYPTO_05128
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Reset() noexcept = 0;
    /// @brief Set (deploy) a key to the verifier public algorithm context.
    /// @brief Set (deploy) a key to the verifier public algorithm context.
    /// @param key  the source key object
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @error:  SecurityErrorDomain::kIncompatibleObject    if the provided key object is incompatible with this
    /// symmetric key context
    /// @error:  SecurityErrorDomain::kUsageViolation        if the transformation type associated with this context is
    /// prohibited by the "allowed usage" restrictions of provided key object
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24115}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02349
    /// @trace_id_dd=DD_CRYPTO_05129
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetKey(PublicKey const &key) noexcept = 0;
    /// @brief Verify the signature against the digest value stored in the hash function context.
    ///         This is a pass-through interface to SWS_CRYPT_24113 for developer convenience, meaning it adds extra input checks and then calls the verify() interface from SWS_CRYPT_24113.
    /// @brief Verify signature by a digest value stored in the hash-function context.
    ///         This is a pass-through interface to SWS_CRYPT_24113 for developer convenience,
    ///         i.e. it adds additional input checks amd then calls the verify() interface from SWS_CRYPT_24113.
    /// @param hashFn  hash function to be used for hashing
    /// @param signature  the signature object for verification
    /// @param context  an optional user supplied "context" (its support depends from concrete algorithm)
    /// @returns @c true if the signature was verified successfully and @c false otherwise
    /// @code{.isoft}
    /// @error:  SecurityErrorDomain::kProcessingNotFinished  if the method @c hashFn.Finish() was not called before
    /// this method call
    /// @error:  SecurityErrorDomain::kInvalidArgument  if the CryptoAlgId of @c hashFn differs from the CryptoAlgId of
    /// this context
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24111}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02350
    /// @trace_id_dd=DD_CRYPTO_05130
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< bool > VerifyPrehashed(HashFunctionCtx const &hashFn,
                                                      Signature const &signature,
                                                      ReadOnlyMemRegion const &context
                                                      = ReadOnlyMemRegion()) const noexcept = 0;
    /// @brief Verify the signature BLOB against a directly provided hash value or message value.
    ///         This method can be used to implement "multi-pass" signature algorithms that process messages directly, i.e., without the need for "pre-hashing" (such as Ed25519ctx).
    ///         This method is also suitable for implementing traditional pre-hashed signature schemes (such as Ed25519ph, Ed448ph, ECDSA).
    ///         If the target algorithm does not support the context parameter, an empty value (default) must be provided!
    ///         The user-provided context can be used for the following algorithms: Ed25519ctx, Ed25519ph, Ed448ph.
    /// @brief Verify signature BLOB by a directly provided hash or message value.
    ///         This method can be used for implementation of the "multiple passes" signature algorithms that process a
    ///         message directly, i.e. without "pre-hashing" (like Ed25519ctx). But also this method is suitable for
    ///         implementation of the traditional signature schemes with pre-hashing (like Ed25519ph, Ed448ph, ECDSA).
    ///         If the target algorithm doesn't support the @c context argument then the empty (default) value must be
    ///         supplied! The user supplied @c context may be used for such algorithms as: Ed25519ctx, Ed25519ph,
    ///         Ed448ph.
    /// @param value  the (pre-)hashed or direct message value that should be verified
    /// @param signature  the signature BLOB for the verification (the BLOB contains a plain sequence of the digital
    ///            signature components located in fixed/maximum length fields defined by the algorithm specification,
    ///            and each component is presented by a raw bytes sequence padded by zeroes to full length of the field;
    ///            e.g. in case of (EC)DSA-256 (i.e. length of the q module is 256 bits) the signature BLOB must have
    ///            two fixed-size fields: 32 + 32 bytes, for R and S components respectively, i.e. total BLOB size is 64 bytes)
    /// @param context  an optional user supplied "context" (its support depends from concrete algorithm)
    /// @returns @c true if the signature was verified successfully and @c false otherwise
    /// @code{.isoft}
    /// @error:  SecurityErrorDomain::kUninitializedContext  if the context was not initialized by a key value
    /// @error:  SecurityErrorDomain::kInvalidInputSize      if the @c context argument has unsupported size
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24112}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02351
    /// @trace_id_dd=DD_CRYPTO_05131
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< bool > Verify(ReadOnlyMemRegion const &value,
                                             ReadOnlyMemRegion const &signature,
                                             ReadOnlyMemRegion const &context = ReadOnlyMemRegion()) const noexcept = 0;
    /// @brief Verify the signature against the digest value stored in the hash function context.
    ///         This is a pass-through interface to SWS_CRYPT_24112 for developer convenience, meaning it adds extra input checks and then calls the default verify() interface.
    /// @brief Verify signature by a digest value stored in the hash-function context. This is a pass-through
    ///        interface to SWS_CRYPT_24112 for developer convenience, i.e. it adds additional input checks
    ///        amd then calls the default verify() interface.
    /// @param hashAlgId  hash function algorithm ID
    /// @param hashValue  hash function value (resulting digest without any truncations)
    /// @param signature  the signature object for the verification
    /// @param context    an optional user supplied "context" (its support depends from concrete algorithm)
    /// @returns @c true if the signature was verified successfully and @c false otherwise
    /// @code{.isoft}
    /// @error:  SecurityErrorDomain::kIncompatibleObject    if the CryptoAlgId of this context does not match the
    /// CryptoAlgId of @c signature;
    ///         or the required CryptoAlgId of the hash is not kAlgIdDefault and the required hash CryptoAlgId of this
    ///         context does not match @c hashAlgId or the hash CryptoAlgId of @c signature
    /// @error:  SecurityErrorDomain::kIncompatibleArguments if the provided hashAlgId is not kAlgIdDefault and the
    /// AlgId of the provided signature object does not match the provided hashAlgId
    /// @error:  SecurityErrorDomain::kBadObjectReference    if the provided signature object does not reference the
    /// public key loaded to the context,
    ///                                                   i.e. if the COUID of the public key in the context is not
    ///                                                   equal to the COUID referenced from the signature object.
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24113}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02352
    /// @trace_id_dd=DD_CRYPTO_05132
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< bool > VerifyPrehashed(CryptoAlgId hashAlgId,
                                                      ReadOnlyMemRegion const &hashValue,
                                                      Signature const &signature,
                                                      ReadOnlyMemRegion const &context
                                                      = ReadOnlyMemRegion()) const noexcept = 0;
    /// @brief Verify the signature against the digest value stored in the hash function context.
    ///         This is a pass-through interface to SWS_CRYPT_24112 for developer convenience, meaning it adds extra input checks and then calls the default verify() interface.
    /// @brief Verify signature by a digest value stored in the hash-function context. This is a pass-through
    ///        interface to SWS_CRYPT_24112 for developer convenience, i.e. it adds additional input checks
    ///        amd then calls the default verify() interface.
    /// @param hashFn  hash function to be used for hashing
    /// @param signature  the data BLOB to be verified
    /// @param context  an optional user supplied "context" (its support depends from concrete algorithm)
    /// @returns @c true if the signature was verified successfully and @c false otherwise
    /// @code{.isoft}
    /// @error:  SecurityErrorDomain::kProcessingNotFinished     if the method @c hashFn.Finish() was not called before
    /// this method call
    /// @error:  SecurityErrorDomain::kInvalidArgument           if the CryptoAlgId of @c hashFn differs from the
    /// CryptoAlgId of this context
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24114}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02353
    /// @trace_id_dd=DD_CRYPTO_05133
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< bool > VerifyPrehashed(HashFunctionCtx const &hashFn,
                                                      ReadOnlyMemRegion const &signature,
                                                      ReadOnlyMemRegion const &context
                                                      = ReadOnlyMemRegion()) const noexcept = 0;

public:
};
//********************************/

}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_VERIFIER_PUBLIC_CTX_H_
