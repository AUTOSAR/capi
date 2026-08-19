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
/// @file       idsm_state_filter.h
/// @brief      State filter implementation
/// @details
/// @date       2023-02-15
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Filter chain
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0007
/// @unit_name=StateFilter
/// @unit_description=State filter implementation
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_STATE_FILTER_H_
#define ARA_IDSM_STATE_FILTER_H_
#include "idsm_base_filter.h"
namespace ara {
namespace idsm {

/// @brief State filter
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00220
/// @trace_id_dd=DD_IDSM_00625
/// @needwork = ad
/// @endcode
class StateFilter : public FilterBase
{
public:
    /// @brief Type redefinition: define data structure for functional group state blacklist
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00626
    /// @needwork = dda
    /// @endcode
    using FGStateVec = std::vector< ara::core::String >;
    /// @brief Filter interface
    /// @param event event to filter
    /// @return filter result true: passed, false: not passed
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00627
    /// @needwork = dda
    /// @endcode
    bool Filter(EventPtr const& event) override;
    /// @brief Set blacklist of functional group states
    /// @param fgName functional group name
    /// @param fgState functional group state
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// export_level=/idsm/Filter chain
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00221
    /// @trace_id_dd=DD_IDSM_00628
    /// @needwork = ad
    /// @endcode
    void SetFGState(ara::core::String const& fgName, ara::core::String const& fgState);

public:
    /// @brief default constructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// export_level=/idsm/Filter chain
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00222
    /// @trace_id_dd=DD_IDSM_00629
    /// @needwork = ad
    /// @endcode
    StateFilter() = default;
    /// @brief copy constructor
    /// @param filter object to copy
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00630
    /// @needwork = dda
    /// @endcode
    StateFilter(StateFilter const& filter) = default;
    /// @brief move constructor
    /// @param filter object to move
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00631
    /// @needwork = dda
    /// @endcode
    StateFilter(StateFilter&& filter) = default;
    /// @brief copy assignment operator
    /// @param filter object to copy in assignment operator
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00632
    /// @needwork = dda
    /// @endcode
    StateFilter& operator=(StateFilter const& filter) = default;
    /// @brief move assignment operator
    /// @param filter object to move in assignment operator
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00633
    /// @needwork = dda
    /// @endcode
    StateFilter& operator=(StateFilter&& filter) = default;
    /// @brief destructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// export_level=/idsm/Filter chain
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00223
    /// @trace_id_dd=DD_IDSM_00634
    /// @needwork = ad
    /// @endcode
    ~StateFilter() override = default;

private:
    /// @brief functional group state blacklist
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00635
    /// @needwork = dda
    /// @endcode
    std::map< ara::core::String, FGStateVec > fgStateMap_;
};

}  // namespace idsm
}  // namespace ara
#endif