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
/// @file       fg_info_config.cpp
/// @brief      Read configuration files related to function group information
/// @details
/// @date       2024-04-30
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Config
/// @interface_level=uint
/// @trace_id_sr=SR_SM_00001, SR_SM_00101，SR_SM_00106
/// @unit_name=FGInfoConfig
/// @unit_description=Read configuration files related to function group information
/// @endcode
///
/// ================================================================

#include "fg_info_config.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include "define.h"
#include "helper.h"
namespace ara {
namespace sm {
namespace config {

/// @brief Constructor

FGInfoConfig::FGInfoConfig() noexcept
    : log_{log::CreateLogger((core::StringView{"#CFG"}), (core::StringView{"Config context"}))}
    , fgInfoList_{}
    , physicalAddr2FgFQNs_{}
    , functionalAddr2FgFQNs_{}
    , machineFgFQN_{}
{
    log_.LogInfo() << "FGInfoConfig::FGInfoConfig()";
}

/// @brief Destructor function
FGInfoConfig::~FGInfoConfig() noexcept { log_.LogInfo() << "FGInfoConfig::~FGInfoConfig()"; }

/// @brief Get all funtion groups' info list
/// @return funtion groups' info list
core::Vector< FGInfoSimple > const &FGInfoConfig::GetAllFGInfoList() const noexcept { return fgInfoList_; }

/// @brief Get the mapping of physical address to function group FQN
/// @return Mapping of physical address to function group FQN
core::Map< int32_t, core::Vector< core::String > > const &FGInfoConfig::GetPhysicalAddr2FGFQNs() const noexcept
{
    return physicalAddr2FgFQNs_;
}

/// @brief Get the mapping of functional address to function group FQN
/// @return Mapping of functional address to function group FQN
core::Map< int32_t, core::Vector< core::String > > const &FGInfoConfig::GetFunctionalAddr2FGFQNs() const noexcept
{
    return functionalAddr2FgFQNs_;
}

/// @brief Get the FQN of MachineFG
/// @return FQN of MachineFG

core::String FGInfoConfig::GetMachineFgFQN() const noexcept { return machineFgFQN_; }

/// @brief Load all funtion groups' info list
/// @return true - success
/// @return false - failure

bool FGInfoConfig::Load() noexcept
{
    log_.LogInfo() << "FGInfoConfig::Load()";
    isoft::ara_fsh::Platform platform;
    std::string const appSwclMan{platform.GetApplicationSwclsManifest()};
    std::string const plfSwclMan{platform.GetPlatformSwclsManifest()};
    core::Vector< ara::sm::config::FGInfoConfig::SwclInfo > const appSwclsInfos{
        _loadSwclsInfo(core::String(appSwclMan))};
    core::Vector< ara::sm::config::FGInfoConfig::SwclInfo > const platformSwclsInfos{
        _loadSwclsInfo(core::String(plfSwclMan))};
    for (auto const &swclsInfo : appSwclsInfos) {
        std::string const fgSetPath{platform.GetSwclFuncGrpSet(swclsInfo.name.c_str(), swclsInfo.version.c_str())};
        _loadFGInfoSimple(core::String(fgSetPath), swclsInfo.physicalDiagnosticAddress,
                          swclsInfo.functionalDiagnosticAddresses);
    }
    for (auto const &swclsInfo : platformSwclsInfos) {
        std::string const fgSetPath{platform.GetSwclFuncGrpSet(swclsInfo.name.c_str(), swclsInfo.version.c_str())};
        _loadFGInfoSimple(core::String(fgSetPath), swclsInfo.physicalDiagnosticAddress,
                          swclsInfo.functionalDiagnosticAddresses);
    }

    return true;
}

/// @brief Load software cluster information
/// @param path Software cluster configuration file path
/// @return List of software cluster information

core::Vector< FGInfoConfig::SwclInfo > FGInfoConfig::_loadSwclsInfo(core::String const &path) noexcept
{
    log_.LogInfo() << "FGInfoConfig::_loadSwclsInfo(), path:" << path.c_str();
    core::Vector< FGInfoConfig::SwclInfo > infos;
    core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > swclManifestRes{
        isoft::manifestreader::OpenManifest((core::StringView{path}))};
    if (swclManifestRes) {
        std::unique_ptr< isoft::manifestreader::Manifest > const swclManifest{std::move(swclManifestRes).Value()};
        std::ignore = swclManifest->IterateArray(
            (core::StringView{common::GetkSwcls()}),
            [&infos, this](std::size_t, isoft::manifestreader::ManifestNode const &swclNode) {
                FGInfoConfig::SwclInfo info;
                core::String swclName{
                    swclNode.GetValue< core::String >((core::StringView{common::GetkSwclName()}), core::String{""})};
                core::String swclVersion{
                    swclNode.GetValue< core::String >((core::StringView{common::GetkSwclVersion()}), core::String{""})};
                info.name    = std::move(swclName);
                info.version = std::move(swclVersion);
                int32_t physicalAddress{swclNode.GetValue< int32_t >(
                    (core::StringView{core::String{common::GetkDiagnosticAddress()} + "."
                                      + core::String{common::GetkPhysicalDiagnosticAddress()}}),
                    -1)};
                info.physicalDiagnosticAddress = std::move(physicalAddress);
                std::ignore                    = swclNode.IterateArray(
                    (core::StringView{core::String{common::GetkDiagnosticAddress()} + "."
                                      + core::String{common::GetkFunctionalDiagnosticAddress()}}),
                    [&info](std::size_t, isoft::manifestreader::ManifestNode const &functonalNode) {
                        int32_t functionalAddress{functonalNode.GetValue< int32_t >((core::StringView{""}), -1)};
                        info.functionalDiagnosticAddresses.push_back(std::move(functionalAddress));
                    });
                _printSwclInfo(info);
                infos.push_back(std::move(info));
            });
    } else {
        log_.LogWarn() << "FGInfoConfig::_loadSwclsInfo(), OpenManifest error, stateMachineManifestPath:"
                       << path.c_str();
    }
    return infos;
}  // namespace config

/// @brief Print software cluster information
/// @param info Software cluster information

void FGInfoConfig::_printSwclInfo(FGInfoConfig::SwclInfo const &info) const noexcept
{
    core::String functionalDiagnosticAddressesStr{"["};
    for (int32_t const &functionalDiagnosticAddresses : info.functionalDiagnosticAddresses) {
        functionalDiagnosticAddressesStr
            = functionalDiagnosticAddressesStr + core::to_string(functionalDiagnosticAddresses) + ",";
    }
    if (functionalDiagnosticAddressesStr.empty() == false) {
        functionalDiagnosticAddressesStr
            = functionalDiagnosticAddressesStr.substr(0U, functionalDiagnosticAddressesStr.size() - 1U) + "]";
    } else {
        functionalDiagnosticAddressesStr = functionalDiagnosticAddressesStr + "]";
    }
    log_.LogDebug() << "SwclInfo:" << info.name.c_str() << info.version.c_str() << info.physicalDiagnosticAddress
                    << functionalDiagnosticAddressesStr.c_str();
}

/// @brief Load function group information
/// @param path fgSet configuration file path
/// @param diganosticAddress Physical address
/// @param functionalAddresses Set of functional addresses

void FGInfoConfig::_loadFGInfoSimple(core::String const &path,
                                     int32_t const &diganosticAddress,
                                     core::Vector< int32_t > const &functionalAddresses) noexcept
{
    log_.LogInfo() << "FGInfoConfig::_loadFGInfoSimple(), got fgSetPath:" << path.c_str();
    /// @brief Type simplification definition
    using MainfestUniqResul = core::Result< std::unique_ptr< isoft::manifestreader::Manifest > >;
    MainfestUniqResul machineManifestRes{isoft::manifestreader::OpenManifest((core::StringView{path}))};
    if (machineManifestRes.HasValue()) {
        std::unique_ptr< isoft::manifestreader::Manifest > const machineManifest{std::move(machineManifestRes).Value()};
        std::ignore = machineManifest->IterateArray(
            core::StringView{common::GetkFunctionGroups()},
            [&diganosticAddress, &functionalAddresses, this](std::size_t,
                                                             isoft::manifestreader::ManifestNode const &fGNode) {
                core::String fgFQN{fGNode.GetValue< core::String >((core::StringView{common::GetkFunctionGroupName()}),
                                                                   core::String{""})};
                // Function group state
                FGInfoSimple info;
                info.fgFQN = fgFQN;
                if (machineFgFQN_.empty()) {
                    std::size_t const slashPos1{fgFQN.rfind("[")};
                    core::String const slashPart{fgFQN.substr(slashPos1 + 1U)};
                    std::size_t const slashPos2{slashPart.rfind("]")};
                    core::String fgName{slashPart.substr(0U, slashPos2)};
                    if (fgName.empty() || (std::string::npos == slashPos1) || (std::string::npos == slashPos2)) {
                        std::size_t const slashPos{fgFQN.rfind("/")};
                        fgName = fgFQN.substr(slashPos + 1U);
                    }
                    if (fgName == common::GetkMachineFunctionGroupName()) {
                        machineFgFQN_ = fgFQN;
                    }
                }
                int32_t const res2{static_cast< int32_t >(fGNode.IterateArray(
                    (core::StringView{common::GetkStates()}),
                    [&info, this, &fgFQN](std::size_t, isoft::manifestreader::ManifestNode const &stateNode) {
                        core::String const fgState{
                            stateNode.GetValue< core::String >((core::StringView{""}), core::String{""})};
                        info.fgStates.push_back(fgState);
                        log_.LogDebug() << "FGInfoConfig::_loadFGInfoSimple(), get fgState:" << fgState.c_str()
                                        << "for fgFQN:" << fgFQN.c_str();
                    }))};
                log_.LogDebug() << "FGInfoConfig::_loadFGInfoSimple(), insert fgFQN:" << fgFQN.c_str()
                                << "and its fgStates:" << common::ConcatenateStrings(info.fgStates).c_str();
                fgInfoList_.push_back(std::move(info));
                log_.LogDebug() << "FGInfoConfig::_loadFGInfoSimple(), insert diganosticAddress:" << diganosticAddress
                                << "fgFQN:" << fgFQN.c_str();
                // Because the diagnostic EcuReset will switch MachineFG to Off, MachineFG is not added to the diagnostic address mapping for now. In the long run, diagnostic needs to modify this.
                if (machineFgFQN_ != fgFQN) {
                    physicalAddr2FgFQNs_[diganosticAddress].push_back(fgFQN);
                    for (int32_t const &functionalAddress : functionalAddresses) {
                        log_.LogDebug() << "FGInfoConfig::_loadFGInfoSimple(), insert functionalAddress:"
                                        << functionalAddress << "fgFQN:" << fgFQN.c_str();
                        functionalAddr2FgFQNs_[functionalAddress].push_back(fgFQN);
                    }
                }
                std::ignore = res2;  // for release
            });
    } else {
        log_.LogWarn() << "FGInfoConfig::_loadFGInfoSimple(), open error, fgSetPath:" << path.c_str();
    }
}

}  // namespace config
}  // namespace sm
}  // namespace ara