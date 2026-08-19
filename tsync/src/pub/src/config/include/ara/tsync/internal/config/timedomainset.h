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
/// @file       timedomainset.h
/// @brief      configuration module time domain set class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/Config
/// module_path=/TimeSync/Config
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_CONFIG_TIMEDOMAINSET_H_
#define ARA_TSYNC_INTERNAL_CONFIG_TIMEDOMAINSET_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include <cstdint>
#include <cstring>

#include "ara/tsync/internal/config/common.h"
#include "ara/tsync/internal/config/globaltimedomain.h"

namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief time domain set class
class TimeDomainSet final
{
public:
    /// @brief Master
    using Master = GlobalTimeMaster;

    /// @brief Slave
    using Slave = GlobalTimeSlave;

    /// @brief Domain
    using Domain = GlobalTimeDomain;

    /// @brief DomainProperty
    using DomainProperty = GlobalTimeDomainProperty;

    /// @brief load time domain configuration from file
    /// @param manifestPath - manifest file path
    /// @return 0 - success
    /// @return <0 - error code
    std::int32_t Load(ara::core::String const &manifestPath) noexcept;

    /// @brief get DomainName via DomainId.
    /// @param domainId time domain ID
    /// @return time domain name
    /// @return empty - not found
    ara::core::StringView const GetNameById(TimeDomainId const &domainId) const noexcept
    {
        ara::core::StringView name;
        Domain const *const d{GetDomain(domainId)};
        if (nullptr != d) {
            name = ara::core::StringView{d->GetName().data(), d->GetName().size()};
        }
        return name;
    }

    /// @brief get DomainId via DomainName.
    /// @param domainName time domain name
    /// @return time domain ID
    TimeDomainId GetIdbyName(ara::core::String const &domainName) const noexcept
    {
        TimeDomainId id;
        Domain const *const d{GetDomain(domainName)};
        if (nullptr != d) {
            id = d->GetDomainId();
        }
        return id;
    }

    /// @brief get configuration of the specified time domain.
    /// @param domainId time domain ID
    /// @return !nullptr - time domain configuration
    /// @return nullptr - configuration not found
    Domain const *GetDomain(TimeDomainId const &domainId) const noexcept
    {
        Domain const *d{nullptr};
        for (ara::core::Vector< GlobalTimeDomain >::const_iterator itm{timeDomainSet_.cbegin()};
             itm != timeDomainSet_.cend(); ++itm) {
            if (domainId == itm->GetDomainId()) {
                d = &(*itm);
                break;
            }
        }
        return d;
    }

    /// @brief get configuration of the specified time domain.
    /// @param domainName time domain name
    /// @return !nullptr - time domain configuration
    /// @return nullptr - configuration not found
    Domain const *GetDomain(ara::core::String const &domainName) const noexcept
    {
        ara::core::StringView const svDomainName{std::move(BaseName(domainName))};
        ara::core::Vector< GlobalTimeDomain >::const_iterator itm{timeDomainSet_.begin()};
        for (; itm != timeDomainSet_.end(); itm++) {
            ara::core::StringView const svDomain{itm->GetName().data(), itm->GetName().size()};
            if (svDomainName == svDomain) {
                return &(*itm);
            }
        }
        return nullptr;
    }

    /// @brief set the current machine as the Master of the specified time domain
    /// @param domainName - time domain name
    void SetMasterOnThisMachine(ara::core::String const &domainName) const noexcept
    {
        Domain *const d{const_cast< Domain * >(GetDomain(domainName))};
        if (nullptr != d) {
            d->SetIsMasterOnThisMachine(true);
        }
    }

    /// @brief get the underlying bound synchronization time domain ID based on the offset time domain ID
    /// @param offsetDomainId - given offset time domain ID
    /// @return time domain ID
    TimeDomainId GetSyncDomainId(TimeDomainId const &offsetDomainId) const noexcept
    {
        Domain const *d{nullptr};
        Domain const *rd{nullptr};
        TimeDomainId id;
        if (true != offsetDomainId.IsOffsetDomain()) {
            return id;
        }
        d = GetDomain(offsetDomainId);
        if (nullptr == d) {
            return id;
        }
        ara::core::StringView const svDomainName{std::move(BaseName(d->GetOffsetTimeDomain()))};
        for (ara::core::Vector< GlobalTimeDomain >::const_iterator itm{timeDomainSet_.cbegin()};
             itm != timeDomainSet_.cend(); ++itm) {
            ara::core::StringView const svSyncDomain{std::move(BaseName(itm->GetName()))};
            if (svDomainName == svSyncDomain) {
                rd = &(*itm);
                break;
            }
        }
        if (nullptr == rd) {
            return id;
        }
        if (rd->GetDomainId().IsSyncDomain()) {
            id = rd->GetDomainId();
        }
        return id;
    }

    /// @brief get Slave configuration
    /// @param domainId - time domain ID
    /// @param slaveName - time slave name
    /// @return time slave configuration pointer
    Slave const *GetSlave(TimeDomainId const &domainId, ara::core::StringView const &slaveName) const noexcept
    {
        Slave const *s{nullptr};
        Domain const *const domain{GetDomain(domainId)};
        if (nullptr != domain) {
            s = domain->GetSlave(slaveName);
        }
        return s;
    }

    /// @brief get Slave configuration
    /// @param domainId - time domain ID
    /// @param slaveName - time slave name
    /// @return time slave configuration pointer
    Slave const *GetSlave(TimeDomainId const &domainId, ara::core::String const &slaveName) const noexcept
    {
        ara::core::StringView const sv{slaveName.data(), slaveName.size()};
        return GetSlave(domainId, sv);
    }

    /// @brief get time master configuration
    /// @param domainId - time domain ID
    /// @return time master configuration pointer
    Master const *GetMaster(TimeDomainId const &domainId) const noexcept
    {
        Master const *m{nullptr};
        Domain const *const domain{GetDomain(domainId)};
        if (nullptr != domain) {
            m = &(domain->GetMaster());
        }
        return m;
    }

    /// @brief print debug information
    void Debug() const noexcept;

private:
    /// @brief time domain configuration set
    /// @name timeDomainSet_
    ara::core::Vector< GlobalTimeDomain > timeDomainSet_;
};  // class TimeDomain

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  /// ARA_TSYNC_INTERNAL_CONFIG_TIMEDOMAINSET_H_
