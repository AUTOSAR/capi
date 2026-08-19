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
/// @file       log_buffer.cpp
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

#include "log_buffer.h"

#include <cstring>

#include "private_log.h"

namespace ara {
namespace log {
namespace internal {
/// @brief Constructor
/// @param[in]  len
LogBuffer::LogBuffer(std::size_t const &len) noexcept : configLen_{len}
{
    buffer_ = std::make_unique< std::vector< std::uint8_t > >(len);
    buffer_->reserve(len);
    buffer_->resize(len);
}

/// @brief Push memory, after storing data, return remaining space
/// @param[in]  buffer
/// @param[in]  len
/// @return
std::size_t LogBuffer::Push(std::uint8_t const *const buffer, std::size_t const &len) noexcept
{
    if (this->configLen_ < len) {
        LOGERROR(__func__) << ",error too small ,configLen_: " << this->configLen_ << " ,len : " << len;

        return 0U;
    }
    if (IsFull(len)) {
        currLen_ = 0U;
    }
    std::ignore = memcpy(static_cast< std::uint8_t * >(buffer_->data()) + currLen_, buffer, len);
    currLen_    = currLen_ + len;
    return currLen_;
}
/// @brief Clear memory
/// @return
void LogBuffer::Clear() noexcept
{
    std::ignore = memset(buffer_->data(), 0, currLen_);
    currLen_    = 0U;
}
/// @brief Get storage capacity
/// @return
std::size_t LogBuffer::GetCapacity() const noexcept { return this->configLen_; }
/// @brief Whether the capacity is full
/// @param[in]  len
/// @return
bool LogBuffer::IsFull(std::size_t const &len) const noexcept
{
    // std::cout << "configLen_: " << this->configLen_ << ",write len: " << len << ",currLen_: " << currLen_ << std::endl;
    if (currLen_ + len > this->configLen_) {
        return true;
    }
    return false;
}
/// @brief Raw data
/// @return
std::uint8_t *LogBuffer::Data() const noexcept { return buffer_->data(); }
/// @brief Current length
/// @return
std::size_t LogBuffer::Len() const noexcept { return currLen_; }
}  // namespace internal
}  // namespace log
}  // namespace ara
