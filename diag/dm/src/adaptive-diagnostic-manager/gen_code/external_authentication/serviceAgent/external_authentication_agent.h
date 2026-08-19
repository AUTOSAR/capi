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
/// @file       external_authentication_agent.h
/// @brief      This file provides the definition of the ExternalAuthenticationAgent interface class.
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef __ExternalAuthenticationSERVICE_H_
#define __ExternalAuthenticationSERVICE_H_

#include <functional>
#include <future>
#include <memory>

#include "ara/core/future.h"
#include "external_authentication_proxy.h"

namespace isoft {
namespace dm {
namespace dis {

class ExternalAuthenticationAgent
{
public:
    explicit ExternalAuthenticationAgent(uint32_t const& serviceInstanceId);

    void RegisterHasClientAddressCallback(HasClientAddressCallback callback) noexcept;
    void RegisterSetAuthStateCallback(SetAuthStateCallback callback) noexcept;
    void RegisterAddDynamicAccessListCallback(AddDynamicAccessListCallback callback) noexcept;
    void RegisterSetDynamicAccessListCallback(SetDynamicAccessListCallback callback) noexcept;
    void RegisterRevokeCallback(RevokeCallback callback) noexcept;
    void RegisterRefreshCallback(RefreshCallback callback) noexcept;
    void RegisterGetAllAddressCallback(GetAllAddressCallback callback) noexcept;
    void UpdateAuthState(std::uint16_t address, bool isAuth) noexcept;

private:
    std::unique_ptr< ExternalAuthenticationProxy > proxy_{nullptr};
};

}  // namespace dis
}  // namespace dm
}  // namespace isoft

#endif  // __ExternalAuthenticationSERVICE_H_