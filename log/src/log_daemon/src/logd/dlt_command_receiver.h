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
/// @file       dlt_command_receiver.h
/// @brief      size of receiver buffer
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltLogd
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00003,LOG_SR_00009
/// @unit_name = dlt_command_receiver
/// @unit_description=Backend module of Dlt for receiving client information types of log records sent from AA.
/// @endcode
///
/// ================================================================

#ifndef __dlt_log_command_reciver__
#define __dlt_log_command_reciver__
#include <cstdint>
#include <string>

#include "control_msg_define.h"
#include "log_lib/include/ara/log/internal/dlt_structures.h"
namespace ara {
namespace log {
namespace internal {

/// @brief Log reception and analysis
/// @code{.isoft}
/// @interface_level = unit
/// @trace_id_ad=AD_LOG_00254
/// @trace_id_dd=DD_LOG_00488
/// @needwork = ad
/// @endcode
class DltCommandReceiver
{
public:
    /// @brief Callback interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00255
    /// @trace_id_dd=DD_LOG_00489
    /// @needwork = ad
    /// @endcode
    class Listener
    {
    public:
        /// @brief Complete log information callback
        /// @param[in]  clientFd Client ID
        /// @param[in]  msg Message structure
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00256
        /// @trace_id_dd=DD_LOG_00490
        /// @needwork = ad
        /// @endcode
        virtual void OnCommandMessage(std::int32_t const &clientFd, DltMessage *msg) = 0;
        /// @brief Destructor, used to free memory
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_00257
        /// @trace_id_dd=DD_LOG_00491
        /// @needwork = ad
        /// @endcode
        virtual ~Listener() noexcept = default;
        /// @brief  Define a default constructor to create an instance of the Listener class. The default constructor does not require any parameters and can initialize the object to a default state.
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_02349
        /// @trace_id_dd=DD_LOG_02748
        /// @needwork = ad
        /// @endcode
        Listener() = default;
        /// @brief  Disable copy construction
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_02302
        /// @trace_id_dd=DD_LOG_02702
        /// @needwork = ad
        /// @endcode
        Listener(Listener const &) = delete;
        /// @brief Disable copy assignment
        /// @return
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_LOG_02303
        /// @trace_id_dd=DD_LOG_02704
        /// @needwork = ad
        /// @endcode
        Listener &operator=(Listener const &) = delete;
        // Add move constructor
        Listener(Listener &&) noexcept = delete;
        // Add move assignment operator
        Listener &operator=(Listener &&) noexcept = delete;
    };

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00258
    /// @trace_id_dd=DD_LOG_00492
    /// @needwork = ad
    /// @endcode
    explicit DltCommandReceiver(DltCommandReceiver::Listener *const lsner);

    /// @brief Destructor, free memory
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00259
    /// @trace_id_dd=DD_LOG_00493
    /// @needwork = ad
    /// @endcode
    ~DltCommandReceiver() noexcept;

    /// @brief Disable copy construction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00361
    /// @trace_id_dd=DD_LOG_01760
    /// @needwork = ad
    /// @endcode
    DltCommandReceiver(DltCommandReceiver const &) = delete;

    /// @brief Disable copy assignment
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00362
    /// @trace_id_dd=DD_LOG_01761
    /// @needwork = ad
    /// @endcode
    DltCommandReceiver &operator=(DltCommandReceiver const &) = delete;

    DltCommandReceiver(DltCommandReceiver &&) noexcept = delete;
    // Add move assignment operator
    DltCommandReceiver &operator=(DltCommandReceiver &&) noexcept = delete;

    /// @brief Initialize DltReceiver
    /// @param[in]  recvFd File descriptor
    /// @param[in]  recvType  Type
    /// @param[in]  bufferSize  Size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00260
    /// @trace_id_dd=DD_LOG_00494
    /// @needwork = ad
    /// @endcode
    void Init(std::int32_t const &recvFd, DltReceiverType const &recvType, std::uint32_t const &bufferSize) noexcept;
    /// @brief Receive data
    /// @return OK or failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00261
    /// @trace_id_dd=DD_LOG_00495
    /// @needwork = ad
    /// @endcode
    bool DataToBuffer();
    /// @brief Parse message
    /// @param[in]  msg Message result
    /// @param[in]  buffer Raw data
    /// @param[in]  length Raw length
    /// @param[in]  resync Whether resynchronization is needed
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00262
    /// @trace_id_dd=DD_LOG_00496
    /// @needwork = ad
    /// @endcode
    std::int32_t MessageRead(DltMessage *const msg,
                             uint8_t const *buffer,
                             std::uint32_t length,
                             std::int32_t const &resync);
    /// @brief Free memory
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00263
    /// @trace_id_dd=DD_LOG_00497
    /// @needwork = ad
    /// @endcode
    void Free() noexcept;
    /// @brief Delete already parsed content of DltReceiver
    /// @param[in]  size
    /// @return  0ok  <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00264
    /// @trace_id_dd=DD_LOG_00498
    /// @needwork = ad
    /// @endcode
    std::int32_t Remove(std::uint32_t const &size) noexcept;
    /// @brief Move to the beginning position
    /// @return 0ok  <0failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00265
    /// @trace_id_dd=DD_LOG_00499
    /// @needwork = ad
    /// @endcode
    std::int32_t MoveToBegin() noexcept;

private:
    /// @brief  bytes received in last receive call
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00254
    /// @trace_id_dd=DD_LOG_00500
    /// @needwork = dda
    /// @endcode
    std::uint32_t lastBytesRcvd_{0U};
    /// @brief received bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00254
    /// @trace_id_dd=DD_LOG_00501
    /// @needwork = dda
    /// @endcode
    std::uint32_t bytesRcvd_{0U};
    /// @brief  total number of received bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00254
    /// @trace_id_dd=DD_LOG_00502
    /// @needwork = dda
    /// @endcode
    std::uint32_t totalBytesRcvd_{0U};
    /// @brief pointer to receiver buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00254
    /// @trace_id_dd=DD_LOG_00503
    /// @needwork = dda
    /// @endcode
    std::uint8_t *mBuffer_{nullptr};
    /// @brief pointer to position within receiver buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00254
    /// @trace_id_dd=DD_LOG_00504
    /// @needwork = dda
    /// @endcode
    std::uint8_t *buf_{nullptr};
    /// @brief pointer to the buffer with partial messages if any
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00254
    /// @trace_id_dd=DD_LOG_00505
    /// @needwork = dda
    /// @endcode
    std::uint8_t *backupBuffer_{nullptr};
    /// @brief connection handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00254
    /// @trace_id_dd=DD_LOG_00506
    /// @needwork = dda
    /// @endcode
    std::int32_t fd_{0};
    /// @brief type of connection handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00254
    /// @trace_id_dd=DD_LOG_00507
    /// @needwork = dda
    /// @endcode
    DltReceiverType type_{DltReceiverType::kDlt_Receive_Socket};
    /// @brief size of receiver buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00254
    /// @trace_id_dd=DD_LOG_00508
    /// @needwork = dda
    /// @endcode
    std::uint32_t mBufferSize_{0U};

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00254
    /// @trace_id_dd=DD_LOG_00509
    /// @needwork = dda
    /// @endcode
    DltMessage controlMsg_{};

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00254
    /// @trace_id_dd=DD_LOG_00510
    /// @needwork = dda
    /// @endcode
    Listener *mListener_{nullptr};
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif