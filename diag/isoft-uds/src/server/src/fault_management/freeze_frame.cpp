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
/// @file       freeze_frame.cpp
/// @brief      This file implements freeze_frame functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=558617ec-8aed-421c-8160-5dc34f77a602
/// @unit_name=FreezeFrame
/// @unit_description=This file provides freeze_frame functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/freeze_frame.h"

#include "common/log.h"
#include "fault_management/data_identifier_set.h"

namespace isoft {
namespace uds {
namespace server {


bool FreezeFrame::CaptureSnapshotRecord(std::shared_ptr< DataIdentifierSet > const& snapshotRecordContent,
                                        SnapshotDataRecordType& record,
                                        std::uint32_t const dtc) const
{
    LogVerbose() << "FreezeFrame::CaptureSnapshotRecord -> enter. dtc=" << dtc
                 << ", recordNumber=" << config_.recordNumber;
    if (nullptr == snapshotRecordContent) {
        LogWarn() << "FreezeFrame::CaptureSnapshotRecord -> snapshotRecordContent is null."
                  << ", recordNumber=" << config_.recordNumber;
        return false;
    }

    record.snapshotRecordNumber = config_.recordNumber;
    snapshotRecordContent->CaptureData(record.snapshotDataIdentifiers, dtc);
    std::ignore = record.snapshotDataIdentifiers;
    return true;
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
