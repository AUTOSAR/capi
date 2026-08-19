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

#ifndef ARA_COM_INTERNAL_TRACE_DDS_MESSAGES_H_
#define ARA_COM_INTERNAL_TRACE_DDS_MESSAGES_H_

#include <cstdint>

namespace ara {
namespace com {
namespace internal {
namespace trace {
namespace dds {

constexpr uint32_t kInt32_0x80000C20U{0x80000C20};
constexpr uint32_t kInt32_0x80000C21U{0x80000C21};
constexpr uint32_t kInt32_0x80000C22U{0x80000C22};
constexpr uint32_t kInt32_0x80000C23U{0x80000C23};
constexpr uint32_t kInt32_0x80000C24U{0x80000C24};
constexpr uint32_t kInt32_0x80000C25U{0x80000C25};
constexpr uint32_t kInt32_0x80000C26U{0x80000C26};
constexpr uint32_t kInt32_0x80000C27U{0x80000C27};
constexpr uint32_t kInt32_0x80000C28U{0x80000C28};
constexpr uint32_t kInt32_0x80000C29U{0x80000C29};
constexpr uint32_t kInt32_0x80000C2AU{0x80000C2A};
constexpr uint32_t kInt32_0x80000C2BU{0x80000C2B};
constexpr uint32_t kInt32_0x80000C2CU{0x80000C2C};

struct MServiceOfferedType
{
    std::uint32_t id{kInt32_0x80000C20U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{8};
};
constexpr static MServiceOfferedType kServiceOffered;

struct MServiceFindType
{
    std::uint32_t id{kInt32_0x80000C21U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{8};
};
constexpr static MServiceFindType kServiceFind;

struct MEventSubscribeType
{
    std::uint32_t id{kInt32_0x80000C22U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{10};
};
constexpr static MEventSubscribeType kEventSubscribe;

struct MEventUnsubscribeType
{
    std::uint32_t id{kInt32_0x80000C23U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{10};
};
constexpr static MEventUnsubscribeType kEventUnsubscribe;

struct MSendEventType
{
    std::uint32_t id{kInt32_0x80000C24U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{12};
};
constexpr static MSendEventType kSendEvent;

struct MRecvEventType
{
    std::uint32_t id{kInt32_0x80000C25U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{12};
};
constexpr static MRecvEventType kRecvEvent;

struct MSendRequestType
{
    std::uint32_t id{kInt32_0x80000C26U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MSendRequestType kSendRequest;

struct MRecvRequestType
{
    std::uint32_t id{kInt32_0x80000C27U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MRecvRequestType kRecvRequest;

struct MSendResponseType
{
    std::uint32_t id{kInt32_0x80000C28U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MSendResponseType kSendResponse;

struct MRecvResponseType
{
    std::uint32_t id{kInt32_0x80000C29U};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static MRecvResponseType kRecvResponse;

struct SerializeType
{
    std::uint32_t id{kInt32_0x80000C2AU};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static SerializeType kSerialize;

struct DeserializeType
{
    std::uint32_t id{kInt32_0x80000C2BU};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{13};
};
constexpr static DeserializeType kDeserialize;

struct MEventSubscribedType
{
    std::uint32_t id{kInt32_0x80000C2CU};
    std::int32_t lineNumber{};
    char const* sourceFile{""};
    char const* typeInfo{"DLT_TRACE_VFB"};
    std::int32_t argsNum{10};
};
constexpr static MEventSubscribedType kEventSubscribed;

}  // namespace dds
}  // namespace trace
}  // namespace internal
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_INTERNAL_TRACE_DDS_MESSAGES_H_
