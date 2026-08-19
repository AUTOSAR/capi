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
/// @file       data_identifier_set.cpp
/// @brief      This file implements data_identifier_set functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=cee8f039-1a33-4416-830d-66da67253862
/// @unit_name=DataIdentifierSet
/// @unit_description=This file provides data_identifier_set functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/data_identifier_set.h"

#include <condition_variable>

#include "common/log.h"
#include "fault_management/public.h"

namespace isoft {
namespace uds {
namespace server {


void DataIdentifierSet::CaptureData(std::vector< SnapshotDataIdentifierType >& snapshotDataIdentifiers,
                                    std::uint32_t const dtc) const
{
    if (dataManage_.get() == nullptr) {
        LogWarn() << "DataIdentifierSet::CaptureData|dataManage is nullptr id =" << id_ << ", dtc =" << dtc;
        return;
    }

    MetaInfoMap metaInfo;
    Helper::MakeMetaInfo(dtc, metaInfo);
    std::shared_ptr< CancellationState > cancellationState;
    std::vector< uint16_t > dataIdentifierTable(dids_.begin(), dids_.end());

    LogVerbose() << "DataIdentifierSet::CaptureData|did_set_id =" << id_ << ", dtc =" << dtc
                 << ", didsize =" << dids_.size() << "dataIdentifierTable =" << dataIdentifierTable.size();

    // Get data from data manager
    Result< std::list< DiagnosticData > > result = dataManage_->Read(dataIdentifierTable, metaInfo, cancellationState);
    if (!result.HasValue()) {
        for (auto const& did : dids_) {
            SnapshotDataIdentifierType type;
            type.dataIdentifier = did;
            type.data.emplace_back(kPandingData);
            snapshotDataIdentifiers.emplace_back(type);
        }
        LogWarn() << "DataIdentifierSet::CaptureData|read data fails error =" << result.Error();
        return;
    }

    for (auto const& did : dids_) {
        bool isFind{false};
        for (auto& entry : result.Value()) {
            if (entry.id != did) {
                continue;
            }

            SnapshotDataIdentifierType type;
            type.dataIdentifier = entry.id;
            type.data           = entry.record;
            snapshotDataIdentifiers.emplace_back(type);

            isFind = true;
            break;
        }

        if (!isFind) {
            SnapshotDataIdentifierType type;
            type.dataIdentifier = did;
            type.data.emplace_back(kPandingData);
            snapshotDataIdentifiers.emplace_back(type);
            LogWarn() << "DataIdentifierSet::CaptureData|read data fails did =" << did;
        }
    }

    LogVerbose() << "DataIdentifierSet::CaptureData -> end. id=" << id_ << ", dtc=" << dtc
                 << "readSize =" << result.Value().size()
                 << "snapshotDataIdentifiers_size =" << snapshotDataIdentifiers.size();
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
