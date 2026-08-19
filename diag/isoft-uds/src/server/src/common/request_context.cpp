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
/// @file       request_context.cpp
/// @brief      This file implements request_context functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=common
/// @trace_id_sr=e81c720c-396c-4cab-b547-98c0a2aad431
/// @unit_name=RequestContext
/// @unit_description=This file provides request_context functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "common/request_context.h"

#include <array>
#include <cinttypes>
#include <cstdint>
#include <iostream>

#include "common/log.h"
#include "common/serialize.h"
#include "common/thread_pool.h"
#include "isoft/uds/type_int.h"
#include "server_impl.h"
namespace isoft {
namespace uds {
namespace server {
std::uint32_t const kAdvanceP2{4};  /// P2 timing advance margin
RequestContext::RequestContext(ServerImpl &impl,
                               MessagePtr request,
                               Address serverAddress,
                               std::vector< Address > functionalAddress,
                               std::shared_ptr< Channel > channelPtr,
                               MetaInfoMap metaInfo,
                               Priority priority,
                               RequestContextConfirmCallback confirmCallback,
                               RequestContextSaveCallback saveCallback) noexcept
    : impl_{impl}
    , request_{std::move(request)}
    , serverAddress_{serverAddress}
    , functionalAddress_{std::move(functionalAddress)}
    , channelPtr_{std::move(channelPtr)}
    , metaInfo_{std::move(metaInfo)}
    , priority_{priority}
    , confirmCallback_{std::move(confirmCallback)}
    , saveCallback_{std::move(saveCallback)}
    , cancellationState_{std::make_shared< CancellationState >()}
{
    // TODO: Lambda expressions cannot be nested
    std::ignore
        = p2Timer_.Initialize([this]() { std::ignore = ThreadPool::GetInstance().Submit([this]() { _timeout(); }); });

    // Create p4 timer
    int32_t const res = p4Timer_.Initialize([this]() { _p4Timeout(); });
    if (res < 0) {
        LogWarn() << "RequestContext::RequestContext|make p4 timer faile";
    }
}

std::int32_t RequestContext::StartP2ServerTimer(
    std::uint32_t p2ServerMax,
    std::uint32_t p2StarServerMax,
    std::uint8_t maxNumberOfRequestCorrectlyReceivedResponsePending) noexcept
{
    _changeState(RequestContextState::kRequest);
    p2ServerMax_                                        = p2ServerMax;
    p2StarServerMax_                                    = p2StarServerMax;
    maxNumberOfRequestCorrectlyReceivedResponsePending_ = maxNumberOfRequestCorrectlyReceivedResponsePending;
    if (p2ServerMax > kAdvanceP2) {
        p2ServerMax -= kAdvanceP2;
    } else {
        p2ServerMax = 1;
    }

    _startP4ServerMax();

    return p2Timer_.Start(p2ServerMax);
}

std::int32_t RequestContext::StopP2ServerTimer() noexcept
{
    _stopP4ServerMax();
    return p2Timer_.Stop();
}

void RequestContext::NotifyRequestContextState(NotifyRequestContextStateCallback const &callback) noexcept
{
    notifyCallbackTable_.push_back(callback);
}

std::shared_ptr< Message > RequestContext::CreateResponse() noexcept
{
    std::shared_ptr< Message > response{std::make_shared< Message >(serverAddress_, request_->GetSA())};
    if (response.get() != nullptr) {
        Serialize serialize{response->GetBody()};
        std::uint8_t responseSid{static_cast< std::uint8_t >(sid_ | kResponseServiceIdMask)};
        serialize.Write(responseSid);
    }
    return response;
}

std::shared_ptr< Message > RequestContext::CreateResponse(std::uint8_t const nrc) noexcept
{
    std::shared_ptr< Message > response{std::make_shared< Message >(request_->GetTA(), request_->GetSA())};
    if (response.get() != nullptr) {
        Serialize serialize{response->GetBody()};
        std::uint8_t const nrsid{0x7f};
        serialize.Write(nrsid);
        serialize.Write(sid_);
        serialize.Write(nrc);
    }
    return response;
}

std::shared_ptr< Message > RequestContext::CreateResponse(NrcErrc const nrc) noexcept
{
    std::uint8_t uNrc{static_cast< std::uint8_t >(nrc)};
    return CreateResponse(uNrc);
}


void RequestContext::Respond(std::uint8_t const nrc, bool ignoreServiceConfirm) noexcept
{
    std::shared_ptr< Message > responsePtr{CreateResponse(nrc)};

    return Respond(std::move(responsePtr), ignoreServiceConfirm);
}

void RequestContext::Respond(NrcErrc const nrc, bool ignoreServiceConfirm) noexcept
{
    std::uint8_t uNrc{static_cast< std::uint8_t >(nrc)};

    return Respond(uNrc, ignoreServiceConfirm);
}


void RequestContext::Respond(MessagePtr &&msgPtr, bool ignoreServiceConfirm) noexcept
{
    _changeState(RequestContextState::kResponse);
    bool isCancelled = IsCancelled();
    if (isCancelled) {
        confirmCallback_(*this, ResponseStatusType::kCanceled, ignoreServiceConfirm);
        return;
    }
    StopP2ServerTimer();
    bool isNegtiveType = _isNegtiveType(msgPtr);
    if (isNegtiveType) {
        std::uint32_t const kNrcPosInMsg{0x05U};
        std::uint8_t nrc = msgPtr->GetBody()[kNrcPosInMsg];
        if (_suppressNegtive(nrc)) {
            _changeState(RequestContextState::kCanceled);
            confirmCallback_(*this, ResponseStatusType::kResNegSuppressed, ignoreServiceConfirm);
        } else {
            auto result = channelPtr_->Respond(msgPtr);
            _changeState(RequestContextState::kConfirm);
            if (result.HasValue() && result.Value()) {
                confirmCallback_(*this, ResponseStatusType::kResNegOk, ignoreServiceConfirm);
            } else {
                confirmCallback_(*this, ResponseStatusType::kResNegNotOk, ignoreServiceConfirm);
            }
        }
    } else {
        if (_suppressPositive()) {
            confirmCallback_(*this, ResponseStatusType::kResPosSuppressed, ignoreServiceConfirm);
            _changeState(RequestContextState::kConfirm);
        } else {
            auto result = channelPtr_->Respond(msgPtr);
            _changeState(RequestContextState::kConfirm);
            if (result.HasValue() && result.Value()) {
                confirmCallback_(*this, ResponseStatusType::kResPosOk, ignoreServiceConfirm);
            } else {
                confirmCallback_(*this, ResponseStatusType::kResPosNotOk, ignoreServiceConfirm);
            }
        }
    }
}

void RequestContext::Respond(std::vector< MessagePtr > &msgPtrTable) noexcept
{
    std::ignore = channelPtr_->Respond(msgPtrTable);
}



std::size_t RequestContext::GetMaxPayloadLength() noexcept
{
    if (channelPtr_.get() != nullptr) {
        return channelPtr_->GetMaxPayloadLength();
    }
    LogWarn() << "RequestContext::GetMaxPayloadLength| channel is nullptr";
    return 0;
}


bool RequestContext::_suppressNegtive(std::uint8_t const nrc) noexcept
{
    ///  [SWS_DM_00862]
    {
        std::unique_lock< std::mutex > lock{lock_};
        if (isPending_) {
            LogInfo() << "RequestContext::_suppressNegtive|send pending,cancel "
                         "suppress negtive";
            return false;
        }
    }
    NrcErrc const nrcErrc{static_cast< NrcErrc >(nrc)};
    return (_isFunctionalAddress())
           && ((nrcErrc == NrcErrc::kServiceNotSupported) || (nrcErrc == NrcErrc::kSubfunctionNotSupported)
               || (nrcErrc == NrcErrc::kRequestOutOfRange) || (nrcErrc == NrcErrc::kServiceNotSupportedInActiveSession)
               || (nrcErrc == NrcErrc::kSubFunctionNotSupportedInActiveSession));
}

bool RequestContext::_suppressPositive() noexcept
{
    std::array< std::uint8_t, kInt32_10U > supportSubfunctionSidTable{kSessionControlId,
                                                                      kEcuResetId,
                                                                      kReadDtcInformationId,
                                                                      kCommunicationControlId,
                                                                      kDynamicallyDefineDataIdentifierId,
                                                                      kTesterPresentId,
                                                                      kRequestFileTransferId,
                                                                      kControlDTCSettingId,
                                                                      kResponseOnEventId};
    auto *findResult = std::find(supportSubfunctionSidTable.begin(), supportSubfunctionSidTable.end(), sid_);
    if (findResult == supportSubfunctionSidTable.end()) {
        return false;
    }
    std::uint8_t const suppressPositiveMask{0x80};

    if (0 != ((request_->GetBody()[1]) & suppressPositiveMask)) {
        {
            std::unique_lock< std::mutex > lock{lock_};
            if (isPending_) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool RequestContext::_isFunctionalAddress() const noexcept
{
    Address const serverAddress{request_->GetTA()};
    for (auto &&eachAddress : functionalAddress_) {
        if (eachAddress == serverAddress) {
            LogVerbose() << "RequestContext::_isFunctionalAddress| current server "
                            "address is function address!";
            return true;
        }
    }

    LogInfo() << "RequestContext::_isFunctionalAddress| not functional address! "
                 "serverAddress:"
              << serverAddress;
    return false;
}
bool RequestContext::_isNegtiveType(std::shared_ptr< Message > const &msgPtr) noexcept
{
    std::vector< std::uint8_t > &body{msgPtr->GetBody()};
    std::uint8_t const negativeResponseServiceIdentifier{kNegativeResponseServiceId};
    return body.front() == negativeResponseServiceIdentifier;
}

void RequestContext::Cancelled() noexcept
{
    _changeState(RequestContextState::kCanceled);
    cancellationState_->DoCanceled();
}

bool RequestContext::IsCancelled() noexcept { return state_ == RequestContextState::kCanceled; }

void RequestContext::_changeState(RequestContextState state) noexcept
{
    if (state_ != RequestContextState::kCanceled) {
        state_ = state;
        _notify();
    }
}

bool RequestContext::ReestablishAfterRestarted(uint16_t const ta) noexcept
{
    return channelPtr_->ReestablishAfterRestarted(ta);
}

void RequestContext::_notify() noexcept
{
    for (auto &&notify : notifyCallbackTable_) {
        Address sa{request_->GetSA()};
        Address ta{request_->GetTA()};
        notify(sa, ta, state_);
    }
}

void RequestContext::_timeout() noexcept
{

    if (p4ServerMax_ != 0U && p4ServerMax_ == p2ServerMax_) {
        LogInfo() << "RequestContext::_timeout|forbidden to send 0x78 when p2 equal p4, p2 =" << p2ServerMax_
                  << ", p4 =" << p4ServerMax_ << ", sid =" << ara::log::LogHex8{sid_};
        return;
    }

    if (maxNumberOfRequestCorrectlyReceivedResponsePending_ != static_cast< std::uint8_t >(UINT8_MAX)) {
        if (maxNumberOfRequestCorrectlyReceivedResponsePending_ == numberOfRequestCorrectlyReceivedResponsePending_) {
            ///  [SWS_DM_00369] Cancel processing
            std::shared_ptr< Message > responsePtr{CreateResponse(NrcErrc::kGeneralReject)};
            std::ignore = channelPtr_->Respond(responsePtr);
            Cancelled();
            return;
        }
        ++numberOfRequestCorrectlyReceivedResponsePending_;
    }
    ///  [SWS_DM_00368]
    std::shared_ptr< Message > responsePtr{CreateResponse(NrcErrc::kRequestCorrectlyReceivedResponsePending)};
    isoft::uds::Result< bool > sendResult = channelPtr_->Respond(responsePtr);
    if (sendResult.HasValue() && sendResult.Value()) {
        uint32_t duration{p2StarServerMax_ * kInt32_10U};
        if (duration > kAdvanceP2) {
            duration -= kAdvanceP2;
        } else {
            duration = 1;
        }
        std::int32_t const status{p2Timer_.Start(duration)};
        if (status != 0) {
            LogWarn() << "RequestContext::_timeout| start p2 timer fails! status:" << status;
        }
        {
            std::unique_lock< std::mutex > lock{lock_};
            isPending_ = true;
        }
    } else {
        LogWarn() << "RequestContext::_timeout| send fails! stop send 0x78 message";
        Cancelled();
    }
}

void RequestContext::End() noexcept
{
    _changeState(RequestContextState::kEnd);
    LogInfo() << "RequestContext::End| end request!";
}

void RequestContext::_p4Timeout() noexcept
{
    if (channelPtr_.get() == nullptr) {
        LogWarn() << "RequestContext::_p4Timeout|channelPtr change invalid";
        return;
    }

    std::shared_ptr< Message > responsePtr{CreateResponse(NrcErrc::kGeneralReject)};
    std::ignore = channelPtr_->Respond(responsePtr);
    Cancelled();

    StopP2ServerTimer();
    LogDebug() << "RequestContext::_p4Timeout|p4 timeout and send ";
}

void RequestContext::_startP4ServerMax() noexcept
{
    std::shared_ptr< ServiceInterface > service = impl_.GetServiceBySid(sid_);
    if (service.get() == nullptr) {
        LogWarn() << "RequestContext::_startP4ServerMax|get service fail";
        return;
    }

    if (request_.get() == nullptr) {
        LogWarn() << "RequestContext::_startP4ServerMax|request is nullptr";
        return;
    }

    std::uint32_t p4ServerMax{0U};
    if (sid_ == 0x31U) {
        decltype(auto) requestBody{request_->GetBody()};
        if (requestBody.size() < 4U) {
            LogWarn()
                << "RequestContext::_startP4ServerMax|0x31 start p4 faile, beacuse request body size is less than 4";
            return;
        }

        Serialize deserialize{requestBody};
        std::ignore              = deserialize.ReadUint8();
        std::uint8_t subfunction = deserialize.ReadUint8();
        std::uint16_t id         = deserialize.ReadUint16();
        p4ServerMax = service->GetP4ServerMax(static_cast< int16_t >(subfunction), static_cast< int32_t >(id));
    } else {
        p4ServerMax = service->GetP4ServerMax();
    }

    if (p4ServerMax == 0U) {
        LogInfo() << "RequestContext::_startP4ServerMax|don't config p4ServerMax, sid =" << ara::log::LogHex8{sid_};
        return;
    }

    std::int32_t const res = p4Timer_.Start(p4ServerMax);
    if (res < 0) {
        LogWarn() << "RequestContext::_p4Timeout|p4Timer Start fail, sid =" << ara::log::LogHex8{sid_};
        return;
    }

    p4ServerMax_ = p4ServerMax;
    LogDebug() << "RequestContext::_startP4ServerMax|success, sid =" << ara::log::LogHex8{sid_}
               << ",p4 =" << p4ServerMax_;
}

void RequestContext::_stopP4ServerMax() noexcept
{
    if (!p4Timer_.IsRunning()) {
        return;
    }

    std::int32_t const res = p4Timer_.Stop();
    if (res < 0) {
        LogWarn() << "RequestContext::_p4Timeout|p4Timer Stop fail";
    }
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
