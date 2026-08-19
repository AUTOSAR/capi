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
/// @file       uds_config.h
/// @brief      This file provides the Configuration File Parsing class
/// @details
/// @date       2024-11-29
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_DIAG_DM_UDS_CONFIG_H_
#define _ARA_DIAG_DM_UDS_CONFIG_H_
#include <isoft/manifestreader/manifest_reader.h>
#ifdef NMODEL
    #include <isoft/nml/definition/adaptive_software_component.h>
    #include <isoft/nml/definition/document.h>
    #include <isoft/nml/definition/uds_auth_role.h>
    #include <isoft/nml/definition/uds_auth_role_proxy.h>
    #include <isoft/nml/definition/uds_authentication_service.h>
    #include <isoft/nml/definition/uds_data_management.h>
    #include <isoft/nml/definition/uds_download_service.h>
    #include <isoft/nml/definition/uds_environmental_condition_formula.h>
    #include <isoft/nml/definition/uds_event.h>
    #include <isoft/nml/definition/uds_external_authentication_identification.h>
    #include <isoft/nml/definition/uds_fault_management.h>
    #include <isoft/nml/definition/uds_permission_group.h>
    #include <isoft/nml/definition/uds_security_level.h>
    #include <isoft/nml/definition/uds_server.h>
    #include <isoft/nml/definition/uds_service_validation_instance.h>
    #include <isoft/nml/definition/uds_session.h>
    #include <isoft/nml/definition/uds_session_control_service.h>
    #include <isoft/nml/definition/uds_transfer_management.h>
#endif
#include <isoft/uds/authentication_management/authentication_management.h>
#include <isoft/uds/com_management/com_management.h>
#include <isoft/uds/custom_management/custom_management.h>
#include <isoft/uds/data_management/diagnostic_data_management.h>
#include <isoft/uds/ecu_reset_management/ecu_reset_management.h>
#include <isoft/uds/fault_management/fault_management.h>
#include <isoft/uds/roe_management/roe_management.h>
#include <isoft/uds/routine_management/routine_management.h>
#include <isoft/uds/security_access_management/security_access_management.h>
#include <isoft/uds/server.h>
#include <isoft/uds/session_management/session_management.h>
#include <isoft/uds/transfer_managment/transfer_managment.h>
namespace ara {
namespace diag {
namespace dmd {

using isoft::uds::server::AgingConfig;
using isoft::uds::server::AuthenticationManagement;
using isoft::uds::server::ComManagementConfig;
using isoft::uds::server::ConditionConfig;
using isoft::uds::server::CustomManagement;
using isoft::uds::server::DebounceAlgorithmConfig;
using isoft::uds::server::DiagnosticDataManagement;
using isoft::uds::server::DtcConfig;
using isoft::uds::server::DtcPropsConfig;
using isoft::uds::server::EcuResetManagement;
using isoft::uds::server::EventConfig;
using isoft::uds::server::ExtendedDataRecordConfig;
using isoft::uds::server::FaultManagement;
using isoft::uds::server::FreezeFrameConfig;
using isoft::uds::server::IndicatorConfig;
using isoft::uds::server::MemoryDestinationConfig;
using isoft::uds::server::OperationCycleConfig;
using isoft::uds::server::RoeManagement;
using isoft::uds::server::RoutineManagement;
using isoft::uds::server::SecurityAccessManagement;
using isoft::uds::server::ServerSetting;
using isoft::uds::server::SessionMangement;
using isoft::uds::server::TransferManagment;
class GenericUDSService;
class GenericDataIdentifier;
class GenericDataElement;
class UdsConfig
{
public:
    UdsConfig(UdsConfig const&) = default;
    UdsConfig(UdsConfig&&)      = default;
    UdsConfig& operator=(UdsConfig const&) = default;
    UdsConfig& operator=(UdsConfig&&) = default;
    UdsConfig()                       = default;
    ~UdsConfig()                      = default;

#ifdef NMODEL
    /// @brief Initialize the configuration of a software set
    /// @param dmc Configuration node
    /// @return Whether initialization is successful
    bool Initialize(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::shared_ptr< isoft::nml::definition::UdsServer > udsServerPtr);
#endif
    /// @brief Initialize the configuration of a software set
    /// @param dmc Configuration node
    /// @return Whether initialization is successful
    bool Initialize(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc);

    /// @brief Get fault configuration
    /// @return Configuration
    FaultManagement& GetFaultConfig() { return faultConfig_; }

    AuthenticationManagement& GetAuthenticationManagement() { return authenticationManagement_; }

    SessionMangement& GetSessionMangement() noexcept { return sessionManagement_; }

    SecurityAccessManagement& GetSecurityAccessManagement() noexcept { return securityAccessManagement_; }

    EcuResetManagement& GetEcuResetManagement() noexcept { return ecuResetManagement_; }

    DiagnosticDataManagement& GetDiagnosticDataManagement() noexcept { return diagnosticDataManagement_; }

    ComManagementConfig& GetComManagementConfig() noexcept { return comManagementConfig_; }

    RoutineManagement& GetRoutineManagement() noexcept { return routineManagement_; }

    TransferManagment& GetTransferManagment() noexcept { return transferManagment_; }

    RoeManagement& GetRoeManagement() noexcept { return roeManagement_; }

    CustomManagement& GetCustomManagement() noexcept { return customManagement_; }

    ServerSetting& GetServerSetting() noexcept { return serverSetting_; }

private:
#if NMODEL
    std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent::PortType > _getPort(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        isoft::nml::ModelTag tag,
        void* key) noexcept;
    /// @brief Get Port unique identifier
    /// @param tag
    /// @param key
    /// @return
    std::vector< std::string > _getPortFqn(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        isoft::nml::ModelTag tag,
        void* key) noexcept;
    bool _hasPort(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        isoft::nml::ModelTag tag,
        void* key) noexcept;

    /// @brief Initialize fault configuration
    /// @param dmc Configuration node
    /// @return Whether initialization is successful
    bool _initializeUdsCommonSetting(std::shared_ptr< isoft::nml::definition::UdsServer > udsServerPtr) noexcept;

    /// @brief Initialize fault configuration
    /// @param dmc Configuration node
    /// @return Whether initialization is successful
    bool _initializeUdsFaultManagement(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::shared_ptr< isoft::nml::definition::UdsFaultManagement > udsFaultManagementPtr) noexcept;

    bool _initializeUdsEvent(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::uint32_t dtcValue,
        std::shared_ptr< isoft::nml::definition::UdsEvent > udsEventPtr) noexcept;

    bool _initializeUdsDtc(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::vector< std::shared_ptr< isoft::nml::definition::UdsDtc > > udsDtcTablePtr,
        std::vector< std::shared_ptr< isoft::nml::definition::UdsDtcGroup > > udsDtcGroupTablePtr) noexcept;

    bool _initializeUdsOperationCycle(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::vector< std::shared_ptr< isoft::nml::definition::UdsOperationCycle > > operationCycleTablePtr) noexcept;

    bool _initializeUdsExtendedDataRecord(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::vector< std::shared_ptr< isoft::nml::definition::UdsExtendedDataRecord > >
            extendedDataRecordTablePtr) noexcept;

    bool _initializeUdsFreezeFrame(
        std::vector< std::shared_ptr< isoft::nml::definition::UdsFreezeFrame > > freezeFrameTablePtr) noexcept;

    bool _initializeUdsDataIdentifierSet(std::vector< std::shared_ptr< isoft::nml::definition::UdsDataIdentifierSet > >
                                             UdsDataIdentifierSetTablePtr) noexcept;

    bool _initializeUdsFaultMemory(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::vector< std::shared_ptr< isoft::nml::definition::UdsFaultMemory > > UdsFaultMemoryTablePtr) noexcept;

    bool _initializeUdsCondition(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::vector< std::shared_ptr< isoft::nml::definition::UdsCondition > > clearConditionTablePtr,
        std::vector< std::shared_ptr< isoft::nml::definition::UdsCondition > > enableConditionTablePtr) noexcept;

    bool _initializeUdsIndicator(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::vector< std::shared_ptr< isoft::nml::definition::UdsIndicator > > indicatorTablePtr) noexcept;

    bool _setConfigByUdsPermissionGroup(std::shared_ptr< isoft::nml::definition::UdsPermissionGroup > groupPtr,
                                        std::set< std::uint8_t >& sessionTable,
                                        std::set< std::uint8_t >& securityLevelTable,
                                        std::int32_t& evCondition,
                                        isoft::uds::server::EnableAuth& enableAuth) noexcept;

    bool _initializeUdsDebounceAlgorithmType(
        std::vector< std::shared_ptr< isoft::nml::definition::UdsFaultManagement::DebounceAlgorithmType > >
            debounceAlgorithmTypeTable) noexcept;

    bool _initializeUdsClearDiagnosticInformationService(
        std::shared_ptr< isoft::nml::definition::UdsClearDiagnosticInformationService > serviceX14Ptr) noexcept;

    bool _initializeUdsReadDTCInformationService(
        std::shared_ptr< isoft::nml::definition::UdsReadDTCInformationService > serviceX19Ptr) noexcept;

    bool _initializeUdsControlDTCSettingService(
        std::shared_ptr< isoft::nml::definition::UdsControlDTCSettingService > serviceX85Ptr) noexcept;

    bool _initializeUdsValidation(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::shared_ptr< isoft::nml::definition::UdsValidation > udsValidationPtr) noexcept;

    bool _initializeUdsExternalAuthenticationIdentification(
        std::vector< std::shared_ptr< isoft::nml::definition::UdsExternalAuthenticationIdentification > >
            udsExtAuthIdTable) noexcept;

    bool _initializeUdsAuthRole(
        std::vector< std::shared_ptr< isoft::nml::definition::UdsAuthRole > > udsAuthRoleTable) noexcept;

    bool _initializeUdsSession(
        std::vector< std::shared_ptr< isoft::nml::definition::UdsSession > > udsSessionTable) noexcept;

    bool _initializeUdsSecurityLevel(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::vector< std::shared_ptr< isoft::nml::definition::UdsSecurityLevel > > udsSecurityLevelTable) noexcept;

    bool _initializeUdsEnvironmentalCondition(
        std::vector< std::shared_ptr< isoft::nml::definition::UdsEnvironmentalCondition > >
            udsEnvironmentalConditionTable) noexcept;

    bool _initializeUdsEnvironmentalConditionFormula(
        std::shared_ptr< isoft::nml::definition::UdsEnvironmentalConditionFormula > udsEnvironmentalConditionFormulaPtr,
        std::shared_ptr< isoft::uds::server::EnvConditionFormula >& formulaPtr) noexcept;

    bool _initializeUdsEnvironmentalDataElementCondition(
        std::shared_ptr< isoft::nml::definition::UdsEnvironmentalDataElementCondition >
            udsEnvironmentalDataElementConditionPtr,
        std::shared_ptr< isoft::uds::server::EnvDataCondition >& dataConditionPtr) noexcept;

    bool _initializeUdsPermissionGroup(
        std::vector< std::shared_ptr< isoft::nml::definition::UdsPermissionGroup > > udsPermissionGroupTable) noexcept;

    bool _initializeUdsServiceValidationInstance(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::vector< std::shared_ptr< isoft::nml::definition::UdsServiceValidationInstance > >
            udsServiceValidationInstanceTable,
        std::vector< std::shared_ptr< isoft::uds::server::ServiceValidationInterface > >& instanceTable) noexcept;

    bool _initializeUdsSessionControlService(
        std::shared_ptr< isoft::nml::definition::UdsSessionControlService > udsSessionControlServicePtr) noexcept;

    bool _initializeUdsSecurityAccessService(
        std::shared_ptr< isoft::nml::definition::UdsSecurityAccessService > udsSecurityAccessServicePtr) noexcept;

    bool _initializeUdsDataIdentifier(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::vector< std::shared_ptr< isoft::nml::definition::UdsDataIdentifier > > udsDataIdentifierTable) noexcept;

    bool _initializeUdsReadDataByIdentifierService(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::shared_ptr< isoft::nml::definition::UdsReadDataByIdentifierService >
            udsReadDataByIdentifierServicePtr) noexcept;

    bool _initializeUdsWriteDataByIdentifierService(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::shared_ptr< isoft::nml::definition::UdsWriteDataByIdentifierService >
            udsWriteDataByIdentifierServicePtr) noexcept;

    bool _initializeUdsReadDataByPeriodicIdentifierService(
        std::shared_ptr< isoft::nml::definition::UdsReadDataByPeriodicIdentifierService >
            udsReadDataByPeriodicIdentifierServicePtr) noexcept;

    bool _initializeUdsDynamicallyDefineDataIdentifierService(
        std::shared_ptr< isoft::nml::definition::UdsDynamicallyDefineDataIdentifierService >
            udsDynamicallyDefineDataIdentifierServicePtr) noexcept;

    bool _initializeUdsDataManagement(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::shared_ptr< isoft::nml::definition::UdsDataManagement > udsDataManagementPtr) noexcept;

    bool _initializeUdsRoutineControlService(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::shared_ptr< isoft::nml::definition::UdsRoutineControlService > udsRoutineControlServicePtr) noexcept;

    bool _initializeUdsEcuResetService(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::shared_ptr< isoft::nml::definition::UdsEcuResetService > udsEcuResetServicePtr) noexcept;

    bool _initializeUdsCommunicationControlService(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::shared_ptr< isoft::nml::definition::UdsCommunicationControlService >
            udsCommunicationControlServicePtr) noexcept;

    bool _initializeUdsResponseOnEventService(
        std::shared_ptr< isoft::nml::definition::UdsResponseOnEventService > udsResponseOnEventServicePtr) noexcept;

    bool _initializeUdsCustomService(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::vector< std::shared_ptr< isoft::nml::definition::UdsCustomService > > udsCustomServiceTable) noexcept;

    bool _initializeUdsDownloadService(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::shared_ptr< isoft::nml::definition::UdsDownloadService > udsDownloadServicePtr) noexcept;

    bool _initializeUdsUploadService(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::shared_ptr< isoft::nml::definition::UdsUploadService > udsUploadServicePtr) noexcept;

    bool _initializeUdsFileTransferService(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::shared_ptr< isoft::nml::definition::UdsFileTransferService > udsFileTransferServicePtr) noexcept;

    bool _initializeUdsTransferManagement(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::shared_ptr< isoft::nml::definition::UdsTransferManagement > udsTransferManagementPtr) noexcept;

    bool _initializeUdsAuthenticationService(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::shared_ptr< isoft::nml::definition::UdsAuthenticationService > udsAuthenticationServicePtr) noexcept;
#endif
    /// @brief Initialize fault configuration
    /// @param dmc Configuration node
    /// @return Whether initialization is successful
    bool _initializeFaultConfig(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc);

    /// @brief Read common attributes
    /// @param[in] dmc Configuration object
    /// @return Success/Failure
    /// @throw
    bool _initializeCommonProperties(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc);

    /// @brief Build DID dataset for configuration
    /// @param[in] dmc Configuration object
    /// @return Success/Failure
    /// @throw
    bool _initializeConfiguredDids(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc);

    /// @brief Create DTC group
    /// @param[in] dmc Configuration object
    /// @return Success/Failure
    /// @throw
    bool _initializeDTCGroups(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc);

    /// @brief Create condition enable group
    /// @param[in] dmc Configuration object
    /// @return Success/Failure
    /// @throw
    bool _initializeConditionGroups(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc);

    /// @brief Create operation cycle
    /// @param[in] dmc Configuration object
    /// @return Success/Failure
    /// @throw
    bool _initializeOperationCycles(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc);

    /// @brief Create diagnostic event object and complete related item mapping
    /// @param[in] dmc Configuration object
    /// @return Success/Failure
    /// @throw
    bool _initializeMapEvents(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc);

    bool _initializeSessionConfig(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc) noexcept;
    bool _initializeServiceTable(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc) noexcept;

    int _initializeServerSetting(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc) noexcept;
    int _initializeServiceValidation(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc) noexcept;
    int _initializeDiagnosticAuthentication(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc) noexcept;
    int _initializeDiagnosticSession(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc) noexcept;
    int _initializeSecurityLevel(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc) noexcept;
    int _initializeDataParameter(isoft::uds::server::DiagnosticDataModel& dataModel,
                                 isoft::manifestreader::ManifestNode const& node) noexcept;
    int _initializeDataManager(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc) noexcept;
    int _initializeEnvConditionFormulaPart(
        std::vector< std::shared_ptr< isoft::uds::server::EnvConditionFormulaPart > >& parts,
        isoft::manifestreader::ManifestNode const& node,
        ara::core::StringView const key) noexcept;
    int _initializeEnvDataCondition(std::shared_ptr< isoft::uds::server::EnvConditionFormulaPart >& partPtr,
                                    isoft::manifestreader::ManifestNode const& node) noexcept;
    int _initializeEnvConditionFormula(std::shared_ptr< isoft::uds::server::EnvConditionFormulaPart >& partPtr,
                                       isoft::manifestreader::ManifestNode const& node,
                                       ara::core::StringView const key) noexcept;
    int _initializeEnvCondition(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc) noexcept;
    int _initializeAccessPermission(std::unique_ptr< isoft::manifestreader::Manifest > const& dmc) noexcept;
    int _initializeServiceX10(isoft::manifestreader::ManifestNode const& node) noexcept;
    int _initializeServiceX11(isoft::manifestreader::ManifestNode const& node) noexcept;
    int _initializeServiceX22(isoft::manifestreader::ManifestNode const& node) noexcept;
    int _initializeServiceX27(isoft::manifestreader::ManifestNode const& node) noexcept;
    int _initializeServiceX28(isoft::manifestreader::ManifestNode const& node) noexcept;
    int _initializeServiceX29(isoft::manifestreader::ManifestNode const& node) noexcept;
    int _initializeServiceX2A(isoft::manifestreader::ManifestNode const& node) noexcept;
    int _initializeServiceX2C(isoft::manifestreader::ManifestNode const& node) noexcept;
    int _initializeServiceX2E(isoft::manifestreader::ManifestNode const& node) noexcept;
    int _initializeServiceX31(isoft::manifestreader::ManifestNode const& node,
                              std::unique_ptr< isoft::manifestreader::Manifest > const& dmc) noexcept;
    int _initializeServiceX34(isoft::manifestreader::ManifestNode const& node) noexcept;
    int _initializeServiceX35(isoft::manifestreader::ManifestNode const& node) noexcept;
    int _initializeServiceX38(isoft::manifestreader::ManifestNode const& node) noexcept;
    int _initializeServiceX86(isoft::manifestreader::ManifestNode const& node) noexcept;
    int _initializeCustomService(isoft::manifestreader::ManifestNode const& node) noexcept;

    int32_t _initializeServiceX14(isoft::manifestreader::ManifestNode const& node) noexcept;
    int32_t _initializeServiceX19(isoft::manifestreader::ManifestNode const& node) noexcept;
    int32_t _initializeServiceX85(isoft::manifestreader::ManifestNode const& node) noexcept;

