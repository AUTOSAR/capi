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
/// @file       raw_stream_ethernet.h
/// @brief
/// @details
/// @date       2021-12-02
/// @author     cuijiusen
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_RAW_DATA_TCP_H_
#define ARA_COM_RAW_DATA_TCP_H_

#include "ara/com/internal/raw/raw_base.h"
#include "ara/com/internal/raw/raw_runtime.h"
#include "ara/core/string.h"
#include "isoft/naicpp/global_evloop.h"
#include "nai/io/nai_buf.h"

namespace ara {
namespace com {
namespace internal {
namespace raw {

/// @brief Raw TCP Server Impl class
class RawEthernetTcp final : public BaseRaw
{
public:
    /// @brief Constructor
    /// @param[in] instance Instance specifier
    /// @exception
    explicit RawEthernetTcp(ara::core::InstanceSpecifier const& instance) noexcept;

    /// @brief Copy constructor
    /// @param[in] other the object to be copy.
    RawEthernetTcp(RawEthernetTcp const& other) noexcept = delete;

    /// @brief Copy assignment function
    /// @param[in] other the object to be copy.
    /// @return RawEthernetTcp object
    RawEthernetTcp& operator=(RawEthernetTcp const& other) noexcept = delete;

    /// @brief Move constructor
    /// @param[in] other the object to be move.
    RawEthernetTcp(RawEthernetTcp&& other) noexcept = delete;

    /// @brief Move assignment function
    /// @param[in] other the object to be move.
    /// @return RawEthernetTcp object
    RawEthernetTcp& operator=(RawEthernetTcp&& other) noexcept = delete;

    /// @brief Destructor
    ~RawEthernetTcp() noexcept final;

    /// @brief Server wait for connection
    /// @param[in] timeout Timeout value for this operation.
    /// @return void if successful, otherwise an error code indicating the error.
    ara::core::Result< void > WaitForConnection(int64_t timeout = -1) noexcept final;

    /// @brief Client connect to server
    /// @param[in] timeout Timeout value for this operation.
    /// @return void if successful, otherwise an error code indicating the error.
    ara::core::Result< void > Connect(int64_t timeout = -1) noexcept final;

    /// @brief TCP connection write data
    /// @param[in] data pointer to the byte array to send. A SamplePtr is used to get std::unique_ptr semantics.
    /// @param[in] maxLength The number of bytes to write to the stream.
    /// @param[in] timeout Parameter to assign a timeout for this operation.
    /// @return the actual number of bytes written if succesful, otherwise an error code indicating the error.
    ara::core::Result< size_t > WriteData(ara::com::SamplePtr< uint8_t > data,
                                          size_t maxLength,
                                          int64_t timeout = -1) noexcept final;

    /// @brief Close TCP connection
    /// @param[in] timeout Parameter to assign a timeout for this operation.
    /// @return void if successful, otherwise an error code indicating the error.
    ara::core::Result< void > Shutdown(int64_t timeout = -1) noexcept final;

    /// @brief TCP connection read data
    /// @param[in] maxLength The number of bytes to read from the stream.
    /// @param[in] timeout Timeout value for this operation.
    /// @return a struct of type ReadDataResult if succesful, otherwise an error code indicating the error.
    ara::core::Result< ReadDataResult > ReadData(size_t maxLength, int64_t timeout = -1) noexcept final;

    /// @brief Get protocol name
    /// @return server connect protocl name
    ara::core::String const Name() noexcept final { return kName; };

private:
    /// @brief Accept connection function
    /// @return 0 success/ -1 failed
    int32_t _rawAccept() noexcept;

    /// @brief Accept callback function
    /// @return 0 success/ -1 failed
    int32_t _acceptCb(std::int32_t const events) noexcept;

    /// @brief Client connection function
    std::int32_t _connectCb(std::int32_t events) noexcept;

    /// @brief the config pointer
    std::shared_ptr< RawConfig > config_{};

    nai_server_t server_{};
    /// @brief fd IobaseType
    nai_iobase_t fd_{};
    // evloop.
    std::shared_ptr< isoft::naicpp::EvLoop > loop_{};
    // ara::com::raw::RawErrc rawCode{ara::com::raw::RawErrc::kUnknownError};
    /// @brief raw use protol name
    ara::core::String const kName{"TCP"};
    std::mutex rawMux_;
    std::condition_variable cv_;

    std::atomic_bool rdFlag_{false};
    std::atomic_bool downReady_{false};

    std::function< void() > readNotify_{};

    std::function< void() > writeNotify_{};

    std::function< void() > closeNotify_{};
};

/// @brief Raw UDP Server impl
class RawEthernetUdp final : public BaseRaw
{
public:
    /// @brief Constructor
    /// @param[in] instance Instance specifier
    /// @exception
    explicit RawEthernetUdp(ara::core::InstanceSpecifier const& instance) noexcept;

    /// @brief Copy constructor
    /// @param[in] other the object to be copy.
    RawEthernetUdp(RawEthernetUdp const& other) noexcept = delete;

    /// @brief Copy assignment function
    /// @param[in] other the object to be copy.
    /// @return RawEthernetUdp object
    RawEthernetUdp& operator=(RawEthernetUdp const& other) noexcept = delete;

    /// @brief Move constructor
    /// @param[in] other the object to be move.
    RawEthernetUdp(RawEthernetUdp&& other) noexcept = delete;

    /// @brief Move assignment function
    /// @param[in] other the object to be move.
    /// @return RawEthernetUdp object
    RawEthernetUdp& operator=(RawEthernetUdp&& other) noexcept = delete;

    /// @brief Destructor
    ~RawEthernetUdp() noexcept final;

    /// @brief Server wait for connection
    /// @param[in] timeout Timeout value for this operation.
    /// @return void if successful, otherwise an error code indicating the error.
    ara::core::Result< void > WaitForConnection(int64_t timeout = -1) noexcept final;

    /// @brief Client connect to server
    /// @param[in] timeout Timeout value for this operation.
    /// @return void if successful, otherwise an error code indicating the error.
    ara::core::Result< void > Connect(int64_t timeout = -1) noexcept final;

    /// @brief UDP connection write data
    /// @param[in] data pointer to the byte array to send. A SamplePtr is used to get std::unique_ptr semantics.
    /// @param[in] maxLength The number of bytes to write to the stream.
    /// @param[in] timeout Parameter to assign a timeout for this operation.
    /// @return the actual number of bytes written if succesful, otherwise an error code indicating the error.
    ara::core::Result< size_t > WriteData(ara::com::SamplePtr< uint8_t > data,
                                          size_t maxLength,
                                          int64_t timeout = -1) noexcept final;

    /// @brief Close UDP connection
    /// @param[in] timeout Parameter to assign a timeout for this operation.
    /// @return void if successful, otherwise an error code indicating the error.
    ara::core::Result< void > Shutdown(int64_t timeout = -1) noexcept final;

    /// @brief UDP connection read data
    /// @param[in] maxLength The number of bytes to read from the stream.
    /// @param[in] timeout Timeout value for this operation.
    /// @return a struct of type ReadDataResult if succesful, otherwise an error code indicating the error.
    ara::core::Result< ReadDataResult > ReadData(size_t maxLength, int64_t timeout = -1) noexcept final;
    /// @brief Get protocol name
    /// @return server connect protocl name
    ara::core::String const Name() noexcept final { return kName; };

private:
    /// @brief Server read/write callback
    /// @param[in] events
    /// @return
    int32_t _serverReadWriteCb(int32_t events) noexcept;

    /// @brief Client connection callback
    /// @param[in] events
    /// @return
    int32_t _connectCb(int32_t events) noexcept;

private:
    /// @brief evloop
    std::shared_ptr< isoft::naicpp::EvLoop > loop_{};

    /// @brief the config pointer
    std::shared_ptr< RawConfig > config_{};

    // /// @brief the nai_dgram_t pointer
    // std::unique_ptr<nai_dgram_t> dgram_{};

    /// @brief peer Address( multicast:local_host udp:peer address)
    nai_sockaddr_t peerAddr_{};

    /// @brief peer address stuct lenght
    int32_t addrLen_{};

    /// @brief mul peer Address
    nai_sockaddr_t mulAddr_{};

    /// @brief peer address stuct lenght
    int32_t mulAddrLen_{};

    /// @brief IobaseType for client connect
    nai_dgram_t fd_{};

    std::mutex rawMux_;
    std::condition_variable cv_;

    std::atomic_bool rdFlag_{false};
    std::atomic_bool downReady_{false};

    std::function< void() > readNotify_{};

    std::function< void() > writeNotify_{};

    std::function< void() > closeNotify_{};
    /// @brief raw use protol name
    ara::core::String const kName{"UDP"};
};
}  // namespace raw
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
