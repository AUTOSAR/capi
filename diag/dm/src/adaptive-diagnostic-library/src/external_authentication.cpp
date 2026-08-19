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
/// @file       external_authentication.cpp
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include <ara/diag/external_authentication.h>

#include "client_authentication_impl.h"
#include "gen_code/external_authentication/apiAgent/external_authentication_agent.h"
#include "resolve.h"
#include "utility.h"
namespace ara {
namespace diag {
/**
 * @brief Constructs the port for authentication of diagnostic clients
 *
 * @param[in] instanceSpecifier InstanceSpecifier to a PortPrototype of a
 * DiagnosticAuthentication service instance in the manifest
 * @param[in] reentrancyType specifies if interface is callable fully- or
 * non-reentrant
 *
 * @artraceid{SWS_DM_01193}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ExternalAuthentication::ExternalAuthentication(ara::core::InstanceSpecifier instanceSpecifier,  /// NOLINT
                                               ReentrancyType reentrancyType) noexcept
{
    ara::core::Result< internal::InstanceInfo > const retrieveResult{internal::Resolve(instanceSpecifier)};
    if (retrieveResult.HasValue()) {
        agentPtr_ = std::make_shared< isoft::dm::dic::ExternalAuthenticationAgent >(
            retrieveResult.Value().instanceId, retrieveResult.Value().serviceInstanceId);
        agentPtr_->SetReentrancyType(reentrancyType);
    } else {
        internal::LogError() << "ExternalAuthentication::ExternalAuthentication|"
                                "not found specifier ="
                             << std::move(instanceSpecifier.ToString());
    }
}

/**
 * @brief Destructor of DiagnosticServiceDynamicAccessList
 *
 * @artraceid{SWS_DM_01198}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ExternalAuthentication::~ExternalAuthentication() noexcept {}  /// NOLINT

/**
 * @brief This function is used by the application to get the
 * ClientAuthentication Instance that is handling the Authentication State of
 * the Client corresponding to the MetaInfo
 *
 * @returns The diagnostic client associated authentication object or error
 *
 * @param[in] metaInfo The meta information of a diagnostic service port
 * @arerror ara::diag::DiagErrc::kServiceNotAvailable
 *
 * @artraceid{SWS_DM_01199}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ara::core::Result< ClientAuthentication > ExternalAuthentication::Get(const MetaInfo& metaInfo) noexcept
{
    if (agentPtr_.get() != nullptr) {
        ara::core::Optional< ara::core::StringView > sa{metaInfo.GetValue(std::move(ara::core::StringView("kSA")))};
        if (!sa.has_value()) {
            internal::LogWarn() << "ExternalAuthentication::Get|not found kSA in metaInfo";
            return ara::core::Result< ClientAuthentication >::FromError(ara::diag::DiagErrc::kServiceNotAvailable);
        }
        unsigned ulAddress{};
        std::string strSa{sa->data()};
        ulAddress = std::stoul(strSa, nullptr, INT16_WIDTH);
        Address address{static_cast< Address >(ulAddress)};
        bool hasResult = agentPtr_->HasClientAddress(address);
        if (hasResult) {
            std::shared_ptr< isoft::dm::ClientAuthenticationImpl > clientImplPtr{
                std::make_shared< isoft::dm::ClientAuthenticationImpl >(address, agentPtr_)};
            ClientAuthentication clientAuthentication{clientImplPtr};
            return ara::core::Result< ClientAuthentication >::FromValue(std::move(clientAuthentication));
        }
    }
    return ara::core::Result< ClientAuthentication >::FromError(ara::diag::DiagErrc::kServiceNotAvailable);
}

/**
 * @brief This function is used by the application to get the
 * ClientAuthentication Instance that is handling the Authentication State of
 * the Client corresponding to the Address
 *
 * @returns The associated diagnostic client authentication object or error
 *
 * @param[in] sourceAddress The source address of the client
 * @arerror ara::diag::DiagErrc::kServiceNotAvailable
 *
 * @artraceid{SWS_DM_01200}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ara::core::Result< ClientAuthentication > ExternalAuthentication::Get(Address sourceAddress) noexcept
{
    if (agentPtr_.get() != nullptr) {
        Address address{sourceAddress};
        bool hasResult = agentPtr_->HasClientAddress(address);
        if (hasResult) {
            std::shared_ptr< isoft::dm::ClientAuthenticationImpl > clientImplPtr{
                std::make_shared< isoft::dm::ClientAuthenticationImpl >(address, agentPtr_)};
            ClientAuthentication clientAuthentication{clientImplPtr};
            return ara::core::Result< ClientAuthentication >::FromValue(std::move(clientAuthentication));
        }
    }
    return ara::core::Result< ClientAuthentication >::FromError(ara::diag::DiagErrc::kServiceNotAvailable);
}

/**
 * @brief This function is used by the application to get all the
 * ClientAuthentication Instances that are currently handled by the DM
 *
 * @returns The list of all diagnostic client associated authentication
 * objects or empty list if none available.
 * @arerror ara::diag::DiagErrc::kServiceNotAvailable
 *
 * @artraceid{SWS_DM_01201}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ara::core::Vector< ClientAuthentication > ExternalAuthentication::GetAll() noexcept
{
    if (agentPtr_.get() != nullptr) {
        ara::core::Vector< ClientAuthentication > table;
        std::vector< std::uint16_t > addressTable = agentPtr_->GetAllAddress();
        for (auto&& address : addressTable) {
            std::shared_ptr< isoft::dm::ClientAuthenticationImpl > clientImplPtr{
                std::make_shared< isoft::dm::ClientAuthenticationImpl >(address, agentPtr_)};
            ClientAuthentication clientAuthentication{clientImplPtr};
            table.emplace_back(std::move(clientAuthentication));
        }
        return table;
    }
    return {};
}
}  // namespace diag
}  // namespace ara
