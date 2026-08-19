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
/// @file       message.h
/// @brief      This file provides message functionality
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
/// @trace_id_sr=ff80e971-507e-4752-89b5-2fe03e9f256d
/// @unit_name=message
/// @unit_description=This file provides message functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_MESSAGE_H_
#define ISOFT_UDS_SERVER_MESSAGE_H_

#include <cstdint>
#include <vector>

#include "common.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Destination address type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01314
/// @trace_id_dd=DD_UDS_02606
/// @needwork = ad
/// @endcode
enum class TargetAddressType : std::uint8_t
{
    kPhysical   = 0,
    kFunctional = 1
};

/// @brief Message
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01315
/// @trace_id_dd=DD_UDS_02607
/// @needwork = ad
/// @endcode
class Message
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00934
    /// @needwork = dda
    /// @endcode
    Message(Message const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00935
    /// @needwork = dda
    /// @endcode
    Message(Message&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00936
    /// @needwork = dda
    /// @endcode
    Message& operator=(Message const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00937
    /// @needwork = dda
    /// @endcode
    Message& operator=(Message&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00987
    /// @trace_id_dd=DD_UDS_02101
    /// @needwork = ad
    /// @endcode
    Message(Address sa, Address ta);
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00988
    /// @trace_id_dd=DD_UDS_02102
    /// @needwork = ad
    /// @endcode
    ~Message() noexcept = default;

    /// @brief Get SA
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00989
    /// @trace_id_dd=DD_UDS_02103
    /// @needwork = ad
    /// @endcode
    Address GetSA() const noexcept;

    /// @brief Get TA
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00990
    /// @trace_id_dd=DD_UDS_02104
    /// @needwork = ad
    /// @endcode
    Address GetTA() const noexcept;

    /// @brief Set TaType
    /// @param TaType Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00991
    /// @trace_id_dd=DD_UDS_02105
    /// @needwork = ad
    /// @endcode
    void SetTaType(TargetAddressType const taType) noexcept;

    /// @brief Get TaType
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00992
    /// @trace_id_dd=DD_UDS_02106
    /// @needwork = ad
    /// @endcode
    TargetAddressType GetTaType() noexcept;

    /// @brief Get message body
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00993
    /// @trace_id_dd=DD_UDS_02107
    /// @needwork = ad
    /// @endcode
    std::vector< std::uint8_t >& GetBody() noexcept;

    /// @brief Convert to byte array
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00994
    /// @trace_id_dd=DD_UDS_02108
    /// @needwork = ad
    /// @endcode
    std::vector< std::uint8_t > Dump() noexcept;

private:
    /// @brief Source address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00995
    /// @trace_id_dd=DD_UDS_02109
    /// @needwork = ad
    /// @endcode
    Address sa_;
    /// @brief Destination address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00996
    /// @trace_id_dd=DD_UDS_02110
    /// @needwork = ad
    /// @endcode
    Address ta_;
    /// @brief Destination address type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00997
    /// @trace_id_dd=DD_UDS_02111
    /// @needwork = ad
    /// @endcode
    TargetAddressType taType_;
    /// @brief Message body
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00998
    /// @trace_id_dd=DD_UDS_02112
    /// @needwork = ad
    /// @endcode
    std::vector< std::uint8_t > body_;
};
/// @brief Message pointer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01316
/// @trace_id_dd=DD_UDS_02608
/// @needwork = ad
/// @endcode
using MessagePtr = std::shared_ptr< Message >;
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif
