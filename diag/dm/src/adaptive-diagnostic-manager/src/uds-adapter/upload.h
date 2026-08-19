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
/// @file       upload.h
/// @brief
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_UPLOAD_H_
#define ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_UPLOAD_H_
#include <isoft/uds/transfer_managment/upload.h>

#include "cancellation/cancellation_event_manager.h"
#include "gen_code/upload/serviceAgent/upload_agent.h"
namespace ara {
namespace diag {
namespace dmd {

class Upload : public isoft::uds::server::UploadInterface
{
public:
    using MetaInfoMap         = isoft::uds::server::MetaInfoMap;
    using CancellationHandler = isoft::uds::server::CancellationHandler;

    /// @brief Default constructor
    /// @param instanceId
    /// @param serviceInstanceId
    Upload(uint16_t const &instanceId, uint32_t const &serviceInstanceId);
    /// @brief Default destructor
    ~Upload() override = default;
    /// @brief  Copy constructor
    /// @param other
    Upload(Upload const &other) noexcept = delete;
    /// @brief Copy assignment operator
    /// @param other
    /// @return
    Upload &operator=(Upload const &other) noexcept = delete;
    /// @brief  Move constructor
    /// @param other
    Upload(Upload &&other) noexcept = delete;
    /// @brief Move assignment operator
    /// @param other
    /// @return
    Upload &operator=(Upload &&other) noexcept = delete;

    isoft::uds::Result< void > RequestUpload(std::uint8_t dataFormatIdentifier,
                                             std::uint8_t addressAndLengthFormatIdentifier,
                                             std::vector< std::uint8_t > memoryAddressAndSize,
                                             MetaInfoMap &metaInfo,
                                             CancellationHandler cancellationHandler) noexcept override;

    isoft::uds::Result< std::vector< std::uint8_t > > UploadData(
        std::uint8_t blockSequenceCounter,
        std::size_t numBytesToReturn,
        MetaInfoMap &metaInfo,
        CancellationHandler cancellationHandler) noexcept override;

    isoft::uds::Result< std::vector< std::uint8_t > > RequestUploadExit(
        std::vector< std::uint8_t > transferRequestParameterRecord,
        MetaInfoMap &metaInfo,
        CancellationHandler cancellationHandler) noexcept override;

private:
    std::unique_ptr< isoft::dm::dic::UploadServiceAgent > agentPtr_;
    isoft::dm::CancellationEventManager cancellationEventManager_{};
};
}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  /// ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_UPLOAD_H_