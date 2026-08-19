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
/// @file       software_cluster_dependency.h
/// @brief      Definition of the software set dependency class
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
/// @unit_name=SoftwareClusterDependency
/// @unit_description=Definition of the software set dependency class
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_SRC_TYPES_SOFTWARE_CLUSTER_DEPENDENCY_H_
#define ARA_UCM_PKGMGR_SRC_TYPES_SOFTWARE_CLUSTER_DEPENDENCY_H_

#include <cstdint>
#include <set>
#include <stdexcept>

#include "common/alias.h"
#include "common/const.h"
#include "common/rjson_manifest.h"
#include "parsing/version.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_10648
/// @needwork = no
/// @endcode
constexpr std::int32_t kFormulaCategoryIllegal{-1};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_10649
/// @needwork = no
/// @endcode
constexpr std::int32_t kFormulaOperatorIllegal{-2};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_10650
/// @needwork = no
/// @endcode
constexpr std::int32_t kCompCondCompareTypeIllegal{-3};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_10651
/// @needwork = no
/// @endcode
constexpr std::int32_t kCompCondVersionIllegal{-4};

/// @brief DependencyParseErrorMessage
/// @param errorCode
/// @return error message
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_10652
/// @needwork = no
/// @endcode
AraString DependencyParseErrorMessage(std::int32_t const errorCode) noexcept;

/// @brief SWCLDependencyPartType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10314
/// @trace_id_dd=DD_UCM_10653
/// @needwork = dd
/// @endcode
enum class SWCLDependencyPartType : std::uint8_t
{
    kFormula          = 0x00U,  // SoftwareClusterDependencyFormulaPart
    kCompareCondition = 0x01U,  // SoftwareClusterDependencyCompareCondition
    kUnkown           = 0x02U
};

/// @brief SoftwareClusterDependencyFormula
struct SoftwareClusterDependencyFormula;
/// @brief SoftwareClusterDependencyCompareCondition
struct SoftwareClusterDependencyCompareCondition;

/// @brief SoftwareClusterDependencyFormulaPart
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10314
/// @trace_id_dd=DD_UCM_10654
/// @needwork = ad
/// @endcode
class SoftwareClusterDependencyFormulaPart
{
public:
    /// @brief ManifestLoader
    /// @param node
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10655
    /// @needwork = dda
    /// @endcode
    std::int32_t ManifestLoader(RManifestNode const& node);

    /// @brief GetType
    /// @return SWCLDependencyPartType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10656
    /// @needwork = dda
    /// @endcode
    SWCLDependencyPartType GetType() const noexcept { return partType_; };

    /// @brief Evaluate
    /// @param sWCL2UpdatedVersionMap
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10657
    /// @needwork = dda
    /// @endcode
    bool Evaluate(AraMap< AraString, Version > const& sWCL2UpdatedVersionMap);

    /// @brief DependedOn
    /// @param swclFQN
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10658
    /// @needwork = dda
    /// @endcode
    bool DependedOn(AraString const& swclFQN);

    // Get DependsOn software set (recursively included)
    /// @brief GetDependsOnSWCL
    /// @param sWCL2DependencyFormulaMap
    /// @return set of Depends On SWCLs
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10659
    /// @needwork = dda
    /// @endcode
    std::set< AraString > GetDependsOnSWCL(
        AraMap< AraString, SoftwareClusterDependencyFormula > const& sWCL2DependencyFormulaMap) const;

    /// // copy constructor is deleted
    /// SoftwareClusterDependencyFormulaPart(const SoftwareClusterDependencyFormulaPart&) = delete;.

    /// SoftwareClusterDependencyFormulaPart(SoftwareClusterDependencyFormulaPart&& other).
    ///     : partType_(std::move(other.partType_))
    ///     , dependencyFormula_(std::move(other.dependencyFormula_))
    ///     , dependencyCompareCondition_(std::move(other.dependencyCompareCondition_))
    /// {}

    /// // copy assignment operator is deleted
    /// SoftwareClusterDependencyFormulaPart& operator=(const SoftwareClusterDependencyFormulaPart&) = delete;

    /// SoftwareClusterDependencyFormulaPart& operator=(SoftwareClusterDependencyFormulaPart&& rhs)
    /// {
    ///     partType_ = std::move(rhs.partType_);
    ///     dependencyFormula_ = std::move(rhs.dependencyFormula_);
    ///     dependencyCompareCondition_ = std::move(rhs.dependencyCompareCondition_);
    /// }

private:
    /// @brief partType_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10660
    /// @needwork = dda
    /// @endcode
    SWCLDependencyPartType partType_{SWCLDependencyPartType::kUnkown};
    /// @brief dependencyFormula_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10661
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< SoftwareClusterDependencyFormula > dependencyFormula_{nullptr};
    /// @brief dependencyCompareCondition_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10662
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< SoftwareClusterDependencyCompareCondition > dependencyCompareCondition_{nullptr};
};

/// @brief SoftwareClusterDependencyCategoryEnum
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10315
/// @trace_id_dd=DD_UCM_10663
/// @needwork = dd
/// @endcode
enum class SoftwareClusterDependencyCategoryEnum : std::uint8_t
{
    kFunctionalDependency = 0x00U,
    kStructuralDependency = 0x01U,
    kNotExist             = 0x02U
};

/// @brief SoftwareClusterDependencyLogicalOperatorEnum
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10315
/// @trace_id_dd=DD_UCM_10664
/// @needwork = dd
/// @endcode
enum class SoftwareClusterDependencyLogicalOperatorEnum : std::uint8_t
{
    kLogicalAnd = 0x00U,
    kLogicalOr  = 0x01U,
    kNoOP       = 0x02U
};

// "dependsOn": [
//       { // SoftwareClusterDependencyFormula
//           "category": "functionalDependency/structuralDependency",.  // may not exist.
//           "operator": "and/or",.      // may not exist.
//           "part": [ // may not exist, can consider this SoftwareClusterDependencyFormula as true
//               { // SoftwareClusterDependencyCompareCondition
//                   "compareType": "isEqual/isGreaterThan/isGreaterThanOrEqual/isLessThan/isLessThanOrEqual/",.
//                   "considerBuildNumber": "true/false",.
//                   "softwareCluster": "swcl1",.   // may not exist???.
//                   "version": "0.1.1"
//               }
//           ]
//       }
//   ],
/// @brief SoftwareClusterDependencyFormula
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10315
/// @trace_id_dd=DD_UCM_10665
/// @needwork = ad
/// @endcode
class SoftwareClusterDependencyFormula
{
public:
    /// @brief create SoftwareClusterDependencyFormula
    /// @param isDependsOn
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10666
    /// @needwork = dda
    /// @endcode
    explicit SoftwareClusterDependencyFormula(bool const isDependsOn = true) noexcept : isDependsOn_{isDependsOn} {}

    /// @brief ManifestLoader
    /// @param node
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10667
    /// @needwork = dda
    /// @endcode
    std::int32_t ManifestLoader(RManifestNode const& node);

    // Calculate the result of this Formula
    /// @brief Evaluate
    /// @param sWCL2UpdatedVersionMap
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10668
    /// @needwork = dda
    /// @endcode
    bool Evaluate(AraMap< AraString, Version > const& sWCL2UpdatedVersionMap);

    // Check if it depends on a software set named swclFQN
    /// @brief DependedOn
    /// @param swclFQN
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10669
    /// @needwork = dda
    /// @endcode
    bool DependedOn(AraString const& swclFQN);

    // Get DependsOn software set (recursively included)
    /// @brief GetDependsOnSWCL
    /// @param sWCL2DependencyFormulaMap
    /// @return set of Depends On SWCLs
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10670
    /// @needwork = dda
    /// @endcode
    std::set< AraString > GetDependsOnSWCL(
        AraMap< AraString, SoftwareClusterDependencyFormula > const& sWCL2DependencyFormulaMap) const;

private:
    /// @brief category_
    /// Can be: functionalDependency/structuralDependency/empty
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10671
    /// @needwork = dda
    /// @endcode
    SoftwareClusterDependencyCategoryEnum category_{SoftwareClusterDependencyCategoryEnum::kNotExist};
    /// @brief logicalOperator_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10672
    /// @needwork = dda
    /// @endcode
    SoftwareClusterDependencyLogicalOperatorEnum logicalOperator_{SoftwareClusterDependencyLogicalOperatorEnum::kNoOP};
    /// @brief parts_
    ///AraVector<std::unique_ptr<SoftwareClusterDependencyFormulaPart>> parts_;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10673
    /// @needwork = dda
    /// @endcode
    AraList< SoftwareClusterDependencyFormulaPart > parts_{};

private:
    /// @brief isDependsOn_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10674
    /// @needwork = dda
    /// @endcode
    bool isDependsOn_{false};  // If true, it means dependsOn; if false, it means conflictsTo
};

/// @brief SoftwareClusterDependencyOperatorEnum
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10316
/// @trace_id_dd=DD_UCM_10675
/// @needwork = dd
/// @endcode
enum class SoftwareClusterDependencyOperatorEnum : std::uint8_t
{
    kIsEqual              = 0x00U,
    kIsGreaterThan        = 0x01U,
    kIsGreaterThanOrEqual = 0x02U,
    kIsLessThan           = 0x03U,
    kIsLessThanOrEqual    = 0x04U
};

/// @brief SoftwareClusterDependencyCompareCondition
/// @code{.isoft}
///   { // SoftwareClusterDependencyCompareCondition
///       "compareType": "isEqual/isGreaterThan/isGreaterThanOrEqual/isLessThan/isLessThanOrEqual/",
///       "considerBuildNumber": "true/false",
///       "softwareCluster": "swcl1",   // may not exist???
///       "version": "0.1.1"
///   }
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10316
/// @trace_id_dd=DD_UCM_10676
/// @needwork = ad
/// @endcode
class SoftwareClusterDependencyCompareCondition
{
public:
    /// @brief Software
    /// isEqual/isGreaterThan/isGreaterThanOrEqual/isLessThan/isLessThanOrEqual
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10677
    /// @needwork = dda
    /// @endcode
    SoftwareClusterDependencyOperatorEnum compareType{SoftwareClusterDependencyOperatorEnum::kIsEqual};
    /// @brief considerBuildNumber
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10678
    /// @needwork = dda
    /// @endcode
    bool considerBuildNumber{false};
    /// @brief swclFQN
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10679
    /// @needwork = dda
    /// @endcode
    AraString swclFQN{""};
    /// @brief swclVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10680
    /// @needwork = dda
    /// @endcode
    Version swclVersion{0, 0, 0};

    /// @brief ManifestLoader
    /// @param node
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10681
    /// @needwork = dda
    /// @endcode
    std::int32_t ManifestLoader(RManifestNode const& node);

    // Calculate the result of this CompareCondition
    /// @brief Evaluate
    /// @param sWCL2UpdatedVersionMap
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10682
    /// @needwork = dda
    /// @endcode
    bool Evaluate(AraMap< AraString, Version > const& sWCL2UpdatedVersionMap) const;

    // Check if it depends on a software set named swclFQN
    /// @brief DependedOn
    /// @param swclFQN2Check
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10683
    /// @needwork = dda
    /// @endcode
    bool DependedOn(AraString const& swclFQN2Check) const noexcept;

    // Get DependsOn software set (recursively included)
    /// @brief GetDependsOnSWCL
    /// @param sWCL2DependencyFormulaMap
    /// @return set of Depends On SWCLs
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10684
    /// @needwork = dda
    /// @endcode
    std::set< AraString > GetDependsOnSWCL(
        AraMap< AraString, SoftwareClusterDependencyFormula > const& sWCL2DependencyFormulaMap) const;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_SRC_TYPES_SOFTWARE_CLUSTER_DEPENDENCY_H_
