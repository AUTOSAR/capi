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
/// @file       streamable.h
/// @brief      The Streamable interface which is for object passed in parts.
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
/// @unit_name=Streamable
/// @unit_description=The Streamable interface which is for object passed in parts.
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_TRANSFER_STREAMABLE_H_
#define ARA_UCM_PKGMGR_TRANSFER_STREAMABLE_H_

#include <cstdint>

#include "ara/ucm/internal/extraction/alias.h"
#include "ara/ucm/internal/types/impl_type_swpackagestatetype.h"
#include "receive_types.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief DeleteTransferReturnType
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00003
/// @trace_id_dd=DD_UCM_00225
/// @needwork = no
/// @endcode
using DeleteTransferReturnType = ::ara::ucm::pkgmgr::GeneralResponseType;
constexpr uint8_t kNUM_255{0xff};
/// @brief Interface for object passed in parts
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00007, c9e41bf68059e0421604c79e27feced11f961119}
/// @uptrace={SWS_UCM_00088, 6fde704d91b8e40ced3cb4f3f78b3659d274fee3}
/// @uptrace={SWS_UCM_00140, 4e28804e0e7c549a4d37463254ca2afe0e3c70d2}
/// @uptrace={SWS_UCM_00008, d4f76d1688bffafd81411bb5ba6b3e9cb6ce2985}
/// @uptrace={SWS_UCM_00010, a1bea3f583c3ef00e3c12792db807ec836d80f78}
/// @uptrace={SWS_UCM_00145, 19b7bde3be9348f8f5ae9aab907e7ae367284030}
/// @uptrace={SWS_UCM_00087, 1464c3180cffb347c7f488cb0fde516d3c913480}
/// @uptrace={SWS_UCM_00148, 288f0a63d9c148ae2134413849f740cf9bf3bcb6}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00003
/// @trace_id_dd=DD_UCM_00214
/// @needwork = ad
/// @endcode
class Streamable
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00215
    /// @needwork = dda
    /// @endcode
    Streamable() = default;

    // Rule of five, cause we need virtual dtor for interface class
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00216
    /// @needwork = dda
    /// @endcode
    virtual ~Streamable() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00217
    /// @needwork = dda
    /// @endcode
    Streamable(Streamable const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00218
    /// @needwork = dda
    /// @endcode
    Streamable& operator=(Streamable const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00219
    /// @needwork = dda
    /// @endcode
    Streamable(Streamable&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00220
    /// @needwork = dda
    /// @endcode
    Streamable& operator=(Streamable&& other) = delete;

    /// @brief Start the transfer of a Software Package.
    /// Transfer Id for subsequent calls to TransferData will be generated
    /// and returned as a part of TransferStartOutput.
    ///
    /// @param size Size (in bytes) of the Software Package to be transferred.
    ///
    /// @return The struct which contains the result of the transfer start operation
    /// and transfer id generated for this operation.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00221
    /// @needwork = dda
    /// @endcode
    virtual TransferStartReturnType TransferStart(std::uint64_t size) = 0;

    /// @brief Block-wise transfer of a Software Package.
    ///
    /// @param data Data block of the Software Package.
    /// @param blockCounter Block counter of the current block.
    ///
    /// @return The result of transferring current data block, which specifies if the
    /// sw package has been successfully transferred.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00222
    /// @needwork = dda
    /// @endcode
    virtual TransferDataReturnType TransferData(ByteVectorType const& data, std::uint64_t const& blockCounter) = 0;

    /// @brief Finish the transfer of a Software Package.
    ///
    /// @return The result of finishing this sw package transfer, which specifies if the
    /// sw package has been successfully transferred.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00223
    /// @needwork = dda
    /// @endcode
    virtual TransferExitReturnType TransferExit() = 0;

    /// @brief GetPackageFilename
    /// @return Package Filename
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00224
    /// @needwork = dda
    /// @endcode
    virtual AraString GetPackageFilename() const = 0;

    /// @brief Delete a Software Package.
    ///
    /// @return The result of deleting sw package, which specifies if the
    /// sw package has been successfully deleted.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00225
    /// @needwork = dda
    /// @endcode
    virtual DeleteTransferReturnType DeleteTransfer() = 0;

    /// @brief Set the process progress value.
    /// @param value The new progress value to set.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00226
    /// @needwork = dda
    /// @endcode
    virtual void SetProcessProgressValue(std::uint8_t value) noexcept
    {
        // prevent setting of values larger than max allowed value
        constexpr std::uint8_t kMax{100U};
        if (value <= kMax) {
            processProgressValue_ = value;
        }
        ///processProgressValue_ = (value > 100U) ? processProgressValue_ : value;
    }

    /// @brief Get the process progress value.
    /// @return The current process progress value.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00227
    /// @needwork = dda
    /// @endcode
    virtual std::uint8_t GetProcessProgressValue() const noexcept { return processProgressValue_; }

    // added by hanzhibo
    /// @brief Checks if it is transferring now.
    ///
    /// @returns True if it is transferring now, false otherwise.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00228
    /// @needwork = dda
    /// @endcode
    virtual bool IsTransferring() const = 0;

    /// @brief set new state to this object
    ///
    /// @param state new state to set
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00229
    /// @needwork = dda
    /// @endcode
    virtual void SetState(SwPackageStateType state) = 0;

    /// @brief get state of the package (e.g. kTransferring, kProcessed)
    ///
    /// @returns state of the object
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00230
    /// @needwork = dda
    /// @endcode
    virtual SwPackageStateType const& GetState() const = 0;

    /// @brief set software package info to this object
    ///
    /// @param swName
    /// @param version
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00231
    /// @needwork = dda
    /// @endcode
    virtual void SetSwPackageInfo(AraString const& swName, AraString const& version) = 0;

    /// @brief get info of the package (e.g. swName, version)
    ///
    /// @param swName
    /// @param version
    /// @returns info of the package
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00232
    /// @needwork = dda
    /// @endcode
    virtual void GetSwPackageInfo(AraString& swName, AraString& version) const = 0;

    /// @brief Get the transferred bytes of a Software Package.
    ///
    /// @return The number of transferred bytes of a Software Package.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00233
    /// @needwork = dda
    /// @endcode
    virtual std::uint64_t GetReceivedBytes() const = 0;

    /// @brief Get the transferred blocks of a Software Package.
    ///
    /// @return The number of transferred blocks of a Software Package.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00234
    /// @needwork = dda
    /// @endcode
    virtual std::uint64_t GetReceivedBlocks() const = 0;

private:
    // the default value 0xff is used as "no information available"
    /// @brief processProgressValue_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00003
    /// @trace_id_dd=DD_UCM_00235
    /// @needwork = dda
    /// @endcode
    std::uint8_t processProgressValue_{kNUM_255};
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_TRANSFER_STREAMABLE_H_
