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
/// @file       uds_config.cpp
/// @brief      This file provides the Implementation of Configuration File Parsing class
/// @details
/// @date       2024-11-29
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "uds_config.h"

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/tps_enumeration.h>
#ifdef NMODEL
    #include <isoft/nml/definition/diagnostic_authentication_port.h>
    #include <isoft/nml/definition/diagnostic_communication_control_port.h>
    #include <isoft/nml/definition/diagnostic_condition_port.h>
    #include <isoft/nml/definition/diagnostic_data_port.h>
    #include <isoft/nml/definition/diagnostic_download_service_port.h>
    #include <isoft/nml/definition/diagnostic_dtc_information_port.h>
    #include <isoft/nml/definition/diagnostic_ecu_reset_request_port.h>
    #include <isoft/nml/definition/diagnostic_event_port.h>
    #include <isoft/nml/definition/diagnostic_external_authentication_port.h>
    #include <isoft/nml/definition/diagnostic_file_transfer_service_port.h>
    #include <isoft/nml/definition/diagnostic_generic_uds_service_port.h>
    #include <isoft/nml/definition/diagnostic_indicator_port.h>
    #include <isoft/nml/definition/diagnostic_monitor_port.h>
    #include <isoft/nml/definition/diagnostic_operation_cycle_port.h>
    #include <isoft/nml/definition/diagnostic_routine_port.h>
    #include <isoft/nml/definition/diagnostic_security_access_port.h>
    #include <isoft/nml/definition/diagnostic_service_validation_port.h>
    #include <isoft/nml/definition/diagnostic_upload_service_port.h>
    #include <isoft/nml/definition/sw_base_type.h>
    #include <isoft/nml/definition/uds_aging.h>
    #include <isoft/nml/definition/uds_clear_diagnostic_information_service.h>
    #include <isoft/nml/definition/uds_communication_control_service.h>
    #include <isoft/nml/definition/uds_communication_control_subfunction.h>
    #include <isoft/nml/definition/uds_compare_type_enum.h>
    #include <isoft/nml/definition/uds_condition.h>
    #include <isoft/nml/definition/uds_condition_group.h>
    #include <isoft/nml/definition/uds_connected_indicator.h>
    #include <isoft/nml/definition/uds_control_dtc_setting_service.h>
    #include <isoft/nml/definition/uds_custom_service.h>
    #include <isoft/nml/definition/uds_data_element.h>
    #include <isoft/nml/definition/uds_data_identifier.h>
    #include <isoft/nml/definition/uds_data_identifier_set.h>
    #include <isoft/nml/definition/uds_data_management.h>
    #include <isoft/nml/definition/uds_download_service.h>
    #include <isoft/nml/definition/uds_dtc.h>
    #include <isoft/nml/definition/uds_dtc_group.h>
    #include <isoft/nml/definition/uds_dynamically_define_data_identifier.h>
    #include <isoft/nml/definition/uds_dynamically_define_data_identifier_service.h>
    #include <isoft/nml/definition/uds_dynamically_define_data_identifier_subfunction_enum.h>
    #include <isoft/nml/definition/uds_ecu_reset_service.h>
    #include <isoft/nml/definition/uds_ecu_reset_service_subfunction.h>
    #include <isoft/nml/definition/uds_environmental_condition.h>
    #include <isoft/nml/definition/uds_environmental_condition_formula.h>
    #include <isoft/nml/definition/uds_environmental_data_element_condition.h>
    #include <isoft/nml/definition/uds_event.h>
    #include <isoft/nml/definition/uds_event_debounce_counter_based.h>
    #include <isoft/nml/definition/uds_event_debounce_time_based.h>
    #include <isoft/nml/definition/uds_event_window_time_enum.h>
    #include <isoft/nml/definition/uds_extended_data_record.h>
    #include <isoft/nml/definition/uds_fault_management.h>
    #include <isoft/nml/definition/uds_fault_memory.h>
    #include <isoft/nml/definition/uds_file_transfer_service.h>
    #include <isoft/nml/definition/uds_freeze_frame.h>
    #include <isoft/nml/definition/uds_generic_service_subfunction.h>
    #include <isoft/nml/definition/uds_indicator.h>
    #include <isoft/nml/definition/uds_logical_operator_enum.h>
    #include <isoft/nml/definition/uds_operation_cycle.h>
    #include <isoft/nml/definition/uds_periodic_rate.h>
    #include <isoft/nml/definition/uds_periodic_rate_category_enum.h>
    #include <isoft/nml/definition/uds_permission_group.h>
    #include <isoft/nml/definition/uds_read_data_by_identifier.h>
    #include <isoft/nml/definition/uds_read_data_by_identifier_service.h>
    #include <isoft/nml/definition/uds_read_data_by_periodic_identifier_service.h>
    #include <isoft/nml/definition/uds_read_dtc_information_service.h>
    #include <isoft/nml/definition/uds_read_dtc_information_subfunction.h>
    #include <isoft/nml/definition/uds_response_on_event_action_enum.h>
    #include <isoft/nml/definition/uds_response_on_event_service.h>
    #include <isoft/nml/definition/uds_response_on_event_subfunction.h>
    #include <isoft/nml/definition/uds_response_to_ecu_reset_enum.h>
    #include <isoft/nml/definition/uds_routine.h>
    #include <isoft/nml/definition/uds_routine_control_service.h>
    #include <isoft/nml/definition/uds_routine_subfunction.h>
    #include <isoft/nml/definition/uds_security_access_service.h>
    #include <isoft/nml/definition/uds_security_access_subfunction.h>
    #include <isoft/nml/definition/uds_security_level.h>
    #include <isoft/nml/definition/uds_server.h>
    #include <isoft/nml/definition/uds_service_validation_instance.h>
    #include <isoft/nml/definition/uds_session.h>
    #include <isoft/nml/definition/uds_session_control_service.h>
    #include <isoft/nml/definition/uds_session_control_subfunction.h>
    #include <isoft/nml/definition/uds_transfer_management.h>
    #include <isoft/nml/definition/uds_upload_service.h>
    #include <isoft/nml/definition/uds_validation.h>
    #include <isoft/nml/definition/uds_write_data_by_identifier.h>
    #include <isoft/nml/definition/uds_write_data_by_identifier_service.h>
#endif
#include <cmath>
#ifdef NMODEL
    #include "hash/hash.h"
