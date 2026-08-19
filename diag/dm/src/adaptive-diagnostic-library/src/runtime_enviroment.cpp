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
/// @file       runtime_enviroment.cpp
/// @brief      This file provides the implementation of RuntimeEnviroment.
/// @details
/// @date       2022-11-21
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "runtime_enviroment.h"

#include <isoft/ara_fsh/process.h>
#include <isoft/manifestreader/manifest_reader.h>
#ifdef NMODEL
    #include <isoft/nml/definition/adaptive_software_component.h>
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
    #include <isoft/nml/definition/dm_module.h>
    #include <isoft/nml/definition/dm_network_configuration.h>
    #include <isoft/nml/definition/document.h>
    #include <isoft/nml/definition/doip_module.h>
    #include <isoft/nml/definition/doip_port.h>
    #include <isoft/nml/definition/executable.h>
    #include <isoft/nml/definition/ipv4_configuration.h>
    #include <isoft/nml/definition/machine.h>
    #include <isoft/nml/definition/network_endpoint.h>
    #include <isoft/nml/definition/package.h>
    #include <isoft/nml/definition/process.h>
    #include <isoft/nml/definition/uds_condition.h>
    #include <isoft/nml/definition/uds_event.h>
    #include <isoft/nml/definition/uds_fault_memory.h>
    #include <isoft/nml/definition/uds_indicator.h>
    #include <isoft/nml/definition/uds_operation_cycle.h>
    #include <isoft/nml/definition/uds_server.h>
    #include <isoft/nml/model_tag.h>
    #include <isoft/nml/nml.h>
    #include <unistd.h>

    #include "hash/hash.h"
#endif
#include "ara/diag/diag_error_domain.h"
#include "netProxy/net_proxy.h"
#include "nsomeipConfig/nsomeip_config.h"
#include "serialization/serialization/common_data_type.h"
#include "thread_pool/thread_pool.h"
namespace ara {
namespace diag {
namespace internal {
/// @brief RuntimeEnviroment constructor
/// @throws on overflow
RuntimeEnviroment::RuntimeEnviroment()
    : diagLogger_{ara::log::CreateLogger(std::move(ara::core::StringView{"#DIA"}),
                                         std::move(ara::core::StringView{"Diagnostics"}))}
    , registry_{}
    , conversationManager_{}
    , netProxyStart_(false)
{
}

/// @brief RuntimeEnviroment destructor
RuntimeEnviroment::~RuntimeEnviroment()
{
    isoft::ThreadPool::GetInstance().Stop();
    if (netProxyStart_) {
        isoft::dm::NetProxy::GetInstance().Start(false);
    }
}
#ifdef NMODEL
void RuntimeEnviroment::LoadPackage(std::shared_ptr< isoft::nml::definition::Package > packagePtr,
                                    ara::core::String &multicastIP,
                                    std::uint16_t &multicastPort,
                                    std::uint16_t &appid,
                                    std::string &doipModuleFqn) noexcept
{
    if (packagePtr.get() == nullptr) {
        return;
    }
    for (auto &&machinePtr : packagePtr->machine) {
        if (machinePtr.get() == nullptr) {
            continue;
        }
        for (auto &&processPtr : machinePtr->process) {
            if (processPtr.get() == nullptr) {
                continue;
            }

            std::string specifier{""};
            auto executablePtr{processPtr->executableRef.lock()};
            if (executablePtr.get() != nullptr && executablePtr->shortName.get() != nullptr) {
                specifier += *executablePtr->shortName;
                specifier += "/";
            }
            auto softwareComponentPtr{processPtr->softwareComponent};
            if (softwareComponentPtr.get() == nullptr) {
                continue;
            }
            if (softwareComponentPtr->shortName.get() != nullptr) {
                specifier += *softwareComponentPtr->shortName;
                specifier += "/";
                appid = Hash16(softwareComponentPtr->isoftNmlObjectFqn);
                appid |= 0x8000;
            }
            for (auto &&portPtr : softwareComponentPtr->port) {
                if (portPtr.get() == nullptr) {
                    continue;
                }
                ara::core::String portSpecifier{specifier.c_str()};
                InstanceInfo info{};
                if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticCommunicationControlPort) {
                    auto diagnosticCommunicationControlPortPtr{portPtr->diagnosticCommunicationControlPort};
                    if (diagnosticCommunicationControlPortPtr.get() != nullptr) {
                        if (diagnosticCommunicationControlPortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticCommunicationControlPortPtr->shortName->c_str();
                        }
                        info.serviceInstanceId = Hash32(diagnosticCommunicationControlPortPtr->isoftNmlObjectFqn);
                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticConditionPort) {
                    auto diagnosticConditionPortPtr{portPtr->diagnosticConditionPort};
                    if (diagnosticConditionPortPtr.get() != nullptr) {
                        if (diagnosticConditionPortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticConditionPortPtr->shortName->c_str();
                        }
                        auto conditionPtr{diagnosticConditionPortPtr->conditionRef.lock()};
                        if (conditionPtr.get() != nullptr) {
                            info.serviceInstanceId = Hash32(conditionPtr->isoftNmlObjectFqn);
                        }
                        info.instanceId = Hash16(diagnosticConditionPortPtr->isoftNmlObjectFqn);

                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticDTCInformationPort) {
                    auto diagnosticDTCInformationPortPtr{portPtr->diagnosticDTCInformationPort};
                    if (diagnosticDTCInformationPortPtr.get() != nullptr) {
                        if (diagnosticDTCInformationPortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticDTCInformationPortPtr->shortName->c_str();
                        }
                        auto faultMemoryPtr{diagnosticDTCInformationPortPtr->faultMemoryRef.lock()};
                        if (faultMemoryPtr.get() != nullptr) {
                            info.serviceInstanceId = Hash32(faultMemoryPtr->isoftNmlObjectFqn);
                        }
                        info.instanceId = Hash16(diagnosticDTCInformationPortPtr->isoftNmlObjectFqn);
                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticDataPort) {
                    auto diagnosticDataPortPtr{portPtr->diagnosticDataPort};
                    if (diagnosticDataPortPtr.get() != nullptr) {
                        if (diagnosticDataPortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticDataPortPtr->shortName->c_str();
                        }
                        info.serviceInstanceId = Hash32(diagnosticDataPortPtr->isoftNmlObjectFqn);

                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticExternalAuthenticationPort) {
                    auto diagnosticExternalAuthenticationPortPtr{portPtr->diagnosticExternalAuthenticationPort};
                    if (diagnosticExternalAuthenticationPortPtr.get() != nullptr) {
                        if (diagnosticExternalAuthenticationPortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticExternalAuthenticationPortPtr->shortName->c_str();
                        }
                        if (diagnosticExternalAuthenticationPortPtr->udsServerRef.lock() != nullptr) {
                            std::string serverFqn
                                = diagnosticExternalAuthenticationPortPtr->udsServerRef.lock()->isoftNmlObjectFqn;
                            serverFqn += "/ExternalAuthentication";
                            info.serviceInstanceId = Hash32(serverFqn);
                        }
                        info.instanceId = Hash16(diagnosticExternalAuthenticationPortPtr->isoftNmlObjectFqn);

                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticDownloadServicePort) {
                    auto diagnosticDownloadServicePortPtr{portPtr->diagnosticDownloadServicePort};
                    if (diagnosticDownloadServicePortPtr.get() != nullptr) {
                        if (diagnosticDownloadServicePortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticDownloadServicePortPtr->shortName->c_str();
                        }
                        info.serviceInstanceId = Hash32(diagnosticDownloadServicePortPtr->isoftNmlObjectFqn);
                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticEcuResetRequestPort) {
                    auto diagnosticEcuResetRequestPortPtr{portPtr->diagnosticEcuResetRequestPort};
                    if (diagnosticEcuResetRequestPortPtr.get() != nullptr) {
                        if (diagnosticEcuResetRequestPortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticEcuResetRequestPortPtr->shortName->c_str();
                        }

                        info.serviceInstanceId = Hash32(diagnosticEcuResetRequestPortPtr->isoftNmlObjectFqn);
                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticEventPort) {
                    auto diagnosticEventPortPtr{portPtr->diagnosticEventPort};
                    if (diagnosticEventPortPtr.get() != nullptr) {
                        if (diagnosticEventPortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticEventPortPtr->shortName->c_str();
                        }
                        auto eventPtr{diagnosticEventPortPtr->eventRef.lock()};
                        if (eventPtr.get() != nullptr) {
                            std::string service{eventPtr->isoftNmlObjectFqn};
                            service += "/Event";
                            info.serviceInstanceId = Hash32(service);
                        }
                        info.instanceId = Hash16(diagnosticEventPortPtr->isoftNmlObjectFqn);
                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticFileTransferServicePort) {
                    auto diagnosticFileTransferServicePortPtr{portPtr->diagnosticFileTransferServicePort};
                    if (diagnosticFileTransferServicePortPtr.get() != nullptr) {
                        if (diagnosticFileTransferServicePortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticFileTransferServicePortPtr->shortName->c_str();
                        }
                        info.serviceInstanceId = Hash32(diagnosticFileTransferServicePortPtr->isoftNmlObjectFqn);

                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticGenericUDSServicePort) {
                    auto diagnosticGenericUDSServicePortPtr{portPtr->diagnosticGenericUDSServicePort};
                    if (diagnosticGenericUDSServicePortPtr.get() != nullptr) {
                        if (diagnosticGenericUDSServicePortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticGenericUDSServicePortPtr->shortName->c_str();
                        }
                        info.serviceInstanceId = Hash32(diagnosticGenericUDSServicePortPtr->isoftNmlObjectFqn);

                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticIndicatorPort) {
                    auto diagnosticIndicatorPortPtr{portPtr->diagnosticIndicatorPort};
                    if (diagnosticIndicatorPortPtr.get() != nullptr) {
                        if (diagnosticIndicatorPortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticIndicatorPortPtr->shortName->c_str();
                        }
                        auto indicatorPtr{diagnosticIndicatorPortPtr->indicatorRef.lock()};
                        if (indicatorPtr.get() != nullptr) {
                            info.serviceInstanceId = Hash32(indicatorPtr->isoftNmlObjectFqn);
                        }
                        info.instanceId = Hash16(diagnosticIndicatorPortPtr->isoftNmlObjectFqn);
                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticMonitorPort) {
                    auto diagnosticMonitorPortPtr{portPtr->diagnosticMonitorPort};
                    if (diagnosticMonitorPortPtr.get() != nullptr) {
                        if (diagnosticMonitorPortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticMonitorPortPtr->shortName->c_str();
                        }
                        auto eventPtr{diagnosticMonitorPortPtr->eventRef.lock()};
                        if (eventPtr.get() != nullptr) {
                            std::string service{eventPtr->isoftNmlObjectFqn};
                            service += "/Monitor";
                            info.serviceInstanceId = Hash32(service);
                        }
                        info.instanceId = Hash16(diagnosticMonitorPortPtr->isoftNmlObjectFqn);
                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticOperationCyclePort) {
                    auto diagnosticOperationCyclePortPtr{portPtr->diagnosticOperationCyclePort};
                    if (diagnosticOperationCyclePortPtr.get() != nullptr) {
                        if (diagnosticOperationCyclePortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticOperationCyclePortPtr->shortName->c_str();
                        }
                        auto operationCyclePtr{diagnosticOperationCyclePortPtr->operationCycleRef.lock()};
                        if (operationCyclePtr.get() != nullptr) {
                            info.serviceInstanceId = Hash32(operationCyclePtr->isoftNmlObjectFqn);
                        }
                        info.instanceId = Hash16(diagnosticOperationCyclePortPtr->isoftNmlObjectFqn);

                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticRoutinePort) {
                    auto diagnosticRoutinePortPtr{portPtr->diagnosticRoutinePort};
                    if (diagnosticRoutinePortPtr.get() != nullptr) {
                        if (diagnosticRoutinePortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticRoutinePortPtr->shortName->c_str();
                        }
                        info.serviceInstanceId = Hash32(diagnosticRoutinePortPtr->isoftNmlObjectFqn);

                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticSecurityAccessPort) {
                    auto diagnosticSecurityAccessPortPtr{portPtr->diagnosticSecurityAccessPort};
                    if (diagnosticSecurityAccessPortPtr.get() != nullptr) {
                        if (diagnosticSecurityAccessPortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticSecurityAccessPortPtr->shortName->c_str();
                        }

                        info.serviceInstanceId = Hash32(diagnosticSecurityAccessPortPtr->isoftNmlObjectFqn);
                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticServiceValidationPort) {
                    auto diagnosticServiceValidationPortPtr{portPtr->diagnosticServiceValidationPort};
                    if (diagnosticServiceValidationPortPtr.get() != nullptr) {
                        if (diagnosticServiceValidationPortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticServiceValidationPortPtr->shortName->c_str();
                        }

                        info.serviceInstanceId = Hash32(diagnosticServiceValidationPortPtr->isoftNmlObjectFqn);

                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticUploadServicePort) {
                    auto diagnosticUploadServicePortPtr{portPtr->diagnosticUploadServicePort};
                    if (diagnosticUploadServicePortPtr.get() != nullptr) {
                        if (diagnosticUploadServicePortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticUploadServicePortPtr->shortName->c_str();
                        }
                        info.serviceInstanceId = Hash32(diagnosticUploadServicePortPtr->isoftNmlObjectFqn);

                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDoipPort) {
                    auto doipPortPtr{portPtr->doipPort};
                    if (doipPortPtr.get() != nullptr) {
                        if (doipPortPtr->shortName.get() != nullptr) {
                            portSpecifier += doipPortPtr->shortName->c_str();
                        }
                        if (doipPortPtr->interface.get() != nullptr) {
                            if (*doipPortPtr
                                     ->interface == isoft::nml::definition::DoipInterfaceEnum::kDiagnosticDoIpTriggerVehicleAnnouncementInterface) {
                                std::string const defaultServiceName{".TriggerVehicleAnnouncement"};
                                std::string serviceInstance{doipModuleFqn};
                                portSpecifier = "DefaultDoIpTriggerVehicleAnnouncementInstanceSpecifier";
                                serviceInstance += defaultServiceName;
                                info.serviceInstanceId = Hash32(serviceInstance);
                                info.instanceId        = appid;
                            } else {
                                info.serviceInstanceId = Hash32(doipPortPtr->isoftNmlObjectFqn);
                            }
                        }
                        registry_.insert({portSpecifier, info});
                    }
                } else if (portPtr->tag == isoft::nml::ModelTag::kDiagnosticAuthenticationPort) {
                    auto diagnosticAuthenticationPortPtr{portPtr->diagnosticAuthenticationPort};
                    if (diagnosticAuthenticationPortPtr.get() != nullptr) {
                        if (diagnosticAuthenticationPortPtr->shortName.get() != nullptr) {
                            portSpecifier += diagnosticAuthenticationPortPtr->shortName->c_str();
                        }
                        info.serviceInstanceId = Hash32(diagnosticAuthenticationPortPtr->isoftNmlObjectFqn);

                        registry_.insert({portSpecifier, info});
                    }
                }
            }
        }
    }

    for (auto &&machinePtr : packagePtr->machine) {
        if (machinePtr.get() == nullptr) {
            continue;
        }
        for (auto &&modulePtr : machinePtr->module) {
            if (modulePtr.get() == nullptr) {
                continue;
            }
            if (modulePtr->tag == isoft::nml::ModelTag::kDmModule) {
                auto dmModulePtr{modulePtr->dmModule};
                if (dmModulePtr.get() == nullptr) {
                    continue;
                }
                auto networkConfigurationPtr{dmModulePtr->networkConfiguration};
                if (networkConfigurationPtr.get() == nullptr) {
                    continue;
                }
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
            }
        }
    }
    for (auto &&childPackagePtr : packagePtr->package) {
        LoadPackage(childPackagePtr, multicastIP, multicastPort, appid, doipModuleFqn);
    }
    return;
}

std::string RuntimeEnviroment::GetDoipModuleFqn(std::shared_ptr< isoft::nml::definition::Package > packagePtr) noexcept
{
    std::string doipModuleFqn{};
    if (packagePtr.get() == nullptr) {
        return doipModuleFqn;
    }
    for (auto &&machinePtr : packagePtr->machine) {
        if (machinePtr.get() == nullptr) {
            continue;
        }
        for (auto &&modulePtr : machinePtr->module) {
            if (modulePtr.get() == nullptr) {
                continue;
            }
            if (modulePtr->tag == isoft::nml::ModelTag::kDmModule) {
                auto dmModulePtr{modulePtr->dmModule};
                if (dmModulePtr.get() == nullptr) {
                    continue;
                }
                if (dmModulePtr->doipModule.get() != nullptr) {
                    doipModuleFqn = dmModulePtr->doipModule->isoftNmlObjectFqn;
                    return doipModuleFqn;
                }
            }
        }
    }
    for (auto &&childPackagePtr : packagePtr->package) {
        doipModuleFqn = GetDoipModuleFqn(childPackagePtr);
        if (!doipModuleFqn.empty()) {
            break;
        }
    }
    return doipModuleFqn;
}
#endif
/// @brief Runtime environment initialization
/// @return Initialization result
/// @throws on overflow
ara::core::Result< void > RuntimeEnviroment::Initialize()
{
    isoft::ara_fsh::Process const process;
    ara::core::String const etcPath{process.GetEtcDir()};
    ara::core::String const path{etcPath + "/dapi.json"};
    std::uint16_t appId{};

    ara::core::String multicastIP;
    std::uint16_t multicastPort{};
#ifdef NMODEL
    std::string nmodelPath{etcPath.c_str()};
    nmodelPath += "/diag.json";
    auto doc = isoft::nml::Open(nmodelPath);
    if (doc.get() != nullptr) {
        std::string doipModuleFqn{};
        for (auto &&packagePtr : doc->package) {
            doipModuleFqn = GetDoipModuleFqn(packagePtr);
            if (!doipModuleFqn.empty()) {
                break;
            }
        }

        for (auto &&packagePtr : doc->package) {
            LoadPackage(packagePtr, multicastIP, multicastPort, appId, doipModuleFqn);
        }
        if (multicastIP.empty()) {
            diagLogger_.LogWarn()
                << "RuntimeEnviroment::Initialize|multicastIP field is empty! please check diag.json file";
            multicastIP = "239.0.0.1";
        }
        if (multicastPort == 0) {
            diagLogger_.LogWarn()
                << "RuntimeEnviroment::Initialize|multicastPort field is empty! please check diag.json file";
            multicastPort = 30723;
        }
    } else {
#endif
        ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > const result{
            isoft::manifestreader::OpenManifest(std::move(ara::core::StringView{path.data()}))};
        if (!result.HasValue()) {
            diagLogger_.LogWarn() << "RuntimeEnviroment::Initialize|isoft::manifestreader::OpenManifest|"
                                  << result.Error();
            return {};
        }
        std::function< void(std::size_t, isoft::manifestreader::ManifestNode const &) > callback{
            [&](std::size_t const idx, isoft::manifestreader::ManifestNode const &node) {
                ara::core::String specifier;
                InstanceInfo info{};
                if ((node.Load(std::move(ara::core::StringView{"specifier"}), specifier) == 0)
                    && (node.Load(std::move(ara::core::StringView{"clientInstanceId"}), info.instanceId) == 0)
                    && (node.Load(std::move(ara::core::StringView{"serviceInstanceId"}), info.serviceInstanceId)
                        == 0)) {
                    std::ignore = registry_.emplace(specifier, info);
                } else {
                    diagLogger_.LogWarn() << "RuntimeEnviroment::Initialize|failed to parse node" << idx;
                }
            }};
        int32_t const status{std::move(result.Value()->IterateArray(
            std::move(ara::core::StringView{"diagnosticInterface"}), std::move(callback)))};
        if (status != 0) {
            diagLogger_.LogError() << "RuntimeEnviroment::Initialize|ManifestNode::IterateArray|" << status;
            return ara::core::Result< void >::FromError(MakeErrorCode(DiagErrc::kGenericError, __LINE__));
        }

        if (result.Value()->Load(std::move(ara::core::StringView("netConfig.appId")), appId) != 0) {
            diagLogger_.LogWarn() << "RuntimeEnviroment::Initialize|Load [netConfig.appId] failed.|" << result.Error();
            return {};
        }
        if (result.Value()->Load(std::move(ara::core::StringView("netConfig.multicastIP")), multicastIP) != 0) {
            diagLogger_.LogWarn() << "RuntimeEnviroment::Initialize|Load [netConfig.multicastIP] failed.|"
                                  << result.Error();
            return {};
        }
        if (result.Value()->Load(std::move(ara::core::StringView("netConfig.multicastPort")), multicastPort) != 0) {
            diagLogger_.LogWarn() << "RuntimeEnviroment::Initialize|Load [netConfig.multicastPort] failed.|"
                                  << result.Error();
            return {};
        }
#ifdef NMODEL
    }

#endif
    const uint32_t bufLen{1024};
    char buf[bufLen] = {0};
    sprintf(buf, szCltSNsiCfgJson, appId, appId, multicastIP.data(), multicastPort);
    std::string strJson = buf;
    strJson.erase(std::remove_if(strJson.begin(), strJson.end(), isspace), strJson.end());

    if (!isoft::dm::NetProxy::GetInstance().Init(strJson)) {
        LogError() << "init net proxy failed. " << strJson;
    }
    isoft::dm::NetProxy::GetInstance().Start(true);
    netProxyStart_ = true;
    conversationManager_.Initialize();
    return {};
}

}  // namespace internal
}  // namespace diag
}  // namespace ara
