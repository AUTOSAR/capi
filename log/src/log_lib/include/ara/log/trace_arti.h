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
/// @file       trace_arti.h
/// @brief      Encapsulation of internal log parameters
/// @details
/// @date       2025-09-15
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG
/// @interface_level = module
/// @trace_id_sr=LOG_SR_00001
/// @unit_name = logger_impl
/// @unit_description=Lib interface side of Dlt, encapsulation of internal log parameters
/// @endcode
///
/// ================================================================

#ifndef ARA_LOG_TRACE_ARTI_H
#define ARA_LOG_TRACE_ARTI_H

#include <tuple>
namespace apext {
namespace log {

template < typename End >
void ExTraceArti(End end)
{
    std::ignore = end;
}

template < typename First, typename... Args >
void ExTraceArti(First first, const Args&... args)
{
    std::ignore = first;
    ExTraceArti(args...);  // Fix: parameter pack needs to be expanded with ...
}

template < typename MsgId, typename... Params >
void TraceArti(const MsgId& messageId, const Params&... args) noexcept
{
    std::ignore = messageId;
    ExTraceArti(args...);  // Supplement: it is also recommended to explicitly expand the parameter pack here (though optional, it is more standard)
    // by default do nothing
    // specializations should implement the trace
}

}  // namespace log
}  // namespace apext

#endif /* ARA_LOG_TRACE_ARTI_H */