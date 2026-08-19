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
/// @file       ipv6filter.h
/// @brief      Data link layer filter
/// @details    Data link layer firewall filtering
/// @date       2025-01-21
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/filter
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0002
/// @unit_name=Filter_Ipv6
/// @unit_description=Firewall ipv6 filter
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_FILTER_IPV6FILTER_H_
#define ARA_FW_FILTER_IPV6FILTER_H_

// Add header file includes
#include <ara/core/vector.h>

#include "ara/fw/filterengine/engine_icmp.h"
#include "ara/fw/filterengine/engine_ipv6.h"
#include "ara/fw/internal/manifest_data.h"
#include "filterbase.h"
namespace ara {
namespace fw {
namespace internal {

/// @brief ipv4 filter.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00488
/// @trace_id_dd=DD_FW_00770
/// @needwork = ad
/// @endcode
class Ipv6Filter : public FilterBase
{
public:
    /// @brief Constructor
    /// @param vecIpv6Rules datalink rule set
    /// @param action Processing method
    /// @param inOutFlag input or output hook point
    /// @param pRate Processing method
    /// @param qLength Queue length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00489
    /// @trace_id_dd=DD_FW_00771
    /// @needwork = ad
    /// @endcode
    Ipv6Filter(ara::core::Vector< PRuleParse > vecIpv6Rules,
               int32_t const &action,
               int32_t const &inOutFlag,
               int32_t pRate   = -1,
               int32_t qLength = -1) noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00490
    /// @trace_id_dd=DD_FW_00772
    /// @needwork = ad
    /// @endcode
    ~Ipv6Filter() override = default;

    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00491
    /// @trace_id_dd=DD_FW_00773
    /// @needwork = ad
    /// @endcode
    Ipv6Filter(Ipv6Filter const &other) = delete;

    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00492
    /// @trace_id_dd=DD_FW_00774
    /// @needwork = ad
    /// @endcode
    Ipv6Filter(Ipv6Filter const &&other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return Ipv6Filter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00493
    /// @trace_id_dd=DD_FW_00775
    /// @needwork = ad
    /// @endcode
    Ipv6Filter &operator=(Ipv6Filter const &other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return Ipv6Filter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00494
    /// @trace_id_dd=DD_FW_00776
    /// @needwork = ad
    /// @endcode
    Ipv6Filter &operator=(Ipv6Filter const &&other) = delete;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00495
    /// @trace_id_dd=DD_FW_00777
    /// @needwork = ad
    /// @endcode
    void RulesFilter() noexcept override;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00496
    /// @trace_id_dd=DD_FW_00778
    /// @needwork = ad
    /// @endcode
    void GetRules() noexcept override;

private:
    /// @brief ipv6 rule validation set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00779
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecIpv6Rules_{};
    /// @brief Destination address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00780
    /// @needwork = dda
    /// @endcode
    ara::core::String destMacAddress_{};
    /// @brief Destination address mask
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00781
    /// @needwork = dda
    /// @endcode
    ara::core::String destMacAddressMask_{};
    /// @brief  flowlabel
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00782
    /// @needwork = dda
    /// @endcode
    int32_t flowLabel_{-1};
    /// @brief hoplimit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00783
    /// @needwork = dda
    /// @endcode
    int32_t hopLimit_{-1};
    /// @brief Checksum
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00784
    /// @needwork = dda
    /// @endcode
    bool icmpCheckSum_{true};
    /// @brief icmpv6 code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00785
    /// @needwork = dda
    /// @endcode
    int32_t icmpCode_{-1};
    /// @brief icmpv6 type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00786
    /// @needwork = dda
    /// @endcode
    int32_t icmpType_{-1};
    /// @brief next header
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00787
    /// @needwork = dda
    /// @endcode
    int32_t nextHeader_{-1};
    /// @brief Source address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00788
    /// @needwork = dda
    /// @endcode
    ara::core::String srcIpAddress_{};
    /// @brief Source address mask
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00789
    /// @needwork = dda
    /// @endcode
    ara::core::String srcNetworkMask_{};
    /// @brief traffic class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00790
    /// @needwork = dda
    /// @endcode
    int32_t trafficClass_{-1};

    /// @brief ipv6 engine ipv4 engine
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00791
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< Ipv6Engine > unpIpv6Engine_{};
    /// @brief ipv6 icmp engine
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00792
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< IcmpEngine > unpIcmpEngine_{};

private:
    /// @brief SetTraffiClassValue
    /// @param traffClass  ipv6 trafficlass.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00793
    /// @needwork = dda
    /// @endcode
    void _setTraffiClassValue(int32_t traffClass) noexcept;
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif