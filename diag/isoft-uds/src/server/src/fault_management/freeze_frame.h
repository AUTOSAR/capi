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
/// @file       freeze_frame.h
/// @brief      This file provides freeze_frame functionality
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
/// @trace_id_sr=558617ec-8aed-421c-8160-5dc34f77a602
/// @unit_name=FreezeFrame
/// @unit_description=This file provides freeze_frame functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_FREEZE_FRAME_H__
#define __ISOFT_UDS_FAULT_FREEZE_FRAME_H__
#include <memory>

#include "fault_management/public.h"
#include "isoft/uds/fault_management/dtc_information_interface.h"
#include "isoft/uds/fault_management/fault_management.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief DataIdentifierSet class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02533
/// @needwork = dd
/// @endcode
class DataIdentifierSet;

/// @brief freeze frame wrapper class, encapsulates various configuration parameters and behaviors such as configuration parsing and data capture
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02534
/// @needwork = dd
/// @endcode
class FreezeFrame final
{
public:
    /// @brief FreezeFrame function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01804
    /// @needwork = dd
    /// @endcode
    explicit FreezeFrame(FreezeFrameConfig& config) : config_{config}
    {
        if (!config_.customTrigger.empty()) {
            config_.trigger = DiagnosticRecordTriggerEnum::kCustom;
        }
    };

    /// @brief Copy constructor
    /// @param[in] other Indicator reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01805
    /// @needwork = dd
    /// @endcode
    FreezeFrame(FreezeFrame const& other) = default;

    /// @brief Move constructor
    /// @param[in] other Indicator rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01806
    /// @needwork = dd
    /// @endcode
    FreezeFrame(FreezeFrame&& other) = default;

    /// @brief Copy assignment function
    /// @param[in] right Indicator reference
    /// @return Indicator reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01807
    /// @needwork = dd
    /// @endcode
    FreezeFrame& operator=(FreezeFrame const& right) = default;

    /// @brief Move assignment function
    /// @param[in] right Indicator rvalue reference
    /// @return Indicator reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01808
    /// @needwork = dd
    /// @endcode
    FreezeFrame& operator=(FreezeFrame&& right) = default;

    /// @brief Destructor
    /// @param[in] object Reference
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01809
    /// @needwork = dd
    /// @endcode
    ~FreezeFrame() = default;

    /// @brief Get this record number
    /// @return Record number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01810
    /// @needwork = dd
    /// @endcode
    uint8_t GetRecordNumber() const noexcept { return config_.recordNumber; }

    /// @brief Get whether the record can be updated
    /// @return Returns true if updatable, false if not
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01811
    /// @needwork = dd
    /// @endcode
    bool GetUpdate() const noexcept { return config_.update; }

    /// @brief Get configured trigger
    /// @return Configured trigger
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01812
    /// @needwork = dd
    /// @endcode
    DiagnosticRecordTriggerEnum GetTrigger() const noexcept { return config_.trigger; }

    /// @brief Get custom trigger
    /// @return Custom trigger
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01813
    /// @needwork = dd
    /// @endcode
    std::string GetCustomTrigger() const noexcept { return config_.customTrigger; }

    /// @brief Capture data of all dids in the did set corresponding to this freeze frame
    /// @param[in] snapshotRecordContent Corresponding did set
    /// @param[out] record Obtained data is placed in this structure, return parameter
    /// @param[in] dtc The dtc value to which the freeze frame belongs
    /// @return Returns true on success, false on failure
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool CaptureSnapshotRecord(std::shared_ptr< DataIdentifierSet > const& snapshotRecordContent,
                               SnapshotDataRecordType& record,
                               std::uint32_t const dtc) const;

private:
    /// @brief Configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01814
    /// @needwork = dd
    /// @endcode
    FreezeFrameConfig config_{};
};
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_FREEZE_FRAME_H__