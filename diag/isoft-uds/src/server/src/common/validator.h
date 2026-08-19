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
/// @file       validator.h
/// @brief      This file provides validator functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/common
/// @interface_level=software
/// @trace_id_sr=9e1a0f10-a981-45be-a95d-aef2a92918c5
/// @unit_name=validator
/// @unit_description=This file provides validator functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_VALIDATOR_H_
#define ISOFT_UDS_SERVER_VALIDATOR_H_

#include <ara/core/map.h>
#include <ara/core/vector.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "common/request_context.h"
#include "isoft/uds/uds_nrc_error_domain.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Validator base class
/// @return Return value
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
class Validator
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00840
    /// @needwork = dda
    /// @endcode
    Validator(Validator const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00841
    /// @needwork = dda
    /// @endcode
    Validator& operator=(Validator const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00842
    /// @needwork = dda
    /// @endcode
    Validator(Validator&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00843
    /// @needwork = dda
    /// @endcode
    Validator& operator=(Validator&&) = delete;

    /// @brief Constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    explicit Validator() noexcept = default;

    /// @brief Destructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    virtual ~Validator() noexcept = default;

    /// @brief Validate function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    virtual bool Validate(std::shared_ptr< RequestContext > const& context) noexcept = 0;
};

/// @brief Validator pointer type
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
using ValidatorPtr = std::shared_ptr< Validator >;

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_VALIDATOR_H_