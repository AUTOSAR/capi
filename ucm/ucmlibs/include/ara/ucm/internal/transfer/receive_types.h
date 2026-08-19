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
/// @file       receive_types.h
/// @brief      The various types which the transfer APIs use.
/// @details
/// @date       2023-10-26
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/UCMLib
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00025
/// @unit_name=receive_types
/// @unit_description=The various types which the transfer APIs use.
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_TRANSFER_RECEIVE_TYPES_H_
#define ARA_UCM_PKGMGR_TRANSFER_RECEIVE_TYPES_H_

#include <cstdint>

#include "ara/ucm/internal/types/impl_type_bytevectortype.h"
#include "ara/ucm/internal/types/impl_type_transferidtype.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief TransferStartSuccessType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00003
/// @trace_id_dd=DD_UCM_00272
/// @needwork = ad
/// @endcode
enum class TransferStartSuccessType : std::uint32_t
{
    kSuccess            = 0U,
    kInsufficientMemory = 1U
};

/// @brief TransferStartReturnType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00003
/// @trace_id_dd=DD_UCM_00273
/// @needwork = ad
/// @endcode
struct TransferStartReturnType
{
    /// @brief transferId
    TransferIdType transferId;
    /// @brief transferStartSuccess
    TransferStartSuccessType transferStartSuccess;
};

/// @brief TransferDataReturnType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00003
/// @trace_id_dd=DD_UCM_00274
/// @needwork = ad
/// @endcode
enum class TransferDataReturnType : std::uint32_t
{
    kSuccess               = 0U,
    kIncorrectBlock        = 1U,
    kIncorrectSize         = 2U,
    kInsufficientMemory    = 3U,
    kInvalidTransferId     = 4U,
    kOperationNotPermitted = 5U
};

/// @brief GeneralResponseType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00003
/// @trace_id_dd=DD_UCM_00275
/// @needwork = ad
/// @endcode
enum class GeneralResponseType : std::uint32_t
{
    kSuccess               = 0U,
    kGeneralReject         = 1U,
    kGeneralMemoryError    = 2U,
    kTransferIdInvalid     = 3U,
    kOperationNotPermitted = 4U
};

/// @brief TransferExitReturnType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00003
/// @trace_id_dd=DD_UCM_00276
/// @needwork = ad
/// @endcode
enum class TransferExitReturnType : std::uint32_t
{
    kSuccess,
    kInsufficientData,
    kPackageInconsistent,
    kInvalidTransferId,
    kOperationNotPermitted,
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_TRANSFER_RECEIVE_TYPES_H_
