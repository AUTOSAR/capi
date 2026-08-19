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

#ifndef ARA_COM_INTERNAL_TRACE_SOMEIP_MESSAGES_H_
#define ARA_COM_INTERNAL_TRACE_SOMEIP_MESSAGES_H_

#include <cstdint>

namespace ara {
namespace com {
namespace internal {
namespace trace {
namespace someip {

constexpr uint32_t kInt32_0x80000C00U{0x80000C00};
constexpr uint32_t kInt32_0x80000C01U{0x80000C01};
constexpr uint32_t kInt32_0x80000C02U{0x80000C02};
constexpr uint32_t kInt32_0x80000C03U{0x80000C03};
constexpr uint32_t kInt32_0x80000C04U{0x80000C04};
constexpr uint32_t kInt32_0x80000C05U{0x80000C05};
constexpr uint32_t kInt32_0x80000C06U{0x80000C06};
constexpr uint32_t kInt32_0x80000C07U{0x80000C07};
constexpr uint32_t kInt32_0x80000C08U{0x80000C08};
constexpr uint32_t kInt32_0x80000C09U{0x80000C09};
constexpr uint32_t kInt32_0x80000C0AU{0x80000C0A};
constexpr uint32_t kInt32_0x80000C0BU{0x80000C0B};
constexpr uint32_t kInt32_0x80000C0CU{0x80000C0C};

struct MServiceOfferedType
{
    std::uint32_t id{kInt32_0x80000C00U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{8};
};
constexpr static MServiceOfferedType kServiceOffered;

struct MServiceFindType
{
    std::uint32_t id{kInt32_0x80000C01U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{8};
};
constexpr static MServiceFindType kServiceFind;

struct MEventSubscribeType
{
    std::uint32_t id{kInt32_0x80000C02U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{10};
};
constexpr static MEventSubscribeType kEventSubscribe;

struct MEventUnsubscribeType
{
    std::uint32_t id{kInt32_0x80000C03U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{10};
};
constexpr static MEventUnsubscribeType kEventUnsubscribe;

struct MSendEventType
{
    std::uint32_t id{kInt32_0x80000C04U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{12};
};
constexpr static MSendEventType kSendEvent;

struct MRecvEventType
{
    std::uint32_t id{kInt32_0x80000C05U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{12};
};
constexpr static MRecvEventType kRecvEvent;

struct MSendRequestType
{
    std::uint32_t id{kInt32_0x80000C06U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MSendRequestType kSendRequest;

struct MRecvRequestType
{
    std::uint32_t id{kInt32_0x80000C07U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MRecvRequestType kRecvRequest;

struct MSendResponseType
{
    std::uint32_t id{kInt32_0x80000C08U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MSendResponseType kSendResponse;

struct MRecvResponseType
{
    std::uint32_t id{kInt32_0x80000C09U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MRecvResponseType kRecvResponse;

struct SerializeType
{
    std::uint32_t id{kInt32_0x80000C0AU};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static SerializeType kSerialize;

struct DeserializeType
{
    std::uint32_t id{kInt32_0x80000C0BU};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static DeserializeType kDeserialize;

struct MEventSubscribedType
{
    std::uint32_t id{kInt32_0x80000C0CU};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{10};
};
constexpr static MEventSubscribedType kEventSubscribed;

}  // namespace someip
}  // namespace trace
}  // namespace internal
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_INTERNAL_TRACE_SOMEIP_MESSAGES_H_
