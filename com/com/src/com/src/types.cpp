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
/// @file       types.cpp
/// @brief      Communication type implementation file
/// @details
/// @date       2023-10-07
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/types.h"

#include <atomic>

namespace ara {
namespace com {
/// @brief Service instance identifier implementation type
class InstanceIdentifier::Impl
{
public:
    /// @brief Constructor
    /// @param[in] value Instance identifier value
    explicit Impl(ara::core::StringView value) noexcept : value_{value}
    {
        // ComLogTrace(GenArg0(value_));
    }
    /// @brief Destructor
    ~Impl() noexcept  // NOLINT
    {
        // ComLogTrace(GenArg0(value_));
    }
    /// @brief Copy constructor
    /// @param other
    Impl(Impl const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    Impl(Impl&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return Impl
    Impl& operator=(Impl const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return Impl
    Impl& operator=(Impl&& other) noexcept = default;
    /// @brief Operator -- equality comparison
    /// @param[in] other
    /// @return bool
    bool operator==(Impl const& other) const noexcept
    {
        if (!(value_ == other.value_)) {
            return false;
        }
        return true;
    }
    /// @brief Operator -- less than comparison
    /// @param[in] other
    /// @return bool
    bool operator<(Impl const& other) const noexcept
    {
        if (!(value_ == other.value_)) {
            return value_ < other.value_;
        }
        return false;
    }
    /// @brief Check if it is an any instance identifier
    /// @param[in] prefix Instance identifier prefix
    /// @return bool
    bool IsAny(ara::core::String const& prefix) const noexcept { return value_ == kAny || value_ == prefix + kAny; }
    /// @brief Convert to string
    /// @return Instance identifier string
    ara::core::StringView ToString() const noexcept { return value_; }

private:
    /// @brief Instance identifier value
    ara::core::String value_;
};

InstanceIdentifier::InstanceIdentifier(ara::core::StringView value) noexcept
    : impl_{std::make_shared< InstanceIdentifier::Impl >(value)}
{
}
InstanceIdentifier::~InstanceIdentifier() noexcept                               = default;
InstanceIdentifier::InstanceIdentifier(InstanceIdentifier const& other) noexcept = default;
InstanceIdentifier::InstanceIdentifier(InstanceIdentifier&& other) noexcept      = default;
InstanceIdentifier& InstanceIdentifier::operator=(InstanceIdentifier const& other) noexcept
{
    if (this != &other) {
        impl_ = other.impl_;
    }
    return *this;
}
InstanceIdentifier& InstanceIdentifier::operator=(InstanceIdentifier&& other) noexcept
{
    if (this != &other) {
        impl_ = std::move(other.impl_);
    }
    return *this;
}
bool InstanceIdentifier::operator==(InstanceIdentifier const& other) const noexcept { return *impl_ == *other.impl_; }
bool InstanceIdentifier::operator<(InstanceIdentifier const& other) const noexcept { return *impl_ < *other.impl_; }
ara::core::StringView InstanceIdentifier::ToString() const noexcept { return impl_->ToString(); }
bool InstanceIdentifier::IsAny(ara::core::String const& prefix) const noexcept { return impl_->IsAny(prefix); }
InstanceIdentifier InstanceIdentifier::MakeAny() noexcept
{
    static InstanceIdentifier const kInstance{kAny};
    return kInstance;
}
constexpr char InstanceIdentifier::kAny[];

FindServiceHandle::FindServiceHandle() noexcept
    : serviceIdentifier{{}}, instanceIdentifier{InstanceIdentifier::MakeAny()}, uid{}
{
}
FindServiceHandle::FindServiceHandle(internal::ServiceIdentifier serviceIdentifierIn,
                                     InstanceIdentifier instanceIdentifierIn,
                                     uint32_t uidIn) noexcept
    : serviceIdentifier{std::move(serviceIdentifierIn)}, instanceIdentifier{std::move(instanceIdentifierIn)}, uid{uidIn}
{
}
FindServiceHandle::~FindServiceHandle() noexcept = default;
bool FindServiceHandle::operator==(FindServiceHandle const& other) const noexcept
{
    if (!(serviceIdentifier == other.serviceIdentifier)) {
        return false;
    }
    if (!(instanceIdentifier == other.instanceIdentifier)) {
        return false;
    }
    if (!(uid == other.uid)) {
        return false;
    }
    return true;
}
bool FindServiceHandle::operator<(FindServiceHandle const& other) const noexcept
{
    if (!(serviceIdentifier == other.serviceIdentifier)) {
        return serviceIdentifier < other.serviceIdentifier;
    }
    if (!(instanceIdentifier == other.instanceIdentifier)) {
        return instanceIdentifier < other.instanceIdentifier;
    }
    return uid < other.uid;
}
ara::core::String FindServiceHandle::ToString() const noexcept
{
    return FmtStr(GenArg0(serviceIdentifier), GenArg(instanceIdentifier));
}
uint32_t FindServiceHandle::GenerateUID() noexcept
{
    static std::atomic< decltype(uid) > s_Instance{};
    return s_Instance.fetch_add(1);
}
}  // namespace com
}  // namespace ara

namespace std {
/// @brief Template type specialization -- Hash instance identifier
template <>
struct hash< ara::com::InstanceIdentifier >  // NOLINT -- Standard library interface name > naming convention
{
    /// @brief Operator -- returns hash value -- makes instance identifier usable in associative containers
    /// @param[in] id Instance identifier
    /// @return Hash value
    size_t operator()(ara::com::InstanceIdentifier const& id) const noexcept
    {
        return hash< ara::core::String >()(ara::core::String{id.ToString()});
    }
};
}  // namespace std