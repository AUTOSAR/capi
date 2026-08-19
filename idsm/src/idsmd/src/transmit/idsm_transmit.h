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
/// @file       idsm_transmit.h
/// @brief      Security event remote transmission implementation class
/// @details
/// @date       2023-02-15
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/qualified security event storage
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0011
/// @unit_name=IdsmWriter
/// @unit_description=Security event remote transmission implementation class
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_TRANSMIT_H_
#define ARA_IDSM_TRANSMIT_H_
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>

#include "ara/idsm/ids_proto_encode.h"
#include "ara/idsm/internal/event.h"
#include "idsm_async_sender.h"
#include "idsm_sender.h"
#include "idsm_sync_sender.h"
#include "idsm_trans_limit.h"

namespace ara {
namespace idsm {
/// @brief Forward declaration
class DiagDtcServer;
/// @brief Security event storage class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00263
/// @trace_id_dd=DD_IDSM_00788
/// @needwork = ad
/// @endcode
class IdsmWriter final
{
public:
    /// @brief
    void StartMonitor();
    /// @brief
    void StopMonitor();
    /// @brief Entry function of the qualified security event storage thread
    /// @param arg Thread entry function parameter
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00264
    /// @trace_id_dd=DD_IDSM_00789
    /// @needwork = ad
    /// @endcode
    static void Run(void* const arg);
    /// @brief Initialize
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00265
    /// @trace_id_dd=DD_IDSM_00790
    /// @needwork = ad
    /// @endcode
    void Init() noexcept;
    /// @brief Parameterless constructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00791
    /// @needwork = dda
    /// @endcode
    IdsmWriter() = default;

private:
    /// @brief Store security event
    /// @param event Security event to store
    /// @return Returns 0 on success
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00792
    /// @needwork = dda
    /// @endcode
    IdsmErrorCode _write(EventPtr event);
    /// @brief Serialize the security event to be stored and check rate limiting and traffic shaping
    /// @param event  Security event to be stored
    /// @param message Byte stream after security event serialization
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00793
    /// @needwork = dda
    /// @endcode
    void _checkAndEncode(EventPtr& event, std::shared_ptr< BytesVec >& message);
    /// @brief Determine whether it is an IDSM internal security event
    /// @param event Security event to check
    /// @return Returns true if it is an internal security event, otherwise returns false
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00794
    /// @needwork = dda
    /// @endcode
    bool _isInternalEvent(EventPtr const& event) noexcept;
    /// @brief Handle communication exception between IDSM and IDSR
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00795
    /// @needwork = dda
    /// @trace_id_sr=SR_IDSM_0014
    /// @endcode
    IdsmErrorCode _handleComErr();
    /// @brief Send security event byte stream to IDSR
    /// @param eventTrans Security event byte stream to send
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00796
    /// @needwork = dda
    /// @endcode
    IdsmErrorCode _remoteStorage(Message const& eventTrans);
    /// @brief Initialize security event sender
    /// @param ipAddr IP address of the IDSR service
    /// @param port Port number of the IDSR service
    /// @param async Whether to send asynchronously
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00797
    /// @needwork = dda
    /// @endcode
    void _initIdsmSender(ara::core::String const& ipAddr, uint16_t port, bool const async);
    /// @brief Security event serialization
    /// @param event Security event to be serialized
    /// @return Security event byte stream
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00798
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< BytesVec > _serialize(std::shared_ptr< Event > const& event)
    {
        std::shared_ptr< BytesVec > res{serializer_->Encode(event)};
        return res;
    }

private:
    /// @name serializer
    /// @brief Security event serialization
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00799
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< IdsmSerialize > serializer_;
    /// @name internalEvents
    /// @brief IDSM internal events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00800
    /// @needwork = dda
    /// @endcode
    IdsmInternalEvent internalEvents_;
    /// @name diagDtcServerMap
    /// @brief Mapping from security event to diagnostic event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00801
    /// @needwork = dda
    /// @endcode
    std::map< uint16_t, std::shared_ptr< DiagDtcServer > > diagDtcServerMap_;
    /// @brief Traffic shaping filter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00802
    /// @needwork = dda
    /// @endcode
    TrafficChecker trafChecker_;
    /// @brief Rate limiting filter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00803
    /// @needwork = dda
    /// @endcode
    RateChecker rateChecker_;
    /// @brief Security event sender
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00804
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< IdsmSender > sender_;
    /// @brief Main thread exit flag
    /// @details The main thread updates its own exit flag to notify the daemon thread
    /// @details The daemon thread obtains the flag by polling, so there is no read-after-write issue
    bool threadExit_{false};
    /// @brief Daemon thread end flag
    /// @details The daemon thread updates its own end flag to notify the main thread
    /// @details The main thread obtains the flag by polling, so there is no read-after-write issue
    bool threadStop_{true};
};

}  // namespace idsm
}  // namespace ara
#endif
