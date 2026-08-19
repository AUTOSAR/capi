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
/// @file       service_x86.cpp
/// @brief      This file implements service_x86 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=roe_management
/// @trace_id_sr=6c5fdea2-dff3-4c67-b7f0-9663d71a2841
/// @unit_name=service_x86.h
/// @unit_description=This file provides service_x86 functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_x86.h"

#include <functional>

#include "common/log.h"
#include "common/serialize.h"
#include "common/thread_pool.h"
namespace isoft {
namespace uds {
namespace server {
namespace service_x86 {
std::uint8_t const kSubfunctionMask{0x3FU};
std::uint8_t const kOffsetMask{0x03U};
std::uint8_t const kMinRequestSize{3U};
std::uint8_t const kInfiniteResponseTime{2U};
std::uint8_t const kOnDTCStatusChangeSize{7U};
std::uint8_t const kDtcStatusMaskIndex{3U};
std::int8_t const kRequestContentIndex{2};
std::uint8_t const kDataIdentifierHightIndex{3U};
std::uint8_t const kDataIdentifierLowIndex{4U};
std::uint8_t const kOnComparisonOfValuesSize{16U};
std::uint8_t const kComparisonLogicIndex{5U};
std::uint8_t const kHysteresisValueIndex{10U};
std::uint8_t const kReadDTCInformationResponseSID{0x59U};
std::uint8_t const kReadDataByIdentifierResponseSID{0x62U};
std::uint8_t const kMaxComparisonLogic{4U};
std::uint8_t const kMaxHysteresisValue{0x64U};
std::uint8_t const kLocalizationByteIndex{11U};
std::uint8_t const kLocalizationByteLength{32U};
std::uint8_t const kOffsetLowIndex{12U};
std::uint8_t const kLocalizationSignMask{0x80U};
std::uint8_t const kLocalizationSign{0x80U};
std::uint8_t const kReferenceHightIndex{0x6U};
std::uint8_t const kReferenceMiddleHightIndex{0x7U};
std::uint8_t const kReferenceMiddleLowIndex{0x8U};
std::uint8_t const kReferenceLowIndex{0x9U};
std::uint8_t const kUint32Size{32U};
std::uint8_t const kComparisonLogic{5U};
std::uint8_t const kLessThan{1U};
std::uint8_t const kGeaterThan{2U};
std::uint8_t const kEqual{3U};
std::uint8_t const kNotEqual{4U};
std::int8_t const kTwo{2};

// template <typename Type>
// std::int32_t Read(internal::Chunk& data, std::size_t pos, Type& value) noexcept {
//     isoft::serialize::Buffer<internal::Chunk> buffer{data};
//     return isoft::serialize::DeserializePart(buffer, pos, value);
// }
}  // namespace service_x86

ServiceX86::ServiceX86() noexcept
    : instanceTable_{}
    , sessionManagerPtr_{nullptr}
    , sessionValidator_{std::make_shared< ServerLevelSessionValidator< std::uint8_t > >()}
    , securityLevelValidator_{std::make_shared< ServerLevelSecurityLevelValidator< std::uint8_t > >()}
    , diagnosticRoeManagerPtr_{nullptr}
{
}

std::int32_t ServiceX86::Initialize(RoeManagement& config,
                                    std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                                    std::shared_ptr< SessionManager >& sessionManagerPtr,
                                    std::shared_ptr< SecurityLevelManager >& securityLevelManagerPtr,
                                    std::shared_ptr< DiagnosticRoeManager >& diagnosticRoeManagerPtr) noexcept
{
    if (config.instanceTable.empty()) {
        LogError() << "ServiceX86::Initialize|RoeManagement.instanceTable_ is empty!";
        return -1;
    }
    if (sessionManagerPtr.get() == nullptr) {
        LogError() << "ServiceX86::Initialize|sessionManagerPtr is nullptr!";
        return -1;
    }
    sessionManagerPtr_ = sessionManagerPtr;
    if (securityLevelManagerPtr.get() == nullptr) {
        LogError() << "ServiceX86::Initialize| securityLevelManagerPtr is nullptr!";
        return -1;
    }
    if (diagnosticRoeManagerPtr.get() == nullptr) {
        LogError() << "ServiceX86::Initialize| diagnosticRoeManagerPtr is nullptr!";
        return -1;
    }
    diagnosticRoeManagerPtr_ = diagnosticRoeManagerPtr;
    for (auto&& instanceConfig : config.instanceTable) {
        std::shared_ptr< Instance > instancePtr{nullptr};
        instancePtr = std::make_shared< Instance >();
        if (instancePtr.get() == nullptr) {
            LogWarn() << "allow memory fails in new struct Instance";
            return -1;
        }
        instancePtr->responseOnEventAction = instanceConfig.responseOnEventAction;
        std::int32_t result{0};
        std::uint8_t const subfunction{static_cast< std::uint8_t >(instanceConfig.responseOnEventAction)};
        if (instanceConfig.accessPermissionAuth.enable) {
            std::shared_ptr< SubfunctionLevelAuthValidator< std::uint8_t > > authValidator
                = std::make_shared< SubfunctionLevelAuthValidator< std::uint8_t > >();
            if (authValidator.get() == nullptr) {
                LogWarn() << "allow memory fails in new SubfunctionLevelAuthValidator instance";
                return -1;
            }
            result = static_cast< int32_t >(authValidator->Initialize(
                subfunction, instanceConfig.accessPermissionAuth.authRoleList, authManagerPtr));
            if (result != 0) {
                LogWarn() << "auth config initialize fails!";
                return -1;
            }
            if (authValidator_.get() == nullptr) {
                authValidator_ = std::make_shared< ServerLevelAuthValidator< std::uint8_t > >();
            }
            result = authValidator_->RegisterSubfunctionLevel(subfunction, authValidator);
            if (result != 0) {
                LogWarn() << "auth id repeatability! Please check ControlDTCSettingSubfunction.accessPermissionAuth";
                return -1;
            }
            instancePtr->authValidator = authValidator;
        }
        instancePtr->sessionValidator = std::make_shared< SubfunctionLevelSessionValidator< std::uint8_t > >();
        if (instancePtr->sessionValidator.get() == nullptr) {
            LogWarn() << "allow memory fails in new SubfunctionLevelSessionValidator instance";
            return -1;
        }
        result = static_cast< int32_t >(instancePtr->sessionValidator->Initialize(
            subfunction, instanceConfig.accessPermissionSession, sessionManagerPtr));
        if (result != 0) {
            LogWarn() << "session config initialize fails!";
            return -1;
        }
        result = sessionValidator_->RegisterSubfunctionLevel(subfunction, instancePtr->sessionValidator);
        if (result != 0) {
            LogWarn() << "session id repeatability! Please check ComControlInstanceConfig.accessPermissionSession";
            return -1;
        }

        instancePtr->securityLevelValidator
            = std::make_shared< SubfunctionLevelSecurityLevelValidator< std::uint8_t > >();
        if (instancePtr->securityLevelValidator.get() == nullptr) {
            LogWarn() << "allow memory fails in new SubfunctionLevelSecurityLevelValidator instance";
            return -1;
        }
        result = static_cast< int32_t >(instancePtr->securityLevelValidator->Initialize(
            subfunction, instanceConfig.accessPermissionSecurityLevel, securityLevelManagerPtr));
        if (result != 0) {
            LogWarn() << "security level config initialize fails!";
            return -1;
        }
        result = securityLevelValidator_->RegisterSubfunctionLevel(subfunction, instancePtr->securityLevelValidator);
        if (result != 0) {
            LogWarn() << "security level id repeatability! Please check "
                         "ReadDiagnosticDataByIdentifier.accessPermissionSecurityLevel";
            return -1;
        }

        auto findInstanceResult = instanceTable_.find(subfunction);
        if (findInstanceResult != instanceTable_.end()) {
            LogWarn() << "ServiceX86.subfunctionNumber:" << subfunction << " alreay exist";
            return -1;
        }
        std::ignore = instanceTable_.insert({subfunction, instancePtr});
    }

    p4ServerMax_ = config.p4ServerMax;
    return 0;
}

std::vector< ValidatorPtr > ServiceX86::GetSidValidator() noexcept
{
    std::vector< ValidatorPtr > validatorTable{};
    if (authValidator_.get() != nullptr) {
        validatorTable.push_back(authValidator_);
    }
    if (sessionValidator_.get() != nullptr) {
        validatorTable.push_back(sessionValidator_);
    }
    if (securityLevelValidator_.get() != nullptr) {
        validatorTable.push_back(securityLevelValidator_);
    }
    return validatorTable;
}

std::size_t ServiceX86::GetMinimumSize(std::uint8_t subfunction) noexcept
{
    std::uint8_t const subfunctionMask{0x3F};
    std::uint8_t const id{static_cast< std::uint8_t >(subfunction & subfunctionMask)};
    std::uint8_t const stopResponseOnEvent{0x00};
    std::uint8_t const onDTCStatusChange{0x01};
    std::uint8_t const onChangeOfDataIdentifier{0x03};
    std::uint8_t const reportActivatedEvents{0x04};
    std::uint8_t const startResponseOnEvent{0x05};
    std::uint8_t const clearResponseOnEvent{0x06};
    std::uint8_t const onComparisonOfValues{0x07};
    if (id == stopResponseOnEvent || id == startResponseOnEvent || id == reportActivatedEvents
        || id == clearResponseOnEvent) {
        std::size_t const minSize{3};
        return minSize;
    }
    if (onDTCStatusChange == id) {
        std::size_t const minSize{5};
        return minSize;
    }
    if (onChangeOfDataIdentifier == id) {
        std::size_t const minSize{7};
        return minSize;
    }
    if (onComparisonOfValues == id) {
        std::size_t const minSize{15};
        return minSize;
    }
    return 0;
}

bool ServiceX86::IsSupport(std::uint8_t subfunction) noexcept
{
    std::uint8_t const subfunctionMask{0x3F};
    std::uint8_t const id{static_cast< std::uint8_t >(subfunction & subfunctionMask)};
    auto findInstacneResult = instanceTable_.find(id);
    if (findInstacneResult != instanceTable_.end()) {
        return true;
    }
    return false;
}

std::shared_ptr< Validator > ServiceX86::GetSubfunctionAuth(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->authValidator;
    }
    LogInfo() << "ServiceX86::GetSubfunctionAuth|Please config auth validator in subfunction:" << subfunction;
    return nullptr;
}

std::shared_ptr< Validator > ServiceX86::GetSubfunctionSession(std::uint8_t subfunction) noexcept
{
    std::uint8_t const subfunctionMask{0x3F};
    std::uint8_t const id{static_cast< std::uint8_t >(subfunction & subfunctionMask)};
    auto findInstacneResult = instanceTable_.find(id);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->sessionValidator;
    }
    LogWarn() << "ServiceX86::GetSubfunctionSession|Please config session validator in subfunction:" << subfunction;
    return nullptr;
}

bool ServiceX86::SupportSecurityLevel() noexcept
{
    if (securityLevelValidator_.get() != nullptr) {
        return true;
    }
    return false;
}

std::shared_ptr< Validator > ServiceX86::GetSubfunctionSecurityLevel(std::uint8_t subfunction) noexcept
{
    std::uint8_t const subfunctionMask{0x3F};
    std::uint8_t const id{static_cast< std::uint8_t >(subfunction & subfunctionMask)};
    auto findInstacneResult = instanceTable_.find(id);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->securityLevelValidator;
    }
    LogWarn() << "ServiceX86::GetSubfunctionSecurityLevel|Please config security level validator in subfunction:"
              << subfunction;
    return nullptr;
}

void ServiceX86::ParseAndResponse(std::shared_ptr< RequestContext > context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX86::ParseAndResponse| context is nullptr";
        return;
    }
    MessagePtr requestPtr = context->GetRequest();
    if (requestPtr.get() != nullptr) {
        Serialize serialize{requestPtr->GetBody()};
        serialize.Seek(1);
        std::uint8_t subfunction = serialize.ReadUint8();
        auto findInstacneResult  = instanceTable_.find(subfunction);
        if (findInstacneResult != instanceTable_.end()) {
            EventSetup setup{};
            std::uint8_t const eventTypeMask{0x3F};
            std::uint8_t const eventStoreMask{0xBF};
            std::uint8_t const stopResponseOnEvent{0x00};
            std::uint8_t const onDTCStatusChange{0x01};
            std::uint8_t const onChangeOfDataIdentifier{0x03};
            std::uint8_t const reportActivatedEvents{0x04};
            std::uint8_t const startResponseOnEvent{0x05};
            std::uint8_t const clearResponseOnEvent{0x06};
            std::uint8_t const onComparisonOfValues{0x07};
            setup.eventType       = static_cast< std::uint8_t >(subfunction & eventTypeMask);
            setup.store           = 0 != (subfunction & eventStoreMask);
            setup.eventWindowTime = serialize.ReadUint8();

            // if (setup.store) {
            //     if (setup.eventWindowTime != 0x02) {
            //         context->Respond(NrcErrc::kRequestOutOfRange);
            //     }
            //     if (setup.eventType != startResponseOnEvent) {
            //         context->Respond(NrcErrc::kRequestOutOfRange);
            //     }
            //     SessionId const defaultSession{0x01};
            //     if (sessionManagerPtr_->GetCurrentSessionId() != defaultSession) {
            //         context->Respond(NrcErrc::kRequestOutOfRange);
            //     }
            // }
            if (setup.eventType == onDTCStatusChange) {
                setup.eventTypeRecord.push_back(serialize.ReadUint8());
                setup.serviceToRespondToRecord = serialize.ReadAll();
            } else if (setup.eventType == onChangeOfDataIdentifier) {
                setup.eventTypeRecord.push_back(serialize.ReadUint8());
                setup.eventTypeRecord.push_back(serialize.ReadUint8());
                setup.serviceToRespondToRecord = serialize.ReadAll();
            } else if (setup.eventType == onComparisonOfValues) {
                std::size_t const eventTypeRecordSize{0x0A};
                while (setup.eventTypeRecord.size() != eventTypeRecordSize) {
                    setup.eventTypeRecord.push_back(serialize.ReadUint8());
                }
                setup.serviceToRespondToRecord = serialize.ReadAll();
            }
            std::shared_ptr< Message > respond{context->CreateResponse()};
            Serialize writer{respond->GetBody()};
            if (setup.eventType == startResponseOnEvent) {
                NrcErrc result = diagnosticRoeManagerPtr_->Start(setup.store, context);
                if (result != NrcErrc::kSuccess) {
                    LogWarn() << "ServiceX86::ParseAndResponse|start event fails!";
                    return context->Respond(result);
                }
                writer.Write(setup.eventType);
                std::uint8_t const numberOfIdentifiedEvents{0};
                writer.Write(numberOfIdentifiedEvents);
                writer.Write(setup.eventWindowTime);
            } else if (setup.eventType == stopResponseOnEvent) {
                diagnosticRoeManagerPtr_->Stop();
                writer.Write(setup.eventType);
                std::uint8_t const numberOfIdentifiedEvents{0};
                writer.Write(numberOfIdentifiedEvents);
                writer.Write(setup.eventWindowTime);
            } else if (setup.eventType == clearResponseOnEvent) {
                diagnosticRoeManagerPtr_->Clear();
                writer.Write(setup.eventType);
                std::uint8_t const numberOfIdentifiedEvents{0};
                writer.Write(numberOfIdentifiedEvents);
                writer.Write(setup.eventWindowTime);
            } else if (setup.eventType == reportActivatedEvents) {
                std::uint8_t eventType{0U};
                std::uint8_t eventWindowTime{0U};
                std::vector< std::uint8_t > eventTypeRecord;
                std::vector< std::uint8_t > serviceToRespondToRecord;
                std::ignore = diagnosticRoeManagerPtr_->Report(eventType, eventWindowTime, eventTypeRecord,
                                                               serviceToRespondToRecord);
                writer.Write(setup.eventType);
                std::uint8_t const numberOfActivatedEvents{1};
                writer.Write(numberOfActivatedEvents);
                writer.Write(eventType);
                writer.Write(eventWindowTime);
                writer.Write(eventTypeRecord);
                writer.Write(serviceToRespondToRecord);
            } else {
                diagnosticRoeManagerPtr_->Setup(setup);
                writer.Write(setup.eventType);
                std::uint8_t const numberOfIdentifiedEvents{0};
                writer.Write(numberOfIdentifiedEvents);
                writer.Write(setup.eventWindowTime);
                writer.Write(setup.eventTypeRecord);
                writer.Write(setup.serviceToRespondToRecord);
            }
            return context->Respond(std::move(respond));
        }
    }
    LogWarn() << "ServiceX86::ParseAndResponse| request message is nullptr ";
    return;
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
