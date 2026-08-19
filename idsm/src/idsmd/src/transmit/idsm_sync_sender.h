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
/// @file       idsm_sync_sender.h
/// @brief      Synchronous data sender: data sending will block security event processing
/// @details
/// @date       2023-02-16
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/qualified security event storage
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0011
/// @unit_name=SyncSender
/// @unit_description=Synchronous data sender: data sending will block security event processing
/// @endcode
///
/// ================================================================

#ifndef IDSM_SYNC_SENDER_H_
#define IDSM_SYNC_SENDER_H_
#include "ara/idsm/internal/event.h"
#include "idsm_sender.h"
#include "nai/runtime/nai_types.h"

namespace ara {
namespace idsm {

/// @brief Synchronous transmission class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00266
/// @trace_id_dd=DD_IDSM_00805
/// @needwork = ad
/// @endcode
class SyncSender : public IdsmSender
{
public:
    /// @brief Send data to peer
    /// @param data Data to transmit
    /// @param highPriority Callback function called upon transmission completion
    /// @return Number of bytes sent, returns -1 on error
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00806
    /// @needwork = dda
    /// @endcode
    int64_t Send(Message const& data, bool highPriority = false) override;

public:
    /// @brief Constructor and initialization
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00807
    /// @needwork = dda
    /// @endcode
    SyncSender() : SyncSender{ara::core::String{""}, 0U} {}
    /// @brief Constructor and initialization
    /// @param ip Peer IP address
    /// @param p Port on which the peer application listens
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00808
    /// @needwork = dda
    /// @endcode
    SyncSender(ara::core::String const& ip, uint16_t const p) : IdsmSender{ip, p} {}
    /// @brief Destructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00809
    /// @needwork = dda
    /// @endcode
    ~SyncSender() override = default;

public:
    /// @brief Copy constructor
    /// @param sender Object to be copied
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00810
    /// @needwork = dda
    /// @endcode
    SyncSender(SyncSender const& sender) = delete;
    /// @brief Move constructor
    /// @param sender Object to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00811
    /// @needwork = dda
    /// @endcode
    SyncSender(SyncSender&& sender) = delete;
    /// @brief Copy assignment operator
    /// @param sender Object to be copied for assignment operator
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00812
    /// @needwork = dda
    /// @endcode
    SyncSender& operator=(SyncSender const& sender) = delete;
    /// @brief Move assignment operator
    /// @param sender Object to be moved for assignment operator
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00813
    /// @needwork = dda
    /// @endcode
    SyncSender& operator=(SyncSender&& sender) = delete;

private:
    /// @brief Establish connection to peer
    /// @return 0 on success, -1 on failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00814
    /// @needwork = dda
    /// @endcode
    int32_t _createSocket();
    /// @brief Send data
    /// @param data Data to send
    /// @param size Length of data to send
    /// @return Length of data sent, returns -1 on failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00815
    /// @needwork = dda
    /// @endcode
    int64_t _write(uint8_t const* const data, size_t const size);

private:
    /// @brief Socket file descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00816
    /// @needwork = dda
    /// @endcode
    nai_fd_t fd_{-1};
};

}  // namespace idsm
}  // namespace ara

#endif
