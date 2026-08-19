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
/// @file       per_transfer_status_storage.h
/// @brief      The PerTransferStatusStorage class which is used to store and get the transfer status.
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
/// @unit_name=PerTransferStatusStorage
/// @unit_description=The PerTransferStatusStorage class which is used to store and get the transfer status.
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_TRANSFER_PER_TRANSFER_STATUS_STORAGE_H_
#define ARA_UCM_PKGMGR_TRANSFER_PER_TRANSFER_STATUS_STORAGE_H_

#include "ara/per/key_value_storage.h"
#include "transfer_status_storage.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief A class represents a persistency storage (key-value database)
/// that is used to store and get the transfer status.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00003
/// @trace_id_dd=DD_UCM_00277
/// @needwork = dd
/// @endcode
class PerTransferStatusStorage : public TransferStatusStorage
{
public:
    /// @brief Constructor.
    /// @param modelIdentifier Identifier used to access KVS.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_00278
    /// @needwork = dda
    /// @endcode
    explicit PerTransferStatusStorage(AraStringView const modelIdentifier);

    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_00279
    /// @needwork = dda
    /// @endcode
    ~PerTransferStatusStorage() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_00280
    /// @needwork = dda
    /// @endcode
    PerTransferStatusStorage(PerTransferStatusStorage const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_00281
    /// @needwork = dda
    /// @endcode
    PerTransferStatusStorage& operator=(PerTransferStatusStorage const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_00282
    /// @needwork = dda
    /// @endcode
    PerTransferStatusStorage(PerTransferStatusStorage&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_00283
    /// @needwork = dda
    /// @endcode
    PerTransferStatusStorage& operator=(PerTransferStatusStorage&& other) = delete;

    /// @brief Store transfer status with the given key.
    /// @param key Associated Key.
    /// @param status Transfer Status to store.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_00284
    /// @needwork = dda
    /// @endcode
    void StoreStatus(AraStringView const& key, value_type const& status) override;

    /// @brief Retrieve stored state with given key.
    /// @param key Associated Key.
    /// @return Optional Transfer Status.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_00285
    /// @needwork = dda
    /// @endcode
    AraOptional< value_type > GetStatus(AraStringView const& key) override;

    /// @brief Remove stored status from the storage by a given key.
    /// @param key Associated key.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_00286
    /// @needwork = dda
    /// @endcode
    void RemoveStatus(AraStringView const& key) override;

    /// @brief Store software package info with the given key.
    /// @param key Associated Key.
    /// @param packageInfo Software package info to store.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_00287
    /// @needwork = dda
    /// @endcode
    void StoreSwPackageInfo(AraStringView const& key, SwPackageInfoValueType const& packageInfo) override;

    /// @brief Retrieve stored software package info with given key.
    /// @param key Associated Key.
    /// @return Optional software package info.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_00288
    /// @needwork = dda
    /// @endcode
    AraOptional< SwPackageInfoValueType > GetSwPackageInfo(AraStringView const& key) override;

    /// @brief Remove stored software package info from the storage by a given key.
    /// @param key Associated key.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_00289
    /// @needwork = dda
    /// @endcode
    void RemoveSwPackageInfo(AraStringView const& key) override;

private:
    /// @brief A shared handle to access key-value storage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_00290
    /// @needwork = dda
    /// @endcode
    ara::per::SharedHandle< ara::per::KeyValueStorage > kvs_;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_TRANSFER_PER_TRANSFER_STATUS_STORAGE_H_
