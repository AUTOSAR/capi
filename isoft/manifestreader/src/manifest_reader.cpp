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
/// @file       manifest_reader.cpp
/// @brief
/// @details
/// @date       2021-04-14
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#include "isoft/manifestreader/manifest_reader.h"

#include <fstream>
#include <iostream>
#include <streambuf>

#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "isoft/manifestreader/manifest.h"
#include "isoft/manifestreader/manifestreader_error_domain.h"

namespace isoft {
namespace manifestreader {

ara::core::Result< std::unique_ptr< Manifest > > ParseManifest(ara::core::StringView jsonString) noexcept
{
    rapidjson::Document document;
    document.Parse(jsonString.data());

    if (document.HasParseError()) {
        return ara::core::Result< std::unique_ptr< Manifest > >::FromError(ManifestReaderErrc::kParsing);
    }
    return ara::core::Result< std::unique_ptr< Manifest > >::FromValue(
        std::make_unique< Manifest >(std::move(document)));
}

ara::core::Result< std::unique_ptr< Manifest > > OpenManifest(ara::core::StringView path) noexcept
{
    std::ifstream fileStream(path.data());

    if (!fileStream.is_open()) {
        return ara::core::Result< std::unique_ptr< Manifest > >::FromError(ManifestReaderErrc::kOpenFile);
    }

    ara::core::String str((std::istreambuf_iterator< char >(fileStream)), std::istreambuf_iterator< char >());

    return ParseManifest(str);
}

}  // namespace manifestreader
}  // namespace isoft
