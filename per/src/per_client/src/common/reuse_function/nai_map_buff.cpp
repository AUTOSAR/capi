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
/// @file       nai_map_buff.cpp
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

#include "ara/per/internal/isoftkv/nai_map_buff.h"

#include <nai/os/nai_mman.h>

#include <cstring>
#include <tuple>

#include "ara/per/internal/common/isoft_common_api.h"

namespace {
/// @brief Adapt Nai buffer length
/// @param nBuffLen
/// @return
uint32_t G_NaiBuff_AlignLen(uint32_t const nBuffLen) noexcept
{
    uint32_t const nMinLen{static_cast< uint32_t >(nai_sector_size)};
    if (nBuffLen <= 0U) {
        return nMinLen;
    }
    return ara::per::isoftkv::T_AlignNumberUp(nBuffLen, nMinLen);
}
/// @brief Create new Nai buffer
/// @param nBuffLen
/// @return
void *G_NaiBuff_New(uint32_t const nBuffLen) noexcept
{
    return nai_mmap(nullptr, nullptr, static_cast< std::size_t >(nBuffLen), 0, NAI_MPROT_READ | NAI_MPROT_WRITE);
}
}  // namespace

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief
/// @param nBuffLen
PNaiMapBuff::PNaiMapBuff(uint32_t const nBuffLen) noexcept
    : nBuffLen_{G_NaiBuff_AlignLen(nBuffLen)}, pBBuff_{T_TransBytes(G_NaiBuff_New(nBuffLen_))}
{
}
/// @brief
PNaiMapBuff::~PNaiMapBuff() noexcept { _DelBuff(); }
/// @brief
/// @param a
PNaiMapBuff::PNaiMapBuff(PNaiMapBuff &&a) noexcept : nBuffLen_{std::move(a).nBuffLen_}, pBBuff_{std::move(a).pBBuff_}
{
    a.pBBuff_   = nullptr;
    a.nBuffLen_ = 0U;
}
/// @brief
/// @param a
/// @return
PNaiMapBuff &PNaiMapBuff::operator=(PNaiMapBuff &&a) noexcept
{
    if (this == &a) {
        return *this;
    }
    pBBuff_     = std::move(a).pBBuff_;
    nBuffLen_   = std::move(a).nBuffLen_;
    a.pBBuff_   = nullptr;
    a.nBuffLen_ = 0U;
    return *this;
}
/// @brief Reset all buffer values
/// @param nData
void PNaiMapBuff::ResetBuffData(uint8_t const nData) noexcept
{
    if (nullptr != pBBuff_) {
        std::ignore = memset(pBBuff_, static_cast< int32_t >(nData), static_cast< std::size_t >(nBuffLen_));
    }
}
/// @brief Reset Buff length
/// @param nBuffLen
void PNaiMapBuff::ResizeBuff(uint32_t const nBuffLen) noexcept
{
    if ((nBuffLen_ == nBuffLen) && (nullptr != pBBuff_)) {
        return;
    }
    _DelBuff();
    _NewBuff(nBuffLen);
}
/// @brief Allocate new buffer
/// @param nBuffLen
void PNaiMapBuff::_NewBuff(uint32_t const nBuffLen) noexcept
{
    nBuffLen_ = G_NaiBuff_AlignLen(nBuffLen);
    pBBuff_   = T_TransBytes(G_NaiBuff_New(nBuffLen_));
}
/// @brief Delete buffer
void PNaiMapBuff::_DelBuff() noexcept
{
    if (nullptr != pBBuff_) {
        std::ignore = nai_munmap(nullptr, pBBuff_, static_cast< std::size_t >(nBuffLen_));
    }
    pBBuff_   = nullptr;
    nBuffLen_ = 0U;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
