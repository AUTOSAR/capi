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
/// @file       authentication_proxy.cpp
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "authentication_proxy.h"

#include <thread>

#include "ara/diag/diag_error_domain.h"

namespace isoft {
namespace dm {
namespace dic {

using isoft::dm::RetData;

constexpr uint8_t kFuncIDVerifyCertificateUnidirectional{1U};
constexpr uint8_t kFuncIDVerifyCertificateBidirectional{2U};
constexpr uint8_t kFuncIDVerifyOwnership{3U};
constexpr uint8_t kFuncIDTransmitCertificate{4U};

AuthenticationProxy::AuthenticationProxy(uint16_t const &instanceId, uint32_t const &serviceInstanceId)
    : BussinessClientProxy{serviceInstanceId, instanceId}
{
    // AccessClient::GetComProxy().RegisterNotificationCallBack(instanceId,
    //  std::move([this](FuncData const& funcData){Notify(funcData);}));
}

ara::core::Future< VerifyCertificateUnidirectionalResponse > AuthenticationProxy::VerifyCertificateUnidirectional(
    std::uint8_t communicationConfiguration,
    ara::core::Vector< std::uint8_t > clientCertificate,
    ara::core::Vector< std::uint8_t > clientChallenge,
    MetaInfoMap metaInfo,
    std::shared_ptr< CancellationEvent > &cancellationEventPtr)
{
    constexpr uint8_t kFuncId{kFuncIDVerifyCertificateUnidirectional};
    static uint8_t s_CallId{0U};
    std::uint8_t const communicationConfigurationU{static_cast< std::uint8_t >(communicationConfiguration)};
    ara::core::Vector< std::uint8_t > clientCertificateV{clientCertificate.begin(), clientCertificate.end()};
    ara::core::Vector< std::uint8_t > clientChallengeV{clientChallenge.begin(), clientChallenge.end()};
    ara::core::Future< VerifyCertificateUnidirectionalResponse > result{
        AsyncCallFunc< VerifyCertificateUnidirectionalResponse >(kFuncId, s_CallId, cancellationEventPtr,
                                                                 communicationConfigurationU, clientCertificateV,
                                                                 clientChallengeV, std::move(metaInfo))};

    return result;
}

ara::core::Future< VerifyCertificateBidirectionalResponse > AuthenticationProxy::VerifyCertificateBidirectional(
    std::uint8_t communicationConfiguration,
    ara::core::Vector< std::uint8_t > clientCertificate,
    ara::core::Vector< std::uint8_t > clientChallenge,
    MetaInfoMap metaInfo,
    std::shared_ptr< CancellationEvent > &cancellationEventPtr)
{
    constexpr uint8_t kFuncId{kFuncIDVerifyCertificateBidirectional};
    static uint8_t s_CallId{0U};

    ara::core::Future< VerifyCertificateBidirectionalResponse > result{
        AsyncCallFunc< VerifyCertificateBidirectionalResponse >(
            kFuncId, s_CallId, cancellationEventPtr, communicationConfiguration, std::move(clientCertificate),
            std::move(clientChallenge), std::move(metaInfo))};

    return result;
}

ara::core::Future< ara::core::Vector< std::uint8_t > > AuthenticationProxy::VerifyOwnership(
    ara::core::Vector< std::uint8_t > clientPOWN,
    ara::core::Vector< std::uint8_t > clientEphemeralPublicKey,
    MetaInfoMap metaInfo,
    std::shared_ptr< CancellationEvent > &cancellationEventPtr)
{
    constexpr uint8_t kFuncId{kFuncIDVerifyOwnership};
    static uint8_t s_CallId{0U};

    ara::core::Future< ara::core::Vector< std::uint8_t > > result{AsyncCallFunc< ara::core::Vector< std::uint8_t > >(
        kFuncId, s_CallId, cancellationEventPtr, std::move(clientPOWN), std::move(clientEphemeralPublicKey),
        std::move(metaInfo))};

    return result;
}

ara::core::Future< void > AuthenticationProxy::TransmitCertificate(
    ara::core::Vector< std::uint8_t > certificateData,
    MetaInfoMap metaInfo,
    std::shared_ptr< CancellationEvent > &cancellationEventPtr)
{
    constexpr uint8_t kFuncId{kFuncIDVerifyOwnership};
    static uint8_t s_CallId{0U};

    ara::core::Future< void > result{AsyncCallFunc< void >(kFuncId, s_CallId, cancellationEventPtr,
                                                           std::move(certificateData), std::move(metaInfo))};

    return result;
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft
