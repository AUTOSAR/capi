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
/// @file       service_x27.cpp
/// @brief      This file provides SecurityAccess (0x27) service
/// @details
/// @date       2022-11-07
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=security_access_management
/// @trace_id_sr=c112411e-7929-493a-a287-4d7bfb07c627
/// @unit_name=service_x31.h
/// @unit_description=This file provides service_x31 functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_x27.h"

#include <iostream>

#include "common/log.h"
#include "common/serialize.h"
#include "isoft/uds/type_int.h"

namespace isoft {
namespace uds {
namespace server {
namespace service_x27 {
std::uint8_t const kMinRequestSize{2U};
std::uint8_t const kTwo{2U};
std::uint8_t const kKeySize{32U};

/// @brief Determine whether the current subfunction belongs to a Seed request
/// @param[in] subfunction service subfunction value
/// @return bool
/// @retval true: Belongs to Seed request
/// @retval false: Belongs to key request
inline bool IsRequestSeed(std::uint8_t const subfunction)
{
    bool isRequestSeed{false};
    if (subfunction % service_x27::kTwo != 0U) {
        isRequestSeed = true;
    }
    return isRequestSeed;
}

inline SecurityLevelId ComputeSecurityLevelId(std::uint8_t const seedId) noexcept
{
    std::uint8_t const number{2U};
    return static_cast< SecurityLevelId >((seedId + 1U) / number);
}

/// @brief Determine whether the current subfunction belongs to sending a key
/// @param[in] subfunction service subfunction value
/// @return bool
/// @retval true: Belongs to Seed request
/// @retval false: Belongs to key request
inline bool IsSeedKey(std::uint8_t const subfunction) { return !IsRequestSeed(subfunction); }

}  // namespace service_x27

ServiceX27::ServiceX27(std::shared_ptr< PersistenceService >& persistenceServicePtr) noexcept
    : storage_{persistenceServicePtr}
    , serverSessionValidator_{std::make_shared< ServerLevelSessionValidator< std::uint8_t > >()}
    , serverSecurityLevelValidator_{std::make_shared< ServerLevelSecurityLevelValidator< std::uint8_t > >()}
    , securityLevelManager_{nullptr}
{
}

ServiceX27::~ServiceX27() noexcept { Stop(); }
std::int32_t ServiceX27::Initialize(std::set< SecurityAccessInstanceConfig > const& securityAccessInstanceTable,
                                    std::uint32_t securityDelayTimeOnBoot,
                                    bool sharedTimer,
                                    std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                                    std::shared_ptr< SessionManager > const& sessionManager,
                                    std::shared_ptr< SecurityLevelManager > const& securityLevelManager,
                                    std::uint32_t const p4ServerMax) noexcept
{
    delayTimer_ = std::make_shared< Timer >();
    if (delayTimer_->Initialize([this]() { _handleSharedDelayTimer(); }) != 0) {
        LogWarn() << "ServiceX27::Initialize|Timer::Initialize|";
        return __LINE__;
    }
    NotifyOfSessionChangeCallback sessionChangeCallback{[this](SessionId, SessionId) {
        lastSubfunction_       = 0U;
        unlockSecurityLevelId_ = 0U;
        securityLevelState_    = SecurityLevelState::kLockNoActiveSeed;
    }};
    std::ignore           = sessionManager->AddNotifyOfSessionChange(sessionChangeCallback);
    securityLevelManager_ = securityLevelManager;
    std::uint32_t sharedSecurityDelayTime{0U};
    for (auto&& securityAccessInstacne : securityAccessInstanceTable) {
        std::shared_ptr< SecurityLevelInfo > securityLevelInfoPtr{std::make_shared< SecurityLevelInfo >()};
        securityLevelInfoPtr->sessionValidator = std::make_shared< SubfunctionLevelSessionValidator<> >();
        if (securityLevelInfoPtr->sessionValidator.get() == nullptr) {
            LogWarn() << "allow memory fails in new SubfunctionLevelSessionValidator instance";
            return -1;
        }
        std::int32_t result{0};
        if (securityAccessInstacne.accessPermissionAuth.enable) {
            std::shared_ptr< SubfunctionLevelAuthValidator< std::uint8_t > > authValidator
                = std::make_shared< SubfunctionLevelAuthValidator< std::uint8_t > >();
            if (authValidator.get() == nullptr) {
                LogWarn() << "allow memory fails in new SubfunctionLevelAuthValidator instance";
                return -1;
            }
            result = static_cast< int32_t >(
                authValidator->Initialize(securityAccessInstacne.securityLevel.id,
                                          securityAccessInstacne.accessPermissionAuth.authRoleList, authManagerPtr));
            if (result != 0) {
                LogWarn() << "auth config initialize fails!";
                return -1;
            }
            if (authValidator_.get() == nullptr) {
                authValidator_ = std::make_shared< ServerLevelAuthValidator< std::uint8_t > >();
            }
            result = authValidator_->RegisterSubfunctionLevel(securityAccessInstacne.securityLevel.id, authValidator);
            if (result != 0) {
                LogWarn() << "auth id repeatability! Please check SecurityAccessInstanceConfig.accessPermissionAuth";
                return -1;
            }
            securityLevelInfoPtr->authValidator = authValidator;
        }
        result = static_cast< int32_t >(securityLevelInfoPtr->sessionValidator->Initialize(
            securityAccessInstacne.securityLevel.id, securityAccessInstacne.accessPermissionSession, sessionManager));
        if (result != 0) {
            LogWarn() << "session config initialize fails!";
            return -1;
        }
        result = serverSessionValidator_->RegisterSubfunctionLevel(securityAccessInstacne.securityLevel.id,
                                                                   securityLevelInfoPtr->sessionValidator);
        if (result != 0) {
            LogWarn()
                << "session id repeatability! Please check ReadDiagnosticDataByIdentifier.accessPermissionSession";
            return -1;
        }

        securityLevelInfoPtr->securityLevelValidator = std::make_shared< SubfunctionLevelSecurityLevelValidator<> >();
        if (securityLevelInfoPtr->securityLevelValidator.get() == nullptr) {
            LogWarn() << "allow memory fails in new SubfunctionLevelSecurityLevelValidator instance";
            return -1;
        }
        result = static_cast< int32_t >(securityLevelInfoPtr->securityLevelValidator->Initialize(
            securityAccessInstacne.securityLevel.id, securityAccessInstacne.accessPermissionSecurityLevel,
            securityLevelManager));
        if (result != 0) {
            LogWarn() << "security level config initialize fails!";
            return -1;
        }
        result = serverSecurityLevelValidator_->RegisterSubfunctionLevel(securityAccessInstacne.securityLevel.id,
                                                                         securityLevelInfoPtr->securityLevelValidator);
        if (result != 0) {
            LogWarn() << "security level id repeatability! Please check "
                         "ReadDiagnosticDataByIdentifier.accessPermissionSecurityLevel";
            return -1;
        }

        securityLevelInfoPtr->proxy                   = securityAccessInstacne.securityLevel.interfacePtr;
        securityLevelInfoPtr->id                      = securityAccessInstacne.securityLevel.id;
        securityLevelInfoPtr->securityDelayTime       = securityAccessInstacne.securityLevel.securityDelayTime;
        securityLevelInfoPtr->numFailedSecurityAccess = securityAccessInstacne.securityLevel.numFailedSecurityAccess;
        securityLevelInfoPtr->failedCounter           = _loadFailedCounter(securityLevelInfoPtr->id);
        securityLevelInfoPtr->requestSeedId           = securityAccessInstacne.requestSeedId;
        if (!sharedTimer) {
            securityLevelInfoPtr->delayTimer = std::make_shared< Timer >();
            std::int32_t const status2{
                securityLevelInfoPtr->delayTimer->Initialize([this, id = securityLevelInfoPtr->requestSeedId]() {
                    std::shared_ptr< SecurityLevelInfo > temp{instanceTable_[id]};
                    _handleDelayTimer(temp);
                })};
            if (status2 != 0) {
                LogWarn() << "ServiceX27::Initialize|Timer::Initialize" << status2;
                return -1;
            }
        } else {
            if (sharedSecurityDelayTime == 0) {
                sharedSecurityDelayTime = securityAccessInstacne.securityLevel.securityDelayTime;
            } else {
                if (sharedSecurityDelayTime != securityAccessInstacne.securityLevel.securityDelayTime) {
                    LogError() << "if sharedTimer is true,The value of SecurityLevelConfig.securityDelayTime must be "
                                  "the same in all security levels.";
                    return -1;
                }
            }
        }
        std::ignore = instanceTable_.emplace(securityLevelInfoPtr->requestSeedId, securityLevelInfoPtr);
    }
    delayTimerShared_        = sharedTimer;
    securityDelayTimeOnBoot_ = securityDelayTimeOnBoot;

    p4ServerMax_ = p4ServerMax;
    return Start();
}
std::vector< ValidatorPtr > ServiceX27::GetSidValidator() noexcept
{
    std::vector< ValidatorPtr > validatorTable{};
    if (authValidator_.get() != nullptr) {
        validatorTable.push_back(authValidator_);
    }
    if (serverSessionValidator_.get() != nullptr) {
        validatorTable.push_back(serverSessionValidator_);
    }
    if (serverSecurityLevelValidator_.get() != nullptr) {
        validatorTable.push_back(serverSecurityLevelValidator_);
    }
    return validatorTable;
}
std::int32_t ServiceX27::Start() noexcept
{
    std::uint32_t bootingDelay{securityDelayTimeOnBoot_};
    for (decltype(auto) each : instanceTable_) {
        if (each.second->failedCounter >= each.second->numFailedSecurityAccess) {
            if (delayTimerShared_) {
                if (each.second->securityDelayTime > bootingDelay) {
                    bootingDelay = each.second->securityDelayTime;
                }
            } else {
                std::uint32_t duration{securityDelayTimeOnBoot_};
                if (each.second->securityDelayTime > securityDelayTimeOnBoot_) {
                    duration = each.second->securityDelayTime;
                }
                securityLevelManager_->SetSecurityTimerElapsed(true);
                if (each.second->delayTimer->Start(duration) != 0) {
                    LogWarn() << "ServiceX27::Start|start delay timer";
                    return __LINE__;
                }
            }
        }
    }
    securityLevelManager_->SetSecurityTimerElapsed(true);
    if (delayTimer_->Start(bootingDelay) != 0) {
        LogWarn() << "ServiceX27::Start|start timer on booting";
        return __LINE__;
    }
    return 0;
}

void ServiceX27::Stop() noexcept
{
    for (decltype(auto) each : instanceTable_) {
        if (!delayTimerShared_) {
            if (each.second->delayTimer->Stop() != 0) {
                LogWarn() << "ServiceX27::Start|stop delay timer";
            }
        }
    }
    std::ignore = delayTimer_->Stop();
}

std::size_t ServiceX27::GetMinimumSize(std::uint8_t subfunction) noexcept
{
    std::size_t const minSize{2};
    std::ignore = subfunction;
    return minSize;
}

bool ServiceX27::IsSupport(std::uint8_t subfunction) noexcept
{
    std::uint8_t const id   = _getRealSubfunction(subfunction);
    auto findInstacneResult = instanceTable_.find(id);
    if (findInstacneResult != instanceTable_.end()) {
        return true;
    }
    return false;
}

std::shared_ptr< Validator > ServiceX27::GetSubfunctionAuth(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->authValidator;
    }
    LogInfo() << "ServiceX27::GetSubfunctionAuth|Please config auth validator in subfunction:" << subfunction;
    return nullptr;
}

std::shared_ptr< Validator > ServiceX27::GetSubfunctionSession(std::uint8_t subfunction) noexcept
{
    std::uint8_t id         = _getRealSubfunction(subfunction);
    auto findInstacneResult = instanceTable_.find(id);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->sessionValidator;
    }
    LogWarn() << "ServiceX27::GetSubfunctionSession|Please config session validator in subfunction:" << subfunction;
    return nullptr;
}

bool ServiceX27::SupportSecurityLevel() noexcept
{
    if (serverSecurityLevelValidator_.get() != nullptr) {
        return true;
    }
    return false;
}

std::shared_ptr< Validator > ServiceX27::GetSubfunctionSecurityLevel(std::uint8_t subfunction) noexcept
{
    std::uint8_t id         = _getRealSubfunction(subfunction);
    auto findInstacneResult = instanceTable_.find(id);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->securityLevelValidator;
    }
    LogWarn() << "ServiceX27::GetSubfunctionSecurityLevel|Please config security level validator in subfunction:"
              << subfunction;
    return nullptr;
}

void ServiceX27::ParseAndResponse(std::shared_ptr< RequestContext > context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX27::ParseAndResponse| context is nullptr";
        return;
    }
    MessagePtr requestPtr = context->GetRequest();
    if (requestPtr.get() != nullptr) {
        Serialize serialize{requestPtr->GetBody()};
        serialize.Seek(1);
        std::uint8_t subfunction = serialize.ReadUint8();
        std::uint8_t const subfunctionMask{0x7FU};
        subfunction             = subfunction & subfunctionMask;
        std::uint8_t id         = _getRealSubfunction(subfunction);
        auto findInstacneResult = instanceTable_.find(id);
        if (findInstacneResult != instanceTable_.end()) {
            // blockek by a delay timer since booting or too many failed change attemps
            bool const isActive{_activeDelayTimer(findInstacneResult->second)};
            if (isActive) {
                context->Respond(NrcErrc::kRequiredTimeDelayNotExpired);
                LogWarn() << "ServiceX27::ParseAndResponse|kRequiredTimeDelayNotExpired,the delay timer is active and "
                             "a request is transmitted, please wait delay timer expired!";
                return;
            }

            std::shared_ptr< SecurityLevel > securityLevelPtr{
                securityLevelManager_->GetSecurityLevel(findInstacneResult->second->id)};
            return _handlerState(context, securityLevelPtr, findInstacneResult->second);
        }
    }
    LogWarn() << "ServiceX27::ParseAndResponse| request message is nullptr ";
    return;
}

std::size_t ServiceX27::_loadFailedCounter(SecurityLevelId const securityLevelId) noexcept
{
    std::string key{"uds::ServiceX27|"};
    std::ignore                        = key.append(std::to_string(static_cast< unsigned >(securityLevelId)));
    std::vector< std::uint8_t > result = storage_->LoadData(key);
    if (result.empty()) {
        return 0;
    }
    Serialize deserialize{result};
    std::uint32_t count{0U};
    deserialize.Read(count);
    return count;
}

void ServiceX27::_storeFailedCounter(SecurityLevelId const securityLevelId, std::size_t const value) noexcept
{
    std::string key{"uds::ServiceX27|"};
    std::ignore = key.append(std::to_string(static_cast< unsigned >(securityLevelId)));
    std::vector< std::uint8_t > result;
    Serialize serialize{result};
    serialize.Write(static_cast< std::uint32_t >(value));
    std::ignore = storage_->SaveData(key, result);
}

void ServiceX27::_handleSharedDelayTimer() noexcept
{
    std::int32_t const status{delayTimer_->Stop()};
    if (status != 0) {
        LogWarn() << "ServiceX27::Start|stop delay timer|" << status;
    }
}

void ServiceX27::_handleDelayTimer(std::shared_ptr< SecurityLevelInfo >& securityLevel) noexcept
{
    std::int32_t const status{securityLevel->delayTimer->Stop()};
    securityLevelManager_->SetSecurityTimerElapsed(false);
    if (status != 0) {
        LogWarn() << "ServiceX27::Start|stop delay timer|" << status;
    }
}

void ServiceX27::Confirm(RequestContext& requestContext, ResponseStatusType status) noexcept
{
    MessagePtr requestPtr = requestContext.GetRequest();
    if (requestPtr.get() == nullptr) {
        return;
    }
    Serialize serialize{requestPtr->GetBody()};
    serialize.Seek(1);
    std::uint8_t subfunction = serialize.ReadUint8();
    std::uint8_t const subfunctionMask{0x7fU};
    subfunction     = subfunction & subfunctionMask;
    std::uint8_t id = _getRealSubfunction(subfunction);
    if (status == ResponseStatusType::kResPosOk) {
        switch (securityLevelState_) {
            case SecurityLevelState::kLockNoActiveSeed: {
                securityLevelState_ = SecurityLevelState::kLockActiveSeedWaitKey;
            } break;
            case SecurityLevelState::kLockActiveSeedWaitKey: {
                securityLevelState_ = SecurityLevelState::kUnlockNoActiveSeed;
                securityLevelManager_->SetSecurityLevel(id);
            } break;
            case SecurityLevelState::kUnlockNoActiveSeed: {
                if (unlockSecurityLevelId_ == subfunction) {
                    securityLevelState_ = SecurityLevelState::kUnlockNoActiveSeed;
                } else {
                    securityLevelState_ = SecurityLevelState::kUnlockActiveSeedWaitKey;
                }
            } break;
            case SecurityLevelState::kUnlockActiveSeedWaitKey: {
                securityLevelState_ = SecurityLevelState::kUnlockNoActiveSeed;
                securityLevelManager_->SetSecurityLevel(id);
            } break;
        }
    } else if (status == ResponseStatusType::kResNegOk) {
        if (securityLevelState_ == SecurityLevelState::kLockActiveSeedWaitKey) {
            securityLevelState_ = SecurityLevelState::kLockNoActiveSeed;
        } else if (securityLevelState_ == SecurityLevelState::kUnlockActiveSeedWaitKey) {
            securityLevelState_ = SecurityLevelState::kUnlockNoActiveSeed;
        }
        auto findInstacneResult = instanceTable_.find(id);
        if (findInstacneResult != instanceTable_.end()) {
            std::shared_ptr< SecurityLevelInfo > securityLevelInfoPtr = findInstacneResult->second;

            if (securityLevelInfoPtr->failedCounter == securityLevelInfoPtr->numFailedSecurityAccess) {
                std::shared_ptr< Timer > delayTimer;
                if (delayTimerShared_) {
                    delayTimer = delayTimer_;
                } else {
                    delayTimer = securityLevelInfoPtr->delayTimer;
                }
                securityLevelManager_->SetSecurityTimerElapsed(true);
                std::int32_t const statusTimer{delayTimer->Start(securityLevelInfoPtr->securityDelayTime)};
                if (statusTimer != 0) {
                    LogWarn() << "ServiceX27::Process|start delay timer|" << statusTimer;
                }
            }
        }
    }
}

bool ServiceX27::_activeDelayTimer(std::shared_ptr< SecurityLevelInfo > const& securityLevelInfoPtr) noexcept
{
    bool isRun{false};
    bool isActive{false};
    if (securityLevelInfoPtr->delayTimer != nullptr) {
        isRun = securityLevelInfoPtr->delayTimer->IsRunning();
    }
    if (delayTimer_->IsRunning() || ((!delayTimerShared_) && isRun)) {
        isActive = true;
    }
    return isActive;
}

void ServiceX27::_handlerState(std::shared_ptr< RequestContext >& context,
                               std::shared_ptr< SecurityLevel > const& securityLevelPtr,
                               std::shared_ptr< SecurityLevelInfo > const& securityLevelInfoPtr) noexcept
{
    switch (securityLevelState_) {
        case SecurityLevelState::kLockNoActiveSeed: {
            _handlerRequestSeed(context, securityLevelPtr, securityLevelInfoPtr);
        } break;
        case SecurityLevelState::kLockActiveSeedWaitKey: {
            _handlerSeedKey(context, securityLevelPtr, securityLevelInfoPtr);
        } break;
        case SecurityLevelState::kUnlockNoActiveSeed: {
            _handlerRequestSeed(context, securityLevelPtr, securityLevelInfoPtr);
        } break;
        case SecurityLevelState::kUnlockActiveSeedWaitKey: {
            _handlerSeedKey(context, securityLevelPtr, securityLevelInfoPtr);
        } break;

        default:
            break;
    }
}

void ServiceX27::_handlerRequestSeed(std::shared_ptr< RequestContext >& context,
                                     std::shared_ptr< SecurityLevel > const& securityLevelPtr,
                                     std::shared_ptr< SecurityLevelInfo > const& securityLevelInfoPtr)
{
    decltype(auto) request{context->GetRequest()};
    Serialize serialize{request->GetBody()};
    serialize.Seek(1);
    std::uint8_t subfunction = serialize.ReadUint8();
    std::uint8_t const subfunctionMask{0x7FU};
    subfunction = subfunction & subfunctionMask;
    /// Requested level is unlocked.
    if (unlockSecurityLevelId_ == subfunction && securityLevelState_ == SecurityLevelState::kUnlockNoActiveSeed) {
        std::shared_ptr< Message > response{context->CreateResponse()};
        decltype(auto) body{response->GetBody()};
        response->GetBody().push_back(subfunction);
        body.emplace_back(0x00U);
        body.emplace_back(0x00U);
        return context->Respond(std::move(response));
    }

    /// SecurityAccess sendKey received.
    bool const isRequestSeed{service_x27::IsRequestSeed(subfunction)};
    if (!isRequestSeed) {
        context->Respond(NrcErrc::kRequestSequenceError);
        LogWarn() << "ServiceX27::HandlerRequestSeed|kRequestSequenceError,Please first send request seed message!";
        return;
    }

    /// Message length OK.
    std::vector< std::uint8_t > const record{request->GetBody().begin() + 2, request->GetBody().end()};
    bool const accessDataRecordSizeMatch{_checkAccessDataRecordSize(securityLevelPtr, record.size())};
    if (!accessDataRecordSizeMatch) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "ServiceX27::HandlerRequestSeed|kIncorrectMessageLengthOrInvalidFormat request length != access "
                     "data record size!";
        return;
    }

    /// handler GetSeed mothod width result
    MetaInfoMap metaInfo{context->GetMetaInfoMap()};
    Result< std::vector< std::uint8_t > > result{
        securityLevelInfoPtr->proxy->GetSeed(record, metaInfo, CancellationHandler(context->GetCancellationState()))};
    if (!result.HasValue()) {
        NrcErrc const nrc{static_cast< NrcErrc >(result.Error())};
        context->Respond(nrc);
        LogWarn() << "ServiceX27::HandlerRequestSeed|SecurityAccessProxy::GetSeed|" << result.Error();
    } else {
        std::shared_ptr< Message > response{context->CreateResponse()};
        decltype(auto) body{response->GetBody()};
        response->GetBody().push_back(subfunction);
        std::ignore = body.insert(body.cend(), result.Value().begin(), result.Value().end());
        context->Respond(std::move(response));
        lastSubfunction_ = subfunction;
    }
}
void ServiceX27::_handlerSeedKey(std::shared_ptr< RequestContext >& context,
                                 std::shared_ptr< SecurityLevel > const& securityLevelPtr,
                                 std::shared_ptr< SecurityLevelInfo > const& securityLevelInfoPtr)
{
    decltype(auto) request{context->GetRequest()};
    Serialize serialize{request->GetBody()};
    serialize.Seek(1);
    std::uint8_t subfunction = serialize.ReadUint8();
    std::uint8_t const subfunctionMask{0x7FU};
    subfunction = subfunction & subfunctionMask;
    /// Requested level is unlocked.
    if (unlockSecurityLevelId_ == subfunction && securityLevelState_ == SecurityLevelState::kUnlockNoActiveSeed) {
        std::shared_ptr< Message > response{context->CreateResponse()};
        decltype(auto) body{response->GetBody()};
        response->GetBody().push_back(subfunction);
        body.emplace_back(0x00U);
        body.emplace_back(0x00U);
        return context->Respond(std::move(response));
    }

    /// SecurityAccess requestSeed received.
    bool const isSendKey{service_x27::IsSeedKey(subfunction)};
    if (!isSendKey) {
        switch (securityLevelState_) {
            case SecurityLevelState::kLockActiveSeedWaitKey:
                securityLevelState_ = SecurityLevelState::kLockNoActiveSeed;
                break;
            case SecurityLevelState::kUnlockActiveSeedWaitKey:
                securityLevelState_ = SecurityLevelState::kUnlockNoActiveSeed;
                break;
            default:
                break;
        }
        _handlerState(context, securityLevelPtr, securityLevelInfoPtr);
        return;
    }

    /// sub-function: yy <> xx+1.
    bool const matchKey{_matchRequestSeedAndSeedKey(subfunction)};
    if (!matchKey) {
        ++securityLevelInfoPtr->failedCounter;
        context->Respond(NrcErrc::kRequestSequenceError);
        LogWarn() << "ServiceX27::HandlerSeedKey|kRequestSequenceError sub-function: seedkey != requestSeed+1.";
        _storeFailedCounter(securityLevelInfoPtr->id, securityLevelInfoPtr->failedCounter);
        return;
    }

    /// Message length OK.
    std::vector< std::uint8_t > const key{request->GetBody().begin() + 2, request->GetBody().end()};
    bool const matchKeySize{_checkKeySize(securityLevelPtr, key.size())};
    if (!matchKeySize) {
        ++securityLevelInfoPtr->failedCounter;
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "ServiceX27::HandlerSeedKey|kIncorrectMessageLengthOrInvalidFormat| request length != key size!";
        _storeFailedCounter(securityLevelPtr->GetId(), securityLevelInfoPtr->failedCounter);
        return;
    }

    MetaInfoMap metaInfo{context->GetMetaInfoMap()};
    std::shared_ptr< CancellationState > cancellationStatePtr{context->GetCancellationState()};
    Result< KeyCompareResultType > result{
        securityLevelInfoPtr->proxy->CompareKey(key, metaInfo, CancellationHandler(cancellationStatePtr))};
    if (result.HasValue()) {
        if (result.Value() == KeyCompareResultType::kKeyValid) {
            securityLevelInfoPtr->failedCounter = 0U;
            std::shared_ptr< Message > response{context->CreateResponse()};
            response->GetBody().push_back(subfunction);
            context->Respond(std::move(response));
            lastSubfunction_       = subfunction;
            unlockSecurityLevelId_ = subfunction;
        } else {
            ++securityLevelInfoPtr->failedCounter;
            if (securityLevelInfoPtr->failedCounter < securityLevelInfoPtr->numFailedSecurityAccess) {
                context->Respond(NrcErrc::kInvalidKey);
            } else {
                context->Respond(NrcErrc::kExceedNumberOfAttempts);
            }
            LogWarn() << "ServiceX27::HandlerSeedKey|SecurityAccessProxy::CompareKey|kKeyInvalid";
        }
    } else {
        NrcErrc const nrc{static_cast< NrcErrc >(result.Error())};
        if (NrcErrc::kInvalidKey == nrc) {
            ++securityLevelInfoPtr->failedCounter;
            if (securityLevelInfoPtr->failedCounter < securityLevelInfoPtr->numFailedSecurityAccess) {
                context->Respond(NrcErrc::kInvalidKey);
            } else {
                context->Respond(NrcErrc::kExceedNumberOfAttempts);
            }
            LogWarn() << "ServiceX27::HandlerSeedKey|SecurityAccessProxy::CompareKey|kKeyInvalid";
        } else {
            context->Respond(nrc);
        }
        LogWarn() << "ServiceX27::HandlerSeedKey|SecurityAccessProxy::CompareKey" << result.Error();
    }
}

std::uint8_t ServiceX27::_getRealSubfunction(std::uint8_t const subfunction)
{
    if (subfunction % 2 == 1) {
        return subfunction;
    }

    return subfunction - 1U;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
