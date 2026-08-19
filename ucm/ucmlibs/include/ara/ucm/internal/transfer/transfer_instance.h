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
/// @file       transfer_instance.h
/// @brief      The TransferInstance class definition which composes a file from vectors of bytes according to initially passed size.
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
/// @trace_id_sr=SR_UCM_00013, SR_UCM_00025
/// @unit_name=TransferInstance
/// @unit_description=TransferInstance definition provided for UCM
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_TRANSFER_TRANSFER_INSTANCE_H_
#define ARA_UCM_PKGMGR_TRANSFER_TRANSFER_INSTANCE_H_

#include <memory>

#include "ara/core/instance_specifier.h"
#include "streamable.h"
#include "transfer_status_storage.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief This class composes a file from vectors of bytes according to initially passed size.
/// It also check errors
///
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00003
/// @trace_id_dd=DD_UCM_00236
/// @needwork = dd
/// @endcode
class TransferInstance : public Streamable
{
public:
    /// @brief constructor
    /// @param id
    /// @param path
    /// @param storage
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00237
    /// @needwork = dda
    /// @endcode
    TransferInstance(TransferIdType const id, AraString const& path, std::unique_ptr< TransferStatusStorage > storage);

    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00238
    /// @needwork = dda
    /// @endcode
    TransferInstance() = delete;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00239
    /// @needwork = dda
    /// @endcode
    ~TransferInstance() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00240
    /// @needwork = dda
    /// @endcode
    TransferInstance(TransferInstance const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00241
    /// @needwork = dda
    /// @endcode
    TransferInstance& operator=(TransferInstance const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00242
    /// @needwork = dda
    /// @endcode
    TransferInstance(TransferInstance&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00243
    /// @needwork = dda
    /// @endcode
    TransferInstance& operator=(TransferInstance&& other) = delete;

    /// @brief TransferStart
    /// @param size Size of the transfer
    /// @return TransferStartReturnType
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00244
    /// @needwork = dda
    /// @endcode
    TransferStartReturnType TransferStart(std::uint64_t size) override;

    /// @brief TransferData
    /// @param data Data to transfer
    /// @param blockCounter Block counter
    /// @return TransferDataReturnType
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00245
    /// @needwork = dda
    /// @endcode
    TransferDataReturnType TransferData(ByteVectorType const& data, std::uint64_t const& blockCounter) override;

    /// @brief TransferExit
    /// @return TransferExitReturnType
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00246
    /// @needwork = dda
    /// @endcode
    TransferExitReturnType TransferExit() override;

    /// @brief GetPackageFilename
    /// @return Package Filename
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00247
    /// @needwork = dda
    /// @endcode
    AraString GetPackageFilename() const override;

    /// @brief DeleteTransfer
    /// @return DeleteTransferReturnType
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00248
    /// @needwork = dda
    /// @endcode
    DeleteTransferReturnType DeleteTransfer() override;

    // added by hanzhibo
    /// @brief IsTransferring
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00249
    /// @needwork = dda
    /// @endcode
    bool IsTransferring() const noexcept override;

    // added by hanzhibo
    /// @brief SetState
    /// @param state State to set
    /// @throws no
    /// @return void
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00250
    /// @needwork = dda
    /// @endcode
    void SetState(SwPackageStateType state) override;

    // added by hanzhibo
    /// @brief GetState
    /// @return SwPackageStateType
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00251
    /// @needwork = dda
    /// @endcode
    SwPackageStateType const& GetState() const noexcept override;

    // added by hanzhibo
    /// @brief SetSwPackageInfo
    /// @param swName Software package name
    /// @param version Software package version
    /// @throws no
    /// @return void
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00252
    /// @needwork = dda
    /// @endcode
    void SetSwPackageInfo(AraString const& swName, AraString const& version) override;

    // added by hanzhibo
    /// @brief GetSwPackageInfo
    /// @param swName Software package name
    /// @param version Software package version
    /// @return SwPackageInfo
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00253
    /// @needwork = dda
    /// @endcode
    void GetSwPackageInfo(AraString& swName, AraString& version) const override;

    /// @brief GetReceivedBytes
    /// @return Received Bytes
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00254
    /// @needwork = dda
    /// @endcode
    std::uint64_t GetReceivedBytes() const noexcept override;

    /// @brief GetReceivedBlocks
    /// @return Received Blocks
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00255
    /// @needwork = dda
    /// @endcode
    std::uint64_t GetReceivedBlocks() const noexcept override;

private:
    /// @brief _initPath
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00256
    /// @needwork = dda
    /// @endcode
    void _initPath();

    /// @brief _setState
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00257
    /// @needwork = dda
    /// @endcode
    void _setState() const;

private:
    /// @brief transferId_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00258
    /// @needwork = dda
    /// @endcode
    TransferIdType transferId_;
    /// @brief receivedBytes_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00259
    /// @needwork = dda
    /// @endcode
    std::uint64_t receivedBytes_;
    /// @brief transferDirectory_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00260
    /// @needwork = dda
    /// @endcode
    AraString transferDirectory_;
    /// @brief expectedBytes_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00261
    /// @needwork = dda
    /// @endcode
    std::uint64_t expectedBytes_;
    /// @brief state_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00262
    /// @needwork = dda
    /// @endcode
    SwPackageStateType state_;
    /// @brief expectedBlock_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00263
    /// @needwork = dda
    /// @endcode
    std::uint64_t expectedBlock_;
    /// @brief fd_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00264
    /// @needwork = dda
    /// @endcode
    std::int32_t fd_;
    /// @brief key_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00265
    /// @needwork = dda
    /// @endcode
    AraString key_;
    /// @brief swPackageInfoKey_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00266
    /// @needwork = dda
    /// @endcode
    AraString swPackageInfoKey_;  // Key used to store software package information
    /// @brief swName_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00267
    /// @needwork = dda
    /// @endcode
    AraString swName_;  // Software set/software package name
    /// @brief version_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00268
    /// @needwork = dda
    /// @endcode
    AraString version_;  // Software set version
    /// @brief statusStorage_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00269
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< TransferStatusStorage > statusStorage_;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_TRANSFER_TRANSFER_INSTANCE_H_
