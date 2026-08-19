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
/// @file       encryptor_public_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Asymmetric encryption public key context interface.
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Asymmetric Encryption
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=EncryptorPublicCtx
/// @unit_description=Asymmetric Encryption Public Key Context Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_ENCRYPTOR_PUBLIC_CTX_H_
#define ARA_CRYPTO_CRYP_ENCRYPTOR_PUBLIC_CTX_H_

#include "ara/core/result.h"
#include "ara/crypto/cryp/cryobj/public_key.h"
#include "ara/crypto/cryp/crypto_context.h"
#include "ara/crypto/cryp/crypto_service.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Asymmetric encryption public key context interface.
/// @brief Asymmetric Encryption Public key Context interface.
/// @code{.isoft}
/// @interface EncryptorPublicCtx
/// @export_level=/crypto
/// @AUTOSAR_SWS {SWS_CRYPT_21000}
/// @uptrace={RS_CRYPTO_02202}
/// @tracestatus={draft}
/// @endcode
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02367
/// @trace_id_dd=DD_CRYPTO_05147
/// @needwork = ad
/// @endcode
class EncryptorPublicCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21001}
    /// @uptrace={RS_CRYPTO_02202}
    /// @tracestatus={draft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03202
    /// @trace_id_dd=DD_CRYPTO_06414
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< EncryptorPublicCtx >;
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork = no
    /// @endcode
    using CryptoContext::CryptoContext;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02368
    /// @trace_id_dd=DD_CRYPTO_05148
    /// @needwork = ad
    /// @endcode
    ~EncryptorPublicCtx() override = default;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another EncryptorPublicCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02369
    /// @trace_id_dd=DD_CRYPTO_05149
    /// @needwork = ad
    /// @endcode
    EncryptorPublicCtx &operator=(EncryptorPublicCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another EncryptorPublicCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this,
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02370
    /// @trace_id_dd=DD_CRYPTO_05150
    /// @needwork = ad
    /// @endcode
    EncryptorPublicCtx &operator=(EncryptorPublicCtx &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02371
    /// @trace_id_dd=DD_CRYPTO_05151
    /// @needwork = ad
    /// @endcode
    EncryptorPublicCtx(EncryptorPublicCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02372
    /// @trace_id_dd=DD_CRYPTO_05152
    /// @needwork = ad
    /// @endcode
    EncryptorPublicCtx(EncryptorPublicCtx &&other) noexcept = delete;

public:
    /// @brief Get the CryptoService instance.
    /// @brief Get CryptoService instance.
    /// @return CryptoService instance
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21002}
    /// @uptrace={RS_CRYPTO_02006}
    /// @tracestatus={draft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02373
    /// @trace_id_dd=DD_CRYPTO_05153
    /// @needwork = ad
    /// @endcode
    virtual CryptoService::Uptr GetCryptoService() const noexcept = 0;
    /// @brief Process an input block according to the encryptor configuration (encrypt/decrypt).
    /// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
    ///         Encryption with (suppressPadding == true) expects that: in.size() == GetMaxInputSize(true) && out.size()
    ///         >= GetMaxOutputSize(true). Encryption with (suppressPadding == false) expects that: in.size() <=
    ///         GetMaxInputSize(false) && in.size() > 0 && out.size() >= GetMaxOutputSize(false). Decryption expects
    ///         that: in.size() == GetMaxInputSize() && out.size() >= GetMaxOutputSize(suppressPadding). The case
    ///         (out.size() < GetMaxOutputSize()) should be used with caution, only if you are strictly certain about
    ///         the size of the output data! In case of (suppressPadding == true) the actual size of plain text should
    ///         be equal to full size of the plain data block (defined by the algorithm)!
    /// @param in  the input data block
    /// @param suppressPadding  if @c true then the method doesn't apply the padding, but the payload should fill
    ///     the whole block of the plain data
    /// @return actual size of output data (it always <tt><= out.size()</tt>) or 0 if the input data block has
    ///     incorrect content
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21012}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncorrectInputSize     if the mentioned above rules about the input size is
    ///     violated
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02374
    /// @trace_id_dd=DD_CRYPTO_05154
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > ProcessBlock(ReadOnlyMemRegion const &in,
                                                                                   bool suppressPadding
                                                                                   = false) const noexcept = 0;
    /// @brief Process an input block according to the encryptor configuration (encrypt/decrypt).
    /// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
    /// @returns  the managed container for output block
    /// @param in  the input data block
    /// @param suppressPadding  if @c true then the method doesn't apply the padding, but the payload should fill
    ///     the whole block of the plain data
    /// @return actual size of output data (it always <tt><= out.size()</tt>) or 0 if the input data block has
    ///     incorrect content
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21013}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @tparam Alloc  a custom allocator type of the output container
    /// @error: SecurityErrorDomain::kIncorrectInputSize     if the mentioned above rules about the input size is
    ///     violated
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the @c out.size() is not enough to store the
    ///     transformation result
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02375
    /// @trace_id_dd=DD_CRYPTO_05155
    /// @needwork = ad
    /// @endcode
    template < typename Alloc = VENDOR_IMPLEMENTATION_DEFINED >
    ara::core::Result< ByteVector< Alloc > > ProcessBlock(ReadOnlyMemRegion in,
                                                          bool suppressPadding = false) const noexcept
    {
        ara::core::Result< ara::core::Vector< ara::core::Byte > > result = ProcessBlock(in, suppressPadding);
        if (false == result.HasValue()) {
            return ara::core::Result< ByteVector< Alloc > >::FromError(result.Error());
        }
        ByteVector< Alloc > out;
        out.resize(result.Value().size());
        memcpy(core::data(out), result.Value().data(), result.Value().size());
        return ara::core::Result< ByteVector< Alloc > >::FromValue(out);
    }
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21011}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02376
    /// @trace_id_dd=DD_CRYPTO_05156
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Reset() noexcept = 0;
    /// @brief Set (deploy) a key for the encryptor public algorithm context.
    /// @brief Set (deploy) a key to the encryptor public algorithm context.
    /// @param key  the source key object
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21010}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    ///     symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context is
    ///     prohibited by the "allowed usage" restrictions of provided key object
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02377
    /// @trace_id_dd=DD_CRYPTO_05157
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetKey(PublicKey const &key) noexcept = 0;

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_ENCRYPTOR_PUBLIC_CTX_H_
