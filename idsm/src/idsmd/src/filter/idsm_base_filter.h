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
/// @file       idsm_base_filter.h
/// @brief      Abstract filter class
/// @details
/// @date       2023-02-15
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Filter chain
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0007,SR_IDSM_0008,SR_IDSM_0009,SR_IDSM_0010
/// @unit_name=FilterBase
/// @unit_description=Abstract filter class
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_BASE_FILTER_H_
#define ARA_IDSM_BASE_FILTER_H_
#include <chrono>
#include <list>
#include <map>
#include <memory>

#include "ara/idsm/internal/event.h"

namespace ara {
namespace idsm {
/// @brief Base class of filter, with a single pure virtual function Filter interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00244
/// @trace_id_dd=DD_IDSM_00697
/// @needwork = ad
/// @endcode
class FilterBase
{
public:
    /// @brief Filter interface, pure virtual function
    /// @param event event to filter
    /// @return filter result true: passed, false: not passed
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00245
    /// @trace_id_dd=DD_IDSM_00698
    /// @needwork = ad
    /// @endcode
    virtual bool Filter(std::shared_ptr< Event > const& event) = 0;

protected:
    /// @brief default constructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00699
    /// @needwork = dda
    /// @endcode
    FilterBase() = default;
    /// @brief copy constructor
    /// @param filter object to copy
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00700
    /// @needwork = dda
    /// @endcode
    FilterBase(FilterBase const& filter) = default;
    /// @brief move constructor
    /// @param filter object to move
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00701
    /// @needwork = dda
    /// @endcode
    FilterBase(FilterBase&& filter) = default;
    /// @brief copy assignment operator
    /// @param filter object to copy
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00702
    /// @needwork = dda
    /// @endcode
    FilterBase& operator=(FilterBase const& filter) = default;
    /// @brief move assignment operator
    /// @param filter object to move
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00703
    /// @needwork = dda
    /// @endcode
    FilterBase& operator=(FilterBase&& filter) = default;

public:
    /// @brief destructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00704
    /// @needwork = dda
    /// @endcode
    virtual ~FilterBase() = default;
};

/// @brief Type redefinition: define smart pointer type for data structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00246
/// @trace_id_dd=DD_IDSM_00705
/// @needwork = ad
/// @endcode
using FilterBasePtr = std::shared_ptr< FilterBase >;

}  // namespace idsm
}  // namespace ara

#endif