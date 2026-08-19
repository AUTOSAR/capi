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
/// @file       ra_message.h
/// @brief      defines the communication protocol.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/RecoveryActionCom
/// @unit_description=defines communication protocol.
/// @trace_id_sr=SR_PHM_01027
/// @unit_name=Message
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_RACOM_MESSAGE_H_
#define ARA_PHM_INTERNAL_RACOM_MESSAGE_H_

#include <ara/core/result.h>
#include <ara/core/string_view.h>
#include <ara/exec/execution_error_event.h>
#include <isoft/serialize/serialize.h>

#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {
namespace racom {

/// @brief returns client addr of racom.
/// @return client addr of racom.
/// @trace_id_sr=SR_PHM_01027
/// @needwork = ad
inline ara::core::String GetIpcPhmRaCmClient() noexcept { return ara::core::String{"smd/racm"}; }
/// @brief returns server addr of racom.
/// @return server addr of racom.
/// @trace_id_sr=SR_PHM_01027
/// @needwork = ad
inline ara::core::String GetIpcPhmRaCmServer() noexcept { return ara::core::String{"racm"}; }

/// @brief defines the communication message of racom.
/// @trace_id_sr=SR_PHM_01027
/// @needwork = ad
struct Message final
{
    /// @brief identifier of RecoveryAction
    ara::core::String identifier;

    /// @brief name of function group
    ara::core::String functionGroup;

    /// @brief execution error code
    uint32_t executionError{0U};

    /// @brief type of base supervision
    uint32_t supervisionType{0U};

    /// @brief time stamp
    int64_t timestamp{0};
};

/// @brief recovery result.
/// @trace_id_sr=SR_PHM_01027
/// @needwork = ad
enum class RecoveryResult : uint32_t
{
    kSuccess    = 0,  ///< success.
    kNotOffered = 1,  ///< not offered
    kFail       = 2   ///< fail.
};

/// @brief Serialize and Deserialize Message
/// @trace_id_sr=SR_PHM_01027
/// @needwork = ad
class MessageSerializer final
{
public:
    /// @brief Constructor of MessageSerializer
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    MessageSerializer() = default;

    /// @brief Default deconstructor.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    ~MessageSerializer() = default;

    /// @brief The copy constructor for MessageSerializer shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    MessageSerializer(MessageSerializer& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    MessageSerializer& operator=(MessageSerializer const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    MessageSerializer(MessageSerializer&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    MessageSerializer& operator=(MessageSerializer const&& obj) = delete;

    /// @brief standard usage of serialization and deserialization
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = no
    using IsEnumerableTag = void;

    /// @brief standard usage of serialization and deserialization
    /// @tparam F
    /// @param fun
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = no
    template < typename F >
    void Enumerate(F& fun) noexcept
    {
        fun(msg_.identifier);
        fun(msg_.functionGroup);
        fun(msg_.executionError);
        fun(msg_.supervisionType);
        fun(msg_.timestamp);
    }

    /// @brief serialization.
    /// @return data after serialization.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    Chunk Serialize() const noexcept;

    /// @brief deserialization.
    /// @param chunk msg after deserialization.
    /// @return 0 success;other fail.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    int32_t Deserialize(Chunk& chunk) noexcept;

    /// @brief set msg.
    /// @param msg msg.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    void SetMsg(Message const& msg) noexcept;

    /// @brief Returns the msg.
    /// @return msg.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    Message GetMsg() const noexcept;

private:
    /// @brief msg
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = dda
    Message msg_;
};

}  // namespace racom
}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_RACOM_MESSAGE_H_