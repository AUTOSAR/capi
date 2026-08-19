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
/// @file       utility.h
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_IPC_UTILITY_CPP_H
#define ISOFT_IPC_UTILITY_CPP_H

#include <isoft/naicpp/global_evloop.h>

#include "isoft/ipc/ipc.h"

namespace isoft {
namespace ipc {

/*
 * @brief Initialize the eventloop method, needs to be called first before calling other IPC functions
 * @param event_loop NAI event loop
 *  */
inline int IPCInitNaiUDS(std::shared_ptr< isoft::naicpp::EvLoop > event_loop)
{
    return ipc_init_nai_unix_socket(event_loop->GetRawEvLoop());
}

inline int IPCDeInitNaiUDS() { return ipc_deinit_nai_unix_socket(); }
}  // namespace ipc
}  // namespace isoft
#endif