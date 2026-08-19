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
/// @file       idsm_manifest_name.cpp
/// @brief      Configuration file attribute name pool
/// @details
/// @date       2024-09-18
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/config center
/// @interface_level=unit
/// @trace_id_sr=SRS_IDSM_0006
/// @unit_name=ManifestNamePool
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_manifest_name.h"

#include <mutex>
namespace ara {
namespace idsm {
/// @brief Unique instance of the event pool
std::shared_ptr< ManifestNamePool > ManifestNamePool::s_SingleInstance_{nullptr};  // NOLINT
/// @brief Singleton thread-safe mutex lock
std::mutex ManifestNamePool::s_SingleMutex_{};
/// @brief Get the unique class object in singleton mode
/// @return Class object
/// @throws Stack overflow exception
std::shared_ptr< ManifestNamePool > ManifestNamePool::GetInstance()
{
    if (s_SingleInstance_ == nullptr) {
        std::unique_lock< std::mutex > const lock{s_SingleMutex_};
        if (s_SingleInstance_ == nullptr) {
            s_SingleInstance_.reset(new ManifestNamePool);
            s_SingleInstance_->_init();
        }
    }
    return s_SingleInstance_;
}
/// @brief Initialization function
/// @throws Stack overflow exception
void ManifestNamePool::_init()
{
    /// @brief Establish mapping relationship of configuration item attribute names
    using MapPair = std::pair< ara::core::String, ara::core::StringView >;
    std::ignore   = fieldMap_.insert(MapPair{"TIME_INTERVAL", "timeInterval"});
    std::ignore   = fieldMap_.insert(MapPair{"SHORT_NAME", "shortName"});
    std::ignore   = fieldMap_.insert(MapPair{"FQN", "fqn"});
    std::ignore   = fieldMap_.insert(MapPair{"ID", "id"});
    std::ignore   = fieldMap_.insert(MapPair{"DID", "did"});
    std::ignore   = fieldMap_.insert(MapPair{"SECURITY_EVENT_CONTEXT_PROPS_FQN", "securityEventContextPropsFqn"});
    std::ignore   = fieldMap_.insert(MapPair{"EVENT_PORT_INSTANCE", "eventPortInstance"});
    std::ignore   = fieldMap_.insert(MapPair{"FILTER_CHAIN", "filterChain"});
    std::ignore   = fieldMap_.insert(MapPair{"MAPPED_SECURITY_EVENT", "mappedSecurityEvent"});
}
/// @brief Get the domain name in the configuration file
/// @param name Identifier of the domain name
/// @return Domain name
/// @throws Stack overflow exception
ara::core::StringView ManifestNamePool::GetFieldByName(ara::core::String const& name)
{
    ara::core::Map< ara::core::String, ara::core::StringView >::iterator const itFind{fieldMap_.find(name)};
    if (itFind != fieldMap_.end()) {
        return fieldMap_.at(name);
    }
    return {""};
}

}  // namespace idsm
}  // namespace ara
