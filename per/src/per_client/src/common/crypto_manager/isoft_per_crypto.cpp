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
/// @file       isoft_per_crypto.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Encryption and decryption operation class used by the PER persistence module
/// @date       2022-12-22
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Encryption Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PPerCrypto
/// @unit_description=Encryption and decryption operation class used by the PER persistence module
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2022-12-22 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/internal/crypto/isoft_per_crypto.h"
#ifdef ARA_WITH_CRYPTO
    #include <ara/crypto/common/entry_point.h>
    #include <ara/crypto/cryp/crypto_provider.h>
    #include <ara/crypto/cryp/hash_function_ctx.h>
    #include <ara/crypto/keys/key_storage_provider.h>
#endif
#include "ara/per/internal/common/isoft_file_opt_page.h"
#include "ara/per/internal/isoftkv/nai_map_buff.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @traceid {SWS_PER_00451}
/// @brief Create the corresponding derived class
/// @return
std::unique_ptr< IPerCrypto > NewPerCrypto() noexcept { return std::make_unique< PPerCrypto >(); }
/// @brief Load the corresponding key slot
/// @param stKeySlotName
/// @return true on success, false on error
bool PPerCrypto::LoadCrypto(ara::core::String const &stKeySlotName, ara::core::String const &stAlgorithmName) noexcept
{
#ifdef ARA_WITH_CRYPTO
    ara::crypto::keys::KeyStorageProvider::Uptr pKeyProvider;
    pKeyProvider = ara::crypto::LoadKeyStorageProvider();
    // Load key slot
    ara::core::InstanceSpecifier iSpecify{stKeySlotName};
    ara::core::Result< ara::crypto::keys::KeySlot::Uptr > resultLoad{pKeyProvider->LoadKeySlot(iSpecify)};
    if (false == resultLoad.HasValue()) {
        return false;
    }
    ara::crypto::keys::KeySlot::Uptr pKeySlot{std::move(resultLoad).Value()};
    ara::core::Result< ara::crypto::IOInterface::Uptr > resultOpenSlot{pKeySlot->Open()};
    if (false == resultOpenSlot.HasValue()) {
        return false;
    }
    if (pKeySlot->IsEmpty()) {
        return false;
    }
    ara::crypto::IOInterface::Uptr const pKeyInterface{std::move(resultOpenSlot).Value()};
    ara::core::Result< ara::crypto::cryp::CryptoProvider::Uptr > resultCryptoProvider{pKeySlot->MyProvider()};
    if (false == resultCryptoProvider.HasValue()) {
        return false;
    }
    ara::crypto::cryp::CryptoProvider::Uptr pCryptoProvider{std::move(std::move(resultCryptoProvider).Value())};
    stKeySlotName_   = stKeySlotName;
    stAlgorithmName_ = stAlgorithmName;
    pKeySlot_        = std::move(pKeySlot);
    // Load potentially existing symmetric key: the key may not be symmetric within the slot (any encryption material is allowed during verification, symmetric key is recommended)
    ara::core::Result< ara::crypto::cryp::SymmetricKey::Uptrc > resultKey{
        pCryptoProvider->LoadSymmetricKey(*pKeyInterface)};
    if (true == resultKey.HasValue()) {
        pSymmetricKey_ = std::move(resultKey).Value();
    }
    pCryptoProvider_ = std::move(pCryptoProvider);
    return true;
#else
    std::ignore = stKeySlotName;
    std::ignore = stAlgorithmName;
    return false;
#endif
}
/// @brief Whether the key slot has finished loading
/// @return true on completion, false on error
bool PPerCrypto::IsLoadKeyLoad() const noexcept
{
#ifdef ARA_WITH_CRYPTO
    if (nullptr == pSymmetricKey_) {
        return false;
    }
    return false == stKeySlotName_.empty();
#else
    return false;
#endif
}
/// @brief Get the length of the key in the key slot
/// @return
uint32_t PPerCrypto::GetKeySlotObjectSize() const noexcept
{
#ifdef ARA_WITH_CRYPTO
    if (false == IsLoadKeyLoad()) {
        return 0U;
    }
    ara::core::Result< ara::crypto::keys::KeySlotContentProps > const resultGet{pKeySlot_->GetContentProps()};
    PER_Assert(resultGet.HasValue());
    if (false == resultGet.HasValue()) {
        return 0U;
    }
    return static_cast< uint32_t >(resultGet.Value().mObjectSize);
#else
    return 0U;
#endif
}
/// @traceid {SWS_PER_00210, SWS_PER_00211}
/// @brief Perform encryption or decryption operation on the input data
/// @param vecData Input data, also the output data
/// @param bEncodeOrDecode true for encryption, false for decryption
/// @return Number of bytes encrypted/decrypted
int32_t PPerCrypto::CryptoData(ara::core::Vector< ara::core::Byte > &vecData, bool const bEncodeOrDecode) const noexcept
{
#ifdef ARA_WITH_CRYPTO
    if (false == IsLoadKeyLoad()) {
        return -1;
    }
    ara::crypto::CryptoTransform transForm{ara::crypto::CryptoTransform::kEncrypt};
    if (false == bEncodeOrDecode) {
        transForm = ara::crypto::CryptoTransform::kDecrypt;
    }
    ara::crypto::cryp::SymmetricBlockCipherCtx::Uptr const pCryptoCtx{_MakeSymmetricCtx(transForm)};
    if (false == pCryptoCtx.operator bool()) {
        return -1;
    }
    uint8_t const *const pBuffIn{T_TransBytes(vecData.data())};
    ara::crypto::ReadOnlyMemRegion const memIn{pBuffIn, vecData.size()};
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultProcess{pCryptoCtx->ProcessBlocks(memIn)};
    if (false == resultProcess.HasValue()) {
        return -1;
    }
    vecData = resultProcess.Value();
    return static_cast< int32_t >(vecData.size());
#else
    std::ignore = vecData;
    std::ignore = bEncodeOrDecode;
    return -1;
#endif
}
/// @brief Perform encryption or decryption operation on the input data
/// @param pData Input data, also the output data
/// @param nLen Input data length, also the output data length
/// @param bEncodeOrDecode true for encryption, false for decryption
/// @return Number of bytes encrypted/decrypted, -1 indicates error
int32_t PPerCrypto::CryptoData(uint8_t *const pData, uint32_t const nLen, bool const bEncodeOrDecode) const noexcept
{
#ifdef ARA_WITH_CRYPTO
    if (false == IsLoadKeyLoad()) {
        return -1;
    }
    ara::crypto::CryptoTransform transForm{ara::crypto::CryptoTransform::kEncrypt};
    if (false == bEncodeOrDecode) {
        transForm = ara::crypto::CryptoTransform::kDecrypt;
    }
    ara::crypto::cryp::SymmetricBlockCipherCtx::Uptr const pCryptoCtx{_MakeSymmetricCtx(transForm)};
    if (false == pCryptoCtx.operator bool()) {
        return -1;
    }
    ara::crypto::ReadOnlyMemRegion const memIn{pData, static_cast< std::size_t >(nLen)};
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultProcess{pCryptoCtx->ProcessBlocks(memIn)};
    if (false == resultProcess.HasValue()) {
        return -1;
    }
    ara::core::Vector< ara::core::Byte > const &vecData{resultProcess.Value()};
    uint32_t const nCopyLen{std::min< uint32_t >(static_cast< uint32_t >(vecData.size()), nLen)};
    if (nCopyLen > 0U) {
        std::ignore = T_Memcpy(pData, vecData.data(), nCopyLen);
    }
    return static_cast< int32_t >(nCopyLen);
#else
    std::ignore = pData;
    std::ignore = nLen;
    std::ignore = bEncodeOrDecode;
    return -1;
#endif
}
/// @brief Perform hash operation on a file
/// @param stFileName File name
/// @param stHashCtx Hash algorithm string identifier
/// @return Output file hash code
ara::core::Vector< uint8_t > PPerCrypto::CryptoFileHash(ara::core::StringView const &stFileName,
                                                        ara::core::StringView const &stHashCtx) const noexcept
{
    ara::core::Vector< uint8_t > vecOut;
#ifdef ARA_WITH_CRYPTO
    if (false == IsLoadKeyLoad()) {
        return vecOut;
    }
    ara::crypto::CryptoAlgId nAlgIdHash{0U};
    if (false == stHashCtx.empty()) {
        nAlgIdHash = pCryptoProvider_->ConvertToAlgId(stHashCtx);
    } else {  // Obtain the corresponding algorithm ID from the KeySlot
        ara::core::Result< crypto::keys::KeySlotContentProps > resultSlotContentProps{pKeySlot_->GetContentProps()};
        PER_Assert(resultSlotContentProps.HasValue());
        nAlgIdHash = resultSlotContentProps.Value().mAlgId;
    }
    // Create a Hash context for calculating MD5
    ara::core::Result< ara::crypto::cryp::HashFunctionCtx::Uptr > resultHashCtx{
        pCryptoProvider_->CreateHashFunctionCtx(nAlgIdHash)};
    if (false == resultHashCtx.HasValue()) {
        return vecOut;
    }
    constexpr uint32_t kPageLen{4096U};                  // 2022-12-27 Uniformly read according to 4K page size
    ara::per::isoftkv::PFileOpt_Page fileOpt{kPageLen};  // All files are read page by page like the Kv library
    if (false == fileOpt.DoPrepareWork(stFileName, kPageLen)) {
        if (EINVAL != PFileOpt_Page::GetLastError()) {
            return vecOut;
        }
        // Some development boards do not support direct file reading
        uint32_t const nNewFileFlags{
            T_AndData< uint32_t >(fileOpt.GetFileFlags(), static_cast< uint32_t >(~NAI_O_DIRECT))};
        fileOpt.SetFileFlags(nNewFileFlags);
        if (false == fileOpt.DoPrepareWork(stFileName, kPageLen)) {
            return vecOut;
        }
    }
    // Start Hash
    ara::crypto::cryp::HashFunctionCtx::Uptr const pHashFunctionCtx{std::move(resultHashCtx).Value()};
    std::ignore = pHashFunctionCtx->Start();
    // Read the file page by page, decrypt page by page
    uint32_t const nPageCount{fileOpt.GetPageTotal()};
    PNaiMapBuff naiMapBuff{kPageLen};
    for (uint32_t i{0U}; i < nPageCount; i++) {
        naiMapBuff.ResetBuffData(0U);
        int32_t const nBufLen{fileOpt.ReadPage(i + 1U, naiMapBuff.GetBuff(), naiMapBuff.GetLen())};
        if (nBufLen <= 0) {
            break;
        }
        ara::crypto::ReadOnlyMemRegion const hashDataIn{T_TransBytes(naiMapBuff.GetBuff()),
                                                        static_cast< std::size_t >(nBufLen)};
        // Calculate Hash
        std::ignore = pHashFunctionCtx->Update(hashDataIn);
    }
    // Aggregate hash result
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultHashResult{pHashFunctionCtx->Finish()};
    ara::core::Vector< ara::core::Byte > const &vecResult{resultHashResult.Value()};
    vecOut.reserve(vecResult.size());
    for (ara::core::Byte const &byData : vecResult) {
        vecOut.push_back(static_cast< uint8_t >(byData));
    }
#else
    std::ignore = stFileName;
    std::ignore = stHashCtx;
#endif
    return vecOut;
}
/// @traceid {SWS_PER_00449, SWS_PER_00450}
/// @brief Perform hash operation on a file
/// @param fileOpt File operation object
/// @param stHashCtx Hash algorithm string identifier
/// @return Output file hash code
ara::core::Vector< uint8_t > PPerCrypto::CryptoFileHash(isoftkv::PFileOpt const &fileOpt,
                                                        ara::core::StringView const &stHashCtx) const noexcept
{
    ara::core::Vector< uint8_t > vecOut;
#ifdef ARA_WITH_CRYPTO
    if (false == IsLoadKeyLoad()) {
        return vecOut;
    }
    ara::crypto::CryptoAlgId nAlgIdHash{0U};
    if (false == stHashCtx.empty()) {
        nAlgIdHash = pCryptoProvider_->ConvertToAlgId(stHashCtx);
    } else {  // Obtain the corresponding algorithm ID from the KeySlot
        ara::core::Result< crypto::keys::KeySlotContentProps > resultSlotContentProps{pKeySlot_->GetContentProps()};
        PER_Assert(resultSlotContentProps.HasValue());
        nAlgIdHash = resultSlotContentProps.Value().mAlgId;
    }
    // Create a Hash context for calculating MD5
    ara::core::Result< ara::crypto::cryp::HashFunctionCtx::Uptr > resultHashCtx{
        pCryptoProvider_->CreateHashFunctionCtx(nAlgIdHash)};
    if (false == resultHashCtx.HasValue()) {
        return vecOut;
    }
    constexpr uint32_t kPageLen{4096U};  // 2022-12-27 Uniformly read according to 4K page size
    // Start Hash
    ara::crypto::cryp::HashFunctionCtx::Uptr const pHashFunctionCtx{std::move(resultHashCtx).Value()};
    std::ignore = pHashFunctionCtx->Start();
    // Read the file page by page, decrypt page by page
    fileOpt.SeekPos(EFileSeekType::kSeekSet, 0);
    PNaiMapBuff naiMapBuff{kPageLen};
    while (false == fileOpt.IsEof()) {
        naiMapBuff.ResetBuffData(0U);
        int32_t const nBufLen{fileOpt.ReadData(naiMapBuff.GetBuff(), naiMapBuff.GetLen())};
        if (nBufLen <= 0) {
            break;
        }
        ara::crypto::ReadOnlyMemRegion const hashDataIn{T_TransBytes(naiMapBuff.GetBuff()),
                                                        static_cast< std::size_t >(nBufLen)};
        // Calculate Hash
        std::ignore = pHashFunctionCtx->Update(hashDataIn);
    }
    // Aggregate hash result
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultHashResult{pHashFunctionCtx->Finish()};
    ara::core::Vector< ara::core::Byte > const &vecResult{resultHashResult.Value()};
    vecOut.reserve(vecResult.size());
    for (ara::core::Byte const &byData : vecResult) {
        vecOut.push_back(static_cast< uint8_t >(byData));
    }
#else
    std::ignore = fileOpt;
    std::ignore = stHashCtx;
#endif
    return vecOut;
}
/// @brief Perform hash operation on data
/// @param pData Data buffer
/// @param nDataLen Data length
/// @param stHashCtx Hash algorithm string identifier
/// @return Output file hash code
ara::core::Vector< uint8_t > PPerCrypto::CryptoDataHash(const uint8_t *const pData,
                                                        uint32_t const nDataLen,
                                                        ara::core::StringView const &stHashCtx) const noexcept
{
    ara::core::Vector< uint8_t > vecOut;
#ifdef ARA_WITH_CRYPTO
    if (false == IsLoadKeyLoad()) {
        return vecOut;
    }
    ara::crypto::CryptoAlgId nAlgIdHash{0U};
    if (false == stHashCtx.empty()) {
        nAlgIdHash = pCryptoProvider_->ConvertToAlgId(stHashCtx);
    } else {  // Obtain the corresponding algorithm ID from the KeySlot
        ara::core::Result< crypto::keys::KeySlotContentProps > resultSlotContentProps{pKeySlot_->GetContentProps()};
        PER_Assert(resultSlotContentProps.HasValue());
        nAlgIdHash = resultSlotContentProps.Value().mAlgId;
    }
    // Create a Hash context for calculating MD5
    ara::core::Result< ara::crypto::cryp::HashFunctionCtx::Uptr > resultHashCtx{
        pCryptoProvider_->CreateHashFunctionCtx(nAlgIdHash)};
    if (false == resultHashCtx.HasValue()) {
        return vecOut;
    }
    constexpr uint32_t kPageLen{4096U};  // 2022-12-27 Uniformly read according to 4K page size
    // Start Hash
    ara::crypto::cryp::HashFunctionCtx::Uptr const pHashFunctionCtx{std::move(resultHashCtx).Value()};
    std::ignore = pHashFunctionCtx->Start();
    PNaiMapBuff naiMapBuff{kPageLen};
    ara::crypto::ReadOnlyMemRegion const hashDataIn{pData, static_cast< std::size_t >(nDataLen)};
    // Calculate Hash
    std::ignore = pHashFunctionCtx->Update(hashDataIn);
    // Aggregate hash result
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultHashResult{pHashFunctionCtx->Finish()};
    ara::core::Vector< ara::core::Byte > const &vecResult{resultHashResult.Value()};
    vecOut.reserve(vecResult.size());
    for (ara::core::Byte const &byData : vecResult) {
        vecOut.push_back(static_cast< uint8_t >(byData));
    }
#else
    std::ignore = pData;
    std::ignore = nDataLen;
    std::ignore = stHashCtx;
#endif
    return vecOut;
}
//********************************/
/// @brief Create an encryption algorithm context and set the key
/// @param transForm Encryption direction: encrypt or decrypt
/// @return Encryption algorithm context
#ifdef ARA_WITH_CRYPTO
ara::crypto::cryp::SymmetricBlockCipherCtx::Uptr PPerCrypto::_MakeSymmetricCtx(
    ara::crypto::CryptoTransform const transForm) const noexcept
{
    if (false == IsLoadKeyLoad()) {
        return ara::crypto::cryp::SymmetricBlockCipherCtx::Uptr{nullptr};
    }
    ara::crypto::CryptoAlgId nAlgID{0U};
    if (false == stAlgorithmName_.empty()) {
        nAlgID = pCryptoProvider_->ConvertToAlgId(T_StringView(stAlgorithmName_));
    } else {  // 2011 Obtain the encryption algorithm identifier from the key slot
        nAlgID = pKeySlot_->GetContentProps().Value().mAlgId;
    }
    ara::core::Result< ara::crypto::cryp::SymmetricBlockCipherCtx::Uptr > resultCryptoCtx{
        pCryptoProvider_->CreateSymmetricBlockCipherCtx(nAlgID)};
    if (false == resultCryptoCtx.HasValue()) {
        return ara::crypto::cryp::SymmetricBlockCipherCtx::Uptr{nullptr};
    }
    std::ignore = resultCryptoCtx.Value()->SetKey(*pSymmetricKey_, transForm);
    return std::move(resultCryptoCtx).Value();
}
#endif

}  // namespace isoftkv
}  // namespace per
}  // namespace ara
