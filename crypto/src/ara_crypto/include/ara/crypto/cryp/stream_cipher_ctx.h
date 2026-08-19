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
/// @file       stream_cipher_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Generic stream cipher context interface (covers all operation modes).
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Symmetric Encryption
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_01002
/// @unit_name=StreamCipherCtx
/// @unit_description=Generic Stream Cipher Context Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_STREAM_CIPHER_CTX_H_
#define ARA_CRYPTO_CRYP_STREAM_CIPHER_CTX_H_

#include "ara/core/result.h"
#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/cryp/block_service.h"
#include "ara/crypto/cryp/cryobj/secret_seed.h"
#include "ara/crypto/cryp/cryobj/symmetric_key.h"
#include "ara/crypto/cryp/crypto_context.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Generic stream cipher context interface (covers all operation modes).
/// @brief Generalized Stream Cipher Context interface (it covers all modes of operation).
/// @interface StreamCipherCtx
/// @AUTOSAR_SWS {SWS_CRYPT_23600}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02201}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02282
/// @trace_id_dd=DD_CRYPTO_05058
/// @needwork = ad
/// @endcode
class StreamCipherCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23601}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03196
    /// @trace_id_dd=DD_CRYPTO_06408
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< StreamCipherCtx >;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork = no
    /// @endcode
    StreamCipherCtx() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02283
    /// @trace_id_dd=DD_CRYPTO_05059
    /// @needwork = ad
    /// @endcode
    ~StreamCipherCtx() override = default;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another StreamCipherCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02284
    /// @trace_id_dd=DD_CRYPTO_05060
    /// @needwork = ad
    /// @endcode
    StreamCipherCtx &operator=(StreamCipherCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another StreamCipherCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02285
    /// @trace_id_dd=DD_CRYPTO_05061
    /// @needwork = ad
    /// @endcode
    StreamCipherCtx &operator=(StreamCipherCtx &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02286
    /// @trace_id_dd=DD_CRYPTO_05062
    /// @needwork = ad
    /// @endcode
    StreamCipherCtx(StreamCipherCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02287
    /// @trace_id_dd=DD_CRYPTO_05063
    /// @needwork = ad
    /// @endcode
    StreamCipherCtx(StreamCipherCtx &&other) noexcept = delete;

public:
    /// @brief Count the number of bytes currently kept in the context cache.
    ///         In block mode, if the application provides the last incomplete input data block, the context saves the rest of the last (incomplete) block into an internal "cache" memory and waits for the next call for additional input to complete this block.
    /// @brief Count number of bytes now kept in the context cache.
    ///         In block-wise modes if an application has supplied input data chunks with incomplete last block then the
    ///         context saves the rest part of the last (incomplete) block to internal "cache" memory and wait a next
    ///         call for additional input to complete this block.
    /// @return number of bytes now kept in the context cache
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23620}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02288
    /// @trace_id_dd=DD_CRYPTO_05064
    /// @needwork = ad
    /// @endcode
    virtual std::size_t CountBytesInCache() const noexcept = 0;
    /// @brief Estimate the maximum number of input bytes that can be used to fill the output buffer without overflow.
    /// @brief Estimate maximal number of input bytes that may be processed for filling of an output buffer without
    /// overflow.
    /// @param outputCapacity  capacity of the output buffer
    /// @return maximum number of input bytes
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23621}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02289
    /// @trace_id_dd=DD_CRYPTO_05065
    /// @needwork = ad
    /// @endcode
    std::size_t EstimateMaxInputSize(std::size_t const outputCapacity) const noexcept
    {
        std::size_t const kGranularity{IsBytewiseMode() ? 1U : GetBlockService()->GetBlockSize()};
        std::size_t const kUsableCapacity{outputCapacity / kGranularity * kGranularity};
        if (kUsableCapacity == 0U) {
            return 0U;
        }
        return kUsableCapacity - CountBytesInCache();
    }
    /// @brief Estimate the minimum capacity required for the output buffer, sufficient to hold the result of processing the input data.
    /// @brief Estimate minimal required capacity of the output buffer, which is enough for saving a result of input
    /// data processing.
    /// @param inputSize  size of input data
    /// @param isFinal  flag that indicates processing of the last data chunk (if @c true)
    /// @return required capacity of the output buffer (in bytes)
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23622}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02290
    /// @trace_id_dd=DD_CRYPTO_05066
    /// @needwork = ad
    /// @endcode
    std::size_t EstimateRequiredCapacity(std::size_t const inputSize, bool const isFinal) const noexcept
    {
        std::size_t kGranularity{0U};
        if (IsBytewiseMode()) {
            kGranularity = 1U;
        } else {
            kGranularity = GetBlockService()->GetBlockSize();
        }
        std::size_t maxPaddingSize{0U};
        if (false == isFinal) {
            maxPaddingSize = 0U;
        } else {
            if (IsBytewiseMode() == false) {
                maxPaddingSize = 0U;
            } else {
                if (GetTransformation().HasValue()) {
                    maxPaddingSize = 0U;
                } else {
                    maxPaddingSize = kGranularity;
                }
            }
        }
        return (inputSize + CountBytesInCache() + maxPaddingSize) / kGranularity * kGranularity;
    }
    /// @brief Process the last part of the message (may not be aligned to the block size boundary).
    ///         If (IsBytewiseMode() == false) it must be: bs = GetBlockSize(), out.size() >= (((in.size() + bs
    ///         *((CryptoTransform::kEncrypt == GetTransformation().Value()) ? 2 : 1) - 1) / bs) * bs)
    ///         if(IsBytewiseMode() == true)   it must be: out.size() >= in.size() Input and output buffers must not intersect!
    ///         When processing the last data block in block mode, this method must be used! This method can be used to process the entire message in a single call (in any mode)!
    /// @brief Processe the final part of message (that may be not aligned to the block-size boundary).
    ///         If (IsBytewiseMode() == false) then it must be: bs = GetBlockSize(), out.size() >= (((in.size() + bs
    ///         *((CryptoTransform::kEncrypt == GetTransformation().Value()) ? 2 : 1) - 1) / bs) * bs) If
    ///         (IsBytewiseMode() == true)  then it must be: out.size() >= in.size() The input and output buffers must
    ///         not intersect! Usage of this method is mandatory for processing of the last data chunk in block-wise
    ///         modes! This method may be used for processing of a whole message in a single call (in any mode)!
    /// @param in  an input data buffer
    /// @return an output data buffer
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the output buffer is not enough
    /// @error: SecurityErrorDomain::kInOutBuffersIntersect  if the input and output buffers intersect
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if data processing was not started by a call of the @c
    /// Start() method
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23618}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02291
    /// @trace_id_dd=DD_CRYPTO_05067
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > FinishBytes(
        ReadOnlyMemRegion const &in) noexcept = 0;
    // PRQA L:QAC
    /// @brief Process the last part of the message (may not be aligned to the block size boundary). This method sets the size of the output container based on the actually saved values.
    ///         If (IsBytewise Mode() == false) then it must be: bs = GetBlockSize(), out.capacity() >= (((in.size() +
    ///         bs * ((CryptoTransform::kEncrypt == GetTransformation.Value()) ? 2 : 1) - 1) / bs) * bs) If
    ///         (IsBytewiseMode() == true) then it must be: out.capacity() >= in.size()
    ///         Use this method when processing the last data block in block mode! This method can be used to process the entire message in a single call (in any mode)!
    /// @brief Processe the final part of message (that may be not aligned to the block-size boundary).
    ///         This method sets the size of the output container according to actually saved value.
    ///         If (IsBytewise Mode() == false) then it must be: bs = GetBlockSize(), out.capacity() >= (((in.size() +
    ///         bs * ((CryptoTransform::kEncrypt == GetTransformation.Value()) ? 2 : 1) - 1) / bs) * bs) If
    ///         (IsBytewiseMode() == true) then it must be: out.capacity() >= in.size() Usage of this method is
    ///         mandatory for processing of the last data chunk in block-wise modes! This method may be used for
    ///         processing of a whole message in a single call (in any mode)!
    /// @return a managed container for output data
    /// @param in  an input data buffer.    The input buffer @b must @b not point inside the output container!
    /// @code{.isoft}
    /// @tparam Alloc  a custom allocator type of the output container
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the output container is not enough
    /// @error: SecurityErrorDomain::kInOutBuffersIntersect  if the input and output buffers intersect
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if data processing was not started by a call of the @c
    /// Start() method
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23619}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02292
    /// @trace_id_dd=DD_CRYPTO_05068
    /// @needwork = ad
    /// @endcode
    template < typename Alloc = VENDOR_IMPLEMENTATION_DEFINED >
    ara::core::Result< ByteVector< Alloc > > FinishBytes(ReadOnlyMemRegion in) noexcept
    {
        ara::core::Result< ara::core::Vector< ara::core::Byte > > result = FinishBytes(in);
        if (false == result.HasValue()) {
            return ara::core::Result< ByteVector< Alloc > >::FromError(result.Error());
        }
        ByteVector< Alloc > out;
        out.resize(result.Value().size());
        memcpy(core::data(out), result.Value().data(), result.Value().size());
        return ara::core::Result< ByteVector< Alloc > >::FromValue(out);
    }
    /// @brief Get the BlockService instance.
    /// @brief Get BlockService instance.
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23602}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02293
    /// @trace_id_dd=DD_CRYPTO_05069
    /// @needwork = ad
    /// @endcode
    virtual BlockService::Uptr GetBlockService() const noexcept = 0;
    /// @brief Check the operation mode for byte attributes.
    /// @brief Check the operation mode for the bytewise property.
    /// @return @c true if the mode can process messages the byte-by-byte (without padding up to the block boundary)
    ///          and @c false if only the block-by-block (only full blocks can be processed, the padding is mandatory)
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23611}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02294
    /// @trace_id_dd=DD_CRYPTO_05070
    /// @needwork = ad
    /// @endcode
    virtual bool IsBytewiseMode() const noexcept = 0;
    /// @brief Get the transformation type configured for this context: kEncrypt or kDecrypt.
    /// @brief Get the kind of transformation configured for this context: kEncrypt or kDecrypt
    /// @return @c CryptoTransform
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext  if the transformation direction of this context
    /// is configurable during an initialization, but the context was not initialized yet
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23624}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02295
    /// @trace_id_dd=DD_CRYPTO_05071
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< CryptoTransform > GetTransformation() const noexcept = 0;
    // PRQA L:QAC
    /// @brief Check whether seek operations are supported in the current mode.
    /// @brief Check if the seek operation is supported in the current mode.
    /// @return @c true the seek operation is supported in the current mode and @c false otherwise
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23612}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02296
    /// @trace_id_dd=DD_CRYPTO_05072
    /// @needwork = ad
    /// @endcode
    virtual bool IsSeekableMode() const noexcept = 0;
    /// @brief Process the initial part of the message aligned with the block size boundary.
    ///         This is a copy-optimized method that does not use an internal cache buffer! It can only be used before processing any data that is not aligned with the block size boundary.
    ///         Pointers to input and output buffers must be aligned with the block size boundary! Input and output buffers can completely overlap, but they must not partially intersect!
    /// @brief Processe initial parts of message aligned to the block-size boundary.
    ///         It is a copy-optimized method that doesn’t use the internal cache buffer!
    ///         It can be used only before processing of any non-aligned to the block-size boundary data.
    ///         Pointers to the input and output buffers must be aligned to the block-size boundary!
    ///         The input and output buffers may completely coincide, but they must not partially intersect!
    /// @param in  an input data buffer
    /// @returns  an output data buffer
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if sizes of the input and output buffers are not equal
    /// @error: SecurityErrorDomain::kInvalidInputSize       if size of the input buffer is not divisible by the block
    /// size (see @c GetBlockSize())
    /// @error: SecurityErrorDomain::kInOutBuffersIntersect  if the input and output buffers partially intersect
    /// @error: SecurityErrorDomain::kInvalidUsageOrder      if this method is called after processing of non-aligned
    /// data (to the block-size boundary)
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the data processing was not started by a call of the @c
    /// Start() method
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23614}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02297
    /// @trace_id_dd=DD_CRYPTO_05073
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > ProcessBlocks(
        ReadOnlyMemRegion const &in) noexcept = 0;
    /// @brief Process the initial part of the message aligned with the block size boundary.
    ///         This is a copy-optimized method that does not use an internal cache buffer! It can be used for the first non-block-aligned data processing. Pointers to input and output buffers must be aligned with the block size boundary!
    /// @brief Processe initial parts of message aligned to the block-size boundary.
    ///         It is a copy-optimized method that doesn’t use internal cache buffer! It can be used up to first
    ///         non-block aligned data processing. Pointer to the input-output buffer must be aligned to the block-size
    ///         boundary!
    /// @param inOut  an input and output data buffer, i.e. the whole buffer should be updated
    /// @return  an output data buffer
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidInputSize       if size of the @c inOut buffer is not divisible by the
    /// block size (see @c GetBlockSize())
    /// @error: SecurityErrorDomain::kInvalidUsageOrder      if this method is called after processing of non-aligned
    /// data (to the block-size boundary)
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the data processing was not started by a call of the @c
    /// Start() method
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23615}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02298
    /// @trace_id_dd=DD_CRYPTO_05074
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > ProcessBlocks(ReadWriteMemRegion &inOut) noexcept = 0;
    /// @brief Process a non-final part of the message (not aligned with the block size boundary).
    ///         If (IsBytewiseMode() == false) then it must be: bs = GetBlockSize(), out.size() >= (((in.size() + bs -
    ///         1) / bs) * bs) If (IsBytewiseMode() == true) then it must be: out.size() >= in.size()
    ///         Input and output buffers must not intersect! This method is "copy-ineffective", so it should only be used when the application cannot control the chunking of the original message!
    /// @brief Processe a non-final part of message (that is not aligned to the block-size boundary).
    ///         If (IsBytewiseMode() == false) then it must be: bs = GetBlockSize(), out.size() >= (((in.size() + bs -
    ///         1) / bs) * bs) If (IsBytewiseMode() == true) then it must be: out.size() >= in.size() The input and
    ///         output buffers must not intersect! This method is "copy inefficient", therefore it should be used only
    ///         in conditions when an application cannot control the chunking of the original message!
    /// @param in  an input data buffer
    /// @returns an output data buffer
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the output buffer has capacity insufficient for placing
    /// of the transformation result
    /// @error: SecurityErrorDomain::kInOutBuffersIntersect  if the input and output buffers intersect
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if data processing was not started by a call of the @c
    /// Start() method
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23616}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02299
    /// @trace_id_dd=DD_CRYPTO_05075
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > ProcessBytes(
        ReadOnlyMemRegion const &in) noexcept = 0;
    /// @brief Process a non-final part of the message (not aligned with the block size boundary).
    /// @brief Processes a non-final part of message (that is not aligned to the block-size boundary).
    /// @param in  an input data buffer
    /// @returns  a managed container for the output data
    /// @code{.isoft}
    /// @tparam Alloc  a custom allocator type of the output container
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23617}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02300
    /// @trace_id_dd=DD_CRYPTO_05076
    /// @needwork = ad
    /// @endcode
    template < typename Alloc = VENDOR_IMPLEMENTATION_DEFINED >
    ara::core::Result< ByteVector< Alloc > > ProcessBytes(ReadOnlyMemRegion in) noexcept
    {
        ara::core::Result< ara::core::Vector< ara::core::Byte > > result = ProcessBytes(in);
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
    /// @return  has value if Reset sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23627}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02301
    /// @trace_id_dd=DD_CRYPTO_05077
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Reset() noexcept = 0;
    /// @brief Set the position of the next byte in the encryption/decryption gamma stream.
    /// @brief Set the position of the next byte within the stream of the encryption/decryption gamma.
    /// @param offset  the offset value in bytes, relative to begin or current position in the gamma stream
    /// @param fromBegin  the starting point for positioning within the stream: from begin (if @c true) or from
    /// current position (if @c false)
    /// @return  has value if Seek sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnsupported            if the seek operation is not supported by the current mode
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the data processing was not started by a call of the @c
    /// Start() method
    /// @error: SecurityErrorDomain::kBelowBoundary          if the @c offset value is incorrect (in context of the the
    /// @c fromBegin argument),
    ///         i.e. it points before begin of the stream (note: it is an optional error condition)
    /// @error: SecurityErrorDomain::kInvalidArgument        if the offset is not aligned to the required boundary (see
    /// @c IsBytewiseMode())
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23613}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02304}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02302
    /// @trace_id_dd=DD_CRYPTO_05078
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Seek(std::int64_t offset, bool fromBegin = true) noexcept = 0;
    /// @brief Set (deploy) a key to the stream cipher algorithm context.
    /// @brief Set (deploy) a key to the stream chiper algorithm context.
    /// @param key  the source key object
    /// @param transform  the "direction" indicator: deploy the key for direct transformation (if @c true) or
    /// for reverse one (if @c false)
    /// @return
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject  if the provided key object is incompatible with this symmetric
    /// key context
    /// @error: SecurityErrorDomain::kUsageViolation  if the transformation type associated with this context
    ///                (taking into account the direction specified by @c transform) is prohibited by the "allowed
    ///                usage" restrictions of provided key object
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23623}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02303
    /// @trace_id_dd=DD_CRYPTO_05079
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetKey(SymmetricKey const &key,
                                             CryptoTransform transform = CryptoTransform::kEncrypt) noexcept = 0;
    /// @brief
    /// Initialize the context for new data stream processing or generation (depending on the primitive). If the IV size is larger than the maximum size supported by the algorithm, the implementation may use only the leading bytes of the sequence.
    /// @brief Initialize the context for a new data stream processing or generation (depending from the primitive).
    ///       If IV size is greater than maximally supported by the algorithm then an implementation may use the leading
    ///       bytes only from the sequence.
    /// @param iv  an optional Initialization Vector (IV) or "nonce" value
    /// @return has value if Start sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by deploying a key
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the size of provided IV is not supported (i.e. if it is
    /// not enough for the initialization)
    /// @error: SecurityErrorDomain::kUnsupported            if the base algorithm (or its current implementation)
    ///                principally doesn't support the IV variation, but provided IV value is not empty, i.e. if
    ///                <tt>(iv.empty() == false)</tt>
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23625}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}s
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02304
    /// @trace_id_dd=DD_CRYPTO_05080
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Start(ReadOnlyMemRegion const &iv = ReadOnlyMemRegion()) noexcept = 0;
    /// @brief
    /// Initialize the context for new data stream processing or generation (depending on the primitive). If the IV size is larger than the maximum size supported by the algorithm, the implementation may use only the leading bytes of the sequence.
    /// @brief Initialize the context for a new data stream processing or generation (depending from the primitive).
    ///       If IV size is greater than maximally supported by the algorithm then an implementation may use the leading
    ///       bytes only from the sequence.
    /// @param iv  the Initialization Vector (IV) or "nonce" object
    /// @return has value if Start sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by deploying a key
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the size of provided IV is not supported (i.e. if it is
    /// not enough for the initialization)
    /// @error: SecurityErrorDomain::kUnsupported            if the base algorithm (or its current implementation)
    /// principally doesn't support the IV variation
    /// @error: SecurityErrorDomain::kUsageViolation         if this transformation type is prohibited by the "allowed
    /// usage" restrictions of the provided @c SecretSeed object
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23626}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02305
    /// @trace_id_dd=DD_CRYPTO_05081
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Start(SecretSeed const &iv) noexcept = 0;

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_STREAM_CIPHER_CTX_H_
