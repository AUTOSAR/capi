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
/// @file       manager.h
/// @brief      time domain management class
/// @details
/// @date       2023-01-12
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/TimeDomain
/// module_path=/TimeSync/TimeDomain
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEDOMAIN_MANAGER_H_
#define ARA_TSYNC_INTERNAL_TIMEDOMAIN_MANAGER_H_

#include <ara/core/map.h>

#include <chrono>
#include <cstdint>

#include "ara/tsync/internal/config/timedomainset.h"
#include "ara/tsync/internal/log/logger.h"
#include "ara/tsync/internal/timedomain/tdcontext.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timedomain {

/// Maximum 32 time domains: 0-15 synchronization time domains, 16-31 offset time domains.
static std::uint8_t const kMaxNumber{32U};

/// @brief time domain, used to save context and configuration information of time synchronization process related domains
class TDManager final
{
public:
    /// @brief constructor
    /// @param tdConfig
    explicit TDManager(std::shared_ptr< config::TimeDomainSet const > tdConfig) noexcept
        : configSet_{std::move(tdConfig)}, contextSet_{}
    {
    }

    /// @brief destructor
    ~TDManager() = default;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    TDManager(TDManager const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    TDManager &operator=(TDManager const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    TDManager(TDManager &&other) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    TDManager &operator=(TDManager &&other) &noexcept = default;

    /// @brief create manager
    /// @param pConfig configuration manager
    /// @return manager handle
    static std::shared_ptr< TDManager > CreateManager(
        std::shared_ptr< config::TimeDomainSet const > const &pConfig) noexcept
    {
        std::shared_ptr< TDManager > man{std::make_shared< TDManager >(pConfig)};
        return man;
    }

    /// @brief get context address of the specified time domain
    /// @param domainId - time domain ID
    /// @return context address
    std::shared_ptr< TDContext > GetContext(internal::TimeDomainId const &domainId) noexcept
    {
        std::shared_ptr< TDContext > ctx{nullptr};
        if (true == domainId.IsValid()) {
            ara::core::Map< std::uint8_t, std::shared_ptr< TDContext > >::iterator const domainCtx{
                contextSet_.find(domainId.ToUint8())};
            if (contextSet_.end() == domainCtx) {
                contextSet_[domainId.ToUint8()] = std::make_shared< TDContext >();
                contextSet_[domainId.ToUint8()]->SetDomainId(domainId);
            }
            ctx = contextSet_[domainId.ToUint8()];
        }
        return ctx;
    }

    /// @brief get configurations of all time domains
    /// @return time domain configuration handle
    std::shared_ptr< config::TimeDomainSet const > GetAllConfig() const noexcept { return configSet_; }

    /// @brief get configuration of the specified time domain
    /// @param domainId - time domain ID
    /// @returns configuration address
    config::TimeDomainSet::Domain const *GetConfig(internal::TimeDomainId const &domainId) const noexcept
    {
        if (nullptr == configSet_) {
            return nullptr;
        }
        return configSet_->GetDomain(domainId);
    }

    /// @brief get Master configuration of the specified time domain
    /// @param domainId - time domain ID
    /// @returns Master configuration address
    config::TimeDomainSet::Master const *GetMasterConfig(internal::TimeDomainId const &domainId) const noexcept
    {
        if (nullptr == configSet_) {
            return nullptr;
        }
        config::GlobalTimeDomain const *const domain{configSet_->GetDomain(domainId)};
        if (nullptr == domain) {
            return nullptr;
        }
        return &(domain->GetMaster());
    }

    /// @brief get Slave configuration of the specified time domain
    /// @param domainId - time domain ID
    /// @param name - Slave name
    /// @returns Slave configuration address, nullptr indicates failure
    config::TimeDomainSet::Slave const *GetSlaveConfig(internal::TimeDomainId const &domainId,
                                                       ara::core::String const &name) const noexcept
    {
        if (nullptr == configSet_) {
            return nullptr;
        }
        config::GlobalTimeDomain const *const domain{configSet_->GetDomain(domainId)};
        if (nullptr == domain) {
            return nullptr;
        }
        return domain->GetSlave(name);
    }

private:
    /// @name configSet_ - time domain configuration set
    std::shared_ptr< config::TimeDomainSet const > configSet_;

    ara::core::Map< std::uint8_t, std::shared_ptr< TDContext > >
        /// @name contextSet_ - time domain context set
        contextSet_;

};  // class TDManager
}  // namespace timedomain
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  //ARA_TSYNC_INTERNAL_TIMEDOMAIN_MANAGER_H_
