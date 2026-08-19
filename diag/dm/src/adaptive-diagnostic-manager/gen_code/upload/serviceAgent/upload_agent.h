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
/// @file       upload_agent.h
/// @brief      This file provides the definition of the CommunicationControlAgent interface class.
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef __SERVICEAGENT_UPLOADAGENT_H_
#define __SERVICEAGENT_UPLOADAGENT_H_

#include <functional>
#include <future>
#include <memory>

#include "ara/core/future.h"
#include "upload_proxy.h"

namespace isoft {
namespace dm {
namespace dic {

class UploadServiceAgent
{
public:
    explicit UploadServiceAgent(uint16_t const& instanceId, uint32_t const& serviceInstanceId);

    ara::core::Future< void > RequestUpload(std::uint8_t dataFormatIdentifier,
                                            std::uint8_t addressAndLengthFormatIdentifier,
                                            std::vector< std::uint8_t > memoryAddressAndSize,
                                            MetaInfoMap& metaInfo,
                                            std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept;

    ara::core::Future< std::vector< std::uint8_t > > UploadData(
        std::size_t numBytesToReturn,
        MetaInfoMap& metaInfo,
        std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept;

    ara::core::Future< std::vector< std::uint8_t > > RequestUploadExit(
        std::vector< std::uint8_t > transferRequestParameterRecord,
        MetaInfoMap& metaInfo,
        std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept;

private:
    std::unique_ptr< UploadServiceProxy > proxy_{nullptr};
};

}  // namespace dic
}  // namespace dm
}  // namespace isoft

#endif  // __SERVICEAGENT_UPLOADAGENT_H_