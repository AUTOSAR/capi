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
/// @file       offset_time_base_provider_private_impl.h
/// @brief      offset time base provider private implementation class
/// @details
/// @date       2023-02-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/OffsetTimeBase
/// module_path=/TimeSync/OffsetTimeBase
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_OFFSET_TIME_BASE_PROVIDER_PRIVATE_IMPL_H_
#define ARA_TSYNC_OFFSET_TIME_BASE_PROVIDER_PRIVATE_IMPL_H_

#include "ara/tsync/offset_time_base_provider.h"
#include "ara/tsync/time_base_provider_common.h"

namespace ara {
namespace tsync {

/// @brief offset time base provider private implementation class
class OffsetTimeBaseProvider::OffsetTimeBaseProviderPrivateImpl : public TimeBaseProviderCommon
{
public:
    /// @brief copy constructor is prohibited
    /// @param other - other object
    OffsetTimeBaseProviderPrivateImpl(OffsetTimeBaseProviderPrivateImpl const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to private class object
    OffsetTimeBaseProviderPrivateImpl &operator=(OffsetTimeBaseProviderPrivateImpl const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    OffsetTimeBaseProviderPrivateImpl(OffsetTimeBaseProviderPrivateImpl &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to private class object
    /// TODO(zhoubo)：move assignment operator is explicitly defaulted but implicitly deleted
    OffsetTimeBaseProviderPrivateImpl &operator=(OffsetTimeBaseProviderPrivateImpl &&) &noexcept = delete;

    /// @brief destructor
    ~OffsetTimeBaseProviderPrivateImpl() noexcept override = default;

    /// @brief constructor
    /// @param inst - instance descriptor
    explicit OffsetTimeBaseProviderPrivateImpl(ara::core::String const &inst) noexcept
        : TimeBaseProviderCommon{inst} {};

    /// @brief create time base provider private class
    /// @param inst - time base instance descriptor
    /// @return pointer to time base provider private class
    static std::unique_ptr< OffsetTimeBaseProvider::OffsetTimeBaseProviderPrivateImpl > CreatePrivateImpl(
        ara::core::String const &inst) noexcept
    {
        std::unique_ptr< OffsetTimeBaseProviderPrivateImpl > pri{nullptr};
        pri = std::make_unique< OffsetTimeBaseProviderPrivateImpl >(inst);
        if (nullptr != pri) {
            if (0 != pri->_Init()) {
                pri = nullptr;
            }
        }
        return pri;
    }

    /// @brief get current time
    /// @return timestamp
    ara::tsync::Timestamp GetCurrentTime() const noexcept override;

    /// @brief used to set a new offset time value for the time domain, which will immediately trigger bus transmission. Does not modify the system clock, only records the difference.
    /// @param offsetTime - time value
    /// @param userData - user data
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t SetTime(ara::tsync::Timestamp const &offsetTime,
                         ara::core::Span< ara::core::Byte const > const &userData) noexcept override;

protected:
    /// @brief check the validity of the current time base
    /// @return 0 - no exception
    /// @return <0 - problematic
    std::int32_t Validate() const noexcept override;

};  /// class OffsetTimeBaseProvider::OffsetTimeBaseProviderPrivateImpl

}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_OFFSET_TIME_BASE_PROVIDER_PRIVATE_IMPL_H_