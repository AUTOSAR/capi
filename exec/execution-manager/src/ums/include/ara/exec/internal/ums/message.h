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
/// @file       message.h
/// @brief      Update client communication protocol Message definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/UMS
/// @unit_name=Message
/// @unit_description=The Message between UMS Server and UMS Client.
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_UMS_MESSAGE_H_
#define _ARA_EXEC_INTERNAL_UMS_MESSAGE_H_

#include <cstddef>
#include <cstring>

#include "ara/core/string_view.h"
#include "ara/core/vector.h"
#include "ara/exec/exec_error_domain.h"

namespace ara {
namespace exec {
namespace internal {
namespace ums {

#pragma pack(push, 1)
/// @brief SMS communication protocol message content
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_00063,SR_EM_00065,SR_EM_10007
/// @trace_id_ad=AD_EM_00088
/// @trace_id_dd=DD_EM_00263
/// @needwork = ad
/// @endcode
class Message
{
public:
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_EM_00063,SR_EM_00065,SR_EM_10007
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00264
    /// @needwork = dd
    /// @endcode
    enum class Operation : uint8_t
    {
 kUpdateUserSwcl = 1, ///< Update user software cluster operation
 kGetUserSwclManifest = 2 ///< Get user software cluster Manifest operation
    };

 /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00265
    /// @needwork = dda
    /// @endcode
    Message() = default;

 /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00266
    /// @needwork = dda
    /// @endcode
    ~Message() = default;

 /// @brief Default copy construction
 /// @param other Message object
    Message(Message const &other) = default;

 /// @brief Default move construction
 /// @param other Message object
    Message(Message &&other) = default;

 /// @brief Default copy assignment
 /// @param other Message object
 /// @return New message object
    Message &operator=(Message const &other) = default;

 /// @brief Default move assignment
 /// @param other Message object
 /// @return New message object
    Message &operator=(Message &&other) = default;

 /// @brief Set operation code
 /// @param t Operation code
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00267
    /// @needwork = dda
    /// @endcode
    void SetOperation(Operation const t) noexcept { opration_ = t; }

 /// @brief Get operation code
 /// @return Operation code
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00268
    /// @needwork = dda
    /// @endcode
    Operation GetOperation() const noexcept { return opration_; }

 /// @brief Set error code
 /// @param e Error code
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00269
    /// @needwork = dda
    /// @endcode
    void SetErrorCode(ara::exec::ExecErrc const &e) noexcept { errorCode_ = e; }

 /// @brief Get error code
 /// @return Error code
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00270
    /// @needwork = dda
    /// @endcode
    ara::exec::ExecErrc GetErrorCode() const noexcept { return errorCode_; }

 /// @brief Get message size
 /// @return Bytes
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00271
    /// @needwork = dda
    /// @endcode
    uint64_t GetSize() const noexcept { return GetSize(payloadSize_); }

 /// @brief Get payload size
 /// @return Bytes
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00272
    /// @needwork = dda
    /// @endcode
    uint64_t GetPayloadSize() const noexcept { return payloadSize_; }

 /// @brief Set payload size
 /// @param size Bytes
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00273
    /// @needwork = dda
    /// @endcode
    void SetPayloadSize(size_t const size) noexcept { payloadSize_ = size; }

 /// @brief Set user software cluster Manifest path
 /// @param swclManifestPath Software cluster Manifest path
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00274
    /// @needwork = dda
    /// @endcode
    void SetUserSwclManifest(ara::core::StringView const &swclManifestPath) noexcept
    {
        SetPayloadSize(CalculatePayloadSize(swclManifestPath));
        std::size_t const len{swclManifestPath.size()};
        std::ignore = memmove(&payload_, swclManifestPath.data(), len);
    }

 /// @brief Set user software cluster information
 /// @param swclList Serialized user software cluster information
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00275
    /// @needwork = dda
    /// @endcode
    void SetUserSwcls(ara::core::Vector< uint8_t > &swclList) noexcept
    {
        std::size_t const len{swclList.size()};
        SetPayloadSize(len);
        std::ignore = memmove(&payload_, swclList.data(), len);
    }

 /// @brief Get user software cluster manifest name
 /// @param name User software cluster manifest name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00276
    /// @needwork = dda
    /// @endcode
    void GetUserSwclManifest(ara::core::StringView &name) const noexcept
    {
        std::ignore = name;
        name = ara::core::StringView(reinterpret_cast< Char8_t const * >(payload_), payloadSize_);  // PRQA S 3049
    }

 /// @brief Set user software cluster information
 /// @return User software cluster information
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00277
    /// @needwork = dda
    /// @endcode
    uint8_t const *GetPayload() const noexcept { return static_cast< uint8_t const * >(payload_); }

 /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept
    {
#if ARA_EXEC_DEBUG
        std::cout << "Operation:" << std::endl;
        switch (opration_) {
            case Operation::kUpdateUserSwcl: {
                std::cout << "UpdateUserSwcls" << std::endl;
                break;
            }
            case Operation::kGetUserSwclManifest: {
                std::cout << "GetUserSwclManifest";
                break;
            }
            default: {
                std::cout << "Unknown Operation !!!";
                break;
            }
        }
        ara::core::StringView name;
        GetUserSwclManifest(name);
        std::cout << "UserSwclManifest: " << name << std::endl;
        std::cout << "--- ums::Message::Debug ---" << std::endl;
#endif  ///< ARA_EXEC_DEBUG
    }

 /// @brief Calculate total message size based on payload size
 /// @param payloadSize Payload size
 /// @return Bytes
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00278
    /// @needwork = dda
    /// @endcode
    static size_t GetSize(size_t const payloadSize) noexcept
    {
        return __builtin_offsetof(Message, payload_) + payloadSize;
    }

 /// @brief Calculate payload size
 /// @param name Process list name
 /// @return Bytes
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00279
    /// @needwork = dda
    /// @endcode
    static size_t CalculatePayloadSize(ara::core::StringView const &name) noexcept { return name.size(); }

 /// @brief Calculate message size
 /// @param name Process list name
 /// @return Bytes
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00280
    /// @needwork = dda
    /// @endcode
    static size_t CalculateSize(ara::core::StringView const &name) noexcept
    {
        return GetSize(CalculatePayloadSize(name));
    }

private:
 /// @brief Operation code
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00281
    /// @needwork = dda
    /// @endcode
    Operation opration_{Operation::kGetUserSwclManifest};
 /// @brief Error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00282
    /// @needwork = dda
    /// @endcode
    ara::exec::ExecErrc errorCode_{ara::exec::ExecErrc::kNoError};
 /// @brief Payload size
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00283
    /// @needwork = dda
    /// @endcode
    size_t payloadSize_{0U};
 /// @brief Payload content
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00088
    /// @trace_id_dd=DD_EM_00284
    /// @needwork = dda
    /// @endcode
    uint8_t payload_[1]{0U};  // the -Wpedantic forbiden zero-array, so increase 1
};                            ///< class INTERNAL_UMS_MESSAGE
#pragma pack(pop)

}  // namespace ums
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif
