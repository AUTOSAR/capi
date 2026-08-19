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
/// @file       dcp.h
/// @brief      Debug communication protocol
/// @details
/// @date       2023-11-17
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_DEBUG_DCP_H_
#define _ARA_EXEC_INTERNAL_DEBUG_DCP_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>

#include <cstdint>

#include "ara/exec/internal/rds/debug_info.h"
#include "isoft/utils/endian.h"

namespace ara {
namespace exec {
namespace internal {
namespace rds {

/// @brief 8-bit character type
using Char8_t = char;

/// @brief Debug communication protocol
class DCP
{
public:
 /// @brief Message header information class
    class Header
    {
    public:
 /// @brief Constructor
        Header() = default;

 /// @brief Destructor
        ~Header() = default;

 /// @brief Default copy constructor
        /// @param  other
        Header(Header const &other) = delete;

 /// @brief Default move constructor
        /// @param  other
        Header(Header &&other) = delete;

 /// @brief Default copy assignment function
        /// @param other the other instance
 /// @return New Header
        Header &operator=(Header const &other) = delete;

 /// @brief Default move assignment function
        /// @param other the other instance
 /// @return New Header
        Header &operator=(Header &&other) = delete;

 /// @brief Header validation
 /// @return true: validation passed; false: validation failed
        bool Validate() const noexcept
        {
            if (kMagicKey == isoft::utils::NtoH16(kMagicId)) {
                return true;
            }

            return false;
        }

 /// @brief Get frame header size
 /// @return Frame header size
        static constexpr uint16_t GetHeaderSize() noexcept { return __builtin_offsetof(Header, content_); }

 /// @brief Calculate frame size
 /// @param content Frame content
 /// @return Frame size
        static uint16_t CalculateFrameSize(Char8_t const *const content) noexcept
        {
            return static_cast< uint16_t >(GetHeaderSize() + ara::core::StringView(content).size());
        }

 /// @brief Calculate frame size
 /// @param content Frame content
 /// @return Frame size
        static uint16_t CalculateFrameSize(ara::core::StringView const &content) noexcept
        {
            return static_cast< uint16_t >(GetHeaderSize() + content.size());
        }

 /// @brief Get magic number of frame header
 /// @return Magic number
        uint16_t GetMagicId() const noexcept { return isoft::utils::NtoH16(kMagicId); }

 /// @brief Set frame size
 /// @param size Frame size
        void SetSize(uint16_t const size) noexcept { size_ = isoft::utils::HtoN16(size); }

 /// @brief Get frame size
 /// @return Frame size
        uint16_t GetSize() const noexcept { return isoft::utils::NtoH16(size_); }

 /// @brief Get frame content
 /// @return Frame content
        ara::core::StringView const GetContent() const noexcept
        {
            return ara::core::StringView{static_cast< Char8_t const * >(content_),
                                         static_cast< size_t >(GetContentSize())};
        }

 /// @brief Set frame content
 /// @param content Frame content
        void SetContent(ara::core::StringView const &content) noexcept
        {
            std::ignore = strncpy(static_cast< Char8_t * >(content_), content.data(), content.size());
        }

 /// @brief Get frame content size
 /// @return Frame content size
        uint16_t GetContentSize() const noexcept
        {
            uint16_t res{0U};
            uint16_t const msgSize{GetSize()};
            uint16_t const headerSize{GetHeaderSize()};
            if (msgSize >= headerSize) {
                res = msgSize - headerSize;
            }
            return res;
        }

    private:
 /// @brief Protocol magic id
        uint16_t const kMagicId{isoft::utils::HtoN16(kMagicKey)};

 /// @brief Message size
        uint16_t size_{0U};

 /// @brief Message content
        Char8_t content_[1]{0};  // PRQA S 4151
    };

 /// @brief Packet type
    enum class Type : uint8_t
    {
 kUnknown = 0, ///< Unknown type
 kTraceRequest = 0x11, ///< Trace request
 kTraceResponse = 0x12, ///< Trace response
 kTraceCancel = 0x13, ///< Cancel trace
 kRequestFunctionGroup = 0x21, ///< Request function group information
 kResponseFunctionGroup = 0x22, ///< Response function group information
 kRequestProcess = 0x23, ///< Request process information
 kResponseProcess = 0x24, ///< Response process information
 kNotifyFunctionGroup = 0x31, ///< Notify function group information
 kNotifyProcess = 0x32 ///< Notify process information
    };

 /// @brief Disable default constructor
    DCP() = delete;

 /// @brief Constructor
 /// @param bytes Output or input frame buffer
    explicit DCP(ara::core::Vector< Char8_t > &bytes) noexcept : frame_{bytes} {}
 /// @brief Default destructor
    ~DCP() = default;

 /// @brief Default copy constructor
    /// @param  other
    DCP(DCP &other) = delete;

 /// @brief Default move constructor
    /// @param  other
    DCP(DCP &&other) = delete;

 /// @brief Default copy assignment function
    /// @param other the other instance
 /// @return New DCP
    DCP &operator=(DCP const &other) = delete;

 /// @brief Default move assignment function
    /// @param other the other instance
 /// @return New DCP
    DCP &operator=(DCP &&other) = delete;

 /// @brief Parse and get packet type
 /// @return Type
    Type GetType() const noexcept;

 /// @brief Get content (json string)
 /// @return json format message content
    ara::core::StringView const GetContent() const noexcept;

 /// @brief Parse and get data from trace request packet
 /// @param procNames Parsed process list
    /// @return true/false
    bool GetValue4TraceRequest(ara::core::Vector< ara::core::String > &procNames) const noexcept;

 /// @brief Parse and get data from trace cancel packet
 /// @param procNames Parsed process list
    /// @return true/false
    bool GetValue4TraceCancel(ara::core::Vector< ara::core::String > &procNames) const noexcept;

 /// @brief Parse and get function group name from request packet
 /// @param fgName Parsed function group name
    /// @return true/false
    bool GetValue4RequestFunctionGroup(ara::core::String &fgName) const noexcept;

 /// @brief Parse and get process group name from request packet
 /// @param procName Parsed process group name
    /// @return true/false
    bool GetValue4RequestProcess(ara::core::String &procName) const noexcept;

 /// @brief Assemble function group information response packet
 /// @param fg Function group information
    /// @return true/false
    bool Pack4ReponseFunctionGroup(FunctionGroupInfo const &fg) noexcept
    {
        return _Pack4FunctionGroup(Type::kResponseFunctionGroup, fg);
    }

 /// @brief Assemble process information response packet
 /// @param proc Process information
    /// @return true/false
    bool Pack4ResponseProcess(ProcessInfo const &proc) noexcept { return _Pack4Process(Type::kResponseProcess, proc); }

 /// @brief Assemble process trace notification packet
 /// @param proc Process information
 /// @return true assembly success; false assembly failure
    bool Pack4TraceResponse(ProcessInfo const &proc) noexcept { return _Pack4Process(Type::kTraceResponse, proc); }

 /// @brief Print debug information
    void Debug() const noexcept;

#ifdef ARA_ENABLE_TESTS
 /// @brief Assemble TraceRequest packet for testing
 /// @param procList Process list
    /// @return true/false
    bool Pack4TraceRequest(ara::core::Vector< ara::core::String > const &procList) noexcept;

 /// @brief Assemble RequestFunctionGroup packet for testing
 /// @param fgName Function group name
    /// @return true/false
    bool Pack4RequestFunctionGroup(ara::core::String const &fgName) noexcept;

 /// @brief Assemble RequestProcess packet for testing
 /// @param procName Process list
    /// @return true/false
    bool Pack4RequestProcess(ara::core::String const &procName) noexcept;

 /// @brief Assemble function group information notification packet
 /// @param fg Function group information
    /// @return true/false
    bool Pack4NotifyFunctionGroup(FunctionGroupInfo const &fg) noexcept
    {
        return _Pack4FunctionGroup(Type::kNotifyFunctionGroup, fg);
    }

 /// @brief Assemble process information notification packet
 /// @param proc Process information
    /// @return true/false
    bool Pack4NotifyProcess(ProcessInfo const &proc) noexcept { return _Pack4Process(Type::kNotifyProcess, proc); }
#endif

private:
 /// @brief Common function to assemble process information packet
 /// @param type Packet type
 /// @param proc Process information
    /// @return true/false
    bool _Pack4Process(Type const type, ProcessInfo const &proc) noexcept;

 /// @brief Common function to assemble function group information packet
 /// @param type Packet type
 /// @param fg Function group information
    /// @return true/false
    bool _Pack4FunctionGroup(Type const type, FunctionGroupInfo const &fg) noexcept;

 /// @brief Get Json type string
 /// @return Json type string
    static constexpr Char8_t const *GetJsonType() noexcept { return "type"; }

 /// @brief Get Json value string
 /// @return Json value string
    static constexpr Char8_t const *GetJsonValue() noexcept { return "value"; }

 /// @brief Get TraceRequest string
 /// @return TraceRequest string
    static constexpr Char8_t const *GetJsonTypeTraceRequest() noexcept { return "trace_request"; }

 /// @brief Get TraceResponse string
 /// @return TraceResponse string
    static constexpr Char8_t const *GetJsonTypeTraceResponse() noexcept { return "trace_response"; }

 /// @brief Get TraceCancel string
 /// @return TraceCancel string
    static constexpr Char8_t const *GetJsonTypeTraceCancel() noexcept { return "trace_cancel"; }

 /// @brief Get requested function group string
 /// @return Request function group string
    static constexpr Char8_t const *GetJsonTypeRequestFunctionGroup() noexcept { return "request_function_group"; }

 /// @brief Get Response function group string
 /// @return response function group string
    static constexpr Char8_t const *GetJsonTypeResponseFunctionGroup() noexcept { return "response_function_group"; }

 /// @brief Get request process string
 /// @return request process string
    static constexpr Char8_t const *GetJsonTypeRequestProcess() noexcept { return "request_process"; }

 /// @brief Get response process string
 /// @return response process string
    static constexpr Char8_t const *GetJsonTypeResponseProcess() noexcept { return "response_process"; }

 /// @brief Get notify process string
 /// @return notify process string
    static constexpr Char8_t const *GetJsonTypeNotifyProcess() noexcept { return "notify_process"; }

 /// @brief Get notify function group string
 /// @return notify function group string
    static constexpr Char8_t const *GetJsonTypeNotifyFunctionGroup() noexcept { return "notify_function_group"; }

 /// @brief Get name string
 /// @return name string
    static constexpr Char8_t const *GetJsonValueName() noexcept { return "name"; }

 /// @brief Get pid string
 /// @return pid string
    static constexpr Char8_t const *GetJsonValuePid() noexcept { return "pid"; }

 /// @brief Get state string
 /// @return state string
    static constexpr Char8_t const *GetJsonValueState() noexcept { return "state"; }

 /// @brief Get exit code string
 /// @return exit code string
    static constexpr Char8_t const *GetJsonValueExitCode() noexcept { return "exit_code"; }

 /// @brief Get process list string
 /// @return process list string
    static constexpr Char8_t const *GetJsonValueProcessList() noexcept { return "process_list"; }

private:
    /// @brief Magic key
    static constexpr uint16_t const kMagicKey{0x4441U};

 /// @brief Frame buffer reference
    ara::core::Vector< Char8_t > &frame_;
};

}  // namespace rds
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_DEBUG_DCP_H_