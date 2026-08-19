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
/// @file       isoft_common_api.h
/// @brief      AutoSar-AP
/// @details    Some common methods
/// @date       2021-07-14
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-07-14  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_COMMON_PH_COMMON_API_H_
#define ARA_PER_COMMON_PH_COMMON_API_H_

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>

#include "ara/per/internal/common/isoft_data_type.h"

namespace ara {
namespace per {
namespace isoftkv {
/// @brief Function for setting breakpoint to find bugs, macro is best
/// @code{.isoft}
/// @unit_name=PER_Debug
/// @endcode
/// @param bExp Condition
/// @param pChFile __FILE__
/// @param nLineID __LINE__
inline void PER_Debug(bool const bExp, char const *const pChFile, int32_t const nLineID)
{
    bool const bDebug{bExp};
    if (true == bDebug) {
        std::ignore = printf("<PER_Debug> : %s:%d\n", pChFile, nLineID);
    }
}
//********************************/
/// @brief Addition operation applicable to uint8_t, uint16_t, uint32_t, and int32_t
/// @code{.isoft}
/// @unit_name=T_AddInt
/// @endcode
/// @tparam T
/// @param nData
/// @param nAdd
/// @return
template < typename T >
inline T T_AddInt(T const nData, int32_t const nAdd) noexcept
{
    int32_t const nTemp{static_cast< int32_t >(nData) + nAdd};
    return static_cast< T >(nTemp);
}
/// @brief Calculate remainder
/// @code{.isoft}
/// @unit_name=T_Mod
/// @endcode
/// @tparam T_Src1
/// @tparam T_Src2
/// @param nDataA
/// @param nDataB
/// @return nDataA % nDataB
template < typename T_Src1, typename T_Src2 >
inline int32_t T_Mod(T_Src1 const nDataA, T_Src2 const nDataB) noexcept
{
    return static_cast< int32_t >(nDataA) % static_cast< int32_t >(nDataB);
}
/// @brief Calculate quotient, Gaussian rounding
/// @code{.isoft}
/// @unit_name=T_DivUp
/// @endcode
/// @tparam TDst
/// @tparam TSrc
/// @param nDataA
/// @param nDataB
/// @return (nDataA + (nDataB - 1)) / nDataB
template < typename T_Src, typename T_Dst >
inline T_Dst T_DivUp(T_Src const nDataA, T_Src const nDataB) noexcept
{
    T_Src const nDiv{(nDataA + (nDataB - 1)) / nDataB};
    return static_cast< T_Dst >(nDiv);
}
/// @brief Calculate quotient, discard remainder
/// @code{.isoft}
/// @unit_name=T_Div
/// @endcode
/// @tparam T_Src
/// @tparam T_Dst
/// @param nDataA
/// @param nDataB
/// @return nDataA / nDataB
template < typename T_Src, typename T_Dst >
inline T_Dst T_Div(T_Src const nDataA, T_Src const nDataB) noexcept
{
    T_Src const nDiv{nDataA / nDataB};
    return static_cast< T_Dst >(nDiv);
}
/// @brief Do nothing, just occupy function call
/// @code{.isoft}
/// @unit_name=T_Void
/// @endcode
/// @param n
/// @return
inline int32_t T_Void(int32_t n = 0) noexcept
{
    if (n > 0) {
        n += 1;
    }
    return n;
}
/// @brief Number => Base16 character
/// @code{.isoft}
/// @unit_name=T_itochar
/// @endcode
/// @tparam T
/// @param n
/// @return
template < typename T >
inline char8_t T_itochar(T n) noexcept
{
    n = n % static_cast< T >(kInt_0x10);
    if (n >= static_cast< T >(kInt_10)) {
        return static_cast< char8_t >(static_cast< T >(kChar_a) + (n - static_cast< T >(kInt_10)));
    }
    return static_cast< char8_t >(static_cast< T >(kChar_0) + n);
}
/// @brief Base16 character => Number
/// @code{.isoft}
/// @unit_name=T_chartoi
/// @endcode
/// @tparam T
/// @param ch
/// @return
template < typename T >
inline T T_chartoi(char8_t const ch) noexcept
{
    T nData;
    if ((ch >= kChar_A) && (ch <= kChar_Z)) {
        nData = static_cast< T >(ch) - static_cast< T >(kChar_A) + static_cast< T >(kInt_10);
    } else if ((ch >= kChar_a) && (ch <= kChar_z)) {
        nData = static_cast< T >(ch) - static_cast< T >(kChar_a) + static_cast< T >(kInt_10);
    } else if ((ch >= kChar_0) && (ch <= kChar_9)) {
        nData = static_cast< T >(ch) - static_cast< T >(kChar_0);
    } else {
        nData = static_cast< T >(0);
    }
    return nData;
}
/// @brief Base16 character => Number
/// @code{.isoft}
/// @unit_name=T_chartoi
/// @endcode
/// @tparam T
/// @param ch
/// @return
template < typename T >
inline T T_chartoi(uint8_t const ch) noexcept
{
    return T_chartoi< T >(static_cast< char8_t >(ch));
}
/// @brief Memory-stored string => Number
/// @code{.isoft}
/// @unit_name=T_stoi
/// @endcode
/// @tparam T
/// @param pData
/// @param nLen
/// @param nBase
/// @return
/// @throws
template < typename T >
inline T T_stoi(void const *const pData, uint32_t const nLen, int32_t const nBase = kInt_10)
{
    std::string const stData{static_cast< char8_t const * >(pData), static_cast< std::size_t >(nLen)};
    return static_cast< T >(std::stoi(stData, nullptr, nBase));
}
/// @brief Memory-stored string => uint32_t number
/// @code{.isoft}
/// @unit_name=T_stoi
/// @endcode
/// @param pData
/// @param nLen
/// @param nBase
/// @return
/// @throws
template <>
inline uint32_t T_stoi(void const *const pData, uint32_t const nLen, int32_t const nBase)
{
    return static_cast< uint32_t >(std::stoul(
        std::string{static_cast< char8_t const * >(pData), static_cast< std::size_t >(nLen)}, nullptr, nBase));
}
/// @brief
/// @code{.isoft}
/// @unit_name=T_stoi
/// @endcode
/// @param pData
/// @param nLen
/// @param nBase
/// @return
/// @throws
template <>
inline int64_t T_stoi(void const *const pData, uint32_t const nLen, int32_t const nBase)
{
    return static_cast< int64_t >(std::stoll(
        std::string{static_cast< char8_t const * >(pData), static_cast< std::size_t >(nLen)}, nullptr, nBase));
}
/// @brief
/// @code{.isoft}
/// @unit_name=T_stoi
/// @endcode
/// @param pData
/// @param nLen
/// @param nBase
/// @return
/// @throws
template <>
inline uint64_t T_stoi(void const *const pData, uint32_t const nLen, int32_t const nBase)
{
    return static_cast< uint64_t >(std::stoull(
        std::string{static_cast< char8_t const * >(pData), static_cast< std::size_t >(nLen)}, nullptr, nBase));
}
/// @brief Check if b == (a & b)
/// @code{.isoft}
/// @unit_name=T_IsAndData
/// @endcode
/// @tparam T
/// @param a
/// @param b
/// @return
template < typename T >
inline bool T_IsAndData(T const a, T const b) noexcept
{
    uint32_t const nA{static_cast< uint32_t >(a)};
    uint32_t const nB{static_cast< uint32_t >(b)};
    return nB == (nA & nB);
}
/// @brief Calculate a & b
/// @code{.isoft}
/// @unit_name=T_AndData
/// @endcode
/// @tparam T
/// @param a
/// @param b
/// @return
template < typename T >
inline T T_AndData(T const a, T const b) noexcept
{
    uint32_t const nA{static_cast< uint32_t >(a)};
    uint32_t const nB{static_cast< uint32_t >(b)};
    return static_cast< T >(nA & nB);
}
/// @brief Return integer A rounded up by B
/// @code{.isoft}
/// @unit_name=T_AlignNumberUp
/// @endcode
/// @param a
/// @param b
/// @return
inline uint32_t T_AlignNumberUp(uint32_t const a, uint32_t const b) noexcept
{
    uint32_t const bStep{b - 1U};
    return (a + bStep) & (~bStep);
}
/// @brief Return integer A rounded up by B
/// @code{.isoft}
/// @unit_name=T_AlignNumberUp
/// @endcode
/// @param a
/// @param b
/// @return
inline int64_t T_AlignNumberUp(int64_t const a, uint32_t const b) noexcept
{
    uint64_t const bStep{b - static_cast< uint64_t >(1)};
    uint64_t nA{static_cast< uint64_t >(a) + bStep};
    nA = nA & (~bStep);
    return static_cast< int64_t >(nA);
}
/// @brief Return integer A rounded down by B
/// @code{.isoft}
/// @unit_name=T_AlignNumberDown
/// @endcode
/// @param a
/// @param b
/// @return
inline int64_t T_AlignNumberDown(int64_t const a, uint32_t const b) noexcept
{
    uint64_t const bStep{b - static_cast< uint64_t >(1)};
    uint64_t nA{static_cast< uint64_t >(a)};
    nA = nA & (~bStep);
    return static_cast< int64_t >(nA);
}
/// @brief Memory copy function
/// @code{.isoft}
/// @unit_name=T_Memcpy
/// @endcode
/// @param pDst
/// @param pSrc
/// @param nLen
/// @return
inline void *T_Memcpy(void *const pDst, const void *const pSrc, uint32_t const nLen) noexcept
{
    return memcpy(pDst, pSrc, static_cast< std::size_t >(nLen));
}
//********************************/ // Following type conversions are to resolve QAC rules (e.g., 3080)
/// @brief Pointer conversion: T * => uint8_t *
/// @code{.isoft}
/// @unit_name=T_TransBytes
/// @endcode
/// @tparam T
/// @param pData
/// @return
template < typename T >
inline uint8_t *T_TransBytes(T *const pData) noexcept
{
    return static_cast< uint8_t * >(static_cast< void * >(pData));
}
/// @brief Pointer conversion: T * => char8_t *
/// @code{.isoft}
/// @unit_name=T_TransChar
/// @endcode
/// @tparam T
/// @param pData
/// @return
template < typename T >
inline char8_t *T_TransChar(T *const pData) noexcept
{
    return static_cast< char8_t * >(static_cast< void * >(pData));
}
/// @brief Pointer conversion: T const * => char8_t const *
/// @code{.isoft}
/// @unit_name=T_TransChar
/// @endcode
/// @tparam T
/// @param pData
/// @return
template < typename T >
inline char8_t const *T_TransChar(T const *const pData) noexcept
{
    return static_cast< char8_t const * >(static_cast< void const * >(pData));
}
/// @brief Pointer conversion: T const * => uint8_t const *
/// @code{.isoft}
/// @unit_name=T_TransBytes
/// @endcode
/// @tparam T
/// @param pData
/// @return
template < typename T >
inline uint8_t const *T_TransBytes(T const *const pData) noexcept
{
    return static_cast< uint8_t const * >(static_cast< const void * >(pData));
}
/// @brief Pointer conversion: uint8_t * => T *
/// @code{.isoft}
/// @unit_name=T_TransPtr
/// @endcode
/// @tparam T
/// @param pData
/// @return
template < typename T >
inline T *T_TransPtr(uint8_t *const pData) noexcept
{
    return static_cast< T * >(static_cast< void * >(pData));
}
/// @brief Pointer conversion: uint8_t const * => T const *
/// @code{.isoft}
/// @unit_name=T_TransPtr
/// @endcode
/// @tparam T
/// @param pData
/// @return
template < typename T >
inline T const *T_TransPtr(uint8_t const *const pData) noexcept
{
    return static_cast< T const * >(static_cast< const void * >(pData));
}
/// @brief Pointer conversion: T_Src * => T_Dst *
/// @code{.isoft}
/// @unit_name=T_TransPtr
/// @endcode
/// @tparam T
/// @param pData
/// @return
template < typename T_Src, typename T_Dst >
inline T_Dst *T_TransPtr(T_Src *const pData) noexcept
{
    return static_cast< T_Dst * >(static_cast< void * >(pData));
}
/// @brief Pointer conversion: T_Src const * => T_Dst const *
/// @code{.isoft}
/// @unit_name=T_TransPtr
/// @endcode
/// @tparam T_Src
/// @tparam T_Dst
/// @param pData
/// @return
template < typename T_Src, typename T_Dst >
inline T_Dst const *T_TransPtr(T_Src const *const pData) noexcept
{
    return static_cast< T_Dst const * >(static_cast< void const * >(pData));
}
/// @brief Pointer conversion: Array type to pointer type
/// @code{.isoft}
/// @unit_name=T_TransArray
/// @endcode
/// @tparam T
/// @param array
/// @return
template < typename T >
inline T const *T_TransArray(T const array[]) noexcept
{
    return static_cast< T const * >(array);
}
/// @brief Pointer arithmetic
/// @code{.isoft}
/// @unit_name=T_OffsetPtr
/// @endcode
/// @tparam T
/// @param pData
/// @param nOffset
/// @return pData + nOffset;
template < typename T >
inline T *T_OffsetPtr(T *const pData, int32_t const nOffset) noexcept
{
    return pData + nOffset;
}
/// @brief Pointer arithmetic
/// @code{.isoft}
/// @unit_name=T_OffsetPtr
/// @endcode
/// @tparam T
/// @param pData
/// @param nOffset
/// @return pData + nOffset;
template < typename T >
inline T const *T_OffsetPtr(T const *const pData, int32_t const nOffset) noexcept
{
    return pData + nOffset;
}
/// @brief Pointer arithmetic pDataA - pDataB;
/// @code{.isoft}
/// @unit_name=T_SubPtr
/// @endcode
/// @tparam T_Src
/// @tparam T_Dst
/// @param pVoidA
/// @param pVoidB
/// @return Return value is difference between two addresses: Byte unit
template < typename T_Src, typename T_Dst >
inline int32_t T_SubPtr(T_Src const *const pVoidA, T_Dst const *const pVoidB) noexcept
{
    uint8_t const *const pDataA{T_TransBytes< T_Src >(pVoidA)};
    uint8_t const *const pDataB{T_TransBytes< T_Dst >(pVoidB)};
    return static_cast< int32_t >(pDataA - pDataB);
}
//********************************/
/// @brief ara::core::String => ara::core::StringView
/// @code{.isoft}
/// @unit_name=T_StringView
/// @endcode
/// @param stData
/// @return
inline ara::core::StringView T_StringView(ara::core::String const &stData) noexcept
{
    return std::move(ara::core::StringView{stData.data(), stData.size()});
}
/// @brief char[] => ara::core::StringView
/// @code{.isoft}
/// @unit_name=T_StringView
/// @endcode
/// @param chData
/// @return
inline ara::core::StringView T_StringView(ara::per::char8_t const chData[]) noexcept
{
    return std::move(ara::core::StringView{chData});
}
/// @brief uint8_t * => ara::core::StringView
/// @code{.isoft}
/// @unit_name=T_StringView
/// @endcode
/// @param pData
/// @param nLen Byte unit
/// @return
inline ara::core::StringView T_StringView(void const *const pData, uint32_t const nLen) noexcept
{
    return std::move(ara::core::StringView{T_TransPtr< void, char8_t >(pData),
                                           static_cast< ara::core::StringView::size_type >(nLen)});
}
/// @brief ara::core::StringView => ara::core::String
/// @code{.isoft}
/// @unit_name=T_String
/// @endcode
/// @param stData
/// @return
inline ara::core::String T_String(ara::core::StringView const &stData) noexcept
{
    return ara::core::String{stData.data(), stData.size()};
}
/// @brief (char *, int) => ara::core::String
/// @code{.isoft}
/// @unit_name=T_String
/// @endcode
/// @param pData
/// @param nLen
/// @return
inline ara::core::String T_String(void const *const pData, uint32_t const nLen) noexcept
{
    return ara::core::String{T_TransPtr< void, char8_t >(pData), static_cast< ara::core::StringView::size_type >(nLen)};
}
//********************************/
/// @brief Get current time: Seconds since 1970-01-01
/// @code{.isoft}
/// @unit_name=TM_NowSecond
/// @endcode
/// @return
inline uint64_t TM_NowSecond() noexcept
{
    std::chrono::time_point< std::chrono::system_clock, std::chrono::seconds > const tmSeconds{
        std::chrono::time_point_cast< std::chrono::seconds >(std::chrono::system_clock::now())};
    return static_cast< uint64_t >(tmSeconds.time_since_epoch().count());
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
#endif
