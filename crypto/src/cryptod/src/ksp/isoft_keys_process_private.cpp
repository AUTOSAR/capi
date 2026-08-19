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
/// @file       isoft_keys_process_private.cpp
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
/// @unit_name=PKeysProcess_Private
/// @unit_description=Asymmetric private key IPC service
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_keys_process_private.h"

#include <openssl/pem.h>
#include <openssl/rsa.h>

#include "ara/crypto/ksp/isoft_keys_manager.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Private key related logic processing
/// @name  PKeysProcess_Private
/// @param lpcProcessManager LCP command processing manager
PKeysProcess_Private::PKeysProcess_Private(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept
    : PKeysProcess_T_Base< PKeysProcess_Private >{this, lpcProcessManager}
{
    /// private key
    _InsertMapCmd(FUNC_NAME_PrivateKey(GetPublicKey_Rsa), &PKeysProcess_Private::GetPublicKey_Rsa);
    _InsertMapCmd(FUNC_NAME_PrivateKey(GetPublicKey_Ecc), &PKeysProcess_Private::GetPublicKey_Ecc);
    _InsertMapCmd(FUNC_NAME_PrivateKey(GetPayloadSize_Ecc), &PKeysProcess_Private::GetPayloadSize_Ecc);
    _InsertMapCmd(FUNC_NAME_PrivateKey(GetPayloadSize_Rsa), &PKeysProcess_Private::GetPayloadSize_Rsa);
    _InsertMapCmd(FUNC_NAME_PrivateKey(ExportPrivateKeyPublicly_Rsa),
                  &PKeysProcess_Private::ExportPrivateKeyPublicly_Rsa);
    _InsertMapCmd(FUNC_NAME_PrivateKey(ExportPrivateKeyPublicly_Ecc),
                  &PKeysProcess_Private::ExportPrivateKeyPublicly_Ecc);
    _InsertMapCmd(FUNC_NAME_PrivateKey(CheckKey), &PKeysProcess_Private::CheckKey);
}
/// @brief Gets public key
/// @name  GetPublicKey_Rsa
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has value if get publickey sucess else fail
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Private::GetPublicKey_Rsa(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    keys::isoft_def::cryptoctx::PIpcReq_Common const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_Common >()};
    uint32_t const nIpcSlotID{pReqMsg->GetIpcSlotID()};

    keys::isoft_def::PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoader(nIpcSlotID)};
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);  // IPC parameter error;
    }

    ara::core::Result< ara::core::Vector< uint8_t > > const result{pFindSlotLoader->LoadKeyData()};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    ara::core::Vector< uint8_t > vecData{result.Value()};
    size_t const nDataLen{vecData.size()};
    EVP_PKEY *const pRsa{TransfromToEvpKey(vecData.data(), nDataLen, true, true)};
    if (pRsa == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);  // IPC parameter error;
    }

    // Generate public key
    BIO *const pPublic{BIO_new(BIO_s_mem())};
    std::ignore = PEM_write_bio_PUBKEY(pPublic, pRsa);
    int32_t const nLen{PH_BIO_pending(pPublic)};
    if (nLen <= 0) {
        EVP_PKEY_free(pRsa);
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    internal::PAutoBuff const autoBuff{static_cast< uint32_t >(nLen) + 2U};
    uint8_t *const pChData{static_cast< uint8_t * >(static_cast< void * >(autoBuff.Data()))};
    std::ignore = BIO_read(pPublic, pChData, nLen);
    EVP_PKEY_free(pRsa);

    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcAsw_GetPublic))
        + static_cast< uint16_t >(nLen))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_GetPublic *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_GetPublic >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(nLen));
    pPacAsw->SetAllowedUsage(pFindSlotLoader->GetKeyContent().Value().mContentAllowedUsage);
    std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)), pChData,
                         static_cast< std::size_t >(nLen));
    return PResultLen::FromValue(pReqHead->nPacSize);
}

