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
/// @file       execution_manifest.h
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

#ifndef ARA_EXEC_INTERNAL_CONFIG_EXECUTION_MANIFEST_H_
#define ARA_EXEC_INTERNAL_CONFIG_EXECUTION_MANIFEST_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/tps_enumeration.h>

#include <cstdint>

#include "isoft/osi/cpu/cpu_info.h"

namespace ara {
namespace exec {
namespace internal {
namespace config {

/// @brief Read and save Executable information
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_EM_20009
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00063
/// @needwork = ad
/// @endcode
class Executable  // PRQA S 5215
{
public:
    /// @brief Default constructor
    /// @exception std::bad_alloc Thrown when memory allocation fails
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00064
    /// @needwork = dda
    /// @endcode
    Executable() = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00065
    /// @needwork = dda
    /// @endcode
    ~Executable() noexcept = default;

    /// @brief Disable move constructor
    /// @param other the other Executable
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00066
    /// @needwork = dda
    /// @endcode
    Executable(Executable &&other) noexcept = delete;

    /// @brief Disable copy constructor
    /// @param other the other Executable
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00067
    /// @needwork = dda
    /// @endcode
    Executable(Executable const &other) noexcept = delete;

    /// @brief Disable move assignment
    /// @param other the other Executable
    /// @return Reference to this Executable
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00068
    /// @needwork = dda
    /// @endcode
    Executable &operator=(Executable &&other) noexcept = delete;

    /// @brief Disable copy assignment
    /// @param other the other Executable
    /// @return Reference to this Executable
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00069
    /// @needwork = dda
    /// @endcode
    Executable &operator=(Executable const &other) noexcept = delete;

    /// @brief Get the executable file name
    /// @return Executable file name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00070
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetName() const noexcept { return name_; }

    /// @brief Get the FQN of the executable file
    /// @return FQN of the executable file
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00071
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetFqn() const noexcept { return fqn_; }

    /// @brief Define reporting behavior
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using ReportingBehaviorEnum = isoft::manifestreader::tps::ExecutionStateReportingBehaviorEnum;

    /// @brief Whether it is a reporting process
    /// @return true is reporting process; false is not reporting process
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00073
    /// @needwork = dda
    /// @endcode
    bool IsExceutionReporting() const noexcept
    {
        return !(reportingBehavior_ == ReportingBehaviorEnum::kDoesNotReportExecutionState);
    }

    /// @brief Load configuration items of the executable file
    /// @param node Executable file configuration item node
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00074
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;

    /// @brief Check whether the executable file configuration is correct
    /// @return true configuration error; false configuration correct
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00075
    /// @needwork = dda
    /// @endcode
    bool HasConfigError() const noexcept;

    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept;

#ifdef EXTRA_EXECUTABLE_INFO
    /// @brief Get version number
    /// @return Version number
    ara::core::String const &GetVersion() const noexcept { return version_; }

    /// @brief Get compilation type
    /// @return Compilation type
    BuildTypeEnum const &GetBuildType() const noexcept { return buildType_; }

    /// @brief Get minimum time granularity
    /// @return Minimum time granularity
    double GetMininumTimerGranularity() const noexcept { return mininumTimerGranularity_; }

    /// @brief Whether to use logging
    /// @return true use logging; false do not use logging
    bool IsUseLogging() const noexcept
    {
        bool ret;
        switch (this->loggingBehavior_) {
            case LoggingEnum::kDoesNotUseLogging: {
                ret = false;
                break;
            }
            case LoggingEnum::kUsesLogging: {
                ret = true;
                break;
            }
            default: {
                break;
            }
        }
        return ret;
    }

    /// @brief Whether it is a platform application
    /// @return true is platform application; false is not platform application
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00072
    /// @needwork = dda
    /// @endcode
    bool IsPlatformApplication() const noexcept { return isPlatformApplication_; }
#endif

private:
    // PRQA S 2428,4151 ++ # Use char* to define json keys to be compatible with manifestreader

    /// @brief Executable file name
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kName{"name"};

    /// @brief Executable file FQN
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kFullQualifiedName{"fullQualifiedName"};

    /// @brief Reporting behavior
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kReportingBehavior{"reportingBehavior"};

    // PRQA S 2428,4151 -- # Use char* to define json keys to be compatible with manifestreader

#ifdef EXTRA_EXECUTABLE_INFO
    /// @brief Version number
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kVersion{"version"};

    /// @brief Executable file compilation type
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kBuildType{"buildType"};

    /// @brief Logging behavior
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kLoggingBehavior{"loggingBehavior"};

    /// @brief Minimum time precision
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kMininumTimerGranularity{"mininumTimerGranularity"};

    /// @brief Whether it is a platform application
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kPlatformApplication{"platform_application"};
#endif

    /// @brief Name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00076
    /// @needwork = dda
    /// @endcode
    ara::core::String name_{};
    /// @brief FQN
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00077
    /// @needwork = dda
    /// @endcode
    ara::core::String fqn_{};

    /// @brief Whether it is a reporting process
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00078
    /// @needwork = dda
    /// @endcode
    ReportingBehaviorEnum reportingBehavior_{ReportingBehaviorEnum::kReportsExecutionState};

    /// @brief Whether it is a platform application
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00079
    /// @needwork = dda
    /// @endcode
    bool isPlatformApplication_{false};

#ifdef EXTRA_EXECUTABLE_INFO
    /// @brief Version number
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ara::core::String version_;

    /// @brief Define compilation type
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using BuildTypeEnum = isoft::manifestreader::tps::BuildTypeEnum;

    /// @brief Compilation type
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    BuildTypeEnum buildType_;

    /// @brief Define logging behavior
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using LoggingEnum = isoft::manifestreader::tps::LoggingEnum;

    /// @brief Whether to use logging
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    LoggingEnum loggingBehavior_;

    /// @brief Minimum time granularity
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    double mininumTimerGranularity_;
#endif
};  ///< class Executable

/// @brief Processor information bound to the process
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_EM_20014
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00080
/// @needwork = ad
/// @endcode
class Processor
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00081
    /// @needwork = dda
    /// @endcode
    Processor() noexcept = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00082
    /// @needwork = dda
    /// @endcode
    ~Processor() noexcept = default;

    /// @brief Copy constructor
    /// @param other the other Processor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00083
    /// @needwork = dda
    /// @endcode
    Processor(Processor const &other) noexcept = default;

    /// @brief Disable move constructor
    /// @param other the other Processor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00084
    /// @needwork = dda
    /// @endcode
    Processor(Processor &&other) noexcept = default;

    /// @brief Disable move assignment
    /// @param other the other Processor
    /// @return New Processor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00085
    /// @needwork = dda
    /// @endcode
    Processor &operator=(Processor &&other) noexcept = delete;

    /// @brief Disable copy assignment
    /// @param other the other Processor
    /// @return New Processor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00086
    /// @needwork = dda
    /// @endcode
    Processor &operator=(Processor const &other) noexcept = delete;

    /// @brief Get processor ID
    /// @return Processor ID
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00087
    /// @needwork = dda
    /// @endcode
    uint16_t GetId() const noexcept { return id_; }

    /// @brief Core ID list
    /// @return Core ID list
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00088
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< uint16_t > const &GetCores() const noexcept { return cores_; }

    /// @brief Load processor information
    /// @param node Processor information node
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00089
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;

    /// @brief Check whether the Processor configuration is correct
    /// @return true configuration error; false configuration correct
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00090
    /// @needwork = dda
    /// @endcode
    bool HasConfigError() const noexcept;

    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept;

private:
    // PRQA S 2428,4151 ++ # Use char* to define json keys to be compatible with manifestreader

    /// @brief Processor ID
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kId{"id"};

    /// @brief Core ID
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kCore{"core"};

    // PRQA S 2428,4151 -- # Use char* to define json keys to be compatible with manifestreader

    /// @brief Processor ID
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00091
    /// @needwork = dda
    /// @endcode
    uint16_t id_{0U};

    /// @brief Core ID list
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00092
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< uint16_t > cores_;
};

/// @brief Execution dependency state enumeration
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_EM_20022
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00093
/// @needwork = dd
/// @endcode
enum class ExecutionDependencyState : uint8_t
{
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00820
    /// @needwork = dd
    /// @endcode
    kRunning = 0,  ///< Running state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00821
    /// @needwork = dd
    /// @endcode
    kTerminated = 1  ///< Terminated state
};

/// @brief Process execution dependency relationship
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_EM_20022
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00094
/// @needwork = dd
/// @endcode
class ExecutionDependency
{
public:
    /// @brief Redefine char type
    using Char8_t = char;

    /// @brief Default constructor
    /// @exception std::bad_alloc Thrown when memory allocation fails
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00095
    /// @needwork = dda
    /// @endcode
    ExecutionDependency() = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00096
    /// @needwork = dda
    /// @endcode
    ~ExecutionDependency() noexcept = default;
    /// @brief Disable copy constructor
    /// @param other Other execution dependency
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00097
    /// @needwork = dda
    /// @endcode
    ExecutionDependency(ExecutionDependency const &other) = default;

    /// @brief Disable move constructor
    /// @param other Other execution dependency
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00098
    /// @needwork = dda
    /// @endcode
    ExecutionDependency(ExecutionDependency &&other) noexcept = delete;

    /// @brief Disable move assignment
    /// @param other Other execution dependency
    /// @return New execution dependency
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00099
    /// @needwork = dda
    /// @endcode
    ExecutionDependency &operator=(ExecutionDependency &&other) noexcept = delete;

    /// @brief Disable copy assignment
    /// @param other Other execution dependency
    /// @return New execution dependency
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00100
    /// @needwork = dda
    /// @endcode
    ExecutionDependency &operator=(ExecutionDependency const &other) noexcept = delete;

    /// @brief Get the dependent process name
    /// @return Process name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00101
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetProcessName() const noexcept { return procName_; }

    /// @brief Get the execution state of the dependent process
    /// @return Execution state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00102
    /// @needwork = dda
    /// @endcode
    ExecutionDependencyState const &GetProcessState() const noexcept { return procState_; }

    /// @brief Load configuration from string
    /// @param execDependStr Dependency relationship string
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00103
    /// @needwork = dda
    /// @endcode
    int32_t Load(ara::core::String const &execDependStr) noexcept;

    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept;

private:
    /// @brief Constant string for running state
    /// @return Running state string
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetRunningState() noexcept { return "running"; };

    /// @brief Constant string for terminated state
    /// @return Terminated state string
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetTerminatedState() noexcept { return "terminated"; };

    /// @brief Separator between process name and process state
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t kNameAndStateSeparator{'.'};

    /// @brief Process name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00104
    /// @needwork = dda
    /// @endcode
    ara::core::String procName_;

    /// @brief Process execution state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00105
    /// @needwork = dda
    /// @endcode
    ExecutionDependencyState procState_{ExecutionDependencyState::kRunning};
};

/// @brief Function group state
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_EM_20023
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00106
/// @needwork = dd
/// @endcode
class FunctionGroupConfig
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00107
    /// @needwork = dda
    /// @endcode
    FunctionGroupConfig() = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00108
    /// @needwork = dda
    /// @endcode
    ~FunctionGroupConfig() noexcept = default;

    /// @brief Disable copy constructor
    /// @param other Other function group configuration
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00109
    /// @needwork = dda
    /// @endcode
    FunctionGroupConfig(FunctionGroupConfig const &other) = default;

    /// @brief Disable move constructor
    /// @param other Other function group configuration
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00110
    /// @needwork = dda
    /// @endcode
    FunctionGroupConfig(FunctionGroupConfig &&other) noexcept = delete;

    /// @brief Disable move assignment
    /// @param other Other function group configuration
    /// @return New function group configuration
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00111
    /// @needwork = dda
    /// @endcode
    FunctionGroupConfig &operator=(FunctionGroupConfig &&other) noexcept = delete;

    /// @brief Disable copy assignment
    /// @param other Other function group configuration
    /// @return New function group configuration
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00112
    /// @needwork = dda
    /// @endcode
    FunctionGroupConfig &operator=(FunctionGroupConfig const &other) noexcept = delete;

    /// @brief Get the function group name
    /// @return Function group name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00113
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetFunctionGroupName() const noexcept { return functionGroupName_; }

    /// @brief Get the list of function group state names
    /// @return List of state names
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00114
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > const &GetStates() const noexcept { return states_; }

    /// @brief Load function group state configuration
    /// @param node Function group configuration item node
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00115
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;

    /// @brief Check whether the function group state configuration is correct
    /// @return true configuration error; false configuration correct
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00116
    /// @needwork = dda
    /// @endcode
    bool HasConfigError() const noexcept;

    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept;

private:
    // PRQA S 2428,4151 ++ # Use char* to define json keys to be compatible with manifestreader

    /// @brief name key
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kName{"name"};

    /// @brief state key
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kStates{"states"};

    // PRQA S 2428,4151 -- # Use char* to define json keys to be compatible with manifestreader

    /// @brief Function group name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00117
    /// @needwork = dda
    /// @endcode
    ara::core::String functionGroupName_;

    /// @brief List of function group state names
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00118
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > states_;
};

/// @brief Resource consumption configuration
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_EM_20017, SR_EM_20018
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00119
/// @needwork = dd
/// @endcode
class ResourceConsumption
{
public:
    /// @brief Get the process's heap usage limit
    /// @return Heap size limit (in bytes)
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00120
    /// @needwork = dda
    /// @endcode
    uint32_t GetHeapUsage() const noexcept { return heapUsage_; }

    /// @brief Get the process's system memory usage limit
    /// @return System memory usage limit (in bytes)
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00121
    /// @needwork = dda
    /// @endcode
    uint32_t GetSystemMemoryUsage() const noexcept { return systemMemoryUsage_; }

    /// @brief Load resource consumption configuration
    /// @param node Resource consumption configuration item node
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00122
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;

    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept;

private:
    // PRQA S 2428,4151 ++ # Use char* to define json keys to be compatible with manifestreader

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kHeapUsage{"heapUsage"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kSystemMemoryUsage{"systemMemoryUsage"};

    // PRQA S 2428,4151 -- # Use char* to define json keys to be compatible with manifestreader

    /// @brief Process heap usage limit
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00123
    /// @needwork = dda
    /// @endcode
    uint32_t heapUsage_;

    /// @brief Process system memory usage limit
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00124
    /// @needwork = dda
    /// @endcode
    uint32_t systemMemoryUsage_;
};

/// @brief Process scheduling policy enumeration
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_EM_20015
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00125
/// @needwork = dd
/// @endcode
enum class SchedulingPolicy : uint8_t
{
    kFIFO       = 0,  ///< First in first out scheduling policy
    kRoundRobin = 1,  ///< Round-robin scheduling policy
    kOther      = 2   ///< Other scheduling policies
};

/// @brief Process startup configuration, used to enable corresponding configuration options for the process when switching different function groups
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_EM_20011,SR_EM_20012,SR_EM_20015,SR_EM_20020,SR_EM_20021,SR_EM_20024
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00126
/// @needwork = dd
/// @endcode
class StartupConfig
{
public:
    /// @brief Get the list of environment variables
    /// @return List of process environment variables
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20021
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00127
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > const &GetEnvironmentVariables() const noexcept
    {
        return environmentVariable_;
    }

    /// @brief Get the process execution error code
    /// @return Execution error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20024
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00128
    /// @needwork = dda
    /// @endcode
    uint32_t GetExecutionError() const noexcept { return executionError_; }

    /// @brief Get the list of process parameters
    /// @return List of process parameters
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20020
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00129
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > const &GetProcessArguments() const noexcept { return processArgument_; }

    /// @brief Get the process scheduling policy
    /// @return Process scheduling policy
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20015
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00130
    /// @needwork = dda
    /// @endcode
    SchedulingPolicy const &GetSchedulingPolicy() const noexcept { return schedulingPolicy_; }

    /// @brief Get the process scheduling priority
    /// @return Process scheduling priority
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20015
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00131
    /// @needwork = dda
    /// @endcode
    uint32_t GetSchedulingPriority() const noexcept { return schedulingPriority_; }

    /// @brief Whether it is a self-terminating process
    /// @return true is self-terminating process; false is not self-terminating process
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20011
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00132
    /// @needwork = dda
    /// @endcode
    bool IsSelfTerminate() const noexcept
    {
        bool ret{false};
        switch (this->terminationBehavior_) {
            case isoft::manifestreader::tps::TerminationBehaviorEnum::kProcessIsNotSelfTerminating: {
                ret = false;
                break;
            }
            case isoft::manifestreader::tps::TerminationBehaviorEnum::kProcessIsSelfTerminating: {
                ret = true;
                break;
            }
            default: {
                break;
            }
        }
        return ret;
    }

    /// @brief Get the startup timeout
    /// @return Process startup timeout
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20012
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00133
    /// @needwork = dda
    /// @endcode
    double GetEnterTimeout() const noexcept { return enterTimeout_; }

    /// @brief Set the startup timeout
    /// @param enterTimeout The process startup timeout to be set (in seconds)
    /// @return No return value
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20012
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00134
    /// @needwork = dda
    /// @endcode
    void SetEnterTimeout(double const enterTimeout) noexcept { enterTimeout_ = enterTimeout; }

    /// @brief Get the process exit timeout
    /// @return Process exit timeout
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20012
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00135
    /// @needwork = dda
    /// @endcode
    double GetExitTimeout() const noexcept { return exitTimeout_; }

    /// @brief Set the process exit timeout
    /// @param exitTimeout The process exit timeout to be set
    /// @return No return value
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20012
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00136
    /// @needwork = dda
    /// @endcode
    void SetExitTimeout(double const exitTimeout) noexcept { exitTimeout_ = exitTimeout; }

    /// @brief Load the process startup configuration items
    /// @param node Process startup configuration item node
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00137
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;

