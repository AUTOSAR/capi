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
/// @file       idsm_transmit.cpp
/// @brief      Security event remote transmission implementation class
/// @details
/// @date       2023-02-15
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/qualified security event storage
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0011
/// @unit_name=IdsmWriter
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_transmit.h"

#include <arpa/inet.h>
#include <sys/types.h>

#include <chrono>
#include <thread>

#include "ara/idsm/internal/idsm_error_domain.h"
#include "config/idsm_struct_init.h"
#include "event/idsm_event_memory.h"
#include "event/idsm_event_sink.h"
#include "log/idsm_log.h"
#ifdef ARA_WITH_DIAG
    #include "diag/idsm_diag_did_server.h"
    #include "diag/idsm_diag_dtc_server.h"
#endif
namespace ara {
namespace idsm {
void IdsmWriter::StartMonitor()
{
    /// @brief Start data transmission thread
    Init();
    std::thread t{ara::idsm::IdsmWriter::Run, this};
    t.detach();
}

void IdsmWriter::StopMonitor()
{
    /// @details Set thread exit flag
    threadExit_ = true;

    /// @details Sleep for 1 second waiting for thread exit
    uint8_t const timeSleepSec{1U};
    std::this_thread::sleep_for(std::chrono::seconds(timeSleepSec));

    /// @details Check every 0.05 seconds whether the thread has exited
    uint8_t const timeSleepMiSec{50U};
    while (true) {
        if (threadStop_) {
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(timeSleepMiSec));
    }
    return;
}
/// @brief Entry function of the qualified security event storage thread
/// @param arg Thread entry function parameter
/// @exception Stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void IdsmWriter::Run(void* const arg)
{
    IdsmWriter* const threadPtr{static_cast< IdsmWriter* >(arg)};
    threadPtr->threadStop_ = false;

    LOG_INFO << "idsm writer backgroud thread start";
    /// @details Outer loop retrieves security events, inner loop is responsible for sending security events
    while (true) {
        std::list< EventPtr > events;
        /// @details When the interface returns, the events list may be empty
        EventPool::GetInstance()->Get(events);

        /// @details Check thread exit flag
        if (threadPtr->threadExit_) {
            threadPtr->threadStop_ = true;
            return;
        }
        /// @details Check if there are security events to send
        if (events.empty()) {
            continue;
        }

        /// @details Send the retrieved security events
        while (true) {
            /// @details All events in the list have been sent
            if (events.empty()) {
                break;
            }
            /// @details Select one event from the list to send
            EventPtr const event{events.front()};
            IdsmErrorCode retCode{threadPtr->_write(event)};
            /// @details When handling communication exceptions, the data sending thread exits
            if (retCode == IdsmErrorCode::kIdsmThreadExitSign) {
                threadPtr->threadStop_ = true;
                return;
            }

            /// @details When event sending ends, check if the data sending thread has exited
            events.pop_front();
            if (threadPtr->threadExit_) {
                threadPtr->threadStop_ = true;
                return;
            }
        }
    }
}
/// @brief Initialize
/// @exception Stack overflow exception
void IdsmWriter::Init() noexcept
{
    internalEvents_ = IdsmStruct::GetInstance()->GetInternalEvent();
#ifdef ARA_WITH_DIAG
    diagDtcServerMap_ = IdsmStruct::GetInstance()->GetDiagDtcServer();
#endif
    /// @brief Get key slot name
    ara::core::String slotName{IdsmStruct::GetInstance()->GetSignSlotName()};
    /// @brief Get encryption algorithm name
    ara::core::String algName{IdsmStruct::GetInstance()->GetSignAlgName()};
    /// @brief IDS protocol serialization
    serializer_ = std::make_shared< IdsmSerialize >(IdsmStruct::GetInstance()->GetIdsmInstanceId(), slotName, algName);

    /// @brief Get IDSR address
    ara::core::String ipAddr{IdsmStruct::GetInstance()->GetIdsrIpAddr()};
    /// @brief Get IDSR listening port
    uint16_t const tcpPort{IdsmStruct::GetInstance()->GetIdsrPort()};
    /// @brief Initialize synchronous sender
    _initIdsmSender(ipAddr, tcpPort, false);

    // Initialize rate limiting and traffic shaping filters
    /// @brief Get rate limiting period and the upper limit on the number of security events that can be sent within the period
    time_t rateIntervals{0};
    uint32_t maxEvents{0U};
    IdsmStruct::GetInstance()->GetRateLimit(rateIntervals, maxEvents);
    /// @brief Get traffic shaping period and the upper limit on the number of bytes that can be sent within the period
    time_t trafficIntervals{0};
    uint64_t maxBytes{0U};
    IdsmStruct::GetInstance()->GetTrafficLimit(trafficIntervals, maxBytes);
    /// @brief Initialize rate limiting filter
    rateChecker_.SetIntervalAndEvents(rateIntervals, maxEvents);
    /// @brief Initialize traffic shaping filter
    trafChecker_.SetIntervalAndBytes(trafficIntervals, maxBytes);
}
/// @brief Store security event
/// @param event Security event to store
/// @return Returns 0 on success
/// @exception Stack overflow exception
IdsmErrorCode IdsmWriter::_write(EventPtr event)
{
    std::shared_ptr< BytesVec > message{nullptr};
    /// @brief Check result is returned via event
    _checkAndEncode(event, message);
    if (event.get() == nullptr) {
        LOG_WARN << "idsm qualified security event transmit: rate or traffic check fail. because up to one by "
                    "transmit, so drop it";
        return IdsmErrorCode::kIdsmSuccess;
    }
#ifdef ARA_WITH_DIAG
    if (event->GetPerStorage()) {
        LOG_DEBUG << "idsm qualified security event persist: start. event id: " << event->GetEventId();
        // Does QSEV need persistence?
        if (diagDtcServerMap_.count(event->GetEventId()) == 0U) {
            LOG_ERROR << "idsm qualified security event persist: not found dtc server by event id. event id: "
                      << event->GetEventId();
        } else {
            // persist event context data to diag
            diagDtcServerMap_[event->GetEventId()]->PushData(*message);
        }
    }
#endif
    // Send to remote
    Message eventTrans;
    eventTrans.userData  = nullptr;
    eventTrans.transData = std::make_shared< TransData >(*message, event->GetContextDataSize());
    LOG_DEBUG << "idsm qualified security event transmit: start. event id: " << event->GetEventId();
    return _remoteStorage(eventTrans);
}
/// @brief Serialize the security event to be stored and check rate limiting and traffic shaping
/// @param event  Security event to be stored
/// @param message Byte stream after security event serialization
/// @exception Stack overflow exception
void IdsmWriter::_checkAndEncode(EventPtr& event, std::shared_ptr< BytesVec >& message)
{
    std::ignore = message;
    if (_isInternalEvent(event)) {
        message = _serialize(event);
        return;
    }

    uint8_t limitedCounter{0U};
    if (rateChecker_.Check(1U, limitedCounter) == false) {
        LOG_WARN << "idsm qualified security event transmit: rate check fail. event id: " << event->GetEventId();
        if (limitedCounter > 0U) {
            event.reset();
        } else {
            event   = internalEvents_.trafficEvent;
            message = _serialize(event);
        }
        return;
    }

    message        = _serialize(event);
    limitedCounter = 0U;
    if (trafChecker_.Check(message->size(), limitedCounter) == false) {
        LOG_WARN << "idsm qualified security event transmit: traffic check fail. event id: " << event->GetEventId();
        if (limitedCounter > 0U) {
            event.reset();
        } else {
            event   = internalEvents_.trafficEvent;
            message = _serialize(event);
        }
        return;
    }
}
/// @brief Determine whether it is an IDSM internal security event
/// @param event Security event to check
/// @return Returns true if it is an internal security event, otherwise returns false
/// @exception Stack overflow exception
bool IdsmWriter::_isInternalEvent(EventPtr const& event) noexcept
{
    uint16_t const eventBufId{internalEvents_.eventbufEvent->GetEventId()};
    uint16_t const contextBufId{internalEvents_.contextBufEvent->GetEventId()};
    uint16_t const trafficId{internalEvents_.trafficEvent->GetEventId()};
    uint16_t const communicationId{internalEvents_.communicationEvent->GetEventId()};
    uint16_t const eventId{event->GetEventId()};

    if ((eventId == eventBufId) || (eventId == contextBufId) || (eventId == trafficId)
        || (eventId == communicationId)) {
        return true;
    }
    return false;
}
/// @brief Handle communication exception between IDSM and IDSR
/// @exception Stack overflow exception
/// @return Return value: -1, data sending thread exits
/// @return Return value:  0, communication exception event has been sent to IDSR
IdsmErrorCode IdsmWriter::_handleComErr()
{
    Message eventTrans;
    eventTrans.transData = std::make_shared< TransData >(*_serialize(internalEvents_.communicationEvent));
    eventTrans.userData  = nullptr;
    int64_t const eventSize{static_cast< int64_t >(eventTrans.transData->Size())};
    /// @details Synchronous data send failure: repeatedly probe network, every 2 seconds
    /// @details When network recovers, send communication failure security event
    while (true) {
        int64_t const len{sender_->Send(eventTrans, true)};
        /// @brief Check thread exit flag. When the thread exits, successful sending of the communication exception event is not guaranteed
        if (threadExit_) {
            return IdsmErrorCode::kIdsmThreadExitSign;
        }

        /// @brief Communication failure event has been sent
        if (len == eventSize) {
            return IdsmErrorCode::kIdsmSuccess;
        }
        uint8_t const timeSleep{1U};
        std::this_thread::sleep_for(std::chrono::seconds(timeSleep));
    }
}
/// @brief Send security event byte stream to IDSR
/// @param eventTrans Security event byte stream to send
/// @exception Stack overflow exception
IdsmErrorCode IdsmWriter::_remoteStorage(Message const& eventTrans)
{
    if (sender_ != nullptr) {
        TransDataPtr transData{eventTrans.GetData()};
        int64_t const eventSize{static_cast< int64_t >(transData->Size())};

        /// @details Loop sending security events until successful or until the data sending thread exits
        while (true) {
            int64_t const bytes{sender_->Send(eventTrans)};
            /// @details bytes == eventSize: synchronous send succeeded, returns -1 on send failure
            /// @details bytes == 0 for asynchronous send, the Send interface returns 0. The send status is notified via callback function
            if ((bytes == 0) || (bytes == eventSize)) {
                LOG_DEBUG << "idsm qualified security event transmit: success. bytes:" << bytes;
                return IdsmErrorCode::kIdsmSuccess;
            }

            /// @details Handle synchronous send failure: loop probing network. When network recovers, send communication exception event
            /// @details The _handleComErr interface returns when network recovers or the data sending thread exits
            LOG_WARN << "idsm qualified security event transmit: communication exception.";
            if (IdsmErrorCode::kIdsmThreadExitSign == _handleComErr()) {
                /// @details Return value -1: data sending thread exits, cancel event sending
                LOG_WARN << "idsm qualified security event transmit: transmit fail, thread has be exit";
                return IdsmErrorCode::kIdsmThreadExitSign;
            }
        }
    }
    return IdsmErrorCode::kIdsmSuccess;
}
/// @brief Initialize security event sender
/// @param ipAddr IP address of the IDSR service
/// @param port Port number of the IDSR service
/// @param async Whether to send asynchronously
/// @exception Stack overflow exception
void IdsmWriter::_initIdsmSender(ara::core::String const& ipAddr, uint16_t port, bool const async = false)
{
    if (!async) {
        sender_ = std::make_shared< SyncSender >(ipAddr, port);
        return;
    }
    SendCallBack const handler{[this](TransDataPtr const& data, bool const status) {
        std::ignore = data;
        if (status == true) {
            return;
        }
        Message errMsg;
        errMsg.transData = std::make_shared< TransData >(*_serialize(internalEvents_.communicationEvent));
        errMsg.userData  = nullptr;
        std::ignore      = this->sender_->Send(errMsg, true);
    }};
    std::shared_ptr< AsyncSender > asyncSender{std::make_shared< AsyncSender >(ipAddr, port)};
    if (port == 0U) {
    }
    asyncSender->RegisterCallback(handler);
    sender_ = asyncSender;
}

}  // namespace idsm
}  // namespace ara
