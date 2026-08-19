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
/// @file       ipv4filter.h
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
/// @unit_name=Filter_Ipv4
/// @unit_description=Firewall ipv4 filter
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_FILTER_IPV4FILTER_H_
#define ARA_FW_FILTER_IPV4FILTER_H_

// Add header file includes
#include <ara/core/vector.h>

#include "ara/fw/filterengine/engine_icmp.h"
#include "ara/fw/filterengine/engine_ipv4.h"
#include "ara/fw/internal/manifest_data.h"
#include "filterbase.h"

namespace ara {
namespace fw {
namespace internal {
/// @brief ipv4 filter.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00479
/// @trace_id_dd=DD_FW_00742
/// @needwork = ad
/// @endcode
class Ipv4Filter : public FilterBase

{
public:
    /// @brief Constructor
    /// @param vecIpv4Rules datalink rule set
    /// @param action Processing method
    /// @param inOutFlag input or output hook point
    /// @param pRate Processing method
    /// @param qLength Queue length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00480
    /// @trace_id_dd=DD_FW_00743
    /// @needwork = ad
    /// @endcode
    Ipv4Filter(ara::core::Vector< PRuleParse > vecIpv4Rules,
               int32_t const &action,
               int32_t const &inOutFlag,
               int32_t pRate   = -1,
               int32_t qLength = -1) noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00481
    /// @trace_id_dd=DD_FW_00744
    /// @needwork = ad
    /// @endcode
    ~Ipv4Filter() override = default;

    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00482
    /// @trace_id_dd=DD_FW_00745
    /// @needwork = ad
    /// @endcode
    Ipv4Filter(Ipv4Filter const &other) = delete;

    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00483
    /// @trace_id_dd=DD_FW_00746
    /// @needwork = ad
    /// @endcode
    Ipv4Filter(Ipv4Filter const &&other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return Ipv4Filter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00484
    /// @trace_id_dd=DD_FW_00747
    /// @needwork = ad
    /// @endcode
    Ipv4Filter &operator=(Ipv4Filter const &other) = delete;

    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return Ipv4Filter&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00485
    /// @trace_id_dd=DD_FW_00748
    /// @needwork = ad
    /// @endcode
    Ipv4Filter &operator=(Ipv4Filter const &&other) = delete;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00486
    /// @trace_id_dd=DD_FW_00749
    /// @needwork = ad
    /// @endcode
    void RulesFilter() noexcept override;

    /// @brief Rule filtering
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00487
    /// @trace_id_dd=DD_FW_00750
    /// @needwork = ad
    /// @endcode
    void GetRules() noexcept override;

private:
    /// @brief ipv4 rule set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00751
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PRuleParse > vecIpv4Rules_{};
    /// @brief Checksum
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00752
    /// @needwork = dda
    /// @endcode
    bool checksumVerification_{true};
    /// @brief Destination address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00753
    /// @needwork = dda
    /// @endcode
    ara::core::String destMacAddress_{};
    /// @brief Destination address Mask
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00754
    /// @needwork = dda
    /// @endcode
    ara::core::String destMacAddressMask_{};
    /// @brief dscp
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00755
    /// @needwork = dda
    /// @endcode
    int32_t differentiatedServiceCodePoint_{-1};
    /// @brief DF Don't Fragment
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00756
    /// @needwork = dda
    /// @endcode
    bool doNotFragment_{false};
    /// @brief ecn value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00757
    /// @needwork = dda
    /// @endcode
    int32_t explicitCongestionNotification_{-1};
    /// @brief icmp checksum
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00758
    /// @needwork = dda
    /// @endcode
    bool icmpCheckSum_{true};
    /// @brief icmp code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00759
    /// @needwork = dda
    /// @endcode
    int32_t icmpCode_{-1};
    /// @brief icmp type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00760
    /// @needwork = dda
    /// @endcode
    int32_t icmpType_{-1};
    /// @brief headerLength value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00761
    /// @needwork = dda
    /// @endcode
    int32_t internetHeaderLength_{-1};
    /// @brief MF fragmentation
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00762
    /// @needwork = dda
    /// @endcode
    bool moreFragments_{false};
    /// @brief Protocol value tcp udp icmp..
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00763
    /// @needwork = dda
    /// @endcode
    int32_t protocol_{-1};
    /// @brief Source address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00764
    /// @needwork = dda
    /// @endcode
    ara::core::String srcIpAddress_{};
    /// @brief Source address mask
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00765
    /// @needwork = dda
    /// @endcode
    ara::core::String srcNetworkMask_{};
    /// @brief ttl max
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00766
    /// @needwork = dda
    /// @endcode
    int32_t ttlMax_{-1};
    /// @brief ttl min
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00767
    /// @needwork = dda
    /// @endcode
    int32_t ttlMin_{-1};

private:
    /// @brief ipv4 engine ipv4 engine
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00768
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< Ipv4Engine > unpIpv4Engine_{};
    /// @brief ipv4 icmp engine
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00769
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< IcmpEngine > unpIcmpEngine_{};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif