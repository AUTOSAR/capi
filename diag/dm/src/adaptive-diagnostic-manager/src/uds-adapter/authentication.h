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
/// @file       authentication.h
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_AUTHENTICATION_H_
#define ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_AUTHENTICATION_H_
#include <isoft/uds/authentication_management/authentication.h>

#include "cancellation/cancellation_event_manager.h"
#include "gen_code/authentication/serviceAgent/authentication_agent.h"
namespace ara {
namespace diag {
namespace dmd {

class Authentication : public isoft::uds::server::AuthenticationInterface
{
public:
    using MetaInfoMap             = isoft::uds::server::MetaInfoMap;
    using CancellationHandler     = isoft::uds::server::CancellationHandler;
    using AuthenticationInterface = isoft::uds::server::AuthenticationInterface;

    Authentication(uint16_t const &instanceId, uint32_t const &serviceInstanceId);
    /// @brief Default destructor
    ~Authentication() override = default;
    /// @brief copy constructor
    /// @param other
    Authentication(Authentication const &other) noexcept = delete;
    /// @brief copy assignment operator
    /// @param other
    /// @return Authentication&
    Authentication &operator=(Authentication const &other) noexcept = delete;
    /// @brief move constructor
    /// @param other
    Authentication(Authentication &&other) noexcept = delete;
    /// @brief move assignment operator
    /// @param other
    /// @return Authentication&
    Authentication &operator=(Authentication &&other) noexcept = delete;

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
   * @param[in] clientCertificate The certificate that is received from the
   * tester during Unidirectional Authentication.
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
    isoft::uds::Result< AuthenticationInterface::VerifyCertificateUnidirectionalResponse >
    VerifyCertificateUnidirectional(std::uint8_t communicationConfiguration,
                                    std::vector< std::uint8_t > clientCertificate,
                                    std::vector< std::uint8_t > clientChallenge,
                                    const MetaInfoMap &metaInfo,
                                    CancellationHandler cancellationHandler) override;

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
   * @param[in] clientCertificate The certificate that is received from the
   * tester during Bidirectional Authentication.
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
    isoft::uds::Result< AuthenticationInterface::VerifyCertificateBidirectionalResponse >
    VerifyCertificateBidirectional(std::uint8_t communicationConfiguration,
                                   std::vector< std::uint8_t > clientCertificate,
                                   std::vector< std::uint8_t > clientChallenge,
                                   const MetaInfoMap &metaInfo,
                                   CancellationHandler cancellationHandler) override;

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
   * @param[in] clientEphemeralPublicKey As defined in ISO14229-1:2020, this is
   * the Ephemeral public key generated by the client for Diffie-Hellman key
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
    isoft::uds::Result< std::vector< std::uint8_t > > VerifyOwnership(
        std::vector< std::uint8_t > clientPOWN,
        std::vector< std::uint8_t > clientEphemeralPublicKey,
        const MetaInfoMap &metaInfo,
        CancellationHandler cancellationHandler) override;

    /// @brief This function accepts the certificate received from the tester,
    /// verifies it, and takes necessary actions in the applications. Typical use
    /// cases include updates to the access-rights provided to a tester that is
    /// already authenticated
    /// @param certificateData Certificate to be verified, transmitted by the
    /// tester
    /// @param metaInfo MetaInfo of the request
    /// @param cancellationHandler This parameter used to -Query the current
    /// cancellation status by calling cancellationHandler.IsCanceled() (which
    /// returns true in case the current conversation has been cancelled and false
    /// otherwise), -Register a notifier function by calling
    /// cancellationHandler.Set Notifier() (the registered notifier is called if
    /// the current conversation is cancelled)..
    /// @return void
    isoft::uds::Result< void > TransmitCertificate(std::vector< std::uint8_t > certificateData,
                                                   const MetaInfoMap &metaInfo,
                                                   CancellationHandler cancellationHandler) override;

private:
    std::unique_ptr< isoft::dm::dic::AuthenticationAgent > agentPtr_;
    isoft::dm::CancellationEventManager cancellationEventManager_{};
};
}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  /// ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_AUTHENTICATION_H_