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
/// @file       isoft_ctx_dsv_signer_private_ecdsa.cpp
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
/// @unit_name=PCtxDsv_SignerPrivate_Ecdsa
/// @unit_description=Signature ECC private key context interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_signer_private_ecdsa.h"

#include "ara/crypto/cryp/cryobj/isoft_key_private_ecc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_private_ipc.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_signature.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// //AUTOSAR-AP Interface
/// @brief Returns the CryptoPrimitiveId instance containing the instance identifier.
/// @returns CryptoPrimitiveId instance.
CryptoPrimitiveId::Uptr PCtxDsv_SignerPrivate_Ecdsa::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_SignerPrivateEcdsa >()};
}
/// @brief Gets the CryptoService instance.
/// @returns CryptoService instance.
SignatureService::Uptr PCtxDsv_SignerPrivate_Ecdsa::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_SignerPrivate_Ecdsa > >(*this)};
}
//*************/ //PServiceCrypto Interface
/// @brief Gets the maximum supported key length (in bits).
/// @returns maximal supported length of the key in bits
std::size_t PCtxDsv_SignerPrivate_Ecdsa::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_EccKey::EKeyLen::kEccKeyMinBitLength);
}
/// @brief Gets the minimum supported key length (in bits).
/// @returns minimal supported key length in bits.
std::size_t PCtxDsv_SignerPrivate_Ecdsa::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_EccKey::EKeyLen::kEccKeyMinBitLength);
}
/// @brief Validates support for a specific key length based on the context.
/// @param keyBitLength Key length: in bits.
/// @returns  @c true if provided value of the key length is supported by the context
bool PCtxDsv_SignerPrivate_Ecdsa::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if ((keyBitLength < GetMinKeyBitLength()) || (keyBitLength > GetMaxKeyBitLength())) {
        return false;
    }
    return true;
}
/// @brief Gets the size of the signature value produced and required by the current algorithm.
/// @returns Size of the signature value produced and required.
std::size_t PCtxDsv_SignerPrivate_Ecdsa::GetSignatureSize() const noexcept
{
    PKeyPrivate_Ecc const *const pEccKey{
        static_cast< PKeyPrivate_Ecc const * >(static_cast< void const * >(GetPrivateKey()))};
    if (pEccKey != nullptr) {
        return pEccKey->GetPayloadSize();
    }
    return 0U;
}
//***************/
/// @brief Here cryptoKey is the default value kAsymmetricRsaKey.
/// @brief However, the value of the passed-in key is dynamically determined based on the length,
/// which could be kAsymmetricRsaKey512 or kAsymmetricRsaKey1024, etc.
/// @param key Private key.
/// @returns  true if check key sucess false otherwise
bool PCtxDsv_SignerPrivate_Ecdsa::CheckKey(PrivateKey const &key) const noexcept
{
    PAlgId_Asymmetric_EccKey const cryptoKey;
    return cryptoKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId());
}
/// @brief Gets the crypto primitive ID.
/// @return Crypto primitive ID.
CryptoPrimitiveId::AlgId PCtxDsv_SignerPrivate_Ecdsa::GetCryptoAlgId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateEcdsa);
}
/// @brief Performs signature encryption logic.
ara::core::Result< ara::core::Vector< ara::core::Byte > >
/// @brief Performs signature encryption logic.
/// @param pInputData Starting address of the input data for the algorithm operation.
/// @param nDataLen Data length.
/// @param suppressPadding Whether to suppress padding.
/// @return Encrypted data.
PCtxDsv_SignerPrivate_Ecdsa::DoEncrypto(uint8_t const *pInputData,
                                        uint32_t nDataLen,
                                        bool suppressPadding) const noexcept
{
    std::ignore = suppressPadding;
    if (pInputData == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    PKeyPrivate_Ipc_Ecc const *const pKeyPrivateIpc{dynamic_cast< PKeyPrivate_Ipc_Ecc const * >(GetPrivateKey())};
    if (pKeyPrivateIpc != nullptr) {
        ///
        return DoEncrypto_Ipc(pInputData, nDataLen, pKeyPrivateIpc->GetSlotId());
    }
    return ExecuteSignLogic(pInputData, nDataLen);
}
/// @brief Performs signature encryption logic.
/// @param pInputData Starting address of the input data for the algorithm operation.
/// @param nDataLen Data length.
/// @return Encrypted data.
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxDsv_SignerPrivate_Ecdsa::ExecuteSignLogic(
    uint8_t const *const pInputData, uint32_t const nDataLen) const noexcept
{
    PKeyPrivate_Ecc const *const pEccKey{
        static_cast< PKeyPrivate_Ecc const * >(static_cast< void const * >(GetPrivateKey()))};
    if (pEccKey == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    int32_t const len{Get_EccKey_Size(pEccKey->GetEcc())};
    internal::PAutoBuff const sign{static_cast< uint32_t >(len)};
    size_t signLen{static_cast< size_t >(len)};
    int32_t const ret{
        Ecdsa_sign(pEccKey->GetEcc(), pInputData, static_cast< int32_t >(nDataLen), sign.Data(), &signLen)};
    if (ret <= 0) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    ara::core::Vector< ara::core::Byte > sigData;
    uint8_t const *const pData{sign.Data()};
    if (pData == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    for (uint32_t i{0U}; i < signLen; i++) {
        sigData.push_back(static_cast< ara::core::Byte >(*(pData + i)));
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(sigData);
}
/// @brief IPC performs encryption operation.
/// @param pInputData Starting address of the input data for the algorithm operation.
/// @param nInDataLen Length of the input data for the algorithm operation.
/// @param slotId Key slot ID.
/// @return Encrypted data.
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxDsv_SignerPrivate_Ecdsa::DoEncrypto_Ipc(
    uint8_t const *const pInputData, uint32_t const nInDataLen, uint32_t const slotId) noexcept
{
    if (slotId == 0U) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }

    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient->DealIpcRequest(
        FUNC_NAME_Asymmetric(Ecc_sign), aswMsg,
        [pInputData, nInDataLen, slotId](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_RsaLogic ipcReq;
                ipcReq.SetIpcSlotID(slotId);
                ipcReq.SetDataLen(nInDataLen);
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(pInputData, static_cast< uint16_t >(nInDataLen));
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_RsaLogic) + nInDataLen);
        })};
    if (false == bDealIpc) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    uint8_t const *const pData{pIpcAsw->GetData()};
    uint32_t const nDataLen{pIpcAsw->GetDataLen()};
    ara::core::Vector< ara::core::Byte > vecResult;
    try {
        vecResult.reserve(static_cast< std::size_t >(nDataLen));
    } catch (std::bad_alloc &) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    for (uint32_t i{0U}; i < nDataLen; ++i) {
        vecResult.push_back(static_cast< ara::core::Byte >(*(pData + i)));
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecResult);
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
