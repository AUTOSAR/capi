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
/// @file       idsm_diag_dtc_server.cpp
/// @brief      Diagnosis DtcServer implementation
/// @details
/// @date       2023-01-17
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Qualified security event storage
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0012
/// @unit_name=DiagDtcServer
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_diag_dtc_server.h"

#include "ara/idsm/internal/idsm_error_domain.h"
#include "log/idsm_log.h"

namespace ara {
namespace idsm {
#ifdef ARA_WITH_DIAG
/// @brief constructor
/// @param specifier TODO
/// @param reentrancyType TODO
/// @param monitor monitor instance descriptor
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
DiagDtcServer::DiagDtcServer(ara::core::InstanceSpecifier const& specifier,
                             ara::diag::DataIdentifierReentrancyType const reentrancyType,
                             ara::core::InstanceSpecifier monitor)
    : GenericDataIdentifier{specifier, reentrancyType}, monitorInstance_{std::move(monitor)}
{
}
/// @brief Initialize event id mapped by diagnostic data identifier
/// @param id Id of the security event
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void DiagDtcServer::Init(uint16_t const id)
{
    eventId_ = id;
    monitor_ = std::make_shared< ara::diag::Monitor >(
        monitorInstance_,
        [](ara::diag::InitMonitorReason const reason) noexcept {
            LOG_INFO << "monitor init callback reason=" << static_cast< std::uint32_t >(reason);
        },
        []() noexcept { return 0; });
}
/// @brief Read data of security event
/// @param dataIdentifier data identifier mapped to event id
/// @param metaInfo
/// @param cancellationHandler
/// @return security event information
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
ara::core::Future< OperationOutput > DiagDtcServer::Read(std::uint16_t dataIdentifier,
                                                         ara::diag::MetaInfo& metaInfo,
                                                         ara::diag::CancellationHandler cancellationHandler)
{
    static_cast< void >(metaInfo);
    static_cast< void >(cancellationHandler);
    _checkOldData();
    DataCachePtr dataPtr{std::make_shared< DataCache >()};
    {
        std::lock_guard< std::mutex > const lock{bufferLock_};
        if (dataQueue_.empty()) {
            LOG_ERROR << "dataQueue is empty. dataIdentifier:" << dataIdentifier << "eventId:" << eventId_;
            uint8_t const errInt{5U};
            ContextDataType errData{errInt};
            dataPtr->SetContextData(errData);
        } else {
            dataPtr = dataQueue_.front();
            dataQueue_.pop_front();
        }
    }

    ContextDataType ctxData;
    dataPtr->GetContextData(ctxData);
    LOG_INFO << "diag read event data. dataIdentifier:" << dataIdentifier << "eventId:" << eventId_
             << "data size:" << ctxData.size();

    ara::core::Promise< OperationOutput > promise;
    OperationOutput buffer{};
    std::ignore = buffer.responseData.insert(buffer.responseData.cend(), ctxData.cbegin(), ctxData.cend());
    promise.set_value(buffer);
    return promise.get_future();
}
/// @brief will not be called
/// @param dataIdentifier
/// @param requestData
/// @param metaInfo
/// @param cancellationHandler
/// @return whether data write succeeded
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
ara::core::Future< void > DiagDtcServer::Write(std::uint16_t dataIdentifier,
                                               ara::core::Span< std::uint8_t > requestData,
                                               ara::diag::MetaInfo& metaInfo,
                                               ara::diag::CancellationHandler cancellationHandler)
{
    static_cast< void >(dataIdentifier);
    static_cast< void >(requestData);
    static_cast< void >(metaInfo);
    static_cast< void >(cancellationHandler);
    // Here didserver only provides Read method; Write method will not be called
    LOG_ERROR << "idsmd DiagDtcServer::Write can't call";

    ara::core::Promise< void > promise;
    promise.SetError(static_cast< ara::core::ErrorCode >(IdsmErrorCode::kIdsmDtcServerWriteNotCall));
    return promise.get_future();
}
/// @brief Append data of security event
/// @param data data of the security event
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void DiagDtcServer::PushData(ContextDataType const& data) noexcept
{
    if (monitor_ == nullptr) {
        LOG_ERROR << "diag: monitor init fail. monitor is nullptr";
        return;
    }
    LOG_INFO << "idsm have new security event. notify diag pull event data.";
    time_t timestamp{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
    DataCachePtr const dataPtr{std::make_shared< DataCache >(timestamp, data)};
    if (timestamp == 0) {
    }
    {
        std::lock_guard< std::mutex > const lock{bufferLock_};
        dataQueue_.push_back(dataPtr);
    }
    ara::core::Result< void > const resultReport{monitor_->ReportMonitorAction(ara::diag::MonitorAction::kFailed)};
    if (!resultReport.HasValue()) {
        LOG_ERROR << "diag: monitor->ReportMonitorAction err=" << resultReport.Error();
        return;
    }
}
/// @brief Discard data of security event
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void DiagDtcServer::PopData() noexcept
{
    std::lock_guard< std::mutex > const lock{bufferLock_};
    dataQueue_.pop_front();
}
/// @brief Check if data of security event has timed out
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void DiagDtcServer::_checkOldData()
{
    if (dataQueue_.empty()) {
        return;
    }

    DataCachePtr dataPtr;
    {
        std::lock_guard< std::mutex > const lock{bufferLock_};
        dataPtr = dataQueue_.front();
    }
    std::time_t const now{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
    int32_t const temp{60 * 3};
    if ((now - dataPtr->GetTimeStamp()) > temp) {
        LOG_WARN << "DiagDtcServer have oldData more than 180s clear. eventId:" << eventId_;
    }
}
#endif

}  // namespace idsm
}  // namespace ara
