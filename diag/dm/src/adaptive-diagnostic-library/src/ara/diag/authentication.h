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

#ifndef ARA_DIAG_AUTHENTICATION_H_
#define ARA_DIAG_AUTHENTICATION_H_

#include <chrono>

#include "ara/core/future.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/diag/cancellation_handler.h"
#include "ara/diag/client_authentication_handle.h"
#include "ara/diag/meta_info.h"
#include "ara/diag/reentrancy.h"
/// @brief
namespace isoft {
namespace dm {
namespace dis {
/// @brief Declare AuthenticationAgent
class AuthenticationAgent;
}  // namespace dis
}  // namespace dm
}  // namespace isoft
namespace ara {
namespace diag {

/**
 * @brief Class to implement the Service Authentication interfaces to
 * application
 *
 * @artraceid{SWS_DM_01123}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
class Authentication
{
public:
    /**
   * @brief Constructor of Authentication
   *
   * @param[in] specifier InstanceSpecifier to a PortPrototype of a
   * DiagnosticAuthentication service instance in the manifest
   * @param[in] reentrancyType specifies if interface is callable fully- or
   * non-reentrant
   *
   * @artraceid{SWS_DM_01124}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    explicit Authentication(const ara::core::InstanceSpecifier &specifier, ReentrancyType reentrancyType);

    /**
   * @brief Move constructor of Authentication
   *
   * @artraceid{SWS_DM_01610}@artracestatus{draft}
   * @aruptrace{RS_AP_00147}
   */
    Authentication(Authentication &&) noexcept = delete;

    /**
   * @brief Authentication shall be a single not copy-able instance.
   *
   * @artraceid{SWS_DM_01609}@artracestatus{draft}
   * @aruptrace{RS_AP_00147}
   */
    Authentication(Authentication &) = delete;

    /**
   * @brief Move assignment operator of Authentication
   *
   * @artraceid{SWS_DM_01608}@artracestatus{draft}
   * @aruptrace{RS_AP_00147}
   */
    Authentication &operator=(Authentication &&) = delete;

    /**
   * @brief Authentication shall be a single not assignable instance.
   *
   * @artraceid{SWS_DM_01607}@artracestatus{draft}
   * @aruptrace{RS_AP_00147}
   */
    Authentication &operator=(Authentication &) = delete;

    /**
   * @brief Destructor of Authentication
   *
   * @artraceid{SWS_DM_01125}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    virtual ~Authentication() noexcept = default;

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
    virtual ara::core::Future< std::tuple< ara::core::Span< ara::core::Byte >, ara::core::Span< ara::core::Byte > > >
    VerifyCertificateUnidirectional(ara::core::Byte communicationConfiguration,
                                    ara::core::Span< ara::core::Byte > clientCertificate,
                                    ara::core::Span< ara::core::Byte > clientChallenge,
                                    const MetaInfo &metaInfo,
                                    CancellationHandler cancellationHandler)
        = 0;

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
    virtual ara::core::Future< std::tuple< ara::core::Span< ara::core::Byte >,
                                           ara::core::Span< ara::core::Byte >,
                                           ara::core::Span< ara::core::Byte >,
                                           ara::core::Span< ara::core::Byte > > >
    VerifyCertificateBidirectional(ara::core::Byte communicationConfiguration,
                                   ara::core::Span< ara::core::Byte > clientCertificate,
                                   ara::core::Span< ara::core::Byte > clientChallenge,
                                   const MetaInfo &metaInfo,
                                   CancellationHandler cancellationHandler)
        = 0;

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
    virtual ara::core::Future< ara::core::Span< ara::core::Byte > > VerifyOwnership(
        ara::core::Span< ara::core::Byte > clientPOWN,
        ara::core::Span< ara::core::Byte > clientEphemeralPublicKey,
        const MetaInfo &metaInfo,
        CancellationHandler cancellationHandler)
        = 0;

    /**
   * @brief This function accepts the certificate received from the tester,
   * verifies it, and takes necessary actions in the applications. Typical use
   * cases include updates to the access-rights provided to a tester that is
   * already authenticated.
   *
   * @returns Session Key Info or error
   * @param[in] certificateData Certificate to be verified, transmitted by the
   * tester
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
    virtual ara::core::Future< void > TransmitCertificate(ara::core::Span< ara::core::Byte > certificateData,
                                                          const MetaInfo &metaInfo,
                                                          CancellationHandler cancellationHandler)
        = 0;
    /**
   * @brief This Offer will enable the DM to forward request messages to this
   * handler
   *
   * @arerror DiagErrorDomain::DiagOfferErrc::kAlreadyOffered  This service was
   * already offered.
   *
   * @artraceid{SWS_DM_01130}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ara::core::Result< void > Offer();

    /**
   * @brief This StopOffer will disable the forwarding of request messages from
   * DM
   *
   * @artraceid{SWS_DM_01131}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    void StopOffer();

private:
    /// @name specifier_
    ara::core::InstanceSpecifier specifier_;
    /// @name reentrancyType_
    ReentrancyType reentrancyType_;

    std::shared_ptr< isoft::dm::dis::AuthenticationAgent >
        /// @name skeleton_
        skeleton_{};
};

}  // namespace diag
}  // namespace ara

#endif
