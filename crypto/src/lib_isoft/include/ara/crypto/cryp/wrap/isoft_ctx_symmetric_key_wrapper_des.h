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
/// @file       isoft_ctx_symmetric_key_wrapper_des.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Symmetric key wrapping based on DES.
/// @date       2023-07-04
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr>}<2023-07-04  </td>1.0.0    </td>Chang Zheng     </td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Key wrapping
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01019
/// @unit_name=PCtxSymmetricKeyWrapperDes
/// @unit_description=DES-based key wrapping and unwrapping context
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_KEY_WRAPPER_DES_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_KEY_WRAPPER_DES_H_

#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_des.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_block_des.h"
#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00560
/// @trace_id_dd=DD_CRYPTO_02061
/// @needwork = dd
/// @endcode
int32_t const kTargetKeyGranularity{8};
/// @brief Symmetric key wrapping based on DES.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00560
/// @trace_id_dd=DD_CRYPTO_02062
/// @needwork = ad
/// @endcode
class PCtxSymmetricKeyWrapperDes : public PCtxSymmetricKeyWrapper_Base
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02063
    /// @needwork = dda
    /// @endcode
    explicit PCtxSymmetricKeyWrapperDes(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02064
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricKeyWrapperDes() = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02065
    /// @needwork = dda
    /// @endcode
    ~PCtxSymmetricKeyWrapperDes() noexcept override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02066
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricKeyWrapperDes(PCtxSymmetricKeyWrapperDes &&other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02067
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricKeyWrapperDes &operator=(PCtxSymmetricKeyWrapperDes &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02068
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricKeyWrapperDes &operator=(PCtxSymmetricKeyWrapperDes const &other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02069
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricKeyWrapperDes(PCtxSymmetricKeyWrapperDes const &other) = delete;

private:
    /// @brief Symmetric block encryption unique pointer: 3Des_Cbc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02070
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Cbc::Uptr pSymmetricBlockDes_;

public:
    /// @brief Calculates the size (in bytes) of the wrapped key from the original key length (bits). This method is useful for implementations differing from RFC3394 / RFC5649.
    /// @brief Calculate size of the wrapped key in bytes from original key length in bits.
    ///       This method can be useful for some implementations different from RFC3394 / RFC5649.
    /// @param keyLength  original key length in bits
    /// @returns size of the wrapped key in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24013}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02071
    /// @needwork = dda
    /// @endcode
    std::size_t CalculateWrappedKeySize(std::size_t keyLength) const noexcept override;
    /// @brief Gets the maximum length of the target key supported by the implementation. This method is useful for implementations differing from RFC3394 / RFC5649.
    /// @brief Get maximum length of the target key supported by the implementation.
    ///       This method can be useful for some implementations different from RFC3394 / RFC5649.
    /// @returns maximum length of the target key in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24012}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02072
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxTarGetKeyLength() const noexcept override;
    /// @brief Sets (deploys) a key to the symmetric key wrapper algorithm context.
    /// @brief Set (deploy) a key to the symmetric key wrapper algorithm context.
    /// @param key  the source key object
    /// @param transform  the "direction" indicator: deploy the key for direct transformation (if @c true) or
    /// for reverse one (if @c false)
    /// @return has value if setkey sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24018}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context
    ///                (taking into account the direction specified by @c transform) is prohibited by the "allowed
    ///                usage" restrictions of provided key object
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02073
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > SetKey(SymmetricKey const &key, CryptoTransform transform) noexcept override;
    /// @brief Clears the crypto context.
    /// @brief Clear the crypto context.
    /// @return has vlaue if Reset sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24019}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02074
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Gets the expected granularity (block size) of the target key.
    ///         If the class implements RFC3394 (KW without padding), this method should return 8 (i.e., 8 bytes = 64 bits).
    ///         If the class implements RFC5649 (KW with padding), this method should return 1 (i.e., 1 octet = 8 bits).
    /// @brief Get expected granularity of the target key (block size).
    ///       If the class implements RFC3394 (KW without padding) then this method should return 8 (i.e. 8 octets = 64
    ///       bits). If the class implements RFC5649 (KW with padding) then this method should return 1 (i.e. 1 octet =
    ///       8 bits).
    /// @returns size of the block in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24011}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02075
    /// @needwork = dda
    /// @endcode
    std::size_t GetTargetKeyGranularity() const noexcept override;
    /// @brief Gets the ExtensionService instance.
    /// @brief Get ExtensionService instance.
    /// @return ExtensionService instance
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24002}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02076
    /// @needwork = dda
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override;

public:  // PServiceExtension interface
    /// @brief Gets the actual bit length of the key loaded into the context. Returns 0 if no key is set for the context.
    /// @brief Get actual bit-length of a key loaded to the context. If no key was set to the context yet then 0 is
    /// returned.
    /// @returns actual length of a key (now set to the algorithm context) in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29045}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02077
    /// @needwork = dda
    /// @endcode
    std::size_t GetActualKeyBitLength() const noexcept override;
    /// @brief Gets the COUID of the key deployed to the context attached to this extension service. Returns an empty COUID (Nil) if no key is set for the context.
    /// @brief Get the COUID of the key deployed to the context this extension service is attached to.
    /// If no key was set to the context yet then an empty COUID (Nil) is returned.
    /// @returns the COUID of the CryptoObject
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29047}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02078
    /// @needwork = dda
    /// @endcode
    CryptoObjectUid GetActualKeyCOUID() const noexcept override;
    /// @brief Gets the allowed usage of this context (based on the properties of the key object loaded into this context).
    /// If the context has not been initialized with a key object, it must return 0 (all flags reset).
    /// @brief Get allowed usages of this context (according to the key object attributes loaded to this context).
    /// If the context is not initialized by a key object yet then zero (all flags are reset) must be returned.
    /// @returns a combination of bit-flags that specifies allowed usages of the context
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29046}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02079
    /// @needwork = dda
    /// @endcode
    AllowedUsageFlags GetAllowedUsage() const noexcept override;
    /// @brief Gets the maximum supported key length in bits.
    /// @brief Get maximal supported key length in bits.
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29044}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02080
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Gets the minimum supported key length in bits.
    /// @brief Get minimal supported key length in bits.
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29043}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02081
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Verifies support for a specific key length based on the context.
    /// @brief Verify supportness of specific key length by the context.
    /// @param keyBitLength Key length: in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29048}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02082
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;
    /// @brief Checks if a key is set for this context.
    /// @brief Check if a key has been set to this context.
    /// @return true if a key has been set to this context false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29049}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02083
    /// @needwork = dda
    /// @endcode
    bool IsKeyAvailable() const noexcept override;

public:
    /// @brief Gets the crypto primitive ID
    /// @return Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02084
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Executes wrap logic
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @return Wrapped data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02085
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > DoWrap(uint8_t const *pInputData,
                                                                     uint32_t nDataLen) const noexcept override;
    /// @brief Executes local unwrap logic
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @return Unwrapped data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02086
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > DoUnWrap(uint8_t const *pInputData,
                                                                       uint32_t nDataLen) const noexcept override;

protected:
    /// @brief Performs the "close key wrapping" operation on the provided key material.
    /// @param key Key material
    /// @return Wrapped data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02087
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > DoWrapKeyMaterial(
        RestrictedUseObject const &key) const noexcept override;
    /// @brief Performs the "key unwrap" operation on the provided BLOB and generates a SecretSeed object.
    /// @param wrappedSeed Wrapped seed
    /// @return Unwrapped data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02088
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > DoUnwrap(
        ReadOnlyMemRegion const &wrappedSeed) const noexcept override;
    /// @brief Performs the "key unwrap" operation on the provided BLOB and generates a SecretSeed object.
    /// @param stFuncName Function name used for IPC call
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param nKeySlotId Key slot ID
    /// @return Wrap or unwrap result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02089
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > _DoWrapAndUnWrapIpc(
        ara::core::StringView const &stFuncName,
        uint8_t const *const pInputData,
        uint32_t const nDataLen,
        uint32_t const nKeySlotId = 0U) const noexcept;
    /// @brief Gets the first 3DES key
    /// @return DES_key_schedule pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02090
    /// @needwork = dda
    /// @endcode
    void const *_GetDesKeySchedule1() const noexcept
    {
        if (pSymmetricBlockDes_.get() == nullptr) {
            return nullptr;
        }
        return pSymmetricBlockDes_->GetPDesKeySchedule1();
    }
    /// @brief Gets the second 3DES key
    /// @return DES_key_schedule pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02091
    /// @needwork = dda
    /// @endcode
    void const *_GetDesKeySchedule2() const noexcept
    {
        if (pSymmetricBlockDes_.get() == nullptr) {
            return nullptr;
        }
        return pSymmetricBlockDes_->GetPDesKeySchedule2();
    }
    /// @brief Gets the third 3DES key
    /// @return DES_key_schedule pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02092
    /// @needwork = dda
    /// @endcode
    void const *_GetDesKeySchedule3() const noexcept
    {
        if (pSymmetricBlockDes_.get() == nullptr) {
            return nullptr;
        }
        return pSymmetricBlockDes_->GetPDesKeySchedule3();
    }
    /// @brief Gets the key slot ID
    /// @return Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02093
    /// @needwork = dda
    /// @endcode
    uint32_t _GetSlotId() const noexcept
    {
        if (pSymmetricBlockDes_.get() == nullptr) {
            return 0U;
        }
        return pSymmetricBlockDes_->GetSlotId();
    }
    /// @brief Gets the SymmetricKey
    /// @return SymmetricKey pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02094
    /// @needwork = dda
    /// @endcode
    SymmetricKey const *_GetSymmetricKey() const noexcept
    {
        if (pSymmetricBlockDes_.get() == nullptr) {
            return nullptr;
        }
        return pSymmetricBlockDes_->GetSymmetricKey();
    }
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_KEY_WRAPPER_DES_H_