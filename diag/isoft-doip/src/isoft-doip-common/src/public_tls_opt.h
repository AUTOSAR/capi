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
/// @file       public_tls_opt.h
/// @brief      This file provides TLS configuration item definitions.
/// @details
/// @date       2025-09-02
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_COMMON_PUBLIC_TLS_OPT_H_
#define ISOFT_DOIP_COMMON_PUBLIC_TLS_OPT_H_
#ifdef NAI_ENABLE_SSL
    #include <nai/io/nai_ssl.h>
#endif

namespace isoft {
namespace doip {

#ifdef NAI_ENABLE_SSL
static nai_ssl_ops_t tls_ops = {
    nullptr, /* lookup cert */
    nullptr, /* lookup server */
    nullptr, /* cookie verify */
    nullptr, /* cookie generate */
    nai_ssl_iobase_poll,
    nai_ssl_iobase_want,
    nai_ssl_iobase_emit,
    nai_ssl_iobase_get_loop,
    nai_ssl_iobase_get_opt,
    nai_ssl_iobase_get_peer,
    nullptr, /* realloc */
    (nai_ssl_read_f)nai_stream_read,
    (nai_ssl_write_f)nai_stream_write,
    (nai_ssl_recvm_f)nai_stream_recvm,
    (nai_ssl_sendm_f)nai_stream_sendm,
    (nai_ssl_sendfile_f)nai_stream_sendfile,
    nai_ssl_iobase_get_socket,
    nullptr,
    nullptr,
};
#endif

}  // namespace doip
}  // namespace isoft

#endif  // ISOFT_DOIP_COMMON_PUBLIC_H_