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
/// @file       helper.h
/// @brief      Common helper methods
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/UCM Master/Utils
/// @module_path=/UCM Master/Utils
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=Helper
/// @unit_description=Common helper methods
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_HELPER_H_
#define ARA_UCM_PKGMGR_HELPER_H_

#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <ara/crypto/common/entry_point.h>
#include <ara/crypto/cryp/crypto_provider.h>
#include <isoft/manifestreader/tps_enumeration.h>
#include <nai/os/nai_file.h>
#include <nai/runtime/nai_errno.h>
#include <nai/runtime/nai_util.h>

#include <fstream>
#include <iostream>
#include <set>

#include "ara/core/error_code.h"
#include "ara/ucm/internal/extraction/filesystem.h"
#include "ara/ucm/pkgmgr/impl_type_campaignhistorytype.h"
#include "ara/ucm/pkgmgr/impl_type_campaignresulttype.h"
#include "ara/ucm/pkgmgr/impl_type_ucmsteperrortype.h"
#include "consts.h"
#include "utils/types.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {
namespace helper {

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00486
/// @trace_id_dd=DD_UCM_Master_00853
/// @needwork = ad
/// @endcode
constexpr const char8_t* kPathSeparator{"/"};  // PRQA S 2428,4151

/// @brief Get the MD5 value of a file's content (lowercase hexadecimal string of length 32); returns an empty string if the file does not exist
/// @param fileName
///
/// @returns
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00486
/// @trace_id_dd=DD_UCM_Master_00854
/// @needwork = dd
/// @endcode
inline ara::core::String MD5ForFile(ara::core::String const& fileName) noexcept
{  // PRQA S 1503
    // (void)fileName;.
    // return "7B064DAD507C266A161FFC73C53DCDC5";.
    // Check if file exists
    const bool exist{pkgmgr::Filesystem::DoesFileExist(fileName)};
    if (!exist) {
        return ara::core::String("");
    }

    using ara::crypto::LoadCryptoProvider;
    using ara::crypto::ReadOnlyMemRegion;
    using ara::crypto::cryp::CryptoContext;
    using ara::crypto::cryp::CryptoProvider;
    using ara::crypto::cryp::HashFunctionCtx;

    // Calculate md5 value
    ara::core::InstanceSpecifier const iSpecify{core::StringView("isoft")};
    CryptoProvider::Uptr const cryptoProvider{LoadCryptoProvider(iSpecify)};
    CryptoContext::AlgId const algId{cryptoProvider->ConvertToAlgId(core::StringView("md5"))};

    ara::core::Result< void > hashResult;

    // Initialize context
    ara::core::Result< HashFunctionCtx::Uptr > const result{cryptoProvider->CreateHashFunctionCtx(algId)};
    HashFunctionCtx::Uptr const& pHashCrc{result.Value()};
    hashResult = pHashCrc->Start();
    assert(hashResult);

    // Open file
    nai_fd_t const fd{nai_file_open(fileName.c_str(), NAI_O_RDONLY)};  // PRQA S 3600
    assert(fd != NAI_FD_INVALID);                                      // PRQA S 3080
    intptr_t r{nai_file_seek(fd, 0, 0)};
    assert(r == 0);  // PRQA S 2410

    // Buffer
    size_t const kBufLen{1024U};
    uint8_t buf[kBufLen];  // PRQA S 2410

    // Read file
    while (true) {
        // Clear buffer
        std::ignore = nai_memset(buf, 0, kBufLen);      // PRQA S 3840, 4127
        r           = nai_file_read(fd, buf, kBufLen);  // PRQA S 3840
        assert(r >= 0);
        if (r > 0) {
            ReadOnlyMemRegion const memRegion{buf, static_cast< size_t >(r)};  // PRQA S 3840
            hashResult = pHashCrc->Update(memRegion);
            assert(hashResult);
        } else {
            break;
        }
    }

    // Close file
    r = nai_file_close(fd);
    assert(r == 0);

    // Finalize to get result
    ara::core::Vector< ara::core::Byte > resultVec{pHashCrc->Finish().Value()};

    // Number of characters in the MD5 string
    size_t const nMD5LenPerChar{2U};
    size_t const nMD5FullLenPerChar{3U};

    // Convert md5 value to hexadecimal
    size_t const len{resultVec.size()};
    assert(len == kMD5DigestLen);
    size_t const nMD5StrLen{kMD5DigestLen * 2U};
    core::String stMD5Str(nMD5StrLen, '\0');
    for (size_t i{0U}; i < len; i++) {
        std::ignore = snprintf(&stMD5Str[i * nMD5LenPerChar], nMD5FullLenPerChar, "%02x",
                               static_cast< uchar8_t >(resultVec[i]));
    }

    // Return result
    return stMD5Str;
}

/// @brief GetOption
/// @param args
/// @param optionName
/// @return option value
/// @throws no no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00486
/// @trace_id_dd=DD_UCM_Master_00855
/// @needwork = dd
/// @endcode
inline ara::core::String GetOption(std::list< ara::core::StringView > const& args,
                                   ara::core::StringView const& optionName)
{
    ara::core::String optionValue;

    std::list< ara::core::StringView >::const_iterator it;
    for (it = args.begin(); it != args.end(); ++it) {
        if (*it == optionName) {
            auto const nextIt = std::next(it);
            if (nextIt != args.end()) {
                optionValue = *nextIt;
                break;
            }
        }
    }

    return optionValue;
}

/// @brief SoftwarePackageStoringToString
/// @param storing
/// @return StringView
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00486
/// @trace_id_dd=DD_UCM_Master_00856
/// @needwork = dd
/// @endcode
inline ara::core::StringView SoftwarePackageStoringToString(
    isoft::manifestreader::tps::SoftwarePackageStoringEnum const storing)
{
    ara::core::StringView str;
    switch (storing) {
        case isoft::manifestreader::tps::SoftwarePackageStoringEnum::kNone: {
            str = ara::core::StringView("kNone");
            break;
        }
        case isoft::manifestreader::tps::SoftwarePackageStoringEnum::kUcmMaster: {
            str = ara::core::StringView("kUcmMaster");
            break;
        }
        case isoft::manifestreader::tps::SoftwarePackageStoringEnum::kUcm: {
            str = ara::core::StringView("kUcm");
            break;
        }
        default: {
            str = ara::core::StringView("Unknown");
            break;
        }
    }
    return str;
}

/// @brief UCMStepErrorTypeToString
/// @param ucmStepError
/// @return str
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00486
/// @trace_id_dd=DD_UCM_Master_00857
/// @needwork = dd
/// @endcode
inline ara::core::String UCMStepErrorTypeToString(pkgmgr::UCMStepErrorType const& ucmStepError)
{
    core::String padding;
    return "ucmId:" + ucmStepError.ucmId + " softwarePackageStep:"
           + com::internal::format::Formatter< core::String, pkgmgr::SoftwarePackageStepType >::ToString(
               padding, ucmStepError.softwarePackageStep)
           + " returnedError:" + std::to_string(static_cast< int32_t >(ucmStepError.returnedError));
}

/// @brief CampaignResultTypeToString
/// @param campaignResult
/// @return str
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00486
/// @trace_id_dd=DD_UCM_Master_00858
/// @needwork = dd
/// @endcode
inline ara::core::String CampaignResultTypeToString(pkgmgr::CampaignResultType const& campaignResult)
{
    core::String padding;
    core::String resolutionStr;

    auto const& campaignResolutionVec = campaignResult.campaignResolution;
    std::ignore                       = std::for_each(
        campaignResolutionVec.begin(),
        campaignResolutionVec.begin()
            + static_cast< ara::core::Vector< pkgmgr::UCMMasterResolutionType >::difference_type >(
                campaignResolutionVec.size()),
        [&padding, &resolutionStr](pkgmgr::UCMMasterResolutionType const& resolutionInfo) {
            std::ignore = resolutionStr.append(
                com::internal::format::Formatter< core::String, pkgmgr::UCMMasterResolutionType >::ToString(
                    padding, resolutionInfo));
            std::ignore = resolutionStr.append(",");
        });
    if (!resolutionStr.empty()) {  // Remove the trailing comma
        resolutionStr.pop_back();
    }

    core::String stUcmStepError;
    auto const& vecUcmStepError = campaignResult.UCMStepError;
    std::ignore                 = std::for_each(
        vecUcmStepError.begin(),
        vecUcmStepError.begin()
            + static_cast< ara::core::Vector< pkgmgr::UCMStepErrorType >::difference_type >(vecUcmStepError.size()),
        [&stUcmStepError](pkgmgr::UCMStepErrorType const& ucmStepErrorInfo) {
            std::ignore = stUcmStepError.append(UCMStepErrorTypeToString(ucmStepErrorInfo));
            std::ignore = stUcmStepError.append(",");
        });
    if (!stUcmStepError.empty()) {  // Remove the trailing comma
        stUcmStepError.pop_back();
    }

    return "campaignResolution:" + resolutionStr + " UCMStepError:" + stUcmStepError
           + " campaignStartTime:" + std::to_string(static_cast< uint64_t >(campaignResult.campaignStartTime))
           + " campaignResolutionTime:" + std::to_string(static_cast< uint64_t >(campaignResult.campaignResolutionTime))
           + " driverNotified:" + std::to_string(static_cast< int32_t >(campaignResult.driverNotified));
}

/// @brief CampaignHistoryTypeToString
/// @param campaignRecord
/// @return str
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00486
/// @trace_id_dd=DD_UCM_Master_00859
/// @needwork = dd
/// @endcode
inline ara::core::String CampaignHistoryTypeToString(pkgmgr::CampaignHistoryType const& campaignRecord)
{
    return "campaignResult:" + core::String(CampaignResultTypeToString(campaignRecord.campaignResult))
           + " historyVector.size():" + std::to_string(static_cast< int32_t >(campaignRecord.historyVector.size()))
           + " repository:" + campaignRecord.repository;
}

/// @brief convert string with format of bigcase
/// @param data source string
/// @returns string in format of bigcase
/// @throws no no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00486
/// @trace_id_dd=DD_UCM_Master_00860
/// @needwork = dd
/// @endcode
inline core::String ToUpper(core::String const& data)
{
    core::String ret{data};

    for (char8_t& c : ret) {
        c = static_cast< char8_t >(std::toupper(static_cast< std::int32_t >(static_cast< uchar8_t >(c))));
    }

    return ret;
}

/// @brief hex string convert to byte vector
/// @param hexStr source hex string
/// @returns byte vector
/// @throws no no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00486
/// @trace_id_dd=DD_UCM_Master_00861
/// @needwork = dd
/// @endcode
inline ara::core::Vector< ara::core::Byte > HexToBytes(core::String const& hexStr)
{
    ara::core::Vector< ara::core::Byte > ret;
    size_t const step{2U};

    for (size_t i{0U}; i < hexStr.length(); i += step) {
        ara::core::StringView const sub{hexStr.substr(i, step)};
        ara::core::Byte const byt{static_cast< uchar8_t >(strtol(sub.data(), nullptr, 16))};
        ret.push_back(byt);
    }

    return ret;
}

/// @brief Case-insensitive string comparison
/// @param first
/// @param second
/// @returns bool
/// @throws no no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00486
/// @trace_id_dd=DD_UCM_Master_00862
/// @needwork = dd
/// @endcode
inline bool CompareStrings(core::String const& first, core::String const& second) noexcept
{
    if (first.length() != second.length()) {
        return false;
    }

    return 0 == strncasecmp(first.c_str(), second.c_str(), first.length());
}

/// @brief CheckResultIsError
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00486
/// @trace_id_dd=DD_UCM_Master_00863
/// @needwork = dd
/// @endcode
inline bool CheckResultIsError(const ara::core::Result< void >& res, const std::list< ara::core::ErrorCode >& errorList)
{
    for (ara::core::ErrorCode const& error : errorList) {
        if (res.CheckError(error)) {
            return true;
        }
    }
    return false;
}

}  // namespace helper
}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_HELPER_H_
