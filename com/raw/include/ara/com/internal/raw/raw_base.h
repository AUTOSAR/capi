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
/// @file       raw_base.h
/// @brief
/// @details
/// @date       2021-12-02
/// @author     cuijiusen
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_RAW_BASE_CONNECTION_H_
#define ARA_COM_RAW_BASE_CONNECTION_H_

#include <chrono>
#include <memory>
#include <string>

#include "ara/com/internal/raw/raw_config.h"
#include "ara/com/raw/raw_error_domain.h"
#include "ara/com/types.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "nai/io/nai_io.h"
#include "nai/os/nai_socket.h"
#include "nai/os/nai_task.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_util.h"

namespace ara {
namespace com {
namespace raw {
/// @brief ReadDataResult structure used as return value of ReadData()
class ReadDataResult;
}  // namespace raw
}  // namespace com
}  // namespace ara
namespace ara {
namespace com {
namespace internal {
namespace raw {

/// @brief ReadDataResult structure used as return value of ReadData()
using ReadDataResult = ara::com::raw::ReadDataResult;

/// @brief ara::core::result<ReadDataResult>
using AraCoreDataResult = ara::core::Result< ReadDataResult >;
/// @brief ara::core::Result<size_t>
using AraCoreSizeResult = ara::core::Result< size_t >;

/// @brief Alias
using CharType = char;

/// @brief Raw server base class
class BaseRaw
{
protected:
    /// @brief Server connection status
    enum class ConnectionState : int32_t
    {
        /// @brief Socket connection unavailable
        kRawConnectionStateNotAvailable = 0,
        /// @brief Socket waiting for connection
        kRawConnectionStateWaitForConnect,
        /// @brief Socket connected
        kRawConnectionStateConnect
    };

protected:
    /// @brief Constructor
    BaseRaw() noexcept = default;
    /// @brief Move constructor
    /// @param[in] other the object to be move.
    BaseRaw(BaseRaw&& other) noexcept = default;

    /// @brief Move assignment function
    /// @param[in] other the object to be move.
    /// @return BaseRaw object
    BaseRaw& operator=(BaseRaw&& other) noexcept = default;

public:
    /// @brief Copy constructor
    /// @param[in] other the object to be copy.
    BaseRaw(BaseRaw const& other) noexcept = delete;

    /// @brief Copy assignment function
    /// @param[in] other the object to be copy.
    /// @return BaseRaw object
    BaseRaw& operator=(BaseRaw const& other) noexcept = delete;

public:
    /// @brief Destructor
    virtual ~BaseRaw() noexcept = default;

    /// @brief Raw byte stream server create connection function
    /// @param[in] timeout Timeout value for this operation.
    /// @return void if successful, otherwise an error code indicating the error.
    virtual ara::core::Result< void > WaitForConnection(int64_t timeout = -1) noexcept = 0;

    /// @brief Raw byte stream client connection function
    /// @param[in] timeout Timeout value for this operation.
    /// @return void if successful, otherwise an error code indicating the error.
    virtual ara::core::Result< void > Connect(int64_t timeout = -1) noexcept = 0;

    /// @brief Raw byte stream read data
    /// @param[in] maxLength The number of bytes to read from the stream.
    /// @param[in] timeout Timeout value for this operation.
    /// @return a struct of type ReadDataResult if succesful, otherwise an error code indicating the error.
    /// @exception no
    virtual AraCoreDataResult ReadData(size_t maxLength, int64_t timeout = -1) noexcept = 0;

    /// @brief Raw byte stream write data
    /// @param[in] data pointer to the byte array to send. A SamplePtr is used to get std::unique_ptr semantics.
    /// @param[in] maxLength The number of bytes to write to the stream.
    /// @param[in] timeout Parameter to assign a timeout for this operation.
    /// @return the actual number of bytes written if succesful, otherwise an error code indicating the error.
    /// @exception no
    virtual AraCoreSizeResult WriteData(ara::com::SamplePtr< uint8_t > data,
                                        size_t maxLength,
                                        int64_t timeout = -1) noexcept = 0;

    /// @brief Close raw byte stream connection
    /// @param[in] timeout Parameter to assign a timeout for this operation.
    /// @return void if successful, otherwise an error code indicating the error.
    /// @exception no
    virtual ara::core::Result< void > Shutdown(int64_t timeout = -1) noexcept = 0;

    /// @brief Get protocol name
    /// @return server connect protocl name
    virtual ara::core::String const Name() noexcept = 0;

protected:
    /// @brief Set raw byte stream connection status
    /// @param[in] s the connection state to be set
    inline void _SetConnectionState(ConnectionState const s) noexcept { state_ = s; };

    /// @brief Get raw byte stream connection status
    /// @return Connection state
    inline ConnectionState _GetConnectionState() const noexcept { return state_; };

    /// @brief Communication error and raw error conversion
    /// @param[in] err system errno
    /// @return void if successful, otherwise an error code indicating the error.
    ara::core::Result< void > _Errno2RawErrc(int32_t err) noexcept;

private:
    /// @brief Raw byte stream connection status
    ConnectionState state_{ConnectionState::kRawConnectionStateNotAvailable};
};

}  // namespace raw
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif