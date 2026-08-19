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
/// @file       messages.h
/// @brief
/// @details
/// @date       2024-12-19
/// @author     jiusen.cui
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_INTERNAL_TRACE_MESSAGES_H_
#define ARA_COM_INTERNAL_TRACE_MESSAGES_H_

#include <cstdint>

namespace ara {
namespace com {
namespace internal {
namespace trace {

constexpr int32_t kInt32_8{8};
constexpr int32_t kInt32_10{10};
constexpr int32_t kInt32_12{12};
constexpr int32_t kInt32_13{13};

constexpr uint32_t kInt32_0x8000C000U{0x8000C000};
constexpr uint32_t kInt32_0x8000C001U{0x8000C001};
constexpr uint32_t kInt32_0x8000C002U{0x8000C002};
constexpr uint32_t kInt32_0x8000C003U{0x8000C003};
constexpr uint32_t kInt32_0x8000C004U{0x8000C004};
constexpr uint32_t kInt32_0x8000C005U{0x8000C005};
constexpr uint32_t kInt32_0x8000C006U{0x8000C006};
constexpr uint32_t kInt32_0x8000C007U{0x8000C007};
constexpr uint32_t kInt32_0x8000C008U{0x8000C008};
constexpr uint32_t kInt32_0x8000C009U{0x8000C009};
constexpr uint32_t kInt32_0x8000C00AU{0x8000C00A};
constexpr uint32_t kInt32_0x8000C00BU{0x8000C00B};
constexpr uint32_t kInt32_0x8000C00CU{0x8000C00C};

struct MServiceOfferedType
{
    std::uint32_t id{kInt32_0x8000C000U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{8};
};
constexpr static MServiceOfferedType kServiceOffered;

struct MServiceFindType
{
    std::uint32_t id{kInt32_0x8000C001U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{8};
};
constexpr static MServiceFindType kServiceFind;

struct MEventSubscribeType
{
    std::uint32_t id{kInt32_0x8000C002U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{10};
};
constexpr static MEventSubscribeType kEventSubscribe;

struct MEventUnsubscribeType
{
    std::uint32_t id{kInt32_0x8000C003U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{10};
};
constexpr static MEventUnsubscribeType kEventUnsubscribe;

struct MSendEventType
{
    std::uint32_t id{kInt32_0x8000C004U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{12};
};
constexpr static MSendEventType kSendEvent;

struct MRecvEventType
{
    std::uint32_t id{kInt32_0x8000C005U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{12};
};
constexpr static MRecvEventType kRecvEvent;

struct MSendRequestType
{
    std::uint32_t id{kInt32_0x8000C006U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MSendRequestType kSendRequest;

struct MRecvRequestType
{
    std::uint32_t id{kInt32_0x8000C007U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MRecvRequestType kRecvRequest;

struct MSendResponseType
{
    std::uint32_t id{kInt32_0x8000C008U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MSendResponseType kSendResponse;

struct MRecvResponseType
{
    std::uint32_t id{kInt32_0x8000C009U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MRecvResponseType kRecvResponse;

struct SerializeType
{
    std::uint32_t id{kInt32_0x8000C00AU};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static SerializeType kSerialize;

struct DeserializeType
{
    std::uint32_t id{kInt32_0x8000C00BU};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static DeserializeType kDeserialize;

struct MEventSubscribedType
{
    std::uint32_t id{kInt32_0x8000C00CU};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{10};
};
constexpr static MEventSubscribedType kEventSubscribed;

}  // namespace trace
}  // namespace internal
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_INTERNAL_TRACE_MESSAGES_H_