    /// @brief Check whether the startup configuration items are correct
    /// @return true configuration error; false configuration correct
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00138
    /// @needwork = dda
    /// @endcode
    bool HasConfigError() const noexcept;

    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept;

private:
    // PRQA S 2428,4151 ++ # Use char* to define json keys to be compatible with manifestreader

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kEnvironmentVariable{"environmentVariable"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kExecutionError{"executionError"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kProcessArgument{"processArgument"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kSchedulingPolicy{"schedulingPolicy"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kSchedulingPriority{"schedulingPriority"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kTerminationBehavior{"terminationBehavior"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kEnterTimeout{"timeout.enterTimeout"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kExitTimeout{"timeout.exitTimeout"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kSchedPolicyEnumRR{"SCHED_RR"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kSchedPolicyEnumFIFO{"SCHED_FIFO"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kSchedPolicyEnumOther{"SCHED_OTHER"};

    // PRQA S 2428,4151 -- # Use char* to define json keys to be compatible with manifestreader

    /// @brief Environment variables
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20021
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00139
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > environmentVariable_;

    /// @brief Execution error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20024
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00140
    /// @needwork = dda
    /// @endcode
    uint32_t executionError_;

    /// @brief Process parameters
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20020
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00141
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > processArgument_;

    /// @brief Scheduling policy
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20015
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00142
    /// @needwork = dda
    /// @endcode
    SchedulingPolicy schedulingPolicy_;

    /// @brief Scheduling priority
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20015
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00143
    /// @needwork = dda
    /// @endcode
    uint32_t schedulingPriority_;

    /// @brief Define termination behavior
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using TerminationBehaviorEnum = isoft::manifestreader::tps::TerminationBehaviorEnum;

    /// @brief Termination behavior
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20011
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00144
    /// @needwork = dda
    /// @endcode
    TerminationBehaviorEnum terminationBehavior_;

    /// @brief Enter timeout
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20012
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00145
    /// @needwork = dda
    /// @endcode
    double enterTimeout_;

    /// @brief Exit timeout
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20012
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00146
    /// @needwork = dda
    /// @endcode
    double exitTimeout_;

    /// @brief Whether configuration is incorrect
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00147
    /// @needwork = dda
    /// @endcode
    bool hasConfigError_;
};

/// @brief Process state dependency configuration
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_20016,SR_EM_20022,SR_EM_20023
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00148
/// @needwork = dd
/// @endcode
class StateDependentConfig
{
public:
    /// @brief Get the list of dependency relationships
    /// @return Reference to the list of dependency relationships
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20022
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00149
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ExecutionDependency > const &GetExecutionDependencyList() const noexcept
    {
        return executionDependency_;
    }

    /// @brief Get the bound function group state
    /// @return Reference to the bound function group state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20023
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00150
    /// @needwork = dda
    /// @endcode
    FunctionGroupConfig const &GetFunctionGroupState() const noexcept { return functionGroupState_; }

    /// @brief Get the resource consumption limit
    /// @return Reference to the resource consumption limit
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20017, SR_EM_20018
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00151
    /// @needwork = dda
    /// @endcode
    ResourceConsumption const &GetResouceConsumption() const noexcept { return resouceConsumption_; }

    /// @brief Get the name of the bound resource group
    /// @return Reference to the bound resource group name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20016
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00152
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetResourceGroupName() const noexcept { return resourceGroupName_; }

    /// @brief Get the list of environment variables
    /// @return Reference to the list of environment variables
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20021
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00153
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > const &GetEnvironmentVariables() const noexcept
    {
        return startupConfig_.GetEnvironmentVariables();
    }

    /// @brief Get the execution error code
    /// @return Error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20024
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00154
    /// @needwork = dda
    /// @endcode
    uint32_t GetExecutionError() const noexcept { return startupConfig_.GetExecutionError(); }

    /// @brief Get the list of parameters
    /// @return Reference to the list of parameters
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20020
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00155
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > const &GetProcessArguments() const noexcept
    {
        return startupConfig_.GetProcessArguments();
    }

    /// @brief Get the scheduling policy
    /// @return Scheduling policy
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20015
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00156
    /// @needwork = dda
    /// @endcode
    SchedulingPolicy const &GetSchedulingPolicy() const noexcept { return startupConfig_.GetSchedulingPolicy(); }

    /// @brief Get the scheduling priority
    /// @return Scheduling priority
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20015
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00157
    /// @needwork = dda
    /// @endcode
    uint32_t GetSchedulingPriority() const noexcept { return startupConfig_.GetSchedulingPriority(); }

    /// @brief Whether it is a self-terminating process
    /// @return true is self-terminating process; false is not self-terminating process
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20011
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00158
    /// @needwork = dda
    /// @endcode
    bool IsSelfTerminate() const noexcept { return startupConfig_.IsSelfTerminate(); }

    /// @brief Get the startup timeout
    /// @return Process startup timeout
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20012
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00159
    /// @needwork = dda
    /// @endcode
    double GetEnterTimeout() const noexcept { return startupConfig_.GetEnterTimeout(); }

    /// @brief Set the startup timeout
    /// @param enterTimeout The process startup timeout to be set (in seconds)
    /// @return No return value
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20012
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00160
    /// @needwork = dda
    /// @endcode
    void SetEnterTimeout(double const enterTimeout) noexcept { return startupConfig_.SetEnterTimeout(enterTimeout); }

    /// @brief Get the process exit timeout
    /// @return Process exit timeout
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20012
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00161
    /// @needwork = dda
    /// @endcode
    double GetExitTimeout() const noexcept { return startupConfig_.GetExitTimeout(); }

    /// @brief Set the process exit timeout
    /// @param exitTimeout The process exit timeout to be set
    /// @return No return value
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20012
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00162
    /// @needwork = dda
    /// @endcode
    void SetExitTimeout(double const exitTimeout) noexcept { startupConfig_.SetExitTimeout(exitTimeout); }

    /// @brief Load the process's state dependency configuration
    /// @param node State dependency configuration item node
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00163
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;

    /// @brief Check whether the state dependency configuration is correct
    /// @return true configuration error; false configuration correct
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00164
    /// @needwork = dda
    /// @endcode
    bool HasConfigError() const noexcept;

    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept;

private:
    // PRQA S 2428,4151 ++ # Use char* to define json keys to be compatible with manifestreader

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kExecutionDependency{"executionDependency"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kFunctionGroupState{"functionGroupState"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kResourceConsumption{"resourceConsumption"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kResourceGroup{"resourceGroup"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kStartupConfig{"startupConfig"};

    // PRQA S 2428,4151 -- # Use char* to define json keys to be compatible with manifestreader

    /// @brief List of process dependency relationships
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20022
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00165
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ExecutionDependency > executionDependency_;

    /// @brief Process dependency string, needs further parsing
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20022
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00166
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > execDependStr_;

    /// @brief Bound function group state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20023
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00167
    /// @needwork = dda
    /// @endcode
    FunctionGroupConfig functionGroupState_;

    /// @brief Resource consumption limit
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20017, SR_EM_20018
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00168
    /// @needwork = dda
    /// @endcode
    ResourceConsumption resouceConsumption_;

    /// @brief Bound resource group name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20016
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00169
    /// @needwork = dda
    /// @endcode
    ara::core::String resourceGroupName_;

    /// @brief Startup configuration
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_20011, SR_EM_20012, SR_EM_20015, SR_EM_20020, SR_EM_20021
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00170
    /// @needwork = dda
    /// @endcode
    StartupConfig startupConfig_;

    /// @brief Whether the state dependency configuration is incorrect
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00171
    /// @needwork = dda
    /// @endcode
    bool hasConfigError_;
};

/// @brief Read and hold the Process execution manifest information
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_00066
/// @trace_id_ad=AD_EM_00111
/// @trace_id_dd=DD_EM_00172
/// @needwork = ad
/// @endcode
class ExecutionManifest
{
public:
    /// @brief Default constructor
    /// @exception std::bad_alloc Thrown when memory allocation fails
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00173
    /// @needwork = dda
    /// @endcode
    ExecutionManifest() = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00174
    /// @needwork = dda
    /// @endcode
    ~ExecutionManifest() = default;

    /// @brief Copy constructor
    /// @param other Other execution manifest
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ExecutionManifest(ExecutionManifest const &other) = delete;

    /// @brief Disable move constructor
    /// @param other Other execution manifest
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ExecutionManifest(ExecutionManifest &&other) noexcept = delete;

    /// @brief Disable copy assignment
    /// @param other Other execution manifest
    /// @return New execution manifest
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ExecutionManifest &operator=(ExecutionManifest const &other) = delete;

    /// @brief Disable move assignment
    /// @param other Other execution manifest
    /// @return New execution manifest
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ExecutionManifest &operator=(ExecutionManifest &&other) noexcept = delete;

    /// @brief Create an execution manifest object
    /// @param manifestPath Execution manifest path
    /// @return Execution manifest handle
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00175
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< ExecutionManifest > CreateInstance(  // PRQA S 2024
        ara::core::String const &manifestPath) noexcept
    {
        std::shared_ptr< ExecutionManifest > mf{std::make_shared< ExecutionManifest >()};
        if (nullptr == mf) {
            return mf;
        }

        if (0 != mf->Load(manifestPath)) {
            mf.reset();
        }
        return mf;
    }

    /// @brief Load the execution manifest
    /// @param manifestPath Execution manifest json file path
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00176
    /// @needwork = dda
    /// @endcode
    int32_t Load(ara::core::String const &manifestPath) noexcept;

    /// @brief Get the software cluster name where the process is located
    /// @return Software cluster name where the process is located
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00177
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetSwclName() const noexcept { return swclName_; }

    /// @brief Set the software cluster name to which the process belongs
    /// @param swclName Software cluster name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00178
    /// @needwork = dda
    /// @endcode
    void SetSwclName(ara::core::String const &swclName) noexcept { swclName_ = swclName; }

    /// @brief Get the version number of the software cluster to which the process belongs
    /// @return Software cluster version number where the process is located
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00179
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetSwclVersion() const noexcept { return swclVersion_; }

    /// @brief Set the version of the software cluster to which the process belongs
    /// @param swclVersion Software cluster version number
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00180
    /// @needwork = dda
    /// @endcode
    void SetSwclVersion(ara::core::String const &swclVersion) noexcept { swclVersion_ = swclVersion; }

    /// @brief Get the process name corresponding to the execution manifest
    /// @return Process name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_EM_20008
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00181
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetProcName() const noexcept { return procName_; }

    /// @brief Get the process FQN
    /// @return Process FQN
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_EM_20008
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00182
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetProcFqn() const noexcept { return procFqn_; }

    /// @brief Get the executable program name
    /// @return Executable program name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_EM_20009
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00183
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetExecutableName() const noexcept { return executable_.GetName(); }

    /// @brief Get the executable program FQN
    /// @return Executable program FQN
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_EM_20009
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00184
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetExecutableFqn() const noexcept { return executable_.GetFqn(); }

    /// @brief Get the process's IPC name
    /// @return IPC name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_EM_20009
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00185
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetIpcName() const noexcept
    {
        if (!ipcName_.empty()) {
            return ipcName_;
        }

        return executable_.GetName();
    }

    /// @brief Get the maximum number of process restarts
    /// @return Number of restarts
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_EM_20013
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00186
    /// @needwork = dda
    /// @endcode
    uint16_t GetNumberOfRestartAttempts() const noexcept { return numberOfRestartAttempts_; }

    /// @brief Set the executable program path
    /// @param execPath Executable program path
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00188
    /// @needwork = dda
    /// @endcode
    int32_t SetExecutablePath(ara::core::String const &execPath) noexcept;

    /// @brief Get the executable program path
    /// @return Executable program path
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00189
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetExecutablePath() const noexcept
    {
        if (preMapping_) {
            return executablePreMappedPath_;
        }

        return executablePath_;
    }

    /// @brief Get the list of state dependency configurations
    /// @return List of state dependency configurations
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_EM_20011 ~ SR_EM_20024
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00190
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< StateDependentConfig > const &GetStateDependentConfigs() const noexcept
    {
        return stateDependentConfigs_;
    }

    /// @brief Get the bound processor
    /// @return Bound processor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_EM_20014
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00191
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< Processor > const &GetProcessors() const noexcept { return processors_; }

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
    void UpdateProcessors(ara::core::Vector< uint8_t > const &processors,
                          std::shared_ptr< isoft::osi::cpu::CpuInfo const > const &cpuInfo) noexcept;

    /// @brief Get the list of system environment variables
    /// @return List of system environment variables
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_EM_20004
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00193
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > const &GetSysEnvironments() const noexcept { return sysEnvironments_; }

    /// @brief Add a system environment variable
    /// @param env Environment variable separated by '='
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_EM_20004
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00194
    /// @needwork = dda
    /// @endcode
    void AddSysEnvironment(ara::core::String const &env) noexcept { sysEnvironments_.emplace_back(env); }

    /// @brief Add system environment variables
    /// @param envList List of environment variables
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_EM_20004
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00195
    /// @needwork = dda
    /// @endcode
    void AddSysEnvironment(ara::core::Vector< ara::core::String > const &envList) noexcept
    {
        if (!envList.empty()) {
            std::ignore
                = sysEnvironments_.insert(sysEnvironments_.cend(), envList.cbegin(), envList.cend());  // PRQA S 2961
        }
    }

    /// @brief Get the logical processors bound to the process
    /// @return List of logical processors bound to the process
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_EM_20014
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00196
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< uint16_t > const &GetCpuSet() const noexcept { return cpuSet_; }

    /// @brief Get the logical processors bound to the process
    /// @param cpuId Logical processor ID
    /// @return List of logical processors bound to the process
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_EM_20014
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00197
    /// @needwork = dda
    /// @endcode
    void AddCpuSet(uint16_t const cpuId) noexcept { cpuSet_.emplace_back(cpuId); }

    /// @brief Get the name of the function group to which the process belongs
    /// @return Function group name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_EM_20023
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00198
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetFunctionGroupName() const noexcept;

    /// @brief Find the startup configuration for the specified function group state
    /// @param fgState Function group state name
    /// @return Pointer to the obtained startup configuration; nullptr, failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00199
    /// @needwork = dda
    /// @endcode
    StateDependentConfig const *FindStartupConfig(ara::core::String const &fgState) const noexcept;

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
    ExecutionDependency const *FindExecutionDepend(ara::core::String const &fgState,
                                                   ara::core::String const &procName) const noexcept;

    /// @brief Update timer settings
    /// @param enterTimeout Startup timer value
    /// @param exitTimeout Exit timer value
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00201
    /// @needwork = dda
    /// @endcode
    void UpdateTimer(double const enterTimeout, double const exitTimeout) noexcept;

    /// @brief Get the list of function group state names that the process depends on
    /// @param nameList Save the obtained list of state names
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00202
    /// @needwork = dda
    /// @endcode
    void GetFunctionGroupStateNames(ara::core::Vector< ara::core::String > &nameList) const noexcept;

    /// @brief Get the list of process names that the process execution depends on
    /// @param fgState Function group name, only meaningful when specifying a function group state, because processes cannot depend across function group states
    /// @param nameList Save the obtained list of process names
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00203
    /// @needwork = dda
    /// @endcode
    void GetExecutionDependNames(ara::core::String const &fgState,
                                 ara::core::Vector< ara::core::String > &nameList) const noexcept;

    /// @brief Get the process execution error code
    /// @param fgState Function group state
    /// @return Execution error code
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00204
    /// @needwork = dda
    /// @endcode
    uint32_t GetExecutionError(ara::core::String const &fgState) const noexcept;

    /// @brief Determine whether it is a reporting process
    /// @return true is reporting process; false is not reporting process
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00205
    /// @needwork = dda
    /// @endcode
    bool IsReportingProcess() const noexcept { return executable_.IsExceutionReporting(); }

    /// @brief Determine whether it is a state management process
    /// @return true is state management process; false is not state management process
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00206
    /// @needwork = dda
    /// @endcode
    bool IsStateManager() const noexcept { return kStateManagerAffiliation == functionClusterAffiliation_; }

    /// @brief Determine whether it is UCM
    /// @return Yes/No
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00207
    /// @needwork = dda
    /// @endcode
    bool IsUpdateManager() const noexcept { return kUpdateManagerAffiliation == functionClusterAffiliation_; }

    /// @brief Check whether the execution manifest configuration is correct
    /// @return true configuration error; false configuration correct
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00208
    /// @needwork = dda
    /// @endcode
    bool HasConfigError() const noexcept;

    /// @brief Print content
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept;

private:
    /// @brief Get the list of process execution dependencies
    /// @param fgState Function group name, only meaningful when specifying a function group state, because processes cannot depend across function group states
    /// @param depList Save the obtained dependency list
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00211
    /// @needwork = dda
    /// @endcode
    void _GetExecutionDepends(ara::core::String const &fgState,
                              ara::core::Vector< ExecutionDependency > &depList) const noexcept;

private:
    // PRQA S 2428,4151 ++ # Use char* to define json keys to be compatible with manifestreader

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kName{"name"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kFullQualifiedName{"fullQualifiedName"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kExecutable{"executable"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kFunctionClusterAffiliation{"functionClusterAffiliation"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kNumberOfRestartAttempts{"numberOfRestartAttempts"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kPreMapping{"preMapping"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kIpcName{"ipcName"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kSecurityEvent{"securityEvent"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kStateDependentStartupConfig{"stateDependentStartupConfig"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kProcessor{"processor"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kStateManagerAffiliation{"STATE_MANAGEMENT"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kUpdateManagerAffiliation{"ISOFT_UCM"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kPlatformName{"core"};

    // PRQA S 2428,4151 -- # Use char* to define json keys to be compatible with manifestreader

    /// @brief Software cluster name where the process is located
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00212
    /// @needwork = dda
    /// @endcode
    ara::core::String swclName_;

    /// @brief Software cluster version number where the process is located
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00213
    /// @needwork = dda
    /// @endcode
    ara::core::String swclVersion_;

    /// @brief Process name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00214
    /// @needwork = dda
    /// @endcode
    ara::core::String procName_;

    /// @brief Process FQN
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00215
    /// @needwork = dda
    /// @endcode
    ara::core::String procFqn_;

    /// @brief Executable program
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00216
    /// @needwork = dda
    /// @endcode
    Executable executable_;

    /// @brief Executable program path
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00217
    /// @needwork = dda
    /// @endcode
    ara::core::String executablePath_;

    /// @brief Executable program preload path
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00218
    /// @needwork = dda
    /// @endcode
    ara::core::String executablePreMappedPath_;

    /// @brief Function group affiliation
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00219
    /// @needwork = dda
    /// @endcode
    ara::core::String functionClusterAffiliation_;

    /// @brief Number of restarts
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00220
    /// @needwork = dda
    /// @endcode
    uint16_t numberOfRestartAttempts_{0U};

    /// @brief Whether to pre-map before startup
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00221
    /// @needwork = dda
    /// @endcode
    bool preMapping_{false};

    /// @brief IPC name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00222
    /// @needwork = dda
    /// @endcode
    ara::core::String ipcName_;

    /// @brief List of state dependency configurations
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00223
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< StateDependentConfig > stateDependentConfigs_;

    /// @brief Bound processor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00224
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< Processor > processors_;

    /// @brief Configuration from the machine manifest and system environment variables that EM cares about
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00225
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > sysEnvironments_;

    /// @brief Logical processors bound to the process, converted from processor combined with the actual machine situation
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00226
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< uint16_t > cpuSet_;

    /// @brief Whether the execution manifest configuration is incorrect
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00111
    /// @trace_id_dd=DD_EM_00227
    /// @needwork = dda
    /// @endcode
    bool hasConfigError_{true};
};

}  // namespace config
}  // namespace internal
}  // namespace exec
}  // namespace ara
#endif  ///< ARA_EXEC_INTERNAL_CONFIG_EXECUTION_MANIFEST_H_
