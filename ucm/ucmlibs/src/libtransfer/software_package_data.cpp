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
/// @file       software_package_data.cpp
/// @brief      The SoftwarePackageData class which stores data about software package in work.
/// @details
/// @date       2022-06-13
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00025
/// @unit_name=SoftwarePackageData
/// @unit_description=The SoftwarePackageData class which stores data about software package in work.
/// @endcode
///
/// ================================================================

#include "ara/ucm/internal/transfer/software_package_data.h"

#include "ara/ucm/internal/transfer/helper.h"
#include "ara/ucm/internal/transfer/transfer_instance.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief constructor
/// @param id
/// @param path
/// @param storage
/// @throws no
SoftwarePackageData::SoftwarePackageData(TransferIdType const& id,
                                         AraString const& path,
                                         std::unique_ptr< TransferStatusStorage > storage)
    : StreamableSoftwarePackage{}
    , transferID_{id}
    ,
    /// TODO(Han Zhibo): Get the processing directory from a global place, e.g.
    /// installDirectory + "/tmp"
    transfer_{std::make_unique< TransferInstance >(id, path, std::move(storage))}
    , access_{}
{
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
