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
/// @file       persistence.cpp
/// @brief      Persistence impl
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
/// @unit_name=Persistence
/// @unit_description=Persistence impl
/// @endcode
///
/// ================================================================

#include "fsm/storage/persistence.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/manifestreader/manifest_reader.h>
#include <isoft/manifestreader/manifestreader_error_domain.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>

#include "ara/ucm/internal/extraction/tinyfs.h"
#include "fsm/fsm_manager.h"
#include "fsm/states/campaign_state_base.h"
#include "utils/helper.h"
#include "utils/ucmm_log.h"
#include "utils/utils.h"
#include "utils/version.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

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
/// @return bool
/// @throw
void Persistence::SetCurrentState(
    core::String const& version,
    pkgmgr::CampaignStateType const preCampaignState,
    pkgmgr::CampaignStateType const campaignState,
    pkgmgr::TransferIdType const* const vpkTransferId,
    bool const beginFinish,
    std::uint64_t const lastHeatBeatTime,
    bool const allowCampaign,
    ara::core::Map< ara::core::String, pkgmgr::TransferIdType > const& swpkFQN2TransferIDMap,
    core::String const& campaignHistoryKey,
    pkgmgr::UCMMasterResolutionType const cancelCause) const
{
    core::String padding;
    core::String vpkTransferIdStr;
    if (nullptr != vpkTransferId) {
        vpkTransferIdStr = pkgmgr::helper::ToHexString(*vpkTransferId);
    }
    log_.LogDebug() << "Persistence::SetCurrentState(), begin with version:" << version.c_str()
                    << " preCampaignState:" << CampaignStateTypeToString(preCampaignState).data()
                    << " campaignState:" << CampaignStateTypeToString(campaignState).data()
                    << " vpkTransferId:" << vpkTransferIdStr.c_str() << " beginFinish:" << beginFinish
                    << " lastHeatBeatTime:" << lastHeatBeatTime << " allowCampaign:" << allowCampaign
                    << " swpkFQN2TransferIDMap:" << Utils::ConcatenateStrings(swpkFQN2TransferIDMap).c_str()
                    << " campaignHistoryKey:" << campaignHistoryKey.c_str() << " cancelCause:"
                    << com::internal::format::Formatter< core::String, pkgmgr::UCMMasterResolutionType >::ToString(
                           padding, cancelCause)
                           .c_str();

    // ucmm status file path
    ara::core::String const statusFilePath{FsmManager::GetInstance()->GetStatusFilePath()};
    log_.LogDebug() << "Persistence::SetCurrentState(), statusFilePath:" << statusFilePath.c_str();

    /// {
    /// 	"Version": "1.2.0",
    /// 	"CampaignState": "Idle",
    /// 	"VPKTransferID": "",
    /// 	"BeginFinish": "0",         // Valid when CampaignState is VEHICLE_CHECKING, indicates whether Finish has started after all ucms are Activated
    /// 	"LastHeatBeatTime": "0",    // Invalid in IDLE and SYNCING states
    /// 	"AllowCampaign": "1",
    ///  "SWPKGs":
    ///      [
    ///      ]
    /// }

    rapidjson::Document statusDom;
    std::ignore = statusDom.SetObject();

    // Version
    std::ignore = statusDom.AddMember(rapidjson::Value(kUCMMVersion, statusDom.GetAllocator()).Move(),
                                      rapidjson::Value(version.c_str(), statusDom.GetAllocator()).Move(),
                                      statusDom.GetAllocator());

    // PreCampaignState
    std::ignore = statusDom.AddMember(rapidjson::Value(kUCMMPreCampaignState, statusDom.GetAllocator()).Move(),
                                      rapidjson::Value(static_cast< uint64_t >(preCampaignState)).Move(),
                                      statusDom.GetAllocator());

    // CampaignState
    std::ignore = statusDom.AddMember(rapidjson::Value(kUCMMCampaignState, statusDom.GetAllocator()).Move(),
                                      rapidjson::Value(static_cast< uint64_t >(campaignState)).Move(),
                                      statusDom.GetAllocator());

    // VPKTransferID
    std::ignore = statusDom.AddMember(rapidjson::Value(kUCMMVPKTransferID, statusDom.GetAllocator()).Move(),
                                      rapidjson::Value(vpkTransferIdStr.c_str(), statusDom.GetAllocator()).Move(),
                                      statusDom.GetAllocator());

    // BeginFinish
    std::ignore
        = statusDom.AddMember(rapidjson::Value(kUCMMBeginFinish, statusDom.GetAllocator()).Move(),
                              rapidjson::Value(static_cast< uint64_t >(beginFinish)).Move(), statusDom.GetAllocator());

    // LastHeatBeatTime
    std::ignore = statusDom.AddMember(rapidjson::Value(kUCMMLastHeatBeatTime, statusDom.GetAllocator()).Move(),
                                      rapidjson::Value(static_cast< uint64_t >(lastHeatBeatTime)).Move(),
                                      statusDom.GetAllocator());

    // AllowCampaign
    std::ignore = statusDom.AddMember(rapidjson::Value(kUCMMAllowCampaign, statusDom.GetAllocator()).Move(),
                                      rapidjson::Value(static_cast< uint64_t >(allowCampaign)).Move(),
                                      statusDom.GetAllocator());
    // SWPKGs
    rapidjson::Value transferredSoftwarePackagesDom{};
    std::ignore = transferredSoftwarePackagesDom.SetArray();
    for (auto const& it : swpkFQN2TransferIDMap) {
        log_.LogDebug() << "Persistence::SetCurrentState(), get swpkFQN:" << it.first.c_str()
                        << "TransferID:" << pkgmgr::helper::ToHexString(it.second).c_str();
        rapidjson::Value transferredSoftwarePackage{rapidjson::Type::kObjectType};

        // SWPFQN
        std::ignore = transferredSoftwarePackage.AddMember(
            rapidjson::Value(kUCMMSWPFQN, statusDom.GetAllocator()).Move(),
            rapidjson::Value(it.first.c_str(), statusDom.GetAllocator()).Move(), statusDom.GetAllocator());

        // TransferID
        std::ignore = transferredSoftwarePackage.AddMember(
            rapidjson::Value(kUCMMTransferID, statusDom.GetAllocator()).Move(),
            rapidjson::Value(pkgmgr::helper::ToHexString(it.second).c_str(), statusDom.GetAllocator()).Move(),
            statusDom.GetAllocator());
        std::ignore = transferredSoftwarePackagesDom.PushBack(transferredSoftwarePackage, statusDom.GetAllocator());
    }
    std::ignore = statusDom.AddMember(rapidjson::Value(kUCMMSWPKGs, statusDom.GetAllocator()).Move(),
                                      transferredSoftwarePackagesDom, statusDom.GetAllocator());

    // CampaignHistoryKey
    std::ignore = statusDom.AddMember(rapidjson::Value(kUCMMCampaignHistoryKey, statusDom.GetAllocator()).Move(),
                                      rapidjson::Value(campaignHistoryKey.c_str(), statusDom.GetAllocator()).Move(),
                                      statusDom.GetAllocator());

    // CancelCause
    std::ignore
        = statusDom.AddMember(rapidjson::Value(kUCMMCancelCause, statusDom.GetAllocator()).Move(),
                              rapidjson::Value(static_cast< uint64_t >(cancelCause)).Move(), statusDom.GetAllocator());

    std::ofstream ofs{statusFilePath.c_str()};
    rapidjson::OStreamWrapper osw{ofs};

    rapidjson::Writer< rapidjson::OStreamWrapper > writer{osw};
    std::ignore = statusDom.Accept(writer);

    log_.LogDebug() << "Persistence::SetCurrentState, end. ucm status is successfully updated to statusFilePath:"
                    << statusFilePath.c_str();
}

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
/// @throw
bool Persistence::RetrieveState(core::String& version,
                                pkgmgr::CampaignStateType& preCampaignState,
                                pkgmgr::CampaignStateType& campaignState,
                                std::unique_ptr< pkgmgr::TransferIdType >& vpkTransferId,
                                bool& beginFinish,
                                std::uint64_t& lastHeatBeatTime,
                                std::atomic< bool >& allowCampaign,
                                ara::core::Map< ara::core::String, pkgmgr::TransferIdType >& swpkFQN2TransferIDMap,
                                core::String& campaignHistoryKey,
                                pkgmgr::UCMMasterResolutionType& cancelCause) const
{
    std::ignore = version;
    std::ignore = vpkTransferId;
    std::ignore = lastHeatBeatTime;
    std::ignore = campaignHistoryKey;
    ara::core::String const statusFilePath{FsmManager::GetInstance()->GetStatusFilePath()};
    log_.LogDebug() << "Persistence::RetrieveState(), begin with statusFilePath:" << statusFilePath.c_str();

    // Read Version field, assert it is the current version ucmVersion_:1.0.0
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > ret{
        isoft::manifestreader::OpenManifest(std::move(ara::core::StringView(statusFilePath)))};
    assert(ret.HasValue());

    std::unique_ptr< isoft::manifestreader::Manifest > const doc{std::move(std::move(ret).Value())};

    /// "Version": "1.0.0",
    version = doc->GetValue< ara::core::String >(std::move(ara::core::StringView(kUCMMVersion)), ara::core::String(""));
    log_.LogDebug() << "Persistence::RetrieveState, got version:" << version.c_str()
                    << "and UCMM_Version:" << kUcmmVersion;

    // Version info in the status file
    Version const statusVersion{version};
    Version const ucmVersion{kUcmmVersion};
    if (statusVersion <= ucmVersion) {  // Version in status file is less than or equal to ucmm version
        // No conversion needed for now
    } else {  // Version in status file is greater than ucmm version
        log_.LogFatal() << "Persistence::RetrieveState, version is greater than UCMM_Version, so just exit.";

        // TODO(Han Zhibo): Modify to return appropriate value to exit normally
        std::exit(-1);  // NOLINT : function is not thread safe
    }

    /// "PreCampaignState":0,
    *reinterpret_cast< uint8_t* >(&(preCampaignState))
        = static_cast< uint8_t >(doc->GetValue(std::move(ara::core::StringView(kUCMMPreCampaignState)), uint64_t{0U}));
    log_.LogDebug() << "Persistence::RetrieveState, got preCampaignState:"
                    << CampaignStateTypeToString(preCampaignState).data();

    /// "CampaignState":0,
    *reinterpret_cast< uint8_t* >(&(campaignState))
        = static_cast< uint8_t >(doc->GetValue(std::move(ara::core::StringView(kUCMMCampaignState)), uint64_t{0U}));
    log_.LogDebug() << "Persistence::RetrieveState, got campaignState:"
                    << CampaignStateTypeToString(campaignState).data();

    // VPKTransferID
    core::String const stVPKTransferIDStr{doc->GetValue< ara::core::String >(
        std::move(ara::core::StringView(kUCMMVPKTransferID)), ara::core::String(""))};
    log_.LogDebug() << "Persistence::RetrieveState, got stVPKTransferIDStr:" << stVPKTransferIDStr.c_str();
    if (stVPKTransferIDStr.empty()) {
        vpkTransferId = nullptr;
    } else {
        vpkTransferId
            = std::make_unique< pkgmgr::TransferIdType >(pkgmgr::helper::HexStrToTransferId(stVPKTransferIDStr));
    }

    // BeginFinish
    *reinterpret_cast< uint8_t* >(&(beginFinish))
        = static_cast< uint8_t >(doc->GetValue(std::move(ara::core::StringView(kUCMMBeginFinish)), uint64_t{0U}));
    log_.LogDebug() << "Persistence::RetrieveState, got beginFinish:" << beginFinish;

    // LastHeatBeatTime
    lastHeatBeatTime = doc->GetValue(std::move(ara::core::StringView(kUCMMLastHeatBeatTime)), uint64_t{0U});
    log_.LogDebug() << "Persistence::RetrieveState, got lastHeatBeatTime:" << lastHeatBeatTime;

    // AllowCampaign
    *reinterpret_cast< uint8_t* >(&(allowCampaign))
        = static_cast< uint8_t >(doc->GetValue(std::move(ara::core::StringView(kUCMMAllowCampaign)), uint64_t{0U}));
    log_.LogDebug() << "Persistence::RetrieveState, got allowCampaign:" << allowCampaign;

    /// Restore actions_ in SWCLManager_ from ProcessedSoftwarePackages in status.json
    /// "SWPKGs":
    /// [
    ///     {
    ///         "SWPFQN":"XXXX",
    ///         "TransferID":"XXXX",
    ///     }
    /// ]
    /// @brief SwpkFQN2TransferIDInfoType
    class SwpkFQN2TransferIDInfoType
    {
    public:
        /// @brief Software package FQN
        core::String swpFqn;
        /// @brief Transfer ID
        core::String stTransferID;

        /// @brief ManifestLoader
        /// @param node
        /// @return int
        /// @throws no
        int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node)
        {
            swpFqn       = node.GetValue(std::move(ara::core::StringView(kUCMMSWPFQN)), ara::core::String());
            stTransferID = node.GetValue(std::move(ara::core::StringView(kUCMMTransferID)), ara::core::String());
            return isoft::kSuccess;
        }
    };
    ara::core::Vector< SwpkFQN2TransferIDInfoType > swpkFQN2TransferIDInfoVec;
    std::int32_t const res{doc->Load(std::move(ara::core::StringView(kUCMMSWPKGs)), swpkFQN2TransferIDInfoVec)};
    assert(isoft::kSuccess == res);
    std::ignore = res;
    AraList< SwpkFQN2TransferIDInfoType > swpkFQN2TransferIDInfoList{swpkFQN2TransferIDInfoVec.begin(),
                                                                     swpkFQN2TransferIDInfoVec.end()};

    /// "CancelCause":0,
    *reinterpret_cast< uint8_t* >(&(cancelCause))
        = static_cast< uint8_t >(doc->GetValue(std::move(ara::core::StringView(kUCMMCancelCause)), uint64_t{0U}));
    core::String padding;
    log_.LogDebug() << "Persistence::RetrieveState, got cancelCause:"
                    << com::internal::format::Formatter< core::String, pkgmgr::UCMMasterResolutionType >::ToString(
                           padding, cancelCause)
                           .c_str();

    // Read ProcessedSoftwarePackages and restore actions according to Persistence::ProcessSoftwarePackage;.
    // Restore actionResolution and actionTimeStamp to the corresponding actions
    for (SwpkFQN2TransferIDInfoType const& it : swpkFQN2TransferIDInfoList) {
        log_.LogDebug() << "Persistence::RetrieveState(), get swpkFQN2TransferIDInfo.swpFqn:" << it.swpFqn.c_str()
                        << "swpkFQN2TransferIDInfo.stTransferID:" << it.stTransferID.c_str();

        std::ignore = swpkFQN2TransferIDMap.insert(
            std::make_pair(it.swpFqn, pkgmgr::helper::HexStrToTransferId(it.stTransferID)));
    }

    log_.LogDebug() << "Persistence::RetrieveState, got swpkFQN2TransferIDMap:"
                    << Utils::ConcatenateStrings(swpkFQN2TransferIDMap).c_str();

    // CampaignHistoryKey
    campaignHistoryKey = doc->GetValue< ara::core::String >(std::move(ara::core::StringView(kUCMMCampaignHistoryKey)),
                                                            ara::core::String(""));
    log_.LogDebug() << "Persistence::RetrieveState, got campaignHistoryKey:" << campaignHistoryKey.c_str();
    return true;
}

/// @brief BeginSaveTransaction
/// @throw
void Persistence::BeginSaveTransaction() const
{
    log_.LogDebug() << "Persistence::BeginSaveTransaction(), start.";

    ara::core::String const statusMD5FilePath{FsmManager::GetInstance()->GetStatusMD5FilePath()};
    log_.LogDebug() << "Persistence::BeginSaveTransaction(), statusMD5FilePath:" << statusMD5FilePath.c_str();

    // Delete UCMM status MD5 file
    std::int32_t const removeRet{std::remove(statusMD5FilePath.c_str())};
    if (0 != removeRet) {
        log_.LogDebug() << "Persistence::BeginSaveTransaction, removeRet:" << removeRet;
        // << "with error:" << std::strerror(errno);.
    }

    log_.LogDebug() << "Persistence::BeginSaveTransaction(), end.";
}

/// @brief CommitSaveTransaction
/// @throw
void Persistence::CommitSaveTransaction() const
{
    log_.LogDebug() << "Persistence::CommitSaveTransaction(), start.";

    // ucmm status file path
    ara::core::String const statusFilePath{FsmManager::GetInstance()->GetStatusFilePath()};
    log_.LogDebug() << "Persistence::CommitSaveTransaction(), statusFilePath:" << statusFilePath.c_str();

    // Generate corresponding md5 file
    // ucm status md5 file path
    ara::core::String const stMD5Str{helper::MD5ForFile(statusFilePath)};
    log_.LogDebug() << "Persistence::CommitSaveTransaction, get stMD5Str:" << stMD5Str.c_str()
                    << " for statusFilePath:" << statusFilePath.c_str();

    assert(!stMD5Str.empty());

    ara::core::String const statusMD5FilePath{FsmManager::GetInstance()->GetStatusMD5FilePath()};
    log_.LogDebug() << "Persistence::CommitSaveTransaction(), statusMD5FilePath:" << statusMD5FilePath.c_str();

    pkgmgr::tinyfs::OverWriteToFile(statusMD5FilePath, stMD5Str);

    log_.LogDebug() << "Persistence::CommitSaveTransaction(), end.";
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
