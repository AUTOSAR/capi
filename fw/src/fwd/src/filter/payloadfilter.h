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
/// @file       payloadfilter.h
/// @brief      payload firewall filtering
/// @details    payload firewall filtering
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
/// @unit_name=Filter_Payload
/// @unit_description=Firewall payload filter
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_FILTER_PAYLOADFILTER_H_
#define ARA_FW_FILTER_PAYLOADFILTER_H_

// Add header file includes
#include <ara/core/vector.h>

#include "ara/fw/filterengine/engine_payload.h"
#include "ara/fw/internal/manifest_data.h"
#include "filterbase.h"
namespace ara {
namespace fw {
namespace internal {

/// @brief tcp filter.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00443
/// @trace_id_dd=DD_FW_00673
/// @needwork = ad
/// @endcode
class PayloadFilter : public FilterBase
{
public:
    /// @brief PayloadFilter constructor
    /// @param vecPayloadRules tcp rule validation set
    /// @param action action
    /// @param inOutFlag hook point in/out
    /// @param pRate Processing method
    /// @param qLength Queue length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00444
    /// @trace_id_dd=DD_FW_00674
    /// @needwork = ad
    /// @endcode
    PayloadFilter(ara::core::Vector< PRuleParse > vecPayloadRules,
                  int32_t const &action,
                  int32_t const &inOutFlag,
                  int32_t pRate   = -1,
                  int32_t qLength = -1) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00445
    /// @trace_id_dd=DD_FW_00675
    /// @needwork = ad
    /// @endcode
    ~PayloadFilter() override = default;

    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00446
    /// @trace_id_dd=DD_FW_00676
    /// @needwork = ad
    /// @endcode
    PayloadFilter(PayloadFilter const &other) = delete;

    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00447
    /// @trace_id_dd=DD_FW_00677
    /// @needwork = ad
    /// @endcode
    PayloadFilter(PayloadFilter const &&other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return PayloadFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00448
    /// @trace_id_dd=DD_FW_00678
    /// @needwork = ad
    /// @endcode
    PayloadFilter &operator=(PayloadFilter const &other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return PayloadFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00449
    /// @trace_id_dd=DD_FW_00679
    /// @needwork = ad
    /// @endcode
    PayloadFilter &operator=(PayloadFilter const &&other) = delete;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00450
    /// @trace_id_dd=DD_FW_00680
    /// @needwork = ad
    /// @endcode
    void RulesFilter() noexcept override;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00451
    /// @trace_id_dd=DD_FW_00681
    /// @needwork = ad
    /// @endcode
    void GetRules() noexcept override;

private:
    /// @brief tcp filter rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00682
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecPayloadRules_{};
    /// @brief Offset
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00683
    /// @needwork = dda
    /// @endcode
    int32_t offset_{-1};
    /// @brief Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00684
    /// @needwork = dda
    /// @endcode
    int32_t value_{-1};

    /// @brief udp engine udp engine
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00685
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< PayloadEngine > unpPayloadEngine_{};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif