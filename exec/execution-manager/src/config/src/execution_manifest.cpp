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
/// @file       execution_manifest.cpp
/// @brief      ara configuration series execution manifest class
/// @details
/// @date       2023-03-14
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Config
/// @unit_name=ExecutionManifest
/// @unit_description=Used to read and save the execution manifest configuration.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "ara/exec/internal/config/execution_manifest.h"

#include <set>

#include "ara/exec/internal/config/log.h"
#include "ara/exec/internal/config/sysconfig.h"
#include "isoft/ara_fsh/platform.h"
#include "isoft/manifestreader/manifest_reader.h"
#include "isoft/utils/file.h"
#include "isoft/utils/mix.h"
#include "isoft/utils/security.h"
#include "isoft/utils/string.h"

namespace ara {
namespace exec {
namespace internal {
namespace config {

namespace {
/// @brief Maximum Processor Id
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
constexpr uint16_t kMaxProcessorId{512U};

/// @brief Maximum Core Id
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
constexpr uint16_t kMaxCoreId{512U};
}  // namespace

/// @brief Load configuration items of the executable file
/// @param node Executable file configuration item node
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00074
/// @needwork = dda
/// @endcode
int32_t Executable::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    if (isoft::kSuccess != node.Load(kName, name_)) {
        LOGE() << "lack of {" << kName << "}";
    }

    if (isoft::kSuccess != node.Load(kFullQualifiedName, fqn_)) {
        LOGE() << "lack of {" << kFullQualifiedName << "}";
    }

    ara::core::String reportingBehaviorStr;
    std::ignore = node.Load(kReportingBehavior, reportingBehaviorStr);
    if (true != isoft::manifestreader::tps::FromString(reportingBehaviorStr, reportingBehavior_)) {
        reportingBehavior_ = ReportingBehaviorEnum::kReportsExecutionState;
    }

#ifdef EXTRA_EXECUTABLE_INFO
    std::ignore = node.Load(kVersion, version_);

    ara::core::String buildTypeStr;
    std::ignore = node.Load(kBuildType, buildTypeStr);
    if (true != isoft::manifestreader::tps::FromString< ara::core::String >(buildTypeStr, buildType_)) {
        buildType_ = BuildTypeEnum::kBuildTypeRelease;
    }

    std::ignore              = node.Load(kLoggingBehavior, loggingBehavior_);
    mininumTimerGranularity_ = node.GetValue(kMininumTimerGranularity, 0.0);
    isPlatformApplication_   = node.GetValue(kPlatformApplication, false);
#endif

    return isoft::kSuccess;
}

/// @brief Check whether the executable file configuration is correct
/// @return true configuration error; false configuration correct
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00075
/// @needwork = dda
/// @endcode
bool Executable::HasConfigError() const noexcept
{
    bool const isNameEmpty{name_.empty()};
    bool const isFqnEmpty{fqn_.empty()};
    if (isNameEmpty || isFqnEmpty) {
        LOGE() << "Executable name or fqn is empty !!!";
        return true;
    }

    std::string const execShortName{isoft::utils::GetShortName(fqn_.c_str())};
    /// If the last "[]" in the FQN is not the short name_, then the configuration is considered incorrect
    if (execShortName.compare(name_.c_str()) != 0) {
        LOGE() << "Executable name {" << name_ << "} and fqn {" << execShortName << "} mismatch !!!";
        return true;
    }

    return false;
}

/// @brief Print debug information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void Executable::Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    std::cout << "+++++ Executable::Debug +++++\n";
    std::cout << "Name:\t" << GetName() << std::endl;
    std::cout << "FQN:\t" << GetFqn() << std::endl;
    std::cout << "ExecutionReporting:\t";
    std::cout << isoft::utils::ToString(IsExceutionReporting()) << std::endl;

    #ifdef EXTRA_EXECUTABLE_INFO
    std::cout << "Version:\t" << GetVersion() << std::endl;
    std::cout << "BuildType:\t";
    std::cout << isoft::manifestreader::tps::ToString(buildType_) << std::endl;
    std::cout << "IsUseLogging:\t";
    std::cout << isoft::utils::ToString(IsUseLogging()) << std::endl;
    std::cout << "TimerGranularity:\t" << GetMininumTimerGranularity() << std::endl;
    std::cout << "IsPlatformApplication:\t";
    std::cout << isoft::utils::ToString(IsPlatformApplication()) << std::endl;
    #endif

    std::cout << "----- Executable::Debug -----\n";
#endif  ///< ARA_EXEC_DEBUG
}  ///< Debug()

/// @brief Load processor information
/// @param node Processor information node
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00089
/// @needwork = dda
/// @endcode
int32_t Processor::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    id_ = node.GetValue(kId, kMaxProcessorId);
    if (id_ >= kMaxProcessorId) {
        LOGE() << "Processor: lack of {" << kId << "}";
    }

    if (isoft::kSuccess != node.Load(kCore, cores_)) {
        LOGE() << "Processor: lack of {" << kCore << "}";
    }

    return 0;
}

/// @brief Check whether the Processor configuration is correct
/// @return true configuration error; false configuration correct
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00090
/// @needwork = dda
/// @endcode
bool Processor::HasConfigError() const noexcept
{
    if (id_ >= kMaxProcessorId) {
        LOGE() << "Invalid Processor Id {" << id_ << "}!!!";
        return true;
    }

    if (cores_.empty()) {
        LOGE() << "No Core Id !!!";
        return true;
    }

    ara::core::Vector< uint16_t >::const_iterator const it{
        std::find_if(cores_.cbegin(), cores_.cend(), [](auto& coreId) {
            if (coreId >= kMaxCoreId) {
                LOGE() << "Invalid Core Id {" << coreId << "} !!!";
                return true;
            }
            return false;
        })};

    if (it != cores_.end()) {
        return true;
    }

    return false;
}

/// @brief Print debug information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void Processor::Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    std::cout << "+++++ Processor::Debug +++++\n";
    std::cout << "Id:\t" << GetId() << std::endl;
    std::cout << "Cores:\t";
    std::ignore = std::for_each(cores_.cbegin(), cores_.cend(), [](auto& coreId) { std::cout << coreId << " "; });
    std::cout << std::endl;
    std::cout << "----- Processor::Debug -----\n";
#endif  ///< ARA_EXEC_DEBUG
}

/// @brief Load configuration from string
/// @param execDependStr Dependency relationship string
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00103
/// @needwork = dda
/// @endcode
int32_t ExecutionDependency::Load(ara::core::String const& execDependStr) noexcept
{
    ara::core::String::size_type const pos{execDependStr.find(kNameAndStateSeparator)};
    /// There is no separator '.' in the dependency relationship string, then it is considered as Running dependency by default
    if (ara::core::String::npos == pos) {
        procName_  = execDependStr;
        procState_ = ExecutionDependencyState::kRunning;
    } else {
        procName_ = execDependStr.substr(0U, pos);
        if (procName_.empty()) {
            LOGE() << "ExecutionDependency: Process Name is empty !!!";
            return -1;
        }

        ara::core::String const depStr{execDependStr.substr(pos + 1U)};
        if (GetRunningState() == depStr) {
            procState_ = ExecutionDependencyState::kRunning;
        } else if (GetTerminatedState() == depStr) {
            procState_ = ExecutionDependencyState::kTerminated;
        } else {
            LOGE() << "ExecutionDependency: Wrong Process State {" << depStr << "} !!!";
            return -1;
        }
    }

    return 0;
}

/// @brief Print debug information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void ExecutionDependency::Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    std::cout << "+++++ ExecutionDependency::Debug +++++\n";
    std::cout << GetProcessName() << ".";
    switch (GetProcessState()) {
        case ExecutionDependencyState::kRunning: {
            std::cout << "Running" << std::endl;
            break;
        }
        case ExecutionDependencyState::kTerminated: {
            std::cout << "Terminated" << std::endl;
            break;
        }
        default: {
            std::cout << "Unknown State" << std::endl;
            break;
        }
    }
    std::cout << "----- ExecutionDependency::Debug -----\n";
#endif
}

