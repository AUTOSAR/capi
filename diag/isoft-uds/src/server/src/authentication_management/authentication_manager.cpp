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
/// @file       authentication_manager.cpp
/// @brief      This file implements authentication_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=authentication_management
/// @trace_id_sr=8d0228f3-6205-47db-a0a5-bd8ad38a80e8
/// @unit_name=AuthenticationManager
/// @unit_description=This file provides authentication_manager functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "authentication_manager.h"

namespace isoft {
namespace uds {
namespace server {

std::int32_t AuthenticationManager::Initialize(
    std::shared_ptr< SessionManager > sessionManagerPtr,
    std::vector< AuthRole > const &authRoleList,
    std::uint32_t authenticationTimeout,
    std::vector< ExternalAuthenticationIdentification > externalAuthenticationList) noexcept

{
    sessionManagerPtr_          = std::move(sessionManagerPtr);
    authRoleList_               = authRoleList;
    authenticationTimeout_      = authenticationTimeout;
    externalAuthenticationList_ = std::move(externalAuthenticationList);
    return 0;
}

Result< ClientAuthenticationPtr > AuthenticationManager::GetClientAuthentication(Address const &clientAddress) noexcept
{
    bool result{false};
    for (auto &&externalAuthentication : externalAuthenticationList_) {
        std::uint16_t code = externalAuthentication.sourceAddressMask & clientAddress;
        if (externalAuthentication.sourceAddressCode == code) {
            result = true;
            break;
        }
    }
    if (result) {
        ClientAuthenticationPtr clientAuthPtr{nullptr};
        auto findResult = clientAuthenticationTable_.find(clientAddress);
        if (findResult == clientAuthenticationTable_.end()) {
            clientAuthPtr = std::make_shared< ClientAuthentication >(clientAddress);
            AuthRoleTable table{};
            for (auto &&authRole : authRoleList_) {
                table.insert({authRole.shortName, authRole});
            }

            clientAuthPtr->Initialize(sessionManagerPtr_, table, std::chrono::milliseconds(authenticationTimeout_));
            clientAuthPtr->Default();
            clientAuthenticationTable_.insert({clientAddress, clientAuthPtr});

        } else {
            clientAuthPtr = findResult->second;
        }
        return Result< ClientAuthenticationPtr >::FromValue(clientAuthPtr);
    }
    return Result< ClientAuthenticationPtr >::FromError(__LINE__);
}

Result< std::vector< Address > > AuthenticationManager::GetAllClientAuthenticationAddress() noexcept
{
    std::vector< Address > table{};
    for (auto &&clientAuthenticationPair : clientAuthenticationTable_) {
        table.emplace_back(clientAuthenticationPair.first);
    }
    if (table.empty()) {
        return Result< std::vector< Address > >::FromError(__LINE__);
    }
    return Result< std::vector< Address > >::FromValue(table);
}

void AuthenticationManager::SetVerifyCertificateState(Address clientAddress, bool state) noexcept
{
    verifyCertificateStateTable_[clientAddress] = state;
}

bool AuthenticationManager::GetVerifyCertificateState(Address clientAddress) noexcept
{
    auto findResult = verifyCertificateStateTable_.find(clientAddress);
    if (findResult == verifyCertificateStateTable_.end()) {
        return false;
    }
    return findResult->second;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
