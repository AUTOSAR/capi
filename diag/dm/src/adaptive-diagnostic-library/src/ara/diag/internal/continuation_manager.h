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
/// @file       continuation_manager.h
/// @brief      This file provides the definitions of ContinuationManager and related types.
/// @details
/// @date       2022-08-14
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_INTERNAL_CONTINUATION_MANAGER_H_
#define ARA_DIAG_INTERNAL_CONTINUATION_MANAGER_H_

#include <ara/core/future.h>
#include <ara/core/map.h>
#include <ara/core/promise.h>
#include <ara/core/result.h>

#include <functional>
#include <memory>
#include <mutex>

namespace ara {
namespace diag {
namespace internal {

/// @brief Continuation manager of future-promise model
/// @code{.isoft}
/// export_level=/Diagnostics/Diagnostic Library
/// @endcode
/// @tparam FutureType Future type, used to specify the input of continuation
/// @tparam PromiseType Promise type, used to specify the output of continuation
///
template < typename FutureType, typename PromiseType >
class ContinuationManager
{
public:
    /// @brief Copy constructor is prohibited
    /// @param
    ContinuationManager(ContinuationManager const&) = delete;
    /// @brief Copy assignment constructor is prohibited
    /// @param
    /// @return
    ContinuationManager& operator=(ContinuationManager const&) = delete;
    /// @brief Move constructor is prohibited
    /// @param
    /// @throws on overflow
    ContinuationManager(ContinuationManager&&) noexcept = default;
    /// @brief Move assignment operator is prohibited
    /// @param
    /// @return
    /// @throws on overflow
    ContinuationManager& operator=(ContinuationManager&&) noexcept = default;

    /// @brief Constructor of ContinuationManager
    /// @code{.isoft}
    /// export_level=/Diagnostics/Diagnostic Library
    /// @endcode
    /// @throws on overflow
    ContinuationManager() : tableLock_{std::make_shared< std::recursive_mutex >()} {}

    /// @brief Destructor of ContinuationManager
    ~ContinuationManager() = default;

    /// @brief Submit the future to be processed
    /// @code{.isoft}
    /// export_level=/Diagnostics/Diagnostic Library
    /// @endcode
    /// @param future Future to be processed
    /// @param process Function used to process the future result
    /// @return Future of PromiseType
    /// @throws on overflow
    ara::core::Future< PromiseType > Submit(
        ara::core::Future< FutureType > future,
        std::function< ara::core::Result< PromiseType >(ara::core::Result< FutureType >) >&& process)
    {
        std::unique_lock< std::recursive_mutex > const lock{*tableLock_};
        std::uint64_t tableIndex{0U};
        for (auto& it : table_) {
            if (it.first == tableIndex) {
                ++tableIndex;
            } else {
                break;
            }
        }
        decltype(table_.emplace(tableIndex, std::make_shared< Continuation >()))
            const it{std::move(table_.emplace(tableIndex, std::make_shared< Continuation >()))};
        decltype(auto) continuation{it.first->second};
        continuation->process_ = process;
        continuation->future_  = std::move(future);
        continuation->future_.then([this, tableIndex]() { this->Fire(tableIndex); });
        return std::move(continuation->promise_.get_future());
    }

    /// @brief Trigger the execution of a specific continuation
    /// @param tableIndex Identifier of continuation
    /// @throws on overflow
    void Fire(std::uint64_t const tableIndex)
    {
        std::unique_lock< std::recursive_mutex > const lock{*tableLock_};
        decltype(table_.find(tableIndex)) const it{std::move(table_.find(tableIndex))};
        if (it != table_.end()) {
            it->second->promise_.SetResult(std::move(it->second->process_(std::move(it->second->future_.GetResult()))));
            std::ignore = table_.erase(it);
        }
    }

    /// @brief Remove a specific continuation
    /// @param[in] index Identifier of continuation
    /// @throws on overflow
    void Remove(std::uint64_t index)
    {
        std::unique_lock< std::recursive_mutex > lock{*tableLock_};
        table_.erase(index);
    }

private:
    /// @brief Continuation object
    struct Continuation
    {
        /// @brief Input of continuation
        ara::core::Future< FutureType > future;
        /// @brief Logic of continuation
        std::function< ara::core::Result< PromiseType >(ara::core::Result< FutureType >) > process;

        ara::core::Promise< PromiseType >
            /// @brief Output of continuation
            promise;
    };

    ara::core::Map< std::uint64_t, std::shared_ptr< Continuation > >
        /// @brief Continuation object table
        table_;

    std::shared_ptr< std::recursive_mutex >
        /// @brief Lock
        tableLock_;
};

}  // namespace internal
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_INTERNAL_CONTINUATION_MANAGER_H_