/// @brief Gets public key
/// @name  GetPublicKey_Ecc
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has value if get publickey sucess else fail
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Private::GetPublicKey_Ecc(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    keys::isoft_def::cryptoctx::PIpcReq_Common const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_Common >()};
    uint32_t const nIpcSlotID{pReqMsg->GetIpcSlotID()};

    keys::isoft_def::PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoader(nIpcSlotID)};
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);  // IPC parameter error;
    }

    ara::core::Result< ara::core::Vector< uint8_t > > const result{pFindSlotLoader->LoadKeyData()};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    ara::core::Vector< uint8_t > vecData{result.Value()};
    std::size_t const nDataLen{vecData.size()};
    EVP_PKEY *const ecKey{TransfromToEvpKey(vecData.data(), nDataLen, false, true)};
    if (ecKey == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    // Generate public key
    BIO *const pPublic{BIO_new(BIO_s_mem())};
    std::ignore = PEM_write_bio_PUBKEY(pPublic, ecKey);
    int32_t const nLen{PH_BIO_pending(pPublic)};
    if (nLen <= 0) {
        EVP_PKEY_free(ecKey);
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    internal::PAutoBuff const autoBuff{static_cast< uint32_t >(nLen) + 2U};
    int8_t *const pChData{static_cast< int8_t * >(static_cast< void * >(autoBuff.Data()))};
    std::ignore = BIO_read(pPublic, pChData, nLen);
    EVP_PKEY_free(ecKey);

    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcAsw_GetPublic))
        + static_cast< uint16_t >(nLen))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_GetPublic *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_GetPublic >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(nLen));
    pPacAsw->SetAllowedUsage(pFindSlotLoader->GetKeyContent().Value().mContentAllowedUsage);
    std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)), pChData,
                         static_cast< std::size_t >(nLen));
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Gets rsa key size
/// @name  GetPayloadSize_Rsa
/// @returns  PKeysProcess_Base::PResultLen
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Private::GetPayloadSize_Rsa(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    keys::isoft_def::cryptoctx::PIpcReq_Common const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_Common >()};

    ara::core::Result< ara::core::Vector< uint8_t > > const result{_LoadKeyData(pReqMsg->GetIpcSlotID())};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    ara::core::Vector< uint8_t > vecData{result.Value()};
    std::size_t const nLen{vecData.size()};
    EVP_PKEY *const pRsaPair{TransfromToEvpKey(vecData.data(), nLen, true, true)};
    int32_t const nPayloadSize{(pRsaPair != nullptr) ? Get_RsaKey_Size(pRsaPair) : 0};
    EVP_PKEY_free(pRsaPair);

    // Build response packet
    uint16_t const nAswLen{
        static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keys::isoft_def::cryptoctx::PIpcAsw_common))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(nPayloadSize));
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Gets ecc key size
/// @name  GetPayloadSize_Ecc
/// @returns  PKeysProcess_Base::PResultLen
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Private::GetPayloadSize_Ecc(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    keys::isoft_def::cryptoctx::PIpcReq_Common const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_Common >()};

    ara::core::Result< ara::core::Vector< uint8_t > > const result{_LoadKeyData(pReqMsg->GetIpcSlotID())};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    ara::core::Vector< uint8_t > vecData{result.Value()};
    std::size_t const nLen{vecData.size()};
    EVP_PKEY *const pEccPair{TransfromToEvpKey(vecData.data(), nLen, false, true)};
    if (pEccPair == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    int32_t const nPayloadSize{Get_EccKey_Size(pEccPair)};

    // Build response packet
    uint16_t const nAswLen{
        static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keys::isoft_def::cryptoctx::PIpcAsw_common))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(nPayloadSize));

    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Exports private key information
/// @name   ExportPrivateKeyPublicly_Rsa
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns
/// @throws
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Private::ExportPrivateKeyPublicly_Rsa(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    keys::isoft_def::cryptoctx::PIpcReq_Common const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_Common >()};

    ara::core::Result< ara::core::Vector< uint8_t > > const result{_LoadKeyData(pReqMsg->GetIpcSlotID())};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    ara::core::Vector< uint8_t > const &vecData{result.Value()};
    uint8_t const *const pSrcData{vecData.data()};
    size_t const nDataLen{vecData.size()};

    uint32_t const nFormatId{pReqMsg->GetDataLen()};
    uint8_t *pFromatData{nullptr};
    size_t nFroamtDataLen{0U};
    /// Default format directly returns data from the slot
    if (nFormatId != Serializable::kFormatDefault) {
        EVP_PKEY *const pRsaPair{TransfromToEvpKey(pSrcData, nDataLen, true, true)};
        if (pRsaPair == nullptr) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
        }

        int32_t ret{0};
        BIO *const pPrivate{BIO_new(BIO_s_mem())};
        if (nFormatId == Serializable::kFormatDerEncoded) {
            ret = i2d_PrivateKey_bio(pPrivate, pRsaPair);
        } else {
            ret = PEM_write_bio_PrivateKey(pPrivate, pRsaPair, nullptr, nullptr, 0, nullptr, nullptr);
        }
        ara::core::Vector< uint8_t > vecFormatData;
        if (ret >= 1) {
            int32_t const nNeedLen{PH_BIO_pending(pPrivate)};
            if (nNeedLen > 0) {
                vecFormatData.resize(static_cast< size_t >(nNeedLen));
                pFromatData    = vecFormatData.data();
                std::ignore    = BIO_read(pPrivate, pFromatData, nNeedLen);
                nFroamtDataLen = static_cast< uint32_t >(nNeedLen);
            }
        }
        std::ignore = BIO_free(pPrivate);
        EVP_PKEY_free(pRsaPair);
    } else {
        nFroamtDataLen = nDataLen;
    }
    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(static_cast< size_t >(pReqHead->GetHeadLen())
                                                   + sizeof(keys::isoft_def::cryptoctx::PIpcAsw_common)
                                                   + static_cast< size_t >(nFroamtDataLen))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(nFroamtDataLen));
    if (pFromatData != nullptr) {
        std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)), pFromatData,
                             static_cast< size_t >(nFroamtDataLen));
    } else {
        std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)), pSrcData,
                             static_cast< size_t >(nFroamtDataLen));
    }
    return PResultLen::FromValue(pReqHead->nPacSize);
}

