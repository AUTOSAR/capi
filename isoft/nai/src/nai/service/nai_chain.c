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
/// @file       nai_chain.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/service/nai_chain.h"
#include "nai/runtime/nai_util.h"



nai_int_t nai_chain_link(nai_chain_t* c, nai_chain_t* node)
{
    nai_chain_t* n;
    nai_list_entry_t* e;

    e = c->ent.next;
    for ( ; e != &c->ent; e = e->next) {
        n = (nai_chain_t*)e;
        if (n->priority > node->priority) {
            break;
        };
    };

    nai_list_insert_before(e, &node->ent);
    return 0;
};


