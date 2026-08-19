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
/// @file       buffer.h
/// @brief
/// @details
/// @date       2021-04-16
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_ISOFTCRC_CRC_BUFFER_H
#define ISOFT_ISOFTCRC_CRC_BUFFER_H

#include <cstdint>
#include <ostream>
#include <vector>

namespace isoft {

namespace crc {

using Buffer = std::vector< uint8_t >;

/// @brief Represents a view over data held in another container
class BufferView
{
public:
    BufferView() = delete;

    /// @brief Constructs a buffer view from a byte sequence represented as a pointer to the data and its length
    ///
    /// @param dataPtr     pointer to data
    /// @param dataLength  number of elements in @a dataPtr
    ///
    /// @throws std::invalid_argument is thrown if pointer to data is null or data length is zero
    BufferView(const uint8_t* dataPtr, size_t dataLength);

    /// @brief Constructs a buffer view from std::vector of bytes.
    ///
    /// @param buf    vector of bytes
    explicit BufferView(const Buffer& buf) noexcept : dataPtr_{buf.data()}, dataLength_{buf.size()} {}

    /// @brief Constructs a buffer view from the part of a vector of bytes starting from the beginning.
    ///
    /// @param buf  vector of bytes
    /// @param length  length of the buffer in bytes
    ///
    /// @throws std::length_error is thrown if given length is equal to zero or exceeds buf size
    BufferView(const Buffer& buf, size_t length);

    /// @brief Constructs a buffer view from part of a vector set by begin and end positions.
    ///
    /// @param buf  vector of bytes
    /// @param begin start position of a view
    /// @param end   end position of a view
    ///
    /// @throws std::invalid_argument is thrown if begin position is greater than end position
    /// @throws std::out_of_range is thrown if end position is greater than buf size
    BufferView(const Buffer& buf, size_t begin, size_t end);

    /// @brief Defaulted copy constructor
    BufferView(const BufferView& oth) = default;

    /// @brief Constructs a buffer view from other buffer view by moving pointer to data and length
    ///
    /// @param oth    another buffer view
    BufferView(BufferView&& oth) noexcept;

    /// @brief Defaulted assignment operator
    BufferView& operator=(const BufferView& oth) = default;

    /// @brief move-assign buffer view from antoher buffer view
    ///
    /// @param oth    another buffer view
    BufferView& operator=(BufferView&& oth) noexcept;

    ~BufferView() noexcept = default;

    /// @brief Returns starting position of a view
    ///
    /// @return  pointer to the beginning of the buffer
    const uint8_t* begin() const noexcept { return dataPtr_; }

    /// @brief Returns end position of a view
    ///
    /// @return  pointer to the end of the buffer
    const uint8_t* end() const noexcept { return dataPtr_ + dataLength_; }

    /// @brief Check if buffer view points to the data
    ///
    /// @return  true if buffer view represent real data buffer, false - othervise
    bool isValid() const noexcept;

private:
    /// @brief   pointer to data represented by a buffer view
    const uint8_t* dataPtr_;

    /// @brief   length of the data represented by a buffer view.
    size_t dataLength_;
};

}  // namespace crc
}  // namespace isoft

/// @brief Represent buffer content as a HEX-string suitable for printing
///
/// @param os output stream
/// @param buffer   buffer to be represented as a sequence of hex values
///
/// @return output stream
std::ostream& operator<<(std::ostream& os, const isoft::crc::Buffer& buffer);

#endif
