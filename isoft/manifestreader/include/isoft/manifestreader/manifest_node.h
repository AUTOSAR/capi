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
/// @file       manifest_node.h
/// @brief
/// @details
/// @date       2021-04-14
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_MANIFESTREADER_INCLUDE_PUBLIC_ISOFT_MANIFESTREADER_MANIFEST_NODE_H_
#define ISOFT_MANIFESTREADER_INCLUDE_PUBLIC_ISOFT_MANIFESTREADER_MANIFEST_NODE_H_

#include <cstdint>
#include <functional>
#include <type_traits>
#include <utility>

#include "ara/core/optional.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/core/vector.h"
#include "isoft/manifestreader/internal/reader_helper.h"
#include "isoft/manifestreader/manifestreader_error_domain.h"
#include "rapidjson/document.h"

namespace isoft {
namespace manifestreader {

///
/// @brief Manifest Node access class
///        For usage examples, see ManifestNodeTest, ManifestStruct_SUCCESS
///
class ManifestNode
{
public:
    /// @brief load value to ret by key
    ///
    /// Supported standard types are: ara::core::Optional,bool, float,double, ara::core::String,
    /// int/uint(16/32/64)
    /// Examples of acceptable keys: "some_key" "some_key.some_other.target" "some_array[1].some_key"
    ///
    /// @tparam type of expected result
    /// @param key path to find the value
    /// @param ret dst
    /// @returns result with the isoft::kSuccess or ManifestReaderErrc
    template < typename T >
    int Load(ara::core::StringView key, T& ret, DisableManifestLoader< T >* = nullptr) const noexcept
    {
        if (key.empty())
            return Helper< T >::Load(ret, node_);

        const rapidjson::Value* node = nullptr;
        int err                      = GetValueByPath(key, &node);
        if (err != isoft::kSuccess) {
            return static_cast< int >(err);
        }
        return Helper< T >::Load(ret, *node);
    }
    /// @brief load value to ret by key
    ///
    /// Supported class of have member function :ManifestLoader
    /// Examples of acceptable keys:
    /// "some_key"
    /// "some_key.some_other.target"
    /// "some_array[1].some_key"
    ///
    /// @tparam type of expected result
    /// @param key path to find the value
    /// @param ret dst
    /// @returns result with the isoft::kSuccess or ManifestReaderErrc
    template < typename T >
    int Load(ara::core::StringView key, T& ret, EnableManifestLoader< T >* = nullptr) const noexcept
    {
        if (key.empty())
            return ret.ManifestLoader(*this);

        const rapidjson::Value* node = nullptr;
        int err                      = GetValueByPath(key, &node);
        if (err != isoft::kSuccess) {
            return static_cast< int >(err);
        }
        return ret.ManifestLoader(ManifestNode(*node));
    }

    /// @brief load value to ret by key
    ///
    /// Supported  ara::core::Vector<T> with T::ManifestLoader()
    /// Examples of acceptable keys:
    /// "some_key"
    /// "some_key.some_other.target"
    /// "some_array[1].some_key"
    ///
    /// @tparam type of expected result
    /// @param key path to find the value
    /// @param ret dst
    /// @returns result with the isoft::kSuccess or ManifestReaderErrc
    template < typename T >
    int Load(ara::core::StringView key,
             ara::core::Vector< T >& ret,
             EnableManifestLoader< T >* = nullptr) const noexcept
    {
        const rapidjson::Value* node = nullptr;
        int err                      = GetValueByPath(key, &node);
        if (err != isoft::kSuccess) {
            return static_cast< int >(err);
        }
        if (node->IsArray() == false) {
            return static_cast< int >(ManifestReaderErrc::kInvalidTypeRequested);
        }
        ret.resize(node->Size());
        for (rapidjson::SizeType i = 0; i < node->Size(); i++) {
            err = ret[i].ManifestLoader((*node)[i]);
            if (err != isoft::kSuccess) {
                return err;
            }
        }

        return isoft::kSuccess;
    }

