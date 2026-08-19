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
/// @file       npc_port.h
/// @brief      the message port base on shared memory
/// @details    the message ports are simplex message queues that support
/// @details    the code example is as follows:
/// @date       2022-07-09
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @file    npc_port.h
 * @brief   the message port base on shared memory
 *
 * @details the message ports are simplex message queues that support 
 *          unicast or multicast.
 *
 * @details the code example is as follows:
 *
 *
 * @par     create a pair message port for peer-to-peer communication
 * @code
 *          int r;
 *          npc_port_t* rd;
 *          npc_port_t* wr;
 *          npc_comif_t* cif;
 *          npc_cid_t self;
 *          npc_cid_t peer;
 *
 *
 *          rd = npc_port_create(cif, 1, 32, NPC_PORT_READ, peer);
 *          if (rd == 0) {
 *              goto _fail;
 *          };
 *
 *          wr = npc_port_create(cif, 1, 32, NPC_PORT_SEND);
 *          if (wr == 0) {
 *              goto _fail;
 *          };
 *
 *          r = npc_port_add(wr, peer);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     create an event group port
 * @code
 *          int r;
 *          npc_port_t* p;
 *          npc_comif_t* cif;
 *
 *
 *          p = npc_port_create(cif, 32, 128, NPC_PORT_SEND);
 *          if (p == 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     attach an existing message port
 * @code
 *          int r;
 *          npc_port_t* p;
 *          npc_comif_t* cif;
 *          npc_cid_t self;
 *          npc_mref_t mref;
 *
 *
 *          p = npc_port_attach(cif, mref, NPC_PORT_READ);
 *          if (p == 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     send a message to the message port
 * @code
 *          int r;
 *          npc_port_t* p;
 *          npc_message_t* m;
 *
 *
 *          r = npc_port_send(p, m);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     read a message from the message port
 * @code
 *          int ec;
 *          npc_port_t* p;
 *          npc_message_t* m;
 *
 *
 *          m = npc_port_read(p);
 *          if (m == 0) {
 *              ec = nai_errno;
 *              if (ec != EAGAIN) {
 *                  goto _fail;
 *              }:
 *
 *              goto _end;
 *          };
 *
 *          // handle message
 *          ...
 * @endcode
 *
 * @par     close the message port
 * @code
 *          int r;
 *          int ec;
 *          npc_port_t* p;
 *
 *
 *          r = npc_port_close(p);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 */

#ifndef _PORT_H_NPC
#define _PORT_H_NPC

#pragma once

#include "npc/shm/npc_comm.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

//////////////////////////////////////////////////////////////////////////////
// message port

/**
 * @anchor  NPC_PORT_TYPE
 * @name    NPC_PORT_TYPE   the usage of the message
 * @{
 */
#define NPC_PORT_READ     0 /**< specifies the port for read mode */
#define NPC_PORT_SEND     1 /**< specifies the port for send mode */
#define NPC_PORT_RELIABLE 2 /**< specifies the port is reliable */
/** @} */

/**
 * @anchor  NPC_PORT_EVENT
 * @name    NPC_PORT_EVENT  the usage of the message
 * @{
 */
#define NPC_PORT_READABLE  1
#define NPC_PORT_SENDABLE  2
#define NPC_PORT_REMOVABLE 3
    /** @} */

#ifndef _NPC_TYPEDEF_MPEER_T
    #define _NPC_TYPEDEF_MPEER_T
    typedef struct npc_mpeer_s npc_mpeer_t;
#endif
#ifndef _NPC_TYPEDEF_MPORT_T
    #define _NPC_TYPEDEF_MPORT_T
    typedef struct npc_mport_s npc_mport_t;
#endif
#ifndef _NPC_TYPEDEF_PORT_T
    #define _NPC_TYPEDEF_PORT_T
    typedef struct npc_port_s npc_port_t;
#endif
#ifndef _NPC_TYPEDEF_PORT_EV_T
    #define _NPC_TYPEDEF_PORT_EV_T
    typedef struct npc_port_ev_s npc_port_ev_t;
#endif
#ifndef _NPC_TYPEDEF_MESSAGE_T
    #define _NPC_TYPEDEF_MESSAGE_T
    typedef struct npc_message_s npc_message_t;
#endif

    /**
 * the structure of the peer
 */
    struct npc_mpeer_s
    {
        uint16_t closed : 1; /**< is peer closed */
        uint16_t next : 15;  /**< the read pointer */
        uint16_t peer;       /**< the id of peer */
    };

    /**
 * the structure of the port
 */
    struct npc_mport_s
    {
        uint16_t tag;    /**< the tag of the port */
        uint16_t type;   /**< the type of the port */
        uint16_t randid; /**< the random id for debug */
        uint16_t source; /**< the sender id */
        uint16_t closed; /**< is closed */
        uint16_t last;   /**< the last pointer of the message queue */
        uint16_t head;   /**< the head pointer of the message queue */
        uint16_t pstart; /**< the offset of the peer array */
        uint16_t pcount; /**< the max count of the peer array */
        uint16_t qstart; /**< the offset of the message queue */
        uint16_t qcount; /**< the max count of the message queue */
    };

    /**
 * the structure of the port event
 */
    struct npc_port_ev_s
    {
        union
        {
            struct
            {
                uint32_t port : 28;
                uint32_t event : 4;
            };
            uint32_t value;
        };
    };

    /**
 * the structure of the message port
 */
    struct npc_port_s
    {
        nai_rbnode_t ent; /**< the entry of rbtree in comm interface */

        npc_comif_t* cif; /**< pointer to the comm interface */

        /* state */
        union
        {
            struct
            {
                uint16_t local : 1;  /**< the current process is creator */
                uint16_t quit : 1;   /**< the message port is in quiting */
                uint16_t clean : 1;  /**< is cleaning by npc_comif_clean */
                uint16_t closed : 1; /**< is closed port */
                uint16_t reserved : 4;
                uint16_t mode : 8; /**< the mode of port */
                uint16_t self;     /**< the self id */
            };
            uint32_t stat;
        };

        npc_mref_t mref;    /**< the reference of port */
        npc_mport_t* mport; /**< pointer to the port */
        npc_mpeer_t* mpeer; /**< pointer to the peer array */
        npc_mref_t* mqueue; /**< pointer to the message queue */

        union
        {
            /* for read */
            struct
            {
                uint16_t last;   /**< read last pointer */
                uint16_t next;   /**< read next pointer */
                uint16_t slot;   /**< the slot of reader in peer array */
                uint8_t stat[2]; /**< the bits vector of read state */
            } in;

            /* for send */
            struct
            {
                nai_list_t send; /**< the sending queue */
                nai_list_t wait; /**< the waiting queue */
            } out;
        };
    };

    /**
 * attach an existing message port
 * @param   p       pointer to the comm interface
 * @param   ptr     the reference of port
 * @param   flags   the open flags of port, see @ref NAI_PORT_OPEN
 * @retval  !=0     the address of the new message port on success
 * @retval  0       an error occurred, see #nai_errno
 */
    npc_port_t* npc_port_attach(npc_comif_t* p, npc_mref_t ptr, int flags);

    /**
 * create a new message port
 * @param   p       pointer to the comm interface
 * @param   pcount  the max count of the peer array
 * @param   qcount  the max count of the message queue
 * @param   flags   the open flags of port, see @ref NPC_PORT_OPEN
 * @param   ...     the sender id, used with #NPC_PORT_READ
 * @retval  !=0     the address of the new message port on success
 * @retval  0       an error occurred, see #nai_errno
 */
    npc_port_t* npc_port_create(npc_comif_t* p, uint16_t pcount, uint16_t qcount, int flags, ...);

    /**
 * close the message port
 * @param   p       pointer to the message port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int npc_port_close(npc_port_t* p);

    /**
 * set the state of the message port
 * @param   p       pointer to the message port
 * @param   state   the state code, see @ref NAI_STATE_TYPE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the state is NAI_STATE_QUIT and it fails and the error code 
 *          is NAI_EAGIAN, it means that the object is still referenced and 
 *          cannot be closed temporarily, and will be notified through a 
 *          callback after the reference is released.
 */
    int npc_port_state(npc_port_t* p, int stat);

    /**
 * add a new peer into the message port
 * @param   p       pointer to the message port
 * @param   peer    the peer id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int npc_port_add(npc_port_t* p, uint16_t peer);

    /**
 * remove a peer from the message port
 * @param   p       pointer to the message port
 * @param   peer    the peer id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int npc_port_remove(npc_port_t* p, uint16_t peer);

    /**
 * queue a message to the message port
 * @param   p       pointer to the message port
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int npc_port_send(npc_port_t* p, npc_message_t* m);

    int npc_port_is_full(npc_port_t* p);

    /**
 * read a message from the message port
 * @param   p       pointer to the message port
 * @retval  !=0     the address of the new message port on success
 * @retval  0       an error occurred, see #nai_errno
 */
    npc_message_t* npc_port_read(npc_port_t* p);

    //////////////////////////////////////////////////////////////////////////////
    // internal api

    nai_rbnode_t** npc_port_find(npc_comif_t* p, npc_mref_t mref, nai_rbnode_t** pparent);

    /**
 * internal api, handle the event of port readable
 * @param   p       pointer to the message port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int npc_port_handle_read(npc_port_t* p);

    /**
 * internal api, handle the event of port sendable
 * @param   p       pointer to the message port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int npc_port_handle_send(npc_port_t* p);

    /**
 * internal api, handle the event of the message has been seen
 * @param   p       pointer to the message port
 * @param   m       pointer to the message which has been seen
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int npc_port_handle_seen(npc_port_t* p, npc_message_t* m);

    /**
 * internal api, handle the event of peer is closed
 * @param   p       pointer to the message port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int npc_port_handle_remove(npc_port_t* e);

    /**
 * internal api, handle the state of port or comm interface is changed
 * @param   p       pointer to the message port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int npc_port_handle_state(npc_port_t* p);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
