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
/// @file       common.h
/// @brief      This file provides common definitions of the DoIP adaptation layer
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_DOIP_COMMON_H_
#define ARA_DIAG_DOIP_COMMON_H_
#include <ara/core/string.h>
#include <ara/diag/uds_transport/protocol_handler.h>
#include <ara/diag/uds_transport/protocol_mgr.h>
#include <ara/diag/uds_transport/protocol_types.h>
#include <ara/diag/uds_transport/uds_message.h>
#include <isoft/naicpp/global_evloop.h>

#include <iomanip>  // std::setiosflags
#include <iostream>
#include <memory>
#include <sstream>  // std::cout, std::hex, std::endl
#include <string>
namespace ara {
namespace diag {
namespace doip {

///------------------------------------------------------------------------------------------------------
/// @brief Declare UdsHandler
using UdsHandler = uds_transport::UdsTransportProtocolHandler;

/// @brief Declare UdsHandlerID
using UdsHandlerID = uds_transport::UdsTransportProtocolHandlerID;

/// @brief Declare UdsMgr
using UdsMgr = uds_transport::UdsTransportProtocolMgr;

/// @brief Declare UdsChannelID
using UdsChannelID = uds_transport::ChannelID;

/// @brief Declare UdsPeriodicHandler
using UdsPeriodicHandler = uds_transport::UdsTransportProtocolPeriodicHandler;

/// @brief Declare UdsMessage
using UdsMessage = uds_transport::UdsMessage;

/// @brief Declare UdsMessagePtr
using UdsMessagePtr = uds_transport::UdsMessagePtr;

/// @brief Declare UdsMessageConstPtr
using UdsMessageConstPtr = uds_transport::UdsMessageConstPtr;

/// @brief Declare UdsAddressType
using UdsAddressType = uds_transport::UdsMessage::TargetAddressType;

/// @brief Declare UdsTransmissionResult
using UdsTransmissionResult = uds_transport::UdsTransportProtocolMgr::TransmissionResult;

/// @brief Declare UdsIndicationResult
using UdsIndicationResult = uds_transport::UdsTransportProtocolMgr::IndicationResult;

/// @brief Declare UdsGlobalChannelId
using UdsGlobalChannelId = uds_transport::UdsTransportProtocolMgr::GlobalChannelIdentifier;

/// @brief Declare UdsPriority
using UdsPriority = uds_transport::Priority;

/// @brief Declare UdsProtocolKind
using UdsProtocolKind = uds_transport::ProtocolKind;
///-------------------------------------------------------------------------------------------------------------

///-------------------------------------------------------------------------------------------------------------
/// @brief Task tool, used to delay task execution
class TaskTool final
{
public:
    /// @brief Delay execution of function
    /// @param[in] fun Function to be executed
    static void PostTask(std::function< void() >&& fun) noexcept
    {
        std::shared_ptr< isoft::naicpp::EvLoop > loop{isoft::naicpp::GlobalGeneralEvLoop::Get()};
        std::ignore = loop->Exec(std::move(fun));
    }
};
///---------------------------------------------------------------------------------------------------------------

///------------------------------------------------------------------------------------------------------------------
/// @brief Assist inherited classes in managing life cycle
class SupportWeakPtr
{
public:
    /// @brief Constructor
    /// @throw unknown
    SupportWeakPtr() : flagPtr_{std::make_shared< std::int32_t >()} {}

    /// @brief Destructor
    /// @throw unknown
    virtual ~SupportWeakPtr() noexcept = default;

    /// @brief Copy constructor is prohibited
    /// @param[in] other
    /// @throw unknown
    SupportWeakPtr(SupportWeakPtr const& other) = delete;

    /// @brief Copy assignment operator is prohibited
    /// @param[in] right
    /// @return Reference
    SupportWeakPtr& operator=(SupportWeakPtr const& right) = delete;

    /// @brief Move constructor is prohibited
    /// @param[in] right
    /// @throw unknown
    SupportWeakPtr(SupportWeakPtr&& right) = delete;

    /// @brief Move assignment operator is prohibited
    /// @param[in] right
    /// @return Reference
    SupportWeakPtr& operator=(SupportWeakPtr&& right) = delete;

    /// @brief Get weak reference pointer
    /// @return Weak reference pointer
    std::weak_ptr< std::int32_t > WeakPtr() const noexcept { return flagPtr_; }

private:
    /// @name flagPtr_
    /// Implement weak pointer reference
    std::shared_ptr< std::int32_t > flagPtr_;
};
///---------------------------------------------------------------------------------------------------------------------

}  // namespace doip
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_DOIP_COMMON_H_