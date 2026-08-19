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
/// @file       isoft_select_rsa.cpp
/// @brief      AutoSar-Crypto Encryption and Decryption Module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Asymmetric Encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PSelectRsa
/// @unit_description=RSA Encryption and Decryption Algorithm Encapsulation
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/select/isoft_select_rsa.h"

#include "ara/crypto/common/isoft_assert.h"
#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/hash/isoft_ctx_hash_function.h"
#include "ara/crypto/ipc/isoft_ipc_client.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief RSA Algorithm Encapsulation
//********************************/
/// @brief Constructor with parameters
/// @name  PSelectRsa
/// @param pRsaKey RSA key resource
PSelectRsa::PSelectRsa(EVP_PKEY* const pRsaKey) noexcept : PSelectRsa{nullptr, pRsaKey, 0U}
{
    PH_ASSERT(pRsaKey_ != nullptr);
}
/// @brief Constructor with parameters
/// @name  PSelectRsa
/// @param nSlotId Key slot ID
/// @returns
/// @throws
PSelectRsa::PSelectRsa(uint32_t const nSlotId) noexcept : PSelectRsa{nullptr, nullptr, nSlotId} {}
/// @brief Constructor with parameters
/// @param pBuffOutput Output buffer
/// @param pRsaKey RSA key resource
/// @param nSlotId Key slot ID
PSelectRsa::PSelectRsa(internal::PAutoBuff* const pBuffOutput, EVP_PKEY* const pRsaKey, uint32_t const nSlotId) noexcept
    : pBuffOutput_{pBuffOutput}, pRsaKey_{pRsaKey}, nSlotId_{nSlotId}
{
}
/// @brief Set the external buffer
/// @name  AttachOutputBuff
/// @param pBuffOutput Output buffer
void PSelectRsa::AttachOutputBuff(internal::PAutoBuff* const pBuffOutput) noexcept { pBuffOutput_ = pBuffOutput; }
/// @brief Execute RSA logic
/// @name  DoRsaLogic
/// @param bEncrypto Whether to encrypt
/// @param pInputData Starting address of input data in algorithm operation
/// @param nDataLen Data length
/// @param nPadding Data padding scheme
/// @param cbFun Callback function for logic execution
/// @returns Encryption/decryption result
PSelectRsa::VecResult PSelectRsa::DoRsaLogic(bool const bEncrypto,
                                             uint8_t const* const pInputData,
                                             uint32_t const nDataLen,
                                             int32_t const nPadding,
                                             CB_RsaLogic const& cbFun) const noexcept
{
    if (bEncrypto) {
        return _DoRsaLogic(pInputData, nDataLen, _GetModulusSize(nPadding), cbFun);
    }
    return _DoRsaLogic(pInputData, nDataLen, static_cast< int32_t >(GetBlockSize()), cbFun);
}

/// @brief Local private key encryption _Pss
/// @param pInputData Starting address of input data in algorithm operation
/// @param nInDataLen Length of input data in algorithm operation
/// @param nInHashId Hash algorithm crypto primitive ID
/// @param nInSaltLen Salt data length
/// @return PSS private key encryption result
PSelectRsa::VecResult PSelectRsa::PrivateEncryptoPssLocal(uint8_t const* const pInputData,
                                                          uint32_t const nInDataLen,
                                                          uint32_t const nInHashId,
                                                          uint32_t const nInSaltLen) noexcept
{
    std::ignore = nInDataLen;
    int32_t const rsaSize{Get_RsaKey_Size(pRsaKey_)};
    std::unique_ptr< uint8_t[] > const pResultPssData{new uint8_t[static_cast< size_t >(rsaSize)]};
    size_t outLen = rsaSize;
    int32_t const paddingSuccess{
        RSA_padding_add_PKCS1_PSS_evp(pRsaKey_, pResultPssData.get(), &outLen,
                                      PCtxHashFunction::GetEVP_MD(static_cast< CryptoContext::AlgId >(nInHashId)),
                                      static_cast< int32_t >(nInSaltLen), pInputData, nInDataLen)};
    if (paddingSuccess == 0) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kDsvSignerPrivateRsaPss);
    }

    // Then encrypt the PSS result
    std::unique_ptr< uint8_t[] > const pOutData{new uint8_t[static_cast< size_t >(rsaSize)]};
    int32_t const nCipherLen{Rsa_private_encrypt_evp(pResultPssData.get(), static_cast< int32_t >(rsaSize),
                                                     pOutData.get(), pRsaKey_, RSA_NO_PADDING)};
    if (nCipherLen < 0) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kDsvSignerPrivateRsaPss);
    }

    ara::core::Vector< ara::core::Byte > vecResult;
    vecResult.reserve(static_cast< std::size_t >(nCipherLen));
    for (int32_t i{0}; i < nCipherLen; ++i) {
        vecResult.push_back(static_cast< ara::core::Byte >(*(pOutData.get() + i)));
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecResult);
}
/// @brief IPC private key encryption _Pss
/// @name  PrivateEncryptoPssIpc
/// @param pInputData Starting address of input data in algorithm operation
/// @param nInDataLen Length of input data in algorithm operation
/// @param nInHashId Hash algorithm crypto primitive ID
/// @param nInSaltLen Salt data length
/// @returns PSS IPC private key encryption result
PSelectRsa::VecResult PSelectRsa::PrivateEncryptoPssIpc(uint8_t const* const pInputData,
                                                        uint32_t const nInDataLen,
                                                        uint32_t const nInHashId,
                                                        uint32_t const nInSaltLen) noexcept
{
    if (nSlotId_ == 0U) {
        return PSelectRsa::VecResult::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    keys::isoft_def::PIpcClient* const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return PSelectRsa::VecResult::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }

    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient->DealIpcRequest(
        FUNC_NAME_Asymmetric(RSA_private_encrypt_pss), aswMsg,
        [this, pInputData, nInDataLen, nInHashId,
         nInSaltLen](keys::isoft_def::PIpcAutoPacket const* const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_RsaLogic ipcReq;
                ipcReq.SetIpcSlotID(nSlotId_);
                ipcReq.SetDataLen(nInDataLen);
                ipcReq.SetHashId(nInHashId);
                ipcReq.SetSaltLen(nInSaltLen);
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(pInputData, static_cast< uint16_t >(nInDataLen));
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_RsaLogic) + nInDataLen);
        })};
    if (false == bDealIpc) {
        return PSelectRsa::VecResult::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    keys::isoft_def::cryptoctx::PIpcAsw_common* const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return PSelectRsa::VecResult::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    uint8_t const* const pData{pIpcAsw->GetData()};
    uint32_t const nDataLen{pIpcAsw->GetDataLen()};
    ara::core::Vector< ara::core::Byte > vecResult;
    try {
        vecResult.reserve(static_cast< std::size_t >(nDataLen));
    } catch (std::bad_alloc&) {
        return PSelectRsa::VecResult::FromError(SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    for (uint32_t i{0U}; i < nDataLen; ++i) {
        vecResult.push_back(static_cast< ara::core::Byte >(*(pData + i)));
    }
    return PSelectRsa::VecResult::FromValue(vecResult);
}
/// @brief Private key encryption
/// @name  PrivateEncrypto
/// @param pInputData Starting address of input data in algorithm operation
/// @param nInDataLen Length of input data in algorithm operation
/// @param nPadding Data padding scheme
/// @returns Private key encryption result
PSelectRsa::VecResult PSelectRsa::PrivateEncrypto(uint8_t const* const pInputData,
                                                  uint32_t const nInDataLen,
                                                  int32_t const nPadding) noexcept
{
    if (pRsaKey_ != nullptr) {
        return _DoRsaLogic(pInputData, nInDataLen, _GetModulusSize(nPadding),
                           [this, nPadding](uint32_t const nDataLen, uint8_t const* const pInData,
                                            uint8_t* const pOutData) -> int32_t {
                               return Rsa_private_encrypt_evp(pInData, nDataLen, pOutData, pRsaKey_, nPadding);
                           });
    }
    if (nSlotId_ == 0U) {
        return PSelectRsa::VecResult::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    return DoRsaLogicIpc(FUNC_NAME_Asymmetric(RSA_private_encrypt), nSlotId_, pInputData, nInDataLen, nPadding);
}
/// @brief Private key decryption
/// @name  PrivateDecrypto
/// @param pInputData Starting address of input data in algorithm operation
/// @param nInDataLen Length of input data in algorithm operation
/// @param nPadding Data padding scheme
/// @returns Private key decryption result
PSelectRsa::VecResult PSelectRsa::PrivateDecrypto(uint8_t const* const pInputData,
                                                  uint32_t const nInDataLen,
                                                  int32_t const nPadding) noexcept
{
    if (pRsaKey_ != nullptr) {
        return _DoRsaLogic(pInputData, nInDataLen, static_cast< int32_t >(GetBlockSize()),
                           [this, nPadding](uint32_t const nDataLen, uint8_t const* const pInData,
                                            uint8_t* const pOutData) -> int32_t {
                               return Rsa_private_decrypt_evp(pRsaKey_, pOutData, pInData,
                                                              static_cast< int32_t >(nDataLen), nPadding);
                           });
    }
    if (nSlotId_ == 0U) {
        return PSelectRsa::VecResult::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    return DoRsaLogicIpc(FUNC_NAME_Asymmetric(RSA_private_decrypt), nSlotId_, pInputData, nInDataLen, nPadding);
}
/// @brief Public key encryption
/// @name  PublicEncrypto
/// @param pInputData Starting address of input data in algorithm operation
/// @param nInDataLen Length of input data in algorithm operation
/// @param nPadding Data padding scheme
/// @returns Public key encryption result
PSelectRsa::VecResult PSelectRsa::PublicEncrypto(uint8_t const* const pInputData,
                                                 uint32_t const nInDataLen,
                                                 int32_t const nPadding) noexcept
{
    if (pRsaKey_ != nullptr) {
        return _DoRsaLogic(pInputData, nInDataLen, _GetModulusSize(nPadding),
                           [this, nPadding](uint32_t const nDataLen, uint8_t const* const pInData,
                                            uint8_t* const pOutData) -> int32_t {
                               return Rsa_public_encrypt_evp(pRsaKey_, pOutData, pInData,
                                                             static_cast< int32_t >(nDataLen), nPadding);
                           });
    }
    if (nSlotId_ == 0U) {
        return PSelectRsa::VecResult::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    return DoRsaLogicIpc(FUNC_NAME_Asymmetric(RSA_public_encrypt), nSlotId_, pInputData, nInDataLen, nPadding);
}
/// @brief Public key decryption
/// @name  PublicDecrypto
/// @param pInputData Starting address of input data in algorithm operation
/// @param nInDataLen Length of input data in algorithm operation
/// @param nPadding Data padding scheme
/// @returns Public key decryption result
PSelectRsa::VecResult PSelectRsa::PublicDecrypto(uint8_t const* const pInputData,
                                                 uint32_t const nInDataLen,
                                                 int32_t const nPadding) noexcept
{
    if (pRsaKey_ != nullptr) {
        return _DoRsaLogic(pInputData, nInDataLen, static_cast< int32_t >(GetBlockSize()),
                           [this, nPadding](uint32_t const nDataLen, uint8_t const* const pInData,
                                            uint8_t* const pOutData) -> int32_t {
                               return Rsa_public_decrypt_evp(pInData, nDataLen, pOutData, pRsaKey_, nPadding);
                           });
    }
    if (nSlotId_ == 0U) {
        return PSelectRsa::VecResult::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    return DoRsaLogicIpc(FUNC_NAME_Asymmetric(RSA_public_decrypt), nSlotId_, pInputData, nInDataLen, nPadding);
}
/// @brief Public key decryption _Pss
/// @param pInputData Starting address of input data in algorithm operation
/// @param nInDataLen Length of input data in algorithm operation
/// @param nInHashId Hash algorithm crypto primitive ID
/// @param nInSaltLen Salt data length
/// @param pInDigestData Digest data
/// @param nInDigestDataLen Digest data length
/// @return PSS public key decryption result
PSelectRsa::BoolResult PSelectRsa::PublicDecryptoPssLocal(uint8_t const* const pInputData,
                                                          uint32_t const nInDataLen,
                                                          uint32_t const nInHashId,
                                                          uint32_t const nInSaltLen,
                                                          uint8_t const* const pInDigestData,
                                                          uint32_t const nInDigestDataLen) noexcept
{
    int32_t const nPadding{RSA_NO_PADDING};
    // Obtain the decrypted signature result, then call RSA_verify_PKCS1_PSS()
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resVec{
        PublicDecrypto(pInputData, nInDataLen, nPadding)};
    if (resVec.HasValue() == false) {
        return PSelectRsa::BoolResult::FromError(SecurityErrorDomain::Errc::kDsvSignerPrivateRsaPss);
    }
    // uint8_t const *EM
    uint8_t const* const sigDataToVerify{
        static_cast< uint8_t const* >(static_cast< void const* >(resVec.Value().data()))};
    // Call RSA_verify_PKCS1_PSS and verify the result
    int32_t const verifySucccess{
        RSA_verify_PKCS1_PSS_evp(pRsaKey_, pInDigestData, nInDigestDataLen,
                                 PCtxHashFunction::GetEVP_MD(static_cast< CryptoContext::AlgId >(nInHashId)),
                                 static_cast< int32_t >(nInSaltLen), sigDataToVerify, resVec.Value().size())};  //
    if (verifySucccess == 0) {
        return PSelectRsa::BoolResult::FromValue(false);
    }
    return PSelectRsa::BoolResult::FromValue(true);
}
/// @brief Public key decryption _PSS
/// @name  PublicDecryptoPssIpc
/// @param pInputData Starting address of input data in algorithm operation
/// @param nInDataLen Length of input data in algorithm operation
/// @param nInHashId Hash algorithm crypto primitive ID
/// @param nInSaltLen Salt data length
/// @param pInDigestData Digest data
/// @param nInDigestDataLen Digest data length
/// @returns PSS IPC public key decryption result
PSelectRsa::BoolResult PSelectRsa::PublicDecryptoPssIpc(uint8_t const* const pInputData,
                                                        uint32_t const nInDataLen,
                                                        uint32_t const nInHashId,
                                                        uint32_t const nInSaltLen,
                                                        uint8_t const* const pInDigestData,
                                                        uint32_t const nInDigestDataLen) noexcept
{
    if (nSlotId_ == 0U) {
        return PSelectRsa::BoolResult::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }

    keys::isoft_def::PIpcClient* const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return PSelectRsa::BoolResult::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }

    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient->DealIpcRequest(
        FUNC_NAME_Asymmetric(RSA_public_decrypt_pss), aswMsg,
        [this, pInputData, nInDataLen, nInHashId, nInSaltLen, pInDigestData,
         nInDigestDataLen](keys::isoft_def::PIpcAutoPacket const* const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_RsaLogic ipcReq;
                ipcReq.SetIpcSlotID(nSlotId_);
                ipcReq.SetDataLen(nInDataLen);
                ipcReq.SetHashId(nInHashId);
                ipcReq.SetSaltLen(nInSaltLen);
                ipcReq.SetDigestDataLen(nInDigestDataLen);
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(pInputData, static_cast< uint16_t >(nInDataLen));
                std::ignore
                    = pReqMsg->AddDataToIpc(pInDigestData, static_cast< uint16_t >(nInDigestDataLen));  // Give it a try
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_RsaLogic) + nInDataLen);
        })};

    if (false == bDealIpc) {
        return PSelectRsa::BoolResult::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }

    keys::isoft_def::cryptoctx::PIpcAsw_common* const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return PSelectRsa::BoolResult::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }

    uint8_t const* const pData{pIpcAsw->GetData()};  // 1 or 0
    uint32_t const nDataLen{pIpcAsw->GetDataLen()};  // Set to 1
    bool isSuccess{false};
    if (nDataLen == 1U) {
        isSuccess = nullptr != pData;
    } else {
        return PSelectRsa::BoolResult::FromValue(false);
    }

    return PSelectRsa::BoolResult::FromValue(isSuccess);
}
/// @brief Get the length of the encrypted block
/// @name  GetBlockSize
/// @returns Length of the encrypted block
uint32_t PSelectRsa::GetBlockSize() const noexcept { return static_cast< uint32_t >(Get_RsaKey_Size(pRsaKey_)); }
/// @brief Get RSA
/// @return RSA pointer
EVP_PKEY* PSelectRsa::GetRSA() const noexcept { return pRsaKey_; }
//***************/
/// @brief Get the key modulus length: different padding schemes in RSA correspond to different values
/// @name  _GetModulusSize
/// @param nPadding Data padding scheme
/// @returns Key modulus length
int32_t PSelectRsa::_GetModulusSize(int32_t const nPadding) const noexcept
{
    uint32_t nBlockLen{GetBlockSize()};
    switch (nPadding) {
        case RSA_NO_PADDING: {
        }
        case RSA_PKCS1_PADDING: {
            nBlockLen -= kInt_11U;
            break;
        }
        case RSA_PKCS1_OAEP_PADDING: {
            nBlockLen -= kInt_43U;
            break;
        } break;
        default: {
        } break;
    }
    return static_cast< int32_t >(nBlockLen);
}
/// @brief Execute logic via IPC
/// @name  DoRsaLogicIpc
/// @param stFuncName Function name used for IPC call
/// @param slotId Key slot ID
/// @param pInputData Starting address of input data in algorithm operation
/// @param nInDataLen Length of input data in algorithm operation
/// @param nPadding Data padding scheme
/// @return RSA IPC logic processing result
PSelectRsa::VecResult PSelectRsa::DoRsaLogicIpc(ara::core::StringView const& stFuncName,
                                                uint32_t const slotId,
                                                uint8_t const* const pInputData,
                                                uint32_t const nInDataLen,
                                                int32_t const nPadding) noexcept
{
    keys::isoft_def::PIpcClient* const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return PSelectRsa::VecResult::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }

    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient->DealIpcRequest(
        stFuncName, aswMsg,
        [slotId, pInputData, nInDataLen, nPadding](keys::isoft_def::PIpcAutoPacket const* const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_RsaLogic ipcReq;
                ipcReq.SetIpcSlotID(slotId);
                ipcReq.SetPadding(static_cast< uint32_t >(nPadding));
                ipcReq.SetDataLen(nInDataLen);
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(pInputData, static_cast< uint16_t >(nInDataLen));
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_RsaLogic) + nInDataLen);
        })};
    if (false == bDealIpc) {
        return PSelectRsa::VecResult::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    keys::isoft_def::cryptoctx::PIpcAsw_common* const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return PSelectRsa::VecResult::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    uint8_t const* const pData{pIpcAsw->GetData()};
    uint32_t const nDataLen{pIpcAsw->GetDataLen()};
    ara::core::Vector< ara::core::Byte > vecResult;
    try {
        vecResult.reserve(static_cast< std::size_t >(nDataLen));
    } catch (std::bad_alloc&) {
        return PSelectRsa::VecResult::FromError(SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    for (uint32_t i{0U}; i < nDataLen; ++i) {
        vecResult.push_back(static_cast< ara::core::Byte >(*(pData + i)));
    }
    return PSelectRsa::VecResult::FromValue(vecResult);
}
/// @brief Execute RSA logic
/// @name  _DoRsaLogic
/// @param pInputData Starting address of input data in algorithm operation
/// @param nDataLen Data length
/// @param nModulusSize Block size for large data chunking
/// @param cbFun Callback function for logic execution
/// @returns RSA logic processing result
PSelectRsa::VecResult PSelectRsa::_DoRsaLogic(uint8_t const* const pInputData,
                                              uint32_t const nDataLen,
                                              int32_t const nModulusSize,
                                              CB_RsaLogic const& cbFun) const noexcept
{
    // Prepare the output buffer
    uint32_t const nBlockSize{GetBlockSize()};
    internal::PAutoBuff* pLocalBuff{nullptr};
    if (nullptr == pBuffOutput_) {
        uint32_t const nBuffSize{(static_cast< uint32_t >(ESelectLength::kMaxOutputBuffLen) > nBlockSize)
                                     ? static_cast< uint32_t >(ESelectLength::kMaxOutputBuffLen)
                                     : nBlockSize};
        pLocalBuff = new internal::PAutoBuff(nBuffSize);
    }
    uint32_t const len{(nDataLen + static_cast< uint32_t >(nModulusSize) - 1U) / static_cast< uint32_t >(nModulusSize)
                       * nBlockSize};
    ara::core::Vector< ara::core::Byte > vecResult;
    try {
        vecResult.reserve(static_cast< std::size_t >(len));
    } catch (std::bad_alloc&) {
        return PSelectRsa::VecResult::FromError(SecurityErrorDomain::Errc::kInsufficientCapacity);
    }

    internal::PAutoBuff* const pWorkBuff{(pBuffOutput_ != nullptr) ? pBuffOutput_ : pLocalBuff};
    PH_ASSERT((pWorkBuff != nullptr) && (pWorkBuff->GetBuffLen() >= nBlockSize));
    // Chunk large data
    uint32_t nProcessLen{0U};
    while (true) {
        if (nProcessLen >= nDataLen) {
            break;
        }
        int32_t nWorkLen{static_cast< int32_t >(nDataLen) - static_cast< int32_t >(nProcessLen)};
        if (nWorkLen > nModulusSize) {
            nWorkLen = nModulusSize;
        }
        pWorkBuff->ResetData();
        // Decryption
        int32_t const nCipherLen{cbFun(static_cast< uint32_t >(nWorkLen), pInputData + nProcessLen, pWorkBuff->Data())};
        if (nCipherLen <= 0) {
            break;
        }
        pWorkBuff->SetDataSize(static_cast< uint32_t >(nCipherLen));
        // Save the decrypted data
        for (int32_t i{0}; i < nCipherLen; ++i) {
            vecResult.push_back(static_cast< ara::core::Byte >(pWorkBuff->at(static_cast< uint32_t >(i))));
        }
        nProcessLen += static_cast< uint32_t >(nWorkLen);
    }
    if (pLocalBuff != nullptr) {
        delete pLocalBuff;
        pLocalBuff = nullptr;
    }
    return PSelectRsa::VecResult::FromValue(vecResult);
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
