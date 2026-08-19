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
/// @file       isoft_file_opt_crypto.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    File operation class supporting crypto operations
/// @date       2021-05-18
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-05-18  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// <tr><td>2023-01-03  <td>1.0.0    <td>hanjingjing      <td>Refactor file directory and crypto support
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/File Operations
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PFileOpt_Crypto
/// @unit_description=File operation class for ciphertext storage
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_COMMON_PH_FILE_OPT_CRYPTO_H_
#define ARA_PER_COMMON_PH_FILE_OPT_CRYPTO_H_

#include "ara/per/internal/common/isoft_file_opt.h"
#include "ara/per/internal/crypto/isoft_per_crypto.h"
#include "ara/per/internal/isoftkv/kv_error_code.h"
#include "ara/per/internal/isoftkv/nai_map_buff.h"
#include "ara/per/internal/isoftkv/page_common_struct.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief HPKS file operation class: Provides Page read/write functionality
class PFileOpt_Crypto : public PFileOpt
{
private:
    /// @brief Local default buffer
    std::unique_ptr< ara::per::isoftkv::PNaiMapBuff > pNaiMapBuff_{nullptr};
    /// @brief Encrypt/decrypt operation object
    std::unique_ptr< ara::per::isoftkv::PPerCrypto > pPerCrypto_{nullptr};

public:
    /// @brief Parameterized constructor
    /// @param nFileFlags File flags
    explicit PFileOpt_Crypto(uint32_t const nFileFlags) noexcept;
    /// @brief
    /// @param  a
    PFileOpt_Crypto(PFileOpt_Crypto const& a) = delete;
    /// @brief
    /// @param a
    /// @return
    PFileOpt_Crypto& operator=(PFileOpt_Crypto const& a) = delete;
    /// @brief
    /// @param  a
    PFileOpt_Crypto(PFileOpt_Crypto const&& a) = delete;
    /// @brief
    /// @param a
    /// @return
    PFileOpt_Crypto& operator=(PFileOpt_Crypto const&& a) = delete;
    /// @brief Destructor
    ~PFileOpt_Crypto() noexcept override;
    /// @brief Read data
    /// @param pBuff
    /// @param nBuffLen
    /// @return
    int32_t ReadData(uint8_t* const pBuff, uint32_t const nBuffLen) const noexcept override;
    /// @brief Write data
    /// @param pVoidData
    /// @param nDataLen
    /// @return
    int32_t WriteData(const void* const pVoidData, uint32_t const nDataLen) const noexcept override;
    /// @brief Get size of valid data in file
    /// @return
    int64_t GetFileSize() const noexcept override;

public:
    /// @brief Create encrypt/decrypt operation object
    /// @param stKeySlotName Key slot name
    /// @param stAlgorithmName Encryption algorithm name
    /// @return Success or failure
    virtual bool BuildCrypto(ara::core::String const& stKeySlotName, ara::core::String const& stAlgorithmName) noexcept;

public:
    /// @brief Read raw data
    /// @param pBuff
    /// @param nBuffLen
    /// @return
    int32_t ReadRawData(uint8_t* const pBuff, uint32_t const nBuffLen) const noexcept;
    /// @brief Write raw data
    /// @param pVoidData
    /// @param nDataLen
    /// @return
    int32_t WriteRawData(const void* const pVoidData, uint32_t const nDataLen) const noexcept;

protected:
    /// @brief Create encrypt/decrypt operation object
    /// @param stKeySlotName Key slot name
    /// @param stAlgorithmName Encryption algorithm name
    /// @param nBuffLen Buff length
    /// @return
    bool _BuildCrypto(ara::core::String const& stKeySlotName,
                      ara::core::String const& stAlgorithmName,
                      uint32_t const nBuffLen) noexcept;
    /// @brief Check if encryption is required
    /// @return
    bool _IsCrypto() const noexcept;
    /// @brief Handle encrypted read
    /// @param nReadPos
    /// @param pBData
    /// @param nDataLen
    /// @return
    int32_t _CryptoRead(int64_t const nReadPos, uint8_t* const pBData, uint32_t const nDataLen) const noexcept;
    /// @brief Handle encrypted write
    /// @param nWritePos
    /// @param pBData
    /// @param nDataLen
    /// @return
    int32_t _CryptoWrite(int64_t const nWritePos, uint8_t* const pBData, uint32_t const nDataLen) const noexcept;
    /// @brief Get temporary file memory space
    /// @return
    inline ara::per::isoftkv::PNaiMapBuff* _GetNaiBuff() const noexcept { return pNaiMapBuff_.get(); }
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
