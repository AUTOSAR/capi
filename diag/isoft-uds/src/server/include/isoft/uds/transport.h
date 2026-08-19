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
/// @file       transport.h
/// @brief      This file provides transport functionality
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
/// @trace_id_sr=fd2ed47d-0297-457e-b22e-2106aaca3f17
/// @unit_name=transport
/// @unit_description=This file provides transport functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_TRANSPORT_H_
#define ISOFT_UDS_SERVER_TRANSPORT_H_

#include <memory>

#include "channel.h"
#include "server_manager.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Transport layer interface
/// @interface_level=unit
/// @needwork=ad
/// @endcode
/// @trace_id_sws={SWS_DM_00776}@tracestatus{draft}
class Transport
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00880
    /// @needwork = dda
    /// @endcode
    Transport(Transport const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00881
    /// @needwork = dda
    /// @endcode
    Transport(Transport&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00882
    /// @needwork = dda
    /// @endcode
    Transport& operator=(Transport const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00883
    /// @needwork = dda
    /// @endcode
    Transport& operator=(Transport&&) = delete;

    /// @brief Constructor
    /// @param[in] serverManager Server manager
    /// @return Return value
    /// @throws Throws exception on overflow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00776}@tracestatus{draft}
    /// @endcode
    explicit Transport(std::shared_ptr< ServerManager > serverManager) : serverManager_{std::move(serverManager)} {}

    /// @brief Destructor
    /// @return Return value
    /// @throws Throws exception on overflow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00776}@tracestatus{draft}
    /// @endcode
    virtual ~Transport() = default;
    /// @brief Initialization function
    /// @return Return value
    /// @throws Throws exception on overflow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00776}@tracestatus{draft}
    /// @endcode
    virtual bool Initialize() = 0;
    /// @brief Start function
    /// @return Return value
    /// @throws Throws exception on overflow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00776}@tracestatus{draft}
    /// @endcode
    virtual bool Start() = 0;
    /// @brief Stop function
    /// @return Return value
    /// @throws Throws exception on overflow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00776}@trac estatus{draft}
    /// @endcode
    virtual void Stop() = 0;
    /// @brief Get channel function
    /// @param[in] serverAddr Server address
    /// @param[in] clientAddr Client address
    /// @return Channel pointer
    /// @throws Throws exception on overflow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00776}@tracestatus{draft}
    /// @endcode
    virtual std::shared_ptr< Channel > GetChannel(Address serverAddr, Address clientAddr) noexcept = 0;

protected:
    /// @brief Server manager
    /// @return Server manager pointer
    /// @throws Throws exception on overflow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00776}@tracestatus{draft}
    /// @endcode
    std::shared_ptr< ServerManager > serverManager_;  // NOLINT
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif
