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
/// @file       sessioncompare.h
/// @brief      time base proxy session management class
/// @details
/// @date       2023-02-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_SESSION_COMPARE_H_
#define ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_SESSION_COMPARE_H_
#include "ara/tsync/internal/timebase/proxy/session.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace proxy {

/// @brief functor for comparing Session classes
class SessionCompare final
{
public:
    /// @brief functor for comparison
    /// @param a - left value
    /// @param b - right value
    /// @return yes/no
    bool operator()(std::unique_ptr< Session > const& a, std::unique_ptr< Session > const& b) const noexcept
    {
        return a->kSid < b->kSid;
    }
};

}  // namespace proxy
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_SESSION_COMPARE_H_
