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
/// @file       ids_sign_common.cpp
/// @brief      IDSM encryption base class
/// @details
/// @date       2023-01-13
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/IDS protocol
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0011
/// @unit_name=IdsmCrypto
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "ids_sign_common.h"

#include <iostream>
#ifdef ARA_WITH_CRYPTO
    #include "ara/crypto/common/io_interface.h"
    #include "ara/crypto/keys/keyslot.h"
#endif
#include "ids_log.h"
namespace ara {
namespace idsm {
#ifdef ARA_WITH_CRYPTO
/// @brief
/// @return
/// @throw
int32_t IdsmCrypto::_InitCrypto(ara::crypto::cryp::CryptoProvider::Uptr& provider,
                                ara::crypto::IOInterface::Uptr& interface)
{
    ara::crypto::keys::KeyStorageProvider::Uptr const keyStorageProvider{ara::crypto::LoadKeyStorageProvider()};
    if (keyStorageProvider.get() == nullptr) {
        IDS_LOG_ERROR << "ids protocol: signature fail, LoadKeyStorageProvider interface return error: nullptr";
        return -1;
    }

    ara::core::InstanceSpecifier instance{ara::core::StringView{crySlot_.c_str()}};
    /// @brief
    using KeySlotPtr = ara::crypto::keys::KeySlot::Uptr;
    ara::core::Result< KeySlotPtr > keySlotRes{keyStorageProvider->LoadKeySlot(instance)};
    if (!keySlotRes.HasValue()) {
        IDS_LOG_ERROR << "ids protocol: signature fail, load key slot:" << instance
                      << "fail. error: " << keySlotRes.Error().Message();
        return -1;
    }

    ara::crypto::keys::KeySlot::Uptr const keySlot{std::move(keySlotRes).Value()};
    /// @brief
    using IOInterfacePtr = ara::crypto::IOInterface::Uptr;
    ara::core::Result< IOInterfacePtr > ioInterfaceRes{keySlot->Open(false, false)};
    if (!ioInterfaceRes.HasValue()) {
        IDS_LOG_ERROR << "ids protocol: signature fail, open key slot:" << instance
                      << "fail. error: " << ioInterfaceRes.Error();
        return -1;
    }
    interface = std::move(ioInterfaceRes).Value();

    provider = ara::crypto::LoadCryptoProvider(ara::core::InstanceSpecifier{ara::core::StringView("isoft")});
    return 0;
}
/// @brief
/// @param data
/// @param size
/// @param digest
/// @throw
void IdsmCrypto::GenDigitDigest(std::uint8_t const* const data, size_t const size, AraBytesVec& digest)
{
    std::ignore = digest;
    ara::crypto::cryp::CryptoProvider::Uptr cryptoProvider{
        ara::crypto::LoadCryptoProvider(ara::core::InstanceSpecifier{ara::core::StringView("isoft")})};
    if (cryptoProvider.get() == nullptr) {
        IDS_LOG_FATAL << "ids protocol: crypto get isoft provider fail. provider is nullptr";
        return;
    }

    ara::crypto::cryp::CryptoContext::AlgId const algId{cryptoProvider->ConvertToAlgId("md5")};
    ara::core::Result< ara::crypto::cryp::HashFunctionCtx::Uptr > hashTempCtx{
        cryptoProvider->CreateHashFunctionCtx(algId)};
    ara::crypto::cryp::HashFunctionCtx::Uptr const hashCtx{std::move(hashTempCtx).Value()};
    ara::crypto::cryp::DigestService::Uptr const digestSrv{hashCtx->GetDigestService()};
    std::ignore = digestSrv;
    ara::core::Result< void > const startRes{hashCtx->Start()};
    if (!startRes.HasValue()) {
        IDS_LOG_ERROR << "ids protocol: signatrue fail, Start interface call error. " << startRes.Error().Message();
        return;
    }

    ara::crypto::ReadOnlyMemRegion const srcData{data, size};
    ara::core::Result< void > const updateRes{hashCtx->Update(srcData)};
    if (!updateRes.HasValue()) {
        IDS_LOG_ERROR << "ids protocol: signatrue fail, Update interface call error. " << updateRes.Error().Message();
        return;
    }

    ara::core::Result< AraBytesVec > const crcRes{hashCtx->Finish()};
    if (!crcRes.HasValue()) {
        IDS_LOG_ERROR << "ids protocol: signatrue fail, Finish interface call error. " << crcRes.Error().Message();
        return;
    }
    digest = crcRes.Value();
}
#endif

}  // namespace idsm
}  // namespace ara