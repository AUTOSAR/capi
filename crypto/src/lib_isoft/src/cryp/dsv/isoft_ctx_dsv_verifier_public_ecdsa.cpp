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
/// @file       isoft_ctx_dsv_verifier_public_ecdsa.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-03-23
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Signature Storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01010
/// @unit_name=PCtxDsv_VerifierPublic_Ecdsa
/// @unit_description=Signature Verification ECC Public Key Context Interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_verifier_public_ecdsa.h"

#include "ara/crypto/cryp/cryobj/isoft_key_public_ecc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_ipc.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_signature.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Signature verification public key context interface.
//********************************/
/// @brief Get the maximum supported key length in bits.
/// @returns maximal supported length of the key in bits
std::size_t PCtxDsv_VerifierPublic_Ecdsa::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_EccKey::EKeyLen::kEccKeyMaxBitLength);
}
/// @brief Get the minimum supported key length in bits.
/// @returns  std::size_t
std::size_t PCtxDsv_VerifierPublic_Ecdsa::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_EccKey::EKeyLen::kEccKeyMinBitLength);
}
/// @brief Verify support for a specific key length based on the context.
/// @param keyBitLength Key length: in bits
/// @returns  @c true if provided value of the key length is supported by the context
bool PCtxDsv_VerifierPublic_Ecdsa::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if ((keyBitLength < GetMinKeyBitLength()) || (keyBitLength > GetMaxKeyBitLength())) {
        return false;
    }
    return true;
}
/// @brief Get the size of the signature value produced and required by the current algorithm.
/// @returns  Size of the produced and required signature value
std::size_t PCtxDsv_VerifierPublic_Ecdsa::GetSignatureSize() const noexcept
{
    PKeyPublic_Ecc const *const pEccKey{
        static_cast< PKeyPublic_Ecc const * >(static_cast< void const * >(GetPublicKey()))};
    if (pEccKey != nullptr) {
        return pEccKey->GetPayloadSize();
    }
    return 0U;
}
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxDsv_VerifierPublic_Ecdsa::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_VerifierPublicEcdsa >()};
}
/// @brief Get CryptoService instance.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_VerifierPublic_Ecdsa::GetSignatureService() const noexcept
{
    return {std::make_unique< isoft_def::PServiceSignature< PCtxDsv_VerifierPublic_Ecdsa > >(*this)};
}
/// @brief Check if equal
/// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
/// @returns true if Macth sucess false otherwise
bool PCtxDsv_VerifierPublic_Ecdsa::IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept
{
    if (nSigCryId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateEcdsa)) {
        return true;
    }
    return false;
}
//***************/
/// @brief Check if Key meets requirements
/// @param key Public key
/// @returns  true if check key sucess false otherwise
bool PCtxDsv_VerifierPublic_Ecdsa::CheckKey(PublicKey const &key) const noexcept
{
    PAlgId_Asymmetric_EccKey const cryptoKey;
    if (false == cryptoKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId())) {
        return false;
    }

    return key.CheckKey();
}
/// @brief Execute signature encryption logic
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param suppressPadding Whether padding is suppressed
/// @param pDigestData Digest data
/// @param nDigestDataLen Digest data length
/// @return true if DoDecrypto sucess false otherwise
bool PCtxDsv_VerifierPublic_Ecdsa::DoDecrypto(uint8_t const *pInputData,
                                              uint32_t nDataLen,
                                              bool suppressPadding,
                                              uint8_t const *const pDigestData,
                                              uint32_t nDigestDataLen) const noexcept
{
    std::ignore = suppressPadding;
    if (pDigestData == nullptr) {
        return false;
    }
    if (nDigestDataLen == 0U) {
        return false;
    }
    if (pInputData == nullptr) {
        return false;
    }
    if (nDataLen == 0U) {
        return false;
    }
    PKeyPublic_Ipc_Ecc const *const pKeyPublicIpc{dynamic_cast< PKeyPublic_Ipc_Ecc const * >(GetPublicKey())};
    if (pKeyPublicIpc != nullptr) {
        return DoDecrypto_Ipc(pInputData, nDataLen, pDigestData, nDigestDataLen, pKeyPublicIpc->GetSlotId());
    }
    return ExecuteVerifyLogic(pInputData, nDataLen, pDigestData, nDigestDataLen);
}
/// @brief Execute signature verification logic (external interface, used in key manager)
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param pDigestData Digest data
/// @param nDigestDataLen Digest data length
/// @return true if ExecuteVerifyLogic sucess false otherwise
bool PCtxDsv_VerifierPublic_Ecdsa::ExecuteVerifyLogic(uint8_t const *const pInputData,
                                                      uint32_t const nDataLen,
                                                      uint8_t const *const pDigestData,
                                                      uint32_t const nDigestDataLen) const noexcept
{
    PKeyPublic_Ecc const *const pEccKey{
        static_cast< PKeyPublic_Ecc const * >(static_cast< void const * >(GetPublicKey()))};
    if (pEccKey == nullptr) {
        return false;
    }
    /// NID_sha1 here is unused and can be any value; OpenSSL 1.1 does not process it
    int32_t const ret{
        Ecdsa_verify(pEccKey->GetEcc(), pDigestData, nDigestDataLen, pInputData, static_cast< int32_t >(nDataLen))};

    // int32_t const ret{ECDSA_verify(NID_sha1, pDigestData, static_cast< int32_t >(nDigestDataLen), pInputData,
    //                                static_cast< int32_t >(nDataLen), pEccKey->GetEcc())};
    if (ret <= 0) {
        return false;
    }
    return true;
}
/// @brief Execute signature encryption logic
/// @param pInputData Start address of input data in algorithm operation
/// @param nInDataLen Input data length in algorithm operation
/// @param pInDigestData Digest data
/// @param nInDigestDataLen Digest data length
/// @param slotId Key slot ID
/// @return true if DoDecrypto_Ipc sucess false otherwise
bool PCtxDsv_VerifierPublic_Ecdsa::DoDecrypto_Ipc(uint8_t const *const pInputData,
                                                  uint32_t const nInDataLen,
                                                  uint8_t const *const pInDigestData,
                                                  uint32_t const nInDigestDataLen,
                                                  uint32_t const slotId) noexcept
{
    if (slotId == 0U) {
        return false;
    }

    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return false;
    }

    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient->DealIpcRequest(
        FUNC_NAME_Asymmetric(Ecc_verify), aswMsg,
        [pInputData, nInDataLen, pInDigestData, nInDigestDataLen,
         slotId](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_RsaLogic ipcReq;
                ipcReq.SetIpcSlotID(slotId);
                ipcReq.SetDataLen(nInDataLen);
                ipcReq.SetDigestDataLen(nInDigestDataLen);
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(pInputData, static_cast< uint16_t >(nInDataLen));
                std::ignore = pReqMsg->AddDataToIpc(pInDigestData,
                                                    static_cast< uint16_t >(nInDigestDataLen));  // Try if this works
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_RsaLogic) + nInDataLen);
        })};

    if (false == bDealIpc) {
        return false;
    }

    keys::isoft_def::cryptoctx::PIpcAsw_common *const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return false;
    }
    uint32_t const nDataLen{pIpcAsw->GetDataLen()};  // Set 1
    if (nDataLen != 1U) {
        return false;
    }
    return true;
}

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
