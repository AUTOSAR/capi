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
/// @file       ids_proto_decode.cpp
/// @brief      IDS protocol deserialization implementation
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
/// @unit_name=IdsmDeserialize
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "ara/idsm/ids_proto_decode.h"

#include <arpa/inet.h>

#include "ids_log.h"
#include "ids_proto_common.h"
#include "ids_sign_verify.h"

namespace ara {
namespace idsm {
/// @brief Parameterized constructor
/// @param version
/// @param header
/// @param id
/// @param rvd
/// @param size
/// @param len
/// @param v
/// @param slot
/// @param alg
/// @param digest
/// @param decrypt
/// @throws Stack overflow exception
IdsmDeserialize::IdsmDeserialize(uint8_t const version,
                                 uint8_t const header,
                                 uint16_t const id,
                                 uint8_t const rvd,
                                 uint16_t const size,
                                 uint32_t const len,
                                 std::vector< uint8_t > v,
                                 ara::core::String const &slot,
                                 ara::core::String const &alg,
                                 DigestFunc digest,
                                 DecryptFunc decrypt)
    : protoVersion_{version}
    , protoHeader_{header}
    , idsmId_{id}
    , reserved_{rvd}
    , bufSize_{size}
    , headBuf_{}
    , msgSize_{len}
    , message_{std::move(v)}
    , signVerifier_{nullptr}
    , digest_{std::move(digest)}
    , decrypt_{std::move(decrypt)}
{
    signVerifier_ = std::make_shared< IdsmSignVerify >(slot, alg);
}
/// @brief Protocol header processing
/// @param data
/// @param size
/// @return
/// @throws Stack overflow exception
uint32_t IdsmDeserialize::_processProtoHead(uint8_t const *const data, uint32_t const size)
{
    // Under normal circumstances, this possibility does not occur
    if (bufSize_ >= kIdsProtoHeadSize) {
        return 0U;
    }
    // Determine whether a complete protocol header can be formed
    uint32_t const copySize{(size < (kIdsProtoHeadSize - bufSize_)) ? size : (kIdsProtoHeadSize - bufSize_)};
    std::ignore = std::memcpy(headBuf_ + bufSize_, data, static_cast< size_t >(copySize));
    bufSize_ += copySize;

    // A complete protocol header appears, read the size of the protocol data
    if (bufSize_ >= kIdsProtoHeadSize) {
        uint32_t netMsgSize{0U};
        uint32_t const msgSizePos{4U};
        std::ignore = std::memcpy(static_cast< uint8_t * >(static_cast< void * >(&netMsgSize)), headBuf_ + msgSizePos,
                                  sizeof(netMsgSize));
        msgSize_    = ntohl(netMsgSize);
        msgSize_ -= kIdsProtoHeadSize;
        // Request storage space for the protocol in advance
        message_.reserve(static_cast< size_t >(msgSize_));
    }
    return copySize;
}
/// @brief Deserialization
/// @param data
/// @param dataSize
/// @return
/// @throw
std::vector< EventPtr > IdsmDeserialize::Decode(uint8_t const *data, uint32_t dataSize)
{
    std::vector< EventPtr > res;
    // Loop processing protocol data stream to handle packet sticking issues
    while (true) {
        if (dataSize == 0U) {
            break;
        }
        uint32_t const usedSize{_processProtoHead(data, dataSize)};
        data += usedSize;
        dataSize -= usedSize;
        // Protocol header incomplete, exit directly
        if (bufSize_ < kIdsProtoHeadSize) {
            break;
        }

        // Determine whether a complete protocol data can be formed
        uint64_t copySize{0U};
        if (msgSize_ - message_.size() < dataSize) {
            copySize = msgSize_ - message_.size();
        } else {
            copySize = dataSize;
        }
        std::ignore = message_.insert(message_.cend(), data, data + copySize);
        data += copySize;
        dataSize -= static_cast< uint32_t >(copySize);
        // Protocol data incomplete, exit directly
        if (message_.size() < msgSize_) {
            break;
        }

        // Process complete protocol data
        size_t pos{0U};
        EventPtr event{std::make_shared< Event >()};
        _decodeEventFrame(pos, event);
        _decodeTimestamp(pos, event);
        _decodeContextData(pos, event);
#ifdef ARA_WITH_CRYPTO
        _decodeSignature(pos, event);
#endif
        if (pos == 0U) {
        }
        if (event.get() != nullptr) {
            res.push_back(event);
        }

        // Reset protocol header related data items
        message_.clear();
        bufSize_ = 0U;
        msgSize_ = 0U;
    }
    return res;
}
/// @brief Deserialize security event stack frame
/// @param pos
/// @param event
void IdsmDeserialize::_decodeEventFrame(size_t &pos, EventPtr &event) noexcept
{
    /// @brief Protocol version number
    uint32_t const versionBits{4U};
    uint8_t const versionExtract{0xF0U};
    this->protoVersion_
        = static_cast< uint8_t >(static_cast< uint8_t >((message_.at(pos) & versionExtract)) >> versionBits);
    /// @brief Protocol header
    uint8_t const headerExtract{0x0FU};
    this->protoHeader_ = message_.at(pos) & headerExtract;
    /// @brief IDSM instance ID
    // Calculate the high 8 bits of the IDSM instance ID
    size_t const instIndexH{1U};
    uint32_t const instBitsH{2U};
    this->idsmId_ = static_cast< uint16_t >(message_.at(pos + instIndexH));
    this->idsmId_ = static_cast< uint16_t >(this->idsmId_ << instBitsH);
    // Calculate the byte where the lower 2 bits of the IDSM instance ID are located
    size_t const instIndexL{2U};
    uint16_t const instIdL{static_cast< uint16_t >(message_.at(pos + instIndexL))};
    // Combine the IDSM instance ID
    uint8_t const instExtract{0xC0U};
    uint32_t const instBitsL{6U};
    this->idsmId_ = static_cast< uint16_t >(
        this->idsmId_ | (static_cast< uint32_t >(instIdL & instExtract) >> static_cast< uint32_t >(instBitsL)));
    ///   @brief Sensor ID
    uint8_t const sensorExtract{0x3FU};
    size_t const sensorIndex{2U};
    uint8_t const sensorId{static_cast< uint8_t >(message_.at(pos + sensorIndex) & sensorExtract)};
    event->SetSensorId(sensorId);
    /// @brief Event type ID
    uint16_t netEventId{0U};
    size_t const eventIdIndex{3U};
    std::ignore = std::memcpy(static_cast< uint8_t * >(static_cast< void * >(&netEventId)),
                              message_.data() + pos + eventIdIndex, sizeof(netEventId));
    event->SetEventId(ntohs(netEventId));
    /// @brief Event counter
    uint16_t netCounter{0U};
    size_t const counterIndex{5U};
    std::ignore = std::memcpy(static_cast< uint8_t * >(static_cast< void * >(&netCounter)),
                              message_.data() + pos + counterIndex, sizeof(netCounter));
    event->SetCounter(ntohs(netCounter));
    /// @brief Reserved field
    size_t const reservedIndex{7U};
    this->reserved_ = message_[pos + reservedIndex];
    size_t const eventFrameSize{8U};
    pos += eventFrameSize;
}
/// @brief Deserialize timestamp
/// @param pos
/// @param event
/// @throw
void IdsmDeserialize::_decodeTimestamp(size_t &pos, EventPtr &event)
{
    uint8_t const timeStampFlag{static_cast< uint8_t >(ProtoFlag::kTimestamp)};
    if ((protoHeader_ & timeStampFlag) == 0U) {
        return;
    }
    uint32_t const timeFormat{0x80U};
    if ((message_.at(pos) & timeFormat) != 0U) {
        event->SetTimeSource(TimestampSource::kAppOem);
        uint64_t netTimestamp{0U};
        std::ignore = std::memcpy(static_cast< uint8_t * >(static_cast< void * >(&netTimestamp)), message_.data() + pos,
                                  sizeof(netTimestamp));
        uint64_t const timestamp{Ntohll(netTimestamp)};
        uint64_t const temp{0x7FFFFFFFFFFFFFFFU};
        event->SetTimeStamp(timestamp & temp);
        pos += sizeof(timestamp);
    } else {
        event->SetTimeSource(TimestampSource::kAraTsync);

        uint32_t netNanoSec{0U};
        std::ignore = std::memcpy(static_cast< uint8_t * >(static_cast< void * >(&netNanoSec)), message_.data() + pos,
                                  sizeof(netNanoSec));
        pos += sizeof(netNanoSec);
        uint32_t const nanoSec{ntohl(netNanoSec)};

        uint32_t netSec{0U};
        std::ignore = std::memcpy(static_cast< uint8_t * >(static_cast< void * >(&netSec)), message_.data() + pos,
                                  sizeof(netSec));
        pos += sizeof(netSec);
        uint64_t const sec{ntohl(netSec)};
        uint64_t const temp{1000U * 1000U * 1000U};
        event->SetTimeStamp((sec * temp) + nanoSec);
    }
}
/// @brief Deserialize context data
/// @param pos
/// @param event
/// @throw
void IdsmDeserialize::_decodeContextData(size_t &pos, EventPtr &event)
{
    uint8_t const ctxFlag{static_cast< uint8_t >(ProtoFlag::kContextData)};
    if ((protoHeader_ & ctxFlag) == 0U) {
        return;
    }
    uint32_t netSize{message_[pos]};
    uint32_t const singleByteCap{128U};
    ContextDataType ctxData;
    if (netSize < singleByteCap) {
        /// @brief context data start position
        int64_t const contextDataStart{static_cast< int64_t >(pos) + 1};
        /// @brief context data end position: next position after the last byte of context data
        int64_t const contextDataEnd{contextDataStart + static_cast< int64_t >(netSize)};
        ctxData.assign(message_.begin() + contextDataStart, message_.begin() + contextDataEnd);
        pos = static_cast< size_t >(contextDataEnd);
    } else {
        std::ignore = std::memcpy(static_cast< uint8_t * >(static_cast< void * >(&netSize)), message_.data() + pos,
                                  sizeof(netSize));
        uint32_t const dataSize{ntohl(netSize) & 0x7fffffffU};
        /// @brief context data start position
        int64_t const contextDataStart{static_cast< int64_t >(pos) + static_cast< int64_t >(sizeof(dataSize))};
        /// @brief context data end position: next position after the last byte of context data
        int64_t const contextDataEnd{contextDataStart + static_cast< int64_t >(dataSize)};
        ctxData.assign(message_.begin() + contextDataStart, message_.begin() + contextDataEnd);
        pos = static_cast< size_t >(contextDataEnd);
    }
    event->SetContextData(ctxData);
}
/// @brief Register custom encryption related interfaces
/// @param digest
/// @param decrypt
/// @throw
void IdsmDeserialize::Register(DigestFunc const &digest, DecryptFunc const &decrypt)
{
    if (digest == nullptr) {
        IDS_LOG_WARN
            << "idsr should have digest and decrypt user-defined function, but has only one user-defined function.";
        return;
    }
    if (decrypt == nullptr) {
        IDS_LOG_WARN
            << "idsr should have digest and decrypt user-defined function, but has only one user-defined function.";
        return;
    }
    digest_  = digest;
    decrypt_ = decrypt;
}
/// @brief Signature verification
/// @param pos Data length
/// @param event The signature ciphertext to be verified is stored in the event structure
/// @return
/// @exception Stack overflow exception
bool IdsmDeserialize::_signVerify(size_t const &pos, EventPtr const &event)
{
#ifdef ARA_WITH_CRYPTO
    BytesVec cipherText;
    event->GetSignature(cipherText);
    /// @details Use the AP platform's crypto module for signature verification
    if (digest_ == nullptr) {
        return signVerifier_->VerifySignature(message_.data(), pos, cipherText);
    }
    if (decrypt_ == nullptr) {
        return signVerifier_->VerifySignature(message_.data(), pos, cipherText);
    }
    /// @details Use user-defined functions for signature verification
    BytesVec const digestData{digest_(message_.data(), pos)};
    std::size_t const cipherTextSize{cipherText.size()};
    BytesVec const plainText{decrypt_(cipherText.data(), cipherTextSize)};
    if (digestData == plainText) {
        return true;
    }
    return false;
#else
    return false;
#endif
}

/// @brief Deserialize signature from byte stream
/// @param pos Signature serialization data starts from pos. When the function ends, pos moves to the next byte after the end of the signature serialization data
/// @param event Structured event
/// @exception Stack overflow exception
void IdsmDeserialize::_decodeSignature(size_t &pos, EventPtr &event)
{
#ifdef ARA_WITH_CRYPTO
    uint8_t const signFlag{static_cast< uint8_t >(ProtoFlag::kSignature)};
    if ((protoHeader_ & signFlag) == 0U) {
        return;
    }

    uint16_t signatureSize{0U};
    std::ignore = std::memcpy(static_cast< uint8_t * >(static_cast< void * >(&signatureSize)), message_.data() + pos,
                              sizeof(signatureSize));
    uint16_t const hostSize{ntohs(signatureSize)};
    /// start position of signature
    int64_t const signatureStart{static_cast< int64_t >(pos) + static_cast< int64_t >(sizeof(signatureSize))};
    /// end position of signature: next position after the last byte of signature
    int64_t const signatureEnd{signatureStart + static_cast< int64_t >(hostSize)};
    BytesVec signData;
    signData.assign(message_.begin() + signatureStart, message_.begin() + signatureEnd);
    event->SetSignature(signData);
    bool const verifyRes{_signVerify(pos, event)};
    if (verifyRes == false) {
        IDS_LOG_ERROR << "ids protocol: verify signature fail.";
        event.reset();
    }
    pos = static_cast< size_t >(signatureEnd);
#endif
    return;
}

}  // namespace idsm
}  // namespace ara