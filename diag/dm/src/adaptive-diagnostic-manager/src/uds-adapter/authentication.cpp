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
/// @file       authentication.cpp
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "authentication.h"

#include <isoft/uds/uds_nrc_error_domain.h>

#include "log/log.h"
#include "serialization/serialization/common_data_type.h"
namespace ara {
namespace diag {
namespace dmd {

Authentication::Authentication(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : agentPtr_{std::make_unique< isoft::dm::dic::AuthenticationAgent >(instanceId, serviceInstanceId)}
{
}

/**
   * @brief This function accepts the certificate received from the tester,
   * verifies it, and creates a challenge and ephemeral public key that must be
   * returned to the tester
   * @brief This callback may be called re-entrant to requests from different
   * clients
   *
   * @returns Challenge created by the application, Ephemeral Public Key of
   * Server
   * @param[in] communicationConfiguration As defined in ISO14229-1:2020, this
   * parameter provides information about how to proceed with security in
   * further diagnostic communication after the Authentication.
   * @param[in] clientCertificate The certificate that is received from the tester
   * during Unidirectional Authentication.
   * @param[in] clientChallenge As defined in ISO14229-1:2020, this parameter
   * provides the challenge received fom the tester during Unidirectional
   * Authentication. This parameter has a dependency on the
   * CommunicationConfiguration used
   * @param[in] metaInfo MetaInfo of the request.
   * @param[in] cancellationHandler This parameter used to
   * -Query the current cancellation status by calling
   * cancellationHandler.IsCanceled() (which returns true in case the current
   * conversation has been cancelled and false otherwise), -Register a notifier
   * function by calling cancellationHandler.SetNotifier() (the registered
   * notifier is called if the current conversation is cancelled)..
   *
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidTimePeriod
   * Date and time of the server does not match the validity period of the
   * Certificate.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidSignature
   * Signature of the Certificate could not be verified.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidChainOfTrust
   * Certificate could not be verified against stored information about the
   * issuing authority.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidType
   * Certificate does not match the current requested use case.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidFormat
   * Certificate could not be evaluated because the format requirement has not
   * been met.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidContent
   * Certificate could not be verified because the content does not match.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidScope
   * The scope of the Certificate does not match the contents of the server.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidCert
   * Certificate received from client is invalid, because the server has revoked
   * access for some reason.
   * @arerror DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kChallengeCalculationFailed
   * The challenge could not be calculated on the server side.
   *
   * @arthreadsafety{reentrant}
   * @artraceid{SWS_DM_01126}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   * @aruptrace{RS_AP_00128}
   */
isoft::uds::Result< isoft::uds::server::AuthenticationInterface::VerifyCertificateUnidirectionalResponse >
Authentication::VerifyCertificateUnidirectional(std::uint8_t communicationConfiguration,
                                                std::vector< std::uint8_t > clientCertificate,
                                                std::vector< std::uint8_t > clientChallenge,
                                                const MetaInfoMap& metaInfo,
                                                CancellationHandler cancellationHandler)
{
    isoft::dm::MetaInfoMap comMetaInfo{};
    for (auto&& pair : metaInfo) {
        ara::core::String key{pair.first};
        ara::core::String value{pair.second};
        comMetaInfo.insert({key, value});
    }
    std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr{
        std::make_shared< isoft::dm::CancellationEvent >()};
    cancellationHandler.SetNotifier([&cancellationEventPtr]() { cancellationEventPtr->Notifier(); });

    ara::core::Future< isoft::dm::VerifyCertificateUnidirectionalResponse > requestResult
        = agentPtr_->VerifyCertificateUnidirectional(communicationConfiguration, clientCertificate, clientChallenge,
                                                     comMetaInfo, cancellationEventPtr);
    ara::core::Result< isoft::dm::VerifyCertificateUnidirectionalResponse > result{requestResult.GetResult()};
    if (result.HasValue()) {
        AuthenticationInterface::VerifyCertificateUnidirectionalResponse rsp;
        rsp.challengeServer          = result.Value().challengeServer;
        rsp.ephemeralPublicKeyServer = result.Value().ephemeralPublicKeyServer;
        return isoft::uds::Result< AuthenticationInterface::VerifyCertificateUnidirectionalResponse >::FromValue(rsp);
    }
    if (result.Error().Value() < 0) {
        ara::diag::common::LogWarn() << "Authentication::VerifyCertificateUnidirectional| send msg fails!";
        return isoft::uds::Result< AuthenticationInterface::VerifyCertificateUnidirectionalResponse >::FromError(
            static_cast< int32_t >(isoft::uds::server::NrcErrc::kGeneralReject));
    }
    return isoft::uds::Result< AuthenticationInterface::VerifyCertificateUnidirectionalResponse >::FromError(
        result.Error().Value());
}

/**
   * @brief This function accepts the certificate and challenge received from
   * the tester, verifies the certificate, and creates a challenge, Ephemeral
   * Public Key  and Proof Of Ownership that must be returned to the tester. The
   * function also returns the server certificate that will be used by the
   * tester to verify the Proof Of Ownership
   * @brief This callback may be called re-entrant to requests from different
   * clients
   *
   * @returns Challenge created by the application, Certificate of the server,
   * ProofOfOwnership calculated by the server, Ephemeral Public Key of server
   * @param[in] communicationConfiguration As defined in ISO14229-1:2020, this
   * parameter provides information about how to proceed with security in
   * further diagnostic communication after the Authentication.
   * @param[in] clientCertificate The certificate that is received from the tester
   * during Bidirectional Authentication.
   * @param[in] clientChallenge As defined in ISO14229-1:2020, this parameter
   * provides the challenge received from the tester during Bidirectional
   * Authentication.
   * @param[in] metaInfo MetaInfo of the request.
   * @param[in] cancellationHandler This parameter used to
   * -Query the current cancellation status by calling
   * cancellationHandler.IsCanceled() (which returns true in case the current
   * conversation has been cancelled and false otherwise), -Register a notifier
   * function by calling cancellationHandler.SetNotifier() (the registered
   * notifier is called if the current conversation is cancelled)..
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidTimePeriod
   * Date and time of the server does not match the validity period of the
   * Certificate.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidSignature
   * Signature of the Certificate could not be verified.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidChainOfTrust
   * Certificate could not be verified against stored information about the
   * issuing authority.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidType
   * Certificate does not match the current requested use case.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidFormat
   * Certificate could not be evaluated because the format requirement has not
   * been met.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidContent
   * Certificate could not be verified because the content does not match.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidScope
   * The scope of the Certificate does not match the contents of the server.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidCert
   * Certificate received from client is invalid, because the server has revoked
   * access for some reason.
   * @arerror DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kChallengeCalculationFailed
   * The challenge could not be calculated on the server side.
   *
   * @arthreadsafety{reentrant}
   * @artraceid{SWS_DM_01127}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   * @aruptrace{RS_AP_00128}
   */
isoft::uds::Result< isoft::uds::server::AuthenticationInterface::VerifyCertificateBidirectionalResponse >
Authentication::VerifyCertificateBidirectional(std::uint8_t communicationConfiguration,
                                               std::vector< std::uint8_t > clientCertificate,
                                               std::vector< std::uint8_t > clientChallenge,
                                               const MetaInfoMap& metaInfo,
                                               CancellationHandler cancellationHandler)
{
    isoft::dm::MetaInfoMap comMetaInfo{};
    for (auto&& pair : metaInfo) {
        ara::core::String key{pair.first};
        ara::core::String value{pair.second};
        comMetaInfo.insert({key, value});
    }
    std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr{
        std::make_shared< isoft::dm::CancellationEvent >()};
    cancellationHandler.SetNotifier([&cancellationEventPtr]() { cancellationEventPtr->Notifier(); });

    ara::core::Future< isoft::dm::VerifyCertificateBidirectionalResponse > requestResult
        = agentPtr_->VerifyCertificateBidirectional(communicationConfiguration, clientCertificate, clientChallenge,
                                                    comMetaInfo, cancellationEventPtr);
    ara::core::Result< isoft::dm::VerifyCertificateBidirectionalResponse > result{requestResult.GetResult()};
    if (result.HasValue()) {
        AuthenticationInterface::VerifyCertificateBidirectionalResponse rsp{};
        rsp.challengeServer          = result.Value().challengeServer;
        rsp.certificateServer        = result.Value().certificateServer;
        rsp.proofOfOwnershipServer   = result.Value().proofOfOwnershipServer;
        rsp.ephemeralPublicKeyServer = result.Value().ephemeralPublicKeyServer;
        return isoft::uds::Result< AuthenticationInterface::VerifyCertificateBidirectionalResponse >::FromValue(rsp);
    }
    if (result.Error().Value() < 0) {
        ara::diag::common::LogWarn() << "Authentication::VerifyCertificateBidirectional| send msg fails!";
        return isoft::uds::Result< AuthenticationInterface::VerifyCertificateBidirectionalResponse >::FromError(
            static_cast< int32_t >(isoft::uds::server::DiagErrc::kGenericError));
    }
    return isoft::uds::Result< AuthenticationInterface::VerifyCertificateBidirectionalResponse >::FromError(
        result.Error().Value());
}

/**
   * @brief This function accepts the Proof Of Ownership received from the
   * tester and verifies it with the Public Key of the certificate received in
   * the verifycertificateunidirectional/ verifycertificatebidirectional against
   * the server challenge created in the last call to
   * verifycertificateunidirectional/ verifycertificatebidirectional
   *
   * @returns Session Key Info or error
   * @param[in] clientPOWN The Proof Of Ownership provided by the Tester to the
   * previously exchanged Server Challenge
   * @param[in] clientEphemeralPublicKey As defined in ISO14229-1:2020, this is the
   * Ephemeral public key generated by the client for Diffie-Hellman key
   * agreement.
   * @param[in] metaInfo MetaInfo of the request.
   * @param[in] cancellationHandler This parameter used to
   * -Query the current cancellation status by calling
   * cancellationHandler.IsCanceled() (which returns true in case the current
   * conversation has been cancelled and false otherwise), -Register a notifier
   * function by calling cancellationHandler.SetNotifier() (the registered
   * notifier is called if the current conversation is cancelled)..
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidTimePeriod
   * Date and time of the server does not match the validity period of the
   * Certificate.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidSignature
   * Signature of the Certificate could not be verified.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidChainOfTrust
   * Certificate could not be verified against stored information about the
   * issuing authority.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidType
   * Certificate does not match the current requested use case.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidFormat
   * Certificate could not be evaluated because the format requirement has not
   * been met.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidContent
   * Certificate could not be verified because the content does not match.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidScope
   * The scope of the Certificate does not match the contents of the server.
   * @arerror
   * DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidCert
   * Certificate received from client is invalid, because the server has revoked
   * access for some reason.
   * @arerror DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kChallengeCalculationFailed
   * The challenge could not be calculated on the server side.
   *
   * @artraceid{SWS_DM_01128}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   * @aruptrace{RS_AP_00128}
   */
isoft::uds::Result< std::vector< std::uint8_t > > Authentication::VerifyOwnership(
    std::vector< std::uint8_t > clientPOWN,
    std::vector< std::uint8_t > clientEphemeralPublicKey,
    const MetaInfoMap& metaInfo,
    CancellationHandler cancellationHandler)
{
    isoft::dm::MetaInfoMap comMetaInfo{};
    for (auto&& pair : metaInfo) {
        ara::core::String key{pair.first};
        ara::core::String value{pair.second};
        comMetaInfo.insert({key, value});
    }
    std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr{
        std::make_shared< isoft::dm::CancellationEvent >()};
    cancellationHandler.SetNotifier([&cancellationEventPtr]() { cancellationEventPtr->Notifier(); });

    ara::core::Future< ara::core::Vector< std::uint8_t > > requestResult
        = agentPtr_->VerifyOwnership(clientPOWN, clientEphemeralPublicKey, comMetaInfo, cancellationEventPtr);
    ara::core::Result< ara::core::Vector< std::uint8_t > > result{requestResult.GetResult()};
    if (result.HasValue()) {
        return isoft::uds::Result< std::vector< std::uint8_t > >::FromValue(result.Value());
    }
    if (result.Error().Value() < 0) {
        ara::diag::common::LogWarn() << "Authentication::VerifyOwnership| send msg fails!";
        return isoft::uds::Result< std::vector< std::uint8_t > >::FromError(
            static_cast< int32_t >(isoft::uds::server::DiagErrc::kGenericError));
    }
    return isoft::uds::Result< std::vector< std::uint8_t > >::FromError(result.Error().Value());
}

/// @brief This function accepts the certificate received from the tester, verifies it, and takes necessary
/// actions in the applications. Typical use cases include updates to the access-rights provided to a
/// tester that is already authenticated
/// @param certificateData Certificate to be verified, transmitted by the tester
/// @param metaInfo MetaInfo of the request
/// @param cancellationHandler This parameter used to -Query the current cancellation status by
/// calling cancellationHandler.IsCanceled() (which returns true in case
/// the current conversation has been cancelled and false otherwise),
/// -Register a notifier function by calling cancellationHandler.Set
/// Notifier() (the registered notifier is called if the current conversation
/// is cancelled)..
/// @return void
isoft::uds::Result< void > Authentication::TransmitCertificate(std::vector< std::uint8_t > certificateData,
                                                               const MetaInfoMap& metaInfo,
                                                               CancellationHandler cancellationHandler)
{
    isoft::dm::MetaInfoMap comMetaInfo{};
    for (auto&& pair : metaInfo) {
        ara::core::String key{pair.first};
        ara::core::String value{pair.second};
        comMetaInfo.insert({key, value});
    }
    std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr{
        std::make_shared< isoft::dm::CancellationEvent >()};
    cancellationHandler.SetNotifier([&cancellationEventPtr]() { cancellationEventPtr->Notifier(); });
    ara::core::Future< void > requestResult
        = agentPtr_->TransmitCertificate(certificateData, comMetaInfo, cancellationEventPtr);
    ara::core::Result< void > result{requestResult.GetResult()};
    if (result.HasValue()) {
        return {};
    }
    if (result.Error().Value() < 0) {
        ara::diag::common::LogWarn() << "Authentication::TransmitCertificate| send msg fails!";
        return isoft::uds::Result< void >::FromError(
            static_cast< int32_t >(isoft::uds::server::DiagErrc::kGenericError));
    }
    return isoft::uds::Result< void >::FromError(result.Error().Value());
}
}  // namespace dmd
}  // namespace diag
}  // namespace ara
