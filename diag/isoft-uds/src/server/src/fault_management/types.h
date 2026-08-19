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
/// @file       types.h
/// @brief      This file provides types functionality
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
/// @trace_id_sr=0386e60e-4d77-4d37-91b5-da4e3ff20f06
/// @unit_name=types
/// @unit_description=This file provides types functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_TYPES_H__
#define __ISOFT_UDS_FAULT_TYPES_H__
#include <cstdint>
#include <memory>
#include <vector>

namespace isoft {
namespace uds {
namespace server {

/// @brief extended data record class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02456
/// @needwork = dd
/// @endcode
class DTCExtDataRecord final
{
public:
    /// @name recordNumber
    /// record number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01523
    /// @needwork = dd
    /// @endcode
    std::uint8_t recordNumber{0U};
    /// @name data
    /// data of this record
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01524
    /// @needwork = dd
    /// @endcode
    std::vector< std::uint8_t > data{};
};

/// @brief dtc and status record class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02457
/// @needwork = dd
/// @endcode
struct DTCAndStatusRecord final
{
    /// @name status
    /// dtc status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01525
    /// @needwork = dd
    /// @endcode
    std::uint8_t status{0U};
    /// @name dtcValue
    /// dtc value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01526
    /// @needwork = dd
    /// @endcode
    std::uint32_t dtcValue{0U};
};

/// @brief dtc and status record class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02458
/// @needwork = dd
/// @endcode
struct DTCAndSnapshotNumber final
{
    /// @name dtcValue
    /// dtc value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01527
    /// @needwork = dd
    /// @endcode
    std::uint32_t dtcValue{0U};
    /// @name snapshotIdentifierTable
    /// snapshot identifier table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01528
    /// @needwork = dd
    /// @endcode
    std::vector< std::uint8_t > snapshotIdentifierTable{};
};

/// @brief dtc fault detection counter record class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02459
/// @needwork = dd
/// @endcode
struct DTCFaultDetectionCounterRecord final
{
    /// @name fdc
    /// fault detected counter, value range: [0-127].
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01529
    /// @needwork = dd
    /// @endcode
    std::int8_t fdc{0U};
    /// @name dtcValue
    /// dtc value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01530
    /// @needwork = dd
    /// @endcode
    std::uint32_t dtcValue{0U};
};

/// @brief global const value enum
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02460
/// @needwork = dd
/// @endcode
enum class GlobalConstValueEnum : std::uint32_t
{
    kSnapshotDataRecordInvalid = 0x00,
    kSnapshotDataRecordAll     = 0xFF,
    kExtDataRecordInvalid      = 0x00,
    kExtDataRecordAll          = 0xFF
};
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_TYPES_H__