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

#ifndef ARA_COM_INTERNAL_TRACE_USERDEFINE_MESSAGES_H_
#define ARA_COM_INTERNAL_TRACE_USERDEFINE_MESSAGES_H_

#include <cstdint>

namespace ara {
namespace com {
namespace internal {
namespace trace {
namespace userdefine {

constexpr uint32_t kInt32_0x80000C40U{0x80000C40};
constexpr uint32_t kInt32_0x80000C41U{0x80000C41};
constexpr uint32_t kInt32_0x80000C42U{0x80000C42};
constexpr uint32_t kInt32_0x80000C43U{0x80000C43};
constexpr uint32_t kInt32_0x80000C44U{0x80000C44};
constexpr uint32_t kInt32_0x80000C45U{0x80000C45};
constexpr uint32_t kInt32_0x80000C46U{0x80000C46};
constexpr uint32_t kInt32_0x80000C47U{0x80000C47};
constexpr uint32_t kInt32_0x80000C48U{0x80000C48};
constexpr uint32_t kInt32_0x80000C49U{0x80000C49};
constexpr uint32_t kInt32_0x80000C4AU{0x80000C4A};
constexpr uint32_t kInt32_0x80000C4BU{0x80000C4B};
constexpr uint32_t kInt32_0x80000C4CU{0x80000C4C};

struct MServiceOfferedType
{
    std::uint32_t id{kInt32_0x80000C40U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{8};
};
constexpr static MServiceOfferedType kServiceOffered;

struct MServiceFindType
{
    std::uint32_t id{kInt32_0x80000C41U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{8};
};
constexpr static MServiceFindType kServiceFind;

struct MEventSubscribeType
{
    std::uint32_t id{kInt32_0x80000C42U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{10};
};
constexpr static MEventSubscribeType kEventSubscribe;

struct MEventUnsubscribeType
{
    std::uint32_t id{kInt32_0x80000C43U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{10};
};
constexpr static MEventUnsubscribeType kEventUnsubscribe;

struct MSendEventType
{
    std::uint32_t id{kInt32_0x80000C44U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{12};
};
constexpr static MSendEventType kSendEvent;

struct MRecvEventType
{
    std::uint32_t id{kInt32_0x80000C45U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{12};
};
constexpr static MRecvEventType kRecvEvent;

struct MSendRequestType
{
    std::uint32_t id{kInt32_0x80000C46U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MSendRequestType kSendRequest;

struct MRecvRequestType
{
    std::uint32_t id{kInt32_0x80000C47U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MRecvRequestType kRecvRequest;

struct MSendResponseType
{
    std::uint32_t id{kInt32_0x80000C48U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MSendResponseType kSendResponse;

struct MRecvResponseType
{
    std::uint32_t id{kInt32_0x80000C49U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MRecvResponseType kRecvResponse;

struct SerializeType
{
    std::uint32_t id{kInt32_0x80000C4AU};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static SerializeType kSerialize;

struct DeserializeType
{
    std::uint32_t id{kInt32_0x80000C4BU};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static DeserializeType kDeserialize;

struct MEventSubscribedType
{
    std::uint32_t id{kInt32_0x80000C4CU};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{10};
};
constexpr static MEventSubscribedType kEventSubscribed;

}  // namespace userdefine
}  // namespace trace
}  // namespace internal
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_INTERNAL_TRACE_USERDEFINE_MESSAGES_H_
