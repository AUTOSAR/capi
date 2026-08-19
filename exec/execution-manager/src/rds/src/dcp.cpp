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
/// @file       dcp.cpp
/// @brief      Debug communication protocol
/// @details
/// @date       2023-11-17
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/exec/internal/rds/dcp.h"

#include <ara/core/string_view.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace ara {
namespace exec {
namespace internal {
namespace rds {

/// @brief Get content (json string)
/// @return json format message content
ara::core::StringView const DCP::GetContent() const noexcept
{
    DCP::Header const* const h{reinterpret_cast< Header const* >(frame_.data())};  // PRQA S 3049
    return h->GetContent();
}

/// @brief Parse and get packet type
/// @return Type
DCP::Type DCP::GetType() const noexcept
{
    Type type{Type::kUnknown};
    rapidjson::Document document;
    ara::core::StringView const jsonSv{GetContent()};

    std::ignore = document.Parse(jsonSv.data(), jsonSv.size());
    if (!document.IsObject()) {
        return type;
    }
    rapidjson::Value& typeObj{document[GetJsonType()]};
    if (!typeObj.IsString()) {
        return type;
    }

    if (typeObj.GetString() == ara::core::StringView{GetJsonTypeTraceRequest()}) {
        type = Type::kTraceRequest;
    } else if (typeObj.GetString() == ara::core::StringView{GetJsonTypeTraceResponse()}) {
        type = Type::kTraceResponse;
    } else if (typeObj.GetString() == ara::core::StringView{GetJsonTypeTraceCancel()}) {
        type = Type::kTraceCancel;
    } else if (typeObj.GetString() == ara::core::StringView{GetJsonTypeRequestFunctionGroup()}) {
        type = Type::kRequestFunctionGroup;
    } else if (typeObj.GetString() == ara::core::StringView{GetJsonTypeResponseFunctionGroup()}) {
        type = Type::kResponseFunctionGroup;
    } else if (typeObj.GetString() == ara::core::StringView{GetJsonTypeRequestProcess()}) {
        type = Type::kRequestProcess;
    } else if (typeObj.GetString() == ara::core::StringView{GetJsonTypeResponseProcess()}) {
        type = Type::kResponseProcess;
    } else if (typeObj.GetString() == ara::core::StringView{GetJsonTypeNotifyFunctionGroup()}) {
        type = Type::kNotifyFunctionGroup;
    } else if (typeObj.GetString() == ara::core::StringView{GetJsonTypeNotifyProcess()}) {
        type = Type::kNotifyProcess;
    } else {
        type        = Type::kUnknown;
        std::ignore = printf("GetType(): Unkonw type - %s\n", typeObj.GetString());
    }

    return type;
}

/// @brief Parse and get data from trace request packet
/// @param procNames Parsed process list
/// @return true/false
bool DCP::GetValue4TraceRequest(ara::core::Vector< ara::core::String >& procNames) const noexcept
{
    if (GetType() != Type::kTraceRequest) {
        return false;
    }

    rapidjson::Document document;
    ara::core::StringView const jsonSv{GetContent()};
    std::ignore = document.Parse(jsonSv.data(), jsonSv.size());
    if (!document.IsObject()) {
        return false;
    }
    rapidjson::Value& value{document[GetJsonValue()]};
    if (!value.IsArray()) {
        return false;
    }

    procNames.clear();
    for (rapidjson::SizeType i{0U}; i < value.Size(); i++) {
        rapidjson::Value const& object{value[i]};
        procNames.emplace_back(object.GetString());
    }
    return true;
}

/// @brief Parse and get data from trace cancel packet
/// @param procNames Parsed process list
/// @return true/false
bool DCP::GetValue4TraceCancel(ara::core::Vector< ara::core::String >& procNames) const noexcept
{
    if (GetType() != Type::kTraceCancel) {
        return false;
    }

    rapidjson::Document document;
    ara::core::StringView const jsonSv{GetContent()};
    std::ignore = document.Parse(jsonSv.data(), jsonSv.size());
    if (!document.IsObject()) {
        return false;
    }
    rapidjson::Value& value{document[GetJsonValue()]};
    if (!value.IsArray()) {
        return false;
    }

    procNames.clear();
    for (rapidjson::SizeType i{0U}; i < value.Size(); i++) {
        rapidjson::Value const& object{value[i]};
        procNames.emplace_back(object.GetString());
    }
    return true;
}

/// @brief Parse and get function group name from request packet
/// @param fgName Parsed function group name
/// @return true/false
bool DCP::GetValue4RequestFunctionGroup(ara::core::String& fgName) const noexcept
{
    std::ignore = fgName;
    rapidjson::Document document;
    ara::core::StringView const jsonSv{GetContent()};
    std::ignore = document.Parse(jsonSv.data(), jsonSv.size());
    if (!document.IsObject()) {
        return false;
    }
    rapidjson::Value& value{document[GetJsonValue()]};
    if (!value.IsString()) {
        return false;
    }
    fgName = value.GetString();
    return true;
}

/// @brief Parse and get process group name from request packet
/// @param procName Parsed process group name
/// @return true/false
bool DCP::GetValue4RequestProcess(ara::core::String& procName) const noexcept
{
    std::ignore = procName;
    rapidjson::Document document;
    ara::core::StringView const jsonSv{GetContent()};
    std::ignore = document.Parse(jsonSv.data(), jsonSv.size());
    if (!document.IsObject()) {
        return false;
    }

    rapidjson::Value& value{document[GetJsonValue()]};
    if (!value.IsString()) {
        return false;
    }

    procName = value.GetString();
    return true;
}

///////////////// Assembly /////////////////////

/// @brief Common function to assemble function group information packet
/// @param type Packet type
/// @param fg Function group information
/// @return true/false
bool DCP::_Pack4FunctionGroup(Type const type, FunctionGroupInfo const& fg) noexcept
{
 /// Check packet type
    std::string typeStr{};
    switch (type) {
        case Type::kResponseFunctionGroup: {
            typeStr = GetJsonTypeResponseFunctionGroup();
            break;
        }
        case Type::kNotifyFunctionGroup: {
            typeStr = GetJsonTypeNotifyFunctionGroup();
            break;
        }
        default: {
            break;
        }
    }

    if (typeStr.empty()) {
        return false;
    }

 /// Create json object
    rapidjson::Document document;
    std::ignore = document.SetObject();
    rapidjson::Document::AllocatorType& allocator{document.GetAllocator()};

 /// Create process list
    rapidjson::Value processList{rapidjson::kArrayType};
    for (auto const& proc : fg.GetProcessList()) {
        rapidjson::Value procObj{rapidjson::kObjectType};
        std::ignore = procObj.AddMember(rapidjson::StringRef(GetJsonValuePid()), proc.GetPid(), allocator);
        std::ignore = procObj.AddMember(rapidjson::StringRef(GetJsonValueName()),
                                        rapidjson::StringRef(proc.GetName().c_str()), allocator);
        std::ignore = procObj.AddMember(rapidjson::StringRef(GetJsonValueState()),
                                        rapidjson::StringRef(proc.GetState().c_str()), allocator);
        std::ignore = procObj.AddMember(rapidjson::StringRef(GetJsonValueExitCode()), proc.GetExitCode(), allocator);
        std::ignore = processList.PushBack(procObj, allocator);
    }

 /// Create function group object
    rapidjson::Value value{rapidjson::kObjectType};
    std::ignore = value.AddMember(rapidjson::StringRef(GetJsonValueName()), rapidjson::StringRef(fg.GetName().c_str()),
                                  allocator);
    std::ignore = value.AddMember(rapidjson::StringRef(GetJsonValueState()),
                                  rapidjson::StringRef(fg.GetState().c_str()), allocator);
    std::ignore = value.AddMember(rapidjson::StringRef(GetJsonValueProcessList()), processList, allocator);

 /// Add type and value members to json object
    std::ignore
        = document.AddMember(rapidjson::StringRef(GetJsonType()), rapidjson::StringRef(typeStr.c_str()), allocator);
    std::ignore = document.AddMember(rapidjson::StringRef(GetJsonValue()), value, allocator);

 /// Generate json string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
    std::ignore = document.Accept(writer);

 /// Generate packet
    ara::core::StringView const content{buffer.GetString()};
    std::uint16_t const frameSize{Header::CalculateFrameSize(content)};
    frame_.resize(static_cast< size_t >(frameSize));
    Header* const h{new (frame_.data()) Header()};
    h->SetContent(content);
    h->SetSize(frameSize);

    return true;
}

/// @brief Common function to assemble process information packet
/// @param type Packet type
/// @param proc Process information
/// @return true/false
bool DCP::_Pack4Process(Type const type, ProcessInfo const& proc) noexcept
{
 /// Check packet type
    std::string typeStr{};
    switch (type) {
        case Type::kResponseProcess: {
            typeStr = GetJsonTypeResponseProcess();
            break;
        }
        case Type::kNotifyProcess: {
            typeStr = GetJsonTypeNotifyProcess();
            break;
        }
        case Type::kTraceResponse: {
            typeStr = GetJsonTypeTraceResponse();
            break;
        }
        default: {
            break;
        }
    }

    if (typeStr.empty()) {
        return false;
    }

 /// Create json object
    rapidjson::Document document;
    std::ignore = document.SetObject();
    rapidjson::Document::AllocatorType& allocator{document.GetAllocator()};

 /// Create process object
    rapidjson::Value value{rapidjson::kObjectType};
    std::ignore = value.AddMember(rapidjson::StringRef(GetJsonValuePid()), proc.GetPid(), allocator);
    std::ignore = value.AddMember(rapidjson::StringRef(GetJsonValueName()),
                                  rapidjson::StringRef(proc.GetName().c_str()), allocator);
    std::ignore = value.AddMember(rapidjson::StringRef(GetJsonValueState()),
                                  rapidjson::StringRef(proc.GetState().c_str()), allocator);
    std::ignore = value.AddMember(rapidjson::StringRef(GetJsonValueExitCode()), proc.GetExitCode(), allocator);

 /// Add type and value members to json object
    std::ignore
        = document.AddMember(rapidjson::StringRef(GetJsonType()), rapidjson::StringRef(typeStr.c_str()), allocator);
    std::ignore = document.AddMember(rapidjson::StringRef(GetJsonValue()), value, allocator);

 /// Generate json string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
    std::ignore = document.Accept(writer);

 /// Generate packet
    uint16_t const frameSize{Header::CalculateFrameSize(buffer.GetString())};
    frame_.resize(static_cast< size_t >(frameSize));
    Header* const h{new (frame_.data()) Header()};
    ara::core::StringView const content{buffer.GetString()};
    h->SetContent(content);
    h->SetSize(frameSize);

    return true;
}

#ifdef ARA_ENABLE_TESTS
/// @brief Assemble TraceRequest packet for testing
/// @param procList Process list
/// @return true/false
bool DCP::Pack4TraceRequest(ara::core::Vector< ara::core::String > const& procList) noexcept
{
 /// Create json object
    rapidjson::Document document;
    std::ignore = document.SetObject();
    rapidjson::Document::AllocatorType& allocator{document.GetAllocator()};

 /// Create process list object
    rapidjson::Value procListObj{rapidjson::kArrayType};
    for (auto const& procName : procList) {  // PRQA S 2961
        std::ignore = procListObj.PushBack(rapidjson::StringRef(procName.c_str()), allocator);
    }

 /// Add type and value members to json object
    std::ignore = document.AddMember(rapidjson::StringRef(GetJsonType()),
                                     rapidjson::StringRef(GetJsonTypeTraceRequest()), allocator);
    std::ignore = document.AddMember(rapidjson::StringRef(GetJsonValue()), procListObj, allocator);

 /// Generate json string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
    std::ignore = document.Accept(writer);

 /// Generate packet
    uint16_t const frameSize{Header::CalculateFrameSize(buffer.GetString())};
    frame_.resize(static_cast< size_t >(frameSize));
    Header* const h{new (frame_.data()) Header()};
    ara::core::StringView const content{buffer.GetString()};
    h->SetContent(content);
    h->SetSize(frameSize);

    return true;
}

/// @brief Assemble RequestFunctionGroup packet for testing
/// @param fgName Function group name
/// @return true/false
bool DCP::Pack4RequestFunctionGroup(ara::core::String const& fgName) noexcept
{
 /// Create json object
    rapidjson::Document document;
    std::ignore = document.SetObject();
    rapidjson::Document::AllocatorType& allocator{document.GetAllocator()};

 /// Add type and value members to json object
    std::ignore = document.AddMember(rapidjson::StringRef(GetJsonType()),
                                     rapidjson::StringRef(GetJsonTypeRequestFunctionGroup()), allocator);
    std::ignore
        = document.AddMember(rapidjson::StringRef(GetJsonValue()), rapidjson::StringRef(fgName.c_str()), allocator);

 /// Generate json string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
    std::ignore = document.Accept(writer);

 /// Generate packet
    uint16_t const frameSize{Header::CalculateFrameSize(buffer.GetString())};
    frame_.resize(static_cast< size_t >(frameSize));
    Header* const h{new (frame_.data()) Header()};
    ara::core::StringView const content{buffer.GetString()};
    h->SetContent(content);
    h->SetSize(frameSize);

    return true;
}

/// @brief Assemble RequestProcess packet for testing
/// @param procName Process list
/// @return true/false
bool DCP::Pack4RequestProcess(ara::core::String const& procName) noexcept
{
 /// Create json object
    rapidjson::Document document;
    std::ignore = document.SetObject();
    rapidjson::Document::AllocatorType& allocator{document.GetAllocator()};

 /// Add type and value members to json object
    std::ignore = document.AddMember(rapidjson::StringRef(GetJsonType()),
                                     rapidjson::StringRef(GetJsonTypeRequestProcess()), allocator);
    std::ignore
        = document.AddMember(rapidjson::StringRef(GetJsonValue()), rapidjson::StringRef(procName.c_str()), allocator);

 /// Generate json string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
    std::ignore = document.Accept(writer);

 /// Generate packet
    uint16_t const frameSize{Header::CalculateFrameSize(buffer.GetString())};
    frame_.resize(static_cast< size_t >(frameSize));
    Header* const h{new (frame_.data()) Header()};
    ara::core::StringView const content{buffer.GetString()};
    h->SetContent(content);
    h->SetSize(frameSize);

    return true;
}
#endif

/// @brief Print debug information
void DCP::Debug() const noexcept
{
    DCP::Header const* const h{reinterpret_cast< DCP::Header const* >(frame_.data())};  // PRQA S 3049
    std::cout << "+++ DCP Frame +++" << std::endl;
    std::ignore = printf("magicId: %#x\n", h->GetMagicId());
    std::ignore = printf("   size: %d\n", h->GetSize());
    std::ignore = printf("content: ");
    ara::core::String const json{h->GetContent().data()};
    for (std::uint16_t i{0U}; i < h->GetContentSize(); i++) {
        std::ignore = putchar(static_cast< int32_t >(json[static_cast< size_t >(i)]));
    }
    Char8_t const terminator{'\n'};
    std::ignore = putchar(terminator);
    std::cout << "--- DCP Frame ---" << std::endl;
}

}  // namespace rds
}  // namespace internal
}  // namespace exec
}  // namespace ara