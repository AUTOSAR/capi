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
/// @file       manifest_reader.h
/// @brief
/// @details
/// @date       2021-04-14
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_MANIFESTREADER_INCLUDE_PUBLIC_ISOFT_MANIFESTREADER_MANIFEST_READER_H_
#define ISOFT_MANIFESTREADER_INCLUDE_PUBLIC_ISOFT_MANIFESTREADER_MANIFEST_READER_H_

#include <memory>
#include <utility>

#include "ara/core/result.h"
#include "ara/core/string_view.h"
#include "isoft/manifestreader/manifest.h"

namespace isoft {
namespace manifestreader {

/// @brief open manifest from file at location defined by @path
///
/// @param path to manifest file
/// @returns result with Manifest object pointer or error
///
/// @note possible errors are: kParsing and kOpenFile
ara::core::Result< std::unique_ptr< Manifest > > OpenManifest(ara::core::StringView path) noexcept;

/// @brief open manifest from string
///
/// @param jsonString to json string
/// @returns result with Manifest object pointer or error
///
/// @note possible errors are: kParsing and kOpenFile
ara::core::Result< std::unique_ptr< Manifest > > ParseManifest(ara::core::StringView jsonString) noexcept;

/// @brief Load JSON string to C++ object
///
/// @param path to json string
/// @param dataObject[out] load manifest file into dataObject
/// @returns result with T object pointer or error
///
/// @note possible errors are: kParsing
template < typename T >
int LoadManifestFromString(ara::core::StringView jsonString, T& dataObject) noexcept
{  // NOLINT
    auto manifestRes = ParseManifest(jsonString);
    if (!manifestRes) {
        return static_cast< int >(manifestRes.Error().Value());
    }

    int err = manifestRes.Value()->Load("", dataObject);
    if (err != isoft::kSuccess) {
        return err;
    }
    return isoft::kSuccess;
}

/// @brief Load JSON file to C++ object
///
/// @param path to manifest file
/// @param dataObject[out] load manifest file into dataObject
/// @returns result with T object pointer or error
///
/// @note possible errors are: kParsing and kOpenFile
template < typename T >
int LoadManifestFromFile(ara::core::StringView path, T& dataObject) noexcept
{  // NOLINT
    auto manifestRes = OpenManifest(path);
    if (!manifestRes) {
        return static_cast< int >(manifestRes.Error().Value());
    }
    int err = manifestRes.Value()->Load< T >("", dataObject);
    if (err != isoft::kSuccess) {
        return err;
    }
    return isoft::kSuccess;
}

}  // namespace manifestreader
}  // namespace isoft

#endif  // ISOFT_MANIFESTREADER_INCLUDE_PUBLIC_ISOFT_MANIFESTREADER_MANIFEST_READER_H_
