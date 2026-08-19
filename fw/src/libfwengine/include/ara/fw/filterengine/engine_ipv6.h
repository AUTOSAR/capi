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
/// @file       engine_ipv6.h
/// @brief      ipv6 Rule
/// @details    ipv6 Rule input
/// @date       2025-06-04
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
/// @unit_name=Engine_Ipv6
/// @unit_description=Firewall engine abstraction layer ipv6 data abstraction
/// processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef _ARA_FW_ENGINE_IPV6_H_
#define _ARA_FW_ENGINE_IPV6_H_
#include <ara/core/string.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
namespace ara {
namespace fw {
namespace internal {

/// @brief for ipv6 mask
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00920
/// @needwork = dda
/// @endcode
struct IPv6 final
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00951
    /// @needwork = dda
    /// @endcode
    explicit IPv6() = default;
    /// @brief Parameterized constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00921
    /// @needwork = dda
    /// @endcode
    IPv6(uint64_t h, uint64_t l) : high(h), low(l) {}

    /// @brief Overloaded bitwise AND operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00922
    /// @needwork = dda
    /// @endcode
    IPv6 operator&(IPv6 const &other) const { return IPv6(high & other.high, low & other.low); }

    /// @brief Overloaded bitwise OR operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00923
    /// @needwork = dda
    /// @endcode
    IPv6 operator|(IPv6 const &other) const { return IPv6(high | other.high, low | other.low); }

    /// @brief Overloaded bitwise NOT operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00924
    /// @needwork = dda
    /// @endcode
    IPv6 operator~() const { return IPv6(~high, ~low); }

    /// @brief Overloaded equal to operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00925
    /// @needwork = dda
    /// @endcode
    bool operator==(IPv6 const &other) const { return (high == other.high) && (low == other.low); }

    /// @brief Overloaded less than operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00926
    /// @needwork = dda
    /// @endcode
    bool operator<(IPv6 const &other) const
    {
        if (high < other.high) {
            return true;
        }

        if (high > other.high) {
            return false;
        }
        return low < other.low;
    }

    /// @brief Overloaded less than or equal to operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00927
    /// @needwork = dda
    /// @endcode
    bool operator<=(IPv6 const &other) const { return *this < other || *this == other; }

    /// @brief IPv6 address increment by 1
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00928
    /// @needwork = dda
    /// @endcode
    IPv6 operator+(uint64_t value) const
    {
        uint64_t newLow  = low + value;
        uint64_t newHigh = high;
        if (newLow < low) {  // Handle lower 64-bit overflow
            newHigh += 1;
        }
        return IPv6(newHigh, newLow);
    }

    /// @brief IPv6 address decrement by 1
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00929
    /// @needwork = dda
    /// @endcode
    IPv6 operator-(uint64_t value) const
    {
        uint64_t newLow  = low - value;
        uint64_t newHigh = high;
        if (newLow > low) {  // Handle lower 64-bit borrow
            newHigh -= 1;
        }
        return IPv6(newHigh, newLow);
    }

    /// @brief Error check
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00930
    /// @needwork = dda
    /// @endcode
    bool CheckVaild() const noexcept
    {
        if (this->high == 0 && this->low == 0) {
            return false;
        }
        return true;
    }

public:
    /// @brief High bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00931
    /// @needwork = dda
    /// @endcode
    uint64_t high{0};  // Upper 64 bits
    /// @brief Low bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00932
    /// @needwork = dda
    /// @endcode
    uint64_t low{0};  // Lower 64 bits
};

/// @brief IPv6 filter engine processing class.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00045
/// @trace_id_dd=DD_FW_00075
/// @needwork = ad
/// @endcode
class Ipv6Engine final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00046
    /// @trace_id_dd=DD_FW_00076
    /// @needwork = ad
    /// @endcode
    explicit Ipv6Engine(int32_t action, int32_t inOutFlag, int32_t pRate = -1, int32_t qLength = -1) noexcept;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00047
    /// @trace_id_dd=DD_FW_00077
    /// @needwork = ad
    /// @endcode
    ~Ipv6Engine() = default;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00048
    /// @trace_id_dd=DD_FW_00078
    /// @needwork = ad
    /// @endcode
    Ipv6Engine &operator=(Ipv6Engine const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00049
    /// @trace_id_dd=DD_FW_00079
    /// @needwork = ad
    /// @endcode
    Ipv6Engine &operator=(Ipv6Engine &&other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00050
    /// @trace_id_dd=DD_FW_00080
    /// @needwork = ad
    /// @endcode
    Ipv6Engine(Ipv6Engine &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00051
    /// @trace_id_dd=DD_FW_00081
    /// @needwork = ad
    /// @endcode
    Ipv6Engine(Ipv6Engine const &other) noexcept = delete;
    /// @brief Set address filtering command (hook point is input.)
    /// @param iProtocol
    /// @param strProtocol

public:
    /// @brief Set flowlabel rule filtering
    /// @param flowlabel flowlabel filter value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00052
    /// @trace_id_dd=DD_FW_00082
    /// @needwork = ad
    /// @endcode
    void SetFlowLabelFilter(int32_t flowlabel) const noexcept;

    /// @brief Set hopLimit rule filtering
    /// @param hopLimit User set hopLimit value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00053
    /// @trace_id_dd=DD_FW_00083
    /// @needwork = ad
    /// @endcode
    void SetHopLimitFilter(int32_t hopLimit) const noexcept;

    /// @brief Set next header
    /// @param hdrLength User set ipv6 nextheader field value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00054
    /// @trace_id_dd=DD_FW_00084
    /// @needwork = ad
    /// @endcode
    void SetNextHeaderFilter(int32_t nexthdr) const noexcept;

    /// @brief Set DSCP rule filtering
    /// @param dscp DSCP filter value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00055
    /// @trace_id_dd=DD_FW_00085
    /// @needwork = ad
    /// @endcode
    void SetIp6DscpFilter(uint8_t dscp) const noexcept;

    /// @brief Set ECN rule filtering
    /// @param ecn User set ECN value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00056
    /// @trace_id_dd=DD_FW_00086
    /// @needwork = ad
    /// @endcode
    void SetIp6ECNFilter(uint8_t ecn) const noexcept;

    /// @brief Set source address and destination address rule filtering
    /// @param srcAddress Source address
    /// @param srcAddrMask Source address
    /// @param srcAddress Source address
    /// @param destAddress Destination address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00057
    /// @trace_id_dd=DD_FW_00087
    /// @needwork = ad
    /// @endcode
    void SetIp6AddressFilter(ara::core::String const &srcAddr,
                             ara::core::String const &srcAddrMask,
                             ara::core::String const &destAddr,
                             ara::core::String const &destAddrMask) noexcept;

private:
    /// @brief parse ipv6 subnet mask configuration.
    /// @param ipv6Str
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00088
    /// @needwork = dda
    /// @endcode
    IPv6 _parseIPv6(ara::core::String const &ipv6Str) noexcept;

    /// @brief Format IPv6 address to string
    /// @param addr
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00089
    /// @needwork = dda
    /// @endcode
    std::string _formatIPv6(const IPv6 &addr) noexcept;

    /// @brief Calculate Ipv6 ip host range. (applicable to both src and dest)
    /// @param ipv6Str
    /// @param ipv6StrMask
    /// @return ip range string.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00090
    /// @needwork = dda
    /// @endcode
    ara::core::String _calIp6Range(ara::core::String const &ipv6Str, ara::core::String const &ipv6StrMask) noexcept;

private:
    /// @brief Local filter rule action.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00091
    /// @needwork = dda
    /// @endcode
    int32_t action_{-1};
    /// @brief IN/OUT Flag.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00092
    /// @needwork = dda
    /// @endcode
    int32_t inOutFlag_{-1};
    /// @brief Rate
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00093
    /// @needwork = dda
    /// @endcode
    int32_t packetRate_{-1};
    /// @brief Queue length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00094
    /// @needwork = dda
    /// @endcode
    int32_t queueLength_{-1};
    /// @brief log prefix identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00095
    /// @needwork = dda
    /// @endcode
    ara::core::String logPrefix_{"Firewall-Drop"};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif