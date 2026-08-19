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
/// @file       strtype.cpp
/// @brief      type convert to/from str implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/Utils
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=StrType
/// @unit_description=type convert to/from str implementation
/// @endcode
///
/// ================================================================

#include "strtype.h"

#include <cassert>

namespace ara {
namespace ucm {
namespace pkgmgr {
namespace strtype {

/// @brief Convert Enum Type To String
/// @param dictionary
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
template < typename TMap, typename TEnum >
static AraString EnumTypeToStr(TMap const& dictionary, TEnum const type)
{
    AraString stype{"UnknownType"};

    typename TMap::const_iterator const it{std::move(dictionary.find(type))};
    if (it != dictionary.end()) {
        stype = it->second;
    }

    return stype;
}

/// @brief get Enum Type from String
/// @param dictionary
/// @param defaultType
/// @param stype
/// @throws no
/// @return enum type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
template < typename TMap, typename TEnum >
static TEnum EnumTypeFromStr(TMap const& dictionary, TEnum const defaultType, AraString const& stype) noexcept
{
    TEnum type{defaultType};

    typename TMap::const_iterator it{std::move(dictionary.begin())};
    while (it != dictionary.end()) {
        if (stype == it->second) {
            type = it->first;
            break;
        }
        it++;
    }

    return type;
}

/// @brief ActionTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10409
/// @trace_id_dd=DD_UCM_11128
/// @needwork = no
/// @endcode
AraString ActionTypeToStr(ActionType const type)
{
    AraString stype{"UnknownType"};
    switch (type) {
        case ActionType::kInstall: {
            stype = "kInstall";
            break;
        }
        case ActionType::kRemove: {
            stype = "kRemove";
            break;
        }
        case ActionType::kUpdate: {
            stype = "kUpdate";
            break;
        }
        default: {
            break;
        }
    }
    return stype;
}
/// @brief ActionTypeFromStr
/// @param stype
/// @throws no
/// @return ActionType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10409
/// @trace_id_dd=DD_UCM_11129
/// @needwork = no
/// @endcode
ActionType ActionTypeFromStr(AraString const& stype)
{
    if (stype == "kInstall") {
        return ActionType::kInstall;
    }
    if (stype == "kUpdate") {
        return ActionType::kUpdate;
    }
    if (stype == "kRemove") {
        return ActionType::kRemove;
    }
    return ActionType::kInstall;
}

/// @brief ResultTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10410
/// @trace_id_dd=DD_UCM_11130
/// @needwork = no
/// @endcode
AraString ResultTypeToStr(ResultType const type)
{
    AraString stype{"UnknownType"};
    switch (type) {
        case ResultType::kSuccessfull: {
            stype = "kSuccessfull";
            break;
        }
        case ResultType::kFailed: {
            stype = "kFailed";
            break;
        }
        default: {
            break;
        }
    }
    return stype;
}
/// @brief ResultTypeFromStr
/// @param stype
/// @throws no
/// @return ResultType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10410
/// @trace_id_dd=DD_UCM_11131
/// @needwork = no
/// @endcode
ResultType ResultTypeFromStr(AraString const& stype)
{
    if (stype == "kSuccessfull") {
        return ResultType::kSuccessfull;
    }
    if (stype == "kFailed") {
        return ResultType::kFailed;
    }
    return ResultType::kFailed;
}

/// @brief ActivateOptionTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10411
/// @trace_id_dd=DD_UCM_11132
/// @needwork = no
/// @endcode
AraString ActivateOptionTypeToStr(ActivateOptionType const type)
{
    AraString stype{"UnknownType"};
    switch (type) {
        case ActivateOptionType::kWaitForReboot: {
            stype = "kWaitForReboot";
            break;
        }
        case ActivateOptionType::kRestartApplication: {
            stype = "kRestartApplication";
            break;
        }
        case ActivateOptionType::kReboot: {
            stype = "kReboot";
            break;
        }
        default: {
            break;
        }
    }
    return stype;
}

/// @brief FinalActionTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10412
/// @trace_id_dd=DD_UCM_11133
/// @needwork = no
/// @endcode
AraString FinalActionTypeToStr(FinalActionType const type)
{
    AraString stype{"UnknownType"};
    switch (type) {
        case FinalActionType::kRevert: {
            stype = "Revert";
            break;
        }
        case FinalActionType::kCommit: {
            stype = "Commit";
            break;
        }
        default: {
            break;
        }
    }
    return stype;
}

/// @brief PackageManagerStatusTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10413
/// @trace_id_dd=DD_UCM_11134
/// @needwork = no
/// @endcode
AraString PackageManagerStatusTypeToStr(PackageManagerStatusType const type)
{
    AraString stype{"UnknownType"};
    switch (type) {
        case PackageManagerStatusType::kIdle: {
            stype = "kIdle";
            break;
        }
        case PackageManagerStatusType::kProcessing: {
            stype = "kProcessing";
            break;
        }
        case PackageManagerStatusType::kReady: {
            stype = "kReady";
            break;
        }
        case PackageManagerStatusType::kActivating: {
            stype = "kActivating";
            break;
        }
        case PackageManagerStatusType::kVerifying: {
            stype = "kVerifying";
            break;
        }
        case PackageManagerStatusType::kActivated: {
            stype = "kActivated";
            break;
        }
        case PackageManagerStatusType::kRollingBack: {
            stype = "kRollingBack";
            break;
        }
        case PackageManagerStatusType::kRolledBack: {
            stype = "kRolledBack";
            break;
        }
        case PackageManagerStatusType::kCleaningUp: {
            stype = "kCleaningUp";
            break;
        }
        case PackageManagerStatusType::kExceptionRollingBack: {
            stype = "kExceptionRollingBack";
            break;
        }
        case PackageManagerStatusType::kExceptionCleaningUp: {
            stype = "kExceptionCleaningUp";
            break;
        }
        default: {
            break;
        }
    }
    return stype;
}

/// @brief SwPackageStateTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10414
/// @trace_id_dd=DD_UCM_11135
/// @needwork = no
/// @endcode
AraString SwPackageStateTypeToStr(SwPackageStateType const type)
{
    AraString stype{"UnknownType"};
    switch (type) {
        case SwPackageStateType::kTransferring: {
            stype = "kTransferring";
            break;
        }
        case SwPackageStateType::kTransferred: {
            stype = "kTransferred";
            break;
        }
        case SwPackageStateType::kProcessing: {
            stype = "kProcessing";
            break;
        }
        case SwPackageStateType::kProcessed: {
            stype = "kProcessed";
            break;
        }
        case SwPackageStateType::kProcessingStream: {
            stype = "kProcessingStream";
            break;
        }
        default: {
            break;
        }
    }
    return stype;
}

/// @brief SoftwareClusterDependencyCategoryEnumToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10415
/// @trace_id_dd=DD_UCM_11136
/// @needwork = no
/// @endcode
AraString SoftwareClusterDependencyCategoryEnumToStr(SoftwareClusterDependencyCategoryEnum const type)
{
    AraString stype{"UnknownType"};
    switch (type) {
        case SoftwareClusterDependencyCategoryEnum::kFunctionalDependency: {
            stype = "kFunctionalDependency";
            break;
        }
        case SoftwareClusterDependencyCategoryEnum::kStructuralDependency: {
            stype = "kStructuralDependency";
            break;
        }
        case SoftwareClusterDependencyCategoryEnum::kNotExist: {
            stype = "kNotExist";
            break;
        }
        default: {
            break;
        }
    }
    return stype;
}

/// @brief SoftwareClusterDependencyLogicalOperatorEnumToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10415
/// @trace_id_dd=DD_UCM_11137
/// @needwork = no
/// @endcode
AraString SoftwareClusterDependencyLogicalOperatorEnumToStr(SoftwareClusterDependencyLogicalOperatorEnum const type)
{
    AraString stype{"UnknownType"};
    switch (type) {
        case SoftwareClusterDependencyLogicalOperatorEnum::kLogicalAnd: {
            stype = "kLogicalAnd";
            break;
        }
        case SoftwareClusterDependencyLogicalOperatorEnum::kLogicalOr: {
            stype = "kLogicalOr";
            break;
        }
        case SoftwareClusterDependencyLogicalOperatorEnum::kNoOP: {
            stype = "kNoOP";
            break;
        }
        default: {
            break;
        }
    }
    return stype;
}

/// @brief SoftwareClusterDependencyOperatorEnumToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10415
/// @trace_id_dd=DD_UCM_11138
/// @needwork = no
/// @endcode
AraString SoftwareClusterDependencyOperatorEnumToStr(SoftwareClusterDependencyOperatorEnum const type)
{
    AraString stype{"UnknownType"};
    switch (type) {
        case SoftwareClusterDependencyOperatorEnum::kIsEqual: {
            stype = "kIsEqual";
            break;
        }
        case SoftwareClusterDependencyOperatorEnum::kIsGreaterThan: {
            stype = "kIsGreaterThan";
            break;
        }
        case SoftwareClusterDependencyOperatorEnum::kIsGreaterThanOrEqual: {
            stype = "kIsGreaterThanOrEqual";
            break;
        }
        case SoftwareClusterDependencyOperatorEnum::kIsLessThan: {
            stype = "kIsLessThan";
            break;
        }
        case SoftwareClusterDependencyOperatorEnum::kIsLessThanOrEqual: {
            stype = "kIsLessThanOrEqual";
            break;
        }
        default: {
            break;
        }
    }
    return stype;
}

/// @brief HelpExecutorOSUpdateTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10416
/// @trace_id_dd=DD_UCM_11139
/// @needwork = no
/// @endcode
AraString HelpExecutorOSUpdateTypeToStr(HelpExecutor::OSUpdateType const type)
{
    AraString stype{"UnknownType"};
    switch (type) {
        case HelpExecutor::OSUpdateType::kOSActivate: {
            stype = "kOSActivate";
            break;
        }
        case HelpExecutor::OSUpdateType::kOSVerify: {
            stype = "kOSVerify";
            break;
        }
        case HelpExecutor::OSUpdateType::kOSRollback: {
            stype = "kOSRollback";
            break;
        }
        case HelpExecutor::OSUpdateType::kOSExceptionRollback: {
            stype = "kOSExceptionRollback";
            break;
        }
        default: {
            break;
        }
    }
    return stype;
}

/// @brief HelpExecutorSwclUpdateTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10416
/// @trace_id_dd=DD_UCM_11140
/// @needwork = no
/// @endcode
AraString HelpExecutorSwclUpdateTypeToStr(HelpExecutor::SwclUpdateType const type)
{
    AraString stype{"UnknownType"};
    switch (type) {
        case HelpExecutor::SwclUpdateType::kSwclActivate: {
            stype = "kSwclActivate";
            break;
        }
        case HelpExecutor::SwclUpdateType::kSwclVerify: {
            stype = "kSwclVerify";
            break;
        }
        case HelpExecutor::SwclUpdateType::kSwclRollback: {
            stype = "kSwclRollback";
            break;
        }
        default: {
            break;
        }
    }
    return stype;
}

/// @brief convert TransferId to hex str
/// @param id TransferIdType value
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10417
/// @trace_id_dd=DD_UCM_11141
/// @needwork = no
/// @endcode
AraString TransferIdTypeToHexStr(TransferIdType const& id)
{
    std::vector< char8_t > const hexmap{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    std::size_t const len{id.size()};
    std::size_t i{0U};
    std::size_t d{0U};
    std::size_t const twoU{2U};
    std::size_t const high{0xF0U};
    std::size_t const fourU{4U};
    std::size_t const low{0x0FU};

    AraString ret(len * twoU, ' ');

    for (std::int32_t end{static_cast< std::int32_t >(len) - 1}; end >= 0; --end) {
        i                  = static_cast< std::size_t >(end);
        d                  = static_cast< std::size_t >(id[i]);
        ret[twoU * i]      = hexmap[(d & high) >> fourU];
        ret[twoU * i + 1U] = hexmap[(d & low)];
    }

    return ret;
}

/// @brief get TransferId from hex str
/// @param hex AraString value
/// @throws no
/// @return TransferIdType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10417
/// @trace_id_dd=DD_UCM_11142
/// @needwork = no
/// @endcode
TransferIdType TransferIdTypeFromHexStr(AraString const& hex)
{
    TransferIdType ret{};
    std::size_t const len{hex.size()};
    std::size_t i{0U};
    std::size_t const twoU{2U};
    std::int32_t const step{2};
    std::int32_t const hexNumber{16};

    assert(ret.size() * twoU == len);

    for (std::int32_t end{static_cast< std::int32_t >(len) - step}; end >= 0; end -= step) {
        i = static_cast< std::size_t >(end);
        AraString const byteStr{hex.substr(i, twoU)};
        ret[i / twoU] = static_cast< std::uint8_t >(strtol(byteStr.c_str(), nullptr, hexNumber));
    }

    return ret;
}

}  // namespace strtype
}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
