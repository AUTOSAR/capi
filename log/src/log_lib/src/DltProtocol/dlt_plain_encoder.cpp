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
/// @file       dlt_plain_encoder.cpp
/// @brief      Protocol encapsulation V2 for non-modeled logs
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "dlt_plain_encoder.h"

#include <sys/time.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <tuple>

namespace ara {
namespace log {
namespace internal {

DltPlainEncoder::DltPlainEncoder() noexcept
{
    // Reserve space for the timestamp during construction: "YYYY/MM/DD HH:mm:ss.mmm" = 24 bytes
    // Timestamp format example: "2026/01/15 18:53:04.948 "
    std::int32_t const kTimestampLen = 23;
    std::size_t &payloadSize         = this->PayloadSize();
    std::uint8_t *payloadData        = this->PayloadData();
    if (payloadData != nullptr && payloadSize + 24 <= BufferCapacity()) {
        // Fill the reserved space with spaces
        std::memset(payloadData + payloadSize, ' ', kTimestampLen);
        payloadSize += kTimestampLen;
    }
}

void DltPlainEncoder::AppendString(std::string const &str) noexcept { return AppendGeneric(str.data(), str.size(), 0); }

void DltPlainEncoder::AppendRawData(void const *data,
                                    std::uint16_t const &length,
                                    DltFormatType type,
                                    std::string const &name,
                                    bool const &withVarInfo) noexcept
{
    std::ignore = type;
    std::ignore = name;
    std::ignore = withVarInfo;
    return AppendGeneric(data, length, 0);
}

void DltPlainEncoder::AppendGeneric(const void *datap,
                                    std::size_t const &datalen,
                                    std::uint32_t const &typeInfo) noexcept
{
    std::ignore               = typeInfo;
    std::size_t &payloadSize  = this->PayloadSize();
    std::uint8_t *payloadData = this->PayloadData();
    /// Leave \0
    if (payloadData == nullptr || datap == nullptr || datalen == 0 || payloadSize + datalen + 2 > BufferCapacity()) {
        return;
    }
    payloadData[payloadSize] = ' ';
    payloadSize++;
    std::memcpy(payloadData + payloadSize, datap, datalen);
    payloadSize += datalen;
}
std::int32_t DltPlainEncoder::Encode() noexcept
{
    const std::uint32_t kDecOneThousand = 1000;
    const std::uint32_t kTimestampLen   = 23;  // "YYYY/MM/DD HH:mm:ss.mmm"
    const std::uint32_t kBufferSize     = 64;
    DltTimeStamp &dltTs                 = this->GetDltTimeStamp();
    std::uint8_t *payloadData           = this->PayloadData();
    std::size_t &payloadSize            = this->PayloadSize();

    std::time_t const sec{static_cast< std::time_t >(dltTs.seconds)};
    std::uint32_t const mPart{static_cast< std::uint32_t >(dltTs.uSeconds / kDecOneThousand)};

    std::tm ttm{};
    localtime_r(&sec, &ttm);

    char buf[kBufferSize]{};
    const int len = std::snprintf(buf, sizeof(buf), "%04d/%02d/%02d %02d:%02d:%02d.%03u", ttm.tm_year + 1900,
                                  ttm.tm_mon + 1, ttm.tm_mday, ttm.tm_hour, ttm.tm_min, ttm.tm_sec, mPart);

    if (len == static_cast< int >(kTimestampLen)) {
        // Write the timestamp into the reserved space (first 23 bytes, the 24th byte is a space, followed by subsequent parameters)
        if (payloadData != nullptr) {
            std::memcpy(payloadData, buf, kTimestampLen);
            // Keep the 24th byte as a space
        }
    }
    if (payloadData == nullptr) {
        return 0;
    }
    payloadData[payloadSize] = '\0';
    payloadSize++;
    return payloadSize;
}
std::uint8_t *DltPlainEncoder::EncodeWithInfo(std::string const &ecuId,
                                              std::string const &appId,
                                              std::string const &ctxId,
                                              std::uint32_t const &sessionId,
                                              std::int32_t const &msgCount,
                                              std::int32_t const &timestamp,
                                              std::uint8_t const &logLevelel) noexcept
{
    std::ignore               = ecuId;
    std::ignore               = appId;
    std::ignore               = ctxId;
    std::ignore               = sessionId;
    std::ignore               = msgCount;
    std::ignore               = timestamp;
    std::ignore               = logLevelel;
    std::size_t &payloadSize  = this->PayloadSize();
    std::ignore               = payloadSize;
    std::uint8_t *payloadData = this->PayloadData();

    return payloadData;
}

}  // namespace internal
}  // namespace log
}  // namespace ara