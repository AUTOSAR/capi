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
/// @file       secoc_helper.h
/// @brief      secoc helper header file
/// @details
/// @date       2023-09-04
/// @author     shigang.zan
/// @version    1.2.0
///
/// ================================================================

#ifndef __SECOC_HELPER_H__
#define __SECOC_HELPER_H__

#include "ara/com/internal/log/log.h"
#include "ara/com/internal/secoc/secoc_type.h"
#include "ara/core/steady_clock.h"
#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/entry_point.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/cryp/crypto_provider.h"
#include "ara/crypto/cryp/hash_function_ctx.h"
#include "nsomeip/net/nsi_message.h"

namespace ara {
namespace com {
namespace internal {
namespace secoc {

uint16_t const kBitNuMinByte{8};

/// @brief SecOC helper class
class SecOCHelper
{
public:
    /// @brief get second time value since 1970.1.1
    /// @return second value
    static uint64_t GetSecondValueFrom1970() noexcept
    {
        auto now{std::chrono::system_clock::now()};
        std::chrono::time_point< std::chrono::system_clock, std::chrono::seconds > const nowS{
            std::chrono::time_point_cast< std::chrono::seconds >(now)};
        return static_cast< uint64_t >(nowS.time_since_epoch().count());
    }

    /// @brief just for testing
    /// @param[in] message
    static void DumpSomeIpMsg(nsi_message_t* const message) noexcept
    {
        ComLogInfo("Dump SomeIp Msg");
        ComLogInfo("header len is ", message->hdr.len);
        ComLogInfo("client is ", message->hdr.client);
        ComLogInfo("session is ", message->hdr.session);
        ComLogInfo("protocol is ", message->hdr.protocol);
        ComLogInfo("interface is ", message->hdr.interface);
        ComLogInfo("type is ", message->hdr.type);
        ComLogInfo("code is ", message->hdr.code);
        ara::core::Vector< uint8_t > buffer;
        nsi_message_io_t io;
        std::ignore = nsi_message_read_start(&io, message, 0);
        // header length includes payload length and header part II length(8Bytes).
        buffer.resize(message->hdr.len - 8);  // NOLINT -- TODO[magic-numbers]
        std::ignore = nsi_message_read(&io, buffer.data(), buffer.size());
        std::ignore = nsi_message_read_end(&io);
        std::string macOut;
        SecOCHelper::PrintBit4Vector(buffer, macOut);
        ComLogInfo(macOut.c_str());
    }
    /// @brief dump bit to string
    /// @param[in] b
    /// @param[in] out
    /// @return
    static void PrintBit(uint8_t const b, std::string& out) noexcept
    {
        for (int8_t i{7}; i >= 0; --i) {  // NOLINT -- TODO[magic-numbers]
            uint8_t const v{static_cast< uint8_t >(b & static_cast< uint8_t >(1 << i))};
            if (v != 0) {
                std::ignore = out.append("1");
            } else {
                std::ignore = out.append("0");
            }
        }
        std::ignore = out.append(" ");
    }

    /// @brief dump bit for buffer
    /// @param[in] payload
    /// @param[in] length
    /// @param[in] out
    static void PrintBit4Buffer(uint8_t const* const payload, uint16_t const length, std::string& out) noexcept
    {
        for (uint16_t i{}; i < length; ++i) {
            PrintBit(payload[i], out);
        }
    }

    /// @brief dump bit for vector
    /// @param[in] vec
    /// @param[in] out
    static void PrintBit4Vector(std::vector< uint8_t >& vec, std::string& out) noexcept
    {
        for (uint8_t& v : vec) {
            PrintBit(v, out);
        }
    }

    /// @brief calculate byte number for bit number
    /// @param[in] bitNum bit number
    /// @return byte number
    static inline uint16_t CalcByteNum(uint16_t const bitNum) noexcept
    {
        if ((bitNum % kBitNuMinByte) != 0) {
            return static_cast< uint16_t >((bitNum / kBitNuMinByte) + 1);
        }
        return static_cast< uint16_t >(bitNum / kBitNuMinByte);
    }

    /// @brief calculate remain bit number for bit number
    /// @param[in] bitNum
    /// @return remain bit number
    static inline uint8_t CalcRemainBitNum(uint16_t const bitNum) noexcept
    {
        return static_cast< uint8_t >(bitNum % kBitNuMinByte);
    }

