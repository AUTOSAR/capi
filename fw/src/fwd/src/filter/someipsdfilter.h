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
/// @file       someipsdfilter.h
/// @brief      someipsdfilter filter
/// @details    someipsdfilter filter
/// @date       2025-10-15
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
/// @unit_name=Filter_SomeipSd
/// @unit_description=Firewall someipSd filter
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_FILTER_SOMEIPSDFILTER_H_
#define ARA_FW_FILTER_SOMEIPSDFILTER_H_

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
/// @trace_id_ad=AD_FW_00434
/// @trace_id_dd=DD_FW_00655
/// @needwork = ad
/// @endcode
class SomeIpSdFilter : public FilterBase
{
public:
    /// @brief SomeIpSdFilter constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00435
    /// @trace_id_dd=DD_FW_00656
    /// @needwork = ad
    /// @endcode
    SomeIpSdFilter() noexcept = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00436
    /// @trace_id_dd=DD_FW_00657
    /// @needwork = ad
    /// @endcode
    ~SomeIpSdFilter() override = default;

    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00437
    /// @trace_id_dd=DD_FW_00658
    /// @needwork = ad
    /// @endcode
    SomeIpSdFilter(SomeIpSdFilter const &other) = delete;

    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00438
    /// @trace_id_dd=DD_FW_00659
    /// @needwork = ad
    /// @endcode
    SomeIpSdFilter(SomeIpSdFilter const &&other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return SomeIpSdFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00439
    /// @trace_id_dd=DD_FW_00660
    /// @needwork = ad
    /// @endcode
    SomeIpSdFilter &operator=(SomeIpSdFilter const &other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return SomeIpSdFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00440
    /// @trace_id_dd=DD_FW_00661
    /// @needwork = ad
    /// @endcode
    SomeIpSdFilter &operator=(SomeIpSdFilter const &&other) = delete;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00441
    /// @trace_id_dd=DD_FW_00662
    /// @needwork = ad
    /// @endcode
    void RulesFilter() noexcept override;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00442
    /// @trace_id_dd=DD_FW_00663
    /// @needwork = ad
    /// @endcode
    void GetRules() noexcept override;

private:
    /// @brief tcp filter rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00664
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecSomeipRules_{};
    /// @brief Entry type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00665
    /// @needwork = dda
    /// @endcode
    int32_t entryType_{-1};
    /// @brief event group ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00666
    /// @needwork = dda
    /// @endcode
    int32_t eventGroupId_{-1};
    /// @brief Maximum major version number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00667
    /// @needwork = dda
    /// @endcode
    int32_t maxMajorVersion_{-1};
    /// @brief Maximum Minor version number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00668
    /// @needwork = dda
    /// @endcode
    int32_t maxMinorVersion_{-1};
    /// @brief Minimum major version number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00669
    /// @needwork = dda
    /// @endcode
    int32_t minMajorVersion_{-1};
    /// @brief Minimum Minor version number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00670
    /// @needwork = dda
    /// @endcode
    int32_t minMinorVersion_{-1};
    /// @brief Service instance ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00671
    /// @needwork = dda
    /// @endcode
    int32_t serviceInstanceId_{-1};
    /// @brief Service interface ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00672
    /// @needwork = dda
    /// @endcode
    int32_t serviceInterfaceId_{-1};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif