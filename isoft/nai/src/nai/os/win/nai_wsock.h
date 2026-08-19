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
/// @file       nai_wsock.h
/// @brief      
/// @details
/// @date       2020-12-06
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _WSOCK_H_NAI
#define _WSOCK_H_NAI

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined(_WIN32)

    #include <mswsock.h>
    #include <winsock2.h>

    #include "nai/os/nai_sendfile.h"
    #include "nai_windows.h"

    #if 0
typedef BOOL (WINAPI *LPFN_ACCEPTEX)(
    SOCKET sListenSocket,
    SOCKET sAcceptSocket,
    PVOID lpOutputBuffer,
    DWORD dwReceiveDataLength,
    DWORD dwLocalAddressLength,
    DWORD dwRemoteAddressLength,
    LPDWORD lpdwBytesReceived,
    LPOVERLAPPED lpOverlapped
);
typedef BOOL (WINAPI *LPFN_TRANSMITFILE)(
    SOCKET hSocket,
    HANDLE hFile,
    DWORD nNumberOfBytesToWrite,
    DWORD nNumberOfBytesPerSend,
    LPOVERLAPPED lpOverlapped,
    LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
    DWORD dwReserved
);
    #endif

    typedef struct nai_wsockops_s
    {
        nai_int_t inited;

        LPFN_ACCEPTEX AcceptEx;
        LPFN_CONNECTEX ConnectEx;
        LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockaddrs;
        LPFN_TRANSMITFILE TransmitFile;
        LPFN_TRANSMITPACKETS TransmitPackets;
    } nai_wsockops_t;

    extern nai_wsockops_t nai_wsockops;

    #define NAI_SENDFILE_FLAGS TF_USE_KERNEL_APC

    /**
 * convert the file array to the transmit packets
 * @param   a       pointer to the file array
 * @param   e       pointer to the transmit packets
 * @param   ecnt    the count of the transmit packets
 * @param   psize   pointer to the size to get converted bytes
 * @retval  >=0     the count of converted packets is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_filearray_to_packets(nai_filearray_t* a, TRANSMIT_PACKETS_ELEMENT* e, nai_int_t ecnt, size_t* psize);

    /**
 * convert the file chunk to the transmit packets
 * @param   a       pointer to the file chunk
 * @param   e       pointer to the transmit packets
 * @param   ecnt    the count of the transmit packets
 * @param   psize   pointer to the size to get converted bytes
 * @retval  >=0     the count of converted packets is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_filechunk_to_packets(nai_filechunk_t* c, TRANSMIT_PACKETS_ELEMENT* e, nai_int_t ecnt, size_t* psize);

#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
