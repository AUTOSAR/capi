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
/// @file       configure.h
/// @brief      PTP management class configuration
/// @details
/// @date       2023-01-17
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_PTP_CONFIG_H_
#define ARA_TSYNC_INTERNAL_PTP_CONFIG_H_

#include <cstdint>

#include "ara/core/string.h"
#include "ara/core/vector.h"
#include "ara/tsync/internal/TimeDomainId.h"
#include "ara/tsync/internal/ptp/network.h"
#include "ara/tsync/internal/ptp/protclparameter.h"
#include "isoft/define.h"
#include "isoft/manifestreader/manifest_node.h"
#include "isoft/manifestreader/manifest_reader.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {

/// @brief configuration class, the only configuration during PTP communication. The PTP layer only cares about the configuration here, not the TimeDomain configuration.
///          Data comes from two sources: one is from default configuration, the other is from TimeDomain configuration.
struct Configure final
{
public:
    /// @name domainPtp - domain protocol configuration
    ProtclParameter domainPtp{};

    /// @name debounceTime - minimum interval between two Sync transmissions
    double debounceTime{0};

    /// @name syncLossTimeout - Sync packet loss timeout
    /// TODO(zhoubo): magic number should be place a common file
    double syncLossTimeout{kTS_NUM_DOUBLE_3};

    ///////////////////// From TimeDomain and other extensions //////////////////////
    /// @name isMaster - flag indicating whether it is a Master; this flag is used in the following scenarios:
    //  1. When receiving a Pdelay packet on this time domain, check whether this machine is the Master:
    //           If yes, respond with PdelayResp/PdelayRespFollowup;
    //           If not, ignore.
    //  2. When it is detected that the local machine's time domain 0 is the Master, Pdelay is no longer sent.
    bool isMaster{false};

    //////////////////  The following configurations come from TimeDomain //////////////////
    /// @name isSystemWideGlobalTimeMaster - flag indicating whether it is the global time master (Gateways have many Masters, but none are GlobalMaster)
    bool isSystemWideGlobalTimeMaster{false};

    /// @name domainId - time domain ID
    internal::TimeDomainId domainId;

    /// @name fupDataIDList - DataID list used for security verification in Followup messages
    ara::core::Vector< std::uint8_t > fupDataIDList;

    /// @name immediateResumeTime - minimum time interval for immediate time (minimum interval between SetTime() sending messages and periodic sending)
    double immediateResumeTime{0};

    /// @name destAddr - destination address
    Network::Address destAddr;

    /// @name localAddr - local network card address
    Network::Address localAddr;

    /// @name etherconnector - network card IP address
    ara::core::String etherconnector;

    /// @name vlanPri vlan priority
    std::int8_t vlanPri{-1};

    /// @name vlanId vlan ID
    std::int16_t vlanId{-1};

};  /// class Configure

}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  /// ARA_TSYNC_INTERNAL_PTP_CONFIG_H_
