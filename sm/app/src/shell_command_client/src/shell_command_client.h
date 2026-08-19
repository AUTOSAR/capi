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
/// @file       shell_command_client.h
/// @brief
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================

#ifndef SHELL_COMMAND_CLIENT_H_
#define SHELL_COMMAND_CLIENT_H_

#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <ara/sm/shellrequest_proxy.h>
#include <bits/stdint-intn.h>

#include <memory>
#include <mutex>
#include <string>

#define SHELL_GENERAL_ERR (-1)
#define SHELL_TIMEOUT_ERR (-2)
#define SHELL_PROXY_ERR   (-3)

#define FIVE_THOUSAND_L 5000
#define FIVE_HUNDRED_UL 500UL
#define TWENTY_FIVE_UL  25UL
#define TEN_UL          10UL

class ShellCommandClient
{
private:
    /* data */
public:
    ShellCommandClient() = default;
    ~ShellCommandClient();
    ShellCommandClient(ShellCommandClient& other) noexcept  = default;             // NOLINT
    ShellCommandClient(ShellCommandClient&& other) noexcept = default;             // NOLINT
    ShellCommandClient& operator=(ShellCommandClient& other) noexcept = default;   // NOLINT
    ShellCommandClient& operator=(ShellCommandClient&& other) noexcept = default;  // NOLINT

    int32_t SetFgState(const std::string& fgName, const std::string& fgState);
    int32_t GetFgState(const std::string& fgName);
    int32_t SetSmState(const std::string& smName, const uint32_t& requestNo);
    int32_t GetSmState(const std::string& smName);
    int32_t GetAllInfos();
    int32_t SubscribeSMStateInfo();

private:
    void _stateMachienStateChangeHandler() noexcept;
    ara::core::String _concatenateStrings(ara::core::Vector< ara::core::String > const& strVec) const;
    int32_t _init();
    void _destroy() noexcept;
    int32_t _findService();
    std::unique_ptr< ara::sm::proxy::ShellRequestProxy > shellRequestProxy_{nullptr};
    bool initialized_{false};
    std::mutex serviceMutex_{};
    const uint32_t kSingleTimeoutMs{FIVE_HUNDRED_UL};
    const uint32_t kMaxTimeoutCnt{TWENTY_FIVE_UL};
    const ara::core::String kSeperator{","};
};

#endif  // SHELL_COMMAND_CLIENT_H_
