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
/// @file       dlt_protocol_encoder.cpp
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

#include "dlt_protocol_encoder.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <utility>

#include "DltLog/log_manager.h"
#include "Utils/src/private_log.h"
#include "logger_api.h"
//// Unique data needed for encoding one piece of data
namespace ara {
namespace log {
namespace internal {

/// @brief Default constructor, uses an internal exclusive buffer
DltProtocolEncoder::DltProtocolEncoder() noexcept
{
    bufferCapacity_ = kLogBufferSize;
    ownedBuffer_    = std::make_unique< BufferType >(bufferCapacity_);
    buffer_         = ownedBuffer_->data();
    standardheader_ = reinterpret_cast< DltStandardHeader * >(buffer_);
    if (bufferCapacity_ > kHeaderSegmentSize) {
        bodybuffer_   = buffer_ + kHeaderSegmentSize;
        bodyCapacity_ = bufferCapacity_ - kHeaderSegmentSize;
    } else {
        bodybuffer_   = nullptr;
        bodyCapacity_ = 0U;
    }
}

DltProtocolEncoder::~DltProtocolEncoder() noexcept {}

void DltProtocolEncoder::SetExternalBuffer(std::uint8_t *externalBuffer, std::size_t externalCapacity) noexcept
{
    if ((externalBuffer != nullptr) && (externalCapacity > 0U)) {
        ownedBuffer_.reset();
        buffer_         = externalBuffer;
        bufferCapacity_ = externalCapacity;
        standardheader_ = reinterpret_cast< DltStandardHeader * >(buffer_);
        if (bufferCapacity_ > kHeaderSegmentSize) {
            bodybuffer_   = buffer_ + kHeaderSegmentSize;
            bodyCapacity_ = bufferCapacity_ - kHeaderSegmentSize;
        } else {
            bodybuffer_   = nullptr;
            bodyCapacity_ = 0U;
        }
    }
    payloadSize_ = 0U;
    argsNum_     = 0U;
    headerSize_  = 0U;

    encoded_ = false;
}

void DltProtocolEncoder::InitModeledMsgId(std::uint32_t msgid) noexcept
{
    std::uint8_t *payloadData = this->PayloadData();
    if ((payloadData == nullptr) || (BufferCapacity() < sizeof(std::uint32_t))) {
        return;
    }

    std::ignore         = memcpy(payloadData, &msgid, sizeof(std::uint32_t));
    this->PayloadSize() = sizeof(std::uint32_t);
    this->SetArgsNum(0U);
    this->SetEncoded(false);
}

bool DltProtocolEncoder::AppendToPayload(void const *src, std::size_t len) noexcept
{
    std::uint8_t *payloadData = this->PayloadData();
    if (!HasPayloadCapacity(len) || (payloadData == nullptr) || (src == nullptr)) {
        return false;
    }

    std::ignore = memcpy(payloadData + payloadSize_, src, len);
    payloadSize_ += len;
    return true;
}

bool DltProtocolEncoder::PreparePayloadWrite(std::size_t need,
                                             std::uint8_t *&payloadData,
                                             std::size_t *&payloadSize) noexcept
{
    payloadData = this->PayloadData();
    if (payloadData == nullptr) {
        return false;
    }

    payloadSize = &this->PayloadSize();
    if (((*payloadSize) + need) > BufferCapacity()) {
        return false;
    }

    return true;
}

std::uint16_t DltProtocolEncoder::CalcUtf8TailReduce(Char8_T const *tail3) noexcept
{
    if (tail3 == nullptr) {
        return 0U;
    }

    std::uint16_t reduceSize{0U};
    constexpr std::uint8_t kUtf8ContinuationMask    = 0x80;  // 10000000
    constexpr std::uint8_t kUtf8ContinuationHighBit = 0x40;  // 01000000
    constexpr std::uint8_t kUtf8ThreeBytePrefix     = 0xE0;  // 11100000
    constexpr std::uint8_t kUtf8FourBytePrefix      = 0xF0;  // 11110000

    if ((tail3[2] & kUtf8ContinuationMask) != 0) {
        if ((tail3[2] & kUtf8ContinuationHighBit) != 0) {
            reduceSize = 1U;
        } else if ((tail3[1] & kUtf8ThreeBytePrefix) == kUtf8ThreeBytePrefix) {
            reduceSize = 2U;
        } else if ((tail3[0] & kUtf8FourBytePrefix) == kUtf8FourBytePrefix) {
            reduceSize = 3U;
        }
    }

    return reduceSize;
}

void DltProtocolEncoder::CopyDltId4(char *dst4, std::string const &src) noexcept
{
    if (dst4 == nullptr) {
        return;
    }
    std::size_t const kMaxIdLen{4U};
    std::size_t const copyLen{(std::min)(src.length(), kMaxIdLen)};
    std::ignore = memcpy(static_cast< void * >(dst4), src.c_str(), copyLen);
}

bool DltProtocolEncoder::BuildStandardHeaderCommon(bool withUeh,
                                                   DltStandardHeader *&standardheader,
                                                   DltStandardHeaderExtra *&headerExtra,
                                                   std::uint32_t &baseHeaderSize) noexcept
{
    std::uint8_t *headerbuffer = this->HeaderData();
    if (headerbuffer == nullptr) {
        return false;
    }

    standardheader = reinterpret_cast< internal::DltStandardHeader * >(headerbuffer);
    headerExtra    = reinterpret_cast< DltStandardHeaderExtra * >(headerbuffer + sizeof(DltStandardHeader));

    standardheader->htyp = kDlt_Htyp_Protocol_Version_N1;
    standardheader->htyp |= kDlt_Htyp_Weid;
    standardheader->htyp |= kDlt_Htyp_Wtms;
    standardheader->htyp |= kDlt_Htyp_Wsid;
    if (withUeh) {
        standardheader->htyp |= kDlt_Htyp_Ueh;
    }
#if (BYTE_ORDER == BIG_ENDIAN)
    standardheader->htyp |= kDlt_Htyp_Msbf;
#endif
    standardheader->mcnt = GetMsgCount();

    auto const &ctx = GetEncodeContext();
    CopyDltId4(reinterpret_cast< char * >(headerExtra->ecu), ctx.ecuId);
    headerExtra->seid = htobe32(ctx.sessionId);
    headerExtra->tmsp = htobe32(this->GetDltIntTime());

    baseHeaderSize = static_cast< std::uint32_t >(sizeof(internal::DltStandardHeader)
                                                  + DLT_STANDARD_HEADER_EXTRA_SIZE(standardheader->htyp));
    return true;
}

bool DltProtocolEncoder::FinalizeMessageLength(std::uint32_t headerSize, std::size_t payloadSize) noexcept
{
    if (this->StandardHeaderField() == nullptr) {
        return false;
    }

    std::uint32_t const len{headerSize + static_cast< std::uint32_t >(payloadSize)};
    if (len > std::numeric_limits< std::uint16_t >::max()) {
        return false;
    }

    this->StandardHeaderField()->len = htobe16(static_cast< std::uint16_t >(len));
    return true;
}

void DltProtocolEncoder::SetMsgCount(std::uint8_t msgCount) noexcept { msgcnt_ = msgCount; }

std::uint8_t DltProtocolEncoder::GetMsgCount() const noexcept { return this->msgcnt_; }

std::uint8_t DltProtocolEncoder::GetArgsNum() const noexcept { return this->argsNum_; }

void DltProtocolEncoder::SetArgsNum(std::uint8_t argsNum) noexcept { this->argsNum_ = argsNum; }

void DltProtocolEncoder::IncreaseArgsNum() noexcept { ++this->argsNum_; }

LogLevel DltProtocolEncoder::GetLineLogLevel() const noexcept { return mLineLogLevel_; }

void DltProtocolEncoder::SetLineLogLevel(LogLevel lineLevel) noexcept { mLineLogLevel_ = lineLevel; }

void DltProtocolEncoder::SetEncodeContext(std::shared_ptr< const EncodeContext > ctx) noexcept
{
    mEncodeContextPtr_ = std::move(ctx);
}

const EncodeContext &DltProtocolEncoder::GetEncodeContext() const noexcept
{
    static const EncodeContext kDefault{};
    return mEncodeContextPtr_ ? *mEncodeContextPtr_ : kDefault;
}

bool DltProtocolEncoder::SetDltTime(DltTimeStamp const &dltTimeStamp) noexcept
{
    mDltTimeStamp_ = dltTimeStamp;
    return true;
}

DltTimeStamp &DltProtocolEncoder::GetDltTimeStamp() noexcept { return this->mDltTimeStamp_; }

std::uint32_t DltProtocolEncoder::GetDltIntTime() const noexcept
{
    return this->mDltTimeStamp_.seconds * 10000 + this->mDltTimeStamp_.uSeconds / 100;
}

bool DltProtocolEncoder::IsEncoded() noexcept { return encoded_; }

void DltProtocolEncoder::SetEncoded(bool encode) noexcept { encoded_ = encode; }

void DltProtocolEncoder::AppendBool(bool value) noexcept
{
    std::uint8_t data = value ? 1U : 0U;
    AppendGeneric(&data, sizeof(std::uint8_t), MakeTypeInfo(kDlt_Type_Info_Bool, kDlt_Tyle_8_Bit));
}

void DltProtocolEncoder::AppendUInt8(std::uint8_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::uint8_t), MakeTypeInfo(kDlt_Type_Info_Uint, kDlt_Tyle_8_Bit));
}

void DltProtocolEncoder::AppendUInt16(std::uint16_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::uint16_t), MakeTypeInfo(kDlt_Type_Info_Uint, kDlt_Tyle_16_Bit));
}

