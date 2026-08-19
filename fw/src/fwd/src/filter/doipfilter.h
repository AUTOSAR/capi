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
/// @file       doipfilter.h
/// @brief      doip filter
/// @details    doip filter
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
/// @unit_name=Filter_Doip
/// @unit_description=Firewall doip filter.
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_FILTER_DOIPFILTER_H_
#define ARA_FW_FILTER_DOIPFILTER_H_

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
/// @trace_id_ad=AD_FW_00452
/// @trace_id_dd=DD_FW_00686
/// @needwork = ad
/// @endcode
class DoipFilter : public FilterBase
{
public:
    /// @brief DoipFilter constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00453
    /// @trace_id_dd=DD_FW_00687
    /// @needwork = ad
    /// @endcode
    DoipFilter() noexcept = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00454
    /// @trace_id_dd=DD_FW_00688
    /// @needwork = ad
    /// @endcode
    ~DoipFilter() override = default;

    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00455
    /// @trace_id_dd=DD_FW_00689
    /// @needwork = ad
    /// @endcode
    DoipFilter(DoipFilter const &other) = delete;

    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00456
    /// @trace_id_dd=DD_FW_00690
    /// @needwork = ad
    /// @endcode
    DoipFilter(DoipFilter const &&other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return DoipFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00457
    /// @trace_id_dd=DD_FW_00691
    /// @needwork = ad
    /// @endcode
    DoipFilter &operator=(DoipFilter const &other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return DoipFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00458
    /// @trace_id_dd=DD_FW_00692
    /// @needwork = ad
    /// @endcode
    DoipFilter &operator=(DoipFilter const &&other) = delete;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00459
    /// @trace_id_dd=DD_FW_00693
    /// @needwork = ad
    /// @endcode
    void RulesFilter() noexcept override;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00460
    /// @trace_id_dd=DD_FW_00694
    /// @needwork = ad
    /// @endcode
    void GetRules() noexcept override;

private:
    /// @brief tcp filter rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00695
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecSomeipRules_{};
    /// @brief Maximum destination address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00696
    /// @needwork = dda
    /// @endcode
    int32_t destinationMaxAddress_{-1};
    /// @brief Minimum destination address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00697
    /// @needwork = dda
    /// @endcode
    int32_t destinationMinAddress_{-1};
    /// @brief Inverse version number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00698
    /// @needwork = dda
    /// @endcode
    int32_t inverseProtocolVersion_{-1};
    /// @brief Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00699
    /// @needwork = dda
    /// @endcode
    int32_t payloadLength_{-1};
    /// @brief Data type (diagnostic data | DoIP data..)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00700
    /// @needwork = dda
    /// @endcode
    int32_t payloadType_{-1};
    /// @brief Protocol version number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00701
    /// @needwork = dda
    /// @endcode
    int32_t protocolVersion_{-1};
    /// @brief Maximum source address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00702
    /// @needwork = dda
    /// @endcode
    int32_t sourceMaxAddress_{-1};
    /// @brief Minimum source address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00703
    /// @needwork = dda
    /// @endcode
    int32_t sourceMinAddress_{-1};
    /// @brief Specific UDS service ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00704
    /// @needwork = dda
    /// @endcode
    int32_t udsService_{-1};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif