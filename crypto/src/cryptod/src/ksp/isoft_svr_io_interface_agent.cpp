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
/// @file       isoft_svr_io_interface_agent.cpp
/// @brief      AutoSar-Crypto Common Encryption/Decryption Module
/// @details
/// @date       2022-04-12
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/IO Interface
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PSvrIoInterfaceAgent
/// @unit_description=Server-side proxy interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_svr_io_interface_agent.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Formal interface of IOInterface for saving and loading security objects.
/// Actual saving and loading should be implemented through known internal methods of the trusted pair consisting of the crypto provider and storage provider.
/// Each object should be uniquely identified by its type and Crypto Object Unique Identifier (COUID). This interface assumes objects in the container are compressed, i.e., have a minimally optimized size.
//********************************/
/// @brief Initializes static members
IoAgentIndex PSvrIoInterfaceAgent::s_IoAgentIndex_{};  // NOLINT
/// @brief IO interface proxy
/// @param bWriteModal Whether write mode
/// @param nSlotID Key slot ID
/// @param pMainKvInterface KV storage IO interface
PSvrIoInterfaceAgent::PSvrIoInterfaceAgent(bool const bWriteModal,
                                           uint32_t const nSlotID,
                                           PIoInterface_KvShare* const pMainKvInterface) noexcept
    : bWriteModal_{bWriteModal}, nSlotID_{nSlotID}, pMainKvInterface_{pMainKvInterface}
{
    nIoInterfaceID_ = s_IoAgentIndex_.GetIoAgentIndex();
    if (bWriteModal_) {
        pMemInterface_ = std::make_unique< PIoInterface_Mem_Server >(pMainKvInterface_->IsVolatile());
        std::ignore    = pMemInterface_->InitIoInterface(pMainKvInterface_->GetKeySlotName());
        // Initialize memory IO data here using KV store content
        std::ignore = pMemInterface_->SaveKeyContent(pMainKvInterface_->GetKeyContent());
        std::ignore = pMemInterface_->SaveSlotProps(pMainKvInterface_->GetSlotProps());

        CryptoAlgId nKeyAlgID{kAlgIdAny};  // Algorithm ID of the encryption key
        ara::core::Vector< uint8_t > vecKeyKey;
        std::ignore = pMainKvInterface_->ReadKeyKey(nKeyAlgID, vecKeyKey);
        ara::core::Vector< uint8_t > vecKeyData;
        std::ignore = pMainKvInterface_->ReadKeyData(vecKeyData);
        uint8_t* const pKeyData{vecKeyData.data()};
        uint8_t* const pKeyKeyData{vecKeyKey.data()};
        std::ignore = pMemInterface_->SaveKeyData(ReadOnlyMemRegion(pKeyData, vecKeyData.size()), nKeyAlgID,
                                                  ReadOnlyMemRegion(pKeyKeyData, vecKeyKey.size()));
        std::ignore = pMemInterface_->CommitSaveAction();
    }
}

/// @brief Full-parameter constructor
/// @param bWriteModal Whether write mode
/// @param nSlotID Key slot ID
/// @param nIoInterfaceID IO interface ID
/// @param pMainKvInterface KV storage IO interface
/// @param pMemInterface Memory IO interface
PSvrIoInterfaceAgent::PSvrIoInterfaceAgent(bool const bWriteModal,
                                           uint32_t const nSlotID,
                                           uint32_t const nIoInterfaceID,
                                           PIoInterface_KvShare* const pMainKvInterface,
                                           PIoInterface_Mem_Server::Uptr pMemInterface) noexcept
    : bWriteModal_{bWriteModal}
    , nSlotID_{nSlotID}
    , nIoInterfaceID_{nIoInterfaceID}
    , pMainKvInterface_{pMainKvInterface}
    , pMemInterface_{std::move(pMemInterface)}
{
}
/// @brief Gets the corresponding IO interface
/// @return PIoInterface pointer
PIoInterface* PSvrIoInterfaceAgent::GetIoInterface() const noexcept
{
    if (bWriteModal_) {
        return pMemInterface_.get();
    }
    return pMainKvInterface_;
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