void DltProtocolEncoder::AppendUInt32(std::uint32_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::uint32_t), MakeTypeInfo(kDlt_Type_Info_Uint, kDlt_Tyle_32_Bit));
}

void DltProtocolEncoder::AppendUInt64(std::uint64_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::uint64_t), MakeTypeInfo(kDlt_Type_Info_Uint, kDlt_Tyle_64_Bit));
}

void DltProtocolEncoder::AppendInt8(std::int8_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::int8_t), MakeTypeInfo(kDlt_Type_Info_Sint, kDlt_Tyle_8_Bit));
}

void DltProtocolEncoder::AppendInt16(std::int16_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::int16_t), MakeTypeInfo(kDlt_Type_Info_Sint, kDlt_Tyle_16_Bit));
}

void DltProtocolEncoder::AppendInt32(std::int32_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::int32_t), MakeTypeInfo(kDlt_Type_Info_Sint, kDlt_Tyle_32_Bit));
}

void DltProtocolEncoder::AppendInt64(std::int64_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::int64_t), MakeTypeInfo(kDlt_Type_Info_Sint, kDlt_Tyle_64_Bit));
}

void DltProtocolEncoder::AppendFloat(float value) noexcept
{
    AppendGeneric(&value, sizeof(float), MakeTypeInfo(kDlt_Type_Info_Floa, kDlt_Tyle_32_Bit));
}

void DltProtocolEncoder::AppendDouble(double value) noexcept
{
    AppendGeneric(&value, sizeof(double), MakeTypeInfo(kDlt_Type_Info_Floa, kDlt_Tyle_64_Bit));
}

void DltProtocolEncoder::AppendHex8(std::uint8_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::uint8_t), MakeTypeInfo(kDlt_Type_Info_Uint, kDlt_Tyle_8_Bit, kDlt_Scod_Hex));
}

void DltProtocolEncoder::AppendHex16(std::uint16_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::uint16_t), MakeTypeInfo(kDlt_Type_Info_Uint, kDlt_Tyle_16_Bit, kDlt_Scod_Hex));
}

void DltProtocolEncoder::AppendHex32(std::uint32_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::uint32_t), MakeTypeInfo(kDlt_Type_Info_Uint, kDlt_Tyle_32_Bit, kDlt_Scod_Hex));
}

void DltProtocolEncoder::AppendHex64(std::uint64_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::uint64_t), MakeTypeInfo(kDlt_Type_Info_Uint, kDlt_Tyle_64_Bit, kDlt_Scod_Hex));
}

void DltProtocolEncoder::AppendBin8(std::uint8_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::uint8_t), MakeTypeInfo(kDlt_Type_Info_Uint, kDlt_Tyle_8_Bit, kDlt_Scod_Bin));
}

void DltProtocolEncoder::AppendBin16(std::uint16_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::uint16_t), MakeTypeInfo(kDlt_Type_Info_Uint, kDlt_Tyle_16_Bit, kDlt_Scod_Bin));
}

void DltProtocolEncoder::AppendBin32(std::uint32_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::uint32_t), MakeTypeInfo(kDlt_Type_Info_Uint, kDlt_Tyle_32_Bit, kDlt_Scod_Bin));
}

void DltProtocolEncoder::AppendBin64(std::uint64_t value) noexcept
{
    AppendGeneric(&value, sizeof(std::uint64_t), MakeTypeInfo(kDlt_Type_Info_Uint, kDlt_Tyle_64_Bit, kDlt_Scod_Bin));
}

void DltProtocolEncoder::AppendCString(const char *value) noexcept
{
    if (value != nullptr) {
        AppendString(value);
    }
}

void DltProtocolEncoder::AppendBytes(std::vector< std::uint8_t > const &value) noexcept
{
    if (value.empty()) {
        return;
    }
    std::size_t const capped
        = (std::min)(value.size(), static_cast< std::size_t >(std::numeric_limits< std::uint16_t >::max()));
    AppendRawData(value.data(), static_cast< std::uint16_t >(capped), DltFormatType::kDlt_Format_Default, "", false);
}

std::size_t DltProtocolEncoder::BufferCapacity() noexcept { return bodyCapacity_; }

std::size_t &DltProtocolEncoder::PayloadSize() noexcept { return payloadSize_; }

std::size_t DltProtocolEncoder::PayloadSize() const noexcept { return payloadSize_; }

std::uint8_t &DltProtocolEncoder::ArgsNum() noexcept { return argsNum_; }

std::uint8_t DltProtocolEncoder::ArgsNum() const noexcept { return argsNum_; }

std::uint8_t *DltProtocolEncoder::HeaderData() noexcept { return buffer_; }

std::uint8_t const *DltProtocolEncoder::HeaderData() const noexcept { return buffer_; }

std::uint32_t &DltProtocolEncoder::HeaderSizeField() noexcept { return headerSize_; }

DltStandardHeader *&DltProtocolEncoder::StandardHeaderField() noexcept { return standardheader_; }

DltStandardHeader const *DltProtocolEncoder::StandardHeaderField() const noexcept { return standardheader_; }

std::uint8_t *DltProtocolEncoder::PayloadData() noexcept { return bodybuffer_; }

std::uint8_t const *DltProtocolEncoder::PayloadData() const noexcept { return bodybuffer_; }

bool DltProtocolEncoder::HasPayloadCapacity(std::size_t need) const noexcept
{
    return (bodybuffer_ != nullptr) && ((payloadSize_ + need) <= bodyCapacity_);
}

void DltProtocolEncoder::ResetEncodingState() noexcept
{
    payloadSize_    = 0U;
    argsNum_        = 0U;
    headerSize_     = 0U;
    standardheader_ = nullptr;
    SetEncoded(false);
}

}  // namespace internal
}  // namespace log
}  // namespace ara
