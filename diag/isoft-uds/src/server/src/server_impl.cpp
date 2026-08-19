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
/// @file       server_impl.cpp
/// @brief      This file implements server_impl functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=src
/// @trace_id_sr=0ff53c6f-8e31-4351-8da4-0934285c7506
/// @unit_name=ServerImpl
/// @unit_description=This file provides server_impl functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "server_impl.h"

#include <sstream>

#include "authentication_management/client_authentication_controller_impl.h"
#include "authentication_management/service_x29.h"
#include "com_management/service_x28.h"
#include "common/log.h"
#include "common/serialize.h"
#include "common/thread_pool.h"
#include "custom_management/service_custom.h"
#include "data_management/service_x2a.h"
#include "data_management/service_x2c.h"
#include "data_management/service_x2e.h"
#include "ecu_reset_management/service_x11.h"
#include "fault_management/service_x14.h"
#include "fault_management/service_x19.h"
#include "fault_management/service_x85.h"
#include "isoft/uds/transport.h"
#include "isoft/uds/type_int.h"
#include "roe_management/service_x86.h"
#include "routine_management/service_x31.h"
#include "security_access_management/service_x27.h"
#include "session_management/service_x10.h"
#include "transfer_managment/service_x34.h"
#include "transfer_managment/service_x35.h"
#include "transfer_managment/service_x36.h"
#include "transfer_managment/service_x37.h"
#include "transfer_managment/service_x38.h"
namespace isoft {
namespace uds {
namespace server {
ServerImpl::ServerImpl() noexcept
    : serviceValidationServicePtr_{std::make_unique< ServiceValidationService >()}
    , sessionManagerPtr_{std::make_shared< SessionManager >()}
    , securityLevelManagerPtr_{std::make_shared< SecurityLevelManager >()}
    , transferManagerPtr_{std::make_shared< TransferManager >()}
{
}
bool ServerImpl::Initialize(ServerSetting& config)
{
    config_                = config;
    physicalAddress_       = config.physicalAddress;
    functionalAddress_     = config.functionAddressTable;
    persistenceServicePtr_ = std::make_shared< PersistenceService >(physicalAddress_);
    for (auto&& serviceValidationPtr : config.manufacturerServiceValidationTable) {
        AddManufacturerServiceValidation(serviceValidationPtr);
    }
    for (auto&& serviceValidationPtr : config.supplierServiceValidationTable) {
        AddSupplierServiceValidation(serviceValidationPtr);
    }
    std::ignore
        = s3Timer_.Initialize([this]() { std::ignore = ThreadPool::GetInstance().Submit([this]() { _s3Timeout(); }); });
    return true;
}

bool ServerImpl::Initialize(SessionMangement& config) noexcept
{
    std::int32_t result{0};

    result = sessionManagerPtr_->Initialize(config.sessionConfigTable);
    if (result != 0) {
        LogWarn() << "session Manager Initialize fails,error:" << result;
        return false;
    }
    NotifyOfSessionChangeCallback sessionChangeCallback{[this](SessionId oldSessionId, SessionId newSessionId) {
        _updateNonDefaultPriority(oldSessionId, newSessionId);
    }};
    std::ignore = sessionManagerPtr_->AddNotifyOfSessionChange(sessionChangeCallback);
    if (config.sessionControlInstanceTable.empty()) {
        LogWarn() << "session control instance is empty! Please config session control instance!";
        return true;
    }
    std::shared_ptr< ServiceX10 > serviceX10{nullptr};
    serviceX10 = std::make_shared< ServiceX10 >();
    if (serviceX10.get() == nullptr) {
        LogWarn() << "Failed to allocate memory,Please check memory";
        return false;
    }
    result = serviceX10->Initialize(config.sessionControlInstanceTable, authenticationManagerPtr_, sessionManagerPtr_,
                                    securityLevelManagerPtr_, config.p4ServerMax);
    if (result != 0) {
        LogWarn() << "serviceX10 Initialize fails,error:" << result;
        return false;
    }
    auto findServiceX10Result = protocolTable_.find(kSessionControlId);
    if (findServiceX10Result != protocolTable_.end()) {
        protocolTable_[kSessionControlId] = serviceX10;
    } else {
        std::ignore = protocolTable_.insert({kSessionControlId, serviceX10});
    }
    return true;
}

bool ServerImpl::Initialize(SecurityAccessManagement& config) noexcept
{
    std::int32_t result{0};

    result = securityLevelManagerPtr_->Initialize(config.securityLevelTable, sessionManagerPtr_);
    if (result != 0) {
        LogWarn() << "security level Manager Initialize fails,error:" << result;
        return false;
    }
    if (config.securityAccessInstanceTable.empty()) {
        LogWarn() << "security access instance is empty! Please config security access instance!";
        return false;
    }
    std::shared_ptr< ServiceX27 > serviceX27Ptr{nullptr};
    serviceX27Ptr = std::make_shared< ServiceX27 >(persistenceServicePtr_);
    if (serviceX27Ptr.get() == nullptr) {
        LogWarn() << "Failed to allocate memory,Please check memory";
        return false;
    }
    result = serviceX27Ptr->Initialize(config.securityAccessInstanceTable, config.securityDelayTimeOnBoot,
                                       config.sharedTimer, authenticationManagerPtr_, sessionManagerPtr_,
                                       securityLevelManagerPtr_, config.p4ServerMax);
    if (result != 0) {
        LogWarn() << "serviceX27Ptr Initialize fails,error:" << result;
        return false;
    }
    auto findServiceX27Result = protocolTable_.find(kSecurityAccessId);
    if (findServiceX27Result != protocolTable_.end()) {
        protocolTable_[kSecurityAccessId] = serviceX27Ptr;
    } else {
        std::ignore = protocolTable_.insert({kSecurityAccessId, serviceX27Ptr});
    }
    return true;
}

bool ServerImpl::Initialize(DiagnosticDataManagement& config) noexcept
{
    std::int32_t result{0};
    std::shared_ptr< DiagnosticDataManager > diagnosticDataManagerPtr{
        std::make_shared< DiagnosticDataManager >(persistenceServicePtr_)};
    result = diagnosticDataManagerPtr->Initialize(config.didManager, sessionManagerPtr_, securityLevelManagerPtr_);
    if (result != 0) {
        LogWarn() << "diagnostic data Manager Initialize fails,error:" << result;
        return false;
    }
    diagnosticDataManagerPtr_ = diagnosticDataManagerPtr;

    std::shared_ptr< EnvironmentalConditionManager > envConditionManagerPtr{
        std::make_shared< EnvironmentalConditionManager >()};
    if (!config.environmentalConditionManager.empty()) {
        result = envConditionManagerPtr->Initialize(config.environmentalConditionManager, diagnosticDataManagerPtr_);
        if (result != 0) {
            LogWarn() << "env condition Manager Initialize fails,error:" << result;
            return false;
        }
        envConditionManagerPtr_ = envConditionManagerPtr;
    }
    std::shared_ptr< ServiceX22 > serviceX22Ptr{nullptr};
    if (config.service.serviceX22.get() != nullptr) {
        serviceX22Ptr = std::make_shared< ServiceX22 >();
        if (serviceX22Ptr.get() == nullptr) {
            LogWarn() << "Failed to allocate memory,Please check memory";
            return false;
        }
        result
            = serviceX22Ptr->Initialize(*(config.service.serviceX22), authenticationManagerPtr_, sessionManagerPtr_,
                                        securityLevelManagerPtr_, envConditionManagerPtr_, diagnosticDataManagerPtr_);
        if (result != 0) {
            LogWarn() << "serviceX22Ptr Initialize fails,error:" << result;
            return false;
        }
        auto findServiceX22Result = protocolTable_.find(kReadDataByIdentifierId);
        if (findServiceX22Result != protocolTable_.end()) {
            protocolTable_[kReadDataByIdentifierId] = serviceX22Ptr;
        } else {
            std::ignore = protocolTable_.insert({kReadDataByIdentifierId, serviceX22Ptr});
        }
    }

    std::shared_ptr< ServiceX2E > serviceX2EPtr{nullptr};
    if (config.service.serviceX2E.get() != nullptr) {
        serviceX2EPtr = std::make_shared< ServiceX2E >();
        if (serviceX2EPtr.get() == nullptr) {
            LogWarn() << "Failed to allocate memory,Please check memory";
            return false;
        }
        result
            = serviceX2EPtr->Initialize(*config.service.serviceX2E, authenticationManagerPtr_, sessionManagerPtr_,
                                        securityLevelManagerPtr_, envConditionManagerPtr_, diagnosticDataManagerPtr_);
        if (result != 0) {
            LogWarn() << "serviceX2EPtr Initialize fails,error:" << result;
            return false;
        }
        auto findServiceX2EResult = protocolTable_.find(kWriteDataByIdentifierId);
        if (findServiceX2EResult != protocolTable_.end()) {
            protocolTable_[kWriteDataByIdentifierId] = serviceX2EPtr;
        } else {
            std::ignore = protocolTable_.insert({kWriteDataByIdentifierId, serviceX2EPtr});
        }
    }
    if (config.service.serviceX2C.get() != nullptr) {
        std::shared_ptr< ServiceX2C > serviceX2CPtr{nullptr};
        serviceX2CPtr = std::make_shared< ServiceX2C >();
        if (serviceX2CPtr.get() == nullptr) {
            LogWarn() << "Failed to allocate memory,Please check memory";
            return false;
        }
        result = serviceX2CPtr->Initialize(*config.service.serviceX2C, serviceX22Ptr, authenticationManagerPtr_,
                                           sessionManagerPtr_, securityLevelManagerPtr_, diagnosticDataManagerPtr_);
        if (result != 0) {
            LogWarn() << "serviceX2CPtr Initialize fails,error:" << result;
            return false;
        }
        auto findServiceX2CResult = protocolTable_.find(kDynamicallyDefineDataIdentifierId);
        if (findServiceX2CResult != protocolTable_.end()) {
            protocolTable_[kDynamicallyDefineDataIdentifierId] = serviceX2CPtr;
        } else {
            std::ignore = protocolTable_.insert({kDynamicallyDefineDataIdentifierId, serviceX2CPtr});
        }
    }
    if (config.service.serviceX2A.get() != nullptr) {
        std::shared_ptr< ServiceX2A > serviceX2APtr{nullptr};
        serviceX2APtr = std::make_shared< ServiceX2A >();
        if (serviceX2APtr.get() == nullptr) {
            LogWarn() << "Failed to allocate memory,Please check memory";
            return false;
        }
        result = serviceX2APtr->Initialize(*config.service.serviceX2A, serviceX22Ptr, sessionManagerPtr_,
                                           securityLevelManagerPtr_, diagnosticDataManagerPtr_);
        if (result != 0) {
            LogWarn() << "serviceX2APtr Initialize fails,error:" << result;
            return false;
        }
        auto findServiceX2AResult = protocolTable_.find(kReadDataByPeriodicIdentifierId);
        if (findServiceX2AResult != protocolTable_.end()) {
            protocolTable_[kReadDataByPeriodicIdentifierId] = serviceX2APtr;
        } else {
            std::ignore = protocolTable_.insert({kReadDataByPeriodicIdentifierId, serviceX2APtr});
        }
    }

    return true;
}

bool ServerImpl::Initialize(EcuResetManagement& config) noexcept
{
    std::shared_ptr< ServiceX11 > serviceX11Ptr{nullptr};
    serviceX11Ptr
        = std::make_shared< ServiceX11 >(persistenceServicePtr_, [this](ServiceX11::BlockState const state) noexcept {
              if (state == ServiceX11::BlockState::kBlocked) {
                  blockInCaseEcuResetProcess_ = true;
              } else {
                  blockInCaseEcuResetProcess_ = false;
              }
          });
    if (serviceX11Ptr.get() == nullptr) {
        LogWarn() << "Failed to allocate memory,Please check memory";
        return false;
    }
    std::int32_t result{};
    result = serviceX11Ptr->Initialize(config, authenticationManagerPtr_, sessionManagerPtr_, securityLevelManagerPtr_);
    if (result != 0) {
        LogWarn() << "serviceX11Ptr Initialize fails,error:" << result;
        return false;
    }
    auto findServiceX11Result = protocolTable_.find(kEcuResetId);
    if (findServiceX11Result != protocolTable_.end()) {
        protocolTable_[kEcuResetId] = serviceX11Ptr;
    } else {
        std::ignore = protocolTable_.insert({kEcuResetId, serviceX11Ptr});
    }
    return true;
}

bool ServerImpl::Initialize(FaultManagement& config) noexcept
{
    std::int32_t res
        = faultManager_.Initialize(config, persistenceServicePtr_->GetPersistenceInstance(), diagnosticDataManagerPtr_);
    if (res != 0) {
        LogError() << "ServerImpl Initialize| init fault fails res =" << res;
        return false;
    }

    if (!config.controlDTCSettingInstanceConfig.instances.empty()) {
        std::shared_ptr< ServiceX85 > serviceX85 = std::make_shared< ServiceX85 >(faultManager_);
        res = serviceX85->Initialize(config.controlDTCSettingInstanceConfig, authenticationManagerPtr_,
                                     sessionManagerPtr_, securityLevelManagerPtr_);
        if (res != 0) {
            LogError() << "ServerImpl Initialize| init serviceX85 fails res =" << res;
            return false;
        }
        protocolTable_[kControlDTCSettingId] = serviceX85;
    } else {
        LogWarn() << "ServerImpl::Initialize|no config 0x85";
    }

    if (!config.clearDiagnosticInformationInstanceConfig.accessPermissionSession.empty()) {
        std::shared_ptr< ServiceX14 > serviceX14 = std::make_shared< ServiceX14 >(faultManager_);
        res = serviceX14->Initialize(config.clearDiagnosticInformationInstanceConfig, authenticationManagerPtr_,
                                     sessionManagerPtr_, securityLevelManagerPtr_);
        if (res != 0) {
            LogError() << "ServerImpl Initialize| init serviceX14 fails res =" << res;
            return false;
        }
        protocolTable_[kClearInformationId] = serviceX14;
    } else {
        LogWarn() << "ServerImpl::Initialize|no config 0x14";
    }

    if (!config.readDTCInformationInstanceConfig.instances.empty()) {
        std::shared_ptr< ServiceX19 > serviceX19 = std::make_shared< ServiceX19 >(faultManager_);
        res = serviceX19->Initialize(config.readDTCInformationInstanceConfig, authenticationManagerPtr_,
                                     sessionManagerPtr_, securityLevelManagerPtr_);
        if (res != 0) {
            LogError() << "ServerImpl Initialize| init serviceX19 fails res =" << res;
            return false;
        }
        protocolTable_[kReadDtcInformationId] = serviceX19;
    } else {
        LogWarn() << "ServerImpl::Initialize|no config 0x19";
    }

    LogInfo() << "ServerImpl Initialize| init fault success";
    return true;
}

bool ServerImpl::Initialize(ComManagementConfig& config) noexcept
{
    std::shared_ptr< ServiceX28 > serviceX28Ptr{nullptr};
    serviceX28Ptr = std::make_shared< ServiceX28 >();
    if (serviceX28Ptr.get() == nullptr) {
        LogWarn() << "Failed to allocate memory,Please check memory";
        return false;
    }
    std::int32_t result{};
    result = serviceX28Ptr->Initialize(config, authenticationManagerPtr_, sessionManagerPtr_, securityLevelManagerPtr_);
    if (result != 0) {
        LogWarn() << "serviceX28Ptr Initialize fails,error:" << result;
        return false;
    }
    auto findServiceX28Result = protocolTable_.find(kCommunicationControlId);
    if (findServiceX28Result != protocolTable_.end()) {
        protocolTable_[kCommunicationControlId] = serviceX28Ptr;
    } else {
        std::ignore = protocolTable_.insert({kCommunicationControlId, serviceX28Ptr});
    }
    return true;
}

bool ServerImpl::Initialize(RoutineManagement& config) noexcept
{
    std::shared_ptr< ServiceX31 > serviceX31Ptr{nullptr};
    serviceX31Ptr = std::make_shared< ServiceX31 >();
    if (serviceX31Ptr.get() == nullptr) {
        LogWarn() << "Failed to allocate memory,Please check memory";
        return false;
    }
    std::int32_t result{};
    result = serviceX31Ptr->Initialize(config, authenticationManagerPtr_, sessionManagerPtr_, securityLevelManagerPtr_);
    if (result != 0) {
        LogWarn() << "serviceX31Ptr Initialize fails,error:" << result;
        return false;
    }
    auto findServiceX31Result = protocolTable_.find(kRoutineControlId);
    if (findServiceX31Result != protocolTable_.end()) {
        protocolTable_[kRoutineControlId] = serviceX31Ptr;
    } else {
        std::ignore = protocolTable_.insert({kRoutineControlId, serviceX31Ptr});
    }
    return true;
}

bool ServerImpl::Initialize(TransferManagment& config) noexcept
{
    std::int32_t result{0};
    result = transferManagerPtr_->Initialize(config, authenticationManagerPtr_, sessionManagerPtr_,
                                             securityLevelManagerPtr_, envConditionManagerPtr_);
    if (result != 0) {
        LogWarn() << "transfer Manager Initialize fails,error:" << result;
        return false;
    }
    if (config.requestDownload.get() != nullptr) {
        std::shared_ptr< ServiceX34 > serviceX34Ptr{nullptr};
        serviceX34Ptr = std::make_shared< ServiceX34 >();
        if (serviceX34Ptr.get() == nullptr) {
            LogWarn() << "Failed to allocate memory,Please check memory";
            return false;
        }
        result = serviceX34Ptr->Initialize(transferManagerPtr_, config.requestDownload->p4ServerMax);
        if (result != 0) {
            LogWarn() << "serviceX34Ptr Initialize fails,error:" << result;
            return false;
        }
        std::uint8_t const sid{0x34};
        auto findServiceX34Result = protocolTable_.find(sid);
        if (findServiceX34Result != protocolTable_.end()) {
            protocolTable_[sid] = serviceX34Ptr;
        } else {
            std::ignore = protocolTable_.insert({sid, serviceX34Ptr});
        }
    }

    if (config.requestUpload.get() != nullptr) {
        std::shared_ptr< ServiceX35 > serviceX35Ptr{nullptr};
        serviceX35Ptr = std::make_shared< ServiceX35 >();
        if (serviceX35Ptr.get() == nullptr) {
            LogWarn() << "Failed to allocate memory,Please check memory";
            return false;
        }
        result = serviceX35Ptr->Initialize(transferManagerPtr_, config.requestUpload->p4ServerMax);
        if (result != 0) {
            LogWarn() << "serviceX35Ptr Initialize fails,error:" << result;
            return false;
        }
        std::uint8_t const sid{0x35};
        auto findServiceX35Result = protocolTable_.find(sid);
        if (findServiceX35Result != protocolTable_.end()) {
            protocolTable_[sid] = serviceX35Ptr;
        } else {
            std::ignore = protocolTable_.insert({sid, serviceX35Ptr});
        }
    }
    if (config.requestFileTransfer.get() != nullptr) {
        std::shared_ptr< ServiceX38 > serviceX38Ptr{nullptr};
        serviceX38Ptr = std::make_shared< ServiceX38 >();
        if (serviceX38Ptr.get() == nullptr) {
            LogWarn() << "Failed to allocate memory,Please check memory";
            return false;
        }
        result = serviceX38Ptr->Initialize(transferManagerPtr_, config.requestFileTransfer->p4ServerMax);
        if (result != 0) {
            LogWarn() << "serviceX38Ptr Initialize fails,error:" << result;
            return false;
        }
        std::uint8_t const sid{0x38};
        auto findServiceX38Result = protocolTable_.find(sid);
        if (findServiceX38Result != protocolTable_.end()) {
            protocolTable_[sid] = serviceX38Ptr;
        } else {
            std::ignore = protocolTable_.insert({sid, serviceX38Ptr});
        }
    }
    if (config.requestDownload.get() != nullptr || config.requestUpload.get() != nullptr
        || config.requestFileTransfer.get() != nullptr) {
        std::shared_ptr< ServiceX36 > serviceX36Ptr{nullptr};
        serviceX36Ptr = std::make_shared< ServiceX36 >();
        if (serviceX36Ptr.get() == nullptr) {
            LogWarn() << "Failed to allocate memory,Please check memory";
            return false;
        }
        result = serviceX36Ptr->Initialize(transferManagerPtr_, config);
        if (result != 0) {
            LogWarn() << "serviceX36Ptr Initialize fails,error:" << result;
            return false;
        }
        {
            std::uint8_t const sid{0x36};
            auto findServiceX36Result = protocolTable_.find(sid);
            if (findServiceX36Result != protocolTable_.end()) {
                protocolTable_[sid] = serviceX36Ptr;
            } else {
                std::ignore = protocolTable_.insert({sid, serviceX36Ptr});
            }
        }
        std::shared_ptr< ServiceX37 > serviceX37Ptr{nullptr};
        serviceX37Ptr = std::make_shared< ServiceX37 >();
        if (serviceX37Ptr.get() == nullptr) {
            LogWarn() << "Failed to allocate memory,Please check memory";
            return false;
        }
        result = serviceX37Ptr->Initialize(transferManagerPtr_, config);
        if (result != 0) {
            LogWarn() << "serviceX37Ptr Initialize fails,error:" << result;
            return false;
        }
        std::uint8_t const sid{0x37};
        auto findServiceX37Result = protocolTable_.find(sid);
        if (findServiceX37Result != protocolTable_.end()) {
            protocolTable_[sid] = serviceX37Ptr;
        } else {
            std::ignore = protocolTable_.insert({sid, serviceX37Ptr});
        }
    }

    return true;
}

bool ServerImpl::Initialize(RoeManagement& config) noexcept
{
    std::int32_t result{0};
    std::shared_ptr< DiagnosticRoeManager > diagnosticRoeManagerPtr{std::make_shared< DiagnosticRoeManager >()};
    std::shared_ptr< DtcFilter > dtcFilterPtr{faultManager_.GetReadDtcInformationInterface()};
    result = diagnosticRoeManagerPtr->Initialize(config, sessionManagerPtr_, diagnosticDataManagerPtr_, dtcFilterPtr,
                                                 persistenceServicePtr_);
    diagnosticRoeManagerPtr->SetGetChannelCallback(
        [this](Address client) { return config_.transportPtr->GetChannel(physicalAddress_, client); });
    diagnosticRoeManagerPtr->SetIndicateCallback([this](std::shared_ptr< Message > const& request, Priority priority,
                                                        std::shared_ptr< Channel > const& channel) mutable {
        Result< void > indicateResult = Indicate(request, priority, channel);
        if (indicateResult.HasValue()) {
            HandleMessage(request, priority, channel);
        } else {
            LogWarn() << "ServerImpl::Initialize| indicate fails!";
        }
        return indicateResult;
    });
    if (result != 0) {
        LogWarn() << "diagnostic roe Manager Initialize fails,error:" << result;
        return false;
    }
    diagnosticRoeManagerPtr_ = diagnosticRoeManagerPtr;
    std::shared_ptr< ServiceX86 > serviceX86Ptr{nullptr};
    serviceX86Ptr = std::make_shared< ServiceX86 >();
    if (serviceX86Ptr.get() == nullptr) {
        LogWarn() << "Failed to allocate memory,Please check memory";
        return false;
    }
    result = serviceX86Ptr->Initialize(config, authenticationManagerPtr_, sessionManagerPtr_, securityLevelManagerPtr_,
                                       diagnosticRoeManagerPtr_);
    if (result != 0) {
        LogWarn() << "serviceX86Ptr Initialize fails,error:" << result;
        return false;
    }
    auto findServiceX86Result = protocolTable_.find(kResponseOnEventId);
    if (findServiceX86Result != protocolTable_.end()) {
        protocolTable_[kResponseOnEventId] = serviceX86Ptr;
    } else {
        std::ignore = protocolTable_.insert({kResponseOnEventId, serviceX86Ptr});
    }
    return true;
}

bool ServerImpl::Initialize(CustomManagement& config) noexcept
{
    if (config.table.empty()) {
        LogError() << "ServerImpl::Initialize| CustomManagement.table is empty!";
        return false;
    }
    for (auto&& customConfig : config.table) {
        if (customConfig.get() == nullptr) {
            LogError() << "CustomInstancePtr is nullptr!";
            return false;
        }
        std::shared_ptr< ServiceCustom > serviceCustomPtr{nullptr};
        serviceCustomPtr = std::make_shared< ServiceCustom >();
        if (serviceCustomPtr.get() == nullptr) {
            LogWarn() << "Failed to allocate memory,Please check memory";
            return false;
        }
        std::int32_t result{};
        result = serviceCustomPtr->Initialize(*customConfig, authenticationManagerPtr_, sessionManagerPtr_,
                                              securityLevelManagerPtr_);
        if (result != 0) {
            LogWarn() << "serviceCustomPtr Initialize fails,error:" << result;
            return false;
        }
        auto findServiceCustomResult = protocolTable_.find(customConfig->nSid);
        if (findServiceCustomResult != protocolTable_.end()) {
            protocolTable_[customConfig->nSid] = serviceCustomPtr;
        } else {
            std::ignore = protocolTable_.insert({customConfig->nSid, serviceCustomPtr});
        }
    }

    return true;
}

bool ServerImpl::Initialize(AuthenticationManagement& config) noexcept
{
    std::int32_t result{0};
    std::shared_ptr< AuthenticationManager > authenticationManagerPtr{std::make_shared< AuthenticationManager >()};
    result = authenticationManagerPtr->Initialize(sessionManagerPtr_, config.authRoleList, config.authenticationTimeout,
                                                  config.externalAuthenticationList);
    if (result != 0) {
        LogWarn() << "authentication Manager Initialize fails,error:" << result;
        return false;
    }
    authenticationManagerPtr_ = authenticationManagerPtr;

    std::shared_ptr< ServiceX29 > serviceX29{nullptr};
    serviceX29 = std::make_shared< ServiceX29 >();
    if (serviceX29.get() == nullptr) {
        LogWarn() << "Failed to allocate memory,Please check memory";
        return false;
    }
    result = serviceX29->Initialize(config.transmitCertificate, config.interface, authenticationManagerPtr_);
    if (result != 0) {
        LogWarn() << "serviceX10 Initialize fails,error:" << result;
        return false;
    }
    auto findServiceX29Result = protocolTable_.find(kAuthenticationId);
    if (findServiceX29Result != protocolTable_.end()) {
        protocolTable_[kAuthenticationId] = serviceX29;
    } else {
        std::ignore = protocolTable_.insert({kAuthenticationId, serviceX29});
    }
    return true;
}

bool ServerImpl::Start()
{
    ThreadPool::GetInstance().Initialize(kInt32_10U);
    ThreadPool::GetInstance().Start();
    int32_t const res = faultManager_.Start();
    if (res != 0) {
        LogError() << "ServerImpl::Start|faultManager start fail res =" << res;
        return false;
    }

    LogDebug() << "ServerImpl::Start|uds server start success";
    return true;
}

void ServerImpl::Stop()
{
    faultManager_.Stop();
    ThreadPool::GetInstance().Stop();
}


Address ServerImpl::GetPhysicalAddress() const noexcept { return physicalAddress_; }


std::vector< Address > const& ServerImpl::GetFunctionalAddress() { return functionalAddress_; }





Result< void > ServerImpl::Indicate(std::shared_ptr< Message > const& request,
                                    Priority priority,
                                    std::shared_ptr< Channel > const& channel)
{
    if (blockInCaseEcuResetProcess_) {
        LogWarn() << "Server::AssignRequest|requestReset runing";
        return Result< void >::FromError(-kInt32_16);
    }
    LogInfo() << "ServerImpl::Indicate| receive uds message Priority:" << priority;
    std::this_thread::sleep_for(std::chrono::milliseconds());
    if (request.get() == nullptr) {
        LogWarn() << "request paramter is nullptr";
        return Result< void >::FromError(-kInt32_16);
    }
    if (channel.get() == nullptr) {
        LogWarn() << "channel paramter is nullptr";
        return Result< void >::FromError(-kInt32_16);
    }
    std::vector< std::uint8_t >& body{request->GetBody()};

    Address sa = request->GetSA();
    Serialize serialize{body};
    std::uint8_t sid = serialize.ReadUint8();

    _stopS3Timer();

    if (lock_) {
        MessagePtr response{std::make_shared< Message >(physicalAddress_, sa)};
        std::vector< std::uint8_t >& responseBody{response->GetBody()};
        Serialize serializeResponse{responseBody};
        std::uint8_t nrcSid{kNegativeResponseServiceId};
        std::uint8_t rejectNrc{static_cast< std::uint8_t >(NrcErrc::kBusyRepeatRequest)};
        serializeResponse.Write(nrcSid);
        serializeResponse.Write(sid);
        serializeResponse.Write(rejectNrc);
        std::ignore = responseMsgList_.insert({channel->GetIdentifier(), response});

        return Result< void >::FromValue();
    }
    if (sid == kTesterPresentId) {
        if (body.size() != 0x02) {
            MessagePtr response{std::make_shared< Message >(physicalAddress_, sa)};
            std::vector< std::uint8_t >& responseBody{response->GetBody()};
            Serialize serializeResponse{responseBody};
            std::uint8_t nrcSid{kNegativeResponseServiceId};
            std::uint8_t rejectNrc{static_cast< std::uint8_t >(NrcErrc::kIncorrectMessageLengthOrInvalidFormat)};
            serializeResponse.Write(nrcSid);
            serializeResponse.Write(sid);
            serializeResponse.Write(rejectNrc);
            std::ignore = responseMsgList_.insert({channel->GetIdentifier(), response});
            LogWarn() << "ServerImpl::Indicate| 0x3E service length must is 2 bytes!";
        } else if (body[1] == 0x00) {
            MessagePtr response{std::make_shared< Message >(physicalAddress_, sa)};
            std::vector< std::uint8_t >& responseBody{response->GetBody()};
            Serialize serializeResponse{responseBody};
            std::uint8_t responseSid{static_cast< std::uint8_t >(sid | kResponseServiceIdMask)};
            serializeResponse.Write(responseSid);
            std::uint8_t const responseValue{0x00};
            serializeResponse.Write(responseValue);
            std::ignore = responseMsgList_.insert({channel->GetIdentifier(), response});
            LogVerbose() << "ServerImpl::Indicate| accept 0x3e 00 message";
        } else if (body[1] == kSuppressPosRspMsgIndicationMask) {
            std::ignore = responseMsgList_.insert({channel->GetIdentifier(), nullptr});
            LogVerbose() << "ServerImpl::Indicate| accept 0x3e 80 message";
        } else {
            MessagePtr response{std::make_shared< Message >(physicalAddress_, sa)};
            std::vector< std::uint8_t >& responseBody{response->GetBody()};
            Serialize serializeResponse{responseBody};
            std::uint8_t nrcSid{kNegativeResponseServiceId};
            std::uint8_t rejectNrc{static_cast< std::uint8_t >(NrcErrc::kSubfunctionNotSupported)};
            serializeResponse.Write(nrcSid);
            serializeResponse.Write(sid);
            serializeResponse.Write(rejectNrc);
            std::ignore = responseMsgList_.insert({channel->GetIdentifier(), response});
            LogWarn() << "ServerImpl::Indicate| 0x3E service subfunction only support zero!";
        }
        std::unique_lock< std::mutex > lock{requestContextLock_};

        if (!_isRequestState(sa)) {
            _startS3Timer();
        }

        return Result< void >::FromValue();
    }

    if (_isRequestState(sa)) {
        LogWarn() << "The previous request is being processed. Ignore this message";
        return Result< void >::FromError(-kInt32_16);
    }

    if (!_prioritize(sa, priority)) {
        if (config_.responseOnSecondDeclinedRequest) {
            MessagePtr response{std::make_shared< Message >(physicalAddress_, sa)};
            std::vector< std::uint8_t >& responseBody{response->GetBody()};
            Serialize serializeResponse{responseBody};
            std::uint8_t nrcSid{kNegativeResponseServiceId};
            std::uint8_t rejectNrc{static_cast< std::uint8_t >(NrcErrc::kBusyRepeatRequest)};
            serializeResponse.Write(nrcSid);
            serializeResponse.Write(sid);
            serializeResponse.Write(rejectNrc);
            std::ignore = responseMsgList_.insert({channel->GetIdentifier(), response});
            return {};
        }
        return Result< void >::FromError(-kInt32_16);
    }

    return Result< void >::FromValue();
}




void ServerImpl::HandleMessage(std::shared_ptr< Message > const& request,
                               Priority priority,
                               std::shared_ptr< Channel > const& channel) noexcept
{
    decltype(auto) findResponseMsgResult = responseMsgList_.find(channel->GetIdentifier());
    if (findResponseMsgResult != responseMsgList_.end()) {
        MessagePtr responsePtr{findResponseMsgResult->second};
        if (responsePtr.get() != nullptr) {
            std::ignore = channel->Respond(responsePtr);
            LogInfo() << "ServerImpl::HandleMessage| send response!";
        }
        std::ignore = responseMsgList_.erase(findResponseMsgResult);
        return;
    }
    Serialize serialize{request->GetBody()};
    std::uint8_t sid         = serialize.ReadUint8();
    std::uint8_t subfunction = serialize.ReadUint8();
    MetaInfoMap metaInfoMap{
        _createMetaInfoMap(request, 0 != (subfunction & kSuppressPosRspMsgIndicationMask), channel)};
    RequestContextConfirmCallback confirmCallback{
        [this](RequestContext& requestContext, ResponseStatusType status, bool ignoreServiceConfirm) mutable {
            Confirm(requestContext, status, ignoreServiceConfirm);
        }};
    RequestContextSaveCallback saveCallback{[this](RequestContext& requestContext) { std::ignore = requestContext; }};
    std::shared_ptr< RequestContext > requestContextPtr{
        std::make_shared< RequestContext >(*this, request, physicalAddress_, functionalAddress_, channel, metaInfoMap,
                                           priority, confirmCallback, saveCallback)};
    requestContextPtr->SetSid(sid);
    requestContextPtr->NotifyRequestContextState([this](Address client, Address server, RequestContextState state) {
        _updateRequestContextStateTable(client, state);
        for (auto&& notify : notifyTable_) {
            notify(client, server, state);
        }
    });
    if (authenticationManagerPtr_.get() != nullptr) {
        auto GetClientAuthenticationResult = authenticationManagerPtr_->GetClientAuthentication(request->GetSA());
        if (GetClientAuthenticationResult.HasValue()) {
            GetClientAuthenticationResult.Value()->Stop();
        }
    }
    {
        std::unique_lock< std::mutex > lock{waitQueueLock_};
        waitQueue_.push(requestContextPtr);
    }

    std::ignore = ThreadPool::GetInstance().Submit([this]() {
        std::unique_lock< std::mutex > waitLock{waitQueueLock_};
        while (!waitQueue_.empty()) {
            std::shared_ptr< RequestContext > requestContext1Ptr{nullptr};
            {
                requestContext1Ptr = waitQueue_.front();
                waitLock.unlock();
                Address clientAddres = requestContext1Ptr->GetRequest()->GetSA();
                {
                    std::unique_lock< std::mutex > requestLock{requestContextLock_};
                    decltype(auto) findResult = requestContextTable_.find(clientAddres);
                    if (findResult != requestContextTable_.end()) {
                        return;
                    }
                    std::ignore = requestContextTable_.insert({clientAddres, requestContext1Ptr});
                }
                waitLock.lock();
                waitQueue_.pop();
                waitLock.unlock();
                {
                    if (_s3Running()) {
                        _stopS3Timer();
                    }
                }
                _hanldeRequestContext(requestContext1Ptr);
                {
                    if (authenticationManagerPtr_.get() != nullptr) {
                        auto GetClientAuthenticationResult
                            = authenticationManagerPtr_->GetClientAuthentication(clientAddres);
                        if (GetClientAuthenticationResult.HasValue()) {
                            LogInfo() << "ServerImpl::_hanldeRequestContext| start auth timer!";
                            GetClientAuthenticationResult.Value()->Start();
                        }
                    }
                    if (_isNonDefaultSession()) {
                        _startS3Timer();
                    }
                    requestContext1Ptr->End();
                    std::unique_lock< std::mutex > lock{requestContextLock_};
                    LogDebug() << "ServerImpl::_hanldeRequestContext| erase request context,client:"
                               << requestContext1Ptr->GetRequest()->GetSA();
                    std::ignore = requestContextTable_.erase(requestContext1Ptr->GetRequest()->GetSA());
                }
                requestContext1Ptr.reset();
                waitLock.lock();
            }
        }
    });
}


void ServerImpl::NotifyReestablishment(std::shared_ptr< Channel > const& channel) noexcept
{
    auto findServiceCustomResult = protocolTable_.find(kEcuResetId);
    if (findServiceCustomResult != protocolTable_.end()) {
        std::shared_ptr< ServiceX11 > serviceX11Ptr{
            std::dynamic_pointer_cast< ServiceX11 >(findServiceCustomResult->second)};
        serviceX11Ptr->NotifyReestablishment(channel);
    }
}


void ServerImpl::AddManufacturerServiceValidation(
    std::shared_ptr< ServiceValidationInterface > const& serviceValidation) noexcept
{
    serviceValidationServicePtr_->AddManufacturerServiceValidation(serviceValidation);
}


void ServerImpl::AddSupplierServiceValidation(
    std::shared_ptr< ServiceValidationInterface > const& serviceValidation) noexcept
{
    serviceValidationServicePtr_->AddSupplierServiceValidation(serviceValidation);
}



bool ServerImpl::_prioritize(Address newClient, Priority const priority) noexcept
{
    // deal with Pseudo Parallel Mode only since Fully Parallel Mode is removed in 2111
    // std::unique_lock<std::mutex> const lock{nonDefaultLock_};
    Address const client{sessionManagerPtr_->GetClient()};
    if (newClient == client) {
        return true;
    }
    if (_isNonDefaultSession()) {
        ///  [SWS_DM_00429]
        LogDebug() << "ServerImpl::_prioritize| instead of nonDefault conversation";
        if (priority < nonDefaultPriority_) {
            LogDebug() << "ServerImpl::_prioritize| client address:" << client;
            std::unique_lock< std::mutex > lock{requestContextLock_};
            auto findRequestContextResult = requestContextTable_.find(client);
            if (findRequestContextResult != requestContextTable_.end()) {
                LogDebug() << "ServerImpl::_prioritize| start Cancelled";
                findRequestContextResult->second->Cancelled();
            } else {
                LogInfo() << "ServerImpl::_prioritize| reset default session";
                ResetDefaultSession();
            }
            return true;
        }
        return false;
    }
    LogDebug() << "ServerImpl::_prioritize| new conversation";
    std::unique_lock< std::mutex > lock{requestContextLock_};
    if (requestContextTable_.size() == config_.maxParallelRequests) {
        Address lowestPrioritizeClientAddress{0U};
        Priority lowestPriority{priority};
        std::shared_ptr< RequestContext > lowestRequestContext{nullptr};
        for (auto&& requestContextPair : requestContextTable_) {
            Priority currentRequestContextPriority = requestContextPair.second->GetPriority();
            if (lowestPriority < currentRequestContextPriority) {
                lowestPrioritizeClientAddress = requestContextPair.first;
                lowestRequestContext          = requestContextPair.second;
            }
        }
        if (0 == lowestPrioritizeClientAddress) {
            if (lowestRequestContext.get() != nullptr) {
                lowestRequestContext->Cancelled();
            }

            std::ignore = requestContextTable_.erase(lowestPrioritizeClientAddress);
            return true;
        }
        return false;
    }
    return true;
}

MetaInfoMap ServerImpl::_createMetaInfoMap(std::shared_ptr< Message > const& request,
                                           bool bSuppPosResponse,
                                           std::shared_ptr< Channel > const& channelPtr) noexcept
{
    MetaInfoMap map;
    std::stringstream strStream;
    strStream << std::hex << std::setw(4) << std::setfill('0') << request->GetSA();
    std::ignore = map.insert({"kSA", strStream.str()});
    strStream.clear();
    strStream.str("");
    strStream << std::hex << std::setw(4) << std::setfill('0') << request->GetTA();
    std::ignore = map.insert({"kTA", strStream.str()});
    TargetAddressType const type{request->GetTaType()};
    std::string strType;
    switch (type) {
        case TargetAddressType::kPhysical: {
            strType = "PHYS";
        } break;
        case TargetAddressType::kFunctional: {
            strType = "FUNC";
        } break;
        default: {
            break;
        }
    }
    std::ignore = map.insert({"kTAType", strType});
    // Due to the uniqueness of SA, temporarily use SA as the value of the kRequestHandle field.
    std::string strSuppPosResponse;
    if (bSuppPosResponse) {
        strSuppPosResponse = "TRUE";
    } else {
        strSuppPosResponse = "FALSE";
    }
    std::ignore = map.insert({"kSuppPosResponse", strSuppPosResponse});

    std::string strSecurityTimerElapsed;
    if (!securityLevelManagerPtr_->SecurityTimerElapsed()) {
        strSecurityTimerElapsed = "TRUE";
    } else {
        strSecurityTimerElapsed = "FALSE";
    }
    std::ignore = map.insert({"kSecurityTimerElapsed", strSecurityTimerElapsed});
    std::ignore = map.insert({"kRequestHandle", std::to_string(static_cast< unsigned int >(_allocRequestHandler()))});
    std::ignore = map.insert({"kLocalIP", channelPtr->GetLocalIp()});
    std::ignore = map.insert({"kLocalPort", std::to_string(static_cast< int32_t >(channelPtr->GetLocalPort()))});
    std::ignore = map.insert({"kRemoteIP", channelPtr->GetRemoteIp()});
    std::ignore = map.insert({"kRemotePort", std::to_string(static_cast< int32_t >(channelPtr->GetRemotePort()))});

    return map;
}

void ServerImpl::_hanldeRequestContext(std::shared_ptr< RequestContext > const& requestContextPtr) noexcept
{
    if (requestContextPtr.get() == nullptr) {
        LogWarn() << __FILE__ << ":" << __LINE__ << "ServerImpl::_hanldeRequestContext|requestContextPtr is invalid";
    }
    bool result{false};
    if (requestContextTable_.empty()) {
        LogDebug() << "ServerImpl::_hanldeRequestContext| add requestContextTable_ fails!";
    }
    SessionId currentSessionId            = sessionManagerPtr_->GetCurrentSessionId();
    std::shared_ptr< Session > sessionPtr = sessionManagerPtr_->GetSession(currentSessionId);
    std::uint32_t p2ServerMax             = sessionPtr->GetP2ServerMax();
    std::uint32_t p2StarServerMax         = sessionPtr->GetP2StarServerMax();
    std::int32_t startP2ServerTimerResult = requestContextPtr->StartP2ServerTimer(
        p2ServerMax, p2StarServerMax, config_.maxNumberOfRequestCorrectlyReceivedResponsePending);
    if (startP2ServerTimerResult != isoft::kSuccess) {
        LogError() << "ServerImpl::_hanldeRequestContext| start p2 server timer fials! code:"
                   << startP2ServerTimerResult;
        return;
    }

    result = serviceValidationServicePtr_->ManufacturerServiceValidate(requestContextPtr);
    if (!result) {
        LogWarn() << "ServerImpl::_hanldeRequestContext| manufacturer validate fail!,exit";
        return;
    }
    std::uint8_t sid{requestContextPtr->GetSid()};
    auto findProtocolTableResult = protocolTable_.find(sid);
    if ((findProtocolTableResult == protocolTable_.end()) || (findProtocolTableResult->second.get() == nullptr)) {
        LogWarn() << "ServerImpl::_hanldeRequestContext| service not support,service id:" << LogHex8{sid}
                  << "please check service id config";
        requestContextPtr->Respond(NrcErrc::kServiceNotSupported, true);
        return;
    }

    auto validatorTable = findProtocolTableResult->second->GetSidValidator();
    for (auto&& validator : validatorTable) {
        result = validator->Validate(requestContextPtr);
        if (!result) {
            LogWarn() << "ServerImpl::_hanldeRequestContext|"
                      << " Validate fail!";
            return;
        }
    }

    result = serviceValidationServicePtr_->SupplierServiceValidate(requestContextPtr);
    if (!result) {
        LogWarn() << "ServerImpl::_hanldeRequestContext| supplier validate fail!,exit";
        return;
    }

    findProtocolTableResult->second->Process(requestContextPtr);
}

void ServerImpl::Confirm(RequestContext& request, ResponseStatusType status, bool ignoreServiceConfirm) noexcept
{
    if (!ignoreServiceConfirm) {
        std::uint8_t sid{0U};
        sid                          = request.GetSid();
        auto findProtocolTableResult = protocolTable_.find(sid);
        if ((findProtocolTableResult == protocolTable_.end()) || (findProtocolTableResult->second.get() == nullptr)) {
            LogWarn() << "ServerImpl::Confirm | service not support,service id:" << sid
                      << "please check service id config";
            return;
        }
        findProtocolTableResult->second->Confirm(request, status);
    }
    MetaInfoMap metaInfoMap;
    metaInfoMap = request.GetMetaInfoMap();
    serviceValidationServicePtr_->SupplierConfirmation(static_cast< ConfirmationStatusType >(status), metaInfoMap);
    serviceValidationServicePtr_->ManufacturerConfirmation(static_cast< ConfirmationStatusType >(status), metaInfoMap);
    _removeRequestHandler(request);
}

void ServerImpl::_updateNonDefaultPriority(SessionId, SessionId newSessionId) noexcept
{
    SessionId const defaultSession{1U};
    Priority const minPriority{255U};
    if (defaultSession != newSessionId) {
        Address const client{sessionManagerPtr_->GetClient()};
        std::unique_lock< std::mutex > lock{requestContextLock_};
        auto findRequestContextResult = requestContextTable_.find(client);
        if (findRequestContextResult != requestContextTable_.end()) {
            nonDefaultPriority_ = findRequestContextResult->second->GetPriority();
        }
    } else {
        nonDefaultPriority_ = minPriority;
    }
    LogDebug() << "ServerImpl::_updateNonDefaultPriority: priority:" << nonDefaultPriority_;
}

std::uint16_t ServerImpl::_allocRequestHandler() noexcept
{
    std::uint16_t const maxUint16{65535U};
    std::lock_guard< std::mutex > lock(requestHandlerLock_);
    for (std::uint16_t i = 0; i < maxUint16; i++) {
        auto result = requestHandlerTable_.find(i);
        if (result == requestHandlerTable_.end()) {
            std::ignore = requestHandlerTable_.insert(i);
            return i;
        }
    }
    return maxUint16;
}

void ServerImpl::_removeRequestHandler(RequestContext& request) noexcept
{
    MetaInfoMap metaInfo;
    metaInfo = request.GetMetaInfoMap();
    std::uint16_t requestHandler{0U};
    std::stringstream strStream;
    strStream << metaInfo["kRequestHandle"];
    strStream >> requestHandler;
    std::lock_guard< std::mutex > lock(requestHandlerLock_);
    auto result = requestHandlerTable_.find(requestHandler);
    if (result != requestHandlerTable_.end()) {
        std::ignore = requestHandlerTable_.erase(requestHandler);
    }
}


bool ServerImpl::TryLock() noexcept
{
    if (_isNonDefaultSession()) {
        return false;
    }
    lock_ = true;
    return true;
}

void ServerImpl::Unlock() noexcept
{
    lock_ = false;
    return;
}



Result< std::uint8_t > ServerImpl::Check(
    EnvironmentalConditionId id,
    MetaInfoMap const& metaInfo,
    std::shared_ptr< isoft::uds::server::CancellationState > const& cancellationState) noexcept
{
    Result< std::uint8_t > result{};
    std::shared_ptr< EnvironmentalCondition > envConditionPtr{envConditionManagerPtr_->Get(id)};
    if (envConditionPtr.get() == nullptr) {
        LogError() << "ServerImpl::Check| id is invalid! not find envCOndition in envConditionManager!";
        std::int32_t err{-1};
        result.ErrorOr(err);
        return result;
    }
    std::uint8_t checkResult = envConditionPtr->Check(metaInfo, cancellationState);
    result.ValueOr(checkResult);
    return result;
}


Result< SessionId > ServerImpl::GetCurrentSession() noexcept
{
    Result< SessionId > result{};
    if (sessionManagerPtr_.get() == nullptr) {
        LogError() << "ServerImpl::GetCurrentSession| session manager not init!";
        std::int32_t err{-1};
        result.ErrorOr(err);
        return result;
    }
    SessionId currentSessionId{sessionManagerPtr_->GetCurrentSessionId()};
    result.ValueOr(currentSessionId);
    return result;
}



Result< NotifyOfSessionChangeCallbackId > ServerImpl::AddNotifyOfSessionChange(
    NotifyOfSessionChangeCallback callback) noexcept
{
    Result< NotifyOfSessionChangeCallbackId > result{};
    if (sessionManagerPtr_.get() == nullptr) {
        LogError() << "ServerImpl::AddNotifyOfSessionChange| session manager not init!";
        std::int32_t err{-1};
        result.ErrorOr(err);
        return result;
    }
    NotifyOfSessionChangeCallbackId currentSessionId{sessionManagerPtr_->AddNotifyOfSessionChange(callback)};
    result.ValueOr(currentSessionId);
    return result;
}

void ServerImpl::RemoveNotifyOfSessionChange(NotifyOfSessionChangeCallbackId id) noexcept
{
    if (sessionManagerPtr_.get() == nullptr) {
        LogError() << "ServerImpl::RemoveNotifyOfSessionChange| session manager not init!";

        return;
    }
    sessionManagerPtr_->RemoveNotifyOfSessionChange(id);
    return;
}

void ServerImpl::ResetDefaultSession() noexcept
{
    if (sessionManagerPtr_.get() == nullptr) {
        LogError() << "ServerImpl::RemoveNotifyOfSessionChange| session manager not init!";

        return;
    }
    SessionId const defaultSession{0x01U};
    Address const unknow{0x00};
    LogInfo() << "ServerImpl::ResetDefaultSession| reset to default session";
    sessionManagerPtr_->SetSession(defaultSession, unknow);
    return;
}

Result< SecurityLevelId > ServerImpl::GetCurrentSecurityLevelType() noexcept
{
    Result< SecurityLevelId > result{};
    if (securityLevelManagerPtr_.get() == nullptr) {
        LogError() << "ServerImpl::GetCurrentSecurityLevelType| security level manager not init!";
        std::int32_t err{-1};
        result.ErrorOr(err);
        return result;
    }
    SecurityLevelId currentSessionId{securityLevelManagerPtr_->GetCurrentSecurityLevel()};
    result.ValueOr(currentSessionId);
    return result;
}

Result< NotifyOfSecurityLevelChangeCallbackId > ServerImpl::AddNotifyOfSecurityLevelChange(
    NotifyOfSecurityLevelChangeCallback& callback) noexcept
{
    Result< NotifyOfSecurityLevelChangeCallbackId > result{};
    if (securityLevelManagerPtr_.get() == nullptr) {
        LogError() << "ServerImpl::AddNotifyOfSecurityLevelChange| security level manager not init!";
        std::int32_t err{-1};
        result.ErrorOr(err);
        return result;
    }
    NotifyOfSecurityLevelChangeCallbackId currentSecurityLevelId{
        securityLevelManagerPtr_->AddNotifyOfSecurityLevelChange(callback)};
    result.ValueOr(currentSecurityLevelId);
    return result;
}

void ServerImpl::RemoveNotifyOfSecurityLevelChange(NotifyOfSecurityLevelChangeCallbackId id) noexcept
{
    if (securityLevelManagerPtr_.get() == nullptr) {
        LogError() << "ServerImpl::RemoveNotifyOfSecurityLevelChange| security level manager not init!";
        return;
    }
    securityLevelManagerPtr_->RemoveNotifyOfSecurityLevelChange(id);
    return;
}

void ServerImpl::AddNotifyRequestContextState(NotifyRequestContextStateCallback const& callback) noexcept
{
    notifyTable_.push_back(callback);
}


void ServerImpl::RegisterPersistence(std::shared_ptr< PersistenceInterface > const& persistenceInterfacePtr) noexcept
{
    persistenceServicePtr_->Register(persistenceInterfacePtr);
}

void ServerImpl::_startS3Timer() noexcept
{
    std::uint32_t const s3time{5000U};
    std::ignore = s3Timer_.Start(s3time);
}

bool ServerImpl::_s3Running() noexcept { return s3Timer_.IsRunning(); }

void ServerImpl::_s3Timeout() noexcept
{
    LogWarn() << "ServerImpl::_s3Timeout| s3 timer timeout! reset session to default session!";
    Address const client{sessionManagerPtr_->GetClient()};
    sessionManagerPtr_->SetSession(0x01, 0x00);
    if (authenticationManagerPtr_.get() != nullptr) {
        auto GetClientAuthenticationResult = authenticationManagerPtr_->GetClientAuthentication(client);
        if (GetClientAuthenticationResult.HasValue()) {
            GetClientAuthenticationResult.Value()->Default();
        }
    }
}

void ServerImpl::_stopS3Timer() noexcept { std::ignore = s3Timer_.Stop(); }

void ServerImpl::_updateRequestContextStateTable(Address client, RequestContextState state) noexcept
{
    std::unique_lock< std::mutex > lock{clientRequestContextStateLock_};
    decltype(auto) findResult = clientRequestContextStateTable_.find(client);
    if (findResult != clientRequestContextStateTable_.end()) {
        findResult->second = state;
    } else {
        std::ignore = clientRequestContextStateTable_.insert({client, state});
    }
}

bool ServerImpl::_isRequestState(Address client) noexcept
{
    std::unique_lock< std::mutex > lock{clientRequestContextStateLock_};
    bool isRequestState{false};
    decltype(auto) findResult = clientRequestContextStateTable_.find(client);
    if (findResult != clientRequestContextStateTable_.end()) {
        if (findResult->second == RequestContextState::kRequest) {
            isRequestState = true;
        }
    }
    return isRequestState;
}
Result< std::shared_ptr< ClientAuthenticationController > > ServerImpl::GetClientAuthentication(
    Address const clientAddress) noexcept
{
    if (authenticationManagerPtr_.get() != nullptr) {
        Result< ClientAuthenticationPtr > result = authenticationManagerPtr_->GetClientAuthentication(clientAddress);
        if (result.HasValue()) {
            std::shared_ptr< ClientAuthenticationController > controllerPtr{
                std::make_shared< ClientAuthenticationControllerImpl >(result.Value())};
            return Result< std::shared_ptr< ClientAuthenticationController > >::FromValue(controllerPtr);
        }
        LogWarn() << "ServerImpl::GetClientAuthentication| client address invalid! client address:" << clientAddress;
    } else {
        LogWarn() << "ServerImpl::GetClientAuthentication| authentication Manager not init!";
    }

    return Result< std::shared_ptr< ClientAuthenticationController > >::FromError(__LINE__);
}

Result< std::vector< Address > > ServerImpl::GetAllClientAuthenticationAddress() noexcept
{
    if (authenticationManagerPtr_.get() != nullptr) {
        Result< std::vector< Address > > result = authenticationManagerPtr_->GetAllClientAuthenticationAddress();
        if (result.HasValue()) {
            return Result< std::vector< Address > >::FromValue(result.Value());
        }
        LogWarn() << "ServerImpl::GetAllClientAuthentication| not client authentication!";
    } else {
        LogWarn() << "ServerImpl::GetAllClientAuthentication| authentication Manager not init!";
    }

    return Result< std::vector< Address > >::FromError(__LINE__);
}

std::shared_ptr< ServiceInterface > ServerImpl::GetServiceBySid(std::uint8_t const sid) noexcept
{
    auto findProtocolTableResult = protocolTable_.find(sid);
    if ((findProtocolTableResult == protocolTable_.end()) || (findProtocolTableResult->second.get() == nullptr)) {
        LogWarn() << "ServerImpl::GetServiceBySid| service not support,service id:" << LogHex8{sid}
                  << "please check service id config";
        return nullptr;
    }

    return findProtocolTableResult->second;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft