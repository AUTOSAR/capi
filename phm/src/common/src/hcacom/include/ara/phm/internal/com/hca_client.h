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
/// @file       hca_client.h
/// @brief      used by HealthChannelManager to send request to SM when health channel status need recover.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/HealthChannelActionCom
/// @unit_description=used by HealthChannelManager to send request to SM when health channel status need recover.
/// @trace_id_sr=SR_PHM_01028
/// @unit_name=Client
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_HCACOM_CLIENT_H_
#define ARA_PHM_INTERNAL_HCACOM_CLIENT_H_

#include <memory>

#include "ara/phm/internal/com/base_client.h"
#include "ara/phm/internal/com/hca_message.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {
namespace hcacom {

/// @brief Base client inherited by Client.
/// @trace_id_sr=SR_PHM_01028
/// @needwork = no
using BaseClient = ara::phm::internal::com::common::BaseClient;

/// @brief Param for function Client::Notify.
/// @trace_id_sr=SR_PHM_01028
/// @needwork = ad
struct NotifyInfo
{
    /// @brief id of health status.
    HealthStatus status;

    /// @brief health channel action instance specifier.
    ara::core::String identifier;

    /// @brief timeout.
    int32_t waitResponseTimeOutMs;

    /// @brief max retry times.
    int32_t maxRetryTimes;

    /// @brief have retried times.
    int32_t haveRetriedTimes;
};

/// @brief Client of hcacom.
/// @trace_id_sr=SR_PHM_01028
/// @needwork = ad
class Client : public BaseClient
{
public:
    /// @brief Creation of Client, unique singleton.
    /// @param recoveryResultHandler Function called when recovery failed.
    /// @return unique ptr of Client
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    static std::unique_ptr< Client > GetInstanceUnique(
        std::function< void(RecoveryResult result, ara::core::String instance) > const recoveryResultHandler)
    {
        static std::unique_ptr< Client > s_Instance{nullptr};
        static std::once_flag s_Flag{};
        std::call_once(s_Flag, [&recoveryResultHandler]() { s_Instance.reset(new Client(recoveryResultHandler)); });
        std::ignore = s_Flag;

        /// Exclusive singleton, only allowed to be obtained once
        assert(s_Instance);
        return std::move(s_Instance);
    }

    /// @brief Default deconstructor
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    ~Client() noexcept override = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    Client(Client& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    Client& operator=(Client const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    Client(Client&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    Client& operator=(Client const&& obj) = delete;

    /// @brief Send data.
    /// @param notifyInfo data to send.
    /// @return 0 success；other fail.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    int32_t Notify(std::shared_ptr< NotifyInfo > const& notifyInfo) noexcept;

private:
    /// @brief Creation of a Client.
    /// @param recoveryResultHandler Function called to process recovery result.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    explicit Client(
        std::function< void(RecoveryResult result, ara::core::String instance) > recoveryResultHandler) noexcept
        : BaseClient{GetIpcPhmHcaCmClient()}
        , recoveryResultHandler_{std::move(recoveryResultHandler)}
        , notifyInfo_{}
        , messageSerializer_{} {};

    /// @brief the async callback of ipc SendAsync.
    /// @throws QAC
    /// @param context context set by user.
    /// @param status status of ipc SendAsync.
    /// @param responsePacket the response packet of ipc SendAsync.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    void _responseCallBack(void* const context,
                           isoft::ipc::IPCClientHandlerStatus const status,
                           isoft::ipc::IPCPacket* const responsePacket);

    /// @brief Process the recovery result.
    /// @param result result of recovery.
    /// @param notifyInfo the notify info from user.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    void _processRecoveryResult(RecoveryResult const result, NotifyInfo const* const notifyInfo) noexcept;

private:
    /// @brief The user callback to process the recovery result.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    std::function< void(RecoveryResult result, ara::core::String instance) > recoveryResultHandler_;

    ///@brief <identifier of HealthChannelAction, notify info>
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< NotifyInfo > > notifyInfo_;

    /// @brief message serializer.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    MessageSerializer messageSerializer_;
};  // class Client

}  // namespace hcacom
}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_HCACOM_CLIENT_H_
