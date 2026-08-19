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
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_EXTERNAL_AUTHENTICATION_H_
#define ARA_DIAG_EXTERNAL_AUTHENTICATION_H_

#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"
#include "ara/core/vector.h"
#include "ara/diag/client_authentication.h"
#include "ara/diag/meta_info.h"
#include "ara/diag/reentrancy.h"

namespace isoft {
namespace dm {
namespace dic {
class ExternalAuthenticationAgent;
}  // namespace dic
}  // namespace dm
}  // namespace isoft
namespace ara {
namespace diag {

/**
 * @brief Definition of the ExternalAuthentication class, which is used by the
 * application to receive an instance of the ClientAuthentication Class relevant
 * to the specific client
 *
 * @artraceid{SWS_DM_01191}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
class ExternalAuthentication final
{
public:
    /**
   * @brief Alias for tester address
   *
   * @artraceid{SWS_DM_01192}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    using Address = std::uint16_t;

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
    explicit ExternalAuthentication(ara::core::InstanceSpecifier instanceSpecifier,
                                    ReentrancyType reentrancyType) noexcept;

    /**
   * @brief Move constructor of ExternalAuthentication
   *
   * @param[in] other Object to move-construct from
   *
   * @artraceid{SWS_DM_01194}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ExternalAuthentication(ExternalAuthentication&& other) noexcept = default;

    /**
   * @brief Move assignment operator of ExternalAuthentication
   *
   * @param[in] other Object to move-assign from.
   *
   * @artraceid{SWS_DM_01195}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    auto operator=(ExternalAuthentication&& other) & noexcept -> ExternalAuthentication& = default;

    /**
   * @brief Copy constructor of ExternalAuthentication
   *
   * @param[in] other Object to copy-construct from
   *
   * @artraceid{SWS_DM_01196}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ExternalAuthentication(ExternalAuthentication const& other) = delete;

    /**
   * @brief Copy assignment operator of ExternalAuthentication
   *
   * @param[in] other Object to copy-assign from.
   *
   * @artraceid{SWS_DM_01197}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    auto operator=(ExternalAuthentication const& other) -> ExternalAuthentication& = delete;

    /**
   * @brief Destructor of DiagnosticServiceDynamicAccessList
   *
   * @artraceid{SWS_DM_01198}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ~ExternalAuthentication() noexcept;

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
    ara::core::Result< ClientAuthentication > Get(const MetaInfo& metaInfo) noexcept;

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
    ara::core::Result< ClientAuthentication > Get(Address sourceAddress) noexcept;

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
    ara::core::Vector< ClientAuthentication > GetAll() noexcept;

private:
    std::shared_ptr< isoft::dm::dic::ExternalAuthenticationAgent > agentPtr_{};
};

}  // namespace diag
}  // namespace ara

#endif
