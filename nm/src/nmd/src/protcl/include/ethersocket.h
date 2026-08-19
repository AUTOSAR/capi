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
/// @file       ethersocket.h
/// @brief      Socket management class
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/protcl
/// @interface_level=unit
/// @trace_id_sr=SRS_NM_00001,SRS_NM_00002
/// @unit_name=EtherSocket
/// @unit_description=Socket management class
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_ETHERSOCKET_H_
#define _ARA_NM_ETHERSOCKET_H_

#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <nai/io/nai_event.h>
#include <nai/io/nai_io.h>
#include <nai/runtime/nai_errno.h>

#include "common/common.h"
#include "config/include/configure.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief Callback function type for handling received NM messages
/// @param pMsgBuffer -  NM message pointer
/// @param msgLenth -  Message length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100002
/// @trace_id_dd=DD_NM_00972
/// @needwork = ad
/// @endcode
using MsgProcesHandler = std::function< void(std::uint8_t const *const pMsgBuffer, std::uint32_t const msgLenth) >;

/// @brief socket management of ethernet communicator
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100100
/// @trace_id_dd=DD_NM_00845
/// @needwork = ad
/// @endcode
class EtherSocket final
{
public:
    /// @brief constructor of EtherSocket.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00707
    /// @needwork = dda
    /// @endcode
    EtherSocket() = default;

    /// @brief copy constructor is forbidden.
    ///
    /// @param other class instance.
    /// @code{.isoft}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00708
    /// @needwork = dda
    /// @endcode
    EtherSocket(EtherSocket const &other) = delete;

    /// @brief copy operator is forbidden.
    ///
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @trace_id_sws=
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00709
    /// @needwork = dda
    /// @endcode
    EtherSocket &operator=(EtherSocket const &other) = delete;

    /// @brief move constructor is default
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00710
    /// @needwork = dda
    /// @endcode
    EtherSocket(EtherSocket &&other) = default;

    /// @brief move operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00711
    /// @needwork = dda
    /// @endcode
    EtherSocket &operator=(EtherSocket &&other) = default;

    /// @brief destructor of EtherSocket.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00712
    /// @needwork = dda
    /// @endcode
    ~EtherSocket() noexcept;

    /// @brief create multicast socket.
    /// @param localIpAddress   unicast ip address of ethernet communicator.
    /// @param mulcastIpAddress multicast ip address of udp cluster.
    /// @param mulcastPort      multicast port of udp cluster.
    /// @param pduLength        pdu length.
    /// @returns                0 ok.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00713
    /// @needwork = dda
    /// @endcode
    std::int32_t OpenSocket(ara::core::String const &localIpAddress,
                            ara::core::String const &mulcastIpAddress,
                            std::uint32_t const mulcastPort,
                            std::uint32_t const pduLength) noexcept;

    /// @brief socket message reciver call back.
    /// @param stream           stream handle pointer.
    /// @param events           socket events specific.
    /// @returns                0 ok.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00714
    /// @needwork = dda
    /// @endcode
    static std::int32_t RecvNmMessageHandle(nai_stream_t *const stream, std::int32_t const events) noexcept;

    /// @brief send protocol message.
    /// @param pktBuffer           message buffer.
    /// @param bufferLenth         message length.
    /// @returns                On success, return the number of bytes sent.  On
    /// error, -1 is returned.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00715
    /// @needwork = dda
    /// @endcode
    std::int32_t SendNmMessage(std::uint8_t const pktBuffer[], std::size_t const bufferLenth) const noexcept;

    /// @brief add message process function.
    /// @param msgCallBack callback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00716
    /// @needwork = dda
    /// @endcode
    void RegistMsgProceHandler(MsgProcesHandler const &msgCallBack) noexcept { msgCallBack_ = msgCallBack; };

private:
    /// @brief process of receiving protocol message.
    /// @returns                kNmOperOK ok.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00717
    /// @needwork = dda
    /// @endcode
    NmOperCode _recvNmMessage() noexcept;

private:
    /// @brief socketFd_
    /// socket fd of the multicast socket
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00718
    /// @needwork = dda
    /// @endcode
    nai_fd_t socketFd_{-1};

    /// @brief netStream_
    /// net stream of the multicast socket
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00719
    /// @needwork = dda
    /// @endcode
    nai_stream_t netStream_{};

    /// @brief errCode_
    /// check the open of stream
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00720
    /// @needwork = dda
    /// @endcode
    std::int32_t errCode_{-1};

    /// @brief localIpAddress_
    /// ethernet communicator IPV4 unicast address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00721
    /// @needwork = dda
    /// @endcode
    ara::core::String localIpAddress_{};

    /// @brief mulcastIpAddress_
    /// IPV4 multicast address of udp cluster
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00722
    /// @needwork = dda
    /// @endcode
    ara::core::String mulcastIpAddress_{};

    /// @brief mulcastPort_
    /// udp port of udp cluster
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00723
    /// @needwork = dda
    /// @endcode
    std::uint32_t mulcastPort_{0};

    /// @brief pduLength_
    /// pdu message length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00724
    /// @needwork = dda
    /// @endcode
    std::uint32_t pduLength_{0};

    /// @brief msgCallBack_
    /// message process callback
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00725
    /// @needwork = dda
    /// @endcode
    MsgProcesHandler msgCallBack_{};

    /// @brief pBuffer_
    /// socket buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00726
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< std::uint8_t[] > pBuffer_{nullptr};

    /// @brief lastBufferLen_
    /// last socket buffer, if it is less than pduLength_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00727
    /// @needwork = dda
    /// @endcode
    std::uint32_t lastBufferLen_{0};
};

}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  //_ARA_NM_ETHERSOCKET_H_