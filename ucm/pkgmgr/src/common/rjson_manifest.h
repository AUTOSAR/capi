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
/// @file       rjson_manifest.h
/// @brief      manifest reader which use rapid json
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/Utils
/// @module_path=/UCM/Utils
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=RJsonManifest
/// @unit_description=manifest reader which use rapid json
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_RJSON_MANIFEST_H_
#define ARA_UCM_PKGMGR_RJSON_MANIFEST_H_

#include "isoft/define.h"
#include "isoft/manifestreader/manifest_node.h"
#include "isoft/manifestreader/manifest_reader.h"
#include "log.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief manifestreader wrapper
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using RManifest = isoft::manifestreader::Manifest;
/// @brief unique_ptr of manifestreader
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using RManifestUPtr = std::unique_ptr< RManifest >;
/// @brief ManifestNode wrapper
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using RManifestNode = isoft::manifestreader::ManifestNode;
/// @brief ManifestReaderErrc wrapper
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using RManifestReaderErrc = isoft::manifestreader::ManifestReaderErrc;

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RManifestOpenAndCheck(doc, file, retFailedValue)                                                               \
    AraResult< RManifestUPtr > _openRet_{isoft::manifestreader::OpenManifest(std::move(AraStringView(file)))};         \
    if (!_openRet_.HasValue()) {                                                                                       \
        LOGE << (file).c_str() << " open err:" << _openRet_.Error().Message().data();                                  \
        return (retFailedValue);                                                                                       \
    }                                                                                                                  \
    RManifestUPtr const doc { std::move(std::move(_openRet_).Value()) }

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RManifestLoad_(rmanifestUPtr, key, storedValue)                                                                \
    rmanifestUPtr->Load(std::move(AraStringView(key)), (storedValue))

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RManifestLoad(rmanifestUPtr, key, storedValue)                                                                 \
    std::ignore = RManifestLoad_(rmanifestUPtr, key, storedValue);                                                     \
    std::ignore = (storedValue)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RManifestLoadResultStrictCheck(key, result, retFailedValue)                                                    \
    do {                                                                                                               \
        if (isoft::kSuccess != (result)) {                                                                             \
            LOGE << "invalid key:" << (key);                                                                           \
            return (retFailedValue);                                                                                   \
        }                                                                                                              \
    } while (false)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RManifestLoadResultCheck(key, result, retFailedValue)                                                          \
    do {                                                                                                               \
        bool const keyFound{static_cast< std::int32_t >(RManifestReaderErrc::kKeyNotFound) != (result)};               \
        if ((isoft::kSuccess != (result)) && keyFound) {                                                               \
            LOGE << "invalid key:" << (key);                                                                           \
            return (retFailedValue);                                                                                   \
        }                                                                                                              \
    } while (false)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RManifestLoadAndStrictCheck(rmanifestUPtr, key, storedValue, retFailedValue)                                   \
    do {                                                                                                               \
        std::int32_t const result{RManifestLoad_(rmanifestUPtr, key, storedValue)};                                    \
        std::ignore = (storedValue);                                                                                   \
        RManifestLoadResultStrictCheck(key, result, retFailedValue);                                                   \
    } while (false)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RManifestLoadAndCheck(rmanifestUPtr, key, storedValue, retFailedValue)                                         \
    do {                                                                                                               \
        std::int32_t const result{RManifestLoad_(rmanifestUPtr, key, storedValue)};                                    \
        std::ignore = (storedValue);                                                                                   \
        RManifestLoadResultCheck(key, result, retFailedValue);                                                         \
    } while (false)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RManifestDocGetI(doc, key, defaultV) (doc)->GetValue< std::int32_t >(std::move(AraStringView(key)), (defaultV))
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RManifestDocGetUI64(doc, key, defaultV)                                                                        \
    (doc)->GetValue< std::uint64_t >(std::move(AraStringView(key)), (defaultV))
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RManifestDocGetS(doc, key, defaultV) (doc)->GetValue< AraString >(std::move(AraStringView(key)), (defaultV))
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RManifestDocGetSS(doc, key) (doc)->GetValue< AraString >(std::move(AraStringView(key)), "")

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RManifestNodeGetI(node, key, defaultV)                                                                         \
    (node).GetValue< std::int32_t >(std::move(AraStringView(key)), (defaultV))
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RManifestNodeGetUI64(node, key, defaultV)                                                                      \
    (node).GetValue< std::uint64_t >(std::move(AraStringView(key)), (defaultV))
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RManifestNodeGetS(node, key, defaultV)                                                                         \
    std::move((node).GetValue< AraString >(std::move(AraStringView(key)), (defaultV)))
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RManifestNodeGetSS(node, key) (node).GetValue< AraString >(AraStringView(key), "")

namespace ara {
namespace ucm {
namespace pkgmgr {
namespace rmnf {

/// @brief manifestreader wrapper
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using Manifest = isoft::manifestreader::Manifest;
/// @brief unique_ptr of manifestreader
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using ManifestUPtr = std::unique_ptr< Manifest >;
/// @brief ManifestNode wrapper
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using ManifestNode = isoft::manifestreader::ManifestNode;
/// @brief ManifestReaderErrc wrapper
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using ManifestReaderErrc = isoft::manifestreader::ManifestReaderErrc;

/// @brief open Manifest file to get ManifestUPtr
/// @param file
/// @param ls
/// @return doc of ManifestUPtr
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10408
/// @trace_id_dd=DD_UCM_11143
/// @needwork = ad
/// @endcode
inline ManifestUPtr DocOpen(AraStringView const& file, ara::log::LogStream&& ls) noexcept
{
    ManifestUPtr mnf{nullptr};
    AraResult< ManifestUPtr > openRet{isoft::manifestreader::OpenManifest(file)};
    if (!openRet.HasValue()) {
        ls << file << " open errc:" << openRet.Error().Message().data();
        return mnf;
    }
    mnf = std::move(std::move(openRet).Value());
    return mnf;
}

/// @brief load value from Manifest, and do not check result of load
/// @param doc
/// @param key
/// @param storedValue
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10408
/// @trace_id_dd=DD_UCM_11144
/// @needwork = dd
/// @endcode
template < typename T >
inline void DocLoad(Manifest const& doc, AraStringView const& key, T& storedValue) noexcept
{
    std::ignore = doc.Load(key, storedValue);
}

/// @brief strictly check result of load
/// @param key
/// @param result
/// @param ls
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10408
/// @trace_id_dd=DD_UCM_11145
/// @needwork = dd
/// @endcode
inline bool DocLoadResultStrictCheck(AraStringView const& key,
                                     std::int32_t const result,
                                     ara::log::LogStream&& ls) noexcept
{
    if (isoft::kSuccess != result) {
        ls << "invalid key:" << key;
        return false;
    }
    return true;
}

/// @brief check result of load
/// @param key
/// @param result
/// @param ls
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10408
/// @trace_id_dd=DD_UCM_11146
/// @needwork = dd
/// @endcode
inline bool DocLoadResultCheck(AraStringView const& key, std::int32_t const result, ara::log::LogStream&& ls) noexcept
{
    if ((isoft::kSuccess != result) && (static_cast< std::int32_t >(RManifestReaderErrc::kKeyNotFound) != result)) {
        ls << "invalid key:" << key;
        return false;
    }
    return true;
}

/// @brief load value from Manifest, and strictly check result of load
/// @param doc
/// @param key
/// @param storedValue
/// @param ls
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10408
/// @trace_id_dd=DD_UCM_11147
/// @needwork = dd
/// @endcode
template < typename T >
inline bool DocLoadAndStrictCheck(Manifest const& doc,
                                  AraStringView const& key,
                                  T& storedValue,
                                  ara::log::LogStream&& ls) noexcept
{
    std::int32_t const result{doc.Load(key, storedValue)};
    if (isoft::kSuccess != result) {
        ls << "invalid key:" << key;
        return false;
    }
    return true;
}

/// @brief load value from Manifest, and check result of load
/// @param doc
/// @param key
/// @param storedValue
/// @param ls
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10408
/// @trace_id_dd=DD_UCM_11148
/// @needwork = dd
/// @endcode
template < typename T >
inline bool DocLoadAndCheck(Manifest const& doc,
                            AraStringView const& key,
                            T& storedValue,
                            ara::log::LogStream&& ls) noexcept
{
    std::int32_t const result{doc.Load(key, storedValue)};
    if ((isoft::kSuccess != result) && (static_cast< std::int32_t >(RManifestReaderErrc::kKeyNotFound) != result)) {
        ls << "invalid key:" << key;
        return false;
    }
    return true;
}

/// @brief get int32_t value from Manifest
/// @param doc
/// @param key
/// @param defaultValue
/// @return int32_t value
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10408
/// @trace_id_dd=DD_UCM_11149
/// @needwork = dd
/// @endcode
inline std::int32_t DocGetI(Manifest const& doc, AraStringView const& key, std::int32_t const defaultValue) noexcept
{
    return doc.GetValue< std::int32_t >(key, defaultValue);
}

/// @brief get uint64_t value from Manifest
/// @param doc
/// @param key
/// @param defaultValue
/// @return uint64_t value
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10408
/// @trace_id_dd=DD_UCM_11150
/// @needwork = dd
/// @endcode
inline std::uint64_t DocGetUI64(Manifest const& doc,
                                AraStringView const& key,
                                std::uint64_t const defaultValue) noexcept
{
    return doc.GetValue< std::uint64_t >(key, defaultValue);
}

/// @brief get string value from Manifest
/// @param doc
/// @param key
/// @param defaultValue
/// @return string value
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10408
/// @trace_id_dd=DD_UCM_11151
/// @needwork = dd
/// @endcode
inline AraString DocGetS(Manifest const& doc, AraStringView const& key, AraString const& defaultValue) noexcept
{
    return doc.GetValue< AraString >(key, defaultValue);
}

/// @brief get string value from Manifest
/// @param doc
/// @param key
/// @return string value
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10408
/// @trace_id_dd=DD_UCM_11152
/// @needwork = dd
/// @endcode
inline AraString DocGetSS(ManifestUPtr const& doc, AraStringView const& key) noexcept
{
    return doc->GetValue< AraString >(key, "");
}

/// @brief get int32_t value from ManifestNode
/// @param node
/// @param key
/// @param defaultValue
/// @return int32_t value
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10408
/// @trace_id_dd=DD_UCM_11153
/// @needwork = dd
/// @endcode
inline std::int32_t NodeGetI(ManifestNode const& node,
                             AraStringView const& key,
                             std::int32_t const defaultValue) noexcept
{
    return node.GetValue< std::int32_t >(key, defaultValue);
}

/// @brief get uint64_t value from ManifestNode
/// @param node
/// @param key
/// @param defaultValue
/// @return uint64_t value
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10408
/// @trace_id_dd=DD_UCM_11154
/// @needwork = dd
/// @endcode
inline std::uint64_t NodeGetUI64(ManifestNode const& node,
                                 AraStringView const& key,
                                 std::uint64_t const defaultValue) noexcept
{
    return node.GetValue< std::uint64_t >(key, defaultValue);
}

/// @brief get string value from ManifestNode
/// @param node
/// @param key
/// @param defaultValue
/// @return string value
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10408
/// @trace_id_dd=DD_UCM_11155
/// @needwork = dd
/// @endcode
inline AraString NodeGetS(ManifestNode const& node, AraStringView const& key, AraString const& defaultValue) noexcept
{
    return node.GetValue< AraString >(key, defaultValue);
}

/// @brief get string value from ManifestNode
/// @param node
/// @param key
/// @return string value
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10408
/// @trace_id_dd=DD_UCM_11156
/// @needwork = dd
/// @endcode
inline AraString NodeGetSS(ManifestNode const& node, AraStringView const& key) noexcept
{
    return node.GetValue< AraString >(key, "");
}

}  // namespace rmnf
}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_RJSON_MANIFEST_H_