    /// @brief Compare two memory blocks
    /// @param[in] s1 one block
    /// @param[in] s2 the other block
    /// @param[in] length bit length
    /// @return true/false
    static bool MemBlockCompare(uint8_t const* const s1, uint8_t const* const s2, uint16_t const length) noexcept
    {
        if ((nullptr == s1) || (nullptr == s2) || (length == 0)) {
            return false;
        }

        uint8_t remainBit{CalcRemainBitNum(length)};
        uint16_t const totalByte{CalcByteNum(length)};

        for (uint16_t i{}; i < (totalByte - 1); ++i) {
            if (s1[i] != s2[i]) {
                return false;
            }
        }
        if (remainBit == 0) {
            remainBit = kBitNuMinByte;
        }

        uint8_t const moveBit{static_cast< uint8_t >(kBitNuMinByte - remainBit)};
        uint8_t const s1End{*(s1 + totalByte - 1)};
        uint8_t const s2End{*(s2 + totalByte - 1)};
        if (static_cast< uint8_t >(s1End >> moveBit) != static_cast< uint8_t >(s2End >> moveBit)) {
            return false;
        }
        return true;
    }

    /// @brief Get new position info after bit length writing
    /// @param[in] startByte old start position byte offset from 0
    /// @param[in] startBit old start position bit offset 0 1 2 3 4 5 6 7
    /// @param[in] bitLength bit length
    /// @param[in] newStartByte new start position byte offset
    /// @param[in] newStartBit new start position bit offset
    static void NewStartPosition4Stream(uint16_t const startByte,
                                        uint8_t const startBit,
                                        uint16_t const bitLength,
                                        uint16_t& newStartByte,
                                        uint8_t& newStartBit) noexcept
    {
        uint8_t endBit{};
        uint16_t endByte{};
        uint8_t const startRemainBit{static_cast< uint8_t >(kBitNuMinByte - startBit)};
        if (startRemainBit >= bitLength) {
            endByte = startByte;
            endBit  = static_cast< uint8_t >(startBit + bitLength - 1);
        } else {
            endByte = startByte + CalcByteNum(static_cast< uint16_t >(bitLength - startRemainBit));
            endBit  = static_cast< uint8_t >((startBit + bitLength - 1) % kBitNuMinByte);
        }
        newStartBit  = static_cast< uint8_t >(endBit + 1);
        newStartByte = endByte;
        if (newStartBit == kBitNuMinByte) {
            newStartBit = 0;
            newStartByte++;
        }
    }

    /// @brief Copy bit steam to bit stream. If last byte is not 8 bit, fill 0 for remain space.
    /// @param[in] source source data pointer
    /// @param[in] startByte source data offset 0 1 2 ...
    /// @param[in] startBit source bit offset. seq: 0 1 2 3 4 5 6 7
    /// @param[in] bitLength total bit length to copy
    /// @param[in] destination destination data pointer
    /// @param[in] destStartByte destination data byte offset
    /// @param[in] destStartBit destination data bit offset
    static void CopyBitStream2BitStream(uint8_t const* const source,
                                        uint16_t const startByte,
                                        uint8_t const startBit,
                                        uint16_t const bitLength,
                                        uint8_t* const destination,
                                        uint16_t const destStartByte,
                                        uint8_t const destStartBit) noexcept
    {
        // Calculate the gap field of the target first byte (needs padding), then pad it first, then adjust source parameters, call existing function
        uint8_t const destRemainBit{static_cast< uint8_t >(kBitNuMinByte - destStartBit)};

        uint16_t pickBit{destRemainBit};
        if (destRemainBit > bitLength) {
            pickBit = bitLength;
        }
        uint8_t pickValue{};
        SecOCHelper::CopyBitStream2BytePtr(source, startByte, startBit, pickBit, &pickValue);

        destination[destStartByte] >>= (destRemainBit);
        destination[destStartByte] <<= (destRemainBit);
        // Right shift then left shift may have type promotion issues, e.g., byte promoted to int, result may be problematic
        destination[destStartByte] |= pickValue >> ((kBitNuMinByte - destRemainBit) % kBitNuMinByte);
        // One byte is enough, just pad and return
        if (destRemainBit >= bitLength) {
            return;
        }

        uint8_t pickEndBit{};
        uint16_t pickEndByte{};
        uint8_t const startRemainBit{static_cast< uint8_t >(kBitNuMinByte - startBit)};
        if (startRemainBit >= destRemainBit) {
            pickEndByte = startByte;
            pickEndBit  = static_cast< uint8_t >(startBit + destRemainBit - 1);
        } else {
            pickEndByte = static_cast< uint16_t >(startByte + 1);
            pickEndBit  = static_cast< uint8_t >((startBit + destRemainBit - 1) % kBitNuMinByte);
        }
        uint8_t newStartBit{static_cast< uint8_t >(pickEndBit + 1)};
        uint16_t newStartByte{pickEndByte};
        if (newStartBit == kBitNuMinByte) {
            newStartBit = 0;
            newStartByte++;
        }

        // Adjust parameters, call CopyBitStreaming2BytePtr
        return SecOCHelper::CopyBitStream2BytePtr(source, newStartByte, newStartBit,
                                                  static_cast< uint16_t >(bitLength - destRemainBit),
                                                  &destination[destStartByte + 1]);
    }

    /// @brief Copy Bit Streaming to byte pointer, if last byte is not 8 bit, fill 0 for remain space.
    /// @param[in] source source data pointer
    /// @param[in] startByte source data byte offset from 0
    /// @param[in] startBit source data bit offset. sequence is 0 1 2 3 4 5 6 7 including this bit
    /// @param[in] bitLength total copy bit length
    /// @param[in] destination destination data pointer
    static void CopyBitStream2BytePtr(uint8_t const* const source,
                                      uint16_t const startByte,
                                      uint8_t const startBit,
                                      uint16_t const bitLength,
                                      uint8_t* const destination) noexcept
    {
        uint8_t const startRemainBit{static_cast< uint8_t >(kBitNuMinByte - startBit)};
        uint16_t endByte{};
        uint8_t newByte{};
        uint8_t remain{};
        uint16_t position{};
        uint16_t const remainBit{static_cast< uint16_t >(bitLength - startRemainBit)};

        if (bitLength <= startRemainBit) {
            endByte = startByte;
        } else {
            endByte = startByte + CalcByteNum(remainBit);
        }

        if (startByte == endByte) {
            newByte = static_cast< uint8_t >(source[startByte] << startBit);
            newByte >>= (kBitNuMinByte - bitLength);
            newByte <<= (kBitNuMinByte - bitLength);
            *(destination + position) = newByte;
            return;
        }

        for (uint16_t i{startByte}; i <= endByte; ++i) {
            // First byte not written, left for subsequent byte writes, write each middle byte
            if ((i != startByte) && (i != endByte)) {
                newByte                   = remain | static_cast< uint8_t >(source[i] >> (kBitNuMinByte - startBit));
                *(destination + position) = newByte;
                position++;
            } else if (i == endByte) {
                // Handling of last byte, if less than or equal to one byte
                uint8_t lastRemainBit = (remainBit % kBitNuMinByte) != 0 ? (remainBit % kBitNuMinByte) : kBitNuMinByte;
                if ((lastRemainBit + kBitNuMinByte - startBit) <= kBitNuMinByte) {
                    newByte = remain | (source[i] >> (kBitNuMinByte - startBit));
                    newByte >>= (kBitNuMinByte - bitLength % kBitNuMinByte) % kBitNuMinByte;
                    newByte <<= (kBitNuMinByte - bitLength % kBitNuMinByte) % kBitNuMinByte;
                    *(destination + position) = newByte;
                } else {
                    // If more than one byte, need to write two bytes
                    newByte = remain | static_cast< uint8_t >(source[i] >> (kBitNuMinByte - startBit));
                    *(destination + position) = newByte;
                    position++;
                    // Still one byte
                    newByte = static_cast< uint8_t >(source[i] << startBit);
                    newByte >>= (kBitNuMinByte - bitLength % kBitNuMinByte) % kBitNuMinByte;
                    newByte <<= (kBitNuMinByte - bitLength % kBitNuMinByte) % kBitNuMinByte;
                    *(destination + position) = newByte;
                }
                break;
            }
            remain = source[i] << startBit;
        }
    }

    /// @brief Copy Bit Streaming to Vector Byte Buffer, if last byte is not 8 bit, fill 0 for remain space.
    /// @param[in] source source data pointer
    /// @param[in] startByte source data byte offset from 0
    /// @param[in] startBit source data bit offset. sequence is 0 1 2 3 4 5 6 7 including this bit
    /// @param[in] bitLength total copy bit length
    /// @param[in] outputBuffer destination vector buffer
    static void CopyBitStream2ByteBuffer(uint8_t const* const source,
                                         uint16_t const startByte,
                                         uint8_t const startBit,
                                         uint16_t const bitLength,
                                         std::vector< uint8_t >& outputBuffer) noexcept
    {
        uint8_t const startRemainBit{static_cast< uint8_t >(kBitNuMinByte - startBit)};
        uint16_t endByte{};
        uint8_t newByte{};
        uint8_t remain{};
        uint16_t position{};
        uint16_t const remainBit{static_cast< uint16_t >(bitLength - startRemainBit)};
        outputBuffer.resize(static_cast< std::size_t >(CalcByteNum(bitLength)));
        if (bitLength <= startRemainBit) {
            endByte = startByte;
        } else {
            endByte = startByte + CalcByteNum(remainBit);
        }

        if (startByte == endByte) {
            newByte = source[startByte] << startBit;
            newByte >>= (kBitNuMinByte - bitLength);
            newByte <<= (kBitNuMinByte - bitLength);
            *outputBuffer.data() = newByte;
            return;
        }

        for (uint16_t i{startByte}; i <= endByte; ++i) {
            // First byte not written, left for subsequent byte writes, write each middle byte
            if ((i != startByte) && (i != endByte)) {
                newByte = remain | static_cast< uint8_t >(source[i] >> (kBitNuMinByte - startBit));
                *(outputBuffer.data() + position) = newByte;
                position++;
            } else if (i == endByte) {
                // Handling of last byte, if less than or equal to one byte
                uint8_t lastRemainBit{static_cast< uint8_t >(remainBit % kBitNuMinByte)};
                if (lastRemainBit == 0) {
                    lastRemainBit = kBitNuMinByte;
                }
                uint8_t const fill0Move{
                    static_cast< uint8_t >((kBitNuMinByte - bitLength % kBitNuMinByte) % kBitNuMinByte)};
                if (lastRemainBit <= startBit) {
                    newByte = remain | static_cast< uint8_t >(source[i] >> (kBitNuMinByte - startBit));
                    newByte >>= fill0Move;
                    newByte <<= fill0Move;
                    *(outputBuffer.data() + position) = newByte;
                } else {
                    // If more than one byte, need to write two bytes
                    newByte = remain | static_cast< uint8_t >(source[i] >> (kBitNuMinByte - startBit));
                    *(outputBuffer.data() + position) = newByte;
                    position++;
                    // Still one byte
                    newByte = static_cast< uint8_t >(source[i] << startBit);
                    newByte >>= fill0Move;
                    newByte <<= fill0Move;
                    *(outputBuffer.data() + position) = newByte;
                }
                break;
            }
            remain = source[i] << startBit;
        }
    }

#ifdef SECOC_UT_MOCK_CMAC_BY_OPENSSL
    /// @brief just for uint test
    /// @param[in] payload
    /// @param[in] length
    /// @param[in] cryptoName
    /// @param[in] cryptoKeyName
    /// @param[in] macOutput
    /// @return true / false
    static bool DOMacAuthenticateByOpenssl(uint8_t const* const payload,
                                           uint16_t const length,
                                           ara::core::String& cryptoName,
                                           ara::core::String& cryptoKeyName,
                                           ara::core::Vector< uint8_t >& macOutput) noexcept
    {
        std::ignore = cryptoName;
        std::ignore = cryptoKeyName;
        // fixed key for unit test
        constexpr uint16_t const keySize{16};
        uint8_t const key[keySize]{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                   0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
        uint8_t mac[keySize]{};
        size_t macOutLen;
        CMAC_CTX* const ctx{CMAC_CTX_new()};
        std::ignore = CMAC_Init(ctx, static_cast< const void* >(key), keySize, EVP_aes_128_cbc(), nullptr);
        std::ignore = CMAC_Update(ctx, payload, static_cast< std::size_t >(length));
        std::ignore = CMAC_Final(ctx, static_cast< unsigned char* >(mac), &macOutLen);
        macOutput.resize(macOutLen);
        std::ignore = std::copy(mac, mac + macOutLen, macOutput.data());
        CMAC_CTX_free(ctx);
        return true;
    }
#endif
public:
    /// @brief disable
    ~SecOCHelper() noexcept = delete;
    /// @brief disable
    SecOCHelper() noexcept = delete;
    /// @brief disable
    /// @param[in] other
    SecOCHelper(SecOCHelper const& other) noexcept = delete;
    /// @brief disable
    /// @param[in] other
    /// @return none
    SecOCHelper& operator=(SecOCHelper const& other) noexcept = delete;
    /// @brief disable
    /// @param[in] other
    SecOCHelper(SecOCHelper const&& other) noexcept = delete;
    /// @brief disable
    /// @param[in] other
    /// @return none
    SecOCHelper& operator=(SecOCHelper const&& other) noexcept = delete;
};

}  // namespace secoc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif