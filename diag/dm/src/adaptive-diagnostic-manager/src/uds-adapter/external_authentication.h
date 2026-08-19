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
/// @file       external_authentication.h
/// @brief
/// @details
/// @date       2025-09-22
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "gen_code/external_authentication/serviceAgent/external_authentication_agent.h"
#include "isoft/uds/authentication_management/client_authentication_controller.h"
namespace ara {
namespace diag {
namespace dmd {
class ExternalAuthentication
{
public:
    using Callback
        = std::function< std::shared_ptr< isoft::uds::server::ClientAuthenticationController >(std::uint16_t) >;
    using GetAllClientAddressCallback = std::function< std::vector< std::uint16_t >() >;
    ExternalAuthentication();
    ~ExternalAuthentication() = default;

    /// @brief copy constructor
    /// @param other
    ExternalAuthentication(ExternalAuthentication const &other) noexcept = delete;
    /// @brief copy assignment operator
    /// @param other
    /// @return EcuResetRequest&
    ExternalAuthentication &operator=(ExternalAuthentication const &other) noexcept = delete;
    /// @brief move constructor
    /// @param other
    ExternalAuthentication(ExternalAuthentication &&other) noexcept = default;  /// NOLINT
    /// @brief move assignment operator
    /// @param other
    /// @return EcuResetRequest&
    ExternalAuthentication &operator=(ExternalAuthentication &&other) noexcept = default;  /// NOLINT

    std::uint32_t Initialize(uint32_t const &serviceInstanceId) noexcept;

    void RegisterGetClientAuthenticationController(Callback callback) noexcept;

    void RegisterGetAllClientAddress(GetAllClientAddressCallback callback) noexcept;

private:
    bool _hasClientAddress(std::uint16_t address) noexcept;

    void _setAuthState(std::uint16_t address,
                       bool isAuth,
                       std::vector< std::string > roleList,
                       std::uint64_t msecond) noexcept;

    void _addDynamicAccessList(std::uint16_t address, std::vector< std::uint8_t > dynamicAccessList) noexcept;

    void _setDynamicAccessList(std::uint16_t address, std::vector< std::uint8_t > dynamicAccessList) noexcept;

    void _revoke(std::uint16_t address) noexcept;

    void _refresh(std::uint16_t address) noexcept;

    std::vector< std::uint16_t > _getAllAddress() noexcept;

    Callback callbck_{};
    GetAllClientAddressCallback getAllClientAddress_{};
    std::shared_ptr< isoft::dm::dis::ExternalAuthenticationAgent > agentPtr_{};
};

}  // namespace dmd
}  // namespace diag
}  // namespace ara
