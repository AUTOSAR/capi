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
/// @file       noncopy.h
/// @brief      non copy or move
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/Utils
/// @module_path=/UCM/Utils
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=NonCopy
/// @unit_description=non copy or move
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_UTIL_NONCOPY_H_
#define ARA_UCM_PKGMGR_UTIL_NONCOPY_H_

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief This class deletes copy constructor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10318
/// @trace_id_dd=DD_UCM_10697
/// @needwork = ad
/// @endcode
class NonCopy
{
protected:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10698
    /// @needwork = dda
    /// @endcode
    NonCopy() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10699
    /// @needwork = dda
    /// @endcode
    ~NonCopy() = default;

public:
    /// @brief delete copy constructor
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10700
    /// @needwork = dda
    /// @endcode
    NonCopy(NonCopy const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10701
    /// @needwork = dda
    /// @endcode
    NonCopy& operator=(NonCopy const& other) = delete;
    /// @brief default move constructor
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10702
    /// @needwork = dda
    /// @endcode
    NonCopy(NonCopy&& other) = default;
    /// @brief default move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10703
    /// @needwork = dda
    /// @endcode
    NonCopy& operator=(NonCopy&& other) = default;
};

/// @brief This class deletes copy constructor and move constructor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10319
/// @trace_id_dd=DD_UCM_10704
/// @needwork = ad
/// @endcode
class NonCopyNonMove
{
protected:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10705
    /// @needwork = dda
    /// @endcode
    NonCopyNonMove() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10706
    /// @needwork = dda
    /// @endcode
    ~NonCopyNonMove() = default;

public:
    /// @brief delete copy constructor
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10707
    /// @needwork = dda
    /// @endcode
    NonCopyNonMove(NonCopyNonMove const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10708
    /// @needwork = dda
    /// @endcode
    NonCopyNonMove& operator=(NonCopyNonMove const& other) = delete;
    /// @brief delete move constructor
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10709
    /// @needwork = dda
    /// @endcode
    NonCopyNonMove(NonCopyNonMove&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10710
    /// @needwork = dda
    /// @endcode
    NonCopyNonMove& operator=(NonCopyNonMove&& other) = delete;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_UTIL_NONCOPY_H_
