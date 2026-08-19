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
/// @file       ids_proto_common.h
/// @brief      Byte order conversion related functions
/// @details
/// @date       2023-01-13
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/IDS protocol
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0011
/// @unit_description=Byte order conversion related functions
/// @endcode
///
/// ================================================================

#ifndef ARA_IDS_PROTO_COMMON_H_
#define ARA_IDS_PROTO_COMMON_H_
#include <iostream>

#include "ara/core/vector.h"
namespace ara {
namespace idsm {
/// @brief Determine endianness
/// @return Endianness of the system
/// @code{.isoft}
/// @unit_name=isBigEndian
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00114
/// @trace_id_dd=DD_IDSM_00181
/// @needwork = dd
/// @endcode
bool IsBigEndian() noexcept;
/// @brief Convert host byte order to network byte order
/// @param hostll Value to be converted
/// @return Network byte order value
/// @exception Stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @unit_name=htonll
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00114
/// @trace_id_dd=DD_IDSM_00182
/// @needwork = dd
/// @endcode
uint64_t Htonll(uint64_t const hostll);
/// @brief Convert network byte order to host byte order
/// @param netll Network byte order value
/// @return Host byte order value
/// @exception Stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @unit_name=ntohll
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00126
/// @trace_id_dd=DD_IDSM_00183
/// @needwork = dd
/// @endcode
uint64_t Ntohll(uint64_t const netll);
/// @brief Byte order conversion
/// @param hostll Byte order to be converted
/// @return Converted byte order
/// @exception Stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @unit_name=byte_order_swith
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00126
/// @trace_id_dd=DD_IDSM_00184
/// @needwork = dd
/// @endcode
uint64_t ByteOrderSwith(uint64_t hostll);
/// @brief IDS protocol custom enumeration values
/// @code{.isoft}
/// @unit_name=ProtoFlag
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00107
/// @trace_id_dd=DD_IDSM_00185
/// @needwork = ad
/// @endcode
enum class ProtoFlag : uint8_t
{
    kContextData = 0x1U,
    kTimestamp   = 0x2U,
    kSignature   = 0x4U
};
}  // namespace idsm
}  // namespace ara
#endif