/// @brief Load function group state configuration
/// @param node Function group configuration item node
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00115
/// @needwork = dda
/// @endcode
int32_t FunctionGroupConfig::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    if (isoft::kSuccess != node.Load(kName, functionGroupName_)) {
        LOGE() << "FunctionGroupConfig: lack of {" << kName << "}";
    }

    if (isoft::kSuccess != node.Load(kStates, states_)) {
        LOGE() << "FunctionGroupConfig: lack of {" << kStates << "}";
    }

    return 0;
}

/// @brief Check whether the function group state configuration is correct
/// @return true configuration error; false configuration correct
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00116
/// @needwork = dda
/// @endcode
bool FunctionGroupConfig::HasConfigError() const noexcept
{
    if (functionGroupName_.empty()) {
        LOGE() << "FunctionGroupConfig: Function Group Name should not be empty !!!";
        return true;
    }

    if (states_.empty()) {
        LOGE() << "FunctionGroupConfig: Function Group State should be Configured !!!";
        return true;
    }

    ara::core::Vector< ara::core::String >::const_iterator const iter{
        std::find_if(states_.cbegin(), states_.cend(), [](auto& state) {
            if (state.empty()) {
                LOGE() << "FunctionGroupConfig: Function Group State should not be empty !!!";
                return true;
            }

            if (state.compare(GetFunctionGroupStateOff()) == 0) {
                LOGE() << "FunctionGroupConfig: Process should not belong to { Off } State !!!";
                return true;
            }
            return false;
        })};

    if (iter != states_.cend()) {
        return true;
    }

    return false;
}

/// @brief Print debug information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void FunctionGroupConfig::Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    std::cout << "+++++ FunctionGroupSate::Debug +++++\n";
    std::cout << "FunctionGroupName:\t" << GetFunctionGroupName() << std::endl;
    std::cout << "FunctionGroupStates:\t";
    ara::core::Vector< ara::core::String > const& states{GetStates()};
    for (ara::core::String const& state : states) {  // PRQA S 2961
        std::cout << state << " ";
    }
    std::cout << std::endl;
    std::cout << "----- FunctionGroupConfig::Debug -----\n";
#endif  ///< ARA_EXEC_DEBUG
}  ///< Debug()

/// @brief Load resource consumption configuration
/// @param node Resource consumption configuration item node
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00122
/// @needwork = dda
/// @endcode
int32_t ResourceConsumption::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    heapUsage_         = node.GetValue(kHeapUsage, 0U);
    systemMemoryUsage_ = node.GetValue(kSystemMemoryUsage, 0U);
    return 0;
}

/// @brief Print debug information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void ResourceConsumption::Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    std::cout << "+++++ ResourceConsumption::Debug +++++\n";
    std::cout << "HeapUsage:\t" << GetHeapUsage() << std::endl;
    std::cout << "SystemMemoryUsage:\t" << GetSystemMemoryUsage() << std::endl;
    std::cout << "----- ResourceConsumption::Debug -----\n";
#endif  ///< ARA_EXEC_DEBUG
}  ///< Debug()

/// @brief Load the process startup configuration items
/// @param node Process startup configuration item node
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00137
/// @needwork = dda
/// @endcode
int32_t StartupConfig::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    /// Environment variables
    ara::core::Vector< ara::core::String > envValueStrs;
    std::ignore = node.Load(kEnvironmentVariable, envValueStrs);
    for (auto const& envStr : envValueStrs) {  // PRQA S 2961
        if (envStr.empty()) {
            continue;
        }
        std::size_t const pos{envStr.find("=")};
        if (pos != ara::core::String::npos) {
            environmentVariable_.emplace_back(envStr);
        } else {
            /// @code{.isoft}
            /// @trace_id_sws=SWS_EM_02249 If the environment variable has no value, then an empty string should be treated as the value of the KEY
            /// @endcode
            environmentVariable_.emplace_back(envStr + "=");
        }
    }

    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_02543 If executionError is not configured, the default returns 1
    /// @endcode
    executionError_ = node.GetValue(kExecutionError, 1U);

    /// Process parameters
    std::ignore = node.Load(kProcessArgument, processArgument_);

    /// Scheduling policy
    ara::core::String schedPolicyStr;
    std::ignore = node.Load(kSchedulingPolicy, schedPolicyStr);
    if (kSchedPolicyEnumFIFO == schedPolicyStr) {
        schedulingPolicy_ = SchedulingPolicy::kFIFO;
    } else if (kSchedPolicyEnumRR == schedPolicyStr) {
        schedulingPolicy_ = SchedulingPolicy::kRoundRobin;
    } else if (kSchedPolicyEnumOther == schedPolicyStr) {
        schedulingPolicy_ = SchedulingPolicy::kOther;
    } else {
        LOGW() << "Invalid SchedulingPolicy of {" << schedPolicyStr << "}, set to default value {Other}";
        schedulingPolicy_ = SchedulingPolicy::kOther;
    }

    /// Scheduling priority
    schedulingPriority_ = node.GetValue(kSchedulingPriority, 0U);

    /// Termination behavior
    ara::core::String terminatitonBehaviorStr;
    std::ignore = node.Load(kTerminationBehavior, terminatitonBehaviorStr);
    if (!isoft::manifestreader::tps::FromString(terminatitonBehaviorStr, terminationBehavior_)) {
        terminationBehavior_ = TerminationBehaviorEnum::kProcessIsNotSelfTerminating;
    }

    enterTimeout_ = node.GetValue(kEnterTimeout, 0.0);  ///< Enter timeout
    exitTimeout_  = node.GetValue(kExitTimeout, 0.0);   ///< Exit timeout

    hasConfigError_ = false;
    return 0;
}

/// @brief Check whether the startup configuration items are correct
/// @return true configuration error; false configuration correct
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00138
/// @needwork = dda
/// @endcode
bool StartupConfig::HasConfigError() const noexcept { return hasConfigError_; }

/// @brief Print debug information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void StartupConfig::Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    std::cout << "+++++ StartupConfig::Debug +++++\n";
    std::cout << "Environments:" << std::endl;
    std::ignore = std::for_each(environmentVariable_.cbegin(), environmentVariable_.cend(),
                                [](auto& env) { std::cout << "\t" << env << std::endl; });
    std::cout << "Arguments:" << std::endl;
    std::ignore = std::for_each(processArgument_.cbegin(), processArgument_.cend(),
                                [](auto& arg) { std::cout << "\t" << arg << std::endl; });
    std::cout << "ExecutionError:\t" << GetExecutionError() << std::endl;
    std::cout << "SchedulingPolicy:\t";
    switch (GetSchedulingPolicy()) {
        case SchedulingPolicy::kFIFO: {
            std::cout << kSchedPolicyEnumFIFO << std::endl;
            break;
        }
        case SchedulingPolicy::kRoundRobin: {
            std::cout << kSchedPolicyEnumRR << std::endl;
            break;
        }
        case SchedulingPolicy::kOther: {
            std::cout << kSchedPolicyEnumOther << std::endl;
            break;
        }
        default: {
            std::cout << "Unknown" << std::endl;
            break;
        }
    }
    std::cout << "SchedulingPriority:\t" << GetSchedulingPriority() << std::endl;
    std::cout << "IsSelfTerminate:\t" << isoft::utils::ToString(IsSelfTerminate()) << std::endl;
    std::cout << "EnterTimeout: " << GetEnterTimeout() << std::endl;
    std::cout << "ExitTimeout: " << GetExitTimeout() << std::endl;
    std::cout << "----- StartupConfig::Debug -----\n";
#endif  ///< ARA_EXEC_DEBUG
}  ///< Debug()

/// @brief Load the process's state dependency configuration
/// @param node State dependency configuration item node
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00163
/// @needwork = dda
/// @endcode
int32_t StateDependentConfig::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    int32_t ret{0};
    /// Bound function group state
    if (isoft::kSuccess != node.Load(kFunctionGroupState, functionGroupState_)) {
        LOGE() << "lack of {" << kFunctionGroupState << "}";
        ret = -1;
    }

    /// Bound resource group name
    if (isoft::kSuccess != node.Load(kResourceGroup, resourceGroupName_)) {
        LOGE() << "lack of {" << kResourceGroup << "}";
        ret = -1;
    }

    /// Startup configuration
    if (isoft::kSuccess != node.Load(kStartupConfig, startupConfig_)) {
        LOGE() << "lack of {" << kStartupConfig << "}";
        ret = -1;
    }

    /// List of dependency relationships
    std::ignore = node.Load(kExecutionDependency, execDependStr_);
    for (ara::core::String const& execDepStr : execDependStr_) {  // PRQA S 2961
        if (execDepStr.empty()) {
            continue;
        }

        ExecutionDependency execDep;
        if (0 != execDep.Load(execDepStr)) {
            LOGW() << "ExecutionDependency: invalid of {" << execDepStr << "}";
            ret = -1;
            break;
        }

        executionDependency_.emplace_back(execDep);
    }

    /// Resource consumption limit
    std::ignore = node.Load(kResourceConsumption, resouceConsumption_);

    if (ret == 0) {
        hasConfigError_ = false;
    }

    return 0;
}

/// @brief Check whether the state dependency configuration is correct
/// @return true configuration error; false configuration correct
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00164
/// @needwork = dda
/// @endcode
bool StateDependentConfig::HasConfigError() const noexcept
{
    if (hasConfigError_) {
        LOGE() << "StateDependentConfig: lack of some field !!!";
        return true;
    }

    if (resourceGroupName_.empty()) {
        LOGE() << "StateDependentConfig: resourceGroupName should not be empty !!!";
        return true;
    }

    return (functionGroupState_.HasConfigError() || startupConfig_.HasConfigError());
}

/// @brief Print debug information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void StateDependentConfig::Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    std::cout << "+++++ StateDependentConfig::Debug +++++\n";
    std::cout << "ExecutionDependency: " << std::endl;
    ara::core::Vector< ExecutionDependency > const& execDepStr{GetExecutionDependencyList()};
    for (ExecutionDependency const& dep : execDepStr) {  // PRQA S 2961
        dep.Debug();
    }
    std::cout << "FunctionGroupConfig: " << std::endl;
    GetFunctionGroupState().Debug();

    std::cout << "ResourceConsumption: " << std::endl;
    GetResouceConsumption().Debug();

    std::cout << "ResourceGroupName:\t" << GetResourceGroupName() << std::endl;
    std::cout << "StartupConfig:" << std::endl;
    startupConfig_.Debug();
    std::cout << "----- StateDependentConfig::Debug -----\n";
#endif  ///< ARA_EXEC_DEBUG
}  ///< Debug()

/// @brief Load the execution manifest
/// @param manifestPath Manifest json file path
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00176
/// @needwork = dda
/// @endcode
int32_t ExecutionManifest::Load(ara::core::String const& manifestPath) noexcept
{
    LOGV() << "Load ExecutionManifest {" << manifestPath << "}";
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
        isoft::manifestreader::OpenManifest(manifestPath)};
    if (!manifestRes.HasValue()) {
        LOGE() << "ExecutionManifest::Load: OpenManifest(" << manifestPath << ") failed !!!";
        return -1;
    }

    int32_t ret{0};
    std::unique_ptr< isoft::manifestreader::Manifest > const manifest{std::move(manifestRes).Value()};
    if (isoft::kSuccess != manifest->Load(kName, procName_)) {
        LOGE() << "lack of {" << kName << "}";
        ret = -1;
    }

    if (isoft::kSuccess != manifest->Load(kFullQualifiedName, procFqn_)) {
        LOGE() << "lack of {" << kFullQualifiedName << "}";
        ret = -1;
    }

    if (isoft::kSuccess != manifest->Load(kExecutable, executable_)) {
        LOGE() << "lack of {" << kExecutable << "}";
        ret = -1;
    }

    std::ignore              = manifest->Load(kIpcName, ipcName_);
    std::ignore              = manifest->Load(kFunctionClusterAffiliation, functionClusterAffiliation_);
    numberOfRestartAttempts_ = manifest->GetValue< uint16_t >(kNumberOfRestartAttempts, 0U);
    preMapping_              = manifest->GetValue(kPreMapping, false);

    if (isoft::kSuccess != manifest->Load(kStateDependentStartupConfig, stateDependentConfigs_)) {
        LOGE() << "lack of {" << kStateDependentStartupConfig << "}";
        ret = -1;
    }

    if (isoft::kSuccess != manifest->Load(kProcessor, processors_)) {
        LOGW() << "lack of {" << kProcessor << "}";
    }

    if (0 == ret) {
        hasConfigError_ = false;
    }

    /// Only when the execution manifest fails to open is the load considered a failure. Configuration errors can be judged by HasConfigError()
    return 0;
}

/// @brief Set the executable program path
/// @param execPath Executable program path
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00188
/// @needwork = dda
/// @endcode
int32_t ExecutionManifest::SetExecutablePath(ara::core::String const& execPath) noexcept
{
    executablePath_ = execPath;

    /// When pre-mapping is set, preload this executable file
    /// So you need to set the path of the preloaded file
    if (preMapping_) {
        isoft::ara_fsh::Platform const pf;
        executablePreMappedPath_
            = pf.GetAraTmpDir() + "/ara-preload-" + isoft::utils::security::GetMd5OfData(executablePath_.c_str());
        if (isoft::utils::file::CopyFile(executablePath_.c_str(), executablePreMappedPath_.c_str()) < 0) {
            LOGW() << "PreMap executable {" << execPath << "} failed !!!";
            executablePreMappedPath_ = executablePath_;
        }
    }

    return 0;
}

/// @brief Check whether the execution manifest configuration is correct
/// @return true configuration error; false configuration correct
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00208
/// @needwork = dda
/// @endcode
bool ExecutionManifest::HasConfigError() const noexcept
{
    if (hasConfigError_) {
        LOGE() << "ExecutionManifest: lack of some field !!!";
        return true;
    }

    bool const isProcNameEmpty{procName_.empty()};
    bool const isProcFqnEmpty{procFqn_.empty()};
    if (isProcNameEmpty || isProcFqnEmpty) {
        LOGE() << "ExecutionManifest: procName or fqn should not be empty !!!";
        return true;
    }

    std::set< ara::core::String > fgNames;
    ara::core::Vector< StateDependentConfig > const& stateDepConfigs{GetStateDependentConfigs()};
    for (StateDependentConfig const& depConfig : stateDepConfigs) {  // PRQA S 2961
        if (depConfig.HasConfigError()) {
            return true;
        }
        std::ignore = fgNames.insert(depConfig.GetFunctionGroupState().GetFunctionGroupName());
    }

    if (fgNames.size() > 1U) {
        LOGE() << "ExecutionManifest: More than one Function Groups for process {" << procName_ << "} !!!";
        return true;
    }

    return executable_.HasConfigError();
}

/// @brief Print content
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void ExecutionManifest::Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    std::cout << "+++++ ExecutionManifest::Debug +++++\n";
    std::cout << "SwclName:\t" << GetSwclName() << std::endl;
    std::cout << "SwclVersion:\t" << GetSwclVersion() << std::endl;
    std::cout << "Name:\t" << GetProcName() << std::endl;
    std::cout << "FullQualifiedName:\t" << GetProcFqn() << std::endl;
    std::cout << "Executable:" << std::endl;
    executable_.Debug();
    std::cout << "FunctionClusterAffiliation: " << functionClusterAffiliation_ << std::endl;
    std::cout << "RestartNumber:\t" << GetNumberOfRestartAttempts() << std::endl;
    std::cout << "PreMapping:\t" << isoft::utils::ToString(preMapping_) << std::endl;
    std::cout << "StartupConfigs:" << std::endl;
    ara::core::Vector< StateDependentConfig > const& stateDepConfigs{GetStateDependentConfigs()};
    for (StateDependentConfig const& depConfig : stateDepConfigs) {  // PRQA S 2961
        depConfig.Debug();
    }
    std::cout << "Processor:" << std::endl;
    ara::core::Vector< Processor > const& processors{GetProcessors()};
    for (Processor const& proc : processors) {  // PRQA S 2961
        proc.Debug();
    }

    std::cout << "SystemEnvironments:" << std::endl;
    ara::core::Vector< ara::core::String > const& sysEnvs{GetSysEnvironments()};
    for (ara::core::String const& it : sysEnvs) {  // PRQA S 2961
        std::cout << "\t" << it << std::endl;
    }

    std::cout << "----- ExecutionManifest::Debug -----\n";
#endif  ///< ARA_EXEC_DEBUG
}

/// @brief Get the name of the function group to which the process belongs
/// @return Function group name
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_20023
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00198
/// @needwork = dda
/// @endcode
ara::core::StringView ExecutionManifest::GetFunctionGroupName() const noexcept
{
    ara::core::StringView ret{""};
    ara::core::Vector< StateDependentConfig > const& depConfig{GetStateDependentConfigs()};
    if (!depConfig.empty()) {
        ret = depConfig[0UL].GetFunctionGroupState().GetFunctionGroupName();
    }

    return ret;
}

/// @brief Find the startup configuration for the specified function group state
/// @param fgState Function group state name
/// @return Pointer to the obtained startup configuration; nullptr, failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00199
/// @needwork = dda
/// @endcode
StateDependentConfig const* ExecutionManifest::FindStartupConfig(ara::core::String const& fgState) const noexcept
{
    StateDependentConfig const* ret{nullptr};
    for (StateDependentConfig const& conf : stateDependentConfigs_) {  // PRQA S 2961
        ara::core::Vector< ara::core::String > const& states{conf.GetFunctionGroupState().GetStates()};
        for (ara::core::String const& state : states) {  // PRQA S 2961
            if (fgState == state) {
                // If found, return immediately
                ret = &conf;
            }
        }
    }

    return ret;
}

/// @brief Update timer settings
/// @param enterTimeout Startup timer value
/// @param exitTimeout Exit timer value
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00201
/// @needwork = dda
/// @endcode
void ExecutionManifest::UpdateTimer(double const enterTimeout, double const exitTimeout) noexcept  // PRQA S 4649
{
    for (StateDependentConfig& stateDepConf : stateDependentConfigs_) {  // PRQA S 2961
        /// @code{.isoft}
        /// @trace_id_sws=SWS_EM_02251 If the execution manifest configures a termination timeout value, then it overrides the termination timeout configuration in the machine manifest
        /// @endcode
        /// FIXME: The override rule for startup timeout is not found in the SWS document
        if (isoft::utils::IsEq0(stateDepConf.GetEnterTimeout())) {
            stateDepConf.SetEnterTimeout(enterTimeout);
        }

        if (isoft::utils::IsEq0(stateDepConf.GetExitTimeout())) {
            stateDepConf.SetExitTimeout(exitTimeout);
        }
    }
}

/// @brief Update the bound processor
/// @param processors Restricted processor
/// @param cpuInfo OS CPU information
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_20014
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00192
/// @needwork = dda
/// @endcode
void ExecutionManifest::UpdateProcessors(ara::core::Vector< uint8_t > const& processors,
                                         std::shared_ptr< isoft::osi::cpu::CpuInfo const > const& cpuInfo) noexcept
{
    // PRQA S 2961 ++
    for (Processor const& processor : processors_) {
        uint16_t const phyId{processor.GetId()};
        ara::core::Vector< uint16_t > const& cores{processor.GetCores()};
        for (uint16_t const& coreId : cores) {  // PRQA S 2961
            ara::core::Vector< uint8_t >::const_iterator const iter{
                std::find_if(processors.cbegin(), processors.cend(),  // PRQA S 2961
                             [coreId](uint8_t const& id) noexcept { return id == coreId; })};

            if (iter == processors.end()) {
                LOGW() << "Invalid processor id {" << coreId << "}";
                continue;
            }
            ara::core::Vector< uint16_t > cpus;
            if (0 >= cpuInfo->GetProcessorNumbers(phyId, coreId, cpus)) {
                LOGW() << "cpuInfos_.GetProcessorNumbers(" << phyId << "," << coreId
                       << "): Invalid CPU ID, the CPU affinity may be NOT effect";
                continue;
            }

            for (uint16_t const& cpu : cpus) {
                AddCpuSet(cpu);
            }
        }
    }
    // PRQA S 2961 --
}

