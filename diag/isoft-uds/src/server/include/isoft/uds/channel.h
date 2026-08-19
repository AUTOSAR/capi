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
/// @file       channel.h
/// @brief      This file provides channel functionality
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
/// @trace_id_sr=e7d580ca-8f74-42a1-b4d3-1d92845f0f32
/// @unit_name=channel
/// @unit_description=This file provides channel functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_CHANNEL_H_
#define ISOFT_UDS_SERVER_CHANNEL_H_

#include <cstdint>
#include <memory>
#include <vector>

#include "message.h"
#include "result.h"

namespace isoft {
namespace uds {
namespace server {
/// @brief Channel interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01293
/// @trace_id_dd=DD_UDS_02585
/// @needwork = ad
/// @endcode
class Channel
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00871
    /// @needwork = dda
    /// @endcode
    Channel(Channel const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00872
    /// @needwork = dda
    /// @endcode
    Channel(Channel&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00873
    /// @needwork = dda
    /// @endcode
    Channel& operator=(Channel const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00874
    /// @needwork = dda
    /// @endcode
    Channel& operator=(Channel&&) = delete;
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00900
    /// @trace_id_dd=DD_UDS_02014
    /// @needwork = ad
    /// @endcode
    Channel() = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00901
    /// @trace_id_dd=DD_UDS_02015
    /// @needwork = ad
    /// @endcode
    virtual ~Channel() = default;

    /// @brief Response function
    /// @param[in] response Response message
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00902
    /// @trace_id_dd=DD_UDS_02016
    /// @needwork = ad
    /// @endcode
    virtual Result< bool > Respond(std::shared_ptr< Message >& response) = 0;
    /// @brief Response function
    /// @param[in] responses Response message list
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00903
    /// @trace_id_dd=DD_UDS_02017
    /// @needwork = ad
    /// @endcode
    virtual Result< void > Respond(std::vector< std::shared_ptr< Message > >& responses) = 0;

    /// @brief Re-establish channel
    /// @param[in] ta Peer diagnostic logical address
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00904
    /// @trace_id_dd=DD_UDS_02018
    /// @needwork = ad
    /// @endcode
    virtual bool ReestablishAfterRestarted(uint16_t const ta) = 0;

    /// @brief Get Identifier
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00905
    /// @trace_id_dd=DD_UDS_02019
    /// @needwork = ad
    /// @endcode
    virtual ChannelIdentifier GetIdentifier() = 0;
    /// @brief Get MaxPayloadLength
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00906
    /// @trace_id_dd=DD_UDS_02020
    /// @needwork = ad
    /// @endcode
    virtual size_t GetMaxPayloadLength() = 0;

    /// @brief Get LocalIp
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00907
    /// @trace_id_dd=DD_UDS_02021
    /// @needwork = ad
    /// @endcode
    virtual std::string GetLocalIp() = 0;
    /// @brief Get LocalPort
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00908
    /// @trace_id_dd=DD_UDS_02022
    /// @needwork = ad
    /// @endcode
    virtual std::uint16_t GetLocalPort() = 0;
    /// @brief Get RemoteIp
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00909
    /// @trace_id_dd=DD_UDS_02023
    /// @needwork = ad
    /// @endcode
    virtual std::string GetRemoteIp() = 0;
    /// @brief Get RemotePort
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00910
    /// @trace_id_dd=DD_UDS_02024
    /// @needwork = ad
    /// @endcode
    virtual std::uint16_t GetRemotePort() = 0;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif
