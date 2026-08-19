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
/// @file       persistence_service.h
/// @brief      This file provides persistence_service functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @module_path=common
/// @interface_level=
/// @trace_id_sr=89658027-3194-4f45-ba8a-ac6ff8ff4591
/// @unit_name=PersistenceService
/// @unit_description=This file provides persistence_service functionality
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_PERSISTENCESERVICE_H_
#define ISOFT_UDS_SERVER_PERSISTENCESERVICE_H_
#include <cstdint>
#include <memory>

#include "isoft/uds/persistence_interface.h"
namespace isoft {
namespace uds {
namespace server {

// Define persistence interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00848
/// @needwork = dd
/// @endcode
class PersistenceService
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00849
    /// @needwork = dda
    /// @endcode
    PersistenceService(PersistenceService const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00850
    /// @needwork = dda
    /// @endcode
    PersistenceService& operator=(PersistenceService const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00851
    /// @needwork = dda
    /// @endcode
    PersistenceService(PersistenceService&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00852
    /// @needwork = dda
    /// @endcode
    PersistenceService& operator=(PersistenceService&&) = delete;

public:
    /// @brief Constructor
    /// @param serverAddr Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01979
    /// @needwork = dd
    /// @endcode
    explicit PersistenceService(std::uint16_t serverAddr) noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01980
    /// @needwork = dd
    /// @endcode
    ~PersistenceService() = default;

    /// @brief Register persistence interface
    /// @param persistencePtr Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01981
    /// @needwork = dd
    /// @endcode
    void Register(std::shared_ptr< PersistenceInterface > const& persistencePtr) noexcept;

    /// @brief Get persistence interface instance
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01982
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< PersistenceInterface >& GetPersistenceInstance() noexcept;

    // Save data
    /// @brief Save data
    /// @param std Parameter
    /// @param std Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01983
    /// @needwork = dd
    /// @endcode
    bool SaveData(const std::string& key, const std::vector< std::uint8_t >& value) noexcept;

    /// @brief Load data
    /// @param std Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01984
    /// @needwork = dd
    /// @endcode
    std::vector< std::uint8_t > LoadData(const std::string& key) const noexcept;

    /// @brief Remove data
    /// @param std Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01985
    /// @needwork = dd
    /// @endcode
    bool RemoveData(const std::string& key) noexcept;

    /// @brief List all keys
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01986
    /// @needwork = dd
    /// @endcode
    std::vector< std::string > ListKeys() const noexcept;

private:
    /// @brief Persistence interface instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01987
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< PersistenceInterface > persistencePtr_;
    /// @brief Server address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01988
    /// @needwork = dd
    /// @endcode
    std::uint16_t serverAddr_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_PERSISTENCESERVICE_H_