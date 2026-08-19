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
/// @file       nsomeip_config.h
/// @brief      This file provides the Diagnostic Management class
/// @details
/// @date       2024-12-31
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_DIAG_NSOMEIP_CONFIG_H_
#define _ARA_DIAG_NSOMEIP_CONFIG_H_

namespace ara {
namespace diag {

char szSrvSNsiCfgJson[]  //NOLINT
    = "{\
    \"netname\": \"channelForRpc\",\
    \"host\": \"%s\",\
    \"mode\": \"route\",\
    \"application\": [\
        {\
            \"name\": \"access_server\",\
            \"client\": \"%d\"\
        }\
    ],\
    \"service\": [\
        {\
            \"service\": \"1\",\
            \"instance\": \"1\",\
            \"major\": \"1\",\
            \"minor\": \"1\",\
            \"multicast\": \"%s:%d\",\
            \"reliable\": \"%d\",\
            \"unreliable\": \"%d\",\
            \"reliability\": \"both\",\
            \"event\": [\
                {\
                    \"event\": \"1\",\
                    \"type\": \"event\",\
                    \"reliability\": \"unreliable\"\
                }\
            ],\
            \"group\": [\
                {\
                    \"group\": \"%d\",\
                    \"threshold\": \"10\",\
                    \"multicast\": \"%s:%d\"\
                }\
            ]\
        }\
    ],\
    \"service_discovery\": {\
        \"multicast\": \"%s\",\
        \"unicast\": \"%d\"\
    }\
}";

char szCltSNsiCfgJson[]  //NOLINT
    = "{\
    \"netname\": \"channelForRpc\",\
    \"host\": \"0.0.0.0\",\
    \"mode\": \"proxy\",\
    \"application\": [\
        {\
            \"name\": \"access_%d\",\
            \"client\": \"%d\"\
        }\
    ],\
    \"service\": [],\
    \"service_discovery\": {\
        \"multicast\": \"%s\",\
        \"unicast\": \"%d\"\
    }\
}";

}  // namespace diag
}  // namespace ara

#endif  // _ARA_DIAG_NSOMEIP_CONFIG_H_