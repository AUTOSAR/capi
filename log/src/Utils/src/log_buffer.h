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
/// @file       log_buffer.h
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/Utils
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00006,LOG_SR_00015
/// @unit_name = log_buffer
/// @unit_description=Internal support function of Dlt lib, utility class, used for log caching
/// @endcode
///
/// ================================================================

#ifndef __LOG_INTERNAL_BUFFER__
#define __LOG_INTERNAL_BUFFER__

#include <array>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

namespace ara {
namespace log {
namespace internal {
/// @brief Cache log data to reduce hard disk reads/writes, copy prohibited.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00120
/// @trace_id_dd=DD_LOG_00170
/// @needwork = ad
/// @endcode
class LogBuffer final
{
    /// @brief Cache type definition
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00318
    /// @trace_id_dd=DD_LOG_01714
    /// @needwork = ad
    /// @endcode
    using BUFFERTYPE = std::vector< std::uint8_t >;

public:
    /// @brief LogBuffer LogBuffer
    /// @param[in]  len
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00121
    /// @trace_id_dd=DD_LOG_00171
    /// @needwork = ad
    /// @endcode
    explicit LogBuffer(std::size_t const& len) noexcept;
    /// @brief RingBuffer destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00122
    /// @trace_id_dd=DD_LOG_00172
    /// @needwork = ad
    /// @endcode
    ~LogBuffer() = default;
    /// @brief Copy constructor
    /// @param[in]  other Other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00123
    /// @trace_id_dd=DD_LOG_00173
    /// @needwork = ad
    /// @endcode
    LogBuffer(LogBuffer const& other) = delete;
    /// @brief Move constructor
    /// @param[in]  other Other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00124
    /// @trace_id_dd=DD_LOG_00174
    /// @needwork = ad
    /// @endcode
    LogBuffer(LogBuffer const&& other) = delete;
    /// @brief Copy assignment operator
    /// @param[in]  other Other
    /// @return  Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00125
    /// @trace_id_dd=DD_LOG_00175
    /// @needwork = ad
    /// @endcode
    LogBuffer& operator=(LogBuffer const& other) = delete;
    /// @brief Move assignment operator
    /// @param[in]  other Other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00126
    /// @trace_id_dd=DD_LOG_00176
    /// @needwork = ad
    /// @endcode
    LogBuffer& operator=(LogBuffer const&& other) = delete;
    /// @brief Put data into cache
    /// @param[in]  buffer
    /// @param[in]  len
    /// @return Size of stored data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00127
    /// @trace_id_dd=DD_LOG_00177
    /// @needwork = ad
    /// @endcode
    std::size_t Push(std::uint8_t const* const buffer, std::size_t const& len) noexcept;
    /// @brief Clear cache
    /// @return 0ok <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00128
    /// @trace_id_dd=DD_LOG_00178
    /// @needwork = ad
    /// @endcode
    void Clear() noexcept;
    /// @brief Get cache capacity
    /// @return Capacity size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00129
    /// @trace_id_dd=DD_LOG_00179
    /// @needwork = ad
    /// @endcode
    std::size_t GetCapacity() const noexcept;
    /// @brief Whether the cache is full enough to store data of the specified length
    /// @param[in]  len
    /// @return  true cache full, false not full
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00130
    /// @trace_id_dd=DD_LOG_00180
    /// @needwork = ad
    /// @endcode
    bool IsFull(std::size_t const& len) const noexcept;
    /// @brief Raw data pointer of the cache
    /// @return Data pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00131
    /// @trace_id_dd=DD_LOG_00181
    /// @needwork = ad
    /// @endcode
    std::uint8_t* Data() const noexcept;
    /// @brief Actually used cache length
    /// @return Current length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00132
    /// @trace_id_dd=DD_LOG_00182
    /// @needwork = ad
    /// @endcode
    std::size_t Len() const noexcept;

private:
    /// @brief buffer_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00120
    /// @trace_id_dd=DD_LOG_00183
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< BUFFERTYPE > buffer_{};
    /// @brief len_ Size of cache already used
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00120
    /// @trace_id_dd=DD_LOG_00184
    /// @needwork = dda
    /// @endcode
    std::size_t currLen_{0U};
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00120
    /// @trace_id_dd=DD_LOG_00185
    /// @needwork = dda
    /// @endcode
    std::size_t configLen_{0U};
    /// @brief Cache lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00120
    /// @trace_id_dd=DD_LOG_00186
    /// @needwork = dda
    /// @endcode
    std::mutex bufferLocker_{};
};

}  // namespace internal

}  // namespace log

}  // namespace ara
#endif