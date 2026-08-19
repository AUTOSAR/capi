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
/// @file       dlt_protocol_nomodel_encoder.h
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

#ifndef __INTERNAL_LOG_STREAM_PREIVATE_DATA_VERBOSE__
#define __INTERNAL_LOG_STREAM_PREIVATE_DATA_VERBOSE__

#include "dlt_protocol_encoder.h"

namespace ara {
namespace log {
namespace internal {
/// @brief Data cache definition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00325
/// @trace_id_dd=DD_LOG_01721
/// @needwork = ad
/// @endcode
using BUFFERTYPE = std::vector< std::uint8_t >;

/// @brief
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00202
/// @trace_id_dd=DD_LOG_00404
/// @needwork = ad
/// @endcode
class DltProtocolNonModelEncoder final : public DltProtocolEncoder
{
public:
    using DltProtocolEncoder::AppendBin16;
    using DltProtocolEncoder::AppendBin32;
    using DltProtocolEncoder::AppendBin64;
    using DltProtocolEncoder::AppendBin8;
    using DltProtocolEncoder::AppendBool;
    using DltProtocolEncoder::AppendBytes;
    using DltProtocolEncoder::AppendCString;
    using DltProtocolEncoder::AppendDouble;
    using DltProtocolEncoder::AppendFloat;
    using DltProtocolEncoder::AppendHex16;
    using DltProtocolEncoder::AppendHex32;
    using DltProtocolEncoder::AppendHex64;
    using DltProtocolEncoder::AppendHex8;
    using DltProtocolEncoder::AppendInt16;
    using DltProtocolEncoder::AppendInt32;
    using DltProtocolEncoder::AppendInt64;
    using DltProtocolEncoder::AppendInt8;
    using DltProtocolEncoder::AppendUInt16;
    using DltProtocolEncoder::AppendUInt32;
    using DltProtocolEncoder::AppendUInt64;
    using DltProtocolEncoder::AppendUInt8;

    /// @brief Constructor
    /// @param[in]  lineLevel Log level
    /// @param[in]  msgcnt Message count
    /// @param[in]  logtmsp Timestamp
    /// @param[in]  loggerData Common data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00203
    /// @trace_id_dd=DD_LOG_00405
    /// @needwork = ad
    /// @endcode
    DltProtocolNonModelEncoder() noexcept;

    /// @brief Copy constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00355
    /// @trace_id_dd=DD_LOG_01754
    /// @needwork = ad
    /// @endcode
    DltProtocolNonModelEncoder(DltProtocolNonModelEncoder const &other) = delete;
    /// @brief Assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00356
    /// @trace_id_dd=DD_LOG_01755
    /// @needwork = ad
    /// @endcode
    DltProtocolNonModelEncoder &operator=(DltProtocolNonModelEncoder const &other) = delete;
    /// @brief Move constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_02341
    /// @trace_id_dd=DD_LOG_02740
    /// @needwork = ad
    /// @endcode

    DltProtocolNonModelEncoder(DltProtocolNonModelEncoder const &&other) = delete;
    /// @brief Move assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_02342
    /// @trace_id_dd=DD_LOG_02741
    /// @needwork = ad
    /// @endcode

    DltProtocolNonModelEncoder &operator=(DltProtocolNonModelEncoder const &&other) = delete;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00204
    /// @trace_id_dd=DD_LOG_00406
    /// @needwork = ad
    /// @endcode
    ~DltProtocolNonModelEncoder() noexcept final;
    ///---------Related to logdata----------

    /// @brief Append a regular string
    /// @param[in]  str Regular string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00205
    /// @trace_id_dd=DD_LOG_00407
    /// @needwork = ad
    /// @endcode
    void AppendString(std::string const &str) noexcept final;
    /// @brief Append raw data
    /// @param[in]  data Raw data
    /// @param[in]  length Length
    /// @param[in]  type Type
    /// @param[in]  name Name
    /// @param[in]  with_var_info Info
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00206
    /// @trace_id_dd=DD_LOG_00408
    /// @needwork = ad
    /// @endcode
    void AppendRawData(void const *data,
                       std::uint16_t const &length,
                       DltFormatType type,
                       std::string const &name,
                       bool const &withVarInfo) noexcept final;

    /// @brief Append std::int8 int16 int32 int64 uint8 uint16 uint32 uint64 bool
    /// @param[in]  datap Data pointer
    /// @param[in]  datalen Data length
    /// @param[in]  type_info Information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00207
    /// @trace_id_dd=DD_LOG_00409
    /// @needwork = ad
    /// @endcode
    void AppendGeneric(const void *datap, std::size_t const &datalen, std::uint32_t const &typeInfo) noexcept final;

    /// @brief Encode the log
    /// @needwork = ad
    std::int32_t Encode() noexcept final;

    std::uint8_t *HeaderBuffer() noexcept final { return this->HeaderData(); }
    std::uint32_t HeaderSize() noexcept final { return this->HeaderSizeField(); }
    std::uint8_t *BodyBuffer() noexcept final { return this->PayloadData(); }
    std::uint32_t BodySize() noexcept final { return static_cast< std::uint32_t >(this->PayloadSize()); }

private:
};
}  // namespace internal
}  // namespace log
}  // namespace ara

#endif