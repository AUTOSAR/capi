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
/// @file       nai_map_buff.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Auto-allocated/released (512-byte aligned) memory used by KV storage
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
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PNaiMapBuff
/// @unit_description=Auto-allocated/released (512-byte aligned) memory used by KV storage
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_PHKV_NAI_MAP_BUFF_H_
#define ARA_PER_PHKV_NAI_MAP_BUFF_H_
#include <cstdint>

namespace ara {
namespace per {
namespace isoftkv {
//********************************/

/// @brief Aligned buffer provided by nai
class PNaiMapBuff final
{
private:
    /// @brief
    uint32_t nBuffLen_;
    /// @brief
    uint8_t *pBBuff_;

public:
    /// @brief Default constructor: Deleted
    PNaiMapBuff() = default;
    /// @brief
    /// @param nBuffLen
    explicit PNaiMapBuff(uint32_t const nBuffLen) noexcept;
    /// @brief
    ~PNaiMapBuff() noexcept;
    /// @brief
    /// @param a
    PNaiMapBuff(PNaiMapBuff const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PNaiMapBuff &operator=(PNaiMapBuff const &a) = delete;
    /// @brief
    /// @param a
    PNaiMapBuff(PNaiMapBuff &&a) noexcept;
    /// @brief
    /// @param a
    /// @return
    PNaiMapBuff &operator=(PNaiMapBuff &&a) noexcept;
    /// @brief
    /// @return
    inline uint8_t *GetBuff() const noexcept { return pBBuff_; }
    /// @brief
    /// @return
    inline uint32_t GetLen() const noexcept { return nBuffLen_; }
    /// @brief Reset all buffer values
    /// @param nData
    void ResetBuffData(uint8_t const nData) noexcept;
    /// @brief Reset Buff length
    /// @param nBuffLen
    void ResizeBuff(uint32_t const nBuffLen) noexcept;

protected:
    /// @brief Allocate new buffer
    /// @param nBuffLen
    void _NewBuff(uint32_t const nBuffLen) noexcept;
    /// @brief Delete buffer
    void _DelBuff() noexcept;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
