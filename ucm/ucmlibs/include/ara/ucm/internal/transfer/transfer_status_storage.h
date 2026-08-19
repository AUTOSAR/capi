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
/// @file       transfer_status_storage.h
/// @brief      The TransferStatusStorage class definition which is used to store and get the transfer status.
/// @details
/// @date       2022-06-13
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
/// @unit_name=TransferStatusStorage
/// @unit_description=TransferStatusStorage definition provided for UCM
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_TRANSFER_TRANSFER_STATUS_STORAGE_H_
#define ARA_UCM_PKGMGR_TRANSFER_TRANSFER_STATUS_STORAGE_H_

#include "ara/core/array.h"
#include "ara/ucm/internal/extraction/alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief A class represents a storage
/// that is used to store and get the transfer status.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00003
/// @trace_id_dd=DD_UCM_00196
/// @needwork = dd
/// @endcode
class TransferStatusStorage
{
public:
    /// @brief Stored status type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00197
    /// @needwork = dda
    /// @endcode
    static constexpr std::size_t kValueTypeSize{4U};
    /// @brief Stored status type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00208,DD_UCM_00209
    /// @needwork = dda
    /// @endcode
    using value_type = ara::core::Array< std::uint64_t, kValueTypeSize >;  // NOLINT

    /// @brief Stored SwPackageInfo type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00198
    /// @needwork = dda
    /// @endcode
    class SwPackageInfoValueType
    {
    public:
        /// @brief swName
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_00003
        /// @trace_id_dd=DD_UCM_00199
        /// @needwork = dda
        /// @endcode
        AraString swName;  // Software set/software package name
        /// @brief version
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_00003
        /// @trace_id_dd=DD_UCM_00200
        /// @needwork = dda
        /// @endcode
        AraString version;  // Software set version

        /// @brief IsEnumerableTag
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_
        /// @trace_id_dd=DD_UCM_
        /// @needwork = no
        /// @endcode
        using IsEnumerableTag = void;
        /// @brief enumerate
        /// @param fun
        /// @throws no
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_00003
        /// @trace_id_dd=DD_UCM_00201
        /// @needwork = dda
        /// @endcode
        template < typename F >
        void enumerate(F& fun)  // NOLINT
        {
            fun(swName);
            fun(version);
        }
    };

    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00202
    /// @needwork = dda
    /// @endcode
    TransferStatusStorage() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00203
    /// @needwork = dda
    /// @endcode
    virtual ~TransferStatusStorage() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00204
    /// @needwork = dda
    /// @endcode
    TransferStatusStorage(TransferStatusStorage const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00205
    /// @needwork = dda
    /// @endcode
    TransferStatusStorage& operator=(TransferStatusStorage const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00206
    /// @needwork = dda
    /// @endcode
    TransferStatusStorage(TransferStatusStorage&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00207
    /// @needwork = dda
    /// @endcode
    TransferStatusStorage& operator=(TransferStatusStorage&& other) = delete;

    /// @brief Stores transfer status with the given key
    /// @param key Associated Key
    /// @param status Transfer Status to store
    /// @return void
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00208
    /// @needwork = dda
    /// @endcode
    virtual void StoreStatus(AraStringView const& key, value_type const& status) = 0;

    /// @brief Retrieves stored state with given key
    /// @param key Associated Key
    /// @return Optional Transfer Status
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00209
    /// @needwork = dda
    /// @endcode
    virtual AraOptional< value_type > GetStatus(AraStringView const& key) = 0;

    /// @brief Removes stored status from the storage by a given key
    /// @param key Associated key
    /// @return void
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00210
    /// @needwork = dda
    /// @endcode
    virtual void RemoveStatus(AraStringView const& key) = 0;

    /// @brief Stores software package info with the given key
    /// @param key Associated Key
    /// @param packageInfo software package info to store
    /// @return void
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00211
    /// @needwork = dda
    /// @endcode
    virtual void StoreSwPackageInfo(AraStringView const& key, SwPackageInfoValueType const& packageInfo) = 0;

    /// @brief Retrieves stored software package info with given key
    /// @param key Associated Key
    /// @return Optional software package info
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00212
    /// @needwork = dda
    /// @endcode
    virtual AraOptional< SwPackageInfoValueType > GetSwPackageInfo(AraStringView const& key) = 0;

    /// @brief Removes stored software package info from the storage by a given key
    /// @param key Associated key
    /// @return void
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00213
    /// @needwork = dda
    /// @endcode
    virtual void RemoveSwPackageInfo(AraStringView const& key) = 0;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_TRANSFER_TRANSFER_STATUS_STORAGE_H_
