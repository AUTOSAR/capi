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
/// @file       isoft_ctx_symmetric_stream.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    General stream cipher context interface (it covers all operation modes).
/// @date       2022-02-16
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
/// @module_path=/CRYPTO/Default Encryption and Decryption/Symmetric Encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01002
/// @unit_name=PCtxSymmetricStream
/// @unit_description=Stream symmetric encryption base class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_STREAM_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_STREAM_H_

#include "ara/crypto/cryp/cryobj/secret_seed.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"
#include "ara/crypto/cryp/stream_cipher_ctx.h"
#include "ara/crypto/ipc/isoft_ipc_client.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Encryption provider
class PCryptoProvider;
//********************************/
/// @brief General stream cipher context interface (it covers all operation modes).
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00337
/// @trace_id_dd=DD_CRYPTO_01683
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream : public StreamCipherCtx
{
    /// @brief In stream encryption schemes, the smallest unit of "stream" can be a byte or an entire block.
public:
    /// @brief Stream encryption working state enumeration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00338
    /// @trace_id_dd=DD_CRYPTO_01684
    /// @needwork = ad
    /// @endcode
    enum class EStreamWorkState : std::uint8_t
    {
        /// @brief Not started
        kNotStarted = 0,
        /// @brief Started
        kStart = 0x01,
        /// @brief Working
        kUpdate = 0x02,
        /// @brief Completed
        kFinish = 0x0F,
    };
    /// @brief the buff data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00339
    /// @trace_id_dd=DD_CRYPTO_01685
    /// @needwork = ad
    /// @endcode
    enum class EBuffLen : std::uint32_t
    {
        /// @brief the max cache data length
        kMaxCacheBuffLen = 64U,
        /// @brief the max output data length
        kMaxOutputBuffLen = 1024U,
    };

private:
    /// @brief Encryption provider object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01686
    /// @needwork = dda
    /// @endcode
    PCryptoProvider &cryptoProvider_;
    /// @brief Working state //
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01687
    /// @needwork = dda
    /// @endcode
    EStreamWorkState eWorkState_;
    /// @brief Encryption direction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01688
    /// @needwork = dda
    /// @endcode
    CryptoTransform eCryptoTransform_;
    /// @brief Symmetric key pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01689
    /// @needwork = dda
    /// @endcode
    SymmetricKey const *pSymmetricKey_;
    /// @brief If the IV is initialized with a secretSeed, record its COUID for use in GetActualIvBitLength calls
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01690
    /// @needwork = dda
    /// @endcode
    CryptoObjectUid secretSeedCouId_;
    /// @brief Output buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01691
    /// @needwork = dda
    /// @endcode
    internal::PAutoBuff buffOutput_;
    /// @brief Count in cache
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01692
    /// @needwork = dda
    /// @endcode
    uint32_t nCountInCache_;
    /// @brief Cache buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01693
    /// @needwork = dda
    /// @endcode
    internal::PAutoBuff buffCache_;
    /// @brief Indicates after processing of non-aligned (i.e., ProcessBytes has been called)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01694
    /// @needwork = dda
    /// @endcode
    bool isAfterPreocessNonAligned_;
    /// @brief Parameterized constructor
    /// @param cryptoProvider encryption provider
    /// @param eWorkState working state
    /// @param eCryptoTransform encryption direction: encryption or decryption, etc.
    /// @param pSymmetricKey symmetric key pointer
    /// @param secretSeedCouId secret seed cryptographic material ID
    /// @param buffOutput output buffer
    /// @param nCountInCache count in cache
    /// @param buffCache cache buffer
    /// @param isAfterPreocessNonAligned whether ProcessBytes has been called
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01695
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricStream(PCryptoProvider &cryptoProvider,
                        EStreamWorkState const eWorkState,
                        CryptoTransform const eCryptoTransform,
                        SymmetricKey const *const pSymmetricKey,
                        CryptoObjectUid const &secretSeedCouId,
                        internal::PAutoBuff const *const buffOutput,
                        uint32_t const nCountInCache,
                        internal::PAutoBuff const *const buffCache,
                        bool const isAfterPreocessNonAligned) noexcept;

public:
    /// @brief Get SymmetricKey
    /// @return SymmetricKey pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00340
    /// @trace_id_dd=DD_CRYPTO_01696
    /// @needwork = ad
    /// @endcode
    SymmetricKey const *GetSymmetricKey() const noexcept { return pSymmetricKey_; }

protected:
    /// @brief Is Transform Encrypto
    /// @return true if kEncrypt false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00341
    /// @trace_id_dd=DD_CRYPTO_01697
    /// @needwork = ad
    /// @endcode
    bool _IsTransformEnc() const noexcept { return CryptoTransform::kEncrypt == eCryptoTransform_; }
    /// @brief Get Crypto Transform
    /// @return kEncrypt or kDecrypt
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00342
    /// @trace_id_dd=DD_CRYPTO_01698
    /// @needwork = ad
    /// @endcode
    CryptoTransform _GetCryptoTransform() const noexcept { return eCryptoTransform_; }
    /// @brief Set CryptoObject Uid
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00343
    /// @trace_id_dd=DD_CRYPTO_01699
    /// @needwork = ad
    /// @endcode
    void _SetCryptoObjectUid(CryptoObjectUid const &other) noexcept { secretSeedCouId_ = other; }
    /// @brief Get CryptoObject Uid
    /// @return CryptoObject Uid
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00344
    /// @trace_id_dd=DD_CRYPTO_01700
    /// @needwork = ad
    /// @endcode
    CryptoObjectUid _GetCryptoObjectUid() const noexcept { return secretSeedCouId_; }

public:
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00344
    /// @trace_id_dd=DD_CRYPTO_06299
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PCtxSymmetricStream >;

public:
    /// @brief Parameterized constructor
    /// @name   PCtxSymmetricStream
    /// @param cryptoProvider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00345
    /// @trace_id_dd=DD_CRYPTO_01701
    /// @needwork = ad
    /// @endcode
    explicit PCtxSymmetricStream(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00346
    /// @trace_id_dd=DD_CRYPTO_01702
    /// @needwork = ad
    /// @endcode
    PCtxSymmetricStream() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00347
    /// @trace_id_dd=DD_CRYPTO_01703
    /// @needwork = ad
    /// @endcode
    ~PCtxSymmetricStream() override = default;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00348
    /// @trace_id_dd=DD_CRYPTO_01704
    /// @needwork = ad
    /// @endcode
    PCtxSymmetricStream(PCtxSymmetricStream &&other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00349
    /// @trace_id_dd=DD_CRYPTO_01705
    /// @needwork = ad
    /// @endcode
    PCtxSymmetricStream &operator=(PCtxSymmetricStream &&other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00350
    /// @trace_id_dd=DD_CRYPTO_01706
    /// @needwork = ad
    /// @endcode
    PCtxSymmetricStream &operator=(PCtxSymmetricStream const &other) = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00351
    /// @trace_id_dd=DD_CRYPTO_01707
    /// @needwork = ad
    /// @endcode
    PCtxSymmetricStream(PCtxSymmetricStream const &other) = delete;

public:  // CryptoContext interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name  GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00352
    /// @trace_id_dd=DD_CRYPTO_01708
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Check whether the encryption context has been initialized and is usable. It checks all required values, including: key value, IV/seed, etc.
    /// @name  IsInitialized
    /// @returns true if already init false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00353
    /// @trace_id_dd=DD_CRYPTO_01709
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override;
    /// @brief Get a reference to the Crypto Provider for this context.
    /// @name  MyProvider
    /// @returns reference to the encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00354
    /// @trace_id_dd=DD_CRYPTO_01710
    /// @needwork = ad
    /// @endcode
    CryptoProvider &MyProvider() const noexcept override;

public:  // StreamCipherCtx interface
    /// @brief Count the number of bytes currently held in the context cache.
    ///        In block mode, if the application provides the last incomplete input data block, the context saves the remainder of the last (incomplete) block into the internal "cache" memory and waits for the next call with additional input to complete the block.
    /// @name  CountBytesInCache
    /// @returns number of bytes in the context cache
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00355
    /// @trace_id_dd=DD_CRYPTO_01711
    /// @needwork = ad
    /// @endcode
    inline std::size_t CountBytesInCache() const noexcept override
    {
        return static_cast< std::size_t >(nCountInCache_);
    }
    /// @brief Process the final part of the message (may not be aligned to block size boundary).
    ///         If (IsBytewiseMode() == false) it must be: bs = GetBlockSize(), out.size() >= (((in.size() + bs
    ///         *((CryptoTransform::kEncrypt == GetTransformation().Value()) ? 2 : 1) - 1) / bs) * bs)
    ///         if(IsBytewiseMode() == true)   it must be: out.size() >= in.size() Input and output buffers must not intersect!
    ///         When processing the last data block in block mode, this method must be used! This method can be used to process the entire message in a single call (in any mode)!
    /// @name   FinishBytes
    /// @param in input data buffer
    /// @returns ciphertext data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00356
    /// @trace_id_dd=DD_CRYPTO_01712
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > FinishBytes(
        ReadOnlyMemRegion const &in) noexcept override;
    /// @brief Use base class template member function
    using StreamCipherCtx::FinishBytes;
    /// @brief Get the BlockService instance.
    /// @name  GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00357
    /// @trace_id_dd=DD_CRYPTO_01713
    /// @needwork = ad
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override = 0;
    /// @brief Check the operation mode for bytewise properties.
    /// @name  IsBytewiseMode
    /// @returns true if byte mode false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00358
    /// @trace_id_dd=DD_CRYPTO_01714
    /// @needwork = ad
    /// @endcode
    bool IsBytewiseMode() const noexcept override;
    /// @brief Get the transformation type configured for this context: kEncrypt or kDecrypt.
    /// @name  GetTransformation
    /// @returns kEncrypt or kDecrypt
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00359
    /// @trace_id_dd=DD_CRYPTO_01715
    /// @needwork = ad
    /// @endcode
    ara::core::Result< CryptoTransform > GetTransformation() const noexcept override;
    /// @brief Check whether seek operation is supported in the current mode.
    /// @name  IsSeekableMode
    /// @returns true if seekable false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00360
    /// @trace_id_dd=DD_CRYPTO_01716
    /// @needwork = ad
    /// @endcode
    bool IsSeekableMode() const noexcept override;
    /// @brief Process the initial part of the message aligned to block size boundary.
    ///        This is a copy-optimized method that does not use internal cache buffers! It can only be used before processing any non-aligned block size boundary data.
    ///        Pointers to input and output buffers must be aligned to block size boundary! Input and output buffers can completely overlap, but they must not partially intersect!
    /// @name  ProcessBlocks
    /// @param in input data buffer
    /// @returns ciphertext data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00361
    /// @trace_id_dd=DD_CRYPTO_01717
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ProcessBlocks(
        ReadOnlyMemRegion const &in) noexcept override;
    /// @brief Process the initial part of the message aligned to block size boundary.
    ///        This is a copy-optimized method that does not use internal cache buffers! It can be used for the first non-block-aligned data processing. Pointers to input and output buffers must be aligned to block size boundary!
    /// @name  ProcessBlocks
    /// @param inOut  the input data and output data
    /// @returns has value if ProcessBlocks sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00362
    /// @trace_id_dd=DD_CRYPTO_01718
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > ProcessBlocks(ReadWriteMemRegion &inOut) noexcept override;
    /// @brief Process a non-final part of the message (not aligned to block size boundary).
    ///         If (IsBytewiseMode() == false) then it must be: bs = GetBlockSize(), out.size() >= (((in.size() + bs -
    ///         1) / bs) * bs) If (IsBytewiseMode() == true) then it must be: out.size() >= in.size()
    ///         Input and output buffers must not intersect! This method is "copy-ineffective", so it should only be used when the application cannot control the chunking of the original message!
    /// @name   ProcessBytes
    /// @param in input data buffer
    /// @returns ciphertext data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00363
    /// @trace_id_dd=DD_CRYPTO_01719
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ProcessBytes(
        ReadOnlyMemRegion const &in) noexcept override;
    /// @brief Use base class template member function
    using StreamCipherCtx::ProcessBytes;
    /// @brief Clear the encryption context.
    /// @name  Reset
    /// @returns  has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00364
    /// @trace_id_dd=DD_CRYPTO_01720
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Set the position of the next byte in the encryption/decryption gamma stream.
    /// @name   Seek
    /// @param offset offset
    /// @param fromBegin whether to start from the Begin position
    /// @returns has value if Seek sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00365
    /// @trace_id_dd=DD_CRYPTO_01721
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Seek(std::int64_t offset, bool fromBegin = true) noexcept override;
    /// @brief Set (deploy) a key to the stream cipher algorithm context.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00366
    /// @trace_id_dd=DD_CRYPTO_01722
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void >
    /// @brief Set (deploy) a key to the stream chiper algorithm context.
    /// @param key  the source key object
    /// @param transform  the "direction" indicator: deploy the key for direct transformation (if @c true) or
    ///     for reverse one (if @c false)
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23623}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncompatibleObject  if the provided key object is incompatible with this symmetric
    ///     key context
    /// @error: SecurityErrorDomain::kUsageViolation  if the transformation type associated with this context
    ///                (taking into account the direction specified by @c transform) is prohibited by the "allowed
    ///                usage" restrictions of provided key object
    /// @endcode
    SetKey(SymmetricKey const &key, CryptoTransform transform = CryptoTransform::kEncrypt) noexcept override;
    /// @brief Initialize the context for new data stream processing or generation (depending on the primitive).
    /// If the IV size is larger than the maximum supported by the algorithm, the implementation may only use the leading bytes of the sequence.
    /// @brief Initialize the context for a new data stream processing or generation (depending from the primitive).
    ///       If IV size is greater than maximally supported by the algorithm then an implementation may use the leading
    ///       bytes only from the sequence.
    /// @param iv  an optional Initialization Vector (IV) or "nonce" value
    /// @return has value if Start sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23625}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by deploying a key
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the size of provided IV is not supported (i.e. if it is
    ///     not enough for the initialization)
    /// @error: SecurityErrorDomain::kUnsupported  if the base algorithm (or its current implementation) principally
    ///     doesn't support the IV variation, but provided IV value is not empty, i.e. if <tt>(iv.empty() == false)</tt>
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00367
    /// @trace_id_dd=DD_CRYPTO_01723
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Start(ReadOnlyMemRegion const &iv = ReadOnlyMemRegion()) noexcept override;
    /// @brief Initialize the context for new data stream processing or generation (depending on the primitive).
    ///         If the IV size is larger than the maximum supported by the algorithm, the implementation may only use the leading bytes of the sequence.
    /// @name   Start
    /// @param iv an optional Initialization Vector (IV) or "nonce" value
    /// @returns has value if Start sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00368
    /// @trace_id_dd=DD_CRYPTO_01724
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Start(SecretSeed const &iv) noexcept override;

public:  // PServiceStream interface
    /// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, return 0.
    /// @name  GetActualKeyBitLength
    /// @returns length of the key set for the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00369
    /// @trace_id_dd=DD_CRYPTO_01725
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualKeyBitLength() const noexcept;
    /// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, return an empty COUID (Nil).
    /// @name  GetActualKeyCOUID
    /// @returns COUID of the key set for the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00370
    /// @trace_id_dd=DD_CRYPTO_01726
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectUid GetActualKeyCOUID() const noexcept;
    /// @brief Get the allowed usage of this context (according to the key object attributes loaded into this context).
    ///         If the context has not been initialized with a key object, 0 must be returned (all flags reset).
    /// @name  GetAllowedUsage
    /// @returns allowed usage of the key object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00371
    /// @trace_id_dd=DD_CRYPTO_01727
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetAllowedUsage() const noexcept;
    /// @brief Get the maximum supported key length (in bits).
    /// @name  GetMaxKeyBitLength
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00372
    /// @trace_id_dd=DD_CRYPTO_01728
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxKeyBitLength() const noexcept = 0;
    /// @brief Get the minimum supported key length (in bits).
    /// @name  GetMinKeyBitLength
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00373
    /// @trace_id_dd=DD_CRYPTO_01729
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMinKeyBitLength() const noexcept = 0;
    /// @brief Verify support for a specific key length according to the context.
    /// @name  IsKeyBitLengthSupported
    /// @param keyBitLength key length in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00374
    /// @trace_id_dd=DD_CRYPTO_01730
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept;
    /// @brief Check if a key is set for this context.
    /// @name  IsKeyAvailable
    /// @returns  true if a key has been set to this context false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00375
    /// @trace_id_dd=DD_CRYPTO_01731
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyAvailable() const noexcept;
    /// @brief Get the block (or internal buffer) size of the underlying algorithm.
    /// @name  GetBlockSize
    /// @returns block (or internal buffer) size of the underlying algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00376
    /// @trace_id_dd=DD_CRYPTO_01732
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetBlockSize() const noexcept;
    /// @brief Get the actual bit length of the IV loaded into the context.
    /// @name  GetActualIvBitLength
    /// @param ivUid UID of the initialization vector (IV)
    /// @returns actual bit length of the IV of the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00377
    /// @trace_id_dd=DD_CRYPTO_01733
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualIvBitLength(ara::core::Optional< CryptoObjectUid > &ivUid) const noexcept;
    /// @brief Get the expected default size of the initialization vector (IV) or nonce.
    /// @brief Get default expected size of the Initialization Vector (IV) or nonce.
    /// @returns default expected size of IV in bytes
    /// @name   GetIvSize
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29032}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00378
    /// @trace_id_dd=DD_CRYPTO_01734
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetIvSize() const noexcept;
    /// @brief Verify the validity of a specific initialization vector (IV) length.
    /// @brief Verify validity of specific Initialization Vector (IV) length.
    /// @name  IsValidIvSize
    /// @param ivSize  the length of the IV in bytes
    /// @returns @c true if provided IV length is supported by the algorithm and @c false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29034}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00379
    /// @trace_id_dd=DD_CRYPTO_01735
    /// @needwork = ad
    /// @endcode
    virtual bool IsValidIvSize(std::size_t ivSize) const noexcept;

public:  // PCtxSymmetricStream interface
    /// @brief Check the current state of stream processing: whether it has been started.
    /// @name  IsStarted
    /// @returns true if has already started false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00380
    /// @trace_id_dd=DD_CRYPTO_01736
    /// @needwork = ad
    /// @endcode
    virtual bool IsStarted() const noexcept;
    /// @brief Check the current state of stream processing: whether it is completed.
    /// @name  IsFinished
    /// @returns true if has already finished false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00381
    /// @trace_id_dd=DD_CRYPTO_01737
    /// @needwork = ad
    /// @endcode
    virtual bool IsFinished() const noexcept;
    /// @brief Compute encryption/decryption, the result is stored in buffOutput_, the return value is the ciphertext length. Overload here for convenience in key manager.
    /// @name   DoCipher
    /// @param vecOut output result vector
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nDataLen data length
    /// @param alignedData whether data is aligned
    /// @returns ciphertext length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00382
    /// @trace_id_dd=DD_CRYPTO_01738
    /// @needwork = ad
    /// @endcode
    uint32_t DoCipher(ara::core::Vector< ara::core::Byte > &vecOut,
                      uint8_t const *const pInputData,
                      uint32_t const nDataLen,
                      bool const alignedData = false) noexcept
    {
        internal::PAutoBuff buffOutput{static_cast< uint32_t >(vecOut.size())};
        std::ignore = DoCipherLocal(buffOutput, pInputData, nDataLen, alignedData);
        vecOut.clear();
        for (size_t i{0U}; i < nDataLen; i++) {
            vecOut.push_back(static_cast< ara::core::Byte >(*buffOutput.Data(static_cast< uint32_t >(i))));
        }
        return nDataLen;
    }

protected:  // PCtxSymmetricStream interface
    /// @brief Perform Seek operation
    /// @name   DoSeek
    /// @param offset offset
    /// @param fromBegin whether to start from the Begin position
    /// @returns has value if DoSeek sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01739
    /// @needwork = dda
    /// @endcode
    virtual ara::core::Result< void > DoSeek(std::int64_t offset, bool fromBegin = true) noexcept;
    /// @brief Check whether the key meets the requirements
    /// @name   CheckKey
    /// @param key symmetric key
    /// @returns  true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01740
    /// @needwork = dda
    /// @endcode
    virtual bool CheckKey(SymmetricKey const &key) const noexcept = 0;
    /// @brief Perform initialization operation: using memory
    /// @name   DoInit
    /// @param iv initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01741
    /// @needwork = dda
    /// @endcode
    virtual void DoInit(ReadOnlyMemRegion const &iv) noexcept = 0;
    /// @brief Perform initialization operation: using secret seed
    /// @param iv initialization secret seed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01742
    /// @needwork = dda
    /// @endcode
    virtual void DoInit(SecretSeed const &iv) noexcept = 0;
    /// @brief Compute encryption/decryption, the result is stored in buffOutput_, the return value is the ciphertext length
    /// @name   DoCipherLocal
    /// @param buffOutput output buffer
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nDataLen data length
    /// @param alignedData whether data is aligned
    /// @returns ciphertext length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01743
    /// @needwork = dda
    /// @endcode
    virtual uint32_t DoCipherLocal(internal::PAutoBuff &buffOutput,
                                   uint8_t const *pInputData,
                                   uint32_t nDataLen,
                                   bool alignedData = false) noexcept = 0;

protected:
    /// @brief Save to cache, return the actual number saved
    /// @name  _SaveToCache
    /// @param pData data: starting memory address
    /// @param nLen data length in bytes
    /// @returns actual number saved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01744
    /// @needwork = dda
    /// @endcode
    int32_t _SaveToCache(uint8_t const *const pData, int32_t const nLen) noexcept;
    /// @brief Clear cache
    /// @name _ClearCache
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01745
    /// @needwork = dda
    /// @endcode
    void _ClearCache() noexcept;
    /// @brief Compute encryption/decryption
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nDataLen data length
    /// @return ciphertext data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01746
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::Byte > _ProcessBlocks(uint8_t const *const pInputData,
                                                        uint32_t const nDataLen) noexcept;

protected:  // ipc
    /// @brief IPC encryption/decryption operation
    /// @param stFuncName function name used for IPC call
    /// @param buffOutput output buffer
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nDataLen data length
    /// @param pIvData initialization data
    /// @param nIvLen initialization data length
    /// @param alignedData whether data is aligned
    /// @return ciphertext data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01747
    /// @needwork = dda
    /// @endcode
    uint32_t _DoCipherIpc(ara::core::StringView const &stFuncName,
                          internal::PAutoBuff &buffOutput,
                          uint8_t const *const pInputData,
                          uint32_t const nDataLen,
                          uint8_t const *const pIvData = nullptr,
                          uint32_t const nIvLen        = 0U,
                          bool const alignedData       = false) noexcept;

    /// @brief IPC setkey operation
    /// @name   _SetKeyIpc
    /// @param stFuncName function name used for IPC call
    /// @param key symmetric key
    /// @param transform encryption direction: encrypt or decrypt
    /// @return  has value if set key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01748
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > _SetKeyIpc(ara::core::StringView const &stFuncName,
                                         SymmetricKey const &key,
                                         CryptoTransform const transform) noexcept;
    /// @brief Return the IPC interface: SetKey
    /// @name  GetIpcFuncNameSetKey
    /// @returns IPC setkey function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01749
    /// @needwork = dda
    /// @endcode
    static ara::core::StringView GetIpcFuncNameSetKey() noexcept;
    /// @brief Get the IPC function name corresponding to encryption/decryption
    /// @name    GetIpcFuncNameDoCipher
    /// @returns IPC encryption/decryption function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01750
    /// @needwork = dda
    /// @endcode
    virtual ara::core::StringView GetIpcFuncNameDoCipher() const noexcept = 0;
    /// @brief Padding Input
    /// @param buffInput input buffer
    /// @return padding data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01751
    /// @needwork = dda
    /// @endcode
    std::size_t _PaddingInput(internal::PAutoBuff &buffInput) const noexcept;
    /// @brief calculate Padding Len
    /// @param nInputLen data length before padding
    /// @return length after padding
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01752
    /// @needwork = dda
    /// @endcode
    uint32_t _CalPaddingLen(uint32_t const nInputLen) const noexcept;

public:
    /// @brief Get the key slot ID
    /// @returns key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00383
    /// @trace_id_dd=DD_CRYPTO_01753
    /// @needwork = ad
    /// @endcode
    virtual uint32_t GetSlotId() noexcept;
    /// @brief Get the offset
    /// @returns offset
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00384
    /// @trace_id_dd=DD_CRYPTO_01754
    /// @needwork = ad
    /// @endcode
    virtual int64_t GetOffset() const noexcept { return 0; }
    /// @brief Set the current cursor position
    /// @returns current cursor position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00385
    /// @trace_id_dd=DD_CRYPTO_01755
    /// @needwork = ad
    /// @endcode
    virtual uint32_t GetCurrentPos() const noexcept { return 0U; }
    /// @brief Get the current position
    /// @param currentPos current cursor position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00386
    /// @trace_id_dd=DD_CRYPTO_01756
    /// @needwork = ad
    /// @endcode
    virtual void SetCurrentPos(uint32_t currentPos) noexcept { std::ignore = currentPos; }
    /// @brief Set offset
    /// @param offset offset
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00387
    /// @trace_id_dd=DD_CRYPTO_01757
    /// @needwork = ad
    /// @endcode
    virtual void SetOffset(int64_t offset) noexcept { std::ignore = offset; }
    /// @brief Set offset from the beginning position
    /// @param fromBegin whether to start from the Begin position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00388
    /// @trace_id_dd=DD_CRYPTO_01758
    /// @needwork = ad
    /// @endcode
    virtual void SetFromBegin(bool fromBegin) noexcept { std::ignore = fromBegin; }
    /// @brief Set the offset bytes from the end position
    /// @param finishBytes offset bytes from the end position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00389
    /// @trace_id_dd=DD_CRYPTO_01759
    /// @needwork = ad
    /// @endcode
    virtual void SetFinishBytes(uint32_t finishBytes) noexcept { std::ignore = finishBytes; }
    /// @brief Get whether offset from the beginning position
    /// @name   GetFromBegin
    /// @returns true if from begin false otherwie
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00390
    /// @trace_id_dd=DD_CRYPTO_01760
    /// @needwork = ad
    /// @endcode
    virtual bool GetFromBegin() const noexcept { return true; }
    /// @brief Get the offset bytes from the end position
    /// @name   GetFinishBytes
    /// @returns offset bytes from the end position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00391
    /// @trace_id_dd=DD_CRYPTO_01761
    /// @needwork = ad
    /// @endcode
    virtual uint32_t GetFinishBytes() const noexcept { return 0U; }
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_STREAM_H_
