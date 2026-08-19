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
/// @file       service_x29.cpp
/// @brief      This file implements service_x29 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=authentication_management
/// @trace_id_sr=d6f4612e-af8e-452c-a69f-ccf71d1b967f
/// @unit_name=service_x29.h
/// @unit_description=This file provides service_x29 functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_x29.h"

#include <functional>

#include "common/log.h"
#include "common/serialize.h"
#include "isoft/uds/type_int.h"
#include "isoft/uds/uds_nrc_error_domain.h"

namespace isoft {
namespace uds {
namespace server {



std::int32_t ServiceX29::Initialize(AuthTransmitCertificate const &transmitCertificate,
                                    std::shared_ptr< AuthenticationInterface > &interfacePtr,
                                    std::shared_ptr< AuthenticationManager > manager) noexcept
{
    if (interfacePtr.get() == nullptr) {
        LogWarn() << "ServiceX29::Initialize|Please Init Authentication Interface!";
        return -1;
    }
    interfacePtr_        = interfacePtr;
    transmitCertificate_ = transmitCertificate;
    manager_             = std::move(manager);
    return 0;
}

void ServiceX29::Process(std::shared_ptr< RequestContext > context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX29::Process| request context is nullptr!";
        return;
    }
    MessagePtr requestPtr = context->GetRequest();
    if (requestPtr.get() == nullptr) {
        LogWarn() << "ServiceX29::Process| request message is nullptr!";
        return;
    }
    std::vector< std::uint8_t > &body{requestPtr->GetBody()};
    std::size_t const minSizeOfBody{2U};
    if (body.size() < minSizeOfBody) {
        LogWarn() << "ServiceX29::Process| request size must >= 2 byte!";
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    Serialize serialize{body};
    std::ignore = serialize.ReadUint8();
    std::uint8_t const subfunction{serialize.ReadUint8()};
    bool subfunctionSupport{false};
    subfunctionSupport = _IsSupport(subfunction);
    if (!subfunctionSupport) {
        LogWarn() << "ServiceX29::Process| current service not support "
                     "subfunction! subfunction id:"
                  << subfunction;
        context->Respond(NrcErrc::kSubfunctionNotSupported);
        return;
    }
    LogVerbose() << "ServiceX29::Process| start subfunction process!";
    switch (subfunction) {
        case kDeAuthenticate:
            _DeAuthenticate(context);
            break;
        case kVerifyCertificateUnidirectional:
            _VerifyCertificateUnidirectional(context);
            break;
        case kVerifyCertificateBidirectional:
            _VerifyCertificateBidirectional(context);
            break;
        case kProofOfOwnership:
            _ProofOfOwnership(context);
            break;
        case kTransmitCertificate:
            _TransmitCertificate(context);
            break;
        case kAuthenticationConfiguration:
            _AuthenticationConfiguration(context);
            break;
        default:
            LogWarn() << "ServiceX29::Process| invalid subfunction! id:" << subfunction;
            break;
    }
    LogVerbose() << "ServiceX29::Process| end subfunction process!";
}

bool ServiceX29::_IsSupport(std::uint8_t subfunction) const noexcept
{
    bool result{false};
    switch (subfunction) {
        case kDeAuthenticate:
        case kVerifyCertificateUnidirectional:
        case kVerifyCertificateBidirectional:
        case kProofOfOwnership:
        case kAuthenticationConfiguration:
            result = true;
            break;
        case kTransmitCertificate:
            if (!transmitCertificate_.certificateEvaluation.empty()) {
                result = true;
            }
        default:
            break;
    }
    return result;
}

void ServiceX29::Confirm(RequestContext &, ResponseStatusType) noexcept
{
    LogInfo() << "ServiceX29::Confirm| send response finish!";
}

void ServiceX29::_DeAuthenticate(std::shared_ptr< RequestContext > &context) noexcept
{
    MessagePtr requestPtr{context->GetRequest()};
    if (requestPtr.get() == nullptr) {
        LogWarn() << "ServiceX29::_DeAuthenticate|request message is nulptr!";
        return;
    }
    std::vector< std::uint8_t > &body{requestPtr->GetBody()};
    std::size_t const minSizeOfBody{2U};
    if (body.size() != minSizeOfBody) {
        LogWarn() << "ServiceX29::_DeAuthenticate| deauthenticate request size must == 2 byte!";
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    Address const clientAddress{requestPtr->GetSA()};
    Result< ClientAuthenticationPtr > resultClientAuthPtr{manager_->GetClientAuthentication(clientAddress)};
    if (resultClientAuthPtr.HasValue()) {
        ClientAuthenticationPtr clientAuthPtr{resultClientAuthPtr.Value()};
        if (clientAuthPtr.get() != nullptr) {
            clientAuthPtr->Default();
        }
    } else {
        LogWarn() << "ServiceX29::_DeAuthenticate|get client fails!";
    }

    MessagePtr responsePtr{context->CreateResponse()};
    Serialize serialize{responsePtr->GetBody()};
    std::uint8_t const subfunction{0x00U};
    std::uint8_t const authenticationReturnParameter{0x10U};
    serialize.Write(subfunction);
    serialize.Write(authenticationReturnParameter);
    context->Respond(std::move(responsePtr));
    return;
}

void ServiceX29::_VerifyCertificateUnidirectional(std::shared_ptr< RequestContext > &context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX29::_VerifyCertificateUnidirectional| request "
                     "context is nullptr!";
        return;
    }
    MessagePtr requestPtr = context->GetRequest();
    if (requestPtr.get() == nullptr) {
        LogWarn() << "ServiceX29::_VerifyCertificateUnidirectional| request "
                     "message is nullptr!";
        return;
    }
    std::vector< std::uint8_t > &body{requestPtr->GetBody()};
    std::size_t const minSize{7U};
    std::size_t totalLength{minSize};
    if (body.size() < minSize) {
        LogWarn() << "ServiceX29::_VerifyCertificateUnidirectional| request "
                     "message size too short!body size:"
                  << body.size() << " totalLength:" << totalLength;
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    Serialize deserialize{body};
    std::ignore                             = deserialize.ReadUint8();
    std::uint8_t subfunction                = deserialize.ReadUint8();
    std::uint8_t communicationConfiguration = deserialize.ReadUint8();
    std::uint16_t lengthOfCertificateClient = deserialize.ReadUint16();
    totalLength += lengthOfCertificateClient;
    if (body.size() < totalLength) {
        LogWarn() << "ServiceX29::_VerifyCertificateUnidirectional| request "
                     "message size too short!body size:"
                  << body.size() << " totalLength:" << totalLength
                  << " lengthOfCertificateClient:" << lengthOfCertificateClient;
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    std::vector< std::uint8_t > certificateClient
        = deserialize.ReadVector(static_cast< std::size_t >(lengthOfCertificateClient));
    std::uint16_t lengthOfChallengeClient = deserialize.ReadUint16();
    totalLength += lengthOfChallengeClient;
    if (body.size() != totalLength) {
        LogWarn() << "ServiceX29::_VerifyCertificateUnidirectional| request "
                     "message size too short!body size:"
                  << body.size() << " totalLength:" << totalLength;
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    std::vector< std::uint8_t > challengeClient
        = deserialize.ReadVector(static_cast< std::size_t >(lengthOfChallengeClient));
    if (interfacePtr_.get() != nullptr) {
        MetaInfoMap metaInfo{context->GetMetaInfoMap()};
        Result< AuthenticationInterface::VerifyCertificateUnidirectionalResponse > result
            = interfacePtr_->VerifyCertificateUnidirectional(communicationConfiguration, certificateClient,
                                                             challengeClient, metaInfo,
                                                             CancellationHandler(context->GetCancellationState()));
        if (result.HasValue()) {
            AuthenticationInterface::VerifyCertificateUnidirectionalResponse responseContent{result.Value()};
            MessagePtr responsePtr{context->CreateResponse()};
            Serialize serialize{responsePtr->GetBody()};
            serialize.Write(subfunction);
            std::uint8_t const authenticationReturnParameter{0x11U};
            serialize.Write(authenticationReturnParameter);
            std::uint16_t const lengthOfChallengeServer{
                static_cast< std::uint16_t >(responseContent.challengeServer.size())};
            serialize.Write(lengthOfChallengeServer);
            serialize.Write(responseContent.challengeServer);
            std::uint16_t const lengthOfEphemeralPublicKeyServer{
                static_cast< std::uint16_t >(responseContent.ephemeralPublicKeyServer.size())};
            serialize.Write(lengthOfEphemeralPublicKeyServer);
            serialize.Write(responseContent.ephemeralPublicKeyServer);
            context->Respond(std::move(responsePtr));
            if (manager_.get() != nullptr) {
                manager_->SetVerifyCertificateState(requestPtr->GetSA(), true);
            }
        } else {
            LogWarn() << "ServiceX29::_VerifyCertificateUnidirectional|interface "
                         "result error! error:"
                      << result.Error();
            std::uint8_t const nrc{static_cast< std::uint8_t >(result.Error())};
            context->Respond(nrc);
        }
    } else {
        LogWarn() << "ServiceX29::_VerifyCertificateUnidirectional|interface not config!";
        context->Respond(NrcErrc::kGeneralReject);
    }
}

void ServiceX29::_VerifyCertificateBidirectional(std::shared_ptr< RequestContext > &context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX29::_VerifyCertificateBidirectional| request context "
                     "is nullptr!";
        return;
    }
    MessagePtr requestPtr = context->GetRequest();
    if (requestPtr.get() == nullptr) {
        LogWarn() << "ServiceX29::_VerifyCertificateBidirectional| request message "
                     "is nullptr!";
        return;
    }
    std::vector< std::uint8_t > &body{requestPtr->GetBody()};
    std::size_t const minSize{7U};
    std::size_t totalLength{minSize};
    if (body.size() < minSize) {
        LogWarn() << "ServiceX29::_VerifyCertificateBidirectional| request message "
                     "size too short!";
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    Serialize deserialize{body};
    std::ignore                             = deserialize.ReadUint8();
    std::uint8_t subfunction                = deserialize.ReadUint8();
    std::uint8_t communicationConfiguration = deserialize.ReadUint8();
    std::uint16_t lengthOfCertificateClient = deserialize.ReadUint16();
    totalLength += lengthOfCertificateClient;
    if (body.size() < totalLength) {
        LogWarn() << "ServiceX29::_VerifyCertificateBidirectional| request message "
                     "size too short!";
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    std::vector< std::uint8_t > certificateClient
        = deserialize.ReadVector(static_cast< std::size_t >(lengthOfCertificateClient));
    std::uint16_t lengthOfChallengeClient = deserialize.ReadUint16();
    totalLength += lengthOfChallengeClient;
    if (body.size() != totalLength) {
        LogWarn() << "ServiceX29::_VerifyCertificateBidirectional| request message "
                     "size too short!";
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    std::vector< std::uint8_t > challengeClient
        = deserialize.ReadVector(static_cast< std::size_t >(lengthOfChallengeClient));
    if (interfacePtr_.get() != nullptr) {
        MetaInfoMap metaInfo{context->GetMetaInfoMap()};
        Result< AuthenticationInterface::VerifyCertificateBidirectionalResponse > result
            = interfacePtr_->VerifyCertificateBidirectional(communicationConfiguration, certificateClient,
                                                            challengeClient, metaInfo,
                                                            CancellationHandler(context->GetCancellationState()));
        if (result.HasValue()) {
            AuthenticationInterface::VerifyCertificateBidirectionalResponse responseContent{result.Value()};
            MessagePtr responsePtr{context->CreateResponse()};
            Serialize serialize{responsePtr->GetBody()};
            serialize.Write(subfunction);
            std::uint8_t const authenticationReturnParameter{0x11U};
            serialize.Write(authenticationReturnParameter);
            std::uint16_t const lengthOfChallengeServer{
                static_cast< std::uint16_t >(responseContent.challengeServer.size())};
            serialize.Write(lengthOfChallengeServer);
            serialize.Write(responseContent.challengeServer);
            std::uint16_t const lengthOfCertificateServer{
                static_cast< std::uint16_t >(responseContent.certificateServer.size())};
            serialize.Write(lengthOfCertificateServer);
            serialize.Write(responseContent.certificateServer);
            std::uint16_t const lengthOfProofOfOwnershipServer{
                static_cast< std::uint16_t >(responseContent.proofOfOwnershipServer.size())};
            serialize.Write(lengthOfProofOfOwnershipServer);
            serialize.Write(responseContent.proofOfOwnershipServer);
            std::uint16_t const lengthOfEphemeralPublicKeyServer{
                static_cast< std::uint16_t >(responseContent.ephemeralPublicKeyServer.size())};
            serialize.Write(lengthOfEphemeralPublicKeyServer);
            serialize.Write(responseContent.ephemeralPublicKeyServer);
            context->Respond(std::move(responsePtr));
            if (manager_.get() != nullptr) {
                manager_->SetVerifyCertificateState(requestPtr->GetSA(), true);
            }
        } else {
            LogWarn() << "ServiceX29::_VerifyCertificateBidirectional|interface "
                         "result error! error:"
                      << result.Error();
            std::uint8_t const nrc{static_cast< std::uint8_t >(result.Error())};
            context->Respond(nrc);
        }
    } else {
        LogWarn() << "ServiceX29::_VerifyCertificateBidirectional|interface not config!";
        context->Respond(NrcErrc::kGeneralReject);
    }
}

void ServiceX29::_ProofOfOwnership(std::shared_ptr< RequestContext > &context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX29::_ProofOfOwnership| request context is nullptr!";
        return;
    }
    MessagePtr requestPtr = context->GetRequest();
    if (requestPtr.get() == nullptr) {
        LogWarn() << "ServiceX29::_ProofOfOwnership| request message is nullptr!";
        return;
    }
    std::vector< std::uint8_t > &body{requestPtr->GetBody()};
    std::size_t const minSize{6U};
    std::size_t totalLength{minSize};
    if (body.size() < minSize) {
        LogWarn() << "ServiceX29::_ProofOfOwnership| request message size must > 6 byte!";
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    Serialize deserialize{body};
    std::ignore                                  = deserialize.ReadUint8();
    std::uint8_t subfunction                     = deserialize.ReadUint8();
    std::uint16_t lengthOfProofOfOwnershipClient = deserialize.ReadUint16();
    totalLength += lengthOfProofOfOwnershipClient;
    if (body.size() < totalLength) {
        LogWarn() << "ServiceX29::_ProofOfOwnership| request message "
                     "ProofOfOwnershipClient size too short!";
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    std::vector< std::uint8_t > proofOfOwnershipClient
        = deserialize.ReadVector(static_cast< std::size_t >(lengthOfProofOfOwnershipClient));
    std::uint16_t lengthOfEphemeralPublicKeyClient = deserialize.ReadUint16();
    totalLength += lengthOfEphemeralPublicKeyClient;
    if (body.size() != totalLength) {
        LogWarn() << "ServiceX29::_ProofOfOwnership| request message "
                     "EphemeralPublicKeyClient size too short!";
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    if (manager_.get() != nullptr) {
        bool const verifyCertificateState = manager_->GetVerifyCertificateState(requestPtr->GetSA());
        if (!verifyCertificateState) {
            LogWarn() << "ServiceX29::_ProofOfOwnership| If the ‘proofOfOwnership’ "
                         "SubFunction is received "
                      << "without first successfully processing either a "
                         "‘verifyCertificateUnidirectional’ "
                      << "or ‘verifyCertificateBidirectional’ request message ";
            context->Respond(NrcErrc::kRequestSequenceError);
            return;
        }
    }
    std::vector< std::uint8_t > ephemeralPublicKeyClient
        = deserialize.ReadVector(static_cast< std::size_t >(lengthOfEphemeralPublicKeyClient));
    if (interfacePtr_.get() != nullptr) {
        MetaInfoMap metaInfo{context->GetMetaInfoMap()};
        Result< std::vector< std::uint8_t > > result
            = interfacePtr_->VerifyOwnership(proofOfOwnershipClient, ephemeralPublicKeyClient, metaInfo,
                                             CancellationHandler(context->GetCancellationState()));
        if (result.HasValue()) {
            std::vector< std::uint8_t > responseContent{result.Value()};
            MessagePtr responsePtr{context->CreateResponse()};
            Serialize serialize{responsePtr->GetBody()};
            serialize.Write(subfunction);
            std::uint8_t const authenticationReturnParameter{0x12U};
            serialize.Write(authenticationReturnParameter);
            std::uint16_t const lengthOfChallengeServer{static_cast< std::uint16_t >(responseContent.size())};
            serialize.Write(lengthOfChallengeServer);
            serialize.Write(responseContent);
            context->Respond(std::move(responsePtr));
            if (manager_.get() != nullptr) {
                manager_->SetVerifyCertificateState(requestPtr->GetSA(), false);
            }
        } else {
            LogWarn() << "ServiceX29::_ProofOfOwnership|interface result error! error:" << result.Error();
            std::uint8_t const nrc{static_cast< std::uint8_t >(result.Error())};
            context->Respond(nrc);
        }
    } else {
        LogWarn() << "ServiceX29::_ProofOfOwnership|interface not config!";
        context->Respond(NrcErrc::kGeneralReject);
    }
}

void ServiceX29::_TransmitCertificate(std::shared_ptr< RequestContext > &context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX29::_TransmitCertificate| request context is nullptr!";
        return;
    }
    MessagePtr requestPtr = context->GetRequest();
    if (requestPtr.get() == nullptr) {
        LogWarn() << "ServiceX29::_TransmitCertificate| request message is nullptr!";
        return;
    }
    std::vector< std::uint8_t > &body{requestPtr->GetBody()};
    std::size_t const minSize{5U};
    std::size_t totalLength{minSize};
    if (body.size() < minSize) {
        LogWarn() << "ServiceX29::_TransmitCertificate| request message size too short!";
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    Serialize deserialize{body};
    std::ignore                           = deserialize.ReadUint8();
    std::uint8_t subfunction              = deserialize.ReadUint8();
    std::uint16_t certificateEvaluationId = deserialize.ReadUint16();
    std::uint16_t lengthOfCertificateData = deserialize.ReadUint16();
    totalLength += lengthOfCertificateData;
    if (body.size() != totalLength) {
        LogWarn() << "ServiceX29::_TransmitCertificate| request message size too short!";
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    auto findResult = std::find_if(transmitCertificate_.certificateEvaluation.begin(),
                                   transmitCertificate_.certificateEvaluation.end(),
                                   [certificateEvaluationId](AuthTransmitCertificateEvaluation &other) {
                                       if (certificateEvaluationId == other.certificateEvaluationId) {
                                           return true;
                                       }
                                       return false;
                                   });
    if (findResult == transmitCertificate_.certificateEvaluation.end()) {
        LogWarn() << "ServiceX29::_TransmitCertificate| Validation of the "
                     "transmitCertificate "
                  << " certificateEvaluationId fails !";
        context->Respond(NrcErrc::kRequestOutOfRange);
        return;
    }
    std::vector< std::uint8_t > certificateData
        = deserialize.ReadVector(static_cast< std::size_t >(lengthOfCertificateData));
    if (interfacePtr_.get() != nullptr) {
        MetaInfoMap metaInfo{context->GetMetaInfoMap()};
        Result< void > result = interfacePtr_->TransmitCertificate(
            certificateData, metaInfo, CancellationHandler(context->GetCancellationState()));
        if (result.HasValue()) {
            MessagePtr responsePtr{context->CreateResponse()};
            Serialize serialize{responsePtr->GetBody()};
            serialize.Write(subfunction);
            std::uint8_t const authenticationReturnParameter{0x13U};
            serialize.Write(authenticationReturnParameter);
            context->Respond(std::move(responsePtr));
        } else {
            LogWarn() << "ServiceX29::_TransmitCertificate|interface result error! error:" << result.Error();
            std::uint8_t const nrc{static_cast< std::uint8_t >(result.Error())};
            context->Respond(nrc);
        }
    } else {
        LogWarn() << "ServiceX29::_TransmitCertificate|interface not config!";
        context->Respond(NrcErrc::kGeneralReject);
    }
}

void ServiceX29::_AuthenticationConfiguration(std::shared_ptr< RequestContext > &context) noexcept
{
    MessagePtr requestPtr{context->GetRequest()};
    if (requestPtr.get() == nullptr) {
        LogWarn() << "ServiceX29::_DeAuthenticate|request message is nulptr!";
        return;
    }
    std::vector< std::uint8_t > &body{requestPtr->GetBody()};
    std::size_t const minSizeOfBody{2U};
    if (body.size() != minSizeOfBody) {
        LogWarn()
            << "ServiceX29::_AuthenticationConfiguration| authentication configuration request size must == 2 byte!";
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        return;
    }
    MessagePtr responsePtr{context->CreateResponse()};
    Serialize serialize{responsePtr->GetBody()};
    std::uint8_t const subfunction{0x08U};
    std::uint8_t const authenticationReturnParameter{0x02U};
    serialize.Write(subfunction);
    serialize.Write(authenticationReturnParameter);
    context->Respond(std::move(responsePtr));
    return;
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
