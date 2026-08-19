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
/// @file       server_manager.h
/// @brief      This file provides server_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds
/// @interface_level=component
/// @trace_id_sr=e175c450-1201-44a2-b2d7-6fcf23cdd168
/// @unit_name=ServerManager
/// @unit_description=This file provides server_manager functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVERMANAGER_H_
#define ISOFT_UDS_SERVER_SERVERMANAGER_H_

#include <map>
#include <mutex>

#include "server.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Server manager
/// @code{.isoft}
/// @interface_level=module
/// @needwork=ad
/// @endcode
class ServerManager
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00867
    /// @needwork = dda
    /// @endcode
    ServerManager(ServerManager const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00868
    /// @needwork = dda
    /// @endcode
    ServerManager& operator=(ServerManager const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00869
    /// @needwork = dda
    /// @endcode
    ServerManager(ServerManager&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00870
    /// @needwork = dda
    /// @endcode
    ServerManager& operator=(ServerManager&&) = delete;

    /// @brief Constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=module
    /// @needwork=ad
    /// @endcode
    ServerManager() noexcept = default;

    /// @brief Destructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=module
    /// @needwork=ad
    /// @endcode
    virtual ~ServerManager() noexcept = default;

    /// @brief Register function
    /// @return Return code
    /// @param[in] serverPtr Server pointer
    /// @code{.isoft}
    /// @interface_level=module
    /// @needwork=ad
    /// @endcode
    std::int32_t Register(std::shared_ptr< Server > const& serverPtr) noexcept;

    /// @brief Get Server object by diagnostic logical physical address
    /// @param[in] address Diagnostic logical physical address
    /// @return Server pointer
    /// @code{.isoft}
    /// @interface_level=module
    /// @needwork=ad
    /// @endcode
    std::shared_ptr< Server > GetServerByPa(Address const address) noexcept;

    /// @brief Get Server object by diagnostic logical functional address
    /// @param[in] address Diagnostic logical functional address
    /// @return Server list pointer
    /// @code{.isoft}
    /// @interface_level=module
    /// @needwork=ad
    /// @endcode
    std::list< std::shared_ptr< Server > > GetServerByFa(Address const address) noexcept;

private:
    /// @brief Server manager mutex lock
    /// @code{.isoft}
    /// @interface_level=module
    /// @needwork=ad
    /// @endcode
    std::mutex serverLock_;

    /// @brief Server manager physical address mapping table
    /// @brief Server manager physical address to server object mapping
    /// @code{.isoft}
    /// @interface_level=module
    /// @needwork=ad
    /// @interface_level=module
    /// @needwork=ad
    /// @endcode
    std::map< Address, std::shared_ptr< Server > > serverByPa_;  // physical addressing

    /// @brief Server manager functional address to server object list mapping
    /// @code{.isoft}
    /// @interface_level=module
    /// @needwork=ad
    /// @endcode
    std::map< Address, std::list< std::shared_ptr< Server > > > serverByFa_;  // functional addressing
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVERMANAGER_H_