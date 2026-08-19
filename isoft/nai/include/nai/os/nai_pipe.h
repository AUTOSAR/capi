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
/// @file       nai_pipe.h
/// @brief      anonymous pipe interface
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation does not provide named pipes. the main 
 *          reason is that named pipes are not friendly to use. 
 *          for example, it is difficult to build communication services 
 *          with named pipes. At the same time, the open two parties cannot 
 *          guarantee that one is the server and the other is the client. 
 *          on the other hand, the connection process of named pipes 
 *          on win32 is difficult to adapt to the event loop. in contrast, 
 *          sockets can completely cover the functions of named pipes, 
 *          such as unix socket.
 *
 * @details the code example is as follows:
 *
 * @par     create a pair of pipe
 * @code
 *          nai_int_t r;
 *          nai_fd_t fds[2];
 *
 *          r = nai_pipe(fds, 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          // fds[0] is input endpoint
 *          // fds[1] is output endpoint
 * @endcode
 */

#ifndef _PIPE_H_NAI
#define _PIPE_H_NAI

#pragma once

#include "nai_file.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
 * create a pair of pipe
 * @param   fds     the array is used to return the file descriptors
 * @param   flags   the flags of open file, see @ref NAI_FOPEN
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the flags can be one or more of the following values:
 *          #NAI_O_NOCLOEXEC, #NAI_O_DIRECT, #NAI_O_NONBLOCK
 */
    NAI_EXTERN
    nai_int_t nai_pipe(nai_fd_t fds[2], nai_int_t flags);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
