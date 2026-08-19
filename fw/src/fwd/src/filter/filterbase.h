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
/// @file       filterbase.h
/// @brief      Filter base class
/// @details    Filter base class
/// @date       2024-11-26
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/filter Base
/// @interface_level=unitc
/// export_level=module
/// @trace_id_sr=SR_FW_0002,SR_FW_0004,SR_FW_0005
/// @unit_name=Filter_Base
/// @unit_description=Firewall filter base class.
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_FILTER_BASE_H_
#define ARA_FW_FILTER_BASE_H_

// Add header file includes
namespace ara {
namespace fw {
namespace internal {

/// @brief datalink filter.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00506
/// @trace_id_dd=DD_FW_00814
/// @needwork = ad
/// @endcode
class FilterBase
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00507
    /// @trace_id_dd=DD_FW_00815
    /// @needwork = ad
    /// @endcode
    FilterBase() = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00508
    /// @trace_id_dd=DD_FW_00816
    /// @needwork = ad
    /// @endcode
    virtual ~FilterBase() = default;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00509
    /// @trace_id_dd=DD_FW_00817
    /// @needwork = ad
    /// @endcode
    FilterBase &operator=(FilterBase const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00510
    /// @trace_id_dd=DD_FW_00818
    /// @needwork = ad
    /// @endcode
    FilterBase &operator=(FilterBase &&other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00511
    /// @trace_id_dd=DD_FW_00819
    /// @needwork = ad
    /// @endcode
    FilterBase(FilterBase &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00512
    /// @trace_id_dd=DD_FW_00820
    /// @needwork = ad
    /// @endcode
    FilterBase(FilterBase const &other) noexcept = delete;

    /// @brief Get rules.
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00513
    /// @trace_id_dd=DD_FW_00821
    /// @needwork = ad
    /// @endcode
    virtual void GetRules() noexcept = 0;

    /// @brief Perform actual rule filtering.
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00514
    /// @trace_id_dd=DD_FW_00822
    /// @needwork = ad
    /// @endcode
    virtual void RulesFilter() noexcept = 0;
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif