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
/// @file       symmetric_block_cipher_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Interface for symmetric block cipher contexts with padding.
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Symmetric Encryption
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_01001
/// @unit_name=SymmetricBlockCipherCtx
/// @unit_description=Symmetric Block Cipher Context Base Class with Padding
/// @endcode
///
/// ================================================================
///
/// </table>
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_SYMMETRIC_BLOCK_CIPHER_CTX_H_
#define ARA_CRYPTO_CRYP_SYMMETRIC_BLOCK_CIPHER_CTX_H_

#include "ara/core/result.h"
#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/cryp/cryobj/symmetric_key.h"
#include "ara/crypto/cryp/crypto_context.h"
#include "ara/crypto/cryp/crypto_service.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Interface for symmetric block cipher contexts with padding.
/// @brief Interface of a Symmetric Block Cipher Context with padding.
/// @interface SymmetricBlockCipherCtx
/// @AUTOSAR_SWS {SWS_CRYPT_23700}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02201}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02378
/// @trace_id_dd=DD_CRYPTO_05158
/// @needwork = ad
/// @endcode
class SymmetricBlockCipherCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23701}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03203
    /// @trace_id_dd=DD_CRYPTO_06415
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< SymmetricBlockCipherCtx >;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork = no
    /// @endcode
    SymmetricBlockCipherCtx() noexcept = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02379
    /// @trace_id_dd=DD_CRYPTO_05159
    /// @needwork = ad
    /// @endcode
    ~SymmetricBlockCipherCtx() override = default;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another SymmetricBlockCipherCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02380
    /// @trace_id_dd=DD_CRYPTO_05160
    /// @needwork = ad
    /// @endcode
    SymmetricBlockCipherCtx &operator=(SymmetricBlockCipherCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another SymmetricBlockCipherCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02381
    /// @trace_id_dd=DD_CRYPTO_05161
    /// @needwork = ad
    /// @endcode
    SymmetricBlockCipherCtx &operator=(SymmetricBlockCipherCtx &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02382
    /// @trace_id_dd=DD_CRYPTO_05162
    /// @needwork = ad
    /// @endcode
    SymmetricBlockCipherCtx(SymmetricBlockCipherCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02383
    /// @trace_id_dd=DD_CRYPTO_05163
    /// @needwork = ad
    /// @endcode
    SymmetricBlockCipherCtx(SymmetricBlockCipherCtx &&other) noexcept = delete;

public:
    /// @brief Get the CryptoService instance.
    /// @brief Get CryptoService instance.
    /// @return CryptoService::Uptr
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23702}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02384
    /// @trace_id_dd=DD_CRYPTO_05164
    /// @needwork = ad
    /// @endcode
    virtual CryptoService::Uptr GetCryptoService() const noexcept = 0;
    /// @brief Get the transformation type configured for this context: kEncrypt or kDecrypt.
    /// @brief Get the kind of transformation configured for this context: kEncrypt or kDecrypt
    /// @return @c CryptoTransform
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the transformation direction of this context
    ///                is configurable during an initialization, but the context was not initialized yet
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23711}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02385
    /// @trace_id_dd=DD_CRYPTO_05165
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< CryptoTransform > GetTransformation() const noexcept = 0;
    // PRQA L:QAC
    /// @brief Indicates that the currently configured transformation only accepts complete input data blocks.
    /// @brief Indicate that the currently configured transformation accepts only complete blocks of input data.
    /// @return @c true if the transformation requires the maximum size of input data and @c false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the transformation direction of this context
    ///                is configurable during an initialization, but the context was not initialized yet
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23712}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02386
    /// @trace_id_dd=DD_CRYPTO_05166
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< bool > IsMaxInputOnly() const noexcept
    // PRQA L:QAC
    {
        ara::core::Result< CryptoTransform > const result{GetTransformation()};
        if (!result.HasValue()) {
            return ara::core::Result< bool >::FromError(result.Error());
        }
        if (CryptoTransform::kDecrypt == result.Value()) {
            return ara::core::Result< bool >::FromValue(true);
        }
        return ara::core::Result< bool >::FromValue(false);
    }
    /// @brief Indicates that the currently configured transformation can only produce complete output data blocks.
    /// @brief Indicate that the currently configured transformation can produce only complete blocks of output data.
    /// @return @c true if the transformation can produce only the maximum size of output data and @c false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the transformation direction of this context
    ///                is configurable during an initialization, but the context was not initialized yet
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23713}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02387
    /// @trace_id_dd=DD_CRYPTO_05167
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< bool > IsMaxOutputOnly() const noexcept
    {
        ara::core::Result< CryptoTransform > const result{GetTransformation()};

        if (!result.HasValue()) {
            return ara::core::Result< bool >::FromError(result.Error());
        }
        return ara::core::Result< bool >::FromValue((GetTransformation().Value() == CryptoTransform::kEncrypt));
    }
    /// @brief Process an input block according to the encryptor configuration (encrypt/decrypt).
    /// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
    /// @param in  the input data block
    /// @param suppressPadding  if @c true then the method doesn't apply the padding, but the payload should fill
    /// the whole block of the plain data
    /// @return    the output buffer
    /// @code{.isoft}
    /// @error:  SecurityErrorDomain::kIncorrectInputSize   if the mentioned above rules about the input size is
    /// violated
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the @c out.size() is not enough to store the
    /// transformation result
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23716}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02388
    /// @trace_id_dd=DD_CRYPTO_05168
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > ProcessBlock(ReadOnlyMemRegion const &in,
                                                                                   bool suppressPadding
                                                                                   = false) const noexcept = 0;
    // PRQA L:QAC

    /// @brief Process an input block according to the encryptor configuration (encrypt/decrypt). This method sets the size of the output container based on the actually saved values!
    /// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
    /// @returns  the managed container for output block
    /// @param in  the input data block
    /// @param suppressPadding  if @c true then the method doesn't apply the padding, but the payload should fill
    /// the whole block of the plain data
    /// @code{.isoft}
    /// @tparam Alloc  a custom allocator type of the output container
    /// @error: SecurityErrorDomain::kIncorrectInputSize     if the mentioned above rules about the input size is
    /// violated
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the @c out.size() is not enough to store the
    /// transformation result
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23717}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02389
    /// @trace_id_dd=DD_CRYPTO_05169
    /// @needwork = ad
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03204
    /// @trace_id_dd=DD_CRYPTO_06416
    /// @needwork = ad
    /// @endcode
    template < typename Alloc = VENDOR_IMPLEMENTATION_DEFINED >
    ara::core::Result< ByteVector< Alloc > > ProcessBlock(ReadOnlyMemRegion in,
                                                          bool suppressPadding = false) const noexcept
    {
        using PResult                                                    = ara::core::Result< ByteVector< Alloc > >;
        ara::core::Result< ara::core::Vector< ara::core::Byte > > result = ProcessBlock(in, suppressPadding);
        if (false == result.HasValue()) {
            return PResult::FromError(result.Error());
        }
        ByteVector< Alloc > out;
        auto const &vecData = result.Value();
        out.resize(vecData.size());
        memcpy(core::data(out), vecData.data(), vecData.size());
        return PResult::FromValue(out);
    }
    /// @brief
    /// Process provided blocks without padding. Input and output buffers must have the same size, and that size must be divisible by the block size (see GetBlockSize()). Pointers to input and output buffers must be aligned with the block size boundary!
    /// @brief Processe provided blocks without padding.
    ///         The @c in and @c out buffers @b must have same size and this size @b must be divisible by the block size
    ///         (see @c GetBlockSize()). Pointers to the input and output buffers must be aligned to the block-size
    ///         boundary!
    /// @returns  an output data buffer
    /// @param in  an input data buffer
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @error: SecurityErrorDomain::kInvalidInputSize       if size of the input buffer is not divisible by the block
    /// size (see @c GetBlockSize())
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if sizes of the input and output buffer are not equal
    /// @error: SecurityErrorDomain::kInOutBuffersIntersect  if the input and output buffers partially intersect
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23715}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02390
    /// @trace_id_dd=DD_CRYPTO_05170
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > ProcessBlocks(
        ReadOnlyMemRegion const &in) const noexcept = 0;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return has value if Reset sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23714}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02391
    /// @trace_id_dd=DD_CRYPTO_05171
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Reset() noexcept = 0;
    /// @brief Set (deploy) a key for the symmetric algorithm context.
    /// @brief Set (deploy) a key to the symmetric algorithm context.
    /// @param key  the source key object
    /// @param transform  the "direction" indicator: deploy the key for direct transformation (if @c true) or
    /// for reverse one (if @c false)
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context
    ///                (taking into account the direction specified by @c transform) is prohibited by the "allowed
    ///                usage" restrictions of provided key object
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23710}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02392
    /// @trace_id_dd=DD_CRYPTO_05172
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetKey(SymmetricKey const &key,
                                             CryptoTransform transform = CryptoTransform::kEncrypt) noexcept = 0;

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_SYMMETRIC_BLOCK_CIPHER_CTX_H_
