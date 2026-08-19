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
/// @file       evnode_exec.h
/// @brief
/// @details
/// @date       2021-11-10
/// @author     wei.jia
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_EVNODE_EXEC_H_
#define ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_EVNODE_EXEC_H_

#include <nai/io/nai_event.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>

#include "isoft/naicpp/internal/evnode.h"

namespace isoft {
namespace naicpp {

/// @brief EvNodeExec encapsulation
class EvNodeExec final
{
public:
    /// @brief Execute function
    ///
    /// @param fun
    /// @return int
    int Exec(std::function< void() >&& fun) noexcept
    {
        {
            std::lock_guard< std::mutex > const lock_guard{functionListLock_};
            functionList_.emplace(std::move(fun));
        }
        return evnode_.Signal();
    }

protected:
    friend class EvLoop;

    /// @brief default constructor
    ///
    /// @traceid
    /// @uptrace
    EvNodeExec() = default;

    /// @brief destructor.
    ///
    /// @traceid
    /// @uptrace
    ~EvNodeExec() = default; /*//{ evnode_.Close(); }*/

    /// @brief copy constructor is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param other class instance.
    EvNodeExec(EvNodeExec const& other) noexcept = default;

    /// @brief Copy Assignment Operator is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param other class instance.
    /// @returns   instance reference
    EvNodeExec& operator=(EvNodeExec const& other) noexcept = default;

    /// @brief Move Constructor is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param obj class instance.
    /// @returns   instance object
    EvNodeExec(EvNodeExec&& obj) noexcept = default;

    /// @brief Move Assignment Operator is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param other class instance.
    /// @returns   instance reference
    EvNodeExec& operator=(EvNodeExec&& other) & noexcept = default;

    /// @brief Execute functionList_
    ///
    /// @param evloop
    /// @return int
    int Open(nai_evloop_t* const evloop) noexcept
    {
        int const r{evnode_.Open(evloop, 0, std::move([this]() {
                                     std::queue< std::function< void() > > funs;
                                     functionListLock_.lock();
                                     functionList_.swap(funs);
                                     functionListLock_.unlock();

                                     while (funs.size() > 0U) {
                                         funs.front()();
                                         funs.pop();
                                     }
                                 }))};
        if (r >= 0) {
            return isoft::kSuccess;
        }
        return r;
    }

private:
    /// @name evnode_
    /// event node

    internal::EvNode evnode_;

    /// @name functionListLock_
    /// Mutex lock
    std::mutex functionListLock_;

    /// @name functionList_
    /// functionList_
    std::queue< std::function< void() > > functionList_;
};

}  // namespace naicpp
}  // namespace isoft

#endif  // ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_EVNODE_EXEC_H_
