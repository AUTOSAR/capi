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
/// @file       rjson.h
/// @brief      rapid json simple wrapper
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
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=RJson
/// @unit_description=rapid json simple wrapper
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_UTIL_RJSON_H_
#define ARA_UCM_PKGMGR_UTIL_RJSON_H_

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>

#include <fstream>

#include "common/alias.h"
#include "common/log.h"

namespace ara {
namespace ucm {
namespace pkgmgr {
namespace rjson {

/// @brief Doc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using Doc = rapidjson::Document;
/// @brief Value
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using Value = rapidjson::Value;

/// @brief make DomObject
/// @returns DomObject
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10056, AD_UCM_10102, AD_UCM_10307, AD_UCM_10311
/// @trace_id_dd=DD_UCM_10711
/// @needwork = no
/// @endcode
inline Doc MakeDomObject()
{
    Doc doc;
    std::ignore = doc.SetObject();
    return doc;
}

/// @brief make DomArray
/// @returns DomArray
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10712
/// @needwork = dd
/// @endcode
inline Doc MakeDomArray()
{
    Doc doc;
    std::ignore = doc.SetArray();
    return doc;
}

/// @brief make Object
/// @returns Object
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10713
/// @needwork = dd
/// @endcode
inline Value MakeObject() noexcept
{
    Value val{rapidjson::Type::kObjectType};
    return val;
}

/// @brief make Array
/// @returns Array
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10714
/// @needwork = dd
/// @endcode
inline Value MakeArray() noexcept
{
    Value val{rapidjson::Type::kArrayType};
    return val;
}

/// @brief AddKN
/// @param rootDom
/// @param curDom
/// @param key
/// @param num
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10715
/// @needwork = dd
/// @endcode
template < typename TDocOrValue, typename TNum >
inline void AddKN(Doc& rootDom, TDocOrValue& curDom, AraStringView const& key, TNum const num)
{
    std::ignore
        = curDom.AddMember(Value(key.data(), rootDom.GetAllocator()).Move(), Value(num).Move(), rootDom.GetAllocator());
}
/// @brief AddKN
/// @param rootDom
/// @param curDom
/// @param key
/// @param num
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10716
/// @needwork = dd
/// @endcode
template < typename TNum >
inline void AddKN(Doc& rootDom, Doc& curDom, AraStringView const& key, TNum const num)
{
    std::ignore
        = curDom.AddMember(Value(key.data(), rootDom.GetAllocator()).Move(), Value(num).Move(), rootDom.GetAllocator());
}
/// @brief AddKN
/// @param rootDom
/// @param curDom
/// @param key
/// @param num
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10717
/// @needwork = dd
/// @endcode
template < typename TNum >
inline void AddKN(Doc& rootDom, Value& curDom, AraStringView const& key, TNum const num)
{
    std::ignore
        = curDom.AddMember(Value(key.data(), rootDom.GetAllocator()).Move(), Value(num).Move(), rootDom.GetAllocator());
}

/// @brief AddKS
/// @param rootDom
/// @param curDom
/// @param key
/// @param str
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10718
/// @needwork = dd
/// @endcode
inline void AddKS(Doc& rootDom, Doc& curDom, AraStringView const& key, AraString const& str)
{
    std::ignore = curDom.AddMember(Value(key.data(), rootDom.GetAllocator()).Move(),
                                   Value(str.c_str(), rootDom.GetAllocator()).Move(), rootDom.GetAllocator());
}
/// @brief AddKS
/// @param rootDom
/// @param curDom
/// @param key
/// @param str
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10719
/// @needwork = dd
/// @endcode
inline void AddKS(Doc& rootDom, Value& curDom, AraStringView const& key, AraString const& str)
{
    std::ignore = curDom.AddMember(Value(key.data(), rootDom.GetAllocator()).Move(),
                                   Value(str.c_str(), rootDom.GetAllocator()).Move(), rootDom.GetAllocator());
}

/// @brief AddKO
/// @param rootDom
/// @param curDom
/// @param key
/// @param obj
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10720
/// @needwork = dd
/// @endcode
inline void AddKO(Doc& rootDom, Doc& curDom, AraStringView const& key, Value const& obj)
{
    std::ignore = curDom.AddMember(Value(key.data(), rootDom.GetAllocator()).Move(),
                                   Value(obj, rootDom.GetAllocator()).Move(), rootDom.GetAllocator());
}
/// @brief AddKO
/// @param rootDom
/// @param curDom
/// @param key
/// @param obj
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10721
/// @needwork = dd
/// @endcode
inline void AddKO(Doc& rootDom, Value& curDom, AraStringView const& key, Value const& obj)
{
    std::ignore = curDom.AddMember(Value(key.data(), rootDom.GetAllocator()).Move(),
                                   Value(obj, rootDom.GetAllocator()).Move(), rootDom.GetAllocator());
}

/// @brief PushN
/// @param rootDom
/// @param curDom
/// @param num
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10722
/// @needwork = dd
/// @endcode
template < typename TNum >
inline void PushN(Doc& rootDom, Doc& curDom, TNum const num)
{
    std::ignore = curDom.PushBack(num, rootDom.GetAllocator());
}
/// @brief PushN
/// @param rootDom
/// @param curDom
/// @param num
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10723
/// @needwork = dd
/// @endcode
template < typename TNum >
inline void PushN(Doc& rootDom, Value& curDom, TNum const num)
{
    std::ignore = curDom.PushBack(num, rootDom.GetAllocator());
}

/// @brief PushS
/// @param rootDom
/// @param curDom
/// @param str
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10724
/// @needwork = dd
/// @endcode
inline void PushS(Doc& rootDom, Doc& curDom, AraString const& str)
{
    std::ignore = curDom.PushBack(Value(str.c_str(), rootDom.GetAllocator()).Move(), rootDom.GetAllocator());
}
/// @brief PushS
/// @param rootDom
/// @param curDom
/// @param str
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10725
/// @needwork = dd
/// @endcode
inline void PushS(Doc& rootDom, Value& curDom, AraString const& str)
{
    std::ignore = curDom.PushBack(Value(str.c_str(), rootDom.GetAllocator()).Move(), rootDom.GetAllocator());
}

/// @brief PushO
/// @param rootDom
/// @param curDom
/// @param obj
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10726
/// @needwork = dd
/// @endcode
inline void PushO(Doc& rootDom, Doc& curDom, Value const& obj)
{
    std::ignore = curDom.PushBack(Value(obj, rootDom.GetAllocator()).Move(), rootDom.GetAllocator());
}
/// @brief PushO
/// @param rootDom
/// @param curDom
/// @param obj
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10727
/// @needwork = dd
/// @endcode
inline void PushO(Doc& rootDom, Value& curDom, Value const& obj)
{
    std::ignore = curDom.PushBack(Value(obj, rootDom.GetAllocator()).Move(), rootDom.GetAllocator());
}

/// @brief Save doc to file
/// @param rootDom
/// @param file
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10728
/// @needwork = dd
/// @endcode
inline void Save(Doc& rootDom, AraStringView const& file)
{
    std::ofstream ofs{file.data()};
    rapidjson::OStreamWrapper osw{ofs};
    rapidjson::Writer< rapidjson::OStreamWrapper > wrt{osw};
    std::ignore = rootDom.Accept(wrt);
}

/// @brief SavePretty
/// @param rootDom
/// @param file
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10729
/// @needwork = dd
/// @endcode
inline void SavePretty(Doc& rootDom, AraStringView const& file)
{
    std::ofstream ofs{file.data()};
    rapidjson::OStreamWrapper osw{ofs};
    rapidjson::PrettyWriter< rapidjson::OStreamWrapper > wrt{osw};
    std::ignore = rootDom.Accept(wrt);
}

/// @brief convert doc to string
/// @param rootDom
/// @returns string
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10730
/// @needwork = dd
/// @endcode
inline std::string Str(Doc& rootDom)
{
    rapidjson::StringBuffer buf;
    rapidjson::PrettyWriter< rapidjson::StringBuffer > wrt{buf};
    std::ignore = rootDom.Accept(wrt);
    std::string str{std::move(buf.GetString())};
    return str;
}

/// @brief OpenAndCheck
/// @param rootDom
/// @param file
/// @param ls
/// @returns bool and doc
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10264
/// @trace_id_dd=DD_UCM_10731
/// @needwork = dd
/// @endcode
bool OpenAndCheck(Doc& rootDom, AraStringView const& file, ara::log::LogStream&& ls);

}  // namespace rjson
}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_UTIL_RJSON_H_
