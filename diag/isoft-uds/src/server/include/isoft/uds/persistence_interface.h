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
/// @file       persistence_interface.h
/// @brief      This file provides persistence_interface functionality
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
/// @trace_id_sr=143b9c6e-c43d-45bc-a589-6f52531697a3
/// @unit_name=PersistenceInterface
/// @unit_description=This file provides persistence_interface functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_PERSISTENCEINTERFACE_H_
#define ISOFT_UDS_SERVER_PERSISTENCEINTERFACE_H_
#include <cstdint>
#include <string>
#include <vector>
namespace isoft {
namespace uds {
namespace server {

/// @brief Define persistence interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01292
/// @trace_id_dd=DD_UDS_02584
/// @needwork = ad
/// @endcode
class PersistenceInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00863
    /// @needwork = dda
    /// @endcode
    PersistenceInterface(PersistenceInterface const&) = default;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00864
    /// @needwork = dda
    /// @endcode
    PersistenceInterface(PersistenceInterface&&) = default;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00865
    /// @needwork = dda
    /// @endcode
    PersistenceInterface& operator=(PersistenceInterface const&) = default;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00866
    /// @needwork = dda
    /// @endcode
    PersistenceInterface& operator=(PersistenceInterface&&) = default;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00892
    /// @trace_id_dd=DD_UDS_02006
    /// @needwork = ad
    /// @endcode
    PersistenceInterface() = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00893
    /// @trace_id_dd=DD_UDS_02007
    /// @needwork = ad
    /// @endcode
    virtual ~PersistenceInterface() = default;

    /// @brief Save data
    /// @param key Key value
    /// @param value Data
    /// @return true: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00894
    /// @trace_id_dd=DD_UDS_02008
    /// @needwork = ad
    /// @endcode
    virtual bool SaveData(const std::string& key, const std::vector< std::uint8_t >& value) = 0;

    /// @brief Load data
    /// @param key Key value
    /// @return Data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00895
    /// @trace_id_dd=DD_UDS_02009
    /// @needwork = ad
    /// @endcode
    virtual std::vector< std::uint8_t > LoadData(const std::string& key) const = 0;

    //
    /// @brief Delete data
    /// @param key Key value
    /// @return true: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00896
    /// @trace_id_dd=DD_UDS_02010
    /// @needwork = ad
    /// @endcode
    virtual bool RemoveData(const std::string& key) = 0;

    /// @brief Check if key exists
    /// @param key Key value
    /// @return true: Corresponding key exists
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00897
    /// @trace_id_dd=DD_UDS_02011
    /// @needwork = ad
    /// @endcode
    virtual bool KeyExists(std::string const& key) = 0;

    /// @brief Synchronize data
    /// @return true: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00898
    /// @trace_id_dd=DD_UDS_02012
    /// @needwork = ad
    /// @endcode
    virtual bool SyncData() { return true; }

    /// @brief List all keys
    /// @return List of all key values
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00899
    /// @trace_id_dd=DD_UDS_02013
    /// @needwork = ad
    /// @endcode
    virtual std::vector< std::string > ListKeys() const = 0;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_PERSISTENCEINTERFACE_H_