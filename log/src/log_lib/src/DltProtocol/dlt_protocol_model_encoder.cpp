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
/// @file       dlt_protocol_model_encoder.cpp
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
/// @unit_name = dlt_protocol_model_encoder
/// @unit_description=Encoding function implemented by the Dlt lib, used for encoding logs into DLT format
/// @endcode
///
/// ================================================================

#include "dlt_protocol_model_encoder.h"

#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

#include "DltLog/log_manager.h"
#include "Utils/src/private_log.h"
#include "logger_api.h"
namespace ara {
namespace log {
namespace internal {

/// @brief
/// @param[in]  lineLevel
/// @param[in]  loggerData
DltProtocolModelEncoder::DltProtocolModelEncoder() noexcept
{
    LOGVERBOSE("enter: ") << __func__;
    ResetEncodingState();
}

/// @brief
/// @param[in]  str
void DltProtocolModelEncoder::AppendString(std::string const& str) noexcept
{
    std::uint8_t* payloadData = this->PayloadData();
    if (payloadData == nullptr) {
        return;
    }

    std::size_t& payloadSize = this->PayloadSize();

    if (str.empty()) {
        return;
    }
    constexpr std::size_t kPayloadHeaderSize = sizeof(std::uint16_t);
    constexpr std::size_t kMinSpaceNeeded    = kPayloadHeaderSize + 1U;
    if ((payloadSize + kMinSpaceNeeded) > BufferCapacity()) {
        return;
    }
    std::uint16_t argSize{static_cast< std::uint16_t >(str.length() + 1U)};
    static constexpr char kTruncatedMarker[]         = "...";
    static constexpr std::size_t kTruncatedMarkerLen = 4;  // including '\0'
    std::size_t const availablePayloadStrMsg{BufferCapacity() - payloadSize - kPayloadHeaderSize};
    std::size_t copyLength{str.length()};
    bool appendTruncatedSuffix{false};

    if (argSize > availablePayloadStrMsg) {
        if (availablePayloadStrMsg >= kTruncatedMarkerLen) {
            copyLength            = availablePayloadStrMsg - kTruncatedMarkerLen;
            appendTruncatedSuffix = true;
        } else {
            copyLength = availablePayloadStrMsg - 1U;
        }

        while ((copyLength > 0U) && (copyLength < str.length())
               && ((static_cast< std::uint8_t >(str[copyLength]) & 0xC0U) == 0x80U)) {
            copyLength--;
        }

        std::size_t const suffixLength{appendTruncatedSuffix ? kTruncatedMarkerLen : 1U};
        argSize = static_cast< std::uint16_t >(copyLength + suffixLength);
    }

    if (!AppendToPayload(&argSize, sizeof(std::uint16_t))) {
        return;
    }

    if (copyLength > 0U) {
        std::ignore = memcpy(payloadData + payloadSize, str.data(), copyLength);
        payloadSize += copyLength;
    }

    if (appendTruncatedSuffix) {
        std::ignore = memcpy(payloadData + payloadSize, kTruncatedMarker, kTruncatedMarkerLen);
        payloadSize += kTruncatedMarkerLen;
    } else {
        payloadData[payloadSize] = static_cast< std::uint8_t >('\0');
        payloadSize += 1U;
    }

    this->IncreaseArgsNum();
}
/// @brief
/// @param[in]  data
/// @param[in]  length
/// @param[in]  type
/// @param[in]  name
/// @param[in]  withVarInfo
void DltProtocolModelEncoder::AppendRawData(void const* data,
                                            std::uint16_t const& length,
                                            DltFormatType type,
                                            std::string const& name,
                                            bool const& withVarInfo) noexcept
{
    std::uint8_t* payloadData = this->PayloadData();
    if (payloadData == nullptr) {
        return;
    }

    std::size_t& payloadSize = this->PayloadSize();

    std::ignore = withVarInfo;
    std::ignore = name;
    if ((type < DltFormatType::kDlt_Format_Default) || (type >= DltFormatType::kDlt_Format_Max)) {
        LOGERROR(__func__) << "format error ";
        return;
    }

    std::size_t const neededSize{static_cast< std::size_t >(length) + sizeof(std::uint16_t)};
    if ((payloadSize + neededSize) > BufferCapacity()) {
        return;
    }

    if (!AppendToPayload(&length, sizeof(std::uint16_t))) {
        return;
    }

    std::ignore = memcpy(payloadData + payloadSize, data, static_cast< std::size_t >(length));
    payloadSize += length;

    this->IncreaseArgsNum();
}

/// @brief
/// @param[in]  datap
/// @param[in]  datalen
/// @param[in]  typeInfo
void DltProtocolModelEncoder::AppendGeneric(const void* datap,
                                            std::size_t const& datalen,
                                            std::uint32_t const& typeInfo) noexcept
{
    LOGVERBOSE("enter: ") << __func__;
    std::ignore = typeInfo;
    std::uint8_t* payloadData{nullptr};
    std::size_t* payloadSize{nullptr};
    if (!PreparePayloadWrite(datalen, payloadData, payloadSize)) {
        return;
    }
    std::ignore = memcpy(payloadData + (*payloadSize), datap, datalen);
    (*payloadSize) += datalen;
    this->IncreaseArgsNum();
}

/// @brief
std::int32_t DltProtocolModelEncoder::Encode() noexcept
{
    if (this->GetArgsNum() == 0U) {
        LOGERROR(__func__) << ": args <= 0";
        return static_cast< std::int32_t >(DltProtocolEncoder::EncodeError::kEncodeNoArgs);
    }
    if (this->IsEncoded()) {
        return static_cast< std::int32_t >(DltProtocolEncoder::EncodeError::kEncodeAlreadyEncoded);
    }
    this->SetEncoded(true);

    LOGVERBOSE("enter: ") << __func__ << "args numb" << this->GetArgsNum();
    DltStandardHeader*& standardheader = this->StandardHeaderField();
    std::uint32_t& headersize          = this->HeaderSizeField();
    std::size_t& payloadSize           = this->PayloadSize();
    DltStandardHeaderExtra* headerExtra{nullptr};

    if (!BuildStandardHeaderCommon(false, standardheader, headerExtra, headersize)) {
        return static_cast< std::int32_t >(DltProtocolEncoder::EncodeError::kEncodeMessageTooLarge);
    }

    if (!FinalizeMessageLength(headersize, payloadSize)) {
        LOGERROR(__func__) << " no space ";
        return static_cast< std::int32_t >(DltProtocolEncoder::EncodeError::kEncodeMessageTooLarge);
    }
    LOGVERBOSE("exit: ") << __func__ << "len" << (headersize + payloadSize);

    return static_cast< std::int32_t >(DltProtocolEncoder::EncodeError::kEncodeOk);
}

}  // namespace internal
}  // namespace log
}  // namespace ara
