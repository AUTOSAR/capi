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
/// @file       datalinkfilter.h
/// @brief      Data link layer filter
/// @details    Data link layer firewall filtering
/// @date       2024-11-26
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Filter Mode
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0002
/// @unit_name=Filter_Datalink
/// @unit_description=Firewall data link layer filter.
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_FILTER_DATALINKFILTER_H_
#define ARA_FW_FILTER_DATALINKFILTER_H_

// Add header file includes
#include <ara/core/vector.h>

#include "ara/fw/filterengine/engine_datalink.h"
#include "ara/fw/internal/manifest_data.h"
#include "filterbase.h"

namespace ara {
namespace fw {
namespace internal {

/// @brief datalink filter.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00461
/// @trace_id_dd=DD_FW_00705
/// @needwork = ad
/// @endcode
class DataLinkFilter : public FilterBase
{
public:
    /// @brief Constructor
    /// @param vecDataLinkRules datalink rule set
    /// @param action Processing method
    /// @param inOutFlag input or output hook point
    /// @param pRate Processing method
    /// @param qLength Queue length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00462
    /// @trace_id_dd=DD_FW_00706
    /// @needwork = ad
    /// @endcode
    DataLinkFilter(ara::core::Vector< PRuleParse > vecDataLinkRules,
                   int32_t const &action,
                   int32_t const &inOutFlag,
                   int32_t pRate   = -1,
                   int32_t qLength = -1) noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00463
    /// @trace_id_dd=DD_FW_00707
    /// @needwork = ad
    /// @endcode
    ~DataLinkFilter() override = default;

    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00464
    /// @trace_id_dd=DD_FW_00708
    /// @needwork = ad
    /// @endcode
    DataLinkFilter(DataLinkFilter const &other) = delete;

    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00465
    /// @trace_id_dd=DD_FW_00709
    /// @needwork = ad
    /// @endcode
    DataLinkFilter(DataLinkFilter const &&other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return DataLinkFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00466
    /// @trace_id_dd=DD_FW_00710
    /// @needwork = ad
    /// @endcode
    DataLinkFilter &operator=(DataLinkFilter const &other) = delete;

    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return DataLinkFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00467
    /// @trace_id_dd=DD_FW_00711
    /// @needwork = ad
    /// @endcode
    DataLinkFilter &operator=(DataLinkFilter const &&other) = delete;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00468
    /// @trace_id_dd=DD_FW_00712
    /// @needwork = ad
    /// @endcode
    void RulesFilter() noexcept override;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00469
    /// @trace_id_dd=DD_FW_00713
    /// @needwork = ad
    /// @endcode
    void GetRules() noexcept override;

private:
    /// @brief datalink rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00714
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > dataLinkRules_{};
    /// @brief Destination address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00715
    /// @needwork = dda
    /// @endcode
    ara::core::String destMacAddress_{};
    /// @brief Destination address mask.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00716
    /// @needwork = dda
    /// @endcode
    ara::core::String destMacAddressMask_{};
    /// @brief Data link type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00717
    /// @needwork = dda
    /// @endcode
    int32_t etherType_{-1};
    /// @brief Source address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00718
    /// @needwork = dda
    /// @endcode
    ara::core::String srcMacAddress_{};
    /// @brief Source address mask
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00719
    /// @needwork = dda
    /// @endcode
    ara::core::String srcAddressMask_{};
    /// @brief vlanid.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00720
    /// @needwork = dda
    /// @endcode
    int32_t vlanId_{-1};
    /// @brief vlan priority
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00721
    /// @needwork = dda
    /// @endcode
    int32_t vlanPriority_{-1};
    /// @brief datalink engine ipv4 engine.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00722
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< DataLinkEngine > unpDataLinkEngine_{};
};
}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif