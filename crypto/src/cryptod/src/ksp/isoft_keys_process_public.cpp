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
/// @file       isoft_keys_process_public.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2024-07-15
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/Asymmetric Encryption IPC Service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PKeysProcess_Public
/// @unit_description=Asymmetric public key IPC service
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_keys_process_public.h"

#include <openssl/pem.h>
#include <openssl/rsa.h>

#include "ara/crypto/ksp/isoft_keys_manager.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Public key logic processing
/// @name  PKeysProcess_Public
/// @param lpcProcessManager LCP command processing manager
PKeysProcess_Public::PKeysProcess_Public(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept
    : PKeysProcess_T_Base< PKeysProcess_Public >{this, lpcProcessManager}
{
    /// public key
    _InsertMapCmd(FUNC_NAME_PublicKey(ExportPublicKeyPublicly_Rsa), &PKeysProcess_Public::ExportPublicKeyPublicly_Rsa);
    _InsertMapCmd(FUNC_NAME_PublicKey(ExportPublicKeyPublicly_Ecc), &PKeysProcess_Public::ExportPublicKeyPublicly_Ecc);
    _InsertMapCmd(FUNC_NAME_PublicKey(GetPublicKeyPayloadSize_Ecc), &PKeysProcess_Public::GetPublicKeyPayloadSize_Ecc);
    _InsertMapCmd(FUNC_NAME_PublicKey(GetPublicKeyPayloadSize_Rsa), &PKeysProcess_Public::GetPublicKeyPayloadSize_Rsa);
}
/// @brief Exports rsa public key
/// @name   ExportPublicKeyPublicly_Rsa
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns has vlaue if export pulickey sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Public::ExportPublicKeyPublicly_Rsa(
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

    if (nFormatId != Serializable::kFormatDefault) {
        EVP_PKEY *const pRsaPair{TransfromToEvpKey(pSrcData, nDataLen, true, false)};
        if (pRsaPair == nullptr) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
        }

        int32_t ret{0};
        BIO *const pPublic{BIO_new(BIO_s_mem())};
        if (nFormatId == Serializable::kFormatDerEncoded) {
            ret = i2d_PUBKEY_bio(pPublic, pRsaPair);
        } else {
            ret = PEM_write_bio_PUBKEY(pPublic, pRsaPair);
        }
        ara::core::Vector< uint8_t > vecFormatData;
        if (ret >= 1) {
            int32_t const nNeedLen{PH_BIO_pending(pPublic)};
            if (nNeedLen > 0) {
                vecFormatData.resize(static_cast< size_t >(nNeedLen));
                pFromatData    = vecFormatData.data();
                std::ignore    = BIO_read(pPublic, pFromatData, nNeedLen);
                nFroamtDataLen = static_cast< uint32_t >(nNeedLen);
            }
        }
        std::ignore = BIO_free(pPublic);
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
/// @brief Exports ecc public key
/// @name   ExportPublicKeyPublicly_Ecc
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns has vlaue if export pulickey sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Public::ExportPublicKeyPublicly_Ecc(
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

    uint8_t *pFromatData{nullptr};
    size_t nFroamtDataLen{0U};
    uint32_t const nFormatId{pReqMsg->GetDataLen()};
    if (nFormatId != Serializable::kFormatDefault) {
        EVP_PKEY *const pEccPair{TransfromToEvpKey(pSrcData, nDataLen, false, false)};
        if (pEccPair == nullptr) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
        }

        int32_t ret{0};
        BIO *const pPublic{BIO_new(BIO_s_mem())};
        if (nFormatId == Serializable::kFormatDerEncoded) {
            ret = i2d_PUBKEY_bio(pPublic, pEccPair);
        } else {
            ret = PEM_write_bio_PUBKEY(pPublic, pEccPair);
        }
        ara::core::Vector< uint8_t > vecFormatData;
        if (ret >= 1) {
            int32_t const nNeedLen{PH_BIO_pending(pPublic)};
            if (nNeedLen > 0) {
                vecFormatData.resize(static_cast< size_t >(nNeedLen));
                pFromatData    = vecFormatData.data();
                std::ignore    = BIO_read(pPublic, pFromatData, nNeedLen);
                nFroamtDataLen = static_cast< uint32_t >(nNeedLen);
            }
        }
        std::ignore = BIO_free(pPublic);
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
/// @brief Gets ECC public key size
/// @name   GetPublicKeyPayloadSize_Ecc
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns Public key object payload size
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Public::GetPublicKeyPayloadSize_Ecc(
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
    EVP_PKEY *const pEccPair{TransfromToEvpKey(pSrcData, nDataLen, false, false)};
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

/// @brief Gets RSA public key size
/// @name   GetPublicKeyPayloadSize_Rsa
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns Public key object payload size
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Public::GetPublicKeyPayloadSize_Rsa(
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
    EVP_PKEY *const pRsaPair{TransfromToEvpKey(pSrcData, nDataLen, true, false)};
    if (pRsaPair == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    int32_t const nPayloadSize{(pRsaPair != nullptr) ? Get_RsaKey_Size(pRsaPair) : 0};

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
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara