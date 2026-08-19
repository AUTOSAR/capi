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
/// @file       nai_io.h
/// @brief      unified a/synchronous io
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details the iobase supports blocking, non-blocking and asynchronous 
 *          backends, and unifies these backends into a reactive model. it also
 *          supports stream/datagram and connection-oriented/connectionless 
 *          io objects.
 *
 * @details the iobase can be instantiated into listening sockets, 
 *          connection-oriented streams/datagrams and connectionless datagrams, 
 *          providing a unified interface to facilitate logical reuse.
 *
 * @details the listening socket is used to monitor and accept the connectiion 
 *          requests from connection-oriented socket. similar to 
 *          the use steps of the system interface, start listening, 
 *          wait for the event, accept the connection, and close listening.
 *
 * @details the stream can be a connection-oriented socket or pipe or file 
 *          and other communication devices. it can also work in 
 *          a message-oriented way, such as seqpacket sockets, message queues.
 *
 * @details the dgram is mainly for connectionless sockets. for a connected 
 *          dgram, its behavior is similar to that of a message-oriented 
 *          connection.
 *
 * @details the code example is as follows:
 *
 * @par     implement a complete event callback
 * @code
 *          nai_int_t my_event_cb(nai_iobase_t* n, nai_int_t events)
 *          {
 *              nai_int_t r;
 *              nai_int_t error;
 *              nai_int_t notify_id;
 *
 *              if (events & NAI_EV_ERROR) {
 *                  error = nai_ev_error_code(events);
 *                  if (error) {
 *                      // Error occurred
 *                  } else {
 *                      // Error event: POLLERR, EPOLLERR
 *                  };
 *
 *                  // May include remaining read/write events, need to continue processing
 *                  events &= ~NAI_EV_ERROR;
 *              };
 *
 *              switch (events & NAI_EV_MASK) { // for demo only
 *              case NAI_EV_READ:
 *              case NAI_EV_WRITE:
 *              case NAI_EV_READ|NAI_EV_WRITE:
 *                  // Read/write events
 *                  break;
 *
 *              case NAI_EV_EXCEPT:
 *                  // Urgent events
 *                  break;
 *
 *              case NAI_EV_READ|NAI_EV_TIMEOUT:
 *                  // Read timeout
 *                  break;
 *
 *              case NAI_EV_WRITE|NAI_EV_TIMEOUT:
 *                  // Write timeout
 *                  break;
 *
 *              case NAI_EV_TIMEOUT:
 *                  // User-defined timer
 *                  break;
 *
 *              caee NAI_EV_NOTIFY:
 *                  // User-defined event
 *                  notify_id = nai_ev_notify_code(events);
 *                  switch (notify_id) {
 *                  case 0:
 *                  case 1:
 *                  case 2:
 *                      break;
 *                  };
 *                  break;
 *              };
 *
 *              r = 0;
 *
 *              return r;
 *          };
 * @endcode
 *
 * @par     preparations for opening the iobase
 * @code
 *          nai_iobase_t s;
 *
 *          nai_iobase_init(&s);
 *          nai_iobase_set_cb(&s, my_event_cb);
 *          nai_iobase_set_opt(&s, NAI_IO_SENDTIMEO, 30*1000); // optional
 *          nai_iobase_set_opt(&s, NAI_IO_RECVTIMEO, 30*1000); // optional
 *          nai_iobase_set_wait(&s, NAI_IO_WRITE); // optional
 *
 * @endcode
 *
 * @par     open with an exist file descriptor
 * @code
 *          nai_int_t r;
 *          nai_fd_t fd;
 *          nai_evloop_t* l;
 *          nai_iobase_t* s;
 *
 *          nai_stream_set_fd(s, fd, NAI_FD_SOCK);
 *          nai_stream_set_fdown(s, 1);
 *
 *          r = nai_stream_open(s, l);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     start connect to the server
 * @code
 *          nai_int_t r;
 *          nai_evloop_t* l;
 *          nai_iobase_t* s;
 *          nai_sockname_t* name;
 *
 *          r = nai_stream_connect(s, l, name->addr, name->len);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     start a listening server
 * @code
 *          nai_int_t r;
 *          nai_evloop_t* l;
 *          nai_iobase_t* s;
 *          nai_sockname_t* name;
 *
 *          r = nai_server_bind(s, l, name->addr, name->len);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     start a dgram server
 * @code
 *          nai_int_t r;
 *          nai_evloop_t* l;
 *          nai_iobase_t* s;
 *          nai_sockname_t* name;
 *
 *          r = nai_dgram_bind(s, l, name->addr, name->len);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     read and write data
 * @code
 *
 *          intptr_t r;
 *          nai_int_t ec;
 *          nai_int_t bytes;
 *          char* buf;
 *          size_t buflen;
 *
 *
 *          // other versions: nai_stream_readv|readq
 *          r = nai_stream_read(s, buf, buflen);
 *          if (r < 0) {
 *              ec = nai_errno;
 *              if (ec == NAI_AGAIN || ec == NAI_INPROCESS) {
 *                  // Operation blocked or incomplete
 *                  // When the operation is incomplete, the passed buffer must not be released,
 *                  // until the next read succeeds
 *                  goto _wait;
 *              };
 *
 *              // Other errors
 *              goto _fail;
 *          };
 *
 *          // Data processing
 *          bytes = r;
 *          ...
 *
 *
 *          // other versions: nai_stream_writev|writeq
 *          r = nai_stream_write(s, buf, buflen);
 *          if (r < 0) {
 *              ec = nai_errno;
 *              if (ec == NAI_AGAIN || ec == NAI_INPROCESS) {
 *                  // Operation blocked or incomplete
 *                  // When the operation is incomplete, the passed buffer must not be released,
 *                  // until the next write succeeds
 *                  r = 0;
 *                  goto _wait;
 *              };
 *
 *              // Other errors
 *              goto _fail;
 *          };
 *
 *          // Result processing
 *          bytes = r;
 *          ...
 *
 * @endcode
 */

#ifndef _IO_H_NAI
#define _IO_H_NAI

#pragma once

#include "nai/io/nai_event.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * the event means the iobase can be safely closed now
 */
#define NAI_EV_FINALIZE NAI_EV_SPECIFIC

/** 
 * @anchor  NAI_IO_MODE
 * @name    NAI_IO_MODE         the modes of iobase 
 * @{
 */
#define NAI_IO_READ      0x01 /**< read mode */
#define NAI_IO_WRITE     0x02 /**< write mode */
#define NAI_IO_READWRITE 0x03 /**< read/write mode */
/** @} */

/**
 * @anchor  NAI_IO_OPTION
 * @name    NAI_IO_OPTION       the options of iobase
 * options should be used on a iobase which is seted with fd, 
 * except mode, blocking, recvtimeo, sendtimeo, sendfile, loadfile, reuseaddr
 * @{
 */
#define NAI_IO_MODE           1  /**< int, switch event of iobase */
#define NAI_IO_BLOCKING       2  /**< bool, switch blocking mode */
#define NAI_IO_RECVTIMEO      3  /**< int, recv timeout, -1 to disable */
#define NAI_IO_SENDTIMEO      4  /**< int, send timeout, -1 to disable */
#define NAI_IO_RECVBUF        5  /**< int, recv buffer size */
#define NAI_IO_SENDBUF        6  /**< int, send buffer size */
#define NAI_IO_LINGER         7  /**< int, linger timeout, -1 to disable */
#define NAI_IO_NOPUSH         8  /**< bool, enable nopush option */
#define NAI_IO_NODELAY        9  /**< bool, enable nodelay option */
#define NAI_IO_REUSEADDR      10 /**< bool, enable address reuse */
#define NAI_IO_REUSEPORT      11 /**< bool, enable port reuse */
#define NAI_IO_SENDFILE       12 /**< bool, enable sendfile */
#define NAI_IO_LOADFILE       13 /**< bool, enable loadfile */
#define NAI_IO_PENDING        14 /**< int, pending opeartions mask */
#define NAI_IO_CANCEL         15 /**< int, to cancel opeartions mask */
#define NAI_IO_MULTICAST_JOIN 16 /**< nai_sockaddr_t*[2], join multi-cast */
#define NAI_IO_MULTICAST_DROP 17 /**< nai_sockaddr_t*[2], drop multi-cast */
#define NAI_IO_MULTICAST_IF   18 /**< nai_sockaddr_t*, send interface */
#define NAI_IO_MULTICAST_LOOP 19 /**< bool, enable/disable multicast loop */
#define NAI_IO_MULTICAST_TTL  20 /**< int, multicast ttl */
#define NAI_IO_FEAT_SENDFILE  30 /**< bool, is natively supported */
#define NAI_IO_FEAT_VECTORIO  31 /**< bool, is natively supported */
#define NAI_IO_FEAT_MODEL                                                                                              \
    32 /**< int, io model type, 
                                         see @ref NAI_EV_FEAT */
/** @} */

/**
 * @anchor  NAI_IO_TYPE
 * @name    NAI_IO_TYPE         the types of iobase
 * @{
 */
#define NAI_IO_TYPE_SERVER 1 /**< a listen socket */
#define NAI_IO_TYPE_STREAM 2 /**< a stream, ie file, pipe, socket */
#define NAI_IO_TYPE_DGRAM  3 /**< a dgram or seqpacket socket */
/** @} */

/**
 * @anchor  NAI_IO_SUBTYPE
 * @name    NAI_IO_SUBTYPE      the subtypes of iobase
 * @{
 */
#define NAI_IO_SUBT_DEFAULT   0 /**< the default type of file descriptor */
#define NAI_IO_SUBT_SEQPACKET 1 /**< connection-oriented message stream */
    /** @} */

    //////////////////////////////////////////////////////////////////////////////
    // iobase

#ifndef _NAI_TYPEDEF_SOCKADDR_T
    #define _NAI_TYPEDEF_SOCKADDR_T
    typedef struct sockaddr nai_sockaddr_t;
#endif
#ifndef _NAI_TYPEDEF_BUFVEC_T
    #define _NAI_TYPEDEF_BUFVEC_T
    typedef struct nai_bufvec_s nai_bufvec_t;
#endif
#ifndef _NAI_TYPEDEF_BUFARRAY_T
    #define _NAI_TYPEDEF_BUFARRAY_T
    typedef struct nai_bufarray_s nai_bufarray_t;
#endif
#ifndef _NAI_TYPEDEF_BUFLIST_T
    #define _NAI_TYPEDEF_BUFLIST_T
    typedef struct nai_buflist_s nai_buflist_t;
#endif
#ifndef _NAI_TYPEDEF_IOBASE_T
    #define _NAI_TYPEDEF_IOBASE_T
    typedef struct nai_iobase_s nai_iobase_t;
#endif
#ifndef _NAI_TYPEDEF_SERVER_T
    #define _NAI_TYPEDEF_SERVER_T
    typedef struct nai_iobase_s nai_server_t;
#endif
#ifndef _NAI_TYPEDEF_IOBASE_OPS_T
    #define _NAI_TYPEDEF_IOBASE_OPS_T
    typedef struct nai_iobase_ops_s nai_iobase_ops_t;
#endif
#ifndef _NAI_TYPEDEF_IOBASE_CB_F
    #define _NAI_TYPEDEF_IOBASE_CB_F
    typedef nai_int_t (*nai_iobase_cb_f)(nai_iobase_t* s, nai_int_t events);
#endif

    struct nai_iobase_s
    {
        nai_evnode_t ev;    /**< the event node */
        nai_iobase_cb_f cb; /**< the user callback */

        /* intenral status, don't modify */
        struct
        {
            nai_iobase_ops_t* ops; /**< pointer to the operations */
            void* ctx;             /**< pointer to the internal data */
            uint32_t timer[3];     /**< read and write and custom timer */
            uint32_t timeout[2];   /**< read and write timeout */
            union
            {
                struct
                {
                    uint32_t mode : 2;       /**< open mode */
                    uint32_t type : 2;       /**< type: listen, stream, dgram */
                    uint32_t subtype : 1;    /**< subtype: default, seqpacket */
                    uint32_t family : 2;     /**< 0:uninit 1:in4 2:in6 3:unknown */
                    uint32_t fdown : 1;      /**< owner fd */
                    uint32_t wants : 2;      /**< expected events, use for ssl */
                    uint32_t blocking : 1;   /**< whether block-mode is seted */
                    uint32_t blockset : 1;   /**< fd's block-mode had been set */
                    uint32_t blocked : 2;    /**< read or write are blocked */
                    uint32_t timeosup : 1;   /**< os support recv/send timeout */
                    uint32_t timeoset : 2;   /**< timeout had been set */
                    uint32_t timeochg : 2;   /**< timeout changed but not set fd */
                    uint32_t timerevt : 1;   /**< timer event had been set */
                    uint32_t timerset : 3;   /**< timer started flags */
                    uint32_t sendfile : 1;   /**< enable sendfile */
                    uint32_t loadfile : 1;   /**< enable loadfile */
                    uint32_t reuseaddr : 2;  /**< enable address reuse */
                    uint32_t reuseport : 2;  /**< enable port reuse */
                    uint32_t polling : 1;    /**< preferred a polling model */
                    uint32_t agent : 1;      /**< is agent context */
                    uint32_t connection : 1; /**< is a connection */
                };
                uint32_t flags;
            };
        } st;

        /* the data for subclass, don't modify */
        union
        {
            nai_off64_t offset; /**< the file offset */
            uint32_t backlog;   /**< the listen backlog */
            intptr_t agent;     /**< the context of connect agent */
        };
    };

/**
 * @name    nai_iobase_defines  the functions of iobase
 * @{
 */

/**
 * test it is a file
 * @param   s       pointer to the iobase
 * @return  if it is a file, return 1, otherwise return 0
 */
#define nai_iobase_is_file(s) (nai_evnode_get_type(&(s)->ev) == NAI_FD_TYPE_FILE)

/**
 * test it is a pipe
 * @param   s       pointer to the iobase
 * @return  if it is a pipe, return 1, otherwise return 0
 */
#define nai_iobase_is_pipe(s) (nai_evnode_get_type(&(s)->ev) == NAI_FD_TYPE_PIPE)

/**
 * test it is a socket
 * @param   s       pointer to the iobase
 * @return  if it is a socket, return 1, otherwise return 0
 */
#define nai_iobase_is_sock(s) (nai_evnode_get_type(&(s)->ev) == NAI_FD_TYPE_SOCK)

/**
 * test it is a connection-oriented message stream
 * @param   s       pointer to the iobase
 * @return  if it is a message stream, return 1, otherwise return 0
 */
#define nai_iobase_is_message(s) ((s)->st.subtype == NAI_IO_SUBT_SEQPACKET)

/**
 * test it is a seekable fd
 * @param   s       pointer to the iobase
 * @return  if it is a seekable fd, return 1, otherwise return 0
 */
#define nai_iobase_is_seekable(s) (nai_evnode_get_type(&(s)->ev) & 1)

/**
 * test the iobase is readable
 * @param   s       pointer to the iobase
 * @return  if it is readable, return 1, otherwise return 0
 */
#define nai_iobase_is_readable(s) (nai_iobase_get_event(s) & NAI_EV_READ)

/**
 * test the iobase is writable
 * @param   s       pointer to the iobase
 * @return  if it is writable, return 1, otherwise return 0
 */
#define nai_iobase_is_writable(s) (nai_iobase_get_event(s) & NAI_EV_WRITE)

/**
 * test the iobase is in blocking mode
 * @param   s       pointer to the iobase
 * @retval  0       the iobase in non-blocking mode
 * @retval  1       the iobase in blocking mode
 */
#define nai_iobase_is_blocking(s) ((nai_int_t)(s)->st.blocking)

/**
 * test whether the specific opeartions is pending
 * @param   s       pointer to the iobase
 * @param   w       specifies the opeartions to test, see @ref NAI_IO_MODE
 * @retval  1       the opeartions is pending
 * @retval  0       the opeartions is done
 */
#define nai_iobase_is_pending(s, w) (!!(nai_iobase_get_pending(s) & (w)))

/**
 * test the iobase is opened
 * @param   s       pointer to the iobase
 * @return  if it is opened, return 1, otherwise return 0
 */
#define nai_iobase_is_opened(s) ((s)->st.ops != 0)

/**
 * test the caller thread is the event loop thread
 * @param   s       pointer to the iobase
 * @return  if in the event loop thread, return 1, otherwise return 0
 */
#define nai_iobase_in_dispatch(s) nai_evnode_in_dispatch(&(s)->ev)

/**
 * get the event handle of the iobase
 * @param   s       pointer to the iobase
 * @return  the event handle, see #nai_iobase_cb_f
 */
#define nai_iobase_get_cb(s) ((nai_iobase_cb_f)(s)->cb)

/**
 * get the event loop associated with the iobase
 * @param   s       pointer to the iobase
 * @return  the pointer of the event loop
 */
#define nai_iobase_get_loop(s) nai_evnode_get_loop(&(s)->ev)

/**
 * get the type of file descriptor
 * @param   s       pointer to the iobase
 * @return  the type of file descriptor, see @ref NAI_FD_TYPE
 */
#define nai_iobase_get_type(s) nai_evnode_get_type(&(s)->ev)

/**
 * get the file descriptor associated with the iobase
 * @param   s       pointer to the iobase
 * @return  the file descriptor
 */
#define nai_iobase_get_fd(s) nai_evnode_get_fd(&(s)->ev)

/**
 * get the ownerships of file descriptor
 * @param   s       pointer to the iobase
 * @retval  0       will not close file descriptor on close iobase
 * @retval  1       will auto close file descriptor on close iobase
 */
#define nai_iobase_get_fdown(s) ((nai_int_t)(s)->st.fdown)

/**
 * get the mode of the iobase
 * @param   s       pointer to the iobase
 * @return  the value of mode, see @ref NAI_IO_MODE
 */
#define nai_iobase_get_mode(s) ((nai_int_t)(s)->st.mode)

/**
 * get the available events of read and write
 * @param   s       pointer to the iobase
 * @return  the value of events, see @ref NAI_EV_EVENT
 */
