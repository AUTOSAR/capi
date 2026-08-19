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
/// @file       ids_proto_encode.cpp
/// @brief      IDS protocol serialization
/// @details
/// @date       2023-01-13
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/IDS protocol
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0011
/// @unit_name=IdsmSerialize
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "ara/idsm/ids_proto_encode.h"

#include <arpa/inet.h>

#include "ids_log.h"
#include "ids_proto_common.h"
#include "ids_sign_encode.h"
namespace ara {
namespace idsm {
/// @brief
/// @param version
/// @param header
/// @param id
/// @param rvd
/// @param slot
/// @param alg
/// @throws Stack overflow exception
IdsmSerialize::IdsmSerialize(uint8_t const version,
                             uint8_t const header,
                             uint16_t const id,
                             ara::core::String const& slot,
                             ara::core::String const& alg)
    : protoVersion_{version}, protoHeader_{header}, idsmId_{id}, idsmSignEncoder_{nullptr}
{
    idsmSignEncoder_ = std::make_shared< IdsmSignEncode >(slot, alg);
}

/// @brief
/// @param event
/// @return
/// @throw
std::shared_ptr< BytesVec > IdsmSerialize::Encode(EventPtr const& event)
{
    IDS_LOG_VERBOSE << "ids protocol: serialize start. event id: " << event->GetEventId();
    this->protoHeader_ = 0U;
    ContextDataType ctxData;
    event->GetContextData(ctxData);
    if (!ctxData.empty()) {
        uint32_t const contextDataSizeMax{0x80000000U};
        if (ctxData.size() >= contextDataSizeMax) {
            IDS_LOG_ERROR << "idsm protocol: context data length is exceed max 0x80000000. protocol fail";
            return std::shared_ptr< BytesVec >(nullptr);
        }

        uint8_t const ctxFlag{static_cast< uint8_t >(ProtoFlag::kContextData)};
        this->protoHeader_ |= ctxFlag;
    }
    if (event->GetTimeStamp() != 0U) {
        uint8_t const timesFlag{static_cast< uint8_t >(ProtoFlag::kTimestamp)};
        this->protoHeader_ |= timesFlag;
    }
#ifdef ARA_WITH_CRYPTO
    if (idsmSignEncoder_->HasSign()) {
        uint8_t const signFlag{static_cast< uint8_t >(ProtoFlag::kSignature)};
        this->protoHeader_ |= signFlag;
    }
#endif
    std::shared_ptr< BytesVec > message{std::make_shared< BytesVec >()};

    /// @brief calculate event serialize message length, pre-allocate storage avoid multi-times allocate
    /// @details 8 bytes ids message separation, 8 bytes ids event frame
    /// @details optional timestamp: 8 bytes
    /// @details optional context data: 1 or 4 bytes length, context data
    /// @details optional signature: 2 bytes length, singature data
    uint16_t constexpr kFrameSize{16U};
    /// @details The length of the serialized data is initialized to the frame length
    size_t encodeSize{kFrameSize};

    /// @details Add the length of the timestamp
    uint8_t const timesFlag{static_cast< uint8_t >(ProtoFlag::kTimestamp)};
    if ((protoHeader_ & timesFlag) == timesFlag) {
        encodeSize += sizeof(event->GetTimeStamp());
    }

    /// @details Add the length of the context data
    if (!ctxData.empty()) {
        uint16_t constexpr kCtxDataSizeLen{4U};
        encodeSize = encodeSize + kCtxDataSizeLen + ctxData.size();
    }

#ifdef ARA_WITH_CRYPTO
    uint16_t constexpr kSignSizeLen{2U};
    uint16_t constexpr kSignSize{8192U};
    encodeSize = encodeSize + +kSignSizeLen + kSignSize;
#endif

    message->reserve(static_cast< size_t >(encodeSize));
    size_t const headerSize{8U};
    message->resize(headerSize, 0U);

    _encodeEventFrame(event, message);
    _encodeTimeStamp(event, message);
    _encodeContextData(ctxData, message);
#ifdef ARA_WITH_CRYPTO
    _encodeSignature(message);
#endif

    uint32_t const netSize{htonl(static_cast< uint32_t >(message->size()))};
    uint32_t const idsProtocolSizeMax{2147549207U};
    uint32_t const idsProtocolSizeMin{16U};
    std::ignore = std::memcpy(message->data() + sizeof(uint32_t),
                              static_cast< uint8_t const* >(static_cast< void const* >(&netSize)), sizeof(netSize));
    size_t const msgSize{message->size()};
    if ((message->size() < idsProtocolSizeMin) || (msgSize > idsProtocolSizeMax)) {
        IDS_LOG_ERROR << "ids protocol: event encode byte length is invalid. size: " << message->size();
        message->clear();
    }
    IDS_LOG_VERBOSE << "ids protocol: serialize end. event id: " << event->GetEventId();
    return message;
}

/// @brief
/// @param event
/// @param message
/// @throw
void IdsmSerialize::_encodeEventFrame(EventPtr const& event, std::shared_ptr< BytesVec >& message) const
{
    size_t const bufSize{128U};
    std::unique_ptr< uint8_t[] > const buf{std::make_unique< uint8_t[] >(bufSize)};
    uint8_t* const bufPtr{static_cast< uint8_t* >(buf.get())};
    uint16_t pos{0U};

    std::ignore = std::memset(bufPtr, 0, sizeof(buf));

    uint8_t byte0{static_cast< uint8_t >(static_cast< uint32_t >(this->protoVersion_) << 4U)};
    byte0 |= this->protoHeader_;
    std::ignore = std::memcpy(bufPtr + pos, &byte0, sizeof(byte0));
    pos += sizeof(byte0);

    uint32_t constexpr kBits{6U};
    uint16_t hostByte1{static_cast< uint16_t >(static_cast< uint32_t >(this->idsmId_) << kBits)};
    hostByte1 |= static_cast< uint16_t >(event->GetSensorId());
    uint16_t const byte1{htons(hostByte1)};
    std::ignore = std::memcpy(bufPtr + pos, &byte1, sizeof(byte1));
    pos += sizeof(byte1);

    uint16_t const byte3{htons(event->GetEventId())};
    std::ignore = std::memcpy(bufPtr + pos, &byte3, sizeof(byte3));
    pos += sizeof(byte3);

    uint16_t const byte5{htons(event->GetCounter())};
    std::ignore = std::memcpy(bufPtr + pos, &byte5, sizeof(byte5));
    pos += sizeof(byte5);

    uint8_t const byte7{0U};
    std::ignore = std::memcpy(bufPtr + pos, &byte7, sizeof(byte7));
    pos += sizeof(byte7);
    std::ignore = message->insert(message->cend(), bufPtr, bufPtr + pos);
}

/// @brief
/// @param event
/// @param message
/// @throw
void IdsmSerialize::_encodeTimeStamp(EventPtr const& event, std::shared_ptr< BytesVec >& message) const
{
    uint8_t const timesFlag{static_cast< uint8_t >(ProtoFlag::kTimestamp)};
    if ((protoHeader_ & timesFlag) == 0U) {
        return;
    }
    size_t const bufSize{128U};
    std::unique_ptr< uint8_t[] > const buf{std::make_unique< uint8_t[] >(bufSize)};
    uint8_t* const bufPtr{static_cast< uint8_t* >(buf.get())};
    uint16_t pos{0U};
    std::ignore = std::memset(bufPtr, 0, bufSize);

    if (event->GetTimeSource() == TimestampSource::kAraTsync) {
        uint64_t const temp{1000U * 1000U * 1000U};
        uint32_t const nanSecond{static_cast< uint32_t >(event->GetTimeStamp() % temp) & 0x7fffffffU};
        uint32_t const byte8{htonl(nanSecond)};
        std::ignore = std::memcpy(bufPtr + pos, &byte8, sizeof(byte8));
        pos += sizeof(byte8);

        uint32_t const second{static_cast< uint32_t >(event->GetTimeStamp() / temp)};
        uint32_t const byte12{htonl(second)};
        std::ignore = std::memcpy(bufPtr + pos, &byte12, sizeof(byte12));
        pos += sizeof(byte12);
    } else {
        uint64_t const temp{0x8000000000000000U};
        uint64_t timestamp{event->GetTimeStamp()};
        timestamp = timestamp | temp;
        uint64_t const byte8{Htonll(timestamp)};
        std::ignore = std::memcpy(bufPtr + pos, &byte8, sizeof(byte8));
        pos += sizeof(byte8);
    }
    std::ignore = message->insert(message->cend(), bufPtr, bufPtr + pos);
}

/// @brief
/// @param ctxData
/// @param message
/// @throw
void IdsmSerialize::_encodeContextData(ContextDataType const& ctxData, std::shared_ptr< BytesVec >& message) const
{
    uint8_t const ctxFlag{static_cast< uint8_t >(ProtoFlag::kContextData)};
    if ((protoHeader_ & ctxFlag) == 0U) {
        return;
    }
    size_t const bufSize{128U};
    std::unique_ptr< uint8_t[] > const buf{std::make_unique< uint8_t[] >(bufSize)};
    uint8_t* const bufPtr{static_cast< uint8_t* >(buf.get())};
    std::ignore = std::memset(bufPtr, 0, bufSize);
    uint16_t pos{0U};

    uint32_t const byteCapacity{128U};
    if (ctxData.size() < byteCapacity) {
        uint8_t const dataSize{static_cast< uint8_t >(ctxData.size())};
        std::ignore = std::memcpy(bufPtr, &dataSize, sizeof(dataSize));
        pos += sizeof(dataSize);
    } else {
        uint32_t const dataSize{static_cast< uint32_t >(ctxData.size())};
        uint32_t const netSize{htonl(dataSize | 0x80000000U)};
        std::ignore
            = std::memcpy(bufPtr, static_cast< uint8_t const* >(static_cast< void const* >(&netSize)), sizeof(netSize));
        pos += sizeof(netSize);
    }
    std::ignore = message->insert(message->cend(), bufPtr, bufPtr + pos);
    std::ignore = message->insert(message->cend(), ctxData.cbegin(), ctxData.cend());
}

/// @brief
/// @param event
/// @param message
/// @throw

void IdsmSerialize::_encodeSignature(std::shared_ptr< BytesVec >& message)
{
#ifdef ARA_WITH_CRYPTO
    uint8_t const signFlag{static_cast< uint8_t >(ProtoFlag::kSignature)};
    if ((protoHeader_ & signFlag) == 0U) {
        return;
    }

    uint8_t const signDataPos{8U};
    BytesVec signData;
    // signData.clear();
    // event->GetSignature(signData);
    ara::idsm::BytesVecWithAlloc& nSignature{signData};
    idsmSignEncoder_->GetSignature(message->data() + signDataPos, static_cast< size_t >(message->size() - signDataPos),
                                   nSignature);
    if (nSignature.empty()) {
        return;
    }
    /// @brief Add signature to IDS protocol
    size_t const bufSize{128U};
    std::unique_ptr< uint8_t[] > const buf{std::make_unique< uint8_t[] >(bufSize)};
    uint8_t* const bufPtr{static_cast< uint8_t* >(buf.get())};
    std::ignore = std::memset(bufPtr, 0, bufSize);

    uint16_t pos{0U};
    uint16_t const hostSize{static_cast< uint16_t >(nSignature.size())};
    uint16_t const signatureSize{htons(hostSize)};
    std::ignore = std::memcpy(bufPtr, &signatureSize, sizeof(signatureSize));
    pos += sizeof(signatureSize);
    std::ignore = message->insert(message->cend(), bufPtr, bufPtr + pos);
    std::ignore = message->insert(message->cend(), nSignature.begin(), nSignature.end());
#endif
    return;
}

}  // namespace idsm
}  // namespace ara