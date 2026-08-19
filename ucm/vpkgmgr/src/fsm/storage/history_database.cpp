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
/// @file       history_database.cpp
/// @brief      HistoryDatabase impl
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=HistoryDatabase
/// @unit_description=HistoryDatabase impl
/// @endcode
///
/// ================================================================

#include "fsm/storage/history_database.h"

#include <rapidjson/istreamwrapper.h>

#include <fstream>
///#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/reader.h>
///#include <rapidjson/stringbuffer.h>
///#include <rapidjson/writer.h>

#include "utils/helper.h"
#include "utils/ucmm_log.h"
///#include "consts.h"
#include "ara/ucm/internal/transfer/helper.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Initialize
/// @return bool
/// @throws no
bool HistoryDatabase::Init()
{
    bool ret{false};
    log_.LogDebug() << "HistoryDatabase::Init, try to OpenKeyValueStorage with CAMPAIGN_HISTORY_KV_MODEL_IDENTIFIER:"
                    << CAMPAIGN_HISTORY_KV_MODEL_IDENTIFIER;
    ara::core::Result< ara::per::SharedHandle< ara::per::KeyValueStorage > > const resultOpen{
        ara::per::OpenKeyValueStorage(std::move(
            ara::core::InstanceSpecifier(std::move(ara::core::StringView(CAMPAIGN_HISTORY_KV_MODEL_IDENTIFIER)))))};
    if (!resultOpen.HasValue()) {
        log_.LogError() << "HistoryDatabase::Init, failed to ara::per::OpenKeyValueStorage with, errc:"
                        << resultOpen.Error().Message().data();
        ret = false;
    } else {
        kvs_ = resultOpen.Value();
        ret  = true;
    }

    log_.LogDebug() << "HistoryDatabase::Init(), end with ret:" << ret;
    return ret;
}

/// @brief Get a new CampaignKey
/// @param campaignKey
/// @return bool
/// @throws no
bool HistoryDatabase::GetNewCampaignKey(ara::core::String& campaignKey)
{
    bool ret{false};
    log_.LogDebug() << "HistoryDatabase::GetNewCampaignKey(), begin.";

    ara::core::Result< std::vector< ara::core::String > > const allKeysRes{kvs_->GetAllKeys()};
    if (allKeysRes.HasValue()) {
        std::size_t keysSize{allKeysRes.Value().size()};
        log_.LogDebug() << "HistoryDatabase::GetNewCampaignKey(), get keysSize:" << keysSize;

        char8_t newCampaignKey[kInt32_50U] = "";
        sprintf(newCampaignKey, "%s-%010ld", kCampaignHistoryKeyPrefix, keysSize);
        campaignKey = newCampaignKey;
        log_.LogDebug() << "HistoryDatabase::GetNewCampaignKey(), get campaignKey:" << campaignKey.c_str();
        ret = true;
    } else {
        log_.LogError() << "HistoryDatabase::GetNewCampaignKey(), failed to GetAllKeys with error:"
                        << allKeysRes.Error().Message().data();
        ret = false;
    }

    log_.LogDebug() << "HistoryDatabase::GetNewCampaignKey(), end with ret:" << ret;
    return ret;
}

/// @brief Save Campaign repository
/// @param campaignKey
/// @param repository
/// @return bool
/// @throws no
bool HistoryDatabase::SaveRepository(ara::core::StringView const campaignKey, ara::core::String const& repository)
{
    bool ret{false};
    log_.LogDebug() << "HistoryDatabase::SaveRepository(), begin with campaignKey:" << campaignKey
                    << "repository:" << repository;

    // Get original record
    pkgmgr::CampaignHistoryType record;
    record.campaignResult.campaignStartTime      = 0;
    record.campaignResult.campaignResolutionTime = 0;
    record.campaignResult.driverNotified         = false;

    if (kvs_->KeyExists(campaignKey)) {
        ara::core::Result< pkgmgr::CampaignHistoryType > const res{
            kvs_->GetValue< pkgmgr::CampaignHistoryType >(campaignKey)};
        if (res.HasValue()) {
            record = res.Value();
        }
    }

    // Update repository in it
    record.repository = repository;

    ara::core::Result< void > const setValueRes{kvs_->SetValue< pkgmgr::CampaignHistoryType >(campaignKey, record)};
    if (setValueRes.HasValue()) {
        kvs_->SyncToStorage();
        ret = true;
    } else {
        log_.LogError() << "HistoryDatabase::SaveRepository(), failed to SetValue with error:"
                        << setValueRes.Error().Message().data();
        ret = false;
    }

    log_.LogDebug() << "HistoryDatabase::SaveRepository(), end with ret:" << ret;
    return ret;
}

/// @brief Save Campaign driverNotified
/// @param campaignKey
/// @param driverNotified
/// @return bool
/// @throws no
bool HistoryDatabase::SaveDriverNotified(ara::core::StringView const campaignKey, bool const driverNotified)
{
    bool ret{false};
    log_.LogDebug() << "HistoryDatabase::SaveDriverNotified(), begin with campaignKey:" << campaignKey
                    << "driverNotified:" << driverNotified;

    // Get original record
    pkgmgr::CampaignHistoryType record;
    record.campaignResult.campaignStartTime      = 0;
    record.campaignResult.campaignResolutionTime = 0;
    record.campaignResult.driverNotified         = false;

    if (kvs_->KeyExists(campaignKey)) {
        ara::core::Result< pkgmgr::CampaignHistoryType > const res{
            kvs_->GetValue< pkgmgr::CampaignHistoryType >(campaignKey)};
        if (res.HasValue()) {
            record = res.Value();
        }
    }

    // Update driverNotified in it
    record.campaignResult.driverNotified = driverNotified;

    ara::core::Result< void > const setValueRes{kvs_->SetValue< pkgmgr::CampaignHistoryType >(campaignKey, record)};
    if (setValueRes.HasValue()) {
        kvs_->SyncToStorage();
        ret = true;
    } else {
        log_.LogError() << "HistoryDatabase::SaveDriverNotified(), failed to SetValue with error:"
                        << setValueRes.Error().Message().data();
        ret = false;
    }

    log_.LogDebug() << "HistoryDatabase::SaveDriverNotified(), end with ret:" << ret;
    return ret;
}

/// @brief Save Campaign Resolution
/// @param campaignKey
/// @param resolution
/// @return bool
/// @throws no
bool HistoryDatabase::SaveResolution(ara::core::StringView const campaignKey,
                                     pkgmgr::UCMMasterResolutionType const resolution)
{
    bool ret{false};
    core::String padding;
    log_.LogDebug() << "HistoryDatabase::SaveResolution(), begin with campaignKey:" << campaignKey << "resolution:"
                    << com::internal::format::Formatter< core::String, pkgmgr::UCMMasterResolutionType >::ToString(
                           padding, resolution);
    // Get original record
    pkgmgr::CampaignHistoryType record;
    record.campaignResult.campaignStartTime      = 0;
    record.campaignResult.campaignResolutionTime = 0;
    record.campaignResult.driverNotified         = false;

    if (kvs_->KeyExists(campaignKey)) {
        ara::core::Result< pkgmgr::CampaignHistoryType > const res{
            kvs_->GetValue< pkgmgr::CampaignHistoryType >(campaignKey)};
        if (res.HasValue()) {
            record = res.Value();
        }
    }

    // Update resolution in it
    record.campaignResult.campaignResolution.push_back(resolution);

    ara::core::Result< void > const setValueRes{kvs_->SetValue< pkgmgr::CampaignHistoryType >(campaignKey, record)};
    if (setValueRes.HasValue()) {
        kvs_->SyncToStorage();
        ret = true;
    } else {
        log_.LogError() << "HistoryDatabase::SaveResolution(), failed to SetValue with error:"
                        << setValueRes.Error().Message().data();
        ret = false;
    }

    log_.LogDebug() << "HistoryDatabase::SaveResolution(), end with ret:" << ret;
    return ret;
}

/// @brief Record execution errors during the upgrade activity
/// @param campaignKey
/// @param ucmStepError
/// @return bool
/// @throws no
bool HistoryDatabase::SaveUcmStepError(ara::core::StringView const campaignKey,
                                       pkgmgr::UCMStepErrorType const& ucmStepError)
{
    bool ret{false};
    log_.LogDebug() << "HistoryDatabase::SaveUcmStepError(), begin with campaignKey:" << campaignKey
                    << "UcmStepError:" << helper::UCMStepErrorTypeToString(ucmStepError);

    // Get original record
    pkgmgr::CampaignHistoryType record;
    record.campaignResult.campaignStartTime      = 0;
    record.campaignResult.campaignResolutionTime = 0;
    record.campaignResult.driverNotified         = false;

    if (kvs_->KeyExists(campaignKey)) {
        ara::core::Result< pkgmgr::CampaignHistoryType > const res{
            kvs_->GetValue< pkgmgr::CampaignHistoryType >(campaignKey)};
        if (res.HasValue()) {
            record = res.Value();
        }
    }

    // Update UcmStepError in it
    record.campaignResult.UCMStepError.push_back(ucmStepError);

    ara::core::Result< void > const setValueRes{kvs_->SetValue< pkgmgr::CampaignHistoryType >(campaignKey, record)};
    if (setValueRes.HasValue()) {
        kvs_->SyncToStorage();
        ret = true;
    } else {
        log_.LogError() << "HistoryDatabase::SaveUcmStepError(), failed to SetValue with error:"
                        << setValueRes.Error().Message().data();
        ret = false;
    }

    log_.LogDebug() << "HistoryDatabase::SaveUcmStepError(), end with ret:" << ret;
    return ret;
}

