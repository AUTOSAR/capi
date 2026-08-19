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
/// @file       dlt_protocol_encoder.h
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltProtocol
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00005,LOG_SR_00008,LOG_SR_00010,LOG_SR_00015,LOG_SR_00016,LOG_SR_00017,LOG_SR_00018,LOG_SR_00019,LOG_SR_00020
/// @unit_name = dlt_protocol_encoder
/// @unit_description=Encoding function implemented by the Dlt lib, used for encoding logs into DLT format
/// @endcode
///
/// ================================================================

#ifndef __LOG_INTERNAL_DLTPROTOCOL_INTERFACE__
#define __LOG_INTERNAL_DLTPROTOCOL_INTERFACE__
#include <algorithm>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "argument.h"
#include "common.h"
#include "dlt_protocol_internal_types.h"
#include "dlt_protocol_types.h"
#include "log_api_define_v1.h"

namespace ara {
namespace log {
namespace internal {

/// @brief Encoder context (lightweight, for decoupling)
/// @note Used to replace the complete LoggerScopeData, reducing coupling between modules
struct EncodeContext
{
    std::string ecuId;
    std::string appId;
    std::string contextId;
    std::uint32_t sessionId{0U};
    bool sessionSupport{false};
    bool nonVerboseMode{true};
};

/// @brief Interface class for protocol encoding
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00218
/// @trace_id_dd=DD_LOG_00431
/// @needwork = ad
/// @endcode
class DltProtocolEncoder
{
public:
    enum EncodeError : std::int32_t
    {
        kEncodeOk              = 0,
        kEncodeNoArgs          = -1,
        kEncodeAlreadyEncoded  = -2,
        kEncodeMessageTooLarge = -3
    };

    /// @brief Constructor
    /// @param[in]  lineLevel  Log level
    /// @param[in]  msgcnt  Message count
    /// @param[in]  logtmsp  Timestamp
    /// @param[in]  loggerDataValue  Log data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00219
    /// @trace_id_dd=DD_LOG_00432
    /// @needwork = ad
    /// @endcode
    /// @brief Default constructor, uses an internal exclusive buffer
    DltProtocolEncoder() noexcept;

    /// @brief Inject an external raw buffer (optional release callback)
    /// @param[in] externalBuffer Starting address of the external raw buffer
    /// @param[in] externalCapacity Capacity of the external buffer
    void SetExternalBuffer(std::uint8_t *externalBuffer, std::size_t externalCapacity) noexcept;

    /// @brief Initialize the starting position of the payload for a modeled message ID
    /// @param[in] msgid Modeled message ID
    /// @note If SetExternalBuffer() is also called, this function must be called after SetExternalBuffer()
    void InitModeledMsgId(std::uint32_t msgid) noexcept;

    /// @brief Copy constructor, deleted
    /// @param[in]  other Other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00220
    /// @trace_id_dd=DD_LOG_00433
    /// @needwork = ad
    /// @endcode
    DltProtocolEncoder(DltProtocolEncoder const &other) = delete;
    /// @brief Move copy constructor, deleted
    /// @param[in]  other Other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_02344
    /// @trace_id_dd=DD_LOG_02743
    /// @needwork = ad
    /// @endcode
    DltProtocolEncoder(DltProtocolEncoder &&other) = delete;
    /// @brief Copy assignment
    /// @param[in]  other Other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_02345
    /// @trace_id_dd=DD_LOG_02744
    /// @needwork = ad
    /// @endcode

    DltProtocolEncoder &operator=(DltProtocolEncoder const &other) = delete;
    /// @brief Move assignment
    /// @param[in]  other Other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_02346
    /// @trace_id_dd=DD_LOG_02745
    /// @needwork = ad
    /// @endcode

    DltProtocolEncoder &operator=(DltProtocolEncoder &&other) = delete;
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_02347
    /// @trace_id_dd=DD_LOG_02746
    /// @needwork = ad
    /// @endcode

    virtual ~DltProtocolEncoder() noexcept;
    /// @brief Append a regular string
    /// @param[in]  str String
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00225
    /// @trace_id_dd=DD_LOG_00438
    /// @needwork = ad
    /// @endcode
    virtual void AppendString(std::string const &str) noexcept = 0;
    /// @brief Append raw data
    /// @param[in]  data Data
    /// @param[in]  length  Length
    /// @param[in]  type  Type
    /// @param[in]  name  Name
    /// @param[in]  with_var_info Info
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00226
    /// @trace_id_dd=DD_LOG_00439
    /// @needwork = ad
    /// @endcode
    virtual void AppendRawData(void const *data,
                               std::uint16_t const &length,
                               DltFormatType type,
                               std::string const &name,
                               bool const &withVarInfo) noexcept = 0;

    /// @brief Append generic data
    /// @param[in]  datap Pointer
    /// @param[in]  datalen  Length
    /// @param[in]  type_info  Type information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00227
    /// @trace_id_dd=DD_LOG_00440
    /// @needwork = ad
    /// @endcode
    virtual void AppendGeneric(const void *datap,
                               std::size_t const &datalen,
                               std::uint32_t const &typeInfo) noexcept = 0;

    /// @brief Log encoding
    /// @needwork = ad
    virtual std::int32_t Encode() noexcept = 0;

    /// @brief Get the current log sequence number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_10230
    /// @trace_id_dd=DD_LOG_10443
    /// @needwork = ad
    /// @endcode
    void SetMsgCount(std::uint8_t msgCount) noexcept;

    std::uint8_t GetMsgCount() const noexcept;

    std::uint8_t GetArgsNum() const noexcept;

    void SetArgsNum(std::uint8_t argsNum) noexcept;

    void IncreaseArgsNum() noexcept;

    /// @brief
    /// @needwork = ad
    LogLevel GetLineLogLevel() const noexcept;

    void SetLineLogLevel(LogLevel lineLevel) noexcept;

    /// @brief Set the encoding context (decoupled way)
    /// @param[in] ctx Key information required for encoding (held by external LoggerStaticContext, shared ownership)
    /// @note This method is recommended over SetLoggerData, reducing dependence on LoggerScopeData
    void SetEncodeContext(std::shared_ptr< const EncodeContext > ctx) noexcept;

    /// @brief Get the encoding context
    const EncodeContext &GetEncodeContext() const noexcept;
    /// @brief  Get memory pointer

    bool SetDltTime(DltTimeStamp const &dltTimeStamp) noexcept;

    virtual std::uint8_t *HeaderBuffer() noexcept = 0;
    virtual std::uint32_t HeaderSize() noexcept   = 0;
    virtual std::uint8_t *BodyBuffer() noexcept   = 0;
    virtual std::uint32_t BodySize() noexcept     = 0;

    DltTimeStamp &GetDltTimeStamp() noexcept;
    std::uint32_t GetDltIntTime() const noexcept;

    bool IsEncoded() noexcept;

    void SetEncoded(bool encode) noexcept;

    static constexpr std::uint32_t MakeTypeInfo(std::uint32_t baseType,
                                                std::uint32_t bitWidth,
                                                std::uint32_t scod = 0U) noexcept
    {
        return baseType | bitWidth | scod;
    }

    void AppendBool(bool value) noexcept;

    void AppendUInt8(std::uint8_t value) noexcept;

    void AppendUInt16(std::uint16_t value) noexcept;

    void AppendUInt32(std::uint32_t value) noexcept;

    void AppendUInt64(std::uint64_t value) noexcept;

    void AppendInt8(std::int8_t value) noexcept;

    void AppendInt16(std::int16_t value) noexcept;

    void AppendInt32(std::int32_t value) noexcept;

    void AppendInt64(std::int64_t value) noexcept;

    void AppendFloat(float value) noexcept;

    void AppendDouble(double value) noexcept;

    void AppendHex8(std::uint8_t value) noexcept;

    void AppendHex16(std::uint16_t value) noexcept;

    void AppendHex32(std::uint32_t value) noexcept;

    void AppendHex64(std::uint64_t value) noexcept;

    void AppendBin8(std::uint8_t value) noexcept;

    void AppendBin16(std::uint16_t value) noexcept;

    void AppendBin32(std::uint32_t value) noexcept;

    void AppendBin64(std::uint64_t value) noexcept;

    void AppendCString(const char *value) noexcept;

    void AppendBytes(std::vector< std::uint8_t > const &value) noexcept;

    using BufferType = std::vector< std::uint8_t >;
    static constexpr std::size_t kHeaderSegmentSize
        = sizeof(DltStandardHeader) + sizeof(DltStandardHeaderExtra) + sizeof(DltExtendedHeader);

    /// @brief Get the buffer capacity
    std::size_t BufferCapacity() noexcept;

    /// @brief Current written payload size
    std::size_t &PayloadSize() noexcept;
    std::size_t PayloadSize() const noexcept;

    /// @brief Parameter count
    std::uint8_t &ArgsNum() noexcept;
    std::uint8_t ArgsNum() const noexcept;

    /// @brief Access the starting address of the header area
    std::uint8_t *HeaderData() noexcept;
    std::uint8_t const *HeaderData() const noexcept;

    /// @brief Header related fields
    std::uint32_t &HeaderSizeField() noexcept;

    DltStandardHeader *&StandardHeaderField() noexcept;
    DltStandardHeader const *StandardHeaderField() const noexcept;

    /// @brief Access the starting address of payload data
    std::uint8_t *PayloadData() noexcept;
    std::uint8_t const *PayloadData() const noexcept;

    /// @brief Capacity check
    bool HasPayloadCapacity(std::size_t need) const noexcept;

    /// @brief Append data to the payload, return true on success
    bool AppendToPayload(void const *src, std::size_t len) noexcept;

    /// @brief Reset encoding state, convenient for reuse/unit testing
    void ResetEncodingState() noexcept;

protected:
    /// @brief Calculate the number of bytes to roll back for UTF-8 truncation boundary (up to 3 bytes)
    static std::uint16_t CalcUtf8TailReduce(Char8_T const *tail3) noexcept;

    /// @brief Check payload write conditions and return a writable pointer/size reference
    bool PreparePayloadWrite(std::size_t need, std::uint8_t *&payloadData, std::size_t *&payloadSize) noexcept;

    /// @brief Copy a DLT 4-character ID (copy actual length if less than 4 bytes, truncate if exceeded)
    static void CopyDltId4(char *dst4, std::string const &src) noexcept;

    /// @brief Build common fields of the standard header + extended standard header (ECU/SEID/TMSP/MCNT)
    bool BuildStandardHeaderCommon(bool withUeh,
                                   DltStandardHeader *&standardheader,
                                   DltStandardHeaderExtra *&headerExtra,
                                   std::uint32_t &baseHeaderSize) noexcept;

    /// @brief Unify total length verification and write back to standardheader->len
    bool FinalizeMessageLength(std::uint32_t headerSize, std::size_t payloadSize) noexcept;

private:
    static constexpr std::uint32_t kDlt_Type_Info_Bool{0x00000010U};

    LogLevel mLineLogLevel_{LogLevel::kVerbose};

    std::uint8_t msgcnt_;
    DltTimeStamp mDltTimeStamp_{};

    bool encoded_{false};

    /// @brief Encoding context (shared read-only, created by external LoggerStaticContext and passed in; falls back to static defaults when nullptr)
    std::shared_ptr< const EncodeContext > mEncodeContextPtr_{nullptr};

    /// @brief Payload buffer (internal exclusive)
    std::unique_ptr< BufferType > ownedBuffer_{nullptr};

    /// @brief Overall buffer start (start of the header segment)
    std::uint8_t *buffer_{nullptr};

    /// @brief Start of the payload area (buffer_ + kHeaderSegmentSize)
    std::uint8_t *bodybuffer_{nullptr};

    /// @brief Overall buffer capacity (header + payload)
    std::size_t bufferCapacity_{internal::kLogBufferSize};

    /// @brief Available payload capacity
    std::size_t bodyCapacity_{0U};

    /// @brief Payload write position
    std::size_t payloadSize_{0U};
    /// @brief Number of parameters
    std::uint8_t argsNum_{0U};

    std::uint32_t headerSize_{0U};

    DltStandardHeader *standardheader_{nullptr};

};  // namespace internal

}  // namespace internal
}  // namespace log
}  // namespace ara
#endif