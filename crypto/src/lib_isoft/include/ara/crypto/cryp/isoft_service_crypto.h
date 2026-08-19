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
/// @file       isoft_service_crypto.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    Extended metadata service for crypto contexts.
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
/// @module_path=/CRYPTO/Default Encryption and Decryption/Service Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PServiceCrypto
/// @unit_description=Extended metadata service for crypto contexts
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_SERVICE_CRYPTO_H_
#define ARA_CRYPTO_CRYP_PUHUA_SERVICE_CRYPTO_H_

#include "ara/crypto/cryp/crypto_service.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Extended metadata service for crypto contexts.
/// @brief Extension meta-information service for cryptographic contexts.
/// @interface PServiceCrypto
/// @AUTOSAR_SWS {SWS_CRYPT_29020}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
//********************************/
/// @brief Crypto service
/// @code{.isoft}
/// @tparam T_CtxCrypto
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00039
/// @trace_id_dd=DD_CRYPTO_00578
/// @needwork = ad
/// @endcode
template < typename T_CtxCrypto >
class PServiceCrypto : public CryptoService
{
private:
    /// @brief Encryption and decryption algorithm context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00579
    /// @needwork = dda
    /// @endcode
    T_CtxCrypto const &ctxCrypto_;

public:
    /// @brief Parameterized constructor
    /// @param ctxCrypto encryption context template class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00040
    /// @trace_id_dd=DD_CRYPTO_00580
    /// @needwork = ad
    /// @endcode
    explicit PServiceCrypto(T_CtxCrypto const &ctxCrypto) noexcept : CryptoService{}, ctxCrypto_{ctxCrypto} {}
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00041
    /// @trace_id_dd=DD_CRYPTO_00581
    /// @needwork = ad
    /// @endcode
    PServiceCrypto() noexcept = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00042
    /// @trace_id_dd=DD_CRYPTO_00582
    /// @needwork = ad
    /// @endcode
    ~PServiceCrypto() noexcept override = default;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00043
    /// @trace_id_dd=DD_CRYPTO_00583
    /// @needwork = ad
    /// @endcode
    PServiceCrypto &operator=(PServiceCrypto const &other) noexcept = delete;
    /// @brief Default copy/move function
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00044
    /// @trace_id_dd=DD_CRYPTO_00584
    /// @needwork = ad
    /// @endcode
    PServiceCrypto &operator=(PServiceCrypto &&other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00045
    /// @trace_id_dd=DD_CRYPTO_00585
    /// @needwork = ad
    /// @endcode
    PServiceCrypto(PServiceCrypto &&other) noexcept = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00046
    /// @trace_id_dd=DD_CRYPTO_00586
    /// @needwork = ad
    /// @endcode
    PServiceCrypto(PServiceCrypto const &other) noexcept = delete;

public:
    /// @brief Unique smart pointer for the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29024}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00039
    /// @trace_id_dd=DD_CRYPTO_06242
    /// @needwork = dd
    /// @endcode
    // PRQA S 2025 QAC /// @qac: false positive
    using Uptr = std::unique_ptr< PServiceCrypto >;
    // PRQA L:QAC

public:  // ExtensionService interface
    /// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
    /// @brief Get actual bit-length of a key loaded to the context. If no key was set to the context yet then 0 is
    /// returned.
    /// @returns actual length of a key (now set to the algorithm context) in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29045}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00047
    /// @trace_id_dd=DD_CRYPTO_00587
    /// @needwork = ad
    /// @endcode
    inline std::size_t GetActualKeyBitLength() const noexcept final { return ctxCrypto_.GetActualKeyBitLength(); }
    /// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, returns an empty COUID (Nil).
    /// @brief Get the COUID of the key deployed to the context this extension service is attached to.
    /// If no key was set to the context yet then an empty COUID (Nil) is returned.
    /// @returns the COUID of the CryptoObject
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29047}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00048
    /// @trace_id_dd=DD_CRYPTO_00588
    /// @needwork = ad
    /// @endcode
    inline CryptoObjectUid GetActualKeyCOUID() const noexcept final
    {
        return std::move(ctxCrypto_.GetActualKeyCOUID());
    }
    /// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
    /// If the context has not been initialized with a key object, must return 0 (all flags reset).
    /// @brief Get allowed usages of this context (according to the key object attributes loaded to this context).
    /// If the context is not initialized by a key object yet then zero (all flags are reset) must be returned.
    /// @returns a combination of bit-flags that specifies allowed usages of the context
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29046}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00049
    /// @trace_id_dd=DD_CRYPTO_00589
    /// @needwork = ad
    /// @endcode
    inline AllowedUsageFlags GetAllowedUsage() const noexcept final { return ctxCrypto_.GetAllowedUsage(); }
    /// @brief Get the maximum supported key length (in bits).
    /// @brief Get maximal supported key length in bits.
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29044}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00050
    /// @trace_id_dd=DD_CRYPTO_00590
    /// @needwork = ad
    /// @endcode
    inline std::size_t GetMaxKeyBitLength() const noexcept final { return ctxCrypto_.GetMaxKeyBitLength(); }
    /// @brief Get the minimum supported key length (in bits).
    /// @brief Get minimal supported key length in bits.
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29043}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00051
    /// @trace_id_dd=DD_CRYPTO_00591
    /// @needwork = ad
    /// @endcode
    inline std::size_t GetMinKeyBitLength() const noexcept final { return ctxCrypto_.GetMinKeyBitLength(); }
    /// @brief Validate support for a specific key length according to the context.
    /// @brief Verify supportness of specific key length by the context.
    /// @param keyBitLength Key length in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29048}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00052
    /// @trace_id_dd=DD_CRYPTO_00592
    /// @needwork = ad
    /// @endcode
    inline bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept final
    {
        return ctxCrypto_.IsKeyBitLengthSupported(keyBitLength);
    }
    /// @brief Check whether a key is set for this context.
    /// @brief Check if a key has been set to this context.
    /// @return true if a key has been set to this context false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29049}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00053
    /// @trace_id_dd=DD_CRYPTO_00593
    /// @needwork = ad
    /// @endcode
    inline bool IsKeyAvailable() const noexcept final { return ctxCrypto_.IsKeyAvailable(); }

public:  // CryptoService interface
    /// @brief Get the block (or internal buffer) size of the underlying algorithm.
    ///         For digest, byte stream cipher and RNG contexts, it is an informational method only used to optimize interface usage.
    /// @brief Get block (or internal buffer) size of the base algorithm.
    ///         For digest, byte-wise stream cipher and RNG contexts it is an informative method, intended only for
    ///         optimization of the interface usage.
    /// @returns size of the block in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29023}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00054
    /// @trace_id_dd=DD_CRYPTO_00594
    /// @needwork = ad
    /// @endcode
    inline std::size_t GetBlockSize() const noexcept final { return ctxCrypto_.GetBlockSize(); }
    /// @brief Get the maximum expected size of the input data block. The suppressPadding parameter will make it equal to the block size.
    /// @brief Get maximum expected size of the input data block. @c suppressPadding argument and it will be equal to
    /// the block size.
    /// @param suppressPadding  if @c true then the method calculates the size for the case when the whole space of
    /// the plain data block is used for the payload only
    /// @returns maximum size of the input data block in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29021}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00055
    /// @trace_id_dd=DD_CRYPTO_00595
    /// @needwork = ad
    /// @endcode
    inline std::size_t GetMaxInputSize(bool suppressPadding) const noexcept final
    {
        return ctxCrypto_.GetMaxInputSize(suppressPadding);
    }
    /// @brief Get the maximum possible size of the output data block.
    ///         If (IsEncryption() == true), the value returned by this method is independent of the suppressPadding parameter and will equal the block size.
    /// @brief Get maximum possible size of the output data block.
    ///         If (IsEncryption() == true) then a value returned by this method is independent from the @c
    ///         suppressPadding argument and will be equal to the block size.
    /// @param suppressPadding  if @c true then the method calculates the size for the case when the whole space of
    /// the plain data block is used for the payload only
    /// @returns maximum size of the output data block in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29022}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00056
    /// @trace_id_dd=DD_CRYPTO_00596
    /// @needwork = ad
    /// @endcode
    inline std::size_t GetMaxOutputSize(bool suppressPadding) const noexcept final
    {
        return ctxCrypto_.GetMaxOutputSize(suppressPadding);
    }
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_SERVICE_CRYPTO_H_