/// @brief Set activity start time
/// @param campaignKey
/// @param startTime
/// @return bool
/// @throws no
bool HistoryDatabase::SaveCampaignStartTime(ara::core::StringView const campaignKey, uint64_t const startTime)
{
    bool ret{false};
    log_.LogDebug() << "HistoryDatabase::SaveCampaignStartTime(), begin with campaignKey:" << campaignKey
                    << "startTime:" << startTime;

    // Get original record
    pkgmgr::CampaignHistoryType record;
    record.campaignResult.campaignStartTime      = 0;
    record.campaignResult.campaignResolutionTime = 0;
    record.campaignResult.driverNotified         = false;

    if (kvs_->KeyExists(campaignKey)) {
        ara::core::Result< pkgmgr::CampaignHistoryType > const res{
            kvs_->GetValue< pkgmgr::CampaignHistoryType >(campaignKey)};
        if (res.HasValue()) {
            record = res.Value();
        }
    }

    // Update campaignStartTime in it
    record.campaignResult.campaignStartTime = startTime;

    ara::core::Result< void > const setValueRes{kvs_->SetValue< pkgmgr::CampaignHistoryType >(campaignKey, record)};
    if (setValueRes.HasValue()) {
        kvs_->SyncToStorage();
        ret = true;
    } else {
        log_.LogError() << "HistoryDatabase::SaveCampaignStartTime(), failed to SetValue with error:"
                        << setValueRes.Error().Message().data();
        ret = false;
    }

    log_.LogDebug() << "HistoryDatabase::SaveCampaignStartTime(), end with ret:" << ret;
    return ret;
}

/// @brief Set activity end time
/// @param campaignKey
/// @param resolutionTime
/// @return bool
/// @throws no
bool HistoryDatabase::SaveCampaignResolutionTime(ara::core::StringView const campaignKey, uint64_t const resolutionTime)
{
    bool ret{false};
    log_.LogDebug() << "HistoryDatabase::SaveCampaignResolutionTime(), begin with campaignKey:" << campaignKey
                    << "resolutionTime:" << resolutionTime;

    // Get original record
    pkgmgr::CampaignHistoryType record;
    record.campaignResult.campaignStartTime      = 0;
    record.campaignResult.campaignResolutionTime = 0;
    record.campaignResult.driverNotified         = false;

    if (kvs_->KeyExists(campaignKey)) {
        ara::core::Result< pkgmgr::CampaignHistoryType > const res{
            kvs_->GetValue< pkgmgr::CampaignHistoryType >(campaignKey)};
        if (res.HasValue()) {
            record = res.Value();
        }
    }

    // Update campaignResolutionTime in it
    record.campaignResult.campaignResolutionTime = resolutionTime;

    ara::core::Result< void > const setValueRes{kvs_->SetValue< pkgmgr::CampaignHistoryType >(campaignKey, record)};
    if (setValueRes.HasValue()) {
        kvs_->SyncToStorage();
        ret = true;
    } else {
        log_.LogError() << "HistoryDatabase::SaveCampaignResolutionTime(), failed to SetValue with error:"
                        << setValueRes.Error().Message().data();
        ret = false;
    }

    log_.LogDebug() << "HistoryDatabase::SaveCampaignResolutionTime(), end with ret:" << ret;
    return ret;
}

/// @brief Save upgrade activity information to disk
/// @param timestampGE
/// @param timestampLT
/// @return bool
/// @throws no
ara::core::Vector< pkgmgr::CampaignHistoryType > HistoryDatabase::GetCampaignHistory(std::uint64_t const& timestampGE,
                                                                                     std::uint64_t const& timestampLT)
{
    log_.LogDebug() << "HistoryDatabase::GetCampaignHistory(), begin with timestampGE:" << timestampGE
                    << "timestampLT:" << timestampLT;
    // Return result
    ara::core::Vector< pkgmgr::CampaignHistoryType > out{};

    ara::core::Result< std::vector< ara::core::String > > const allKeysRes{kvs_->GetAllKeys()};
    if (allKeysRes.HasValue()) {
        for (ara::core::String const& campaignKey : allKeysRes.Value()) {
            log_.LogVerbose() << "HistoryDatabase::GetCampaignHistory(), get a campaignKey:" << campaignKey.c_str();

            if (kvs_->KeyExists(campaignKey).HasValue()) {
                ara::core::Result< pkgmgr::CampaignHistoryType > const res{
                    kvs_->GetValue< pkgmgr::CampaignHistoryType >(campaignKey)};
                if (res.HasValue()) {
                    pkgmgr::CampaignHistoryType const& record{res.Value()};
                    log_.LogVerbose() << "HistoryDatabase::GetCampaignHistory(), get a record:"
                                      << helper::CampaignHistoryTypeToString(record).data();

                    if ((record.campaignResult.campaignResolutionTime > 0)
                        && (record.campaignResult.campaignResolutionTime >= timestampGE)
                        && (record.campaignResult.campaignResolutionTime < timestampLT)) {  // Compare times
                        log_.LogVerbose() << "HistoryDatabase::GetCampaignHistory(), push_back a record:"
                                          << helper::CampaignHistoryTypeToString(record).data();
                        out.push_back(record);
                    }
                }
            }
        }
    } else {
        log_.LogWarn() << "HistoryDatabase::GetCampaignHistory(), failed to GetAllKeys with error:"
                       << allKeysRes.Error().Message().data();
    }

    log_.LogDebug() << "HistoryDatabase::GetCampaignHistory(), end with out.size():" << out.size();
    return out;
}

void HistoryDatabase::ClearUCMTransferInfo(ara::core::String const& stUcmId)
{
    log_.LogDebug() << "HistoryDatabase::ClearUCMTransferInfo(), begin with UCMId:" << stUcmId.c_str();

    // Clear information
    kvs_->RemoveKey(core::String(kUCMTransferInfoKeyPrefix) + stUcmId.c_str());

    log_.LogDebug() << "HistoryDatabase::ClearUCMTransferInfo(), end.";
}

bool HistoryDatabase::AddUCMTransferInfo(ara::core::String const& stUcmId,
                                         ::ara::ucm::pkgmgr::TransferIdType const& transferID)
{
    bool ret{false};
    log_.LogDebug() << "HistoryDatabase::AddUCMTransferInfo(), begin with UCMId:" << stUcmId.c_str()
                    << "TransferID:" << pkgmgr::helper::ToHexString(transferID).c_str();

    // Get original record
    ara::core::Vector< ::ara::ucm::pkgmgr::TransferIdType > record;
    if (kvs_->KeyExists(core::String(kUCMTransferInfoKeyPrefix) + stUcmId.c_str())) {
        ara::core::Result< ara::core::Vector< ::ara::ucm::pkgmgr::TransferIdType > > const res{
            kvs_->GetValue< ara::core::Vector< ::ara::ucm::pkgmgr::TransferIdType > >(
                core::String(kUCMTransferInfoKeyPrefix) + stUcmId.c_str())};
        if (res.HasValue()) {
            record = res.Value();
        }
    }

    // Add information
    record.push_back(transferID);
    ara::core::Result< void > const setValueRes{
        kvs_->SetValue< ara::core::Vector< ::ara::ucm::pkgmgr::TransferIdType > >(
            core::String(kUCMTransferInfoKeyPrefix) + stUcmId.c_str(), record)};
    if (setValueRes.HasValue()) {
        kvs_->SyncToStorage();
        ret = true;
    } else {
        log_.LogError() << "HistoryDatabase::AddUCMTransferInfo(), failed to SetValue with error:"
                        << setValueRes.Error().Message().data();
        ret = false;
    }

    log_.LogDebug() << "HistoryDatabase::AddUCMTransferInfo(), end with ret:" << ret;
    return ret;
}

ara::core::Vector< ::ara::ucm::pkgmgr::TransferIdType > HistoryDatabase::GetUCMTransferInfo(
    ara::core::String const& stUcmId)
{
    log_.LogDebug() << "HistoryDatabase::GetUCMTransferInfo(), begin with UCMId:" << stUcmId.c_str();

    // Get original record
    ara::core::Vector< ::ara::ucm::pkgmgr::TransferIdType > record;
    if (kvs_->KeyExists(core::String(kUCMTransferInfoKeyPrefix) + stUcmId.c_str())) {
        ara::core::Result< ara::core::Vector< ::ara::ucm::pkgmgr::TransferIdType > > const res{
            kvs_->GetValue< ara::core::Vector< ::ara::ucm::pkgmgr::TransferIdType > >(
                core::String(kUCMTransferInfoKeyPrefix) + stUcmId.c_str())};
        if (res.HasValue()) {
            record = std::move(res.Value());
        }
    }

    log_.LogDebug() << "HistoryDatabase::GetUCMTransferInfo(), end.";
    return record;
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
