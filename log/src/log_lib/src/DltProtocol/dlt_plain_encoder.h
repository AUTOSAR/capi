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
/// @file       dlt_plain_encoder.h
/// @brief      Protocol encapsulation V2 for non-modeled logs
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_DLT_PLAIN_ENCODER__
#define __LOG_INTERNAL_DLT_PLAIN_ENCODER__

// Only include the necessary content to avoid introducing unnecessary dependencies
#include "dlt_protocol_encoder.h"
#include "internal/dlt_constants.h"
#include "internal/dlt_structures.h"
namespace ara {
namespace log {
namespace internal {

/// @brief Protocol encapsulation V2 for non-modeled logs
class DltPlainEncoder final : public DltProtocolEncoder
{
public:
    /// @brief Constructor
    /// @param[in] buffer Cache pointer
    /// @param[in] bufferSize Cache size
    DltPlainEncoder() noexcept;

    /// @brief Destructor
    ~DltPlainEncoder() final = default;

    // Disable copy and move operations
    DltPlainEncoder(DltPlainEncoder const &) = delete;
    DltPlainEncoder &operator=(DltPlainEncoder const &) = delete;
    DltPlainEncoder(DltPlainEncoder &&)                 = delete;
    DltPlainEncoder &operator=(DltPlainEncoder &&) = delete;

    /// @brief Append a regular string
    void AppendString(std::string const &str) noexcept;

    /// @brief Append raw data
    void AppendRawData(void const *data,
                       std::uint16_t const &length,
                       DltFormatType type,
                       std::string const &name,
                       bool const &withVarInfo) noexcept final;

    /// @brief Append generic data
    void AppendGeneric(const void *datap, std::size_t const &datalen, std::uint32_t const &typeInfo) noexcept final;
    std::int32_t Encode() noexcept final;

    /// @brief Encode the log
    std::uint8_t *EncodeWithInfo(std::string const &ecuId,
                                 std::string const &appId,
                                 std::string const &ctxId,
                                 std::uint32_t const &sessionId,
                                 std::int32_t const &msgCount,
                                 std::int32_t const &timestamp,
                                 std::uint8_t const &logLevelel) noexcept;
    /// @brief Get the encoder version
    std::string GetEncoderVersion() noexcept { return "1.0.0"; }

    std::uint8_t *HeaderBuffer() noexcept final { return nullptr; }
    std::uint32_t HeaderSize() noexcept final { return 0; }
    std::uint8_t *BodyBuffer() noexcept final { return this->PayloadData(); }
    std::uint32_t BodySize() noexcept final { return static_cast< std::uint32_t >(this->PayloadSize()); }
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // __LOG_INTERNAL_DLT_PLAIN_ENCODER__
