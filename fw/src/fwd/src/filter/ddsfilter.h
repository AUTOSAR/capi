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
/// @file       ddsfilter.h
/// @brief      dds filter
/// @details    dds firewall filtering
/// @date       2025-08-27
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Filter Mode
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0005
/// @unit_name=Filter_Dds
/// @unit_description=Firewall dds filter.
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_FILTER_DDSFILTER_H_
#define ARA_FW_FILTER_DDSFILTER_H_

// Add header file includes
#include <ara/core/vector.h>

#include "ara/fw/internal/manifest_data.h"
#include "filterbase.h"

namespace ara {
namespace fw {
namespace internal {

/// @brief ipv4 filter.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00497
/// @trace_id_dd=DD_FW_00794
/// @needwork = ad
/// @endcode
class DdsFilter : public FilterBase
{
public:
    /// @brief DdsFilter constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00498
    /// @trace_id_dd=DD_FW_00795
    /// @needwork = ad
    /// @endcode
    DdsFilter() noexcept = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00499
    /// @trace_id_dd=DD_FW_00796
    /// @needwork = ad
    /// @endcode
    ~DdsFilter() noexcept override = default;

    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00500
    /// @trace_id_dd=DD_FW_00797
    /// @needwork = ad
    /// @endcode
    DdsFilter(DdsFilter const &other) = delete;

    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00501
    /// @trace_id_dd=DD_FW_00798
    /// @needwork = ad
    /// @endcode
    DdsFilter(DdsFilter const &&other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return DdsFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00502
    /// @trace_id_dd=DD_FW_00799
    /// @needwork = ad
    /// @endcode
    DdsFilter &operator=(DdsFilter const &other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return DdsFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00503
    /// @trace_id_dd=DD_FW_00800
    /// @needwork = ad
    /// @endcode
    DdsFilter &operator=(DdsFilter const &&other) = delete;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00504
    /// @trace_id_dd=DD_FW_00801
    /// @needwork = ad
    /// @endcode
    void RulesFilter() noexcept override;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00505
    /// @trace_id_dd=DD_FW_00802
    /// @needwork = ad
    /// @endcode
    void GetRules() noexcept override;

private:
    /// @brief tcp filter rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00803
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecSomeipRules_{};
    /// @brief Process ID of the application on the host
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00804
    /// @needwork = dda
    /// @endcode
    int32_t appId_{-1};
    /// @brief Host ID where the participant runs
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00805
    /// @needwork = dda
    /// @endcode
    int32_t hostId_{-1};
    /// @brief ID of the DDS-RTPS running instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00806
    /// @needwork = dda
    /// @endcode
    int32_t instanceId_{-1};
    /// @brief Major version number of RTPS
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00807
    /// @needwork = dda
    /// @endcode
    int32_t majorProtocolVersion_{-1};
    /// @brief Minor version number of RTPS
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00808
    /// @needwork = dda
    /// @endcode
    int32_t minorProtocolVersion_{-1};
    /// @brief Protocol ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00809
    /// @needwork = dda
    /// @endcode
    int32_t protocolId_{-1};
    /// @brief DataReader ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00810
    /// @needwork = dda
    /// @endcode
    int32_t readerEntityId_{-1};
    /// @brief Sub-message type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00811
    /// @needwork = dda
    /// @endcode
    int32_t submessageType_{-1};
    /// @brief Uniquely identifies the DDS/RTPS implementation vendor that
    /// generated the message
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00812
    /// @needwork = dda
    /// @endcode
    int32_t vendorId_{-1};
    /// @brief Datawriter ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00813
    /// @needwork = dda
    /// @endcode
    int32_t writerEntityId_{-1};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif