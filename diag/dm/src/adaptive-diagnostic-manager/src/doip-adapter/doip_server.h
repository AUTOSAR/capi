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
/// @file       doip_server.h
/// @brief      This file provides the definition of the Diagnostic Service class, which is used to implement the standard diagnostic transmission handler
/// @details
/// @date       2022-07-07
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_DOIP_DOIP_SERVICE_H_
#define ARA_DIAG_DOIP_DOIP_SERVICE_H_
#include <ara/core/map.h>
#include <ara/core/vector.h>
#include <isoft/doip/config.h>
#include <isoft/doip/public_def.h>
#include <isoft/doip/public_message.h>
#include <isoft/doip/server.h>
#include <isoft/uds/channel.h>
#include <isoft/uds/server_manager.h>
#include <isoft/uds/transport.h>
#ifdef NMODEL
    #include <isoft/nml/definition/adaptive_software_component.h>
    #include <isoft/nml/definition/doip_module.h>
#endif
#include "common.h"

namespace isoft {
namespace dm {
namespace dis {
class TriggerVehicleAnnouncementAgent;
}  // namespace dis

namespace dic {
class ActivationLineAgent;
class PowerModeAgent;
class GroupIdentificationAgent;
class GenericDataIdentifierAgent;
class RawDataIdentifierAgent;
}  // namespace dic

}  // namespace dm
}  // namespace isoft
namespace ara {
namespace diag {
namespace doip {

/// @brief Declare diagnostic connection
class DiagChannel;

/// @brief Declare configuration
class DoIPConfig;

/// @brief Declare NetworkEndpointAddress
class NetworkEndpointAddress;

/// @brief Declare diagnostic connection management
class DiagChannelManager;

/// @brief Diagnostic service, used to implement the UdsHandler interface
///
/// @code{.isoft}
/// export_level=/Diagnostics/Diagnostic Manager/DoIP
/// @endcode
///
class DoIPServer : public isoft::uds::server::Transport
{
public:
    /// @brief Copy constructor
    /// @param[in] other
    /// @throw unknown
    DoIPServer(DoIPServer const& other) = delete;

    /// @brief Copy assignment operator
    /// @param[in]  other
    /// @return Reference
    /// @throw unknown
    DoIPServer& operator=(DoIPServer const& other) = delete;

    /// @brief Move constructor
    /// @param[in] other
    /// @throw unknown
    DoIPServer(DoIPServer&& other) = delete;

    /// @brief Move assignment operator
    /// @param[in] other
    /// @return Reference
    /// @throw unknown
    DoIPServer& operator=(DoIPServer&& other) = delete;

    ///@brief Constructor
    /// @code{.isoft}
    /// export_level=/Diagnostics/Diagnostic Manager/DoIP
    /// @endcode
    /// @throw
    explicit DoIPServer(std::shared_ptr< isoft::uds::server::ServerManager > serverManager) noexcept;

#ifdef NMODEL
    /// @brief Initialization
    /// @code{.isoft}
    /// export_level=/Diagnostics/Diagnostic Manager/DoIP
    /// @endcode
    /// @return kInitializeFailed: Failure  kInitializeOk: Success
    /// @throw
    bool Initialize(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentList,
        std::shared_ptr< isoft::nml::definition::DoipModule > doipModulePtr) noexcept;
#endif

    /// @brief Destructor
    /// @code{.isoft}
    /// export_level=/Diagnostics/Diagnostic Manager/DoIP
    /// @endcode
    ~DoIPServer() noexcept override;

    /// @brief Initialization
    /// @code{.isoft}
    /// export_level=/Diagnostics/Diagnostic Manager/DoIP
    /// @endcode
    /// @return kInitializeFailed: Failure  kInitializeOk: Success
    /// @throw
    bool Initialize() override;

    /// @brief Start DoIP service
    /// @code{.isoft}
    /// export_level=/Diagnostics/Diagnostic Manager/DoIP
    /// @endcode
    /// @throw
    bool Start() override;

    /// @brief Stop DoIP service
    /// @code{.isoft}
    /// export_level=/Diagnostics/Diagnostic Manager/DoIP
    /// @endcode
    /// @throw
    void Stop() override;

    std::shared_ptr< isoft::uds::server::Channel > GetChannel(isoft::uds::server::Address serverAddr,
                                                              isoft::uds::server::Address clientAddr) noexcept override;

    /// @brief Reply to DoIP message
    /// @param[in] networkId Network port ID
    /// @param[in] fd fd
    /// @param[in] protocloType Protocol number
    /// @param[in] msg Message body
    /// @param[in] isSend Whether to send
    /// @return Reply result
    bool SendDoIPMessage(uint8_t const networkId,
                         int32_t const fd,
                         isoft::doip::PayloadType const protocloType,
                         ara::core::Vector< uint8_t > const& msg,
                         bool const isSend = true);

private:
    /// @brief Enable DoIP network
    /// @param[in] networkId Network port ID
    bool _startNetworkService(uint8_t const networkId) noexcept;

    /// @brief Stop DoIP network
    /// @param[in] networkId Network port ID
    void _stopNetworkService(uint8_t const networkId) noexcept;

    /// @brief Monitor network card IP changes and update the IP corresponding to the network port
    /// @param networkId Network port ID
    /// @param ip Updated IP
    void _updateNetInterfaceIp(uint8_t const networkId, ara::core::String const& ip);

    /// @brief Get the IP of the network port
    /// @param networkId Network port ID
    /// @return ip
    ara::core::String _getNetInterfaceIpById(uint8_t const networkId);

    /// @brief Open a diagnostic connection
    /// @param[in] networkId Network port ID
    /// @param[in] fd fd
    /// @param[in] localIp Local IP
    /// @param[in] localPort Local port
    /// @param[in] peerIp Peer IP
    /// @param[in] peerPort Peer port
    void _openDiagChannel(uint8_t const networkId,
                          int32_t const fd,
                          uint16_t const sa,
                          ara::core::String const& localIp,
                          uint16_t const localPort,
                          ara::core::String const& peerIp,
                          uint16_t const peerPort);

    /// @brief Close a diagnostic connection
    /// @param[in] localIp Local IP
    /// @param[in] localPort Local port
    /// @param[in] peerIp Peer IP
    /// @param[in] peerPort Peer port
    void _closeDiagChannel(uint8_t const networkId, int32_t const fd);

    /// @brief Handle VIN acquisition logic
    /// @param[in] networkId Network port ID
    /// @param[in] fd sock fd
    void _dealVehicleIdRequest(uint8_t const networkId, int32_t const fd);

    /// @brief Handle vehicle announcement request with EID
    /// @param[in] networkId Network port ID
    /// @param[in] fd File descriptor
    /// @param[in] msg Message body
    void _dealVehicleIdRequestWithEid(uint8_t const networkId,
                                      int32_t const fd,
                                      ara::core::Vector< uint8_t > const& msg) noexcept;

    /// @brief Handle vehicle announcement request with VIN
    /// @param[in] networkId Network port ID
    /// @param[in] fd File descriptor
    /// @param[in] msg Message body
    void _dealVehicleIdRequestWithVin(uint8_t const networkId,
                                      int32_t const fd,
                                      ara::core::Vector< uint8_t > const& msg) noexcept;

    /// @brief Handle power mode data request
    /// @param[in] networkId Network port ID
    /// @param[in] fd File descriptor
    void _dealPowerModeInfoRequest(uint8_t const networkId, int32_t const fd) noexcept;

    /// @brief Handle diagnostic message (UDS)
    /// @param[in] fd sock fd
    /// @param[in] msg Diagnostic message body
    void _dealDiagMessage(uint8_t networkId, int32_t const fd, ara::core::Vector< uint8_t > const& msg);

    /// @brief Get vehicle announcement
    /// @param[in] cb Callback notification for getting the complete announcement
    void _getVehicleId(std::function< void(isoft::doip::VehicleIdResponse&) > const& cb) noexcept;

    /// @brief Get EID
    /// @param[in] networkId Network port ID
    /// @return Result
    isoft::doip::EidValue _getEid(uint8_t const networkId) noexcept;

    /// @brief Find request type by receiver logical address
    /// @param[in] ta Receiver logical address
    /// @return kPhysical , kFunctional
    /// @throw unknown
    isoft::uds::server::TargetAddressType _findUdsAddressType(uint16_t const ta);

    /// @brief Get priority according to diagnostic SA
    /// @param[in] sa
    /// @return 0-3 Three levels agreed with DCM  0: Highest priority  3: Lowest priority
    uint8_t _getAddressPriority(uint16_t sa);

    /// @brief Create interface for reading VIN
    void _createReadVinInterface();

    /// @brief Broadcast announcement encapsulation
    /// @param networkId Network port
    void _broadcastAnnouncement(uint8_t const networkId);

private:
    /// @brief Diagnostic connection manager
    std::shared_ptr< DiagChannelManager > diagChannelManagerPtr_;

    /// @brief Activation line
    ara::core::Map<

        uint32_t

        ,
        std::shared_ptr< isoft::dm::dic::ActivationLineAgent > >
        mapActiveHandlerPtr_;

    /// @brief Actively activate DoIP network
    std::shared_ptr< isoft::dm::dis::TriggerVehicleAnnouncementAgent > doipTriggerVehicleAnnouncementPtr_;

    /// @brief Power mode
    std::shared_ptr< isoft::dm::dic::PowerModeAgent > doipPowerModeHandlerPtr_;

    /// @brief Group ID
    std::shared_ptr< isoft::dm::dic::GroupIdentificationAgent > doipGroupIdHandlerPtr_;

    std::shared_ptr< isoft::dm::dic::GenericDataIdentifierAgent > genericVin_;

    std::shared_ptr< isoft::dm::dic::RawDataIdentifierAgent > rawVin_;

    /// @name  stop_
    /// Service stop flag
    bool stop_;

    /// DoIP service collection
    /// key: Network port ID  value: DoIP service
    ara::core::Map< uint8_t, std::shared_ptr< isoft::doip::Server > > mapDoipServer_;

    /// Mapping relationship between network port ID and IP
    ara::core::Map< uint8_t, ara::core::String > mapNetinterfaceToIp_;
};

}  // namespace doip
}  // namespace diag
}  // namespace ara
#endif  // ARA_DIAG_DOIP_DOIP_SERVICE_H_