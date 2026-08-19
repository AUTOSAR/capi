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
/// @file       hc_message.h
/// @brief      defines the communication protocol.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/HealthChannelCom
/// @unit_description=defines the communication protocol.
/// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
/// @unit_name=Message
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_HCCOM_MESSAGE_H_
#define ARA_PHM_INTERNAL_HCCOM_MESSAGE_H_

#include <ara/core/result.h>
#include <ara/core/string_view.h>
#include <isoft/serialize/serialize.h>

#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {
namespace hccom {

/// @brief returns client addr of hccom.
/// @return client addr of hccom.
/// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
/// @needwork = ad
inline ara::core::String GetIpcPhmHcCmClient() noexcept { return ara::core::String{"phmd/hccm"}; }

/// @brief returns server addr of hccom.
/// @return server addr of hccom.
/// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
/// @needwork = ad
inline ara::core::String GetIpcPhmHcCmServer() noexcept { return ara::core::String{"hccm"}; }

/// @brief defines the communication meassage of hccom
/// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
/// @needwork = ad
struct Message final
{
    /// @brief represents a HealthChannel
    ara::core::String instance;

    /// @brief id of health status
    HealthStatus healthStatus{0};

    /// @brief id of process who send message
    ProcessId processId{0};

    /// @brief time stamp
    int64_t timestamp{0};
};

/// @brief Serialize and Deserialize Message
/// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
/// @needwork = ad
class MessageSerializer final
{
public:
    /// @brief Constructor of MessageSerializer
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    MessageSerializer() = default;

    /// @brief Destructor of MessageSerializer
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    ~MessageSerializer() = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    MessageSerializer(MessageSerializer& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    MessageSerializer& operator=(MessageSerializer const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    MessageSerializer(MessageSerializer&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    MessageSerializer& operator=(MessageSerializer const&& obj) = delete;

    /// @brief standard usage of serialization and deserialization
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = no
    using IsEnumerableTag = void;

    /// @brief standard usage of serialization and deserialization
    /// @tparam F
    /// @param fun
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = no
    template < typename F >
    void Enumerate(F& fun) noexcept
    {
        fun(msg_.instance);
        fun(msg_.healthStatus);
        fun(msg_.processId);
        fun(msg_.timestamp);
    }

    /// @brief serialization.
    /// @return data after serialization.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    Chunk Serialize() const noexcept;

    /// @brief deserialization.
    /// @param chunk msg after deserialization.
    /// @return 0 success;other fail.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    int32_t Deserialize(Chunk& chunk) noexcept;

    /// @brief set msg.
    /// @param msg msg.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    void SetMsg(Message const& msg) noexcept;

    /// @brief Returns the msg.
    /// @return msg.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    Message GetMsg() const noexcept;

private:
    /// @brief msg.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = dda
    Message msg_;
};

}  // namespace hccom
}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_HCCOM_MESSAGE_H_