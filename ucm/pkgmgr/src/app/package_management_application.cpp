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
/// @file       package_management_application.cpp
/// @brief      Package management application implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/PackageManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=PackageManagementApplicationImpl
/// @unit_description=Package management application implementation
/// @endcode
///
/// ================================================================

#include "package_management_application.h"

#include <ara/per/internal/initialize.h>
#include <ara/per/key_value_storage.h>

#include "ara/ucm/internal/extraction/lib_poco_zip_extractor.h"
#include "common/const.h"
#include "common/log.h"
#include "common/path.h"
#include "common/rjson_manifest.h"
#include "data/software_package_manager.h"
#include "em/em_client.h"
#include "fsm/fsm_manager.h"
#include "package_manager/package_manager.h"
#include "parsing/software_package_parser_impl.h"
#include "sm/sm_service.h"
#include "storage/filesystem_swcl_manager.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief init something here
/// @return bool
bool PackageManagementApplication::OnInitialize() noexcept
{
    // <SERVICE-INSTANCE-ID>828</SERVICE-INSTANCE-ID>
    // std::uint16_t type is used as internal::ServiceInstanceId to create InstanceIdentifier
    // It is not currently possible to create InstanceIdentifier from string
    // --- Possibly because the serviceInstanceId in the configuration file is in hexadecimal, while this is in decimal. This ServiceInstanceId is extracted from the
    // generated Service Interface Manifest file
    LOGI << "begin...";

    // Load directory and manifest path information
    GetPath().Load();
    LOGD << "load ara path:" << GetPath().Str().c_str();

    /// Get the configuration value at ucm/identifier from machine_manifest.json
    /// First configure it at the UCM-MODULE-INSTANTIATION/IDENTIFIER node in isoft/arxmls/machine_manifest.arxml,
    /// Then it will be generated under the ucm/identifier node of ucm/build/pkgmgr/gen/machines/developmentmachine_machine_manifest.json
    /// get ucm identifier from machine manifest
    AraString const ucmId{GetUcmId(GetPathSettings().machineManifestPath)};
    if (ucmId.empty()) {
        return false;
    }
    LOGI << "got ucmId:" << ucmId.c_str();
    LOGI << "get kUCM_DATA_BLOCK_SIZE:" << kUCM_DATA_BLOCK_SIZE;

    // Initialize the persistent database and try to open it
    if (!OpenKVStorage()) {
        return false;
    }

    // Maximum wait time when UpdateRequest's PrepareUpdate returns kRejected
    std::int32_t const maxWaitTime{GetMaxWaitTimeWhenCallUpdateMethod(GetPathSettings().executionManifestPath)};
    if (maxWaitTime < 0) {
        return false;
    }
    LOGI << "got maxWaitTimeWhenCallUpdateMethod:" << maxWaitTime;

    // report that execution state is running state
    LOGI << "call ReportRunningState";
    ExecClient::ReportExecutionStateRunning();

    // Find UpdateRequest service
    smService_ = std::make_unique< UpdateRequestService >();
    smService_->SetMaxWaitTime(maxWaitTime);
    smService_->AsyncInit([this]() { _onUpdateRequestAvailabeFromSM(); });

    LOGI << "end...";
    return true;
}

/// @brief run the app
/// @return no
void PackageManagementApplication::Run() noexcept
{
    LOGI << "begin...";
    _WaitUntilTermination();
    LOGI << "end...";
}

/// @brief cleanup something here
/// @return no
void PackageManagementApplication::OnTerminate() noexcept { LOGI << "terminating..."; }

/// @brief parse the given manifest file
/// @param machineManifestPath machine manifest path
/// @throws no
/// @return the value of the ucm ID
AraString PackageManagementApplication::GetUcmId(AraString const& machineManifestPath)
{
    AraString defaultValue;
    RManifestOpenAndCheck(doc, machineManifestPath, defaultValue);
    return RManifestDocGetS(doc, "ucm.identifier", defaultValue);
}

/// @brief parse the value of maxWaitTimeWhenCallUpdateMethod
/// @param executionManifestPath execution manifest path
/// @throws no
/// @return int
std::int32_t PackageManagementApplication::GetMaxWaitTimeWhenCallUpdateMethod(AraString const& executionManifestPath)
{
    std::int32_t const defaultValue{2000};
    RManifestOpenAndCheck(doc, executionManifestPath, defaultValue);
    return RManifestDocGetI(doc, kMaxWaitTimeWhenCallUpdateMethodKey, defaultValue);
}

/// @brief open the kv storage
/// @throws no
/// @return bool
bool PackageManagementApplication::OpenKVStorage()
{
    LOGD << "kKV_MODEL_IDENTIFIER:" << kKV_MODEL_IDENTIFIER;

    AraResult< ara::per::SharedHandle< ara::per::KeyValueStorage > > const openRet{ara::per::OpenKeyValueStorage(
        std::move(ara::core::InstanceSpecifier(std::move(AraStringView(kKV_MODEL_IDENTIFIER)))))};
    if (!openRet.HasValue()) {
        LOGE << "failed:" << openRet.Error().Message().data();
        return false;
    }

    return true;
}

/// @brief callback after find sm service
/// @throws no
/// @return no
void PackageManagementApplication::_onUpdateRequestAvailabeFromSM()
{
    LOGI << "call...";

    // init service of PackageManager
    LOGI << "call PackageManager::Create";

    service_ = PackageManager::Create(
        ara::core::InstanceSpecifier(std::move(AraStringView("ucmd/package_manager_root/PackageManagement"))));
    PackageManager* const srv{dynamic_cast< PackageManager* >(service_.get())};

    // init SoftwarePackageManager
    LOGI << "init swpkgManager";
    std::unique_ptr< SoftwarePackageManager > swpkgManager{std::make_unique< SoftwarePackageManager >(
        std::make_unique< LibPocoZipExtractor >(), std::make_unique< SoftwarePackageParserImpl >(),
        std::make_unique< SynchronizedStorage< TransferIdType, StreamableSoftwarePackage > >())};

    // init FileSystemSWCLManager
    LOGI << "init swclManager";
    std::unique_ptr< FileSystemSWCLManager > swclManager{std::make_unique< FileSystemSWCLManager >()};

    // init FsmManager
    LOGI << "init fsm";
    std::unique_ptr< FsmManager > fsmManager{std::make_unique< FsmManager >(
        [srv](std::underlying_type< PackageManagerStatusType >::type const& st) {
            srv->UpdateCurrentStatus(static_cast< PackageManagerStatusType >(st));
        },
        *swpkgManager, *swclManager, std::move(smService_))};

    // init PackageManagerImpl
    LOGI << "init impl";
    std::unique_ptr< PackageManagerImpl > impl{
        std::make_unique< PackageManagerImpl >(GetUcmId(GetPathSettings().machineManifestPath), std::move(swpkgManager),
                                               std::move(swclManager), std::move(fsmManager))};
    srv->SetImpl(std::move(impl));

    std::ignore = _Add2EvLoop2Exec([this, srv]() {
        // continue the life-cycle of the FSM if needed
        // TODO: review: after OfferService() UCM could run parallel tasks
        srv->GetImpl().ContinueIfNecessary();

        std::ignore = srv->CurrentStatus.RegisterGetHandler(std::bind(&PackageManager::CurrentStatus_Getter, srv));

        // start offering service here
        AraResultVoid const ret{this->service_->OfferService()};
        if (!ret.HasValue()) {
            LOGE << "OfferService failed, errc:" << ret.Error().Message().data();
            mainLoop_->Stop();
        }
        LOGI << "OfferService success, UCM can receive request from client";
    });

    LOGI << "end";
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
