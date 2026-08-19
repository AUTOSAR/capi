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
/// @file       exec_trust_platform.cpp
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
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "ara/exec/internal/emd/exec_trust_platform.h"

#include <unistd.h>

#include <array>

#include "ara/core/vector.h"
#include "ara/exec/internal/log/log.h"
#include "isoft/utils/error.h"
#include "isoft/utils/security.h"
#include "isoft/utils/string.h"

#ifdef LOG
    #undef LOG
#endif

/// @brief Trusted platform log macro definition
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define LOG() ara::exec::internal::log1::Log< ExecTrustPlatform >()

namespace ara {
namespace exec {
namespace internal {
namespace emd {

namespace {

/// @brief Redefine character type
using Char8_t = char;  // PRQA S 2025

/// @brief Hash information has 3 fields
constexpr uint8_t const kThreeFields{3U};

constexpr uint8_t const kFirstField{0U};
constexpr uint8_t const kSecondField{1U};
constexpr uint8_t const kThirdField{2U};

/// @brief Hash information length
constexpr std::size_t const kSizeOfHashInfo{1024U};

/// @brief Reserve space for the public key
constexpr std::size_t const kSizeOfPubKey{1024U};

/// @brief Global variable storing the public key, placed in a specific section
/// @note The following attribute places the public key in a custom section named "PUBLICKEY".
/// This is GCC/Clang-specific and may not be portable to other compilers.
/// If cross-platform support is needed, consider defining a macro for section attributes.
__attribute__((section("PUBLICKEY"))) std::array< Char8_t, kSizeOfPubKey > g_Pubkey;  // NOLINT

}  // namespace

/// @brief Public key information
EVP_PKEY *ExecTrustPlatform::s_EvpPubKey_{nullptr};

/// @brief Whether the trust anchor is available
bool ExecTrustPlatform::s_IsTrustPlatformEnabled_{false};

/// @brief Startup behavior
ExecTrustPlatform::TrustedPlatformLaunchBehaviorEnum ExecTrustPlatform::s_LaunchBehavior_{
    ExecTrustPlatform::TrustedPlatformLaunchBehaviorEnum::kNoTrustedPlatformSupport};

/// @brief Platform file information
std::shared_ptr< isoft::ara_fsh::Platform > ExecTrustPlatform::s_Fsh_{nullptr};  // NOLINT

/// @brief Stores verified executable files and lib libraries
std::set< std::string > ExecTrustPlatform::s_VerfiedExecObj_{};  // NOLINT

/// @brief Signature information
ara::core::Map< std::string,  // NOLINT
                ara::core::Map< std::string, ara::core::Map< std::string, ExecTrustPlatform::HashInfo > > >
    ExecTrustPlatform::s_SignInfoOfProc_{};

/// @brief Initialize (get trust anchor)
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00121
/// @trace_id_dd=DD_EM_00493
/// @needwork = dda
/// @endcode
void ExecTrustPlatform::Initialize() noexcept
{
    if (strnlen(g_Pubkey.data(), kSizeOfPubKey) == 0U) {
        s_IsTrustPlatformEnabled_ = false;
        LOGD() << "No available trust anchor - No public key found !!!";
    } else {
        if (nullptr != s_EvpPubKey_) {
            return;
        }
        s_EvpPubKey_ = isoft::utils::security::CreateEvpPubKey(g_Pubkey.data());
        if (nullptr == s_EvpPubKey_) {
            s_IsTrustPlatformEnabled_ = false;
            s_Fsh_.reset();
            LOGD() << "No available trust anchor  - Public key is invalid !!!";
        } else {
            s_IsTrustPlatformEnabled_ = true;
            s_LaunchBehavior_         = TrustedPlatformLaunchBehaviorEnum::kStrictMode;
            s_Fsh_                    = isoft::ara_fsh::Platform::CreatePlatform();
            LOGD() << "Found available trust anchor !!!";
        }
    }
}

/// @brief Reset (release public key, set trust anchor unavailable, etc.)
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00121
/// @trace_id_dd=DD_EM_00494
/// @needwork = dda
/// @endcode
void ExecTrustPlatform::DeInitialize() noexcept
{
    if (s_EvpPubKey_ != nullptr) {
        isoft::utils::security::DeleteEvpPubKey(s_EvpPubKey_);
        s_EvpPubKey_ = nullptr;
    }

    s_IsTrustPlatformEnabled_ = false;
    s_VerfiedExecObj_.clear();
    s_SignInfoOfProc_.clear();
}

/// @brief Set the startup behavior
/// @param launchBehavior Startup behavior
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00121
/// @trace_id_dd=DD_EM_00495
/// @needwork = dda
/// @endcode
void ExecTrustPlatform::SetLaunchBehavior(TrustedPlatformLaunchBehaviorEnum const launchBehavior) noexcept
{
    s_LaunchBehavior_ = launchBehavior;

    if (TrustedPlatformLaunchBehaviorEnum::kNoTrustedPlatformSupport == s_LaunchBehavior_) {
        s_IsTrustPlatformEnabled_ = false;
    } else {
        s_IsTrustPlatformEnabled_ = true;
    }

    LOGD() << "LaunchBehavior is" << isoft::manifestreader::tps::ToString(launchBehavior);
}

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
int32_t ExecTrustPlatform::VerifyMachineManifest(std::string const &machineManifest,
                                                 std::string const &platformSecDir) noexcept
{
    /// If there is no available trust anchor, do not perform signature verification, return success directly
    if (!IsTrustPlatformEnabled()) {
        return 0;
    }

    std::string const machineHashFile{platformSecDir + isoft::ara_fsh::Platform::kMachineManifestHashFile};
    std::string const machineSignFile{platformSecDir + isoft::ara_fsh::Platform::kMachineManifestSignFile};
    if (isoft::utils::security::VerifySignature(s_EvpPubKey_, std::string(isoft::utils::security::GetSha256DgstAlgo()),
                                                machineHashFile, machineSignFile)
        != 0) {
        LOGE() << "Verify failed !!!";
        return -1;
    }

    FILE *const fp{fopen(machineHashFile.c_str(), "re")};
    if (fp == nullptr) {
        LOGE() << "Open unsigned file(" << machineHashFile << ") failed - " << isoft::utils::StrError();
        return -1;
    }

    std::vector< std::string > splitStrs;
    ara::core::Vector< Char8_t > hashInfo(kSizeOfHashInfo);
    while (true) {
        if (nullptr == fgets(hashInfo.data(), static_cast< std::int32_t >(kSizeOfHashInfo), fp)) {
            break;
        }
        std::string tmpStr(hashInfo.data());
        std::ignore = isoft::utils::TrimSpace(tmpStr);
        isoft::utils::SplitString(tmpStr, splitStrs);

        /// {fileName DegestAlgo Degest}
        if (splitStrs.size() == kThreeFields) {
            if (splitStrs[kFirstField].compare(isoft::ara_fsh::Platform::kMachineManifest) != 0) {
                LOGW() << "Not Machine Manifest: " << splitStrs[kFirstField];
                splitStrs.clear();
                continue;
            }

            bool const isMd5{splitStrs[kSecondField].compare(isoft::utils::security::GetMd5DgstAlgo()) == 0};
            bool const isSha1{splitStrs[kSecondField].compare(isoft::utils::security::GetSha1DgstAlgo()) == 0};
            bool const isSha256{splitStrs[kSecondField].compare(isoft::utils::security::GetSha256DgstAlgo()) == 0};

            if (!isMd5 && !isSha1 && !isSha256) {
                LOGW() << "Unsupported Hash Algorithm: " << splitStrs[kSecondField] << " !!!";
                splitStrs.clear();
                continue;
            }

            if (splitStrs[kThirdField].empty()) {
                LOGW() << "No Hash Degest found !!!";
                splitStrs.clear();
                continue;
            }
        } else {
            LOGW() << "Empty line or Wrongly configured line !!!";
            splitStrs.clear();
        }
    }
    std::ignore = fclose(fp);

    if (splitStrs.empty()) {
        LOGE() << "Not find the hash info !!!";
        return -1;
    }

    if (!VerifyFileHash(splitStrs[kThirdField], splitStrs[kSecondField], machineManifest)) {
        LOGE() << "File (" << machineManifest << ") is tempered !!!";
        return -1;
    }

    return 0;
}

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
int32_t ExecTrustPlatform::VerifyProcManifest(std::string const &swclName,
                                              std::string const &swclVersion,
                                              std::string const &procName) noexcept
{
    /// If there is no available trust anchor, do not perform signature verification, return success directly
    if (!IsTrustPlatformEnabled()) {
        return 0;
    }

    std::string unsignedHashFile;
    std::string signedHashFile;
    bool const isCore{swclName.compare(isoft::ara_fsh::Platform::kCore) == 0};
    bool const isFramework{swclName.compare(isoft::ara_fsh::Platform::kFramework) == 0};
    if (isCore || isFramework) {
        signedHashFile   = s_Fsh_->GetPlatformSecurityDir();
        unsignedHashFile = signedHashFile;
    } else {
        signedHashFile   = s_Fsh_->GetSwclSecurityDir(swclName, swclVersion);
        unsignedHashFile = signedHashFile;
    }

    unsignedHashFile += procName + ".hash";
    signedHashFile += procName + ".hash.sign";

    int32_t ret{isoft::utils::security::VerifySignature(
        s_EvpPubKey_, std::string(isoft::utils::security::GetSha256DgstAlgo()), unsignedHashFile, signedHashFile)};
    if (0 != ret) {
        LOGE() << "Failed to verify hash file signature (" << unsignedHashFile << ", " << signedHashFile << ") with"
               << ret << "!!!";
        if (!IsAllowedToContinue()) {
            return -1;
        }
    } else {
        LOGD() << "Verify hash file signature (" << unsignedHashFile << ", " << signedHashFile << ") successfully";
    }

    ret = LoadHashFile(swclName, procName, unsignedHashFile);
    if (0 != ret) {
        LOGE() << "Load Hash File failed with" << ret << "!!!";
        if (!IsAllowedToContinue()) {
            return -1;
        }
    }

    ret = VerifyManifestHash(swclName, swclVersion, procName);
    if (0 != ret) {
        LOGE() << "Verify Manifest Hash failed with" << ret << "!!!";
        if (!IsAllowedToContinue()) {
            return -1;
        }
    }

    return 0;
}

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
int32_t ExecTrustPlatform::VerifyProcExecObj(std::string const &swclName,
                                             std::string const &swclVersion,
                                             std::string const &procName) noexcept
{
    /// If there is no available trust anchor, do not perform signature verification, return success directly
    if (!IsTrustPlatformEnabled()) {
        return 0;
    }

    /// Because when loading the manifest, the hash file has already been verified and loaded into memory
    /// So there is no need to verify the signature and reload the hash file, just verify whether the hash is complete
    int32_t const ret{VerifyExecObjHash(swclName, swclVersion, procName)};
    if (0 != ret) {
        LOGE() << "Verify Exec/Obj Hash failed with" << ret << "!!!";
        if (!IsAllowedToContinue()) {
            return -1;
        }
    }

    return 0;
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
int32_t ExecTrustPlatform::LoadHashFile(std::string const &swclName,
                                        std::string const &procName,
                                        std::string const &hashFile) noexcept
{
    FILE *const fp{fopen(hashFile.c_str(), "re")};
    if (fp == nullptr) {
        LOGE() << "Open unsigned file(" << hashFile << ") failed - " << isoft::utils::StrError();
        return -1;
    }

    std::vector< std::string > splitStrs;
    ara::core::Vector< Char8_t > hashInfo(kSizeOfHashInfo);
    while (true) {
        if (nullptr == fgets(hashInfo.data(), static_cast< int32_t >(kSizeOfHashInfo), fp)) {
            break;
        }
        std::string tmpStr(hashInfo.data());
        std::ignore = isoft::utils::TrimSpace(tmpStr);
        isoft::utils::SplitString(tmpStr, splitStrs);

        if (kThreeFields != splitStrs.size()) {
            continue;
        }
        /// {swclName, {procName, {fileName, {hashAlgo, hashValue}}}}
        s_SignInfoOfProc_[swclName][procName][splitStrs[kFirstField]]
            = {splitStrs[kSecondField], splitStrs[kThirdField]};
        splitStrs.clear();
    }
    LOGD() << "Load " << s_SignInfoOfProc_[swclName][procName].size() << " hash value for process" << procName << "of"
           << swclName;
    std::ignore = fclose(fp);
    return 0;
}

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
bool ExecTrustPlatform::VerifyFileHash(std::string const &hashValue,
                                       std::string const &hashAlgo,
                                       std::string const &fileName) noexcept
{
    std::string const fileHashValue{isoft::utils::security::GetDigestOfFile(fileName, hashAlgo)};
    bool const isHashEmpty{fileHashValue.empty()};
    bool const isNotSame{hashValue != fileHashValue};
    if (isHashEmpty || isNotSame) {
        if (IsAllowedToContinue()) {
            LOGD() << "Failed to verify the hash of file (" << fileName << ") in non-strick mode !!!";
        } else {
            LOGE() << "Failed to verify the hash of file (" << fileName << ") !!!";
            return false;
        }
    } else {
        LOGD() << "Verify the hash of file (" << fileName << ") successfully !!!";
    }

    return true;
}

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
int32_t ExecTrustPlatform::VerifyManifestHash(std::string const &swclName,
                                              std::string const &swclVersion,
                                              std::string const &procName) noexcept
{
    int32_t ret{0};
    bool findManifest{false};
    for (auto &signInfo : s_SignInfoOfProc_[swclName][procName]) {
        std::string const extName{GetExtName(signInfo.first)};
        if ("json" != extName) {
            continue;
        }

        /// The execution manifest must be verified. If not configured, it is considered a configuration error
        if (0 == signInfo.first.compare("manifest.json")) {
            findManifest = true;
        }

        std::string const manifestPath{s_Fsh_->GetSwclRootDir(swclName, swclVersion) + isoft::ara_fsh::Platform::kEtc
                                       + procName};
        if (0 != access(manifestPath.c_str(), F_OK | R_OK)) {
            ret = -1;
            continue;
        }

        if (!VerifyFileHash(signInfo.second.value, signInfo.second.algo, manifestPath + "/" + signInfo.first)) {
            ret = -1;
        }
    }

    if (!findManifest) {
        LOGE() << "No manifest hash is configured for" << procName;
        ret = -1;
    }

    return ret;
}

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
std::string ExecTrustPlatform::GetExecObjPath(std::string const &swclName,
                                              std::string const &swclVersion,
                                              std::string const &execObjName) noexcept
{
    std::string execObjPath{""};
    std::string const extName{GetExtName(execObjName)};
    bool const hasExtName{!extName.empty()};
    bool const notLibFile{"so" != extName};
    if (hasExtName && notLibFile) {
        return "";
    }

    bool const isCore{isoft::ara_fsh::Platform::kCore == swclName};
    bool const isFramework{isoft::ara_fsh::Platform::kFramework == swclName};
    if (isCore || isFramework) {
        /// First search in framework
        if (extName.empty()) {  /// Get the executable file path
            execObjPath = s_Fsh_->GetSwclBinDir(std::string(s_Fsh_->kFramework), swclVersion);
        } else {
            execObjPath = s_Fsh_->GetSwclLibDir(std::string(s_Fsh_->kFramework), swclVersion);
        }

        std::ignore = execObjPath.append(execObjName);
        if (0 == access(execObjPath.c_str(), F_OK | R_OK)) {
            return execObjPath;
        }

        /// Then search in core
        if (extName.empty()) {  /// Get the executable file path
            execObjPath = s_Fsh_->GetSwclBinDir(std::string(s_Fsh_->kCore), swclVersion);
        } else {
            execObjPath = s_Fsh_->GetSwclLibDir(std::string(s_Fsh_->kCore), swclVersion);
        }

        std::ignore = execObjPath.append(execObjName);
        if (0 == access(execObjPath.c_str(), F_OK | R_OK)) {
            return execObjPath;
        }

        LOGD() << "The executable or shared library doesn't exit (" << execObjPath << ")";
        /// If not found, return an empty string
        return std::string{""};
    }
    if (extName.empty()) {
        execObjPath = s_Fsh_->GetSwclBinDir(swclName, swclVersion);
    } else {
        execObjPath = s_Fsh_->GetSwclLibDir(swclName, swclVersion);
    }

    std::ignore = execObjPath.append(execObjName);
    if (0 == access(execObjPath.c_str(), F_OK | R_OK)) {
        return execObjPath;
    }

    LOGW() << "The executable or shared library doesn't exit (" << execObjPath << ")";
    return std::string("");
}

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
int32_t ExecTrustPlatform::VerifyExecObjHash(std::string const &swclName,
                                             std::string const &swclVersion,
                                             std::string const &procName) noexcept
{
    LOGD() << "VerifyExecObjHash(): swclName =" << swclName << ", swclVersion =" << swclVersion
           << ", procName =" << procName;

    int32_t ret{0};
    bool findExec{false};

    for (auto &signInfo : s_SignInfoOfProc_[swclName][procName]) {
        /// Skip files that are not executables or dynamic libraries
        std::string const extName{GetExtName(signInfo.first)};
        bool const nameValid{!extName.empty()};
        if ("so" != extName && nameValid) {
            continue;
        }

        std::string const execObjPath{GetExecObjPath(swclName, swclVersion, signInfo.first)};
        if (execObjPath.empty()) {
            ret = -1;
            continue;
        }

        /// A process must correspond to an executable program. If there is no configuration to verify the executable program, it is considered a configuration error and this process will not be started
        /// If there is a hash check for the executable program, set the flag
        /// TODO: Check the dependent dynamic libraries of the executable program, verify each dynamic library
        if (extName.empty()) {
            findExec = true;
        }

        std::set< std::string >::iterator const end{s_VerfiedExecObj_.end()};
        if (s_VerfiedExecObj_.find(execObjPath) != end) {
            LOGD() << "Already verified, no need to verify again !!!";
            continue;
        }

        LOGD() << "VerifyExecObjHash(): swclName =" << swclName << ", procName = " << procName
               << ", fileName = " << signInfo.first;
        if (VerifyFileHash(signInfo.second.value, signInfo.second.algo, execObjPath)) {
            std::ignore = s_VerfiedExecObj_.emplace(execObjPath);
        } else {
            ret = -1;
        }
    }

    if (!findExec) {
        LOGE() << "No executable hash is configured for" << procName;
        ret = -1;
    }

    return ret;
}

}  // namespace emd
}  // namespace internal
}  // namespace exec
}  // namespace ara
