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
/// @file       cg_manager.h
/// @brief
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_CG_MANAGER_H
#define ARA_COM_CG_MANAGER_H
#include "ara/com/internal/cg/cg_instance.h"
namespace ara {
namespace com {
namespace internal {
namespace cg {
/// @brief
struct CgClient final
{
    /// @brief client instance specifier
    ara::core::String instanceSpecifier;
    /// @brief client instance id
    ara::core::String instanceId;
    /// @brief client id
    uint32_t clientId;
};
/// @brief
struct CgConf final
{
    /// @brief server instancer specifier
    ara::core::String instanceSpecifier;
    /// @brief client list
    ara::core::Vector< CgClient > clients;
    /// @brief cg type
    uint32_t cgType;
};

/// @brief
class CommunicationGroupMgr final
{
    enum class ServiceType
    {
        kCommunicationGroupService = 0,
        kPowerModeService,
        kDiagnosticResetService,
    };

public:
    /// @brief
    explicit CommunicationGroupMgr() noexcept = default;
    /// @brief
    ~CommunicationGroupMgr() noexcept = default;
    /// @brief Copy constructor
    /// @param other
    CommunicationGroupMgr(CommunicationGroupMgr const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    CommunicationGroupMgr(CommunicationGroupMgr&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return CommunicationGroupMgr
    CommunicationGroupMgr& operator=(CommunicationGroupMgr const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return CommunicationGroupMgr
    CommunicationGroupMgr& operator=(CommunicationGroupMgr&& other) noexcept = default;

public:
    /// @brief Start the communication group service
    /// @return bool
    /// @retval true Start succeeded
    /// @retval false Start failed
    static bool StartCommunicationGroupService() noexcept;
    /// @brief Iterate and process method and event requests received by the skeleton
    /// @return ara::core::Future<bool>
    static void ProcessNextMethodCall() noexcept;
    /// @brief Stop the communication group service
    /// @return false Start failed
    static bool StopCommunicationGroupService() noexcept;
};
}  // namespace cg
}  // namespace internal
}  // namespace com
}  // namespace ara

#endif