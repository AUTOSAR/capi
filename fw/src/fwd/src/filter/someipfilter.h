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
/// @file       someipfilter.h
/// @brief      someipfilter filter
/// @details    someipfilter filter
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
/// @unit_name=Filter_Someip
/// @unit_description=Firewall someip filter
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_FILTER_SOMEIPFILTER_H_
#define ARA_FW_FILTER_SOMEIPFILTER_H_

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
/// @trace_id_ad=AD_FW_00425
/// @trace_id_dd=DD_FW_00637
/// @needwork = ad
/// @endcode
class SomeIpFilter : public FilterBase
{
public:
    /// @brief SomeIpFilter constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00426
    /// @trace_id_dd=DD_FW_00638
    /// @needwork = ad
    /// @endcode
    SomeIpFilter() noexcept = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00427
    /// @trace_id_dd=DD_FW_00639
    /// @needwork = ad
    /// @endcode
    ~SomeIpFilter() override = default;

    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00428
    /// @trace_id_dd=DD_FW_00640
    /// @needwork = ad
    /// @endcode
    SomeIpFilter(SomeIpFilter const &other) = delete;

    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00429
    /// @trace_id_dd=DD_FW_00641
    /// @needwork = ad
    /// @endcode
    SomeIpFilter(SomeIpFilter const &&other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return SomeIpFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00430
    /// @trace_id_dd=DD_FW_00642
    /// @needwork = ad
    /// @endcode
    SomeIpFilter &operator=(SomeIpFilter const &other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return SomeIpFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00431
    /// @trace_id_dd=DD_FW_00643
    /// @needwork = ad
    /// @endcode
    SomeIpFilter &operator=(SomeIpFilter const &&other) = delete;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00432
    /// @trace_id_dd=DD_FW_00644
    /// @needwork = ad
    /// @endcode
    void RulesFilter() noexcept override;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00433
    /// @trace_id_dd=DD_FW_00645
    /// @needwork = ad
    /// @endcode
    void GetRules() noexcept override;

private:
    /// @brief tcp filter rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00646
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecSomeipRules_{};
    /// @brief Client ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00647
    /// @needwork = dda
    /// @endcode
    int32_t clientId_{-1};
    /// @brief Whether to perform checksum
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00648
    /// @needwork = dda
    /// @endcode
    bool lengthVerification_{false};
    /// @brief Major version number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00649
    /// @needwork = dda
    /// @endcode
    int32_t majorVersion_{-1};
    /// @brief Minimum port value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00650
    /// @needwork = dda
    /// @endcode
    int32_t messageType_{-1};
    /// @brief Method ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00651
    /// @needwork = dda
    /// @endcode
    int32_t methodId_{-1};
    /// @brief Protocol version
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00652
    /// @needwork = dda
    /// @endcode
    int32_t protocolVersion_{-1};
    /// @brief Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00653
    /// @needwork = dda
    /// @endcode
    int32_t returnCode_{-1};
    /// @brief Service interface ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00654
    /// @needwork = dda
    /// @endcode
    int32_t serviceInterfaceId_{-1};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif