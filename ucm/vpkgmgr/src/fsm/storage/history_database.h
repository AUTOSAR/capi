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
/// @file       history_database.h
/// @brief      HistoryDatabase impl
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
/// @unit_name=HistoryDatabase
/// @unit_description=HistoryDatabase impl
/// @endcode
///
/// ================================================================

#ifndef _VPKGMGR_HISTORY_DATABASE_H_
#define _VPKGMGR_HISTORY_DATABASE_H_

#include <ara/core/array.h>
#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <ara/log/logger.h>
#include <ara/per/file_storage.h>
#include <ara/per/key_value_storage.h>
#include <ara/per/per_error_domain.h>
#include <ara/ucm/pkgmgr/impl_type_campaignhistoryvectortype.h>
#include <ara/ucm/pkgmgr/impl_type_campaignstatetype.h>
#include <ara/ucm/pkgmgr/impl_type_packagemanagerstatustype.h>
#include <ara/ucm/pkgmgr/impl_type_transferidtype.h>

#include <atomic>

#include "consts.h"
#include "utils/single_ton.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief HistoryDatabase
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00118
/// @trace_id_dd=DD_UCM_Master_00350
/// @needwork = ad
/// @endcode
class HistoryDatabase : public Singleton< HistoryDatabase >
{
public:
    /// @brief Initialize
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00119
    /// @trace_id_dd=DD_UCM_Master_00351
    /// @needwork = ad
    /// @endcode
    bool Init();

    /// @brief Get a new CampaignKey
    /// @param campaignKey
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00120
    /// @trace_id_dd=DD_UCM_Master_00352
    /// @needwork = ad
    /// @endcode
    bool GetNewCampaignKey(ara::core::String& campaignKey);

    /// @brief Save Campaign repository
    /// @param campaignKey
    /// @param repository
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00121
    /// @trace_id_dd=DD_UCM_Master_00353
    /// @needwork = ad
    /// @endcode
    bool SaveRepository(ara::core::StringView const campaignKey, ara::core::String const& repository);

    /// @brief Save Campaign driverNotified
    /// @param campaignKey
    /// @param driverNotified
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00122
    /// @trace_id_dd=DD_UCM_Master_00354
    /// @needwork = ad
    /// @endcode
    bool SaveDriverNotified(ara::core::StringView const campaignKey, bool const driverNotified);

    /// @brief Save Campaign Resolution
    /// @param campaignKey
    /// @param resolution
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00123
    /// @trace_id_dd=DD_UCM_Master_00355
    /// @needwork = ad
    /// @endcode
    bool SaveResolution(ara::core::StringView const campaignKey, pkgmgr::UCMMasterResolutionType const resolution);

    /// @brief Record execution errors during the upgrade activity
    /// @param campaignKey
    /// @param ucmStepError
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00124
    /// @trace_id_dd=DD_UCM_Master_00356
    /// @needwork = ad
    /// @endcode
    bool SaveUcmStepError(ara::core::StringView const campaignKey, pkgmgr::UCMStepErrorType const& ucmStepError);

    /// @brief Set activity start time
    /// @param campaignKey
    /// @param startTime
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00125
    /// @trace_id_dd=DD_UCM_Master_00357
    /// @needwork = ad
    /// @endcode
    bool SaveCampaignStartTime(ara::core::StringView const campaignKey, uint64_t const startTime);

    /// @brief Set activity end time
    /// @param campaignKey
    /// @param resolutionTime
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00126
    /// @trace_id_dd=DD_UCM_Master_00358
    /// @needwork = ad
    /// @endcode
    bool SaveCampaignResolutionTime(ara::core::StringView const campaignKey, uint64_t const resolutionTime);

    /// @brief Save upgrade activity information to disk
    /// @param timestampGE
    /// @param timestampLT
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00127
    /// @trace_id_dd=DD_UCM_Master_00359
    /// @needwork = ad
    /// @endcode
    ara::core::Vector< pkgmgr::CampaignHistoryType > GetCampaignHistory(std::uint64_t const& timestampGE,
                                                                        std::uint64_t const& timestampLT);

    // Storage and retrieval of UCMTransferInfoType information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00128
    /// @trace_id_dd=DD_UCM_Master_00360
    /// @needwork = ad
    /// @endcode
    bool AddUCMTransferInfo(ara::core::String const& stUcmId, ::ara::ucm::pkgmgr::TransferIdType const& transferID);
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00129
    /// @trace_id_dd=DD_UCM_Master_00361
    /// @needwork = ad
    /// @endcode
    ara::core::Vector< ::ara::ucm::pkgmgr::TransferIdType > GetUCMTransferInfo(ara::core::String const& stUcmId);
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00130
    /// @trace_id_dd=DD_UCM_Master_00362
    /// @needwork = ad
    /// @endcode
    void ClearUCMTransferInfo(ara::core::String const& stUcmId);

private:
    /// @brief kvs_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00363
    /// @needwork = dda
    /// @endcode
    ara::per::SharedHandle< ara::per::KeyValueStorage > kvs_;

    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00364
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                  std::move(ara::core::StringView("HistoryDatabase context")),
                                                  ara::log::LogLevel::kVerbose)};
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // _VPKGMGR_HISTORY_DATABASE_H_
