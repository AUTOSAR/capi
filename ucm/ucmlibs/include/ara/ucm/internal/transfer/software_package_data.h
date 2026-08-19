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
/// @file       software_package_data.h
/// @brief      The SoftwarePackageData class which stores data about software package in work.
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
/// @trace_id_sr=SR_UCM_00010, SR_UCM_00025, SR_UCM_00013
/// @unit_name=SoftwarePackageData
/// @unit_description=The SoftwarePackageData class which stores data about software package in work.
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_TRANSFER_SOFTWARE_PACKAGE_DATA_H_
#define ARA_UCM_PKGMGR_TRANSFER_SOFTWARE_PACKAGE_DATA_H_

#include <mutex>

#include "ara/ucm/internal/types/impl_type_transferidtype.h"
#include "streamable_software_package.h"
#include "transfer_status_storage.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Class for storing data about software package in work
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00003
/// @trace_id_dd=DD_UCM_00172
/// @needwork = ad
/// @endcode
class SoftwarePackageData : public StreamableSoftwarePackage
{
public:
    /// @brief Constructor.
    /// @param id The transfer ID.
    /// @param path The path to the software package.
    /// @param storage The storage for transfer status.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00173
    /// @needwork = dda
    /// @endcode
    SoftwarePackageData(TransferIdType const& id, AraString const& path, std::unique_ptr< TransferStatusStorage >);

    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00174
    /// @needwork = dda
    /// @endcode
    ~SoftwarePackageData() override = default;

    /// @brief default copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00175
    /// @needwork = dda
    /// @endcode
    SoftwarePackageData(SoftwarePackageData const& other) = delete;
    /// @brief default copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00176
    /// @needwork = dda
    /// @endcode
    SoftwarePackageData& operator=(SoftwarePackageData const& other) = delete;
    /// @brief default move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00177
    /// @needwork = dda
    /// @endcode
    SoftwarePackageData(SoftwarePackageData&& other) = delete;
    /// @brief default move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00178
    /// @needwork = dda
    /// @endcode
    SoftwarePackageData& operator=(SoftwarePackageData&& other) = delete;

    /// @brief Get the transfer ID.
    /// @return The transfer ID.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00179
    /// @needwork = dda
    /// @endcode
    TransferIdType const& GetID() const noexcept override { return transferID_; }

    /// @brief Set the state of the software package.
    /// @param state The new state to set.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00180
    /// @needwork = dda
    /// @endcode
    void SetState(SwPackageStateType const state) override
    {
        std::lock_guard< std::mutex > const guard{access_};
        _getStreamable()->SetState(state);
    }

    /// @brief Get the state of the software package.
    /// @return The current state of the software package.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00181
    /// @needwork = dda
    /// @endcode
    SwPackageStateType const& GetState() const override
    {
        std::lock_guard< std::mutex > const guard{access_};
        return _getStreamable()->GetState();
    }

    /// @brief Set the software package information.
    /// @param swName The name of the software package.
    /// @param version The version of the software package.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00182
    /// @needwork = dda
    /// @endcode
    void SetSwPackageInfo(AraString const& swName, AraString const& version) override
    {
        std::lock_guard< std::mutex > const guard{access_};
        return _getStreamable()->SetSwPackageInfo(swName, version);
    }

    /// @brief Get the software package information.
    /// @param swName The name of the software package.
    /// @param version The version of the software package.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00183
    /// @needwork = dda
    /// @endcode
    void GetSwPackageInfo(AraString& swName, AraString& version) const override
    {
        std::lock_guard< std::mutex > const guard{access_};
        return _getStreamable()->GetSwPackageInfo(swName, version);
    }

    /// @brief Start the transfer of the software package.
    /// @param size The size of the software package.
    /// @return The result of the transfer start operation.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00184
    /// @needwork = dda
    /// @endcode
    TransferStartReturnType TransferStart(std::uint64_t const size) override
    {
        std::lock_guard< std::mutex > const guard{access_};
        return _getStreamable()->TransferStart(size);
    }

    /// @brief Transfer data block of the software package.
    /// @param data The data block to transfer.
    /// @param blockCounter The block counter of the current block.
    /// @return The result of transferring the current data block.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00185
    /// @needwork = dda
    /// @endcode
    TransferDataReturnType TransferData(ByteVectorType const& data, std::uint64_t const& blockCounter) override
    {
        std::lock_guard< std::mutex > const guard{access_};
        return _getStreamable()->TransferData(data, blockCounter);
    }

    /// @brief Finish the transfer of the software package.
    /// @return The result of finishing the transfer.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00186
    /// @needwork = dda
    /// @endcode
    TransferExitReturnType TransferExit() override
    {
        std::lock_guard< std::mutex > const guard{access_};
        return _getStreamable()->TransferExit();
    }

    /// @brief Get the filename of the software package.
    /// @return The filename of the software package.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00187
    /// @needwork = dda
    /// @endcode
    AraString GetPackageFilename() const override { return _getStreamable()->GetPackageFilename(); }

    /// @brief Delete the transfer of the software package.
    /// @return The result of deleting the transfer.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00188
    /// @needwork = dda
    /// @endcode
    DeleteTransferReturnType DeleteTransfer() override
    {
        std::lock_guard< std::mutex > const guard{access_};
        return _getStreamable()->DeleteTransfer();
    }

    // added by hanzhibo
    /// @brief Check if the software package is transferring.
    /// @return True if the software package is transferring, false otherwise.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00189
    /// @needwork = dda
    /// @endcode
    bool IsTransferring() const override { return _getStreamable()->IsTransferring(); }

    /// @brief Get the number of received bytes.
    /// @return The number of received bytes.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00190
    /// @needwork = dda
    /// @endcode
    std::uint64_t GetReceivedBytes() const override { return _getStreamable()->GetReceivedBytes(); }

    /// @brief Get the number of received blocks.
    /// @return The number of received blocks.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00191
    /// @needwork = dda
    /// @endcode
    std::uint64_t GetReceivedBlocks() const override { return _getStreamable()->GetReceivedBlocks(); }

private:
    /// @brief _getStreamable
    /// @return unique_ptr of Streamable
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00192
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< Streamable > const& _getStreamable() const noexcept { return transfer_; }

private:
    /// @brief Id of the object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00193
    /// @needwork = dda
    /// @endcode
    TransferIdType transferID_;

    /// @brief part of package responsible for receiving transferring data
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00194
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< Streamable > transfer_;

    /// @brief protect transfer_ and state_ from simultaneous access
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00195
    /// @needwork = dda
    /// @endcode
    mutable std::mutex access_;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_TRANSFER_SOFTWARE_PACKAGE_DATA_H_
