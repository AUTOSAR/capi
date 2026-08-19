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
/// @file       idsm_sample_filter.h
/// @brief      Sampling filter implementation
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
/// @trace_id_sr=SR_IDSM_0008
/// @unit_name=SampleFilter
/// @unit_description=Sampling filter implementation
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_SAMPLE_FILTER_H_
#define ARA_IDSM_SAMPLE_FILTER_H_
#include "ara/idsm/internal/event.h"
#include "idsm_base_filter.h"
namespace ara {
namespace idsm {

/// @brief Sampling filter
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00240
/// @trace_id_dd=DD_IDSM_00685
/// @needwork = ad
/// @endcode
class SampleFilter : public FilterBase
{
public:
    /// @brief Filter interface
    /// @param event event to filter
    /// @return filter result true: passed, false: not passed
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00686
    /// @needwork = dda
    /// @endcode
    bool Filter(EventPtr const& event) noexcept override;

public:
    /// @brief parameterless constructor
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// export_level=/idsm/Filter chain
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00241
    /// @trace_id_dd=DD_IDSM_00687
    /// @needwork = ad
    /// @endcode
    SampleFilter() noexcept : SampleFilter{1U, 0U} {}
    /// @brief parameterized constructor
    /// @param circle period
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// export_level=/idsm/Filter chain
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00242
    /// @trace_id_dd=DD_IDSM_00688
    /// @needwork = ad
    /// @endcode
    explicit SampleFilter(uint32_t const circle) noexcept : SampleFilter{circle, 0U} {}

public:
    /// @brief copy constructor
    /// @param filter object to copy
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00689
    /// @needwork = dda
    /// @endcode
    SampleFilter(SampleFilter const& filter) noexcept = default;
    /// @brief move constructor
    /// @param filter object to move
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00690
    /// @needwork = dda
    /// @endcode
    SampleFilter(SampleFilter&& filter) noexcept = default;
    /// @brief copy assignment operator
    /// @param filter object to copy
    /// @return left operand of assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00691
    /// @needwork = dda
    /// @endcode
    SampleFilter& operator=(SampleFilter const& filter) noexcept = default;
    /// @brief move assignment operator
    /// @param filter object to move
    /// @return left operand of assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00692
    /// @needwork = dda
    /// @endcode
    SampleFilter& operator=(SampleFilter&& filter) noexcept = default;
    /// @brief destructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// export_level=/idsm/Filter chain
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00243
    /// @trace_id_dd=DD_IDSM_00693
    /// @needwork = ad
    /// @endcode
    ~SampleFilter() override = default;

private:
    /// @brief parameterized constructor
    /// @param circle sampling period
    /// @param events accumulated number of security events within period
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00694
    /// @needwork = dda
    /// @endcode
    SampleFilter(uint32_t const circle, uint32_t const events) noexcept : circleNum_{circle}, eventSum_{events} {}

private:
    /// @brief period
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00695
    /// @needwork = dda
    /// @endcode
    uint32_t circleNum_;
    /// @brief accumulated event count within period
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00696
    /// @needwork = dda
    /// @endcode
    uint32_t eventSum_;
};

}  // namespace idsm
}  // namespace ara
#endif