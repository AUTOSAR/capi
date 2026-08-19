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
/// @file       isoft_ctx_symmetric_block.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Interface for symmetric block cipher context with padding.
/// @date       2022-01-20
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-01-20  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Symmetric Encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01001
/// @unit_name=PCtxSymmetricBlock
/// @unit_description=Symmetric Block Encryption Context Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_BLOCK_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_BLOCK_H_

#include <functional>

#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"
#include "ara/crypto/cryp/symmetric_block_cipher_ctx.h"
#include "ara/crypto/ipc/isoft_ipc_client.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Encryption provider
class PCryptoProvider;
//********************************/
/// @brief Interface for symmetric block cipher context with padding.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00392
/// @trace_id_dd=DD_CRYPTO_01762
/// @needwork = ad
/// @endcode
class PCtxSymmetricBlock : public SymmetricBlockCipherCtx
{
private:
    /// @brief Encryption provider object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01763
    /// @needwork = dda
    /// @endcode
    PCryptoProvider &cryptoProvider_;
    /// @brief Encryption direction: encrypt or decrypt
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01764
    /// @needwork = dda
    /// @endcode
    CryptoTransform eCryptoTransform_;
    /// @brief Symmetric key pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01765
    /// @needwork = dda
    /// @endcode
    SymmetricKey const *pSymmetricKey_;
    /// @brief Parameterized constructor
    /// @param cryptoProvider Encryption provider
    /// @param eCryptoTransform Encryption direction: encrypt or decrypt, etc.
    /// @param pSymmetricKey Symmetric key pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01766
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock(PCryptoProvider &cryptoProvider,
                       CryptoTransform const eCryptoTransform,
                       SymmetricKey const *const pSymmetricKey) noexcept;

public:
    /// @brief Get symmetric key pointer
    /// @return SymmetricKey pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00393
    /// @trace_id_dd=DD_CRYPTO_01767
    /// @needwork = ad
    /// @endcode
    SymmetricKey const *GetSymmetricKey() const noexcept { return pSymmetricKey_; }

protected:
    /// @brief Check if it is encryption direction
    /// @return true if kEncrypt false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00394
    /// @trace_id_dd=DD_CRYPTO_01768
    /// @needwork = ad
    /// @endcode
    bool _IsTransformEnc() const noexcept { return CryptoTransform::kEncrypt == eCryptoTransform_; }
    /// @brief Get encryption direction
    /// @return kEncrypt or kDecrypt
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00395
    /// @trace_id_dd=DD_CRYPTO_01769
    /// @needwork = ad
    /// @endcode
    CryptoTransform _GetCryptoTransform() const noexcept { return eCryptoTransform_; }

public:
    /// @brief the buff data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00396
    /// @trace_id_dd=DD_CRYPTO_01770
    /// @needwork = ad
    /// @endcode
    enum class EBuffLen : std::uint32_t
    {
        /// @brief the max input data length
        kMaxInputBuffLen = 1024,
        /// @brief the max output data length
        kMaxOutputBuffLen = 1024,
    };

public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Encryption provider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00397
    /// @trace_id_dd=DD_CRYPTO_01771
    /// @needwork = ad
    /// @endcode
    explicit PCtxSymmetricBlock(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00398
    /// @trace_id_dd=DD_CRYPTO_01772
    /// @needwork = ad
    /// @endcode
    PCtxSymmetricBlock() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00399
    /// @trace_id_dd=DD_CRYPTO_01773
    /// @needwork = ad
    /// @endcode
    ~PCtxSymmetricBlock() override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00400
    /// @trace_id_dd=DD_CRYPTO_01774
    /// @needwork = ad
    /// @endcode
    PCtxSymmetricBlock(PCtxSymmetricBlock &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00401
    /// @trace_id_dd=DD_CRYPTO_01775
    /// @needwork = ad
    /// @endcode
    PCtxSymmetricBlock(PCtxSymmetricBlock const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00402
    /// @trace_id_dd=DD_CRYPTO_01776
    /// @needwork = ad
    /// @endcode
    PCtxSymmetricBlock &operator=(PCtxSymmetricBlock &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00403
    /// @trace_id_dd=DD_CRYPTO_01777
    /// @needwork = ad
    /// @endcode
    PCtxSymmetricBlock &operator=(PCtxSymmetricBlock const &other) = delete;

public:  // CryptoContext interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00404
    /// @trace_id_dd=DD_CRYPTO_01778
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Checks if the crypto context has been initialized and is ready for use. It checks all required values, including: key values, IV/seed, etc.
    /// @returns true if already init false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00405
    /// @trace_id_dd=DD_CRYPTO_01779
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override;
    /// @brief Gets the reference to the Crypto Provider for this context.
    /// @returns Reference to encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00406
    /// @trace_id_dd=DD_CRYPTO_01780
    /// @needwork = ad
    /// @endcode
    CryptoProvider &MyProvider() const noexcept override;

public:  // SymmetricBlockCipherCtx interface
    /// @brief Gets the CryptoService instance.
    /// @brief Get CryptoService instance.
    /// @returns CryptoService instance
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23702}
    /// @uptrace={RS_CRYPTO_02006}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00407
    /// @trace_id_dd=DD_CRYPTO_01781
    /// @needwork = ad
    /// @endcode
    CryptoService::Uptr GetCryptoService() const noexcept override = 0;
    /// @brief Gets the transformation type configured for this context: kEncrypt or kDecrypt.
    /// @brief Get the kind of transformation configured for this context: kEncrypt or kDecrypt
    /// @returns @c CryptoTransform
    /// @returns  kEncrypt or kDecrypt
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the transformation direction of this context
    ///     is configurable during an initialization, but the context was not initialized yet
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23711}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00408
    /// @trace_id_dd=DD_CRYPTO_01782
    /// @needwork = ad
    /// @endcode
    ara::core::Result< CryptoTransform > GetTransformation() const noexcept override;
    /// @brief Indicates that the currently configured transformation accepts only full input data blocks.
    /// @brief Indicate that the currently configured transformation accepts only complete blocks of input data.
    /// @returns @c true if the transformation requires the maximum size of input data and @c false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the transformation direction of this context
    ///     is configurable during an initialization, but the context was not initialized yet
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23712}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00409
    /// @trace_id_dd=DD_CRYPTO_01783
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > IsMaxInputOnly() const noexcept override;
    /// @brief Indicates that the currently configured transformation can only produce full output data blocks.
    /// @brief Indicate that the currently configured transformation can produce only complete blocks of output data.
    /// @returns @c true if the transformation can produce only the maximum size of output data and @c false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the transformation direction of this context
    ///     is configurable during an initialization, but the context was not initialized yet
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23713}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00410
    /// @trace_id_dd=DD_CRYPTO_01784
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > IsMaxOutputOnly() const noexcept override;
    /// @brief Processes (encrypts/decrypts) an input block according to the cipher configuration.
    /// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
    /// @returns    the output buffer
    /// @param in  the input data block
    /// @param suppressPadding  if @c true then the method doesn't apply the padding, but the payload should fill
    ///     the whole block of the plain data
    /// @code{.isoft}
    /// @error:  SecurityErrorDomain::kIncorrectInputSize   if the mentioned above rules about the input size is
    ///     violated
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the @c out.size() is not enough to store the
    ///     transformation result
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23716}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00411
    /// @trace_id_dd=DD_CRYPTO_01785
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ProcessBlock(ReadOnlyMemRegion const &in,
                                                                           bool suppressPadding
                                                                           = false) const noexcept override;
    /// @brief Uses base class template member functions
    using SymmetricBlockCipherCtx::ProcessBlock;
    /// @brief Process blocks without padding.
    /// Input and output buffers must have the same size, and this size must be divisible by the block size (see GetBlockSize()).
    /// Pointers to input and output buffers must be aligned to block size boundaries!
    /// @brief Processe provided blocks without padding.
    ///         The @c in and @c out buffers @b must have same size and this size @b must be divisible by the block size
    ///         (see @c GetBlockSize()). Pointers to the input and output buffers must be aligned to the block-size
    ///         boundary!
    /// @param in an input data buffer
    /// @returns an output data buffer
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @error: SecurityErrorDomain::kInvalidInputSize       if size of the input buffer is not divisible by the block
    ///     size (see @c GetBlockSize())
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if sizes of the input and output buffer are not equal
    /// @error: SecurityErrorDomain::kInOutBuffersIntersect  if the input and output buffers partially intersect
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23715}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00412
    /// @trace_id_dd=DD_CRYPTO_01786
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ProcessBlocks(
        ReadOnlyMemRegion const &in) const noexcept override;
    /// @brief Clears the crypto context.
    /// @brief Clear the crypto context.
    /// @returns has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23714}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00413
    /// @trace_id_dd=DD_CRYPTO_01787
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Sets (deploys) a key for the symmetric algorithm context.
    /// @brief Set (deploy) a key to the symmetric algorithm context.
    /// @param key  the source key object
    /// @param transform  the "direction" indicator: deploy the key for direct transformation (if @c true) or
    ///     for reverse one (if @c false)
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23710}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    ///     symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context
    ///     (taking into account the direction specified by @c transform) is prohibited by the "allowed usage"
    ///     restrictions of provided key object
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00414
    /// @trace_id_dd=DD_CRYPTO_01788
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetKey(SymmetricKey const &key,
                                     CryptoTransform transform = CryptoTransform::kEncrypt) noexcept override;

public:  // PServiceCrypto interface
    /// @brief Gets the actual bit length of the key loaded into the context. Returns 0 if no key is set for the context.
    /// @returns Length of the key set for the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00415
    /// @trace_id_dd=DD_CRYPTO_01789
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualKeyBitLength() const noexcept;
    /// @brief Gets the COUID of the key deployed to the context attached to this extension service. Returns an empty COUID (Nil) if no key is set for the context.
    /// @returns COUID of the key set for the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00416
    /// @trace_id_dd=DD_CRYPTO_01790
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectUid GetActualKeyCOUID() const noexcept;
    /// @brief Gets the allowed usage of this context (based on the properties of the key object loaded into this context).
    ///         If the context has not been initialized with a key object, it must return 0 (all flags reset).
    /// @returns Allowed usage of the key object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00417
    /// @trace_id_dd=DD_CRYPTO_01791
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetAllowedUsage() const noexcept;
    /// @brief Gets the maximum supported key length in bits.
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00418
    /// @trace_id_dd=DD_CRYPTO_01792
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxKeyBitLength() const noexcept = 0;
    /// @brief Gets the minimum supported key length in bits.
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00419
    /// @trace_id_dd=DD_CRYPTO_01793
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMinKeyBitLength() const noexcept = 0;
    /// @brief Verifies support for a specific key length based on the context.
    /// @param keyBitLength Key length: in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00420
    /// @trace_id_dd=DD_CRYPTO_01794
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept;
    /// @brief Checks if a key is set for this context.
    /// @returns true if a key has been set to this context false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00421
    /// @trace_id_dd=DD_CRYPTO_01795
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyAvailable() const noexcept;
    /// @brief Gets the block (or internal buffer) size of the underlying algorithm.
    ///         For digest, byte-by-byte stream ciphers, and RNG contexts, it is an informational method used only for optimizing interface usage.
    /// @returns Block (or internal buffer) size of the underlying algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00422
    /// @trace_id_dd=DD_CRYPTO_01796
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetBlockSize() const noexcept = 0;
    /// @brief Gets the maximum expected size of the input data block. The suppressPadding parameter will equal the block size.
    /// @param suppressPadding Whether to suppress padding
    /// @returns maximum size of the input data block in bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00423
    /// @trace_id_dd=DD_CRYPTO_01797
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxInputSize(bool suppressPadding = false) const noexcept;
    /// @brief Gets the maximum possible size of the output data block. If (IsEncryption() == true), the value returned by this method is independent of the suppressPadding parameter and will equal the block size.
    /// @param suppressPadding Whether to suppress padding
    /// @returns maximum size of the output data block in bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00424
    /// @trace_id_dd=DD_CRYPTO_01798
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxOutputSize(bool suppressPadding = false) const noexcept;
    /// @brief Local calculation of encryption/decryption, result stored in buffOutput_, returns ciphertext length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00425
    /// @trace_id_dd=DD_CRYPTO_01799
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint32_t >
    /// @brief Local calculation of encryption/decryption, result stored in buffOutput_, returns ciphertext length
    /// @param vecOut Output vector
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param alignedData Whether data is aligned
    /// @return Ciphertext length
    DoCipher(ara::core::Vector< ara::core::Byte > &vecOut,
             uint8_t const *const pInputData,
             uint32_t const nDataLen,
             bool const alignedData = false) const noexcept;

protected:
    /// @brief Check input parameters, return the actual space required for output (0 means check failed)
    /// @param in Input data buffer
    /// @param suppressPadding Whether to suppress padding
    /// @returns Actual space required for output
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01800
    /// @needwork = dda
    /// @endcode
    uint32_t _CheckSize(ReadOnlyMemRegion const &in, bool const suppressPadding) const noexcept;
    /// @brief Calculate encryption/decryption
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param bUsePadding Whether to enable padding
    /// @return Encryption/decryption result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01801
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > _ProcessBlocks(uint8_t const *const pInputData,
                                                                             uint32_t const nDataLen,
                                                                             bool const bUsePadding) const noexcept;
    /// @brief IPC calculation of encryption/decryption, result stored in buffOutput_, returns ciphertext length
    /// @param vecOut Output vector
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @returns Ciphertext length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01802
    /// @needwork = dda
    /// @endcode
    ara::core::Result< uint32_t > _DoCipherIpc(ara::core::Vector< ara::core::Byte > &vecOut,
                                               uint8_t const *const pInputData,
                                               uint32_t const nDataLen) const noexcept;

protected:
    /// @brief Check if Key meets requirements
    /// @param key Symmetric key
    /// @returns true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01803
    /// @needwork = dda
    /// @endcode
    virtual bool CheckKey(SymmetricKey const &key) const noexcept = 0;
    /// @brief Pad input data
    /// @param buffInput Input buffer
    /// @param in Input data buffer
    /// @returns Padded data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01804
    /// @needwork = dda
    /// @endcode
    std::size_t _PaddingInput(internal::PAutoBuff &buffInput, ReadOnlyMemRegion const &in) const noexcept;
    /// @brief Calculate length after padding
    /// @param nInputLen Data length before padding
    /// @param suppressPadding Whether to suppress padding
    /// @returns Length after padding
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01805
    /// @needwork = dda
    /// @endcode
    uint32_t _CalPaddingLen(uint32_t const nInputLen, bool const suppressPadding = false) const noexcept;
    /// @brief Local encryption/decryption functional function
    /// @param pOutput Output
    /// @param pInput Input
    /// @param nInLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01806
    /// @needwork = dda
    /// @endcode
    virtual void DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept = 0;

