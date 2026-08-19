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
/// @file       swp_step_transfer.h
/// @brief      TransferStep impl
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=TransferStep
/// @unit_description=TransferStep impl
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_VPKGMGR_TRANSFER_STEP_H_
#define ARA_UCM_VPKGMGR_TRANSFER_STEP_H_
#include <ara/core/error_code.h>
#include <ara/core/map.h>
#include <ara/core/promise.h>
#include <ara/core/result.h>
#include <ara/log/log_stream.h>
#include <ara/log/logger.h>

#include <memory>

#include "ara/ucm/pkgmgr/impl_type_swnametype.h"
#include "ara/ucm/pkgmgr/impl_type_transferidtype.h"
#include "fsm/parsing/step_info.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Transfer step
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00325
/// @trace_id_dd=DD_UCM_Master_00659
/// @needwork = ad
/// @endcode
class TransferStep final
{
public:
    /// @brief Define alias
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using Ptr = std::shared_ptr< TransferStep >;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00326
    /// @trace_id_dd=DD_UCM_Master_00660
    /// @needwork = ad
    /// @endcode
    TransferStep() = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00327
    /// @trace_id_dd=DD_UCM_Master_00661
    /// @needwork = ad
    /// @endcode
    ~TransferStep() = default;

    /// @brief Copy constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00328
    /// @trace_id_dd=DD_UCM_Master_00662
    /// @needwork = ad
    /// @endcode
    TransferStep(TransferStep const& other) = delete;
    /// @brief Move constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00329
    /// @trace_id_dd=DD_UCM_Master_00663
    /// @needwork = ad
    /// @endcode
    TransferStep(TransferStep&& other) = delete;
    /// @brief Copy assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00330
    /// @trace_id_dd=DD_UCM_Master_00664
    /// @needwork = ad
    /// @endcode
    TransferStep& operator=(TransferStep const& other) = delete;
    /// @brief Move assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00331
    /// @trace_id_dd=DD_UCM_Master_00665
    /// @needwork = ad
    /// @endcode
    TransferStep& operator=(TransferStep&& other) = delete;

    /// @brief Load configuration
    /// @param transferInfo Configuration related to transferring the software package
    /// @param ucmId
    /// @param storing
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00332
    /// @trace_id_dd=DD_UCM_Master_00666
    /// @needwork = ad
    /// @endcode
    void Load(TransferStepInfoPtr const& transferInfo,
              ara::core::String const& ucmId,
              SoftwarePackageStoringEnum const storing);

    /// @brief Execute TransferThread in a separate thread
    /// @param ucmId
    /// @param swclNameToTid
    /// @param promise
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00333
    /// @trace_id_dd=DD_UCM_Master_00667
    /// @needwork = ad
    /// @endcode
    void TransferThread(ara::core::String const& ucmId,
                        ara::core::Map< ara::core::String, std::pair< bool, pkgmgr::TransferIdType > >* swclNameToTid,
                        ara::core::Promise< void >&& promise);

    /// @brief GetSpkgRef
    /// @return spkgRef_
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00334
    /// @trace_id_dd=DD_UCM_Master_00668
    /// @needwork = ad
    /// @endcode
    ara::core::String GetSpkgRef() const { return spkgRef_; }

private:
    /// @brief Execute the Transfer step, i.e. trigger UCM Subordinate to transfer the SwPackage identified by the
    /// current Transfer ID
    ///
    /// @param ucmId The ID of the UCM subordinate that shall compute the action
    /// @param swclNameToTid Map that link a software cluster name to a Transfer ID
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00669
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > _execute(
        ara::core::String const& ucmId,
        ara::core::Map< ara::core::String, std::pair< bool, pkgmgr::TransferIdType > >& swclNameToTid);

private:
    /// @brief storing_
    /// @code{.isoft}
    /// reference to the Software Package to be transferred
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00670
    /// @needwork = dda
    /// @endcode
    SoftwarePackageStoringEnum storing_{SoftwarePackageStoringEnum::kNone};
    /// @brief spkgRef_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00671
    /// @needwork = dda
    /// @endcode
    ara::core::String spkgRef_{""};

    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00672
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                  std::move(ara::core::StringView("TransferStep context")),
                                                  ara::log::LogLevel::kVerbose)};
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_VPKGMGR_TRANSFER_STEP_H_
