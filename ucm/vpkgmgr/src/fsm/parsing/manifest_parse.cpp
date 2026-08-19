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
/// @file       manifest_parse.cpp
/// @brief      ManifestParse implementation
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=ManifestParse
/// @unit_description=ManifestParse impl
/// @endcode
///
/// ================================================================

#include "fsm/parsing/manifest_parse.h"

#include <isoft/manifestreader/manifest_reader.h>
#include <isoft/manifestreader/manifestreader_error_domain.h>

#include "fsm/fsm_manager.h"
#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Parse
/// @param manifestFile
/// @return int
/// @throw
int32_t ManifestParse::Parse(ara::core::StringView const manifestFile)
{
    /*
    {
        "ucm":
        {
            "identifier": "DevelopmentMachineUCM"
        },
        "vucm":{
            "waitTime":,
            "blockSize":
        }
        ...
    }
    */
    // Parse swcls to get software package version number

    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > res{
        isoft::manifestreader::OpenManifest(manifestFile)};
    if (!res.HasValue()) {
        LOG_ERROR << "Error reading manifest=" << manifestFile.data() << " errmsg=" << res.Error().Message().data();
        return kVpkgError;
    }
    std::unique_ptr< isoft::manifestreader::Manifest > const rootNode{std::move(res).Value()};

    /// ara::core::String const ucmId = rootNode->GetValue<ara::core::String>(.
    ///     std::move(ara::core::StringView("ucm.identifier")), "none");.
    /// fsmPtr->SetUcmId(ucmId);.
    /// uint32_t const waitTime = rootNode->GetValue<uint32_t>("vucm.waitTime", 0U);.
    /// fsmPtr->SetWaitTime(waitTime);.

    uint32_t const blockSize{
        rootNode->GetValue< uint32_t >(std::move(ara::core::StringView("vucm.blockSize")), (40U * 1024U))};
    FsmManager::GetInstance()->SetBlockSize(blockSize);

    uint32_t const defaultValue{3000U};
    apiWaitTime_ = rootNode->GetValue< uint32_t >(std::move(ara::core::StringView("vucm.apiWaitTime")), defaultValue);

    return 0;
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara