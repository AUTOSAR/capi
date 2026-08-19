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
/// @file       runtime_enviroment.h
/// @brief      This file provides the definitions of RuntimeEnviroment and related types.
/// @details
/// @date       2022-11-21
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_INTERNAL_RUNTIME_ENVIROMENT_H_
#define ARA_DIAG_INTERNAL_RUNTIME_ENVIROMENT_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/map.h>
#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/log/logger.h>
#ifdef NMODEL
    #include <isoft/nml/definition/package.h>
#endif
#include "ara/diag/diag_error_domain.h"
#include "conversation_manager.h"
#include "resolve.h"
namespace ara {
namespace diag {
namespace internal {
/// @brief RuntimeEnviroment
class RuntimeEnviroment final
{
public:
    /// @brief Copy constructor is prohibited
    /// @param
    RuntimeEnviroment(RuntimeEnviroment const&) = delete;
    /// @brief Copy assignment operator is prohibited
    /// @param
    /// @return
    RuntimeEnviroment& operator=(RuntimeEnviroment const&) = delete;
    /// @brief Move constructor is prohibited
    /// @param
    RuntimeEnviroment(RuntimeEnviroment&&) = delete;
    /// @brief Move assignment operator is prohibited
    /// @param
    /// @return
    RuntimeEnviroment& operator=(RuntimeEnviroment&&) = delete;

    /// @brief RuntimeEnviroment constructor
    /// @throws on overflow
    RuntimeEnviroment();

    /// @brief RuntimeEnviroment destructor
    ~RuntimeEnviroment();

    /// @brief Runtime environment initialization
    /// @return Initialization result
    /// @throws on overflow
    ara::core::Result< void > Initialize();

    /// @brief Get log operation handle
    /// @return Log handle
    inline ara::log::Logger& GetLogger() const noexcept { return diagLogger_; }

    /// @brief Get instance descriptor
    /// @param specifier Instance specifier
    /// @return Instance descriptor
    inline ara::core::Result< InstanceInfo > Retrieve(ara::core::InstanceSpecifier const& specifier)
    {
        ara::core::Map< ara::core::String, InstanceInfo >::iterator const it{
            registry_.find(std::move(ara::core::String{std::move(specifier.ToString())}))};
        if (it != registry_.end()) {
            return {it->second};
        }
        return ara::core::Result< InstanceInfo >::FromError(MakeErrorCode(DiagErrc::kInvalidArgument, __LINE__));
    }

    /// @brief Get conversation manager
    /// @return Conversation manager
    inline ConversationManager& GetConversationManager() noexcept { return conversationManager_; }
#ifdef NMODEL
    void LoadPackage(std::shared_ptr< isoft::nml::definition::Package > packagePtr,
                     ara::core::String& multicastIP,
                     std::uint16_t& multicastPort,
                     std::uint16_t& appid,
                     std::string& doipModuleFqn) noexcept;

    std::string GetDoipModuleFqn(std::shared_ptr< isoft::nml::definition::Package > packagePtr) noexcept;
#endif
private:
    /// @name diagLogger_
    ara::log::Logger& diagLogger_;
    /// @name registry_
    ara::core::Map< ara::core::String, InstanceInfo > registry_;
    /// @name conversationManager_
    ConversationManager conversationManager_;
    /// @name netProxyStart_
    bool netProxyStart_;
};

}  // namespace internal
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_INTERNAL_RUNTIME_ENVIROMENT_H_