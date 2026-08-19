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
/// @file       npc_comm.h
/// @brief      the comm interface base on shared memory
/// @details    the comm interface manage the shared memory and the message ports.
/// @details    the code example is as follows:
/// @date       2025-03-07
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

/**
 * @file    npc_comm.h
 * @brief   the comm interface base on shared memory
 *
 * @details the comm interface manage the shared memory and the message ports.
 *
 * @details the code example is as follows:
 *
 * @par     open the comm manager
 * @code
 *          int r;
 *          npc_comm_t* c;
 *          nai_evloop_t* l;
 *
 *
 *          npc_comm_init(c);
 *          r = npc_comm_open(c, l);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     close the comm manager
 * @code
 *          int r;
 *          npc_comm_t* c;
 *
 *
 *          r = npc_comm_close(c);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     create a new message from the comm manager
 * @code
 *          npc_serv_t serv;
 *          npc_inst_t inst;
 *          npc_comm_t* c;
 *          npc_message_t* m;
 *
 *
 *          m = npc_comm_create_message(c, serv, inst, NPC_USAGE_SEND);
 *          if (m == 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     the implemention of npc_comif_ops_t
 * @code
 *          static int comif_post(void* ctx, uint16_t peer, uint32_t event)
 *          {
 *              int r;
 *
 *              // lookup peer
 *              ...
 *
 *              // send event to peer
 *              ...
 *          };
 *
 *          static int comif_test(void* ctx, uint16_t eid, uint16_t uid)
 *          {
 *              int r;
 *
 *              // lookup event
 *              ...
 *
 *              // test unqine id
 *              ...
 *          };
 *
 *          static int comif_read(void* ctx, npc_port_t* p)
 *          {
 *              int r;
 *              int ec;
 *              npc_message_t* m;
 *
 *
 *              while (1) {
 *                  m = npc_port_read(p);
 *                  if (m == 0) {
 *                      ec = nai_errno;
 *                      if (ec == NAI_EAGAIN) {
 *                          r = 0;
 *                          goto _end;
 *                      };
 *
 *                      r = -1;
 *                      goto _end;
 *                  };
 *
 *                  // handle message
 *                  ...
 *              };
 *
 *          _end:
 *              return r;
 *          };
 *
 *          static int comif_quit(void* ctx, npc_port_t* p)
 *          {
 *              int r;
 *
 *
 *              if (p == 0) {
 *                  // the comm interface is quitable, do disconnect
 *                  ...
 *              } else {
 *                  // the message port is quitable, do unsubscribe
 *                  ...
 *              };
 *          };
 *
 *          static npc_comif_ops_t ops = {
 *              "my_comif", 
 *              comif_post, 
 *              comif_test, 
 *              comif_read, 
 *              comif_quit
 *          };
 * @endcode
 *
 * @par     open a new comm infterface
 * @code
 *          npc_serv_t serv;
 *          npc_inst_t inst;
 *          npc_shmz_attr_t a;
 *          npc_comm_t* c;
 *          npc_comif_t* cif;
 *          const char* name = "/tmp/comm.shmz"
 *
 *
 *          a.serv = serv;
 *          a.inst = inst;
 *          a.self = 1;
 *          a.seg_count = 1;
 *          a.seg_size = 128 * 1024 * 1024;
 *
 *          cif = npc_comif_open(c, ops, ctx, 
 *              path, &a, NAI_O_CREAT|NAI_O_RDWR, 0644);
 *          if (cif == 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     handle the event which is received from the peer
 * @code
 *          int r;
 *          int event;
 *          npc_comif_t* cif;
 *
 *
 *          r = npc_comif_event(cif, event);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     close the comm interface
 * @code
 *          int r;
 *          int ec;
 *          npc_comif_t* cif;
 *
 *
 *          r = npc_comif_state(cif, NPC_STATE_QUIT);
 *          if (r < 0) {
 *              ec = nai_errno;
 *              if (ec == NAI_EAGAIN) {
 *                  goto _wait;
 *              };
 *              goto _fail;
 *          };
 *
 *          r = npc_comif_close(cif);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 */

#ifndef _COMM_H_NPC
#define _COMM_H_NPC


#pragma once


#include "npc_zone.h"
#include "npc_types.h"
#include "nai/io/nai_io.h"
#include "nai/io/nai_buf.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */



/**
 * @anchor  NPC_USAGE_TYPE
 * @name    NPC_USAGE_TYPE  the usage of the message
 * @{
 */
#define NPC_USAGE_READ      0   /**< create for read */
#define NPC_USAGE_SEND      1   /**< create for send */
/** @} */


/**
 * @anchor  NPC_STATE_TYPE
 * @name    NPC_STATE_TYPE  the state of the comm object
 * @{
 */
#define NPC_STATE_OPEN      0   /**< state of open */
#define NPC_STATE_QUIT      1   /**< state of quit */
/** @} */



//////////////////////////////////////////////////////////////////////////////
// comm interface


#ifndef _NPC_TYPEDEF_MESSAGE_T
#define _NPC_TYPEDEF_MESSAGE_T
typedef struct nsi_message_s npc_message_t;
#endif

#ifndef _NPC_TYPEDEF_MSGHDR_T
#define _NPC_TYPEDEF_MSGHDR_T
typedef struct nsi_msghdr_s  npc_msghdr_t;
#endif

#ifndef _NPC_TYPEDEF_COMM_T
#define _NPC_TYPEDEF_COMM_T
typedef struct npc_comm_s npc_comm_t;
#endif


/**
 * the structure of comm manager
 */
struct npc_comm_s {
    nai_evnode_t ev;            /**< the evnode */
    nai_rbtree_t ifs;           /**< the map of comm interface */
    nai_spin_t lock;            /**< pointer to the lock */
    nai_list_t mlist;           /**< the list of message to free */
    nai_list_t clist;           /**< the list of comm interface to free */
    void *ud;                   /**< point to network for nsomeip */
};

/**
 * initial the comm manager
 * @param   c       pointer to the comm manager
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_comm_init(npc_comm_t* c);


/**
 * open the comm manager
 * @param   c       pointer to the comm manager
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_comm_open(npc_comm_t* c, nai_evloop_t* loop);


/**
 * close the comm manager
 * @param   c       pointer to the comm manager
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_comm_close(npc_comm_t* c);


/**
 * create a new message
 * @param   c       pointer to the comm manager
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   usage   the usage of the message, see @ref NPC_USAGE_TYPE
 * @retval  !=0     the address of the new message on success
 * @retval  0       an error occurred, see #nai_errno
 */
npc_message_t* npc_comm_create_message(
    npc_comm_t* c, npc_serv_t serv, npc_inst_t inst, int usage);



//////////////////////////////////////////////////////////////////////////////
// comm interface


#ifndef _NPC_TYPEDEF_PORT_T
#define _NPC_TYPEDEF_PORT_T
typedef struct npc_port_s npc_port_t;
#endif
#ifndef _NPC_TYPEDEF_COMIF_T
#define _NPC_TYPEDEF_COMIF_T
typedef struct npc_comif_s npc_comif_t;
#endif
#ifndef _NPC_TYPEDEF_COMIF_OPS_T
#define _NPC_TYPEDEF_COMIF_OPS_T
typedef struct npc_comif_ops_s npc_comif_ops_t;
#endif



/**
 * the structure of comm interface opeartion
 */
struct npc_comif_ops_s {
    /**
     * the opeartion name
     */
    const char* name;


    /**
     * post a event to peer
     * @param   ctx     pointer to the user context
     * @param   peer    the peer id
     * @param   event   the value of comm event
     * @retval  >=0     on success
     * @retval  -1      an error occurred, see #nai_errno
     */
    int (*post)(void* ctx, uint16_t peer, uint32_t event);


    /**
     * test whether the unique id is duplicate
     * @param   ctx     pointer to the user context
     * @param   eid     the event id
     * @param   uid     the unique id
     * @retval  0       on success
     * @retval  !=0     is a duplicate unique id
     */
    int (*test)(void* ctx, uint16_t eid, uint16_t uid);


    /**
     * the event of the message port is readable
     * @param   ctx     pointer to the user context
     * @param   port    pointer to the message port
     * @retval  >=0     on success
     * @retval  -1      an error occurred, see #nai_errno
     */
    int (*read)(void* ctx, npc_port_t* port);


    /**
     * the event of the message port or the comm interface is closable
     * @param   ctx     pointer to the user context
     * @param   port    pointer to the message port, null means it is comif
     * @retval  >=0     on success
     * @retval  -1      an error occurred, see #nai_errno
     */
    int (*quit)(void* ctx, npc_port_t* port);
};


/**
 * the structure of comm interface
 */
struct npc_comif_s {

    union {
        nai_rbnode_t ent;       /**< the entry of comm interface map */
        nai_list_entry_t entl;
    };

    /* service id */
    union {
        /**
         * keep serv is high word, 
         * so that the same serv is continuously in the tree.
         */
        struct {
            uint32_t inst:16;
            uint32_t serv:16;
        };
        npc_servinst_t servinst;
    };

    /* state */
    union {
        struct {
            uint16_t local:1;   /**< the current process is creator */
            uint16_t quit:1;    /**< the comm interface is in quiting */
            uint16_t self;      /**< the self id */
        };
        uint32_t stat;
    };

    npc_shmz_t shmz;            /**< the shared memory zone */
    npc_zone_t zone;            /**< the local memory zone */
    npc_comm_t* com;            /**< the comm manager */
    npc_port_t* defport;        /**< pointer to the default port */
    npc_comif_ops_t* ops;       /**< pointer to comm interface opeartion */
    void* ctx;                  /**< pointer to the used context */
    nai_atomic32_t refs;        /**< the ref count */
    nai_atomic32_t refm;        /**< the ref count of inuse message */
    nai_bufpool_t pool;         /**< the meta memory pool */
    nai_rbtree_t ports;         /**< the map of message ports */
    nai_rbtree_t holds;         /**< the map of memory waiting to free */
    nai_list_t mlist[2];        /**< the freed message list */
};



/**
 * create a comm interface
 * @param   c       pointer to comm manager
 * @param   ops     pointer to comm interface opeartion
 * @param   ctx     pointer to the user context
 * @param   path    pointer to the path of the shared memory
 * @param   attr    the attribute of the shared memory
 * @param   flags   the flags of opening, see @ref NAI_FOPEN
 * @param   ...     mode, the initial permissions, ie. 0755, 0644
 * @retval  !=0     the address of the new comm interface on success
 * @retval  0       an error occurred, see #nai_errno
 */
npc_comif_t* npc_comif_create(
    npc_comm_t* c, 
    npc_comif_ops_t* ops, void* ctx, 
    const char* path, 
    const npc_shmz_attr_t* attr, int flags, ...);


/**
 * handle the event of the comm interface
 * @param   p       pointer to the comm interface
 * @param   event   the value of comm event
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_comif_event(npc_comif_t* p, uint32_t event);


/**
 * set the state of the comm interface
 * @param   p       pointer to the comm interface
 * @param   state   the state code, see @ref NAI_STATE_TYPE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the state is NAI_STATE_QUIT and it fails and the error code 
 *          is NAI_EAGIAN, it means that the object is still referenced and 
 *          cannot be closed temporarily, and will be notified through a 
 *          callback after the reference is released.
 */
int npc_comif_state(npc_comif_t* p, int state);


/**
 * clean the memory which allocated by the peer
 * @param   p       pointer to the comm interface
 * @param   peer    the peer id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is used for abnormally disconnected remote
 */
int npc_comif_clean(npc_comif_t* p, uint16_t peer);


/**
 * close the comm interface
 * @param   p       pointer to the comm interface
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_comif_close(npc_comif_t* p);



#ifdef __cplusplus
};
#endif  /* __cplusplus */


#endif