    /// @brief receive value of type T by key
    ///
    /// Examples of acceptable keys:
    /// "some_key"
    /// "some_key.some_other.target"
    /// "some_array[1].some_key"
    ///
    /// @tparam type of expected result
    /// @param key path to find the value
    /// @param default return if key not exist
    /// @returns result with the data or default
    template < typename T >
    T GetValue(ara::core::StringView key, const T& valDefault) const noexcept
    {  // NOLINT
        T ret;
        if (isoft::kSuccess != Load(key, ret)) {
            ret = valDefault;
        }
        return ret;
    }

    /**
     * @brief Traverse array
     *
     * @param key
     * @param callback Access callback, returning false indicates the end of iteration
     * @return result with the isoft::kSuccess or ManifestReaderErrc
     */
    int IterateArray(ara::core::StringView key,
                     std::function< void(std::size_t idx, ManifestNode const&) > callback) const noexcept;

    /**
     * @brief Traverse object
     *
     * @param key
     * @param callback Access callback, returning false indicates the end of iteration
     * @return result with the isoft::kSuccess or ManifestReaderErrc
     */
    int IterateObject(
        ara::core::StringView key,
        std::function< void(const ara::core::StringView& memberName, ManifestNode const&) > callback) const noexcept;

    /// @brief receive array of values of type T by key
    ///
    /// Supported standard types are: bool, float, ara::core::String, int64_t, uint64_t
    /// Also usable with MR_FIELD_HEADER/_INFO/_FOOTER for parsing of custom structures
    /// Examples of acceptable keys:
    /// "some_key"
    /// "some_key.some_other.target"
    /// "some_array[1].some_key"
    ///
    /// @tparam type of expected result
    /// @param key path to find the value
    /// @returns result with the data array or error
    ///
    /// @note possible errors are: kParsing, kKeyNotFound and kInvalidTypeRequested
    template < typename T >
    [[deprecated]] ara::core::Result< ara::core::Vector< T > > GetArray(ara::core::StringView key) const noexcept
    {
        ara::core::Vector< T > ret;
        int err = Load(key, ret);
        if (err != isoft::kSuccess) {
            return ara::core::Result< ara::core::Vector< T > >::FromError(static_cast< ManifestReaderErrc >(err));
        }
        return ara::core::Result< ara::core::Vector< T > >::FromValue(std::move(ret));
    }
    /// @brief receive value of type T by key
    ///
    /// Supported standard types are: bool, float, ara::core::String, int64_t, uint64_t
    /// Also usable with MR_FIELD_HEADER/_INFO/_FOOTER for parsing of custom structures
    /// Examples of acceptable keys:
    /// "some_key"
    /// "some_key.some_other.target"
    /// "some_array[1].some_key"
    ///
    /// @tparam type of expected result
    /// @param key path to find the value
    /// @returns result with the data or error
    ///
    /// @note possible errors are: kParsing, kKeyNotFound and kInvalidTypeRequested
    template < typename T >
    [[deprecated]] ara::core::Result< T > GetValue(ara::core::StringView key) const noexcept
    {
        T ret;
        int err = Load(key, ret);
        if (err != isoft::kSuccess) {
            return ara::core::Result< T >::FromError(static_cast< ManifestReaderErrc >(err));
        }
        return ara::core::Result< T >::FromValue(std::move(ret));
    }

    // JSON type detection encapsulation
    bool IsArray() const { return node_.IsArray(); }
    bool IsObject() const { return node_.IsObject(); }
    bool IsString() const { return node_.IsString(); }
    bool IsNumber() const { return node_.IsNumber(); }
    bool IsInt() const { return node_.IsInt(); }
    bool IsFloat() const { return node_.IsFloat(); }
    bool IsDouble() const { return node_.IsDouble(); }

protected:
    ManifestNode(const rapidjson::Value& node) : node_(node) {}  // NOLINT
    virtual ~ManifestNode() = default;

private:
    const rapidjson::Value& node_;

    int GetValueByPath(ara::core::StringView key, const rapidjson::Value** valueNode) const noexcept;
};

}  // namespace manifestreader
}  // namespace isoft

#endif  // ISOFT_MANIFESTREADER_INCLUDE_PUBLIC_ISOFT_MANIFESTREADER_MANIFEST_NODE_H_
