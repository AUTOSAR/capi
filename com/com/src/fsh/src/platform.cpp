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
/// @file       platform.cpp
/// @brief
/// @details
/// @date       2024-09-11
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#include "isoft/ara_fsh/platform.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "md5.h"

namespace isoft {
namespace ara_fsh {
Platform::Platform() noexcept
{
    constexpr auto kEnvKeySysroot{"ISOFT_ARA_FSH_SYSROOT"};
    auto* sysroot{getenv(kEnvKeySysroot)};  // NOLINT -- compatibility>[concurrency-mt-unsafe]
    if (sysroot == nullptr) {
        sysroot_ = "./";
    } else {
        sysroot_ = sysroot;
        if (sysroot_.back() != '/') {
            sysroot_ += '/';
        }
    }
    constexpr auto kEnvKeyRuntime{"ISOFT_ARA_RUNTIME_DIR"};
    auto* runtime{getenv(kEnvKeyRuntime)};  // NOLINT -- compatibility>[concurrency-mt-unsafe]
    if (runtime == nullptr) {
        runtime_ = "";
    } else {
        runtime_ = runtime;
        if (runtime_.back() != '/') {
            runtime_ += '/';
        }
    }
}

std::string Platform::GetSysroot() const noexcept { return sysroot_; }

std::string Platform::GetUnixDomainSocketDir() const noexcept
{
    char path[PATH_MAX]{};
#if 1  // j3/BTag35 not support realpath api
    strncpy(path, sysroot_.c_str(), PATH_MAX - 1);
#else
    if (realpath(sysroot_.c_str(), path) == nullptr) {
        fprintf(stderr, "realpath failed, %s\n", sysroot_.c_str());
        return {};
    }
#endif
    uint8_t md5num[MD5_NUM_LEN]{};
    char md5str[MD5_STR_LEN + 1]{};
    Str2MD5Num(path, md5num);
    MD5Num2Str(md5num, md5str);
    return (runtime_.empty() ? std::string("/run/ara/") : runtime_) + std::string(md5str) + "/ipc/";
}
}  // namespace ara_fsh
}  // namespace isoft