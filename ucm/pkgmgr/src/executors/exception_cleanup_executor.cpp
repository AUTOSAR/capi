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
/// @file       exception_cleanup_executor.cpp
/// @brief      ExceptionCleanupExecutor implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/FsmManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=ExceptionCleanupExecutor
/// @unit_description=ExceptionCleanupExecutor implementation
/// @endcode
///
/// ================================================================

#include "exception_cleanup_executor.h"

#include "common/errc.h"
#include "common/log.h"
#include "fsm/fsm_manager.h"
#include "help_executor.h"
#include "util/future_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Clean up the temporary files
///
/// TODO: Implementation is missing.
/// @uptrace={SWS_UCM_00158}
/// @return future with result
AraFutureVoid ExceptionCleanupExecutor::RunInThread() const noexcept
{
    LOGD << "call...";
    AraString const func{__func__};

    ASYNC_CALL_VOID_BEGIN_WITH_FUNC(func)
    {
        AraResultVoid const ret{_doExceptionCleaningUp()};
        if (!ret.HasValue()) {
            LOGE << func.c_str() << "_doExceptionCleaningUp failed, call OnFailure()";
            std::ignore = owner_->OnFailure();
        } else {
            LOGD << func.c_str() << "_doExceptionCleaningUp done, call Done()";
            owner_->Done();
        }

        return ret;
    }
    ASYNC_CALL_END();
}

/// @brief Clean up the temporary files
/// @throws no
/// @return result
AraResultVoid ExceptionCleanupExecutor::_doExceptionCleaningUp() const noexcept
{
    LOGD << "call...";

    // kExceptionCleaningUp
    //   8. After the restart is complete, it will enter the kExceptionCleaningUp state to continue execution;
    //   9. Check the directory where the platform_core software set is located according to boot_option_verX.cfg, delete other versions, and set the software set status to Present accordingly
    //  10. Check all software sets under /ara/swcls/ according to application_swcl_list_verY.json:
    //            Check if the software set exists in application_swcl_list_verY.json
    //                 If exists
    //                    Delete other versions, and set the software set status to Present accordingly
    //                 If does not exist
    //                    Delete the directory where the software set is located
    //  11. StopUpdateSession()
    //  12. Switch the UCM state machine to IDLE;

    // 9. Check the directory where the platform_core software set is located according to boot_option_verX.cfg, delete other versions, and set the software set status to Present accordingly
    LOGD << "call BootOptionFileExceptionCleanupB";
    bool ret{owner_->mABPartition.BootOptionFileExceptionCleanupB()};
    if (!ret) {
        LOGD << "call BootOptionFileExceptionCleanupB failed, todo...";
    }

    //  10. Check all software sets under /ara/swcls/ according to application_swcl_list_verY.json:
    //            Check if the software set exists in application_swcl_list_verY.json
    //                 If exists
    //                    Delete other versions, and set the software set status to Present accordingly
    //                 If does not exist
    //                    Delete the directory where the software set is located
    LOGD << "call SWCLListFileExceptionCleanupB";
    ret = owner_->mABPartition.SwclListFileExceptionCleanupB();
    if (!ret) {
        LOGD << "call SWCLListFileExceptionCleanupB failed, todo...";
    }

    //  11. StopUpdateSession()
    // Notify SM: StopUpdateSession()
    LOGD << "call StopUpdateSession";
    // TODO:
    // If calling StopUpdateSession() fails, how to know and handle??? Its FireAndForget is true, so it cannot have a return value???
    owner_->pSM->StopUpdateSession();

    LOGD << "end";
    return {};
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
