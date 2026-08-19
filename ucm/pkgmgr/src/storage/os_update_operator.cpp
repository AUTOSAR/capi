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
/// @file       os_update_operator.cpp
/// @brief      OS update operator implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/SoftwareClusterManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=OSUpdateOperator
/// @unit_description=OS update operator implementation
/// @endcode
///
/// ================================================================

#include "os_update_operator.h"

#include "ara/ucm/internal/extraction/tinyfsys.h"
#include "ara/ucm/pkgmgr/impl_type_resulttype.h"
#include "common/const.h"
#include "common/log.h"

///#define WEXITSTATUS_UNSIGNED(status) (((static_cast<unsigned int>(status)) & 0xff00U) >> 8)

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief constructor
/// @param oSSwclDestinationPath
/// @throws no
OSUpdateOperator::OSUpdateOperator(AraString oSSwclDestinationPath)
    : dstPath_{std::move(oSSwclDestinationPath)}, scriptPath_{dstPath_ + kPathSeparator + kUpdateOSScriptName}
{
}

/// @brief PlaceToOSPatition
/// @param deltaUpdate
/// @return result
/// @throws no
AraResultVoid OSUpdateOperator::PlaceToOSPatition(bool const deltaUpdate) const
{
    LOGD << "call...";

    /// call updateOS process [-i] OSSwclDestinationPath_
    AraString tag;
    if (deltaUpdate) {
        tag = " -i ";
    }

    if (DoUpdateOSScript(scriptPath_, " process " + tag + dstPath_)) {
        return {};
    }

    return AraResultVoid::FromError(UCMErrorDomainErrc::kProcessedSoftwarePackageInconsistent);
}

/// @brief InvalidOSPatition
/// @param oSVersion
/// @return result
/// @throws no
AraResultVoid OSUpdateOperator::InvalidOSPatition(AraString const& oSVersion) const
{
    LOGD << "call...";
    if (DoUpdateOSScript(scriptPath_, " invalid " + oSVersion)) {
        return {};
    }
    return AraResultVoid::FromError(UCMErrorDomainErrc::kGeneralReject);
}

/// @brief ActiveOSPatition
/// @param oSVersion
/// @return result
/// @throws no
AraResultVoid OSUpdateOperator::ActiveOSPatition(AraString const& oSVersion) const
{
    LOGD << "call...";
    if (DoUpdateOSScript(scriptPath_, " active " + oSVersion)) {
        return {};
    }
    return AraResultVoid::FromError(UCMErrorDomainErrc::kGeneralReject);
}

/// @brief VerifyOSPatition
/// @param oSVersion
/// @return result
/// @throws no
AraResultVoid OSUpdateOperator::VerifyOSPatition(AraString const& oSVersion) const
{
    LOGD << "call...";
    if (DoUpdateOSScript(scriptPath_, " verify " + oSVersion)) {
        return {};
    }
    return AraResultVoid::FromError(UCMErrorDomainErrc::kGeneralReject);
}

/// @brief DoUpdateOSScript
/// @param scriptPath
/// @param scriptArg
/// @return bool
/// @throws no
bool OSUpdateOperator::DoUpdateOSScript(AraString const& scriptPath, AraString const& scriptArg)
{
    LOGD << "scriptPath:" << scriptPath.c_str() << "scriptArg:" << scriptArg.c_str();

    if (!tinyfsys::DoesFileExist(scriptPath)) {
        LOGE << "can't find scriptPath:" << scriptPath.c_str();
        return false;
    }

    AraString const cmd{scriptPath + scriptArg};
    std::int32_t const shellRet{
        system(cmd.c_str())};  // NOLINT : [system]function is not thread safe, QAC also does not recommend using system
    if (0 != shellRet) {
        LOGE << "failed to call command:" << cmd.c_str() << " with shellRet:" << shellRet;
        ///if (-1 == shellRet) {
        ///    LOGE << "further failure with errno:" << strerror(errno);
        ///} else if (0 < shellRet) {
        ///    LOGE << "further failure with WEXITSTATUS:" << WEXITSTATUS_UNSIGNED(shellRet);
        ///} else {
        ///    LOGE << "further failure with other err";
        ///}
        return false;
    }

    return true;
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
