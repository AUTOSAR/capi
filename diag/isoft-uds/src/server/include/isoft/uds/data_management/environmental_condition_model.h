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
/// @file       environmental_condition_model.h
/// @brief      This file provides environmental_condition_model functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/data_management
/// @interface_level=software
/// @trace_id_sr=b3eca745-dbf2-4977-9fca-4995f2876d23
/// @unit_name=EnvironmentalConditionModel
/// @unit_description=This file provides environmental_condition_model functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_INCLUDE_ENVIRONMENTALCONDITIONMODEL_H_
#define ISOFT_UDS_SERVER_INCLUDE_ENVIRONMENTALCONDITIONMODEL_H_
#include <map>
#include <vector>

#include "diagnostic_data_model_manager.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Enumeration of logical operators
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01345
/// @trace_id_dd=DD_UDS_02637
/// @needwork = ad
/// @endcode
enum class DiagnosticLogicalOperatorEnum
{
    kLogicalAnd = 0,  // Logical AND
    kLogicalOr  = 1,  // Logical OR
};

/// @brief Enumeration of comparison types
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01346
/// @trace_id_dd=DD_UDS_02638
/// @needwork = ad
/// @endcode
enum class DiagnosticCompareTypeEnum
{
    kIsEqual          = 0,  // equal
    kIsNotEqual       = 1,  // not equal
    kIsLessThan       = 2,  // less than
    kIsLessOrEqual    = 3,  // less than or equal
    kIsGreaterThan    = 4,  // greater than
    kIsGreaterOrEqual = 5,  // greater than or equal
};

/// @brief Type of environmental condition ID
using EnvironmentalConditionId = std::int32_t;

/// @brief Define attribute and content of value
/// @param isArray Array identifier, identifies whether the current data is array type
/// @param type Type of value, includes common types
/// @param data Raw data, not parsed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01347
/// @trace_id_dd=DD_UDS_02639
/// @needwork = ad
/// @endcode
struct ValueSpecification
{
    bool isArray{false};                 /// Array identifier, identifies whether the current data is array type
    DataType type{DataType::kInt32};     /// Type of value, includes common types
    std::vector< std::uint8_t > data{};  /// Raw data, not parsed
};

/// @brief Base class of environmental condition formula part
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01348
/// @trace_id_dd=DD_UDS_02640
/// @needwork = ad
/// @endcode
struct EnvConditionFormulaPart
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01072
    /// @trace_id_dd=DD_UDS_02186
    /// @needwork = ad
    /// @endcode
    EnvConditionFormulaPart() noexcept = default;

    /// @brief Copy constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01073
    /// @trace_id_dd=DD_UDS_02187
    /// @needwork = ad
    /// @endcode
    EnvConditionFormulaPart(EnvConditionFormulaPart const&) = default;

    /// @brief Copy assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01074
    /// @trace_id_dd=DD_UDS_02188
    /// @needwork = ad
    /// @endcode
    EnvConditionFormulaPart& operator=(EnvConditionFormulaPart const&) = default;

    /// @brief Move constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01075
    /// @trace_id_dd=DD_UDS_02189
    /// @needwork = ad
    /// @endcode
    EnvConditionFormulaPart(EnvConditionFormulaPart&&) = default;

    /// @brief Move assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01076
    /// @trace_id_dd=DD_UDS_02190
    /// @needwork = ad
    /// @endcode
    EnvConditionFormulaPart& operator=(EnvConditionFormulaPart&&) = default;

public:
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01077
    /// @trace_id_dd=DD_UDS_02191
    /// @needwork = ad
    /// @endcode
    virtual ~EnvConditionFormulaPart() noexcept = default;
};

/// @brief Environmental condition data comparison part
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01349
/// @trace_id_dd=DD_UDS_02641
/// @needwork = ad
/// @endcode
struct EnvDataCondition : public EnvConditionFormulaPart
{
    DiagnosticCompareTypeEnum compareType{DiagnosticCompareTypeEnum::kIsEqual};  /// Comparison type
    ValueSpecification compareValue{};                                           /// Data for comparison
    std::uint32_t element{0U};  /// Mapped data element, used for comparison with compareValue field
};

/// @brief Environmental condition formula part
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01350
/// @trace_id_dd=DD_UDS_02642
/// @needwork = ad
/// @endcode
struct EnvConditionFormula : public EnvConditionFormulaPart
{
    std::uint8_t nrcValue{0U};                                                     /// Error code
    DiagnosticLogicalOperatorEnum op{DiagnosticLogicalOperatorEnum::kLogicalAnd};  /// Logical operator
    std::vector< std::shared_ptr< EnvConditionFormulaPart > > parts{};             /// Data waiting for comparison
};

/// @brief Environmental condition model
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01351
/// @trace_id_dd=DD_UDS_02643
/// @needwork = ad
/// @endcode
class EnvironmentalConditionModel
{
public:
    /// @brief Environmental condition formula
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01078
    /// @trace_id_dd=DD_UDS_02192
    /// @needwork = ad
    /// @endcode
    EnvConditionFormula formula{};

    /// @brief Environmental condition ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01079
    /// @trace_id_dd=DD_UDS_02193
    /// @needwork = ad
    /// @endcode
    EnvironmentalConditionId id{0U};
};

/// @brief Pointer type of environmental condition model
using EnvironmentalConditionModelPtr = std::shared_ptr< EnvironmentalConditionModel >;

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  ///ISOFT_UDS_SERVER_INCLUDE_ENVIRONMENTALCONDITIONMODEL_H_