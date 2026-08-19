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
/// @file       raw_runtime.h
/// @brief
/// @details
/// @date       2023-05-19
/// @author     cuijiusen
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_INTERNAL_RAW_RAW_RUNTIME_H
#define ARA_COM_INTERNAL_RAW_RAW_RUNTIME_H

#include "ara/com/internal/raw/raw_base.h"
#include "ara/com/internal/raw/raw_config.h"
#include "ara/com/raw/raw_error_domain.h"
#include "ara/core/future.h"
#include "ara/core/instance_specifier.h"

namespace ara {
namespace com {
namespace internal {
namespace raw {
/// @brief Raw byte stream initialization
/// @return void if successful, otherwise an error code indicating the error.
/// @exception
ara::core::Result< void > Initialize() noexcept;
/// @brief Raw byte stream deinitialization
/// @return void if successful, otherwise an error code indicating the error.
ara::core::Result< void > Deinitialize() noexcept;

/// @brief raw runtime class
class RawRuntime;

namespace runtime {
/// @brief Get raw byte stream instance
/// @return A reference to the RawRuntime instance
/// @exception
RawRuntime& GetInstance() noexcept;

/// @brief Determine whether the expression is true, if not, record FATAL log
/// @param[in] exp The expression to check.
/// @return The value of the expression.
/// @exception
#define RAW_ASSERT(exp, ...)                                                                                           \
    (__extension__({                                                                                                   \
        auto __r{(exp)};                                                                                               \
        if (ara::com::internal::raw::runtime::ValueInvalid(__r)) {                                                     \
            ComLogFatal("RAW_ASSERT failed[", __r, "]: " #exp, ##__VA_ARGS__);                                         \
            ara::core::Abort("RAW_ASSERT failed");                                                                     \
        }                                                                                                              \
        __r;                                                                                                           \
    }))

/// @brief Determine whether the expression is true, if not, record ERROR log
/// @param[in] exp The expression to check.
/// @return The value of the expression.
/// @exception
#define RAW_CHECK(exp, ...)                                                                                            \
    (__extension__({                                                                                                   \
        auto __r{(exp)};                                                                                               \
        if (ara::com::internal::raw::runtime::ValueInvalid(__r)) {                                                     \
            ComLogError("RAW_CHECK failed[", __r, "]: " #exp, ##__VA_ARGS__);                                          \
        }                                                                                                              \
        __r;                                                                                                           \
    }))

/// @brief Determine whether the expression is true, if not, record WARNING log
/// @param[in] exp The expression to check.
/// @return The value of the expression.
/// @exception
#define RAW_VERIFY(exp, ...)                                                                                           \
    (__extension__({                                                                                                   \
        auto __r{(exp)};                                                                                               \
        if (ara::com::internal::raw::runtime::ValueInvalid(__r)) {                                                     \
            ComLogWarning("RAW_VERIFY failed[", __r, "]: " #exp, ##__VA_ARGS__);                                       \
        }                                                                                                              \
        __r;                                                                                                           \
    }))

/// @brief Check if T is true
/// @tparam T The type of the value.
/// @param[in] r The value to check.
/// @return true if the value is invalid, false otherwise.
/// @exception
template < typename T >
inline bool ValueInvalid(T const& r) noexcept
{
    return !(r);
}

/// @brief Check if r is true
/// @param[in] r The value to check.
/// @return true if the value is invalid, false otherwise.
/// @exception
inline bool ValueInvalid(int32_t r) noexcept { return r < 0; }

/// @brief Load configuration
/// @return int32_t Zero on success, non-zero otherwise.
/// @exception
int32_t LoadRawConf() noexcept;
}  // namespace runtime

/// @brief Raw runtime class
class RawRuntime final
{
    /// @brief BaseRawServer unique_prt type
    using BaseRawServerPtr = std::unique_ptr< BaseRaw >;

    /// @brief BaseRawClient unique_prt type
    using BaseRawClientPtr = std::unique_ptr< BaseRaw >;

    /// @brief RawConfig shared_ptr type
    using RawConfigPtr = std::shared_ptr< RawConfig >;

public:
    /// @brief constructor
    /// @exception
    explicit RawRuntime() noexcept;

private:
    /// @brief Copy constructor
    /// @param[in] other the object to be copy.
    RawRuntime(RawRuntime const& other) noexcept = default;

    /// @brief Copy assignment function
    /// @param[in] other the object to be copy.
    /// @return RawRuntime object
    RawRuntime& operator=(RawRuntime const& other) noexcept = default;

public:
    /// @brief Move constructor
    /// @param[in] other the object to be move.
    RawRuntime(RawRuntime&& other) noexcept = delete;

    /// @brief Move assignment function
    /// @param[in] other the object to be move.
    /// @return RawRuntime object
    RawRuntime& operator=(RawRuntime&& other) & = delete;

public:
    /// @brief Destructor
    /// @exception
    ~RawRuntime() noexcept;

    /// @brief Raw byte stream initialization
    /// @return void if successful, otherwise an error code indicating the error.
    /// @exception
    ara::core::Result< void > Initialize() noexcept;
    /// @brief Raw byte stream deinitialization
    /// @return void if successful, otherwise an error code indicating the error.
    /// @exception
    ara::core::Result< void > Deinitialize() noexcept;

    /// @brief Raw byte stream create instance
    /// @param[in] instance InstanceSpecifier
    /// @return instance obj pointer
    /// @exception no
    static BaseRawServerPtr CreateInstance(ara::core::InstanceSpecifier const& instance) noexcept;

    /// @brief Raw byte stream get configuration information
    /// @param[in] instance instancespecifier
    /// @return pointer of RawConfig if successfull,or nullpor if not found
    /// @exception
    static RawConfigPtr GetRawConfig(ara::core::InstanceSpecifier const& instance) noexcept
    {
        return RawMappingManager::GetRawConfig(instance);
    }
};

}  // namespace raw
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
