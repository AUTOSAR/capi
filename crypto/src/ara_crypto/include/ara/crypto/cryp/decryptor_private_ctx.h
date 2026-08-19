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
/// @file       decryptor_private_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Asymmetric decryption private key context interface.
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
/// @unit_name=DecryptorPrivateCtx
/// @unit_description=Asymmetric Decryption Private Key Context Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_DECRYPTOR_PRIVATE_CTX_H_
#define ARA_CRYPTO_CRYP_DECRYPTOR_PRIVATE_CTX_H_

#include "ara/core/result.h"
#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/cryp/cryobj/private_key.h"
#include "ara/crypto/cryp/crypto_context.h"
#include "ara/crypto/cryp/crypto_service.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Asymmetric decryption private key context interface.
/// @brief Asymmetric Decryption Private key Context interface.
/// @code{.isoft}
/// @export_level=/crypto
/// @AUTOSAR_SWS {SWS_CRYPT_20800}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02202}
/// @interface DecryptorPrivateCtx
/// @endcode
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02178
/// @trace_id_dd=DD_CRYPTO_04949
/// @needwork = ad
/// @endcode
class DecryptorPrivateCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20801}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @tracestatus={draft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03190
    /// @trace_id_dd=DD_CRYPTO_06401
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< DecryptorPrivateCtx >;
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork = no
    /// @endcode
    DecryptorPrivateCtx() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02179
    /// @trace_id_dd=DD_CRYPTO_04950
    /// @needwork = ad
    /// @endcode
    ~DecryptorPrivateCtx() override = default;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another DecryptorPrivateCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02180
    /// @trace_id_dd=DD_CRYPTO_04951
    /// @needwork = ad
    /// @endcode
    DecryptorPrivateCtx &operator=(DecryptorPrivateCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another DecryptorPrivateCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this,
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02181
    /// @trace_id_dd=DD_CRYPTO_04952
    /// @needwork = ad
    /// @endcode
    DecryptorPrivateCtx &operator=(DecryptorPrivateCtx &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02182
    /// @trace_id_dd=DD_CRYPTO_04953
    /// @needwork = ad
    /// @endcode
    DecryptorPrivateCtx(DecryptorPrivateCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02183
    /// @trace_id_dd=DD_CRYPTO_04954
    /// @needwork = ad
    /// @endcode
    DecryptorPrivateCtx(DecryptorPrivateCtx &&other) noexcept = delete;

public:
    /// @brief Get the CryptoService instance.
    /// @brief Get CryptoService instance.
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20802}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02184
    /// @trace_id_dd=DD_CRYPTO_04955
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
    ///         the size of the output data! In case of (suppress Padding == true) the actual size of plain text should
    ///         be equal to full size of the plain data block (defined by the algorithm)!
    /// @param in  the input data block
    /// @param suppressPadding  if @c true then the method doesn't apply the padding, but the payload should fill
    /// the whole block of the plain data
    /// @return actual size of output data (it always <tt><= out.size()</tt>) or 0 if the input data block has
    /// incorrect content
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncorrectInputSize     if the mentioned above rules about the input size is
    /// violated
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the @c out.size() is not enough to store the
    /// transformation result
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20812}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02185
    /// @trace_id_dd=DD_CRYPTO_04956
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > ProcessBlock(ReadOnlyMemRegion const &in,
                                                                                   bool suppressPadding
                                                                                   = false) const noexcept = 0;
    /// @brief Process an input block according to the encryptor configuration (encrypt/decrypt).
    /// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
    /// @param in  the input data block
    /// @param suppressPadding  if @c true then the method doesn't apply the padding, but the payload should fill
    /// @return  the managed container for output block
    /// the whole block of the plain data
    /// @code{.isoft}
    /// @tparam Alloc  a custom allocator type of the output container
    /// @error: SecurityErrorDomain::kIncorrectInputSize     if the mentioned above rules about the input size is
    /// violated
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the @c out.size() is not enough to store the
    /// transformation result
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20813}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02186
    /// @trace_id_dd=DD_CRYPTO_04957
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
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20811}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02187
    /// @trace_id_dd=DD_CRYPTO_04958
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Reset() noexcept = 0;
    /// @brief Set (deploy) a key for the decryptor private algorithm context.
    /// @brief Set (deploy) a key to the decryptor private algorithm context.
    /// @param key  the source key object
    /// @return Has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject  if the provided key object is incompatible with this symmetric
    /// key context
    /// @error: SecurityErrorDomain::kUsageViolation  if the transformation type associated with this context is
    /// prohibited by the "allowed usage" restrictions of provided key object
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20810}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02188
    /// @trace_id_dd=DD_CRYPTO_04959
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetKey(PrivateKey const &key) noexcept = 0;

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_DECRYPTOR_PRIVATE_CTX_H_
