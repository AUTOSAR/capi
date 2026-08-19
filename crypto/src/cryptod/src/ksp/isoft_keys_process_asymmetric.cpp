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
/// @file       isoft_keys_process_asymmetric.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2022-08-17
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/Asymmetric Encryption IPC Service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PKeysProcess_Asymmetric
/// @unit_description=Asymmetric processing logic class
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_keys_process_asymmetric.h"

#include <openssl/pem.h>
#include <openssl/rsa.h>

#include "ara/crypto/cryp/cryobj/isoft_key_private_ecc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_ecc.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_signer_private_ecdsa.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_verifier_public_ecdsa.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/select/isoft_select_rsa.h"
#include "ara/crypto/ksp/isoft_keys_manager.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief IPC asymmetric related processing logic class
/// @name  PKeysProcess_Asymmetric
/// @param lpcProcessManager LCP command processing manager
PKeysProcess_Asymmetric::PKeysProcess_Asymmetric(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept
    : PKeysProcess_T_Base< PKeysProcess_Asymmetric >{this, lpcProcessManager}
{
    _InsertMapCmd(FUNC_NAME_Asymmetric(SetKey), &PKeysProcess_Asymmetric::SetKey);

    /// Private key encryption/decryption
    _InsertMapCmd(FUNC_NAME_Asymmetric(RSA_private_encrypt), &PKeysProcess_Asymmetric::Private_Encrypt);
    _InsertMapCmd(FUNC_NAME_Asymmetric(RSA_private_decrypt), &PKeysProcess_Asymmetric::Private_Decrypt);

    /// Public key encryption/decryption
    _InsertMapCmd(FUNC_NAME_Asymmetric(RSA_public_encrypt), &PKeysProcess_Asymmetric::Public_Encrypt);
    _InsertMapCmd(FUNC_NAME_Asymmetric(RSA_public_decrypt), &PKeysProcess_Asymmetric::Public_Decrypt);

    /// Private key encryption/decryption / Public key decryption_RSA_Pss padding
    _InsertMapCmd(FUNC_NAME_Asymmetric(RSA_private_encrypt_pss), &PKeysProcess_Asymmetric::Private_Encrypt_Pss);
    _InsertMapCmd(FUNC_NAME_Asymmetric(RSA_public_decrypt_pss), &PKeysProcess_Asymmetric::Public_Decrypt_Pss);

    /// ECC signature
    _InsertMapCmd(FUNC_NAME_Asymmetric(Ecc_sign), &PKeysProcess_Asymmetric::Ecc_sign);
    _InsertMapCmd(FUNC_NAME_Asymmetric(Ecc_verify), &PKeysProcess_Asymmetric::Ecc_verify);

    /// private key
    _InsertMapCmd(FUNC_NAME_Asymmetric(Compute_key), &PKeysProcess_Asymmetric::Compute_key);
    _InsertMapCmd(FUNC_NAME_Asymmetric(DecapsulateKey), &PKeysProcess_Asymmetric::DecapsulateKey);
}
/// @brief Sets key
/// @name  SetKey
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PKeysProcess_Base::PResultLen
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Asymmetric::SetKey(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _SetKey(pReqHead, aswMsg);
}
/// @brief Executes RSA IPC operation
/// @name  DoRsaIpc
/// @returns  PKeysProcess_Base::PResultLen
/// @throws
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @param operateType Encryption/decryption type 0: Public key encryption, 1: Private key decryption, 2: Private key encryption, 3: Public key decryption
/// @returns has vlaue if DoRsaIpc sucess flase otherwise
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Asymmetric::DoRsaIpc(
    keys::isoft_def::PIpcPac_Head const *pReqHead,
    keys::isoft_def::PIpcAutoPacket &aswMsg,
    DoOperate operateType) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    keys::isoft_def::cryptoctx::PIpcReq_RsaLogic const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_RsaLogic >()};
    uint32_t const nIpcSlotID{pReqMsg->GetIpcSlotID()};
    uint32_t const nInDataLen{pReqMsg->GetDataLen()};
    uint32_t const nPadding{pReqMsg->GetPadding()};
    uint8_t const *const pInputData{pReqMsg->GetData()};

    keys::isoft_def::PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoader(nIpcSlotID)};
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }
    ara::core::Result< ara::core::Vector< uint8_t > > const result{_LoadKeyData(nIpcSlotID)};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    ara::core::Vector< uint8_t > vecData{result.Value()};
    ara::core::Result< ara::core::Vector< ara::core::Byte > > resVecByte{
        ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(SecurityErrorDomain::Errc::kIpcFault)};
    EVP_PKEY *pRsaPair{nullptr};
    std::size_t const nLen{vecData.size()};
    if ((operateType == DoOperate::kDoPublicEncrypt) || (operateType == DoOperate::kDoPublicDecrypt)) {
        pRsaPair = TransfromToEvpKey(vecData.data(), nLen, true, false);
    } else if ((operateType == DoOperate::kDoPrivateDecrypt) || (operateType == DoOperate::kDoPrivateEncrypt)) {
        pRsaPair = TransfromToEvpKey(vecData.data(), nLen, true, true);
    } else {
    }
    if (pRsaPair == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    PSelectRsa selecRsa{pRsaPair};

    switch (operateType) {
        case DoOperate::kDoPublicEncrypt: {
            resVecByte = selecRsa.PublicEncrypto(pInputData, nInDataLen, static_cast< int32_t >(nPadding));
            break;
        }
        case DoOperate::kDoPrivateDecrypt: {
            resVecByte = selecRsa.PrivateDecrypto(pInputData, nInDataLen, static_cast< int32_t >(nPadding));
            break;
        }
        case DoOperate::kDoPrivateEncrypt: {
            resVecByte = selecRsa.PrivateEncrypto(pInputData, nInDataLen, static_cast< int32_t >(nPadding));
            break;
        }
        case DoOperate::kDoPublicDecrypt: {
            resVecByte = selecRsa.PublicDecrypto(pInputData, nInDataLen, static_cast< int32_t >(nPadding));
            break;
        }
        default: {
            break;
        }
    }

    ara::core::Vector< ara::core::Byte > vecByte;
    if (!resVecByte.HasValue()) {
        EVP_PKEY_free(pRsaPair);
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    vecByte = std::move(resVecByte).Value();

    if (vecByte.empty()) {
        EVP_PKEY_free(pRsaPair);
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    EVP_PKEY_free(pRsaPair);

    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcAsw_common))
        + static_cast< uint16_t >(vecByte.size()))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(vecByte.size()));
    ara::core::Byte *const pData{vecByte.data()};
    std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)), pData, vecByte.size());
    return PResultLen::FromValue(vecByte.size());
}

/// @brief Decryption logic
/// @name  Private_Decrypt
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PKeysProcess_Base::PResultLen
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Asymmetric::Private_Decrypt(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return DoRsaIpc(pReqHead, aswMsg, DoOperate::kDoPrivateDecrypt);
}

/// @brief Signature encryption logic
/// @name  Private_Encrypt
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PKeysProcess_Base::PResultLen
/// @throws
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Asymmetric::Private_Encrypt(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return DoRsaIpc(pReqHead, aswMsg, DoOperate::kDoPrivateEncrypt);
}
/// @brief Signature public key decryption logic
/// @name  Public_Decrypt
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PKeysProcess_Base::PResultLen
/// @throws
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Asymmetric::Public_Decrypt(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return DoRsaIpc(pReqHead, aswMsg, DoOperate::kDoPublicDecrypt);
}
/// @brief Signature encryption logic
/// @name  Private_Encrypt_Pss
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PKeysProcess_Base::PResultLen
/// @throws
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Asymmetric::Private_Encrypt_Pss(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    keys::isoft_def::cryptoctx::PIpcReq_RsaLogic const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_RsaLogic >()};
    uint32_t const nIpcSlotID{pReqMsg->GetIpcSlotID()};
    uint32_t const nInHashId{pReqMsg->GetHashId()};

    uint32_t const nInSaltLen{pReqMsg->GetSaltLen()};
    uint8_t const *const pInputData{pReqMsg->GetData()};  // Includes signature data and digest data
    uint32_t const nDataLen{pReqMsg->GetDataLen()};

    // Load rsa from slot
    ara::core::Result< ara::core::Vector< uint8_t > > const result{_LoadKeyData(nIpcSlotID)};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }

    uint8_t const *const pKeyData{static_cast< uint8_t const * >(static_cast< void const * >(result.Value().data()))};
    size_t const nKeyDataLen{static_cast< uint32_t >(result.Value().size())};

    // result -> rsa
    EVP_PKEY *const pRsaPair{TransfromToEvpKey(pKeyData, nKeyDataLen, true, true)};
    if (pRsaPair == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    PSelectRsa selecRsa{pRsaPair};
    ara::core::Result< ara::core::Vector< ara::core::Byte > > resVecByte{
        selecRsa.PrivateEncryptoPssLocal(pInputData, nDataLen, nInHashId, nInSaltLen)};
    if (!resVecByte.HasValue()) {
        EVP_PKEY_free(pRsaPair);
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    ara::core::Vector< ara::core::Byte > vecByte{std::move(resVecByte).Value()};
    if (vecByte.empty()) {
        EVP_PKEY_free(pRsaPair);
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    EVP_PKEY_free(pRsaPair);

    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcAsw_common))
        + static_cast< uint16_t >(vecByte.size()))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(vecByte.size()));
    ara::core::Byte *const pData{vecByte.data()};
    std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)), pData, vecByte.size());

    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief ecc verification
/// @name  Public_Decrypt_Pss
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PKeysProcess_Base::PResultLen
/// @throws
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Asymmetric::Public_Decrypt_Pss(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    keys::isoft_def::cryptoctx::PIpcReq_RsaLogic const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_RsaLogic >()};
    uint32_t const nIpcSlotID{pReqMsg->GetIpcSlotID()};
    uint32_t const nInDataLen{pReqMsg->GetDataLen()};
    uint32_t const nInHashId{pReqMsg->GetHashId()};
    uint32_t const nInSaltLen{pReqMsg->GetSaltLen()};
    uint8_t const *const pInputData{pReqMsg->GetData()};  // Includes signature data and digest data
    uint32_t const nDigestDataLen{pReqMsg->GetDigestDataLen()};

    // Load rsa from slot
    ara::core::Result< ara::core::Vector< uint8_t > > const result{_LoadKeyData(nIpcSlotID)};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }

    uint8_t const *const pKeyData{static_cast< uint8_t const * >(static_cast< void const * >(result.Value().data()))};
    size_t const nKeyDataLen{static_cast< uint32_t >(result.Value().size())};

    // result -> rsa
    EVP_PKEY *const pRsaPair{TransfromToEvpKey(pKeyData, nKeyDataLen, true, false)};
    if (pRsaPair == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    PSelectRsa selecRsa{pRsaPair};
    ara::core::Result< bool > resBool{selecRsa.PublicDecryptoPssLocal(pInputData, nInDataLen, nInHashId, nInSaltLen,
                                                                      pInputData + nInDataLen, nDigestDataLen)};
    if (!resBool.HasValue()) {
        EVP_PKEY_free(pRsaPair);
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    EVP_PKEY_free(pRsaPair);
    bool const ret{resBool.Value()};
    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcAsw_common)))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(ret));
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Signature public key decryption logic_Pss padding
/// @name  Ecc_verify
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PKeysProcess_Base::PResultLen
/// @throws
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Asymmetric::Ecc_verify(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    keys::isoft_def::cryptoctx::PIpcReq_RsaLogic const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_RsaLogic >()};
    uint32_t const nIpcSlotID{pReqMsg->GetIpcSlotID()};
    uint32_t const nInDataLen{pReqMsg->GetDataLen()};
    uint8_t const *const pInputData{pReqMsg->GetData()};  // Includes signature data and digest data
    uint32_t const nDigestDataLen{pReqMsg->GetDigestDataLen()};

    // Load rsa from slot
    ara::core::Result< ara::core::Vector< uint8_t > > const result{_LoadKeyData(nIpcSlotID)};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }

    uint8_t const *const pKeyData{static_cast< uint8_t const * >(static_cast< void const * >(result.Value().data()))};
    size_t const nKeyDataLen{static_cast< uint32_t >(result.Value().size())};

    // result -> ecc
    EVP_PKEY *const pEccPair{TransfromToEvpKey(pKeyData, nKeyDataLen, false, false)};
    if (pEccPair == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    BIO *const pPublicDate{BIO_new(BIO_s_mem())};
    std::ignore = PEM_write_bio_PUBKEY(pPublicDate, pEccPair);
    int32_t const nLen{PH_BIO_pending(pPublicDate)};
    if (nLen <= 0) {
        std::ignore = BIO_free(pPublicDate);
        EVP_PKEY_free(pEccPair);
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    internal::PAutoBuff const autoBuff{static_cast< uint32_t >(nLen) + 2U};
    uint8_t *const pChData{static_cast< uint8_t * >(static_cast< void * >(autoBuff.Data()))};
    std::ignore = BIO_read(pPublicDate, pChData, nLen);
    ara::core::StringView eccPriKeyData{static_cast< char8_t * >(static_cast< void * >(pChData)),
                                        static_cast< uint64_t >(nLen)};
    std::ignore = BIO_free(pPublicDate);
    EVP_PKEY_free(pEccPair);

    cryp::isoft_def::PCryptoProvider::Uptr const pPCryptoProvider{
        std::make_unique< cryp::isoft_def::PCryptoProvider >()};
    std::unique_ptr< PCtxDsv_VerifierPublic_Ecdsa > const pVerifierPublicEcdsa{
        std::make_unique< PCtxDsv_VerifierPublic_Ecdsa >(*pPCryptoProvider)};
    PKeyPublic_Ecc::Uptr const pKeyPublicEcc{std::make_unique< PKeyPublic_Ecc >(eccPriKeyData)};
    if (eccPriKeyData.empty()) {
    }
    if (pKeyPublicEcc == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    ara::core::Result< uint32_t > const resAllowedUsage{_GetAllowedUsage(nIpcSlotID)};
    if (!resAllowedUsage.HasValue()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    pKeyPublicEcc->SetAllowedUsage(resAllowedUsage.Value());

    ara::core::Result< void > const resVoid{pVerifierPublicEcdsa->SetKey(*pKeyPublicEcc)};
    if (!resVoid.HasValue()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    bool const ret{
        pVerifierPublicEcdsa->ExecuteVerifyLogic(pInputData, nInDataLen, pInputData + nInDataLen, nDigestDataLen)};
    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcAsw_common)))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(ret));
    return PResultLen::FromValue(pReqHead->nPacSize);
}

/// @brief Ecc signature
/// @name  Ecc_sign
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if Ecc_sign sucess flase otherwise
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Asymmetric::Ecc_sign(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    keys::isoft_def::cryptoctx::PIpcReq_RsaLogic const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_RsaLogic >()};
    uint32_t const nIpcSlotID{pReqMsg->GetIpcSlotID()};
    uint8_t const *const pInputData{pReqMsg->GetData()};  // Includes signature data and digest data
    uint32_t const nDataLen{pReqMsg->GetDataLen()};

    // Load rsa from slot
    ara::core::Result< ara::core::Vector< uint8_t > > const result{_LoadKeyData(nIpcSlotID)};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }

    uint8_t const *const pKeyData{static_cast< uint8_t const * >(static_cast< void const * >(result.Value().data()))};
    size_t const nKeyDataLen{static_cast< uint32_t >(result.Value().size())};

    // result -> ecc
    EVP_PKEY *const pEccPair{TransfromToEvpKey(pKeyData, nKeyDataLen, false, true)};
    if (pEccPair == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    BIO *const pPrivateDate{BIO_new(BIO_s_mem())};
    std::ignore = PEM_write_bio_PrivateKey(pPrivateDate, pEccPair, nullptr, nullptr, 0, nullptr, nullptr);
    int32_t const nLen{PH_BIO_pending(pPrivateDate)};
    if (nLen <= 0) {
        EVP_PKEY_free(pEccPair);
        std::ignore = BIO_free(pPrivateDate);
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    internal::PAutoBuff const autoBuff{static_cast< uint32_t >(nLen) + 2U};
    uint8_t *const pChData{static_cast< uint8_t * >(static_cast< void * >(autoBuff.Data()))};
    std::ignore = BIO_read(pPrivateDate, pChData, nLen);
    ara::core::StringView const eccPriKeyData{static_cast< char8_t * >(static_cast< void * >(pChData)),
                                              static_cast< uint64_t >(nLen)};
    EVP_PKEY_free(pEccPair);
    std::ignore = BIO_free(pPrivateDate);

    cryp::isoft_def::PCryptoProvider::Uptr const pPCryptoProvider{
        std::make_unique< cryp::isoft_def::PCryptoProvider >()};
    std::unique_ptr< PCtxDsv_SignerPrivate_Ecdsa > const pSignerPrivateEcdsa{
        std::make_unique< PCtxDsv_SignerPrivate_Ecdsa >(*pPCryptoProvider)};
    PKeyPrivate_Ecc::Uptr const pKeyPrivateEcc{std::make_unique< PKeyPrivate_Ecc >()};

    if (pKeyPrivateEcc == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    if (!pKeyPrivateEcc->AttachKey(eccPriKeyData)) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    ara::core::Result< uint32_t > const resAllowedUsage{_GetAllowedUsage(nIpcSlotID)};
    if (!resAllowedUsage.HasValue()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    pKeyPrivateEcc->SetAllowedUsage(resAllowedUsage.Value());
    ara::core::Result< void > const resVoid{pSignerPrivateEcdsa->SetKey(*pKeyPrivateEcc)};
    if (!resVoid.HasValue()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    ara::core::Result< ara::core::Vector< ara::core::Byte > > resVecByte{
        pSignerPrivateEcdsa->ExecuteSignLogic(pInputData, nDataLen)};
    if (!resVecByte.HasValue()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    ara::core::Vector< ara::core::Byte > vecByte{std::move(resVecByte).Value()};
    if (vecByte.empty()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcAsw_common))
        + static_cast< uint16_t >(vecByte.size()))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(vecByte.size()));
    ara::core::Byte *const pData{vecByte.data()};
    std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)), pData, vecByte.size());

    return PResultLen::FromValue(pReqHead->nPacSize);
}

/// @brief Public key encryption logic
/// @name  Public_Encrypt
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PKeysProcess_Base::PResultLen
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Asymmetric::Public_Encrypt(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return DoRsaIpc(pReqHead, aswMsg, DoOperate::kDoPublicEncrypt);
}
/// @brief Generates shared secret
/// @name  Compute_key
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PKeysProcess_Base::PResultLen
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Asymmetric::Compute_key(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    keys::isoft_def::cryptoctx::PIpcReq_AgreeKey const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_AgreeKey >()};

    keys::isoft_def::PSvrKeySlotLoader *const pFindSlotLoaderPrivate{_FindSlotLoader(pReqMsg->GetIpcSlotID())};
    if (nullptr == pFindSlotLoaderPrivate) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    ara::core::Result< ara::core::Vector< uint8_t > > const result{pFindSlotLoaderPrivate->LoadKeyData()};
    if (!result.HasValue()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    BIO *const pKeyBio{BIO_new_mem_buf(result.Value().data(), static_cast< int32_t >(result.Value().size()))};
    EVP_PKEY *ecKeyPrivate{PEM_read_bio_PrivateKey(pKeyBio, nullptr, nullptr, nullptr)};
    std::ignore = BIO_free(pKeyBio);

    uint8_t *pData{nullptr};
    uint32_t nDataLen{0U};

    ara::core::Vector< uint8_t > vecData;
    /// Public key is ipc
    if (pReqMsg->GetIpcPubSlotID() != 0U) {
        keys::isoft_def::PSvrKeySlotLoader *const pFindSlotLoaderPub{_FindSlotLoader(pReqMsg->GetIpcPubSlotID())};
        if (nullptr == pFindSlotLoaderPub) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
        }
        ara::core::Result< ara::core::Vector< uint8_t > > resultPub{pFindSlotLoaderPub->LoadKeyData()};
        if (!result.HasValue()) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
        }
        vecData  = std::move(resultPub).Value();
        nDataLen = static_cast< uint32_t >(vecData.size());
        if (nDataLen > 0U) {
            pData = static_cast< uint8_t * >(&vecData[0U]);
        }
    } else {
        pData    = const_cast< uint8_t * >(pReqMsg->GetData());
        nDataLen = pReqMsg->GetDataLen();
    }

    if (pData == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    BIO *const pKeyBioPub{BIO_new_mem_buf(pData, static_cast< int32_t >(nDataLen))};
    EVP_PKEY *ecKeyPublic{PEM_read_bio_PUBKEY(pKeyBioPub, nullptr, nullptr, nullptr)};
    std::ignore = BIO_free(pKeyBioPub);

    ara::core::Vector< u_char > vecShareKey;
    vecShareKey.resize(kInt_128U);
    u_char *const sharedkey{vecShareKey.data()};

    size_t len{0U};

    /* Generate one party's shared secret */
    if (Ecdh_key_exchange(static_cast< uint8_t * >(sharedkey), &len, ecKeyPublic, ecKeyPrivate) == 0) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcAsw_common))
        + static_cast< uint16_t >(len))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(len));
    std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)),
                         static_cast< void const * >(sharedkey), len);
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Unwraps key
/// @name  DecapsulateKey
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PKeysProcess_Base::PResultLen
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Asymmetric::DecapsulateKey(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    keys::isoft_def::cryptoctx::PIpcReq_Common const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_Common >()};
    uint32_t const nIpcSlotID{pReqMsg->GetIpcSlotID()};
    uint32_t const nDatalen{pReqMsg->GetDataLen()};
    uint8_t const *const pInputData{pReqMsg->GetData()};

    ara::core::Result< ara::core::Vector< uint8_t > > const result{_LoadKeyData(nIpcSlotID)};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    ara::core::Vector< uint8_t > vecKeyData{result.Value()};
    std::size_t const nLen{vecKeyData.size()};
    EVP_PKEY *const pRsa{TransfromToEvpKey(vecKeyData.data(), nLen, true, true)};
    if (pRsa == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    size_t const nlen{static_cast< size_t >(Get_RsaKey_Size(pRsa))};

    if (nlen > static_cast< size_t >(kInt_4096U)) {
        EVP_PKEY_free(pRsa);
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);  // IPC parameter error;
    }

    ara::core::Vector< u_char > vecRandomByteC;
    vecRandomByteC.resize(kInt_4096U);
    u_char *const randomByteC{vecRandomByteC.data()};

    ara::core::Vector< u_char > vecWrapeddataWK;
    vecWrapeddataWK.resize(kInt_4096U);
    u_char *const wrapeddataWK{vecWrapeddataWK.data()};

    std::ignore = memcpy(static_cast< void * >(randomByteC), pInputData, nlen);
    std::ignore = memcpy(static_cast< void * >(wrapeddataWK), pInputData + nlen, nDatalen - nlen);

    ara::core::Vector< u_char > vecRandomByteZ;
    vecRandomByteZ.resize(kInt_4096U);
    u_char *const randomByteZ{vecRandomByteZ.data()};

    size_t retlenZ{0U};
    std::ignore = Rsasve_recover_evp(pRsa, static_cast< u_char * >(randomByteZ), &retlenZ,
                                     static_cast< u_char * >(randomByteC), nlen);

    EVP_PKEY_free(pRsa);

    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcAsw_DecapsulateKey))
        + static_cast< uint16_t >(retlenZ))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_DecapsulateKey *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_DecapsulateKey >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(retlenZ));
    pPacAsw->SetRsaLen(static_cast< uint32_t >(nlen));
    std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)),
                         static_cast< void const * >(randomByteZ), retlenZ);

    return PResultLen::FromValue(pReqHead->nPacSize);
}
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara