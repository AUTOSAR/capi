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
/// @file       manifest_node.cpp
/// @brief
/// @details
/// @date       2021-04-14
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#include "isoft/manifestreader/manifest_node.h"

#include <algorithm>
#include <iostream>
#include <string>

#include "isoft/manifestreader/manifestreader_error_domain.h"
#include "rapidjson/pointer.h"

namespace isoft {
namespace manifestreader {

int ManifestNode::IterateArray(ara::core::StringView key,
                               std::function< void(std::size_t idx, ManifestNode const&) > callback) const noexcept
{
    const rapidjson::Value* node = nullptr;
    int err                      = GetValueByPath(key, &node);
    if (err != isoft::kSuccess) {
        return err;
    }

    if (!node->IsArray()) {
        return static_cast< int >(ManifestReaderErrc::kInvalidTypeRequested);
    }
    for (rapidjson::SizeType i = 0; i < node->Size(); i++) {
        callback(i, ManifestNode((*node)[i]));
    }
    return isoft::kSuccess;
}

int ManifestNode::IterateObject(
    ara::core::StringView key,
    std::function< void(const ara::core::StringView& memberName, ManifestNode const&) > callback) const noexcept
{
    const rapidjson::Value* node = nullptr;
    int err                      = GetValueByPath(key, &node);
    if (err != isoft::kSuccess) {
        return err;
    }

    if (!node->IsObject()) {
        return static_cast< int >(ManifestReaderErrc::kInvalidTypeRequested);
    }

    for (auto& m : node->GetObject()) {
        callback(ara::core::StringView(m.name.GetString(), m.name.GetStringLength()), ManifestNode(m.value));
    }

    return isoft::kSuccess;
}

int ManifestNode::GetValueByPath(ara::core::StringView key, const rapidjson::Value** valNode) const noexcept
{
    std::string path(key.begin(), key.end());

    if (!path.empty()) {
        std::replace(path.begin(), path.end(), '.', '/');
        std::replace(path.begin(), path.end(), '[', '/');
        path.erase(std::remove(path.begin(), path.end(), ']'), path.end());

        if (path.begin()[0] != '/') {
            path = std::string("/") + path;
        }
    }

    rapidjson::Pointer pointer(path.c_str());
    if (!pointer.IsValid()) {
        return static_cast< int >(ManifestReaderErrc::kParsing);
    }

    *valNode = pointer.Get(node_);
    if (*valNode == nullptr) {
        return static_cast< int >(ManifestReaderErrc::kKeyNotFound);
    }

    return isoft::kSuccess;
}

}  // namespace manifestreader
}  // namespace isoft