protected:  // ipc
    /// @brief IPC set key
    /// @param stFuncName Function name used for IPC call
    /// @param key Symmetric key
    /// @param transform Encryption direction: encrypt or decrypt
    /// @return has value if set key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01807
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > _SetKeyIpc(ara::core::StringView const &stFuncName,
                                         SymmetricKey const &key,
                                         CryptoTransform const transform) noexcept;
    /// @brief Return IPC interface function name: SetKey
    /// @returns IPC set key function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01808
    /// @needwork = dda
    /// @endcode
    static ara::core::StringView GetIpcFuncNameSetKey() noexcept;
    /// @brief Return IPC interface: encryption/decryption method
    /// @returns IPC encryption/decryption function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01809
    /// @needwork = dda
    /// @endcode
    virtual ara::core::StringView GetIpcFuncNameDoCipher() const noexcept = 0;

public:
    /// @brief Get key slot id
    /// @returns Key slot id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00426
    /// @trace_id_dd=DD_CRYPTO_01810
    /// @needwork = ad
    /// @endcode
    uint32_t GetSlotId() const noexcept;
    /// @brief This function is a stream encryption method, intended ONLY for key manager template functions, DO NOT call in block encryption.
    /// @param offset Offset
    /// @param fromBegin Whether starting from the beginning position
    /// @returns has value if seek sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00427
    /// @trace_id_dd=DD_CRYPTO_01811
    /// @needwork = ad
    /// @endcode
    static ara::core::Result< void > Seek(std::int64_t const offset, bool const fromBegin = true) noexcept
    {
        std::ignore = offset;
        std::ignore = fromBegin;
        return ara::core::Result< void >::FromValue();
    }
    /// @brief This function is a stream encryption method, intended ONLY for key manager template functions, DO NOT call in block encryption.
    /// @param currentPos Current cursor position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00428
    /// @trace_id_dd=DD_CRYPTO_01812
    /// @needwork = ad
    /// @endcode
    static void SetCurrentPos(uint32_t const currentPos) noexcept { std::ignore = currentPos; }
    /// @brief This function is a stream encryption method, intended ONLY for key manager template functions, DO NOT call in block encryption.
    /// @returns Current encryption/decryption position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00429
    /// @trace_id_dd=DD_CRYPTO_01813
    /// @needwork = ad
    /// @endcode
    static uint32_t GetCurrentPos() noexcept { return 0U; }
    /// @brief This function is a stream encryption method, intended ONLY for key manager template functions, DO NOT call in block encryption.
    /// @param iv Initialization vector
    /// @return  has value if Start sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00430
    /// @trace_id_dd=DD_CRYPTO_01814
    /// @needwork = ad
    /// @endcode
    static ara::core::Result< void > Start(ReadOnlyMemRegion const &iv = ReadOnlyMemRegion()) noexcept
    {
        std::ignore = iv;
        return ara::core::Result< void >::FromValue();
    };
    /// @brief This function is a stream encryption method, intended ONLY for key manager template functions, DO NOT call in block encryption.
    /// @param finishBytes Finish position offset byte count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00431
    /// @trace_id_dd=DD_CRYPTO_01815
    /// @needwork = ad
    /// @endcode
    virtual void SetFinishBytes(uint32_t finishBytes) noexcept { std::ignore = finishBytes; }
    /// @brief This function is a stream encryption method, intended ONLY for key manager template functions, DO NOT call in block encryption.
    /// @return Current finish position offset byte count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00432
    /// @trace_id_dd=DD_CRYPTO_01816
    /// @needwork = ad
    /// @endcode
    virtual uint32_t GetFinishBytes() noexcept { return 0U; }
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_BLOCK_H_
