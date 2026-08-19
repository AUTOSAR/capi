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
/// @file       server.cpp
/// @brief      This file implements server functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds
/// @trace_id_sr=e175c450-1a873b9-0e87-40dc-a9f9-702ca175c69c
/// @unit_name=server
/// @unit_description=This file provides server functionality
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "isoft/uds/server.h"

#include "server_impl.h"

namespace isoft {
namespace uds {
namespace server {
Server::Server() noexcept : impl_{std::make_shared< ServerImpl >()} {}

/// @return
bool Server::Initialize(ServerSetting& config) { return impl_->Initialize(config); }

bool Server::Initialize(SessionMangement& config) noexcept { return impl_->Initialize(config); }

bool Server::Initialize(SecurityAccessManagement& config) noexcept { return impl_->Initialize(config); }

bool Server::Initialize(DiagnosticDataManagement& config) noexcept { return impl_->Initialize(config); }

bool Server::Initialize(EcuResetManagement& config) noexcept { return impl_->Initialize(config); }

bool Server::Initialize(FaultManagement& config) noexcept { return impl_->Initialize(config); }

bool Server::Initialize(ComManagementConfig& config) noexcept { return impl_->Initialize(config); }

bool Server::Initialize(RoutineManagement& config) noexcept { return impl_->Initialize(config); }

bool Server::Initialize(TransferManagment& config) noexcept { return impl_->Initialize(config); }

bool Server::Initialize(RoeManagement& config) noexcept { return impl_->Initialize(config); }

bool Server::Initialize(CustomManagement& config) noexcept { return impl_->Initialize(config); }

bool Server::Initialize(AuthenticationManagement& config) noexcept { return impl_->Initialize(config); }
/// @return
bool Server::Start() { return impl_->Start(); }

void Server::Stop() { return impl_->Stop(); }


Address Server::GetPhysicalAddress() { return impl_->GetPhysicalAddress(); }


std::vector< Address > const& Server::GetFunctionalAddress() { return impl_->GetFunctionalAddress(); }





Result< void > Server::Indicate(std::shared_ptr< Message > const& request,
                                Priority priority,
                                std::shared_ptr< Channel > const& channel)
{
    return impl_->Indicate(std::move(request), priority, std::move(channel));
}




void Server::HandleMessage(std::shared_ptr< Message > const& request,
                           Priority priority,
                           std::shared_ptr< Channel > const& channel) noexcept
{
    return impl_->HandleMessage(std::move(request), priority, std::move(channel));
}


void Server::NotifyReestablishment(std::shared_ptr< Channel > const& channel)
{
    return impl_->NotifyReestablishment(channel);
}


void Server::RegisterPersistence(std::shared_ptr< PersistenceInterface > const& persistenceInterfacePtr) noexcept
{
    return impl_->RegisterPersistence(persistenceInterfacePtr);
}

/// @return
bool Server::TryLock() noexcept { return impl_->TryLock(); }

void Server::Unlock() noexcept { return impl_->Unlock(); }



Result< std::uint8_t > Server::Check(
    EnvironmentalConditionId id,
    MetaInfoMap const& metaInfo,
    std::shared_ptr< isoft::uds::server::CancellationState > const& cancellationState) noexcept
{
    return impl_->Check(id, metaInfo, cancellationState);
}


Result< SessionId > Server::GetCurrentSession() noexcept { return impl_->GetCurrentSession(); }



Result< NotifyOfSessionChangeCallbackId > Server::AddNotifyOfSessionChange(
    NotifyOfSessionChangeCallback const& callback) noexcept
{
    return impl_->AddNotifyOfSessionChange(callback);
}

void Server::RemoveNotifyOfSessionChange(NotifyOfSessionChangeCallbackId id) noexcept
{
    return impl_->RemoveNotifyOfSessionChange(id);
}

void Server::ResetDefaultSession() noexcept { return impl_->ResetDefaultSession(); }

Result< SecurityLevelId > Server::GetCurrentSecurityLevelType() noexcept
{
    return impl_->GetCurrentSecurityLevelType();
}

Result< NotifyOfSecurityLevelChangeCallbackId > Server::AddNotifyOfSecurityLevelChange(
    NotifyOfSecurityLevelChangeCallback& callback) noexcept
{
    return impl_->AddNotifyOfSecurityLevelChange(callback);
}

void Server::RemoveNotifyOfSecurityLevelChange(NotifyOfSecurityLevelChangeCallbackId const id) noexcept
{
    return impl_->RemoveNotifyOfSecurityLevelChange(id);
}

void Server::AddNotifyRequestContextState(NotifyRequestContextStateCallback const& callback) noexcept
{
    return impl_->AddNotifyRequestContextState(callback);
}

Result< std::shared_ptr< ClientAuthenticationController > > Server::GetClientAuthentication(
    Address const clientAddress) noexcept
{
    return impl_->GetClientAuthentication(clientAddress);
}
Result< std::vector< Address > > Server::GetAllClientAuthenticationAddress() noexcept
{
    return impl_->GetAllClientAuthenticationAddress();
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
