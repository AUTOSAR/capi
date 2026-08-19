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
/// @brief      This file provides authentication functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/authentication_management
/// @interface_level= module
/// @trace_id_sr=e3dfe6b5-22bc-44f4-a2e3-9b433759e4e3
/// @unit_name=authentication
/// @unit_description=This file provides authentication functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_AUTHENTICATION_H_
#define ISOFT_UDS_SERVER_AUTHENTICATION_H_
#include <functional>
#include <vector>

#include "isoft/uds/cancellation_handler.h"
#include "isoft/uds/common.h"
#include "isoft/uds/result.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief Class of authentication interface
///
/// @code{.isoft}
///
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
class AuthenticationInterface
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00988
    /// @needwork = dda
    /// @endcode
    AuthenticationInterface(AuthenticationInterface const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00989
    /// @needwork = dda
    /// @endcode
    AuthenticationInterface& operator=(AuthenticationInterface const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00990
    /// @needwork = dda
    /// @endcode
    AuthenticationInterface(AuthenticationInterface&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00991
    /// @needwork = dda
    /// @endcode
    AuthenticationInterface& operator=(AuthenticationInterface&&) = delete;

public:
    /// @brief Structure of verify certificate unidirectional response
    ///
    /// @code{.isoft}
    ///
    /// @interface_level=unit
    /// @needwork = ad
    ///
    /// @endcode
    struct VerifyCertificateUnidirectionalResponse
    {
        ///
        /// @brief Server challenge
        std::vector< std::uint8_t > challengeServer{};
        ///
        /// @brief Server ephemeral public key
        std::vector< std::uint8_t > ephemeralPublicKeyServer{};
    };
    /// @brief Structure of verify certificate bidirectional response
    ///
    /// @code{.isoft}
    ///
    /// @interface_level=unit
    /// @needwork = ad
    ///
    /// @endcode
    struct VerifyCertificateBidirectionalResponse
    {
        ///
        /// @brief Server challenge
        std::vector< std::uint8_t > challengeServer{};
        ///
        /// @brief Server certificate
        std::vector< std::uint8_t > certificateServer{};
        ///
        /// @brief Server proof of ownership
        std::vector< std::uint8_t > proofOfOwnershipServer{};
        ///
        /// @brief Server ephemeral public key
        std::vector< std::uint8_t > ephemeralPublicKeyServer{};
    };

    /// @brief Constructor
    /// @code{.isoft}
    ///
    /// @interface_level=unit
    /// @needwork = ad
    ///
    /// @endcode
    AuthenticationInterface() = default;
    /// @brief Destructor
    /// @code{.isoft}
    ///
    /// @interface_level=unit
    /// @needwork = ad
    ///
    /// @endcode
    virtual ~AuthenticationInterface() noexcept = default;

    /// @brief Receive certificate from test equipment, verify certificate, and generate challenge and ephemeral public key to return to test equipment
    /// @returns Challenge generated by application, server's ephemeral public key
    /// @param[in] communicationConfiguration According to ISO14229-1:2020 definition, this parameter provides information for subsequent diagnostic communication security processing after authentication
    /// @param[in] clientCertificate Certificate received from test equipment during unidirectional authentication
    /// @param[in] clientChallenge According to ISO14229-1:2020 definition, this parameter provides challenge received from test equipment during unidirectional authentication, this parameter depends on the communication configuration used
    /// @param[in] metaInfo Request meta information
    /// @param[in] cancellationHandler Parameter used for:
    /// - Query current cancellation status by calling cancellationHandler.IsCanceled() (returns true if current session is cancelled, otherwise false)
    /// - Register notification function by calling cancellationHandler.SetNotifier() (registered notification function is called when current session is cancelled)
    ///
    /// @returns Challenge generated by application, server's ephemeral public key
    /// @code{.isoft}
    /// @note Callback may be reentrantly called by requests from different clients
    ///
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidTimePeriod
    /// Server date and time do not match certificate validity period
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidSignature
    /// Unable to verify certificate signature
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidChainOfTrust
    /// Unable to verify certificate based on issuer's stored information
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidType
    /// Certificate does not match the use case of the current request
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidFormat
    /// Unable to evaluate certificate due to unmet format requirements
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidContent
    /// Unable to verify certificate due to content mismatch
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidScope
    /// Certificate scope does not match server content
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidCert
    /// Certificate received from client is invalid because server has revoked access for some reason
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kChallengeCalculationFailed
    /// Server unable to calculate challenge
    ///
    /// @interface_level=unit
    /// @needwork = ad
    ///
    /// @endcode
    virtual Result< VerifyCertificateUnidirectionalResponse > VerifyCertificateUnidirectional(
        std::uint8_t communicationConfiguration,
        std::vector< std::uint8_t > clientCertificate,
        std::vector< std::uint8_t > clientChallenge,
        const MetaInfoMap& metaInfo,
        CancellationHandler cancellationHandler)
        = 0;

    /// @brief Receive certificate and challenge from test equipment, verify certificate, and generate challenge, ephemeral public key and proof of ownership to return to test equipment. This function also returns server certificate for test equipment to verify proof of ownership
    /// @returns Challenge generated by application, server certificate, proof of ownership calculated by server, server's ephemeral public key
    /// @param[in] communicationConfiguration According to ISO14229-1:2020 definition, this parameter provides information for subsequent diagnostic communication security processing after authentication
    /// @param[in] clientCertificate Certificate received from test equipment during bidirectional authentication
    /// @param[in] clientChallenge According to ISO14229-1:2020 definition, this parameter provides challenge received from test equipment during bidirectional authentication
    /// @param[in] metaInfo Request meta information
    /// @param[in] cancellationHandler Parameter used for:
    /// - Query current cancellation status by calling cancellationHandler.IsCanceled() (returns true if current session is cancelled, otherwise false)
    /// - Register notification function by calling cancellationHandler.SetNotifier() (registered notification function is called when current session is cancelled)
    /// @code{.isoft}
    /// @note Callback may be reentrantly called by requests from different clients
    ///
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidTimePeriod
    /// Server date and time do not match certificate validity period
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidSignature
    /// Unable to verify certificate signature
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidChainOfTrust
    /// Unable to verify certificate based on issuer's stored information
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidType
    /// Certificate does not match the use case of the current request
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidFormat
    /// Unable to evaluate certificate due to unmet format requirements
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidContent
    /// Unable to verify certificate due to content mismatch
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidScope
    /// Certificate scope does not match server content
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidCert
    /// Certificate received from client is invalid because server has revoked access for some reason
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kChallengeCalculationFailed
    /// Server unable to calculate challenge
    ///
    ///
    /// @interface_level=unit
    /// @needwork = ad
    ///
    /// @endcode
    virtual Result< VerifyCertificateBidirectionalResponse > VerifyCertificateBidirectional(
        std::uint8_t communicationConfiguration,
        std::vector< std::uint8_t > clientCertificate,
        std::vector< std::uint8_t > clientChallenge,
        const MetaInfoMap& metaInfo,
        CancellationHandler cancellationHandler)
        = 0;

    /// @brief Receive proof of ownership from test equipment, verify using the public key of the certificate received in verifycertificateunidirectional/verifycertificatebidirectional, compare with server challenge created during last call to verifycertificateunidirectional/verifycertificatebidirectional
    /// @returns Session key information or error
    /// @param[in] clientPOWN Proof of ownership provided by test equipment for previously exchanged server challenge
    /// @param[in] clientEphemeralPublicKey According to ISO14229-1:2020 definition, this is the ephemeral public key generated by client for Diffie-Hellman key exchange
    /// @param[in] metaInfo Request meta information
    /// @param[in] cancellationHandler Parameter used for:
    /// - Query current cancellation status by calling cancellationHandler.IsCanceled() (returns true if current session is cancelled, otherwise false)
    /// - Register notification function by calling cancellationHandler.SetNotifier() (registered notification function is called when current session is cancelled)
    ///
    /// @code{.isoft}
    /// @note Callback may be reentrantly called by requests from different clients
    ///
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidTimePeriod
    /// Server date and time do not match certificate validity period
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidSignature
    /// Unable to verify certificate signature
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidChainOfTrust
    /// Unable to verify certificate based on issuer's stored information
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidType
    /// Certificate does not match the use case of the current request
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidFormat
    /// Unable to evaluate certificate due to unmet format requirements
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidContent
    /// Unable to verify certificate due to content mismatch
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidScope
    /// Certificate scope does not match server content
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidCert
    /// Certificate received from client is invalid because server has revoked access for some reason
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kChallengeCalculationFailed
    /// Server unable to calculate challenge
    ///
    /// @interface_level=unit
    /// @needwork = ad
    ///
    /// @endcode
    virtual Result< std::vector< std::uint8_t > > VerifyOwnership(std::vector< std::uint8_t > clientPOWN,
                                                                  std::vector< std::uint8_t > clientEphemeralPublicKey,
                                                                  const MetaInfoMap& metaInfo,
                                                                  CancellationHandler cancellationHandler)
        = 0;

    /// @brief Receive certificate from test equipment, verify certificate, and perform operations in the application. Typical use case includes updating access permissions of authenticated test equipment
    /// @returns No return value or error
    /// @param[in] certificateData Certificate to be verified transmitted by test equipment
    /// @param[in] metaInfo Request meta information
    /// @param[in] cancellationHandler Parameter used for:
    /// - Query current cancellation status by calling cancellationHandler.IsCanceled() (returns true if current session is cancelled, otherwise false)
    /// - Register notification function by calling cancellationHandler.SetNotifier() (registered notification function is called when current session is cancelled)
    //
    /// @code{.isoft}
    /// @note Callback may be reentrantly called by requests from different clients
    ///
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidTimePeriod
    /// Server date and time do not match certificate validity period
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidSignature
    /// Unable to verify certificate signature
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidChainOfTrust
    /// Unable to verify certificate based on issuer's stored information
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidType
    /// Certificate does not match the use case of the current request
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidFormat
    /// Unable to evaluate certificate due to unmet format requirements
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidContent
    /// Unable to verify certificate due to content mismatch
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidScope
    /// Certificate scope does not match server content
    /// @retval DiagUdsNrcErrorDomain::DiagUdsNrcErrc::kCertificateVerificationFailedInvalidCert
    /// Certificate received from client is invalid because server has revoked access for some reason
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    virtual Result< void > TransmitCertificate(std::vector< std::uint8_t > certificateData,
                                               const MetaInfoMap& metaInfo,
                                               CancellationHandler cancellationHandler)
        = 0;

private:
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  //ISOFT_UDS_SERVER_SERVICE_COMMUNICATIONCONTROL_H_