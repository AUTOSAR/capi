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
/// @file       data_identifier_set.h
/// @brief      This file provides data_identifier_set functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/fault_management
/// @interface_level=software
/// @trace_id_sr=cee8f039-1a33-4416-830d-66da67253862
/// @unit_name=DataIdentifierSet
/// @unit_description=This file provides data_identifier_set functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_DATA_IDENTIFIER_SET_H__
#define __ISOFT_UDS_FAULT_DATA_IDENTIFIER_SET_H__

#include <cstdint>
#include <memory>
#include <set>

#include "data_management/diagnostic_data_manager.h"
#include "isoft/uds/fault_management/dtc_information_interface.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief DataIdentifierSet class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02499
/// @needwork = dd
/// @endcode
class DataIdentifierSet final
{
public:
    /// @brief Constructor
    /// @param[in] id The marker of this set
    /// @param[in] didList All dids contained in this set
    /// @param[in] dataManager Data manager
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    DataIdentifierSet(uint32_t const id,
                      std::set< uint16_t >& didList,
                      std::shared_ptr< DiagnosticDataManager >& dataManager)
        : id_{id}, dids_{didList}, dataManage_{dataManager}
    {
    }

    /// @brief Copy constructor
    /// @param[in] other DataIdentifierSet reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00798
    /// @needwork = dda
    /// @endcode
    DataIdentifierSet(DataIdentifierSet const& other) = default;

    /// @brief Move constructor
    /// @param[in] other DataIdentifierSet rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01655
    /// @needwork = dd
    /// @endcode
    DataIdentifierSet(DataIdentifierSet&& other) = default;

    /// @brief Copy assignment operator
    /// @param[in] right DataIdentifierSet reference
    /// @return DataIdentifierSet reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01656
    /// @needwork = dd
    /// @endcode
    DataIdentifierSet& operator=(DataIdentifierSet const& right) = default;

    /// @brief Move assignment operator
    /// @param[in] right DataIdentifierSet rvalue reference
    /// @return DataIdentifierSet reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01657
    /// @needwork = dd
    /// @endcode
    DataIdentifierSet& operator=(DataIdentifierSet&& right) = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01658
    /// @needwork = dd
    /// @endcode
    ~DataIdentifierSet() = default;

    /// @brief Capture data of all dids in this set
    /// @param[out] snapshotDataIdentifiers Output parameter, did captured data is placed here
    /// @param[in] dtc dtc value
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void CaptureData(std::vector< SnapshotDataIdentifierType >& snapshotDataIdentifiers, std::uint32_t const dtc) const;

private:
    /// @brief The marker of this set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01659
    /// @needwork = dd
    /// @endcode
    uint32_t id_;

    /// @brief All dids contained in this set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01660
    /// @needwork = dd
    /// @endcode
    std::set< uint16_t > dids_;

    /// @brief Data manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01661
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DiagnosticDataManager > dataManage_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft
#endif  // __ISOFT_UDS_FAULT_DATA_IDENTIFIER_H__