#endif
#include "authentication.h"
#include "communication_control.h"
#include "condition.h"
#include "download.h"
#include "dtc_information.h"
#include "ecu_reset_request.h"
#include "event.h"
#include "file_transfer.h"
#include "generic_data_element.h"
#include "generic_data_identifier.h"
#include "generic_routine.h"
#include "generic_uds_service.h"
#include "indicator.h"
#include "log/log.h"
#include "monitor.h"
#include "operation_cycle.h"
#include "security_access.h"
#include "service_validation.h"
#include "upload.h"
namespace ara {
namespace diag {
namespace dmd {

namespace tps   = isoft::manifestreader::tps;
namespace fault = isoft::uds::server;
using ara::diag::common::LogDebug;
using ara::diag::common::LogError;
using ara::diag::common::LogInfo;
using ara::diag::common::LogVerbose;
using ara::diag::common::LogWarn;
using isoft::uds::server::ControlDTCSettingInstance;
using isoft::uds::server::ControlDTCSettingSubfunction;
using isoft::uds::server::ReadDTCInformationInstance;
using isoft::uds::server::ReadDTCInformationSubfunction;

uint32_t SecondToMs(double second) noexcept
{
    double const msPerSecond{1000.0};
    double ms{};
    ms = second * msPerSecond;
    return std::lround(std::trunc(ms));
}
#ifdef NMODEL
isoft::uds::server::DataType GetBaseType(
    std::shared_ptr< isoft::nml::definition::SwBaseType > baseTypePtr) noexcept  /// NOLINT
{
    isoft::uds::server::DataType type{isoft::uds::server::DataType::kUint8};
    if (baseTypePtr.get() == nullptr) {
        return type;
    }

    if (baseTypePtr->baseTypeEncoding.get() != nullptr && baseTypePtr->baseTypeSize.get() != nullptr) {
        if (*baseTypePtr->baseTypeEncoding == isoft::nml::definition::BaseTypeEncodingEnum::k2c) {
            if (*baseTypePtr->baseTypeSize == INT8_WIDTH) {
                type = isoft::uds::server::DataType::kInt8;
            } else if (*baseTypePtr->baseTypeSize == INT16_WIDTH) {
                type = isoft::uds::server::DataType::kInt16;
            } else if (*baseTypePtr->baseTypeSize == INT32_WIDTH) {
                type = isoft::uds::server::DataType::kInt32;
            }
        } else if (*baseTypePtr->baseTypeEncoding == isoft::nml::definition::BaseTypeEncodingEnum::kNone) {
            if (*baseTypePtr->baseTypeSize == UINT8_WIDTH) {
                type = isoft::uds::server::DataType::kUint8;
            } else if (*baseTypePtr->baseTypeSize == UINT16_WIDTH) {
                type = isoft::uds::server::DataType::kUint16;
            } else if (*baseTypePtr->baseTypeSize == UINT32_WIDTH) {
                type = isoft::uds::server::DataType::kUint32;
            }
        } else if (*baseTypePtr->baseTypeEncoding == isoft::nml::definition::BaseTypeEncodingEnum::kIeee754) {
            type = isoft::uds::server::DataType::kFloat;
        }
    }
    return type;
}

std::string Trim(const std::string &s)
{
    auto start = s.begin();
    while (start != s.end() && (0 != std::isspace(static_cast< unsigned char >(*start)))) {
        ++start;
    }

    auto end = s.end();
    do {
        --end;
    } while (start < end && (0 != std::isspace(static_cast< unsigned char >(*start))));
    return (start < end + 1) ? std::string(start, end + 1) : std::string();
}

std::vector< std::string > SplitString(const std::string &s)
{
    std::vector< std::string > tokens;
    std::string token;
    std::istringstream tokenStream(s);
    char delimiter = ',';
    while (std::getline(tokenStream, token, delimiter)) {
        std::string trimmedToken = Trim(token);
        if (!trimmedToken.empty()) {
            tokens.push_back(trimmedToken);
        }
    }
    return tokens;
}

std::vector< std::uint8_t > ToRawData(const std::string &s, isoft::uds::server::DataType type)
{
    std::istringstream iss(s);
    if (type == isoft::uds::server::DataType::kFloat) {
        float val{};
        if (iss >> val && iss.eof()) {
            std::vector< std::uint8_t > buf;
            std::ignore = isoft::serialize::Serialize(buf, val);
            return buf;
        }
    } else if (type == isoft::uds::server::DataType::kUint8) {
        std::uint8_t val{};
        if (iss >> val && iss.eof()) {
            std::vector< std::uint8_t > buf;
            std::ignore = isoft::serialize::Serialize(buf, val);
            return buf;
        }
    } else if (type == isoft::uds::server::DataType::kUint16) {
        std::uint16_t val{};
        if (iss >> val && iss.eof()) {
            std::vector< std::uint8_t > buf;
            std::ignore = isoft::serialize::Serialize(buf, val);
            return buf;
        }
    } else if (type == isoft::uds::server::DataType::kUint32) {
        std::uint32_t val{};
        if (iss >> val && iss.eof()) {
            std::vector< std::uint8_t > buf;
            std::ignore = isoft::serialize::Serialize(buf, val);
            return buf;
        }
    } else if (type == isoft::uds::server::DataType::kInt8) {
        std::int8_t val{};
        if (iss >> val && iss.eof()) {
            std::vector< std::uint8_t > buf;
            std::ignore = isoft::serialize::Serialize(buf, val);
            return buf;
        }
    } else if (type == isoft::uds::server::DataType::kInt16) {
        std::int16_t val{};
        if (iss >> val && iss.eof()) {
            std::vector< std::uint8_t > buf;
            std::ignore = isoft::serialize::Serialize(buf, val);
            return buf;
        }
    } else if (type == isoft::uds::server::DataType::kInt32) {
        std::int32_t val{};
        if (iss >> val && iss.eof()) {
            std::vector< std::uint8_t > buf;
            std::ignore = isoft::serialize::Serialize(buf, val);
            return buf;
        }
    }
    return {};
}

isoft::uds::server::ValueSpecification Serialize(
    std::string value, std::shared_ptr< isoft::nml::definition::UdsDataElement > udsDataElementPtr) noexcept  // NOLINT
{
    isoft::uds::server::ValueSpecification result{};
    if (udsDataElementPtr.get() == nullptr) {
        return {};
    }
    if (udsDataElementPtr->maxNumberOfElement.get() != nullptr) {
        bool isArray{false};
        isoft::uds::server::DataType type{GetBaseType(udsDataElementPtr->baseTypeRef.lock())};
        if (*udsDataElementPtr->maxNumberOfElement > 1U) {
            isArray = true;
        }
        result.isArray = isArray;
        result.type    = type;
        if (isArray) {
            std::vector< std::string > tokens = SplitString(value);
            for (std::string &token : tokens) {
                std::vector< std::uint8_t > buf{};
                buf = ToRawData(token, type);
                result.data.insert(result.data.end(), buf.begin(), buf.end());
            }
        } else {
            result.data = ToRawData(value, type);
        }
    }
    return result;
}
#endif
static std::uint32_t GetDiagnosticDataElementId(ara::core::String &name)
{
    static std::map< ara::core::String, std::uint32_t > s_Table{};
    std::uint32_t id{0};
    decltype(auto) findResult = s_Table.find(name);
    if (findResult != s_Table.end()) {
        return findResult->second;
    }
    static std::hash< ara::core::String > s_Hasher;
    std::size_t hashId = s_Hasher(name);
    id                 = static_cast< std::uint32_t >(hashId);
    auto findHashId    = std::find_if(s_Table.begin(), s_Table.end(),
                                   [&](std::pair< ara::core::String, std::uint32_t > const &it) mutable {
                                       if (it.second == id) {
                                           return true;
                                       }
                                       return false;
                                   });
    if (findHashId != s_Table.end()) {
        LogError() << "DiagnosticDataElement.path has some hash id ,current element path:" << name;
        assert(false);
        return 0;
    }
    s_Table.insert({name, id});
    return id;
}
#ifdef NMODEL
bool UdsConfig::Initialize(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::shared_ptr< isoft::nml::definition::UdsServer > udsServerPtr)
{
    if (!_initializeUdsCommonSetting(udsServerPtr)) {
        common::LogInfo() << "UdsConfig::Initialize| not config common setting";
    }
    if (!_initializeUdsValidation(adaptiveSoftwareComponentTable, udsServerPtr->validation)) {
        common::LogInfo() << "UdsConfig::Initialize| not config validation";
        return false;
    }

    if (!_initializeUdsSessionControlService(udsServerPtr->sessionControlService)) {
        common::LogInfo() << "UdsConfig::Initialize| not config session control service";
    }

    if (!_initializeUdsSecurityAccessService(udsServerPtr->securityAccessService)) {
        common::LogInfo() << "UdsConfig::Initialize| not config security access service";
    }

    if (!_initializeUdsDataManagement(adaptiveSoftwareComponentTable, udsServerPtr->dataManagement)) {
        common::LogInfo() << "UdsConfig::Initialize| not config data management";
    }

    if (!_initializeUdsRoutineControlService(adaptiveSoftwareComponentTable, udsServerPtr->routineControlService)) {
        common::LogInfo() << "UdsConfig::Initialize| not config routine control service";
    }

    if (!_initializeUdsEcuResetService(adaptiveSoftwareComponentTable, udsServerPtr->ecuResetService)) {
        common::LogInfo() << "UdsConfig::Initialize| not config ecu reset service";
    }

    if (!_initializeUdsCommunicationControlService(adaptiveSoftwareComponentTable, udsServerPtr->commControlService)) {
        common::LogInfo() << "UdsConfig::Initialize| not config comm control service";
    }
    if (!_initializeUdsResponseOnEventService(udsServerPtr->responseOnEventService)) {
        common::LogInfo() << "UdsConfig::Initialize| not config x86 service";
    }
    if (!_initializeUdsCustomService(adaptiveSoftwareComponentTable, udsServerPtr->customService)) {
        common::LogInfo() << "UdsConfig::Initialize| not config custom service";
    }

    if (!_initializeUdsTransferManagement(adaptiveSoftwareComponentTable, udsServerPtr->transferManagement)) {
        common::LogInfo() << "UdsConfig::Initialize| not config transfer management";
    }
    if (!_initializeUdsFaultManagement(adaptiveSoftwareComponentTable, udsServerPtr->faultManagement)) {
    }

    if (!_initializeUdsAuthenticationService(adaptiveSoftwareComponentTable, udsServerPtr->authenticationService)) {
        common::LogInfo() << "UdsConfig::Initialize| not config auth service";
    }

    LogVerbose() << "UdsConfig::Initialize|success softwareCluster.shortName=" << udsServerPtr->shortName->c_str();
    return true;
}
#endif
bool UdsConfig::Initialize(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc)
{
    if (!_initializeFaultConfig(dmc)) {
        return false;
    }

    if (_initializeServerSetting(dmc) != isoft::kSuccess) {
        return false;
    }

    if (_initializeServiceValidation(dmc) != isoft::kSuccess) {
        LogInfo() << "UdsConfig::Initialize| _initializeServiceValidation fails!";
    }
    if (_initializeDiagnosticAuthentication(dmc) != isoft::kSuccess) {
        LogInfo() << "UdsConfig::Initialize| _initializeDiagnosticAuthentication fails!";
    }
    if (_initializeDiagnosticSession(dmc) != isoft::kSuccess) {
        return false;
    }
    if (_initializeSecurityLevel(dmc) != isoft::kSuccess) {
        return false;
    }
    if (_initializeDataManager(dmc) != isoft::kSuccess) {
        return false;
    }
    if (_initializeEnvCondition(dmc) != isoft::kSuccess) {
        return false;
    }
    if (_initializeAccessPermission(dmc) != isoft::kSuccess) {
        return false;
    }
    if (!_initializeServiceTable(dmc)) {
        return false;
    }

    LogVerbose() << "UdsConfig::Initialize|success softwareCluster.shortName=" << faultConfig_.clusterName.c_str();
    return true;
}

#ifdef NMODEL
std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent::PortType > UdsConfig::_getPort(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    isoft::nml::ModelTag tag,
    void *key) noexcept
{
    for (auto &&adaptiveSoftwareComponentPtr : adaptiveSoftwareComponentTable) {
        if (adaptiveSoftwareComponentPtr.get() == nullptr) {
            continue;
        }
        for (auto &&portTypePtr : adaptiveSoftwareComponentPtr->port) {
            if (portTypePtr.get() == nullptr) {
                continue;
            }
            if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticDataPort) {
                if (portTypePtr->diagnosticDataPort.get() == nullptr) {
                    continue;
                }
                for (auto &&instanceRef : portTypePtr->diagnosticDataPort->instanceRef) {
                    if (instanceRef->tag == isoft::nml::ModelTag::kUdsDataElement) {
                        void *point{static_cast< void * >(instanceRef->udsDataElementRef.lock().get())};
                        if (point == key) {
                            return portTypePtr;
                        }
                    } else if (instanceRef->tag == isoft::nml::ModelTag::kUdsDataIdentifier) {
                        void *point{static_cast< void * >(instanceRef->udsDataIdentifierRef.lock().get())};
                        if (point == key) {
                            return portTypePtr;
                        }
                    }
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticRoutinePort) {
                if (portTypePtr->diagnosticRoutinePort.get() == nullptr) {
                    continue;
                }
                for (auto &&instanceRef : portTypePtr->diagnosticRoutinePort->instanceRef) {
                    void *point{static_cast< void * >(instanceRef.lock().get())};
                    if (point == key) {
                        return portTypePtr;
                    }
                }
            }
        }
        if (!adaptiveSoftwareComponentPtr->softwareComponent.empty()) {
            std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent::PortType > portPtr{};
            std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > >
                childSoftwareComponmentList{adaptiveSoftwareComponentPtr->softwareComponent.begin(),
                                            adaptiveSoftwareComponentPtr->softwareComponent.end()};
            portPtr = _getPort(childSoftwareComponmentList, tag, key);
            if (portPtr.get() != nullptr) {
                return portPtr;
            }
        }
    }
    return nullptr;
}
/// @brief Get Port unique identifier
/// @param tag
/// @param key
/// @return
std::vector< std::string > UdsConfig::_getPortFqn(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    isoft::nml::ModelTag tag,
    void *key) noexcept
{
    std::vector< std::string > fqnTable{};
    for (auto &&adaptiveSoftwareComponentPtr : adaptiveSoftwareComponentTable) {
        if (adaptiveSoftwareComponentPtr.get() == nullptr) {
            continue;
        }
        for (auto &&portTypePtr : adaptiveSoftwareComponentPtr->port) {
            if (portTypePtr.get() == nullptr) {
                continue;
            }
            if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticEventPort) {
                auto diagnosticEventPortPtr{portTypePtr->diagnosticEventPort};
                if (diagnosticEventPortPtr.get() == nullptr) {
                    continue;
                }
                auto eventPtr{diagnosticEventPortPtr->eventRef.lock()};
                void *point{static_cast< void * >(eventPtr.get())};
                if (point == key) {
                    fqnTable.emplace_back(diagnosticEventPortPtr->isoftNmlObjectFqn);
                    continue;
                }

            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticCommunicationControlPort) {
                auto diagnosticCommunicationControlPortPtr{portTypePtr->diagnosticCommunicationControlPort};
                if (diagnosticCommunicationControlPortPtr.get() == nullptr) {
                    continue;
                }
                for (auto &&instanceRef : diagnosticCommunicationControlPortPtr->instanceRef) {
                    auto instancePtr{instanceRef.lock()};
                    void *point{static_cast< void * >(instancePtr.get())};
                    if (point == key) {
                        fqnTable.emplace_back(diagnosticCommunicationControlPortPtr->isoftNmlObjectFqn);
                        break;
                    }
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticConditionPort) {
                auto diagnosticConditionPortPtr{portTypePtr->diagnosticConditionPort};
                if (diagnosticConditionPortPtr.get() == nullptr) {
                    continue;
                }
                auto conditionPtr{diagnosticConditionPortPtr->conditionRef.lock()};
                void *point{static_cast< void * >(conditionPtr.get())};
                if (point == key) {
                    fqnTable.emplace_back(diagnosticConditionPortPtr->isoftNmlObjectFqn);
                    continue;
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticDTCInformationPort) {
                auto diagnosticDTCInformationPortPtr{portTypePtr->diagnosticDTCInformationPort};
                if (diagnosticDTCInformationPortPtr.get() == nullptr) {
                    continue;
                }
                auto faultMemoryPtr{diagnosticDTCInformationPortPtr->faultMemoryRef.lock()};
                void *point{static_cast< void * >(faultMemoryPtr.get())};
                if (point == key) {
                    fqnTable.emplace_back(diagnosticDTCInformationPortPtr->isoftNmlObjectFqn);
                    continue;
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticDataPort) {
                auto diagnosticDataPortPtr{portTypePtr->diagnosticDataPort};
                if (diagnosticDataPortPtr.get() == nullptr) {
                    continue;
                }
                for (auto &&instanceRef : diagnosticDataPortPtr->instanceRef) {
                    auto instancePtr{instanceRef};
                    void *point{nullptr};
                    if (instancePtr->tag == isoft::nml::ModelTag::kUdsDataElement) {
                        point = static_cast< void * >(instancePtr->udsDataElementRef.lock().get());
                    } else if (instancePtr->tag == isoft::nml::ModelTag::kUdsDataIdentifier) {
                        point = static_cast< void * >(instancePtr->udsDataIdentifierRef.lock().get());
                    }
                    if (point == key) {
                        fqnTable.emplace_back(diagnosticDataPortPtr->isoftNmlObjectFqn);
                        break;
                    }
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticDownloadServicePort) {
                auto diagnosticDownloadServicePortPtr{portTypePtr->diagnosticDownloadServicePort};
                if (diagnosticDownloadServicePortPtr.get() == nullptr) {
                    continue;
                }
                for (auto &&instanceRef : diagnosticDownloadServicePortPtr->instanceRef) {
                    auto instancePtr{instanceRef.lock()};
                    void *point{static_cast< void * >(instancePtr.get())};
                    if (point == key) {
                        fqnTable.emplace_back(diagnosticDownloadServicePortPtr->isoftNmlObjectFqn);
                        break;
                    }
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticEcuResetRequestPort) {
                auto diagnosticEcuResetRequestPortPtr{portTypePtr->diagnosticEcuResetRequestPort};
                if (diagnosticEcuResetRequestPortPtr.get() == nullptr) {
                    continue;
                }
                for (auto &&instanceRef : diagnosticEcuResetRequestPortPtr->instanceRef) {
                    auto instancePtr{instanceRef.lock()};
                    void *point{static_cast< void * >(instancePtr.get())};
                    if (point == key) {
                        fqnTable.emplace_back(diagnosticEcuResetRequestPortPtr->isoftNmlObjectFqn);
                        break;
                    }
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticFileTransferServicePort) {
                auto diagnosticFileTransferServicePortPtr{portTypePtr->diagnosticFileTransferServicePort};
                if (diagnosticFileTransferServicePortPtr.get() == nullptr) {
                    continue;
                }

                auto instancePtr{diagnosticFileTransferServicePortPtr->instanceRef.lock()};
                void *point{static_cast< void * >(instancePtr.get())};
                if (point == key) {
                    fqnTable.emplace_back(diagnosticFileTransferServicePortPtr->isoftNmlObjectFqn);
                    continue;
                }

            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticAuthenticationPort) {
                auto diagnosticAuthenticationPortPtr{portTypePtr->diagnosticAuthenticationPort};
                if (diagnosticAuthenticationPortPtr.get() == nullptr) {
                    continue;
                }

                auto instancePtr{diagnosticAuthenticationPortPtr->instanceRef.lock()};
                void *point{static_cast< void * >(instancePtr.get())};
                if (point == key) {
                    fqnTable.emplace_back(diagnosticAuthenticationPortPtr->isoftNmlObjectFqn);
                    continue;
                }

            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticGenericUDSServicePort) {
                auto diagnosticGenericUDSServicePortPtr{portTypePtr->diagnosticGenericUDSServicePort};
                if (diagnosticGenericUDSServicePortPtr.get() == nullptr) {
                    continue;
                }
                for (auto &&instanceRef : diagnosticGenericUDSServicePortPtr->instanceRef) {
                    auto instancePtr{instanceRef};
                    void *point{nullptr};
                    if (instancePtr->tag == isoft::nml::ModelTag::kUdsCustomService) {
                        point = static_cast< void * >(instancePtr->udsCustomServiceRef.lock().get());
                    } else if (instancePtr->tag == isoft::nml::ModelTag::kUdsReadDataByIdentifier) {
                        point = static_cast< void * >(instancePtr->udsReadDataByIdentifierRef.lock().get());
                    } else if (instancePtr->tag == isoft::nml::ModelTag::kUdsWriteDataByIdentifier) {
                        point = static_cast< void * >(instancePtr->udsWriteDataByIdentifierRef.lock().get());
                    }

                    if (point == key) {
                        fqnTable.emplace_back(diagnosticGenericUDSServicePortPtr->isoftNmlObjectFqn);
                        break;
                    }
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticIndicatorPort) {
                auto diagnosticIndicatorPortPtr{portTypePtr->diagnosticIndicatorPort};
                if (diagnosticIndicatorPortPtr.get() == nullptr) {
                    continue;
                }
                auto indicatorPtr{diagnosticIndicatorPortPtr->indicatorRef.lock()};
                void *point{static_cast< void * >(indicatorPtr.get())};
                if (point == key) {
                    fqnTable.emplace_back(diagnosticIndicatorPortPtr->isoftNmlObjectFqn);
                    break;
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticMonitorPort) {
                auto diagnosticMonitorPortPtr{portTypePtr->diagnosticMonitorPort};
                if (diagnosticMonitorPortPtr.get() == nullptr) {
                    continue;
                }
                auto eventPtr{diagnosticMonitorPortPtr->eventRef.lock()};
                void *point{static_cast< void * >(eventPtr.get())};
                if (point == key) {
                    fqnTable.emplace_back(diagnosticMonitorPortPtr->isoftNmlObjectFqn);
                    break;
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticOperationCyclePort) {
                auto diagnosticOperationCyclePortPtr{portTypePtr->diagnosticOperationCyclePort};
                if (diagnosticOperationCyclePortPtr.get() == nullptr) {
                    continue;
                }
                auto operationCyclePtr{diagnosticOperationCyclePortPtr->operationCycleRef.lock()};
                void *point{static_cast< void * >(operationCyclePtr.get())};
                if (point == key) {
                    fqnTable.emplace_back(diagnosticOperationCyclePortPtr->isoftNmlObjectFqn);
                    break;
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticRoutinePort) {
                auto diagnosticRoutinePortPtr{portTypePtr->diagnosticRoutinePort};
                if (diagnosticRoutinePortPtr.get() == nullptr) {
                    continue;
                }
                for (auto &&instanceRef : diagnosticRoutinePortPtr->instanceRef) {
                    auto instancePtr{instanceRef.lock()};
                    void *point{static_cast< void * >(instancePtr.get())};
                    if (point == key) {
                        fqnTable.emplace_back(diagnosticRoutinePortPtr->isoftNmlObjectFqn);
                        break;
                    }
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticSecurityAccessPort) {
                auto diagnosticSecurityAccessPortPtr{portTypePtr->diagnosticSecurityAccessPort};
                if (diagnosticSecurityAccessPortPtr.get() == nullptr) {
                    continue;
                }
                auto instancePtr{diagnosticSecurityAccessPortPtr->instanceRef.lock()};
                void *point{static_cast< void * >(instancePtr.get())};
                if (point == key) {
                    fqnTable.emplace_back(diagnosticSecurityAccessPortPtr->isoftNmlObjectFqn);
                    break;
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticServiceValidationPort) {
                auto diagnosticServiceValidationPortPtr{portTypePtr->diagnosticServiceValidationPort};
                if (diagnosticServiceValidationPortPtr.get() == nullptr) {
                    continue;
                }
                auto instancePtr{diagnosticServiceValidationPortPtr->instanceRef.lock()};
                void *point{static_cast< void * >(instancePtr.get())};
                if (point == key) {
                    fqnTable.emplace_back(diagnosticServiceValidationPortPtr->isoftNmlObjectFqn);
                    break;
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticUploadServicePort) {
                auto diagnosticUploadServicePortPtr{portTypePtr->diagnosticUploadServicePort};
                if (diagnosticUploadServicePortPtr.get() == nullptr) {
                    continue;
                }
                auto instancePtr{diagnosticUploadServicePortPtr->instanceRef.lock()};
                void *point{static_cast< void * >(instancePtr.get())};
                if (point == key) {
                    fqnTable.emplace_back(diagnosticUploadServicePortPtr->isoftNmlObjectFqn);
                    break;
                }
            }
        }

        if (!adaptiveSoftwareComponentPtr->softwareComponent.empty()) {
            std::vector< std::string > subFqnTable{};
            std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > >
                childSoftwareComponmentList{adaptiveSoftwareComponentPtr->softwareComponent.begin(),
                                            adaptiveSoftwareComponentPtr->softwareComponent.end()};
            subFqnTable = _getPortFqn(childSoftwareComponmentList, tag, key);
            if (subFqnTable.empty()) {
                continue;
            }
            fqnTable.insert(fqnTable.end(), subFqnTable.begin(), subFqnTable.end());
        }
    }
    return fqnTable;
}
bool UdsConfig::_hasPort(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    isoft::nml::ModelTag tag,
    void *key) noexcept
{
    for (auto &&adaptiveSoftwareComponentPtr : adaptiveSoftwareComponentTable) {
        if (adaptiveSoftwareComponentPtr.get() == nullptr) {
            continue;
        }
        for (auto &&portTypePtr : adaptiveSoftwareComponentPtr->port) {
            if (portTypePtr.get() == nullptr) {
                continue;
            }
            if (portTypePtr->tag != tag) {
                continue;
            }
            if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticEventPort) {
                if (portTypePtr->diagnosticEventPort.get() == nullptr) {
                    continue;
                }
                void *point{static_cast< void * >(portTypePtr->diagnosticEventPort->eventRef.lock().get())};
                if (point == key) {
                    return true;
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticCommunicationControlPort) {
                auto diagnosticCommunicationControlPortPtr{portTypePtr->diagnosticCommunicationControlPort};
                if (diagnosticCommunicationControlPortPtr.get() == nullptr) {
                    continue;
                }
                for (auto &&instanceRef : diagnosticCommunicationControlPortPtr->instanceRef) {
                    auto instancePtr{instanceRef.lock()};
                    void *point{static_cast< void * >(instancePtr.get())};
                    if (point == key) {
                        return true;
                    }
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticConditionPort) {
                auto diagnosticConditionPortPtr{portTypePtr->diagnosticConditionPort};
                if (diagnosticConditionPortPtr.get() == nullptr) {
                    continue;
                }
                auto conditionPtr{diagnosticConditionPortPtr->conditionRef.lock()};
                void *point{static_cast< void * >(conditionPtr.get())};
                if (point == key) {
                    return true;
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticDTCInformationPort) {
                auto diagnosticDTCInformationPortPtr{portTypePtr->diagnosticDTCInformationPort};
                if (diagnosticDTCInformationPortPtr.get() == nullptr) {
                    continue;
                }
                auto faultMemoryPtr{diagnosticDTCInformationPortPtr->faultMemoryRef.lock()};
                void *point{static_cast< void * >(faultMemoryPtr.get())};
                if (point == key) {
                    return true;
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticDataPort) {
                auto diagnosticDataPortPtr{portTypePtr->diagnosticDataPort};
                if (diagnosticDataPortPtr.get() == nullptr) {
                    continue;
                }
                for (auto &&instanceRef : diagnosticDataPortPtr->instanceRef) {
                    auto instancePtr{instanceRef};
                    void *point{nullptr};
                    if (instancePtr->tag == isoft::nml::ModelTag::kUdsDataElement) {
                        point = static_cast< void * >(instancePtr->udsDataElementRef.lock().get());
                    } else if (instancePtr->tag == isoft::nml::ModelTag::kUdsDataIdentifier) {
                        point = static_cast< void * >(instancePtr->udsDataIdentifierRef.lock().get());
                    }
                    if (point == key) {
                        return true;
                    }
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticDownloadServicePort) {
                auto diagnosticDownloadServicePortPtr{portTypePtr->diagnosticDownloadServicePort};
                if (diagnosticDownloadServicePortPtr.get() == nullptr) {
                    continue;
                }
                for (auto &&instanceRef : diagnosticDownloadServicePortPtr->instanceRef) {
                    auto instancePtr{instanceRef.lock()};
                    void *point{static_cast< void * >(instancePtr.get())};
                    if (point == key) {
                        return true;
                    }
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticEcuResetRequestPort) {
                auto diagnosticEcuResetRequestPortPtr{portTypePtr->diagnosticEcuResetRequestPort};
                if (diagnosticEcuResetRequestPortPtr.get() == nullptr) {
                    continue;
                }
                for (auto &&instanceRef : diagnosticEcuResetRequestPortPtr->instanceRef) {
                    auto instancePtr{instanceRef.lock()};
                    void *point{static_cast< void * >(instancePtr.get())};
                    if (point == key) {
                        return true;
                    }
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticFileTransferServicePort) {
                auto diagnosticFileTransferServicePortPtr{portTypePtr->diagnosticFileTransferServicePort};
                if (diagnosticFileTransferServicePortPtr.get() == nullptr) {
                    continue;
                }

                auto instancePtr{diagnosticFileTransferServicePortPtr->instanceRef.lock()};
                void *point{static_cast< void * >(instancePtr.get())};
                if (point == key) {
                    return true;
                }

            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticAuthenticationPort) {
                auto diagnosticAuthenticationPortPtr{portTypePtr->diagnosticAuthenticationPort};
                if (diagnosticAuthenticationPortPtr.get() == nullptr) {
                    continue;
                }

                auto instancePtr{diagnosticAuthenticationPortPtr->instanceRef.lock()};
                void *point{static_cast< void * >(instancePtr.get())};
                if (point == key) {
                    return true;
                }

            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticGenericUDSServicePort) {
                auto diagnosticGenericUDSServicePortPtr{portTypePtr->diagnosticGenericUDSServicePort};
                if (diagnosticGenericUDSServicePortPtr.get() == nullptr) {
                    continue;
                }
                for (auto &&instanceRef : diagnosticGenericUDSServicePortPtr->instanceRef) {
                    auto instancePtr{instanceRef};
                    void *point{nullptr};
                    if (instancePtr->tag == isoft::nml::ModelTag::kUdsCustomService) {
                        point = static_cast< void * >(instancePtr->udsCustomServiceRef.lock().get());
                    } else if (instancePtr->tag == isoft::nml::ModelTag::kUdsReadDataByIdentifier) {
                        point = static_cast< void * >(instancePtr->udsReadDataByIdentifierRef.lock().get());
                    } else if (instancePtr->tag == isoft::nml::ModelTag::kUdsWriteDataByIdentifier) {
                        point = static_cast< void * >(instancePtr->udsWriteDataByIdentifierRef.lock().get());
                    }

                    if (point == key) {
                        return true;
                    }
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticIndicatorPort) {
                auto diagnosticIndicatorPortPtr{portTypePtr->diagnosticIndicatorPort};
                if (diagnosticIndicatorPortPtr.get() == nullptr) {
                    continue;
                }
                auto indicatorPtr{diagnosticIndicatorPortPtr->indicatorRef.lock()};
                void *point{static_cast< void * >(indicatorPtr.get())};
                if (point == key) {
                    return true;
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticMonitorPort) {
                auto diagnosticMonitorPortPtr{portTypePtr->diagnosticMonitorPort};
                if (diagnosticMonitorPortPtr.get() == nullptr) {
                    continue;
                }
                auto eventPtr{diagnosticMonitorPortPtr->eventRef.lock()};
                void *point{static_cast< void * >(eventPtr.get())};
                if (point == key) {
                    return true;
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticOperationCyclePort) {
                auto diagnosticOperationCyclePortPtr{portTypePtr->diagnosticOperationCyclePort};
                if (diagnosticOperationCyclePortPtr.get() == nullptr) {
                    continue;
                }
                auto operationCyclePtr{diagnosticOperationCyclePortPtr->operationCycleRef.lock()};
                void *point{static_cast< void * >(operationCyclePtr.get())};
                if (point == key) {
                    return true;
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticRoutinePort) {
                auto diagnosticRoutinePortPtr{portTypePtr->diagnosticRoutinePort};
                if (diagnosticRoutinePortPtr.get() == nullptr) {
                    continue;
                }
                for (auto &&instanceRef : diagnosticRoutinePortPtr->instanceRef) {
                    auto instancePtr{instanceRef.lock()};
                    void *point{static_cast< void * >(instancePtr.get())};
                    if (point == key) {
                        return true;
                    }
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticSecurityAccessPort) {
                auto diagnosticSecurityAccessPortPtr{portTypePtr->diagnosticSecurityAccessPort};
                if (diagnosticSecurityAccessPortPtr.get() == nullptr) {
                    continue;
                }
                auto instancePtr{diagnosticSecurityAccessPortPtr->instanceRef.lock()};
                void *point{static_cast< void * >(instancePtr.get())};
                if (point == key) {
                    return true;
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticServiceValidationPort) {
                auto diagnosticServiceValidationPortPtr{portTypePtr->diagnosticServiceValidationPort};
                if (diagnosticServiceValidationPortPtr.get() == nullptr) {
                    continue;
                }
                auto instancePtr{diagnosticServiceValidationPortPtr->instanceRef.lock()};
                void *point{static_cast< void * >(instancePtr.get())};
                if (point == key) {
                    return true;
                }
            } else if (portTypePtr->tag == isoft::nml::ModelTag::kDiagnosticUploadServicePort) {
                auto diagnosticUploadServicePortPtr{portTypePtr->diagnosticUploadServicePort};
                if (diagnosticUploadServicePortPtr.get() == nullptr) {
                    continue;
                }
                auto instancePtr{diagnosticUploadServicePortPtr->instanceRef.lock()};
                void *point{static_cast< void * >(instancePtr.get())};
                if (point == key) {
                    return true;
                }
            }
        }
        if (!adaptiveSoftwareComponentPtr->softwareComponent.empty()) {
            bool subResult{false};
            std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > >
                childSoftwareComponmentList{adaptiveSoftwareComponentPtr->softwareComponent.begin(),
                                            adaptiveSoftwareComponentPtr->softwareComponent.end()};
            subResult = _hasPort(childSoftwareComponmentList, tag, key);
            if (subResult) {
                return true;
            }
        }
    }
    return false;
}
/// @brief Initialize fault configuration
/// @param dmc Configuration node
/// @return Whether initialization is successful
bool UdsConfig::_initializeUdsCommonSetting(std::shared_ptr< isoft::nml::definition::UdsServer > udsServerPtr) noexcept
{
    if (udsServerPtr->physicalAddress.get() != nullptr) {
        serverSetting_.physicalAddress = *udsServerPtr->physicalAddress;
    }
    for (auto &&functionalAddress : udsServerPtr->functionalAddress) {
        serverSetting_.functionAddressTable.emplace_back(functionalAddress);
    }

    if (udsServerPtr->maxNumberOfNrc78.get() != nullptr) {
        serverSetting_.maxNumberOfRequestCorrectlyReceivedResponsePending = *udsServerPtr->maxNumberOfNrc78;
    }
    if (udsServerPtr->responseOnAllRequestSid.get() != nullptr) {
        serverSetting_.responseOnAllRequestSids = *udsServerPtr->responseOnAllRequestSid;
    }
    if (udsServerPtr->responseOnSecondRequest.get() != nullptr) {
        serverSetting_.responseOnSecondDeclinedRequest = *udsServerPtr->responseOnSecondRequest;
    }
    return true;
}
bool UdsConfig::_initializeUdsFaultManagement(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::shared_ptr< isoft::nml::definition::UdsFaultManagement > udsFaultManagementPtr) noexcept
{
    if (adaptiveSoftwareComponentTable.empty()) {
        LogError() << "UdsConfig::_initializeFaultConfig|adaptiveSoftwareComponentTable is empty";
        return false;
    }

    if (udsFaultManagementPtr.get() == nullptr) {
        LogError() << "UdsConfig::_initializeFaultConfig|udsFaultManagementPtr is nullptr";
        return false;
    }
    if (udsFaultManagementPtr->occurrenceCounterProcessing.get() != nullptr) {
        switch (*udsFaultManagementPtr->occurrenceCounterProcessing) {
            case isoft::nml::definition::DiagnosticOccurrenceCounterProcessingEnum::kConfirmedDtcBit:
                faultConfig_.commonProps.occurrenceCounterProcessing
                    = isoft::uds::server::DiagnosticOccurrenceCounterProcessingEnum::kConfirmedDtcBit;
                break;
            case isoft::nml::definition::DiagnosticOccurrenceCounterProcessingEnum::kTestFailedBit:
                faultConfig_.commonProps.occurrenceCounterProcessing
                    = isoft::uds::server::DiagnosticOccurrenceCounterProcessingEnum::kTestFailedBit;
                break;
            default:
                break;
        }
    }
    if (udsFaultManagementPtr->resetConfirmedBitOnOverflow.get() != nullptr) {
        faultConfig_.commonProps.resetConfirmedBitOnOverflow = *udsFaultManagementPtr->resetConfirmedBitOnOverflow;
    }
    if (!_initializeUdsOperationCycle(adaptiveSoftwareComponentTable, udsFaultManagementPtr->operationCycle)) {
        return false;
    }

    if (!_initializeUdsIndicator(adaptiveSoftwareComponentTable, udsFaultManagementPtr->indicator)) {
        return false;
    }

    if (!_initializeUdsExtendedDataRecord(adaptiveSoftwareComponentTable, udsFaultManagementPtr->extendedDataRecord)) {
        return false;
    }
    if (!_initializeUdsFreezeFrame(udsFaultManagementPtr->freezeFrame)) {
        return false;
    }
    if (!_initializeUdsDataIdentifierSet(udsFaultManagementPtr->dataIdentifierSet)) {
        return false;
    }
    if (!_initializeUdsFaultMemory(adaptiveSoftwareComponentTable, udsFaultManagementPtr->faultMemory)) {
        return false;
    }
    if (!_initializeUdsCondition(adaptiveSoftwareComponentTable, udsFaultManagementPtr->clearCondition,
                                 udsFaultManagementPtr->enableCondition)) {
        return false;
    }

    if (!_initializeUdsDebounceAlgorithmType(udsFaultManagementPtr->debounceAlgorithm)) {
        return false;
    }
    if (!_initializeUdsDtc(adaptiveSoftwareComponentTable, udsFaultManagementPtr->udsDtc,
                           udsFaultManagementPtr->udsDtcGroup)) {
        return false;
    }

    if (!_initializeUdsClearDiagnosticInformationService(udsFaultManagementPtr->serviceX14)) {
        return false;
    }

    if (!_initializeUdsReadDTCInformationService(udsFaultManagementPtr->serviceX19)) {
        return false;
    }

    if (!_initializeUdsControlDTCSettingService(udsFaultManagementPtr->serviceX85)) {
        return false;
    }

    return true;
}

bool UdsConfig::_initializeUdsEvent(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::uint32_t dtcValue,
    std::shared_ptr< isoft::nml::definition::UdsEvent > udsEventPtr) noexcept
{
    isoft::uds::server::EventConfig eventConfig{};
    if (udsEventPtr.get() == nullptr) {
        common::LogError() << "UdsConfig::_initializeUdsEvent| event not config!";
        return false;
    }
    if (udsEventPtr->eventId.get() != nullptr) {
        eventConfig.associatedEventIdentification = *udsEventPtr->eventId;
    }

    if (udsEventPtr->clearEventAllowedBehavior.get() != nullptr) {
        switch (*udsEventPtr->clearEventAllowedBehavior) {
            case isoft::nml::definition::DiagnosticClearEventAllowedBehaviorEnum::kNoStatusByteChange:
                eventConfig.clearEventAllowedBehavior
                    = isoft::uds::server::DiagnosticClearEventAllowedBehaviorEnum::kNoStatusByteChange;
                break;
            case isoft::nml::definition::DiagnosticClearEventAllowedBehaviorEnum::kOnlyThisCycleAndReadiness:
                eventConfig.clearEventAllowedBehavior
                    = isoft::uds::server::DiagnosticClearEventAllowedBehaviorEnum::kOnlyThisCycleAndReadiness;
                break;
            default:
                break;
        }
    }

    if (udsEventPtr->confirmationThreshold.get() != nullptr) {
        eventConfig.confirmationThreshold = *udsEventPtr->confirmationThreshold;
    }

    if (udsEventPtr->recoverableInSameOC.get() != nullptr) {
        eventConfig.recoverableInSameOperationCycle = *udsEventPtr->recoverableInSameOC;
    }

    if (udsEventPtr->debounceAlgorithmRef.get() != nullptr) {
        if (udsEventPtr->debounceAlgorithmRef->tag == isoft::nml::ModelTag::kUdsEventDebounceTimeBased) {
            std::shared_ptr< isoft::nml::definition::UdsEventDebounceTimeBased > timeBasePtr{
                udsEventPtr->debounceAlgorithmRef->udsEventDebounceTimeBasedRef.lock()};
            if (timeBasePtr.get() != nullptr) {
                eventConfig.debounceAlgorithm = udsEventDebounceTimeBasedMap_.at(timeBasePtr.get());
            }
        }
        if (udsEventPtr->debounceAlgorithmRef->tag == isoft::nml::ModelTag::kUdsEventDebounceCounterBased) {
            std::shared_ptr< isoft::nml::definition::UdsEventDebounceCounterBased > counterBasePtr{
                udsEventPtr->debounceAlgorithmRef->udsEventDebounceCounterBasedRef.lock()};
            if (counterBasePtr.get() != nullptr) {
                eventConfig.debounceAlgorithm = udsEventDebounceCounterBasedMap_.at(counterBasePtr.get());
            }
        }
    }

    for (auto &&connectedIndicator : udsEventPtr->connectedIndicator) {
        isoft::uds::server::ConnectedIndicatorConfig connectedIndicatorConfig;
        if (connectedIndicator.get() == nullptr) {
            continue;
        }
        if (connectedIndicator->behavior.get() != nullptr) {
            switch (*connectedIndicator->behavior) {
                case isoft::nml::definition::DiagnosticConnectedIndicatorBehaviorEnum::kBlinkMode:

                    connectedIndicatorConfig.behavior
                        = isoft::uds::server::DiagnosticConnectedIndicatorBehaviorEnum::kBlinkMode;
                    break;
                case isoft::nml::definition::DiagnosticConnectedIndicatorBehaviorEnum::kBlinkOrContinuousOnMode:
                    connectedIndicatorConfig.behavior
                        = isoft::uds::server::DiagnosticConnectedIndicatorBehaviorEnum::kBlinkOrContinuousOnMode;
                    break;
                case isoft::nml::definition::DiagnosticConnectedIndicatorBehaviorEnum::kContinuousOnMode:
                    connectedIndicatorConfig.behavior
                        = isoft::uds::server::DiagnosticConnectedIndicatorBehaviorEnum::kContinuousOnMode;
                    break;
                case isoft::nml::definition::DiagnosticConnectedIndicatorBehaviorEnum::kFastFlashingMode:
                    connectedIndicatorConfig.behavior
                        = isoft::uds::server::DiagnosticConnectedIndicatorBehaviorEnum::kFastFlashingMode;
                    break;
                case isoft::nml::definition::DiagnosticConnectedIndicatorBehaviorEnum::kSlowFlashingMode:
                    connectedIndicatorConfig.behavior
                        = isoft::uds::server::DiagnosticConnectedIndicatorBehaviorEnum::kSlowFlashingMode;
                    break;
                default:
                    break;
            }
        }
        if (connectedIndicator->healCycleCounterThreshold.get() != nullptr) {
            connectedIndicatorConfig.healingCycleCounterThreshold = *connectedIndicator->healCycleCounterThreshold;
        }

        if (connectedIndicator->healingCycleRef.lock() != nullptr) {
            connectedIndicatorConfig.healingCycle
                = udsOperationCycleMap_.at(connectedIndicator->healingCycleRef.lock().get());
        }

        if (connectedIndicator->indicatorRef.lock() != nullptr) {
            connectedIndicatorConfig.indicatorId = udsIndicatorMap_.at(connectedIndicator->indicatorRef.lock().get());
        }
        eventConfig.connectedIndicator.emplace_back(connectedIndicatorConfig);
    }

    if (udsEventPtr->enableConditionGroup.get() != nullptr) {
        eventConfig.enableConditionGroup = eventConfig.associatedEventIdentification;
        std::set< uint32_t > enableConditionGroup{};
        for (auto &&conditionsRef : udsEventPtr->enableConditionGroup->conditionsRef) {
            std::shared_ptr< isoft::nml::definition::UdsCondition > conditionPtr{conditionsRef.lock()};
            if (conditionPtr.get() != nullptr) {
                enableConditionGroup.emplace(udsEnableConditionMap_.at(conditionPtr.get()));
            }
        }
        faultConfig_.enableConditionGroup.insert({eventConfig.enableConditionGroup, enableConditionGroup});
    }

    if (udsEventPtr->operationCycleRef.lock().get() != nullptr) {
        eventConfig.operationCycle = udsOperationCycleMap_.at(udsEventPtr->operationCycleRef.lock().get());
    }
    eventConfig.troubleCodeUds = dtcValue;
    if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticEventPort, udsEventPtr.get())) {
        std::string service{udsEventPtr->isoftNmlObjectFqn};
        service += "/Event";
        std::uint32_t const serviceInstanceId{Hash32(service)};
        eventConfig.eventInterface = std::make_shared< Event >(serviceInstanceId);
    }
    if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticMonitorPort, udsEventPtr.get())) {
        std::string service{udsEventPtr->isoftNmlObjectFqn};
        service += "/Monitor";
        std::uint32_t const serviceInstanceId{Hash32(service)};
        eventConfig.monitorInterface = std::make_shared< Monitor >(serviceInstanceId);
    }
    faultConfig_.eventConfig.insert({eventConfig.associatedEventIdentification, eventConfig});
    return true;
}

bool UdsConfig::_initializeUdsDtc(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::vector< std::shared_ptr< isoft::nml::definition::UdsDtc > > udsDtcTablePtr,
    std::vector< std::shared_ptr< isoft::nml::definition::UdsDtcGroup > > udsDtcGroupTablePtr) noexcept
{
    bool result{true};
    std::map< isoft::nml::definition::UdsDtc *, std::uint32_t > udsDtcTable{};
    for (auto &&udsDtcPtr : udsDtcTablePtr) {
        isoft::uds::server::DtcConfig dtcConfig{};
        if (udsDtcPtr->udsDtcValue.get() != nullptr) {
            dtcConfig.dtcValue   = static_cast< std::uint32_t >(std::atol(udsDtcPtr->udsDtcValue->c_str()));
            dtcConfig.dtcPropsId = dtcConfig.dtcValue;
        }
        if (udsDtcPtr->severity.get() != nullptr) {
            switch (*udsDtcPtr->severity) {
                case isoft::nml::definition::DiagnosticUdsSeverityEnum::kCheckAtNextHalt:
                    dtcConfig.severity = isoft::uds::server::DiagnosticUdsSeverityEnum::kCheckAtNextHalt;
                    break;
                case isoft::nml::definition::DiagnosticUdsSeverityEnum::kImmediately:
                    dtcConfig.severity = isoft::uds::server::DiagnosticUdsSeverityEnum::kImmediately;
                    break;
                case isoft::nml::definition::DiagnosticUdsSeverityEnum::kMaintenanceOnly:
                    dtcConfig.severity = isoft::uds::server::DiagnosticUdsSeverityEnum::kMaintenanceOnly;
                    break;
                case isoft::nml::definition::DiagnosticUdsSeverityEnum::kNoSeverity:
                    dtcConfig.severity = isoft::uds::server::DiagnosticUdsSeverityEnum::kNoSeverity;
                    break;
                default:
                    break;
            }
        }
        if (udsDtcPtr->clearConditionGroup.get() != nullptr) {
            dtcConfig.clearConditionGroupId = dtcConfig.dtcValue;
            std::set< uint32_t > clearConditionIdGroup{};
            for (auto &&conditionsRef : udsDtcPtr->clearConditionGroup->conditionsRef) {
                std::shared_ptr< isoft::nml::definition::UdsCondition > conditionPtr{conditionsRef.lock()};
                if (conditionPtr.get() != nullptr) {
                    clearConditionIdGroup.emplace(udsClearConditionMap_.at(conditionPtr.get()));
                }
            }
            faultConfig_.clearConditionGroup.insert({dtcConfig.clearConditionGroupId, clearConditionIdGroup});
        }
        udsDtcTable.insert({udsDtcPtr.get(), dtcConfig.dtcValue});
        faultConfig_.dtcConfig.insert({dtcConfig.dtcValue, dtcConfig});
        isoft::uds::server::DtcPropsConfig dtcPropsConfig{};
        dtcPropsConfig.dtcPropsID = dtcConfig.dtcValue;
        if (udsDtcPtr->immediateNvDataStorage.get() != nullptr) {
            dtcPropsConfig.immediateNvDataStorage = *udsDtcPtr->immediateNvDataStorage;
        }
        if (udsDtcPtr->maxNumberFFR.get() != nullptr) {
            dtcPropsConfig.maxNumberFreezeFrameRecords = *udsDtcPtr->maxNumberFFR;
        }
        if (udsDtcPtr->priority.get() != nullptr) {
            dtcPropsConfig.priority = *udsDtcPtr->priority;
        }
        if (udsDtcPtr->significance.get() != nullptr) {
            switch (*udsDtcPtr->significance) {
                case isoft::nml::definition::DiagnosticSignificanceEnum::kFault:
                    dtcPropsConfig.significance = isoft::uds::server::DiagnosticSignificanceEnum::kFault;
                    break;
                case isoft::nml::definition::DiagnosticSignificanceEnum::kOccurence:
                    dtcPropsConfig.significance = isoft::uds::server::DiagnosticSignificanceEnum::kOccurence;
                    break;
                default:
                    break;
            }
        }
        if (udsDtcPtr->snapshotRecordContentRef.lock() != nullptr) {
            dtcPropsConfig.snapshotRecordContent
                = udsDataIdentifierSetMap_.at(udsDtcPtr->snapshotRecordContentRef.lock().get());
        }

        for (auto &&extendedDataRecordRef : udsDtcPtr->extendedDataRecordRef) {
            std::shared_ptr< isoft::nml::definition::UdsExtendedDataRecord > extendedDataRecordPtr{
                extendedDataRecordRef.lock()};
            if (extendedDataRecordPtr.get() != nullptr) {
                std::uint32_t edrId = udsExtendedDataRecordMap_.at(extendedDataRecordPtr.get());
                dtcPropsConfig.extendedDataRecord.emplace(edrId);
            }
        }

        for (auto &&freezeFrameRef : udsDtcPtr->freezeFrameRef) {
            std::shared_ptr< isoft::nml::definition::UdsFreezeFrame > freezeFramePtr{freezeFrameRef.lock()};
            if (freezeFramePtr.get() != nullptr) {
                std::uint32_t ffrId = udsFreezeFrameMap_.at(freezeFramePtr.get());
                dtcPropsConfig.freezeFrame.emplace(ffrId);
            }
        }

        if (udsDtcPtr->diagnosticMemoryRef.lock() != nullptr) {
            std::uint32_t memId = udsFaultMemoryMap_.at(udsDtcPtr->diagnosticMemoryRef.lock().get());
            dtcPropsConfig.memoryDestination.emplace(memId);
        }
        if (udsDtcPtr->aging.get() != nullptr) {
            isoft::uds::server::AgingConfig agingConfig{};
            agingConfig.agingID = static_cast< std::uint32_t >(faultConfig_.agingConfig.size());
            if (udsDtcPtr->aging->threshold.get() != nullptr) {
                agingConfig.threshold = *udsDtcPtr->aging->threshold;
            }
            std::shared_ptr< isoft::nml::definition::UdsOperationCycle > agingCycleRefPtr{
                udsDtcPtr->aging->agingCycleRef.lock()};
            if (agingCycleRefPtr.get() != nullptr) {
                agingConfig.agingCycle = udsOperationCycleMap_.at(agingCycleRefPtr.get());
            }
            faultConfig_.agingConfig.insert({agingConfig.agingID, agingConfig});
            dtcPropsConfig.agingID = agingConfig.agingID;
        }
        faultConfig_.dtcPropsConfig.insert({dtcPropsConfig.dtcPropsID, dtcPropsConfig});
        result = _initializeUdsEvent(adaptiveSoftwareComponentTable, dtcConfig.dtcValue, udsDtcPtr->diagnosticEvent);
        if (!result) {
            common::LogError() << "UdsConfig::_initializeUdsDtc| Event load fails! dtcValue:" << dtcConfig.dtcValue;
        }
    }
    for (auto &&udsDtcGroupPtr : udsDtcGroupTablePtr) {
        std::set< std::uint32_t > dtcTable{};
        if (udsDtcGroupPtr.get() != nullptr) {
            for (auto &&udsDtcRef : udsDtcGroupPtr->udsDtcRef) {
                auto udsDtcPtr{udsDtcRef.lock()};
                dtcTable.insert(udsDtcTable.at(udsDtcPtr.get()));
            }
        }
        if (udsDtcGroupPtr->groupNumber.get() != nullptr) {
            std::uint32_t groupNmber{static_cast< std::uint32_t >(std::atol(udsDtcGroupPtr->groupNumber->c_str()))};
            faultConfig_.dtcGroup.insert({groupNmber, dtcTable});
        }
    }
    return true;
}

bool UdsConfig::_initializeUdsOperationCycle(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::vector< std::shared_ptr< isoft::nml::definition::UdsOperationCycle > > operationCycleTablePtr) noexcept
{
    std::uint32_t id{0};
    for (auto &&operationCyclePtr : operationCycleTablePtr) {
        if (operationCyclePtr.get() == nullptr) {
            continue;
        }
        isoft::uds::server::OperationCycleConfig operationCycleConfig{};
        if (operationCyclePtr->type.get() != nullptr) {
            switch (*operationCyclePtr->type) {
                case isoft::nml::definition::DiagnosticOperationCycleTypeEnum::kIgnition:
                    operationCycleConfig.type = isoft::uds::server::DiagnosticOperationCycleTypeEnum::kIgnition;
                    break;
                case isoft::nml::definition::DiagnosticOperationCycleTypeEnum::kObdDrivingCycle:
                    operationCycleConfig.type = isoft::uds::server::DiagnosticOperationCycleTypeEnum::kObdDrivingCycle;
                    break;
                case isoft::nml::definition::DiagnosticOperationCycleTypeEnum::kOther:
                    operationCycleConfig.type = isoft::uds::server::DiagnosticOperationCycleTypeEnum::kOther;
                    break;
                case isoft::nml::definition::DiagnosticOperationCycleTypeEnum::kPower:
                    operationCycleConfig.type = isoft::uds::server::DiagnosticOperationCycleTypeEnum::kPower;
                    break;
                case isoft::nml::definition::DiagnosticOperationCycleTypeEnum::kTime:
                    operationCycleConfig.type = isoft::uds::server::DiagnosticOperationCycleTypeEnum::kTime;
                    break;
                case isoft::nml::definition::DiagnosticOperationCycleTypeEnum::kWarmup:
                    operationCycleConfig.type = isoft::uds::server::DiagnosticOperationCycleTypeEnum::kWarmup;
                    break;
                default:
                    break;
            }
        }
        operationCycleConfig.operationCycleId = id++;
        if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticOperationCyclePort,
                     operationCyclePtr.get())) {
            std::uint32_t serviceId{Hash32(operationCyclePtr->isoftNmlObjectFqn)};
            auto operationInterfacePtr{std::make_shared< OperationCycle >(serviceId)};
            operationCycleConfig.operationInterface = operationInterfacePtr;
        }
        faultConfig_.operationCycleConfig.insert({operationCycleConfig.operationCycleId, operationCycleConfig});
        udsOperationCycleMap_.insert({operationCyclePtr.get(), operationCycleConfig.operationCycleId});
    }
    return true;
}

bool UdsConfig::_initializeUdsExtendedDataRecord(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::vector< std::shared_ptr< isoft::nml::definition::UdsExtendedDataRecord > > extendedDataRecordTablePtr) noexcept
{
    for (auto &&extendedDataRecordPtr : extendedDataRecordTablePtr) {
        if (extendedDataRecordPtr.get() == nullptr) {
            continue;
        }
        isoft::uds::server::ExtendedDataRecordConfig config{};
        if (extendedDataRecordPtr->recordNumber.get() != nullptr) {
            config.recordNumber = *extendedDataRecordPtr->recordNumber;
        }
        if (extendedDataRecordPtr->trigger.get() != nullptr) {
            switch (*extendedDataRecordPtr->trigger) {
                case isoft::nml::definition::DiagnosticRecordTriggerEnum::kConfirmed:
                    config.trigger = isoft::uds::server::DiagnosticRecordTriggerEnum::kConfirmed;
                    break;
                case isoft::nml::definition::DiagnosticRecordTriggerEnum::kFdcThreshold:
                    config.trigger = isoft::uds::server::DiagnosticRecordTriggerEnum::kFdcThreshold;
                    break;
                case isoft::nml::definition::DiagnosticRecordTriggerEnum::kPending:
                    config.trigger = isoft::uds::server::DiagnosticRecordTriggerEnum::kPending;
                    break;
                case isoft::nml::definition::DiagnosticRecordTriggerEnum::kTestFailed:
                    config.trigger = isoft::uds::server::DiagnosticRecordTriggerEnum::kTestFailed;
                    break;
                case isoft::nml::definition::DiagnosticRecordTriggerEnum::kCustom:
                    config.trigger = isoft::uds::server::DiagnosticRecordTriggerEnum::kCustom;
                    break;
                case isoft::nml::definition::DiagnosticRecordTriggerEnum::kTestFailedThisOperationCycle:
                    config.trigger = isoft::uds::server::DiagnosticRecordTriggerEnum::kTestFailedThisOperationCycle;
                    break;

                default:
                    break;
            }
        }
        if (extendedDataRecordPtr->update.get() != nullptr) {
            config.update = *extendedDataRecordPtr->update;
        }
        if (extendedDataRecordPtr->customTrigger.get() != nullptr) {
            config.customTrigger = *extendedDataRecordPtr->customTrigger;
        }
        for (auto &&recordElementPtr : extendedDataRecordPtr->recordElement) {
            if (recordElementPtr.get() == nullptr) {
                continue;
            }
            isoft::uds::server::EdrDataElementConfig dataElement{};
            if (recordElementPtr->internalProvider.get() != nullptr) {
                std::string value{*recordElementPtr->internalProvider};
                if (value == "DEM_AGINGCTR_DOWNCNT") {
                    dataElement.category = fault::EdrDataElementCategory::kDemAgingctrDowncnt;
                } else if (value == "DEM_AGINGCTR_UPCNT") {
                    dataElement.category = fault::EdrDataElementCategory::kDemAgingctrUpcnt;
                } else if (value == "DEM_CURRENT_FDC") {
                    dataElement.category = fault::EdrDataElementCategory::kDemCurrentFdc;
                } else if (value == "DEM_CYCLES_SINCE_FIRST_FAILED") {
                    dataElement.category = fault::EdrDataElementCategory::kDemCyclesSinceFirstFailed;
                } else if (value == "DEM_CYCLES_SINCE_LAST_FAILED") {
                    dataElement.category = fault::EdrDataElementCategory::kDemCyclesSinceLastFailed;
                } else if (value == "DEM_FAILED_CYCLES") {
                    dataElement.category = fault::EdrDataElementCategory::kDemFailedCycles;
                } else if (value == "DEM_MAX_FDC_DURING_CURRENT_CYCLE") {
                    dataElement.category = fault::EdrDataElementCategory::kDemMaxFdcDuringCurrentCycle;
                } else if (value == "DEM_MAX_FDC_SINCE_LAST_CLEAR") {
                    dataElement.category = fault::EdrDataElementCategory::kDemMaxFdcSinceLastClear;
                } else if (value == "DEM_OCCCTR") {
                    dataElement.category = fault::EdrDataElementCategory::kDemOccctr;
                } else if (value == "DEM_OVFLIND") {
                    dataElement.category = fault::EdrDataElementCategory::kDemOvflind;
                } else if (value == "DEM_SIGNIFICANCE") {
                    dataElement.category = fault::EdrDataElementCategory::kDemSignificance;
                } else if (value == "DEM_PRIORITY") {
                    dataElement.category = fault::EdrDataElementCategory::kDemPriority;
                } else if (value == "DEM_EVENT_ASSOCIATED_IDENTIFICATION") {
                    dataElement.category = fault::EdrDataElementCategory::kDemEventAssociatedIdentification;
                } else {
                    LogError() << "UdsConfig::_initializeConfiguredDids|invalid "
                                  "dataProvider ="
                               << value.c_str();
                    return false;
                }
            }
            ara::core::String strName{recordElementPtr->isoftNmlObjectFqn};
            dataElement.dataElementId = GetDiagnosticDataElementId(strName);
            isoft::uds::server::DiagnosticDataParamterModel model{};
            model.element.id = dataElement.dataElementId;
            diagnosticDataManagement_.didManager.edrElement.push_back(model);
            config.dataElementList.emplace_back(dataElement);
            if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticDataPort,
                         recordElementPtr.get())) {
                dataElement.category = fault::EdrDataElementCategory::kExternal;
                if (genericDataElementPtr_.get() == nullptr) {
                    genericDataElementPtr_ = std::make_shared< GenericDataElement >();
                }
                std::vector< std::string > portTable = _getPortFqn(
                    adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticDataPort, recordElementPtr.get());
                std::uint16_t const instance{Hash16(recordElementPtr->isoftNmlObjectFqn)};
                std::uint32_t const serviceInstance{Hash32(portTable.front())};
                genericDataElementPtr_->RegisterServiceInstance(dataElement.dataElementId, instance, serviceInstance);
            }
        }
        faultConfig_.extendedDataRecordConfig.insert({config.recordNumber, config});
        udsExtendedDataRecordMap_.insert({extendedDataRecordPtr.get(), config.recordNumber});
    }
    return true;
}

bool UdsConfig::_initializeUdsFreezeFrame(
    std::vector< std::shared_ptr< isoft::nml::definition::UdsFreezeFrame > > freezeFrameTablePtr) noexcept
{
    for (auto &&freezeFramePtr : freezeFrameTablePtr) {
        if (freezeFramePtr.get() == nullptr) {
            continue;
        }
        isoft::uds::server::FreezeFrameConfig config{};
        if (freezeFramePtr->recordNumber.get() != nullptr) {
            config.recordNumber = *freezeFramePtr->recordNumber;
        }
        if (freezeFramePtr->trigger.get() != nullptr) {
            switch (*freezeFramePtr->trigger) {
                case isoft::nml::definition::DiagnosticRecordTriggerEnum::kConfirmed:
                    config.trigger = isoft::uds::server::DiagnosticRecordTriggerEnum::kConfirmed;
                    break;
                case isoft::nml::definition::DiagnosticRecordTriggerEnum::kFdcThreshold:
                    config.trigger = isoft::uds::server::DiagnosticRecordTriggerEnum::kFdcThreshold;
                    break;
                case isoft::nml::definition::DiagnosticRecordTriggerEnum::kPending:
                    config.trigger = isoft::uds::server::DiagnosticRecordTriggerEnum::kPending;
                    break;
                case isoft::nml::definition::DiagnosticRecordTriggerEnum::kTestFailed:
                    config.trigger = isoft::uds::server::DiagnosticRecordTriggerEnum::kTestFailed;
                    break;
                case isoft::nml::definition::DiagnosticRecordTriggerEnum::kCustom:
                    config.trigger = isoft::uds::server::DiagnosticRecordTriggerEnum::kCustom;
                    break;
                case isoft::nml::definition::DiagnosticRecordTriggerEnum::kTestFailedThisOperationCycle:
                    config.trigger = isoft::uds::server::DiagnosticRecordTriggerEnum::kTestFailedThisOperationCycle;
                    break;

                default:
                    break;
            }
        }
        if (freezeFramePtr->update.get() != nullptr) {
            config.update = *freezeFramePtr->update;
        }

        if (freezeFramePtr->customTrigger.get() != nullptr) {
            config.customTrigger = *freezeFramePtr->customTrigger;
        }

        faultConfig_.freezeFrameConfig.insert({config.recordNumber, config});
        udsFreezeFrameMap_.insert({freezeFramePtr.get(), config.recordNumber});
    }
    return true;
}

bool UdsConfig::_initializeUdsDataIdentifierSet(
    std::vector< std::shared_ptr< isoft::nml::definition::UdsDataIdentifierSet > >
        UdsDataIdentifierSetTablePtr) noexcept
{
    std::uint32_t id{0};
    for (auto &&UdsDataIdentifierSetPtr : UdsDataIdentifierSetTablePtr) {
        if (UdsDataIdentifierSetPtr.get() == nullptr) {
            continue;
        }
        std::set< std::uint16_t > didTable{};
        for (auto &&dataIdentifierRef : UdsDataIdentifierSetPtr->dataIdentifierRef) {
            auto dataIdentifierPtr{dataIdentifierRef.lock()};
            if (dataIdentifierPtr.get() == nullptr) {
                continue;
            }
            if (dataIdentifierPtr->id.get() != nullptr) {
                didTable.insert(*dataIdentifierPtr->id);
            }
        }
        faultConfig_.dataIdentifierSet.insert({id, didTable});
        udsDataIdentifierSetMap_.insert({UdsDataIdentifierSetPtr.get(), id});
        id++;
    }
    return true;
}

bool UdsConfig::_initializeUdsFaultMemory(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::vector< std::shared_ptr< isoft::nml::definition::UdsFaultMemory > > UdsFaultMemoryTablePtr) noexcept
{
    for (auto &&UdsFaultMemoryPtr : UdsFaultMemoryTablePtr) {
        if (UdsFaultMemoryPtr.get() == nullptr) {
            continue;
        }
        isoft::uds::server::MemoryDestinationConfig config{};

        if (UdsFaultMemoryPtr->memoryId.get() != nullptr) {
            config.destinationID = *UdsFaultMemoryPtr->memoryId;
        }

        if (UdsFaultMemoryPtr->dtcStatusAvailabilityMask.get() != nullptr) {
            config.dtcStatusAvailabilityMask = *UdsFaultMemoryPtr->dtcStatusAvailabilityMask;
        }

        if (UdsFaultMemoryPtr->eventDisplacementStrategy.get() != nullptr) {
            switch (*UdsFaultMemoryPtr->eventDisplacementStrategy) {
                case isoft::nml::definition::DiagnosticEventDisplacementStrategyEnum::kNone:
                    config.displacementStrategy = isoft::uds::server::DiagnosticEventDisplacementStrategyEnum::kNone;
                    break;
                case isoft::nml::definition::DiagnosticEventDisplacementStrategyEnum::kFull:
                    config.displacementStrategy = isoft::uds::server::DiagnosticEventDisplacementStrategyEnum::kFull;
                    break;
                case isoft::nml::definition::DiagnosticEventDisplacementStrategyEnum::kPrioOcc:
                    config.displacementStrategy = isoft::uds::server::DiagnosticEventDisplacementStrategyEnum::kPrioOcc;
                    break;
                default:
                    break;
            }
        }

        if (UdsFaultMemoryPtr->maxNumberOfEventEntries.get() != nullptr) {
            config.maxNumberOfEventEntries = *UdsFaultMemoryPtr->maxNumberOfEventEntries;
        }

        if (UdsFaultMemoryPtr->memoryEntryStorageTrigger.get() != nullptr) {
            switch (*UdsFaultMemoryPtr->memoryEntryStorageTrigger) {
                case isoft::nml::definition::DiagnosticMemoryEntryStorageTriggerEnum::kConfirmed:
                    config.memoryEntryStorageTrigger
                        = isoft::uds::server::DiagnosticMemoryEntryStorageTriggerEnum::kConfirmed;
                    break;
                case isoft::nml::definition::DiagnosticMemoryEntryStorageTriggerEnum::kFdcThreshold:
                    config.memoryEntryStorageTrigger
                        = isoft::uds::server::DiagnosticMemoryEntryStorageTriggerEnum::kFdcThreshold;
                    break;
                case isoft::nml::definition::DiagnosticMemoryEntryStorageTriggerEnum::kPending:
                    config.memoryEntryStorageTrigger
                        = isoft::uds::server::DiagnosticMemoryEntryStorageTriggerEnum::kPending;
                    break;
                case isoft::nml::definition::DiagnosticMemoryEntryStorageTriggerEnum::kTestFailed:
                    config.memoryEntryStorageTrigger
                        = isoft::uds::server::DiagnosticMemoryEntryStorageTriggerEnum::kTestFailed;
                    break;
                default:
                    break;
            }
        }

        if (UdsFaultMemoryPtr->typeOfFFRNum.get() != nullptr) {
            switch (*UdsFaultMemoryPtr->typeOfFFRNum) {
                case isoft::nml::definition::DiagnosticTypeOfFreezeFrameRecordNumerationEnum::kCalculated:
                    config.typeOfFreezeFrameRecordNumeration
                        = isoft::uds::server::DiagnosticTypeOfFreezeFrameRecordNumerationEnum::kCalculated;
                    break;
                case isoft::nml::definition::DiagnosticTypeOfFreezeFrameRecordNumerationEnum::kConfigured:
                    config.typeOfFreezeFrameRecordNumeration
                        = isoft::uds::server::DiagnosticTypeOfFreezeFrameRecordNumerationEnum::kConfigured;
                    break;

                default:
                    break;
            }
        }

        if (UdsFaultMemoryPtr->typeOfDtcSupported.get() != nullptr) {
            switch (*UdsFaultMemoryPtr->typeOfDtcSupported) {
                case isoft::nml::definition::DiagnosticTypeOfDtcSupportedEnum::kIso142291:
                    config.typeOfDtcSupported = isoft::uds::server::DiagnosticTypeOfDtcSupportedEnum::kIso142291;
                    break;
                default:
                    break;
            }
        }
        if (UdsFaultMemoryPtr->clearDtcLimitation.get() != nullptr) {
            switch (*UdsFaultMemoryPtr->clearDtcLimitation) {
                case isoft::nml::definition::DiagnosticClearDtcLimitationEnum::kAllSupportedDtcs:
                    faultConfig_.commonProps.clearDtcLimitation
                        = isoft::uds::server::DiagnosticClearDtcLimitationEnum::kAllSupportedDtcs;
                    break;
                case isoft::nml::definition::DiagnosticClearDtcLimitationEnum::kClearAllDtcs:
                    faultConfig_.commonProps.clearDtcLimitation
                        = isoft::uds::server::DiagnosticClearDtcLimitationEnum::kClearAllDtcs;
                    break;
                default:
                    break;
            }
        }
        if (UdsFaultMemoryPtr->statusBitHandlingTFSLC.get() != nullptr) {
            switch (*UdsFaultMemoryPtr->statusBitHandlingTFSLC) {
                case isoft::nml::definition::DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum::
                    kStatusBitAgingAndDisplacement:
                    faultConfig_.commonProps.statusBitHandlingTestFailedSinceLastClear = isoft::uds::server::
                        DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum::kStatusBitAgingAndDisplacement;
                    break;
                case isoft::nml::definition::DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum::kStatusBitNormal:
                    faultConfig_.commonProps.statusBitHandlingTestFailedSinceLastClear
                        = isoft::uds::server::DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum::kStatusBitNormal;
                    break;
                default:
                    break;
            }
        }
        if (UdsFaultMemoryPtr->statusBitStorageTF.get() != nullptr) {
            faultConfig_.commonProps.statusBitStorageTestFailed = *UdsFaultMemoryPtr->statusBitStorageTF;
        }
        if (UdsFaultMemoryPtr->agingRequiresTestedCycle.get() != nullptr) {
            faultConfig_.commonProps.agingRequiresTestedCycle = *UdsFaultMemoryPtr->agingRequiresTestedCycle;
        }
        if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticDTCInformationPort,
                     UdsFaultMemoryPtr.get())) {
            std::uint32_t const serviceInstance{Hash32(UdsFaultMemoryPtr->isoftNmlObjectFqn)};
            std::shared_ptr< DtcInformation > dtcInformationPtr{std::make_shared< DtcInformation >(serviceInstance)};

            config.dtcInformationInterface = dtcInformationPtr;
        }

        faultConfig_.memoryDestinationConfig.insert({config.destinationID, config});
        udsFaultMemoryMap_.insert({UdsFaultMemoryPtr.get(), config.destinationID});
    }
    return true;
}

bool UdsConfig::_initializeUdsCondition(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::vector< std::shared_ptr< isoft::nml::definition::UdsCondition > > clearConditionTablePtr,
    std::vector< std::shared_ptr< isoft::nml::definition::UdsCondition > > enableConditionTablePtr) noexcept
{
    std::uint32_t id{0};
    for (auto &&clearConditionPtr : clearConditionTablePtr) {
        if (clearConditionPtr.get() == nullptr) {
            continue;
        }
        isoft::uds::server::ConditionConfig config{};
        if (clearConditionPtr->initValue.get() != nullptr) {
            config.state = *clearConditionPtr->initValue;
        }
        config.enableConditionId = id;
        if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticConditionPort,
                     clearConditionPtr.get())) {
            std::uint32_t const serviceInstance{Hash32(clearConditionPtr->isoftNmlObjectFqn)};
            std::shared_ptr< Condition > conditionPtr{std::make_shared< Condition >(serviceInstance)};
            config.conditionInterface = conditionPtr;
        }

        faultConfig_.clearConditionConfig.insert({id, config});
        udsClearConditionMap_.insert({clearConditionPtr.get(), id});
        id++;
    }
    id = 0;
    for (auto &&enableConditionPtr : enableConditionTablePtr) {
        if (enableConditionPtr.get() == nullptr) {
            continue;
        }
        isoft::uds::server::ConditionConfig config{};
        if (enableConditionPtr->initValue.get() != nullptr) {
            config.state = *enableConditionPtr->initValue;
        }
        config.enableConditionId = id;
        if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticConditionPort,
                     enableConditionPtr.get())) {
            std::uint32_t const serviceInstance{Hash32(enableConditionPtr->isoftNmlObjectFqn)};
            std::shared_ptr< Condition > conditionPtr{std::make_shared< Condition >(serviceInstance)};

            config.conditionInterface = conditionPtr;
        }

        faultConfig_.enableConditionConfig.insert({id, config});
        udsEnableConditionMap_.insert({enableConditionPtr.get(), id});
        id++;
    }
    return true;
}

bool UdsConfig::_initializeUdsIndicator(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::vector< std::shared_ptr< isoft::nml::definition::UdsIndicator > > indicatorTablePtr) noexcept
{
    std::uint8_t id{};
    for (auto &&indicatorPtr : indicatorTablePtr) {
        if (indicatorPtr.get() == nullptr) {
            continue;
        }
        isoft::uds::server::IndicatorConfig config{};
        if (indicatorPtr->type.get() != nullptr) {
            switch (*indicatorPtr->type) {
                case isoft::nml::definition::DiagnosticIndicatorTypeEnum::kAmberWarning:
                    config.type = isoft::uds::server::DiagnosticIndicatorTypeEnum::kAmberWarning;
                    break;
                case isoft::nml::definition::DiagnosticIndicatorTypeEnum::kMalfunction:
                    config.type = isoft::uds::server::DiagnosticIndicatorTypeEnum::kMalfunction;
                    break;
                case isoft::nml::definition::DiagnosticIndicatorTypeEnum::kProtectLamp:
                    config.type = isoft::uds::server::DiagnosticIndicatorTypeEnum::kProtectLamp;
                    break;
                case isoft::nml::definition::DiagnosticIndicatorTypeEnum::kRedStopLamp:
                    config.type = isoft::uds::server::DiagnosticIndicatorTypeEnum::kRedStopLamp;
                    break;
                case isoft::nml::definition::DiagnosticIndicatorTypeEnum::kWarning:
                    config.type = isoft::uds::server::DiagnosticIndicatorTypeEnum::kWarning;
                    break;

                default:
                    break;
            }
        }
        config.indicatorId = id;
        if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticIndicatorPort,
                     indicatorPtr.get())) {
            std::uint32_t const serviceInstance{Hash32(indicatorPtr->isoftNmlObjectFqn)};
            std::shared_ptr< Indicator > indicatorServicePtr{std::make_shared< Indicator >(serviceInstance)};
            config.indicatorInterface = indicatorServicePtr;
        }

        faultConfig_.indicatorConfig.insert({id, config});
        udsIndicatorMap_.insert({indicatorPtr.get(), id});
        id++;
    }
    return true;
}

bool UdsConfig::_initializeUdsDebounceAlgorithmType(
    std::vector< std::shared_ptr< isoft::nml::definition::UdsFaultManagement::DebounceAlgorithmType > >
        debounceAlgorithmTypeTable) noexcept
{
    std::uint32_t id{0};
    for (auto &&debounceAlgorithmType : debounceAlgorithmTypeTable) {
        if (debounceAlgorithmType->tag == isoft::nml::ModelTag::kUdsEventDebounceTimeBased) {
            isoft::uds::server::DebounceAlgorithmConfig config{};
            config.algorithmId = id;
            config.type        = isoft::uds::server::DebounceTypeEnum::kTimeBase;
            config.timeBased   = std::make_shared< isoft::uds::server::TimeBasedParam >();
            if (debounceAlgorithmType->udsEventDebounceTimeBased.get() != nullptr) {
                std::shared_ptr< isoft::nml::definition::UdsEventDebounceTimeBased > timerBasePtr{
                    debounceAlgorithmType->udsEventDebounceTimeBased};
                if (timerBasePtr->timeFailedThreshold.get() != nullptr) {
                    config.timeBased->timer.timeFailedThreshold = *timerBasePtr->timeFailedThreshold;
                }
                if (timerBasePtr->timePassedThreshold.get() != nullptr) {
                    config.timeBased->timer.timePassedThreshold = *timerBasePtr->timePassedThreshold;
                }
                if (timerBasePtr->timeFdcThresholdStore.get() != nullptr) {
                    config.timeBased->timeBasedFdcThresholdStorageValue
                        = SecondToMs(*timerBasePtr->timeFdcThresholdStore);
                }
                debounceAlgorithmType->udsEventDebounceTimeBased->timeFailedThreshold;
                if (timerBasePtr->debounceBehavior.get() != nullptr) {
                    switch (*timerBasePtr->debounceBehavior) {
                        case isoft::nml::definition::DiagnosticDebounceBehaviorEnum::kFreeze:
                            config.debounceBehavior = isoft::uds::server::DiagnosticDebounceBehaviorEnum::kFreeze;
                            break;
                        case isoft::nml::definition::DiagnosticDebounceBehaviorEnum::kReset:
                            config.debounceBehavior = isoft::uds::server::DiagnosticDebounceBehaviorEnum::kReset;
                            break;
                        default:
                            break;
                    }
                }
                udsEventDebounceTimeBasedMap_.insert({timerBasePtr.get(), id});
            }
            faultConfig_.debounceAlgorithmConfig.insert({id, config});

        } else if (debounceAlgorithmType->tag == isoft::nml::ModelTag::kUdsEventDebounceCounterBased) {
            isoft::uds::server::DebounceAlgorithmConfig config{};
            config.algorithmId  = id;
            config.type         = isoft::uds::server::DebounceTypeEnum::kCounterBase;
            config.counterBased = std::make_shared< isoft::uds::server::CounterBasedParam >();
            if (debounceAlgorithmType->udsEventDebounceCounterBased.get() != nullptr) {
                std::shared_ptr< isoft::nml::definition::UdsEventDebounceCounterBased > counterBasePtr{
                    debounceAlgorithmType->udsEventDebounceCounterBased};
                if (counterBasePtr->cntFdcThresholdStoreValue.get() != nullptr) {
                    config.counterBased->counterBasedFdcThresholdStorageValue
                        = *counterBasePtr->cntFdcThresholdStoreValue;
                }
                if (counterBasePtr->counterIncrementStepSize.get() != nullptr) {
                    config.counterBased->counter.counterIncrementStepSize = *counterBasePtr->counterIncrementStepSize;
                }
                if (counterBasePtr->counterDecrementStepSize.get() != nullptr) {
                    config.counterBased->counter.counterDecrementStepSize = *counterBasePtr->counterDecrementStepSize;
                }
                if (counterBasePtr->counterJumpDown.get() != nullptr) {
                    config.counterBased->counter.counterJumpDown = *counterBasePtr->counterJumpDown;
                }
                if (counterBasePtr->counterJumpDownValue.get() != nullptr) {
                    config.counterBased->counter.counterJumpDownValue = *counterBasePtr->counterJumpDownValue;
                }
                if (counterBasePtr->counterPassedThreshold.get() != nullptr) {
                    config.counterBased->counter.counterPassedThreshold = *counterBasePtr->counterPassedThreshold;
                }
                if (counterBasePtr->counterJumpUp.get() != nullptr) {
                    config.counterBased->counter.counterJumpUp = *counterBasePtr->counterJumpUp;
                }
                if (counterBasePtr->counterJumpUpValue.get() != nullptr) {
                    config.counterBased->counter.counterJumpUpValue = *counterBasePtr->counterJumpUpValue;
                }
                if (counterBasePtr->counterFailedThreshold.get() != nullptr) {
                    config.counterBased->counter.counterFailedThreshold = *counterBasePtr->counterFailedThreshold;
                }
                debounceAlgorithmType->udsEventDebounceTimeBased->timeFailedThreshold;
                if (counterBasePtr->debounceBehavior.get() != nullptr) {
                    switch (*counterBasePtr->debounceBehavior) {
                        case isoft::nml::definition::DiagnosticDebounceBehaviorEnum::kFreeze:
                            config.debounceBehavior = isoft::uds::server::DiagnosticDebounceBehaviorEnum::kFreeze;
                            break;
                        case isoft::nml::definition::DiagnosticDebounceBehaviorEnum::kReset:
                            config.debounceBehavior = isoft::uds::server::DiagnosticDebounceBehaviorEnum::kReset;
                            break;
                        default:
                            break;
                    }
                }
                udsEventDebounceCounterBasedMap_.insert({counterBasePtr.get(), id});
            }
            faultConfig_.debounceAlgorithmConfig.insert({id, config});
        }
        id++;
    }
    return true;
}

bool UdsConfig::_setConfigByUdsPermissionGroup(std::shared_ptr< isoft::nml::definition::UdsPermissionGroup > groupPtr,
                                               std::set< std::uint8_t > &sessionTable,
                                               std::set< std::uint8_t > &securityLevelTable,
                                               std::int32_t &evCondition,
                                               isoft::uds::server::EnableAuth &enableAuth) noexcept
{
    if (groupPtr.get() == nullptr) {
        return false;
    }
    for (auto &&sessionRef : groupPtr->sessionRef) {
        std::shared_ptr< isoft::nml::definition::UdsSession > sessionPtr{sessionRef.lock()};
        if (sessionPtr.get() != nullptr && sessionPtr->id.get() != nullptr) {
            sessionTable.insert(*sessionPtr->id);
        }
    }
    for (auto &&securityLevelRef : groupPtr->securityLevelRef) {
        std::shared_ptr< isoft::nml::definition::UdsSecurityLevel > securityLevelPtr{securityLevelRef.lock()};
        if (securityLevelPtr.get() != nullptr) {
            securityLevelTable.insert(udsSecurityLevelMap_.at(securityLevelPtr.get()).id);
        }
    }
    if (groupPtr->environmentalConditionRef.lock() != nullptr) {
        std::shared_ptr< isoft::nml::definition::UdsEnvironmentalCondition > envConditionPtr{
            groupPtr->environmentalConditionRef.lock()};
        evCondition = udsEnvironmentalConditionMap_.at(envConditionPtr.get());
    } else {
        evCondition = -1;
    }
    if (groupPtr->authenticationEnabled.get() != nullptr) {
        std::shared_ptr< isoft::nml::definition::UdsAuthRoleProxy > authenticationEnabledPtr{
            groupPtr->authenticationEnabled};
        enableAuth.enable = true;
        std::vector< std::weak_ptr< isoft::nml::definition::UdsAuthRole > > udsAuthRoleRefTable{
            authenticationEnabledPtr->authenticationRoleRef};
        for (auto &&udsAuthRoleRef : udsAuthRoleRefTable) {
            std::shared_ptr< isoft::nml::definition::UdsAuthRole > udsAuthRolePtr{udsAuthRoleRef.lock()};
            if (udsAuthRolePtr.get() != nullptr) {
                if (udsAuthRolePtr->shortName.get() != nullptr) {
                    isoft::uds::server::AuthRoleName authRoleName{*udsAuthRolePtr->shortName};
                    enableAuth.authRoleList.insert(authRoleName);
                }
            }
        }
    }
    return true;
}

bool UdsConfig::_initializeUdsClearDiagnosticInformationService(
    std::shared_ptr< isoft::nml::definition::UdsClearDiagnosticInformationService > serviceX14Ptr) noexcept
{
    if (serviceX14Ptr.get() == nullptr) {
        return true;
    }
    isoft::uds::server::ClearDiagnosticInformationInstanceConfig config{};

    if (serviceX14Ptr->subfunction.get() != nullptr) {
        _setConfigByUdsPermissionGroup(serviceX14Ptr->subfunction->permissionGroupRef.lock(),
                                       config.accessPermissionSession, config.accessPermissionSecurityLevel,
                                       config.accessPermissionEnvCondition, config.accessPermissionAuth);
    }
    faultConfig_.clearDiagnosticInformationInstanceConfig = config;
    return true;
}

bool UdsConfig::_initializeUdsReadDTCInformationService(
    std::shared_ptr< isoft::nml::definition::UdsReadDTCInformationService > serviceX19Ptr) noexcept
{
    if (serviceX19Ptr.get() == nullptr) {
        return true;
    }
    isoft::uds::server::ReadDTCInformationInstanceConfig config{};
    for (auto &&subfunction : serviceX19Ptr->subfunction) {
        if (subfunction.get() != nullptr) {
            isoft::uds::server::ReadDTCInformationInstance instance{};
            if (subfunction->id.get() != nullptr) {
                std::string category{*subfunction->id};
                if (category == "REPORT_NUMBER_OF_DTC_BY_STATUS_MASK") {
                    instance.subfunctionNumber = ReadDTCInformationSubfunction::kReportNumberOfDtcByStatusMask;
                } else if (category == "REPORT_DTC_BY_STATUS_MASK") {
                    instance.subfunctionNumber = ReadDTCInformationSubfunction::kReportDtcByStatusMask;
                } else if (category == "REPORT_DTC_SNAPSHOT_RECORD_BY_DTC_NUMBER") {
                    instance.subfunctionNumber = ReadDTCInformationSubfunction::kReportDtcSnapshotRecordByDtcNumber;
                } else if (category == "REPORT_DTC_EXT_DATA_RECORD_BY_DTC_NUMBER") {
                    instance.subfunctionNumber = ReadDTCInformationSubfunction::kReportDtcExtDataRecordByDtcNumber;
                } else if (category == "REPORT_NUMBER_OF_DTC_BY_SEVERITY_MASK_RECORD") {
                    instance.subfunctionNumber = ReadDTCInformationSubfunction::kReportNumberOfDtcBySeverityMaskRecord;
                } else if (category == "REPORT_SUPPORTED_DTCS") {
                    instance.subfunctionNumber = ReadDTCInformationSubfunction::kReportSupportedDtcs;
                } else if (category == "REPORT_DTC_FAULT_DETECTION_COUNTER") {
                    instance.subfunctionNumber = ReadDTCInformationSubfunction::kReportDtcFaultDetectionCounter;
                } else if (category == "REPORT_USER_DEF_MEMORY_DTC_BY_STATUS_MASK") {
                    instance.subfunctionNumber = ReadDTCInformationSubfunction::kReportUserDefMemoryDtcByStatusMask;
                } else if (category == "REPORT_USER_DEF_MEMORY_DTC_SNAPSHOT_RECORD_BY_DTC_NUMBER") {
                    instance.subfunctionNumber
                        = ReadDTCInformationSubfunction::kReportUserDefMemoryDtcSnapshotRecordByDtcNumber;
                } else if (category == "REPORT_USER_DEF_MEMORY_DTC_EXT_DATA_RECORD_BY_DTC_NUMBER") {
                    instance.subfunctionNumber
                        = ReadDTCInformationSubfunction::kReportUserDefMemoryDtcExtDataRecordByDtcNumber;
                } else {
                    LogError() << "UdsConfig::_initializeServiceX19|failed to parse category =" << category.c_str();
                    return false;
                }
            }
            std::int32_t accessPermissionEnvCondition{};
            _setConfigByUdsPermissionGroup(subfunction->permissionGroupRef.lock(), instance.accessPermissionSession,
                                           instance.accessPermissionSecurityLevel, accessPermissionEnvCondition,
                                           instance.accessPermissionAuth);
            config.instances.emplace(instance);
        }
    }
    faultConfig_.readDTCInformationInstanceConfig = config;
    return true;
}

bool UdsConfig::_initializeUdsControlDTCSettingService(
    std::shared_ptr< isoft::nml::definition::UdsControlDTCSettingService > serviceX85Ptr) noexcept
{
    if (serviceX85Ptr.get() == nullptr) {
        return true;
    }
    isoft::uds::server::ControlDTCSettingInstanceConfig config{};
    for (auto &&subfunction : serviceX85Ptr->subfunction) {
        if (subfunction.get() != nullptr) {
            isoft::uds::server::ControlDTCSettingInstance instance{};
            if (subfunction->id.get() != nullptr) {
                instance.subfunctionNumber = static_cast< ControlDTCSettingSubfunction >(*subfunction->id);
            }
            std::int32_t accessPermissionEnvCondition{};
            _setConfigByUdsPermissionGroup(subfunction->permissionGroupRef.lock(), instance.accessPermissionSession,
                                           instance.accessPermissionSecurityLevel, accessPermissionEnvCondition,
                                           instance.accessPermissionAuth);
            config.instances.emplace(instance);
        }
    }
    faultConfig_.controlDTCSettingInstanceConfig = config;
    return true;
}

bool UdsConfig::_initializeUdsValidation(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::shared_ptr< isoft::nml::definition::UdsValidation > udsValidationPtr) noexcept
{
    if (udsValidationPtr.get() == nullptr) {
        common::LogInfo() << "UdsConfig::_initializeUdsValidation| udsvalidation load fail!";
        return true;
    }
    if (udsValidationPtr->authenticationTimeout.get() != nullptr) {
        authenticationManagement_.authenticationTimeout = SecondToMs(*udsValidationPtr->authenticationTimeout);
    }
    if (!_initializeUdsExternalAuthenticationIdentification(udsValidationPtr->externalAuthentication)) {
        common::LogInfo() << "UdsConfig::_initializeUdsValidation| external Authentication load fails!";
        return true;
    }
    if (!_initializeUdsAuthRole(udsValidationPtr->authRole)) {
        common::LogInfo() << "UdsConfig::_initializeUdsValidation| auth role load fails!";
        return true;
    }
    if (!_initializeUdsSession(udsValidationPtr->session)) {
        common::LogInfo() << "UdsConfig::_initializeUdsValidation| session load fails!";
        return true;
    }
    if (!_initializeUdsSecurityLevel(adaptiveSoftwareComponentTable, udsValidationPtr->securityLevel)) {
        common::LogInfo() << "UdsConfig::_initializeUdsValidation| securityLevel load fails!";
        return false;
    }
    if (!_initializeUdsEnvironmentalCondition(udsValidationPtr->environmentalCondition)) {
        common::LogInfo() << "UdsConfig::_initializeUdsValidation| environmentalCondition load fails!";
        return true;
    }
    if (!_initializeUdsPermissionGroup(udsValidationPtr->permissionGroup)) {
        common::LogInfo() << "UdsConfig::_initializeUdsValidation| permissionGroup load fails!";
        return true;
    }
    if (!_initializeUdsServiceValidationInstance(adaptiveSoftwareComponentTable,
                                                 udsValidationPtr->manufacturerValidation,
                                                 serverSetting_.manufacturerServiceValidationTable)) {
        common::LogInfo() << "UdsConfig::_initializeUdsValidation| manufacturerValidation load fails!";
        return true;
    }
    if (!_initializeUdsServiceValidationInstance(adaptiveSoftwareComponentTable, udsValidationPtr->supplierValidation,
                                                 serverSetting_.supplierServiceValidationTable)) {
        common::LogInfo() << "UdsConfig::_initializeUdsValidation| supplierValidation load fails!";
        return true;
    }
    return true;
}

bool UdsConfig::_initializeUdsExternalAuthenticationIdentification(
    std::vector< std::shared_ptr< isoft::nml::definition::UdsExternalAuthenticationIdentification > >
        udsExtAuthIdTable) noexcept
{
    for (auto &&udsExtAuthIdPtr : udsExtAuthIdTable) {
        if (udsExtAuthIdPtr.get() != nullptr) {
            isoft::uds::server::ExternalAuthenticationIdentification exAuthId{};
            if (udsExtAuthIdPtr->sourceAddressCode.get() != nullptr) {
                exAuthId.sourceAddressCode = *udsExtAuthIdPtr->sourceAddressCode;
            }
            if (udsExtAuthIdPtr->sourceAddressMask.get() != nullptr) {
                exAuthId.sourceAddressMask = *udsExtAuthIdPtr->sourceAddressMask;
            }
            authenticationManagement_.externalAuthenticationList.push_back(exAuthId);
        }
    }
    return true;
}

bool UdsConfig::_initializeUdsAuthRole(
    std::vector< std::shared_ptr< isoft::nml::definition::UdsAuthRole > > udsAuthRoleTable) noexcept
{
    for (auto &&udsAuthRolePtr : udsAuthRoleTable) {
        if (udsAuthRolePtr.get() != nullptr) {
            isoft::uds::server::AuthRole authRoleModel{};
            if (udsAuthRolePtr->isDefault.get() != nullptr) {
                authRoleModel.isDefault = *udsAuthRolePtr->isDefault;
            }
            if (udsAuthRolePtr->shortName.get() != nullptr) {
                authRoleModel.shortName = *udsAuthRolePtr->shortName;
            }
            if (udsAuthRolePtr->bitPosition.get() != nullptr) {
                authRoleModel.bitPosition = *udsAuthRolePtr->bitPosition;
            }
            authenticationManagement_.authRoleList.push_back(authRoleModel);
        }
    }
    return true;
}

bool UdsConfig::_initializeUdsSession(
    std::vector< std::shared_ptr< isoft::nml::definition::UdsSession > > udsSessionTable) noexcept
{
    for (auto &&sessionPtr : udsSessionTable) {
        if (sessionPtr.get() != nullptr) {
            isoft::uds::server::SessionModel sessionModel{};
            if (sessionPtr->id.get() != nullptr) {
                sessionModel.id = *sessionPtr->id;
            }
            if (sessionPtr->shortName.get() != nullptr) {
                sessionModel.shortName = *sessionPtr->shortName;
            }
            if (sessionPtr->p2ServerMax.get() != nullptr) {
                sessionModel.p2ServerMax = SecondToMs(*sessionPtr->p2ServerMax);
            }
            if (sessionPtr->p2StarServerMax.get() != nullptr) {
                sessionModel.p2StarServerMax = SecondToMs(*sessionPtr->p2StarServerMax) / 10;
            }
            sessionManagement_.sessionConfigTable.emplace(sessionModel);
        }
    }
    return true;
}

bool UdsConfig::_initializeUdsSecurityLevel(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::vector< std::shared_ptr< isoft::nml::definition::UdsSecurityLevel > > udsSecurityLevelTable) noexcept
{
    std::uint8_t id{1};
    for (auto &&securityLevelPtr : udsSecurityLevelTable) {
        if (securityLevelPtr.get() != nullptr) {
            isoft::uds::server::SecurityLevelConfig securityLevelConfig{};
            if (securityLevelPtr->shortName.get() != nullptr) {
                securityLevelConfig.shortName = *securityLevelPtr->shortName;
            }
            if (securityLevelPtr->seedSize.get() != nullptr) {
                securityLevelConfig.seedSize = *securityLevelPtr->seedSize;
            }
            if (securityLevelPtr->keySize.get() != nullptr) {
                securityLevelConfig.keySize = *securityLevelPtr->keySize;
            }
            if (securityLevelPtr->numFailedSecurityAccess.get() != nullptr) {
                securityLevelConfig.numFailedSecurityAccess = *securityLevelPtr->numFailedSecurityAccess;
            }
            if (securityLevelPtr->securityDelayTime.get() != nullptr) {
                securityLevelConfig.securityDelayTime = SecondToMs(*securityLevelPtr->securityDelayTime);
            }
            if (securityLevelPtr->accessDataRecordSize.get() != nullptr) {
                securityLevelConfig.accessDataRecordSize = *securityLevelPtr->accessDataRecordSize;
            }
            if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticSecurityAccessPort,
                         securityLevelPtr.get())) {
                std::vector< std::string > serviceTable
                    = _getPortFqn(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticSecurityAccessPort,
                                  securityLevelPtr.get());
                if (!serviceTable.empty()) {
                    std::uint16_t const instance{Hash16(securityLevelPtr->isoftNmlObjectFqn)};
                    std::uint32_t const serviceInstance{Hash32(serviceTable.front())};
                    std::shared_ptr< SecurityAccess > securityAccessPtr{
                        std::make_shared< SecurityAccess >(instance, serviceInstance)};
                    securityLevelConfig.interfacePtr = securityAccessPtr;
                }
            } else {
                common::LogError() << "UdsConfig::_initializeUdsSecurityLevel| must config security level port!";
                return false;
            }
            securityLevelConfig.id = id;
            id++;
            id++;
            udsSecurityLevelMap_.insert({securityLevelPtr.get(), securityLevelConfig});
            securityAccessManagement_.securityLevelTable.emplace(securityLevelConfig);
        }
    }
    return true;
}

bool UdsConfig::_initializeUdsEnvironmentalCondition(
    std::vector< std::shared_ptr< isoft::nml::definition::UdsEnvironmentalCondition > >
        udsEnvironmentalConditionTable) noexcept
{
    std::uint32_t id{0};
    for (auto &&udsEnvironmentalConditionPtr : udsEnvironmentalConditionTable) {
        if (udsEnvironmentalConditionPtr.get() != nullptr) {
            std::shared_ptr< isoft::uds::server::EnvConditionFormula > formulaPtr{};
            if (_initializeUdsEnvironmentalConditionFormula(udsEnvironmentalConditionPtr->formula, formulaPtr)) {
                isoft::uds::server::EnvironmentalConditionModelPtr ptr{
                    std::make_shared< isoft::uds::server::EnvironmentalConditionModel >()};
                ptr->id      = id;
                ptr->formula = *formulaPtr;
                udsEnvironmentalConditionMap_.insert({udsEnvironmentalConditionPtr.get(), id});
                diagnosticDataManagement_.environmentalConditionManager.emplace_back(ptr);
                id++;
            }
        }
    }
    return true;
}

bool UdsConfig::_initializeUdsEnvironmentalConditionFormula(
    std::shared_ptr< isoft::nml::definition::UdsEnvironmentalConditionFormula > udsEnvironmentalConditionFormulaPtr,
    std::shared_ptr< isoft::uds::server::EnvConditionFormula > &formulaPtr) noexcept
{
    if (udsEnvironmentalConditionFormulaPtr.get() == nullptr) {
        return false;
    }
    formulaPtr = std::make_shared< isoft::uds::server::EnvConditionFormula >();
    if (udsEnvironmentalConditionFormulaPtr->nrcValue.get() != nullptr) {
        formulaPtr->nrcValue = *udsEnvironmentalConditionFormulaPtr->nrcValue;
    }
    if (udsEnvironmentalConditionFormulaPtr->op.get() != nullptr) {
        switch (*udsEnvironmentalConditionFormulaPtr->op) {
            case isoft::nml::definition::UdsLogicalOperatorEnum::kLogicalAnd:
                formulaPtr->op = isoft::uds::server::DiagnosticLogicalOperatorEnum::kLogicalAnd;
                break;

            case isoft::nml::definition::UdsLogicalOperatorEnum::kLogicalOr:
                formulaPtr->op = isoft::uds::server::DiagnosticLogicalOperatorEnum::kLogicalOr;
                break;
            default:
                break;
        }
    }
    for (auto &&partPtr : udsEnvironmentalConditionFormulaPtr->part) {
        if (partPtr->tag == isoft::nml::ModelTag::kUdsEnvironmentalConditionFormula) {
            std::shared_ptr< isoft::uds::server::EnvConditionFormula > ptr{nullptr};
            if (_initializeUdsEnvironmentalConditionFormula(partPtr->udsEnvironmentalConditionFormula, ptr)) {
                formulaPtr->parts.emplace_back(ptr);
            }
        } else if (partPtr->tag == isoft::nml::ModelTag::kUdsEnvironmentalDataElementCondition) {
            std::shared_ptr< isoft::uds::server::EnvDataCondition > ptr{nullptr};
            if (_initializeUdsEnvironmentalDataElementCondition(partPtr->udsEnvironmentalDataElementCondition, ptr)) {
                formulaPtr->parts.emplace_back(ptr);
            }
        }
    }
    return true;
}

bool UdsConfig::_initializeUdsEnvironmentalDataElementCondition(
    std::shared_ptr< isoft::nml::definition::UdsEnvironmentalDataElementCondition >
        udsEnvironmentalDataElementConditionPtr,
    std::shared_ptr< isoft::uds::server::EnvDataCondition > &dataConditionPtr) noexcept
{
    if (udsEnvironmentalDataElementConditionPtr.get() == nullptr) {
        return false;
    }
    dataConditionPtr = std::make_shared< isoft::uds::server::EnvDataCondition >();
    if (udsEnvironmentalDataElementConditionPtr->compareType.get() != nullptr) {
        switch (*udsEnvironmentalDataElementConditionPtr->compareType) {
            case isoft::nml::definition::UdsCompareTypeEnum::kIsEqual:
                dataConditionPtr->compareType = isoft::uds::server::DiagnosticCompareTypeEnum::kIsEqual;
                break;
            case isoft::nml::definition::UdsCompareTypeEnum::kIsNotEqual:
                dataConditionPtr->compareType = isoft::uds::server::DiagnosticCompareTypeEnum::kIsNotEqual;
                break;
            case isoft::nml::definition::UdsCompareTypeEnum::kIsLessThan:
                dataConditionPtr->compareType = isoft::uds::server::DiagnosticCompareTypeEnum::kIsLessThan;
                break;
            case isoft::nml::definition::UdsCompareTypeEnum::kIsLessOrEqual:
                dataConditionPtr->compareType = isoft::uds::server::DiagnosticCompareTypeEnum::kIsLessOrEqual;
                break;
            case isoft::nml::definition::UdsCompareTypeEnum::kIsGreaterThan:
                dataConditionPtr->compareType = isoft::uds::server::DiagnosticCompareTypeEnum::kIsGreaterThan;
                break;
            case isoft::nml::definition::UdsCompareTypeEnum::kIsGreaterOrEqual:
                dataConditionPtr->compareType = isoft::uds::server::DiagnosticCompareTypeEnum::kIsGreaterOrEqual;
                break;

            default:
                break;
        }
    }
    std::shared_ptr< isoft::nml::definition::UdsDataElement > elementPtr{
        udsEnvironmentalDataElementConditionPtr->dataElementRef.lock()};
    if (elementPtr.get() != nullptr) {
        ara::core::String name{elementPtr->isoftNmlObjectFqn.c_str()};
        dataConditionPtr->element = GetDiagnosticDataElementId(name);
    }
    if (udsEnvironmentalDataElementConditionPtr->compareValue.get() != nullptr) {
        if (elementPtr.get() != nullptr) {
            dataConditionPtr->compareValue
                = Serialize(*udsEnvironmentalDataElementConditionPtr->compareValue, elementPtr);
        }
    }
    return true;
}

bool UdsConfig::_initializeUdsPermissionGroup(
    std::vector< std::shared_ptr< isoft::nml::definition::UdsPermissionGroup > > udsPermissionGroupTable) noexcept
{
    for (auto &&udsPermissionGroupPtr : udsPermissionGroupTable) {
        if (udsPermissionGroupPtr.get() == nullptr) {
            continue;
        }
        AccessPermission cfg{};
        if (udsPermissionGroupPtr->authenticationEnabled.get() != nullptr) {
            cfg.enableAuth.enable = true;
            auto authRoleProxyPtr{udsPermissionGroupPtr->authenticationEnabled};
            for (auto &&authRolePtr : authRoleProxyPtr->authenticationRoleRef) {
                if (authRolePtr.lock() != nullptr) {
                    cfg.enableAuth.authRoleList.insert(*authRolePtr.lock()->shortName);
                }
            }
        }
        for (auto &&sessionPtr : udsPermissionGroupPtr->sessionRef) {
            if (sessionPtr.lock() != nullptr) {
                cfg.sessionTable.emplace_back(*sessionPtr.lock()->id);
            }
        }
        for (auto &&securityLevelPtr : udsPermissionGroupPtr->securityLevelRef) {
            if (securityLevelPtr.lock() != nullptr) {
                isoft::uds::server::SecurityLevelConfig securityLevelConfig{};
                securityLevelConfig = udsSecurityLevelMap_.at(securityLevelPtr.lock().get());
                cfg.securityLevelTable.emplace_back(securityLevelConfig.id);
            }
        }
        if (udsPermissionGroupPtr->environmentalConditionRef.lock().get() != nullptr) {
            std::uint32_t id{};
            id = udsEnvironmentalConditionMap_.at(udsPermissionGroupPtr->environmentalConditionRef.lock().get());
            cfg.evCondition = id;
        }
        udsPermissionGroupMap_.insert({udsPermissionGroupPtr.get(), cfg});
    }
    return true;
}

bool UdsConfig::_initializeUdsServiceValidationInstance(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::vector< std::shared_ptr< isoft::nml::definition::UdsServiceValidationInstance > >
        udsServiceValidationInstanceTable,
    std::vector< std::shared_ptr< isoft::uds::server::ServiceValidationInterface > > &instanceTable) noexcept
{
    for (auto &&udsServiceValidationInstancePtr : udsServiceValidationInstanceTable) {
        if (udsServiceValidationInstancePtr.get() == nullptr) {
            continue;
        }
        if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticServiceValidationPort,
                     udsServiceValidationInstancePtr.get())) {
            std::vector< std::string > serviceTable
                = _getPortFqn(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticServiceValidationPort,
                              udsServiceValidationInstancePtr.get());
            if (!serviceTable.empty()) {
                std::uint16_t const instance{Hash16(udsServiceValidationInstancePtr->isoftNmlObjectFqn)};
                std::uint32_t const serviceInstance{Hash32(serviceTable.front())};
                std::shared_ptr< ServiceValidation > ServiceValidationPtr{
                    std::make_shared< ServiceValidation >(instance, serviceInstance)};
                instanceTable.emplace_back(ServiceValidationPtr);
            }
        }
    }
    return true;
}
bool UdsConfig::_initializeUdsSessionControlService(
    std::shared_ptr< isoft::nml::definition::UdsSessionControlService > udsSessionControlServicePtr) noexcept
{
    if (udsSessionControlServicePtr.get() == nullptr) {
        common::LogWarn() << "UdsConfig::_initializeUdsSessionControlService| not config x10 service!";
        return false;
    }
    for (auto &&subfunctionPtr : udsSessionControlServicePtr->subfunction) {
        if (subfunctionPtr.get() == nullptr) {
            continue;
        }
        isoft::uds::server::SessionControlInstanceConfig config{};

        std::shared_ptr< isoft::nml::definition::UdsSession > udsSessionPtr{subfunctionPtr->sessionRef.lock()};
        if (udsSessionPtr.get() != nullptr) {
            config.sesssionIdConfig = *udsSessionPtr->id;
        }
        std::shared_ptr< isoft::nml::definition::UdsPermissionGroup > udsPermissionGroup{
            subfunctionPtr->permissionGroupRef.lock()};
        std::int32_t evCondition{};
        _setConfigByUdsPermissionGroup(udsPermissionGroup, config.sessionPermission, config.securityLevelPermission,
                                       evCondition, config.accessPermissionAuth);
        sessionManagement_.sessionControlInstanceTable.emplace(config);
    }
    return true;
}

bool UdsConfig::_initializeUdsSecurityAccessService(
    std::shared_ptr< isoft::nml::definition::UdsSecurityAccessService > udsSecurityAccessServicePtr) noexcept
{
    if (udsSecurityAccessServicePtr.get() == nullptr) {
        common::LogWarn() << "UdsConfig::_initializeUdsSecurityAccessService| not config x27 service!";
        return false;
    }
    for (auto &&subfunctionPtr : udsSecurityAccessServicePtr->subfunction) {
        if (subfunctionPtr.get() == nullptr) {
            continue;
        }
        isoft::uds::server::SecurityAccessInstanceConfig config{};

        std::shared_ptr< isoft::nml::definition::UdsSecurityLevel > udsSecurityLevelPtr{
            subfunctionPtr->securityLevelRef.lock()};
        if (udsSecurityLevelPtr.get() != nullptr) {
            config.securityLevel = udsSecurityLevelMap_.at(udsSecurityLevelPtr.get());
            config.requestSeedId = config.securityLevel.id;
        }
        if (subfunctionPtr->securityDelayTimeOnBoot.get() != nullptr) {
            serverSetting_.securityDelayTimeOnBoot = *subfunctionPtr->securityDelayTimeOnBoot;
        }
        std::shared_ptr< isoft::nml::definition::UdsPermissionGroup > udsPermissionGroup{
            subfunctionPtr->permissionGroupRef.lock()};
        std::int32_t evCondition{};
        _setConfigByUdsPermissionGroup(udsPermissionGroup, config.accessPermissionSession,
                                       config.accessPermissionSecurityLevel, evCondition, config.accessPermissionAuth);
        securityAccessManagement_.securityAccessInstanceTable.emplace(config);
    }
    if (udsSecurityAccessServicePtr->sharedTimer.get() != nullptr) {
        securityAccessManagement_.sharedTimer = *udsSecurityAccessServicePtr->sharedTimer;
    }
    return true;
}
bool UdsConfig::_initializeUdsDataIdentifier(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::vector< std::shared_ptr< isoft::nml::definition::UdsDataIdentifier > > udsDataIdentifierTable) noexcept
{
    for (auto &&udsDataIdentifier : udsDataIdentifierTable) {
        if (udsDataIdentifier.get() == nullptr) {
            continue;
        }

        if (udsDataIdentifier->isDynamic.get() == nullptr) {
            continue;
        }

        if (*udsDataIdentifier->isDynamic) {
            isoft::uds::server::DiagnosticDynamicDataIdentifier dynamicDataIdentifier{};
            if (udsDataIdentifier->id.get() != nullptr) {
                dynamicDataIdentifier = *udsDataIdentifier->id;
                diagnosticDataManagement_.didManager.dynamicData.emplace_back(dynamicDataIdentifier);
            }
        } else {
            isoft::uds::server::DiagnosticDataModel dataModel{};
            if (udsDataIdentifier->id.get() != nullptr) {
                dataModel.id = *udsDataIdentifier->id;
            }
            if (udsDataIdentifier->didSize.get() != nullptr) {
                dataModel.nSize = *udsDataIdentifier->didSize;
            }
            for (auto &&dataElementPtr : udsDataIdentifier->dataElement) {
                if (dataElementPtr.get() == nullptr) {
                    continue;
                }
                isoft::uds::server::DiagnosticDataParamterModel paramterModel{};
                if (dataElementPtr->bitOffset.get() != nullptr) {
                    paramterModel.offset = *dataElementPtr->bitOffset;
                }
                if (dataElementPtr->arraySizeSemantics.get() != nullptr) {
                    paramterModel.element.array = std::make_shared< isoft::uds::server::DiagnosticDataElementArray >();
                    switch (*dataElementPtr->arraySizeSemantics) {
                        case isoft::nml::definition::ArraySizeSemanticsEnum::kFixedSize:
                            paramterModel.element.array->isVariableSize = false;
                            break;
                        case isoft::nml::definition::ArraySizeSemanticsEnum::kVariableSize:
                            paramterModel.element.array->isVariableSize = true;
                            break;
                        default:
                            break;
                    }
                }
                if (dataElementPtr->maxNumberOfElement.get() != nullptr) {
                    if (paramterModel.element.array.get() == nullptr) {
                        paramterModel.element.array
                            = std::make_shared< isoft::uds::server::DiagnosticDataElementArray >();
                    }
                    paramterModel.element.array->maxNumberOfElements = *dataElementPtr->maxNumberOfElement;
                }
                if (!dataElementPtr->baseTypeRef.expired()) {
                    paramterModel.element.dataType = GetBaseType(dataElementPtr->baseTypeRef.lock());
                }
                if (dataElementPtr->internalProvider.get() != nullptr) {
                    std::string category{*dataElementPtr->internalProvider};
                    if (category == "DCM_SESSION") {
                        paramterModel.element.provider = isoft::uds::server::DiagnosticDataElementProvider::kDcmSession;
                    } else if (category == "DCM_SECURITY_LEVEL") {
                        paramterModel.element.provider
                            = isoft::uds::server::DiagnosticDataElementProvider::kDcmSecurityLevel;
                    }
                    dataModel.readType = isoft::uds::server::DiagnosticDataReadMethod::kUseReadElementMothod;
                }
                if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticDataPort,
                             dataElementPtr.get())) {
                    paramterModel.element.provider = isoft::uds::server::DiagnosticDataElementProvider::kExternal;
                    dataModel.readType = isoft::uds::server::DiagnosticDataReadMethod::kUseReadElementMothod;
                    if (genericDataElementPtr_.get() == nullptr) {
                        genericDataElementPtr_ = std::make_shared< GenericDataElement >();
                    }
                    std::vector< std::string > serviceTable
                        = _getPortFqn(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticDataPort,
                                      dataElementPtr.get());
                    std::uint32_t elementId{};
                    ara::core::String name{dataElementPtr->isoftNmlObjectFqn};
                    elementId = GetDiagnosticDataElementId(name);
                    std::uint16_t const instance{Hash16(dataElementPtr->isoftNmlObjectFqn)};
                    std::uint32_t const serviceInstance{Hash32(dataElementPtr->isoftNmlObjectFqn)};
                    genericDataElementPtr_->RegisterServiceInstance(elementId, instance, serviceInstance);
                }
                dataModel.details.emplace_back(paramterModel);
            }

            std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent::PortType > portTypePtr{};
            portTypePtr = _getPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticDataPort,
                                   udsDataIdentifier.get());
            if (portTypePtr.get() != nullptr && portTypePtr->diagnosticDataPort.get() != nullptr) {
                std::shared_ptr< isoft::nml::definition::DiagnosticDataPort > dataPortPtr{
                    portTypePtr->diagnosticDataPort};
                if (genericDataIdentifierPtr_.get() == nullptr) {
                    genericDataIdentifierPtr_                             = std::make_shared< GenericDataIdentifier >();
                    diagnosticDataManagement_.didManager.dataInterfacePtr = genericDataIdentifierPtr_;
                }
                if (dataPortPtr->interfaceType.get() != nullptr) {
                    ara::core::String typeName{};
                    if (*dataPortPtr->interfaceType == isoft::nml::definition::DiagnosticDataInterfaceEnum::kTyped) {
                        typeName = "DIAGNOSTIC-DATA-IDENTIFIER-INTERFACE";
                        std::uint16_t const instance{Hash16(udsDataIdentifier->isoftNmlObjectFqn)};
                        std::uint32_t const serviceInstance{Hash32(dataPortPtr->isoftNmlObjectFqn)};
                        genericDataIdentifierPtr_->RegisterServiceInstance(dataModel.id, instance, serviceInstance,
                                                                           typeName);
                    } else if (*dataPortPtr->interfaceType
                               == isoft::nml::definition::DiagnosticDataInterfaceEnum::kGeneric) {
                        typeName = "DIAGNOSTIC-DATA-IDENTIFIER-GENERIC-INTERFACE";
                        std::uint16_t const instance{Hash16(udsDataIdentifier->isoftNmlObjectFqn)};
                        std::uint32_t const serviceInstance{Hash32(dataPortPtr->isoftNmlObjectFqn)};
                        genericDataIdentifierPtr_->RegisterServiceInstance(dataModel.id, instance, serviceInstance,
                                                                           typeName);
                    }
                }
            }
            diagnosticDataManagement_.didManager.staticData.emplace_back(dataModel);
        }
    }
    if (genericDataElementPtr_.get() == nullptr) {
        common::LogInfo() << "UdsConfig::_initializeUdsDataIdentifier| genericDataElementPtr_ is nullptr!";
    }
    diagnosticDataManagement_.didManager.dataElementInterfacePtr = genericDataElementPtr_;
    return true;
}

bool UdsConfig::_initializeUdsReadDataByIdentifierService(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::shared_ptr< isoft::nml::definition::UdsReadDataByIdentifierService >
        udsReadDataByIdentifierServicePtr) noexcept
{
    if (udsReadDataByIdentifierServicePtr.get() == nullptr) {
        common::LogWarn() << "UdsConfig::_initializeUdsReadDataByIdentifierService| not config x22 service!";
        return false;
    }
    diagnosticDataManagement_.service.serviceX22 = std::make_shared< isoft::uds::server::ServiceX22Model >();
    for (auto &&readDataByIdentifierPtr : udsReadDataByIdentifierServicePtr->readDataByIdentifier) {
        if (readDataByIdentifierPtr.get() == nullptr) {
            continue;
        }
        isoft::uds::server::ReadDiagnosticDataByIdentifier config{};

        if (!readDataByIdentifierPtr->dataIdentifierRef.expired()) {
            std::shared_ptr< isoft::nml::definition::UdsDataIdentifier > udsDataIdentifierPtr{
                readDataByIdentifierPtr->dataIdentifierRef.lock()};
            config.id = *udsDataIdentifierPtr->id;
        }
        std::shared_ptr< isoft::nml::definition::UdsPermissionGroup > udsPermissionGroup{
            readDataByIdentifierPtr->permissionGroupRef.lock()};
        _setConfigByUdsPermissionGroup(udsPermissionGroup, config.accessPermissionSession,
                                       config.accessPermissionSecurityLevel, config.accessPermissionEnvCondition,
                                       config.accessPermissionAuth);
        std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent::PortType > portTypePtr{};
        portTypePtr = _getPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticGenericUDSServicePort,
                               readDataByIdentifierPtr.get());
        if (portTypePtr.get() != nullptr && portTypePtr->diagnosticGenericUDSServicePort.get() != nullptr) {
            std::shared_ptr< isoft::nml::definition::DiagnosticGenericUDSServicePort > dataPortPtr{
                portTypePtr->diagnosticGenericUDSServicePort};
            if (genericDataIdentifierPtr_.get() == nullptr) {
                genericDataIdentifierPtr_                             = std::make_shared< GenericDataIdentifier >();
                diagnosticDataManagement_.didManager.dataInterfacePtr = genericDataIdentifierPtr_;
            }

            ara::core::String typeName{};
            typeName = "DIAGNOSTIC-GENERIC-UDS-INTERFACE";
            std::uint16_t const instance{Hash16(readDataByIdentifierPtr->isoftNmlObjectFqn)};
            std::uint32_t const serviceInstance{Hash32(dataPortPtr->isoftNmlObjectFqn)};
            genericDataIdentifierPtr_->RegisterServiceInstance(config.id, instance, serviceInstance, typeName);
        }
        diagnosticDataManagement_.service.serviceX22->table.emplace_back(config);
    }
    if (udsReadDataByIdentifierServicePtr->maxDidToRead.get() != nullptr) {
        diagnosticDataManagement_.service.serviceX22->maxDidToRead = *udsReadDataByIdentifierServicePtr->maxDidToRead;
    }
    return true;
}

bool UdsConfig::_initializeUdsWriteDataByIdentifierService(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::shared_ptr< isoft::nml::definition::UdsWriteDataByIdentifierService >
        udsWriteDataByIdentifierServicePtr) noexcept
{
    if (udsWriteDataByIdentifierServicePtr.get() == nullptr) {
        common::LogWarn() << "UdsConfig::_initializeUdsWriteDataByIdentifierService| not config x2E service!";
        return false;
    }
    diagnosticDataManagement_.service.serviceX2E = std::make_shared< isoft::uds::server::ServiceX2EModel >();
    for (auto &&writeDataByIdentifier : udsWriteDataByIdentifierServicePtr->writeDataByIdentifier) {
        if (writeDataByIdentifier.get() == nullptr) {
            continue;
        }
        std::string shortName{"unknow"};
        if (writeDataByIdentifier->shortName.get() != nullptr) {
            shortName = *writeDataByIdentifier->shortName;
        }
        isoft::uds::server::WriteDiagnosticDataByIdentifier config{};

        if (!writeDataByIdentifier->dataIdentifierRef.expired()) {
            std::shared_ptr< isoft::nml::definition::UdsDataIdentifier > udsDataIdentifierPtr{
                writeDataByIdentifier->dataIdentifierRef.lock()};
            if (udsDataIdentifierPtr->id.get() != nullptr) {
                config.id = *udsDataIdentifierPtr->id;
            } else {
                common::LogWarn() << "UdsConfig::_initializeUdsWriteDataByIdentifierService| subfunction:" << shortName
                                  << " not config did";
            }
            if (udsDataIdentifierPtr->didSize.get() != nullptr) {
                config.dataSize = *udsDataIdentifierPtr->didSize;
            } else {
                common::LogWarn() << "UdsConfig::_initializeUdsWriteDataByIdentifierService| subfunction:" << shortName
                                  << " not config did size value";
            }
        }
        std::shared_ptr< isoft::nml::definition::UdsPermissionGroup > udsPermissionGroup{
            writeDataByIdentifier->permissionGroupRef.lock()};
        _setConfigByUdsPermissionGroup(udsPermissionGroup, config.accessPermissionSession,
                                       config.accessPermissionSecurityLevel, config.accessPermissionEnvCondition,
                                       config.accessPermissionAuth);
        std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent::PortType > portTypePtr{};
        portTypePtr = _getPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticGenericUDSServicePort,
                               writeDataByIdentifier.get());
        if (portTypePtr.get() != nullptr && portTypePtr->diagnosticGenericUDSServicePort.get() != nullptr) {
            std::shared_ptr< isoft::nml::definition::DiagnosticGenericUDSServicePort > dataPortPtr{
                portTypePtr->diagnosticGenericUDSServicePort};
            if (genericDataIdentifierPtr_.get() == nullptr) {
                genericDataIdentifierPtr_                             = std::make_shared< GenericDataIdentifier >();
                diagnosticDataManagement_.didManager.dataInterfacePtr = genericDataIdentifierPtr_;
            }

            ara::core::String typeName{};
            typeName = "DIAGNOSTIC-GENERIC-UDS-INTERFACE";
            std::uint16_t const instance{Hash16(writeDataByIdentifier->isoftNmlObjectFqn)};
            std::uint32_t const serviceInstance{Hash32(dataPortPtr->isoftNmlObjectFqn)};
            genericDataIdentifierPtr_->RegisterServiceInstance(config.id, instance, serviceInstance, typeName);
        }
        diagnosticDataManagement_.service.serviceX2E->table.emplace_back(config);
    }
    return true;
}

bool UdsConfig::_initializeUdsReadDataByPeriodicIdentifierService(
    std::shared_ptr< isoft::nml::definition::UdsReadDataByPeriodicIdentifierService >
        udsReadDataByPeriodicIdentifierServicePtr) noexcept
{
    if (udsReadDataByPeriodicIdentifierServicePtr.get() == nullptr) {
        common::LogWarn() << "UdsConfig::_initializeUdsReadDataByPeriodicIdentifierService| not config x2A service!";
        return false;
    }
    diagnosticDataManagement_.service.serviceX2A = std::make_shared< isoft::uds::server::ServiceX2AModel >();
    for (auto &&periodicRatePtr : udsReadDataByPeriodicIdentifierServicePtr->periodicRate) {
        if (periodicRatePtr.get() == nullptr) {
            continue;
        }
        isoft::uds::server::DiagnosticPeriodicRate config{};

        if (periodicRatePtr->period.get() != nullptr) {
            config.timeout = SecondToMs(*periodicRatePtr->period);
        }
        if (periodicRatePtr->periodicRateCategory.get() != nullptr) {
            switch (*periodicRatePtr->periodicRateCategory) {
                case isoft::nml::definition::UdsPeriodicRateCategoryEnum::kPeriodicRateFast:
                    config.type = isoft::uds::server::DiagnosticPeriodicRateCategoryEnum::kPeriodicRateFast;
                    break;
                case isoft::nml::definition::UdsPeriodicRateCategoryEnum::kPeriodicRateMedium:
                    config.type = isoft::uds::server::DiagnosticPeriodicRateCategoryEnum::kPeriodicRateMedium;
                    break;
                case isoft::nml::definition::UdsPeriodicRateCategoryEnum::kPeriodicRateSlow:
                    config.type = isoft::uds::server::DiagnosticPeriodicRateCategoryEnum::kPeriodicRateSlow;
                    break;
                default:
                    break;
            }
        }
        diagnosticDataManagement_.service.serviceX2A->rates.emplace_back(config);
    }
    if (udsReadDataByPeriodicIdentifierServicePtr->maxPeriodicDidToRead.get() != nullptr) {
        diagnosticDataManagement_.service.serviceX2A->maxPeriodicDidToRead
            = *udsReadDataByPeriodicIdentifierServicePtr->maxPeriodicDidToRead;
    }
    if (udsReadDataByPeriodicIdentifierServicePtr->schedulerMaxNumber.get() != nullptr) {
        diagnosticDataManagement_.service.serviceX2A->schedulerMaxNumber
            = *udsReadDataByPeriodicIdentifierServicePtr->schedulerMaxNumber;
    }
    return true;
}

bool UdsConfig::_initializeUdsDynamicallyDefineDataIdentifierService(
    std::shared_ptr< isoft::nml::definition::UdsDynamicallyDefineDataIdentifierService >
        udsDynamicallyDefineDataIdentifierServicePtr) noexcept
{
    if (udsDynamicallyDefineDataIdentifierServicePtr.get() == nullptr) {
        common::LogWarn() << "UdsConfig::_initializeUdsDynamicallyDefineDataIdentifierService| not config x2C service!";
        return false;
    }
    diagnosticDataManagement_.service.serviceX2C = std::make_shared< isoft::uds::server::ServiceX2CModel >();
    for (auto &&didSettingPtr : udsDynamicallyDefineDataIdentifierServicePtr->didSetting) {
        if (didSettingPtr.get() == nullptr) {
            continue;
        }
        isoft::uds::server::DynamicDefinedDiagnosticData config{};

        if (!didSettingPtr->dataIdentifierRef.expired()) {
            std::shared_ptr< isoft::nml::definition::UdsDataIdentifier > udsDataIdentifierPtr{
                didSettingPtr->dataIdentifierRef.lock()};
            config.id = *udsDataIdentifierPtr->id;
        }
        if (didSettingPtr->maxSourceElement.get() != nullptr) {
            config.maxSourceElement = *didSettingPtr->maxSourceElement;
        }
        std::shared_ptr< isoft::nml::definition::UdsPermissionGroup > udsPermissionGroup{
            didSettingPtr->permissionGroupRef.lock()};
        std::int32_t envCondition{};
        _setConfigByUdsPermissionGroup(udsPermissionGroup, config.accessPermissionSession,
                                       config.accessPermissionSecurityLevel, envCondition, config.accessPermissionAuth);
        diagnosticDataManagement_.service.serviceX2C->table.emplace_back(config);
    }
    if (udsDynamicallyDefineDataIdentifierServicePtr->checkPerSourceId.get() != nullptr) {
        diagnosticDataManagement_.service.serviceX2C->checkPerSourceId
            = *udsDynamicallyDefineDataIdentifierServicePtr->checkPerSourceId;
        diagnosticDataManagement_.service.serviceX22->checkPerSourceId
            = *udsDynamicallyDefineDataIdentifierServicePtr->checkPerSourceId;
    }
    if (udsDynamicallyDefineDataIdentifierServicePtr->storeDid.get() != nullptr) {
        diagnosticDataManagement_.service.serviceX2C->stored = *udsDynamicallyDefineDataIdentifierServicePtr->storeDid;
    }
    for (auto &&subfunction : udsDynamicallyDefineDataIdentifierServicePtr->subfunction) {
        switch (subfunction) {
            case isoft::nml::definition::UdsDynamicallyDefineDataIdentifierSubfunctionEnum::
                kClearDynamicallyDefineDataIdentifier:
                diagnosticDataManagement_.service.serviceX2C->subfunction.emplace(
                    isoft::uds::server::DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::
                        kClearDynamicallyDefineDataIdentifier);
                break;
            case isoft::nml::definition::UdsDynamicallyDefineDataIdentifierSubfunctionEnum::kDefineByIdentifier:
                diagnosticDataManagement_.service.serviceX2C->subfunction.emplace(
                    isoft::uds::server::DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kDefineByIdentifier);
                break;
            default:
                break;
        }
    }
    return true;
}

bool UdsConfig::_initializeUdsDataManagement(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::shared_ptr< isoft::nml::definition::UdsDataManagement > udsDataManagementPtr) noexcept
{
    if (udsDataManagementPtr.get() == nullptr) {
        common::LogWarn() << "UdsConfig::_initializeUdsDataManagement| not config data manager !";
        return false;
    }
    if (!_initializeUdsDataIdentifier(adaptiveSoftwareComponentTable, udsDataManagementPtr->dataIdentifier)) {
        common::LogWarn() << "UdsConfig::_initializeUdsDataManagement| _initializeUdsDataIdentifier error !";
    }
    if (!_initializeUdsReadDataByIdentifierService(adaptiveSoftwareComponentTable, udsDataManagementPtr->service0x22)) {
        common::LogWarn() << "UdsConfig::_initializeUdsDataManagement| service0x22 error !";
    }
    if (!_initializeUdsWriteDataByIdentifierService(adaptiveSoftwareComponentTable,
                                                    udsDataManagementPtr->service0x2e)) {
        common::LogWarn() << "UdsConfig::_initializeUdsDataManagement| service0x2e error !";
    }
    if (!_initializeUdsReadDataByPeriodicIdentifierService(udsDataManagementPtr->service0x2a)) {
        common::LogWarn() << "UdsConfig::_initializeUdsDataManagement| service0x2a error !";
    }
    if (!_initializeUdsDynamicallyDefineDataIdentifierService(udsDataManagementPtr->service0x2c)) {
        common::LogWarn() << "UdsConfig::_initializeUdsDataManagement| service0x2c error !";
    }
    return true;
}

bool UdsConfig::_initializeUdsRoutineControlService(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::shared_ptr< isoft::nml::definition::UdsRoutineControlService > udsRoutineControlServicePtr) noexcept
{
    if (udsRoutineControlServicePtr.get() == nullptr) {
        common::LogWarn() << "UdsConfig::_initializeUdsRoutineControlService| not config x31 service!";
        return false;
    }
    std::shared_ptr< GenericRoutine > genericRoutinePtr{nullptr};
    for (auto &&routinePtr : udsRoutineControlServicePtr->routine) {
        if (routinePtr.get() == nullptr) {
            continue;
        }
        isoft::uds::server::RoutineControlInstanceConfig config{};
        if (routinePtr->id.get() != nullptr) {
            config.routine.id = *routinePtr->id;
        }
        if (routinePtr->routineInfo.get() != nullptr) {
            config.routine.routineInfo = *routinePtr->routineInfo;
        }
        if (routinePtr->start.get() != nullptr && !routinePtr->start->permissionGroupRef.expired()) {
            std::shared_ptr< isoft::nml::definition::UdsPermissionGroup > udsPermissionGroupPtr{
                routinePtr->start->permissionGroupRef.lock()};
            _setConfigByUdsPermissionGroup(
                udsPermissionGroupPtr, config.sessionPermission, config.securityLevelPermission,
                config.routine.startAccessPermissionEnvCondition, config.accessPermissionAuth);
        }
        if (routinePtr->stop.get() != nullptr && !routinePtr->stop->permissionGroupRef.expired()) {
            std::shared_ptr< isoft::nml::definition::UdsPermissionGroup > udsPermissionGroupPtr{
                routinePtr->stop->permissionGroupRef.lock()};
            _setConfigByUdsPermissionGroup(
                udsPermissionGroupPtr, config.sessionPermission, config.securityLevelPermission,
                config.routine.stopAccessPermissionEnvCondition, config.accessPermissionAuth);
        }
        if (routinePtr->requestResult.get() != nullptr && !routinePtr->requestResult->permissionGroupRef.expired()) {
            std::shared_ptr< isoft::nml::definition::UdsPermissionGroup > udsPermissionGroupPtr{
                routinePtr->requestResult->permissionGroupRef.lock()};
            _setConfigByUdsPermissionGroup(
                udsPermissionGroupPtr, config.sessionPermission, config.securityLevelPermission,
                config.routine.getResultAccessPermissionEnvCondition, config.accessPermissionAuth);
        }
        std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent::PortType > portTypePtr{};
        portTypePtr
            = _getPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticRoutinePort, routinePtr.get());
        if (portTypePtr.get() != nullptr && portTypePtr->diagnosticRoutinePort.get() != nullptr) {
            std::shared_ptr< isoft::nml::definition::DiagnosticRoutinePort > dataPortPtr{
                portTypePtr->diagnosticRoutinePort};
            if (genericRoutinePtr.get() == nullptr) {
                genericRoutinePtr = std::make_shared< GenericRoutine >();
            }
            ara::core::String typeName{};

            if (*dataPortPtr->interfaceType == isoft::nml::definition::DiagnosticRoutineInterfaceEnum::kGeneric) {
                typeName = "DIAGNOSTIC-ROUTINE-GENERIC-INTERFACE";
            } else if (*dataPortPtr->interfaceType == isoft::nml::definition::DiagnosticRoutineInterfaceEnum::kTyped) {
                typeName = "DIAGNOSTIC-ROUTINE-INTERFACE";
            }
            std::uint16_t const instance{Hash16(routinePtr->isoftNmlObjectFqn)};
            std::uint32_t const serviceInstance{Hash32(dataPortPtr->isoftNmlObjectFqn)};
            genericRoutinePtr->RegisterRoutineInstance(config.routine.id, instance, serviceInstance, typeName);
            routineManagement_.interfacePtr = genericRoutinePtr;
        }
        routineManagement_.routineInstanceTable.emplace(config);
    }
    return true;
}

bool UdsConfig::_initializeUdsEcuResetService(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::shared_ptr< isoft::nml::definition::UdsEcuResetService > udsEcuResetServicePtr) noexcept
{
    if (udsEcuResetServicePtr.get() == nullptr) {
        return true;
    }
    for (auto &&subfunction : udsEcuResetServicePtr->subfunction) {
        if (subfunction.get() != nullptr) {
            isoft::uds::server::EcuResetInstanceConfig instanceConfig{};
            if (subfunction->id.get() != nullptr) {
                std::string category{*subfunction->id};
                if (category == "HARD_RESET") {
                    instanceConfig.subfunctionNumber = static_cast< std::uint8_t >(isoft::uds::server::kHardReset);
                } else if (category == "KEY_OFF_ON_RESET") {
                    instanceConfig.subfunctionNumber = static_cast< std::uint8_t >(isoft::uds::server::kKeyOffOnReset);
                } else if (category == "SOFT_RESET") {
                    instanceConfig.subfunctionNumber = static_cast< std::uint8_t >(isoft::uds::server::kSoftReset);
                } else if (category == "ENABLE_RAPID_POWER_SHUT_DOWN") {
                    instanceConfig.subfunctionNumber
                        = static_cast< std::uint8_t >(isoft::uds::server::kEnableRapidPowerShutDown);
                } else if (category == "DISABLE_RAPID_POWER_SHUT_DOWN") {
                    instanceConfig.subfunctionNumber
                        = static_cast< std::uint8_t >(isoft::uds::server::kDisableRapidPowerShutDown);
                } else {
                    instanceConfig.subfunctionNumber = std::stoul(category);
                }
            }
            std::int32_t accessPermissionEnvCondition{};
            _setConfigByUdsPermissionGroup(subfunction->permissionGroupRef.lock(),
                                           instanceConfig.accessPermissionSession,
                                           instanceConfig.accessPermissionSecurityLevel, accessPermissionEnvCondition,
                                           instanceConfig.accessPermissionAuth);

            if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticEcuResetRequestPort,
                         subfunction.get())) {
                std::vector< std::string > serviceTable
                    = _getPortFqn(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticEcuResetRequestPort,
                                  subfunction.get());
                if (!serviceTable.empty() && ecuResetManagement_.interfacePtr.get() == nullptr) {
                    std::uint16_t const instance{Hash16(udsEcuResetServicePtr->isoftNmlObjectFqn)};
                    std::uint32_t const serviceInstance{Hash32(serviceTable.front())};
                    std::shared_ptr< EcuResetRequest > ecuResetServicePtr{
                        std::make_shared< EcuResetRequest >(instance, serviceInstance)};
                    ecuResetManagement_.interfacePtr = ecuResetServicePtr;
                }
                ecuResetManagement_.instanceTable.emplace(instanceConfig);
            }
        }
    }
    if (udsEcuResetServicePtr->respondToReset.get() != nullptr) {
        if (*udsEcuResetServicePtr->respondToReset
            == isoft::nml::definition::UdsResponseToEcuResetEnum::kRespondAfterReset) {
            ecuResetManagement_.respondToReset
                = isoft::uds::server::DiagnosticResponseToEcuResetEnum::kRespondAfterReset;
        } else if (*udsEcuResetServicePtr->respondToReset
                   == isoft::nml::definition::UdsResponseToEcuResetEnum::kRespondBeforeReset) {
            ecuResetManagement_.respondToReset
                = isoft::uds::server::DiagnosticResponseToEcuResetEnum::kRespondBeforeReset;
        }
    } else {
        ecuResetManagement_.respondToReset = isoft::uds::server::DiagnosticResponseToEcuResetEnum::kRespondBeforeReset;
    }

    return true;
}

bool UdsConfig::_initializeUdsCommunicationControlService(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::shared_ptr< isoft::nml::definition::UdsCommunicationControlService >
        udsCommunicationControlServicePtr) noexcept
{
    if (udsCommunicationControlServicePtr.get() == nullptr) {
        return true;
    }
    for (auto &&subfunction : udsCommunicationControlServicePtr->subfunction) {
        if (subfunction.get() != nullptr) {
            isoft::uds::server::ComControlInstanceConfig instanceConfig{};
            if (subfunction->id.get() != nullptr) {
                std::string category{*subfunction->id};
                if (category == "ENABLE_RX_AND_TX") {
                    instanceConfig.subfunctionNumber = isoft::uds::server::kEnableRxAndTx;
                } else if (category == "ENABLE_RX_AND_DISABLE_TX") {
                    instanceConfig.subfunctionNumber = isoft::uds::server::kEnableRxAndDisableTx;
                } else if (category == "DISABLE_RX_AND_ENABLE_TX") {
                    instanceConfig.subfunctionNumber = isoft::uds::server::kDisableRxAndEnableTx;
                } else if (category == "DISABLE_RX_AND_TX") {
                    instanceConfig.subfunctionNumber = isoft::uds::server::kDisableRxAndTx;
                } else if (category == "ENABLE_RX_AND_DISABLE_TX_WITH_ENHANCED_ADDRESS_"
                               "INFORMATION") {
                    instanceConfig.subfunctionNumber
                        = isoft::uds::server::kEnableRxAndDisableTxWithEnhancedAddressInformation;
                } else if (category == "ENABLE_RX_AND_TX_WITH_ENHANCED_ADDRESS_INFORMATION") {
                    instanceConfig.subfunctionNumber = isoft::uds::server::kEnableRxAndTxWithEnhancedAddressInformation;
                } else {
                    instanceConfig.subfunctionNumber = std::stoul(category);
                }
            }
            std::int32_t accessPermissionEnvCondition{};
            _setConfigByUdsPermissionGroup(subfunction->permissionGroupRef.lock(),
                                           instanceConfig.accessPermissionSession,
                                           instanceConfig.accessPermissionSecurityLevel, accessPermissionEnvCondition,
                                           instanceConfig.accessPermissionAuth);
            std::ignore = comManagementConfig_.instanceTable.emplace(instanceConfig);
            if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticCommunicationControlPort,
                         subfunction.get())) {
                std::vector< std::string > serviceTable
                    = _getPortFqn(adaptiveSoftwareComponentTable,
                                  isoft::nml::ModelTag::kDiagnosticCommunicationControlPort, subfunction.get());
                if (!serviceTable.empty()) {
                    std::uint16_t const instance{Hash16(subfunction->isoftNmlObjectFqn)};
                    std::uint32_t const serviceInstance{Hash32(serviceTable.front())};
                    std::shared_ptr< CommunicationControl > communicationControlPtr{
                        std::make_shared< CommunicationControl >(instance, serviceInstance)};
                    comManagementConfig_.interface = communicationControlPtr;
                }
            }
        }
    }

    return true;
}

bool UdsConfig::_initializeUdsResponseOnEventService(
    std::shared_ptr< isoft::nml::definition::UdsResponseOnEventService > udsResponseOnEventServicePtr) noexcept
{
    if (udsResponseOnEventServicePtr.get() == nullptr) {
        return true;
    }
    for (auto &&subfunction : udsResponseOnEventServicePtr->subfunction) {
        if (subfunction.get() != nullptr) {
            isoft::uds::server::ResponseOnEventInstanceConfig instance;
            if (subfunction->responseOnEventAction.get() != nullptr) {
                switch (*subfunction->responseOnEventAction) {
                    case isoft::nml::definition::UdsResponseOnEventActionEnum::kStart:
                        instance.responseOnEventAction
                            = isoft::uds::server::ResponseOnEventActionEnum::kStartResponseOnEvent;
                        break;
                    case isoft::nml::definition::UdsResponseOnEventActionEnum::kStop:
                        instance.responseOnEventAction
                            = isoft::uds::server::ResponseOnEventActionEnum::kStopResponseOnEvent;
                        break;
                    case isoft::nml::definition::UdsResponseOnEventActionEnum::kReport:
                        instance.responseOnEventAction
                            = isoft::uds::server::ResponseOnEventActionEnum::kReportActivatedEvents;
                        break;
                    case isoft::nml::definition::UdsResponseOnEventActionEnum::kOnDtcStatusChange:
                        instance.responseOnEventAction
                            = isoft::uds::server::ResponseOnEventActionEnum::kOnDTCStatusChange;
                        break;
                    case isoft::nml::definition::UdsResponseOnEventActionEnum::kOnChangeOfDataIdentifier:
                        instance.responseOnEventAction
                            = isoft::uds::server::ResponseOnEventActionEnum::kOnChangeOfDataIdentifier;
                        break;
                    case isoft::nml::definition::UdsResponseOnEventActionEnum::kOnComparisonOfValues:
                        instance.responseOnEventAction
                            = isoft::uds::server::ResponseOnEventActionEnum::kOnComparisonOfValues;
                        break;
                    case isoft::nml::definition::UdsResponseOnEventActionEnum::kClear:
                        instance.responseOnEventAction
                            = isoft::uds::server::ResponseOnEventActionEnum::kClearResponseOnEvent;
                        break;
                    default:
                        break;
                }
            }
            std::int32_t accessPermissionEnvCondition{};
            _setConfigByUdsPermissionGroup(subfunction->permissionGroupRef.lock(), instance.accessPermissionSession,
                                           instance.accessPermissionSecurityLevel, accessPermissionEnvCondition,
                                           instance.accessPermissionAuth);
            roeManagement_.instanceTable.emplace(instance);
        }
    }
    if (udsResponseOnEventServicePtr->maxDTCStatusChanges.get() != nullptr) {
        roeManagement_.maxNumberOfStoredDTCStatusChangedEvents = *udsResponseOnEventServicePtr->maxDTCStatusChanges;
    }
    if (udsResponseOnEventServicePtr->maxNumChangeOfDid.get() != nullptr) {
        roeManagement_.maxNumChangeOfDataIdentfierEvents = *udsResponseOnEventServicePtr->maxNumChangeOfDid;
    }
    if (udsResponseOnEventServicePtr->maxNumComparisionOfValue.get() != nullptr) {
        roeManagement_.maxNumComparisionOfValueEvents = *udsResponseOnEventServicePtr->maxNumComparisionOfValue;
    }
    if (udsResponseOnEventServicePtr->maxSupportedDIDLength.get() != nullptr) {
        roeManagement_.maxSupportedDIDLength = *udsResponseOnEventServicePtr->maxSupportedDIDLength;
    }
    if (udsResponseOnEventServicePtr->resOnEventSchedulerRate.get() != nullptr) {
        roeManagement_.responseOnEventSchedulerRate
            = SecondToMs(*udsResponseOnEventServicePtr->resOnEventSchedulerRate);
    }
    if (udsResponseOnEventServicePtr->storeEventEnabled.get() != nullptr) {
        roeManagement_.storeEventEnabled = *udsResponseOnEventServicePtr->storeEventEnabled;
    }

    return true;
}

bool UdsConfig::_initializeUdsCustomService(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::vector< std::shared_ptr< isoft::nml::definition::UdsCustomService > > udsCustomServiceTable) noexcept
{
    for (auto &&udsCustomServicePtr : udsCustomServiceTable) {
        if (udsCustomServicePtr.get() == nullptr) {
            continue;
        }
        std::shared_ptr< isoft::uds::server::CustomInstance > customInstancePtr{
            std::make_shared< isoft::uds::server::CustomInstance >()};
        if (udsCustomServicePtr->sid.get() != nullptr) {
            customInstancePtr->nSid = *udsCustomServicePtr->sid;
        }
        int32_t accessPermissionEnvCondition{};
        _setConfigByUdsPermissionGroup(udsCustomServicePtr->permissionGroupRef.lock(),
                                       customInstancePtr->accessPermissionSession,
                                       customInstancePtr->accessPermissionSecurityLevel, accessPermissionEnvCondition,
                                       customInstancePtr->accessPermissionAuth);
        if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticGenericUDSServicePort,
                     udsCustomServicePtr.get())) {
            std::vector< std::string > serviceTable
                = _getPortFqn(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticGenericUDSServicePort,
                              udsCustomServicePtr.get());
            if (!serviceTable.empty()) {
                if (genericUDSServicePtr_.get() == nullptr) {
                    genericUDSServicePtr_ = std::make_shared< GenericUDSService >();
                }
                std::uint16_t const instance{Hash16(udsCustomServicePtr->isoftNmlObjectFqn)};
                std::uint32_t const serviceInstance{Hash32(serviceTable.front())};
                genericUDSServicePtr_->RegisterUdsServiceInstance(customInstancePtr->nSid, instance, serviceInstance);
                customInstancePtr->interfacePtr = genericUDSServicePtr_;
            }
        }
        customManagement_.table.emplace(customInstancePtr);
    }

    return true;
}
bool UdsConfig::_initializeUdsDownloadService(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::shared_ptr< isoft::nml::definition::UdsDownloadService > udsDownloadServicePtr) noexcept
{
    if (udsDownloadServicePtr.get() == nullptr) {
        return false;
    }
    std::shared_ptr< isoft::uds::server::RequestDownload > requestDownloadPtr{
        std::make_shared< isoft::uds::server::RequestDownload >()};
    if (udsDownloadServicePtr->maxNumberOfBlockLength.get() != nullptr) {
        requestDownloadPtr->maxNumberOfBlockLength = *udsDownloadServicePtr->maxNumberOfBlockLength;
    }
    _setConfigByUdsPermissionGroup(
        udsDownloadServicePtr->permissionGroupRef.lock(), requestDownloadPtr->accessPermissionSession,
        requestDownloadPtr->accessPermissionSecurityLevel, requestDownloadPtr->accessPermissionEnvCondition,
        requestDownloadPtr->accessPermissionAuth);
    if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticDownloadServicePort,
                 udsDownloadServicePtr.get())) {
        std::vector< std::string > serviceTable
            = _getPortFqn(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticDownloadServicePort,
                          udsDownloadServicePtr.get());
        if (!serviceTable.empty()) {
            std::uint16_t const instance{Hash16(udsDownloadServicePtr->isoftNmlObjectFqn)};
            std::uint32_t const serviceInstance{Hash32(serviceTable.front())};
            std::shared_ptr< Download > downloadPtr{std::make_shared< Download >(instance, serviceInstance)};

            requestDownloadPtr->interfacePtr = downloadPtr;
        }
    }
    transferManagment_.requestDownload = requestDownloadPtr;
    return true;
}

bool UdsConfig::_initializeUdsUploadService(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::shared_ptr< isoft::nml::definition::UdsUploadService > udsUploadServicePtr) noexcept
{
    if (udsUploadServicePtr.get() == nullptr) {
        return false;
    }
    std::shared_ptr< isoft::uds::server::RequestUpload > requestUploadPtr{
        std::make_shared< isoft::uds::server::RequestUpload >()};
    if (udsUploadServicePtr->maxNumberOfBlockLength.get() != nullptr) {
        requestUploadPtr->maxNumberOfBlockLength = *udsUploadServicePtr->maxNumberOfBlockLength;
    }
    _setConfigByUdsPermissionGroup(
        udsUploadServicePtr->permissionGroupRef.lock(), requestUploadPtr->accessPermissionSession,
        requestUploadPtr->accessPermissionSecurityLevel, requestUploadPtr->accessPermissionEnvCondition,
        requestUploadPtr->accessPermissionAuth);
    if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticUploadServicePort,
                 udsUploadServicePtr.get())) {
        std::vector< std::string > serviceTable
            = _getPortFqn(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticUploadServicePort,
                          udsUploadServicePtr.get());
        if (!serviceTable.empty()) {
            std::uint16_t const instance{Hash16(udsUploadServicePtr->isoftNmlObjectFqn)};
            std::uint32_t const serviceInstance{Hash32(serviceTable.front())};
            std::shared_ptr< Upload > uploadPtr{std::make_shared< Upload >(instance, serviceInstance)};

            requestUploadPtr->interfacePtr = uploadPtr;
        }
    }
    transferManagment_.requestUpload = requestUploadPtr;
    return true;
}

bool UdsConfig::_initializeUdsFileTransferService(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::shared_ptr< isoft::nml::definition::UdsFileTransferService > udsFileTransferServicePtr) noexcept
{
    if (udsFileTransferServicePtr.get() == nullptr) {
        return false;
    }
    std::shared_ptr< isoft::uds::server::RequestFileTransfer > fileTransferPtr{
        std::make_shared< isoft::uds::server::RequestFileTransfer >()};
    if (udsFileTransferServicePtr->maxNumberOfBlockLength.get() != nullptr) {
        fileTransferPtr->maxNumberOfBlockLength = *udsFileTransferServicePtr->maxNumberOfBlockLength;
    }
    _setConfigByUdsPermissionGroup(
        udsFileTransferServicePtr->permissionGroupRef.lock(), fileTransferPtr->accessPermissionSession,
        fileTransferPtr->accessPermissionSecurityLevel, fileTransferPtr->accessPermissionEnvCondition,
        fileTransferPtr->accessPermissionAuth);
    if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticFileTransferServicePort,
                 udsFileTransferServicePtr.get())) {
        std::vector< std::string > serviceTable
            = _getPortFqn(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticFileTransferServicePort,
                          udsFileTransferServicePtr.get());
        if (!serviceTable.empty()) {
            std::uint16_t const instance{Hash16(udsFileTransferServicePtr->isoftNmlObjectFqn)};
            std::uint32_t const serviceInstance{Hash32(serviceTable.front())};
            std::shared_ptr< FileTransfer > interfacePtr{std::make_shared< FileTransfer >(instance, serviceInstance)};

            fileTransferPtr->interfacePtr = interfacePtr;
        }
    }
    transferManagment_.requestFileTransfer = fileTransferPtr;
    return true;
}
bool UdsConfig::_initializeUdsTransferManagement(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::shared_ptr< isoft::nml::definition::UdsTransferManagement > udsTransferManagementPtr) noexcept
{
    if (udsTransferManagementPtr.get() == nullptr) {
        return false;
    }
    if (!_initializeUdsDownloadService(adaptiveSoftwareComponentTable, udsTransferManagementPtr->downloadService)) {
        common::LogInfo() << "UdsConfig::_initializeUdsTransferManagement| no config download service!";
    }
    if (!_initializeUdsUploadService(adaptiveSoftwareComponentTable, udsTransferManagementPtr->uploadService)) {
        common::LogInfo() << "UdsConfig::_initializeUdsTransferManagement| no config upload service!";
    }
    if (!_initializeUdsFileTransferService(adaptiveSoftwareComponentTable, udsTransferManagementPtr->fileTransfer)) {
        common::LogInfo() << "UdsConfig::_initializeUdsTransferManagement| no config file transfer service!";
    }
    return true;
}

bool UdsConfig::_initializeUdsAuthenticationService(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::shared_ptr< isoft::nml::definition::UdsAuthenticationService > udsAuthenticationServicePtr) noexcept
{
    if (udsAuthenticationServicePtr.get() == nullptr) {
        return false;
    }
    for (auto &&evaluationId : udsAuthenticationServicePtr->evaluationId) {
        isoft::uds::server::AuthTransmitCertificateEvaluation authTransmitCertificateEvaluation{};
        authTransmitCertificateEvaluation.certificateEvaluationId = evaluationId;
        authenticationManagement_.transmitCertificate.certificateEvaluation.push_back(
            authTransmitCertificateEvaluation);
    }

    if (_hasPort(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticAuthenticationPort,
                 udsAuthenticationServicePtr.get())) {
        std::vector< std::string > serviceTable
            = _getPortFqn(adaptiveSoftwareComponentTable, isoft::nml::ModelTag::kDiagnosticAuthenticationPort,
                          udsAuthenticationServicePtr.get());
        if (!serviceTable.empty()) {
            std::uint16_t const instance{Hash16(udsAuthenticationServicePtr->isoftNmlObjectFqn)};
            std::uint32_t const serviceInstance{Hash32(serviceTable.front())};
            common::LogInfo() << "authentication service| client instance:" << instance
                              << "service instance:" << serviceInstance;
            std::shared_ptr< Authentication > interfacePtr{
                std::make_shared< Authentication >(instance, serviceInstance)};
            authenticationManagement_.interface = interfacePtr;
        }
    }
    return true;
}
#endif

bool UdsConfig::_initializeFaultConfig(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc)
{
    if (dmc.get() == nullptr) {
        LogError() << "UdsConfig::_initializeFaultConfig|dmc is nullptr";
        return false;
    }

    if (!_initializeCommonProperties(dmc)) {
        return false;
    }

    if (!_initializeConfiguredDids(dmc)) {
        return false;
    }

    if (!_initializeDTCGroups(dmc)) {
        return false;
    }

    if (!_initializeConditionGroups(dmc)) {
        return false;
    }

    if (!_initializeOperationCycles(dmc)) {
        return false;
    }

    if (!_initializeMapEvents(dmc)) {
        return false;
    }

    return true;
}

/// @brief
/// @param dmc
/// @return
/// @throw
bool UdsConfig::_initializeCommonProperties(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc)
{
    ara::core::String machineName;
    if (dmc->Load(std::move(ara::core::StringView("machine.shortName")), machineName) != 0) {
        LogError() << "UdsConfig::_initializeCommonProperties|load machine.shortName fail";
        return false;
    }

    ara::core::String clusterName;
    if (dmc->Load(std::move(ara::core::StringView("softwareCluster.shortName")), clusterName) != 0) {
        LogError() << "UdsConfig::_initializeCommonProperties|load "
                      "softwareCluster.shortName fail";
        return false;
    }
    faultConfig_.clusterName = std::string(machineName.c_str()) + "_" + std::string(clusterName.c_str());

    /// Whether the aging cycle counter needs to be processed in each aging cycle
    /// true : Only the aging cycle with TestFailed will consider the processing of the aging cycle counter
    /// false : The aging cycle counter needs to be processed in each aging cycle
    if (0
        != dmc->Load(std::move(ara::core::StringView("commonProperties.agingRequiresTestedCycle")),
                     faultConfig_.commonProps.agingRequiresTestedCycle)) {
        LogWarn() << "UdsConfig::_initializeCommonProperties|no config"
                     "commonProperties.agingRequiresTestedCycle, it will use "
                     "default value[false]";
    }

    /// Define the conditions for clearing DTC through API, 0: Support all DTC values (including DTC, DTC group)
    /// 1: Only support clearing groups and the group must be the default DTC group (i.e., all DTC groups)
    tps::DiagnosticClearDtcLimitationEnum clearDtcLimitation;
    if (0 != dmc->Load(std::move(ara::core::StringView("commonProperties.clearDtcLimitation")), clearDtcLimitation)) {
        LogWarn() << "UdsConfig::_initializeCommonProperties|no config "
                     "commonProperties.clearDtcLimitation, "
                     "it will use default vaule[kAllSupportedDtcs]";
    } else {
        faultConfig_.commonProps.clearDtcLimitation
            = static_cast< fault::DiagnosticClearDtcLimitationEnum >(clearDtcLimitation);
    }

    /// Type of counter trigger counting when a diagnostic event occurs
    tps::DiagnosticOccurrenceCounterProcessingEnum occurrenceCounterProcessing;
    if (0
        != dmc->Load(std::move(ara::core::StringView("commonProperties.occurrenceCounterProcessing")),
                     occurrenceCounterProcessing)) {
        LogWarn() << "UdsConfig::_initializeCommonProperties|no config "
                     "commonProperties.occurrenceCounterProcessing,it "
                     "will use"
                     "default vaule[kTestFailedBit]";
    } else {
        faultConfig_.commonProps.occurrenceCounterProcessing
            = static_cast< fault::DiagnosticOccurrenceCounterProcessingEnum >(occurrenceCounterProcessing);
    }

    /// This attribute defines whether to persistently store the "TestFailed" status bit
    /// true : Store
    /// false : Do not store
    if (0
        != dmc->Load(std::move(ara::core::StringView("commonProperties.statusBitStorageTestFailed")),
                     faultConfig_.commonProps.statusBitStorageTestFailed)) {
        LogWarn() << "UdsConfig::_initializeCommonProperties|no config "
                     "commonProperties.statusBitStorageTestFailed,it "
                     "will use "
                     "default vaule[false]";
    }

    /// After an Event Memory Entry is displaced, this attribute defines whether to reset the "Confirm" status bit
    if (0
        != dmc->Load(std::move(ara::core::StringView("commonProperties.resetConfirmedBitOnOverflow")),
                     faultConfig_.commonProps.resetConfirmedBitOnOverflow)) {
        LogWarn() << "UdsConfig::_initializeCommonProperties|no config "
                     "commonProperties.resetConfirmedBitOnOverflow,it "
                     "will use "
                     "default vaule[false]";
    }

    /// Whether aging and displacement mechanisms should be applied to the "TestFailedSinceLastClear" status bit.
    tps::DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum statusBitHandlingTestFailedSinceLastClear;
    if (0
        != dmc->Load(std::move(ara::core::StringView("commonProperties.statusBitHandlingTestFailedSinceLastClear")),
                     statusBitHandlingTestFailedSinceLastClear)) {
        LogWarn() << "UdsConfig::_initializeCommonProperties|not find "
                     "commonProperties.statusBitHandlingTestFailedSinceLastClear, "
                     "it will use "
                     "default value[kStatusBitNormal]";
    } else {
        faultConfig_.commonProps.statusBitHandlingTestFailedSinceLastClear
            = static_cast< fault::DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum >(
                statusBitHandlingTestFailedSinceLastClear);
    }
    return true;
}

/// @brief
/// @param dmc
/// @return
/// @throw
bool UdsConfig::_initializeConfiguredDids(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc)
{
    bool ok{true};
    /// Freeze frame & EDR
    int32_t ret = dmc->IterateArray(
        std::move(ara::core::StringView("freezeFrame")),
        [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            uint32_t id{UINT32_MAX};
            if (0 != node.Load(std::move(ara::core::StringView("recordNumber")), id)) {
                LogError() << "UdsConfig::_initializeConfiguredDids|load freezeFrame "
                              "recordNumber fail";
                ok = false;
                return;
            }
            if (id == UINT32_MAX) {
                LogError() << "UdsConfig::_initializeConfiguredDids|invalid freezeframe id";
                ok = false;
                return;
            }

            fault::FreezeFrameConfig ff;
            ff.recordNumber = id;

            if (0 != node.Load(std::move(ara::core::StringView("update")), ff.update)) {
                LogWarn() << "UdsConfig::_initializeConfiguredDids|no config "
                             "freezeFrame update, it will use default value";
            }

            tps::DiagnosticRecordTriggerEnum trigger;
            if (0 != node.Load(std::move(ara::core::StringView("trigger")), trigger)) {
                LogWarn() << "UdsConfig::_initializeConfiguredDids|no config "
                             "freezeFrame trigger,it will use default value";
            } else {
                ff.trigger = static_cast< fault::DiagnosticRecordTriggerEnum >(trigger);
            }

            ara::core::String customTrigger;
            if (0 != node.Load(std::move(ara::core::StringView("customTrigger")), customTrigger)) {
                LogDebug() << "UdsConfig::_initializeConfiguredDids|no config "
                              "freezeFrame customTrigger";
            } else {
                ff.customTrigger = std::string(customTrigger.c_str());
            }

            std::map< uint32_t, FreezeFrameConfig >::iterator iter = faultConfig_.freezeFrameConfig.find(id);
            if (iter == faultConfig_.freezeFrameConfig.end()) {
                faultConfig_.freezeFrameConfig[id] = ff;
            } else {
                LogError() << "UdsConfig::_initializeConfiguredDids|have same "
                              "freezeframe id ="
                           << id;
            }
        });
    if ((ret != 0) || !ok) {
        return false;
    }

    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > const extendedDataRecordCallback{
        [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            uint32_t id{UINT32_MAX};
            if (0 != node.Load(std::move(ara::core::StringView("recordNumber")), id)) {
                LogError() << "UdsConfig::_initializeConfiguredDids|load "
                              "extendedDataRecord recordNumber fail";
                ok = false;
                return;
            }
            if (id == UINT32_MAX) {
                LogError() << "UdsConfig::_initializeConfiguredDids|invalid edr recordNumber";
                ok = false;
                return;
            }

            fault::ExtendedDataRecordConfig edr;
            edr.recordNumber = id;

            if (0 != node.Load(std::move(ara::core::StringView("update")), edr.update)) {
                LogWarn() << "UdsConfig::_initializeConfiguredDids|no config "
                             "extendedDataRecord update,it will use "
                             "default value";
            }

            tps::DiagnosticRecordTriggerEnum trigger;
            if (0 != node.Load(std::move(ara::core::StringView("trigger")), trigger)) {
                LogWarn() << "UdsConfig::_initializeConfiguredDids|no config "
                             "extendedDataRecord trigger,it will use "
                             "default value";
            } else {
                edr.trigger = static_cast< fault::DiagnosticRecordTriggerEnum >(trigger);
            }

            ara::core::String customTrigger;
            if (0 != node.Load(std::move(ara::core::StringView("customTrigger")), customTrigger)) {
                LogDebug() << "UdsConfig::_initializeConfiguredDids|no config "
                              "extendedDataRecord customTrigger";
            } else {
                edr.customTrigger = std::string(customTrigger.c_str());
            }

            std::int32_t const res{node.IterateArray(
                std::move(ara::core::StringView("recordElement")),
                [this, &ok, &edr](std::size_t, isoft::manifestreader::ManifestNode const &mfNode) {
                    fault::EdrDataElementConfig dataElement;

                    ara::core::String dataElementId;
                    if (0 != mfNode.Load(std::move(ara::core::StringView("dataElement._path")), dataElementId)) {
                        LogError() << "UdsConfig::_initializeConfiguredDids|invalid "
                                      "dataElement id";
                        ok = false;
                        return;
                    }
                    dataElement.dataElementId = GetDiagnosticDataElementId(dataElementId);

                    ara::core::String value;
                    if (mfNode.Load(std::move(ara::core::StringView("dataProvider")), value) == 0) {
                        if (value == "DEM_AGINGCTR_DOWNCNT") {
                            dataElement.category = fault::EdrDataElementCategory::kDemAgingctrDowncnt;
                        } else if (value == "DEM_AGINGCTR_UPCNT") {
                            dataElement.category = fault::EdrDataElementCategory::kDemAgingctrUpcnt;
                        } else if (value == "DEM_CURRENT_FDC") {
                            dataElement.category = fault::EdrDataElementCategory::kDemCurrentFdc;
                        } else if (value == "DEM_CYCLES_SINCE_FIRST_FAILED") {
                            dataElement.category = fault::EdrDataElementCategory::kDemCyclesSinceFirstFailed;
                        } else if (value == "DEM_CYCLES_SINCE_LAST_FAILED") {
                            dataElement.category = fault::EdrDataElementCategory::kDemCyclesSinceLastFailed;
                        } else if (value == "DEM_FAILED_CYCLES") {
                            dataElement.category = fault::EdrDataElementCategory::kDemFailedCycles;
                        } else if (value == "DEM_MAX_FDC_DURING_CURRENT_CYCLE") {
                            dataElement.category = fault::EdrDataElementCategory::kDemMaxFdcDuringCurrentCycle;
                        } else if (value == "DEM_MAX_FDC_SINCE_LAST_CLEAR") {
                            dataElement.category = fault::EdrDataElementCategory::kDemMaxFdcSinceLastClear;
                        } else if (value == "DEM_OCCCTR") {
                            dataElement.category = fault::EdrDataElementCategory::kDemOccctr;
                        } else if (value == "DEM_OVFLIND") {
                            dataElement.category = fault::EdrDataElementCategory::kDemOvflind;
                        } else if (value == "DEM_SIGNIFICANCE") {
                            dataElement.category = fault::EdrDataElementCategory::kDemSignificance;
                        } else if (value == "DEM_PRIORITY") {
                            dataElement.category = fault::EdrDataElementCategory::kDemPriority;
                        } else if (value == "DEM_EVENT_ASSOCIATED_IDENTIFICATION") {
                            dataElement.category = fault::EdrDataElementCategory::kDemEventAssociatedIdentification;
                        } else {
                            LogError() << "UdsConfig::_initializeConfiguredDids|invalid "
                                          "dataProvider ="
                                       << value.c_str();
                            ok = false;
                            return;
                        }
                    } else {
                        dataElement.category = fault::EdrDataElementCategory::kExternal;
                        std::uint16_t clientInstanceId{};
                        std::uint32_t serviceInstanceId{};
                        if (mfNode.Load(std::move(ara::core::StringView("clientInstanceId")), clientInstanceId)
                                != isoft::kSuccess
                            || mfNode.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceInstanceId)
                                   != isoft::kSuccess) {
                            common::LogWarn() << "UdsConfig::_initializeConfiguredDids| "
                                                 "check clientInstanceId,serviceInstanceId! ";
                        } else {
                            if (genericDataElementPtr_.get() == nullptr) {
                                genericDataElementPtr_ = std::make_shared< GenericDataElement >();
                            }
                            genericDataElementPtr_->RegisterServiceInstance(dataElement.dataElementId, clientInstanceId,
                                                                            serviceInstanceId);

                            isoft::uds::server::DiagnosticDataParamterModel elementModel;
                            elementModel.element.id = dataElement.dataElementId;
                            elementModel.element.provider
                                = isoft::uds::server::DiagnosticDataElementProvider::kExternal;
                            diagnosticDataManagement_.didManager.edrElement.emplace_back(elementModel);
                        }
                    }

                    edr.dataElementList.emplace_back(dataElement);
                })};
            if ((res != 0) || !ok) {
                LogError() << "UdsConfig::_initializeConfiguredDids|load recordElement fail";
                return;
            }

            std::map< uint32_t, ExtendedDataRecordConfig >::iterator iter
                = faultConfig_.extendedDataRecordConfig.find(id);
            if (iter == faultConfig_.extendedDataRecordConfig.end()) {
                faultConfig_.extendedDataRecordConfig[id] = edr;
            } else {
                LogError() << "UdsConfig::_initializeConfiguredDids|have same edr id =" << id;
            }
        }};
    ret = dmc->IterateArray(std::move(ara::core::StringView("extendedDataRecord")), extendedDataRecordCallback);
    if ((ret != 0) || !ok) {
        return false;
    }

    ret = dmc->IterateArray(std::move(ara::core::StringView("dataIdentifierSet")),
                            [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
                                uint32_t id{UINT32_MAX};
                                if (0 != node.Load(std::move(ara::core::StringView("id")), id)) {
                                    LogError() << "UdsConfig::_initializeConfiguredDids|load "
                                                  "dataIdentifierSet id fail";
                                    ok = false;
                                    return;
                                }
                                if (id == UINT32_MAX) {
                                    LogError() << "UdsConfig::_initializeConfiguredDids|invalid didSet id";
                                    ok = false;
                                    return;
                                }

                                ara::core::Vector< uint16_t > didIdVector;
                                if (0 != node.Load(std::move(ara::core::StringView("dataIdentifier")), didIdVector)) {
                                    LogError() << "UdsConfig::_initializeConfiguredDids|load "
                                                  "dataIdentifier fail";
                                    ok = false;
                                    return;
                                }

                                /// Freeze frame is configured but the data layout of the freeze frame is not configured
                                if (didIdVector.empty() && !faultConfig_.freezeFrameConfig.empty()) {
                                    LogError() << "UdsConfig::_initializeConfiguredDids|config "
                                                  "freezeFrame but no config data identifier set";
                                    ok = false;
                                    return;
                                }

                                std::map< uint32_t, std::set< uint16_t > >::iterator iter
                                    = faultConfig_.dataIdentifierSet.find(id);
                                if (iter == faultConfig_.dataIdentifierSet.end()) {
                                    std::set< uint16_t > didSet(didIdVector.begin(), didIdVector.end());
                                    faultConfig_.dataIdentifierSet[id] = didSet;
                                } else {
                                    LogError() << "UdsConfig::_initializeConfiguredDids|have same "
                                                  "freezeFrame id ="
                                               << id;
                                }
                            });
    if ((ret != 0) || !ok) {
        return false;
    }

    return true;
}

/// @brief
/// @param dmc
/// @return
/// @throw
bool UdsConfig::_initializeDTCGroups(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc)
{
    bool ok{true};
    /// DTC related
    int32_t ret
        = dmc->IterateArray(std::move(ara::core::StringView("aging")),
                            [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
                                uint32_t id{UINT32_MAX};
                                if (0 != node.Load(std::move(ara::core::StringView("id")), id)) {
                                    LogError() << "UdsConfig::_initializeDTCGroups|load aging id fail";
                                    ok = false;
                                    return;
                                }
                                if (id == UINT32_MAX) {
                                    LogError() << "UdsConfig::_initializeDTCGroups|invalid aging id";
                                    ok = false;
                                    return;
                                }

                                fault::AgingConfig aging;
                                aging.agingID = id;

                                if (0 != node.Load(std::move(ara::core::StringView("threshold")), aging.threshold)) {
                                    LogError() << "UdsConfig::_initializeDTCGroups|load aging threshold "
                                                  "fail id ="
                                               << id;
                                    ok = false;
                                    return;
                                }
                                if (0U == aging.threshold) {
                                    LogError() << "UdsConfig::_initializeDTCGroups|invalid aging "
                                                  "threshold value id ="
                                               << id;
                                    ok = false;
                                    return;
                                }

                                if (0 != node.Load(std::move(ara::core::StringView("agingCycle")), aging.agingCycle)) {
                                    LogError() << "UdsConfig::_initializeDTCGroups|load agingCycle fail id =" << id;
                                    ok = false;
                                    return;
                                }

                                std::map< uint32_t, AgingConfig >::iterator iter = faultConfig_.agingConfig.find(id);
                                if (iter == faultConfig_.agingConfig.end()) {
                                    faultConfig_.agingConfig[id] = aging;
                                } else {
                                    LogError() << "UdsConfig::_initializeDTCGroups|have same agingCycle id =" << id;
                                }
                            });
    if ((ret != 0) || !ok) {
        return false;
    }

    /// DTC storage database
    ret = dmc->IterateArray(
        std::move(ara::core::StringView("memoryDestination")),
        [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            uint32_t id{UINT32_MAX};
            if (0 != node.Load(std::move(ara::core::StringView("memoryId")), id)) {
                LogError() << "UdsConfig::_initializeDTCGroups|load "
                              "memoryDestination memoryId fail";
                ok = false;
                return;
            }
            if (!((id == fault::kPrimaryDestinationId) || (id <= UINT8_MAX))) {
                LogError() << "UdsConfig::_initializeDTCGroups|invalid memoryDestination id=" << id;
                ok = false;
                return;
            }

            /// Only one primary memory can be included in a diagnostic extraction
            if (id == fault::kPrimaryDestinationId) {
                decltype(auto) iter = faultConfig_.memoryDestinationConfig.find(id);
                if (iter != faultConfig_.memoryDestinationConfig.end()) {
                    LogError() << "UdsConfig::_initializeDTCGroups|within the scope of "
                                  "one DiagnosticContributionSet only"
                                  "one primary dest shall exist, name ="
                               << faultConfig_.clusterName.c_str();
                    // todo This logic verification needs to be added eventually
                    // ok = false;
                    // return;
                }
            }

            fault::MemoryDestinationConfig dest;
            dest.destinationID = id;

            if (0
                != node.Load(std::move(ara::core::StringView("dtcStatusAvailabilityMask")),
                             dest.dtcStatusAvailabilityMask)) {
                LogError() << "UdsConfig::_initializeDTCGroups|load "
                              "memoryDestination dtcStatusAvailabilityMask fail id ="
                           << id;
                ok = false;
                return;
            }
            if (0
                != node.Load(std::move(ara::core::StringView("maxNumberOfEventEntries")),
                             dest.maxNumberOfEventEntries)) {
                LogError() << "UdsConfig::_initializeDTCGroups|load "
                              "memoryDestination maxNumberOfEventEntries fail id ="
                           << id;
                ok = false;
                return;
            }

            /// Displacement strategy, optional configuration
            tps::DiagnosticEventDisplacementStrategyEnum eventDisplacementStrategy;
            if (0
                != node.Load(std::move(ara::core::StringView("eventDisplacementStrategy")),
                             eventDisplacementStrategy)) {
                LogWarn() << "UdsConfig::_initializeDTCGroups|no config "
                             "memoryDestination eventDisplacementStrategy, "
                             "it not displacement id ="
                          << id;
            } else {
                dest.displacementStrategy
                    = static_cast< fault::DiagnosticEventDisplacementStrategyEnum >(eventDisplacementStrategy);
            }

            /// Conditions for triggering storage entities, optional configuration
            tps::DiagnosticMemoryEntryStorageTriggerEnum memoryEntryStorageTrigger;
            if (0
                != node.Load(std::move(ara::core::StringView("memoryEntryStorageTrigger")),
                             memoryEntryStorageTrigger)) {
                LogWarn() << "UdsConfig::_initializeDTCGroups|no config "
                             "memoryEntryStorageTrigger, it will use "
                             "default[kTestFailed] id ="
                          << id;
            } else {
                dest.memoryEntryStorageTrigger
                    = static_cast< fault::DiagnosticMemoryEntryStorageTriggerEnum >(memoryEntryStorageTrigger);
            }

            /// Strategy for assigning freeze frame numbers
            tps::DiagnosticTypeOfFreezeFrameRecordNumerationEnum typeOfFreezeFrameRecordNumeration;
            if (0
                != node.Load(std::move(ara::core::StringView("typeOfFreezeFrameRecordNumeration")),
                             typeOfFreezeFrameRecordNumeration)) {
                LogWarn() << "UdsConfig::_initializeDTCGroups|not config "
                             "typeOfFreezeFrameRecordNumeration it will "
                             "use default[kConfigured] id ="
                          << id;
            } else {
                dest.typeOfFreezeFrameRecordNumeration
                    = static_cast< fault::DiagnosticTypeOfFreezeFrameRecordNumerationEnum >(
                        typeOfFreezeFrameRecordNumeration);
            }

            std::uint32_t serviceId{};
            if (0 != node.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceId)) {
                LogWarn() << "UdsConfig::_initializeDTCGroups|no config "
                             "memoryDestination serviceInstanceId id ="
                          << id;
            } else {
                /// Create dtcinformation interface for communication with AA, optional configuration
                dest.dtcInformationInterface = std::make_shared< DtcInformation >(serviceId);
            }

            if (id == fault::kPrimaryDestinationId) {
                tps::DiagnosticTypeOfDtcSupportedEnum typeOfDtcSupported;
                if (0 != node.Load(std::move(ara::core::StringView("typeOfDtcSupported")), typeOfDtcSupported)) {
                    LogError() << "UdsConfig::_initializeDTCGroups|load primary "
                                  "typeOfDtcSupported fail";
                    ok = false;
                    return;
                }
                dest.typeOfDtcSupported = static_cast< fault::DiagnosticTypeOfDtcSupportedEnum >(typeOfDtcSupported);
            }

            std::map< uint32_t, MemoryDestinationConfig >::iterator iter
                = faultConfig_.memoryDestinationConfig.find(id);
            if (iter == faultConfig_.memoryDestinationConfig.end()) {
                faultConfig_.memoryDestinationConfig[id] = dest;
            } else {
                LogError() << "UdsConfig::_initializeDTCGroups|have same "
                              "memorydestination id ="
                           << id;
            }
        });
    if ((ret != 0) || !ok) {
        return false;
    }

    ret = dmc->IterateArray(
        std::move(ara::core::StringView("dtcProps")),
        [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            uint32_t id{UINT32_MAX};
            if (0 != node.Load(std::move(ara::core::StringView("id")), id)) {
                LogError() << "UdsConfig::_initializeDTCGroups|load dtcProps id fail";
                ok = false;
                return;
            }
            if (id == UINT32_MAX) {
                LogError() << "UdsConfig::_initializeDTCGroups|invalid dtc props id=" << id;
                ok = false;
                return;
            }

            fault::DtcPropsConfig props;
            props.dtcPropsID = id;

            ara::core::Vector< uint32_t > mds;
            if (0 != node.Load(std::move(ara::core::StringView("memoryDestination")), mds)) {
                LogError() << "UdsConfig::_initializeDTCGroups|1 no config "
                              "memoryDestination for dtc props_id ="
                           << id;
                ok = false;
                return;
            }

            /// [SWS_DM_CONSTR_00084] DTC must be configured with memory
            if (mds.empty()) {
                LogError() << "UdsConfig::_initializeDTCGroups|2 no config "
                              "memoryDestination for dtc props_id ="
                           << id;
                ok = false;
                return;
            }
            std::set< uint32_t > destSet(mds.begin(), mds.end());
            props.memoryDestination = destSet;

            // [SWS_DM_00237] Whether DTC has an aging mechanism, optional configuration
            if (0 != node.Load(std::move(ara::core::StringView("aging")), props.agingID)) {
                LogWarn() << "UdsConfig::_initializeDTCGroups|no config aging for "
                             "dtc props_id ="
                          << id;
            }

            if (0
                != node.Load(std::move(ara::core::StringView("immediateNvDataStorage")),
                             props.immediateNvDataStorage)) {
                LogWarn() << "UdsConfig::_initializeDTCGroups|no config "
                             "immediateNvDataStorage for dtc, it will use "
                             "default[false] props_id ="
                          << id;
            }

            /// EDR  optional configuration
            ara::core::Vector< uint32_t > edrs;
            if (0 != node.Load(std::move(ara::core::StringView("extendedDataRecord")), edrs)) {
                LogError() << "UdsConfig::_initializeDTCGroups|load "
                              "extendedDataRecord error for dtc props_id ="
                           << id;
                ok = false;
                return;
            }
            std::set< uint32_t > edrSet(edrs.begin(), edrs.end());
            props.extendedDataRecord = edrSet;
            if (props.extendedDataRecord.empty()) {
                LogWarn() << "UdsConfig::_initializeDTCGroups|no config "
                             "extendedDataRecord for dtc props_id ="
                          << id;
            }

            /// Freeze frame
            ara::core::Vector< uint32_t > ffs;
            if (0 != node.Load(std::move(ara::core::StringView("freezeFrame")), ffs)) {
                LogError() << "UdsConfig::_initializeDTCGroups|load freezeFrame "
                              "error for dtc props_id ="
                           << id;
                ok = false;
                return;
            }
            std::set< uint32_t > ffSet(ffs.begin(), ffs.end());
            props.freezeFrame = ffSet;
            if (props.freezeFrame.empty()) {
                LogWarn() << "UdsConfig::_initializeDTCGroups|no config freezeFrame "
                             "for dtc props_id ="
                          << id;
            }

            /// Maximum number of freeze frames
            if (0
                != node.Load(std::move(ara::core::StringView("maxNumberFreezeFrameRecords")),
                             props.maxNumberFreezeFrameRecords)) {
                LogWarn() << "UdsConfig::_initializeDTCGroups|no config "
                             "maxNumberFreezeFrameRecords for dtc "
                             "freezeframe props_id ="
                          << id;
            }

            /// Freeze frame data profile, optional configuration (but once freeze frame or the maximum number of generateable freeze frames is configured, this item must be configured)
            if (0
                != node.Load(std::move(ara::core::StringView("snapshotRecordContent")), props.snapshotRecordContent)) {
                if (!props.freezeFrame.empty() || props.maxNumberFreezeFrameRecords > 0U) {
                    LogError() << "UdsConfig::_initializeDTCGroups|no config "
                                  "snapshotRecordContent for dtc "
                                  "freezeframe props_id ="
                               << id;
                    ok = false;
                    return;
                }
                LogWarn() << "UdsConfig::_initializeDTCGroups|no config "
                             "snapshotRecordContent props_id ="
                          << id;
            }

            /// Must be configured
            if (0 != node.Load(std::move(ara::core::StringView("priority")), props.priority)) {
                LogError() << "UdsConfig::_initializeDTCGroups|no config priority "
                              "for dtc props_id ="
                           << id;
                ok = false;
                return;
            }

            /// Optional configuration
            tps::DiagnosticSignificanceEnum significance;
            if (0 != node.Load(std::move(ara::core::StringView("significance")), significance)) {
                LogWarn() << "UdsConfig::_initializeDTCGroups|no config significance "
                             "for dtc it will use "
                             "default[kFault] props_id ="
                          << id;
            } else {
                props.significance = static_cast< fault::DiagnosticSignificanceEnum >(significance);
            }

            std::map< uint32_t, DtcPropsConfig >::iterator iter = faultConfig_.dtcPropsConfig.find(id);
            if (iter == faultConfig_.dtcPropsConfig.end()) {
                faultConfig_.dtcPropsConfig[id] = props;
            } else {
                LogError() << "UdsConfig::_initializeDTCGroups|have same dtc props_id =" << id;
            }
        });
    if ((ret != 0) || !ok) {
        return false;
    }

    ret = dmc->IterateArray(
        std::move(ara::core::StringView("troubleCodeUds")),
        [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            uint32_t dtcId{UINT32_MAX};
            if (0 != node.Load(std::move(ara::core::StringView("id")), dtcId)) {
                LogError() << "UdsConfig::_initializeDTCGroups|load troubleCodeUds id fail";
                ok = false;
                return;
            }
            if (dtcId == UINT32_MAX) {
                LogError() << "UdsConfig::_initializeDTCGroups|invalid dtc id=" << dtcId;
                ok = false;
                return;
            }

            fault::DtcConfig dtc;
            tps::DiagnosticUdsSeverityEnum severity;
            if (0 != node.Load(std::move(ara::core::StringView("severity")), severity)) {
                LogError() << "UdsConfig::_initializeDTCGroups|load severity fail id =" << dtcId;
                ok = false;
                return;
            }
            dtc.severity = static_cast< fault::DiagnosticUdsSeverityEnum >(severity);

            if (0 != node.Load(std::move(ara::core::StringView("udsDtcValue")), dtc.dtcValue)) {
                LogError() << "UdsConfig::_initializeDTCGroups|load udsDtcValue fail dtcId =" << dtcId;
                ok = false;
                return;
            }

            if (0 != node.Load(std::move(ara::core::StringView("clearConditionGroup")), dtc.clearConditionGroupId)) {
                LogWarn() << "Config::_initializeDTCGroups|not config "
                             "ClearConditionGroup dtcId="
                          << dtcId;
            }

            if (0 != node.Load(std::move(ara::core::StringView("dtcProps")), dtc.dtcPropsId)) {
                LogError() << "Config::_initializeDTCGroups|load dtcProps fail dtcId=" << dtcId;
                ok = false;
                return;
            }

            std::map< uint32_t, DtcConfig >::iterator iter = faultConfig_.dtcConfig.find(dtcId);
            if (iter == faultConfig_.dtcConfig.end()) {
                faultConfig_.dtcConfig[dtcId] = dtc;
            } else {
                LogError() << "Config::_initializeDTCGroups|have same dtcId=" << dtcId;
            }
        });
    if ((ret != 0) || !ok) {
        return false;
    }

    ret = dmc->IterateArray(std::move(ara::core::StringView("diagnosticTroubleCodeGroup")),
                            [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
                                uint32_t groupId{UINT32_MAX};
                                if (0 != node.Load(std::move(ara::core::StringView("groupNumber")), groupId)) {
                                    LogError() << "Config::_initializeDTCGroups|load "
                                                  "diagnosticTroubleCodeGroup groupNumber fail";
                                    ok = false;
                                    return;
                                }
                                // [SWS_DM_CONSTR_00082].
                                if ((groupId == UINT32_MAX) || (groupId == fault::kGroupOfAllDTCs)) {
                                    LogError() << "Config::_initializeDTCGroups|invalid dtc group id=" << groupId;
                                    ok = false;
                                    return;
                                }

                                ara::core::Vector< uint32_t > dtcIds;
                                if (0 != node.Load(std::move(ara::core::StringView("troubleCodeUds")), dtcIds)) {
                                    LogError() << "Config::_initializeDTCGroups|load diagnosticTroubleCodeGroup "
                                                  "troubleCodeUds fail groupId ="
                                               << groupId;
                                    ok = false;
                                    return;
                                }

                                std::map< uint32_t, std::set< uint32_t > >::iterator iter
                                    = faultConfig_.dtcGroup.find(groupId);
                                if (iter == faultConfig_.dtcGroup.end()) {
                                    std::set< uint32_t > didSet(dtcIds.begin(), dtcIds.end());
                                    faultConfig_.dtcGroup[groupId] = didSet;
                                } else {
                                    LogError() << "Config::_initializeDTCGroups|have same dtc group id=" << groupId;
                                }
                            });
    if ((ret != 0) || !ok) {
        return false;
    }

    return true;
}
/// @brief
/// @param dmc
/// @return
/// @throw
bool UdsConfig::_initializeConditionGroups(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc)
{
    bool ok{true};
    /// Clear conditions
    std::int32_t ret{dmc->IterateArray(
        std::move(ara::core::StringView("clearCondition")),
        [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            std::uint32_t id{UINT32_MAX};
            if (0 != node.Load(std::move(ara::core::StringView("id")), id)) {
                LogError() << "UdsConfig::_initializeConditionGroups|load "
                              "clearCondition id fail";
                ok = false;
                return;
            }
            if (id == UINT32_MAX) {
                LogError() << "UdsConfig::_initializeConditionGroups|invalid clear "
                              "condition id";
                ok = false;
                return;
            }

            fault::ConditionConfig condition;
            condition.enableConditionId = id;

            if (0 != node.Load(std::move(ara::core::StringView("initValue")), condition.state)) {
                LogWarn() << "UdsConfig::_initializeConditionGroups|clear condition "
                             "no config initValue id ="
                          << id;
            }

            std::uint32_t serviceId{};
            if (0 != node.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceId)) {
                LogWarn() << "UdsConfig::_initializeConditionGroups|no config clear "
                             "condition "
                             "serviceInstanceId id ="
                          << id;
            } else {
                /// Create interface for clear conditions to interact with AA
                condition.conditionInterface = std::make_shared< Condition >(serviceId);
            }

            std::map< uint32_t, ConditionConfig >::iterator iter = faultConfig_.clearConditionConfig.find(id);
            if (iter == faultConfig_.clearConditionConfig.end()) {
                faultConfig_.clearConditionConfig[id] = condition;
            } else {
                LogError() << "UdsConfig::_initializeConditionGroups|have same clear "
                              "condition id ="
                           << id;
            }
        })};
    if ((ret != 0) || !ok) {
        return false;
    }

    /// Clear condition group
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > const clearConditionGroupCallback{
        [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            uint32_t id{UINT32_MAX};
            if (0 != node.Load(std::move(ara::core::StringView("id")), id)) {
                LogError() << "UdsConfig::_initializeConditionGroups|load "
                              "clearConditionGroup id fail";
                ok = false;
                return;
            }
            if (id == UINT32_MAX) {
                LogError() << "UdsConfig::_initializeConditionGroups|invalid clear "
                              "condition group id";
                ok = false;
                return;
            }

            ara::core::Vector< uint32_t > idVector;
            if (0 != node.Load(std::move(ara::core::StringView("clearCondition")), idVector)) {
                LogError() << "UdsConfig::_initializeConditionGroups|load clear "
                              "conditon list fail id"
                           << id;
                ok = false;
                return;
            }

            std::map< uint32_t, std::set< uint32_t > >::iterator iter = faultConfig_.clearConditionGroup.find(id);
            if (iter == faultConfig_.clearConditionGroup.end()) {
                std::set< uint32_t > ids(idVector.begin(), idVector.end());
                faultConfig_.clearConditionGroup[id] = ids;
            } else {
                LogError() << "UdsConfig::_initializeConditionGroups|have same clear "
                              "conditon list fail id"
                           << id;
            }
        }};
    ret = dmc->IterateArray(std::move(ara::core::StringView("clearConditionGroup")), clearConditionGroupCallback);
    if ((ret != 0) || !ok) {
        return false;
    }

    /// Enable conditions
    ret = dmc->IterateArray(std::move(ara::core::StringView("enableCondition")),
                            [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
                                uint32_t id{UINT32_MAX};
                                if (0 != node.Load(std::move(ara::core::StringView("id")), id)) {
                                    LogError() << "UdsConfig::_initializeConditionGroups|load "
                                                  "enableCondition id fail";
                                    ok = false;
                                    return;
                                }
                                if (id == UINT32_MAX) {
                                    LogError() << "UdsConfig::_initializeConditionGroups|invalid enable "
                                                  "condition id";
                                    ok = false;
                                    return;
                                }

                                fault::ConditionConfig condition;
                                condition.enableConditionId = id;

                                if (0 != node.Load(std::move(ara::core::StringView("initValue")), condition.state)) {
                                    LogWarn() << "UdsConfig::_initializeConditionGroups|no config "
                                                 "enableCondition condition initValue id ="
                                              << id;
                                }

                                std::uint32_t serviceId{};
                                if (0 != node.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceId)) {
                                    LogWarn() << "UdsConfig::_initializeConditionGroups|no config "
                                                 "enableCondition condition "
                                                 "serviceInstanceId id ="
                                              << id;
                                } else {
                                    /// Create interface for enable conditions to interact with AA
                                    condition.conditionInterface = std::make_shared< Condition >(serviceId);
                                }

                                std::map< uint32_t, ConditionConfig >::iterator iter
                                    = faultConfig_.enableConditionConfig.find(id);
                                if (iter == faultConfig_.enableConditionConfig.end()) {
                                    faultConfig_.enableConditionConfig[id] = condition;
                                } else {
                                    LogError() << "UdsConfig::_initializeConditionGroups|have same "
                                                  "enable condition id ="
                                               << id;
                                }
                            });
    if ((ret != 0) || !ok) {
        return false;
    }

    /// Enable condition group
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > const enableConditionGroupCallback{
        [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            uint32_t id{UINT32_MAX};
            if (0 != node.Load(std::move(ara::core::StringView("id")), id)) {
                LogError() << "UdsConfig::_initializeConditionGroups|load enable "
                              "condition group id fail";
                ok = false;
                return;
            }
            if (id == UINT32_MAX) {
                LogError() << "UdsConfig::_initializeConditionGroups|invalid enable "
                              "condition group id ="
                           << id;
                ok = false;
                return;
            }

            ara::core::Vector< uint32_t > idVector;
            if (0 != node.Load(std::move(ara::core::StringView("enableCondition")), idVector)) {
                LogError() << "UdsConfig::_initializeConditionGroups|load clear "
                              "conditon list fail id"
                           << id;
                ok = false;
                return;
            }

            std::map< uint32_t, std::set< uint32_t > >::iterator iter = faultConfig_.enableConditionGroup.find(id);
            if (iter == faultConfig_.enableConditionGroup.end()) {
                std::set< uint32_t > ids(idVector.begin(), idVector.end());
                faultConfig_.enableConditionGroup[id] = ids;
            } else {
                LogError() << "UdsConfig::_initializeConditionGroups|have same clear "
                              "conditon id"
                           << id;
            }
        }};
    ret = dmc->IterateArray(std::move(ara::core::StringView("enableConditionGroup")), enableConditionGroupCallback);
    if ((ret != 0) || !ok) {
        return false;
    }

    return true;
}

/// @brief
/// @param dmc
/// @return
/// @throw
bool UdsConfig::_initializeOperationCycles(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc)
{
    bool ok{true};
    /// Operation cycle
    int32_t ret = dmc->IterateArray(
        std::move(ara::core::StringView("operationCycle")),
        [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            uint32_t id{UINT32_MAX};
            if (0 != node.Load(std::move(ara::core::StringView("id")), id)) {
                LogError() << "UdsConfig::_initializeOperationCycles|load "
                              "operationcycle id fail";
                ok = false;
                return;
            }
            if (id == UINT32_MAX) {
                LogError() << "UdsConfig::_initializeOperationCycles|invalid "
                              "operation cycle id";
                ok = false;
                return;
            }

            fault::OperationCycleConfig op;
            op.operationCycleId = id;

            if (0 != node.Load(std::move(ara::core::StringView("cycleAutostart")), op.autoStart)) {
                LogWarn() << "UdsConfig::_initializeOperationCycles|no config "
                             "cycleAutostart";
            }
            if (0 != node.Load(std::move(ara::core::StringView("automaticEnd")), op.autoEnd)) {
                LogWarn() << "UdsConfig::_initializeOperationCycles|no config automaticEnd";
            }
            if (0 != node.Load(std::move(ara::core::StringView("cycleStatusStorage")), op.cycleStatusStorage)) {
                LogWarn() << "UdsConfig::_initializeOperationCycles|no config "
                             "cycleStatusStorage";
            }

            tps::DiagnosticOperationCycleTypeEnum type;
            if (0 != node.Load(std::move(ara::core::StringView("type")), type)) {
                LogError() << "UdsConfig::_initializeOperationCycles|load "
                              "operationcycle type fail id ="
                           << id;
                ok = false;
                return;
            }
            op.type = static_cast< fault::DiagnosticOperationCycleTypeEnum >(type);

            std::uint32_t serviceId{};
            if (0 != node.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceId)) {
                LogWarn() << "UdsConfig::_initializeOperationCycles|no config "
                             "operationcycle "
                             "diagnosticInstanceIdentifier id ="
                          << id;
            } else {
                /// Create operation cycle interface for interaction with AA
                op.operationInterface = std::make_shared< OperationCycle >(serviceId);
            }

            std::map< uint32_t, OperationCycleConfig >::iterator iter = faultConfig_.operationCycleConfig.find(id);
            if (iter == faultConfig_.operationCycleConfig.end()) {
                faultConfig_.operationCycleConfig[id] = op;
            } else {
                LogError() << "UdsConfig::_initializeOperationCycles|have same "
                              "operationcycle id ="
                           << id;
            }
        });
    if ((ret != 0) || !ok) {
        return false;
    }
    return true;
}

/// @brief
/// @param dmc
/// @return
/// @throw
bool UdsConfig::_initializeMapEvents(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc)
{
    bool ok{true};
    std::int32_t ret{dmc->IterateArray(
        std::move(ara::core::StringView("indicator")),
        [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            uint32_t id{UINT32_MAX};
            if (0 != node.Load(std::move(ara::core::StringView("id")), id)) {
                LogError() << "UdsConfig::_initializeMapEvents|load indicator id fail";
                ok = false;
                return;
            }
            if (id == UINT32_MAX) {
                LogError() << "UdsConfig::_initializeMapEvents|invalid indicator id =" << id;
                ok = false;
                return;
            }

            fault::IndicatorConfig indictor;
            indictor.indicatorId = id;

            tps::DiagnosticIndicatorTypeEnum type;
            if (0 != node.Load(std::move(ara::core::StringView("type")), type)) {
                LogError() << "UdsConfig::_initializeMapEvents|load indicator type fail id =" << id;
                ok = false;
                return;
            }
            indictor.type = static_cast< fault::DiagnosticIndicatorTypeEnum >(type);

            std::uint32_t serviceId{};
            if (0 != node.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceId)) {
                LogWarn() << "UdsConfig::_initializeMapEvents|no config indicator "
                             "serviceInstanceId id ="
                          << id;
            } else {
                /// Create interface instance for communication with AA
                indictor.indicatorInterface = std::make_shared< Indicator >(serviceId);
            }

            std::map< uint32_t, IndicatorConfig >::iterator iter = faultConfig_.indicatorConfig.find(id);
            if (iter == faultConfig_.indicatorConfig.end()) {
                faultConfig_.indicatorConfig[id] = indictor;
            } else {
                LogError() << "UdsConfig::_initializeMapEvents|have same indicator id =" << id;
            }
        })};
    if ((ret != 0) || !ok) {
        return false;
    }

    std::ignore = dmc->IterateArray(
        std::move(ara::core::StringView("commonProperties.debounceAlgorithmProps")),
        [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            uint32_t id{UINT32_MAX};
            if (0 != node.Load(std::move(ara::core::StringView("id")), id)) {
                LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                              "commonProperties.debounceAlgorithmProps.id";
                ok = false;
                return;
            }
            if (id == UINT32_MAX) {
                LogError() << "UdsConfig::_initializeMapEvents|invalid debounce id =" << id;
                ok = false;
                return;
            }

            fault::DebounceAlgorithmConfig ac;
            ac.algorithmId = id;

            ara::core::String typeStr;
            if (0 != node.Load(std::move(ara::core::StringView("debounceAlgorithm.kind")), typeStr)) {
                LogError() << "UdsConfig::_initializeMapEvents|load "
                              "debounceAlgorithm.kind fail id ="
                           << id;
                ok = false;
                return;
            }
            if (typeStr == "DIAG-EVENT-DEBOUNCE-MONITOR-INTERNAL") {
                ac.type = fault::DebounceTypeEnum::kUserDefined;
            } else if (typeStr == "DIAG-EVENT-DEBOUNCE-COUNTER-BASED") {
                ac.type = fault::DebounceTypeEnum::kCounterBase;
            } else if (typeStr == "DIAG-EVENT-DEBOUNCE-TIME-BASED") {
                ac.type = fault::DebounceTypeEnum::kTimeBase;
            } else {
                LogError() << "UdsConfig::_initializeMapEvents|invalid "
                              "debounceAlgorithm kind="
                           << typeStr.c_str();
                ok = false;
                return;
            }

            tps::DiagnosticDebounceBehaviorEnum debounceBehavior;
            if (0 != node.Load(std::move(ara::core::StringView("debounceBehavior")), debounceBehavior)) {
                LogError() << "UdsConfig::_initializeMapEvents|load debounceBehavior "
                              "fail id ="
                           << id;
                ok = false;
                return;
            }
            ac.debounceBehavior = static_cast< fault::DiagnosticDebounceBehaviorEnum >(debounceBehavior);

            if (0 != node.Load(std::move(ara::core::StringView("debounceCounterStorage")), ac.debounceCounterStorage)) {
                LogWarn() << "UdsConfig::_initializeMapEvents|no config "
                             "debounceCounterStorage it will use "
                             "default[false] id ="
                          << id;
            }

            if (fault::DebounceTypeEnum::kCounterBase == ac.type) {
                std::shared_ptr< fault::CounterBasedParam > counterParam
                    = std::make_shared< fault::CounterBasedParam >();
                if (0
                    != node.Load(std::move(ara::core::StringView("debounceAlgorithm."
                                                                 "counterBasedFdcThresholdStorageValue")),
                                 counterParam->counterBasedFdcThresholdStorageValue)) {
                    LogError() << "UdsConfig::_initializeMapEvents|load "
                                  "debounceAlgorithm."
                                  "counterBasedFdcThresholdStorageValue fail id ="
                               << id;
                    ok = false;
                    return;
                }
                if (0
                    != node.Load(std::move(ara::core::StringView("debounceAlgorithm.counterDecrementStepSize")),
                                 counterParam->counter.counterDecrementStepSize)) {
                    LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                                  "debounceAlgorithm.counterDecrementStepSize id ="
                               << id;
                    ok = false;
                    return;
                }

                if (0
                    != node.Load(std::move(ara::core::StringView("debounceAlgorithm.counterFailedThreshold")),
                                 counterParam->counter.counterFailedThreshold)) {
                    LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                                  "debounceAlgorithm.counterFailedThreshold id ="
                               << id;
                    ok = false;
                    return;
                }
                if (0
                    != node.Load(std::move(ara::core::StringView("debounceAlgorithm.counterIncrementStepSize")),
                                 counterParam->counter.counterIncrementStepSize)) {
                    LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                                  "debounceAlgorithm.counterIncrementStepSize id ="
                               << id;
                    ok = false;
                    return;
                }
                if (0
                    != node.Load(std::move(ara::core::StringView("debounceAlgorithm.counterJumpDown")),
                                 counterParam->counter.counterJumpDown)) {
                    LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                                  "debounceAlgorithm.counterJumpDown id ="
                               << id;
                    ok = false;
                    return;
                }
                if (0
                    != node.Load(std::move(ara::core::StringView("debounceAlgorithm.counterJumpDownValue")),
                                 counterParam->counter.counterJumpDownValue)) {
                    LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                                  "debounceAlgorithm.counterJumpDownValue id ="
                               << id;
                    ok = false;
                    return;
                }
                if (0
                    != node.Load(std::move(ara::core::StringView("debounceAlgorithm.counterJumpUp")),
                                 counterParam->counter.counterJumpUp)) {
                    LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                                  "debounceAlgorithm.counterJumpUp id ="
                               << id;
                    ok = false;
                    return;
                }
                if (0
                    != node.Load(std::move(ara::core::StringView("debounceAlgorithm.counterJumpUpValue")),
                                 counterParam->counter.counterJumpUpValue)) {
                    LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                                  "debounceAlgorithm.counterJumpUpValue id ="
                               << id;
                    ok = false;
                    return;
                }
                if (0
                    != node.Load(std::move(ara::core::StringView("debounceAlgorithm.counterPassedThreshold")),
                                 counterParam->counter.counterPassedThreshold)) {
                    LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                                  "debounceAlgorithm.counterPassedThreshold id ="
                               << id;
                    ok = false;
                    return;
                }

                ac.counterBased = counterParam;
            }
            if (fault::DebounceTypeEnum::kTimeBase == ac.type) {
                std::shared_ptr< fault::TimeBasedParam > counterParam = std::make_shared< fault::TimeBasedParam >();
                float tem{0.0};
                if (0
                    != node.Load(
                        std::move(ara::core::StringView("debounceAlgorithm.timeBasedFdcThresholdStorageValue")), tem)) {
                    LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                                  "debounceAlgorithm.timeBasedFdcThresholdStorageValue";
                    ok = false;
                    return;
                }
                counterParam->timeBasedFdcThresholdStorageValue = static_cast< uint32_t >(tem);

                if (0 != node.Load(std::move(ara::core::StringView("debounceAlgorithm.timeFailedThreshold")), tem)) {
                    LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                                  "debounceAlgorithm.timeFailedThreshold";
                    ok = false;
                    return;
                }
                counterParam->timer.timeFailedThreshold = static_cast< uint32_t >(tem);

                if (0 != node.Load(std::move(ara::core::StringView("debounceAlgorithm.timePassedThreshold")), tem)) {
                    LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                                  "debounceAlgorithm.timePassedThreshold";
                    ok = false;
                    return;
                }
                counterParam->timer.timePassedThreshold = static_cast< uint32_t >(tem);
                ac.timeBased                            = counterParam;
            }

            std::map< uint32_t, DebounceAlgorithmConfig >::iterator iter
                = faultConfig_.debounceAlgorithmConfig.find(id);
            if (iter == faultConfig_.debounceAlgorithmConfig.end()) {
                faultConfig_.debounceAlgorithmConfig[id] = ac;
            } else {
                LogError() << "UdsConfig::_initializeMapEvents|have same "
                              "debounceAlgorithm id ="
                           << id;
            }
        });
    if ((ret != 0) || !ok) {
        return false;
    }

    ret = dmc->IterateArray(
        std::move(ara::core::StringView("diagnosticEvent")),
        [this, &ok](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            uint32_t eventId{UINT32_MAX};
            if (0 != node.Load(std::move(ara::core::StringView("associatedEventIdentification")), eventId)) {
                LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                              "diagnosticEvent.id";
                ok = false;
                return;
            }
            if (eventId == UINT32_MAX) {
                LogError() << "UdsConfig::_initializeMapEvents|invalid event id=" << eventId;
                ok = false;
                return;
            }

            fault::EventConfig event;
            event.associatedEventIdentification = eventId;

            std::uint32_t serviceId{};
            if (0 != node.Load(std::move(ara::core::StringView("serviceInstanceId.event")), serviceId)) {
                LogInfo() << "UdsConfig::_initializeMapEvents|no config event "
                             "diagnosticInstanceIdentifier eventId ="
                          << eventId;
            } else {
                /// Create communication interface with AA
                event.eventInterface = std::make_shared< Event >(serviceId);
            }

            tps::DiagnosticClearEventAllowedBehaviorEnum clearEventAllowedBehavior;
            if (0
                != node.Load(std::move(ara::core::StringView("clearEventAllowedBehavior")),
                             clearEventAllowedBehavior)) {
                LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                              "clearEventAllowedBehavior eventId ="
                           << eventId;
                ok = false;
                return;
            }
            event.clearEventAllowedBehavior
                = static_cast< fault::DiagnosticClearEventAllowedBehaviorEnum >(clearEventAllowedBehavior);

            if (0
                != node.Load(std::move(ara::core::StringView("mapping.enableConditionGroup")),
                             event.enableConditionGroup)) {
                event.enableConditionGroup = UINT32_MAX;
                LogWarn() << "UdsConfig::_initializeMapEvents|event no config "
                             "enableConditionGroup eventId ="
                          << eventId;
            }

            if (0 != node.Load(std::move(ara::core::StringView("mapping.operationCycle")), event.operationCycle)) {
                LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                              "mapping.operationCycle eventId ="
                           << eventId;
                ok = false;
                return;
            }

            if (0 != node.Load(std::move(ara::core::StringView("mapping.troubleCodeUds")), event.troubleCodeUds)) {
                LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                              "mapping.troubleCodeUds eventId ="
                           << eventId;
                ok = false;
                return;
            }

            std::uint32_t monitorInstanceId{};
            if (0 != node.Load(std::move(ara::core::StringView("serviceInstanceId.monitor")), monitorInstanceId)) {
                LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                              "diagnosticInstanceIdentifier.monitor eventId ="
                           << eventId;
                ok = false;
                return;
            }

            /// Create communication interface with AA
            event.monitorInterface = std::make_shared< Monitor >(monitorInstanceId);

            // [SWS_DM_01037].
            if ((0 != node.Load(std::move(ara::core::StringView("confirmationThreshold")), event.confirmationThreshold))
                || (0U == event.confirmationThreshold)) {
                event.confirmationThreshold = 1U;
            }

            if (0
                != node.Load(std::move(ara::core::StringView("recoverableInSameOperationCycle")),
                             event.recoverableInSameOperationCycle)) {
                LogWarn() << "UdsConfig::_initializeMapEvents|no config "
                             "recoverableInSameOperationCycle it will use "
                             "default[false] eventId ="
                          << eventId;
            }

            if (0
                != node.Load(std::move(ara::core::StringView("mapping.debounceAlgorithm")), event.debounceAlgorithm)) {
                LogWarn() << "UdsConfig::_initializeMapEvents|no config debouce eventId=" << eventId;
            } else {
                decltype(auto) iter = faultConfig_.debounceAlgorithmConfig.find(event.debounceAlgorithm);
                if (iter == faultConfig_.debounceAlgorithmConfig.end()) {
                    LogError() << "UdsConfig::_initializeMapEvents|no find "
                                  "debounceAlgorithm eventId ="
                               << eventId << "debounceAlgorithmId =" << event.debounceAlgorithm;
                    ok = false;
                    return;
                }
            }

            std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > const
                connectionIndictorCallback{[this, &ok, &event](std::size_t,
                                                               isoft::manifestreader::ManifestNode const &mfNode) {
                    uint32_t idIndicator{UINT32_MAX};
                    if (0 != mfNode.Load(std::move(ara::core::StringView("indicator")), idIndicator)) {
                        LogError() << "UdsConfig::_initializeMapEvents|load fail. indicator";
                        ok = false;
                        return;
                    }
                    if (idIndicator == UINT32_MAX) {
                        LogError() << "UdsConfig::_initializeMapEvents|invalid indicator id =" << idIndicator;
                        ok = false;
                        return;
                    }

                    fault::ConnectedIndicatorConfig indicator;
                    indicator.indicatorId = idIndicator;

                    if (0 != mfNode.Load(std::move(ara::core::StringView("healingCycle")), indicator.healingCycle)) {
                        LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                                      "healingCycle idIndicator ="
                                   << idIndicator;
                        ok = false;
                        return;
                    }

                    tps::DiagnosticConnectedIndicatorBehaviorEnum behavior;
                    if (0 != mfNode.Load(std::move(ara::core::StringView("behavior")), behavior)) {
                        LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                                      "behavior idIndicator ="
                                   << idIndicator;
                        ok = false;
                        return;
                    }
                    indicator.behavior = static_cast< fault::DiagnosticConnectedIndicatorBehaviorEnum >(behavior);

                    if (0
                        != mfNode.Load(std::move(ara::core::StringView("healingCycleCounterThreshold")),
                                       indicator.healingCycleCounterThreshold)) {
                        LogError() << "UdsConfig::_initializeMapEvents|load fail. "
                                      "healingCycleCounterThreshold idIndicator ="
                                   << idIndicator;
                        ok = false;
                        return;
                    }

                    event.connectedIndicator.push_back(indicator);
                }};

            std::ignore
                = node.IterateArray(std::move(ara::core::StringView("connectedIndicator")), connectionIndictorCallback);

            std::map< uint32_t, EventConfig >::iterator iter = faultConfig_.eventConfig.find(eventId);
            if (iter == faultConfig_.eventConfig.end()) {
                faultConfig_.eventConfig[eventId] = event;
            } else {
                LogError() << "UdsConfig::_initializeMapEvents|have same event id =" << eventId;
            }
        });
    if ((ret != 0) || !ok) {
        return false;
    }
    return true;
}

bool UdsConfig::_initializeServiceTable(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc) noexcept
{
    int result{isoft::kSuccess};
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > serviceLoader{
        [&](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            ara::core::String className;
            if (isoft::kSuccess == node.Load("diagnosticServiceClass", className)) {
                if (className == "DIAGNOSTIC-SESSION-CONTROL-CLASS") {
                    result = _initializeServiceX10(node);
                } else if (className == "DIAGNOSTIC-ECU-RESET-CLASS") {
                    result = _initializeServiceX11(node);
                } else if (className == "DIAGNOSTIC-CLEAR-DIAGNOSTIC-INFORMATION-CLASS") {
                    result = _initializeServiceX14(node);
                } else if (className == "DIAGNOSTIC-READ-DTC-INFORMATION-CLASS") {
                    result = _initializeServiceX19(node);
                } else if (className == "DIAGNOSTIC-READ-DATA-BY-IDENTIFIER-CLASS") {
                    result = _initializeServiceX22(node);
                } else if (className == "DIAGNOSTIC-SECURITY-ACCESS-CLASS") {
                    result = _initializeServiceX27(node);
                } else if (className == "DIAGNOSTIC-COM-CONTROL-CLASS") {
                    result = _initializeServiceX28(node);
                } else if (className == "DIAGNOSTIC-AUTHENTICATION-CLASS") {
                    result = _initializeServiceX29(node);
                } else if (className == "DIAGNOSTIC-READ-DATA-BY-PERIODIC-ID-CLASS") {
                    result = _initializeServiceX2A(node);
                } else if (className == "DIAGNOSTIC-DYNAMICALLY-DEFINE-DATA-IDENTIFIER-CLASS") {
                    result = _initializeServiceX2C(node);
                } else if (className == "DIAGNOSTIC-WRITE-DATA-BY-IDENTIFIER-CLASS") {
                    result = _initializeServiceX2E(node);
                } else if (className == "DIAGNOSTIC-ROUTINE-CONTROL-CLASS") {
                    result = _initializeServiceX31(node, dmc);
                } else if (className == "DIAGNOSTIC-REQUEST-DOWNLOAD-CLASS") {
                    result = _initializeServiceX34(node);
                } else if (className == "DIAGNOSTIC-REQUEST-UPLOAD-CLASS") {
                    result = _initializeServiceX35(node);
                } else if (className == "DIAGNOSTIC-REQUEST-FILE-TRANSFER-CLASS") {
                    result = _initializeServiceX38(node);
                } else if (className == "DIAGNOSTIC-CONTROL-DTC-SETTING-CLASS") {
                    result = _initializeServiceX85(node);
                } else if (className == "DIAGNOSTIC-RESPONSE-ON-EVENT-CLASS") {
                    result = _initializeServiceX86(node);
                } else if (className == "DIAGNOSTIC-CUSTOM-SERVICE-CLASS") {
                    result = _initializeCustomService(node);
                } else {
                    LogError() << "Server::Initialize|unknown class " << className.c_str();
                }
                if (result != isoft::kSuccess) {
                    LogError() << " UdsConfig::_initializeSessionConfig| load "
                                  "diagnosticService config fails, service name:"
                               << className << " error:" << result;
                }
                return;
            }
            return;
        }};
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > serviceTableLoader{
        [&](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            result = node.IterateArray("service", serviceLoader);
            if (result != isoft::kSuccess) {
                LogWarn() << "UdsConfig::_initializeServiceTable| load service fails!";
            }
            return;
        }};
    result = dmc->IterateArray("serviceTable", serviceTableLoader);
    if (result != isoft::kSuccess) {
        LogWarn() << " UdsConfig::_initializeSessionConfig| load diagnosticService "
                     "config fails,error:"
                  << result;
    }
    return true;
}

int UdsConfig::_initializeServerSetting(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc) noexcept
{
    if (isoft::kSuccess != dmc->Load("softwareCluster.diagnosticAddress.physical", serverSetting_.physicalAddress)) {
        LogError() << "UdsConfig::_initializeServerSetting| Load "
                      "softwareCluster.diagnosticAddress.physical fails! "
                      "Please check config value!";
        return -1;
    }
    if (isoft::kSuccess
        != dmc->Load("softwareCluster.diagnosticAddress.functional", serverSetting_.functionAddressTable)) {
        LogError() << "UdsConfig::_initializeServerSetting| Load "
                      "softwareCluster.diagnosticAddress.functional fails! "
                      "Please check config value!";
        return -1;
    }
    if (dmc->Load(std::move(ara::core::StringView("commonProperties."
                                                  "maxNumberOfRequestCorrectlyReceivedResponsePending")),
                  serverSetting_.maxNumberOfRequestCorrectlyReceivedResponsePending)
        != 0) {
        serverSetting_.maxNumberOfRequestCorrectlyReceivedResponsePending = static_cast< std::uint8_t >(UINT8_MAX);
    }
    if (dmc->Load(std::move(ara::core::StringView("commonProperties.responseOnSecondDeclinedRequest")),
                  serverSetting_.responseOnSecondDeclinedRequest)
        != 0) {
        serverSetting_.responseOnSecondDeclinedRequest = false;
    }
    if (dmc->Load(std::move(ara::core::StringView("commonProperties.securityDelayTimeOnBoot")),
                  serverSetting_.securityDelayTimeOnBoot)
        != 0) {
        LogInfo() << "UdsConfig::_initializeServerSetting| not config "
                     "commonProperties.securityDelayTimeOnBoot!";
    }
    if (dmc->Load("conversation.maxNum", serverSetting_.maxParallelRequests) != isoft::kSuccess) {
        common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to "
                              "load conversation.maxNum!";
        return __LINE__;
    }

    return 0;
}

int UdsConfig::_initializeServiceValidation(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc) noexcept
{
    int result{isoft::kSuccess};
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > manufacturerLoader{
        [&](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            std::uint16_t clientInstanceId{};
            std::uint32_t serviceInstanceId{};
            if (node.Load(std::move(ara::core::StringView("clientInstanceId")), clientInstanceId) != isoft::kSuccess
                || node.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceInstanceId)
                       != isoft::kSuccess) {
                common::LogWarn() << "UdsConfig::_initializeServiceValidation | "
                                     "check clientInstanceId,serviceInstanceId! ";
                return -1;
            }
            serverSetting_.manufacturerServiceValidationTable.push_back(
                std::make_shared< ServiceValidation >(clientInstanceId, serviceInstanceId));
            return 0;
        }};
    result = dmc->IterateArray("serviceValidation.manufacturer", manufacturerLoader);
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > supplierLoader{
        [&](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            std::uint16_t clientInstanceId{};
            std::uint32_t serviceInstanceId{};
            if (node.Load(std::move(ara::core::StringView("clientInstanceId")), clientInstanceId) != isoft::kSuccess
                || node.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceInstanceId)
                       != isoft::kSuccess) {
                common::LogWarn() << "UdsConfig::_initializeServiceValidation | "
                                     "check clientInstanceId,serviceInstanceId! ";
                return -1;
            }
            serverSetting_.supplierServiceValidationTable.push_back(
                std::make_shared< ServiceValidation >(clientInstanceId, serviceInstanceId));
            return 0;
        }};
    result = dmc->IterateArray("serviceValidation.supplier", supplierLoader);
    return result;
}

int UdsConfig::_initializeDiagnosticAuthentication(
    std::unique_ptr< isoft::manifestreader::Manifest > const &dmc) noexcept
{
    int result{isoft::kSuccess};
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > diagnosticAuthRoleFunc{
        [&](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            isoft::uds::server::AuthRole authRole;
            ara::core::String shortName;
            if (isoft::kSuccess != node.Load("shortName", shortName)) {
                LogError() << "UdsConfig::_initializeDiagnosticAuthentication| "
                              "Load diagnosticAuthRole.shortName "
                              "fails! Please check config value!";
                return;
            }
            authRole.shortName = shortName.c_str();
            if (isoft::kSuccess != node.Load("bitPosition", authRole.bitPosition)) {
                LogError() << "UdsConfig::_initializeDiagnosticAuthentication| "
                              "Load diagnosticAuthRole.bitPosition "
                              "fails! Please check "
                              "config value!";
                return;
            }
            if (isoft::kSuccess != node.Load("isDefault", authRole.isDefault)) {
                LogError() << "UdsConfig::_initializeDiagnosticAuthentication| "
                              "Load diagnosticAuthRole.isDefault "
                              "fails! Please check "
                              "config value!";
                return;
            }
            authenticationManagement_.authRoleList.push_back(authRole);
            return;
        }};
    result = dmc->IterateArray("diagnosticAuthentication.diagnosticAuthRole", diagnosticAuthRoleFunc);

    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > externalAuthenticationFunc{
        [&](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            isoft::uds::server::ExternalAuthenticationIdentification extAuthId{};
            if (isoft::kSuccess != node.Load("sourceAddressCode", extAuthId.sourceAddressCode)) {
                LogError() << "UdsConfig::_initializeDiagnosticAuthentication| Load "
                              "ExternalAuthenticationIdentification.sourceAddressCode "
                              "fails! Please check config value!";
                return;
            }
            std::uint32_t sourceAddressMask{};
            if (isoft::kSuccess != node.Load("sourceAddressMask", sourceAddressMask)) {
                LogError() << "UdsConfig::_initializeDiagnosticAuthentication| Load "
                              "ExternalAuthenticationIdentification."
                              "sourceAddressMask fails! Please check config value!";
                return;
            }
            extAuthId.sourceAddressMask = sourceAddressMask;
            authenticationManagement_.externalAuthenticationList.push_back(extAuthId);
            return;
        }};
    result = dmc->IterateArray("diagnosticAuthentication.externalAuthentication", externalAuthenticationFunc);
    result
        = dmc->Load("diagnosticAuthentication.authenticationTimeout", authenticationManagement_.authenticationTimeout);
    return result;
}

int UdsConfig::_initializeDiagnosticSession(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc) noexcept
{
    int result{isoft::kSuccess};
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > loader{
        [&](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            isoft::uds::server::SessionModel sessionModel;
            ara::core::String shortName;
            if (isoft::kSuccess != node.Load("id", sessionModel.id)) {
                LogError() << "UdsConfig::_initializeSessionConfig| Load "
                              "diagnosticSession.id fails! Please check config value!";
                return;
            }
            if (isoft::kSuccess != node.Load("shortName", shortName)) {
                LogError() << "UdsConfig::_initializeSessionConfig| Load "
                              "diagnosticSession.shortName fails! Please check "
                              "config value!";
                return;
            }
            if (isoft::kSuccess != node.Load("p2ServerMax", sessionModel.p2ServerMax)) {
                LogError() << "UdsConfig::_initializeSessionConfig| Load "
                              "diagnosticSession.p2ServerMax fails! Please check "
                              "config value!";
                return;
            }
            if (isoft::kSuccess != node.Load("p2StarServerMax", sessionModel.p2StarServerMax)) {
                LogError() << "UdsConfig::_initializeSessionConfig| Load "
                              "diagnosticSession.p2StarServerMax fails! Please "
                              "check config value!";
                return;
            }
            sessionModel.shortName = shortName.c_str();
            sessionManagement_.sessionConfigTable.insert(sessionModel);
            return;
        }};
    result = dmc->IterateArray("diagnosticSession", loader);
    return result;
}

int UdsConfig::_initializeSecurityLevel(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc) noexcept
{
    int result{isoft::kSuccess};
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > loader{
        [&](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            isoft::uds::server::SecurityLevelConfig securityLevel;
            ara::core::String shortName;
            if (isoft::kSuccess == node.Load("id", securityLevel.id)
                && isoft::kSuccess == node.Load("shortName", shortName)
                && isoft::kSuccess == node.Load("accessDataRecordSize", securityLevel.accessDataRecordSize)
                && isoft::kSuccess == node.Load("keySize", securityLevel.keySize)
                && isoft::kSuccess == node.Load("numFailedSecurityAccess", securityLevel.numFailedSecurityAccess)
                && isoft::kSuccess == node.Load("securityDelayTime", securityLevel.securityDelayTime)
                && isoft::kSuccess == node.Load("seedSize", securityLevel.seedSize)) {
                std::uint16_t clientInstanceId{};
                std::uint32_t serviceInstanceId{};
                if (node.Load(std::move(ara::core::StringView("clientInstanceId")), clientInstanceId) != isoft::kSuccess
                    || node.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceInstanceId)
                           != isoft::kSuccess) {
                    common::LogWarn() << "UdsConfig::_initializeServiceX27| check "
                                         "clientInstanceId,serviceInstanceId! id: "
                                      << securityLevel.id;
                    return;
                }
                securityLevel.interfacePtr = std::make_shared< SecurityAccess >(clientInstanceId, serviceInstanceId);
                securityLevel.shortName    = shortName.c_str();
                securityAccessManagement_.securityLevelTable.insert(securityLevel);
                return;
            }
            LogWarn() << "UdsConfig::_initializeSecurityLevel| Load security level "
                         "fails! Please check config value!";
            return;
        }};
    securityAccessManagement_.securityDelayTimeOnBoot = serverSetting_.securityDelayTimeOnBoot;
    result                                            = dmc->IterateArray("securityLevel", loader);
    return result;
}

int UdsConfig::_initializeDataParameter(isoft::uds::server::DiagnosticDataModel &dataModel,
                                        isoft::manifestreader::ManifestNode const &node) noexcept
{
    int result{isoft::kSuccess};
    if (genericDataElementPtr_.get() == nullptr) {
        genericDataElementPtr_ = std::make_shared< GenericDataElement >();
    }
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > loader{
        [&](std::size_t, isoft::manifestreader::ManifestNode const &subnode) {
            isoft::uds::server::DiagnosticDataParamterModel model;
            if (isoft::kSuccess != subnode.Load("bitOffset", model.offset)) {
                LogWarn() << "UdsConfig::_initializeDataParameter| Load bitOffset fails!";
                return;
            }
            ara::core::String path;
            if (isoft::kSuccess != subnode.Load("dataElement._path", path)) {
                LogWarn() << "UdsConfig::_initializeDataParameter| Load __path fails!";
                return;
            }
            model.element.id = GetDiagnosticDataElementId(path);
            ara::core::String arraySizeSemantics{};
            if (isoft::kSuccess == subnode.Load("dataElement.arraySizeSemantics", arraySizeSemantics)) {
                std::shared_ptr< isoft::uds::server::DiagnosticDataElementArray > arrayPtr{
                    std::make_shared< isoft::uds::server::DiagnosticDataElementArray >()};
                if (arraySizeSemantics == ara::core::StringView("VARIABLE-SIZE")) {
                    arrayPtr->isVariableSize = true;
                }
                if (isoft::kSuccess != subnode.Load("dataElement.maxNumberOfElements", arrayPtr->maxNumberOfElements)) {
                    LogWarn() << "UdsConfig::_initializeDataParameter| Load "
                                 "dataElement.maxNumberOfElements fails!";
                    return;
                }
                model.element.array = arrayPtr;
            }
            ara::core::String baseType;
            if (isoft::kSuccess != subnode.Load("dataElement.baseType", baseType)) {
                LogWarn() << "UdsConfig::_initializeDataParameter| Load baseType fails!";
            } else {
                if (baseType == ara::core::StringView("int8")) {
                    model.element.dataType = isoft::uds::server::DataType::kInt8;
                } else if (baseType == ara::core::StringView("int16")) {
                    model.element.dataType = isoft::uds::server::DataType::kInt16;
                } else if (baseType == ara::core::StringView("int32")) {
                    model.element.dataType = isoft::uds::server::DataType::kInt32;
                } else if (baseType == ara::core::StringView("uint8")) {
                    model.element.dataType = isoft::uds::server::DataType::kUint8;
                } else if (baseType == ara::core::StringView("uint16")) {
                    model.element.dataType = isoft::uds::server::DataType::kUint16;
                } else if (baseType == ara::core::StringView("uint32")) {
                    model.element.dataType = isoft::uds::server::DataType::kUint32;
                } else if (baseType == ara::core::StringView("float")) {
                    model.element.dataType = isoft::uds::server::DataType::kFloat;
                }
            }
            model.element.provider = isoft::uds::server::DiagnosticDataElementProvider::kUnused;
            std::uint16_t clientInstanceId{};
            std::uint32_t serviceInstanceId{};
            if (subnode.Load(std::move(ara::core::StringView("clientInstanceId")), clientInstanceId) != isoft::kSuccess
                || subnode.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceInstanceId)
                       != isoft::kSuccess) {
                common::LogWarn() << "UdsConfig::_initializeDataParameter| check "
                                     "clientInstanceId,serviceInstanceId! ";
            } else {
                genericDataElementPtr_->RegisterServiceInstance(model.element.id, clientInstanceId, serviceInstanceId);
                model.element.provider = isoft::uds::server::DiagnosticDataElementProvider::kExternal;
            }
            dataModel.details.push_back(model);
            return;
        }};
    result                                                       = node.IterateArray("parameter", loader);
    diagnosticDataManagement_.didManager.dataElementInterfacePtr = genericDataElementPtr_;
    return result;
}

int UdsConfig::_initializeDataManager(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc) noexcept
{
    int result{isoft::kSuccess};
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > loader{
        [&](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            ara::core::String className;
            if (isoft::kSuccess != node.Load("diagnosticdataIdentifierClass", className)) {
                LogWarn() << "UdsConfig::_initializeDataManager| Load "
                             "diagnosticdataIdentifierClass fails!";
                result = __LINE__;
                return;
            }
            if (className == ara::core::StringView("DIAGNOSTIC-DATA-IDENTIFIER")) {
                isoft::uds::server::DiagnosticDataModel dataModel;
                if (isoft::kSuccess != node.Load("id", dataModel.id)) {
                    LogWarn() << "UdsConfig::_initializeDataManager| Load id fails!";
                    result = __LINE__;
                    return;
                }
                if (isoft::kSuccess != node.Load("didSize", dataModel.nSize)) {
                    LogWarn() << "UdsConfig::_initializeDataManager| dataIdentifier.didSize "
                                 "option is not configured fails!";
                    result = __LINE__;
                    // return;
                }
                result = _initializeDataParameter(dataModel, node);
                if (result != isoft::kSuccess) {
                    LogWarn() << "UdsConfig::_initializeDataManager| Load parameter "
                                 "fails! Please check config value!";
                    result = __LINE__;
                    return;
                }
                diagnosticDataManagement_.didManager.staticData.push_back(dataModel);
            } else if (className == ara::core::StringView("DIAGNOSTIC-DYNAMIC-DATA-IDENTIFIER")) {
                isoft::uds::server::DiagnosticDynamicDataIdentifier id{};
                if (isoft::kSuccess != node.Load("id", id)) {
                    LogWarn() << "UdsConfig::_initializeDataManager| Load id fails!";
                    result = __LINE__;
                    return;
                }
                diagnosticDataManagement_.didManager.dynamicData.push_back(id);
            }

            return;
        }};
    std::ignore = dmc->IterateArray("dataIdentifier", loader);
    return result;
}

int UdsConfig::_initializeEnvConditionFormulaPart(
    std::vector< std::shared_ptr< isoft::uds::server::EnvConditionFormulaPart > > &parts,
    isoft::manifestreader::ManifestNode const &node,
    ara::core::StringView const key) noexcept
{
    int result{isoft::kSuccess};
    std::function< void(const ara::core::StringView &, isoft::manifestreader::ManifestNode const &) > objectLoader{
        [&](const ara::core::StringView &memberName, isoft::manifestreader::ManifestNode const &subnode) {
            if (memberName == ara::core::StringView("envDataCondition")) {
                std::shared_ptr< isoft::uds::server::EnvConditionFormulaPart > part{};
                _initializeEnvDataCondition(part, subnode);
                parts.push_back(part);
            } else if (memberName == ara::core::StringView("envConditionFormula")) {
                std::shared_ptr< isoft::uds::server::EnvConditionFormulaPart > part{};
                _initializeEnvConditionFormula(part, subnode, "");
                parts.push_back(part);
            }
            return;
        }};
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > loader{
        [&](std::size_t, isoft::manifestreader::ManifestNode const &subnode) {
            subnode.IterateObject("", objectLoader);
            return;
        }};
    result = node.IterateArray(key, loader);
    return result;
}

int UdsConfig::_initializeEnvDataCondition(std::shared_ptr< isoft::uds::server::EnvConditionFormulaPart > &partPtr,
                                           isoft::manifestreader::ManifestNode const &node) noexcept
{
    int result{isoft::kSuccess};
    std::shared_ptr< isoft::uds::server::EnvDataCondition > conditionPtr
        = std::make_shared< isoft::uds::server::EnvDataCondition >();
    ara::core::String comPareType;
    result = node.Load("comPareType", comPareType);
    if (result != 0) {
        common::LogWarn() << "UdsConfig::InitializeEnvConditionFormulaCondition|"
                             "load compareType fails!";
    }
    if (comPareType == ara::core::StringView("IS-EQUAL")) {
        conditionPtr->compareType = isoft::uds::server::DiagnosticCompareTypeEnum::kIsEqual;
    } else if (comPareType == ara::core::StringView("IS-GREATER-OR-EQUAL")) {
        conditionPtr->compareType = isoft::uds::server::DiagnosticCompareTypeEnum::kIsGreaterOrEqual;
    } else if (comPareType == ara::core::StringView("IS-GREATER-THAN")) {
        conditionPtr->compareType = isoft::uds::server::DiagnosticCompareTypeEnum::kIsGreaterThan;
    } else if (comPareType == ara::core::StringView("IS-LESS-OR-EQUAL")) {
        conditionPtr->compareType = isoft::uds::server::DiagnosticCompareTypeEnum::kIsLessOrEqual;
    } else if (comPareType == ara::core::StringView("IS-LESS-THAN")) {
        conditionPtr->compareType = isoft::uds::server::DiagnosticCompareTypeEnum::kIsLessThan;
    } else if (comPareType == ara::core::StringView("IS-NOT-EQUAL")) {
        conditionPtr->compareType = isoft::uds::server::DiagnosticCompareTypeEnum::kIsNotEqual;
    }
    result = node.Load("comPareValue.isArray", conditionPtr->compareValue.isArray);
    if (result != 0) {
        common::LogWarn() << "UdsConfig::InitializeEnvConditionFormulaCondition|"
                             "load comPareValue.isArray fails!";
    }
    ara::core::String baseType;
    result = node.Load("comPareValue.type", baseType);
    if (result != 0) {
        common::LogWarn() << "UdsConfig::InitializeEnvConditionFormulaCondition|"
                             "load comPareValue.type fails!";
    }
    if (baseType == ara::core::StringView("int8")) {
        conditionPtr->compareValue.type = isoft::uds::server::DataType::kInt8;
    } else if (baseType == ara::core::StringView("int16")) {
        conditionPtr->compareValue.type = isoft::uds::server::DataType::kInt16;
    } else if (baseType == ara::core::StringView("int32")) {
        conditionPtr->compareValue.type = isoft::uds::server::DataType::kInt32;
    } else if (baseType == ara::core::StringView("uint8")) {
        conditionPtr->compareValue.type = isoft::uds::server::DataType::kUint8;
    } else if (baseType == ara::core::StringView("uint16")) {
        conditionPtr->compareValue.type = isoft::uds::server::DataType::kUint16;
    } else if (baseType == ara::core::StringView("uint32")) {
        conditionPtr->compareValue.type = isoft::uds::server::DataType::kUint32;
    } else if (baseType == ara::core::StringView("float")) {
        conditionPtr->compareValue.type = isoft::uds::server::DataType::kFloat;
    }

    result = node.Load("comPareValue.value", conditionPtr->compareValue.data);
    if (result != 0) {
        common::LogWarn() << "UdsConfig::InitializeEnvConditionFormulaCondition|"
                             "load comPareValue.data fails!";
    }
    ara::core::String elementId;
    result = node.Load("dataElement", elementId);
    if (result != 0) {
        common::LogWarn() << "UdsConfig::InitializeEnvConditionFormulaCondition|"
                             "load element id fails!";
    }
    conditionPtr->element = GetDiagnosticDataElementId(elementId);
    partPtr               = conditionPtr;
    return result;
}

int UdsConfig::_initializeEnvConditionFormula(std::shared_ptr< isoft::uds::server::EnvConditionFormulaPart > &partPtr,
                                              isoft::manifestreader::ManifestNode const &node,
                                              ara::core::StringView const key) noexcept
{
    int result{isoft::kSuccess};
    std::shared_ptr< isoft::uds::server::EnvConditionFormula > formulaPtr{
        std::make_shared< isoft::uds::server::EnvConditionFormula >()};
    ara::core::String nrcValueKey{};
    if (key != "") {
        nrcValueKey = key;
        nrcValueKey += ".";
    }
    nrcValueKey += "nrcValue";
    if (isoft::kSuccess != node.Load(nrcValueKey, formulaPtr->nrcValue)) {
        LogWarn() << "UdsConfig::_initializeEnvConditionFormula| Load nrcValue fails!";
        formulaPtr->nrcValue = isoft::serialize::kInt8_0x22U;
    }
    ara::core::String opKey{};
    if (key != "") {
        opKey = key;
        opKey += ".";
    }
    opKey += "OP";
    ara::core::String op;
    if (isoft::kSuccess != node.Load(opKey, op)) {
        LogWarn() << "UdsConfig::_initializeEnvConditionFormula| Load OP fails!";
        return isoft::kFailure;
    }
    if (op == ara::core::StringView("LOGICAL-AND")) {
        formulaPtr->op = isoft::uds::server::DiagnosticLogicalOperatorEnum::kLogicalAnd;
    } else if (op == ara::core::StringView("LOGICAL-OR")) {
        formulaPtr->op = isoft::uds::server::DiagnosticLogicalOperatorEnum::kLogicalOr;
    }
    ara::core::String partKey{};
    if (key != "") {
        partKey = key;
        partKey += ".";
    }
    partKey += "part";
    _initializeEnvConditionFormulaPart(formulaPtr->parts, node, partKey);
    partPtr = formulaPtr;
    return result;
}

int UdsConfig::_initializeEnvCondition(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc) noexcept
{
    int result{isoft::kSuccess};
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > loader{
        [&](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            std::shared_ptr< isoft::uds::server::EnvironmentalConditionModel > modelPtr{
                std::make_shared< isoft::uds::server::EnvironmentalConditionModel >()};
            if (isoft::kSuccess != node.Load("id", modelPtr->id)) {
                LogWarn() << "UdsConfig::_initializeEnvCondition| Load id fails!";
                return;
            }
            std::shared_ptr< isoft::uds::server::EnvConditionFormulaPart > partPtr{};
            _initializeEnvConditionFormula(partPtr, node, "formula");
            std::shared_ptr< isoft::uds::server::EnvConditionFormula > formulaPtr
                = std::dynamic_pointer_cast< isoft::uds::server::EnvConditionFormula >(partPtr);
            if (formulaPtr.get() == nullptr) {
                LogWarn() << "UdsConfig::_initializeEnvCondition| Load formula fails! id:" << modelPtr->id;
                return;
            }
            modelPtr->formula = *formulaPtr;
            diagnosticDataManagement_.environmentalConditionManager.push_back(modelPtr);
            return;
        }};
    result = dmc->IterateArray("environmentalCondition", loader);
    return result;
}

int UdsConfig::_initializeAccessPermission(std::unique_ptr< isoft::manifestreader::Manifest > const &dmc) noexcept
{
    int result{isoft::kSuccess};
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > loader{
        [&](std::size_t, isoft::manifestreader::ManifestNode const &node) {
            AccessPermission accessPermission;
            std::uint32_t id{};
            result = node.Load("id", id);
            if (isoft::kSuccess != result) {
                LogError() << "UdsConfig::_initializeAccessPermission| "
                              "accessPermission.id option is not configured!";
                return;
            }
            result = node.Load("diagnosticSession", accessPermission.sessionTable);
            if (isoft::kSuccess != result) {
                LogError() << "UdsConfig::_initializeAccessPermission| "
                              "accessPermission.diagnosticSession option is not "
                              "configured!";
                return;
            }
            result = node.Load("securityLevel", accessPermission.securityLevelTable);
            if (isoft::kSuccess != result) {
                LogError() << "UdsConfig::_initializeAccessPermission| "
                              "accessPermission.securityLevel option is not configured!";
                return;
            }
            result = node.Load("environmentalCondition", accessPermission.evCondition);
            if (isoft::kSuccess != result) {
                LogWarn() << "UdsConfig::_initializeAccessPermission| "
                             "accessPermission.environmentalCondition option is not "
                             "configured!";
            }
            std::vector< ara::core::String > authRoleList;
            result = node.Load("authenticationEnabled", authRoleList);
            if (isoft::kSuccess != result) {
                accessPermission.enableAuth.enable = false;
                LogInfo() << "UdsConfig::_initializeAccessPermission| "
                             "accessPermission.authenticationEnabled option is false!";
            } else {
                for (auto &&authRole : authRoleList) {
                    accessPermission.enableAuth.authRoleList.insert(authRole.c_str());
                }
                accessPermission.enableAuth.enable = true;
            }
            accessPermissionTable_.insert({id, accessPermission});
            return;
        }};
    result = dmc->IterateArray("accessPermission", loader);
    return result;
}

int UdsConfig::_initializeServiceX10(isoft::manifestreader::ManifestNode const &node) noexcept
{
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > instanceLoader{
        [this](std::size_t, isoft::manifestreader::ManifestNode const &subnode) {
            isoft::uds::server::SessionControlInstanceConfig instanceConfig;
            std::uint32_t accessPermissionId{};
            if (isoft::kSuccess == subnode.Load("diagnosticSession", instanceConfig.sesssionIdConfig)
                && isoft::kSuccess == subnode.Load("accessPermission", accessPermissionId)) {
                decltype(auto) findResult{accessPermissionTable_.find(accessPermissionId)};
                if (findResult == accessPermissionTable_.end()) {
                    LogWarn() << "UdsConfig::_initializeServiceX10| load "
                                 "accessPermission fails!, please check config file!";
                    return;
                }
                instanceConfig.sessionPermission.insert(findResult->second.sessionTable.begin(),
                                                        findResult->second.sessionTable.end());
                instanceConfig.securityLevelPermission.insert(findResult->second.securityLevelTable.begin(),
                                                              findResult->second.securityLevelTable.end());
                instanceConfig.accessPermissionAuth = findResult->second.enableAuth;
                sessionManagement_.sessionControlInstanceTable.insert(instanceConfig);
                return;
            }
            LogWarn() << "UdsConfig::_initializeSessionConfig| Load service x10 "
                         "fails! Please check config value!";
            return;
        }};
    sessionManagement_.p4ServerMax = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    return node.IterateArray("instance", instanceLoader);
}

int UdsConfig::_initializeServiceX11(isoft::manifestreader::ManifestNode const &node) noexcept
{
    ara::core::String respondToReset{};
    if (node.Load(std::move(ara::core::StringView("respondToReset")), respondToReset) != 0) {
        ecuResetManagement_.respondToReset = isoft::uds::server::DiagnosticResponseToEcuResetEnum::kRespondBeforeReset;
    } else {
        if (respondToReset == ara::core::StringView("RESPOND-BEFORE-RESET")) {
            ecuResetManagement_.respondToReset
                = isoft::uds::server::DiagnosticResponseToEcuResetEnum::kRespondBeforeReset;
        } else {
            ecuResetManagement_.respondToReset
                = isoft::uds::server::DiagnosticResponseToEcuResetEnum::kRespondAfterReset;
        }
    }
    std::function< void(std::size_t idx, isoft::manifestreader::ManifestNode const &) > instanceLoader{
        [this](std::size_t idx, isoft::manifestreader::ManifestNode const &instanceNode) {
            ara::core::String category;
            isoft::uds::server::EcuResetInstanceConfig instanceConfig;
            std::ignore = instanceNode.Load(std::move(ara::core::StringView("category")), category);
            /// [TPS_DEXT_01056]
            if (category == ara::core::StringView("HARD_RESET")) {
                instanceConfig.subfunctionNumber = static_cast< std::uint8_t >(isoft::uds::server::kHardReset);
            } else if (category == ara::core::StringView("KEY_OFF_ON_RESET")) {
                instanceConfig.subfunctionNumber = static_cast< std::uint8_t >(isoft::uds::server::kKeyOffOnReset);
            } else if (category == ara::core::StringView("SOFT_RESET")) {
                instanceConfig.subfunctionNumber = static_cast< std::uint8_t >(isoft::uds::server::kSoftReset);
            } else if (category == ara::core::StringView("ENABLE_RAPID_POWER_SHUT_DOWN")) {
                instanceConfig.subfunctionNumber
                    = static_cast< std::uint8_t >(isoft::uds::server::kEnableRapidPowerShutDown);
            } else if (category == ara::core::StringView("DISABLE_RAPID_POWER_SHUT_DOWN")) {
                instanceConfig.subfunctionNumber
                    = static_cast< std::uint8_t >(isoft::uds::server::kDisableRapidPowerShutDown);
            } else {
                if (instanceNode.Load(std::move(ara::core::StringView("customSubFunctionNumber")),
                                      instanceConfig.subfunctionNumber)
                    != 0) {
                    common::LogWarn() << "UdsConfig::_initializeServiceX11|failed to parse node|" << idx;
                    return;
                }
            }
            std::uint16_t clientInstanceId{};
            std::uint32_t serviceInstanceId{};
            if (instanceNode.Load(std::move(ara::core::StringView("clientInstanceId")), clientInstanceId)
                    != isoft::kSuccess
                || instanceNode.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceInstanceId)
                       != isoft::kSuccess) {
                common::LogWarn() << "UdsConfig::_initializeServiceX11| check "
                                     "clientInstanceId,serviceInstanceId! ";
                return;
            }
            ecuResetManagement_.interfacePtr = std::make_shared< EcuResetRequest >(clientInstanceId, serviceInstanceId);

            std::uint32_t accessPermissionId{};
            if (isoft::kSuccess == instanceNode.Load(ara::core::StringView("accessPermission"), accessPermissionId)) {
                decltype(auto) findResult{accessPermissionTable_.find(accessPermissionId)};
                if (findResult == accessPermissionTable_.end()) {
                    LogWarn() << "UdsConfig::_initializeServiceX11| load "
                                 "accessPermission fails!, please check config file!";
                    return;
                }
                instanceConfig.accessPermissionSession.insert(findResult->second.sessionTable.begin(),
                                                              findResult->second.sessionTable.end());
                instanceConfig.accessPermissionSecurityLevel.insert(findResult->second.securityLevelTable.begin(),
                                                                    findResult->second.securityLevelTable.end());
                instanceConfig.accessPermissionAuth = findResult->second.enableAuth;
                ecuResetManagement_.instanceTable.insert(instanceConfig);
                return;
            }
            LogWarn() << "UdsConfig::_initializeServiceX11| Load service x11 "
                         "fails! Please check config value!";
            return;
        }};

    ecuResetManagement_.p4ServerMax = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    return node.IterateArray("instance", instanceLoader);
}

int UdsConfig::_initializeServiceX22(isoft::manifestreader::ManifestNode const &node) noexcept
{
    if (diagnosticDataManagement_.service.serviceX22.get() == nullptr) {
        diagnosticDataManagement_.service.serviceX22 = std::make_shared< isoft::uds::server::ServiceX22Model >();
    }
    if (node.Load(std::move(ara::core::StringView("maxDidToRead")),
                  diagnosticDataManagement_.service.serviceX22->maxDidToRead)
        != isoft::kSuccess) {
        diagnosticDataManagement_.service.serviceX22->maxDidToRead = 1U;
    }
    if (genericDataIdentifierPtr_.get() == nullptr) {
        genericDataIdentifierPtr_                             = std::make_shared< GenericDataIdentifier >();
        diagnosticDataManagement_.didManager.dataInterfacePtr = genericDataIdentifierPtr_;
    }
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > instanceLoader{
        [this](std::size_t, isoft::manifestreader::ManifestNode const &instanceNode) {
            isoft::uds::server::ReadDiagnosticDataByIdentifier instanceConfig;
            if (instanceNode.Load(std::move(ara::core::StringView("dataIdentifier")), instanceConfig.id)
                != isoft::kSuccess) {
                return;
            }
            std::uint32_t accessPermissionId{};
            if (isoft::kSuccess == instanceNode.Load(ara::core::StringView("accessPermission"), accessPermissionId)) {
                decltype(auto) findResult{accessPermissionTable_.find(accessPermissionId)};
                if (findResult == accessPermissionTable_.end()) {
                    LogWarn() << "UdsConfig::_initializeServiceX22| load "
                                 "accessPermission fails!, please check config file!";
                    return;
                }
                instanceConfig.accessPermissionSession.insert(findResult->second.sessionTable.begin(),
                                                              findResult->second.sessionTable.end());
                instanceConfig.accessPermissionSecurityLevel.insert(findResult->second.securityLevelTable.begin(),
                                                                    findResult->second.securityLevelTable.end());
                instanceConfig.accessPermissionEnvCondition = findResult->second.evCondition;
                instanceConfig.accessPermissionAuth         = findResult->second.enableAuth;
                diagnosticDataManagement_.service.serviceX22->table.push_back(instanceConfig);
                std::uint16_t clientInstanceId{};
                std::uint32_t serviceInstanceId{};
                ara::core::String serviceClass;
                if (instanceNode.Load(std::move(ara::core::StringView("clientInstanceId")), clientInstanceId)
                        != isoft::kSuccess
                    || instanceNode.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceInstanceId)
                           != isoft::kSuccess
                    || instanceNode.Load(std::move(ara::core::StringView("diagnosticInterfaceClass")), serviceClass)
                           != isoft::kSuccess) {
                    common::LogWarn() << "UdsConfig::_initializeServiceX22| check "
                                         "clientInstanceId,serviceInstanceId! ";
                }
                auto findModelResult = std::find_if(diagnosticDataManagement_.didManager.staticData.begin(),
                                                    diagnosticDataManagement_.didManager.staticData.end(),
                                                    [&](isoft::uds::server::DiagnosticDataModel &model) {
                                                        if (model.id == instanceConfig.id) {
                                                            return true;
                                                        }
                                                        return false;
                                                    });
                if (findModelResult != diagnosticDataManagement_.didManager.staticData.end()) {
                    if (serviceClass == "DIAGNOSTIC-DATA-IDENTIFIER-GENERIC-INTERFACE"
                        || serviceClass == "DIAGNOSTIC-GENERIC-UDS-INTERFACE"
                        || serviceClass == "DIAGNOSTIC-DATA-IDENTIFIER-INTERFACE") {
                        findModelResult->readType = isoft::uds::server::DiagnosticDataReadMethod::kUseReadMothod;
                        for (auto &&dataParamter : findModelResult->details) {
                            dataParamter.element.provider = isoft::uds::server::DiagnosticDataElementProvider::kUnused;
                        }
                    } else {
                        findModelResult->readType = isoft::uds::server::DiagnosticDataReadMethod::kUseReadElementMothod;
                    }
                }
                genericDataIdentifierPtr_->RegisterServiceInstance(instanceConfig.id, clientInstanceId,
                                                                   serviceInstanceId, serviceClass);
                return;
            }
            LogWarn() << "UdsConfig::_initializeServiceX22| Load service x22 "
                         "fails! Please check config value!";
            return;
        }};
    if (diagnosticDataManagement_.service.serviceX2C.get() != nullptr) {
        diagnosticDataManagement_.service.serviceX22->checkPerSourceId
            = diagnosticDataManagement_.service.serviceX2C->checkPerSourceId;
    }
    diagnosticDataManagement_.didManager.dataInterfacePtr = genericDataIdentifierPtr_;
    diagnosticDataManagement_.service.serviceX22->p4ServerMax
        = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    return node.IterateArray("instance", instanceLoader);
}

int UdsConfig::_initializeServiceX27(isoft::manifestreader::ManifestNode const &node) noexcept
{
    if (node.Load(std::move(ara::core::StringView("sharedTimer")), securityAccessManagement_.sharedTimer)
        != isoft::kSuccess) {
        securityAccessManagement_.sharedTimer = false;
    }
    securityAccessManagement_.securityDelayTimeOnBoot = serverSetting_.securityDelayTimeOnBoot;
    std::function< void(std::size_t idx, isoft::manifestreader::ManifestNode const &) > instanceLoader{
        [this](std::size_t idx, isoft::manifestreader::ManifestNode const &instanceNode) {
            isoft::uds::server::SecurityAccessInstanceConfig instanceConfig;
            if (instanceNode.Load(std::move(ara::core::StringView("requestSeedId")), instanceConfig.requestSeedId)
                != 0) {
                common::LogWarn() << "UdsConfig::_initializeServiceX27|requestSeedId of node  " << idx;
                return;
            }
            if (instanceNode.Load(std::move(ara::core::StringView("securityLevel")), instanceConfig.securityLevel.id)
                != 0) {
                common::LogWarn() << "UdsConfig::_initializeServiceX27|securityLevel of node " << idx;
                return;
            }
            decltype(auto) findSecurityLevel
                = securityAccessManagement_.securityLevelTable.find(instanceConfig.securityLevel);
            if (findSecurityLevel == securityAccessManagement_.securityLevelTable.end()) {
                common::LogError() << "UdsConfig::_initializeServiceX27| "
                                      "securitylevel config not exist! id: "
                                   << instanceConfig.securityLevel.id;
                return;
            }
            instanceConfig.securityLevel = *findSecurityLevel;
            std::uint32_t accessPermissionId{};
            if (isoft::kSuccess == instanceNode.Load(ara::core::StringView("accessPermission"), accessPermissionId)) {
                decltype(auto) findResult{accessPermissionTable_.find(accessPermissionId)};
                if (findResult == accessPermissionTable_.end()) {
                    LogWarn() << "UdsConfig::_initializeServiceX27| load "
                                 "accessPermission fails!, please check config file!";
                    return;
                }
                instanceConfig.accessPermissionSession.insert(findResult->second.sessionTable.begin(),
                                                              findResult->second.sessionTable.end());
                instanceConfig.accessPermissionSecurityLevel.insert(findResult->second.securityLevelTable.begin(),
                                                                    findResult->second.securityLevelTable.end());
                instanceConfig.accessPermissionAuth = findResult->second.enableAuth;
                securityAccessManagement_.securityAccessInstanceTable.insert(instanceConfig);
            }

            return;
        }};

    securityAccessManagement_.p4ServerMax = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    return node.IterateArray("instance", instanceLoader);
}

int UdsConfig::_initializeServiceX28(isoft::manifestreader::ManifestNode const &node) noexcept
{
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > instanceLoader{
        [this](std::size_t idx, isoft::manifestreader::ManifestNode const &instanceNode) {
            ara::core::String category;
            std::ignore = instanceNode.Load(std::move(ara::core::StringView("category")), category);
            /// [TPS_DEXT_01057]
            isoft::uds::server::ComControlInstanceConfig instanceConfig;
            if (category == "ENABLE_RX_AND_TX") {
                instanceConfig.subfunctionNumber = isoft::uds::server::kEnableRxAndTx;
            } else if (category == "ENABLE_RX_AND_DISABLE_TX") {
                instanceConfig.subfunctionNumber = isoft::uds::server::kEnableRxAndDisableTx;
            } else if (category == "DISABLE_RX_AND_ENABLE_TX") {
                instanceConfig.subfunctionNumber = isoft::uds::server::kDisableRxAndEnableTx;
            } else if (category == "DISABLE_RX_AND_TX") {
                instanceConfig.subfunctionNumber = isoft::uds::server::kDisableRxAndTx;
            } else if (category == "ENABLE_RX_AND_DISABLE_TX_WITH_ENHANCED_ADDRESS_"
                               "INFORMATION") {
                instanceConfig.subfunctionNumber
                    = isoft::uds::server::kEnableRxAndDisableTxWithEnhancedAddressInformation;
            } else if (category == "ENABLE_RX_AND_TX_WITH_ENHANCED_ADDRESS_INFORMATION") {
                instanceConfig.subfunctionNumber = isoft::uds::server::kEnableRxAndTxWithEnhancedAddressInformation;
            } else {
                if (instanceNode.Load(std::move(ara::core::StringView("customSubFunctionNumber")),
                                      instanceConfig.subfunctionNumber)
                    != 0) {
                    common::LogWarn() << "UdsConfig::_initializeServiceX28|failed to parse node|" << idx;
                    return;
                }
            }
            std::uint16_t clientInstanceId{};
            std::uint32_t serviceInstanceId{};
            if (instanceNode.Load(std::move(ara::core::StringView("clientInstanceId")), clientInstanceId)
                    != isoft::kSuccess
                || instanceNode.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceInstanceId)
                       != isoft::kSuccess) {
                common::LogWarn() << "UdsConfig::_initializeServiceX28| check "
                                     "clientInstanceId,serviceInstanceId! ";
                return;
            }
            comManagementConfig_.interface = std::make_shared< CommunicationControl >(clientInstanceId,
                                                                                      serviceInstanceId);
            std::uint32_t accessPermissionId{};
            if (isoft::kSuccess == instanceNode.Load(ara::core::StringView("accessPermission"), accessPermissionId)) {
                decltype(auto) findResult{accessPermissionTable_.find(accessPermissionId)};
                if (findResult == accessPermissionTable_.end()) {
                    LogWarn() << "UdsConfig::_initializeServiceX28| load "
                                 "accessPermission fails!, please check config file!";
                    return;
                }
                instanceConfig.accessPermissionSession.insert(findResult->second.sessionTable.begin(),
                                                              findResult->second.sessionTable.end());
                instanceConfig.accessPermissionSecurityLevel.insert(findResult->second.securityLevelTable.begin(),
                                                                    findResult->second.securityLevelTable.end());
                instanceConfig.accessPermissionAuth = findResult->second.enableAuth;
                comManagementConfig_.instanceTable.insert(instanceConfig);
                return;
            }
            LogWarn() << "UdsConfig::_initializeServiceX28| Load service x28 "
                         "fails! Please check config value!";
            return;
        }};

    comManagementConfig_.p4ServerMax = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    return node.IterateArray("instance", instanceLoader);
}

int UdsConfig::_initializeServiceX29(isoft::manifestreader::ManifestNode const &node) noexcept
{
    std::uint16_t clientInstanceId{};
    std::uint32_t serviceInstanceId{};
    if (node.Load(std::move(ara::core::StringView("clientInstanceId")), clientInstanceId) != isoft::kSuccess
        || node.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceInstanceId) != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX29| check "
                             "clientInstanceId,serviceInstanceId! ";
        return __LINE__;
    }
    authenticationManagement_.interface = std::make_shared< Authentication >(clientInstanceId, serviceInstanceId);
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > instanceLoader{
        [this](std::size_t, isoft::manifestreader::ManifestNode const &instanceNode) {
            ara::core::String function{};
            isoft::uds::server::AuthTransmitCertificateEvaluation certificateEvaluation;
            if (instanceNode.Load(std::move(ara::core::StringView("evaluationId")),
                                  certificateEvaluation.certificateEvaluationId)
                    != isoft::kSuccess
                || instanceNode.Load(std::move(ara::core::StringView("function")), function) != isoft::kSuccess) {
                common::LogWarn() << "UdsConfig::_initializeServiceX29| check "
                                     "evaluationId,function! ";
                return;
            }
            certificateEvaluation.function = function.c_str();
            this->authenticationManagement_.transmitCertificate.certificateEvaluation.push_back(certificateEvaluation);
        }};
    return node.IterateArray("diagnosticAuthTransmitCertificate", instanceLoader);
}

int UdsConfig::_initializeServiceX2A(isoft::manifestreader::ManifestNode const &node) noexcept
{
    if (diagnosticDataManagement_.service.serviceX2A.get() == nullptr) {
        diagnosticDataManagement_.service.serviceX2A = std::make_shared< isoft::uds::server::ServiceX2AModel >();
    }
    std::uint32_t maxPeriodicDidToRead{};
    if (node.Load(std::move(ara::core::StringView("maxPeriodicDidToRead")), maxPeriodicDidToRead) != 0) {
        common::LogWarn() << "UdsConfig::_initializeServiceX2A|failed to load "
                             "maxPeriodicDidToRead";
        return __LINE__;
    }
    diagnosticDataManagement_.service.serviceX2A->maxPeriodicDidToRead = maxPeriodicDidToRead;
    std::uint32_t schedulerMaxNumber{};
    if (node.Load(std::move(ara::core::StringView("schedulerMaxNumber")), schedulerMaxNumber) != 0) {
        common::LogWarn() << "UdsConfig::_initializeServiceX2A|failed to load schedulerMaxNumber";
        return __LINE__;
    }
    diagnosticDataManagement_.service.serviceX2A->schedulerMaxNumber = schedulerMaxNumber;
    std::function< void(std::size_t const, isoft::manifestreader::ManifestNode const &) > periodicRateLoader{
        [&](std::size_t const idx, isoft::manifestreader::ManifestNode const &prNode) {
            isoft::uds::server::DiagnosticPeriodicRate rate{};
            std::int32_t const res{prNode.Load(std::move(ara::core::StringView("period")), rate.timeout)};
            if (res != 0) {
                common::LogWarn() << "UdsConfig::_initializeServiceX2A|failed to "
                                     "load period of node"
                                  << idx;
                return;
            }
            ///
            /// @brief Category
            ///
            ///
            using Category = isoft::manifestreader::tps::DiagnosticPeriodicRateCategoryEnum;
            Category category;
            if (prNode.Load(std::move(ara::core::StringView("periodicRateCategory")), category) != 0) {
                common::LogWarn() << "UdsConfig::_initializeServiceX2A|failed to "
                                     "load period of periodicRateCategory"
                                  << idx;
                return;
            }

            if (category == Category::kPeriodicRateSlow) {
                rate.type = isoft::uds::server::DiagnosticPeriodicRateCategoryEnum::kPeriodicRateSlow;
            } else if (category == Category::kPeriodicRateMedium) {
                rate.type = isoft::uds::server::DiagnosticPeriodicRateCategoryEnum::kPeriodicRateMedium;
            } else if (category == Category::kPeriodicRateFast) {
                rate.type = isoft::uds::server::DiagnosticPeriodicRateCategoryEnum::kPeriodicRateFast;
            } else {
                common::LogWarn() << "UdsConfig::_initializeServiceX2A|failed to "
                                     "load category of node"
                                  << idx;
                return;
            }
            diagnosticDataManagement_.service.serviceX2A->rates.push_back(rate);
        }};
    if (node.IterateArray(std::move(ara::core::StringView("periodicRate")), periodicRateLoader) != 0) {
        common::LogWarn() << "UdsConfig::_initializeServiceX2A|failed to load periodicRate";
        return __LINE__;
    }
    if (diagnosticDataManagement_.service.serviceX2C.get() != nullptr) {
        diagnosticDataManagement_.service.serviceX2A->checkPerSourceId
            = diagnosticDataManagement_.service.serviceX2C->checkPerSourceId;

        diagnosticDataManagement_.service.serviceX2A->p4ServerMax
            = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    }
    return 0;
}

int UdsConfig::_initializeServiceX2C(isoft::manifestreader::ManifestNode const &node) noexcept
{
    if (diagnosticDataManagement_.service.serviceX2C.get() == nullptr) {
        diagnosticDataManagement_.service.serviceX2C = std::make_shared< isoft::uds::server::ServiceX2CModel >();
    }
    bool checkPerSourceId{false};
    if (node.Load(std::move(ara::core::StringView("checkPerSourceId")), checkPerSourceId) != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX2C| failed to load checkPerSourceId";
        checkPerSourceId = false;
    }
    diagnosticDataManagement_.service.serviceX2C->checkPerSourceId = checkPerSourceId;
    if (diagnosticDataManagement_.service.serviceX2A.get() != nullptr) {
        diagnosticDataManagement_.service.serviceX2A->checkPerSourceId = checkPerSourceId;
    }
    if (diagnosticDataManagement_.service.serviceX22.get() != nullptr) {
        diagnosticDataManagement_.service.serviceX22->checkPerSourceId = checkPerSourceId;
    }

    ara::core::String storageValue{};
    bool storage{false};
    if (node.Load(std::move(ara::core::StringView("configurationHandling")), storageValue) != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX2C| failed to load "
                             "configurationHandling";
    }
    if (storageValue == ara::core::StringView("NON-VOLATILE")) {
        storage = true;
    }
    diagnosticDataManagement_.service.serviceX2C->stored       = storage;
    diagnosticDataManagement_.didManager.storeDynamicDataModel = storage;

    ara::core::Vector< ara::core::String > subfunctionTable;
    if (node.Load(std::move(ara::core::StringView("subfunction")), subfunctionTable) != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX2C| failed to load subfunction";
    }
    for (auto &&subfunction : subfunctionTable) {
        if (subfunction == ara::core::StringView("DEFINE-BY-IDENTIFIER")) {
            diagnosticDataManagement_.service.serviceX2C->subfunction.insert(
                isoft::uds::server::DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::kDefineByIdentifier);
        } else if (subfunction == ara::core::StringView("CLEAR-DYNAMICALLY-DEFINE-DATA-IDENTIFIER")) {
            diagnosticDataManagement_.service.serviceX2C->subfunction.insert(
                isoft::uds::server::DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum::
                    kClearDynamicallyDefineDataIdentifier);
        }
    }

    std::function< void(std::size_t const, isoft::manifestreader::ManifestNode const &) > const loader{
        [&](std::size_t const, isoft::manifestreader::ManifestNode const &instanceNode) {
            isoft::uds::server::DynamicDefinedDiagnosticData instance;
            if (instanceNode.Load(std::move(ara::core::StringView("maxSourceElement")), instance.maxSourceElement)
                != isoft::kSuccess) {
                LogWarn() << "UdsConfig::_initializeServiceX2C| load "
                             "maxSourceElement fails!, please check config file!";
                return;
            }

            if (instanceNode.Load(std::move(ara::core::StringView("dataIdentifier")), instance.id) != isoft::kSuccess) {
                LogWarn() << "UdsConfig::_initializeServiceX2C| load dataIdentifier "
                             "fails!, please check config file!";
                return;
            }
            std::uint32_t accessPermissionId{};
            if (isoft::kSuccess == instanceNode.Load(ara::core::StringView("accessPermission"), accessPermissionId)) {
                decltype(auto) findResult{accessPermissionTable_.find(accessPermissionId)};
                if (findResult == accessPermissionTable_.end()) {
                    LogWarn() << "UdsConfig::_initializeServiceX2C| load "
                                 "accessPermission fails!, please check config file!";
                    return;
                }
                instance.accessPermissionSession.insert(findResult->second.sessionTable.begin(),
                                                        findResult->second.sessionTable.end());
                instance.accessPermissionSecurityLevel.insert(findResult->second.securityLevelTable.begin(),
                                                              findResult->second.securityLevelTable.end());
                instance.accessPermissionAuth = findResult->second.enableAuth;
                diagnosticDataManagement_.service.serviceX2C->table.push_back(instance);
                return;
            }
        }};
    if (node.IterateArray(std::move(ara::core::StringView("instance")), loader) != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX2C|failed to load instance";
        return __LINE__;
    }

    diagnosticDataManagement_.service.serviceX2C->p4ServerMax
        = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    return 0;
}

int UdsConfig::_initializeServiceX2E(isoft::manifestreader::ManifestNode const &node) noexcept
{
    if (diagnosticDataManagement_.service.serviceX2E.get() == nullptr) {
        diagnosticDataManagement_.service.serviceX2E = std::make_shared< isoft::uds::server::ServiceX2EModel >();
    }

    std::function< void(std::size_t const, isoft::manifestreader::ManifestNode const &) > const loader{
        [&](std::size_t const, isoft::manifestreader::ManifestNode const &instanceNode) {
            isoft::uds::server::WriteDiagnosticDataByIdentifier instance;

            if (instanceNode.Load(std::move(ara::core::StringView("dataIdentifier")), instance.id) != isoft::kSuccess) {
                LogWarn() << "UdsConfig::_initializeServiceX2E| load dataIdentifier "
                             "fails!, please check config file!";
                return;
            }

            auto findModelResult = std::find_if(diagnosticDataManagement_.didManager.staticData.begin(),
                                                diagnosticDataManagement_.didManager.staticData.end(),
                                                [&](isoft::uds::server::DiagnosticDataModel &model) {
                                                    if (model.id == instance.id) {
                                                        return true;
                                                        ;
                                                    }
                                                    return false;
                                                });
            if (findModelResult != diagnosticDataManagement_.didManager.staticData.end()) {
                instance.dataSize = findModelResult->nSize;
                ara::core::String diagnosticInterfaceClass;
                if (instanceNode.Load(std::move(ara::core::StringView("diagnosticInterfaceClass")),
                                      diagnosticInterfaceClass)
                    != isoft::kSuccess) {
                    LogWarn() << "UdsConfig::_initializeServiceX2E| load "
                                 "diagnosticInterfaceClass fails!, please check "
                                 "config file!";
                    return;
                }
                if (diagnosticInterfaceClass == "DIAGNOSTIC-DATA-IDENTIFIER-GENERIC-INTERFACE") {
                    findModelResult->readType = isoft::uds::server::DiagnosticDataReadMethod::kUseReadMothod;
                }
            }
            std::uint32_t accessPermissionId{};
            if (isoft::kSuccess == instanceNode.Load(ara::core::StringView("accessPermission"), accessPermissionId)) {
                decltype(auto) findResult{accessPermissionTable_.find(accessPermissionId)};
                if (findResult == accessPermissionTable_.end()) {
                    LogWarn() << "UdsConfig::_initializeServiceX2E| load "
                                 "accessPermission fails!, please check config file!";
                    return;
                }
                instance.accessPermissionSession.insert(findResult->second.sessionTable.begin(),
                                                        findResult->second.sessionTable.end());
                instance.accessPermissionSecurityLevel.insert(findResult->second.securityLevelTable.begin(),
                                                              findResult->second.securityLevelTable.end());
                instance.accessPermissionEnvCondition = findResult->second.evCondition;
                instance.accessPermissionAuth         = findResult->second.enableAuth;
                diagnosticDataManagement_.service.serviceX2E->table.push_back(instance);
                return;
            }
        }};
    if (node.IterateArray(std::move(ara::core::StringView("instance")), loader) != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX2E|failed to load instance";
        return __LINE__;
    }

    diagnosticDataManagement_.service.serviceX2E->p4ServerMax
        = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    return 0;
}

int UdsConfig::_initializeServiceX31(isoft::manifestreader::ManifestNode const &node,
                                     std::unique_ptr< isoft::manifestreader::Manifest > const &dmc) noexcept
{
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > loadRoutineInfo{
        [&](std::size_t, isoft::manifestreader::ManifestNode const &routineInfoNode) {
            RoutineInfo routine;
            if (isoft::kSuccess != routineInfoNode.Load("id", routine.routineId)) {
                LogError() << "UdsConfig::_initializeServiceX31| failed to load routine.id";
                return;
            }
            if (isoft::kSuccess
                != routineInfoNode.Load(ara::core::StringView("start.accessPermission"),
                                        routine.startAccessPermissionId)) {
                LogError() << "UdsConfig::_initializeServiceX31| load "
                              "start.accessPermission fails!, please check "
                              "config file!";
                return;
            }

            if (isoft::kSuccess
                != routineInfoNode.Load(ara::core::StringView("start.p4ServerMax"), routine.p4ServerMaxStart)) {
                LogInfo() << "UdsConfig::_initializeServiceX31| load "
                             "start.p4ServerMaxStart fails!, please check "
                             "config file!";
            }

            if (isoft::kSuccess
                != routineInfoNode.Load(ara::core::StringView("stop.accessPermission"),
                                        routine.stopAccessPermissionId)) {
                routine.stopAccessPermissionId = -1;
                LogInfo() << "UdsConfig::_initializeServiceX31| load "
                             "stop.accessPermission fails!, please check "
                             "config file!";
            }

            if (isoft::kSuccess
                != routineInfoNode.Load(ara::core::StringView("stop.p4ServerMax"), routine.p4ServerMaxStop)) {
                routine.p4ServerMaxStop = 0U;
                LogInfo() << "UdsConfig::_initializeServiceX31| load "
                             "stop.p4ServerMax fails!, please check "
                             "config file!";
            }

            if (isoft::kSuccess
                != routineInfoNode.Load(ara::core::StringView("requestResult.accessPermission"),
                                        routine.getResultAccessPermissionId)) {
                routine.getResultAccessPermissionId = -1;
                LogInfo() << "UdsConfig::_initializeServiceX31| load "
                             "requestResult.accessPermission fails!, please check "
                             "config file!";
            }

            if (isoft::kSuccess
                != routineInfoNode.Load(ara::core::StringView("requestResult.p4ServerMax"),
                                        routine.p4ServerMaxResult)) {
                routine.p4ServerMaxResult = 0U;
                LogInfo() << "UdsConfig::_initializeServiceX31| load "
                             "requestResult.p4ServerMax fails!, please check "
                             "config file!";
            }

            if (isoft::kSuccess != routineInfoNode.Load("routineInfo", routine.routineInfo)) {
                routine.routineInfo = -1;
                LogInfo() << "UdsConfig::_initializeServiceX31| failed to load "
                             "routine.routineInfo";
            }

            if (isoft::kSuccess != routineInfoNode.Load("p4ServerMax", routine.p4ServerMax)) {
                routine.p4ServerMax = 0U;
                LogInfo() << "UdsConfig::_initializeServiceX31| failed to load "
                             "routine.p4ServerMax";
            }

            mapRoutineInfo_[routine.routineId] = routine;
        }};

    if (mapRoutineInfo_.empty()) {
        if (dmc->IterateArray(std::move(ara::core::StringView("routine")), loadRoutineInfo) != isoft::kSuccess) {
            common::LogWarn() << "UdsConfig::_initializeServiceX31|failed to load routineinfo";
            return __LINE__;
        }
    }

    std::shared_ptr< GenericRoutine > instancePtr{std::make_shared< GenericRoutine >()};
    std::function< void(std::size_t const, isoft::manifestreader::ManifestNode const &) > const loader{
        [&](std::size_t const, isoft::manifestreader::ManifestNode const &instanceNode) {
            isoft::uds::server::RoutineControlInstanceConfig instance;

            if (instanceNode.Load(std::move(ara::core::StringView("routine")), instance.routine.id)
                != isoft::kSuccess) {
                LogWarn() << "UdsConfig::_initializeServiceX31| load routine fails!, "
                             "please check config file!";
                return;
            }

            std::map< uint16_t, RoutineInfo >::iterator it = mapRoutineInfo_.find(instance.routine.id);
            if (it == mapRoutineInfo_.end()) {
                LogError() << "UdsConfig::_initializeServiceX31| find routine "
                              "fails!, please check config file! routine_id ="
                           << instance.routine.id;
                return;
            }

            instance.routine.routineInfo = it->second.routineInfo;

            instance.routine.p4ServerMax       = it->second.p4ServerMax;
            instance.routine.startP4ServerMax  = it->second.p4ServerMaxStart;
            instance.routine.stopP4ServerMax   = it->second.p4ServerMaxStop;
            instance.routine.resultP4ServerMax = it->second.p4ServerMaxResult;

            std::map< std::uint32_t, AccessPermission >::iterator findResult{
                accessPermissionTable_.find(it->second.startAccessPermissionId)};
            if (findResult == accessPermissionTable_.end()) {
                LogWarn() << "UdsConfig::_initializeServiceX31| load "
                             "accessPermission fails!, please check config file!";
                return;
            }
            instance.sessionPermission.insert(findResult->second.sessionTable.begin(),
                                              findResult->second.sessionTable.end());
            instance.securityLevelPermission.insert(findResult->second.securityLevelTable.begin(),
                                                    findResult->second.securityLevelTable.end());

            instance.accessPermissionAuth                      = findResult->second.enableAuth;
            instance.routine.startAccessPermissionEnvCondition = findResult->second.evCondition;

            findResult = accessPermissionTable_.find(it->second.stopAccessPermissionId);
            if (findResult != accessPermissionTable_.end()) {
                instance.routine.stopAccessPermissionEnvCondition = findResult->second.evCondition;
            }

            findResult = accessPermissionTable_.find(it->second.getResultAccessPermissionId);
            if (findResult != accessPermissionTable_.end()) {
                instance.routine.getResultAccessPermissionEnvCondition = findResult->second.evCondition;
            }

            std::uint16_t clientInstanceId{};
            std::uint32_t serviceInstanceId{};
            ara::core::String className;
            if (instanceNode.Load(std::move(ara::core::StringView("clientInstanceId")), clientInstanceId)
                    != isoft::kSuccess
                || instanceNode.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceInstanceId)
                       != isoft::kSuccess
                || instanceNode.Load(std::move(ara::core::StringView("diagnosticInterfaceClass")), className)
                       != isoft::kSuccess) {
                common::LogWarn() << "UdsConfig::_initializeServiceX31| check "
                                     "clientInstanceId,serviceInstanceId! ";
                return;
            }
            instancePtr->RegisterRoutineInstance(instance.routine.id, clientInstanceId, serviceInstanceId, className);
            routineManagement_.routineInstanceTable.insert(instance);
        }};
    if (node.IterateArray(std::move(ara::core::StringView("instance")), loader) != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX31|failed to load instance";
        return __LINE__;
    }
    routineManagement_.interfacePtr = instancePtr;
    return 0;
}  // namespace dmd

int UdsConfig::_initializeServiceX34(isoft::manifestreader::ManifestNode const &node) noexcept
{
    if (transferManagment_.requestDownload.get() == nullptr) {
        transferManagment_.requestDownload = std::make_shared< isoft::uds::server::RequestDownload >();
    }
    std::function< void(std::size_t const, isoft::manifestreader::ManifestNode const &) > const loader{
        [&](std::size_t const, isoft::manifestreader::ManifestNode const &instanceNode) {
            isoft::uds::server::RequestDownload &instance{*transferManagment_.requestDownload};
            if (instanceNode.Load(std::move(ara::core::StringView("maxNumberOfBlockLength")),
                                  instance.maxNumberOfBlockLength)
                != isoft::kSuccess) {
                LogWarn() << "UdsConfig::_initializeServiceX34| load "
                             "maxNumberOfBlockLength fails!, please check "
                             "config file!";
                return;
            }
            std::uint32_t accessPermissionId{};
            if (isoft::kSuccess == instanceNode.Load(ara::core::StringView("accessPermission"), accessPermissionId)) {
                decltype(auto) findResult{accessPermissionTable_.find(accessPermissionId)};
                if (findResult == accessPermissionTable_.end()) {
                    LogWarn() << "UdsConfig::_initializeServiceX34| load "
                                 "accessPermission fails!, please check config file!";
                    return;
                }
                instance.accessPermissionSession.insert(findResult->second.sessionTable.begin(),
                                                        findResult->second.sessionTable.end());
                instance.accessPermissionSecurityLevel.insert(findResult->second.securityLevelTable.begin(),
                                                              findResult->second.securityLevelTable.end());
                instance.accessPermissionAuth         = findResult->second.enableAuth;
                instance.accessPermissionEnvCondition = findResult->second.evCondition;
            }
            std::uint16_t clientInstanceId{};
            std::uint32_t serviceInstanceId{};
            if (instanceNode.Load(std::move(ara::core::StringView("clientInstanceId")), clientInstanceId)
                    != isoft::kSuccess
                || instanceNode.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceInstanceId)
                       != isoft::kSuccess) {
                common::LogWarn() << "UdsConfig::_initializeServiceX34| check "
                                     "clientInstanceId,serviceInstanceId! ";
                return;
            }
            instance.interfacePtr = std::make_shared< Download >(clientInstanceId, serviceInstanceId);
        }};
    if (node.IterateArray(std::move(ara::core::StringView("instance")), loader) != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX34|failed to load instance";
        return __LINE__;
    }

    transferManagment_.requestDownload->p4ServerMax
        = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    transferManagment_.requestDownload->p4ServerMax_0x36
        = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax_0x36")));
    transferManagment_.requestDownload->p4ServerMax_0x37
        = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax_0x37")));
    return 0;
}

int UdsConfig::_initializeServiceX35(isoft::manifestreader::ManifestNode const &node) noexcept
{
    if (transferManagment_.requestUpload.get() == nullptr) {
        transferManagment_.requestUpload = std::make_shared< isoft::uds::server::RequestUpload >();
    }
    std::function< void(std::size_t const, isoft::manifestreader::ManifestNode const &) > const loader{
        [&](std::size_t const, isoft::manifestreader::ManifestNode const &instanceNode) {
            isoft::uds::server::RequestUpload &instance{*transferManagment_.requestUpload};
            if (instanceNode.Load(std::move(ara::core::StringView("maxNumberOfBlockLength")),
                                  instance.maxNumberOfBlockLength)
                != isoft::kSuccess) {
                LogWarn() << "UdsConfig::_initializeServiceX35| load "
                             "maxNumberOfBlockLength fails!, please check "
                             "config file!";
                return;
            }
            std::uint32_t accessPermissionId{};
            if (isoft::kSuccess == instanceNode.Load(ara::core::StringView("accessPermission"), accessPermissionId)) {
                decltype(auto) findResult{accessPermissionTable_.find(accessPermissionId)};
                if (findResult == accessPermissionTable_.end()) {
                    LogWarn() << "UdsConfig::_initializeServiceX35| load "
                                 "accessPermission fails!, please check config file!";
                    return;
                }
                instance.accessPermissionSession.insert(findResult->second.sessionTable.begin(),
                                                        findResult->second.sessionTable.end());
                instance.accessPermissionSecurityLevel.insert(findResult->second.securityLevelTable.begin(),
                                                              findResult->second.securityLevelTable.end());
                instance.accessPermissionAuth         = findResult->second.enableAuth;
                instance.accessPermissionEnvCondition = findResult->second.evCondition;
            }
            std::uint16_t clientInstanceId{};
            std::uint32_t serviceInstanceId{};
            if (instanceNode.Load(std::move(ara::core::StringView("clientInstanceId")), clientInstanceId)
                    != isoft::kSuccess
                || instanceNode.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceInstanceId)
                       != isoft::kSuccess) {
                common::LogWarn() << "UdsConfig::_initializeServiceX35| check "
                                     "clientInstanceId,serviceInstanceId! ";
                return;
            }
            instance.interfacePtr = std::make_shared< Upload >(clientInstanceId, serviceInstanceId);
        }};
    if (node.IterateArray(std::move(ara::core::StringView("instance")), loader) != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX35|failed to load instance";
        return __LINE__;
    }

    transferManagment_.requestUpload->p4ServerMax
        = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    transferManagment_.requestUpload->p4ServerMax_0x36
        = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax_0x36")));
    transferManagment_.requestUpload->p4ServerMax_0x37
        = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax_0x37")));
    return 0;
}

int UdsConfig::_initializeServiceX38(isoft::manifestreader::ManifestNode const &node) noexcept
{
    if (transferManagment_.requestFileTransfer.get() == nullptr) {
        transferManagment_.requestFileTransfer = std::make_shared< isoft::uds::server::RequestFileTransfer >();
    }
    std::function< void(std::size_t const, isoft::manifestreader::ManifestNode const &) > const loader{
        [&](std::size_t const, isoft::manifestreader::ManifestNode const &instanceNode) {
            isoft::uds::server::RequestFileTransfer &instance{*transferManagment_.requestFileTransfer};
            if (instanceNode.Load(std::move(ara::core::StringView("maxNumberOfBlockLength")),
                                  instance.maxNumberOfBlockLength)
                != isoft::kSuccess) {
                LogWarn() << "UdsConfig::_initializeServiceX38| load "
                             "maxNumberOfBlockLength fails!, please check "
                             "config file!";
                return;
            }
            std::uint32_t accessPermissionId{};
            if (isoft::kSuccess == instanceNode.Load(ara::core::StringView("accessPermission"), accessPermissionId)) {
                decltype(auto) findResult{accessPermissionTable_.find(accessPermissionId)};
                if (findResult == accessPermissionTable_.end()) {
                    LogWarn() << "UdsConfig::_initializeServiceX38| load "
                                 "accessPermission fails!, please check config file!";
                    return;
                }
                instance.accessPermissionSession.insert(findResult->second.sessionTable.begin(),
                                                        findResult->second.sessionTable.end());
                instance.accessPermissionSecurityLevel.insert(findResult->second.securityLevelTable.begin(),
                                                              findResult->second.securityLevelTable.end());
                instance.accessPermissionAuth         = findResult->second.enableAuth;
                instance.accessPermissionEnvCondition = findResult->second.evCondition;
            }
            std::uint16_t clientInstanceId{};
            std::uint32_t serviceInstanceId{};
            if (instanceNode.Load(std::move(ara::core::StringView("clientInstanceId")), clientInstanceId)
                    != isoft::kSuccess
                || instanceNode.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceInstanceId)
                       != isoft::kSuccess) {
                common::LogWarn() << "UdsConfig::_initializeServiceX38| check "
                                     "clientInstanceId,serviceInstanceId! ";
                return;
            }
            instance.interfacePtr = std::make_shared< FileTransfer >(clientInstanceId, serviceInstanceId);
        }};
    if (node.IterateArray(std::move(ara::core::StringView("instance")), loader) != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX38|failed to load instance";
        return __LINE__;
    }

    transferManagment_.requestFileTransfer->p4ServerMax
        = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    transferManagment_.requestFileTransfer->p4ServerMax_0x36
        = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax_0x36")));
    transferManagment_.requestFileTransfer->p4ServerMax_0x37
        = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax_0x37")));
    return 0;
}

int UdsConfig::_initializeServiceX86(isoft::manifestreader::ManifestNode const &node) noexcept
{
    if (node.Load(std::move(ara::core::StringView("storeEventSupport")), roeManagement_.storeEventEnabled)
        != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX86| failed to load storeEventSupport";
    }
    if (node.Load(std::move(ara::core::StringView("responseOnEventSchedulerRate")),
                  roeManagement_.responseOnEventSchedulerRate)
        != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX86| failed to load "
                             "responseOnEventSchedulerRate";
    }
    if (node.Load(std::move(ara::core::StringView("maxNumberOfStoredDTCStatusChangedEvents")),
                  roeManagement_.maxNumberOfStoredDTCStatusChangedEvents)
        != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX86| failed to load "
                             "maxNumberOfStoredDTCStatusChangedEvents";
    }
    if (node.Load(std::move(ara::core::StringView("maxNumChangeOfDataIdentfierEvents")),
                  roeManagement_.maxNumChangeOfDataIdentfierEvents)
        != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX86| failed to load "
                             "maxNumChangeOfDataIdentfierEvents";
    }
    if (node.Load(std::move(ara::core::StringView("maxNumComparisionOfValueEvents")),
                  roeManagement_.maxNumComparisionOfValueEvents)
        != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX86| failed to load "
                             "maxNumComparisionOfValueEvents";
    }
    if (node.Load(std::move(ara::core::StringView("maxSupportedDIDLength")), roeManagement_.maxSupportedDIDLength)
        != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX86| failed to load "
                             "maxSupportedDIDLength";
    }
    if (node.Load(std::move(ara::core::StringView("interMessageTime")), roeManagement_.interMessageTime)
        != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX86| failed to load interMessageTime";
    }
    std::function< void(std::size_t const, isoft::manifestreader::ManifestNode const &) > const loader{
        [&](std::size_t const, isoft::manifestreader::ManifestNode const &instanceNode) {
            isoft::uds::server::ResponseOnEventInstanceConfig instance;
            ara::core::String strCategoryValue{};
            if (instanceNode.Load(std::move(ara::core::StringView("category")), strCategoryValue) != isoft::kSuccess) {
                LogWarn() << "UdsConfig::_initializeServiceX86| load category fails!, "
                             "please check config file!";
                return;
            }
            if (strCategoryValue == ara::core::StringView("Start")) {
                instance.responseOnEventAction = isoft::uds::server::ResponseOnEventActionEnum::kStartResponseOnEvent;
            } else if (strCategoryValue == ara::core::StringView("Stop")) {
                instance.responseOnEventAction = isoft::uds::server::ResponseOnEventActionEnum::kStopResponseOnEvent;
            } else if (strCategoryValue == ara::core::StringView("Clear")) {
                instance.responseOnEventAction = isoft::uds::server::ResponseOnEventActionEnum::kClearResponseOnEvent;
            } else if (strCategoryValue == ara::core::StringView("Report")) {
                instance.responseOnEventAction = isoft::uds::server::ResponseOnEventActionEnum::kReportActivatedEvents;
            } else if (strCategoryValue == ara::core::StringView("OnDTCStatusChange")) {
                instance.responseOnEventAction = isoft::uds::server::ResponseOnEventActionEnum::kOnDTCStatusChange;
            } else if (strCategoryValue == ara::core::StringView("OnChangeOfDataIdentifier")) {
                instance.responseOnEventAction
                    = isoft::uds::server::ResponseOnEventActionEnum::kOnChangeOfDataIdentifier;
            } else if (strCategoryValue == ara::core::StringView("OnComparisonOfValues")) {
                instance.responseOnEventAction = isoft::uds::server::ResponseOnEventActionEnum::kOnComparisonOfValues;
            } else {
                LogWarn() << "UdsConfig::_initializeServiceX86| current service not "
                             "support subfunction:"
                          << strCategoryValue;
            }
            bool infiniteTimeToResponse{};
            if (instanceNode.Load(std::move(ara::core::StringView("infiniteTimeToResponse")), infiniteTimeToResponse)
                != isoft::kSuccess) {
                LogWarn() << "UdsConfig::_initializeServiceX86| load "
                             "infiniteTimeToResponse fails!, please check "
                             "config file!";
            }
            if (infiniteTimeToResponse) {
                instance.eventWindowTime.push_back(
                    isoft::uds::server::DiagnosticEventWindowTimeEnum::kInfiniteTimeToResponse);
            }
            bool powerWindowTime{};
            if (instanceNode.Load(std::move(ara::core::StringView("powerWindowTime")), powerWindowTime)
                != isoft::kSuccess) {
                LogWarn() << "UdsConfig::_initializeServiceX86| load powerWindowTime "
                             "fails!, please check config file!";
            }
            if (powerWindowTime) {
                instance.eventWindowTime.push_back(isoft::uds::server::DiagnosticEventWindowTimeEnum::kPowerWindowTime);
            }
            std::uint32_t accessPermissionId{};
            if (isoft::kSuccess == instanceNode.Load(ara::core::StringView("accessPermission"), accessPermissionId)) {
                decltype(auto) findResult{accessPermissionTable_.find(accessPermissionId)};
                if (findResult == accessPermissionTable_.end()) {
                    LogWarn() << "UdsConfig::_initializeServiceX86| load accessPermission "
                                 "fails!, please check config file!";
                    return;
                }
                instance.accessPermissionSession.insert(findResult->second.sessionTable.begin(),
                                                        findResult->second.sessionTable.end());
                instance.accessPermissionSecurityLevel.insert(findResult->second.securityLevelTable.begin(),
                                                              findResult->second.securityLevelTable.end());
                instance.accessPermissionAuth = findResult->second.enableAuth;
                roeManagement_.instanceTable.insert(instance);
            }
        }};
    if (node.IterateArray(std::move(ara::core::StringView("instance")), loader) != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeServiceX86|failed to load instance";
        return __LINE__;
    }

    roeManagement_.p4ServerMax = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    return 0;
}

int UdsConfig::_initializeCustomService(isoft::manifestreader::ManifestNode const &node) noexcept
{
    int result{isoft::kSuccess};
    if (genericUDSServicePtr_.get() == nullptr) {
        genericUDSServicePtr_ = std::make_shared< GenericUDSService >();
    }
    isoft::uds::server::CustomInstancePtr instancePtr{std::make_shared< isoft::uds::server::CustomInstance >()};

    if (node.Load(std::move(ara::core::StringView("customServiceId")), instancePtr->nSid) != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeCustomService| failed to load "
                             "storeEventSupport";
    }
    std::function< void(std::size_t const, isoft::manifestreader::ManifestNode const &) > const loader{
        [&](std::size_t const, isoft::manifestreader::ManifestNode const &instanceNode) {
            std::uint32_t accessPermissionId{};
            if (isoft::kSuccess == instanceNode.Load(ara::core::StringView("accessPermission"), accessPermissionId)) {
                decltype(auto) findResult{accessPermissionTable_.find(accessPermissionId)};
                if (findResult == accessPermissionTable_.end()) {
                    LogWarn() << "UdsConfig::_initializeCustomService| load "
                                 "accessPermission fails!, please check "
                                 "config file!";
                    return;
                }
                instancePtr->accessPermissionSession.insert(findResult->second.sessionTable.begin(),
                                                            findResult->second.sessionTable.end());
                instancePtr->accessPermissionSecurityLevel.insert(findResult->second.securityLevelTable.begin(),
                                                                  findResult->second.securityLevelTable.end());
                instancePtr->accessPermissionAuth = findResult->second.enableAuth;
            }
            std::uint16_t clientInstanceId{};
            std::uint32_t serviceInstanceId{};
            if (instanceNode.Load(std::move(ara::core::StringView("clientInstanceId")), clientInstanceId)
                    != isoft::kSuccess
                || instanceNode.Load(std::move(ara::core::StringView("serviceInstanceId")), serviceInstanceId)
                       != isoft::kSuccess) {
                common::LogWarn() << "UdsConfig::_initializeCustomService| check "
                                     "clientInstanceId,serviceInstanceId! ";
                return;
            }
            genericUDSServicePtr_->RegisterUdsServiceInstance(instancePtr->nSid, clientInstanceId, serviceInstanceId);
        }};
    if (node.IterateArray(std::move(ara::core::StringView("instance")), loader) != isoft::kSuccess) {
        common::LogWarn() << "UdsConfig::_initializeCustomService|failed to load instance";
        return __LINE__;
    }
    instancePtr->interfacePtr = genericUDSServicePtr_;
    instancePtr->p4ServerMax  = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    customManagement_.table.insert(instancePtr);
    return result;
}

int32_t UdsConfig::_initializeServiceX14(isoft::manifestreader::ManifestNode const &node) noexcept
{
    std::function< void(std::size_t const, isoft::manifestreader::ManifestNode const &) > const loader{
        [this](std::size_t const, isoft::manifestreader::ManifestNode const &instanceNode) {
            std::uint32_t accessPermissionId{};
            if (isoft::kSuccess == instanceNode.Load(ara::core::StringView("accessPermission"), accessPermissionId)) {
                decltype(auto) findResult{accessPermissionTable_.find(accessPermissionId)};
                if (findResult == accessPermissionTable_.end()) {
                    LogError() << "UdsConfig::_initializeServiceX14| load "
                                  "accessPermission fails!, please check "
                                  "config file!";
                    return;
                }

                faultConfig_.clearDiagnosticInformationInstanceConfig.accessPermissionSession.insert(
                    findResult->second.sessionTable.begin(), findResult->second.sessionTable.end());
                faultConfig_.clearDiagnosticInformationInstanceConfig.accessPermissionSecurityLevel.insert(
                    findResult->second.securityLevelTable.begin(), findResult->second.securityLevelTable.end());
                faultConfig_.clearDiagnosticInformationInstanceConfig.accessPermissionAuth
                    = findResult->second.enableAuth;
            }
        }};
    if (node.IterateArray(std::move(ara::core::StringView("instance")), loader) != isoft::kSuccess) {
        LogError() << "UdsConfig::_initializeServiceX14|failed to load instance";
        return isoft::kFailure;
    }

    faultConfig_.clearDiagnosticInformationInstanceConfig.p4ServerMax
        = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    return isoft::kSuccess;
}

int32_t UdsConfig::_initializeServiceX19(isoft::manifestreader::ManifestNode const &node) noexcept
{
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > instanceLoader{
        [this](std::size_t, isoft::manifestreader::ManifestNode const &instanceNode) {
            ara::core::String category;
            std::ignore = instanceNode.Load(std::move(ara::core::StringView("category")), category);
            /// [TPS_DEXT_01060]
            ReadDTCInformationInstance instanceConfig;
            if (category == "REPORT_NUMBER_OF_DTC_BY_STATUS_MASK") {
                instanceConfig.subfunctionNumber = ReadDTCInformationSubfunction::kReportNumberOfDtcByStatusMask;
            } else if (category == "REPORT_DTC_BY_STATUS_MASK") {
                instanceConfig.subfunctionNumber = ReadDTCInformationSubfunction::kReportDtcByStatusMask;
            } else if (category == "REPORT_DTC_SNAPSHOT_IDENTIFICATION") {
                instanceConfig.subfunctionNumber = ReadDTCInformationSubfunction::kReportDtcSnapshotIdentification;
            } else if (category == "REPORT_DTC_SNAPSHOT_RECORD_BY_DTC_NUMBER") {
                instanceConfig.subfunctionNumber = ReadDTCInformationSubfunction::kReportDtcSnapshotRecordByDtcNumber;
            } else if (category == "REPORT_DTC_EXT_DATA_RECORD_BY_DTC_NUMBER") {
                instanceConfig.subfunctionNumber = ReadDTCInformationSubfunction::kReportDtcExtDataRecordByDtcNumber;
            } else if (category == "REPORT_NUMBER_OF_DTC_BY_SEVERITY_MASK_RECORD") {
                instanceConfig.subfunctionNumber
                    = ReadDTCInformationSubfunction::kReportNumberOfDtcBySeverityMaskRecord;
            } else if (category == "REPORT_SUPPORTED_DTCS") {
                instanceConfig.subfunctionNumber = ReadDTCInformationSubfunction::kReportSupportedDtcs;
            } else if (category == "REPORT_DTC_FAULT_DETECTION_COUNTER") {
                instanceConfig.subfunctionNumber = ReadDTCInformationSubfunction::kReportDtcFaultDetectionCounter;
            } else if (category == "REPORT_USER_DEF_MEMORY_DTC_BY_STATUS_MASK") {
                instanceConfig.subfunctionNumber = ReadDTCInformationSubfunction::kReportUserDefMemoryDtcByStatusMask;
            } else if (category == "REPORT_USER_DEF_MEMORY_DTC_SNAPSHOT_RECORD_BY_DTC_NUMBER") {
                instanceConfig.subfunctionNumber
                    = ReadDTCInformationSubfunction::kReportUserDefMemoryDtcSnapshotRecordByDtcNumber;
            } else if (category == "REPORT_USER_DEF_MEMORY_DTC_EXT_DATA_RECORD_BY_DTC_NUMBER") {
                instanceConfig.subfunctionNumber
                    = ReadDTCInformationSubfunction::kReportUserDefMemoryDtcExtDataRecordByDtcNumber;
            } else {
                LogError() << "UdsConfig::_initializeServiceX19|failed to parse category =" << category.c_str();
                return;
            }
            std::uint32_t accessPermissionId{};
            if (isoft::kSuccess != instanceNode.Load(ara::core::StringView("accessPermission"), accessPermissionId)) {
                LogError() << "UdsConfig::_initializeServiceX28| Load "
                              "accessPermissionId fails! Please check "
                              "config value!";
                return;
            }
            decltype(auto) findResult{accessPermissionTable_.find(accessPermissionId)};
            if (findResult == accessPermissionTable_.end()) {
                LogError() << "UdsConfig::_initializeServiceX19|find "
                              "accessPermission fails!, please check config file "
                              "accessPermissionId ="
                           << accessPermissionId;
                return;
            }
            instanceConfig.accessPermissionSession.insert(findResult->second.sessionTable.begin(),
                                                          findResult->second.sessionTable.end());
            instanceConfig.accessPermissionSecurityLevel.insert(findResult->second.securityLevelTable.begin(),
                                                                findResult->second.securityLevelTable.end());
            instanceConfig.accessPermissionAuth = findResult->second.enableAuth;
            faultConfig_.readDTCInformationInstanceConfig.instances.insert(instanceConfig);
            return;
        }};

    faultConfig_.readDTCInformationInstanceConfig.p4ServerMax
        = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    return node.IterateArray("instance", instanceLoader);
}

int32_t UdsConfig::_initializeServiceX85(isoft::manifestreader::ManifestNode const &node) noexcept
{
    std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > instanceLoader{
        [this](std::size_t, isoft::manifestreader::ManifestNode const &instanceNode) {
            ara::core::String category;
            std::ignore = instanceNode.Load(std::move(ara::core::StringView("category")), category);
            ControlDTCSettingInstance instanceConfig;
            if (category == ara::core::String("ON")) {
                instanceConfig.subfunctionNumber = ControlDTCSettingSubfunction::kOn;
            } else if (category == ara::core::String("OFF")) {
                instanceConfig.subfunctionNumber = ControlDTCSettingSubfunction::kOff;
            } else {
                LogError() << "UdsConfig::_initializeServiceX85|failed to parse category =" << category.c_str();
                return;
            }
            std::uint32_t accessPermissionId{};
            if (isoft::kSuccess != instanceNode.Load(ara::core::StringView("accessPermission"), accessPermissionId)) {
                LogError() << "UdsConfig::_initializeServiceX85|Load "
                              "accessPermissionId fails! Please check config value!";
                return;
            }
            decltype(auto) findResult{accessPermissionTable_.find(accessPermissionId)};
            if (findResult == accessPermissionTable_.end()) {
                LogError() << "UdsConfig::_initializeServiceX85|find "
                              "accessPermission fails!, please check config "
                              "file accessPermissionId ="
                           << accessPermissionId;
                return;
            }
            instanceConfig.accessPermissionSession.insert(findResult->second.sessionTable.begin(),
                                                          findResult->second.sessionTable.end());
            instanceConfig.accessPermissionSecurityLevel.insert(findResult->second.securityLevelTable.begin(),
                                                                findResult->second.securityLevelTable.end());
            instanceConfig.accessPermissionAuth = findResult->second.enableAuth;
            faultConfig_.controlDTCSettingInstanceConfig.instances.insert(instanceConfig);
        }};

    faultConfig_.controlDTCSettingInstanceConfig.p4ServerMax
        = _getP4ServiceMax(node, std::move(ara::core::StringView("p4ServerMax")));
    return node.IterateArray("instance", instanceLoader);
}

std::uint32_t UdsConfig::_getP4ServiceMax(isoft::manifestreader::ManifestNode const &node,
                                          ara::core::StringView key) noexcept
{
    std::uint32_t p4ServerMax{0U};
    if (isoft::kSuccess != node.Load(key, p4ServerMax)) {
        p4ServerMax = 0U;
        LogInfo() << "UdsConfig::_getP4ServiceMax|do not config p4, key =" << key;
    }

    return p4ServerMax;
}

}  // namespace dmd
}  // namespace diag
}  // namespace ara