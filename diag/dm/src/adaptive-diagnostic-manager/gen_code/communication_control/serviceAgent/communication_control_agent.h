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
/// @file       communication_control_agent.h
/// @brief      This file provides the definition of the CommunicationControlAgent interface class.
/// @details
/// @date       2024-12-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef __COMMUNICATIONCONTROLSERVICE_H_
#define __COMMUNICATIONCONTROLSERVICE_H_

#include <functional>
#include <future>
#include <memory>

#include "ara/core/future.h"
#include "communication_control_proxy.h"

namespace isoft {
namespace dm {
namespace dic {

class CommunicationControlAgent
{
public:
    explicit CommunicationControlAgent(uint16_t const& instanceId, uint32_t const& serviceInstanceId);

    ara::core::Future< void > CommCtrlRequest(ComCtrlRequestParamsType& controlType,
                                              MetaInfoMap& metaInfo,
                                              std::shared_ptr< CancellationEvent >& cancellationEventPtr);

private:
    std::unique_ptr< CommunicationControlProxy > proxy_{nullptr};
};

}  // namespace dic
}  // namespace dm
}  // namespace isoft

#endif  // __COMMUNICATIONCONTROLSERVICE_H_