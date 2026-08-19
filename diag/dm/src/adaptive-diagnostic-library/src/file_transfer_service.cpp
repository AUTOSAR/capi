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
/// @file       file_transfer_service.cpp
/// @brief      This file provides the implementation of UploadService.
/// @details
/// @date       2023-08-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include <ara/core/instance_specifier.h>

#include "ara/diag/diag_error_domain.h"
#include "ara/diag/file_transfer.h"
#include "gen_code/file_transfer/apiAgent/file_transfer_agent.h"
#include "log.h"
#include "resolve.h"
#include "utility.h"

namespace ara {
namespace diag {

///  @brief Constructor for FileTransferService (inherited)
///  @param[in] instanceSpecifier InstanceSpecifier to an PortPrototype of an
///  DownloadServiceInterface
///  @param[in] reentrancyType Specifies if interface is callable fully- or
///  non-reentrant
///  @pre -
///  @reentrant FALSE
///  @traceid{SWS_DM_01325}@tracestatus{draft}
FileTransferService::FileTransferService(ara::core::InstanceSpecifier const &instanceSpecifier,  /// NOLINT
                                         ReentrancyType reentrancyType) noexcept
    : specifier_{instanceSpecifier}, reentrancyType_{reentrancyType}, skeleton_{}, vecMD5_{}
{
}

///  @brief This Offer will enable the DM to forward request messages to this
///  handler
///  @return Positive result if service was offered, error if offer failed
///  @error DiagLibErrc::kConfigurationMismatch if handle is an invalid handle
///  @error DiagLibErrc::kAlreadyOffered if service has been offered already
///  @pre             -
///  @reentrant       FALSE
///  @traceid{SWS_DM_01336}@tracestatus{draft}
auto FileTransferService::Offer() noexcept -> ara::core::Result< void >
{
    if (skeleton_ != nullptr) {
        return ara::core::Result< void >::FromError(DiagErrc::kAlreadyOffered);
    }
    ara::core::Result< internal::InstanceInfo > const retrieveResult{internal::Resolve(specifier_)};
    if (!retrieveResult.HasValue()) {
        return ara::core::Result< void >::FromError(retrieveResult.Error());
    }
    skeleton_ = std::make_shared< isoft::dm::dis::FileTransferServiceAgent >(
        std::move(retrieveResult).Value().serviceInstanceId);
    skeleton_->RegisterService(this);
    skeleton_->SetReentrancyType(reentrancyType_);
    return {};
}

///  @brief This StopOffer will disable the forwarding of request messages from
///  DM
///  @pre -
///  @reentrant  FALSE
///  @traceid{SWS_DM_01337}@tracestatus{draft}
void FileTransferService::StopOffer() noexcept
{
    if (skeleton_ != nullptr) {
        skeleton_.reset();
    }
}

/// @brief AA side to set the MD5 of the transmitted file and return it to the diagnostic management through all responses of FileTransferService, then return it to the tester side through UDS
/// @param vecMD5 MD5 to be set
void FileTransferService::SetMD5(ara::core::Vector< uint8_t > &&vecMD5) noexcept
{
    vecMD5_.clear();
    vecMD5_ = std::move(vecMD5);
    ara::core::String strMD5{vecMD5_.begin(), vecMD5_.end()};
    internal::LogDebug() << "FileTransferService::SetMD5|md5 =" << strMD5.c_str();
}

}  // namespace diag
}  // namespace ara
