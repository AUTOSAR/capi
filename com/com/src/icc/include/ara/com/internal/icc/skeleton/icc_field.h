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
/// @file       icc_field.h
/// @brief      Binding layer service skeleton field header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef __COM_ICC_SKELETON_FIELD_H
#define __COM_ICC_SKELETON_FIELD_H

#include "ara/com/internal/skeleton/field.h"
#include "icc_event.h"
#include "icc_method.h"

/// @brief Namespace -- internal binding layer skeleton
namespace ara {
namespace com {
namespace internal {
namespace icc {
namespace skeleton {
/// @brief Template type -- binding layer service skeleton field -- declaration
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Field data type
/// @tparam mode Field mode -- default notifyable/settable/gettable
template < typename Owner,
           typename Desc,
           typename T,
           ara::com::internal::skeleton::FieldMode mode = ara::com::internal::skeleton::FieldMode::kNSG >
class IccField;
/// @brief Type alias -- notifyable field
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Field data type
template < typename Owner, typename Desc, typename T >
using IccNField = IccField< Owner, Desc, T, ara::com::internal::skeleton::FieldMode::kN >;
/// @brief Type alias -- settable field
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Field data type
template < typename Owner, typename Desc, typename T >
using IccSField = IccField< Owner, Desc, T, ara::com::internal::skeleton::FieldMode::kS >;
/// @brief Type alias -- gettable field
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Field data type
template < typename Owner, typename Desc, typename T >
using IccGField = IccField< Owner, Desc, T, ara::com::internal::skeleton::FieldMode::kG >;
/// @brief Type alias -- notifyable/settable field
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Field data type
template < typename Owner, typename Desc, typename T >
using IccNSField = IccField< Owner, Desc, T, ara::com::internal::skeleton::FieldMode::kNS >;
/// @brief Type alias -- notifyable/gettable field
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Field data type
template < typename Owner, typename Desc, typename T >
using IccNGField = IccField< Owner, Desc, T, ara::com::internal::skeleton::FieldMode::kNG >;
/// @brief Type alias -- settable/gettable field
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Field data type
template < typename Owner, typename Desc, typename T >
using IccSGField = IccField< Owner, Desc, T, ara::com::internal::skeleton::FieldMode::kSG >;
/// @brief Type alias -- notifyable/settable/gettable field
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Field data type
template < typename Owner, typename Desc, typename T >
using IccNSGField = IccField< Owner, Desc, T, ara::com::internal::skeleton::FieldMode::kNSG >;
/// @brief Template type -- notifyable field implementation type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Field data type
/// @tparam bool notifier exists by default
template < typename Owner, typename Desc, typename T, bool = true >
class NFieldImpl
    : public virtual ara::com::internal::skeleton::NFieldBase< T >
    , public IccEvent< Owner, Desc, T >
{
public:
    /// @brief Type alias -- field base type
    using FieldBase = ara::com::internal::skeleton::NFieldBase< T >;
    /// @brief Type alias -- event implementation type
    using EventImpl = IccEvent< Owner, Desc, T >;
    /// @brief Type alias -- field data type
    using FieldType = typename FieldBase::FieldType;
    /// @brief Constructor
    /// @param[in] owner Service element owner
    explicit NFieldImpl(Owner& owner) noexcept : EventImpl{owner} { ComLogTrace(""); }
    /// @brief Destructor
    ~NFieldImpl() noexcept override { ComLogTrace(""); }
    /// @brief Copy constructor
    /// @param other
    NFieldImpl(NFieldImpl const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    NFieldImpl(NFieldImpl&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return NFieldImpl
    NFieldImpl& operator=(NFieldImpl const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return NFieldImpl
    NFieldImpl& operator=(NFieldImpl&& other) noexcept = default;
};
/// @brief Template type -- non-notifyable field implementation type
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Field data type
template < typename Owner, typename Desc, typename T >
class NFieldImpl< Owner, Desc, T, false >
{
public:
    /// @brief Constructor
    /// @param[in] owner Service element owner
    explicit NFieldImpl(Owner& owner) noexcept
    {
        std::ignore = owner;
        ComLogTrace("");
    }
    /// @brief Destructor
    ~NFieldImpl() noexcept { ComLogTrace(""); }
    /// @brief Copy constructor
    /// @param other
    NFieldImpl(NFieldImpl const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    NFieldImpl(NFieldImpl&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return NFieldImpl
    NFieldImpl& operator=(NFieldImpl const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return NFieldImpl
    NFieldImpl& operator=(NFieldImpl&& other) noexcept = default;
};
/// @brief Template type -- settable field implementation type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Field data type
/// @tparam bool setter exists by default
template < typename Owner, typename Desc, typename T, bool = true >
class SFieldImpl : public virtual ara::com::internal::skeleton::SFieldBase< T >
{
public:
    /// @brief Type alias -- field base type
    using FieldBase = ara::com::internal::skeleton::SFieldBase< T >;
    /// @brief Type alias -- field data type
    using FieldType = typename FieldBase::FieldType;
    /// @brief Type alias -- process set method signature
    using SetSignature = typename FieldBase::SetSignature;
    /// @brief Constructor
    /// @param[in] owner Service element owner
    explicit SFieldImpl(Owner& owner) noexcept
        : setMethod_{owner, std::function< decltype(setHandler_)() >{[this]() { return setHandler_; }}}
    {
        ComLogTrace("");
    }
    /// @brief Destructor
    ~SFieldImpl() noexcept override
    {
        ComLogTrace("");
        std::lock_guard< std::mutex > lock(mutex_);
        setHandler_ = nullptr;
    }
    /// @brief Copy constructor
    /// @param other
    SFieldImpl(SFieldImpl const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    SFieldImpl(SFieldImpl&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return SFieldImpl
    SFieldImpl& operator=(SFieldImpl const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return SFieldImpl
    SFieldImpl& operator=(SFieldImpl&& other) noexcept = default;
    /// @brief Register set method receiver -- call before offering service
    /// @return Result object -- empty/value or error
    ara::core::Result< void > RegisterSetReceiver() noexcept { return setMethod_.RegisterReceiver(); }
    /// @brief Unregister set method receiver -- call after stopping service offering
    /// @return Result object -- empty/value or error
    ara::core::Result< void > UnregisterSetReceiver() noexcept { return setMethod_.UnregisterReceiver(); }
    /// @brief Register set handler
    /// @param[in] setHandler Set handler
    /// @return Result object -- empty/value or error
    ara::core::Result< void > RegisterSetHandler(std::function< SetSignature > setHandler) noexcept override
    {
        using Result = ara::core::Result< void >;
        std::lock_guard< std::mutex > lock(mutex_);
        setHandler_ = std::move(setHandler);
        return Result{};
    }

private:
    /// @brief Convert to method description
    struct ToSetMethodDesc : Desc
    {
        static constexpr auto methodId()  // NOLINT -- template interface name > naming convention
        {
            return Desc::setMethodId();
        }
        static constexpr auto possibleErrors()  // NOLINT -- template interface name > naming convention
        {
            return ara::core::Array< ara::core::ErrorCode, 0 >{};
        }
        static constexpr auto isReliable()  // NOLINT -- template interface name > naming convention
        {
            return Desc::isSetterReliable();
        }
    };
    /// @brief Process set method
    IccMethod< Owner, ToSetMethodDesc, std::function< SetSignature > > setMethod_;
    /// @brief Set handler
    std::function< SetSignature > setHandler_;
    /// @brief Mutex
    std::mutex mutex_;
};
/// @brief Template type -- non-settable field implementation type
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Field data type
template < typename Owner, typename Desc, typename T >
class SFieldImpl< Owner, Desc, T, false >
{
public:
    /// @brief Constructor
    /// @param[in] owner Service element owner
    explicit SFieldImpl(Owner& owner) noexcept
    {
        std::ignore = owner;
        ComLogTrace("");
    }
    /// @brief Destructor
    ~SFieldImpl() noexcept { ComLogTrace(""); }
    /// @brief Copy constructor
    /// @param other
    SFieldImpl(SFieldImpl const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    SFieldImpl(SFieldImpl&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return SFieldImpl
    SFieldImpl& operator=(SFieldImpl const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return SFieldImpl
    SFieldImpl& operator=(SFieldImpl&& other) noexcept = default;
};
/// @brief Template type -- gettable field implementation type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Field data type
/// @tparam bool getter exists by default
template < typename Owner, typename Desc, typename T, bool = true >
class GFieldImpl : public virtual ara::com::internal::skeleton::GFieldBase< T >
{
public:
    /// @brief Type alias -- field base type
    using FieldBase = ara::com::internal::skeleton::GFieldBase< T >;
    /// @brief Type alias -- field data type
    using FieldType = typename FieldBase::FieldType;
    /// @brief Type alias -- process get method signature
    using GetSignature = typename FieldBase::GetSignature;
    /// @brief Constructor
    /// @param[in] owner Service element owner
    explicit GFieldImpl(Owner& owner) noexcept
        : getMethod_{owner, std::function< decltype(getHandler_)() >{[this]() { return getHandler_; }}}
    {
        ComLogTrace("");
    }
    /// @brief Destructor
    ~GFieldImpl() noexcept override
    {
        ComLogTrace("");
        std::lock_guard< std::mutex > lock(mutex_);
        getHandler_ = nullptr;
    }
    /// @brief Copy constructor
    /// @param other
    GFieldImpl(GFieldImpl const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    GFieldImpl(GFieldImpl&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return GFieldImpl
    GFieldImpl& operator=(GFieldImpl const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return GFieldImpl
    GFieldImpl& operator=(GFieldImpl&& other) noexcept = default;
    /// @brief Register get method receiver -- call before offering service
    /// @return Result object -- empty/value or error
    ara::core::Result< void > RegisterGetReceiver() noexcept { return getMethod_.RegisterReceiver(); }
    /// @brief Unregister get method receiver -- call after stopping service offering
    /// @return Result object -- empty/value or error
    ara::core::Result< void > UnregisterGetReceiver() noexcept { return getMethod_.UnregisterReceiver(); }
    /// @brief Register get handler
    /// @param[in] getHandler Get handler
    /// @return Result object -- empty/value or error
    ara::core::Result< void > RegisterGetHandler(std::function< GetSignature > getHandler) noexcept override
    {
        using Result = ara::core::Result< void >;
        std::lock_guard< std::mutex > lock(mutex_);
        getHandler_ = std::move(getHandler);
        return Result{};
    }

private:
    /// @brief Convert to method description
    struct ToGetMethodDesc : Desc
    {
        static constexpr auto methodId()  // NOLINT -- template interface name > naming convention
        {
            return Desc::getMethodId();
        }
        static constexpr auto possibleErrors()  // NOLINT -- template interface name > naming convention
        {
            return ara::core::Array< ara::core::ErrorCode, 0 >{};
        }
        static constexpr auto isReliable()  // NOLINT -- template interface name > naming convention
        {
            return Desc::isGetterReliable();
        }
    };
    /// @brief Process get method
    IccMethod< Owner, ToGetMethodDesc, std::function< GetSignature > > getMethod_;
    /// @brief Get handler
    std::function< GetSignature > getHandler_;
    /// @brief Mutex
    std::mutex mutex_;
};
/// @brief Template type -- non-gettable field implementation type
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Field data type
template < typename Owner, typename Desc, typename T >
class GFieldImpl< Owner, Desc, T, false >
{
public:
    /// @brief Constructor
    /// @param[in] owner Service element owner
    explicit GFieldImpl(Owner& owner) noexcept
    {
        std::ignore = owner;
        ComLogTrace("");
    }
    /// @brief Destructor
    ~GFieldImpl() noexcept { ComLogTrace(""); }
    /// @brief Copy constructor
    /// @param other
    GFieldImpl(GFieldImpl const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    GFieldImpl(GFieldImpl&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return GFieldImpl
    GFieldImpl& operator=(GFieldImpl const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return GFieldImpl
    GFieldImpl& operator=(GFieldImpl&& other) noexcept = default;
};
/// @brief Template type -- field type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Field data type
/// @tparam mode Field mode
template < typename Owner, typename Desc, typename T, ara::com::internal::skeleton::FieldMode mode >
class IccField
    : public ara::com::internal::skeleton::FieldBase< T, mode >
    , public NFieldImpl< Owner, Desc, T, ara::com::internal::skeleton::HasNotifier(mode) >
    , public SFieldImpl< Owner, Desc, T, ara::com::internal::skeleton::HasSetter(mode) >
    , public GFieldImpl< Owner, Desc, T, ara::com::internal::skeleton::HasGetter(mode) >
{
public:
    /// @brief Constructor
    /// @param[in] owner Service element owner
    explicit IccField(Owner& owner) noexcept
        : NFieldImpl< Owner, Desc, T, ara::com::internal::skeleton::HasNotifier(mode) >{owner}
        , SFieldImpl< Owner, Desc, T, ara::com::internal::skeleton::HasSetter(mode) >{owner}
        , GFieldImpl< Owner, Desc, T, ara::com::internal::skeleton::HasGetter(mode) >{owner}
    {
        ComLogTrace("");
    }
    /// @brief Destructor
    ~IccField() noexcept override { ComLogTrace(""); }
    /// @brief Copy constructor
    /// @param other
    IccField(IccField const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    IccField(IccField&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return IccField
    IccField& operator=(IccField const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return IccField
    IccField& operator=(IccField&& other) noexcept = default;
};
}  // namespace skeleton
}  // namespace icc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
