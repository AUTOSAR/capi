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
/// @file       diagnostic_manager.h
/// @brief      This file provides the Diagnostic Management class
/// @details
/// @date       2022-04-22
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_DIAG_DM_DIAGNOSTIC_MANAGER_H_
#define _ARA_DIAG_DM_DIAGNOSTIC_MANAGER_H_

#include <atomic>
#ifdef NMODEL
    #include <isoft/nml/definition/adaptive_software_component.h>
    #include <isoft/nml/definition/dm_module.h>
#endif
#include "diagnostic_server.h"
#include "doip-adapter/doip_server.h"
#include "signal_manager.h"
namespace ara {
namespace diag {

namespace doip {
class DoIPServer;
}  // namespace doip
namespace dmd {

/// @brief DiagnosticManager
class DiagnosticManager
{
public:
    /// @brief Copy constructor is prohibited
    /// @param[in]  other
    DiagnosticManager(DiagnosticManager const& other) = delete;

    /// @brief Copy assignment operator is prohibited
    /// @param[in]  right
    /// @return Reference to DiagnosticManager
    DiagnosticManager& operator=(DiagnosticManager const& right) = delete;

    /// @brief Move constructor is prohibited
    /// @param[in]  right
    DiagnosticManager(DiagnosticManager&& right) = delete;

    /// @brief Move assignment operator is prohibited
    /// @param[in]  right
    /// @return Reference to DiagnosticManager
    DiagnosticManager& operator=(DiagnosticManager&& right) = delete;

    /// @brief Constructor
    DiagnosticManager() noexcept = default;

    /// @brief Destructor
    ~DiagnosticManager() = default;

    /// @brief DM initialization
    /// @return Initialization result code
    std::int32_t Initialize() noexcept;

    /// @brief DM starts running
    /// @return Running result code
    std::int32_t Run() noexcept;

private:
    /// @brief Signal trigger processing callback
    /// @param[in] number
    void _processSignal() noexcept;

    bool _initNetProxy();

#ifdef NMODEL
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > _findAdaptiveSoftwareComponent(
        std::shared_ptr< isoft::nml::definition::Document > documentPtr) noexcept;

    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > _findAdaptiveSoftwareComponent(
        std::shared_ptr< isoft::nml::definition::Package > packagePtr,
        std::shared_ptr< isoft::nml::definition::DmModule > dmModulePtr) noexcept;

    std::shared_ptr< isoft::nml::definition::DmModule > _findDmModule(
        std::shared_ptr< isoft::nml::definition::Document > documentPtr) noexcept;

    std::shared_ptr< isoft::nml::definition::DmModule > _findDmModule(
        std::shared_ptr< isoft::nml::definition::Package > packagePtr) noexcept;
#endif
private:
    /// @brief signalManager_
    SignalManager signalManager_;

    /// @brief stopping_
    std::atomic_bool stopping_{false};

    std::vector< std::shared_ptr< DiagnosticServer > > serverTable_;

    std::shared_ptr< doip::DoIPServer > doip_;

    std::shared_ptr< PersistenceFile > persistenceFilePtr_;

    std::shared_ptr< isoft::uds::server::ServerManager > udsServerManager_{};
#ifdef NMODEL

    bool useNmodel_{false};
#endif
};

}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  // _ARA_DIAG_DM_DIAGNOSTIC_MANAGER_H_