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
/// @file       diagnostic_manager.cpp
/// @brief      This file provides the Diagnostic Management class
/// @details
/// @date       2022-04-22
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "diagnostic_manager.h"

#ifdef NMODEL
    #include <isoft/nml/definition/dm_network_configuration.h>
    #include <isoft/nml/definition/executable.h>
    #include <isoft/nml/definition/ipv4_configuration.h>
    #include <isoft/nml/definition/machine.h>
    #include <isoft/nml/definition/network_endpoint.h>
    #include <isoft/nml/definition/package.h>
    #include <isoft/nml/definition/process.h>
    #include <isoft/nml/model_tag.h>
    #include <isoft/nml/nml.h>

    #include "hash/hash.h"
#endif

#include <ara/diag/diag_error_domain.h>
#include <ara/diag/diag_uds_nrc_error_domain.h>
#ifdef ARA_DIAG_WITH_EXEC
    #include <ara/exec/execution_client.h>
#endif
// #include <ara/per/file_storage.h>
// #include <ara/per/key_value_storage.h>
#include <isoft/ara_fsh/process.h>
#include <isoft/manifestreader/manifest_reader.h>
#include <isoft/naicpp/global_evloop.h>
#include <isoft/naicpp/terminating_handler.h>
#include <isoft/uds/server_manager.h>
#include <nai/runtime/nai_errno.h>

#include <csignal>

#include "doip-adapter/doip_server.h"
#include "log/log.h"
#include "netProxy/net_proxy.h"
#include "nsomeipConfig/nsomeip_config.h"
#include "serialization/serialization/common_data_type.h"
#include "thread_pool/thread_pool.h"
#include "uds-adapter/uds_config.h"
namespace ara {
namespace diag {
namespace dmd {

using isoft::ThreadPool;
#ifdef NMODEL
std::uint16_t Hash16(std::string const value) noexcept
{
    std::hash< std::string > hasher;
    std::uint32_t hashValue = static_cast< std::uint16_t >(hasher(value) & 0xFFFFU);
    common::LogDebug() << "Hash16| input:" << value << " output:" << hashValue;
    return hashValue;
}
std::uint32_t Hash32(std::string const value) noexcept
{
    std::hash< std::string > hasher;
    std::uint32_t hashValue = static_cast< std::uint32_t >(hasher(value) & 0xFFFFFFFF);
    common::LogDebug() << "Hash32| input:" << value << " output:" << hashValue;
    return hashValue;
}
#endif
/// @brief Signal processing
/// @param[in] number
void DiagnosticManager::_processSignal() noexcept
{
    bool expected{false};
    if (stopping_.compare_exchange_strong(expected, true)) {
        std::ignore = expected;
        std::ignore = ThreadPool::GetInstance().Submit([this]() noexcept {
            // dcm::ServerManager::GetInstance().Stop();
            // tp::Manager::GetInstance().Stop();
            doip_->Stop();
            for (auto&& server : serverTable_) {
                decltype(auto) udsServerPtr{server->GetUdsServer()};
                udsServerPtr->Stop();
            }
            serverTable_.clear();
            isoft::dm::NetProxy::GetInstance().Start(false);
            isoft::naicpp::GlobalGeneralEvLoop::Get()->Stop();
        });
    }
}

/// @brief DM initialization
/// @return Initialization result code
std::int32_t DiagnosticManager::Initialize() noexcept
{
    // install signal processors
    decltype(auto) status{
        signalManager_.Register(SIGTERM, [this](std::int32_t const) noexcept { this->_processSignal(); })};
    if (status != 0) {
        common::LogFatal() << "main|SignalManager::Register|SIGTERM|" << status;
        return __LINE__;
    }

    // register ErrorDomain
    // decltype(auto) comRuntime{ara::com::internal::GetInstance()};
    // comRuntime.RegisterErrorDomain(ara::diag::GetDiagDomain());
    // comRuntime.RegisterErrorDomain(ara::diag::GetDiagOfferDomain());
    // comRuntime.RegisterErrorDomain(ara::diag::GetDiagReportingDomain());
    // comRuntime.RegisterErrorDomain(ara::diag::GetDiagUdsNrcDomain());

    // load configuration
    // ara::core::InstanceSpecifier const instanceSpecifier{ara::core::StringView("dmd/root/DiagnosticStorage")};
    // decltype(auto) openResult{ara::per::OpenKeyValueStorage(instanceSpecifier)};
    // if (!openResult.HasValue()) {
    //     common::LogWarn() << "DiagnosticManager::Initialize|ara::per::OpenKeyValueStorage|" << openResult.Error();
    //     return __LINE__;
    // }
    // ara::per::SharedHandle< ara::per::KeyValueStorage > storage{std::move(openResult).Value()};

    isoft::ara_fsh::Process const process;
    ara::core::String const etcPath{process.GetEtcDir()};
    ara::core::String path{etcPath + "/dmd.json"};
    common::LogDebug() << "config path:" << path;
    persistenceFilePtr_ = std::make_shared< PersistenceFile >();
    persistenceFilePtr_->Initialize("dmd.db");
    std::uint32_t serverAmount{1U};
#ifdef NMODEL
    std::string nmodelPath{etcPath.c_str()};
    nmodelPath += "/dmd_config.json";
    auto doc = isoft::nml::Open(nmodelPath.c_str());
    if (doc.get() != nullptr) {
        useNmodel_                                                      = true;
        std::shared_ptr< isoft::nml::definition::DmModule > dmModulePtr = _findDmModule(doc);
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adsoftComponentList{
            _findAdaptiveSoftwareComponent(doc)};

        for (auto&& udsServerRef : dmModulePtr->udsServerRef) {
            std::shared_ptr< isoft::nml::definition::UdsServer > udsServerPtr{udsServerRef.lock()};
            if (udsServerPtr.get() != nullptr) {
                std::shared_ptr< DiagnosticServer > diagnosticServerPtr{std::make_shared< DiagnosticServer >()};
                std::string softwareClusterShortName{""};
                if (udsServerPtr->shortName.get() != nullptr) {
                    softwareClusterShortName = *udsServerPtr->shortName;
                }
                common::LogInfo() << "DiagnosticManager::Initialize|begin init " << softwareClusterShortName;
                std::int32_t err
                    = diagnosticServerPtr->InitializeV3(adsoftComponentList, udsServerPtr, doip_, persistenceFilePtr_);
                if (err != 0) {
                    common::LogError() << " softwareClusterShortName:" << softwareClusterShortName
                                       << " initialize fails!";
                    // isLoadConfigSuccess = false;
                    return __LINE__;
                }
                /// assert(err == 0);
                serverAmount++;
                serverTable_.push_back(diagnosticServerPtr);
                common::LogInfo() << "DiagnosticManager::Initialize|end init " << softwareClusterShortName.c_str();
            }
        }
        if (dmModulePtr->doipModule.get() != nullptr) {
            if (!doip_->Initialize(adsoftComponentList, dmModulePtr->doipModule)) {
                common::LogInfo() << "DiagnosticManager::Initialize| load doip Fails!";
            }
        }

    } else {
        common::LogDebug() << "DiagnosticManager::Initialize|failed to open model";
#endif
        decltype(auto) result{isoft::manifestreader::OpenManifest(std::move(ara::core::StringView(path.c_str())))};
        if (!result.HasValue()) {
            common::LogWarn() << "DiagnosticManager::Initialize|failed to open configuration|" << result.Error();
            return __LINE__;
        }
        /// bool isLoadConfigSuccess{true};
        if (result.Value()->IterateArray(
                std::move(ara::core::StringView("serverTable")),
                [&serverAmount, &etcPath, this](std::size_t, isoft::manifestreader::ManifestNode const& node) noexcept {
                    ++serverAmount;
                    ara::core::String machineShortName;
                    if (node.Load(std::move(ara::core::StringView("machineShortName")), machineShortName) != 0) {
                        return;
                    }
                    ara::core::String softwareClusterShortName;
                    if (node.Load(std::move(ara::core::StringView("softwareClusterShortName")),
                                  softwareClusterShortName)
                        != 0) {
                        return;
                    }

                    ara::core::String const serverCfgPath{etcPath + "/dmc_" + machineShortName + "_"
                                                          + softwareClusterShortName + ".json"};
                    std::shared_ptr< DiagnosticServer > diagnosticServerPtr{std::make_shared< DiagnosticServer >()};
                    common::LogInfo() << "DiagnosticManager::Initialize|begin init "
                                      << softwareClusterShortName.c_str();
                    std::int32_t err = diagnosticServerPtr->Initialize(serverCfgPath, doip_, persistenceFilePtr_);
                    if (err != 0) {
                        common::LogError()
                            << "machineShortName:" << machineShortName
                            << " softwareClusterShortName:" << softwareClusterShortName << " initialize fails!";
                        // isLoadConfigSuccess = false;
                        return;
                    }
                    /// assert(err == 0);
                    serverTable_.push_back(diagnosticServerPtr);
                    common::LogInfo() << "DiagnosticManager::Initialize|end init " << softwareClusterShortName.c_str();
                })
            != 0) {
            common::LogWarn() << "DiagnosticManager::Initialize|serverTable absent";
        }
#ifdef NMODEL
    }
#endif
    // if (!isLoadConfigSuccess) {
    //     common::LogError() << "DiagnosticManager::Initialize|load config fail";
    //     return __LINE__;
    // }

    ThreadPool::GetInstance().Initialize(serverAmount);

    // std::unique_ptr<isoft::manifestreader::Manifest> const dmc{std::move(result).Value()};
    // status = dcm::ServerManager::GetInstance().Initialize(dmc, storage);
    // if (status != 0) {
    //     common::LogWarn() << "DiagnosticManager::Initialize|dcm::ServerManager::Initialize|" << status;
    //     return __LINE__;
    // }

    // status = tp::Manager::GetInstance().Initialize(storage);
    // if (status != 0) {
    //     common::LogWarn() << "DiagnosticManager::Initialize|tp::Manager::Initialize|" << status;
    //     return __LINE__;
    // }

    return 0;
}

/// @brief DM operation
/// @return Operation result code
std::int32_t DiagnosticManager::Run() noexcept
{
    decltype(auto) loop{isoft::naicpp::GlobalGeneralEvLoop::Get()};

    if (!_initNetProxy()) {
        return -1;
    }
    udsServerManager_ = std::make_shared< isoft::uds::server::ServerManager >();
    doip_             = std::make_shared< doip::DoIPServer >(udsServerManager_);
    if (Initialize() != 0) {
        signalManager_.Close();
        serverTable_.clear();
        return __LINE__;
    }
    ThreadPool::GetInstance().Start();

    for (auto&& server : serverTable_) {
        decltype(auto) udsServerPtr{server->GetUdsServer()};
        udsServerPtr->Start();
        udsServerManager_->Register(udsServerPtr);
    }
#ifdef NMODEL
    if (!useNmodel_) {
#endif
        if (!doip_->Initialize()) {
            return __LINE__;
        }

#ifdef NMODEL
    }
#endif
    if (!doip_->Start()) {
        return __LINE__;
    }

    // tp::Manager::GetInstance().Start();
    // dcm::ServerManager::GetInstance().Start();
#ifdef ARA_DIAG_WITH_EXEC
    if (loop->Exec([]() noexcept {
            ara::exec::ExecutionClient const executionClient;
            ara::core::Result< void > const result{
                executionClient.ReportExecutionState(ara::exec::ExecutionState::kRunning)};
            if (!result.HasValue()) {
                common::LogWarn() << "DiagnosticManager::Run|ExecutionState::ReportExecutionState|" << result.Error();
            }
            common::LogVerbose() << "DiagnosticManager::Run|finish ExecutionState report!";
        })
        != 0) {
        common::LogWarn() << "DiagnosticManager::Run|isoft::naicpp::EvLoop::Exec";
    }
#endif
    if (loop->Run(true) == -1) {
        common::LogWarn() << "DiagnosticManager::Run|run event loop| res = -1";
    }
    ThreadPool::GetInstance().Stop();
    // dcm::ServerManager::ReleaseInstance();
    // tp::Manager::ReleaseInstance();
    return 0;
}

bool DiagnosticManager::_initNetProxy()
{
    isoft::ara_fsh::Process const process;
    ara::core::String const etcPath{process.GetEtcDir()};
    ara::core::String const path{etcPath + "/dmd.json"};
    common::LogDebug() << "config path:" << path;

    std::uint32_t appId{};
    ara::core::String unicastIP;
    std::uint16_t unicastPort{};
    ara::core::String multicastIP;
    std::uint16_t multicastPort{};
    std::vector< std::uint16_t > processIdList;
#ifdef NMODEL
    std::string nmodelPath{etcPath.c_str()};
    nmodelPath += "/dmd_config.json";
    auto doc = isoft::nml::Open(nmodelPath.c_str());
    if (doc.get() != nullptr) {
        std::shared_ptr< isoft::nml::definition::DmModule > dmModulePtr = _findDmModule(doc);
        if (dmModulePtr.get() == nullptr) {
            common::LogWarn() << "DiagnosticManager::_initNetProxy| load dm module fails!";
            return false;
        }
        auto networkConfigurationPtr{dmModulePtr->networkConfiguration};
        if (networkConfigurationPtr.get() != nullptr) {
            auto multicastAddressPtr{networkConfigurationPtr->multicastAddressRef.lock()};
            if (multicastAddressPtr != nullptr) {
                auto ipv4Ptr{multicastAddressPtr->ipv4};
                if (ipv4Ptr.get() != nullptr) {
                    if (ipv4Ptr->address.get() != nullptr) {
                        multicastIP = ipv4Ptr->address->c_str();
                    }
                }
            }
            if (networkConfigurationPtr->multicastPort.get() != nullptr) {
                multicastPort = *networkConfigurationPtr->multicastPort;
            }
            auto unicastAddressPtr{networkConfigurationPtr->unicastAddressRef.lock()};
            if (unicastAddressPtr != nullptr) {
                auto ipv4Ptr{unicastAddressPtr->ipv4};
                if (ipv4Ptr.get() != nullptr) {
                    if (ipv4Ptr->address.get() != nullptr) {
                        unicastIP = ipv4Ptr->address->c_str();
                    }
                }
            }
            if (networkConfigurationPtr->unicastPort.get() != nullptr) {
                unicastPort = *networkConfigurationPtr->unicastPort;
            }
        }
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adsoftComponentList{
            _findAdaptiveSoftwareComponent(doc)};
        for (auto&& adsoftComponentPtr : adsoftComponentList) {
            if (adsoftComponentPtr.get() == nullptr) {
                continue;
            }
            for (auto&& portPtr : adsoftComponentPtr->port) {
                if (portPtr.get() == nullptr) {
                    continue;
                }
                if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticCommunicationControlPort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticConditionPort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticDTCInformationPort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticDataPort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticDownloadServicePort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticEcuResetRequestPort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticEventPort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticFileTransferServicePort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticGenericUDSServicePort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticIndicatorPort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticMonitorPort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticOperationCyclePort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticRoutinePort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticSecurityAccessPort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticServiceValidationPort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticUploadServicePort
                    || portPtr->tag == isoft::nml::ModelTag::kDiagnosticAuthenticationPort
                    || portPtr->tag == isoft::nml::ModelTag::kDoipPort) {
                    std::uint16_t processId{Hash16(adsoftComponentPtr->isoftNmlObjectFqn)};
                    processId |= 0x8000;
                    processIdList.emplace_back(processId);
                }
            }
        }
        if (processIdList.empty()) {
            common::LogError() << "DiagnosticManager::_initNetProxy|not find diag application process!";
            return false;
        }
        appId = Hash16(path.c_str());

    } else {
        common::LogDebug() << "DiagnosticManager::_initNetProxy|failed to open model";

#endif
        decltype(auto) result{isoft::manifestreader::OpenManifest(std::move(ara::core::StringView(path.c_str())))};
        if (!result.HasValue()) {
            common::LogWarn() << "DiagnosticManager::Initialize|failed to open configuration|" << result.Error();
            return __LINE__;
        }
        if (result.Value()->Load(std::move(ara::core::StringView("netConfig.appId")), appId) != 0) {
            return false;
        }
        if (result.Value()->Load(std::move(ara::core::StringView("netConfig.unicastIP")), unicastIP) != 0) {
            return false;
        }
        if (result.Value()->Load(std::move(ara::core::StringView("netConfig.unicastPort")), unicastPort) != 0) {
            return false;
        }
        if (result.Value()->Load(std::move(ara::core::StringView("netConfig.multicastIP")), multicastIP) != 0) {
            return false;
        }
        if (result.Value()->Load(std::move(ara::core::StringView("netConfig.multicastPort")), multicastPort) != 0) {
            return false;
        }
        if (result.Value()->Load(std::move(ara::core::StringView("processIdList")), processIdList) != 0) {
            return false;
        }
#ifdef NMODEL
    }
#endif
    char buf[isoft::serialize::kInt32_1024U] = {0};
    sprintf(buf, szSrvSNsiCfgJson, unicastIP.data(), appId, multicastIP.data(), multicastPort, multicastPort,
            multicastPort, 1, multicastIP.data(), multicastPort, multicastIP.data(), unicastPort);
    std::string strJson = buf;
    strJson.erase(std::remove_if(strJson.begin(), strJson.end(), isspace), strJson.end());
    if (!isoft::dm::NetProxy::GetInstance().Init(strJson, std::move(processIdList))) {
        common::LogError() << "init net proxy failed. " << strJson;
        return false;
    }
    isoft::dm::NetProxy::GetInstance().Start(true);
    return true;
}
#ifdef NMODEL

std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > >
DiagnosticManager::_findAdaptiveSoftwareComponent(
    std::shared_ptr< isoft::nml::definition::Document > documentPtr) noexcept
{
    if (documentPtr.get() == nullptr) {
        common::LogWarn() << "DiagnosticManager::_findAdaptiveSoftwareComponent| documentPtr is nullptr!";
        return {};
    }
    std::shared_ptr< isoft::nml::definition::DmModule > dmModulePtr{nullptr};
    dmModulePtr = _findDmModule(documentPtr);
    if (dmModulePtr.get() == nullptr) {
        common::LogWarn() << "DiagnosticManager::_findAdaptiveSoftwareComponent| dmModulePtr is nullptr!";
        return {};
    }
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > list{};
    for (auto&& packagePtr : documentPtr->package) {
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > result{};
        result = _findAdaptiveSoftwareComponent(packagePtr, dmModulePtr);
        if (result.empty()) {
            continue;
        }
        list.insert(result.begin(), result.end());
    }
    return list;
}

std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > >
DiagnosticManager::_findAdaptiveSoftwareComponent(
    std::shared_ptr< isoft::nml::definition::Package > packagePtr,
    std::shared_ptr< isoft::nml::definition::DmModule > dmModulePtr) noexcept
{
    if (packagePtr.get() == nullptr) {
        common::LogWarn() << "DiagnosticManager::_findAdaptiveSoftwareComponent| packagePtr is nullptr!";
        return {};
    }

    if (dmModulePtr.get() == nullptr) {
        common::LogWarn() << "DiagnosticManager::_findAdaptiveSoftwareComponent| dmModulePtr is nullptr!";
        return {};
    }
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > list{};
    for (auto&& machinePtr : packagePtr->machine) {
        if (machinePtr.get() == nullptr) {
            std::string packageName{"unknow"};
            if (packagePtr->shortName.get() != nullptr) {
                packageName = *packagePtr->shortName;
            }
            common::LogDebug() << "DiagnosticManager::_findAdaptiveSoftwareComponent| not find machine! package name:"
                               << packageName;
            continue;
        }
        if (machinePtr->process.empty()) {
            std::string machineName{"unknow"};
            if (machinePtr->shortName.get() != nullptr) {
                machineName = *machinePtr->shortName;
            }
            common::LogDebug() << "DiagnosticManager::_findAdaptiveSoftwareComponent| not find process! machine name:"
                               << machineName;
        }
        for (auto&& processPtr : machinePtr->process) {
            if (processPtr.get() == nullptr) {
                continue;
            }
            if (processPtr->softwareComponent.get() != nullptr) {
                common::LogVerbose() << "DiagnosticManager::_findAdaptiveSoftwareComponent| softwareComponent fqn:"
                                     << processPtr->softwareComponent->isoftNmlObjectFqn;
                list.insert(processPtr->softwareComponent);
            } else {
                std::string processName{"unknow"};
                if (processPtr->shortName.get() != nullptr) {
                    processName = *processPtr->shortName;
                }
                common::LogDebug() << "process name:" << processName << " not find AdaptiveSoftwareComponent";
            }
        }
    }
    for (auto&& childPackagePtr : packagePtr->package) {
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > result{};
        result = _findAdaptiveSoftwareComponent(childPackagePtr, dmModulePtr);
        if (result.empty()) {
            continue;
        }
        list.insert(result.begin(), result.end());
    }
    if (list.empty()) {
        std::string packageName{"unknow"};
        if (packagePtr->shortName.get() != nullptr) {
            packageName = *packagePtr->shortName;
        }
        common::LogWarn()
            << "DiagnosticManager::_findAdaptiveSoftwareComponent| not find AdaptiveSoftwareComponent! package name:"
            << packageName;
    }
    return list;
}

std::shared_ptr< isoft::nml::definition::DmModule > DiagnosticManager::_findDmModule(
    std::shared_ptr< isoft::nml::definition::Document > documentPtr) noexcept
{
    std::shared_ptr< isoft::nml::definition::DmModule > result{};
    if (documentPtr.get() == nullptr) {
        common::LogWarn() << "DiagnosticManager::_findAllUdsServer| documentPtr is nullptr!";
        return {};
    }
    for (auto&& packagePtr : documentPtr->package) {
        result = _findDmModule(packagePtr);
        if (result.get() != nullptr) {
            return result;
        }
    }
    return result;
}

std::shared_ptr< isoft::nml::definition::DmModule > DiagnosticManager::_findDmModule(
    std::shared_ptr< isoft::nml::definition::Package > packagePtr) noexcept
{
    std::shared_ptr< isoft::nml::definition::DmModule > result{nullptr};
    if (packagePtr.get() == nullptr) {
        common::LogWarn() << "DiagnosticManager::_findDmModule| packagePtr is nullptr!";
        return {};
    }
    for (auto&& machine : packagePtr->machine) {
        for (auto&& module : machine->module) {
            if (module->tag == isoft::nml::ModelTag::kDmModule) {
                result = module->dmModule;
                break;
            }
        }
    }
    if (result.get() == nullptr) {
        for (auto&& childPackagePtr : packagePtr->package) {
            result = _findDmModule(childPackagePtr);
            if (result.get() != nullptr) {
                break;
            }
        }
    }
    return result;
}
#endif
}  // namespace dmd
}  // namespace diag
}  // namespace ara
