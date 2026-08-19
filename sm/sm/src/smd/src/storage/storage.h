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
/// @file       storage.h
/// @brief      A class represents a persistency storage (key-value database)
/// @details
/// @date       2024-06-26
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Storage
/// @unit_name=Storage
/// @interface_level=module
/// @unit_description=A class represents a persistency storage (key-value database)
/// @trace_id_sr=SR_SM_00002, SR_SM_04004
/// @endcode
///
/// ================================================================

#ifndef STORAGE_H_
#define STORAGE_H_

#include <ara/per/key_value_storage.h>

#include "define.h"

namespace ara {
namespace sm {
namespace storage {
/// @brief Define alias to resolve qac2428 item: Direct use of character type.
/// @param Char8_t alias of char
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using Char8_t = char;

/// @brief A class represents a persistency storage (key-value database) that is used to store and get the calibration data.
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00002, SR_SM_04004
/// @trace_id_ad=AD_SM_08104
/// @trace_id_dd=DD_SM_08389
/// @needwork = ad
/// @endcode
class Storage final
{
public:
    /// @brief Creates a persistency KVS with a given model identifier
    /// @param modelIdentifier Identifier used to access KVS
    /// @return
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00002, SR_SM_04004
    /// @trace_id_ad=AD_SM_00479
    /// @trace_id_dd=DD_SM_00495
    /// @needwork = ad
    /// @endcode
    explicit Storage(core::StringView const &modelIdentifier) noexcept;

    /// @brief Disable default construction
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00002, SR_SM_04004
    /// @trace_id_ad=AD_SM_00480
    /// @trace_id_dd=DD_SM_00496
    /// @needwork = ad
    /// @endcode
    Storage() = delete;

    /// @brief Open the persistence library
    /// @return  true, success; false, failure;
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00002, SR_SM_04004
    /// @trace_id_ad=AD_SM_00481
    /// @trace_id_dd=DD_SM_00497
    /// @needwork = ad
    /// @endcode
    bool Open() noexcept;

    /// @brief Store
    /// @param key Key
    /// @param value Value
    /// @return  true, success; false, failure;
    /// @code{.isoft}
    /// @tparam ValueType Value type
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00002, SR_SM_04004
    /// @trace_id_ad=AD_SM_00482
    /// @trace_id_dd=DD_SM_00498
    /// @needwork = ad
    /// @endcode
    template < typename ValueType >
    bool Store(core::StringView const &key, ValueType const &value) noexcept;

    /// @brief Get
    /// @param key Key
    /// @return Stored value
    /// @code{.isoft}
    /// @tparam ValueType Value type
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00002, SR_SM_04004
    /// @trace_id_ad=AD_SM_00483
    /// @trace_id_dd=DD_SM_00499
    /// @needwork = ad
    /// @endcode
    template < typename ValueType >
    core::Optional< ValueType > Get(core::StringView const &key) noexcept;

    /// @brief Delete
    /// @param key Key
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00002, SR_SM_04004
    /// @trace_id_ad=AD_SM_00484
    /// @trace_id_dd=DD_SM_00500
    /// @needwork = ad
    /// @endcode
    void Remove(core::StringView const &key) noexcept;

private:
    /// @brief Instance descriptor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00002, SR_SM_04004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08390
    /// @needwork = dda
    /// @endcode
    core::InstanceSpecifier kvsInstanceSpecifier_;

    /// @brief A shared handle to access key-value storage
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00002, SR_SM_04004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08391
    /// @needwork = dda
    /// @endcode
    ara::per::SharedHandle< ara::per::KeyValueStorage > kvs_;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00002, SR_SM_04004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08392
    /// @needwork = dda
    /// @endcode
    log::Logger &log_;
};

}  // namespace storage
}  // namespace sm
}  // namespace ara

#endif  // STORAGE_H_
