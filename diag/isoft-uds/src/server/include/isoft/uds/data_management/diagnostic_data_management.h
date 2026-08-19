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
/// @file       diagnostic_data_management.h
/// @brief      This file provides diagnostic_data_management functionality
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
/// @trace_id_sr=d3e324f0-f252-48d6-9620-29e59453618b
/// @unit_name=DiagnosticDataManagement
/// @unit_description=This file provides diagnostic_data_management functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICCUSTOMMANAGEMENT121_H_
#define ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICCUSTOMMANAGEMENT121_H_
#include "environmental_condition_model.h"
#include "generic_data_identifier.h"
#include "isoft/uds/authentication_management/authentication_state.h"
#include "isoft/uds/common.h"
#include "isoft/uds/security_access_management/security_level.h"
#include "isoft/uds/session_management/session_model.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Structure for reading diagnostic data identifier
///
/// @param[in] accessPermissionAuth Access permission authentication.
/// @param[in] accessPermissionSession Access permission session.
/// @param[in] accessPermissionSecurityLevel Access permission security level.
/// @param[in] accessPermissionEnvCondition Access permission environmental condition.
/// @param[in] id Diagnostic data identifier.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct ReadDiagnosticDataByIdentifier
{
    /// @brief Access permission authentication.
    EnableAuth accessPermissionAuth{};
    /// @brief Access permission session.
    std::set< SessionId > accessPermissionSession{};
    /// @brief Access permission security level.
    std::set< SecurityLevelId > accessPermissionSecurityLevel{};
    /// @brief Access permission environmental condition.
    EnvironmentalConditionId accessPermissionEnvCondition{0};
    /// @brief Diagnostic data identifier.
    std::uint16_t id{0U};
};

/// @brief Structure for writing diagnostic data identifier
///
/// @param[in] accessPermissionAuth Access permission authentication.
/// @param[in] accessPermissionSession Access permission session.
/// @param[in] accessPermissionSecurityLevel Access permission security level.
/// @param[in] accessPermissionEnvCondition Access permission environmental condition.
/// @param[in] id Diagnostic data identifier.
/// @param[in] dataSize Data size.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct WriteDiagnosticDataByIdentifier
{
    /// @brief Access permission authentication.
    EnableAuth accessPermissionAuth{};
    /// @brief Access permission session.
    std::set< SessionId > accessPermissionSession{};
    /// @brief Access permission security level.
    std::set< SecurityLevelId > accessPermissionSecurityLevel{};
    /// @brief Access permission environmental condition.
    EnvironmentalConditionId accessPermissionEnvCondition{0};
    /// @brief Diagnostic data identifier.
    std::uint16_t id{0U};
    /// @brief Data size.
    std::uint32_t dataSize{0U};
};

/// @brief Structure for dynamically defining diagnostic data identifier
///
/// @param[in] accessPermissionAuth Access permission authentication.
/// @param[in] accessPermissionSession Access permission session.
/// @param[in] accessPermissionSecurityLevel Access permission security level.
/// @param[in] id Diagnostic data identifier.
/// @param[in] maxSourceElement Maximum number of source elements.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct DynamicDefinedDiagnosticData
{
    /// @brief Access permission authentication.
    EnableAuth accessPermissionAuth{};
    /// @brief Access permission session.
    std::set< SessionId > accessPermissionSession{};
    /// @brief Access permission security level.
    std::set< SecurityLevelId > accessPermissionSecurityLevel{};
    /// @brief Diagnostic data identifier.
    std::uint16_t id{0U};
    /// @brief Maximum number of source elements.
    std::uint32_t maxSourceElement{0U};
};

/// @brief Enumeration of DiagnosticPeriodicRateCategory
///
/// @param[in] type Period rate type.
/// @param[in] timeout Timeout.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
enum class DiagnosticPeriodicRateCategoryEnum : std::uint8_t
{
    /// @brief Slow period rate.
    kPeriodicRateSlow = 0x01,
    /// @brief Medium period rate.
    kPeriodicRateMedium,
    /// @brief Fast period rate.
    kPeriodicRateFast
};

/// @brief Structure of diagnostic data period rate
///
/// @param[in] type Period rate type.
/// @param[in] timeout Timeout.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct DiagnosticPeriodicRate
{
    /// @brief Period rate type.
    DiagnosticPeriodicRateCategoryEnum type{DiagnosticPeriodicRateCategoryEnum::kPeriodicRateSlow};
    /// @brief Timeout.
    std::uint32_t timeout{0U};
};

/// @brief Structure for reading diagnostic data identifier
///
/// @param[in] accessPermissionAuth Access permission authentication.
/// @param[in] accessPermissionSession Access permission session.
/// @param[in] accessPermissionSecurityLevel Access permission security level.
/// @param[in] accessPermissionEnvCondition Access permission environmental condition.
/// @param[in] id Diagnostic data identifier.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct ServiceX22Model : public ServiceConfigBase
{
    /// @brief Table of diagnostic data identifiers for reading.
    std::vector< ReadDiagnosticDataByIdentifier > table{};
    /// @brief Maximum number of diagnostic data identifiers to read.
    std::uint16_t maxDidToRead{0U};
    /// @brief Whether to check source ID.
    bool checkPerSourceId{false};
};

/// @brief Structure for writing diagnostic data identifier
///
/// @param[in] table Table of diagnostic data identifiers for writing.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct ServiceX2EModel : public ServiceConfigBase
{
    /// @brief Table of diagnostic data identifiers for writing.
    std::vector< WriteDiagnosticDataByIdentifier > table{};
};

/// @brief Structure of diagnostic data period rate
///
/// @param[in] schedulerMaxNumber Maximum number of schedulers.
/// @param[in] maxPeriodicDidToRead Maximum number of periodic diagnostic data identifiers to read.
/// @param[in] rates Table of diagnostic data period rates.
/// @param[in] checkPerSourceId Whether to check source ID.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct ServiceX2AModel : public ServiceConfigBase
{
    /// @brief Maximum number of schedulers.
    std::uint8_t schedulerMaxNumber{0U};
    /// @brief Maximum number of periodic diagnostic data identifiers to read.
    std::uint8_t maxPeriodicDidToRead{0U};
    /// @brief Table of diagnostic data period rates.
    std::vector< DiagnosticPeriodicRate > rates{};
    /// @brief Whether to check source ID.
    bool checkPerSourceId{false};
};

/// @brief Enumeration of subfunctions for dynamically defining diagnostic data identifier
///
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
enum class DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum : std::uint8_t
{
    /// @brief Define diagnostic data identifier.
    kDefineByIdentifier = 0x01,
    /// @brief Clear dynamically defined diagnostic data identifier.
    kClearDynamicallyDefineDataIdentifier = 0x03,
};

/// @brief Structure for dynamically defining diagnostic data identifier
///
/// @param[in] table Table of dynamically defined diagnostic data identifiers.
/// @param[in] checkPerSourceId Whether to check source ID.
/// @param[in] stored Whether to store dynamically defined diagnostic data identifiers.
/// @param[in] subfunction Subfunction of dynamically defined diagnostic data identifier.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct ServiceX2CModel : public ServiceConfigBase
{
    /// @brief Table of dynamically defined diagnostic data identifiers.
    std::vector< DynamicDefinedDiagnosticData > table{};
    /// @brief Whether to check source ID.
    bool checkPerSourceId{false};
    /// @brief Whether to store dynamically defined diagnostic data identifiers.
    bool stored{false};
    /// @brief Subfunction of dynamically defined diagnostic data identifier.
    std::set< DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum > subfunction{};
};

/// @brief Structure of service model for diagnostic data management
///
/// @param[in] serviceX22 Pointer to service X22.
/// @param[in] serviceX2E Pointer to service X2E.
/// @param[in] serviceX2A Pointer to service X2A.
/// @param[in] serviceX2C Pointer to service X2C.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct ServiceModel
{
    /// @brief Pointer to service X22.
    std::shared_ptr< ServiceX22Model > serviceX22{nullptr};
    /// @brief Pointer to service X2E.
    std::shared_ptr< ServiceX2EModel > serviceX2E{nullptr};
    /// @brief Pointer to service X2A.
    std::shared_ptr< ServiceX2AModel > serviceX2A{nullptr};
    /// @brief Pointer to service X2C.
    std::shared_ptr< ServiceX2CModel > serviceX2C{nullptr};
};

/// @brief Structure of DiagnosticDataManagement
///
/// @param[in] didManager Diagnostic data identifier manager.
/// @param[in] environmentalConditionManager Vector of environmental condition managers.
/// @param[in] service Service model.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct DiagnosticDataManagement
{
    /// @brief Diagnostic data identifier manager.
    DiagnosticDataModelManager didManager{};
    /// @brief Vector of environmental condition managers.
    std::vector< EnvironmentalConditionModelPtr > environmentalConditionManager{};
    /// @brief Service model.
    ServiceModel service{};
};
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  ///ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICCUSTOMMANAGEMENT_H_