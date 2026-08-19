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
/// @file       service_x2a.cpp
/// @brief      This file implements service_x2a functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=data_management
/// @trace_id_sr=d4b9ac8d-2b1b-4b5b-a3b9-52740cb6106f
/// @unit_name=service_x2a.h
/// @unit_description=This file provides service_x2a functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_x2a.h"

#include "common/log.h"
#include "common/serialize.h"
#include "common/thread_pool.h"
#include "isoft/uds/type_int.h"

namespace isoft {
namespace uds {
namespace server {

ServiceX2A::Scheduler::Scheduler() noexcept
    : rate_{}, timer_{}, getPDIDTableCallback_{}, dataIdentifierManager_{nullptr}, context_{nullptr}
{
}

std::int32_t ServiceX2A::Scheduler::Initialize(DiagnosticPeriodicRate rates,
                                               GetPDIDTableCallback getPDIDTableCallback,
                                               std::shared_ptr< DiagnosticDataManager > dataIdentifierManager) noexcept
{
    rate_                  = rates;
    getPDIDTableCallback_  = std::move(getPDIDTableCallback);
    dataIdentifierManager_ = std::move(dataIdentifierManager);
    std::ignore
        = timer_.Initialize([this]() { std::ignore = ThreadPool::GetInstance().Submit([this]() { _timeOut(); }); });
    return 0;
}

void ServiceX2A::Scheduler::_timeOut() noexcept
{
    if (getPDIDTableCallback_) {
        std::vector< std::uint8_t > pdidTable{};
        pdidTable = getPDIDTableCallback_(rate_.type);
        if (pdidTable.empty()) {
            LogInfo() << "ServiceX2A::Scheduler::_timeOut| get PDID table is empty!";
            return;
        }
        std::vector< std::uint16_t > diagnsoticDataTable;
        std::uint16_t const pdidMask{kMinPeriodicDataIdentifier};
        for (auto &&pdid : pdidTable) {
            diagnsoticDataTable.push_back(pdidMask | pdid);
        }
        Result< std::list< DiagnosticData > > readResult;
        MetaInfoMap metaInfo{context_->GetMetaInfoMap()};
        std::shared_ptr< CancellationState > cancellationState{};
        cancellationState = context_->GetCancellationState();
        readResult        = dataIdentifierManager_->Read(diagnsoticDataTable, metaInfo, cancellationState);
        if (!readResult.HasValue()) {
            LogInfo() << "ServiceX2A::Scheduler::_timeOut| read data fails!";
        } else {
            std::vector< MessagePtr > responseMessageTable;
            for (auto &&readData : readResult.Value()) {
                if (readData.record.empty()) {
                    LogWarn() << "ServiceX2A::Scheduler::_timeOut| read data fails!did:" << readData.id;
                    continue;
                }
                MessagePtr messagePtr{};
                messagePtr = context_->CreateResponse();
                if (messagePtr.get() == nullptr) {
                    LogWarn() << "ServiceX2A::Scheduler::_timeOut| create message fails!";
                    continue;
                }
                Buffer &body{messagePtr->GetBody()};
                body.clear();
                Serialize serialize{body};
                serialize.Write(static_cast< uint8_t >(readData.id));
                serialize.Write(readData.record);
                responseMessageTable.push_back(messagePtr);
            }
            context_->Respond(responseMessageTable);
            std::ignore = timer_.Start(rate_.timeout);
        }
    }
}

void ServiceX2A::Scheduler::Start(std::shared_ptr< RequestContext > context) noexcept
{
    context_ = std::move(context);
    if (!timer_.IsRunning()) {
        std::ignore = timer_.Start(rate_.timeout);
    }
}
void ServiceX2A::Scheduler::Stop() noexcept
{
    std::ignore = timer_.Stop();
    context_.reset();
}

ServiceX2A::ServiceX2A() noexcept
    : sessionValidator_{std::make_shared< ServerLevelSessionValidator< std::uint16_t > >()}
    , securityLevelValidator_{std::make_shared< ServerLevelSecurityLevelValidator< std::uint16_t > >()}
{
}

std::int32_t ServiceX2A::Initialize(ServiceX2AModel &model,
                                    std::shared_ptr< ServiceX22 > &serviceX22Ptr,
                                    std::shared_ptr< SessionManager > &sessionManagerPtr,
                                    std::shared_ptr< SecurityLevelManager > &securityLevelManagerPtr,
                                    std::shared_ptr< DiagnosticDataManager > &diagnosticDataManagerPtr) noexcept
{
    if (serviceX22Ptr.get() == nullptr) {
        LogWarn() << "serviceX22Ptr is nullptr";
        return -1;
    }
    serviceX22Ptr_ = serviceX22Ptr;
    if (sessionManagerPtr.get() == nullptr) {
        LogWarn() << "sessionManagerPtr is nullptr";
        return -1;
    }
    if (securityLevelManagerPtr.get() == nullptr) {
        LogWarn() << "securityLevelManagerPtr is nullptr";
        return -1;
    }

    if (diagnosticDataManagerPtr.get() == nullptr) {
        LogWarn() << "diagnosticDataManager is nullptr";
        return -1;
    }
    dataIdentifierManager_ = diagnosticDataManagerPtr;
    checkPerSourceId_      = model.checkPerSourceId;
    maxPeriodicDidToRead_  = model.maxPeriodicDidToRead;
    if (model.schedulerMaxNumber == 0U) {
        LogWarn() << "ServiceX2AModel.schedulerMaxNumber must > 0";
        return -1;
    }
    schedulerMaxNumber_ = model.schedulerMaxNumber;
    std::size_t const maxNumOfRate{3};
    if (model.rates.empty() || model.rates.size() > maxNumOfRate) {
        LogWarn() << "ServiceX2AModel.rates size must [1,3]";
        return -1;
    }
    checkPerSourceId_ = model.checkPerSourceId;

    NotifyOfSessionChangeCallback sessionChangeCallback{[this](SessionId oldSessionId, SessionId newSessionId) {
        ThreadPool::GetInstance().Submit(
            [this, oldSessionId, newSessionId]() { _clearWhenSessionChange(oldSessionId, newSessionId); });
    }};
    std::ignore = sessionManagerPtr->AddNotifyOfSessionChange(sessionChangeCallback);
    if (checkPerSourceId_) {
        NotifyOfSecurityLevelChangeCallback securityLevelCallback{[this](isoft::uds::server::SecurityLevelId id) {
            ThreadPool::GetInstance().Submit([this, id]() { _clearWhenSecurityLevelChange(id); });
        }};
        std::ignore = securityLevelManagerPtr->AddNotifyOfSecurityLevelChange(securityLevelCallback);
    }

    for (auto &&rate : model.rates) {
        SchedulerPtr schedulerPtr{};
        schedulerPtr = std::make_shared< ServiceX2A::Scheduler >();
        GetPDIDTableCallback getPDIDTableCallback{

            [this](DiagnosticPeriodicRateCategoryEnum rateType) { return _get(rateType); }};
        schedulerPtr->Initialize(rate, getPDIDTableCallback, diagnosticDataManagerPtr);
        std::ignore = schedulerTable_.insert({rate.type, schedulerPtr});
    }
    for (size_t did = kMinPeriodicDataIdentifier; did <= kMaxDynamicallyDefinedDataIdentifier; did++) {
        std::shared_ptr< SubfunctionLevelSessionValidator< std::uint16_t > > sessionValidator
            = serviceX22Ptr_->GetSessionValidator(did);
        if (sessionValidator.get() != nullptr) {
            sessionValidator_->RegisterSubfunctionLevel(did, sessionValidator);
        }
        std::shared_ptr< SubfunctionLevelSecurityLevelValidator< std::uint16_t > > securityLevelValidator
            = serviceX22Ptr_->GetSecurityLevelValidator(did);
        if (securityLevelValidator.get() != nullptr) {
            securityLevelValidator_->RegisterSubfunctionLevel(did, securityLevelValidator);
        }
    }
    p4ServerMax_ = model.p4ServerMax;
    return 0;
}

void ServiceX2A::Process(std::shared_ptr< RequestContext > context) noexcept
{
    MessagePtr &request{context->GetRequest()};

    ///  [SWS_DM_00098]
    Serialize deserialize{request->GetBody()};
    std::uint8_t subfunction{0U};
    std::uint8_t const stopSubfunction{4U};
    deserialize.Seek(1U);
    subfunction = deserialize.ReadUint8();
    std::size_t const bodySize{request->GetBody().size()};
    if (subfunction == stopSubfunction) {
        std::size_t const minBodySize{2U};
        std::size_t const maxBodySize{maxPeriodicDidToRead_ + minBodySize};
        if ((minBodySize > bodySize) || (maxBodySize < bodySize)) {
            LogWarn() << "request data size < 2 byte and < " << maxBodySize << " byte !";
            return context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat, true);
        }
        if (bodySize == minBodySize) {
            _clearAll();
            for (auto &&scheduler : schedulerTable_) {
                scheduler.second->Stop();
            }
        } else {
            std::vector< std::uint8_t > pdidTable;
            pdidTable = deserialize.ReadAll();
            if (!_remove(pdidTable)) {
                LogWarn() << "ServiceX2A::Process| stop fails,exist invalid ppid";
                return context->Respond(NrcErrc::kRequestOutOfRange, true);
            }
        }
    } else {
        std::size_t const minBodySize{3U};
        std::size_t const maxBodySize{maxPeriodicDidToRead_ + minBodySize};
        if ((minBodySize > bodySize) || (maxBodySize <= bodySize)) {
            LogWarn() << "request data size < 2 byte and < " << maxBodySize << " byte !";
            return context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat, true);
            ;
        }

        std::size_t const numOfPeriodicId(bodySize - 2U);
        if (maxPeriodicDidToRead_ < numOfPeriodicId) {
            LogWarn() << "Check requested number of periodic DataIdentifiers fails! maxPeriodicDidToRead:"
                      << maxPeriodicDidToRead_ << " requested number of periodic DataIdentifiers:" << numOfPeriodicId;
            return context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat, true);
        }

        DiagnosticPeriodicRateCategoryEnum transmissionMode{
            static_cast< DiagnosticPeriodicRateCategoryEnum >(subfunction)};
        auto transmissionModeResult = schedulerTable_.find(transmissionMode);
        if (transmissionModeResult == schedulerTable_.end()) {
            LogWarn() << "transmissionMode not support! transmissionMode:" << subfunction;
            return context->Respond(NrcErrc::kRequestOutOfRange, true);
        }

        std::size_t numberOfExistingPDIDs{0U};
        for (auto &&periodicIdentifierState : periodicIdentifierStateTable_) {
            if (periodicIdentifierState.second == transmissionMode) {
                numberOfExistingPDIDs++;
            }
        }
        if (numberOfExistingPDIDs + numOfPeriodicId > schedulerMaxNumber_) {
            LogWarn() << "the number existing PDIDs and the new PDIDs from the request is larger than "
                         "ServiceX2AModel.schedulerMaxNumber";
            return context->Respond(NrcErrc::kRequestOutOfRange, true);
        }
        std::uint8_t pdid{0};
        bool checkResult{true};
        std::vector< std::uint8_t > validPdidTable;
        while (deserialize.Read(pdid)) {
            std::uint16_t did{kMinPeriodicDataIdentifier};
            did += pdid;
            if (!dataIdentifierManager_->HasDataIdentifier(did)) {
                LogWarn() << "ServiceX2A::Process| invalid pdid:" << LogHex8{pdid};
                checkResult = false;
                continue;
            }
            std::size_t didSize{};

            didSize = dataIdentifierManager_->GetDataSize(did);
            if (didSize > context->GetMaxPayloadLength()) {
                LogWarn() << "ServiceX2A::Process| diagnostic data size > max payload length. data size:" << didSize
                          << " max payload length:" << context->GetMaxPayloadLength();
                return context->Respond(NrcErrc::kResponseTooLong);
            }
            if (didSize < 1U) {
                LogWarn() << "ServiceX2A::Process| diagnostic data size too short";
                return context->Respond(NrcErrc::kRequestOutOfRange);
            }
            checkResult = serviceX22Ptr_->CheckDataIdentifierSession(did);
            if (!checkResult) {
                continue;
            }
            checkResult = serviceX22Ptr_->CheckDataIdentifierSecurityLevel(did);
            if (!checkResult) {
                LogWarn() << "ServiceX2A::Process| security access check fails,pdid:" << pdid;
                return context->Respond(NrcErrc::kSecurityAccessDenied, true);
            }

            checkResult = serviceX22Ptr_->CheckDataIdentifierCondition(did, context->GetMetaInfoMap(),
                                                                       context->GetCancellationState());
            if (!checkResult) {
                LogWarn() << "ServiceX2A::Process| security access check fails,pdid:" << pdid;
                return context->Respond(NrcErrc::kConditionsNotCorrect, true);
            }

            validPdidTable.push_back(pdid);
        }
        if (!checkResult) {
            LogWarn() << "ServiceX2A::Process| at least one pDID is not supported in the active session!";
            return context->Respond(NrcErrc::kRequestOutOfRange, true);
        }
        _add(transmissionMode, validPdidTable);
        transmissionModeResult->second->Start(context);
    }
    MessagePtr responsePtr{nullptr};
    responsePtr = context->CreateResponse();
    context->Respond(std::move(responsePtr));
    return;
}

std::vector< ValidatorPtr > ServiceX2A::GetSidValidator() noexcept
{
    std::vector< ValidatorPtr > validatorTable{};
    return validatorTable;
}

void ServiceX2A::Confirm(RequestContext &request, ResponseStatusType status) noexcept
{
    std::ignore = request;
    std::ignore = status;
}
void ServiceX2A::_add(DiagnosticPeriodicRateCategoryEnum rate, std::vector< std::uint8_t > &pdidTable) noexcept
{
    std::unique_lock< std::mutex > lock(periodicIdentifierStateMutex_);
    for (auto &&pdid : pdidTable) {
        std::ignore = periodicIdentifierStateTable_.insert({pdid, rate});
    }
}

std::vector< std::uint8_t > ServiceX2A::_get(DiagnosticPeriodicRateCategoryEnum rate) noexcept
{
    std::unique_lock< std::mutex > lock(periodicIdentifierStateMutex_);
    std::vector< std::uint8_t > result;
    for (auto &&pdid : periodicIdentifierStateTable_) {
        if (pdid.second == rate) {
            result.push_back(pdid.first);
        }
    }
    return result;
}

void ServiceX2A::_clearWhenSessionChange(SessionId, SessionId newSessionId) noexcept
{
    std::unique_lock< std::mutex > lock(periodicIdentifierStateMutex_);
    std::uint16_t const pdidMask{0xF200U};
    std::vector< std::uint8_t > needRemoveTable;
    if (newSessionId == 0x01U) {
        LogWarn() << "ServiceX2A::ClearWhenSessionChange| stop all pdid";
        periodicIdentifierStateTable_.clear();
        return;
    }
    if (checkPerSourceId_) {
        for (auto &&periodicIdentifierState : periodicIdentifierStateTable_) {
            bool sourceCheck{};
            std::uint16_t did{};
            did         = pdidMask | periodicIdentifierState.first;
            sourceCheck = serviceX22Ptr_->CheckDataIdentifierSession(did);
            if (!sourceCheck) {
                LogWarn() << "ServiceX2A::ClearWhenSessionChange|  data Identifier:" << did << " check session fails!";
                needRemoveTable.push_back(periodicIdentifierState.first);
            }
        }
        for (auto &&pdid : needRemoveTable) {
            std::ignore = periodicIdentifierStateTable_.erase(pdid);
        }
    }
}

