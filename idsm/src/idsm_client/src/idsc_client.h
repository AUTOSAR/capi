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
/// @file       idsc_client.h
/// @brief      IDSM client implementation
/// @details
/// @date       2022-12-27
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/lib client
/// @interface_level=unit
/// @trace_id_sr=SRS_IDSM_0005
/// @unit_name=IdsmClient
/// @unit_description=IDSM client implementation
/// @endcode
///
/// ================================================================

#ifndef _ARA_IDSM_IDS_CLIENT_
#define _ARA_IDSM_IDS_CLIENT_

#include <ara/core/instance_specifier.h>
#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/log/logger.h>
#include <isoft/ipccpp/client.h>
#include <isoft/naicpp/evloop.h>

#include <memory>
#include <mutex>

#include "ara/idsm/common.h"
#include "ara/idsm/internal/message_process.h"

namespace isoft {
namespace ipc {
/// @brief Forward declaration of IPC client
class IPCClient;

}  // namespace ipc
}  // namespace isoft

namespace ara {
namespace idsm {

/// @brief IDSM security event reporting client
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00001
/// @trace_id_dd=DD_IDSM_00001
/// @needwork = ad
/// @endcode
class IdsmClient
{
private:
    /// @brief Type redefinition
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00002
    /// @needwork = dda
    /// @endcode
    using IPCClientPtr = std::shared_ptr< ::isoft::ipc::IPCClient >;

    /// @brief Type redefinition
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00003
    /// @needwork = dda
    /// @endcode
    using TimestampProviderType = std::function< TimestampType() >;

public:
    /// @brief Destructor
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00004
    /// @needwork = dda
    /// @endcode
    virtual ~IdsmClient() = default;
    /// @brief Get the unique instance of IdsmClient
    /// @return IdsmClient instance
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={reentrant}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00005
    /// @needwork = dda
    /// @endcode
    static IdsmClient *GetInstance();
    /// @brief Destroy IdsmClient instance
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00006
    /// @needwork = dda
    /// @endcode
    void Destroy();
    /// @brief Initialize IDSM client
    /// @return 0 on success, -1 on failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00007
    /// @needwork = dda
    /// @endcode
    int32_t Init();
    /// @brief Set whether the client is connected to the server
    /// @param bConnect Connection flag
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00008
    /// @needwork = dda
    /// @endcode
    void SetConnect(bool const bConnect) noexcept;
    /// @brief Set the callback function for getting timestamp
    /// @param cb Timestamp callback function
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00009
    /// @needwork = dda
    /// @endcode
    inline void SetTimeProviderCB(TimestampProviderType const &cb) { timeCB_ = cb; }
    /// @brief Generate security event
    /// @param instanceId Port instance identifier
    /// @param count Security event counter
    /// @return 0 on success, non-zero on failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={reentrant}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00010
    /// @needwork = dda
    /// @endcode
    bool Report(ara::core::InstanceSpecifier const &instanceId, CountType const count);
    /// @brief Generate security event
    /// @param instanceId Port instance identifier
    /// @param contextData Context data
    /// @param count Security event counter
    /// @return 0 on success, non-zero on failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={reentrant}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00011
    /// @needwork = dda
    /// @endcode
    bool Report(ara::core::InstanceSpecifier const &instanceId,
                ContextDataType const &contextData,
                CountType const count);
    /// @brief Generate security event
    /// @param instanceId Port instance identifier
    /// @param timestamp Context data
    /// @param count Security event counter
    /// @return 0 on success, non-zero on failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={reentrant}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00012
    /// @needwork = dda
    /// @endcode
    bool Report(ara::core::InstanceSpecifier const &instanceId, TimestampType const timestamp, CountType const count);
    /// @brief Generate security event
    /// @param instanceId Port instance identifier
    /// @param contextData Context data
    /// @param timestamp Timestamp
    /// @param count Security event counter
    /// @return 0 on success, non-zero on failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={reentrant}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00013
    /// @needwork = dda
    /// @endcode
    bool Report(ara::core::InstanceSpecifier const &instanceId,
                ContextDataType const &contextData,
                TimestampType const timestamp,
                CountType const count);

public:
    /// @brief Parameterless constructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00014
    /// @needwork = dda
    /// @endcode
    IdsmClient() = default;
    /// @brief Copy constructor
    /// @param client Object to be copied
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00015
    /// @needwork = dda
    /// @endcode
    IdsmClient(IdsmClient const &client) = delete;
    /// @brief Move constructor
    /// @param client Object to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00016
    /// @needwork = dda
    /// @endcode
    IdsmClient(IdsmClient &&client) = delete;
    /// @brief Copy assignment operator
    /// @param client Object to be copied
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00017
    /// @needwork = dda
    /// @endcode
    IdsmClient &operator=(IdsmClient const &client) = delete;
    /// @brief Move assignment operator
    /// @param client Object to be moved
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00018
    /// @needwork = dda
    /// @endcode
    IdsmClient &operator=(IdsmClient &&client) = delete;

private:
    /// @brief Transmit data to IDSM
    /// @param evMsg Event sent by AA application
    /// @return 0 on success, non-zero on failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={reentrant}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00019
    /// @needwork = dda
    /// @endcode
    bool _sendEventMsg(Chunk const &evMsg);

private:
    /// @name mtx_
    /// @brief Mutex object to ensure thread safety for the singleton class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00020
    /// @needwork = dda
    /// @endcode
    static std::mutex s_Mtx_;
    /// @name instance_
    /// @brief Unique instance object of the singleton class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00021
    /// @needwork = dda
    /// @endcode
    static std::unique_ptr< IdsmClient > s_Instance_;
    /// @name bConnect_
    /// @brief Whether connected to Idsm daemon
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00022
    /// @needwork = dda
    /// @endcode
    bool bConnect_{false};
    /// @name timeout_
    /// @brief Timeout for connecting to idsm daemon, deprecated
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00023
    /// @needwork = dda
    /// @endcode
    //uint32_t const timeout_{500};
    /// @name processId_
    /// @brief Process ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00024
    /// @needwork = dda
    /// @endcode
    uint32_t processId_{0};
    /// @name timeCB_
    /// @brief Timestamp callback function registered by AA application
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00025
    /// @needwork = dda
    /// @endcode
    TimestampProviderType timeCB_{nullptr};
    /// @name evLoopPtr_
    /// @brief Class object implementing the event loop
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00026
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvLoop > evLoopPtr_;
    /// @name clientPtr_
    /// @brief Use forward declaration to hide eventloop related header files
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00027
    /// @needwork = dda
    /// @endcode
    IPCClientPtr clientPtr_;
};

}  // namespace idsm
}  // namespace ara

#endif  // _ARA_IDSM_IDS_CLIENT_