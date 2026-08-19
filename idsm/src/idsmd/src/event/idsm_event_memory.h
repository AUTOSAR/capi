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
/// @file       idsm_event_memory.h
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
/// @unit_description=Statistics of memory used by security events
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_EVENT_MEMORY_H_
#define ARA_IDSM_EVENT_MEMORY_H_

#include <memory>
#include <mutex>
namespace ara {
namespace idsm {
/// @brief Security event memory statistics pool
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00147
/// @trace_id_dd=DD_IDSM_00361
/// @needwork = ad
/// @endcode
class EventMemPool final
{
public:
    /// @brief Get the unique instance of EventMemPool
    /// @return EventMemPool instance
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={reentrant}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00148
    /// @trace_id_dd=DD_IDSM_00362
    /// @needwork = ad
    /// @endcode
    static std::shared_ptr< EventMemPool > GetInstance();
    /// @brief Increase memory occupied by security events
    /// @param eventMem memory of security event frame
    /// @param contextMem memory of context data
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00149
    /// @trace_id_dd=DD_IDSM_00363
    /// @needwork = ad
    /// @endcode
    void AllocMem(uint32_t const eventMem, uint64_t const contextMem);
    /// @brief Decrease memory occupied by security events
    /// @param eventMem memory of security event frame
    /// @param contextMem memory of context data
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00150
    /// @trace_id_dd=DD_IDSM_00364
    /// @needwork = ad
    /// @endcode
    void ReleaseMem(uint32_t const eventMem, uint64_t const contextMem);
    /// @brief Get memory occupied by security events
    /// @param eventMemUsed memory of security event frame
    /// @param contextMemUsed memory of context data
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00151
    /// @trace_id_dd=DD_IDSM_00365
    /// @needwork = ad
    /// @endcode
    void GetUsedMem(uint64_t& eventMemUsed, uint64_t& contextMemUsed);
    /// @brief Clear memory statistics records, used for testing
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00152
    /// @trace_id_dd=DD_IDSM_00366
    /// @needwork = ad
    /// @endcode
    void Reset();

public:
    /// @brief destructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00367
    /// @needwork = dda
    /// @endcode
    ~EventMemPool() = default;

public:
    /// @brief copy constructor
    /// @param pool object to copy
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00368
    /// @needwork = dda
    /// @endcode
    EventMemPool(EventMemPool const& pool) = delete;
    /// @brief move constructor
    /// @param pool object to move
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00369
    /// @needwork = dda
    /// @endcode
    EventMemPool(EventMemPool&& pool) = delete;
    /// @brief copy assignment operator
    /// @param pool object to copy in assignment operator
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00370
    /// @needwork = dda
    /// @endcode
    EventMemPool& operator=(EventMemPool const& pool) = delete;
    /// @brief move assignment operator
    /// @param pool object to move in assignment operator
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00371
    /// @needwork = dda
    /// @endcode
    EventMemPool& operator=(EventMemPool&& pool) = delete;

private:
    /// @brief constructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00372
    /// @needwork = dda
    /// @endcode
    EventMemPool() noexcept = default;

private:
    /// @brief Unique instance of event memory statistics pool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00373
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< EventMemPool > s_SingleInstance_;
    /// @brief Singleton thread-safe mutex lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00374
    /// @needwork = dda
    /// @endcode
    static std::mutex s_SingleMutex_;
    /// @brief memory limit mutex lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00375
    /// @needwork = dda
    /// @endcode
    std::mutex memMutex_;
    /// @brief event stack frame memory statistics
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00376
    /// @needwork = dda
    /// @endcode
    uint64_t eventMemUsed_{0U};
    /// @brief event context memory statistics
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00377
    /// @needwork = dda
    /// @endcode
    uint64_t contextMemUsed_{0U};
};

}  // namespace idsm
}  // namespace ara
#endif