void ServiceX2A::_clearWhenSecurityLevelChange(SecurityLevelId) noexcept
{
    std::unique_lock< std::mutex > lock(periodicIdentifierStateMutex_);
    std::uint16_t const pdidMask{0xF200U};
    std::vector< std::uint8_t > needRemoveTable;
    for (auto &&periodicIdentifierState : periodicIdentifierStateTable_) {
        bool sourceCheck{};
        std::uint16_t did{};
        did         = pdidMask | periodicIdentifierState.first;
        sourceCheck = serviceX22Ptr_->CheckDataIdentifierSecurityLevel(did);
        if (!sourceCheck) {
            LogWarn() << "ServiceX2A::ClearWhenSessionChange|  data Identifier:" << did << " check session fails!";
            needRemoveTable.push_back(periodicIdentifierState.first);
        }
    }
    for (auto &&pdid : needRemoveTable) {
        std::ignore = periodicIdentifierStateTable_.erase(pdid);
    }
}

void ServiceX2A::_clearAll() noexcept
{
    std::unique_lock< std::mutex > lock(periodicIdentifierStateMutex_);
    periodicIdentifierStateTable_.clear();
}

bool ServiceX2A::_remove(std::vector< std::uint8_t > &pdidTable) noexcept
{
    std::unique_lock< std::mutex > lock(periodicIdentifierStateMutex_);
    for (auto &&pid : pdidTable) {
        std::size_t num = periodicIdentifierStateTable_.erase(pid);
        if (num == 0) {
            LogWarn() << "ServiceX2A::Remove| remove fails! invalid ppid:" << pid;
            return false;
        }
    }
    return true;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
