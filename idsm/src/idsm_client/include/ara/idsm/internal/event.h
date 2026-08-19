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
/// @file       event.h
/// @brief      Security event description class
/// @details
/// @date       2023-02-15
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/filter chain
/// @interface_level=module
/// @trace_id_sr=SRS_IDSM_0006,SRS_IDSM_0011
/// @unit_name=
/// @unit_description=Security event description class
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_EVENT_H_
#define ARA_IDSM_EVENT_H_

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "ara/core/string.h"
#include "ara/core/vector.h"
#include "ara/idsm/common.h"
#include "ara/idsm/internal/message_process.h"

/// @brief Security event frame length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00063
/// @trace_id_dd=DD_IDSM_00115
/// @needwork = ad
/// @endcode
#define EVENT_MEM_SIZE 16U

namespace ara {
namespace idsm {
/// @brief Enumeration of event reporting modes
/// @code{.isoft}
/// @unit_name=ReportingMode
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00064
/// @trace_id_dd=DD_IDSM_00116
/// @needwork = ad
/// @endcode
enum class ReportingMode : uint8_t
{
    /// @brief Discard security event
    kOff = 0,
    /// @brief Discard context data of security event
    kBrief,
    /// @brief Keep context data of security event unchanged
    kDetailed,
    /// @brief Security event without context data bypasses filter chain check
    kBriefBypassingFilters,
    /// @brief Security event with context data bypasses filter chain check
    kDetailedBypassingFilters,
    /// @brief Unknown mode
    kUnknown
};

/// @brief Attributes describing event type: attributes common to different events of the same type
/// @code{.isoft}
/// @unit_name=EventProper
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00065
/// @trace_id_dd=DD_IDSM_00117
/// @needwork = ad
/// @endcode
class EventProper
{
public:
    /// @brief Parameterless constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00066
    /// @trace_id_dd=DD_IDSM_00118
    /// @needwork = ad
    /// @endcode
    EventProper() noexcept : EventProper{1U, ReportingMode::kDetailed, false, 0U} {}
    /// @brief Parameterized constructor
    /// @param event Security event ID
    /// @param persistent Persistence switch
    /// @param sensor Sensor ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00067
    /// @trace_id_dd=DD_IDSM_00119
    /// @needwork = ad
    /// @endcode
    EventProper(uint16_t const event, bool const persistent, uint8_t const sensor) noexcept
        : EventProper{event, ReportingMode::kDetailed, persistent, sensor}
    {
    }
    /// @brief Destructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00120
    /// @needwork = dda
    /// @endcode
    virtual ~EventProper() = default;

public:
    /// @brief Get security event ID
    /// @return Security event ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00068
    /// @trace_id_dd=DD_IDSM_00121
    /// @needwork = ad
    /// @endcode
    uint16_t GetEventId() const noexcept { return eventId_; }
    /// @brief Get sensor ID
    /// @return Sensor ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00069
    /// @trace_id_dd=DD_IDSM_00122
    /// @needwork = ad
    /// @endcode
    uint8_t GetSensorId() const noexcept { return sensorId_; }
    /// @brief Get reporting mode
    /// @return Reporting mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00070
    /// @trace_id_dd=DD_IDSM_00123
    /// @needwork = ad
    /// @endcode
    ReportingMode GetReportMode() const noexcept { return mode_; }
    /// @brief Get persistence storage switch
    /// @return Persistence storage switch
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00071
    /// @trace_id_dd=DD_IDSM_00124
    /// @needwork = ad
    /// @endcode
    bool GetPerStorage() const noexcept { return perStorage_; }
    /// @brief Set security event ID
    /// @param id Security event ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00072
    /// @trace_id_dd=DD_IDSM_00125
    /// @needwork = ad
    /// @endcode
    void SetEventId(uint16_t const id) noexcept { eventId_ = id; }
    /// @brief Set sensor ID
    /// @param id Sensor ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00073
    /// @trace_id_dd=DD_IDSM_00126
    /// @needwork = ad
    /// @endcode
    void SetSensorId(uint8_t const id) noexcept { sensorId_ = id; }
    /// @brief Set persistence storage switch
    /// @param storage Persistence storage switch
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00074
    /// @trace_id_dd=DD_IDSM_00127
    /// @needwork = ad
    /// @endcode
    void SetPerStorage(bool const storage) noexcept { perStorage_ = storage; }
    /// @brief Set reporting mode
    /// @param m Reporting mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00075
    /// @trace_id_dd=DD_IDSM_00128
    /// @needwork = ad
    /// @endcode
    void SetReportMode(ReportingMode const m) noexcept { mode_ = m; }

protected:
    /// @brief Copy constructor
    /// @param proper Object to be copied
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00129
    /// @needwork = dda
    /// @endcode
    EventProper(EventProper const& proper) noexcept = default;
    /// @brief Move copy constructor
    /// @param proper Object to be moved
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00130
    /// @needwork = dda
    /// @endcode
    EventProper(EventProper&& proper) noexcept = default;
    /// @brief Copy assignment operator
    /// @param proper Object to be copied
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00131
    /// @needwork = dda
    /// @endcode
    EventProper& operator=(EventProper const& proper) noexcept = default;
    /// @brief Move assignment operator
    /// @param proper Object to be moved
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00132
    /// @needwork = dda
    /// @endcode
    EventProper& operator=(EventProper&& proper) noexcept = default;

private:
    /// @brief Parameterized constructor
    /// @param event Event ID
    /// @param m Reporting mode
    /// @param persistent Whether to persist storage
    /// @param sensor Sensor ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00133
    /// @needwork = dda
    /// @endcode
    EventProper(uint16_t const event, ReportingMode const m, bool const persistent, uint8_t const sensor) noexcept
        : eventId_{event}, mode_{m}, perStorage_{persistent}, sensorId_{sensor}
    {
    }

private:
    /// @brief Event type ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00134
    /// @needwork = dda
    /// @endcode
    uint16_t eventId_;
    /// @brief Event reporting mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00135
    /// @needwork = dda
    /// @endcode
    ReportingMode mode_;
    /// @brief Whether the event is persistent
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00136
    /// @needwork = dda
    /// @endcode
    bool perStorage_;
    /// @brief Sensor ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00137
    /// @needwork = dda
    /// @endcode
    uint8_t sensorId_;
};
/// @brief Type redefinition: define the smart pointer type corresponding to the data structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00076
/// @trace_id_dd=DD_IDSM_00138
/// @needwork = ad
/// @endcode
using EventProperPtr = std::shared_ptr< EventProper >;

/// @brief Attributes describing a specific event. The Event object runs through the entire event processing, with some attributes added or updated during processing.
/// @code{.isoft}
/// @unit_name=Event
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00077
/// @trace_id_dd=DD_IDSM_00139
/// @needwork = ad
/// @endcode
class Event final : public EventProper
{
public:
    /// @brief Parameterless constructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00140
    /// @needwork = dda
    /// @endcode
    Event() noexcept = default;
    /// @brief Parameterized constructor
    /// @param property Static attributes of security event
    /// @param eventMsg Dynamic attributes of security event
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00078
    /// @trace_id_dd=DD_IDSM_00141
    /// @needwork = ad
    /// @endcode
    Event(EventProper const& property, ClientEventMsg& eventMsg) noexcept : EventProper{property}
    {
        counter_    = eventMsg.GetCounter();
        timeSource_ = eventMsg.GetTimeStampSource();
        timestamp_  = eventMsg.GetTimeStamp();
        eventMsg.GetContextData(contextData_);
    }
    /// @brief Copy constructor
    /// @param event Object to be copied
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00142
    /// @needwork = dda
    /// @endcode
    Event(Event const& event) noexcept = default;
    /// @brief Move constructor
    /// @param event Object to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00143
    /// @needwork = dda
    /// @endcode
    Event(Event&& event) noexcept = default;
    /// @brief Copy assignment operator
    /// @param event Object to be copied for assignment operator
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00144
    /// @needwork = dda
    /// @endcode
    Event& operator=(Event const& event) noexcept = default;
    /// @brief Move assignment operator
    /// @param event Object to be moved for assignment operator
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00145
    /// @needwork = dda
    /// @endcode
    Event& operator=(Event&& event) noexcept = default;
    /// @brief Destructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00146
    /// @needwork = dda
    /// @endcode
    ~Event() final = default;

public:
    /// @brief Set signature
    /// @param sign Signature
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00079
    /// @trace_id_dd=DD_IDSM_00147
    /// @needwork = ad
    /// @endcode
    void SetSignature(BytesVec& sign) noexcept { signature_.swap(sign); }
    /// @brief Get signature
    /// @param sign Signature
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00080
    /// @trace_id_dd=DD_IDSM_00148
    /// @needwork = ad
    /// @endcode
    void GetSignature(BytesVec& sign) noexcept
    {
        sign.clear();
        sign.swap(signature_);
    }
    /// @brief Set context data
    /// @param data Context data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00081
    /// @trace_id_dd=DD_IDSM_00149
    /// @needwork = ad
    /// @endcode
    void SetContextData(ContextDataType& data) noexcept { contextData_.swap(data); }
    /// @brief Get context data
    /// @param data Context data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00082
    /// @trace_id_dd=DD_IDSM_00150
    /// @needwork = ad
    /// @endcode
    void GetContextData(ContextDataType& data) noexcept
    {
        data.clear();
        data.swap(contextData_);
    }
    /// @brief Get context data size
    /// @return Context data size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00083
    /// @trace_id_dd=DD_IDSM_00151
    /// @needwork = ad
    /// @endcode
    size_t GetContextDataSize() const noexcept { return contextData_.size(); }
    /// @brief Get timestamp source
    /// @return Timestamp source
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00084
    /// @trace_id_dd=DD_IDSM_00152
    /// @needwork = ad
    /// @endcode
    TimestampSource GetTimeSource() const noexcept { return timeSource_; }
    /// @brief Set timestamp source
    /// @param timeSourceOther Timestamp source
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00085
    /// @trace_id_dd=DD_IDSM_00153
    /// @needwork = ad
    /// @endcode
    void SetTimeSource(TimestampSource const timeSourceOther) noexcept { timeSource_ = timeSourceOther; }
    /// @brief Get timestamp
    /// @return Timestamp
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00086
    /// @trace_id_dd=DD_IDSM_00154
    /// @needwork = ad
    /// @endcode
    uint64_t GetTimeStamp() const noexcept { return timestamp_; }
    /// @brief Set timestamp
    /// @param timestampOther Timestamp
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00087
    /// @trace_id_dd=DD_IDSM_00155
    /// @needwork = ad
    /// @endcode
    void SetTimeStamp(uint64_t const timestampOther) noexcept { timestamp_ = timestampOther; }
    /// @brief Get event counter
    /// @return Event counter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00088
    /// @trace_id_dd=DD_IDSM_00156
    /// @needwork = ad
    /// @endcode
    uint16_t GetCounter() const noexcept { return counter_; }
    /// @brief Set event counter
    /// @param countOther Event counter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00089
    /// @trace_id_dd=DD_IDSM_00157
    /// @needwork = ad
    /// @endcode
    void SetCounter(uint16_t const countOther) noexcept { counter_ = countOther; }
    /// @brief Get security event status
    /// @return Get status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00090
    /// @trace_id_dd=DD_IDSM_00158
    /// @needwork = ad
    /// @endcode
    uint16_t GetStatus() const noexcept { return status_; }

private:
    /// @brief Event signature, added when event is serialized
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00159
    /// @needwork = dda
    /// @endcode
    BytesVec signature_{};
    /// @brief Event context data, reported by AA side application
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00160
    /// @needwork = dda
    /// @endcode
    ContextDataType contextData_{};
    /// @brief Source of event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00161
    /// @needwork = dda
    /// @endcode
    TimestampSource timeSource_{TimestampSource::kAraTsync};
    /// @brief Event timestamp
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00162
    /// @needwork = dda
    /// @endcode
    uint64_t timestamp_{0U};
    /// @brief Event counter, reported by AA-side application or accumulated in aggregation filter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00163
    /// @needwork = dda
    /// @endcode
    uint16_t counter_{0U};
    /// @brief Event status, jointly determined by AA-side application and state manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00164
    /// @needwork = dda
    /// @endcode
    uint16_t status_{0U};
};
/// @brief Type redefinition: define the smart pointer type corresponding to the data structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00091
/// @trace_id_dd=DD_IDSM_00165
/// @needwork = ad
/// @endcode
using EventPtr = std::shared_ptr< Event >;

/// @brief IDSM internal security event
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00092
/// @trace_id_dd=DD_IDSM_00166
/// @needwork = ad
/// @endcode
struct IdsmInternalEvent_S
{
    /// @brief Rate limiting and traffic shaping event
    EventPtr trafficEvent;
    /// @brief Communication exception event
    EventPtr communicationEvent;
    /// @brief Security event frame size exceeded event
    EventPtr eventbufEvent;
    /// @brief Context data size exceeded event
    EventPtr contextBufEvent;
};
/// @brief Type redefinition: define the smart pointer type corresponding to the data structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00093
/// @trace_id_dd=DD_IDSM_00167
/// @needwork = ad
/// @endcode
using IdsmInternalEvent = struct IdsmInternalEvent_S;

}  // namespace idsm
}  // namespace ara

#endif
