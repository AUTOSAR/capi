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
/// @file       serial_id_generator.h
/// @brief      Definition of the sequential transfer ID generator class
/// @details
/// @date       2023-10-26
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/UCMLib
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00025
/// @unit_name=SerialIDGenerator
/// @unit_description=Definition of the sequential transfer ID generator class
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_SERIAL_ID_GENERATOR_H_
#define ARA_UCM_PKGMGR_SERIAL_ID_GENERATOR_H_

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <limits>
#include <type_traits>

#include "helper.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Serial transfer id generator class.
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00004
/// @trace_id_dd=DD_UCM_00299
/// @needwork = ad
/// @endcode
template < typename Id >
class SerialIDGenerator
{
public:
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using ResultType = Id;

    // static_assert(std::is_unsigned<Id>::value, "Unsigned arithmetic type required as Id type for SerialIDGenerator.");

    /// @brief Generate a new ID based on all existing ones.
    /// @param ids Array of existing ids.
    /// @return New key or nothing in case of reaching the limit.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00004
    /// @trace_id_dd=DD_UCM_00300
    /// @needwork = dda
    /// @endcode
    template < class Container, std::enable_if_t< std::is_same< typename Container::value_type, Id >::value, int > = 0 >
    AraOptional< Id > operator()(Container const& ids)
    {
        std::ignore = ids;
        // for (Id i = 0; i < std::numeric_limits<Id>::max(); ++i) {
        //     Id idVal = i + 1;
        //     if (std::find(std::begin(ids), std::end(ids), idVal) == std::end(ids)) return {idVal};
        // }

        return {};
    }

    /// @brief Generate a new ID based on all existing ones for Map-like associative containers.
    /// @param ids Array of existing ids.
    /// @return New key or nothing in case of reaching the limit.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00004
    /// @trace_id_dd=DD_UCM_00301
    /// @needwork = dda
    /// @endcode
    template < class Container,
               std::enable_if_t< std::is_same< typename Container::key_type, Id >::value
                                     && std::is_same< typename Container::value_type::first_type, const Id >::value
                                     && std::is_same< typename Container::key_compare, std::less< Id > >::value,
                                 int > = 0 >
    AraOptional< Id > operator()(Container const& ids)
    {
        Id idVal{0};

        // Find unused ones from ids
        for (auto it = std::begin(ids); it != std::end(ids); ++it) {
            if (it->first != idVal) {
                return {idVal};
            }
            TransferIdAddOne(idVal);
        }

        // Whether all are used up
        std::size_t size{ids.size()};
        constexpr int16_t kNUM_8{8};
        std::size_t maxSize{static_cast< std::size_t >(std::pow(2, idVal.size() * kNUM_8))};
        if (size < maxSize) {
            return {idVal};
        }
        return {};
    }

    /// @brief Increment the transfer ID by one.
    /// @param id The transfer ID to increment.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00004
    /// @trace_id_dd=DD_UCM_00302
    /// @needwork = dda
    /// @endcode
    void TransferIdAddOne(TransferIdType& id)
    {
        int carry = 1;  // Value to add on.
        constexpr int16_t kNUM_8{8};
        for (int64_t i = static_cast< int64_t >(id.size() - 1); i >= 0; i--) {
            carry += id[i];
            id[i] = static_cast< std::uint8_t >(carry);
            carry >>= kNUM_8;
            if (0 == carry) {
                return;
            }
        }
    }
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_SERIAL_ID_GENERATOR_H_
