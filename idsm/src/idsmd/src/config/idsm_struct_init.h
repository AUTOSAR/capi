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
/// @file       idsm_struct_init.h
/// @brief      Configuration item pool. Parse configuration and store for subsequent use
/// @details
/// @date       2024-05-12
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Configuration Center
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0006
/// @unit_name=IdsmStruct
/// @unit_description=Configuration item pool. Parse configuration and store for subsequent use
/// @endcode
///
/// ================================================================

#ifndef IDSM_STRUCT_INIT_H_
#define IDSM_STRUCT_INIT_H_
#include <ara/core/map.h>

#include <memory>
#include <mutex>

#include "ara/idsm/internal/event.h"
#include "ara/idsm/internal/idsm_error_domain.h"
#include "filter/idsm_filter_chain.h"
#include "idsm_manifest.h"
namespace ara {
namespace idsm {
/// @brief Type redefinition: set of security event ids
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00174
/// @trace_id_dd=DD_IDSM_00409
/// @needwork = ad
/// @endcode
using EventVec = ara::core::Vector< uint16_t >;
/// @brief forward declaration
class DiagDidServer;
/// @brief forward declaration
class DiagDtcServer;
/// @brief singleton configuration class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00175
/// @trace_id_dd=DD_IDSM_00410
/// @needwork = ad
/// @endcode
class IdsmStruct : public IdsmManifest
{
private:
    /// @brief default constructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00411
    /// @needwork = dda
    /// @endcode
    IdsmStruct() = default;
    /// @brief copy constructor
    /// @param right object to copy
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00412
    /// @needwork = dda
    /// @endcode
    IdsmStruct(IdsmStruct const& right) = default;
    /// @brief move constructor
    /// @param right object to move
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00413
    /// @needwork = dda
    /// @endcode
    IdsmStruct(IdsmStruct&& right) = default;
    /// @brief copy assignment operator
    /// @param right object to copy
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00414
    /// @needwork = dda
    /// @endcode
    IdsmStruct& operator=(IdsmStruct const& right) = default;
    /// @brief move assignment operator
    /// @param right object to move
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00415
    /// @needwork = dda
    /// @endcode
    IdsmStruct& operator=(IdsmStruct&& right) = default;

public:
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00416
    /// @needwork = dda
    /// @endcode
    ~IdsmStruct() override = default;

public:
    /// @brief Get the unique instance of IdsmStruct
    /// @return IdsmStruct instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00176
    /// @trace_id_dd=DD_IDSM_00417
    /// @needwork = ad
    /// @endcode
    static std::shared_ptr< IdsmStruct > GetInstance() noexcept;
    /// @brief Configuration parsing initialization
    /// @return 0: success, non-zero: failure
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00177
    /// @trace_id_dd=DD_IDSM_00418
    /// @needwork = ad
    /// @endcode
    IdsmErrorCode StructInit() noexcept;
    /// @brief TODO: public for testing, private for non-testing
    /// @param filtPath configuration file path
    /// @return 0: success, non-zero: failure
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00178
    /// @trace_id_dd=DD_IDSM_00419
    /// @needwork = ad
    /// @endcode
    IdsmErrorCode StructParseByPath(ara::core::String const& filtPath);
    /// @brief Configuration parsing deinitialization
    /// @return 0: success, non-zero: failure
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00179
    /// @trace_id_dd=DD_IDSM_00420
    /// @needwork = ad
    /// @endcode
    IdsmErrorCode StructDeInit() noexcept;
    /// @brief Get mapping between event id and filter chain
    /// @param filtersMap mapping between event id and filter chain
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00180
    /// @trace_id_dd=DD_IDSM_00421
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0006
    /// @endcode
    ara::core::Map< uint16_t, FilterChainPtr > GetFilterMap() const;
    /// @brief Get mapping between Port instance descriptor and security event id
    /// @param portMap mapping between Port instance descriptor and security event id
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00181
    /// @trace_id_dd=DD_IDSM_00422
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0001
    /// @endcode
    ara::core::Map< ara::core::String, uint16_t > GetPortMap();
    /// @brief Get mapping between process and security event set
    /// @param processMap mapping between process and security event set
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00182
    /// @trace_id_dd=DD_IDSM_00423
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0001
    /// @endcode
    ara::core::Map< ara::core::String, EventVec > GetProcessMap();
    /// @brief Get internal security events of idsm
    /// @param internalEvent internal security events of idsm
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00183
    /// @trace_id_dd=DD_IDSM_00424
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0002,SR_IDSM_0013,SR_IDSM_0014
    /// @endcode
    IdsmInternalEvent GetInternalEvent() const;
    /// @brief Get mapping between security event id and diagnosis event related data structure
    /// @param dtcServerMap mapping between security event id and diagnosis event related data structure
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00184
    /// @trace_id_dd=DD_IDSM_00425
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0012
    /// @endcode
    ara::core::Map< uint16_t, std::shared_ptr< DiagDtcServer > > GetDiagDtcServer() const;
    /// @brief Get rate limiting related data
    /// @param intervals rate limiting period
    /// @param maxEvents maximum number of security events transmitted within the rate limiting period
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00185
    /// @trace_id_dd=DD_IDSM_00426
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0013
    /// @endcode
    void GetRateLimit(time_t& intervals, uint32_t& maxEvents);
    /// @brief Get flow control related data
    /// @param intervals flow control period
    /// @param maxBytes maximum number of bytes transmitted within the flow control period
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00186
    /// @trace_id_dd=DD_IDSM_00427
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0013
    /// @endcode
    void GetTrafficLimit(time_t& intervals, uint64_t& maxBytes);
    /// @brief Get IP address of remote idsr
    /// @param ipAddr IP address of idsr
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00187
    /// @trace_id_dd=DD_IDSM_00428
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0011
    /// @endcode
    ara::core::String GetIdsrIpAddr();
    /// @brief Get idsm instance id
    /// @return idsm instance id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00188
    /// @trace_id_dd=DD_IDSM_00429
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0011
    /// @endcode
    uint16_t GetIdsmInstanceId() noexcept { return static_cast< uint16_t >(GetIdsmInstance().idsmInstanceId); }
    /// @brief Get listening port of remote idsr
    /// @return listening port of idsr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00189
    /// @trace_id_dd=DD_IDSM_00430
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0011
    /// @endcode
    uint16_t GetIdsrPort() noexcept { return GetIdsmInstance().moduleInstall.net.tcpPort; }
    /// @brief Get memory limit for security event frames, the upper limit of the sum of event frame sizes in processing
    /// @return event stack frame memory limit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00190
    /// @trace_id_dd=DD_IDSM_00431
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0002
    /// @endcode
    uint64_t GetEventMemLimit() noexcept
    {
        return static_cast< uint64_t >(GetIdsmInstance().memLimitation.eventMem) * memLimitUnit_;
    }
    /// @brief Get memory limit for context data, the upper limit of the sum of context data sizes of security events in processing
    /// @return event context memory limit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00191
    /// @trace_id_dd=DD_IDSM_00432
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0002
    /// @endcode
    uint64_t GetContextMemLimit() noexcept
    {
        return static_cast< uint64_t >(GetIdsmInstance().memLimitation.contextMem) * memLimitUnit_;
    }
    /// @brief Get the key slot name used for signing
    /// @param slotName key slot name
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00192
    /// @trace_id_dd=DD_IDSM_00433
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0011
    /// @endcode
    ara::core::String GetSignSlotName() { return GetIdsmInstance().signAp.keySlot; }
    /// @brief Get the signature algorithm name used for signing
    /// @param algName signature algorithm name
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00193
    /// @trace_id_dd=DD_IDSM_00434
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0011
    /// @endcode
    ara::core::String GetSignAlgName() { return GetIdsmInstance().signAp.cryptoPrimitive; }
    /// @brief Get the time base of the time synchronization module
    /// @param timeBase time base
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00194
    /// @trace_id_dd=DD_IDSM_00435
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0003
    /// @endcode
    ara::core::String GetTimeBase() { return GetIdsmInstance().moduleInstall.timeBase; }
    /// @brief Get timestamp format
    /// @param timeStampFormat timestamp format
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00195
    /// @trace_id_dd=DD_IDSM_00436
    /// @needwork = ad
    /// @trace_id_sr=SR_IDSM_0003
    /// @endcode
    ara::core::String GetTimeStampFormat() { return GetIdsmInstance().timestampFormat; }

private:
    /// @brief Data format conversion. Convert string to enumeration value in the specification
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00437
    /// @needwork = dda
    /// @trace_id_sr=SR_IDSM_0009
    /// @endcode
    void _configPreProcess();
    /// @brief Get internal security events of IDSM
    /// @return 0 success, non-zero failure
    /// @exception stack overflow exception, throws exception when memory allocation fails
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00438
    /// @needwork = dda
    /// @trace_id_sr=SR_IDSM_0002,SR_IDSM_0013,SR_IDSM_0014
    /// @endcode
    IdsmErrorCode _internalEventInit();
    /// @brief Get security event Id
    /// @param fqn FQN of the security event
    /// @param eventId Id of the security event
    /// @param isInternalEvent whether fqn is an Idsm internal event
    /// @return 0 success, non-zero failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00439
    /// @needwork = dda
    /// @trace_id_sr=SR_IDSM_0006
    /// @endcode
    ara::core::Result< uint16_t, IdsmErrorCode > _getEventIdByFqn(ara::core::String const& fqn,
                                                                  bool const isInternalEvent = false) const noexcept;
    /// @brief Get security event Id
    /// @param fqn FQN of the security event property
    /// @param eventId Id of the security event
    /// @return 0 success, non-zero failure
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00440
    /// @needwork = dda
    /// @trace_id_sr=SR_IDSM_0004,SR_IDSM_0006,SR_IDSM_0012
    /// @endcode
    ara::core::Result< uint16_t, IdsmErrorCode > _getEventIdByContextFqn(ara::core::String const& fqn) const noexcept;
    /// @brief Get the filter chain for the specified fqn
    /// @param fqn fqn of the chain
    /// @param filters obtained filter chain
    /// @return 0 success, non-zero failure
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00441
    /// @needwork = dda
    /// @trace_id_sr=SR_IDSM_0006
    /// @endcode
    ara::core::Result< FilterChainPtr, IdsmErrorCode > _getFilterChainByFqn(ara::core::String const& fqn);
    /// @brief Establish mapping from prototype to event id
    /// @return 0 success, non-zero failure
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00442
    /// @needwork = dda
    /// @trace_id_sr=SR_IDSM_0001
    /// @endcode
    IdsmErrorCode _initPort();
    /// @brief Initialize Did Server, provide external services
    /// @return 0 success, non-zero failure
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00443
    /// @needwork = dda
    /// @trace_id_sr=SR_IDSM_0004
    /// @endcode
    IdsmErrorCode _initDidServer();
    /// @brief Initialize Dtc Server, provide external services
    /// @return 0 success, non-zero failure
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00444
    /// @needwork = dda
    /// @trace_id_sr=SR_IDSM_0012
    /// @endcode
    IdsmErrorCode _initDtcServer();
    /// @brief Establish mapping from security event to filter chain
    /// @return 0 success, non-zero failure
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00445
    /// @needwork = dda
    /// @trace_id_sr=SR_IDSM_0006
    /// @endcode
    IdsmErrorCode _eventMapFilterChain();

private:
    /// @name singleInstance
    /// @brief Unique instance of IdsmStruct
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00446
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< IdsmStruct > s_SingleInstance_;
    /// @name singleMutex
    /// @brief Mutex used for thread-safe singleton pattern
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00447
    /// @needwork = dda
    /// @endcode
    static std::mutex s_SingleMutex_;
    /// @name memLimitUnit_
    /// @brief Memory unit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00448
    /// @needwork = dda
    /// @endcode
    uint32_t memLimitUnit_{1U};
    /// @name filterMap_
    /// @brief Mapping between event Id and filter chain.
    /// @code{.isoft}
    /// @details 1. Empty key-value pair: event has no filter chain
    /// @details 2. Key exists but value is empty: event has multiple filter chains
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00449
    /// @needwork = dda
    /// @endcode
    ara::core::Map< uint16_t, FilterChainPtr > filterMap_;
    /// @name portMap_
    /// @brief Mapping between port and event type Id. key is port (not port number)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00450
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, uint16_t > portMap_;
    /// @name processMap_
    /// @brief Mapping between port and process. key is process
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00451
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, EventVec > processMap_;
    /// @brief Internal security events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00452
    /// @needwork = dda
    /// @endcode
    IdsmInternalEvent internalSev_;
    /// @brief Data format conversion mapping table. Convert string to enumeration value in the specification
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00453
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, ContextDataSource > configMap_;
    /// @brief Diagnostic data identifier service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00454
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< DiagDidServer > diagDidServer_;
    /// @brief Mapping from security event to diagnosis event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00455
    /// @needwork = dda
    /// @endcode
    ara::core::Map< uint16_t, std::shared_ptr< DiagDtcServer > > diagDtcServerMap_;
};
}  // namespace idsm
}  // namespace ara

#endif
