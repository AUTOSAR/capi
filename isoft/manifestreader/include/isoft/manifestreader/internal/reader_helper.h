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
/// @file       reader_helper.h
/// @brief
/// @details
/// @date       2021-11-17
/// @author     wei.jia
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_MANIFESTREADER_INCLUDE_PUBLIC_ISOFT_MANIFESTREADER_INTERNAL_READER_HELPER_H_
#define ISOFT_MANIFESTREADER_INCLUDE_PUBLIC_ISOFT_MANIFESTREADER_INTERNAL_READER_HELPER_H_

#include <cstdint>
#include <functional>
#include <utility>

#include "ara/core/optional.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/core/vector.h"
#include "isoft/define.h"
#include "isoft/manifestreader/manifestreader_error_domain.h"
#include "isoft/manifestreader/tps_enumeration.h"
#include "rapidjson/document.h"
#define MR_HEADER(TYPE)                                                                                                \
    namespace isoft {                                                                                                  \
    namespace manifestreader {                                                                                         \
    template <>                                                                                                        \
    class Helper< TYPE > final                                                                                         \
    {                                                                                                                  \
    public:                                                                                                            \
        static int32_t Load(TYPE& result, rapidjson::Value const& v)                                                   \
        {                                                                                                              \
            if (!v.IsObject()) {                                                                                       \
                return static_cast< int32_t >(ManifestReaderErrc::kInvalidTypeRequested);                              \
            }

#define MR_FIELD(KEY, FIELDNAME)                                                                                       \
    {                                                                                                                  \
        if (!v.HasMember(KEY)) {                                                                                       \
            return static_cast< int32_t >(ManifestReaderErrc::kKeyNotFound);                                           \
        }                                                                                                              \
        ara::core::ErrorDomain::CodeType const err{                                                                    \
            Helper< decltype(result.FIELDNAME) >::Load(result.FIELDNAME, v[KEY])};                                     \
        if (err != isoft::kSuccess) {                                                                                  \
            return err;                                                                                                \
        }                                                                                                              \
    }

#define MR_OPTIONAL_FIELD(KEY, FIELDNAME)                                                                              \
    {                                                                                                                  \
        if (v.HasMember(KEY)) {                                                                                        \
            ara::core::ErrorDomain::CodeType const err{                                                                \
                Helper< decltype(result.FIELDNAME) >::Load(result.FIELDNAME, v[KEY])};                                 \
            if (err != isoft::kSuccess) {                                                                              \
                return err;                                                                                            \
            }                                                                                                          \
        }                                                                                                              \
    }

#define MR_FOOTER                                                                                                      \
    return isoft::kSuccess;                                                                                            \
    }                                                                                                                  \
    }                                                                                                                  \
    ;                                                                                                                  \
    }  /* namespace manifestreader*/                                                                                   \
    }  // namespace isoft

#define MR_FOOTER_WITH_VALID_CHECK()                                                                                   \
    if (!result.IsValid()) {                                                                                           \
        return static_cast< int32_t >(ManifestReaderErrc::kInvalidChecker);                                            \
    }                                                                                                                  \
    return isoft::kSuccess;                                                                                            \
    }                                                                                                                  \
    }                                                                                                                  \
    ;                                                                                                                  \
    }  /* namespace manifestreader*/                                                                                   \
    }  // namespace isoft

namespace isoft {
namespace manifestreader {
template < typename T, typename = void >
struct Helper
{
    // Template user expects the next function:
    // ara::core::Result<T> Get( rapidjson::Value const& v);
};
/// @brief
/// @tparam T
template < typename T >
struct Helper< ara::core::Optional< T > >
{
    static int32_t Load(ara::core::Optional< T >& ret, rapidjson::Value const& v)
    {
        T t;
        ara::core::ErrorDomain::CodeType const err{Helper< T >::Load(t, v)};
        if (err != isoft::kSuccess) {
            return static_cast< int32_t >(ManifestReaderErrc::kInvalidTypeRequested);
        }
        ret.emplace(t);
        return isoft::kSuccess;
    }
};
/// @brief
/// @tparam T
template < typename T >
class Helper< ara::core::Vector< T > >
{
public:
    static int32_t Load(ara::core::Vector< T >& ret, rapidjson::Value const& v)
    {
        if (!v.IsArray()) {
            return static_cast< int32_t >(ManifestReaderErrc::kInvalidTypeRequested);
        }
        ret.resize(v.Size());
        ara::core::ErrorDomain::CodeType err;
        for (rapidjson::SizeType i = 0; i < v.Size(); i++) {
            err = Helper< T >::Load(ret[i], v[i]);
            if (err != isoft::kSuccess) {
                return err;
            }
        }

        return isoft::kSuccess;
    }
};
/// @brief
template <>
class Helper< ara::core::String >
{
public:
    static int32_t Load(ara::core::String& ret, rapidjson::Value const& v)
    {
        if (!v.IsString()) {
            return static_cast< int32_t >(ManifestReaderErrc::kInvalidTypeRequested);
        }
        ret = v.GetString();
        return isoft::kSuccess;
    }
};
/// @brief
template <>
class Helper< uint64_t >
{
public:
    static int32_t Load(uint64_t& ret, rapidjson::Value const& v)
    {
        if (!v.IsUint64()) {
            return static_cast< int32_t >(ManifestReaderErrc::kInvalidTypeRequested);
        }
        ret = v.GetUint64();
        return isoft::kSuccess;
    }
};
/// @brief
template <>
class Helper< int64_t >
{
public:
    static int32_t Load(int64_t& ret, rapidjson::Value const& v)
    {
        if (!v.IsInt64()) {
            return static_cast< int32_t >(ManifestReaderErrc::kInvalidTypeRequested);
        }
        ret = v.GetInt64();
        return isoft::kSuccess;
    }
};
/// @brief
template <>
class Helper< double >
{
public:
    static int32_t Load(double& ret, rapidjson::Value const& v)
    {
        if (!v.IsNumber()) {
            return static_cast< int32_t >(ManifestReaderErrc::kInvalidTypeRequested);
        }
        ret = v.GetDouble();
        return isoft::kSuccess;
    }
};
/// @brief
template <>
class Helper< float >
{
public:
    static int32_t Load(float& ret, rapidjson::Value const& v)
    {
        if (!v.IsFloat()) {
            return static_cast< int32_t >(ManifestReaderErrc::kInvalidTypeRequested);
        }
        ret = v.GetFloat();
        return isoft::kSuccess;
    }
};
/// @brief
template <>
class Helper< bool >
{
public:
    static int32_t Load(bool& ret, rapidjson::Value const& v)
    {
        if (!v.IsBool()) {
            return static_cast< int32_t >(ManifestReaderErrc::kInvalidTypeRequested);
        }
        ret = v.GetBool();
        return isoft::kSuccess;
    }
};
/// @brief
/// @tparam TDataType
template < typename TDataType >
class Helper< TDataType,
              std::enable_if_t< std::is_same< TDataType, int32_t >::value || std::is_same< TDataType, int8_t >::value
                                || std::is_same< TDataType, int16_t >::value > >
{
public:
    static int32_t Load(TDataType& ret, rapidjson::Value const& v)
    {
        if (!v.IsInt()) {
            return static_cast< int32_t >(ManifestReaderErrc::kInvalidTypeRequested);
        }
        ret = static_cast< TDataType >(v.GetInt());
        return isoft::kSuccess;
    }
};
/// @brief
/// @tparam TDataType
template < typename TDataType >
class Helper< TDataType,
              std::enable_if_t< std::is_same< TDataType, uint32_t >::value || std::is_same< TDataType, uint8_t >::value
                                || std::is_same< TDataType, uint16_t >::value > >
{
public:
    static int32_t Load(TDataType& ret, rapidjson::Value const& v)
    {
        if (!v.IsUint()) {
            return static_cast< int32_t >(ManifestReaderErrc::kInvalidTypeRequested);
        }
        ret = static_cast< TDataType >(v.GetUint());
        return isoft::kSuccess;
    }
};
/// @brief
/// @tparam TpsEnums
template < typename TpsEnums >
class Helper<
    TpsEnums,
    std::enable_if_t< std::is_same< const char*, decltype(tps::ToString(std::declval< TpsEnums >())) >::value > >
{
public:
    static int32_t Load(TpsEnums& ret, rapidjson::Value const& v)
    {
        if (!v.IsString()) {
            return static_cast< int32_t >(ManifestReaderErrc::kInvalidTypeRequested);
        }
        const std::string t(v.GetString());
        if (!tps::FromString(t, ret)) {
            return static_cast< int32_t >(ManifestReaderErrc::kInvalidTypeRequested);
        }
        return isoft::kSuccess;
    }
};

// template helper
/// @brief
class ManifestNode;
/// @brief
/// @tparam T
/// @tparam Tagged
template < typename T, typename Tagged = void >
struct HasManifestLoader : std::false_type
{
};
/// @brief
/// @tparam T
template < typename T >
struct HasManifestLoader<
    T,
    std::enable_if_t<
        std::is_same< std::decay_t< decltype(std::declval< T >().ManifestLoader(std::declval< ManifestNode >())) >,
                      int32_t >::value > > : std::true_type
{
};
/// @brief
/// @tparam T
template < typename T >
using EnableManifestLoader = std::enable_if_t< HasManifestLoader< T >::value >;
/// @brief
/// @tparam T
template < typename T >
using DisableManifestLoader = std::enable_if_t< !HasManifestLoader< T >::value >;
}  // namespace manifestreader
}  // namespace isoft

#endif  // ISOFT_MANIFESTREADER_INCLUDE_PUBLIC_ISOFT_MANIFESTREADER_INTERNAL_READER_HELPER_H_
