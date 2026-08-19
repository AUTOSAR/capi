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
/// @file       kv_data_type.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Centralized definition of KV storage related data structures
/// @date       2021-05-19
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-05-19  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// <tr><td>2021-07-13  <td>1.0.0    <td>hanjingjing      <td>Modify valid PageID starting from 1, 0 invalid
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

#ifndef ARA_PER_PHKV_PH_KV_DATA_TYPE_H_
#define ARA_PER_PHKV_PH_KV_DATA_TYPE_H_

#include <cstring>
#include <functional>

#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/common/isoft_data_type.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Callback function declaration handling one MStringView parameter, returns whether successful
/// @code{.isoft}
/// @unit_name=CB_DealStringView
/// @endcode
using CB_DealStringView = std::function< bool(ara::core::StringView const &stKeyView) >;
/// @brief Process one int32_t integer, return value is uint32_t type
/// @code{.isoft}
/// @unit_name=CB_DealInt32
/// @endcode
using CB_DealInt32 = std::function< uint64_t(int32_t const nIndex) >;
//********************************/
/// @brief Enum: Data type // Real value is: [kArray] + (kDefault, kMaxCount);
/// @code{.isoft}
/// @unit_name=EDataType
/// @endcode
enum class EDataType : uint8_t
{
    kDefault          = 0,    // int32_t
    kInt8             = 1,    // 1-byte signed integer
    kInt16            = 2,    // 2-byte signed integer
    kInt32            = 3,    // 4-byte signed integer
    kInt64            = 4,    // 8-byte signed integer
    kUint8            = 5,    // 1-byte unsigned integer
    kUint16           = 6,    // 2-byte unsigned integer
    kUint32           = 7,    // 4-byte unsigned integer
    kUint64           = 8,    // 8-byte unsigned integer
    kBool             = 9,    // Boolean: false(0), true(1)
    kFloat            = 10,   // 4-byte single-precision float
    kDouble           = 11,   // 8-byte double-precision float
    kString           = 12,   // '\0' terminated string
    kBinary           = 13,   // Binary data
    kObject           = 14,   // Special object type: Not implemented yet
    kMaxCount         = 15,   // Used internally
    kRecordStringKv   = 126,  // Internal specialized type: A string type, KV library deletion record
    kRecordStringFile = 127,  // Internal specialized type: A string type, File library deletion record
    kArray            = 128,  // Array type: Not supported yet
};
/// @brief Convert integer to custom data type enum
/// @code{.isoft}
/// @unit_name=TransToDataType
/// @endcode
/// @param nDataType
/// @return
inline EDataType TransToDataType(int32_t const nDataType) noexcept
{
    if ((nDataType < static_cast< int32_t >(EDataType::kDefault))
        || (nDataType >= static_cast< int32_t >(EDataType::kMaxCount))) {
        return EDataType::kDefault;
    }
    return static_cast< EDataType >(nDataType);
}
//********************************/
/// @brief Return custom data type enum for binary data
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @tparam T
/// @return
template < typename T >
EDataType T_GetDataType() noexcept
{
    return EDataType::kBinary;
}
/// @brief Return custom data type enum for char8_t
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @return
template <>
inline EDataType T_GetDataType< char8_t >() noexcept
{
    return EDataType::kInt8;
}
/// @brief Return custom data type enum for int8_t
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @return
template <>
inline EDataType T_GetDataType< int8_t >() noexcept
{
    return EDataType::kInt8;
}
/// @brief Return custom data type enum for int16_t
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @return
template <>
inline EDataType T_GetDataType< int16_t >() noexcept
{
    return EDataType::kInt16;
}
/// @brief Return custom data type enum for int32_t
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @return
template <>
inline EDataType T_GetDataType< int32_t >() noexcept
{
    return EDataType::kInt32;
}
/// @brief Return custom data type enum for int64_t
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @return
template <>
inline EDataType T_GetDataType< int64_t >() noexcept
{
    return EDataType::kInt64;
}
/// @brief Return custom data type enum for uint8_t
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @return
template <>
inline EDataType T_GetDataType< uint8_t >() noexcept
{
    return EDataType::kUint8;
}
/// @brief Return custom data type enum for uint16_t
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @return
template <>
inline EDataType T_GetDataType< uint16_t >() noexcept
{
    return EDataType::kUint16;
}
/// @brief Return custom data type enum for uint32_t
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @return
template <>
inline EDataType T_GetDataType< uint32_t >() noexcept
{
    return EDataType::kUint32;
}
/// @brief Return custom data type enum for uint64_t
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @return
template <>
inline EDataType T_GetDataType< uint64_t >() noexcept
{
    return EDataType::kUint64;
}
/// @brief Return custom data type enum for bool
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @return
template <>
inline EDataType T_GetDataType< bool >() noexcept
{
    return EDataType::kBool;
}
/// @brief Return custom data type enum for float
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @return
template <>
inline EDataType T_GetDataType< float >() noexcept
{
    return EDataType::kFloat;
}
/// @brief Return custom data type enum for double
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @return
template <>
inline EDataType T_GetDataType< double >() noexcept
{
    return EDataType::kDouble;
}
/// @brief Return custom data type enum for ara::core::String
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @return
template <>
inline EDataType T_GetDataType< ara::core::String >() noexcept
{
    return EDataType::kString;
}
/// @brief Return custom data type enum for ara::core::StringView
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @return
template <>
inline EDataType T_GetDataType< ara::core::StringView >() noexcept
{
    return EDataType::kString;
}
/// @brief Return custom data type enum for uint8_t*
/// @code{.isoft}
/// @unit_name=T_GetDataType
/// @endcode
/// @return
template <>
inline EDataType T_GetDataType< uint8_t * >() noexcept
{
    return EDataType::kBinary;
}
//********************************/
/// @brief Another way of writing T_GetDataType
/// @code{.isoft}
/// @unit_name=T_GetDataTypeNew
/// @endcode
/// @tparam T
/// @return
template < typename T >
EDataType T_GetDataTypeNew() noexcept
{
    if (std::is_same< T, ara::core::StringView >::value) {
        return EDataType::kString;
    }
    if (std::is_same< T, ara::core::String >::value) {
        return EDataType::kString;
    }
    if (std::is_same< T, char >::value) {
        return EDataType::kInt8;
    }
    if (std::is_same< T, int8_t >::value) {
        return EDataType::kInt8;
    }
    if (std::is_same< T, int16_t >::value) {
        return EDataType::kInt16;
    }
    if (std::is_same< T, int32_t >::value) {
        return EDataType::kInt32;
    }
    if (std::is_same< T, int64_t >::value) {
        return EDataType::kInt64;
    }
    if (std::is_same< T, uint8_t >::value) {
        return EDataType::kUint8;
    }
    if (std::is_same< T, uint16_t >::value) {
        return EDataType::kUint16;
    }
    if (std::is_same< T, uint32_t >::value) {
        return EDataType::kUint32;
    }
    if (std::is_same< T, uint64_t >::value) {
        return EDataType::kUint64;
    }
    if (std::is_same< T, bool >::value) {
        return EDataType::kBool;
    }
    if (std::is_same< T, float >::value) {
        return EDataType::kFloat;
    }
    if (std::is_same< T, double >::value) {
        return EDataType::kDouble;
    }
    if (std::is_same< T, uint8_t * >::value) {
        return EDataType::kBinary;
    }

    return EDataType::kBinary;
}
//********************************/

