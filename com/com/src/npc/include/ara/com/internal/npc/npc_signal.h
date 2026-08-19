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
/// @file       npc_signal.h
/// @brief      Binding layer signal header file
/// @details
/// @date       2022-10-12
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef __COM_NPC_SIGNAL_H
#define __COM_NPC_SIGNAL_H

#include "ara/com/internal/log/log.h"
#include "npc_serialize.h"
#include "npc_types.h"

/// @brief Namespace -- internal binding layer signals
namespace ara {
namespace com {
namespace internal {
namespace npc {
namespace signal {
/// @brief Type alias -- PDU header identifier type
using HeaderId = uint32_t;
/// @brief Type alias -- PDU header identifier list type
/// @tparam id PDU header identifier list
template < HeaderId... id >
using HeaderIds = std::integer_sequence< HeaderId, id... >;
/// @brief Get PDU header identifier from description info
/// @tparam Desc Description info
/// @return PDU header identifier
template < typename Desc >
constexpr auto headerId()  // NOLINT -- template interface name > naming convention
{
    static_assert(sizeof(Desc::serviceId()) == 2 && sizeof(Desc::eventId()) == 2, "invalid service/event id type");
    return HeaderId{Desc::serviceId() << (sizeof(Desc::serviceId()) * CHAR_BIT) | Desc::eventId()};
}
/// @brief Template type -- check if signal-based (false)
/// @tparam T Description info
/// @tparam Tagged Tag type for SFINAE
template < typename T, typename Tagged = void >
struct IsSignalBased : std::false_type
{
};
/// @brief Template type -- check if signal-based (true)
/// @tparam Desc Description info
template < typename Desc >
struct IsSignalBased< Desc, typename Desc::TagSignalBased > : std::true_type
{
};
/// @brief Template type -- check if signal-based (true)
/// @tparam Desc Description info
template < typename Desc >
struct IsSignalBased< Desc, typename Desc::IsSignalBasedTag > : std::true_type
{
};
/// @brief Check if signal-based
/// @tparam Desc Description info
/// @return bool
template < typename Desc >
constexpr bool isSignalBased()  // NOLINT -- template interface name > naming convention
{
    return IsSignalBased< Desc >::value;
}
/// @brief Template type -- check if signal service (false)
/// @tparam T Description info
/// @tparam Tagged Tag type for SFINAE
template < typename T, typename Tagged = void >
struct IsSignalService : std::false_type
{
};
/// @brief Template type -- check if signal service (true)
/// @tparam Desc Description info
template < typename Desc >
struct IsSignalService< Desc, typename Desc::TagSignalService > : std::true_type
{
};
/// @brief Template type -- check if signal service (true)
/// @tparam Desc Description info
template < typename Desc >
struct IsSignalService< Desc, typename Desc::IsSignalServiceTag > : std::true_type
{
};
/// @brief Template type -- check if SOME/IP service (false)
/// @tparam T Description info
/// @tparam Tagged Tag type for SFINAE
template < typename T, typename Tagged = void >
struct IsSomeipService : std::false_type
{
};
/// @brief Template type -- check if SOME/IP service (true)
/// @tparam Desc Description info
template < typename Desc >
struct IsSomeipService< Desc, typename Desc::TagSomeipService > : std::true_type
{
};
/// @brief Template type -- check if SOME/IP service (true)
/// @tparam Desc Description info
template < typename Desc >
struct IsSomeipService< Desc, typename Desc::IsSomeipServiceTag > : std::true_type
{
};
/// @brief Check if signal service
/// @tparam T Description info
/// @return bool
template < typename T >
constexpr bool isSomeipService()  // NOLINT -- template interface name > naming convention
{
    return !IsSignalService< T >::value;
}
/// @brief Check if SOME/IP service
/// @tparam T Description info
/// @return bool
template < typename T >
constexpr bool isSignalService()  // NOLINT -- template interface name > naming convention
{
    return IsSignalService< T >::value && !IsSomeipService< T >::value;
}
/// @brief Check if mixed service
/// @tparam T Description info
/// @return bool
template < typename T >
constexpr bool isMixedService()  // NOLINT -- template interface name > naming convention
{
    return IsSignalService< T >::value && IsSomeipService< T >::value;
}

/// @brief S2S conversion properties
struct S2SProps
{
    /// @brief Template type -- S2S conversion value property
    /// @details Used to establish the source/target PDU header identifier list and serialization implementation associated with this value (specialized PDU header identifier)
    /// @tparam id PDU header identifier
    template < HeaderId id >
    struct Value
    {
        /// @brief Type alias -- value type
        using ValueType = void;
        /// @brief Type alias -- source PDU header identifier list type
        using SourceIds = HeaderIds<>;
        /// @brief Type alias -- target PDU header identifier list type
        using TargetIds = HeaderIds<>;
    };
    /// @brief Template type -- S2S conversion association property
    /// @details Used to establish the fan-in/fan-out and whether trigger implementation associated with this association (specialized source/target PDU header identifier pair)
    /// @tparam sourceId Source PDU header identifier
    /// @tparam targetId Target PDU header identifier
    template < HeaderId sourceId, HeaderId targetId >
    struct Mapping
    {
        /// @brief Type alias -- source value type
        using SourceType = typename Value< sourceId >::ValueType;
        /// @brief Type alias -- target value type
        using TargetType = typename Value< targetId >::ValueType;
        /// @brief Write
        /// @param[in] source Source value
        /// @param[in] target Target value
        /// @return bool Whether triggered
        static bool Write(SourceType const& source, TargetType& target);
        /// @brief Read
        /// @param[in] source Source value
        /// @param[in] target Target value
        /// @return bool Whether triggered
        static bool Read(SourceType& source, TargetType const& target);
    };
};
/// @brief
namespace details {
/// @brief Template type -- check if S2S source
/// @tparam id PDU header identifier
/// @tparam SourceIds Source PDU header identifier list type
template < HeaderId id, typename SourceIds >
struct IsS2SSource
{
    /// @brief Not a source PDU header identifier -- empty list
    enum
    {
        kValue = false
    };
};
/// @brief Template type -- check if S2S source
/// @tparam id PDU header identifier
/// @tparam sourceId Source PDU header identifier
template < HeaderId id, HeaderId sourceId >
struct IsS2SSource< id, HeaderIds< sourceId > >
{
    /// @brief Check if source PDU header identifier -- list has value
    enum
    {
        kValue = id == sourceId
    };
};
/// @brief Template type -- check if S2S source
/// @tparam id PDU header identifier
/// @tparam sourceId Source PDU header identifier
/// @tparam sourceIds Source PDU header identifier list
template < HeaderId id, HeaderId sourceId, HeaderId... sourceIds >
struct IsS2SSource< id, HeaderIds< sourceId, sourceIds... > >
{
    /// @brief Check if source PDU header identifier exists -- multiple values in list
    enum
    {
        kValue = id == sourceId || IsS2SSource< id, HeaderIds< sourceIds... > >::kValue
    };
};
/// @brief Template type -- check if S2S target
/// @tparam id PDU header identifier
/// @tparam TargetIds Target PDU header identifier list type
template < HeaderId id, typename TargetIds >
struct IsS2STarget
{
    /// @brief Not a target PDU header identifier -- empty list
    enum
    {
        kValue = false
    };
};
/// @brief Template type -- check if S2S target
/// @tparam id PDU header identifier
/// @tparam targetId Target PDU header identifier
template < HeaderId id, HeaderId targetId >
struct IsS2STarget< id, HeaderIds< targetId > >
{
    /// @brief Check if target PDU header identifier -- list has value
    enum
    {
        kValue = id == targetId
    };
};
/// @brief Template type -- check if S2S target
/// @tparam id PDU header identifier
/// @tparam targetId Target PDU header identifier
/// @tparam targetIds Target PDU header identifier list
template < HeaderId id, HeaderId targetId, HeaderId... targetIds >
struct IsS2STarget< id, HeaderIds< targetId, targetIds... > >
{
    /// @brief Check if target PDU header identifier exists -- multiple values in list
    enum
    {
        kValue = id == targetId || IsS2STarget< id, HeaderIds< targetIds... > >::kValue
    };
};
}  // namespace details
/// @brief Check if S2S source
/// @tparam Desc Description info
/// @return bool
template < typename Desc >
constexpr bool isS2SSource()  // NOLINT -- template interface name > naming convention
{
    return details::IsS2SSource< headerId< Desc >(),
                                 typename S2SProps::Value< headerId< Desc >() >::SourceIds >::kValue;
}
/// @brief Check if S2S target
/// @tparam Desc Description info
/// @return bool
template < typename Desc >
constexpr bool isS2STarget()  // NOLINT -- template interface name > naming convention
{
    return details::IsS2STarget< headerId< Desc >(),
                                 typename S2SProps::Value< headerId< Desc >() >::TargetIds >::kValue;
}
#if 0  // examples for specialized by the generator
/// @brief Specialization of S2SProps::Value for id 11
template <>
struct S2SProps::Value< 11 >
{
    using ValueType = T11;
    using SourceIds = HeaderIds<>;
    using TargetIds = HeaderIds< 21, 22 >;
    /// @brief Enumerates the values of the given ValueType
    /// @tparam F Function object type
    /// @param[in] fun Function object
    /// @param[in] val Value to enumerate
    template < typename F >
    void enumerate(F& fun, ValueType& val)
    {
        fun(val.v1, 0x0000 << 3 | 0, true);
        fun(val.v2, 0x0004 << 3 | 0, true);
    }
};
/// @brief Specialization of S2SProps::Mapping for source header 11 and target header 21
template <>
bool S2SProps::Mapping< 11, 21 >::Write(SourceType const& source, TargetType& target)
{
    target.v1 = source.v1;
    return false;
}
/// @brief Specialization of S2SProps::Mapping for source header 11 and target header 21
template <>
bool S2SProps::Mapping< 11, 21 >::Read(SourceType& source, TargetType const& target)
{
    source.v1 = target.v1;
    return false;
}
/// @brief Specialization of S2SProps::Mapping for source header 11 and target header 22
template <>
bool S2SProps::Mapping< 11, 22 >::Write(SourceType const& source, TargetType& target)
{
    target.v1 = source.v2;
    return false;
}
/// @brief Specialization of S2SProps::Mapping for source header 11 and target header 22
template <>
bool S2SProps::Mapping< 11, 22 >::Read(SourceType& source, TargetType const& target)
{
    source.v2 = target.v1;
    return true;
}
/// @brief Specialization of S2SProps::Mapping for source header 12 and target header 21
template <>
bool S2SProps::Mapping< 12, 21 >::Write(SourceType const& source, TargetType& target)
{
    target.v2 = source.v1;
    return true;
}
/// @brief Specialization of S2SProps::Mapping for source header 12 and target header 21
template <>
bool S2SProps::Mapping< 12, 21 >::Read(SourceType& source, TargetType const& target)
{
    source.v1 = target.v2;
    return false;
}
/// @brief Specialization of S2SProps::Mapping for source header 12 and target header 22
template <>
bool S2SProps::Mapping< 12, 22 >::Write(SourceType const& source, TargetType& target)
{
    target.v2 = source.v2;
    return true;
}
/// @brief Specialization of S2SProps::Mapping for source header 12 and target header 22
template <>
bool S2SProps::Mapping< 12, 22 >::Read(SourceType& source, TargetType const& target)
{
    source.v2 = target.v2;
    return true;
}
#endif
/// @brief A class template for signal-to-signal communication.
/// @tparam id PDU header identifier
template < HeaderId id >
struct S2S
{
    /// @brief Tag for S2S
    using TagS2S = void;
    /// @brief Length field type for S2S
    using LengthFieldType = uint32_t;
    /// @brief Value type for S2S
    using ValueType = typename S2SProps::Value< id >::ValueType;
    /// @brief Source IDs for S2S
    using SourceIds = typename S2SProps::Value< id >::SourceIds;
    /// @brief Target IDs for S2S
    using TargetIds = typename S2SProps::Value< id >::TargetIds;
    /// @brief A reference to the value of the signal.
    ValueType& value;
    /// @brief Calls the given function for each element in the value of the signal.
    /// @tparam F The type of the function.
    /// @param[in] fun The function to call for each element.
    template < typename F >
    void enumerate(F& fun)  // NOLINT -- template interface name > naming convention
    {
        S2SProps::Value< id >::enumerate(fun, value);
    }
    /// @brief Returns a reference to the value of the signal.
    /// @return auto& A reference to the value of the signal.
    static auto& Value() noexcept
    {
        static ValueType s_Instance;
        return s_Instance;
    }
    /// @brief Returns a reference to the sender function of the signal.
    /// @return auto& A reference to the sender function of the signal.
    static auto& Sender() noexcept
    {
        static std::function< ara::core::Result< void >(ValueType const&) > s_Instance;
        return s_Instance;
    }
    /// @brief Returns a reference to the receiver function of the signal.
    /// @return auto& A reference to the receiver function of the signal.
    static auto& Receiver() noexcept
    {
        static std::function< ara::core::Result< void >(ValueType const&) > s_Instance;
        return s_Instance;
    }
    /// @brief Sends the given value to the target signals.
    /// @param[in] value The value to send.
    /// @return Result object -- empty/value or error
    static ara::core::Result< void > Send(ValueType const& value) noexcept { return Send(value, TargetIds{}); }
    /// @brief Receives the given value from the source signals.
    /// @param[in] value The value to receive.
    /// @return Result object -- empty/value or error
    static ara::core::Result< void > Recv(ValueType const& value) noexcept { return Recv(value, SourceIds{}); }

private:
    /// @brief Sends the given value to the target signals with the given header IDs.
    /// @tparam targetId The header ID of the first target signal.
    /// @tparam targetIds The header IDs of the remaining target signals.
    /// @param[in] value The value to send.
    /// @return Result object -- empty/value or error
    static ara::core::Result< void > Send(ValueType const& value, HeaderIds<>) noexcept
    {
        std::ignore = value;
        return {};
    }
    /// @brief Sends the given value to the target signals with the given header IDs.
    /// @tparam targetId The header ID of the first target signal.
    /// @tparam targetIds The header IDs of the remaining target signals.
    /// @param[in] value The value to send.
    /// @return Result object -- empty/value or error
    template < HeaderId targetId, HeaderId... targetIds >
    static ara::core::Result< void > Send(ValueType const& value, HeaderIds< targetId, targetIds... >) noexcept
    {
        using Result = ara::core::Result< void >;
        Result result;
        auto res{Send< targetId >(value)};
        if (!res) {
            ComLogError("send target failed", GenArg(id), GenArg(targetId), GenArg(value), GenArg(result));
            result = std::move(res);
        }
        res = Send(value, HeaderIds< targetIds... >{});
        if (!res) {
            result = std::move(res);
        }
        return result;
    }
    /// @brief Sends the given value to the target signal with the given header ID.
    /// @tparam targetId The header ID of the target signal.
    /// @param[in] value The value to send.
    /// @return Result object -- empty/value or error
    template < HeaderId targetId >
    static ara::core::Result< void > Send(ValueType const& value,
                                          std::enable_if_t< id == targetId >* = nullptr) noexcept
    {
        ComLogInfo(GenK2V0(id, value));
        return Sender()(value);
    }
    /// @brief Sends a value to a target header ID using the S2S protocol.
    /// @tparam targetId The header ID of the target.
    /// @param[in] value The value to send.
    /// @return Result object -- empty/value or error
    template < HeaderId targetId >
    static ara::core::Result< void > Send(ValueType const& value,
                                          std::enable_if_t< id != targetId >* = nullptr) noexcept
    {
        auto& target{S2S< targetId >::Value()};
        auto trigger{S2SProps::Mapping< id, targetId >::Write(value, target)};
        ComLogInfo(GenArg0(trigger), GenK2V(id, value), GenK2V(targetId, target));
        if (!trigger) {
            return {};
        }
        return S2S< targetId >::Sender()(target);
    }
    /// @brief Receives a value from no source header IDs using the S2S protocol.
    /// @param[in] value The value to receive.
    /// @return Result object -- empty/value or error
    static ara::core::Result< void > Recv(ValueType const& value, HeaderIds<>) noexcept
    {
        std::ignore = value;
        return {};
    }
    /// @brief Receives a value from multiple source header IDs using the S2S protocol.
    /// @tparam sourceId The header ID of the first source.
    /// @tparam sourceIds The header IDs of the remaining sources.
    /// @param[in] value The value to receive.
    /// @return Result object -- empty/value or error
    template < HeaderId sourceId, HeaderId... sourceIds >
    static ara::core::Result< void > Recv(ValueType const& value, HeaderIds< sourceId, sourceIds... >) noexcept
    {
        using Result = ara::core::Result< void >;
        Result result;
        auto res{Recv< sourceId >(value)};
        if (!res) {
            ComLogError("recv source failed", GenArg(id), GenArg(sourceId), GenArg(value), GenArg(result));
            result = std::move(res);
        }
        res = Recv(value, HeaderIds< sourceIds... >{});
        if (!res) {
            result = std::move(res);
        }
        return result;
    }
    /// @brief Receives a value from a source header ID using the S2S protocol.
    /// @tparam sourceId The header ID of the source.
    /// @param[in] value The value to receive.
    /// @return Result object -- empty/value or error
    template < HeaderId sourceId >
    static ara::core::Result< void > Recv(ValueType const& value,
                                          std::enable_if_t< id == sourceId >* = nullptr) noexcept
    {
        ComLogInfo(GenK2V0(id, value));
        return Receiver()(value);
    }
    /// @brief Receives a value from a source header ID using the S2S protocol.
    /// @tparam sourceId The header ID of the source.
    /// @param[in] value The value to receive.
    /// @return Result object -- empty/value or error
    template < HeaderId sourceId >
    static ara::core::Result< void > Recv(ValueType const& value,
                                          std::enable_if_t< id != sourceId >* = nullptr) noexcept
    {
        auto& source{S2S< sourceId >::Value()};
        auto trigger{S2SProps::Mapping< sourceId, id >::Read(source, value)};
        ComLogInfo(GenArg0(trigger), GenK2V(id, value), GenK2V(sourceId, source));
        if (!trigger) {
            return {};
        }
        return S2S< sourceId >::Receiver()(source);
    }
};
}  // namespace signal
}  // namespace npc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
