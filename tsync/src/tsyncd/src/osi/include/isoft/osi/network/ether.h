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
/// @file       ether.h
/// @brief      OSI network module Ether class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_NETWORK_ETHER_H_
#define ISOFT_OSI_NETWORK_ETHER_H_

#include <arpa/inet.h>
#include <strings.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

#include "ara/tsync/internal/common.h"

namespace isoft {
namespace osi {
namespace network {

/// Keep 1-byte alignment
#pragma pack(push, 1)
/// @brief Ethernet frame class definition
class Ether final
{
public:
    //static constexpr std::uint16_t kPTPType = 0x88F7   // PTP frame type
    //static constexpr std::uint16_t kVlanType = 0x8100  // Vlan frame type

    /// @brief Ethernet frame type definition
    enum class Type : std::uint16_t
    {
        kPtp    = 0x88F7,  /// PTP frame type, network byte order
        kVlan   = 0x8100,  /// Vlan frame type, network byte order
        kNoType = 0,       /// no type, default initial state
    };

    /// @brief Ethernet frame address class definition
    class Address final
    {
    public:
        /// @name kLength
        static constexpr std::size_t kLength{6U};

        /// @brief default constructor
        Address() = default;

        /// @brief import from string
        /// @param addrStr - MAC address string
        /// @return 0 - success
        /// @return <0 - failure
        std::int32_t FromStr(std::string const &addrStr) noexcept
        {
            std::size_t i{0U};
            std::array< std::uint32_t, ara::tsync::internal::kTS_NUM_INT_6 > byte{};
            // if (6 != sscanf(str.c_str(), "%x:%x:%x:%x:%x:%x", byte+5, byte+4, byte+3, byte+2, byte+1, byte)).
            if (ara::tsync::internal::kTS_NUM_INT_6
                /// TODO(zhoubo): it is recommended to use strtoul instead of sscanf
                != sscanf(addrStr.c_str(), "%2x:%2x:%2x:%2x:%2x:%2x", byte.data(),  // NOLINT
                          byte.data() + ara::tsync::internal::kTS_NUM_1, byte.data() + ara::tsync::internal::kTS_NUM_2,
                          byte.data() + ara::tsync::internal::kTS_NUM_3, byte.data() + ara::tsync::internal::kTS_NUM_4,
                          byte.data() + ara::tsync::internal::kTS_NUM_5)) {
                return ara::tsync::internal::kRET_E1;
            }
            for (i = 0U; i < kLength; i++) {
                addrBytes[i] = static_cast< std::uint8_t >(byte[i]);  // NOLINT
            }
            return 0;
        }

        /// @brief export to string
        /// @param addrStr - buffer to store the exported MAC address string
        void ToStr(std::string &addrStr) const noexcept
        {
            addrStr.resize(kLength * ara::tsync::internal::kTS_NUM_3);
            for (std::size_t i{0U}; i < kLength; ++i) {
                std::ostringstream ss;
                ss << std::hex << std::setw(ara::tsync::internal::kTS_NUM_INT_2)
                   << std::setfill(ara::tsync::internal::kTS_FILL_0)
                   << static_cast< std::int32_t >(addrBytes[i]);  // NOLINT
                if (i > 0U) {
                    addrStr = addrStr + ":";
                }
                addrStr = addrStr + ss.str();
            }
        }

        /// @brief export MAC address to specified memory
        /// @param addr - memory address to store
        void CopyTo(std::uint8_t *const addr) const noexcept
        {
            const void *const src{static_cast< const void * >(addrBytes)};
            std::ignore = ::memmove(addr, src, kLength);
        }

        /// @brief operator overload, comparison function
        /// @param lhs - object reference
        /// @param rhs - object reference
        /// @return true - equal
        /// @return false - not equal
        friend bool operator==(Address const &lhs, Address const &rhs) noexcept
        {
            return (0
                    == ::memcmp(static_cast< std::uint8_t const * >(&lhs.addrBytes[0]),
                                static_cast< std::uint8_t const * >(&rhs.addrBytes[0]), kLength));
        }
        /// @brief MAC address stored in network byte order
        std::uint8_t addrBytes[kLength]{0U};
    };

    /// @brief Ethernet header structure definition
    struct Header final
    {
        /// @name dest - destination MAC address
        Address dest;
        /// @name src - source MAC address
        Address src;
        /// @name type - Ethernet frame type
        Type type{Type::kNoType};
    };

    /// VLAN format definition
    /////////////////////////////////////////////////////////////////////////////////////////////////
    ///   TPID  |     2Byte       |   Tag Protocol Identifier, indicates the data frame type.
    ///                           |   A value of 0x8100 indicates an IEEE 802.1Q VLAN data frame. Devices that do not support 802.1Q will discard such frames.
    ///                           |   Equipment vendors can customize this field. When a neighbor device configures the TPID value to a value other than 0x8100, in order to recognize such packets and achieve interoperability, the TPID value must be modified on the local device to match the neighbor device's TPID configuration.
    ///----------------------------------------------------------------------------------------------
    ///   PRI   |     3bit        |    Priority, indicates the 802.1p priority of the data frame.
    ///                           |    Range is 0～7, higher value means higher priority. When the network is congested, switches will send higher priority data frames first.
    ///----------------------------------------------------------------------------------------------
    ///   CFI   |     1bit        |    Canonical Format Indicator, indicates whether the MAC address is encapsulated in standard format in different transmission media, used for compatibility between Ethernet and Token Ring.
    ///                           |    CFI value 0 indicates the MAC address is encapsulated in standard format, 1 indicates non-standard format. In Ethernet, the value of CFI is 0.
    ///----------------------------------------------------------------------------------------------
    ///   VID   |     12bit       |    VLAN ID, indicates the VLAN number to which the data frame belongs.
    ///                           |    VLAN ID range is 0～4095. Since 0 and 4095 are reserved for protocol, the valid VLAN ID range is 1～4094.
    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief VLAN structure definition
    struct Vlan final
    {
        /// @name tpid - frame type, fixed to VLAN type
        Type tpid{Type::kNoType};
        /// @brief VLAN header structure definition
        struct VlanTag final
        {
            std::uint16_t vid : 12;
            std::uint16_t cfi : 1;
            std::uint16_t pri : 3;
        };
        /// @name pcv - VLAN content
        VlanTag pcv;
    };

    /// @brief Ethernet header structure definition including VLAN tag
    struct HeaderVlan final
    {
        /// @name dest - destination MAC address
        Address dest;
        /// @name src - source MAC address
        Address src;
        /// @name vlan - VLAN tag content
        Vlan vlan;
        /// @name type - Ethernet frame type
        Type type{Type::kNoType};
    };

    /// @brief constructor
    /// @param withVlanTag whether there is a vlan tag header
    /// @traceid{PRS_TS_00163} If FramePrio exists, use Ethernet frame containing VLAN
    explicit Ether(bool const withVlanTag) noexcept
    {
        ::bzero((&frame_), sizeof(frame_));
        if (withVlanTag) {
            SetVlan(0U, 0U);
        }
    }

    /// @brief destructor
    ~Ether() = default;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    Ether(Ether const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    Ether &operator=(Ether const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    Ether(Ether &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    Ether &operator=(Ether &&) &noexcept = default;

    /// @brief set destination MAC address
    /// @param dest - destination MAC address
    void SetDestAddress(Address const &dest) noexcept { frame_.withoutVlan.dest = dest; }

    /// @brief get destination MAC address
    /// @param dest - destination MAC address
    void GetDestAddress(Address &dest) const noexcept
    {
        std::ignore = dest;
        dest        = frame_.withoutVlan.dest;
    }

    /// @brief set source MAC address
    /// @param src - source MAC address
    void SetSourceAddress(Address const &src) noexcept { frame_.withoutVlan.src = src; }

    /// @brief get source MAC address
    /// @param src - source MAC address
    void GetSourceAddress(Address &src) const noexcept
    {
        std::ignore = src;
        src         = frame_.withoutVlan.src;
    }

    /// @brief set VLAN attributes
    /// @param pri - VLAN priority; range 0-7
    /// @param vlanid - VLAN ID
    void SetVlan(std::uint8_t const pri, std::uint16_t const vlanid) noexcept
    {
        frame_.withVlan.vlan.tpid = Type::kVlan;
        std::uint16_t const oriPri{pri};
        std::uint16_t const priBitAll{0b111U};
        frame_.withVlan.vlan.pcv.pri = static_cast< std::uint16_t >(priBitAll & oriPri);
        frame_.withVlan.vlan.pcv.vid = vlanid;
    }

    /// @brief get VLAN priority
    /// @return VLAN priority; -1 indicates no tag
    std::int8_t GetVlanPriority() const noexcept
    {
        if (!HasVlanTag()) {
            return ara::tsync::internal::kRET_E1;
        }
        return static_cast< std::int8_t >(frame_.withVlan.vlan.pcv.pri);
    }

    /// @brief check whether there is a VLAN tag
    /// @return whether
    bool HasVlanTag() const noexcept
    {
        if (Type::kVlan == frame_.withVlan.vlan.tpid) {
            return true;
        }
        return false;
    }

    /// @brief set Ethernet frame type
    /// @param type - frame type to set
    void SetType(Type const type) noexcept
    {
        /// If the current frame contains a vlan tag, set according to vlan format
        if (HasVlanTag()) {
            frame_.withVlan.type = type;
        } else {
            frame_.withoutVlan.type = type;
        }
    }

    /// @brief get Ethernet frame type
    /// @return obtained type
    Type GetType() const noexcept
    {
        /// If the current frame contains a vlan tag, read according to vlan format
        if (HasVlanTag()) {
            return frame_.withVlan.type;
        }
        return frame_.withoutVlan.type;
    }

    /// @brief get Ethernet frame header size (whether it has a VLAN tag affects the frame size)
    /// @return obtained frame header size
    std::uint32_t GetHeadSize() const noexcept
    {
        if (HasVlanTag()) {
            return sizeof(HeaderVlan);
        }
        return sizeof(Header);
    }

    /// @brief tpid, vlantag, ptptype are in network byte order; after modification, HasVlanTag becomes invalid
    /// @return none
    void ToNetworkFormat() noexcept
    {
        if (HasVlanTag()) {
            std::uint16_t *p{reinterpret_cast< std::uint16_t * >(&frame_.withVlan.vlan.tpid)};
            *p = htons(*p);
            p++;
            *p = htons(*p);
            p++;
            *p = htons(*p);
        } else {
            std::uint16_t *const p{reinterpret_cast< std::uint16_t * >(&frame_.withoutVlan.type)};
            *p = htons(*p);
        }
    }

private:
    /// @brief HeaderContent - ether frame header. HeaderVlan and Header have similar structures, type and tpid have the same positions and types, so define a union for convenience.
    union HeaderContent
    {
        Header withoutVlan;
        HeaderVlan withVlan;
    };
    /// @name frame_ - ether frame header. HeaderVlan and Header have similar structures, type and tpid have the same positions and types, so define a union for convenience.
    HeaderContent frame_{};

};  /// class Ether
#pragma pack(pop)

}  // namespace network
}  // namespace osi
}  // namespace isoft

#endif  /// ISOFT_OSI_NETWORK_ETHER_H_
