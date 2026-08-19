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
/// @file       diagnostic_server.cpp
/// @brief      This file provides the Diagnostic Management class
/// @details
/// @date       2024-12-06
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "diagnostic_server.h"

#include <isoft/manifestreader/manifest_reader.h>
#ifdef NMODEL
    #include "hash/hash.h"
#endif
#include "log/log.h"
namespace ara {
namespace diag {
namespace dmd {

/// @brief Constructor
DiagnosticServer::DiagnosticServer() noexcept
    : serverPtr_{std::make_shared< isoft::uds::server::Server >()}
    , udsConfigPtr_{std::make_shared< UdsConfig >()}
    , conversationManagerPtr_{std::make_shared< ConversationManager >()}
    , externalAuthenticationPtr_{std::make_shared< ExternalAuthentication >()}
{
}
#ifdef NMODEL
/// @brief DM initialization
/// @return Initialization result code
std::int32_t DiagnosticServer::InitializeV3(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentList,
    std::shared_ptr< isoft::nml::definition::UdsServer > udsServerPtr,
    std::shared_ptr< isoft::uds::server::Transport > const& transportPtr,
    std::shared_ptr< PersistenceFile > const& persistenceFilePtr) noexcept
{
    if (udsServerPtr.get() == nullptr) {
        common::LogError() << "DiagnosticServer::Initialize| udsServerPtr is nullptr!";
        return __LINE__;
    }
    v3Flag_ = true;
    int32_t initializaResult
        = _initializeUdsServerV3(adaptiveSoftwareComponentList, udsServerPtr, transportPtr, persistenceFilePtr);
    if (initializaResult != isoft::kSuccess) {
        common::LogError() << "DiagnosticServer::Initialize|failed to load uds configuration|";
        return __LINE__;
    }
    return 0;
}
#endif
/// @brief DM initialization
/// @return Initialization result code
std::int32_t DiagnosticServer::Initialize(ara::core::StringView const& path,
                                          std::shared_ptr< isoft::uds::server::Transport > const& transportPtr,
                                          std::shared_ptr< PersistenceFile > const& persistenceFilePtr) noexcept
{
    decltype(auto) result{isoft::manifestreader::OpenManifest(std::move(path))};
    if (!result.HasValue()) {
        common::LogError() << "DiagnosticServer::Initialize|failed to open configuration|" << result.Error();
        return __LINE__;
    }
    v3Flag_ = false;
    std::unique_ptr< isoft::manifestreader::Manifest > dmc{std::move(result).Value()};

    int32_t initializaResult = _initializeUdsServer(dmc, transportPtr, persistenceFilePtr);
    if (initializaResult != isoft::kSuccess) {
        common::LogError() << "DiagnosticServer::Initialize|failed to load uds configuration|";
        return __LINE__;
    }
    return 0;
}
#ifdef NMODEL
std::int32_t DiagnosticServer::_initializeUdsServerV3(

    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentList,
    std::shared_ptr< isoft::nml::definition::UdsServer > udsServerPtr,

    std::shared_ptr< isoft::uds::server::Transport > transportPtr,
    std::shared_ptr< PersistenceFile > const& persistenceFilePtr) noexcept
{
    bool initializaResult = udsConfigPtr_->Initialize(adaptiveSoftwareComponentList, udsServerPtr);

    if (!initializaResult) {
        common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to load uds configuration|";
        return __LINE__;
    }
    bool initServer{false};

    ServerSetting& serverSetting{udsConfigPtr_->GetServerSetting()};
    serverSetting.transportPtr = std::move(transportPtr);
    initServer                 = serverPtr_->Initialize(serverSetting);
    if (!initServer) {
        common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize ServerSetting!";
        return __LINE__;
    }

    /// serverPtr_->RegisterPersistence(persistencePtr_);
    serverPtr_->RegisterPersistence(persistenceFilePtr);

    isoft::uds::server::AuthenticationManagement& authMangement{udsConfigPtr_->GetAuthenticationManagement()};
    if (authMangement.interface.get() != nullptr) {
        initServer = serverPtr_->Initialize(authMangement);
        if (!initServer) {
            common::LogError()
                << "DiagnosticServer::_initializeUdsServer|failed to Initialize AuthenticationManagement!";
            return __LINE__;
        }
    }
    SessionMangement& sessionMangement{udsConfigPtr_->GetSessionMangement()};
    if (!sessionMangement.sessionConfigTable.empty()) {
        initServer = serverPtr_->Initialize(sessionMangement);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize SessionMangement!";
            return __LINE__;
        }
    }

    SecurityAccessManagement& securityAccessManagement{udsConfigPtr_->GetSecurityAccessManagement()};
    if (!securityAccessManagement.securityLevelTable.empty()) {
        initServer = serverPtr_->Initialize(securityAccessManagement);
        if (!initServer) {
            common::LogError()
                << "DiagnosticServer::_initializeUdsServer|failed to Initialize SecurityAccessManagement!";
            return __LINE__;
        }
    }

    DiagnosticDataManagement& diagnosticDataManagement{udsConfigPtr_->GetDiagnosticDataManagement()};
    if (!diagnosticDataManagement.didManager.staticData.empty()) {
        initServer = serverPtr_->Initialize(diagnosticDataManagement);
        if (!initServer) {
            common::LogError()
                << "DiagnosticServer::_initializeUdsServer|failed to Initialize DiagnosticDataManagement!";
            return __LINE__;
        }
    }

    EcuResetManagement& ecuResetManagement{udsConfigPtr_->GetEcuResetManagement()};
    if (!ecuResetManagement.instanceTable.empty()) {
        initServer = serverPtr_->Initialize(ecuResetManagement);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize EcuResetManagement!";
            return __LINE__;
        }
    }

    FaultManagement& faultManagement{udsConfigPtr_->GetFaultConfig()};
    if (!faultManagement.eventConfig.empty()) {
        initServer = serverPtr_->Initialize(faultManagement);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize FaultManagement!";
            return __LINE__;
        }
    }

    ComManagementConfig& comManagementConfig{udsConfigPtr_->GetComManagementConfig()};
    if (!comManagementConfig.instanceTable.empty()) {
        initServer = serverPtr_->Initialize(comManagementConfig);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize ComManagementConfig!";
            return __LINE__;
        }
    }

    RoutineManagement& routineManagement{udsConfigPtr_->GetRoutineManagement()};
    if (!routineManagement.routineInstanceTable.empty()) {
        initServer = serverPtr_->Initialize(routineManagement);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize RoutineManagement!";
            return __LINE__;
        }
    }

    TransferManagment& transferManagment{udsConfigPtr_->GetTransferManagment()};
    if (transferManagment.requestDownload.get() != nullptr || transferManagment.requestFileTransfer.get() != nullptr
        || transferManagment.requestUpload.get() != nullptr) {
        initServer = serverPtr_->Initialize(transferManagment);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize TransferManagment!";
            return __LINE__;
        }
    }

    RoeManagement& roeManagement{udsConfigPtr_->GetRoeManagement()};
    if (!roeManagement.instanceTable.empty()) {
        initServer = serverPtr_->Initialize(roeManagement);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize RoeManagement!";
            return __LINE__;
        }
    }

    CustomManagement& customManagement{udsConfigPtr_->GetCustomManagement()};
    if (!customManagement.table.empty()) {
        initServer = serverPtr_->Initialize(customManagement);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize CustomManagement!";
            return __LINE__;
        }
    }

    std::uint32_t maxNumber{serverSetting.maxParallelRequests};

    std::uint32_t serviceInstanceId{0U};
    std::string serviceInstance{udsServerPtr->isoftNmlObjectFqn};
    std::string conversationServiceInstance = serviceInstance + "/Conversation";
    serviceInstanceId                       = Hash32(conversationServiceInstance);

    initServer = conversationManagerPtr_->Initialize(serverPtr_, maxNumber, serviceInstanceId,

                                                     sessionMangement.sessionConfigTable,
                                                     securityAccessManagement.securityLevelTable);
    if (!initServer) {
        common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize ConversationManager!";
        return __LINE__;
    }
    std::string externalAuthenticationServiceInstance = serviceInstance + "/ExternalAuthentication";
    serviceInstanceId                                 = Hash32(externalAuthenticationServiceInstance);
    std::uint32_t initExternalAuth                    = externalAuthenticationPtr_->Initialize(serviceInstanceId);
    if (initExternalAuth != 0U) {
        common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize ExternalAuthentication!";
        return __LINE__;
    }
    externalAuthenticationPtr_->RegisterGetClientAuthenticationController([this](std::uint16_t address) {
        std::shared_ptr< isoft::uds::server::ClientAuthenticationController > clientAuthPtr{nullptr};
        auto result = serverPtr_->GetClientAuthentication(address);

        if (result.HasValue()) {
            clientAuthPtr = result.Value();
        }
        return clientAuthPtr;
    });
    externalAuthenticationPtr_->RegisterGetAllClientAddress([this]() {
        std::vector< uint16_t > table{};
        auto result = serverPtr_->GetAllClientAuthenticationAddress();
        if (result.HasValue()) {
            table = result.Value();
        }
        return table;
    });
    return 0;
}
#endif
std::int32_t DiagnosticServer::_initializeUdsServer(
    std::unique_ptr< isoft::manifestreader::Manifest >& dmc,
    std::shared_ptr< isoft::uds::server::Transport > transportPtr,
    std::shared_ptr< PersistenceFile > const& persistenceFilePtr) noexcept
{
    bool initializaResult = udsConfigPtr_->Initialize(dmc);
    if (!initializaResult) {
        common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to load uds configuration|";
        return __LINE__;
    }
    bool initServer{false};

    ServerSetting& serverSetting{udsConfigPtr_->GetServerSetting()};
    serverSetting.transportPtr = std::move(transportPtr);
    initServer                 = serverPtr_->Initialize(serverSetting);
    if (!initServer) {
        common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize ServerSetting!";
        return __LINE__;
    }

    /// serverPtr_->RegisterPersistence(persistencePtr_);
    serverPtr_->RegisterPersistence(persistenceFilePtr);

    isoft::uds::server::AuthenticationManagement& authMangement{udsConfigPtr_->GetAuthenticationManagement()};
    if (authMangement.interface.get() != nullptr) {
        initServer = serverPtr_->Initialize(authMangement);
        if (!initServer) {
            common::LogError()
                << "DiagnosticServer::_initializeUdsServer|failed to Initialize AuthenticationManagement!";
            return __LINE__;
        }
    }
    SessionMangement& sessionMangement{udsConfigPtr_->GetSessionMangement()};
    if (!sessionMangement.sessionConfigTable.empty()) {
        initServer = serverPtr_->Initialize(sessionMangement);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize SessionMangement!";
            return __LINE__;
        }
    }

    SecurityAccessManagement& securityAccessManagement{udsConfigPtr_->GetSecurityAccessManagement()};
    if (!securityAccessManagement.securityLevelTable.empty()) {
        initServer = serverPtr_->Initialize(securityAccessManagement);
        if (!initServer) {
            common::LogError()
                << "DiagnosticServer::_initializeUdsServer|failed to Initialize SecurityAccessManagement!";
            return __LINE__;
        }
    }

    DiagnosticDataManagement& diagnosticDataManagement{udsConfigPtr_->GetDiagnosticDataManagement()};
    if (!diagnosticDataManagement.didManager.staticData.empty()) {
        initServer = serverPtr_->Initialize(diagnosticDataManagement);
        if (!initServer) {
            common::LogError()
                << "DiagnosticServer::_initializeUdsServer|failed to Initialize DiagnosticDataManagement!";
            return __LINE__;
        }
    }

    EcuResetManagement& ecuResetManagement{udsConfigPtr_->GetEcuResetManagement()};
    if (!ecuResetManagement.instanceTable.empty()) {
        initServer = serverPtr_->Initialize(ecuResetManagement);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize EcuResetManagement!";
            return __LINE__;
        }
    }

    FaultManagement& faultManagement{udsConfigPtr_->GetFaultConfig()};
    if (!faultManagement.eventConfig.empty()) {
        initServer = serverPtr_->Initialize(faultManagement);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize FaultManagement!";
            return __LINE__;
        }
    }

    ComManagementConfig& comManagementConfig{udsConfigPtr_->GetComManagementConfig()};
    if (!comManagementConfig.instanceTable.empty()) {
        initServer = serverPtr_->Initialize(comManagementConfig);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize ComManagementConfig!";
            return __LINE__;
        }
    }

    RoutineManagement& routineManagement{udsConfigPtr_->GetRoutineManagement()};
    if (!routineManagement.routineInstanceTable.empty()) {
        initServer = serverPtr_->Initialize(routineManagement);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize RoutineManagement!";
            return __LINE__;
        }
    }

    TransferManagment& transferManagment{udsConfigPtr_->GetTransferManagment()};
    if (transferManagment.requestDownload.get() != nullptr || transferManagment.requestFileTransfer.get() != nullptr
        || transferManagment.requestUpload.get() != nullptr) {
        initServer = serverPtr_->Initialize(transferManagment);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize TransferManagment!";
            return __LINE__;
        }
    }

    RoeManagement& roeManagement{udsConfigPtr_->GetRoeManagement()};
    if (!roeManagement.instanceTable.empty()) {
        initServer = serverPtr_->Initialize(roeManagement);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize RoeManagement!";
            return __LINE__;
        }
    }

    CustomManagement& customManagement{udsConfigPtr_->GetCustomManagement()};
    if (!customManagement.table.empty()) {
        initServer = serverPtr_->Initialize(customManagement);
        if (!initServer) {
            common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize CustomManagement!";
            return __LINE__;
        }
    }
    std::uint32_t maxNumber{};
    if (dmc->Load("conversation.maxNum", maxNumber) != isoft::kSuccess) {
        common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to load conversation.maxNum!";
        return __LINE__;
    }
    std::uint32_t serviceInstanceId{0U};
    if (dmc->Load("conversation.serviceInstanceId", serviceInstanceId) != isoft::kSuccess) {
        serviceInstanceId = 0U;
        common::LogWarn() << "DiagnosticServer::_initializeUdsServer|failed to load conversation.serviceInstanceId!";
    }
    initServer = conversationManagerPtr_->Initialize(serverPtr_, maxNumber, serviceInstanceId,
                                                     sessionMangement.sessionConfigTable,
                                                     securityAccessManagement.securityLevelTable);
    if (!initServer) {
        common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize ConversationManager!";
        return __LINE__;
    }
    if (dmc->Load("diagnosticAuthentication.externalAuthenticationService", serviceInstanceId) != isoft::kSuccess) {
        serviceInstanceId = 0U;
        common::LogWarn() << "DiagnosticServer::_initializeUdsServer|failed to load "
                             "diagnosticAuthentication.externalAuthenticationService!";
    }
    std::uint32_t initExternalAuth = externalAuthenticationPtr_->Initialize(serviceInstanceId);
    if (initExternalAuth != 0U) {
        common::LogError() << "DiagnosticServer::_initializeUdsServer|failed to Initialize ExternalAuthentication!";
        return __LINE__;
    }
    externalAuthenticationPtr_->RegisterGetClientAuthenticationController([this](std::uint16_t address) {
        std::shared_ptr< isoft::uds::server::ClientAuthenticationController > clientAuthPtr{nullptr};
        auto result = serverPtr_->GetClientAuthentication(address);

        if (result.HasValue()) {
            clientAuthPtr = result.Value();
        }
        return clientAuthPtr;
    });
    externalAuthenticationPtr_->RegisterGetAllClientAddress([this]() {
        std::vector< uint16_t > table{};
        auto result = serverPtr_->GetAllClientAuthenticationAddress();
        if (result.HasValue()) {
            table = result.Value();
        }
        return table;
    });
    return 0;
}
}  // namespace dmd
}  // namespace diag
}  // namespace ara
