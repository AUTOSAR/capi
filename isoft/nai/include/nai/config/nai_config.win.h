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
/// @file       nai_config.win.h
/// @brief      
/// @details
/// @date       2020-12-08
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#if defined(_WIN32)

    #if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_)                \
        && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_IX86)
        #define _X86_
        #if !defined(_CHPE_X86_ARM64_) && defined(_M_HYBRID)
            #define _CHPE_X86_ARM64_
        #endif
    #endif

    #if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_)                \
        && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_AMD64)
        #define _AMD64_
    #endif

    #if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_)                \
        && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_ARM)
        #define _ARM_
    #endif

    #if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_)                \
        && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_ARM64)
        #define _ARM64_
    #endif

    #if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_)                \
        && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_M68K)
        #define _68K_
    #endif

    #if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_)                \
        && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_MPPC)
        #define _MPPC_
    #endif

    #if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_M_IX86) && !defined(_AMD64_)               \
        && !defined(_ARM_) && !defined(_ARM64_) && defined(_M_IA64)
        #if !defined(_IA64_)
            #define _IA64_
        #endif /* !_IA64_ */
    #endif

    #ifndef _MAC
        #if defined(_68K_) || defined(_MPPC_)
            #define _MAC
        #endif
    #endif

#endif
