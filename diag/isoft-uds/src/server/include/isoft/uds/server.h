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
/// @file       server.h
/// @brief      This file provides server functionality
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
/// @trace_id_sr=e175c450-1a873b9-0e87-40dc-a9f9-702ca175c69c
/// @unit_name=server
/// @unit_description=This file provides server functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVER_H_
#define ISOFT_UDS_SERVER_SERVER_H_

#include <memory>
#include <vector>

#include "authentication_management/authentication_management.h"
#include "authentication_management/client_authentication_controller.h"
#include "channel.h"
#include "com_management/com_management.h"
#include "com_management/communication_control.h"
#include "common.h"
#include "custom_management/custom_management.h"
#include "custom_management/generic_uds_service.h"
#include "data_management/diagnostic_data_management.h"
#include "data_management/generic_data_identifier.h"
#include "ecu_reset_management/ecu_reset_management.h"
#include "ecu_reset_management/ecu_reset_request.h"
#include "fault_management/fault_management.h"
#include "message.h"
#include "persistence_interface.h"
#include "result.h"
#include "roe_management/roe_management.h"
#include "routine_management/generic_routine.h"
#include "routine_management/routine_management.h"
#include "security_access_management/security_access.h"
#include "security_access_management/security_access_management.h"
#include "service_validation.h"
#include "session_management/session_management.h"
#include "transfer_managment/download.h"
#include "transfer_managment/file_transfer.h"
#include "transfer_managment/transfer_managment.h"
#include "transfer_managment/upload.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief Server implementation class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01306
/// @trace_id_dd=DD_UDS_02598
/// @needwork = ad
/// @endcode
class ServerImpl;
/// @brief Transport layer interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01307
/// @trace_id_dd=DD_UDS_02599
/// @needwork = ad
/// @endcode
class Transport;
/// @brief Client address type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01308
/// @trace_id_dd=DD_UDS_02600
/// @needwork = ad
/// @endcode
using ClientAddress = Address;
/// @brief Server address type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01309
/// @trace_id_dd=DD_UDS_02601
/// @needwork = ad
/// @endcode
using ServerAddress = Address;
/// @brief Get channel callback function
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01310
/// @trace_id_dd=DD_UDS_02602
/// @needwork = ad
/// @endcode
using GetChannelCallback = std::function< std::shared_ptr< Channel >(ClientAddress, ServerAddress) >;

/// @brief Server settings
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01311
/// @trace_id_dd=DD_UDS_02603
/// @needwork = ad
/// @endcode
class ServerSetting
{
public:
    /// @brief Maximum number of pending requests
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00913
    /// @trace_id_dd=DD_UDS_02027
    /// @needwork = ad
    /// @endcode
    std::uint32_t maxNumberOfRequestCorrectlyReceivedResponsePending{0U};
    /// @brief Whether to respond to all request SIDs
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00914
    /// @trace_id_dd=DD_UDS_02028
    /// @needwork = ad
    /// @endcode
    bool responseOnAllRequestSids{false};
    /// @brief Whether to respond to the second rejection request
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00915
    /// @trace_id_dd=DD_UDS_02029
    /// @needwork = ad
    /// @endcode
    bool responseOnSecondDeclinedRequest{false};
    /// @brief Secure startup delay time
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00916
    /// @trace_id_dd=DD_UDS_02030
    /// @needwork = ad
    /// @endcode
    std::uint32_t securityDelayTimeOnBoot{0U};  // seconds
    /// @brief Maximum number of parallel requests
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00917
    /// @trace_id_dd=DD_UDS_02031
    /// @needwork = ad
    /// @endcode
    std::uint32_t maxParallelRequests{0U};
    /// @brief Functional address table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00918
    /// @trace_id_dd=DD_UDS_02032
    /// @needwork = ad
    /// @endcode
    std::vector< ServerAddress > functionAddressTable{};
    /// @brief Physical address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00919
    /// @trace_id_dd=DD_UDS_02033
    /// @needwork = ad
    /// @endcode
    ServerAddress physicalAddress{0U};
    /// @brief Transport layer interface pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00920
    /// @trace_id_dd=DD_UDS_02034
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< Transport > transportPtr{nullptr};
    /// @brief Manufacturer service validation interface table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00921
    /// @trace_id_dd=DD_UDS_02035
    /// @needwork = ad
    /// @endcode
    std::vector< std::shared_ptr< ServiceValidationInterface > > manufacturerServiceValidationTable{};
    /// @brief Supplier service validation interface table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00922
    /// @trace_id_dd=DD_UDS_02036
    /// @needwork = ad
    /// @endcode
    std::vector< std::shared_ptr< ServiceValidationInterface > > supplierServiceValidationTable{};
};
/// @brief Server
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01312
/// @trace_id_dd=DD_UDS_02604
/// @needwork = ad
/// @endcode
class Server
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00884
    /// @needwork = dda
    /// @endcode
    Server(Server const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00885
    /// @needwork = dda
    /// @endcode
    Server(Server&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00886
    /// @needwork = dda
    /// @endcode
    Server& operator=(Server const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00887
    /// @needwork = dda
    /// @endcode
    Server& operator=(Server&&) = delete;
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00923
    /// @trace_id_dd=DD_UDS_02037
    /// @needwork = ad
    /// @endcode
    Server() noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00924
    /// @trace_id_dd=DD_UDS_02038
    /// @needwork = ad
    /// @endcode
    ~Server() noexcept = default;

    /// @brief Initialization function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00925
    /// @trace_id_dd=DD_UDS_02039
    /// @needwork = ad
    /// @endcode
    bool Initialize(ServerSetting& config);

    /// @brief Load session management module configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00926
    /// @trace_id_dd=DD_UDS_02040
    /// @needwork = ad
    /// @endcode
    bool Initialize(SessionMangement& config) noexcept;
    /// @brief Load security access management module configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00927
    /// @trace_id_dd=DD_UDS_02041
    /// @needwork = ad
    /// @endcode
    bool Initialize(SecurityAccessManagement& config) noexcept;
    /// @brief Load diagnostic data management module configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00928
    /// @trace_id_dd=DD_UDS_02042
    /// @needwork = ad
    /// @endcode
    bool Initialize(DiagnosticDataManagement& config) noexcept;
    /// @brief Load ECU reset management module configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00929
    /// @trace_id_dd=DD_UDS_02043
    /// @needwork = ad
    /// @endcode
    bool Initialize(EcuResetManagement& config) noexcept;
    /// @brief Load fault management module configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00930
    /// @trace_id_dd=DD_UDS_02044
    /// @needwork = ad
    /// @endcode
    bool Initialize(FaultManagement& config) noexcept;
    /// @brief Load communication management module configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad    /// @endcode
    /// @endcode
    bool Initialize(ComManagementConfig& config) noexcept;
    /// @brief Load routing management module configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00931
    /// @trace_id_dd=DD_UDS_02045
    /// @needwork = ad
    /// @endcode
    bool Initialize(RoutineManagement& config) noexcept;
    /// @brief Load transport management module configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00932
    /// @trace_id_dd=DD_UDS_02046
    /// @needwork = ad
    /// @endcode
    bool Initialize(TransferManagment& config) noexcept;
    /// @brief Load ROE management module configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00933
    /// @trace_id_dd=DD_UDS_02047
    /// @needwork = ad
    /// @endcode
    bool Initialize(RoeManagement& config) noexcept;
    /// @brief Load custom management module configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00934
    /// @trace_id_dd=DD_UDS_02048
    /// @needwork = ad
    /// @endcode
    bool Initialize(CustomManagement& config) noexcept;
    /// @brief Load authentication management module configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00935
    /// @trace_id_dd=DD_UDS_02049
    /// @needwork = ad
    /// @endcode
    bool Initialize(AuthenticationManagement& config) noexcept;

    /// @brief Start server function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00936
    /// @trace_id_dd=DD_UDS_02050
    /// @needwork = ad
    /// @endcode
    bool Start();

    /// @brief Stop server function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00937
    /// @trace_id_dd=DD_UDS_02051
    /// @needwork = ad
    /// @endcode
    void Stop();

    /// @brief Get physical address function
    /// @return Physical address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00938
    /// @trace_id_dd=DD_UDS_02052
    /// @needwork = ad
    /// @endcode
    Address GetPhysicalAddress();

    /// @brief Get functional address table function
    /// @return Functional address table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00939
    /// @trace_id_dd=DD_UDS_02053
    /// @needwork = ad
    /// @endcode
    std::vector< Address > const& GetFunctionalAddress();

    /// @brief Preprocess diagnostic request
    /// @param request Indication message
    /// @param priority Priority
    /// @param channel Channel
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00940
    /// @trace_id_dd=DD_UDS_02054
    /// @needwork = ad
    /// @endcode
    Result< void > Indicate(std::shared_ptr< Message > const& request,
                            Priority priority,
                            std::shared_ptr< Channel > const& channel);

    /// @brief Process diagnostic request
    /// @param request Indication message
    /// @param priority Priority
    /// @param channel Channel
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00941
    /// @trace_id_dd=DD_UDS_02055
    /// @needwork = ad
    /// @endcode
    void HandleMessage(std::shared_ptr< Message > const& request,
                       Priority priority,
                       std::shared_ptr< Channel > const& channel) noexcept;

    /// @brief After triggering session re-establishment time, call this function for notification
    /// @param channel Channel
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00942
    /// @trace_id_dd=DD_UDS_02056
    /// @needwork = ad
    /// @endcode
    void NotifyReestablishment(std::shared_ptr< Channel > const& channel);

    /// @brief Register persistence interface
    /// @param persistenceInterfacePtr Persistence interface
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00943
    /// @trace_id_dd=DD_UDS_02057
    /// @needwork = ad
    /// @endcode
    void RegisterPersistence(std::shared_ptr< PersistenceInterface > const& persistenceInterfacePtr) noexcept;

    /// @brief Try to lock
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00944
    /// @trace_id_dd=DD_UDS_02058
    /// @needwork = ad
    /// @endcode
    bool TryLock() noexcept;

    /// @brief Unlock function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00945
    /// @trace_id_dd=DD_UDS_02059
    /// @needwork = ad
    /// @endcode
    void Unlock() noexcept;

    /// @brief Check environmental condition
    /// @param id Environmental condition identifier
    /// @param metaInfo Meta information
    /// @param cancellationState Cancellation state
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00946
    /// @trace_id_dd=DD_UDS_02060
    /// @needwork = ad
    /// @endcode
    Result< std::uint8_t > Check(
        EnvironmentalConditionId id,
        MetaInfoMap const& metaInfo,
        std::shared_ptr< isoft::uds::server::CancellationState > const& cancellationState) noexcept;

    /// @return

    /// @brief Get current session
    /// @return Session identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00947
    /// @trace_id_dd=DD_UDS_02061
    /// @needwork = ad
    /// @endcode
    Result< SessionId > GetCurrentSession() noexcept;

    /// @brief Add session change notification callback
    /// @param callback
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00948
    /// @trace_id_dd=DD_UDS_02062
    /// @needwork = ad
    /// @endcode
    Result< NotifyOfSessionChangeCallbackId > AddNotifyOfSessionChange(
        NotifyOfSessionChangeCallback const& callback) noexcept;

    /// @brief Remove session change notification callback
    /// @param id Identifier
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00949
    /// @trace_id_dd=DD_UDS_02063
    /// @needwork = ad
    /// @endcode
    void RemoveNotifyOfSessionChange(NotifyOfSessionChangeCallbackId id) noexcept;

    /// @brief ResetDefaultSession function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00950
    /// @trace_id_dd=DD_UDS_02064
    /// @needwork = ad
    /// @endcode
    void ResetDefaultSession() noexcept;

    /// @brief Get current security level
    /// @return Security level
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00951
    /// @trace_id_dd=DD_UDS_02065
    /// @needwork = ad
    /// @endcode
    Result< SecurityLevelId > GetCurrentSecurityLevelType() noexcept;

    /// @brief Add security level change notification callback
    /// @param callback
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00952
    /// @trace_id_dd=DD_UDS_02066
    /// @needwork = ad
    /// @endcode
    Result< NotifyOfSecurityLevelChangeCallbackId > AddNotifyOfSecurityLevelChange(
        NotifyOfSecurityLevelChangeCallback& callback) noexcept;

    /// @brief Remove security level change notification callback
    /// @param id Identifier
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00953
    /// @trace_id_dd=DD_UDS_02067
    /// @needwork = ad
    /// @endcode
    void RemoveNotifyOfSecurityLevelChange(NotifyOfSecurityLevelChangeCallbackId id) noexcept;

    /// @brief Add request context state change notification callback
    /// @param callback
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00954
    /// @trace_id_dd=DD_UDS_02068
    /// @needwork = ad
    /// @endcode
    void AddNotifyRequestContextState(NotifyRequestContextStateCallback const& callback) noexcept;

    /// @brief Get client authentication controller
    /// @param clientAddress Client address
    /// @return Client authentication controller
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00955
    /// @trace_id_dd=DD_UDS_02069
    /// @needwork = ad
    /// @endcode
    Result< std::shared_ptr< ClientAuthenticationController > > GetClientAuthentication(
        Address const clientAddress) noexcept;

    /// @brief Get all client authentication addresses
    /// @return Client authentication address list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00956
    /// @trace_id_dd=DD_UDS_02070
    /// @needwork = ad
    /// @endcode
    Result< std::vector< Address > > GetAllClientAuthenticationAddress() noexcept;

private:
    /// @brief Server implementation
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00957
    /// @trace_id_dd=DD_UDS_02071
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< ServerImpl > impl_{nullptr};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif
