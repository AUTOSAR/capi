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
/// @file       isoft_file_opt_page.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    File operation class reading by page
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
/// @unit_name=PFileOpt_Page
/// @unit_description=File operation class reading by page
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_COMMON_PH_FILE_OPT_PAGE_H_
#define ARA_PER_COMMON_PH_FILE_OPT_PAGE_H_

#include "ara/per/internal/common/isoft_file_opt_crypto.h"
#include "ara/per/internal/isoftkv/kv_error_code.h"
#include "ara/per/internal/isoftkv/page_common_struct.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief HPKS file operation class: Provides Page read/write functionality
class PFileOpt_Page : public PFileOpt_Crypto
{
private:
    /// @brief Single page size
    uint16_t nPageLen_{0U};
    bool bLockSuccess_{false};

public:
    /// @brief Delete default constructor
    PFileOpt_Page() = delete;
    /// @brief Constructor setting direct read page size
    /// @param nPageLen If 0, nPageLen_ will be automatically set to nai::nai_sector_size
    explicit PFileOpt_Page(uint16_t const nPageLen) noexcept;
    /// @brief Destructor
    ~PFileOpt_Page() noexcept override;
    /// @brief
    /// @param a
    PFileOpt_Page(PFileOpt_Page const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PFileOpt_Page &operator=(PFileOpt_Page const &a) = delete;
    /// @brief
    /// @param a
    PFileOpt_Page(PFileOpt_Page &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PFileOpt_Page &operator=(PFileOpt_Page &&a) = delete;
    /// @brief Create encrypt/decrypt operation object
    /// @param stKeySlotName Key slot name
    /// @param stAlgorithmName Encryption algorithm name
    /// @return Success or failure
    bool BuildCrypto(ara::core::String const &stKeySlotName,
                     ara::core::String const &stAlgorithmName) noexcept override;

public:
    /// @brief Prepare everything before reading/writing
    /// @param stFileName
    /// @param nPageLen
    /// @return
    bool DoPrepareWork(ara::core::StringView const &stFileName, uint16_t const nPageLen) noexcept override;
    /// @brief Check if file is open
    /// @return
    bool IsOpen() const noexcept override;
    /// @brief Check if file lock was successful
    /// @return
    bool IsLockSuccess() const noexcept;
    /// @brief Read one page of data, nPageID starts from 1
    /// @param nPageID
    /// @param pBuff
    /// @param nBuffLen
    /// @return Return actual number of data bytes read
    int32_t ReadPage(uint32_t const nPageID, uint8_t *const pBuff, uint32_t const nBuffLen) const noexcept;
    /// @brief Write one page of data, nPageID starts from 1
    /// @param nPageID
    /// @param pBData
    /// @param nDataLen
    /// @return
    bool WritePage(uint32_t const nPageID, uint8_t const *const pBData, int32_t const nDataLen) const noexcept;
    /// @brief Reset file size
    /// @param nPageCount
    /// @return
    bool ResizePage(uint32_t const nPageCount) const noexcept;
    /// @brief Return new page number
    /// @param nPageLen
    /// @return
    uint32_t NewPage(int32_t const nPageLen) const noexcept;
    /// @brief Get Page count
    /// @return
    uint32_t GetPageTotal() const noexcept;
    /// @brief Get single page size
    /// @return
    inline uint16_t GetPageLen() const noexcept { return nPageLen_; }
    /// @brief Get page-aligned file size
    /// @return
    int64_t GetFileSizeByPage() const noexcept;
    /// @brief Move file cursor by page unit
    /// @param eMoveType
    /// @param nPageID
    /// @return
    bool SeekPosByPageID(EFileSeekType const eMoveType, uint32_t const nPageID) const noexcept;
    /// @brief Get start position of last page
    /// @return
    int64_t GetLastSectorStartPos() const noexcept;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