/// @brief Find the dependency configuration for the specified function group state and process name
/// @param fgState Function group state name
/// @param procName Process name
/// @return Pointer to the obtained startup configuration; nullptr, failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00200
/// @needwork = dda
/// @endcode
ExecutionDependency const* ExecutionManifest::FindExecutionDepend(ara::core::String const& fgState,
                                                                  ara::core::String const& procName) const noexcept
{
    StateDependentConfig const* const conf{FindStartupConfig(fgState)};
    if (nullptr == conf) {
        return nullptr;
    }

    ara::core::Vector< ExecutionDependency > const& depList{conf->GetExecutionDependencyList()};
    ara::core::Vector< ExecutionDependency >::const_iterator const iter{std::find_if(
        depList.begin(), depList.end(),  // PRQA S 2961
        [&procName](ExecutionDependency const& dep) noexcept { return dep.GetProcessName() == procName; })};

    if (iter != depList.end()) {
        return &(*iter);
    }

    return nullptr;
}

/// @brief Get the list of function group state names that the process depends on
/// @param nameList Save the obtained list of state names
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00202
/// @needwork = dda
/// @endcode
void ExecutionManifest::GetFunctionGroupStateNames(ara::core::Vector< ara::core::String >& nameList) const noexcept
{
    nameList.clear();
    ara::core::Vector< StateDependentConfig > const& stateDepConfigs{GetStateDependentConfigs()};
    for (StateDependentConfig const& conf : stateDepConfigs) {  // PRQA S 2961
        ara::core::Vector< ara::core::String > const& states{conf.GetFunctionGroupState().GetStates()};
        for (ara::core::String const& state : states) {  // PRQA S 2961
            nameList.emplace_back(state);
        }
    }
}

/// @brief Get the list of process execution dependencies
/// @param fgState Function group name, only meaningful when specifying a function group state, because processes cannot depend across function group states
/// @param depList Save the obtained dependency list
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00211
/// @needwork = dda
/// @endcode
void ExecutionManifest::_GetExecutionDepends(ara::core::String const& fgState,
                                             ara::core::Vector< ExecutionDependency >& depList) const noexcept
{
    StateDependentConfig const* const conf{FindStartupConfig(fgState)};
    if (nullptr == conf) {
        return;
    }
    depList.clear();
    ara::core::Vector< ExecutionDependency > const& execDepList{conf->GetExecutionDependencyList()};
    for (ExecutionDependency const& dep : execDepList) {  // PRQA S 2961
        depList.emplace_back(dep);
    }
}

/// @brief Get the list of process names that the process execution depends on
/// @param fgState Function group name, only meaningful when specifying a function group state, because processes cannot depend across function group states
/// @param nameList Save the obtained list of process names
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00203
/// @needwork = dda
/// @endcode
void ExecutionManifest::GetExecutionDependNames(ara::core::String const& fgState,
                                                ara::core::Vector< ara::core::String >& nameList) const noexcept
{
    ara::core::Vector< ExecutionDependency > depList;
    _GetExecutionDepends(fgState, depList);
    nameList.clear();
    for (auto const& dep : depList) {  // PRQA S 2961
        nameList.emplace_back(dep.GetProcessName());
    }
}

/// @brief Get the process execution error code
/// @param fgState Function group state
/// @return Execution error code
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00204
/// @needwork = dda
/// @endcode
uint32_t ExecutionManifest::GetExecutionError(ara::core::String const& fgState) const noexcept
{
    StateDependentConfig const* const conf{FindStartupConfig(fgState)};

    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_02543 If executionError is not configured, the default returns 1
    /// @endcode
    uint32_t const execError{(conf == nullptr) ? 1U : conf->GetExecutionError()};

    LOGD() << "GetExecutionError: Process {" << GetProcName() << ", fgState =" << fgState
           << "}, execError =" << execError;
    return execError;
}

}  // namespace config
}  // namespace internal
}  // namespace exec
}  // namespace ara
