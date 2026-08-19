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
/// @file       exec_trust_platform.h
/// @brief      Trusted platform class, provides verification of executable files, lib files, and manifest files
/// @details
/// @date       2023-04-01
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Emd
/// @unit_name=ExecTrustPlatform
/// @unit_description=Used to manage the trust platform for executable/config/library files.
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_EXEC_TRUST_PLATFORM_H_
#define _ARA_EXEC_INTERNAL_EXEC_TRUST_PLATFORM_H_

#include <ara/core/map.h>
#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/manifestreader/tps_enumeration.h>
#include <openssl/evp.h>

#include <set>
#include <string>

namespace ara {
namespace exec {
namespace internal {
namespace emd {

/// @brief Execution trusted platform
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_00031~SR_EM_00036
/// @trace_id_ad=AD_EM_00121
/// @trace_id_dd=DD_EM_00490
/// @needwork = ad
/// @endcode
class ExecTrustPlatform  // PRQA S 5215 # Suppress class name and file name inconsistency issue
{
public:
    /// @brief char type redefinition
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using Char8_t = char;

    /// @brief Get the log context ID
    /// @return Log context ID
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetLogCtxId() noexcept { return "EMTP"; }

    /// @brief Get the log context description
    /// @return Log context description
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetLogCtxDesc() noexcept { return "EMD Trust Platform"; }

    /// @brief Trusted platform boot mode enumeration type
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using TrustedPlatformLaunchBehaviorEnum = isoft::manifestreader::tps::TrustedPlatformExecutableLaunchBehaviorEnum;

    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00491
    /// @needwork = dda
    /// @endcode
    ExecTrustPlatform() noexcept = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00492
    /// @needwork = dda
    /// @endcode
    ~ExecTrustPlatform() noexcept = default;

    /// @brief Default copy constructor
    /// @param other Other ExecTrustPlatform object
    ExecTrustPlatform(ExecTrustPlatform const &other) = default;

    /// @brief Default move constructor
    /// @param other Other ExecTrustPlatform object
    ExecTrustPlatform(ExecTrustPlatform &&other) = default;

    /// @brief Default copy assignment
    /// @param other Other ExecTrustPlatform object
    /// @return New ExecTrustPlatform object
    ExecTrustPlatform &operator=(ExecTrustPlatform const &other) = default;

    /// @brief Default move assignment
    /// @param other Other ExecTrustPlatform object
    /// @return New ExecTrustPlatform object
    ExecTrustPlatform &operator=(ExecTrustPlatform &&other) = default;

    /// @brief Initialize (get trust anchor)
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00493
    /// @needwork = dda
    /// @endcode
    static void Initialize() noexcept;

    /// @brief Reset (release public key, set trust anchor unavailable, etc.)
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00494
    /// @needwork = dda
    /// @endcode
    static void DeInitialize() noexcept;

    /// @brief Set the startup behavior
    /// @param launchBehavior Startup behavior
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00495
    /// @needwork = dda
    /// @endcode
    static void SetLaunchBehavior(TrustedPlatformLaunchBehaviorEnum const launchBehavior) noexcept;

    /// @brief Verify the integrity and authenticity of the machine manifest file
    /// @param machineManifest Machine manifest
    /// @param platformSecDir Platform security directory
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sws=SWS_EM_02300 Execution management should ensure the integrity and authenticity of the processed machine manifest is checked
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00496
    /// @needwork = dda
    /// @endcode
    static int32_t VerifyMachineManifest(std::string const &machineManifest,
                                         std::string const &platformSecDir) noexcept;

    /// @brief Verify the integrity and authenticity of the process execution manifest file
    /// @param swclName Software cluster name
    /// @param swclVersion Software cluster version number
    /// @param procName Process name
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sws=SWS_EM_02303 Execution management should ensure the integrity and authenticity of the relevant execution manifest is checked for each process to be started
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00497
    /// @needwork = dda
    /// @endcode
    static int32_t VerifyProcManifest(std::string const &swclName,
                                      std::string const &swclVersion,
                                      std::string const &procName) noexcept;

    /// @brief Verify the integrity and authenticity of the process executable file and dependent libraries
    /// @param swclName Software cluster name
    /// @param swclVersion Software cluster version number
    /// @param procName Process name
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sws=SWS_EM_02301 Execution management should ensure the integrity and authenticity of the executable file itself is checked for each process to be started
    /// @trace_id_sws=SWS_EM_02302 Execution management should ensure the integrity and authenticity of dependent shared libraries is checked for each process to be started
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00498
    /// @needwork = dda
    /// @endcode
    static int32_t VerifyProcExecObj(std::string const &swclName,
                                     std::string const &swclVersion,
                                     std::string const &procName) noexcept;

private:
    /// @brief Check whether the trust anchor is available
    /// @return true available; false unavailable
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00501
    /// @needwork = dda
    /// @endcode
    static bool IsTrustPlatformEnabled() noexcept { return (s_IsTrustPlatformEnabled_ && (s_EvpPubKey_ != nullptr)); }

    /// @brief Whether execution can continue when signature verification, integrity check, etc. fail
    /// @return true can continue execution; false cannot continue execution
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00502
    /// @needwork = dda
    /// @endcode
    static bool IsAllowedToContinue() noexcept
    {
        return TrustedPlatformLaunchBehaviorEnum::kStrictMode != s_LaunchBehavior_;
    }

    /// @brief Get the file extension
    /// @param fileName File name
    /// @return File extension
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00503
    /// @needwork = dda
    /// @endcode
    static std::string GetExtName(std::string const &fileName) noexcept
    {
        std::string extName{""};
        std::size_t const pos{fileName.find_last_of('.')};
        if (std::string::npos != pos) {
            extName = fileName.substr(pos + 1U);
        }

        return extName;
    }

    /// @brief Load the hash file of the process
    /// @param swclName Software cluster name
    /// @param procName Process name
    /// @param hashFile Hash file name
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00504
    /// @needwork = dda
    /// @endcode
    static int32_t LoadHashFile(std::string const &swclName,
                                std::string const &procName,
                                std::string const &hashFile) noexcept;

    /// @brief Verify the file hash
    /// @param hashValue Expected file hash value
    /// @param hashAlgo Hash algorithm
    /// @param fileName File to verify the hash value
    /// @return true success; false failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00505
    /// @needwork = dda
    /// @endcode
    static bool VerifyFileHash(std::string const &hashValue,
                               std::string const &hashAlgo,
                               std::string const &fileName) noexcept;

    /// @brief Verify the manifest file hash
    /// @param swclName Software cluster name
    /// @param swclVersion Software cluster version
    /// @param procName Process name
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00506
    /// @needwork = dda
    /// @endcode
    static int32_t VerifyManifestHash(std::string const &swclName,
                                      std::string const &swclVersion,
                                      std::string const &procName) noexcept;

    /// @brief Get the executable file path
    /// @param swclName Software cluster name
    /// @param swclVersion Software cluster version
    /// @param execObjName Executable object name
    /// @return Non-empty string executable program path; empty string failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00507
    /// @needwork = dda
    /// @endcode
    static std::string GetExecObjPath(std::string const &swclName,
                                      std::string const &swclVersion,
                                      std::string const &execObjName) noexcept;

    /// @brief Verify the hash of the process executable file and dependent lib files
    /// @param swclName Software cluster name
    /// @param swclVersion Software cluster version
    /// @param procName Process name
    /// @return true success; false failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00508
    /// @needwork = dda
    /// @endcode
    static int32_t VerifyExecObjHash(std::string const &swclName,
                                     std::string const &swclVersion,
                                     std::string const &procName) noexcept;

private:
    /// @brief Public key information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00509
    /// @needwork = dda
    /// @endcode
    static EVP_PKEY *s_EvpPubKey_;

    /// @brief Whether the trust anchor is available
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00510
    /// @needwork = dda
    /// @endcode
    static bool s_IsTrustPlatformEnabled_;

    /// @brief Startup behavior
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00511
    /// @needwork = dda
    /// @endcode
    static TrustedPlatformLaunchBehaviorEnum s_LaunchBehavior_;

    /// @brief Platform file information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00512
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< isoft::ara_fsh::Platform > s_Fsh_;

    // PRQA S 2025,2026 ++ # Suppress comment format issue

    /// @brief Stores verified executable files and lib libraries
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00513
    /// @needwork = dda
    /// @endcode
    static std::set< std::string > s_VerfiedExecObj_;

    /// @brief Hash information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_00031~SR_EM_00036
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00514
    /// @needwork = dda
    /// @endcode
    struct HashInfo
    {
        /// @brief Hash algorithm
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00121
        /// @trace_id_dd=DD_EM_00786
        /// @needwork = dda
        /// @endcode
        std::string algo;
        /// @brief Hash value
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00121
        /// @trace_id_dd=DD_EM_00787
        /// @needwork = dda
        /// @endcode
        std::string value;
    };

    /// @brief Signature information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00121
    /// @trace_id_dd=DD_EM_00515
    /// @needwork = dda
    /// @endcode
    static ara::core::Map< std::string, ara::core::Map< std::string, ara::core::Map< std::string, HashInfo > > >
        s_SignInfoOfProc_;
};

}  // namespace emd
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_EXEC_TRUST_PLATFORM_H_
