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
/// @file       software_cluster_dependency.cpp
/// @brief      Implementation of the software set dependency class
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
/// @unit_name=SoftwareClusterDependency
/// @unit_description=Implementation of the software set dependency class
/// @endcode
///
/// ================================================================

#include "software_cluster_dependency.h"

#include "common/log.h"
#include "common/strtype.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief DependencyParseErrorMessage
/// @param errorCode
/// @return error message
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
AraString DependencyParseErrorMessage(std::int32_t const errorCode) noexcept
{
    AraString errmsg{"UnknownError"};

    switch (errorCode) {
        case kFormulaCategoryIllegal: {
            errmsg = "invalid category";
            break;
        }
        case kFormulaOperatorIllegal: {
            errmsg = "invalid operator";
            break;
        }
        case kCompCondCompareTypeIllegal: {
            errmsg = "invalid compareType";
            break;
        }
        case kCompCondVersionIllegal: {
            errmsg = "invalid version";
            break;
        }
        default: {
            break;
        }
    }

    return errmsg;
}

/// @brief ManifestLoader
/// @param node
/// @return int
/// @throws no
std::int32_t SoftwareClusterDependencyFormulaPart::ManifestLoader(RManifestNode const& node)
{
    std::int32_t ret{isoft::kSuccess};

    // Determine whether it is SoftwareClusterDependencyFormula or SoftwareClusterDependencyCompareCondition based on whether the compareType field exists
    AraString const compareTypeStr{RManifestNodeGetSS(node, kSWCLDepCompCondCompareType)};
    if (compareTypeStr.empty()) {  // It is SoftwareClusterDependencyFormula
        dependencyFormula_ = std::make_shared< SoftwareClusterDependencyFormula >();
        ret                = node.Load(std::move(AraStringView("")), *dependencyFormula_);
        if (isoft::kSuccess == ret) {
            partType_ = SWCLDependencyPartType::kFormula;
        } else {
            partType_ = SWCLDependencyPartType::kUnkown;
        }

    } else {  // It is SoftwareClusterDependencyCompareCondition
        dependencyCompareCondition_ = std::make_shared< SoftwareClusterDependencyCompareCondition >();
        ret                         = node.Load(std::move(AraStringView("")), *dependencyCompareCondition_);
        if (isoft::kSuccess == ret) {
            partType_ = SWCLDependencyPartType::kCompareCondition;
        } else {
            partType_ = SWCLDependencyPartType::kUnkown;
        }
    }

    return ret;
}

/// @brief Evaluate
/// @param sWCL2UpdatedVersionMap
/// @return bool
/// @throws no
bool SoftwareClusterDependencyFormulaPart::Evaluate(AraMap< AraString, Version > const& sWCL2UpdatedVersionMap)
{
    if (SWCLDependencyPartType::kFormula == partType_) {
        return dependencyFormula_->Evaluate(sWCL2UpdatedVersionMap);
    }
    if (SWCLDependencyPartType::kCompareCondition == partType_) {
        return dependencyCompareCondition_->Evaluate(sWCL2UpdatedVersionMap);
    };
    return false;
}

/// @brief DependedOn
/// @param swclFQN
/// @return bool
/// @throws no
bool SoftwareClusterDependencyFormulaPart::DependedOn(AraString const& swclFQN)
{
    if (SWCLDependencyPartType::kFormula == partType_) {
        return dependencyFormula_->DependedOn(swclFQN);
    }
    if (SWCLDependencyPartType::kCompareCondition == partType_) {
        return dependencyCompareCondition_->DependedOn(swclFQN);
    };

    return false;
}

/// @brief GetDependsOnSWCL
/// @param sWCL2DependencyFormulaMap
/// @return set of Depends On SWCLs
/// @throws no
std::set< AraString > SoftwareClusterDependencyFormulaPart::GetDependsOnSWCL(
    AraMap< AraString, SoftwareClusterDependencyFormula > const& sWCL2DependencyFormulaMap) const
{
    std::set< AraString > ret{};

    if (SWCLDependencyPartType::kFormula == partType_) {
        ret = dependencyFormula_->GetDependsOnSWCL(sWCL2DependencyFormulaMap);
    } else if (SWCLDependencyPartType::kCompareCondition == partType_) {
        ret = dependencyCompareCondition_->GetDependsOnSWCL(sWCL2DependencyFormulaMap);
    } else {
        ;
    }

    return ret;
}

/// @brief ManifestLoader
/// @param node
/// @return int
/// @throws no
std::int32_t SoftwareClusterDependencyFormula::ManifestLoader(RManifestNode const& node)
{
    // Parse the category field
    AraString const categoryTmp{RManifestNodeGetSS(node, kSWCLDepFormulaCategory)};
    if (0 == categoryTmp.compare(kSWCLDepFormulaCatFunctionalDep)) {
        this->category_ = SoftwareClusterDependencyCategoryEnum::kFunctionalDependency;
    } else if (0 == categoryTmp.compare(kSWCLDepFormulaCatStructuralDep)) {
        this->category_ = SoftwareClusterDependencyCategoryEnum::kStructuralDependency;
    } else if (categoryTmp.empty()) {
        this->category_ = SoftwareClusterDependencyCategoryEnum::kNotExist;
    } else {
        return kFormulaCategoryIllegal;
    }

    // Parse the operator field
    AraString const logicalOperatorTmp{RManifestNodeGetSS(node, kSWCLDepFormulaOperator)};
    if (0 == logicalOperatorTmp.compare(kSWCLDepFormulaOperatorAnd)) {
        this->logicalOperator_ = SoftwareClusterDependencyLogicalOperatorEnum::kLogicalAnd;
    } else if (0 == logicalOperatorTmp.compare(kSWCLDepFormulaOperatorOr)) {
        this->logicalOperator_ = SoftwareClusterDependencyLogicalOperatorEnum::kLogicalOr;
    } else if (logicalOperatorTmp.empty()) {
        this->logicalOperator_ = SoftwareClusterDependencyLogicalOperatorEnum::kNoOP;
    } else {
        return kFormulaOperatorIllegal;
    }

    // Parse the part field
    AraVector< SoftwareClusterDependencyFormulaPart > partsVec;
    const std::int32_t ret = node.Load(std::move(AraStringView(kSWCLDepFormulaParts)), partsVec);
    parts_                 = AraList< SoftwareClusterDependencyFormulaPart >{partsVec.begin(), partsVec.end()};
    return ret;
}

/// @brief Evaluate
/// @param sWCL2UpdatedVersionMap
/// @return bool
/// @throws no
bool SoftwareClusterDependencyFormula::Evaluate(AraMap< AraString, Version > const& sWCL2UpdatedVersionMap)
{
    /// SoftwareClusterDependencyCategoryEnum category = SoftwareClusterDependencyCategoryEnum::kNotExist;  //
    /// Can be: functionalDependency/structuralDependency/empty
    /// SoftwareClusterDependencyLogicalOperatorEnum logicalOperator_ =
    /// SoftwareClusterDependencyLogicalOperatorEnum::kNoOP; ara::core::Vector<SoftwareClusterDependencyFormulaPart> parts_;.
    LOGD << "start with category:" << strtype::SoftwareClusterDependencyCategoryEnumToStr(category_).c_str()
         << " logicalOperator:" << strtype::SoftwareClusterDependencyLogicalOperatorEnumToStr(logicalOperator_).c_str()
         << " isDependsOn_:" << isDependsOn_;

    // Default return is true
    bool ret{isDependsOn_};
    if ((logicalOperator_ == SoftwareClusterDependencyLogicalOperatorEnum::kLogicalAnd)
        || (logicalOperator_ == SoftwareClusterDependencyLogicalOperatorEnum::kNoOP)) {
        for (SoftwareClusterDependencyFormulaPart& part : parts_) {
            if (!part.Evaluate(sWCL2UpdatedVersionMap)) {
                ret = false;
                break;
            }
        }
    } else if (logicalOperator_ == SoftwareClusterDependencyLogicalOperatorEnum::kLogicalOr) {
        for (SoftwareClusterDependencyFormulaPart& part : parts_) {
            if (part.Evaluate(sWCL2UpdatedVersionMap)) {
                ret = true;
                break;
            }
            ret = false;
        }
    } else {
        ;
    }

    LOGD << "end with ret:" << ret;
    return ret;
}

/// @brief DependedOn
/// @param swclFQN
/// @return bool
/// @throws no
bool SoftwareClusterDependencyFormula::DependedOn(AraString const& swclFQN)
{
    bool ret{false};
    for (SoftwareClusterDependencyFormulaPart& part : parts_) {
        if (part.DependedOn(swclFQN)) {
            ret = true;
            break;
        }
    }
    return ret;
}

/// @brief GetDependsOnSWCL
/// @param sWCL2DependencyFormulaMap
/// @return set of Depends On SWCLs
/// @throws no
std::set< AraString > SoftwareClusterDependencyFormula::GetDependsOnSWCL(
    AraMap< AraString, SoftwareClusterDependencyFormula > const& sWCL2DependencyFormulaMap) const
{
    std::set< AraString > ret;
    for (SoftwareClusterDependencyFormulaPart const& part : parts_) {
        std::set< AraString > const partRet{part.GetDependsOnSWCL(sWCL2DependencyFormulaMap)};
        ret.insert(partRet.begin(), partRet.end());
    }
    return ret;
}

/// @brief ManifestLoader
/// @param node
/// @return int
/// @throws no
std::int32_t SoftwareClusterDependencyCompareCondition::ManifestLoader(RManifestNode const& node)
{
    // Parse the compareType field
    AraString const compareTypeStr{RManifestNodeGetSS(node, kSWCLDepCompCondCompareType)};
    if (0 == compareTypeStr.compare(kSWCLDepFormulaCompCondOperatorIsEqual)) {
        this->compareType = SoftwareClusterDependencyOperatorEnum::kIsEqual;
    } else if (0 == compareTypeStr.compare(kSWCLDepFormulaCompCondOperatorIsGreaterThan)) {
        this->compareType = SoftwareClusterDependencyOperatorEnum::kIsGreaterThan;
    } else if (0 == compareTypeStr.compare(kSWCLDepFormulaCompCondOperatorIsGreaterThanOrEqual)) {
        this->compareType = SoftwareClusterDependencyOperatorEnum::kIsGreaterThanOrEqual;
    } else if (0 == compareTypeStr.compare(kSWCLDepFormulaCompCondOperatorIsLessThan)) {
        this->compareType = SoftwareClusterDependencyOperatorEnum::kIsLessThan;
    } else if (0 == compareTypeStr.compare(kSWCLDepFormulaCompCondOperatorIsLessThanOrEqual)) {
        this->compareType = SoftwareClusterDependencyOperatorEnum::kIsLessThanOrEqual;
    } else {
        return kCompCondCompareTypeIllegal;
    }

    // Parse the considerBuildNumber field
    this->considerBuildNumber = node.GetValue(kSWCLDepCompCondConsiderBuildNumber, false);  //mytodo////

    // Parse the softwareCluster field
    this->swclFQN = RManifestNodeGetSS(node, kSWCLDepCompCondSWCL);

    // Parse the version field
    AraString const versionStr{RManifestNodeGetSS(node, kSWCLDepCompCondVersion)};
    try {
        swclVersion = Version(versionStr);  ///////////////mytodo666////remove exception//////
    } catch (std::invalid_argument const& e) {
        return kCompCondVersionIllegal;
    }

    return isoft::kSuccess;
}

/// @brief Evaluate
/// @param sWCL2UpdatedVersionMap
/// @return bool
/// @throws no
bool SoftwareClusterDependencyCompareCondition::Evaluate(
    AraMap< AraString, Version > const& sWCL2UpdatedVersionMap) const
{
    // Default return is true
    bool ret{true};

    LOGD << "start with compareType:" << strtype::SoftwareClusterDependencyOperatorEnumToStr(compareType).c_str()
         << " considerBuildNumber:" << considerBuildNumber << " swclFQN:" << swclFQN.c_str()
         << " swclVersion:" << swclVersion.ToString().c_str();

    if (swclFQN.empty()) {  // If swclFQN is empty, return true
        LOGD << "swclFQN is empty, end with ret:" << ret;
        return ret;
    }

    AraMap< AraString, Version >::const_iterator const it{sWCL2UpdatedVersionMap.find(swclFQN)};
    if (it == sWCL2UpdatedVersionMap.end()) {  // Does not exist, return false
        ret = false;
        LOGD << "can't find swclFQN:" << swclFQN.c_str() << ", end with ret:" << ret;
        return ret;
    }

    // Currently available version
    Version const updatedVersion{it->second};
    LOGD << "get updatedVersion:" << updatedVersion.ToString().c_str() << "for swclFQN:" << swclFQN.c_str();

    // Compare versions
    if (SoftwareClusterDependencyOperatorEnum::kIsEqual == compareType) {
        ret = (updatedVersion == swclVersion);
    } else if (SoftwareClusterDependencyOperatorEnum::kIsGreaterThan == compareType) {
        ret = (updatedVersion > swclVersion);
    } else if (SoftwareClusterDependencyOperatorEnum::kIsGreaterThanOrEqual == compareType) {
        ret = (updatedVersion >= swclVersion);
    } else if (SoftwareClusterDependencyOperatorEnum::kIsLessThan == compareType) {
        ret = (updatedVersion < swclVersion);
    } else if (SoftwareClusterDependencyOperatorEnum::kIsLessThanOrEqual == compareType) {
        ret = (updatedVersion <= swclVersion);
    } else {
        ;
    }

    LOGD << "end with ret:" << ret;
    return ret;
}

/// @brief DependedOn
/// @param swclFQN2Check
/// @return bool
/// @throws no
bool SoftwareClusterDependencyCompareCondition::DependedOn(AraString const& swclFQN2Check) const noexcept
{
    return (0 == swclFQN.compare(swclFQN2Check));
}

/// @brief GetDependsOnSWCL
/// @param sWCL2DependencyFormulaMap
/// @return set of Depends On SWCLs
/// @throws no
std::set< AraString > SoftwareClusterDependencyCompareCondition::GetDependsOnSWCL(
    AraMap< AraString, SoftwareClusterDependencyFormula > const& sWCL2DependencyFormulaMap) const
{
    std::set< AraString > ret;
    if (swclFQN.empty()) {  // If swclFQN is empty, return true
        return ret;
    }

    std::ignore = ret.insert(swclFQN);
    AraMap< AraString, SoftwareClusterDependencyFormula >::const_iterator const it{
        sWCL2DependencyFormulaMap.find(swclFQN)};
    if (it == sWCL2DependencyFormulaMap.end()) {  // Does not exist, return false
        return ret;
    }

    std::set< AraString > const retRecursive{it->second.GetDependsOnSWCL(sWCL2DependencyFormulaMap)};
    ret.insert(retRecursive.begin(), retRecursive.end());
    return ret;
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