    std::uint32_t _getP4ServiceMax(isoft::manifestreader::ManifestNode const& node, ara::core::StringView key) noexcept;

private:
    struct AccessPermission
    {
        std::vector< std::uint8_t > sessionTable;
        std::vector< std::uint8_t > securityLevelTable;
        std::int32_t evCondition{-1};
        isoft::uds::server::EnableAuth enableAuth{};
    };
#ifdef NMODEL
    std::map< isoft::nml::definition::UdsOperationCycle*, std::uint32_t > udsOperationCycleMap_{};
    std::map< isoft::nml::definition::UdsDataIdentifierSet*, std::uint32_t > udsDataIdentifierSetMap_{};
    std::map< isoft::nml::definition::UdsExtendedDataRecord*, std::uint32_t > udsExtendedDataRecordMap_{};
    std::map< isoft::nml::definition::UdsFreezeFrame*, std::uint32_t > udsFreezeFrameMap_{};
    std::map< isoft::nml::definition::UdsFaultMemory*, std::uint32_t > udsFaultMemoryMap_{};
    std::map< isoft::nml::definition::UdsConditionGroup*, std::uint32_t > udsClearConditionGroupMap_{};
    std::map< isoft::nml::definition::UdsCondition*, std::uint32_t > udsClearConditionMap_{};
    std::map< isoft::nml::definition::UdsEventDebounceTimeBased*, std::uint32_t > udsEventDebounceTimeBasedMap_{};
    std::map< isoft::nml::definition::UdsEventDebounceCounterBased*, std::uint32_t > udsEventDebounceCounterBasedMap_{};
    std::map< isoft::nml::definition::UdsIndicator*, std::uint32_t > udsIndicatorMap_{};
    std::map< isoft::nml::definition::UdsConditionGroup*, std::uint32_t > udsEnableConditionGroupMap_{};
    std::map< isoft::nml::definition::UdsCondition*, std::uint32_t > udsEnableConditionMap_{};
    std::map< isoft::nml::definition::UdsSecurityLevel*, isoft::uds::server::SecurityLevelConfig >
        udsSecurityLevelMap_{};
    std::map< isoft::nml::definition::UdsEnvironmentalCondition*, std::uint8_t > udsEnvironmentalConditionMap_{};
    std::map< isoft::nml::definition::UdsPermissionGroup*, AccessPermission > udsPermissionGroupMap_{};
#endif
    FaultManagement faultConfig_;
    SessionMangement sessionManagement_;
    SecurityAccessManagement securityAccessManagement_;
    EcuResetManagement ecuResetManagement_;
    DiagnosticDataManagement diagnosticDataManagement_;
    ComManagementConfig comManagementConfig_;
    RoutineManagement routineManagement_;
    TransferManagment transferManagment_;
    RoeManagement roeManagement_;
    CustomManagement customManagement_;
    ServerSetting serverSetting_;
    AuthenticationManagement authenticationManagement_;
    std::map< std::uint32_t, AccessPermission > accessPermissionTable_;
    std::shared_ptr< GenericUDSService > genericUDSServicePtr_;
    std::shared_ptr< GenericDataIdentifier > genericDataIdentifierPtr_;
    std::shared_ptr< GenericDataElement > genericDataElementPtr_;
    struct RoutineInfo
    {
        std::uint16_t routineId{0U};
        std::int16_t routineInfo{-1};
        std::int32_t startAccessPermissionId{-1};
        std::uint32_t p4ServerMaxStart{0U};
        std::int32_t stopAccessPermissionId{-1};
        std::uint32_t p4ServerMaxStop{0U};
        std::int32_t getResultAccessPermissionId{-1};
        std::uint32_t p4ServerMaxResult{0U};
        std::uint32_t p4ServerMax{0U};
    };
    /// @brief key：routine id  value：routine info
    std::map< uint16_t, RoutineInfo > mapRoutineInfo_;
};

}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif