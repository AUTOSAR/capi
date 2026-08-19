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
/// @file       peer_uds.c
/// @brief
/// @details
/// @date       2022-12-14
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================
///
/// _create() is for use by the IPC common components
/// _find() is for use by the unix domain socket channel
/// Both of the above functions increase the reference count of the returned peer by one.
/// Simply put, creating a peer implicitly adds one reference.
///
/// ================================================================

#include <assert.h>
#include <isoft/ipc/ipc_base_C.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "peer.h"
#include "tree.h"
#include "utility.h"

struct ipc_peer_s
{
    RB_ENTRY(ipc_peer_s) node;
    atomic_uintptr_t reference;
    struct sockaddr_un address;
};

typedef struct
{
    RB_HEAD(ipc_peer_table_s, ipc_peer_s) tree;
    ipc_spinlock_t lock;
} ipc_peer_table_t;

static bool uds_table_initialized = false;
static ipc_peer_table_t uds_table;

static int uds_compare(const ipc_peer_t* left, const ipc_peer_t* right)
{
    return strncmp((const char*)&left->address.sun_path, (const char*)&right->address.sun_path,
                   sizeof(left->address.sun_path));
}

RB_GENERATE_INTERNAL(ipc_peer_table_s, ipc_peer_s, node, uds_compare, static)

void init_ipc_peer_module(void)
{
    RB_INIT(&uds_table.tree);
    ipc_spinlock_init(&uds_table.lock);
    uds_table_initialized = true;
}

void deinit_ipc_peer_module(void) {}

ipc_peer_t* ipc_peer_ref(ipc_peer_t* peer)
{
    if (peer != NULL) {
        atomic_fetch_add(&peer->reference, 1);
    }
    return peer;
}

static void ipc_peer_free(ipc_peer_t* peer) { free(peer); }

void ipc_peer_release(ipc_peer_t* peer)
{
    if (peer != NULL) {
        if (atomic_fetch_sub(&peer->reference, 1) == 1) {
            ipc_spinlock_lock(&uds_table.lock);
            RB_REMOVE(ipc_peer_table_s, &uds_table.tree, peer);
            ipc_spinlock_unlock(&uds_table.lock);
            ipc_peer_free(peer);
        }
    }
}

/*
  note: Lock and Ref are needed, otherwise the peer could be freed before returned
 */
static ipc_peer_t* inner_peer_create(ipc_peer_t* tmp)
{
    ipc_peer_t* peer;

    ipc_spinlock_lock(&uds_table.lock);

    peer = RB_FIND(ipc_peer_table_s, &uds_table.tree, tmp);

    if (peer == NULL) {
        peer = (ipc_peer_t*)malloc(sizeof(*peer));
        if (peer != NULL) {
            peer->address.sun_family = AF_UNIX;
            strncpy(peer->address.sun_path, tmp->address.sun_path, sizeof(peer->address.sun_path));
            atomic_init(&peer->reference, 0);
            RB_INSERT(ipc_peer_table_s, &uds_table.tree, peer);
        }
    }

    if (peer != NULL) {
        ipc_peer_ref(peer);
    }

    ipc_spinlock_unlock(&uds_table.lock);
    return peer;
}

/*
  param: name, the name resolved to path
 */
ipc_peer_t* ipc_peer_find(const char* name)
{
    ipc_peer_t tmp;

    assert(uds_table_initialized);

    if (ipc_base_socket_resolve_name(name, tmp.address.sun_path, sizeof(tmp.address.sun_path)) != 0) {
        return NULL;
    }

    return inner_peer_create(&tmp);
}

/*
  param: path, the key
 */
ipc_peer_t* ipc_peer_uds_find_by_address(struct sockaddr_un* addr)
{
    ipc_peer_t* fake_peer;

    assert(uds_table_initialized);

    // because what only needed is just addr->sun_path
    // it is expected fake_peer is only used to access ->address.sun_path
    fake_peer = ipc_container_of(addr, ipc_peer_t, address);

    return inner_peer_create(fake_peer);
}

struct sockaddr_un* ipc_peer_uds_get_address(ipc_peer_t* peer)
{
    if (peer == NULL) {
        return NULL;
    }
    return &peer->address;
}
