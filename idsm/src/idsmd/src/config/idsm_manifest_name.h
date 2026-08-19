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
/// @file       idsm_manifest_name.h
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
/// @unit_description=Configuration file attribute name pool
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_EVENT_SINK_H_
#define ARA_IDSM_EVENT_SINK_H_
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>

#include "ara/core/map.h"
#include "ara/core/string.h"
#include "ara/core/string_view.h"

namespace ara {
namespace idsm {
/// @brief Configuration file attribute name pool
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00196
/// @trace_id_dd=DD_IDSM_00456
/// @needwork = ad
/// @endcode
class ManifestNamePool final
{
public:
    /// @brief Get the domain name in the configuration file
    /// @param name Identifier of the domain name
    /// @return Domain name
    /// @throws  Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00457
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetFieldByName(ara::core::String const& name);
    /// @brief Get the unique class object in singleton mode
    /// @return Class object
    /// @throws Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00458
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< ManifestNamePool > GetInstance();

private:
    /// @brief Initialization function
    /// @throws Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00459
    /// @needwork = dda
    /// @endcode
    void _init();

public:
    /// @brief Parameterless constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00460
    /// @needwork = dda
    /// @endcode
    ManifestNamePool() = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00461
    /// @needwork = dda
    /// @endcode
    ~ManifestNamePool() = default;

private:
    /// @brief Copy constructor
    /// @param m Object to be copied
    /// @throws Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00462
    /// @needwork = dda
    /// @endcode
    ManifestNamePool(ManifestNamePool const& m) = default;
    /// @brief Move constructor
    /// @param m Object to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00463
    /// @needwork = dda
    /// @endcode
    ManifestNamePool(ManifestNamePool&& m) = default;
    /// @brief Copy assignment operator
    /// @param m Object to be copied
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00464
    /// @needwork = dda
    /// @endcode
    ManifestNamePool& operator=(ManifestNamePool const& m) = default;
    /// @brief Move assignment operator
    /// @param m Object to be moved
    /// @return Left operand of the assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00465
    /// @needwork = dda
    /// @endcode
    ManifestNamePool& operator=(ManifestNamePool&& m) = default;

private:
    /// @brief Singleton thread-safe mutex lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00466
    /// @needwork = dda
    /// @endcode
    static std::mutex s_SingleMutex_;
    /// @brief Unique instance of the event pool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00467
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< ManifestNamePool > s_SingleInstance_;
    /// @brief Mapping from domain name identifier to domain name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00468
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, ara::core::StringView > fieldMap_;
};

}  // namespace idsm
}  // namespace ara

#endif