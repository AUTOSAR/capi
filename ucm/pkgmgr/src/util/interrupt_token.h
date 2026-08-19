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
/// @file       interrupt_token.h
/// @brief      token class for interrupting thread execution
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/Utils
/// @module_path=/UCM/Utils
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=InterruptToken
/// @unit_description=token class for interrupting thread execution
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_INTERRUPTIBLE_THREAD_INTERRUPT_TOKEN_H
#define ARA_UCM_PKGMGR_INTERRUPTIBLE_THREAD_INTERRUPT_TOKEN_H

#include <atomic>
#include <memory>

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief A class represents a token which is used to
/// interrupt thread execution
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10320
/// @trace_id_dd=DD_UCM_10732
/// @needwork = ad
/// @endcode
class InterruptToken
{
public:
    /// @brief Creates a token with a given initial state
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10733
    /// @needwork = dda
    /// @endcode
    explicit InterruptToken()
        : cancelMark_{std::make_shared< std::atomic< CancelState > >(CancelState::kNO_CANCEL_REQUESTED)}
    {
    }

    /// @brief MarkToBeCancelled
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10734
    /// @needwork = dda
    /// @endcode
    void MarkToBeCancelled() noexcept
    {
        cancelMark_->store(CancelState::kTO_BE_CANCELLED);
        placeHolder_ = 0;
    }

    /// @brief IsMarkedToBeCancelled
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10735
    /// @needwork = dda
    /// @endcode
    bool IsMarkedToBeCancelled() const noexcept { return *cancelMark_ == CancelState::kTO_BE_CANCELLED; }

    /// @brief MarkAsCancelled
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10736
    /// @needwork = dda
    /// @endcode
    void MarkAsCancelled() noexcept
    {
        cancelMark_->store(CancelState::kCANCEL_DONE);
        placeHolder_ = 0;
    }

    /// @brief IsMarkedAsCancelled
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10737
    /// @needwork = dda
    /// @endcode
    bool IsMarkedAsCancelled() noexcept { return *cancelMark_ == CancelState::kCANCEL_DONE; }

    /// @brief MarkAsNoCancellRequested
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10738
    /// @needwork = dda
    /// @endcode
    void MarkAsNoCancellRequested() noexcept
    {
        cancelMark_->store(CancelState::kNO_CANCEL_REQUESTED);
        placeHolder_ = 0;
    }

private:
    /// @brief CancelState
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10320
    /// @trace_id_dd=DD_UCM_10739
    /// @needwork = dd
    /// @endcode
    enum class CancelState : uint8_t
    {
        kNO_CANCEL_REQUESTED,
        kTO_BE_CANCELLED,
        kCANCEL_DONE
    };

    /// @brief flag to be raised if Cancel is called. Consumed in Process call
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10740
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< std::atomic< CancelState > > cancelMark_;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10741
    /// @needwork = dda
    /// @endcode
    int placeHolder_{0};
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_INTERRUPTIBLE_THREAD_INTERRUPT_TOKEN_H
