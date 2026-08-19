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
/// @file       cg_runtime.h
/// @brief      header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_INTERNAL_CG_CG_RUNTIME_H
#define ARA_COM_INTERNAL_CG_CG_RUNTIME_H

#include "ara/com/cg/cg_error_domain.h"
#include "ara/com/internal/cg/cg_instance.h"
#include "ara/com/internal/cg/impl_communication_group.h"
#include "ara/core/future.h"
#include "ara/core/instance_specifier.h"

namespace ara {
namespace com {
namespace internal {
namespace cg {
/// @brief
/// @return Result object -- empty/value or error
ara::core::Result< void > Initialize() noexcept;
/// @brief
/// @return Result object -- empty/value or error
ara::core::Result< void > Deinitialize() noexcept;

/// @brief
class CgRuntime;

/// @brief
/// @namespace runtime
namespace runtime {
/// @brief
/// @return
CgRuntime& GetInstance() noexcept;

/// @brief
/// @param[in] serviceIdentifier
template < typename T >
void RegisterCgService(ara::com::internal::ServiceIdentifier serviceIdentifier) noexcept;

#define CG_ASSERT(exp, ...)                                                                                            \
    (__extension__({                                                                                                   \
        auto __r{(exp)};                                                                                               \
        if (ara::com::internal::cg::runtime::ValueInvalid(__r)) {                                                      \
            ComLogFatal("CG_ASSERT failed[", __r, "]: " #exp, ##__VA_ARGS__);                                          \
            ara::core::Abort("CG_ASSERT failed");                                                                      \
        }                                                                                                              \
        __r;                                                                                                           \
    }))

#define CG_CHECK(exp, ...)                                                                                             \
    (__extension__({                                                                                                   \
        auto __r{(exp)};                                                                                               \
        if (ara::com::internal::cg::runtime::ValueInvalid(__r)) {                                                      \
            ComLogError("CG_CHECK failed[", __r, "]: " #exp, ##__VA_ARGS__);                                           \
        }                                                                                                              \
        __r;                                                                                                           \
    }))

#define CG_VERIFY(exp, ...)                                                                                            \
    (__extension__({                                                                                                   \
        auto __r{(exp)};                                                                                               \
        if (ara::com::internal::cg::runtime::ValueInvalid(__r)) {                                                      \
            ComLogWarning("CG_VERIFY failed[", __r, "]: " #exp, ##__VA_ARGS__);                                        \
        }                                                                                                              \
        __r;                                                                                                           \
    }))

/// @brief
/// @param[in] r
/// @return
template < typename T >
inline bool ValueInvalid(T const& r) noexcept
{
    return !(r);
}
/// @brief
/// @param[in] r
/// @return
inline bool ValueInvalid(int32_t r) noexcept { return r < 0; }

/// @brief
/// @return
int32_t LoadCgConf() noexcept;
}  // namespace runtime

/// @brief
class CgRuntime final
{
public:
    /// @brief
    CgRuntime() noexcept;
    /// @brief
    ~CgRuntime() noexcept;
    /// @brief copy constructor - not allowed.
    /// @param[in] other the object to be copy.
    CgRuntime(CgRuntime const& other) noexcept = delete;
    /// @brief move constructor
    /// @param[in] other the object to be move.
    CgRuntime(CgRuntime&& other) noexcept = delete;
    /// @brief Copy assignment operator of the CgRuntime - not allowed.
    /// @param[in] other the object to be copy.
    /// @return CgRuntime object
    CgRuntime& operator=(CgRuntime const& other) noexcept = delete;
    /// @brief move assignment operator of the CgRuntime
    /// @param[in] other the object to be move.
    /// @return CgRuntime object
    CgRuntime& operator=(CgRuntime&& other) noexcept;
    /// @brief
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Initialize() noexcept;
    /// @brief
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Deinitialize() noexcept;

    /// @brief
    template < typename Skeleton >
    void RegisterCgService() noexcept
    {
        ara::com::internal::ServiceIdentifier serviceIdentifier{CgTypeInfo< Skeleton >::type_name()};
        std::unique_ptr< CommunicationGroupObj > obj{std::make_unique< CommunicationGroup< Skeleton > >()};
        std::ignore = cgServiceList_.emplace(std::make_pair(serviceIdentifier, std::move(obj)));
    }
    /// @brief Start the communication group service
    /// @return bool
    /// @retval true Start succeeded
    /// @retval false Start failed
    bool StartCgService() noexcept;
    /// @brief Iterate and process method and event requests received by the skeleton
    void ProcessNextMethodCall() noexcept;
    /// @brief Stop the communication group service
    /// @return bool
    /// @retval true Start succeeded
    /// @retval false Start failed
    bool StopCgService() noexcept;
    /// @brief
    void Dump() noexcept;

    /// @brief
    /// @param[in] name
    /// @return
    ara::core::String GetServerInstanceSpecifier(ara::core::StringView const& name) noexcept;

private:
    /// @brief
    ara::core::Map< ara::com::internal::ServiceIdentifier, std::shared_ptr< CommunicationGroupObj > > cgServiceList_;
};
}  // namespace cg
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
