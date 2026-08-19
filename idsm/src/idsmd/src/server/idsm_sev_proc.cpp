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
/// @file       idsm_sev_proc.cpp
/// @brief      Process security events reported by client
/// @details
/// @date       2023-02-16
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/event receiver handler
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0001
/// @unit_name=EventProcer
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_sev_proc.h"

#include <iostream>

#include "ara/core/instance_specifier.h"
#include "config/idsm_struct_init.h"
#include "event/idsm_event_memory.h"
#include "event/idsm_event_proper.h"
#include "event/idsm_event_sink.h"
#ifdef ARA_WITH_TSYNC
    #include "ara/tsync/synchronized_time_base_consumer.h"
#else
    #include "ara/core/steady_clock.h"
#endif
namespace ara {
namespace idsm {
/// @brief Initialize
/// @return 0 on success, non-zero on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t EventProcer::Init()
{
    /// @brief Read configuration needed for event processing
    if (_procerConfig() != 0) {
        return -1;
    }
#ifdef ARA_WITH_IAM
    // Initialize IAM authentication client /
    grantIAMPtr_ = std::make_unique< ara::iam::internal::grant::IAMGrantIdsmQuery >();
    if (!grantIAMPtr_->Initialize()) {
        LOG_ERROR << "EventProcer::Init grantquery init error!";
        return -1;
    }
#endif
    return 0;
}
/// @brief Security event processing
/// @param eventMsg Dynamic attributes of security event reported by AA application
/// @return 0 on success, non-zero on failure
/// @exception Stack overflow exception
IdsmErrorCode EventProcer::Process(ClientEventMsg& eventMsg)
{
    if (portMap_.count(eventMsg.GetInstanceId()) == 0U) {
        LOG_ERROR << "Idsm not process event: " << eventMsg.GetInstanceId().c_str()
                  << " from AA because not found event id by port ";
        return IdsmErrorCode::kIdsmPortNotEventErr;
    }

    uint16_t const eventId{portMap_.at(eventMsg.GetInstanceId())};
    uint16_t const invalidEventId{0xFFFFU};
    uint16_t const customEventIdMin{0x8000U};
    if ((eventId < customEventIdMin) || (eventId >= invalidEventId)) {
        /// @details Event type ID check failed
        LOG_ERROR << "Idsm not process event: " << eventMsg.GetInstanceId().c_str()
                  << " from AA because invalid event id";
        return IdsmErrorCode::kIdsmInvalidEventId;
    }
    if (_iamCheck(eventMsg.GetProcessId(), eventId) == false) {
        /// @details IAM check failed
        return IdsmErrorCode::kIdsmIAMErr;
    }
    IdsmErrorCode const checkRes{_memoryCheck(EVENT_MEM_SIZE, eventMsg.GetContextDataSize())};
    if (checkRes != IdsmErrorCode::kIdsmSuccess) {
        /// @details Memory limit exceeded
        return checkRes;
    }

    /// @details Passed IAM and memory checks
    EventPtr const event{_assembleEvent(eventMsg, eventId)};
    if (event == nullptr) {
        return IdsmErrorCode::kIdsmEventNotProperErr;
    }

    /// @details Reporting mode handling
    if (ReportingModeProc(event) == false) {
        return IdsmErrorCode::kIdsmSuccess;
    }

    /// @details Filter chain filters security events
    if (filterMap_.count(eventId) == 0U) {
        LOG_ERROR << "Idsm not process event: " << eventId << " from AA because not found filter chain by event id";
        return IdsmErrorCode::kIdsmEventNotFilterChainErr;
    }
    if (filterMap_.at(eventId) == nullptr) {
        LOG_ERROR << "Idsm not process event: " << eventId << " from AA because event has more than one filters chain ";
        return IdsmErrorCode::kIdsmEventMultiFilterChainErr;
    }
    filterMap_.at(eventId)->Filter(event);
    return IdsmErrorCode::kIdsmSuccess;
}
/// @brief Timer processing
/// @exception Stack overflow exception
void EventProcer::Timerhandler()
{
    for (auto& ele : filterMap_) {
        ele.second->Timerhandler();
    }
}
/// @brief Get configuration needed for event processing
/// @return 0 on success, non-zero on failure
/// @exception Stack overflow exception
int32_t EventProcer::_procerConfig()
{
    filterMap_ = IdsmStruct::GetInstance()->GetFilterMap();
    /// @brief Is this structure necessary
    portMap_ = IdsmStruct::GetInstance()->GetPortMap();

    eventMemTotal_   = IdsmStruct::GetInstance()->GetEventMemLimit();
    contextMemTotal_ = IdsmStruct::GetInstance()->GetContextMemLimit();
    internalSev_     = IdsmStruct::GetInstance()->GetInternalEvent();
    timestampFormat_ = IdsmStruct::GetInstance()->GetTimeStampFormat();
    timeBaseFQN_     = IdsmStruct::GetInstance()->GetTimeBase();
    return 0;
}
/// @brief Process security event reporting mode
/// @param event Security event
/// @return true on success, false on processing error
/// @exception Stack overflow exception
bool EventProcer::ReportingModeProc(EventPtr const& event)
{
    ReportingMode const mode{EventProperPool::GetInstance()->GetEventMode(event->GetEventId())};
    if (mode == ReportingMode::kOff) {
        LOG_WARN << "idsm drop event. event id:" << event->GetEventId() << "reason: event report mode is off";
        return false;
    }
    if (mode == ReportingMode::kBrief) {
        ContextDataType ctxData;
        event->SetContextData(ctxData);
    }

    if (mode == ReportingMode::kBriefBypassingFilters) {
        if (event->GetContextDataSize() == 0U) {
            /// @details Count memory used by security event
            uint64_t const ctxSize{event->GetContextDataSize()};
            EventMemPool::GetInstance()->AllocMem(EVENT_MEM_SIZE, ctxSize);
            EventPool::GetInstance()->Push(event);
            return false;
        }
    }
    if (mode == ReportingMode::kDetailedBypassingFilters) {
        if (event->GetContextDataSize() != 0U) {
            /// @details Count memory used by security event
            uint64_t const ctxSize{event->GetContextDataSize()};
            EventMemPool::GetInstance()->AllocMem(EVENT_MEM_SIZE, ctxSize);
            EventPool::GetInstance()->Push(event);
            return false;
        }
    }
    return true;
}
/// @brief IAM check
/// @param procId Process ID
/// @param eventId Security event ID
/// @return 0 on check success, non-zero on check failure
bool EventProcer::_iamCheck(uint32_t const procId, uint16_t const eventId) const noexcept
{
#ifdef ARA_WITH_IAM
    if (grantIAMPtr_->HasIdsmGrant(procId, eventId) == false) {
        LOG_WARN << "Idsm not process event: " << eventId << "from AA because iam check fail.";
        return false;
    }
#else
    std::ignore = procId;
    std::ignore = eventId;
#endif
    return true;
}
/// @brief Memory check
/// @param frameSize Stack frame memory
/// @param contextSize Context memory
/// @return 0 on check success, non-zero on check failure
/// @exception Stack overflow exception
IdsmErrorCode EventProcer::_memoryCheck(uint32_t const frameSize, uint64_t const contextSize) const
{
    uint64_t eventMemUsed{0U};
    uint64_t contextMemUsed{0U};
    EventMemPool::GetInstance()->GetUsedMem(eventMemUsed, contextMemUsed);

    if (eventMemUsed + frameSize > eventMemTotal_) {
        LOG_ERROR << "idsm not enough memory to buffer incoming security event. used event mem:" << eventMemUsed
                  << "total event mem:" << eventMemTotal_;
        EventPool::GetInstance()->Push(internalSev_.eventbufEvent);
        return IdsmErrorCode::kIdsmEventMemErr;
    }
    if (contextMemUsed + contextSize > contextMemTotal_) {
        LOG_ERROR << "idsm not enough memory to buffer incoming security event context data.";
        EventPool::GetInstance()->Push(internalSev_.contextBufEvent);
        return IdsmErrorCode::kIdsmContextMemErr;
    }
    return IdsmErrorCode::kIdsmSuccess;
}
/// @brief Adjust security event timestamp
/// @param eventMsg
/// @exception Stack overflow exception
void EventProcer::_adjustTimestamp(ClientEventMsg& eventMsg) const
{
    if (timestampFormat_.empty()) {
        /// @brief Timestamp not used
        eventMsg.SetTimeStamp(0U);
        return;
    }
    /// @brief Scenario where AA application did not provide timestamp when reporting
    if (eventMsg.GetTimeStampSource() != TimestampSource::kAppReport) {
        /// @brief When timestamp format is AUTOSAR: since callback timestamp may have been obtained, reset timestamp and timestamp source.
        /// @brief Get timestamp from time synchronization module
        if (timestampFormat_ == "AUTOSAR") {
            eventMsg.SetTimeStampSource(TimestampSource::kAraTsync);
            eventMsg.SetTimeStamp(0U);

#ifdef ARA_WITH_TSYNC
            /// @brief Check whether the user has configured the time base of the time synchronization module. If no time base is configured, refuse to process the security event
            if (timeBaseFQN_.empty()) {
                LOG_ERROR << "Idsm not process event from AA: " << eventMsg.GetInstanceId().c_str()
                          << ". because timestamp format is set to AUTOSAR, but user not configure timebase.";
                return;
            }
            ara::core::InstanceSpecifier timeBaseSpecifier{ara::core::StringView{timeBaseFQN_.c_str()}};
            ara::tsync::SynchronizedTimeBaseConsumer const timeBaseConsumer{timeBaseSpecifier};
            ara::tsync::Timestamp const tempTimestamp{timeBaseConsumer.GetCurrentTime()};
            std::chrono::nanoseconds const nanSecond{
                std::chrono::duration_cast< std::chrono::nanoseconds >(tempTimestamp.time_since_epoch())};
            eventMsg.SetTimeStamp(static_cast< TimestampType >(nanSecond.count()));
#else
            ara::core::SteadyClock::time_point const timePoint{ara::core::SteadyClock::now()};
            ara::core::SteadyClock::duration const drNanSeconds{timePoint.time_since_epoch()};
            time_t const timesSec{(std::chrono::duration_cast< std::chrono::seconds >(drNanSeconds)).count()};
            eventMsg.SetTimeStamp(static_cast< TimestampType >(timesSec));
#endif
        }
        /// When timestamp format is non-AUTOSAR: get timestamp via callback function, already handled on client side
        return;
    }
    /// @details: User reports security event with its own timestamp, keep lower 62 bits
    uint64_t const timeStampValidBits{0x3FFFFFFFFFFFFFFFU};
    eventMsg.SetTimeStamp(eventMsg.GetTimeStamp() & timeStampValidBits);
}
/// @brief Combine static and dynamic attributes of security event into a complete security event
/// @param eventMsg
/// @param eventId
/// @return Complete security event. If failure, the security event is empty
/// @exception Stack overflow exception
EventPtr EventProcer::_assembleEvent(ClientEventMsg& eventMsg, uint16_t const eventId) const
{
    EventProperPtr proper{EventProperPool::GetInstance()->GetEventProper(eventId)};
    if (proper == nullptr) {
        /// @details Event type attributes not found
        LOG_ERROR << "Idsm not process event: " << eventId << " from AA because not found event properity";
        return {nullptr};
    }
    _adjustTimestamp(eventMsg);
    return std::make_shared< Event >(*proper, eventMsg);
}

}  // namespace idsm
}  // namespace ara