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
/// @file       impl_type_transferidtype.h
/// @brief      The TransferIdType definition.
/// @details
/// @date       2022-06-13
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/UCMLib
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00006
/// @unit_name=TransferIdType
/// @unit_description=TransferIdType definition
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_IMPL_TYPE_TRANSFERIDTYPE_H_
#define ARA_UCM_PKGMGR_IMPL_TYPE_TRANSFERIDTYPE_H_

#include <cstdint>

#include "ara/core/array.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief kTransferIdTypeSize
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00000
/// @trace_id_dd=DD_UCM_00143
/// @needwork = no
/// @endcode
constexpr std::size_t kTransferIdTypeSize{16U};
/// @brief TransferIdType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00003
/// @trace_id_dd=DD_UCM_00173
/// @needwork = no
/// @endcode
using TransferIdType = ara::core::Array< std::uint8_t, kTransferIdTypeSize >;

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
#endif  // ARA_UCM_PKGMGR_IMPL_TYPE_TRANSFERIDTYPE_H_
