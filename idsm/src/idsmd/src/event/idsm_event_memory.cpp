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
/// @file       idsm_event_memory.cpp
/// @brief      Statistics of memory used by security events
/// @details
/// @date       2023-02-16
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Security event memory statistics
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0002
/// @unit_name=EventMemPool
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_event_memory.h"

#include "log/idsm_log.h"

namespace ara {
namespace idsm {
/// @brief Unique instance of event memory statistics pool
std::shared_ptr< EventMemPool > EventMemPool::s_SingleInstance_{nullptr};  // NOLINT
/// @brief Singleton thread-safe mutex lock
std::mutex EventMemPool::s_SingleMutex_{};
/// @brief Get the unique instance of EventMemPool
/// @return EventMemPool instance
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={reentrant}
/// @endcode
std::shared_ptr< EventMemPool > EventMemPool::GetInstance()
{
    if (s_SingleInstance_ == nullptr) {
        std::unique_lock< std::mutex > const lck{s_SingleMutex_};
        if (s_SingleInstance_ == nullptr) {
            s_SingleInstance_.reset(new EventMemPool);
        }
    }
    return s_SingleInstance_;
}
/// @brief Increase memory occupied by security events
/// @param eventMem memory of security event frame
/// @param contextMem memory of context data
/// @exception stack overflow exception
void EventMemPool::AllocMem(uint32_t const eventMem, uint64_t const contextMem)
{
    std::unique_lock< std::mutex > const lck{memMutex_};
    eventMemUsed_ += eventMem;
    contextMemUsed_ += contextMem;
}
/// @brief Decrease memory occupied by security events
/// @param eventMem memory of security event frame
/// @param contextMem memory of context data
/// @exception stack overflow exception
void EventMemPool::ReleaseMem(uint32_t const eventMem, uint64_t const contextMem)
{
    std::unique_lock< std::mutex > const lck{memMutex_};
    eventMemUsed_ -= eventMem;
    contextMemUsed_ -= contextMem;
}
/// @brief Get memory occupied by security events
/// @param eventMemUsed memory of security event frame
/// @param contextMemUsed memory of context data
/// @exception stack overflow exception
void EventMemPool::GetUsedMem(uint64_t& eventMemUsed, uint64_t& contextMemUsed)
{
    std::ignore = eventMemUsed;
    std::ignore = contextMemUsed;
    std::unique_lock< std::mutex > const lck{memMutex_};
    eventMemUsed   = eventMemUsed_;
    contextMemUsed = contextMemUsed_;
}
/// @brief Clear memory statistics records, used for testing
/// @exception stack overflow exception
void EventMemPool::Reset()
{
    std::unique_lock< std::mutex > const lck{memMutex_};
    eventMemUsed_   = 0U;
    contextMemUsed_ = 0U;
}

}  // namespace idsm
}  // namespace ara