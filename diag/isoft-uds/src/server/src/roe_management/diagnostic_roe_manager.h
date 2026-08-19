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
/// @file       diagnostic_roe_manager.h
/// @brief      This file provides diagnostic_roe_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/roe_management
/// @interface_level=software
/// @trace_id_sr=5c23e90c-715a-4d0d-8416-a4ebb6cf8eea
/// @unit_name=DiagnosticRoeManager
/// @unit_description=This file provides diagnostic_roe_manager functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_DiagnosticRoeManager_H_
#define ISOFT_UDS_SERVER_DiagnosticRoeManager_H_

#include <memory>

#include "common/persistence_service.h"
#include "common/request_context.h"
#include "data_management/diagnostic_data_manager.h"
#include "fault_management/dtc_filter.h"
#include "isoft/uds/roe_management/roe_management.h"
#include "session_management/session_manager.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief data class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02428
/// @needwork = dd
/// @endcode
class Data
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00660
    /// @needwork = dda
    /// @endcode
    Data(Data const&) = delete;

    /// @brief Assignment operator
    /// @param[in] other
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01229
    /// @needwork = dd
    /// @endcode
    Data& operator=(Data const&) = default;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00661
    /// @needwork = dda
    /// @endcode
    Data(Data&&) = delete;

    /// @brief Move assignment operator
    /// @param[in] other
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01230
    /// @needwork = dd
    /// @endcode
    Data& operator=(Data&&) = default;

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01231
    /// @needwork = dd
    /// @endcode
    Data() noexcept = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01232
    /// @needwork = dd
    /// @endcode
    virtual ~Data() noexcept = default;
};

/// @brief data pointer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02429
/// @needwork = dd
/// @endcode
using DataPtr = std::shared_ptr< Data >;

/// @brief event type enum
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02430
/// @needwork = dd
/// @endcode
enum class EventType : std::uint8_t
{
    kUnknow,
    kDTCStatusChange,
    kChangeDataIdentifier,
    kCompareDataValues,
};

/// @brief event setup class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02431
/// @needwork = dd
/// @endcode
class EventSetup
{
public:
    /// @brief Whether to store event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01233
    /// @needwork = dd
    /// @endcode
    bool store{false};
    /// @brief Event type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01234
    /// @needwork = dd
    /// @endcode
    std::uint8_t eventType{0U};
    /// @brief Event window time
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01235
    /// @needwork = dd
    /// @endcode
    std::uint8_t eventWindowTime{0U};
    /// @brief Event type record
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01236
    /// @needwork = dd
    /// @endcode
    std::vector< std::uint8_t > eventTypeRecord{};
    /// @brief Service response record
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01237
    /// @needwork = dd
    /// @endcode
    std::vector< std::uint8_t > serviceToRespondToRecord{};
};
/// @brief response on event class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02432
/// @needwork = dd
/// @endcode
class ResponseOnEvent
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00662
    /// @needwork = dda
    /// @endcode
    ResponseOnEvent(ResponseOnEvent const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00663
    /// @needwork = dda
    /// @endcode
    ResponseOnEvent& operator=(ResponseOnEvent const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00664
    /// @needwork = dda
    /// @endcode
    ResponseOnEvent(ResponseOnEvent&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00665
    /// @needwork = dda
    /// @endcode
    ResponseOnEvent& operator=(ResponseOnEvent&&) = delete;

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01238
    /// @needwork = dd
    /// @endcode
    ResponseOnEvent() noexcept = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01239
    /// @needwork = dd
    /// @endcode
    virtual ~ResponseOnEvent() noexcept = default;
    /// @brief Initialization function
    /// @param[in] setup
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01240
    /// @needwork = dd
    /// @endcode
    virtual std::int32_t Initialize(EventSetup& setup) noexcept = 0;
    /// @brief Active function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01241
    /// @needwork = dd
    /// @endcode
    virtual std::int32_t Active(MetaInfoMap& metaInfo) noexcept = 0;
    /// @brief SampleData function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01242
    /// @needwork = dd
    /// @endcode
    virtual std::int32_t SampleData(std::shared_ptr< Data >& dataPtr) noexcept = 0;
    /// @brief CompareData function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01243
    /// @needwork = dd
    /// @endcode
    virtual std::int32_t CompareData(std::shared_ptr< Data >& dataPtr) noexcept = 0;
    /// @brief Occurred function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01244
    /// @needwork = dd
    /// @endcode
    virtual std::int32_t Occurred() noexcept = 0;
    /// @brief GetEventSetup function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01245
    /// @needwork = dd
    /// @endcode
    virtual EventSetup& GetEventSetup() noexcept = 0;
};

/// @brief event pointer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02433
/// @needwork = dd
/// @endcode
using EventPtr = std::shared_ptr< ResponseOnEvent >;
/// @brief event table class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02434
/// @needwork = dd
/// @endcode
class EventTable
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00666
    /// @needwork = dda
    /// @endcode
    EventTable(EventTable const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00667
    /// @needwork = dda
    /// @endcode
    EventTable& operator=(EventTable const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00668
    /// @needwork = dda
    /// @endcode
    EventTable(EventTable&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00669
    /// @needwork = dda
    /// @endcode
    EventTable& operator=(EventTable&&) = delete;

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01246
    /// @needwork = dd
    /// @endcode
    EventTable() noexcept = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01247
    /// @needwork = dd
    /// @endcode
    ~EventTable() noexcept = default;

    /// @brief Add
    /// @param const Parameter eventPtr
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01248
    /// @needwork = dd
    /// @endcode
    void Add(EventPtr const& eventPtr) noexcept;
    /// @brief Get event table
    /// @return Event table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01249
    /// @needwork = dd
    /// @endcode
    std::vector< EventPtr >& Get() noexcept;
    /// @brief Get time
    /// @return Unsigned integer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01250
    /// @needwork = dd
    /// @endcode
    std::uint8_t GetEventWindowTime() noexcept;
    /// @brief Get event type record
    /// @return Event type record
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01251
    /// @needwork = dd
    /// @endcode
    std::vector< std::uint8_t > GetEventTypeRecord() noexcept;
    /// @brief Get service response record
    /// @return Service response record
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01252
    /// @needwork = dd
    /// @endcode
    std::vector< std::uint8_t > GetServiceToRespondToRecord() noexcept;

    /// @brief Clear function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01253
    /// @needwork = dd
    /// @endcode
    void Clear() noexcept;

private:
    /// @brief Event table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01254
    /// @needwork = dd
    /// @endcode
    std::vector< EventPtr > table_{};
};
/// @brief event processor class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02435
/// @needwork = dd
/// @endcode
class EventProcessor
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00670
    /// @needwork = dda
    /// @endcode
    EventProcessor(EventProcessor const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00671
    /// @needwork = dda
    /// @endcode
    EventProcessor& operator=(EventProcessor const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00672
    /// @needwork = dda
    /// @endcode
    EventProcessor(EventProcessor&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00673
    /// @needwork = dda
    /// @endcode
    EventProcessor& operator=(EventProcessor&&) = delete;

public:
    /// @brief EventProcessor function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01255
    /// @needwork = dd
    /// @endcode
    explicit EventProcessor(EventTable& eventTable) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01256
    /// @needwork = dd
    /// @endcode
    virtual ~EventProcessor() noexcept = default;

    /// @brief Process function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01257
    /// @needwork = dd
    /// @endcode
    virtual void Process() noexcept;

private:
    /// @brief Event table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01258
    /// @needwork = dd
    /// @endcode
    EventTable& eventTable_;
};

/// @brief count by dtc status class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02436
/// @needwork = dd
/// @endcode
class CountByDTCStatus : public Data
{
private:
    /// @brief Count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01259
    /// @needwork = dd
    /// @endcode
    std::uint16_t count_{0U};

public:
    /// @brief Get count
    /// @return Count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01260
    /// @needwork = dd
    /// @endcode
    std::uint16_t& Value() noexcept;
};

/// @brief dtc status change class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02437
/// @needwork = dd
/// @endcode
class DTCStatusChange : public ResponseOnEvent
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00674
    /// @needwork = dda
    /// @endcode
    DTCStatusChange(DTCStatusChange const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00675
    /// @needwork = dda
    /// @endcode
    DTCStatusChange& operator=(DTCStatusChange const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00676
    /// @needwork = dda
    /// @endcode
    DTCStatusChange(DTCStatusChange&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00677
    /// @needwork = dda
    /// @endcode
    DTCStatusChange& operator=(DTCStatusChange&&) = delete;

public:
    /// @brief DTCStatusChange function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01261
    /// @needwork = dd
    /// @endcode
    explicit DTCStatusChange(std::shared_ptr< DtcFilter > dtcFilterPtr) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01262
    /// @needwork = dd
    /// @endcode
    ~DTCStatusChange() noexcept override = default;
    /// @brief Initialization function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01263
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(EventSetup& setup) noexcept override;

    /// @brief Active function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01264
    /// @needwork = dd
    /// @endcode
    std::int32_t Active(MetaInfoMap& metaInfo) noexcept override;
    /// @brief Set RequestCallback
    /// @param[in] callback Callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01265
    /// @needwork = dd
    /// @endcode
    void SetRequestCallback(std::function< void(std::vector< std::uint8_t >) > callback) noexcept;
    /// @brief SampleData function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01266
    /// @needwork = dd
    /// @endcode
    std::int32_t SampleData(std::shared_ptr< Data >& dataPtr) noexcept override;
    /// @brief CompareData function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01267
    /// @needwork = dd
    /// @endcode
    std::int32_t CompareData(std::shared_ptr< Data >& dataPtr) noexcept override;
    /// @brief Occurred function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01268
    /// @needwork = dd
    /// @endcode
    std::int32_t Occurred() noexcept override;
    /// @brief GetEventSetup function
    /// @return Event setup
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01269
    /// @needwork = dd
    /// @endcode
    EventSetup& GetEventSetup() noexcept override;

private:
    /// @brief dtc filter pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01270
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DtcFilter > dtcFilterPtr_{nullptr};
    /// @brief dtc status mask
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01271
    /// @needwork = dd
    /// @endcode
    std::uint8_t dtcStatusMask_{0U};
    /// @brief Callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01272
    /// @needwork = dd
    /// @endcode
    std::function< void(std::vector< std::uint8_t >) > callback_{nullptr};
    /// @brief First trigger
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01273
    /// @needwork = dd
    /// @endcode
    bool first_{false};
    /// @brief Count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01274
    /// @needwork = dd
    /// @endcode
    CountByDTCStatus countByDTCStatus_{};
    /// @brief Event setup
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01275
    /// @needwork = dd
    /// @endcode
    EventSetup setup_{};
};

/// @brief did data class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02438
/// @needwork = dd
/// @endcode
class DidData : public Data
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00678
    /// @needwork = dda
    /// @endcode
    DidData(DidData const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00679
    /// @needwork = dda
    /// @endcode
    DidData& operator=(DidData const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00680
    /// @needwork = dda
    /// @endcode
    DidData(DidData&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00681
    /// @needwork = dda
    /// @endcode
    DidData& operator=(DidData&&) = delete;

private:
    /// @brief did data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01276
    /// @needwork = dd
    /// @endcode
    DiagnosticData data_{};

public:
    /// @brief DidData function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01277
    /// @needwork = dd
    /// @endcode
    explicit DidData(DiagnosticData& data) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01278
    /// @needwork = dd
    /// @endcode
    ~DidData() noexcept override = default;
    /// @brief Get did data
    /// @return did data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01279
    /// @needwork = dd
    /// @endcode
    DiagnosticData& Get() noexcept;
};

/// @brief did data pointer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02439
/// @needwork = dd
/// @endcode
using DidDataPtr = std::shared_ptr< DidData >;

/// @brief change of data identifier class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02440
/// @needwork = dd
/// @endcode
class ChangeOfDataIdentifier : public ResponseOnEvent
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00682
    /// @needwork = dda
    /// @endcode
    ChangeOfDataIdentifier(ChangeOfDataIdentifier const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00683
    /// @needwork = dda
    /// @endcode
    ChangeOfDataIdentifier& operator=(ChangeOfDataIdentifier const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00684
    /// @needwork = dda
    /// @endcode
    ChangeOfDataIdentifier(ChangeOfDataIdentifier&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00685
    /// @needwork = dda
    /// @endcode
    ChangeOfDataIdentifier& operator=(ChangeOfDataIdentifier&&) = delete;

public:
    /// @brief ChangeOfDataIdentifier function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01280
    /// @needwork = dd
    /// @endcode
    explicit ChangeOfDataIdentifier(std::shared_ptr< DiagnosticDataManager > diagnosticDataManagerPtr) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01281
    /// @needwork = dd
    /// @endcode
    ~ChangeOfDataIdentifier() noexcept override = default;
    /// @brief Initialization function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01282
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(EventSetup& setup) noexcept override;

    /// @brief Active function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01283
    /// @needwork = dd
    /// @endcode
    std::int32_t Active(MetaInfoMap& metaInfo) noexcept override;
    /// @brief Set RequestCallback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01284
    /// @needwork = dd
    /// @endcode
    void SetRequestCallback(std::function< void(std::vector< std::uint8_t >) > callback) noexcept;
    /// @brief SampleData function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01285
    /// @needwork = dd
    /// @endcode
    std::int32_t SampleData(std::shared_ptr< Data >& dataPtr) noexcept override;
    /// @brief CompareData function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01286
    /// @needwork = dd
    /// @endcode
    std::int32_t CompareData(std::shared_ptr< Data >& dataPtr) noexcept override;
    /// @brief Occurred function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01287
    /// @needwork = dd
    /// @endcode
    std::int32_t Occurred() noexcept override;
    /// @brief GetEventSetup
    /// @return event setup
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01288
    /// @needwork = dd
    /// @endcode
    EventSetup& GetEventSetup() noexcept override;

private:
    /// @brief diagnostic data manager pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01289
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DiagnosticDataManager > diagnosticDataManagerPtr_{nullptr};

    /// @brief request callback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01290
    /// @needwork = dd
    /// @endcode
    std::function< void(std::vector< std::uint8_t >) > callback_{nullptr};
    /// @brief did data pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01291
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DidData > dataPtr_{nullptr};
    /// @brief did
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01292
    /// @needwork = dd
    /// @endcode
    std::uint16_t did_{0U};
    /// @brief meta info
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01293
    /// @needwork = dd
    /// @endcode
    MetaInfoMap metaInfo_{};
    /// @brief event setup
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01294
    /// @needwork = dd
    /// @endcode
    EventSetup setup_{};
};

/// @brief comparison of value class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02441
/// @needwork = dd
/// @endcode
class ComparisonOfValue : public ResponseOnEvent
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00686
    /// @needwork = dda
    /// @endcode
    ComparisonOfValue(ComparisonOfValue const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00687
    /// @needwork = dda
    /// @endcode
    ComparisonOfValue& operator=(ComparisonOfValue const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00688
    /// @needwork = dda
    /// @endcode
    ComparisonOfValue(ComparisonOfValue&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00689
    /// @needwork = dda
    /// @endcode
    ComparisonOfValue& operator=(ComparisonOfValue&&) = delete;

public:
    /// @brief ComparisonOfValue function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01295
    /// @needwork = dd
    /// @endcode
    explicit ComparisonOfValue(std::shared_ptr< DiagnosticDataManager > diagnosticDataManagerPtr) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01296
    /// @needwork = dd
    /// @endcode
    ~ComparisonOfValue() noexcept override = default;
    /// @brief Initialization function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01297
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(EventSetup& setup) noexcept override;

    /// @brief Active function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01298
    /// @needwork = dd
    /// @endcode
    std::int32_t Active(MetaInfoMap& metaInfo) noexcept override;
    /// @brief Set RequestCallback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01299
    /// @needwork = dd
    /// @endcode
    void SetRequestCallback(std::function< void(std::vector< std::uint8_t >) > callback) noexcept;
    /// @brief SampleData function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01300
    /// @needwork = dd
    /// @endcode
    std::int32_t SampleData(std::shared_ptr< Data >& dataPtr) noexcept override;
    /// @brief CompareData function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01301
    /// @needwork = dd
    /// @endcode
    std::int32_t CompareData(std::shared_ptr< Data >& dataPtr) noexcept override;
    /// @brief Occurred function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01302
    /// @needwork = dd
    /// @endcode
    std::int32_t Occurred() noexcept override;
    /// @brief GetEventSetup
    /// @return event setup
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01303
    /// @needwork = dd
    /// @endcode
    EventSetup& GetEventSetup() noexcept override;

private:
    /// @brief record
    /// @param[in] did did
    /// @param[in] comparisonLogic comparison logic
    /// @param[in] comparisonValue comparison value
    /// @param[in] hysteresisValue hysteresis value
    /// @param[in] sign sign
    /// @param[in] length length
    /// @param[in] offset offset
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01304
    /// @needwork = dd
    /// @endcode
    struct Record
    {
        bool storeStatus{false};
        std::uint16_t did{0U};
        std::uint8_t comparisonLogic{0U};
        std::uint32_t comparisonValue{0U};
        std::uint8_t hysteresisValue{0U};
        bool sign{false};
        std::uint8_t length{0U};
        std::uint16_t offset{0U};
    };

    /// @brief diagnostic data manager pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01305
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DiagnosticDataManager > diagnosticDataManagerPtr_{nullptr};
    /// @brief request callback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01306
    /// @needwork = dd
    /// @endcode
    std::function< void(std::vector< std::uint8_t >) > callback_{nullptr};
    /// @brief record
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01307
    /// @needwork = dd
    /// @endcode
    Record record_{};
    /// @brief event setup
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01308
    /// @needwork = dd
    /// @endcode
    EventSetup setup_{};
    /// @brief meta info map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01309
    /// @needwork = dd
    /// @endcode
    MetaInfoMap metaInfo_{};
};

/// @brief response on event service class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02442
/// @needwork = dd
/// @endcode
class ResponseOnEventService
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00690
    /// @needwork = dda
    /// @endcode
    ResponseOnEventService(ResponseOnEventService const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00691
    /// @needwork = dda
    /// @endcode
    ResponseOnEventService& operator=(ResponseOnEventService const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00692
    /// @needwork = dda
    /// @endcode
    ResponseOnEventService(ResponseOnEventService&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00693
    /// @needwork = dda
    /// @endcode
    ResponseOnEventService& operator=(ResponseOnEventService&&) = delete;

public:
    /// @brief ResponseOnEventService function
    /// @param sessionid Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01310
    /// @needwork = dd
    /// @endcode
    explicit ResponseOnEventService(SessionId sessionid) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01311
    /// @needwork = dd
    /// @endcode
    ~ResponseOnEventService() noexcept = default;

    /// @brief Set RequestCallback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01312
    /// @needwork = dd
    /// @endcode
    void SetRequestCallback(std::function< void(std::vector< std::uint8_t > const&) > const& callback) noexcept;

    /// @brief Initialization function
    /// @param config Parameter
    /// @param dataManager Parameter
    /// @param dtcFilterPtr Parameter
    /// @param persistenceServicePtr Parameter
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01313
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(RoeManagement& config,
                            std::shared_ptr< DiagnosticDataManager > const& dataManager,
                            std::shared_ptr< DtcFilter > const& dtcFilterPtr,
                            std::shared_ptr< PersistenceService > const& persistenceServicePtr) noexcept;

    /// @brief Set EventSetup
    /// @param setup Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01314
    /// @needwork = dd
    /// @endcode
    void Setup(EventSetup& setup) noexcept;

    /// @brief Start function
    /// @param store Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01315
    /// @needwork = dd
    /// @endcode
    NrcErrc Start(bool store, std::shared_ptr< RequestContext > const& requestContext) noexcept;

    /// @brief Stop function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01316
    /// @needwork = dd
    /// @endcode
    void Stop() noexcept;

    /// @brief Clear function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01317
    /// @needwork = dd
    /// @endcode
    void Clear() noexcept;

    /// @brief Active function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01318
    /// @needwork = dd
    /// @endcode
    void Active() noexcept;

    /// @brief Report function
    /// @param eventType Parameter
    /// @param eventWindowTime Parameter
    /// @param eventTypeRecord Parameter
    /// @param serviceToRespondToRecord Parameter
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01319
    /// @needwork = dd
    /// @endcode
    std::int32_t Report(std::uint8_t& eventType,
                        std::uint8_t& eventWindowTime,
                        std::vector< std::uint8_t >& eventTypeRecord,
                        std::vector< std::uint8_t >& serviceToRespondToRecord) noexcept;

private:
    /// @brief _timeout function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01320
    /// @needwork = dd
    /// @endcode
    void _timeout() noexcept;
    /// @brief _sendRequest function
    /// @param data Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01321
    /// @needwork = dd
    /// @endcode
    void _sendRequest(std::vector< std::uint8_t > data) noexcept;
    /// @brief _loadEvent function
    /// @param address Parameter
    /// @param eventData Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01322
    /// @needwork = dd
    /// @endcode
    void _loadEvent(std::uint64_t& address, std::vector< std::uint8_t >& eventData) noexcept;
    /// @brief _saveEvent function
    /// @param key Parameter
    /// @param eventType Parameter
    /// @param eventWindowTime Parameter
    /// @param eventTypeRecord Parameter
    /// @param serviceToRespondToRecord Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01323
    /// @needwork = dd
    /// @endcode
    void _saveEvent(std::uint64_t key,
                    std::uint8_t& eventType,
                    std::uint8_t& eventWindowTime,
                    std::vector< std::uint8_t >& eventTypeRecord,
                    std::vector< std::uint8_t >& serviceToRespondToRecord) noexcept;
    /// @brief _loadEventTable function
    /// @param table Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01324
    /// @needwork = dd
    /// @endcode
    void _loadEventTable(std::vector< std::uint64_t >& table) noexcept;
    /// @brief _saveNewEventAddress function
    /// @param eventAddress Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01325
    /// @needwork = dd
    /// @endcode
    void _saveNewEventAddress(std::uint64_t& eventAddress) noexcept;
    /// @brief _saveEventTable function
    /// @param table Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01326
    /// @needwork = dd
    /// @endcode
    void _saveEventTable(std::vector< std::uint64_t >& table) noexcept;
    /// @brief _clearEventTable function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01327
    /// @needwork = dd
    /// @endcode
    void _clearEventTable() noexcept;
    /// @brief _clearEvent function
    /// @param address Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01328
    /// @needwork = dd
    /// @endcode
    void _clearEvent(std::uint64_t& address) noexcept;

    /// @brief sessionid
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01329
    /// @needwork = dd
    /// @endcode
    SessionId sessionid_{0U};
    /// @brief dtcFilterPtr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01330
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DtcFilter > dtcFilterPtr_{nullptr};
    /// @brief diagnosticDataManagerPtr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01331
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DiagnosticDataManager > diagnosticDataManagerPtr_{nullptr};
    /// @brief requestCallback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01332
    /// @needwork = dd
    /// @endcode
    std::function< void(std::vector< std::uint8_t >) > requestCallback_{nullptr};
    /// @brief priority
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01333
    /// @needwork = dd
    /// @endcode
    Priority priority_{0U};
    /// @brief eventTable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01334
    /// @needwork = dd
    /// @endcode
    EventTable eventTable_{};
    /// @brief processorPtr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01335
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< EventProcessor > processorPtr_{nullptr};
    /// @brief eventType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01336
    /// @needwork = dd
    /// @endcode
    EventType eventType_{EventType::kUnknow};
    /// @brief timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01337
    /// @needwork = dd
    /// @endcode
    Timer timer_{};
    /// @brief persistenceServicePtr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01338
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< PersistenceService > persistenceServicePtr_{nullptr};
    /// @brief config
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01339
    /// @needwork = dd
    /// @endcode
    RoeManagement config_{};
    /// @brief metaInfo
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01340
    /// @needwork = dd
    /// @endcode
    MetaInfoMap metaInfo_{};
    /// @brief lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01341
    /// @needwork = dd
    /// @endcode
    std::mutex lock_{};
    /// @brief stop
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01342
    /// @needwork = dd
    /// @endcode
    bool stop_{false};
};

/// @brief DiagnosticSessionControl service is used to enable different diagnostic sessions in the server.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02443
/// @needwork = dd
/// @endcode
class DiagnosticRoeManager final
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00694
    /// @needwork = dda
    /// @endcode
    DiagnosticRoeManager(DiagnosticRoeManager const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00695
    /// @needwork = dda
    /// @endcode
    DiagnosticRoeManager& operator=(DiagnosticRoeManager const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00696
    /// @needwork = dda
    /// @endcode
    DiagnosticRoeManager(DiagnosticRoeManager&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00697
    /// @needwork = dda
    /// @endcode
    DiagnosticRoeManager& operator=(DiagnosticRoeManager&&) = delete;

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01343
    /// @needwork = dd
    /// @endcode
    DiagnosticRoeManager() noexcept = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01344
    /// @needwork = dd
    /// @endcode
    ~DiagnosticRoeManager() noexcept = default;

    /// @brief Set GetChannelCallback
    /// @param getChannel Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01345
    /// @needwork = dd
    /// @endcode
    void SetGetChannelCallback(std::function< std::shared_ptr< Channel >(Address) > getChannel) noexcept
    {
        getChannel_ = std::move(getChannel);
    }

    /// @brief Set IndicateCallback
    /// @param callback Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01346
    /// @needwork = dd
    /// @endcode
    void SetIndicateCallback(
        std::function< Result< void >(std::shared_ptr< Message > const&, Priority, std::shared_ptr< Channel > const&) >
            callback) noexcept
    {
        indicateCallback_ = std::move(callback);
    }

    /// @brief Initialization function
    /// @param config Parameter
    /// @param sessionManagerPtr Parameter
    /// @param diagnosticDataManagerPtr Parameter
    /// @param dtcFilterPtr Parameter
    /// @param persistenceServicePtr Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01347
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(RoeManagement& config,
                            std::shared_ptr< SessionManager > const& sessionManagerPtr,
                            std::shared_ptr< DiagnosticDataManager > const& diagnosticDataManagerPtr,
                            std::shared_ptr< DtcFilter > const& dtcFilterPtr,
                            std::shared_ptr< PersistenceService > const& persistenceServicePtr) noexcept;

    /// @brief Set Setup function
    /// @param setup Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01348
    /// @needwork = dd
    /// @endcode
    void Setup(EventSetup& setup) noexcept;

    /// @brief Start function
    /// @param store Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01349
    /// @needwork = dd
    /// @endcode
    NrcErrc Start(bool store, std::shared_ptr< RequestContext > const& requestContext) noexcept;

    /// @brief Stop function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01350
    /// @needwork = dd
    /// @endcode
    void Stop() noexcept;

    /// @brief Clear function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @needwork = dd
    /// @endcode
    void Clear() noexcept;

    /// @brief Report function
    /// @param eventType Parameter
    /// @param eventWindowTime Parameter
    /// @param eventTypeRecord Parameter
    /// @param serviceToRespondToRecord Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01351
    /// @needwork = dd
    /// @endcode
    std::int32_t Report(std::uint8_t& eventType,
                        std::uint8_t& eventWindowTime,
                        std::vector< std::uint8_t >& eventTypeRecord,
                        std::vector< std::uint8_t >& serviceToRespondToRecord) noexcept;

private:
    /// @brief _sendRequest function
    /// @param data Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01352
    /// @needwork = dd
    /// @endcode
    void _sendRequest(std::vector< std::uint8_t > data) noexcept;
    /// @brief getChannel_ function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01353
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Channel > _getChannel(Address client) noexcept { return getChannel_(client); }
    /// @brief _indicate function
    /// @param request Parameter
    /// @param priority Parameter
    /// @param channel Parameter
    /// @return Return value
    /// @endcode
    Result< void > _indicate(std::shared_ptr< Message > request,
                             Priority priority,
                             std::shared_ptr< Channel > channel) noexcept
    {
        return indicateCallback_(std::move(request), priority, std::move(channel));
    }

private:
    /// @brief Session manager pointer
    /// @name sessionManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01354
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< SessionManager > sessionManagerPtr_{nullptr};
    /// @brief dtc filter pointer
    /// @name dtcFilterPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01355
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DtcFilter > dtcFilterPtr_{nullptr};
    /// @brief Diagnostic data manager pointer
    /// @name diagnosticDataManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01356
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DiagnosticDataManager > diagnosticDataManagerPtr_{nullptr};

    /// @brief Get channel function pointer
    /// @name getChannel_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01357
    /// @needwork = dd
    /// @endcode
    std::function< std::shared_ptr< Channel >(Address) > getChannel_{nullptr};

    /// @brief Notification callback pointer
    /// @name indicateCallback_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01358
    /// @needwork = dd
    /// @endcode
    std::function< Result< void >(std::shared_ptr< Message >, Priority, std::shared_ptr< Channel >) > indicateCallback_{
        nullptr};

    /// @brief Priority
    /// @name priority_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01359
    /// @needwork = dd
    /// @endcode
    Priority priority_{0U};

    /// @brief Server address
    /// @name serverAddress_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00698
    /// @needwork = dda
    /// @endcode
    Address serverAddress_{0U};

    /// @brief Client address
    /// @name clientAddress_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01360
    /// @needwork = dd
    /// @endcode
    Address clientAddress_{0U};

    /// @brief Response session table
    /// @name responseOnEventServiceTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01361
    /// @needwork = dd
    /// @endcode
    std::map< SessionId, std::shared_ptr< ResponseOnEventService > > responseOnEventServiceTable_{};

    /// @brief Request list
    /// @name requestList_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01362
    /// @needwork = dd
    /// @endcode
    std::list< std::vector< std::uint8_t > > requestList_{};

    /// @brief Request timer
    /// @name requestTimer_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01363
    /// @needwork = dd
    /// @endcode
    Timer requestTimer_{};

    /// @brief Persistence service pointer
    /// @name persistenceServicePtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01364
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< PersistenceService > persistenceServicePtr_{nullptr};

    /// @brief Configuration information
    /// @name config_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01365
    /// @needwork = dd
    /// @endcode
    RoeManagement config_{};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_DiagnosticRoeManager_H_