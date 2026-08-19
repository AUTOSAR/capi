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
/// @file       ipc_error_domain.h
/// @brief
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_CORE_IPC_IPC_ERROR_DOMAIN_H_
#define ISOFT_CORE_IPC_IPC_ERROR_DOMAIN_H_

#include <ara/core/error_code.h>
#include <ara/core/error_domain.h>
#include <ara/core/exception.h>

namespace isoft {
namespace core {

enum class IpcErrc : ara::core::ErrorDomain::CodeType
{
    kCanNotFindExecutionManifest          = 1,  ///< Cannot find execution manifest
    kCanNotFindIpcNameInExecutionManifest = 2,  ///< Cannot determine ipc name from execution manifest
    kInvalidIpcDir                        = 3,  ///< Invalid ipc dir
    kCreateIpcDirFailed                   = 4,  ///< Failed to create ipc dir
    kIpcResourceIsbusy                    = 5,  ///< Ipc socket is occupied
    kCreateIpcSocketFailed                = 6,  ///< Failed to create ipc socket
    kSetSocketFdEnvFailed                 = 7   ///< Failed to set ipc socket fd
};

class IpcException : public ara::core::Exception
{
public:
    explicit IpcException(ara::core::ErrorCode errorCode) noexcept : ara::core::Exception{std::move(errorCode)} {}
};

class IpcErrorDomain final : public ara::core::ErrorDomain
{
public:
    using Errc      = IpcErrc;
    using Exception = IpcException;

    IpcErrorDomain() noexcept : ara::core::ErrorDomain{kId} {}

    char const* Name() const noexcept final { return "AraIpc"; }
    char const* Message(CodeType errorCode) const noexcept final
    {
        Errc const code{static_cast< Errc >(errorCode)};
        ara::core::StringView errcStr;
        switch (code) {
            case Errc::kCanNotFindExecutionManifest: {
                errcStr = ara::core::StringView{
                    "can not find execution manifest, please set environment ISOFT_ARA_FSH_PROC_CONFIG_DIR or cp the "
                    "config files to the dir of your executable."};
                break;
            }
            case Errc::kCanNotFindIpcNameInExecutionManifest: {
                errcStr = ara::core::StringView{"can not find ipc name in execution manifest."};
                break;
            }
            case Errc::kInvalidIpcDir: {
                errcStr = ara::core::StringView{"invalid ipc dir."};
                break;
            }
            case Errc::kCreateIpcDirFailed: {
                errcStr = ara::core::StringView{"create ipc dir failed."};
                break;
            }
            case Errc::kIpcResourceIsbusy: {
                errcStr = ara::core::StringView{"ipc socket is busy, please delete it manually."};
                break;
            }
            case Errc::kCreateIpcSocketFailed: {
                errcStr = ara::core::StringView{"create ipc socket failed."};
                break;
            }
            case Errc::kSetSocketFdEnvFailed: {
                errcStr = ara::core::StringView{"set ipc socket fd env failed."};
                break;
            }
            default: {
                errcStr = ara::core::StringView{"Unknown error"};
                break;
            }
        }
        return errcStr.data();
    }

    void ThrowAsException(ara::core::ErrorCode const& errorCode) const final
    {
        ara::core::internal::ThrowOrTerminate< Exception >(std::move(errorCode));
    }

private:
    static ErrorDomain::IdType const kId{0xC000000000000005U};
};

namespace internal {
static IpcErrorDomain const kIpcErrorDomain;  // NOLINT
}  // namespace internal

constexpr ara::core::ErrorDomain const& GetIpcDomain() noexcept { return internal::kIpcErrorDomain; }

constexpr ara::core::ErrorCode MakeErrorCode(IpcErrc const code,
                                             ara::core::ErrorDomain::SupportDataType const data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetIpcDomain(), data);
}

}  // namespace core
}  // namespace isoft

#endif  // ISOFT_CORE_IPC_IPC_ERROR_DOMAIN_H_