/// @brief
/// @code{.isoft}
/// @unit_name=TSerialize
/// @endcode
class TSerialize final
{
public:
    /// @brief Read T type data from byte stream
    /// @tparam T
    /// @param nTotalLen
    /// @return
    template < typename T >
    static uint32_t GetVectorLength(int32_t const nTotalLen) noexcept
    {  // Only calculated length of fixed-length Vector
        uint32_t nReturnLen{0U};
        EDataType const eDataType{T_GetDataType< T >()};
        if ((eDataType >= EDataType::kInt8) || (eDataType <= EDataType::kDouble)) {
            // Fixed-length Vector length
            nReturnLen = static_cast< uint32_t >((static_cast< std::size_t >(nTotalLen) + sizeof(T) - 1U) / sizeof(T));
        } else {
            nReturnLen = 1U;
        }
        return nReturnLen;
    }
    /// @brief Read T type data from byte stream
    /// @tparam T
    /// @param data
    /// @param pData
    /// @return
    /// @throws
    template < typename T >
    static uint32_t ReadData(T &data, uint8_t const *const pData)
    {
        uint32_t nReturnLen{0U};
        EDataType const eDataType{T_GetDataType< T >()};
        if ((eDataType >= EDataType::kInt8) || (eDataType <= EDataType::kDouble)) {
            nReturnLen  = sizeof(T);
            std::ignore = T_Memcpy(&data, pData, nReturnLen);
        } else {
            nReturnLen = 0U;
        }
        return nReturnLen;
    }
    /// @brief Read T type data from byte stream
    /// @param data
    /// @param pData
    /// @return
    static uint32_t ReadData(ara::core::String &data, uint8_t const *const pData) noexcept
    {
        std::ignore = data;
        uint32_t nReturnLen{0U};
        EDataType const eDataType{T_GetDataType< ara::core::String >()};
        switch (eDataType) {
            case EDataType::kString: {
                uint32_t nStringLen{0U};
                nReturnLen  = sizeof(uint32_t);
                std::ignore = T_Memcpy(&nStringLen, pData, nReturnLen);
                data        = T_String(pData + nReturnLen, nStringLen);
                nReturnLen += nStringLen;
            } break;
            case EDataType::kBinary:  // 2022-04-18 hanjingjing, nested not supported yet
            case EDataType::kObject:  // Special object type: Not implemented yet
            {
                uint32_t nReadLen{sizeof(uint32_t)};
                nReturnLen  = sizeof(uint32_t);
                std::ignore = T_Memcpy(&nReadLen, pData, nReturnLen);
                data        = T_String(pData + nReturnLen, nReadLen);
                nReturnLen  = nReadLen;
            } break;
            default: {
                nReturnLen = 0U;
            } break;
        }
        return nReturnLen;
    }
    /// @brief Write T type data into byte stream
    /// @tparam T
    /// @param data
    /// @param pData
    /// @return
    /// @throws
    template < typename T >
    static uint32_t WriteData(T const &data, uint8_t *const pData)
    {
        uint32_t nReturnLen{0U};
        EDataType const eDataType{T_GetDataType< T >()};
        if ((eDataType >= EDataType::kInt8) || (eDataType <= EDataType::kDouble)) {
            nReturnLen = sizeof(T);
            if (nullptr != pData) {
                std::ignore = T_Memcpy(pData, &data, nReturnLen);
            }
        } else {
            nReturnLen = 0U;
        }
        return nReturnLen;
    }
    /// @brief Write T type data into byte stream
    /// @param data
    /// @param pData
    /// @return
    static uint32_t WriteData(ara::core::String const &data, uint8_t *const pData) noexcept
    {
        uint32_t nReturnLen{0U};
        EDataType const eDataType{T_GetDataType< ara::core::String >()};
        switch (eDataType) {
            case EDataType::kString: {
                uint32_t const nStringLen{static_cast< uint32_t >(data.size())};
                if (nullptr != pData) {
                    std::ignore = T_Memcpy(pData, &nStringLen, sizeof(uint32_t));
                }
                nReturnLen = sizeof(uint32_t);
                if ((nullptr != pData) && (nStringLen > 0U)) {
                    std::ignore = T_Memcpy(pData + nReturnLen, data.data(), nStringLen);
                }
                nReturnLen += nStringLen;
            } break;
            case EDataType::kBinary:  // 2022-04-18 hanjingjing, nested not supported yet
            case EDataType::kObject:  // Special object type: Not implemented yet
            {
                uint32_t const nWriteLen{static_cast< uint32_t >(data.size())};
                if (nullptr != pData) {
                    std::ignore = T_Memcpy(pData, &nWriteLen, sizeof(uint32_t));
                }
                nReturnLen = sizeof(uint32_t);
                if ((nullptr != pData) && (nWriteLen > 0U)) {
                    std::ignore = T_Memcpy(pData + nReturnLen, data.data(), nReturnLen);
                }
                nReturnLen += nWriteLen;
            } break;
            default: {
                nReturnLen = 0U;
            } break;
        }
        return nReturnLen;
    }
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
#endif
