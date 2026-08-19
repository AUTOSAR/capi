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
/// @file       timevalue.h
/// @brief      time value class
/// @details
/// @date       2023-02-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEVALUE_H_
#define ARA_TSYNC_INTERNAL_TIMEVALUE_H_

#include <ara/core/array.h>

#include <chrono>
#include <cstdint>

#include "isoft/util/endian.h"
#include "isoft/util/time.h"
#include "isoft/util/type_cast.h"

namespace ara {
namespace tsync {
namespace internal {

/// @brief time value class
/// @note stored in network byte order, returned in host byte order
class TimeValue final
{
public:
    /// @brief unsigned 32-bit integer width
    static std::uint32_t const kUint32BitWide = static_cast< std::uint32_t >(32);

    /// @brief default parameterless constructor
    TimeValue() = default;
    /// @brief default destructor
    ~TimeValue() = default;

    /// @brief constructor
    /// @param ns - nanoseconds
    explicit TimeValue(std::chrono::nanoseconds const &ns) noexcept { FromChrono(ns); }

    /// @brief copy constructor
    /// @param other - other object
    TimeValue(TimeValue const &other) noexcept { *this = other; }

    /// @brief move constructor
    /// @param other - other object
    TimeValue(TimeValue &&other) = default;

    /// @brief copy assignment
    /// @param other
    /// @return reference to self
    TimeValue &operator=(TimeValue const &other) &noexcept
    {
        if (&other == this) {
            return *this;
        }
        this->secondsMsb_  = other.secondsMsb_;
        this->secondsLsb_  = other.secondsLsb_;
        this->nanoSeconds_ = other.nanoSeconds_;
        return *this;
    }

    /// @brief move assignment
    /// @param other
    /// @return reference to self
    TimeValue &operator=(TimeValue &&other) = default;

    /// @brief get seconds value
    /// @return seconds, host byte order
    std::int64_t GetSecond() const noexcept
    {
        std::uint16_t secMsb{0U};
        std::uint32_t secLsb{0U};
        std::uint64_t sec{0U};

        secMsb
            = isoft::util::NtoH16(*isoft::util::PointerCast< std::uint16_t, std::uint8_t >(this->secondsMsb_.data()));
        secLsb
            = isoft::util::NtoH32(*isoft::util::PointerCast< std::uint32_t, std::uint8_t >(this->secondsLsb_.data()));

        sec = (static_cast< std::uint64_t >(secMsb) << kUint32BitWide) | static_cast< std::uint64_t >(secLsb);
        return static_cast< std::int64_t >(sec);
    }

    /// @brief get nanoseconds value
    /// @return nanoseconds, host byte order
    std::int32_t GetNanoSecond() const noexcept
    {
        std::uint32_t ns{0U};
        ns = isoft::util::NtoH32(*isoft::util::PointerCast< std::uint32_t, std::uint8_t >(this->nanoSeconds_.data()));
        return static_cast< std::int32_t >(ns);
    }

    /// @brief convert to std::chrono nanoseconds
    /// @return nanoseconds
    std::chrono::nanoseconds ToChrono() const noexcept
    {
        std::chrono::nanoseconds ns;
        ns = std::chrono::seconds(GetSecond()) + std::chrono::nanoseconds(GetNanoSecond());
        return ns;
    }

    /// @brief convert from chrono class and store, the structure will be saved in network byte order
    /// @param ns - nanoseconds
    void FromChrono(std::chrono::nanoseconds const &ns) noexcept
    {
        std::uint64_t sec{0U};
        std::uint64_t nsec{0U};

        sec = static_cast< std::uint64_t >(ns.count())
              / static_cast< std::uint64_t >(isoft::util::kRatioOfSecondAndNano);
        nsec = static_cast< std::uint64_t >(ns.count())
               % static_cast< std::uint64_t >(isoft::util::kRatioOfSecondAndNano);

        *isoft::util::PointerCast< std::uint32_t, std::uint8_t >(nanoSeconds_.data())
            = isoft::util::HtoN32(static_cast< std::uint32_t >(nsec));
        *isoft::util::PointerCast< std::uint32_t, std::uint8_t >(secondsLsb_.data())
            = isoft::util::HtoN32(static_cast< std::uint32_t >(sec));
        std::uint64_t const tmp{sec >> kUint32BitWide};
        *isoft::util::PointerCast< std::uint16_t, std::uint8_t >(secondsMsb_.data())
            = isoft::util::HtoN16(static_cast< std::uint16_t >(tmp));
    }
    /// @brief operator overload
    /// @param rtv - timestamp object
    /// @exception
    /// @return object reference
    TimeValue &operator-=(TimeValue const &rtv)
    {
        FromChrono(this->ToChrono() - rtv.ToChrono());
        return *this;
    }

private:
    ara::core::Array< std::uint8_t, sizeof(std::uint16_t) >
        /// @name secondsMsb_ - seconds, high part. Currently Msb will not actually be used, Lsb is sufficient until 2038
        secondsMsb_{static_cast< std::uint16_t >(0)};

    ara::core::Array< std::uint8_t, sizeof(std::uint32_t) >
        /// @name secondsLsb_ - seconds, low part
        secondsLsb_{static_cast< std::uint8_t >(0)};

    ara::core::Array< std::uint8_t, sizeof(std::uint32_t) >
        /// @name nanoSeconds_ - nanoseconds
        nanoSeconds_{static_cast< std::uint8_t >(0)};
};

/// @brief operator overload
/// @param lhs - timestamp object
/// @param rhs - timestamp object
/// @exception
/// @return object
inline TimeValue const operator-(TimeValue const &lhs, TimeValue const &rhs)
{
    TimeValue tmp{lhs};
    tmp -= rhs;
    return tmp;
}
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_TIMEVALUE_H_
