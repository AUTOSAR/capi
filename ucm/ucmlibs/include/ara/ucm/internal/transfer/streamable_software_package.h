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
/// @file       streamable_software_package.h
/// @brief      The StreamableSoftwarePackage definition which is a interface of a streamable software package.
/// @details
/// @date       2023-10-26
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/UCMLib
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00025
/// @unit_name=StreamableSoftwarePackage
/// @unit_description=The StreamableSoftwarePackage definition which is a interface of a streamable software package.
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_TRANSFER_STREAMABLE_SOFTWARE_PACKAGE_H_
#define ARA_UCM_PKGMGR_TRANSFER_STREAMABLE_SOFTWARE_PACKAGE_H_

#include "streamable.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Interface of a streamable software package
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00003
/// @trace_id_dd=DD_UCM_00164
/// @needwork = ad
/// @endcode
class StreamableSoftwarePackage : public Streamable
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00165
    /// @needwork = dda
    /// @endcode
    StreamableSoftwarePackage() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00166
    /// @needwork = dda
    /// @endcode
    ~StreamableSoftwarePackage() override = default;

    /// @brief default copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00167
    /// @needwork = dda
    /// @endcode
    StreamableSoftwarePackage(StreamableSoftwarePackage const& other) = delete;
    /// @brief default copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00168
    /// @needwork = dda
    /// @endcode
    StreamableSoftwarePackage& operator=(StreamableSoftwarePackage const& other) = delete;
    /// @brief default move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00169
    /// @needwork = dda
    /// @endcode
    StreamableSoftwarePackage(StreamableSoftwarePackage&& other) = delete;
    /// @brief default move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00170
    /// @needwork = dda
    /// @endcode
    StreamableSoftwarePackage& operator=(StreamableSoftwarePackage&& other) = delete;

    /// @brief Get the ID of the software package.
    /// @return The ID of the software package.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00171
    /// @needwork = dda
    /// @endcode
    virtual TransferIdType const& GetID() const = 0;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_TRANSFER_STREAMABLE_SOFTWARE_PACKAGE_H_