#define nai_iobase_get_event(s) ((s)->st.mode & ~(s)->st.blocked)

    /** @} */

    /**
 * initial the iobase
 * @param   s       pointer to the iobase to initialize
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_iobase_init(nai_iobase_t* s);

    /**
 * set the event handle of the iobase
 * @param   s       pointer to the iobase
 * @param   cb      the event handle of iobase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_iobase_set_cb(nai_iobase_t* s, nai_iobase_cb_f cb);

    /**
 * binding a file descriptor to the iobase
 * @param   s       pointer to the iobase
 * @param   fd      the file descriptor which to bind with iobase
 * @param   type    the type of file descriptor
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function should be called before open the iobase
 */
    NAI_EXTERN
    nai_int_t nai_iobase_set_fd(nai_iobase_t* s, nai_fd_t fd, nai_int_t type);

    /**
 * set the ownerships of the file descriptor
 * @param   s       pointer to the iobase
 * @param   own     whether auto close the file descriptor when iobase close
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_iobase_set_fdown(nai_iobase_t* s, nai_int_t own);

    /**
 * set the subtype of the file descriptor
 * @param   s       pointer to the iobase
 * @param   type    the value of subtype
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function should be called before open the iobase
 */
    NAI_EXTERN
    nai_int_t nai_iobase_set_subtype(nai_iobase_t* s, nai_int_t type);

    /**
 * set the mode of the iobase
 * @param   s       pointer to the iobase
 * @param   mode    the mode of the iobase, see @ref NAI_IO_MODE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function can be called before open the iobase
 * @note    if iobase opened with the event loop, 
 *          this function must be called in the event loop thread
 */
    NAI_EXTERN
    nai_int_t nai_iobase_set_mode(nai_iobase_t* s, nai_int_t mode);

    /**
 * set read/write events to wait and start timer on opening
 * @param   s       pointer to the iobase
 * @param   events  the blocked events, see @ref NAI_EV_EVENT
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function should be called before open the iobase
 * @note    the state of iobase events is unblocked by default, and some 
 *          the event loops will not poll unblocked events, such as level 
 *          triggers: select, poll. after connecting or accepting is 
 *          successful, if you need to read or write events, you should call 
 *          nai_iobase_set_wait to set the waiting event.
 * @note    when the event to be waited for is set, and the corresponding 
 *          timeout period is also set, iobase will automatically start timing 
 *          instead of being triggered by read/write operations.
 */
    NAI_EXTERN
    nai_int_t nai_iobase_set_wait(nai_iobase_t* s, nai_int_t events);

    /**
 * set polling as the preferred model
 * @param   s       pointer to the iobase
 * @param   pref    indicates whether polling is the preferred model
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function should be called before open the iobase
 */
    NAI_EXTERN
    nai_int_t nai_iobase_set_polling(nai_iobase_t* s, nai_int_t pref);

    /**
 * switch the blocking mode of iobase
 * @param   s       pointer to the iobase
 * @param   on      indicates whether enable blocking mode
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function can be called before open the iobase
 * @note    if iobase opened with the event loop, 
 *          this function must be called in the event loop thread
 */
    NAI_EXTERN
    nai_int_t nai_iobase_set_blocking(nai_iobase_t* s, nai_int_t on);

    /**
 * enable polling urgent event
 * @param   s       pointer to the iobase
 * @param   on      indicates whether enable polling urgent event
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function can be called before open the iobase
 * @note    if iobase opened with the event loop, 
 *          this function must be called in the event loop thread
 */
    NAI_EXTERN
    nai_int_t nai_iobase_set_except(nai_iobase_t* s, nai_int_t on);

    /**
 * set a user timer
 * @param   s       pointer to the iobase
 * @param   op      the operation of timeout, see @ref NAI_TIMEOP
 * @param   msec    the value of timeout, a positive number, -1 to disable
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is not thread-safe
 * @note    this function can be called before open the iobase
 * @note    if iobase opened with the event loop, 
 *          this function must be called in the event loop thread
 */
    NAI_EXTERN
    nai_int_t nai_iobase_set_timeout(nai_iobase_t* s, nai_int_t op, int32_t msec);

    /**
 * set a option of iobase
 * @param   s       pointer to the iobase
 * @param   opt     the name of the option, see @ref NAI_IO_OPTION
 * @param   value   the value of the option, see @ref NAI_IO_OPTION
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is not thread-safe
 * @note    if iobase opened with the event loop, 
 *          this function must be called in the event loop thread
 */
    NAI_EXTERN
    nai_int_t nai_iobase_set_opt(nai_iobase_t* s, nai_int_t opt, intptr_t value);

    /**
 * get a option of iobase
 * @param   s       pointer to the iobase
 * @param   opt     the name of the option, see @ref NAI_IO_OPTION
 * @param   value   pointer to the value of the option, see @ref NAI_IO_OPTION
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is not thread-safe
 * @note    if iobase opened with the event loop, 
 *          this function must be called in the event loop thread
 */
    NAI_EXTERN
    nai_int_t nai_iobase_get_opt(nai_iobase_t* s, nai_int_t opt, intptr_t* value);

    /**
 * get the io opeartions which is in pending
 * @param   s       pointer to the iobase
 * @return  the mask of the io opeartions which is in pending, 
 *          see @ref NAI_IO_MODE
 */
    NAI_EXTERN
    nai_int_t nai_iobase_get_pending(nai_iobase_t* s);

    /**
 * inherit property from the server for a stream or seqpacket socket
 * @param   s       pointer to the iobase which not opened
 * @param   l       pointer to the server
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_iobase_inherit(nai_iobase_t* s, nai_server_t* l);

    /**
 * post a user signal to the iobase
 * @param   s       pointer to the iobase
 * @param   sigid   the id of the user signal, the value range is 0-3
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is thread-safe, can be called in any thread.
 * @note    this function can be called before open the iobase
 * @note    the user callback will be called with event #NAI_EV_NOTIFY, 
 *          use #nai_ev_notify_code to extract 'sigid'
 * @note    repeatedly sending the same signal will not generate additional 
 *          events until the signal is processed
 */
    NAI_EXTERN
    nai_int_t nai_iobase_post(nai_iobase_t* s, nai_int_t sigid);

    /**
 * bind an user signal to the mail slot to receive
 * @param   s       pointer to the iobase
 * @param   slot    the id of the mail slot to receive
 * @param   sigid   the id of the signal to bind, -1 to unbind
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function must be called after open the iobase
 * @note    the callback will be called with event #NAI_EV_NOTIFY, 
 *          use #nai_ev_notify_code to extract 'sigid'
 */
    NAI_EXTERN
    nai_int_t nai_iobase_join(nai_iobase_t* s, nai_int_t slot, nai_int_t sigid);

    /**
 * put the iobase in other thread back to the event loop thread
 * @param   s       pointer to the iobase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is used for a block iobase in other thread.
 * @note    this function will auto set iobase to non-block mode
 */
    NAI_EXTERN
    nai_int_t nai_iobase_return_loop(nai_iobase_t* s);

    /**
 * close the iobase
 * @param   s       pointer to the iobase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if iobase opened with the event loop, 
 *          this function must be called in the event loop thread
 * @note    if read or write opeartions is in progress, then the parameters 
 *          such as memory and file descriptors are still in use, 
 *          which may cause unexpected problems. 
 *          use #nai_iobase_finalize to close the iobase safely.
 */
    NAI_EXTERN
    nai_int_t nai_iobase_close(nai_iobase_t* s);

    /**
 * finalize the iobase
 * @param   s       pointer to the iobase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if iobase opened with the event loop, 
 *          this function must be called in the event loop thread
 * @note    if read or write opeartions is in progress, then the opeartions 
 *          will be canceled and errno is set to NAI_EINPROCESS, 
 *          the user callback will receive the event #NAI_EV_FINALIZE later.
 * @note    if read or write opeartions is completed, then 
 *          the iobase will be closed.
 */
    NAI_EXTERN
    nai_int_t nai_iobase_finalize(nai_iobase_t* s);

    /**
 * shut down send and receive operations
 * @param   s       pointer to the iobase
 * @param   how     specifies the type of shutdown, see #NAI_SOCK_RW
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_iobase_shutdown(nai_iobase_t* s, nai_int_t how);

    /**
 * read data from the iobase
 * @param   s       pointer to the iobase
 * @param   buf     pointer to the buffer to receive data
 * @param   len     the length of buffer
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to NAI_EAGAIN or NAI_EINPROGRESS.
 * @note    if it fails and errno is set to NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing or using the memory on the stack will cause 
 *          the memory area to be destroyed.
 */
    NAI_EXTERN
    intptr_t nai_iobase_read(nai_iobase_t* s, void* buf, size_t len);

    /**
 * read data into multiple buffers
 * @param   s       pointer to the iobase
 * @param   v       pointer to an array of the buffer vector
 * @param   count   the count of buffer vector
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to NAI_EAGAIN or NAI_EINPROGRESS.
 * @note    if it fails and errno is set to NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing or using the memory on the stack will cause 
 *          the memory area to be destroyed.
 * @note    the memory of the vector itself does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_iobase_readv(nai_iobase_t* s, nai_bufvec_t* v, nai_int_t count);

    /**
 * write data to the iobase
 * @param   s       pointer to the iobase
 * @param   buf     pointer to the buffer to write
 * @param   len     the length of buffer
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to NAI_EAGAIN or NAI_EINPROGRESS.
 * @note    if it fails and errno is set to NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing or using the memory on the stack will cause 
 *          incorrect data to be sent.
 */
    NAI_EXTERN
    intptr_t nai_iobase_write(nai_iobase_t* s, const void* buf, size_t len);

    /**
 * write multiple buffers to the iobase
 * @param   s       pointer to the iobase
 * @param   v       pointer to an array of the buffer vector
 * @param   count   the count of buffer vector
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to NAI_EAGAIN or NAI_EINPROGRESS.
 * @note    if it fails and errno is set to NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing or using the memory on the stack will cause 
 *          incorrect data to be sent.
 * @note    the memory of the vector itself does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_iobase_writev(nai_iobase_t* s, const nai_bufvec_t* v, nai_int_t count);

    /**
 * read data into the buffer list
 * @param   s       pointer to the iobase
 * @param   in      pointer to the buffer list to receive data
 * @param   limit   maximum the number of bytes read
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to NAI_EAGAIN or NAI_EINPROGRESS.
 * @note    if it fails and errno is set to NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing the buffer will cause the memory area to be destroyed.
 */
    NAI_EXTERN
    intptr_t nai_iobase_readq(nai_iobase_t* s, nai_buflist_t* in, size_t limit);

    /**
 * write the buffer list to the iobase
 * @param   s       pointer to the iobase
 * @param   out     pointer to the buffer list to write
 * @param   limit   maximum the number of bytes written
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to NAI_EAGAIN or NAI_EINPROGRESS.
 * @note    if it fails and errno is set to NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing the buffer will cause incorrect data to be sent.
 */
    NAI_EXTERN
    intptr_t nai_iobase_writeq(nai_iobase_t* s, nai_buflist_t* out, size_t limit);

    /**
 * send a file to the iobase
 * @param   s       pointer to the iobase
 * @param   fd      the file descriptor
 * @param   size    the number of bytes written
 * @param   offset  the start offset of the file
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to NAI_EAGAIN or NAI_EINPROGRESS.
 * @note    if it fails and errno is set to NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the file descriptor until the next call returns successfully. 
 *          close the file descriptor will cause send failed.
 * @note    this function will failed if it is not support sendfile, 
 *          you can check by nai_iobase_get_opt(#NAI_IO_FEAT_SENDFILE).
 * @note    this function will failed if sendfile is not enabled, 
 *          whether or not sendfile is supported.
 * @note    this function will not failed if loadfile is is enabled.
 */
    NAI_EXTERN
    intptr_t nai_iobase_sendfile(nai_iobase_t* s, nai_fd_t fd, size_t size, nai_off64_t offset);

    /**
 * receive data from the iobase
 * @param   s       pointer to the iobase
 * @param   buf     pointer the buffer to receive data
 * @param   len     the length of the buffer
 * @param   flags   the flags of receive, see man of recv
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if it fails and the error code is NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing or using the memory on the stack will cause 
 *          the memory area to be destroyed.
 */
    NAI_EXTERN
    intptr_t nai_iobase_recv(nai_iobase_t* s, void* buf, size_t len, nai_int_t flags);

    /**
 * receive data into multiple buffers
 * @param   s       pointer to the iobase
 * @param   v       pointer to an array of the buffer vector
 * @param   count   the count of the buffer vector
 * @param   flags   the flags of receive, see man of recv
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if it fails and the error code is NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing or using the memory on the stack will cause 
 *          the memory area to be destroyed.
 * @note    the memory of the vector itself does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_iobase_recvv(nai_iobase_t* s, nai_bufvec_t* v, nai_int_t count, nai_int_t flags);

    /**
 * receive data from the iobase
 * @param   s       pointer to the iobase
 * @param   buf     pointer the buffer to receive data
 * @param   len     the length of the buffer
 * @param   flags   the flags of receive, see man of recv
 * @param   name    pointer to the address buffer to receive address
 * @param   namelen pointer to the length of the address buffer
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if it fails and the error code is NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing or using the memory on the stack will cause 
 *          the memory area to be destroyed.
 * @note    similarly, ensure that the memory of 
 *          the receiving address is valid before completion.
 *          don't use memory on the stack, the stack will be overwritten.
 */
    NAI_EXTERN
    intptr_t nai_iobase_recvfrom(
        nai_iobase_t* s, void* buf, size_t len, nai_int_t flags, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * send data to the iobase
 * @param   s       pointer to the iobase
 * @param   buf     pointer to the buffer to write
 * @param   len     the length of the buffer
 * @param   flags   the flags of send, see man of send
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if it fails and the error code is NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing or using the memory on the stack will cause 
 *          incorrect data to be sent.
 */
    NAI_EXTERN
    intptr_t nai_iobase_send(nai_iobase_t* s, const void* buf, size_t len, nai_int_t flags);

    /**
 * send multiple buffers to the iobase
 * @param   s       pointer to the iobase
 * @param   v       pointer to an array of the buffer vector
 * @param   count   the count of the buffer vector
 * @param   flags   the flags of send, see man of send
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if it fails and the error code is NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing or using the memory on the stack will cause 
 *          incorrect data to be sent.
 * @note    the memory of the vector itself does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_iobase_sendv(nai_iobase_t* s, const nai_bufvec_t* v, nai_int_t count, nai_int_t flags);

    /**
 * send data to the iobase
 * @param   s       pointer to the iobase
 * @param   buf     pointer to the buffer to write
 * @param   len     the length of the buffer
 * @param   flags   the flags of send, see man of send
 * @param   name    pointer to the destination address
 * @param   namelen the length of the address
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if it fails and the error code is NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing or using the memory on the stack will cause 
 *          incorrect data to be sent.
 * @note    the memory of the address does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_iobase_sendto(
        nai_iobase_t* s, const void* buf, size_t len, nai_int_t flags, const nai_sockaddr_t* name, nai_int_t namelen);

    /**
 * receive data into multiple buffers
 * @param   s       pointer to the iobase
 * @param   v       pointer to an array of the buffer vector
 * @param   count   the count of the buffer vector
 * @param   flags   the flags of receive, see man of recv
 * @param   name    pointer to the address buffer to receive address
 * @param   namelen pointer to the length of the address buffer
 * @param   ctrl    pointer to the control buffer
 * @param   ctrllen pointer to the length of the control buffer
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if it fails and the error code is NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing or using the memory on the stack will cause 
 *          the memory area to be destroyed.
 * @note    similarly, ensure that the memory of 
 *          the receiving address is valid before completion.
 *          don't use memory on the stack, the stack will be overwritten.
 * @note    similarly, ensure that the memory of 
 *          the control buffer is valid before completion.
 *          don't use memory on the stack, the stack will be overwritten.
 * @note    the memory of the vector itself does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_iobase_recvm(nai_iobase_t* s,
                              nai_bufvec_t* v,
                              nai_int_t count,
                              nai_int_t flags,
                              nai_sockaddr_t* name,
                              nai_int_t* namelen,
                              void* ctrl,
                              nai_int_t* ctrllen);

    /**
 * send multiple buffers to the iobase
 * @param   s       pointer to the iobase
 * @param   v       pointer to an array of the buffer vector
 * @param   count   the count of the buffer vector
 * @param   flags   the flags of send, see man of send
 * @param   name    pointer to the destination address
 * @param   namelen the length of the address
 * @param   ctrl    pointer to the control buffer
 * @param   ctrllen the length of control buffer
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if it fails and the error code is NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing or using the memory on the stack will cause 
 *          incorrect data to be sent.
 * @note    the memory of the vector itself does not cause the above problems.
 * @note    the memory of the address does not cause the above problems.
 * @note    the memory of the ctrl buffer does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_iobase_sendm(nai_iobase_t* s,
                              const nai_bufvec_t* v,
                              nai_int_t count,
                              nai_int_t flags,
                              const nai_sockaddr_t* name,
                              nai_int_t namelen,
                              const void* ctrl,
                              nai_int_t ctrllen);

    /**
 * send multiple messages to the iobase
 * @param   s       pointer to the iobase
 * @param   v       pointer to an array of the buffer array
 * @param   count   the count of the buffer array
 * @param   flags   the flags of send, see man of send
 * @param   name    pointer to the destination address
 * @param   namelen the length of the address
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if it fails and the error code is NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing or using the memory on the stack will cause 
 *          incorrect data to be sent.
 * @note    the memory of the vector itself does not cause the above problems.
 * @note    the memory of the address does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_iobase_sendmm(nai_iobase_t* s,
                               const nai_bufarray_t* v,
                               nai_int_t count,
                               nai_int_t flags,
                               const nai_sockaddr_t* name,
                               nai_int_t namelen);

//////////////////////////////////////////////////////////////////////////////
// server

/**
 * @anchor  NAI_IO_CACHE
 * @name    NAI_IO_CACHE        the name of server cache
 * @{
 */
#define NAI_IO_CACHE_SOCKNAME 0 /**< the cache of sockname */
#define NAI_IO_CACHE_PEERNAME 1 /**< the cache of peername */
    /** @} */

#ifndef _NAI_TYPEDEF_SERVER_T
    #define _NAI_TYPEDEF_SERVER_T
    typedef struct nai_iobase_s nai_server_t;
#endif
#ifndef _NAI_TYPEDEF_SERVER_CB_F
    #define _NAI_TYPEDEF_SERVER_CB_F
    typedef nai_int_t (*nai_server_cb_f)(nai_server_t* s, nai_int_t events);
#endif

/**
 * @name    nai_server_defines  these functions are alias of nai_iobase_*
 * @{
 */
#define nai_server_init         nai_iobase_init
#define nai_server_is_message   nai_iobase_is_message
#define nai_server_is_opened    nai_iobase_is_opened
#define nai_server_is_readable  nai_iobase_is_readable
#define nai_server_is_blocking  nai_iobase_is_blocking
#define nai_server_is_pending   nai_iobase_is_pending
#define nai_server_in_dispatch  nai_iobase_in_dispatch
#define nai_server_get_cb       nai_iobase_get_cb
#define nai_server_get_loop     nai_iobase_get_loop
#define nai_server_get_type     nai_iobase_get_type
#define nai_server_get_fd       nai_iobase_get_fd
#define nai_server_get_fdown    nai_iobase_get_fdown
#define nai_server_get_event    nai_iobase_get_event
#define nai_server_set_cb       nai_iobase_set_cb
#define nai_server_set_fd       nai_iobase_set_fd
#define nai_server_set_fdown    nai_iobase_set_fdown
#define nai_server_set_wait     nai_iobase_set_wait
#define nai_server_set_polling  nai_iobase_set_polling
#define nai_server_set_blocking nai_iobase_set_blocking
#define nai_server_set_except   nai_iobase_set_except
#define nai_server_set_timeout  nai_iobase_set_timeout
#define nai_server_set_subtype  nai_iobase_set_subtype
#define nai_server_set_opt      nai_iobase_set_opt
#define nai_server_get_opt      nai_iobase_get_opt
#define nai_server_get_pending  nai_iobase_get_pending
#define nai_server_post         nai_iobase_post
#define nai_server_join         nai_iobase_join
#define nai_server_return_loop  nai_iobase_return_loop
#define nai_server_close        nai_iobase_close
#define nai_server_finalize     nai_iobase_finalize

/**
 * set the backlog of the server socket
 * @param   s       pointer to the server socket
 * @param   b       the number of the backlog
 * @return  void
 * @note    this function should be called before open the server socket
 */
#define nai_server_set_backlog(s, b)                                                                                   \
    {                                                                                                                  \
        assert((s)->st.agent == 0);                                                                                    \
        (s)->backlog = (b);                                                                                            \
    }

/**
 * get the backlog of the server socket
 * @param   s       pointer to the server socket
 * @return  the value of the backlog
 */
#define nai_server_get_backlog(s) (s)->backlog

    /** @} */

    /**
 * open an exist server socket
 * @param   s       pointer to the server socket
 * @param   l       pointer to the event loop to accosiated
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    shuold call #nai_server_set_fd before do open
 */
    NAI_EXTERN
    nai_int_t nai_server_open(nai_server_t* s, nai_evloop_t* l);

    /**
 * listen on the specific address
 * @param   s       pointer to the server socket
 * @param   l       pointer to the event loop to accosiated
 * @param   name    pointer to the listen address
 * @param   namelen the length of the listen address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will create a socket, if no file descriptor be seted
 */
    NAI_EXTERN
    nai_int_t nai_server_bind(nai_server_t* s, nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);

    /**
 * accept an incoming socket
 * @param   s       pointer to the server socket
 * @param   name    pointer to the buffer to receive address, can be null
 * @param   namelen pointer to the length of address buffer, can be null
 * @retval  >=0     the file descriptor of new socket
 * @retval  NAI_FD_INVALID an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_fd_t nai_server_accept(nai_server_t* s, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * get the cache of the server socket
 * @param   s       pointer the server socket
 * @param   opt     the options name of cache, see @ref NAI_IO_CACHE
 * @param   buf     pointer to the buffer to receive cache
 * @param   len     the length of buffer
 * @retval  >=0     the number of bytes filled in buffer
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_server_cache(nai_server_t* s, nai_int_t opt, void* buf, nai_int_t len);

    //////////////////////////////////////////////////////////////////////////////
    // stream

#ifndef _NAI_TYPEDEF_STREAM_T
    #define _NAI_TYPEDEF_STREAM_T
    typedef struct nai_iobase_s nai_stream_t;
#endif
#ifndef _NAI_TYPEDEF_STREAM_CB_F
    #define _NAI_TYPEDEF_STREAM_CB_F
    typedef nai_int_t (*nai_stream_cb_f)(nai_stream_t* s, nai_int_t events);
#endif

/**
 * @name    nai_stream_defines  these function are alias of nai_iobase_*
 * @{
 */
#define nai_stream_init         nai_iobase_init
#define nai_stream_is_file      nai_iobase_is_file
#define nai_stream_is_pipe      nai_iobase_is_pipe
#define nai_stream_is_sock      nai_iobase_is_sock
#define nai_stream_is_seekable  nai_iobase_is_seekable
#define nai_stream_is_message   nai_iobase_is_message
#define nai_stream_is_opened    nai_iobase_is_opened
#define nai_stream_is_readable  nai_iobase_is_readable
#define nai_stream_is_writable  nai_iobase_is_writable
#define nai_stream_is_blocking  nai_iobase_is_blocking
#define nai_stream_is_pending   nai_iobase_is_pending
#define nai_stream_in_dispatch  nai_iobase_in_dispatch
#define nai_stream_get_cb       nai_iobase_get_cb
#define nai_stream_get_loop     nai_iobase_get_loop
#define nai_stream_get_type     nai_iobase_get_type
#define nai_stream_get_fd       nai_iobase_get_fd
#define nai_stream_get_fdown    nai_iobase_get_fdown
#define nai_stream_get_mode     nai_iobase_get_mode
#define nai_stream_get_event    nai_iobase_get_event
#define nai_stream_set_cb       nai_iobase_set_cb
#define nai_stream_set_fd       nai_iobase_set_fd
#define nai_stream_set_fdown    nai_iobase_set_fdown
#define nai_stream_set_mode     nai_iobase_set_mode
#define nai_stream_set_wait     nai_iobase_set_wait
#define nai_stream_set_polling  nai_iobase_set_polling
#define nai_stream_set_blocking nai_iobase_set_blocking
#define nai_stream_set_except   nai_iobase_set_except
#define nai_stream_set_timeout  nai_iobase_set_timeout
#define nai_stream_set_opt      nai_iobase_set_opt
#define nai_stream_get_opt      nai_iobase_get_opt
#define nai_stream_get_pending  nai_iobase_get_pending
#define nai_stream_inherit      nai_iobase_inherit
#define nai_stream_post         nai_iobase_post
#define nai_stream_join         nai_iobase_join
#define nai_stream_return_loop  nai_iobase_return_loop
#define nai_stream_close        nai_iobase_close
#define nai_stream_finalize     nai_iobase_finalize
#define nai_stream_shutdown     nai_iobase_shutdown
#define nai_stream_read         nai_iobase_read
#define nai_stream_readv        nai_iobase_readv
#define nai_stream_readq        nai_iobase_readq
#define nai_stream_write        nai_iobase_write
#define nai_stream_writev       nai_iobase_writev
#define nai_stream_writeq       nai_iobase_writeq
#define nai_stream_sendfile     nai_iobase_sendfile
#define nai_stream_recv         nai_iobase_recv
#define nai_stream_recvv        nai_iobase_recvv
#define nai_stream_send         nai_iobase_send
#define nai_stream_sendv        nai_iobase_sendv
#define nai_stream_recvm        nai_iobase_recvm
#define nai_stream_sendm        nai_iobase_sendm
#define nai_stream_sendmm       nai_iobase_sendmm

/**
 * set the offset of the file
 * @param   s       pointer to the stream
 * @param   o       the new value of the offset
 * @return  void
 * @note    this function should not be used on non-file streams
 */
#define nai_stream_set_offset(s, o)                                                                                    \
    {                                                                                                                  \
        assert((s)->st.agent == 0);                                                                                    \
        (s)->offset = (o);                                                                                             \
    }

/**
 * get the offset of the file
 * @param   s       pointer to the stream
 * @return  the value of the offset
 */
#define nai_stream_get_offset(s) (s)->offset

    /** @} */

    /**
 * open an exist file,pipe or stream socket
 * @param   s       pointer to the stream
 * @param   l       pointer to the event loop to accosiated
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    shuold call #nai_stream_set_fd before do open
 */
    NAI_EXTERN
    nai_int_t nai_stream_open(nai_stream_t* s, nai_evloop_t* l);

    /**
 * connect to the specific address
 * @param   s       pointer to the stream
 * @param   l       pointer to the event loop to accosiated
 * @param   name    pointer to the connect address
 * @param   namelen the length of the connect address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will create a socket, if no file descriptor be seted
 */
    NAI_EXTERN
    nai_int_t nai_stream_connect(nai_stream_t* s, nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);

    //////////////////////////////////////////////////////////////////////////////
    // dgram

#ifndef _NAI_TYPEDEF_DGRAM_T
    #define _NAI_TYPEDEF_DGRAM_T
    typedef struct nai_iobase_s nai_dgram_t;
#endif
#ifndef _NAI_TYPEDEF_DGRAM_CB_F
    #define _NAI_TYPEDEF_DGRAM_CB_F
    typedef nai_int_t (*nai_dgram_cb_f)(nai_dgram_t* s, nai_int_t events);
#endif

/**
 * @name    nai_dgram_defines   these functions are alias of nai_iobase_*
 * @{
 */
#define nai_dgram_init         nai_iobase_init
#define nai_dgram_is_message   nai_iobase_is_message
#define nai_dgram_is_opened    nai_iobase_is_opened
#define nai_dgram_is_readable  nai_iobase_is_readable
#define nai_dgram_is_writable  nai_iobase_is_writable
#define nai_dgram_is_blocking  nai_iobase_is_blocking
#define nai_dgram_is_pending   nai_iobase_is_pending
#define nai_dgram_in_dispatch  nai_iobase_in_dispatch
#define nai_dgram_get_cb       nai_iobase_get_cb
#define nai_dgram_get_loop     nai_iobase_get_loop
#define nai_dgram_get_type     nai_iobase_get_type
#define nai_dgram_get_fd       nai_iobase_get_fd
#define nai_dgram_get_fdown    nai_iobase_get_fdown
#define nai_dgram_get_mode     nai_iobase_get_mode
#define nai_dgram_get_event    nai_iobase_get_event
#define nai_dgram_set_cb       nai_iobase_set_cb
#define nai_dgram_set_fd       nai_iobase_set_fd
#define nai_dgram_set_fdown    nai_iobase_set_fdown
#define nai_dgram_set_mode     nai_iobase_set_mode
#define nai_dgram_set_wait     nai_iobase_set_wait
#define nai_dgram_set_polling  nai_iobase_set_polling
#define nai_dgram_set_blocking nai_iobase_set_blocking
#define nai_dgram_set_except   nai_iobase_set_except
#define nai_dgram_set_timeout  nai_iobase_set_timeout
#define nai_dgram_set_subtype  nai_iobase_set_subtype
#define nai_dgram_set_opt      nai_iobase_set_opt
#define nai_dgram_get_opt      nai_iobase_get_opt
#define nai_dgram_get_pending  nai_iobase_get_pending
#define nai_dgram_inherit      nai_iobase_inherit
#define nai_dgram_post         nai_iobase_post
#define nai_dgram_join         nai_iobase_join
#define nai_dgram_return_loop  nai_iobase_return_loop
#define nai_dgram_close        nai_iobase_close
#define nai_dgram_finalize     nai_iobase_finalize
#define nai_dgram_shutdown     nai_iobase_shutdown
#define nai_dgram_read         nai_iobase_read
#define nai_dgram_readv        nai_iobase_readv
#define nai_dgram_readq        nai_iobase_readq
#define nai_dgram_write        nai_iobase_write
#define nai_dgram_writev       nai_iobase_writev
#define nai_dgram_writeq       nai_iobase_writeq
#define nai_dgram_sendfile     nai_iobase_sendfile
#define nai_dgram_recv         nai_iobase_recv
#define nai_dgram_recvv        nai_iobase_recvv
#define nai_dgram_recvfrom     nai_iobase_recvfrom
#define nai_dgram_send         nai_iobase_send
#define nai_dgram_sendv        nai_iobase_sendv
#define nai_dgram_sendto       nai_iobase_sendto
#define nai_dgram_recvm        nai_iobase_recvm
#define nai_dgram_sendm        nai_iobase_sendm
#define nai_dgram_sendmm       nai_iobase_sendmm
    /** @} */

    /**
 * open an exist dgram or seqpacket socket
 * @param   s       pointer to the dgram
 * @param   l       pointer to the event loop to accosiated
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    shuold call #nai_dgram_set_fd before do open
 */
    NAI_EXTERN
    nai_int_t nai_dgram_open(nai_dgram_t* s, nai_evloop_t* l);

    /**
 * bind on the specific address
 * @param   s       pointer to the dgram
 * @param   l       pointer to the event loop to accosiated
 * @param   name    pointer to the address to bind
 * @param   namelen the length of the address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will create a socket, if no file descriptor be seted.
 *          this function is used for dgram socket only.
 */
    NAI_EXTERN
    nai_int_t nai_dgram_bind(nai_dgram_t* s, nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);

    /**
 * connect to the specific address
 * @param   s       pointer to the stream
 * @param   l       pointer to the event loop to accosiated
 * @param   name    pointer to the connect address
 * @param   namelen the length of the connect address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will create a socket, if no file descriptor be seted.
 */
    NAI_EXTERN
    nai_int_t nai_dgram_connect(nai_dgram_t* s, nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);

    /**
 * make a new dgram socket which is connected with the client
 * @param   s       pointer to the dgram
 * @param   l       pointer to the listen dgram
 * @param   name    pointer to the connect address
 * @param   namelen the length of the connect address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_dgram_bypass(nai_dgram_t* s, nai_dgram_t* l, const nai_sockaddr_t* name, nai_int_t namelen);

    /**
 * set options of reuse address and port
 * @param   s       pointer to the dgram
 * @param   connect indicates whether it is used for connect or load balancing
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    on linux, reuse port is used for load balancing
 */
    NAI_EXTERN
    nai_int_t nai_dgram_reuse_port(nai_dgram_t* s, nai_int_t connect);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
