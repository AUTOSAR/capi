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
/// @file       diagnostic_server.h
/// @brief      This file provides the Diagnostic Management class
/// @details
/// @date       2024-12-06
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_DIAG_DM_DIAGNOSTIC_SERVER_H_
#define _ARA_DIAG_DM_DIAGNOSTIC_SERVER_H_

#include <ara/core/string_view.h>
#include <isoft/uds/server.h>

#include <atomic>

#include "uds-adapter/conversation_manager.h"
#include "uds-adapter/external_authentication.h"
#include "uds-adapter/persistence.h"
#include "uds-adapter/persistence_file.h"
#include "uds-adapter/uds_config.h"

namespace ara {
namespace diag {
namespace dmd {

/// @brief DiagnosticServer
class DiagnosticServer
{
public:
    /// @brief Copy constructor is prohibited
    /// @param[in]  other
    DiagnosticServer(DiagnosticServer const& other) = delete;

    /// @brief Copy assignment operator is prohibited
    /// @param[in]  right
    /// @return Reference to DiagnosticManager
    DiagnosticServer& operator=(DiagnosticServer const& right) = delete;

    /// @brief Move constructor is prohibited
    /// @param[in]  right
    DiagnosticServer(DiagnosticServer&& right) = delete;

    /// @brief Move assignment operator is prohibited
    /// @param[in]  right
    /// @return Reference to DiagnosticManager
    DiagnosticServer& operator=(DiagnosticServer&& right) = delete;

    /// @brief Constructor
    DiagnosticServer() noexcept;

    /// @brief Destructor
    ~DiagnosticServer() = default;
#ifdef NMODEL
    /// @brief DM initialization
    /// @return Initialization result code
    std::int32_t InitializeV3(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentList,
        std::shared_ptr< isoft::nml::definition::UdsServer > udsServerPtr,
        std::shared_ptr< isoft::uds::server::Transport > const& transportPtr,
        std::shared_ptr< PersistenceFile > const& persistenceFilePtr) noexcept;
#endif
    /// @brief DM initialization
    /// @return Initialization result code
    std::int32_t Initialize(ara::core::StringView const& path,
                            std::shared_ptr< isoft::uds::server::Transport > const& transportPtr,
                            std::shared_ptr< PersistenceFile > const& persistenceFilePtr) noexcept;

    std::shared_ptr< isoft::uds::server::Server > GetUdsServer() noexcept { return serverPtr_; }

private:
#ifdef NMODEL
    std::int32_t _initializeUdsServerV3(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentList,
        std::shared_ptr< isoft::nml::definition::UdsServer > udsServerPtr,
        std::shared_ptr< isoft::uds::server::Transport > transportPtr,
        std::shared_ptr< PersistenceFile > const& persistenceFilePtr) noexcept;
#endif
    std::int32_t _initializeUdsServer(std::unique_ptr< isoft::manifestreader::Manifest >& dmc,
                                      std::shared_ptr< isoft::uds::server::Transport > transportPtr,
                                      std::shared_ptr< PersistenceFile > const& persistenceFilePtr) noexcept;

    // std::int32_t _initializeUdsServer(std::shared_ptr< isoft::nml::definition::Document > documentPtr,
    //                                   std::shared_ptr< isoft::nml::definition::UdsServer > udsServerPtr,
    //                                   std::shared_ptr< isoft::uds::server::Transport > transportPtr,
    //                                   std::shared_ptr< PersistenceFile > const& persistenceFilePtr) noexcept;

private:
    std::shared_ptr< isoft::uds::server::Server > serverPtr_;
    std::shared_ptr< UdsConfig > udsConfigPtr_;
    //std::shared_ptr< Persistence > persistencePtr_;

    bool v3Flag_{false};

    std::shared_ptr< ConversationManager > conversationManagerPtr_;

    std::shared_ptr< ExternalAuthentication > externalAuthenticationPtr_;
};

}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  // _ARA_DIAG_DM_DIAGNOSTIC_MANAGER_H_