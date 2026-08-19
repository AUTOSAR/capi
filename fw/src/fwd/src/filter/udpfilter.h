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
/// @file       udpfilter.h
/// @brief      udpfilter filter
/// @details    udpfilter filter
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
/// @trace_id_sr=SR_FW_0002
/// @unit_name=Filter_Udp
/// @unit_description=Firewall Udp filter
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_FILTER_UDPFILTER_H_
#define ARA_FW_FILTER_UDPFILTER_H_

// Add header file includes
#include <ara/core/vector.h>

#include "ara/fw/filterengine/engine_udp.h"
#include "ara/fw/internal/manifest_data.h"
#include "filterbase.h"

namespace ara {
namespace fw {
namespace internal {

/// @brief ipv4 filter.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00515
/// @trace_id_dd=DD_FW_00823
/// @needwork = ad
/// @endcode
class UdpFilter : public FilterBase
{
public:
    /// @brief UdpFilter constructor
    /// @param vecUdpRules udp rule validation set
    /// @param action action
    /// @param inOutFlag hook point in/out
    /// @param pRate Processing method
    /// @param qLength Queue length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00516
    /// @trace_id_dd=DD_FW_00824
    /// @needwork = ad
    /// @endcode
    UdpFilter(ara::core::Vector< PRuleParse > vecUdpRules,
              int32_t const &action,
              int32_t const &inOutFlag,
              int32_t pRate   = -1,
              int32_t qLength = -1) noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00517
    /// @trace_id_dd=DD_FW_00825
    /// @needwork = ad
    /// @endcode
    ~UdpFilter() override = default;

    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00518
    /// @trace_id_dd=DD_FW_00826
    /// @needwork = ad
    /// @endcode
    UdpFilter(UdpFilter const &other) = delete;

    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00519
    /// @trace_id_dd=DD_FW_00827
    /// @needwork = ad
    /// @endcode
    UdpFilter(UdpFilter const &&other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return UdpFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00520
    /// @trace_id_dd=DD_FW_00828
    /// @needwork = ad
    /// @endcode
    UdpFilter &operator=(UdpFilter const &other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return UdpFilter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00521
    /// @trace_id_dd=DD_FW_00829
    /// @needwork = ad
    /// @endcode
    UdpFilter &operator=(UdpFilter const &&other) = delete;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00522
    /// @trace_id_dd=DD_FW_00830
    /// @needwork = ad
    /// @endcode
    void RulesFilter() noexcept override;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00523
    /// @trace_id_dd=DD_FW_00831
    /// @needwork = ad
    /// @endcode
    void GetRules() noexcept override;

private:
    /// @brief tcp filter rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00832
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecUdpRules_{};
    /// @brief Checksum
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00833
    /// @needwork = dda
    /// @endcode
    bool checksumVerification_{true};
    /// @brief Maximum port value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00834
    /// @needwork = dda
    /// @endcode
    int32_t maxDestPortNum_{-1};
    /// @brief Minimum port value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00835
    /// @needwork = dda
    /// @endcode
    int32_t minDestPortNum_{-1};
    /// @brief Source address maximum port value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00836
    /// @needwork = dda
    /// @endcode
    int32_t maxSrcPortNum_{-1};
    /// @brief Source address minimum port value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00837
    /// @needwork = dda
    /// @endcode
    int32_t minSrcPortNum_{-1};

    /// @brief udp engine udp engine
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00838
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< UdpEngine > unpUdpEngine_{};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif