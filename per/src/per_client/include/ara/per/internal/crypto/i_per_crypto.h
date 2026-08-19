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
/// @file       i_per_crypto.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Encryption/decryption operation interface used by PER persistence module
/// @date       2023-08-03
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
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_CRYPTO_I_PER_CRYPTO_H_
#define ARA_PER_CRYPTO_I_PER_CRYPTO_H_

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

namespace ara {
namespace per {
namespace isoftkv {
class PFileOpt;
//********************************/
/// @brief Interface class for executing encryption operations
/// @code{.isoft}
/// @unit_name=IPerCrypto
/// @interface_level=unit
/// @endcode
class IPerCrypto
{
public:
    /// @brief Default constructor
    IPerCrypto() = default;
    /// @brief
    virtual ~IPerCrypto() = default;
    /// @brief Copy constructor
    /// @param other Another object instance of this class
    IPerCrypto(IPerCrypto const& other) = default;
    /// @brief Move constructor
    /// @param other Another object instance of this class
    IPerCrypto(IPerCrypto&& other) noexcept = default;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    IPerCrypto& operator=(IPerCrypto const& other) = default;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    IPerCrypto& operator=(IPerCrypto&& other) noexcept = default;

public:
    /// @brief Load corresponding key slot
    /// @param stKeySlotName
    /// @return true if completed, false if error occurred
    virtual bool LoadCrypto(ara::core::String const& stKeySlotName,
                            ara::core::String const& stAlgorithmName) noexcept = 0;
    /// @brief Whether key slot loading is completed
    /// @return true if completed, false if error occurred
    virtual bool IsLoadKeyLoad() const noexcept = 0;
    /// @brief Get key length in key slot
    /// @return
    virtual uint32_t GetKeySlotObjectSize() const noexcept = 0;
    /// @brief Perform encrypt/decrypt operation on input data
    /// @param pData Input data, also output data, caller must ensure output fits (risk of buffer overflow)
    /// @param nLen Input data length, also output data length
    /// @param bEncodeOrDecode true for encrypt, false for decrypt
    /// @return Number of bytes encrypted/decrypted, -1 indicates error
    virtual int32_t CryptoData(uint8_t* const pData,
                               uint32_t const nLen,
                               bool const bEncodeOrDecode) const noexcept = 0;
    /// @brief Perform hash operation on file
    /// @param stFileName File name
    /// @param stHashCtx Hash algorithm string identifier
    /// @return Output file Hash code
    virtual ara::core::Vector< uint8_t > CryptoFileHash(ara::core::StringView const& stFileName,
                                                        ara::core::StringView const& stHashCtx) const noexcept = 0;
    /// @brief Perform hash operation on file
    /// @param fileOpt File operation object
    /// @param stHashCtx Hash algorithm string identifier
    /// @return Output file Hash code
    virtual ara::core::Vector< uint8_t > CryptoFileHash(isoftkv::PFileOpt const& fileOpt,
                                                        ara::core::StringView const& stHashCtx) const noexcept = 0;
    /// @brief Perform hash operation on data
    /// @param pData Data area
    /// @param nDataLen Data length
    /// @param stHashCtx Hash algorithm string identifier
    /// @return Output file Hash code
    virtual ara::core::Vector< uint8_t > CryptoDataHash(const uint8_t* const pData,
                                                        uint32_t const nDataLen,
                                                        ara::core::StringView const& stHashCtx) const noexcept = 0;
};
//********************************/
/// @brief Generate corresponding derived class
/// @code{.isoft}
/// @unit_name=NewPerCrypto
/// @endcode
/// @return
std::unique_ptr< IPerCrypto > NewPerCrypto() noexcept;
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
#endif
