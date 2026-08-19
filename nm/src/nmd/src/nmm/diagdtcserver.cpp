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
/// @file       diagdtcserver.cpp
/// @brief      Reporting diagnostic events related
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @trace_id_sr=SRS_NM_00017
/// @unit_name=DiagDtcServer
/// @unit_description=Reporting diagnostic events related
/// @module_path=/NetworkManager/nmm
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "include/diagdtcserver.h"

#include "common/nmerrordomain.h"
#include "utils/include/utils.h"

namespace ara {
namespace nm {
namespace internal {
/// @brief Constructor
/// @param specifier
/// @param reentrancyType
DiagDtcServer::DiagDtcServer(ara::core::InstanceSpecifier const &specifier,
                             ara::diag::DataIdentifierReentrancyType const reentrancyType) noexcept
    : GenericDataIdentifier{specifier, reentrancyType}
    , bufferLock_{}
    , dataQueue_{}
    , monitorInstance_{nullptr}
    , monitor_{nullptr}
    , bStop_{false}
{
}
/// @brief Constructor
/// @param specifier
/// @param reentrancyType
/// @param operCycleInstance
DiagDtcServer::DiagDtcServer(ara::core::InstanceSpecifier const &specifier,
                             ara::diag::DataIdentifierReentrancyType const reentrancyType,
                             ara::core::InstanceSpecifier const &operCycleInstance) noexcept
    : GenericDataIdentifier{specifier, reentrancyType}
    , bufferLock_{}
    , dataQueue_{}
    , monitorInstance_{nullptr}
    , monitor_{nullptr}
    , bStop_{false}
{
    operationCycle_ = std::make_shared< ara::diag::OperationCycle >(operCycleInstance);
    operationCycle_->SetOperationCycle(ara::diag::OperationCycleType::kOperationCycleStart);
}

/// @brief Initialize the event id mapped by the diagnostic data identifier
/// @param monitor
void DiagDtcServer::Init(ara::core::InstanceSpecifier const &monitor) noexcept
{
    monitorInstance_ = std::make_shared< ara::core::InstanceSpecifier >(monitor);
    monitor_         = std::make_shared< ara::diag::Monitor >(
        *monitorInstance_,
        [](ara::diag::InitMonitorReason const reason) noexcept {
            NmLogger().LogInfo() << "monitor init callback reason=" << static_cast< std::uint32_t >(reason);
        },
        []() noexcept { return 0; });
}

/// @brief Read fault event data
/// @param dataIdentifier Data identifier mapped to event id
/// @param metaInfo Inherited interface parameter, currently not used
/// @param cancellationHandler Inherited interface parameter, currently not used
/// @throws  Throws exception
/// @return Fault event information
ara::core::Future< OperationOutput > DiagDtcServer::Read(std::uint16_t const dataIdentifier,
                                                         ara::diag::MetaInfo &metaInfo,
                                                         ara::diag::CancellationHandler cancellationHandler) noexcept
{
    static_cast< void >(metaInfo);
    static_cast< void >(cancellationHandler);
    ara::core::Promise< OperationOutput > promise;
    OperationOutput buffer{};
    ara::core::String dataString{"\0"};
    {
        std::lock_guard< std::mutex > const lock{bufferLock_};
        if (!dataQueue_.empty()) {
            dataString = dataQueue_.front()->data;
            // dataQueue_.pop_front();
        }
    }
    NmLogger().LogInfo() << "diag read event data. dataIdentifier:" << dataIdentifier
                         << "data size:" << dataString.size();
    std::ignore = buffer.responseData.insert(buffer.responseData.cend(), dataString.cbegin(), dataString.cend());
    promise.set_value(buffer);
    return promise.get_future();
}

/// @brief Will not be called
/// @param dataIdentifier
/// @param requestData
/// @param metaInfo
/// @param cancellationHandler
/// @throws  Throws exception
/// @return Whether data write succeeded
ara::core::Future< void > DiagDtcServer::Write(std::uint16_t dataIdentifier,
                                               ara::core::Span< std::uint8_t > requestData,
                                               ara::diag::MetaInfo &metaInfo,
                                               ara::diag::CancellationHandler cancellationHandler) noexcept
{
    static_cast< void >(dataIdentifier);
    static_cast< void >(requestData);
    static_cast< void >(metaInfo);
    static_cast< void >(cancellationHandler);
    // This didserver only provides Read method, Write method will not be called
    NmLogger().LogError() << "nmd DiagDtcServer::Write can't call";

    ara::core::Promise< void > promise;
    promise.SetError(static_cast< ara::core::ErrorCode >(NmErrc::kDtcServerCantWrite));
    return promise.get_future();
}

/// @brief Append fault event data
/// @param data Fault event data
void DiagDtcServer::PushData(ara::core::String const &data) noexcept
{
    NmLogger().LogInfo() << "nm have init report event. notify diag pull event data.";
    time_t const timestamp{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
    std::shared_ptr< DataCache > const dataPtr{std::make_shared< DataCache >()};
    if (nullptr != dataPtr) {
        std::lock_guard< std::mutex > const lock{bufferLock_};
        dataPtr->timestamp = timestamp;
        dataPtr->data      = data;
        dataQueue_.push_back(dataPtr);
    }
    while (!bStop_) {
        ara::core::Result< void > const resultReport{monitor_->ReportMonitorAction(ara::diag::MonitorAction::kFailed)};
        if (!resultReport.HasValue()) {
            NmLogger().LogWarn() << "diag: monitor->ReportMonitorAction err=" << resultReport.Error() << ", try later";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } else {
            return;
        }
    }
}

}  // namespace internal
}  // namespace nm
}  // namespace ara
