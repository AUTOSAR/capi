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
/// @file       idsm_diag_did_server.cpp
/// @brief      Security event reporting mode access class, providing AA-side users with related functions for accessing security event reporting mode
/// @details
/// @date       2023-01-16
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Diagnostic data identifier service
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0004
/// @unit_name=DiagDidServer
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_diag_did_server.h"

#include "ara/idsm/internal/idsm_error_domain.h"
#include "event/idsm_event_proper.h"
#include "log/idsm_log.h"

namespace ara {
namespace idsm {
#ifdef ARA_WITH_DIAG
/// @brief constructor
/// @param specifier
/// @param reentrancyType
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
DiagDidServer::DiagDidServer(ara::core::InstanceSpecifier const& specifier,
                             ara::diag::DataIdentifierReentrancyType const reentrancyType)
    : GenericDataIdentifier{specifier, reentrancyType}
{
}
/// @brief Initialize mapping between diagnostic data identifier and event id
/// @param m mapping from data identifier to event id
/// @return 0 success, -1 failure
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t DiagDidServer::Init(ara::core::Map< uint16_t, uint16_t > const& m)
{
    didIdToEventIdMap_ = m;
    return 0;
}
/// @brief Read reporting mode of security event
/// @param dataIdentifier data identifier mapped to event id
/// @param metaInfo
/// @param cancellationHandler
/// @return reporting mode of security event
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
ara::core::Future< OperationOutput > DiagDidServer::Read(std::uint16_t dataIdentifier,
                                                         ara::diag::MetaInfo& metaInfo,
                                                         ara::diag::CancellationHandler cancellationHandler)
{
    static_cast< void >(metaInfo);
    static_cast< void >(cancellationHandler);
    ara::core::Promise< OperationOutput > promise;
    OperationOutput buffer;
    buffer.responseData.clear();

    ReportingMode mode;
    if (didIdToEventIdMap_.count(dataIdentifier) == 0U) {
        LOG_ERROR << "diag read event reporting mode fail. dataIdentifier:" << dataIdentifier << "not found event.";
        promise.SetError(static_cast< ara::core::ErrorCode >(IdsmErrorCode::kIdsmDidNotFind));
        return promise.get_future();
    }

    uint16_t const eventId{didIdToEventIdMap_[dataIdentifier]};
    mode = EventProperPool::GetInstance()->GetEventMode(eventId);
    LOG_INFO << "diag read event reporting mode success. dataIdentifier:" << dataIdentifier << "event id:" << eventId
             << "mode:" << static_cast< uint8_t >(mode);
    buffer.responseData.push_back(static_cast< uint8_t >(mode));
    promise.set_value(buffer);
    return promise.get_future();
}
/// @brief Set reporting mode of security event
/// @param dataIdentifier data identifier mapped to event id
/// @param requestData
/// @param metaInfo
/// @param cancellationHandler
/// @return whether data write succeeded
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
ara::core::Future< void > DiagDidServer::Write(std::uint16_t dataIdentifier,
                                               ara::core::Span< std::uint8_t > requestData,
                                               ara::diag::MetaInfo& metaInfo,
                                               ara::diag::CancellationHandler cancellationHandler)
{
    static_cast< void >(metaInfo);
    static_cast< void >(cancellationHandler);
    ara::core::Promise< void > promise;
    if (didIdToEventIdMap_.count(dataIdentifier) == 0U) {
        LOG_ERROR << "diag set event reporting mode fail. dataIdentifier:" << dataIdentifier << "not found event";
        promise.SetError(static_cast< ara::core::ErrorCode >(IdsmErrorCode::kIdsmDidNotFind));
        return promise.get_future();
    }

    uint16_t const eventId{didIdToEventIdMap_[dataIdentifier]};
    ReportingMode const mode{static_cast< ReportingMode >(requestData[0U])};
    if (mode < ReportingMode::kUnknown) {
        LOG_INFO << "diag set event reporting mode success. dataIdentifier:" << dataIdentifier << "event id:" << eventId
                 << "mode:" << static_cast< uint8_t >(mode);
        if (EventProperPool::GetInstance()->SetEventMode(eventId, mode) == -1) {
            // Error setting event reporting mode TODO
        }
        promise.set_value();
        return promise.get_future();
    }
    LOG_ERROR << "diag set event  reporting mode fail. dataIdentifier:" << dataIdentifier << "event id:" << eventId
              << "reporting mode:" << requestData[0U] << "is invalid.";
    promise.SetError(static_cast< ara::core::ErrorCode >(IdsmErrorCode::kIdsmDidReportModeErr));
    return promise.get_future();
}
#endif
}  // namespace idsm
}  // namespace ara
