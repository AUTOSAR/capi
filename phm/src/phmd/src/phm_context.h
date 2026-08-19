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
/// @file       phm_context.h
/// @brief      the manager class of PHMD.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/PhmContext
/// @unit_description=the manager class of PHMD.
/// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
/// @unit_name=PhmContext
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_PHM_CONTEXT_H_
#define ARA_PHM_INTERNAL_PHM_CONTEXT_H_

#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>

#include <cassert>
#include <memory>

#include "ara/phm/internal/config_manager.h"
#include "ara/phm/internal/hcmanager/health_channel_controller.h"
#include "ara/phm/internal/svmanager/supervision_controller.h"
#include "ara/phm/internal/watchdog_interface.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief PHM context，the manager class of PHMD, manage the resource of PHMD.
/// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
/// @needwork = ad
class PhmContext final
{
public:
    /// @brief Creation of PhmContext, unique singleton.
    /// @return unique ptr of PhmContext.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    static std::unique_ptr< PhmContext > GetInstanceUnique()
    {
        static std::unique_ptr< PhmContext > s_Instance{nullptr};
        static std::once_flag s_Flag{};
        std::call_once(s_Flag, []() { s_Instance.reset(new PhmContext()); });
        std::ignore = s_Flag;

        /// Exclusive singleton, only allowed to be obtained once
        assert(s_Instance);
        return std::move(s_Instance);
    }

    /// @brief Destructor for PhmContext.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    ~PhmContext() = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    PhmContext(PhmContext &obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    PhmContext &operator=(PhmContext &obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    PhmContext(PhmContext &&obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    PhmContext &operator=(PhmContext &&obj) = delete;

    /// @brief Init PhmContext, create and init other module such as
    /// SupervisionManager、HealthChannelManager.
    /// @return 0, success；< 0, fail.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    int32_t Init() noexcept;

    /// @brief Start PhmContext, the event loop run.
    /// @return 0, success; < 0, fail
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    int32_t Run() noexcept;

    /// @brief Stop PhmContext, stop the event loop.
    /// @return 0, success; < 0, fail
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    int32_t Stop() const noexcept;

    /// @brief Destroy the resource of PhmContext.
    /// @return 0, success;< 0, fail
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    int32_t Destroy() noexcept;

private:
    /// @brief Constructor of PhmContext.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    PhmContext() = default;

    /// @brief Phmcontext set this function to SupervisionManager and
    /// HealthChannelManager, SupervisionManager and HealthChannelManager call it
    /// to trigger the watchdog.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    void _triggerWatchdog() const;

    /// @brief start watchdog.
    /// @param watchdogConf
    /// @return 0 success; < 0 failed.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    int32_t _startWatchdog(WatchdogConf const &watchdogConf) noexcept;

private:
    /// @brief event loop.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    std::shared_ptr< isoft::naicpp::EvLoop > evLoopPtr_;

    /// @brief config manager.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    std::shared_ptr< ConfigManager > configManager_;

    /// @brief supervision manager.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    std::unique_ptr< SupervisionController > supervisionController_;

    /// @brief health channel manager.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    std::unique_ptr< HealthChannelController > healthChannelController_;

    /// @brief watchdog.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    std::unique_ptr< WatchdogInterface > watchdogInterface_;

    /// @brief conf of watchdog.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    WatchdogConf watchdogConf_;

    /// @brief The timer driving PHM to feed watchdog.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    std::unique_ptr< ara::phm::internal::Timer > feedWatchdogTimer_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_PHM_CONTEXT_H_
