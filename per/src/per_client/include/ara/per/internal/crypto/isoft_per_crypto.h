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
/// @file       isoft_per_crypto.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Encryption/decryption operation class used by PER persistence module
/// @date       2022-12-22
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-12-22  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Crypto Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PPerCrypto
/// @unit_description=Encryption/decryption operation class used by PER persistence module
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_CRYPTO_PER_CRYPTO_H_
#define ARA_PER_CRYPTO_PER_CRYPTO_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/utility.h>
#include <ara/core/vector.h>
#ifdef ARA_WITH_CRYPTO
    #include <ara/crypto/cryp/cryobj/symmetric_key.h>
    #include <ara/crypto/cryp/crypto_provider.h>
    #include <ara/crypto/cryp/symmetric_block_cipher_ctx.h>
    #include <ara/crypto/keys/keyslot.h>
#endif

#include "ara/per/internal/common/isoft_file_opt.h"
#include "ara/per/internal/crypto/i_per_crypto.h"

namespace ara {
namespace per {
namespace isoftkv {

//********************************/
/// @brief
class PPerCrypto final : public IPerCrypto
{
private:
    /// @brief Key slot
    ara::core::String stKeySlotName_;
    /// @brief Encryption algorithm identifier passed in 2211
    ara::core::String stAlgorithmName_;
#ifdef ARA_WITH_CRYPTO
    /// @brief
    ara::crypto::cryp::CryptoProvider::Uptr pCryptoProvider_;
    /// @brief
    ara::crypto::keys::KeySlot::Uptr pKeySlot_;
    /// @brief
    ara::crypto::cryp::SymmetricKey::Uptrc pSymmetricKey_;
#endif

public:
    /// @brief
    PPerCrypto() = default;
    /// @brief
    ~PPerCrypto() override = default;
    /// @brief
    /// @param a
    PPerCrypto(PPerCrypto const& a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPerCrypto& operator=(PPerCrypto const& a) = delete;
    /// @brief
    /// @param a
    PPerCrypto(PPerCrypto&& a) = delete;
    /// @brief
    /// @param a
    /// @return
    PPerCrypto& operator=(PPerCrypto&& a) = delete;

public:
    /// @brief Load corresponding key slot
    /// @param stKeySlotName
    /// @return true if completed, false if error occurred
    bool LoadCrypto(ara::core::String const& stKeySlotName, ara::core::String const& stAlgorithmName) noexcept override;
    /// @brief Whether key slot loading is completed
    /// @return true if completed, false if error occurred
    bool IsLoadKeyLoad() const noexcept override;
    /// @brief Get key length in key slot
    /// @return
    uint32_t GetKeySlotObjectSize() const noexcept override;
    /// @brief Perform encrypt/decrypt operation on input data
    /// @param vecData Input data, also output data
    /// @param bEncodeOrDecode true for encrypt, false for decrypt
    /// @return Number of bytes encrypted/decrypted, -1 indicates error
    int32_t CryptoData(ara::core::Vector< ara::core::Byte >& vecData, bool const bEncodeOrDecode) const noexcept;
    /// @brief Perform encrypt/decrypt operation on input data
    /// @param pData Input data, also output data, caller must ensure output fits (risk of buffer overflow)
    /// @param nLen Input data length, also output data length
    /// @param bEncodeOrDecode true for encrypt, false for decrypt
    /// @return Number of bytes encrypted/decrypted, -1 indicates error
    int32_t CryptoData(uint8_t* const pData, uint32_t const nLen, bool const bEncodeOrDecode) const noexcept override;
    /// @brief Perform hash operation on file
    /// @param stFileName File name
    /// @param stHashCtx Hash algorithm string identifier
    /// @return Output file Hash code
    ara::core::Vector< uint8_t > CryptoFileHash(ara::core::StringView const& stFileName,
                                                ara::core::StringView const& stHashCtx) const noexcept override;
    /// @brief Perform hash operation on file
    /// @param fileOpt File operation object
    /// @param stHashCtx Hash algorithm string identifier
    /// @return Output file Hash code
    ara::core::Vector< uint8_t > CryptoFileHash(isoftkv::PFileOpt const& fileOpt,
                                                ara::core::StringView const& stHashCtx) const noexcept override;
    /// @brief Perform hash operation on data
    /// @param pData Data area
    /// @param nDataLen Data length
    /// @param stHashCtx Hash algorithm string identifier
    /// @return Output file Hash code
    ara::core::Vector< uint8_t > CryptoDataHash(const uint8_t* const pData,
                                                uint32_t const nDataLen,
                                                ara::core::StringView const& stHashCtx) const noexcept override;

protected:
#ifdef ARA_WITH_CRYPTO
    /// @brief Generate crypto context and set key
    /// @param transForm Crypto direction: Encrypt or decrypt
    /// @return Crypto context
    ara::crypto::cryp::SymmetricBlockCipherCtx::Uptr _MakeSymmetricCtx(
        ara::crypto::CryptoTransform const transForm) const noexcept;
#endif
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
#endif
