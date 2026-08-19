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
/// @file       download.h
/// @brief
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_DOWNLOAD_H_
#define ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_DOWNLOAD_H_
#include <isoft/uds/transfer_managment/download.h>

#include "cancellation/cancellation_event_manager.h"
#include "gen_code/download/serviceAgent/download_agent.h"
namespace ara {
namespace diag {
namespace dmd {

class Download : public isoft::uds::server::DownloadInterface
{
public:
    using MetaInfoMap         = isoft::uds::server::MetaInfoMap;
    using CancellationHandler = isoft::uds::server::CancellationHandler;

    /// @brief Default constructor
    Download(uint16_t const &instanceId, uint32_t const &serviceInstanceId);
    /// @brief Default destructor
    ~Download() override = default;
    /// @brief copy constructor
    /// @param other
    Download(Download const &other) noexcept = delete;
    /// @brief copy assignment operator
    /// @param other
    /// @return Download&
    Download &operator=(Download const &other) noexcept = delete;
    /// @brief move constructor
    /// @param other
    Download(Download &&other) noexcept = delete;
    /// @brief move assignment operator
    /// @param other
    /// @return Download&
    Download &operator=(Download &&other) noexcept = delete;  /// NOLINT

    isoft::uds::Result< void > RequestDownload(std::uint8_t dataFormatIdentifier,
                                               std::uint8_t addressAndLengthFormatIdentifier,
                                               std::vector< std::uint8_t > memoryAddressAndSize,
                                               MetaInfoMap &metaInfo,
                                               CancellationHandler cancellationHandler) noexcept override;

    isoft::uds::Result< void > DownloadData(std::uint8_t blockSequenceCounter,
                                            std::vector< std::uint8_t > transferRequestParameterRecord,
                                            MetaInfoMap &metaInfo,
                                            CancellationHandler cancellationHandler) noexcept override;

    isoft::uds::Result< std::vector< std::uint8_t > > RequestDownloadExit(
        std::vector< std::uint8_t > transferRequestParameterRecord,
        MetaInfoMap &metaInfo,
        CancellationHandler cancellationHandler) noexcept override;

private:
    std::unique_ptr< isoft::dm::dic::DownloadServiceAgent > agentPtr_;
    isoft::dm::CancellationEventManager cancellationEventManager_{};
};
}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  /// ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_DOWNLOAD_H_