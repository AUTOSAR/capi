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
/// @file       msg_recovery_public_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Public key context for asymmetric recovery of short messages and its signature verification (similar to RSA).
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </table> <td>2021-12-21 <td>1.0.0 <td>hanjingjing <td>Created initial version
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Decryption/Signature and Verification
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=MsgRecoveryPublicCtx
/// @unit_description=Public key context base class for asymmetric recovery of short messages and its signature verification (similar to RSA)
/// @endcode
///
/// ================================================================
///
/// </table>
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_MSG_RECOVERY_PUBLIC_CTX_H_
#define ARA_CRYPTO_CRYP_MSG_RECOVERY_PUBLIC_CTX_H_

#include "ara/core/result.h"
#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/cryp/cryobj/public_key.h"
#include "ara/crypto/cryp/crypto_context.h"
#include "ara/crypto/cryp/extension_service.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Public key context for asymmetric recovery of short messages and its signature verification (similar to RSA).
///     Restricted groups of trusted subscribers can use this primitive to simultaneously provide confidentiality, authenticity, and non-repudiation of short messages, provided the public keys are generated appropriately and kept confidential.
///     If (0 == BlockCryptor::ProcessBlock(…)), the input message block is corrupted.
/// @brief A public key context for asymmetric recovery of a short message and its signature verification (RSA-like).
///     Restricted groups of trusted subscribers can use this primitive for simultaneous provisioning of
///     confidentiality, authenticity and non-repudiation of short messages, if the public key is generated
///     appropriately and kept in secret. If (0 == BlockCryptor::ProcessBlock(...)) then the input message-block is
///     violated.
/// @interface MsgRecoveryPublicCtx
/// @AUTOSAR_SWS {SWS_CRYPT_22200}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02202}
/// @uptrace={RS_CRYPTO_02204}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02189
/// @trace_id_dd=DD_CRYPTO_04960
/// @needwork = ad
/// @endcode
class MsgRecoveryPublicCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22201}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03191
    /// @trace_id_dd=DD_CRYPTO_06402
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< MsgRecoveryPublicCtx >;

    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02190
    /// @trace_id_dd=DD_CRYPTO_04961
    /// @needwork = ad
    /// @endcode
    MsgRecoveryPublicCtx() noexcept = default;
    /// @brief Default virtual destructor
    /// @brief Destructor.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20401}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02191
    /// @trace_id_dd=DD_CRYPTO_04962
    /// @needwork = ad
    /// @endcode
    ~MsgRecoveryPublicCtx() noexcept override = default;

    /// @brief Default copy assignment operator
    /// @brief Copy-assign another MsgRecoveryPublicCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02192
    /// @trace_id_dd=DD_CRYPTO_04963
    /// @needwork = ad
    /// @endcode
    MsgRecoveryPublicCtx &operator=(MsgRecoveryPublicCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another MsgRecoveryPublicCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02193
    /// @trace_id_dd=DD_CRYPTO_04964
    /// @needwork = ad
    /// @endcode
    MsgRecoveryPublicCtx &operator=(MsgRecoveryPublicCtx &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02194
    /// @trace_id_dd=DD_CRYPTO_04965
    /// @needwork = ad
    /// @endcode
    MsgRecoveryPublicCtx(MsgRecoveryPublicCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02195
    /// @trace_id_dd=DD_CRYPTO_04966
    /// @needwork = ad
    /// @endcode
    MsgRecoveryPublicCtx(MsgRecoveryPublicCtx &&other) noexcept = delete;

public:
    /// @brief Get the ExtensionService instance.
    /// @brief Get ExtensionService instance.
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22210}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02196
    /// @trace_id_dd=DD_CRYPTO_04967
    /// @needwork = ad
    /// @endcode
    virtual ExtensionService::Uptr GetExtensionService() const noexcept = 0;
    /// @brief Get the maximum expected size of the input data block. if (IsEncryption() ==
    /// false), the value returned by this method is independent of the suppressPadding parameter, and the value will equal the block size.
    /// @brief Get maximum expected size of the input data block.
    ///       if (IsEncryption() == false) then a value returned by this method is independent from
    ///       the @c suppressPadding argument and it will be equal to the block size.
    /// @param suppressPadding  if @c true then the method calculates the size for the case when the whole space of
    /// the plain data block is used for the payload only
    /// @return maximum size of the input data block in bytes
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22213}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02197
    /// @trace_id_dd=DD_CRYPTO_04968
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxInputSize(bool suppressPadding = false) const noexcept = 0;
    /// @brief Get the maximum possible size of the output data block. if (IsEncryption() ==
    /// true), the value returned by this method is independent of the suppressPadding parameter, and will equal the block size.
    /// @brief Get maximum possible size of the output data block.
    ///       If (IsEncryption() == true) then a value returned by this method is independent from the
    ///       @c suppressPadding argument and will be equal to the block size.
    /// @param suppressPadding  if @c true then the method calculates the size for the case when the whole space of
    /// the plain data block is used for the payload only
    /// @return maximum size of the output data block in bytes
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02198
    /// @trace_id_dd=DD_CRYPTO_04969
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxOutputSize(bool suppressPadding = false) const noexcept = 0;
    /// @brief Process an input block according to the encryptor configuration (encrypt/decrypt).
    /// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
    ///       Encryption with (suppressPadding == true) expects that: in.size() == GetMaxInputSize(true) && out.size()
    ///       >= GetMaxOutputSize(true). Encryption with (suppressPadding == false) expects that: in.size() <=
    ///       GetMaxInputSize(false) && in.size() > 0 && out.size() >= GetMaxOutputSize(false). Decryption expects that:
    ///       in.size() == GetMaxInputSize() && out.size() >= GetMaxOutputSize(suppressPadding). The case (out.size() <
    ///       GetMaxOutputSize()) should be used with caution, only if you are strictly certain about the size of the
    ///       output data! In case of (suppressPadding == true) the actual size of plain text should be equal to full
    ///       size of the plain data block (defined by the algorithm)!
    /// @param in  the input data block
    /// @return actual size of output data (it always <= out.size()) or 0 if the input data block has incorrect content
    /// @code{.isoft}
    /// @error:  SecurityErrorDomain::kIncorrectInputSize  if the mentioned above rules about the input size is violated
    /// @error:  SecurityErrorDomain::kUninitializedContext  if the context was not initialized by a key value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02199
    /// @trace_id_dd=DD_CRYPTO_04970
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > DecodeAndVerify(
        ReadOnlyMemRegion const &in) const noexcept = 0;
    /// @brief Process an input block according to the encryptor configuration (encrypt/decrypt). This method sets the size of the output container based on the actually saved values!
    /// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
    /// @param in  the input data block
    /// @return the managed container for output block
    /// @code{.isoft}
    /// @tparam Alloc  a custom allocator type of the output container
    /// @error:  SecurityErrorDomain::kIncorrectInputSize  if the mentioned above rules about the input size is violated
    /// @error:  SecurityErrorDomain::kUninitializedContext  if the context was not initialized by a key value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22216}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02200
    /// @trace_id_dd=DD_CRYPTO_04971
    /// @needwork = ad
    /// @endcode
    template < typename Alloc = VENDOR_IMPLEMENTATION_DEFINED >
    ara::core::Result< ByteVector< Alloc > > DecodeAndVerify(ReadOnlyMemRegion in) const noexcept
    {
        ara::core::Result< ara::core::Vector< ara::core::Byte > > result = DecodeAndVerify(in);
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
    /// @trace_id_sws={SWS_CRYPT_22212}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02201
    /// @trace_id_dd=DD_CRYPTO_04972
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Reset() noexcept = 0;
    /// @brief Set (deploy) a key for the msg recovery public algorithm context.
    /// @brief Set (deploy) a key to the msg recovery public algorithm context.
    /// @param key  the source key object
    /// @return  has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @error:  SecurityErrorDomain::kIncompatibleObject  if the provided key object is incompatible with this
    /// symmetric key context
    /// @error:  SecurityErrorDomain::kUsageViolation  if the transformation type associated with this context is
    /// prohibited by the "allowed usage" restrictions of provided key object.
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22211}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02202
    /// @trace_id_dd=DD_CRYPTO_04973
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetKey(PublicKey const &key) noexcept = 0;

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_MSG_RECOVERY_PUBLIC_CTX_H_
