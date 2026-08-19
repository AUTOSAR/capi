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
/// @file       hca_message.h
/// @brief      defines the communication protocol.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/HealthChannelActionCom
/// @unit_description=defines communication protocol.
/// @trace_id_sr=SR_PHM_01028
/// @unit_name=Message
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef _COM_HCACOM_ARA_PHM_INTERNAL_HCACOM_MESSAGE_H_
#define _COM_HCACOM_ARA_PHM_INTERNAL_HCACOM_MESSAGE_H_

#include <ara/core/string_view.h>
#include <isoft/serialize/serialize.h>

#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {
namespace hcacom {

/// @brief returns client addr of hcacom
/// @return client addr of hcacom
/// @trace_id_sr=SR_PHM_01028
/// @needwork = no
inline ara::core::String GetIpcPhmHcaCmClient() noexcept { return ara::core::String{"smd/hcar"}; }

/// @brief returns server addr of hcacom
/// @return server addr of hcacom
/// @trace_id_sr=SR_PHM_01028
/// @needwork = no
inline ara::core::String GetIpcPhmHcaCmServer() noexcept { return ara::core::String{"hcar"}; }

/// @brief defines the communication message of hcacom
/// @trace_id_sr=SR_PHM_01028
/// @needwork = ad
struct Message final
{
    /// @brief model identifier.
    ara::core::String identifier;

    /// @brief id of health status.
    HealthStatus healthStatus;

    /// @brief time stamp.
    int64_t timestamp{0};
};

/// @brief recovery result.
/// @trace_id_sr=SR_PHM_01028
/// @needwork = ad
enum class RecoveryResult : uint32_t
{
    kSuccess    = 0,  ///< success.
    kNotOffered = 1,  ///< not offered
    kFail       = 2   ///< fail.
};

/// @brief Serialize and Deserialize Message.
/// @trace_id_sr=SR_PHM_01028
/// @needwork = ad
class MessageSerializer final
{
public:
    /// @brief Default constructor.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    MessageSerializer() = default;

    /// @brief Default deconstructor.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    ~MessageSerializer() = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    MessageSerializer(MessageSerializer& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    MessageSerializer& operator=(MessageSerializer const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    MessageSerializer(MessageSerializer&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    MessageSerializer& operator=(MessageSerializer const&& obj) = delete;

    /// @brief standard usage of serialization and deserialization
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = no
    using IsEnumerableTag = void;

    /// @brief standard usage of serialization and deserialization
    /// @tparam F
    /// @param fun
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = no
    template < typename F >
    void Enumerate(F& fun) noexcept
    {
        fun(msg_.identifier);
        fun(msg_.healthStatus);
        fun(msg_.timestamp);
    }

    /// @brief serialization
    /// @return data after serialization
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    Chunk Serialize() const noexcept;

    /// @brief deserialization
    /// @param chunk msg after deserialization
    /// @return 0 success; other fail.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    int32_t Deserialize(Chunk& chunk) noexcept;

    /// @brief set msg.
    /// @param msg msg
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    void SetMsg(Message const& msg) noexcept;

    /// @brief Returns the msg.
    /// @return msg.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    Message GetMsg() const noexcept;

private:
    ///@brief message.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    Message msg_;
};

}  // namespace hcacom
}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // _COM_HCACOM_ARA_PHM_INTERNAL_HCACOM_MESSAGE_H_