/// @brief Checks key
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @return has value if check key sucess false otherwise
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Private::CheckKey(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        // IPC parameter error
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    keys::isoft_def::cryptoctx::PIpcReq_Common const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_Common >()};
    bool const strongCheck{(pReqMsg->GetDataLen() == 0U) ? false : true};

    ara::core::Result< ara::core::Vector< uint8_t > > const result{_LoadKeyData(pReqMsg->GetIpcSlotID())};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    ara::core::Vector< uint8_t > vecData{result.Value()};
    std::size_t const nLen{vecData.size()};
    EVP_PKEY *const pRsaPair{TransfromToEvpKey(vecData.data(), nLen, true, true)};
    if (pRsaPair == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    uint32_t checkRes{kInt_0U};
    if ((Rsa_check_key_evp(pRsaPair) == 0) && (strongCheck)) {
        checkRes = kInt_1U;
    }
    EVP_PKEY_free(pRsaPair);
    //  Build response packet
    uint16_t const nAswLen{
        static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keys::isoft_def::cryptoctx::PIpcAsw_common))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(checkRes);
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Exports ecc private key information
/// @name   ExportPrivateKeyPublicly_Ecc
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns
/// @throws
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Private::ExportPrivateKeyPublicly_Ecc(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    keys::isoft_def::cryptoctx::PIpcReq_Common const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_Common >()};
    keys::isoft_def::PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoader(pReqMsg->GetIpcSlotID())};
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    ara::core::Result< ara::core::Vector< uint8_t > > const result{pFindSlotLoader->LoadKeyData()};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    ara::core::Vector< uint8_t > const &vecData{result.Value()};
    uint8_t const *const pSrcData{vecData.data()};
    size_t const nDataLen{vecData.size()};

    uint8_t *pFromatData{nullptr};
    size_t nFroamtDataLen{0U};

    uint32_t const nFormatId{pReqMsg->GetDataLen()};
    /// Default format directly returns data from the slot
    if (nFormatId != Serializable::kFormatDefault) {
        EVP_PKEY *const pEccPair{TransfromToEvpKey(pSrcData, nDataLen, true, true)};
        if (pEccPair == nullptr) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
        }

        int32_t ret{0};
        BIO *const pPrivate{BIO_new(BIO_s_mem())};
        if (nFormatId == Serializable::kFormatDerEncoded) {
            ret = i2d_PrivateKey_bio(pPrivate, pEccPair);
        } else {
            ret = PEM_write_bio_PrivateKey(pPrivate, pEccPair, nullptr, nullptr, 0, nullptr, nullptr);
        }

        ara::core::Vector< uint8_t > vecFormatData;
        if (ret >= 1) {
            int32_t const nNeedLen{PH_BIO_pending(pPrivate)};
            if (nNeedLen > 0) {
                vecFormatData.resize(static_cast< size_t >(nNeedLen));
                pFromatData    = vecFormatData.data();
                std::ignore    = BIO_read(pPrivate, pFromatData, nNeedLen);
                nFroamtDataLen = static_cast< uint32_t >(nNeedLen);
            }
        }
        std::ignore = BIO_free(pPrivate);
        EVP_PKEY_free(pEccPair);
    } else {
        nFroamtDataLen = nDataLen;
    }

    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(static_cast< size_t >(pReqHead->GetHeadLen())
                                                   + sizeof(keys::isoft_def::cryptoctx::PIpcAsw_common)
                                                   + static_cast< size_t >(nFroamtDataLen))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(nFroamtDataLen));
    if (pFromatData != nullptr) {
        std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)), pFromatData,
                             static_cast< size_t >(nFroamtDataLen));
    } else {
        std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)), pSrcData,
                             static_cast< size_t >(nFroamtDataLen));
    }

    return PResultLen::FromValue(pReqHead->nPacSize);
}

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara