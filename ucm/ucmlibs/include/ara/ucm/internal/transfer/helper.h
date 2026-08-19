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
/// @file       helper.h
/// @brief      This file contains a collection of common helper methods that are used in different parts of the code for convenience.
/// @details
/// @date       2023-10-26
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/UCMLib
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00025
/// @unit_name=helper
/// @unit_description=This file contains a collection of common helper methods that are used in different parts
/// of the code for convenience.
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_TRANSFER_HELPER_H_
#define ARA_UCM_PKGMGR_TRANSFER_HELPER_H_

#include "ara/ucm/internal/extraction/alias.h"
#include "ara/ucm/internal/types/impl_type_transferidtype.h"

namespace ara {
namespace ucm {
namespace pkgmgr {
namespace helper {

/// @brief convert TransferId to hex str
/// @param id TransferIdType value
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
inline AraString ToHexString(TransferIdType const& id)
{
    std::vector< Char_T > const hexmap{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    std::size_t const len{id.size()};
    std::size_t i{0U};
    std::size_t d{0U};
    std::size_t const twoU{2U};
    std::size_t const high{0xF0U};
    std::size_t const fourU{4U};
    std::size_t const low{0x0FU};

    AraString ret(len * twoU, ' ');

    for (std::int32_t end{static_cast< std::int32_t >(len) - 1}; end >= 0; --end) {
        i                  = static_cast< std::size_t >(end);
        d                  = static_cast< std::size_t >(id[i]);
        ret[twoU * i]      = hexmap[(d & high) >> fourU];
        ret[twoU * i + 1U] = hexmap[(d & low)];
    }

    return ret;
}

/// @brief get TransferId from hex str
/// @param hex AraString value
/// @throws no
/// @return TransferIdType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
inline TransferIdType HexStrToTransferId(AraString const& hex)
{
    TransferIdType ret{};
    std::size_t const len{hex.size()};
    std::size_t i{0U};
    std::size_t const twoU{2U};
    std::int32_t const step{2};
    std::int32_t const hexNumber{16};

    assert(ret.size() * twoU == len);

    for (std::int32_t end{static_cast< std::int32_t >(len) - step}; end >= 0; end -= step) {
        i = static_cast< std::size_t >(end);
        AraString const byteStr{hex.substr(i, twoU)};
        ret[i / twoU] = static_cast< std::uint8_t >(strtol(byteStr.c_str(), nullptr, hexNumber));
    }

    return ret;
}

}  // namespace helper
}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_TRANSFER_HELPER_H_
