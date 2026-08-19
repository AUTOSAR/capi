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
/// @file       nsomeip_runtime.h
/// @brief      Binding layer runtime header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef __COM_NSOMEIP_RUNTIME_H
#define __COM_NSOMEIP_RUNTIME_H

#include "ara/com/internal/runtime.h"
#include "nsomeip/core/nsi_conf.h"
#include "nsomeip_serialize.h"
#include "nsomeip_signal.h"
#ifdef HAS_COM_SECOC
    #include "ara/com/internal/secoc/secoc_signal.h"
    #include "ara/com/internal/secoc/secoc_someip.h"
#else
#endif
#ifdef HAS_MONITOR_LOG
    #include "ara/com/internal/trace/trace.h"
#endif

/// @brief Namespace -- internal binding layer
namespace ara {
namespace com {
namespace internal {
namespace nsomeip {
/// @brief Initialize binding layer
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @param[in] isDaemon Whether it is a daemon process -- default no
/// @return Result object -- void/value or error
ara::core::Result< void > Initialize(bool isDaemon = false) noexcept;
/// @brief Deinitialize binding layer
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @return Result object -- void/value or error
ara::core::Result< void > Deinitialize() noexcept;
/// @brief Binding layer runtime type -- declaration
class NSomeipRuntime;
/// @brief Namespace -- internal binding layer runtime
namespace runtime {
/// @brief Get binding layer runtime instance
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @return Binding layer runtime instance
NSomeipRuntime& GetInstance() noexcept;

/// @brief Assert expression, if not true, log fatal error and abort
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @param[in] exp Expression
/// @return Expression return value
#define NSOMEIP_ASSERT(exp, ...)                                                                                       \
    (__extension__({                                                                                                   \
        auto __r{(exp)};                                                                                               \
        if (ara::com::internal::nsomeip::runtime::ValueInvalid(__r)) {                                                 \
            ComLogFatal("assert nsomeip failed[", __r, "]: " #exp, ##__VA_ARGS__);                                     \
            ara::core::Abort("assert nsomeip failed");                                                                 \
        }                                                                                                              \
        __r;                                                                                                           \
    }))
/// @brief Check expression, if not true, log error
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @param[in] exp Expression
/// @return Expression return value
#define NSOMEIP_CHECK(exp, ...)                                                                                        \
    (__extension__({                                                                                                   \
        auto __r{(exp)};                                                                                               \
        if (ara::com::internal::nsomeip::runtime::ValueInvalid(__r)) {                                                 \
            ComLogError("check nsomeip failed[", __r, "]: " #exp, ##__VA_ARGS__);                                      \
        }                                                                                                              \
        __r;                                                                                                           \
    }))
/// @brief Verify expression, if not true, log warning
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @param[in] exp Expression
/// @return Expression return value
#define NSOMEIP_VERIFY(exp, ...)                                                                                       \
    (__extension__({                                                                                                   \
        auto __r{(exp)};                                                                                               \
        if (ara::com::internal::nsomeip::runtime::ValueInvalid(__r)) {                                                 \
            ComLogWarning("verify nsomeip failed[", __r, "]: " #exp, ##__VA_ARGS__);                                   \
        }                                                                                                              \
        __r;                                                                                                           \
    }))

/// @brief Determine whether it is an invalid value
/// @tparam T Value type
/// @param[in] r Value
/// @return bool
template < typename T >
inline bool ValueInvalid(T const& r) noexcept
{
    return !r;
}
/// @brief Determine whether it is an invalid value
/// @param[in] r Value
/// @return bool
inline bool ValueInvalid(int32_t r) noexcept { return r < 0; }

/// @brief Convert to instance identifier
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @param[in] input Binding layer instance identifier
/// @return Instance identifier
InstanceIdentifier ToInstanceIdentifier(ServiceInfo::InstanceId const& input) noexcept;
/// @brief Convert to binding layer instance identifier
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @param[in] input Instance identifier
/// @return Optional object -- void/binding layer instance identifier
ara::core::Optional< ServiceInfo::InstanceId > ToNSomeipInstanceId(InstanceIdentifier const& input) noexcept;

/// @brief Serialize value list to message and return number of bytes written or error code
/// @tparam Desc Description information
/// @tparam Values Value list type
/// @param[in] message Message
/// @param[in] values Value list
/// @return Result -- >= 0: number of bytes written; < 0: error code;
template < typename Desc, typename... Values >
inline int32_t Serialize(nsi_message_t& message, Values&&... values) noexcept
{
    TimeStart(TimeNM::Timer, "NSISerialize::E2EIf");
    auto dataId{ara::com::e2exf::StatusHandler::GetDataId(message.hdr.serv, message.inst, message.hdr.method)};
    ComLogTrace("serialize message", GenArg(dataId), GenArg(message), GenArg(std::forward< Values >(values)...));
    int32_t ret{};
    if (ara::com::e2exf::Transformer::E2E_IsProtected(dataId)) {  // enable e2e
        serialize::E2EPayload payload{message, dataId};
        ret = serialize::Serialize< Desc >(payload, std::forward< Values >(values)...);
        ComLogTrace("serialize E2E message", GenArg(ret), GenArg(dataId), GenArg(message),
                    GenArg(std::forward< Values >(values)...));
        if (!payload.r) {
            ComLogWarning("serialize E2E message", GenArg(payload.r.Error()), GenArg(dataId), GenArg(message),
                          GenArg(std::forward< Values >(values)...));
        }
    } else {
        ret = serialize::Serialize< Desc >(message, std::forward< Values >(values)...);
        ComLogTrace("serialize message", GenArg(ret), GenArg(dataId), GenArg(message),
                    GenArg(std::forward< Values >(values)...));
    }
    if (ret < 0) {
        return ret;
    }
    TimeStop(TimeNM::Timer, "NSISerialize::E2EIf");
#ifdef HAS_COM_SECOC
    TimeStart(TimeNM::Timer, "NSISerialize::SecOCIf");
    // secoc tx signal process
    auto secOcSignalDataId{secoc::SecOC4Signal::GetSecOCDataId(
        (message.hdr.serv << (sizeof(Message::ServiceId) * CHAR_BIT)) + message.hdr.method)};
    if (secOcSignalDataId > 0) {
        ret = secoc::SecOC4Signal::SecOCTxHandle(secOcSignalDataId, &message);
        ComLogTrace("invoke SecOC Signal Tx handle", GenArg(ret));
        if (ret < 0) {
            return ret;
        }
    }
    // secoc tx someip process
    auto secOcDataId{
        secoc::SecOC4SOMEIP::GetSecOCDataId(message.hdr.serv, message.inst, message.hdr.method, message.hdr.type)};
    if (secOcDataId > 0) {
        ret = secoc::SecOC4SOMEIP::SecOCTxHandle(secOcDataId, &message);
        ComLogTrace("invoke SecOC Tx handle", GenArg(ret));
    }
    TimeStop(TimeNM::Timer, "NSISerialize::SecOCIf");
#else
#endif
    return ret;
}
/// @brief Deserialize message to value list and return number of bytes read or error code
/// @tparam Desc Description information
/// @tparam Values Value list type
/// @param[in] message Message
/// @param[in] values Value list
/// @return Result -- >= 0: number of bytes read; < 0: error code;
template < typename Desc, typename... Values >
inline int32_t Deserialize(nsi_message_t const& message, Values&&... values) noexcept
{
    TimeRecorder(TimeNM::Timer, "NSIDeserialize(forward)");
    auto ret{serialize::Deserialize< Desc >(message, std::forward< Values >(values)...)};
    ComLogTrace("deserialize message", GenArg(ret), GenArg(message), GenArg(std::forward< Values >(values)...));
    return ret;
}
/// @brief Deserialize E2E payload message to value list and return number of bytes read or error code
/// @tparam Desc Description information
/// @tparam Values Value list type
/// @param[in] message E2E payload message
/// @param[in] values Value list
/// @return Result -- >= 0: number of bytes read; < 0: error code;
template < typename Desc, typename... Values >
inline int32_t Deserialize(serialize::E2EPayload const& message, Values&&... values) noexcept
{
    TimeRecorder(TimeNM::Timer, "NSIDeserialize(E2E)");
    auto ret{serialize::Deserialize< Desc >(message, std::forward< Values >(values)...)};
    ComLogTrace("deserialize E2E message", GenArg(ret), GenArg(message.m), GenArg(std::forward< Values >(values)...));
    if (message.r) {
        auto r{message.r.Value()};
        ComLogTrace("deserialize E2E message", GenArg(r.IsOk()), GenArg(r.GetProfileCheckStatus()),
                    GenArg(r.GetSMState()), GenArg(message.m), GenArg(std::forward< Values >(values)...));
    } else {
        ComLogError("deserialize E2E message", GenArg(message.r.Error()), GenArg(message.m),
                    GenArg(std::forward< Values >(values)...));
    }
    return ret;
}

/// @brief Create message -- event/field notification
/// @tparam Desc Description information
/// @tparam Value Value type
/// @tparam c Specialization condition -- whether it is an S2S conversion target (no)
/// @param[in] app Application layer application
/// @param[in] instanceId Instance identifier
/// @param[in] sessionId Session identifier
/// @param[in] value Notification value
/// @return Notification message
template < typename Desc, typename Value, bool c = signal::isSignalBased< Desc >() && signal::isS2STarget< Desc >() >
inline nsi_message_t* CreateNotification(nsi_app_t* app,
                                         ServiceInfo::InstanceId instanceId,
                                         Message::SessionId sessionId,
                                         Value const& value,
                                         std::enable_if_t< !c >* = nullptr) noexcept
{
    TimeRecorder(TimeNM::Timer, "NSINotify::Create");
    auto notification{NSOMEIP_ASSERT(nsi_app_create_notification(app, Desc::serviceId(), instanceId, Desc::eventId()),
                                     "create notification error: invoke stack create notification failed",
                                     GenArg(Desc::serviceId()), GenArg(instanceId), GenArg(Desc::eventId()),
                                     GenArg(sessionId), GenArg(value))};
    notification->hdr.interface = Desc::serviceVersionMajor();
    // notification->hdr.client = 0;
    notification->hdr.session = sessionId;
    notification->reliable    = Desc::isReliable();
    auto ret{Serialize< Desc >(*notification, value)};
    if (ret < 0) {
        ComLogWarning("create notification error: serialize failed", GenArg(ret), GenArg(notification));
        std::ignore = nsi_message_close(notification);
        return nullptr;
    }
    return notification;
}
/// @brief Create message -- event/field notification
/// @tparam Desc Description information
/// @tparam Value Value type
/// @tparam c Specialization condition -- whether it is an S2S conversion target (yes)
/// @param[in] app Application layer application
/// @param[in] instanceId Instance identifier
/// @param[in] sessionId Session identifier
/// @param[in] value Notification value
/// @return Notification message
template < typename Desc, typename Value, bool c = signal::isSignalBased< Desc >() && signal::isS2STarget< Desc >() >
inline nsi_message_t* CreateNotification(nsi_app_t* app,
                                         ServiceInfo::InstanceId instanceId,
                                         Message::SessionId sessionId,
                                         Value const& value,
                                         std::enable_if_t< c >* = nullptr) noexcept
{
    TimeRecorder(TimeNM::Timer, "NSINotify(S2S)::Create");
    auto notification{NSOMEIP_ASSERT(nsi_app_create_notification(app, Desc::serviceId(), instanceId, Desc::eventId()),
                                     "create notification error: invoke stack create notification failed",
                                     GenArg(Desc::serviceId()), GenArg(instanceId), GenArg(Desc::eventId()),
                                     GenArg(sessionId), GenArg(value))};
    // notification->hdr.interface = Desc::serviceVersionMajor();
    // notification->hdr.client = 0;
    notification->hdr.session = sessionId;
    notification->reliable    = Desc::isReliable();
    signal::S2S< signal::headerId< Desc >() > wrapper{const_cast< Value& >(value)};
    auto ret{Serialize< Desc >(*notification, wrapper)};
    if (ret < 0) {
        ComLogWarning("create notification error: serialize failed", GenArg(ret), GenArg(notification));
        std::ignore = nsi_message_close(notification);
        return nullptr;
    }
    return notification;
}
/// @brief Create message -- method request
/// @tparam Desc Description information
/// @tparam Value Value type
/// @param[in] app Application layer application
/// @param[in] instanceId Instance identifier
/// @param[in] messageType Message type
/// @param[in] sessionId Session identifier
/// @param[in] values Value list
/// @return Request message
template < typename Desc, typename... Values >
inline nsi_message_t* CreateRequest(nsi_app_t* app,
                                    ServiceInfo::InstanceId instanceId,
                                    Message::MessageType messageType,
                                    Message::SessionId sessionId,
                                    Values&&... values) noexcept
{
    TimeRecorder(TimeNM::Timer, "NSIRequest::Create");
    auto request{NSOMEIP_ASSERT(nsi_app_create_request(app, Desc::serviceId(), instanceId, Desc::methodId()),
                                "create request error: invoke stack create request failed", GenArg(Desc::serviceId()),
                                GenArg(instanceId), GenArg(Desc::methodId()), GenArg(sessionId), GenArg(values...))};
    request->hdr.interface = Desc::serviceVersionMajor();
    request->hdr.type      = messageType;
    request->hdr.session   = sessionId;
    request->reliable      = Desc::isReliable();
    auto ret{Serialize< Desc >(*request, std::forward< Values >(values)...)};
    if (ret < 0) {
        ComLogWarning("create request error: serialize failed", GenArg(ret), GenArg(request));
        std::ignore = nsi_message_close(request);
        return nullptr;
    }
    return request;
}
/// @brief Create message -- method response -- no value
/// @tparam Desc Description information
/// @param[in] app Application layer application
/// @param[in] request Request message
/// @return Response message
template < typename Desc >
inline nsi_message_t* CreateResponse(nsi_app_t* app, nsi_message_t* request) noexcept
{
    TimeRecorder(TimeNM::Timer, "NSIResponse(void)::Create");
    // Here no common shared memory is used to send
    auto rcode{request->hdr.code};
    auto* response{NSOMEIP_ASSERT(nsi_app_create_response(app, request, rcode),
                                  "create request error: invoke stack create response failed", GenArg(request))};
    auto dataId{ara::com::e2exf::StatusHandler::GetDataId(response->hdr.serv, response->inst, response->hdr.method)};
    if (ara::com::e2exf::Transformer::E2E_IsProtected(dataId)) {
        auto ret{Serialize< Desc >(*response)};
        if (ret < 0) {
            ComLogWarning("create request error: serialize failed", GenArg(ret), GenArg(response), GenArg(request));
            std::ignore = nsi_message_close(response);
            return nullptr;
        }
    }
    return response;
}
/// @brief Create message -- method response -- normal value
/// @tparam Desc Description information
/// @tparam Value Value type
/// @param[in] app Application layer application
/// @param[in] request Request message
/// @param[in] value Normal value
/// @return Response message
template < typename Desc, typename Value >
inline nsi_message_t* CreateResponse(
    nsi_app_t* app,
    nsi_message_t* request,
    Value const& value,
    std::enable_if_t< !std::is_same< Value, ara::core::ErrorCode >::value
                      && !serialize::IsContainer< Value, ara::core::Result >::value >* = nullptr) noexcept
{
    TimeRecorder(TimeNM::Timer, "NSIResponse(value)::Create");
    // Here no common shared memory is used to send
    auto rcode{request->hdr.code};
    auto* response{NSOMEIP_ASSERT(nsi_app_create_response(app, request, rcode),
                                  "create response error: invoke stack create response failed", GenArg(request),
                                  GenArg(value))};
    auto ret{Serialize< Desc >(*response, value)};
    if (ret < 0) {
        ComLogWarning("create response error: serialize failed", GenArg(ret), GenArg(response), GenArg(request),
                      GenArg(value));
        std::ignore = nsi_message_close(response);
        return nullptr;
    }
    return response;
}
/// @brief Create message -- method response -- error value
/// @tparam Desc Description information
/// @param[in] app Application layer application
/// @param[in] request Request message
/// @param[in] value Error value
/// @return Response message
template < typename Desc >
inline nsi_message_t* CreateResponse(nsi_app_t* app, nsi_message_t* request, ara::core::ErrorCode const& value) noexcept
{
    TimeRecorder(TimeNM::Timer, "NSIResponse(error)::Create");
    // Here no common shared memory is used to send
    auto rcode{request->hdr.code};
    auto* response{NSOMEIP_ASSERT(nsi_app_create_response(app, request, rcode),
                                  "create response error: invoke stack create response failed", GenArg(request),
                                  GenArg(value))};
    auto ret{Serialize< Desc >(*response, value)};
    if (ret < 0) {
        ComLogWarning("create response error: serialize failed", GenArg(ret), GenArg(response), GenArg(request),
                      GenArg(value));
        std::ignore = nsi_message_close(response);
        return nullptr;
    }
    return response;
}
/// @brief Create message -- method response -- value result
/// @tparam Desc Description information
/// @tparam T Value type
/// @param[in] app Application layer application
/// @param[in] request Request message
/// @param[in] value Result value
/// @return Response message
template < typename Desc, typename T >
inline nsi_message_t* CreateResponse(nsi_app_t* app,
                                     nsi_message_t* request,
                                     ara::core::Result< T > const& value) noexcept
{
    TimeRecorder(TimeNM::Timer, "NSIResponse(value|error)::Create");
    return value ? CreateResponse< Desc >(app, request, value.Value())
                 : CreateResponse< Desc >(app, request, value.Error());
}
/// @brief Create message -- method response -- void result
/// @tparam Desc Description information
/// @param[in] app Application layer application
/// @param[in] request Request message
/// @param[in] value Result value
/// @return Response message
template < typename Desc >
inline nsi_message_t* CreateResponse(nsi_app_t* app,
                                     nsi_message_t* request,
                                     ara::core::Result< void > const& value) noexcept
{
    TimeRecorder(TimeNM::Timer, "NSIResponse(void|error)::Create");
    return value ? CreateResponse< Desc >(app, request) : CreateResponse< Desc >(app, request, value.Error());
}

/// @brief Verify whether the received message is valid
/// @tparam Desc Description information
/// @tparam c Specialization condition -- whether it is signal-based (no)
/// @param[in] message Message
/// @return Message::ReturnCode
/// @ref [SWS_CM_10292]
/// @ref [SWS_CM_10291]
/// @ref [SWS_CM_10324]
/// @ref [SWS_CM_10302]
/// @ref [SWS_CM_10334]
template < typename Desc, bool c = signal::isSignalBased< Desc >() >
inline typename Message::ReturnCode VerifyMessage(nsi_message_t const& message,
                                                  std::enable_if_t< !c >* = nullptr) noexcept
{
    if (message.hdr.len < Message::kLengthMin) {
        ComLogWarning("verify message error: malformed message", GenArg(message), GenArg(Message::kLengthMin));
        return Message::kReturnCodeMalformedMessage;
    }
    if (message.hdr.serv != Desc::serviceId()) {
        ComLogWarning("verify message error: unknown service", GenArg(message), GenArg(Desc::serviceId()));
        return Message::kReturnCodeUnknownService;
    }
    if (message.hdr.protocol != Message::kProtocolVersionDefault) {
        ComLogWarning("verify message error: wrong protocol version", GenArg(message),
                      GenArg(Message::kProtocolVersionDefault));
        return Message::kReturnCodeWrongProtocolVersion;
    }
    if (message.hdr.interface != Desc::serviceVersionMajor()) {
        ComLogWarning("verify message error: wrong interface version", GenArg(message),
                      GenArg(Desc::serviceVersionMajor()));
        return Message::kReturnCodeWrongInterfaceVersion;
    }
    return Message::kReturnCodeOk;
}
/// @brief Verify whether the received message is valid
/// @tparam Desc Description information
/// @tparam c Specialization condition -- whether it is signal-based (yes)
/// @param[in] message Message
/// @return Message::ReturnCode
template < typename Desc, bool c = signal::isSignalBased< Desc >() >
inline typename Message::ReturnCode VerifyMessage(nsi_message_t const& message,
                                                  std::enable_if_t< c >* = nullptr) noexcept
{
    if (message.hdr.len < Message::kLengthMin) {
        ComLogWarning("verify message error: malformed message", GenArg(message), GenArg(Message::kLengthMin));
        return Message::kReturnCodeMalformedMessage;
    }
    if (message.hdr.serv != Desc::serviceId()) {
        ComLogWarning("verify message error: unknown service", GenArg(message), GenArg(Desc::serviceId()));
        return Message::kReturnCodeUnknownService;
    }
    return Message::kReturnCodeOk;
}
/// @brief Verify whether the received notification message is valid -- proxy side
/// @tparam Desc Description information
/// @param[in] message Message
/// @return Message::ReturnCode
template < typename Desc >
inline typename Message::ReturnCode VerifyNotification(nsi_message_t const& message) noexcept
{
    auto ret{VerifyMessage< Desc >(message)};
    if (ret != Message::kReturnCodeOk) {
        return ret;
    }
    if (message.hdr.method != Desc::eventId()) {
        ComLogWarning("verify notification error: unknown method", GenArg(message), GenArg(Desc::eventId()));
        return Message::kReturnCodeUnknownMethod;
    }
    if (!Message::IsEvent(message.hdr.type)) {
        ComLogWarning("verify notification error: wrong message type", GenArg(message),
                      GenArg(Message::kMessageTypeNotification));
        return Message::kReturnCodeWrongMessageType;
    }
    // if (message.hdr.client != 0) {
    //     ComLogWarning("verify notification error: wrong client", GenArg(message));
    //     return Message::kReturnCodeNotOk;
    // }
    return Message::kReturnCodeOk;
}
/// @brief Verify whether the received response message is valid -- proxy side
/// @tparam Desc Description information
/// @param[in] message Message
/// @return Message::ReturnCode
template < typename Desc >
inline typename Message::ReturnCode VerifyResponse(nsi_message_t const& message) noexcept
{
    auto ret{VerifyMessage< Desc >(message)};
    if (ret != Message::kReturnCodeOk) {
        return ret;
    }
    if (message.hdr.method != Desc::methodId()) {
        ComLogWarning("verify response error: unknown method", GenArg(message), GenArg(Desc::methodId()));
        return Message::kReturnCodeUnknownMethod;
    }
    if (!Message::IsResponse(message.hdr.type) && !Message::IsError(message.hdr.type)) {
        ComLogWarning("verify response error: wrong message type", GenArg(message),
                      GenArg(Message::kMessageTypeResponse), GenArg(Message::kMessageTypeError));
        return Message::kReturnCodeWrongMessageType;
    }
    return Message::kReturnCodeOk;
}
/// @brief Verify whether the received request message is valid -- skeleton side
/// @tparam Desc Description information
/// @param[in] message Message
/// @param[in] hasReturn Whether it is a two-way method
/// @return Message::ReturnCode
template < typename Desc >
inline typename Message::ReturnCode VerifyRequest(nsi_message_t const& message, bool hasReturn) noexcept
{
    auto ret{VerifyMessage< Desc >(message)};
    if (ret != Message::kReturnCodeOk) {
        return ret;
    }
    if (message.hdr.method != Desc::methodId()) {
        ComLogWarning("verify request error: unknown method", GenArg(message), GenArg(Desc::methodId()));
        return Message::kReturnCodeUnknownMethod;
    }
    if (hasReturn) {
        if (!Message::IsRequest(message.hdr.type)) {
            ComLogWarning("verify request error: wrong message type", GenArg(message),
                          GenArg(Message::kMessageTypeRequest));
            return Message::kReturnCodeWrongMessageType;
        }
    } else {
        if (!Message::IsRequestNoReturn(message.hdr.type)) {
            ComLogWarning("verify request error: wrong message type", GenArg(message),
                          GenArg(Message::kMessageTypeRequestNoReturn));
            return Message::kReturnCodeWrongMessageType;
        }
    }
    return Message::kReturnCodeOk;
}
}  // namespace runtime
/// @brief Namespace -- internal binding layer proxy
namespace proxy {
/// @brief Binding layer proxy mapper -- declaration
/// @tparam Proxy Proxy type
template < typename Proxy >
class NSomeipProxyMapper;
}  // namespace proxy
/// @brief Namespace -- internal binding layer skeleton
namespace skeleton {
/// @brief Binding layer skeleton mapper -- declaration
/// @tparam Skeleton Skeleton type
template < typename Skeleton >
class NSomeipSkeletonMapper;
}  // namespace skeleton
/// @brief Binding layer runtime type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
class NSomeipRuntime
{
public:
    /// @brief Proxy mapper base type
    class ProxyMapperBase
    {
    public:
        /// @brief Constructor
        ProxyMapperBase() noexcept;
        /// @brief Destructor
        virtual ~ProxyMapperBase() noexcept;
        /// @brief Copy constructor
        /// @param other
        ProxyMapperBase(ProxyMapperBase const& other) noexcept = default;
        /// @brief Move constructor
        /// @param other
        ProxyMapperBase(ProxyMapperBase&& other) noexcept = default;
        /// @brief Copy assignment operator
        /// @param other
        /// @return ProxyMapperBase
        ProxyMapperBase& operator=(ProxyMapperBase const& other) noexcept = default;
        /// @brief Move assignment operator
        /// @param other
        /// @return ProxyMapperBase
        ProxyMapperBase& operator=(ProxyMapperBase&& other) noexcept = default;
        /// @brief Get service identifier
        /// @return Service identifier
        virtual ServiceIdentifier const& GetServiceIdentifier() const noexcept = 0;
        /// @brief Get service identifier
        /// @return Service identifier
        virtual ServiceInfo::ServiceId GetServiceId() const noexcept = 0;
        /// @brief Create binding layer proxy handle
        /// @param[in] instanceIdentifier Instance identifier
        /// @return Proxy handle pointer
        virtual std::shared_ptr< ara::com::internal::proxy::BindHandle > CreateBindHandle(
            ara::com::InstanceIdentifier const& instanceIdentifier) const noexcept = 0;
    };
    /// @brief Skeleton mapper base type
    class SkeletonMapperBase
    {
    public:
        /// @brief Constructor
        SkeletonMapperBase() noexcept;
        /// @brief Destructor
        virtual ~SkeletonMapperBase() noexcept;
        /// @brief Copy constructor
        /// @param other
        SkeletonMapperBase(SkeletonMapperBase const& other) noexcept = default;
        /// @brief Move constructor
        /// @param other
        SkeletonMapperBase(SkeletonMapperBase&& other) noexcept = default;
        /// @brief Copy assignment operator
        /// @param other
        /// @return SkeletonMapperBase
        SkeletonMapperBase& operator=(SkeletonMapperBase const& other) noexcept = default;
        /// @brief Move assignment operator
        /// @param other
        /// @return SkeletonMapperBase
        SkeletonMapperBase& operator=(SkeletonMapperBase&& other) noexcept = default;
        /// @brief Get service identifier
        /// @return Service identifier
        virtual ServiceIdentifier const& GetServiceIdentifier() const noexcept = 0;
        /// @brief Get service identifier
        /// @return Service identifier
        virtual ServiceInfo::ServiceId GetServiceId() const noexcept = 0;
        /// @brief Create binding layer skeleton instance
        /// @param[in] skeleton Service skeleton
        /// @param[in] instanceIdentifier Instance identifier
        /// @return Skeleton instance pointer
        virtual std::unique_ptr< ara::com::internal::skeleton::BindSkeleton > CreateBindSkeleton(
            ara::com::internal::skeleton::Skeleton& skeleton,
            ara::com::InstanceIdentifier const& instanceIdentifier) const noexcept = 0;
    };
    /// @brief Register proxy mapper
    /// @tparam Proxy Proxy type
    template < typename Proxy >
    void RegisterProxyMapper() noexcept
    {
        _registerProxyMapper(std::make_shared< proxy::NSomeipProxyMapper< Proxy > >());
    }
    /// @brief Register skeleton mapper
    /// @tparam Skeleton Skeleton type
    template < typename Skeleton >
    void RegisterSkeletonMapper() noexcept
    {
        _registerSkeletonMapper(std::make_shared< skeleton::NSomeipSkeletonMapper< Skeleton > >());
    }

private:
    /// @brief Register proxy mapper
    /// @tparam mapper Proxy mapper instance
    void _registerProxyMapper(std::shared_ptr< ProxyMapperBase > const& mapper) noexcept;
    /// @brief Register skeleton mapper
    /// @tparam mapper Skeleton mapper instance
    void _registerSkeletonMapper(std::shared_ptr< SkeletonMapperBase > const& mapper) noexcept;

public:
    /// @brief Service request holder type -- helper for counting requested service objects
    class ServiceRequestedHolder
    {
    public:
        /// @brief Constructor
        /// @param[in] instanceUID Service instance unique identifier
        explicit ServiceRequestedHolder(ServiceInfo::InstanceUID instanceUID) noexcept;
        /// @brief Destructor
        ~ServiceRequestedHolder() noexcept;
        /// @brief Copy constructor
        /// @param[in] other
        ServiceRequestedHolder(ServiceRequestedHolder const& other) noexcept = delete;
        /// @brief Copy assignment operator
        /// @param[in] other
        /// @return ServiceRequestedHolder&
        ServiceRequestedHolder& operator=(ServiceRequestedHolder const& other) noexcept = delete;
        /// @brief Move constructor
        /// @param[in] other
        ServiceRequestedHolder(ServiceRequestedHolder&& other) noexcept = delete;
        /// @brief Move assignment operator
        /// @param[in] other
        /// @return ServiceRequestedHolder&
        ServiceRequestedHolder& operator=(ServiceRequestedHolder&& other) noexcept = delete;
        /// @brief Service instance unique identifier
        ServiceInfo::InstanceUID instanceUID;
    };

public:
    /// @brief Constructor
    NSomeipRuntime() noexcept;
    /// @brief Destructor
    ~NSomeipRuntime() noexcept;
    /// @brief Copy constructor
    /// @param other
    NSomeipRuntime(NSomeipRuntime const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    NSomeipRuntime(NSomeipRuntime&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return NSomeipRuntime
    NSomeipRuntime& operator=(NSomeipRuntime const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return NSomeipRuntime
    NSomeipRuntime& operator=(NSomeipRuntime&& other) noexcept = default;
    /// @brief Initialize
    /// @return Result object -- void/value or error
    ara::core::Result< void > Initialize() noexcept;
    /// @brief Deinitialize
    /// @return Result object -- void/value or error
    ara::core::Result< void > Deinitialize() noexcept;
    /// @brief Get service information -- skeleton side
    /// @return Service information pointer
    ServiceInfo* GetPServiceInfo(decltype(ServiceInfo::serviceId) serviceId,
                                 decltype(ServiceInfo::instanceId) instanceId) const noexcept;
    /// @brief Get service information -- proxy side
    /// @return Service information pointer
    ServiceInfo* GetRServiceInfo(decltype(ServiceInfo::serviceId) serviceId,
                                 decltype(ServiceInfo::instanceId) instanceId) const noexcept;
    /// @brief Get application layer application
    /// @return Application layer application pointer
    nsi_app_t* GetApp() noexcept;
    /// @brief Get application layer application callback
    /// @return Application layer application callback pointer
    nsi_appcb_t* GetAppCB() noexcept;
    /// @brief Provide service -- skeleton side
    /// @param[in] instanceUID Service instance unique identifier
    /// @return Result -- 0: success; !0: failure;
    int32_t OfferService(ServiceInfo::InstanceUID const& instanceUID) noexcept;
    /// @brief Stop providing service -- skeleton side
    /// @param[in] instanceUID Service instance unique identifier
    /// @return Result -- 0: success; !0: failure;
    int32_t StopService(ServiceInfo::InstanceUID const& instanceUID) noexcept;
    /// @brief Subscribe to event -- proxy side
    /// @param[in] serviceId Service identifier
    /// @param[in] instanceId Instance identifier
    /// @param[in] major Service major version
    /// @param[in] eid Event identifier
    /// @param[in] ev Event information
    /// @param[in] callback Subscription event confirmation callback
    /// @return Result -- 0: success; !0: failure;
    int32_t SubscribeEvent(ServiceInfo::ServiceId serviceId,
                           ServiceInfo::InstanceId instanceId,
                           ServiceInfo::MajorVersion major,
                           ServiceInfo::EventId eid,
                           nsi_event_info_t* ev,
                           std::function< void(int32_t rcode) >&& callback) noexcept;
    /// @brief Unsubscribe from event -- proxy side
    /// @param[in] serviceId Service identifier
    /// @param[in] instanceId Instance identifier
    /// @param[in] major Service major version
    /// @param[in] eid Event identifier
    /// @param[in] ev Event information
    /// @return Result -- 0: success; !0: failure;
    int32_t UnsubscribeEvent(ServiceInfo::ServiceId serviceId,
                             ServiceInfo::InstanceId instanceId,
                             ServiceInfo::MajorVersion major,
                             ServiceInfo::EventId eid,
                             nsi_event_info_t* ev) noexcept;
    /// @brief Send message
    /// @param[in] m Message
    /// @return Result -- 0: success; !0: failure;
    int32_t Send(nsi_message_t* m) noexcept;
#ifdef HAS_MONITOR_LOG
    /// @brief
    /// @tparam MsgId Monitoring log type
    /// @tparam ...Params Log parameter list
    /// @param id Monitoring log type
    /// @param type Service interface type 0 skeleton 1 proxy
    /// @param nServiceId Service interface ID
    /// @param nInstanceId Instance ID
    /// @param serviceIdentifier Service interface identifier
    /// @param ...args Log parameter list
    template < typename MsgId, typename... Params >
    void Trace(const MsgId& id,
               trace::ServiceType type,
               uint16_t nServiceId,
               uint16_t nInstanceId,
               ara::com::internal::ServiceIdentifier const& serviceIdentifier,
               const Params&... args) noexcept
    {
        auto& runtime{runtime::GetInstance()};
        auto& comRuntime{GetInstance()};
        ara::core::String instaneFQN = "unknown";
        if (type == trace::ServiceType::kSkeleton) {
            instaneFQN = runtime.GetPServiceInfo(nServiceId, nInstanceId)->name;
        } else {
            // TODO(Cui Jiusen) Not accurate, temporarily placeholder
            auto* rServiceInfo{runtime.GetRServiceInfo(nServiceId, nInstanceId)};
            if (rServiceInfo == nullptr) {
                rServiceInfo = runtime.GetRServiceInfo(nServiceId, Message::kInstanceIdAny);
            }
            if (rServiceInfo != nullptr) {
                instaneFQN = rServiceInfo->name;
            }
        }
        trace::ComTrace::Instance().Log(id, comRuntime.GetFullQualifiedName(), instaneFQN, serviceIdentifier.ToString(),
                                        "nsomeip", ara::core::to_string(static_cast< uint32_t >(nServiceId)),
                                        nInstanceId, args...);
    }
#endif

    /// @brief Type alias -- mark enumerable
    using IsEnumerableTag = void;
    /// @brief Enumeration operation -- enumerate members of this type by passing in a function body
    /// @tparam F Incoming function body type
    /// @param[in] fun Incoming function body
    template < typename F >
    void enumerate(F& fun)  // NOLINT -- Template interface name > naming convention
    {
        fun(impl_);
    }

private:
    friend class Impl;
    class Impl;
    Impl* impl_{};
};
}  // namespace nsomeip
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
