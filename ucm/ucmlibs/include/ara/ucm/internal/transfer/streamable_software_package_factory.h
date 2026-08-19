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
/// @file       streamable_software_package_factory.h
/// @brief      The SoftwarePackageData class which creates software package data implementations as generic interface.
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
/// @unit_name=StreamableSoftwarePackageFactory
/// @unit_description=The SoftwarePackageData class which creates software package data implementations as generic interface.
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_TRANSFER_STREAMABLE_SOFTWARE_PACKAGE_FACTORY_H_
#define ARA_UCM_PKGMGR_TRANSFER_STREAMABLE_SOFTWARE_PACKAGE_FACTORY_H_

#include <memory>

#include "streamable_software_package.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Generic interface for creating software package data implementations.
///
/// The implementations created by this factory are used to parametrize
/// PackageManager service implementations. A factory is used to support different implementation approaches.
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00002
/// @trace_id_dd=DD_UCM_00291
/// @needwork = ad
/// @endcode
class StreamableSoftwarePackageFactory  // NOLINT
{
public:
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00002
    /// @trace_id_dd=DD_UCM_00292
    /// @needwork = dda
    /// @endcode
    virtual ~StreamableSoftwarePackageFactory() = default;

    /// @brief Create a new Streamable Software Package Implementation.
    /// @param id The transfer ID.
    /// @param kVModelIdentifier The ModelIdentifier of the KV used to persist the transfer status.
    /// @return A unique pointer to the created StreamableSoftwarePackage implementation.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00002
    /// @trace_id_dd=DD_UCM_00293
    /// @needwork = dda
    /// @endcode
    virtual std::unique_ptr< StreamableSoftwarePackage > Create(TransferIdType id, AraStringView kVModelIdentifier) = 0;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_TRANSFER_STREAMABLE_SOFTWARE_PACKAGE_FACTORY_H_
