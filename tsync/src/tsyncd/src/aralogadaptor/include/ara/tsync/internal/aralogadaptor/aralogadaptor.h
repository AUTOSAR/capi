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
/// @file       aralogadaptor.h
/// @brief      time synchronization internal log stream definition
/// @details
/// @date       2023-02-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/LogWrapper
/// module_path=/TimeSync/LogWrapper
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_ARALOGADAPTOR_ARALOGADAPTOR_H_
#define ARA_TSYNC_INTERNAL_ARALOGADAPTOR_ARALOGADAPTOR_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include <cstdint>
#include <functional>
#include <type_traits>

#include "ara/tsync/internal/log/defines.h"
#include "ara/tsync/internal/log/stream.h"

namespace ara {
namespace tsync {
namespace internal {
namespace tsaralog {

/// @brief log printer
class AraLogAdaptor final
{
public:
    /// @brief constructor
    AraLogAdaptor() = default;

    /// @brief default destructor
    ~AraLogAdaptor() = default;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    AraLogAdaptor(AraLogAdaptor const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    AraLogAdaptor &operator=(AraLogAdaptor const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    AraLogAdaptor(AraLogAdaptor &&other) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    AraLogAdaptor &operator=(AraLogAdaptor &&other) &noexcept = default;

    /// @brief print log via AraLog
    /// @param logAttr - log attributes
    /// @param logMesg - log content
    /// @exception
    static void Log2AraLog(ara::tsync::internal::tslog::LogAttribute const &logAttr,
                           ara::core::StringView const &logMesg);
};

}  // namespace tsaralog
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_ARALOGADAPTOR_ARALOGADAPTOR_H_
