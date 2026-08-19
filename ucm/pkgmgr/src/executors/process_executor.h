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
/// @file       process_executor.h
/// @brief      ProcessExecutor header
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/FsmManager
/// @module_path=/UCM/FsmManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001, SR_UCM_00026
/// @unit_name=ProcessExecutor
/// @unit_description=ProcessExecutor header
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_EXECUTORS_PROCESS_EXECUTOR_H_
#define ARA_UCM_PKGMGR_EXECUTORS_PROCESS_EXECUTOR_H_

#include "ara/ucm/pkgmgr/impl_type_transferidtype.h"
#include "base_executor.h"
#include "common/alias.h"
#include "util/interrupt_token.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief ProcessExecutor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10393
/// @trace_id_dd=DD_UCM_10950
/// @needwork = ad
/// @endcode
class ProcessExecutor : public BaseExecutor
{
public:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10951
    /// @needwork = dda
    /// @endcode
    ProcessExecutor() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10952
    /// @needwork = dda
    /// @endcode
    ~ProcessExecutor() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10953
    /// @needwork = dda
    /// @endcode
    ProcessExecutor(ProcessExecutor const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10954
    /// @needwork = dda
    /// @endcode
    ProcessExecutor& operator=(ProcessExecutor const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10955
    /// @needwork = dda
    /// @endcode
    ProcessExecutor(ProcessExecutor&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10956
    /// @needwork = dda
    /// @endcode
    ProcessExecutor& operator=(ProcessExecutor&& other) = delete;

    /// @brief Processing of a sofware package
    ///
    /// @param id Transfer ID of the package to be processed.
    ///
    /// @return The result of of processing this sw package, which specifies if the
    /// application has been successfully installed.
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00001, 9da6dc21e02d92b81d6f3b264ab6b54301c0811b}
    /// @uptrace={SWS_UCM_00161, d180f7488021029b3edfed4d88d943493aad40ee}
    /// @uptrace={SWS_UCM_00166, 7afbfdfdeb96cf8de75f151455606f7d868449de}
    /// @uptrace={SWS_UCM_00168, 161e1b696aa2e268b5342badbe3c8c7f0c4f369a}
    /// @uptrace={SWS_UCM_00169, 51597354432dd982fa90293244f74f17be25e9fe}
    /// @uptrace={SWS_UCM_00150, 3cd4c71afe3d8f9fbdf8eda5798b73fbcd72d205}
    /// @uptrace={SWS_UCM_00104, 2d7ce69b7131d0dcc222d3e04df8f325841ef784}
    /// @uptrace={SWS_UCM_00191, 08da5db248692475e39ef37f6a77bde718a241e8}
    /// @uptrace={SWS_UCM_00195, 0f55eb55b75c1e8abd39d075467d2d7e875fc6db}
    /// @uptrace={SWS_UCM_00196, 86beafc62a0e47d0f0b3ff1cf03e4d2e072f74d6}
    /// @uptrace={SWS_UCM_00165, 9bc3a9d37f8c90f33aa649144a3c0b672a374a0e}
    /// @uptrace={SWS_UCM_00218, a26d2b73cb1ff06625357db0e003569feba066b7}
    /// @uptrace={SWS_UCM_00219, cec914634d1568f8edeb468c231185f92a48d8fd}
    /// TODO: Implement retry strategies AR-115917
    /// @uptrace={SWS_VUCM_00297}
    /// TODO: Implement IncompatibleDelta
    /// @uptrace={SWS_UCM_00231}
    /// TODO: This is only implementable in streaming. For non-streaming, we extract already in
    /// TransferExit and no extraction buffer is needed within ProcessSwPackage therefore
    /// @uptrace={SWS_UCM_00217}
    /// TODO: Implement SoftwareClusterMissing
    /// @uptrace={SWS_UCM_00285}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10957
    /// @needwork = dda
    /// @endcode
    AraResultVoid ProcessSoftwarePackage(TransferIdType const& id) noexcept;

    /// @brief Clean up the temporary files
    /// @param id Transfer ID of the package to be processed.
    /// @param isRevert commit or revert
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10958
    /// @needwork = dda
    /// @endcode
    AraResultVoid Cancel(TransferIdType const& id, bool const isRevert = false) noexcept;
    /// @brief Clean up the temporary files
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10959
    /// @needwork = dda
    /// @endcode
    AraResultVoid CancelForRevert() noexcept;

    /// @brief Clean up the temporary files
    /// @param id Transfer ID of the package to be processed.
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10960
    /// @needwork = dda
    /// @endcode
    AraFutureVoid RunInThread(TransferIdType const& id) noexcept;

private:
    /// @brief The identifier of currently processed package
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10961
    /// @needwork = dda
    /// @endcode
    TransferIdType processedId_{};

    /// @brief A lock for a cancel while processing (thread is used for package processing)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10962
    /// @needwork = dda
    /// @endcode
    std::timed_mutex cancelledLock_{};
    /// @brief A token for a thread interruption (thread is used for package processing)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10963
    /// @needwork = dda
    /// @endcode
    InterruptToken cancelledToken_{};
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_EXECUTORS_PROCESS_EXECUTOR_H_
