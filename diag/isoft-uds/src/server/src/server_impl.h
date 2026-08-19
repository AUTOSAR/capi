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
/// @file       server_impl.h
/// @brief      This file provides server_impl functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds
/// @interface_level=unit
/// @trace_id_sr=0ff53c6f-8e31-4351-8da4-0934285c7506
/// @unit_name=server_impl.h
/// @unit_description=This file provides server_impl functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVERIMPL_H_
#define ISOFT_UDS_SERVER_SERVERIMPL_H_
#include <isoft/uds/server.h>

#include "authentication_management/authentication_manager.h"
#include "common/service_interface.h"
#include "data_management/diagnostic_data_manager.h"
#include "data_management/environmental_condition_manager.h"
#include "fault_management/fault_manager.h"
#include "roe_management/diagnostic_roe_manager.h"
#include "security_access_management/security_level_manager.h"
#include "service_validation_service.h"
#include "session_management/session_manager.h"
#include "time_line.h"
#include "transfer_managment/transfer_manager.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Server implementation class
/// @code{.isoft}
/// @interface_level=unit
/// @needwork=dd
/// @endcode
class ServerImpl
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00569
    /// @needwork = dda
    /// @endcode
    ServerImpl(ServerImpl const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00570
    /// @needwork = dda
    /// @endcode
    ServerImpl(ServerImpl&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00571
    /// @needwork = dda
    /// @endcode
    ServerImpl& operator=(ServerImpl const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00572
    /// @needwork = dda
    /// @endcode
    ServerImpl& operator=(ServerImpl&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    ServerImpl() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    ~ServerImpl() noexcept = default;

    /// @brief Load global configuration
    /// @param[in] config Server configuration
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool Initialize(ServerSetting& config);
    /// @brief Load session management configuration
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool Initialize(SessionMangement& config) noexcept;
    /// @brief Load security access management configuration
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool Initialize(SecurityAccessManagement& config) noexcept;
    /// @brief Load diagnostic data management configuration
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool Initialize(DiagnosticDataManagement& config) noexcept;
    /// @brief Load ECU reset management configuration
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool Initialize(EcuResetManagement& config) noexcept;
    /// @brief Load fault management configuration
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool Initialize(FaultManagement& config) noexcept;
    /// @brief Load communication management configuration
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool Initialize(ComManagementConfig& config) noexcept;
    /// @brief Load routing management configuration
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool Initialize(RoutineManagement& config) noexcept;
    /// @brief Load transport management configuration
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool Initialize(TransferManagment& config) noexcept;
    /// @brief Load ROE management configuration
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool Initialize(RoeManagement& config) noexcept;
    /// @brief Load custom management configuration
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool Initialize(CustomManagement& config) noexcept;
    /// @brief Load authentication management configuration
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool Initialize(AuthenticationManagement& config) noexcept;
    /// @brief Start server
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool Start();

    /// @brief Stop server
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void Stop();

    /// @brief Get physical address
    /// @return Physical address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    Address GetPhysicalAddress() const noexcept;

    /// @brief Get functional address
    /// @return
    /// @return Functional address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::vector< Address > const& GetFunctionalAddress();

    /// @brief Notify new request arrival
    /// @param request Request message
    /// @param priority Priority
    /// @param channel Session channel
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    Result< void > Indicate(std::shared_ptr< Message > const& request,
                            Priority priority,
                            std::shared_ptr< Channel > const& channel);

    /// @brief Process message
    /// @param request Request message
    /// @param priority Priority
    /// @param channel Session channel
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void HandleMessage(std::shared_ptr< Message > const& request,
                       Priority priority,
                       std::shared_ptr< Channel > const& channel) noexcept;

    /// @brief Notify session re-establishment
    /// @param channel Session channel
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void NotifyReestablishment(std::shared_ptr< Channel > const& channel) noexcept;

    /// @brief Add manufacturer service validator
    /// @param serviceValidation Instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void AddManufacturerServiceValidation(
        std::shared_ptr< ServiceValidationInterface > const& serviceValidation) noexcept;

    /// @brief Add supplier service validator
    /// @param serviceValidation Instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void AddSupplierServiceValidation(std::shared_ptr< ServiceValidationInterface > const& serviceValidation) noexcept;

    /// @param sa Client address
    /// @param rawMsg Raw data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00573
    /// @needwork = dda
    /// @endcode
    void RequestServiceToRespond(Address sa, std::vector< std::uint8_t >& rawMsg) noexcept;

    /// @param persistenceInterfacePtr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00574
    /// @needwork = dda
    /// @endcode
    void RegisterPersistence(std::shared_ptr< PersistenceInterface > const& persistenceInterfacePtr) noexcept;

    /// @brief Confirm request
    /// @param request Request context
    /// @param status Status
    /// @param ignoreServiceConfirm Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void Confirm(RequestContext& request, ResponseStatusType status, bool ignoreServiceConfirm) noexcept;

    /// @brief Try to lock
    /// @return Whether locking succeeded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool TryLock() noexcept;

    /// @brief Unlock
    /// @return Whether unlocking succeeded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void Unlock() noexcept;

    /// @brief Check environmental condition
    /// @param id Environmental condition identifier
    /// @param metaInfo Meta information
    /// @param cancelEventPtr Cancel event object
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    Result< std::uint8_t > Check(
        EnvironmentalConditionId id,
        MetaInfoMap const& metaInfo,
        std::shared_ptr< isoft::uds::server::CancellationState > const& cancellationState) noexcept;

    /// @brief Get current session
    /// @return Current session identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    Result< SessionId > GetCurrentSession() noexcept;

    /// @brief Add session change notification callback
    /// @param callback Callback function
    /// @return Callback identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    Result< NotifyOfSessionChangeCallbackId > AddNotifyOfSessionChange(NotifyOfSessionChangeCallback callback) noexcept;

    /// @brief Remove session change notification callback
    /// @param id Identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void RemoveNotifyOfSessionChange(NotifyOfSessionChangeCallbackId id) noexcept;

    /// @brief ResetDefaultSession function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void ResetDefaultSession() noexcept;

    /// @brief Get current security level type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    Result< SecurityLevelId > GetCurrentSecurityLevelType() noexcept;

    /// @brief Add security level change notification callback
    /// @param callback Callback function
    /// @return Callback identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    Result< NotifyOfSecurityLevelChangeCallbackId > AddNotifyOfSecurityLevelChange(
        NotifyOfSecurityLevelChangeCallback& callback) noexcept;

    /// @brief Remove security level change notification callback
    /// @param id Identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void RemoveNotifyOfSecurityLevelChange(NotifyOfSecurityLevelChangeCallbackId id) noexcept;

    /// @brief Add request context state change notification callback
    /// @param const Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void AddNotifyRequestContextState(NotifyRequestContextStateCallback const& callback) noexcept;

    /// @brief Get client authentication controller
    /// @param clientAddress Client address
    /// @return Client authentication controller
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    Result< std::shared_ptr< ClientAuthenticationController > > GetClientAuthentication(
        Address const clientAddress) noexcept;

    /// @brief Get all client authentication addresses
    /// @return All client authentication addresses
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    Result< std::vector< Address > > GetAllClientAuthenticationAddress() noexcept;

    /// @brief Get service interface
    /// @param sid Service identifier
    /// @return Service interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::shared_ptr< ServiceInterface > GetServiceBySid(std::uint8_t const sid) noexcept;

private:
    /// @brief Create meta info map
    /// @param request Request message
    /// @param bSuppPosResponse Whether positive response is supported
    /// @param channelPtr Channel pointer
    /// @return Meta info map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    MetaInfoMap _createMetaInfoMap(std::shared_ptr< Message > const& request,
                                   bool bSuppPosResponse,
                                   std::shared_ptr< Channel > const& channelPtr) noexcept;

    /// @brief Process request context
    /// @param requestContextPtr Request context pointer
    /// @return Void
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void _hanldeRequestContext(std::shared_ptr< RequestContext > const& requestContextPtr) noexcept;

    /// @brief _allocRequestHandler function
    /// @return Unsigned integer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::uint16_t _allocRequestHandler() noexcept;

    /// @brief Remove request handler
    /// @param request Request context
    /// @return Void
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void _removeRequestHandler(RequestContext& request) noexcept;

    /// @brief Prioritize request processing
    /// @param[in] newClient New client address
    /// @param[in] priority Priority
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool _prioritize(Address newClient, Priority const priority) noexcept;

    /// @brief Check if it is a non-default session
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool _isNonDefaultSession() noexcept { return sessionManagerPtr_->GetCurrentSessionId() != 1; }

    /// @brief Update priority of non-default session
    /// @param newSessionId Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void _updateNonDefaultPriority(SessionId, SessionId newSessionId) noexcept;

    /// @brief Start S3 timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void _startS3Timer() noexcept;

    /// @brief S3 timeout handling
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void _s3Timeout() noexcept;

    /// @brief Check if S3 timer is running
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool _s3Running() noexcept;

    /// @brief Stop S3 timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void _stopS3Timer() noexcept;

    /// @brief Update request context state table
    /// @param client Parameter
    /// @param state Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    void _updateRequestContextStateTable(Address client, RequestContextState state) noexcept;

    /// @brief Check request context state
    /// @param client Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    bool _isRequestState(Address client) noexcept;

private:
    /// @brief Physical address
    /// @name physicalAddress_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    Address physicalAddress_{0U};
    /// @brief Functional address
    /// @name functionalAddress_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::vector< Address > functionalAddress_{};

    /// @brief Diagnostic data manager pointer
    /// @name diagnosticDataManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::shared_ptr< DiagnosticDataManager > diagnosticDataManagerPtr_{nullptr};
    /// @brief Get channel callback function pointer
    /// @name getChannelCallback_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    GetChannelCallback getChannelCallback_{nullptr};
    /// @brief Service validation service pointer
    /// @name serviceValidationServicePtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::unique_ptr< ServiceValidationService > serviceValidationServicePtr_{nullptr};
    /// @brief Protocol table
    /// @name protocolTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::map< std::uint8_t, std::shared_ptr< ServiceInterface > > protocolTable_{};
    /// @brief Re-establishment notification table
    /// @name notifyReestablishmentTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::map< Address, std::vector< std::function< void(std::shared_ptr< RequestContext >) > > >
        notifyReestablishmentTable_{};
    /// @brief Session manager pointer
    /// @name sessionManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::shared_ptr< SessionManager > sessionManagerPtr_{nullptr};
    /// @brief Security level manager pointer
    /// @name securityLevelManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::shared_ptr< SecurityLevelManager > securityLevelManagerPtr_{nullptr};
    /// @brief Environmental condition manager pointer
    /// @name envConditionManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::shared_ptr< EnvironmentalConditionManager > envConditionManagerPtr_{nullptr};
    /// @brief Timeline pointer
    /// @name timeLinePtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::shared_ptr< TimeLine > timeLinePtr_{nullptr};
    /// @brief Persistence service pointer
    /// @name persistenceServicePtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::shared_ptr< PersistenceService > persistenceServicePtr_{nullptr};
    /// @brief Transport manager pointer
    /// @name transferManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::shared_ptr< TransferManager > transferManagerPtr_{nullptr};
    /// @brief Request context table
    /// @name requestContextTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::map< Address, std::shared_ptr< RequestContext > > requestContextTable_{};
    /// @brief Client request context state table
    /// @name clientRequestContextStateTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::map< Address, RequestContextState > clientRequestContextStateTable_{};
    /// @brief Request handler table
    /// @name requestHandlerTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::set< std::uint16_t > requestHandlerTable_{};
    /// @brief Wait queue
    /// @name waitQueue_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::queue< std::shared_ptr< RequestContext > > waitQueue_{};
    /// @brief Request handler lock
    /// @name requestHandlerLock_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::mutex requestHandlerLock_{};
    /// @brief Request table lock
    /// @name requestContextLock_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::mutex requestContextLock_{};
    /// @brief Client request context state lock
    /// @name clientRequestContextStateLock_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::mutex clientRequestContextStateLock_{};
    /// @brief Wait queue lock
    /// @name waitQueueLock_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::mutex waitQueueLock_{};
    /// @brief Server configuration
    /// @name config_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    ServerSetting config_{};
    /// @brief Non-default priority
    /// @name nonDefaultPriority_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    Priority nonDefaultPriority_{0U};
    /// @brief Lock
    /// @name lock_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::atomic_bool lock_{false};
    /// @brief Notification table
    /// @name notifyTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::vector< NotifyRequestContextStateCallback > notifyTable_{};
    /// @brief Diagnostic manager pointer
    /// @name diagnosticRoeManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::shared_ptr< DiagnosticRoeManager > diagnosticRoeManagerPtr_{nullptr};
    /// @brief Service 3 timer
    /// @name s3Timer_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    Timer s3Timer_{};
    /// @brief Fault manager
    /// @name faultManager_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    FaultManager faultManager_{};
    /// @brief Response message list
    /// @name responseMsgList_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::map< ChannelIdentifier, MessagePtr > responseMsgList_{};
    /// @brief Blocked during ECU reset process
    /// @name blockInCaseEcuResetProcess_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::atomic_bool blockInCaseEcuResetProcess_{false};
    /// @brief Authentication manager pointer
    /// @name authenticationManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=dd
    /// @endcode
    std::shared_ptr< AuthenticationManager > authenticationManagerPtr_{nullptr};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  /// ISOFT_UDS_SERVER_SERVERIMPL_H_
