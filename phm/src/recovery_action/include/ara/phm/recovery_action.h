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
/// @file       recovery_action.h
/// @brief      action when to recover from the abnormal supervision status.
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/RecoveryAction
/// @unit_description=action when to recover from the abnormal supervision status.
/// @trace_id_sr=SR_PHM_01027
/// @unit_name=RecoveryAction
/// @interface_level=software
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_RECOVERY_ACTION_H_
#define ARA_PHM_RECOVERY_ACTION_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/exec/execution_error_event.h>
#include <ara/phm/supervised_entity.h>

#include <memory>

#include "ara/phm/internal/com/ra_server.h"
#include "ara/phm/internal/com/raext_client.h"

namespace ara {
namespace phm {

/// @brief The type of base supervision
/// @trace_id_sr=SR_PHM_01027
/// @needwork = ad
enum class TypeOfSupervision : uint32_t
{
    /// @brief Supervision is of type AliveSupervision.
    AliveSupervision = 0,

    /// @brief Supervision is of type DeadlineSupervision.
    DeadlineSupervision = 1,

    /// @brief Supervision is of type LogicalSupervision.
    LogicalSupervision = 2
};

/// @brief Provide a virtual function RecoveryHandler which is implemented by SM,
/// called when the supervision need recover
/// @trace_id_sr=SR_PHM_01027
/// @needwork = ad
class RecoveryAction
{
public:
    /// @brief Constructor.
    /// @param instance instance specifier to the PPortPrototype of a Phm RecoveryActionInterface.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    explicit RecoveryAction(const ara::core::InstanceSpecifier &instance);

    /// @brief Destructor for RecoveryAction.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    virtual ~RecoveryAction() noexcept;

    /// @brief The copy constructor shall not be used.
    /// @param other The object to be copied.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    RecoveryAction(const RecoveryAction &other) = delete;

    /// @brief The copy assignment operator shall not be used.
    /// @param other The object to be copied.
    /// @return object.
    /// @return RecoveryAction& The copied object.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    RecoveryAction &operator=(const RecoveryAction &other) = delete;

    /// @brief Move constructor.
    /// @param ra The object to be moved.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    RecoveryAction(RecoveryAction &&ra) noexcept;

    /// @brief  Move assignment operator.
    /// @param ra The object to be moved.
    /// @return RecoveryAction& The moved object.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    RecoveryAction &operator=(RecoveryAction &&ra) &noexcept;

    /// @brief RecoveryHandler to be invoked by PHM.
    ///        The handler invocation needs to be enabled before by a call of RecoveryAction::Offer.
    /// @param executionError Information on detected error, shall give further information for error recovery.
    /// @param supervision The type of elementary supervision which failed.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    virtual void RecoveryHandler(ara::exec::ExecutionErrorEvent const &executionError, TypeOfSupervision supervision)
        = 0;

    /// @brief Returns the status of global supervision that the supervised entity
    /// belongs to.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    ara::core::Result< GlobalSupervisionStatus > GetGlobalSupervisionStatus() const;

    /// @brief Enables potential invocations of RecoveryHandler.
    /// @return ara::core::Result<void> A Result, being either empty or containing any of the errors defined below.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    ara::core::Result< void > Offer();

    /// @brief Disables invocations of RecoveryHandler.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    void StopOffer();

private:
    /// @brief instance specifier of this recovery action.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = dda
    ara::core::InstanceSpecifier instance_;

    /// @brief the ipc server run in SM.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = dda
    std::shared_ptr< ara::phm::internal::racom::Server< TypeOfSupervision > > actionServer_;

    /// @brief communication client for GetGlobalSupervisionStatus/Offer/StopOffer.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = dda
    std::shared_ptr< ara::phm::internal::raextcom::Client > communicationClient_;
};

}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_RECOVERY_ACTION_H_
