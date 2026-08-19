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
/// @file       persistence.h
/// @brief      Persistence impl
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
/// @unit_name=Persistence
/// @unit_description=Persistence impl
/// @endcode
///
/// ================================================================

#ifndef _VPKGMGR_COMMON_CONFIG_H_
#define _VPKGMGR_COMMON_CONFIG_H_

#include <ara/core/array.h>
#include <ara/core/string.h>
#include <ara/log/log_stream.h>
#include <ara/log/logger.h>
#include <ara/ucm/pkgmgr/impl_type_campaignstatetype.h>

#include "consts.h"
#include "fsm/fsm_manager.h"
#include "utils/single_ton.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Persistence
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00131
/// @trace_id_dd=DD_UCM_Master_00365
/// @needwork = ad
/// @endcode
class Persistence : public Singleton< Persistence >
{
public:
    ///virtual ~Persistence() = default;

    /// @brief BeginSaveTransaction
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00132
    /// @trace_id_dd=DD_UCM_Master_00366
    /// @needwork = ad
    /// @endcode
    void BeginSaveTransaction() const;

    /// @brief SetCurrentState
    /// @param version
    /// @param preCampaignState
    /// @param campaignState
    /// @param vpkTransferId
    /// @param beginFinish
    /// @param lastHeatBeatTime
    /// @param allowCampaign
    /// @param swpkFQN2TransferIDMap
    /// @param campaignHistoryKey
    /// @param cancelCause
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00133
    /// @trace_id_dd=DD_UCM_Master_00367
    /// @needwork = ad
    /// @endcode
    void SetCurrentState(core::String const& version,
                         pkgmgr::CampaignStateType const preCampaignState,
                         pkgmgr::CampaignStateType const campaignState,
                         pkgmgr::TransferIdType const* const vpkTransferId,
                         bool const beginFinish,
                         std::uint64_t const lastHeatBeatTime,
                         bool const allowCampaign,
                         ara::core::Map< ara::core::String, pkgmgr::TransferIdType > const& swpkFQN2TransferIDMap,
                         core::String const& campaignHistoryKey,
                         pkgmgr::UCMMasterResolutionType const cancelCause) const;

    /// @brief CommitSaveTransaction
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00134
    /// @trace_id_dd=DD_UCM_Master_00368
    /// @needwork = ad
    /// @endcode
    void CommitSaveTransaction() const;

    /// @brief RetrieveState
    /// @param version
    /// @param preCampaignState
    /// @param campaignState
    /// @param vpkTransferId
    /// @param beginFinish
    /// @param lastHeatBeatTime
    /// @param allowCampaign
    /// @param swpkFQN2TransferIDMap
    /// @param campaignHistoryKey
    /// @param cancelCause
    /// @return bool
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00135
    /// @trace_id_dd=DD_UCM_Master_00369
    /// @needwork = ad
    /// @endcode
    bool RetrieveState(core::String& version,
                       pkgmgr::CampaignStateType& preCampaignState,
                       pkgmgr::CampaignStateType& campaignState,
                       std::unique_ptr< pkgmgr::TransferIdType >& vpkTransferId,
                       bool& beginFinish,
                       std::uint64_t& lastHeatBeatTime,
                       std::atomic< bool >& allowCampaign,
                       ara::core::Map< ara::core::String, pkgmgr::TransferIdType >& swpkFQN2TransferIDMap,
                       core::String& campaignHistoryKey,
                       pkgmgr::UCMMasterResolutionType& cancelCause) const;

    /// @brief GetLastCampaginState
    /// @return CampaignStateType
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00136
    /// @trace_id_dd=DD_UCM_Master_00370
    /// @needwork = ad
    /// @endcode
    inline pkgmgr::CampaignStateType GetLastCampaginState() const noexcept { return campaignState_; }

private:
    /// @brief Current activity state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00371
    /// @needwork = dda
    /// @endcode
    pkgmgr::CampaignStateType campaignState_{pkgmgr::CampaignStateType::kIdle};
    /// @brief Local save path of the vehicle package zip file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00372
    /// @needwork = dda
    /// @endcode
    ara::core::String vpkgZipName_;  // Currently not used

    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00373
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                  std::move(ara::core::StringView("Persistence context")),
                                                  ara::log::LogLevel::kVerbose)};
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // _VPKGMGR_COMMON_CONFIG_H_