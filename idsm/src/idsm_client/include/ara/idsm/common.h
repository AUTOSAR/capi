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
/// @brief      IDSM data definition class
/// @details
/// @date       2023-01-17
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/lib client
/// @export_level=/idsm/lib client
/// @interface_level=module
/// @trace_id_sr=SRS_IDSM_0005
/// @unit_description=IDSM data definition class
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_COMMON_H_
#define ARA_IDSM_COMMON_H_

#include <cstdint>
#include <vector>

#include "ara/core/vector.h"

namespace ara {
namespace idsm {

/// @brief [SWS_AIDSM_10201] ContextDataType used for sending context data to the IdsM.
/// @code{.isoft}
/// @unit_name=ContextDataType
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00022
/// @trace_id_dd=DD_IDSM_00055
/// @needwork = ad
/// @endcode
using ContextDataType = std::vector< std::uint8_t >;

/// @brief [[SWS_AIDSM_10202] Only 62 least-significant bits are used as timestamp value and stored or transmitted,
/// respectively
/// @code{.isoft}
/// @unit_name=TimestampType
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00023
/// @trace_id_dd=DD_IDSM_00056
/// @needwork = ad
/// @endcode
using TimestampType = std::uint64_t;

/// @brief [SWS_AIDSM_10203] CountType used for setting optional count for events pre-qualified by sensors.
/// @code{.isoft}
/// @unit_name=CountType
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00024
/// @trace_id_dd=DD_IDSM_00057
/// @needwork = ad
/// @endcode
using CountType   = std::uint16_t;
using EventIdType = std::uint16_t;

/// @brief Byte array data type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00025
/// @trace_id_dd=DD_IDSM_00058
/// @needwork = ad
/// @endcode
using BytesVec = ara::core::Vector< std::uint8_t >;

}  // namespace idsm
}  // namespace ara
#endif