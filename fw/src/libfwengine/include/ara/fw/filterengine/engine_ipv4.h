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
/// @file       engine_ipv4.h
/// @brief      ipv4 Rule
/// @details    ipv4 Rule input
/// @date       2025-05-23
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Firewall Engine Abstraction Layer
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0010
/// @unit_name=Engine_Ipv4
/// @unit_description=Firewall engine abstraction layer ipv4 data abstraction
/// processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef _ARA_FW_ENGINE_IPV4_H_
#define _ARA_FW_ENGINE_IPV4_H_
#include <ara/core/string.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
namespace ara {
namespace fw {
namespace internal {

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00004
/// @trace_id_dd=DD_FW_00004
/// @needwork = ad
/// @endcode
class Ipv4Engine final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00005
    /// @trace_id_dd=DD_FW_00005
    /// @needwork = ad
    /// @endcode
    explicit Ipv4Engine(int32_t action, int32_t inOutFlag, int32_t pRate = -1, int32_t qLength = -1) noexcept;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00006
    /// @trace_id_dd=DD_FW_00006
    /// @needwork = ad
    /// @endcode
    ~Ipv4Engine() = default;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00007
    /// @trace_id_dd=DD_FW_00007
    /// @needwork = ad
    /// @endcode
    Ipv4Engine &operator=(Ipv4Engine const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00008
    /// @trace_id_dd=DD_FW_00008
    /// @needwork = ad
    /// @endcode
    Ipv4Engine &operator=(Ipv4Engine &&other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00009
    /// @trace_id_dd=DD_FW_00009
    /// @needwork = ad
    /// @endcode
    Ipv4Engine(Ipv4Engine &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00010
    /// @trace_id_dd=DD_FW_00010
    /// @needwork = ad
    /// @endcode
    Ipv4Engine(Ipv4Engine const &other) noexcept = delete;
    /// @brief Set address filtering command (hook point is input.)
    /// @param iProtocol
    /// @param strProtocol

public:
    /// @brief Set address filtering command (hook point is input.)
    /// @param iProtocol
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00011
    /// @trace_id_dd=DD_FW_00011
    /// @needwork = ad
    /// @endcode
    void SetProtocolFilter(int32_t iProtocol) const noexcept;

    /// @brief Set source address and destination address rule filtering
    /// @param srcAddress Source address
    /// @param srcAddrMask Source address
    /// @param srcAddress Source address
    /// @param destAddress Destination address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00012
    /// @trace_id_dd=DD_FW_00012
    /// @needwork = ad
    /// @endcode
    void SetAddressFilter(ara::core::String const &srcAddr,
                          ara::core::String const &srcAddrMask,
                          ara::core::String const &destAddr,
                          ara::core::String const &destAddrMask) const noexcept;

    /// @brief Set TTL rule filtering
    /// @param ttlMin ttl minimum time
    /// @param ttlMax ttl maximum time
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00013
    /// @trace_id_dd=DD_FW_00013
    /// @needwork = ad
    /// @endcode
    void SetTtlFilter(int32_t ttlMin, int32_t ttlMax) const noexcept;

    /// @brief Set DSCP rule filtering
    /// @param dscp DSCP filter value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00014
    /// @trace_id_dd=DD_FW_00014
    /// @needwork = ad
    /// @endcode
    void SetDscpFilter(int32_t dscp) const noexcept;

    /// @brief Set IP fragmentation rule filtering
    /// @param dfFlag Fragmentation flag
    /// @param mfFlag More fragments flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00015
    /// @trace_id_dd=DD_FW_00015
    /// @needwork = ad
    /// @endcode
    void SetDFOrMFFilter(bool dfFlag, bool mfFlag) const noexcept;

    /// @brief Set ECN rule filtering
    /// @param ecn User set ECN value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00016
    /// @trace_id_dd=DD_FW_00016
    /// @needwork = ad
    /// @endcode
    void SetECNFilter(int32_t ecn) const noexcept;

    /// @brief Set minimum IP header filtering
    /// @param hdrLength User set ipv4 header minimum value.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00017
    /// @trace_id_dd=DD_FW_00017
    /// @needwork = ad
    /// @endcode
    void SetHdrLengthFilter(int32_t hdrLength) const noexcept;

private:
    /// @brief Get the corresponding IP range of the firewall through IP and
    /// subnet mask. (ipv4)
    /// @param ip
    /// @param ipMask
    /// @return Get the corresponding ip range.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00018
    /// @needwork = dda
    /// @endcode
    ara::core::String _getIpRange(ara::core::String const &ip, ara::core::String const &ipMask) const noexcept;

private:
    /// @brief Local filter rule action.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00019
    /// @needwork = dda
    /// @endcode
    int32_t action_{-1};
    /// @brief IN/OUT Flag.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00020
    /// @needwork = dda
    /// @endcode
    int32_t inOutFlag_{-1};
    /// @brief Rate
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00021
    /// @needwork = dda
    /// @endcode
    int32_t packetRate_{-1};
    /// @brief Queue length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00022
    /// @needwork = dda
    /// @endcode
    int32_t queueLength_{-1};
    /// @brief log prefix identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00023
    /// @needwork = dda
    /// @endcode
    ara::core::String logPrefix_{"Firewall-Drop"};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif