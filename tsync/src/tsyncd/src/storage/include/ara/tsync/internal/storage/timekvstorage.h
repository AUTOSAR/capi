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
/// @file       timekvstorage.h
/// @brief      time KV persistence management class
/// @details
/// @date       2023-01-12
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_TIMESTORAGE_H_
#define ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_TIMESTORAGE_H_

#include <ara/core/array.h>
#include <ara/core/string.h>

#include <chrono>
#include <cstdint>

#include "ara/per/key_value_storage.h"
#include "ara/tsync/internal/common.h"

namespace ara {
namespace tsync {
namespace internal {
namespace storage {

/// @brief time persistence class, stores when process exits, and restores from it when process starts
class TimeKVStorage
{
public:
    /// @brief Stored status type
    using ValueType = ara::core::String;

    /// @brief constructor
    ///
    /// @param modelIdentifier instance descriptor
    /// @endcode
    explicit TimeKVStorage(ara::core::StringView const &modelIdentifier) noexcept;

    /// @brief open persistence library
    /// @return true, success; false, failure;
    bool Open() noexcept;

    /// @brief persist time base content
    /// @param key - persistent key value
    /// @param userData - user data
    /// @param rateDeviation - rate deviation
    /// @param lastGlobalNano - last set global time value
    /// @return true, success; false, failure;
    bool StoreTime(ara::core::StringView const &key,
                   ara::core::String const &userData,
                   const double rateDeviation,
                   std::uint64_t const &lastGlobalNano) noexcept;

    /// @brief retrieve time base content from persistent storage
    /// @param key - persistent key value
    /// @param userData - user data
    /// @param rateDeviation - rate deviation
    /// @param lastGlobalNano - last set global time value
    /// @return true, success; false, failure;
    bool GetTime(ara::core::StringView const &key,
                 ara::core::String &userData,
                 double &rateDeviation,
                 std::uint64_t &lastGlobalNano) noexcept;

private:
    /// @brief instance descriptor
    ara::core::InstanceSpecifier kvsInstanceSpecifier_;

    /// @brief A shared handle to access key-value storage
    ara::per::SharedHandle< ara::per::KeyValueStorage > kvs_;
    /// @brief splitBit delimiter
    ara::core::StringView splitBit_;
};

}  // namespace storage
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  // ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_TIMESTORAGE_H_
