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
/// @file       dlt_protocol_nomodel_encoder.cpp
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
/// @unit_name = dlt_protocol_nomodel_encoder
/// @unit_description=Encoding function implemented by the Dlt lib, used for encoding logs into DLT format
/// @endcode
///
/// ================================================================

#include "dlt_protocol_nomodel_encoder.h"

#include <memory.h>

#include <cstdint>
#include <cstring>
#include <string>

#include "Utils/src/private_log.h"

namespace ara {
namespace log {
namespace internal {

/// @brief
/// @param[in]  lineLevel
/// @param[in]  loggerData
DltProtocolNonModelEncoder::DltProtocolNonModelEncoder() noexcept { ResetEncodingState(); }
/// @brief
DltProtocolNonModelEncoder::~DltProtocolNonModelEncoder() noexcept = default;

/// @brief
/// @param[in]  str
/// @return
void DltProtocolNonModelEncoder::AppendString(std::string const& str) noexcept
{
    LOGVERBOSE("enter:DltProtocolNonModelEncoder  ") << __func__;
    std::uint8_t* payloadData = this->PayloadData();
    if (payloadData == nullptr) {
        return;
    }

    std::size_t& payloadSize = this->PayloadSize();

    std::uint16_t const length{static_cast< std::uint16_t >(str.length())};
    std::uint16_t const lenOffset{1U};
    std::size_t const payloadHeaderSize{sizeof(std::uint32_t) + sizeof(std::uint16_t)};
    if ((payloadSize + payloadHeaderSize + lenOffset) > BufferCapacity()) {
        return;
    }

    std::uint16_t argSize{static_cast< std::uint16_t >(length + lenOffset)};
    std::uint32_t typeInfo{0U};
    std::string const kStrTruncatedMessage{"..."};
    std::size_t const strTruncateMessageLength{kStrTruncatedMessage.length() + 1U};
    std::size_t availablePayloadStrMsg{BufferCapacity() - payloadSize - payloadHeaderSize};
    std::size_t copyLength{length};
    bool appendTruncatedSuffix{false};

    if (argSize > availablePayloadStrMsg) {
        if (availablePayloadStrMsg >= strTruncateMessageLength) {
            copyLength            = availablePayloadStrMsg - strTruncateMessageLength;
            appendTruncatedSuffix = true;
        } else {
            copyLength = availablePayloadStrMsg - lenOffset;
        }

        while ((copyLength > 0U) && (copyLength < str.length())
               && ((static_cast< std::uint8_t >(str[copyLength]) & 0xC0U) == 0x80U)) {
            copyLength--;
        }

        std::size_t const suffixLength{appendTruncatedSuffix ? strTruncateMessageLength : lenOffset};
        argSize = static_cast< std::uint16_t >(copyLength + suffixLength);
    }
    std::uint32_t const dltTypeInfoStrg{0x00000200U};
    /// @brief
    std::uint32_t const dltScodUtf8{0x00008000U};
    typeInfo |= dltTypeInfoStrg | dltScodUtf8;
    if (!AppendToPayload(&typeInfo, sizeof(uint32_t))) {
        return;
    }

    if (!AppendToPayload(&argSize, sizeof(uint16_t))) {
        return;
    }

    if (copyLength > 0U) {
        std::ignore = memcpy(payloadData + payloadSize, str.data(), copyLength);
        payloadSize += copyLength;
    }

    if (appendTruncatedSuffix) {
        std::ignore = memcpy(payloadData + payloadSize, kStrTruncatedMessage.c_str(), strTruncateMessageLength);
        payloadSize += strTruncateMessageLength;
    } else {
        payloadData[payloadSize] = 0U;
        payloadSize += lenOffset;
    }
    this->IncreaseArgsNum();
}
/// @brief
/// @param[in]  data
/// @param[in]  length
/// @param[in]  type
/// @param[in]  name
/// @param[in]  withVarInfo
void DltProtocolNonModelEncoder::AppendRawData(void const* data,
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

    /// FIXME This judgment may be redundant, as it has already been encapsulated in the log_stream function.
    if ((type < DltFormatType::kDlt_Format_Default) || (type >= DltFormatType::kDlt_Format_Max)) {
        return;
    }
    if (data == nullptr || length == 0U) {
        return;
    }

    std::uint16_t nameSize{0U};
    if (name.empty() == false) {
        nameSize = name.size() + 1U;
    }

    std::size_t neededSize{length + sizeof(uint16_t)};
    if ((payloadSize + neededSize) > BufferCapacity()) {
        return;
    }

    std::uint32_t const dltTypeInfoRawd{0x00000400U};
    std::uint32_t typeInfo{dltTypeInfoRawd};

    neededSize += sizeof(uint32_t);  // Type Info field
    if (withVarInfo) {
        neededSize += sizeof(uint16_t);  // length of name
        neededSize += nameSize;          // the name itself
        std::uint32_t const dltTypeInfoVari{0x00000800U};
        typeInfo |= dltTypeInfoVari;
    }
    if ((payloadSize + neededSize) > BufferCapacity()) {
        std::cout << __func__ << "big error -  no buffer for log Size :  " << neededSize << std::endl;
        return;
    }

    // Genivi extension: put formatting hints into the unused (for RAWD) TYLE + SCOD fields.
    // The SCOD field holds the base (hex or bin); the TYLE field holds the column width (8bit..64bit).
    std::uint32_t const typev{static_cast< std::uint32_t >(type)};
    if ((type >= DltFormatType::kDlt_Format_Hex8) && (type <= DltFormatType::kDlt_Format_Hex64)) {
        typeInfo |= kDlt_Scod_Hex;
        typeInfo = typeInfo + typev;
    } else if ((type >= DltFormatType::kDlt_Format_Bin8) && (type <= DltFormatType::kDlt_Format_Bin16)) {
        std::uint32_t const bin8value{static_cast< std::uint32_t >(DltFormatType::kDlt_Format_Bin8)};
        typeInfo |= kDlt_Scod_Bin;
        typeInfo += typev - bin8value + 1U;
    } else {
        LOGERROR(__func__) << " type error on raw ";
    }

    if (!AppendToPayload(&typeInfo, sizeof(uint32_t))) {
        return;
    }

    if (!AppendToPayload(&length, sizeof(uint16_t))) {
        return;
    }

    if (withVarInfo) {
        // Write length of "name" attribute.
        // We assume that the protocol allows zero-sized strings here (which this code will create
        // when the input pointer is nullptr).
        if (!AppendToPayload(&nameSize, sizeof(uint16_t))) {
            return;
        }

        // Write name string itself.
        // Must not use nullptr as source pointer for memcpy. This check assures that.
        if (nameSize != 0U) {
            std::ignore = memcpy(payloadData + payloadSize, name.c_str(), static_cast< std::size_t >(nameSize));
            payloadSize += nameSize;
        }
    }

    std::ignore = memcpy(payloadData + payloadSize, data, static_cast< std::size_t >(length));
    payloadSize += length;

    this->IncreaseArgsNum();
}
/// @brief
/// @param[in]  datap
/// @param[in]  datalen
/// @param[in]  typeInfo
void DltProtocolNonModelEncoder::AppendGeneric(const void* datap,
                                               std::size_t const& datalen,
                                               std::uint32_t const& typeInfo) noexcept
{
    std::uint8_t* payloadData{nullptr};
    std::size_t* payloadSize{nullptr};
    if (!PreparePayloadWrite(datalen + sizeof(std::uint32_t), payloadData, payloadSize)) {
        return;
    }
    if (datap == nullptr) {
        LOGERROR(__func__) << " datap is null ";
        return;
    }
    if (!AppendToPayload(&typeInfo, sizeof(std::uint32_t))) {
        return;
    }
    std::ignore = memcpy(payloadData + (*payloadSize), datap, datalen);
    (*payloadSize) += datalen;
    this->IncreaseArgsNum();
}

/// @brief
std::int32_t DltProtocolNonModelEncoder::Encode() noexcept
{
    if (this->GetArgsNum() == 0U) {
        return static_cast< std::int32_t >(DltProtocolEncoder::EncodeError::kEncodeNoArgs);
    }
    if (this->IsEncoded()) {
        return static_cast< std::int32_t >(DltProtocolEncoder::EncodeError::kEncodeAlreadyEncoded);
    }
    this->SetEncoded(true);

    DltStandardHeader*& standardheader = this->StandardHeaderField();
    std::uint32_t& headersize          = this->HeaderSizeField();
    std::size_t& payloadSize           = this->PayloadSize();
    DltStandardHeaderExtra* headerExtra{nullptr};

    if (!BuildStandardHeaderCommon(true, standardheader, headerExtra, headersize)) {
        return static_cast< std::int32_t >(DltProtocolEncoder::EncodeError::kEncodeMessageTooLarge);
    }

    std::uint8_t* const headerbuffer = this->HeaderData();
    auto const& ctx                  = GetEncodeContext();

    std::uint8_t const localLogLineLevel{static_cast< std::uint8_t >(GetLineLogLevel())};

    DltExtendedHeader* const extendedheader = reinterpret_cast< internal::DltExtendedHeader* >(
        headerbuffer + sizeof(internal::DltStandardHeader) + DLT_STANDARD_HEADER_EXTRA_SIZE(standardheader->htyp));

    extendedheader->msin = static_cast< std::uint8_t >(
        (kDlt_Type_Log << kDlt_Msin_Mstp_Shift) | ((localLogLineLevel << kDlt_Msin_Mtin_Shift) & kDlt_Msin_Mtin));
    /// @brief verbose
    std::uint8_t const dltMsinVerb{0x01U};
    extendedheader->msin |= dltMsinVerb;
    extendedheader->noar = this->GetArgsNum();
    std::size_t const appidMax{4U};
    if (ctx.appId.length() > appidMax) {
        std::ignore = memcpy(static_cast< void* >(extendedheader->apid), ctx.appId.c_str(), appidMax);

    } else {
        std::ignore = memcpy(static_cast< void* >(extendedheader->apid), ctx.appId.c_str(), ctx.appId.length());
    }

    if (ctx.contextId.length() > appidMax) {
        std::ignore = memcpy(static_cast< void* >(extendedheader->ctid), ctx.contextId.c_str(), appidMax);
    } else {
        std::ignore = memcpy(static_cast< void* >(extendedheader->ctid), ctx.contextId.c_str(), ctx.contextId.length());
    }

    headersize += sizeof(internal::DltExtendedHeader);

    if (!FinalizeMessageLength(headersize, payloadSize)) {
        std::cout << __func__ << "errror: log message size is too big" << (headersize + payloadSize) << std::endl;
        return static_cast< std::int32_t >(DltProtocolEncoder::EncodeError::kEncodeMessageTooLarge);
    }

    return static_cast< std::int32_t >(DltProtocolEncoder::EncodeError::kEncodeOk);
}

}  // namespace internal
}  // namespace log
}  // namespace ara
