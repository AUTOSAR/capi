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
/// @file       secoc_crypto.h
/// @brief
/// @details
/// @date       2023-09-04
/// @author     shigang.zan
/// @version    1.2.0
///
/// ================================================================

#ifndef __SECOC_CRYPTO_H__
#define __SECOC_CRYPTO_H__

#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/entry_point.h"
#include "ara/crypto/common/io_interface.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/keys/isoft_updates_observer.h"
#include "ara/crypto/keys/key_storage_provider.h"
#include "ara/crypto/keys/keyslot.h"

namespace ara {
namespace com {
namespace internal {
namespace secoc {

/// @brief secoc crypto op
class SecOcCryptoOp
{
public:
    /// @brief construct func
    /// @param[in] keySlotName
    /// @param[in] algorithmName
    /// @param[in] cryptoSpecifier
    SecOcCryptoOp(ara::core::String keySlotName,
                  ara::core::String algorithmName,
                  ara::core::InstanceSpecifier cryptoSpecifier) noexcept
        : keySlotName_{std::move(keySlotName)}
        , algorithmName_{std::move(algorithmName)}
        , cryptoSpecifier_{std::move(cryptoSpecifier)}
        , initialized_{false} {};

    /// @brief initialization func
    /// @return true/false
    bool Initialize() noexcept;

    /// @brief Do crypto auth func
    /// @param[in] payload
    /// @param[in] length
    /// @param[in] macOutput
    /// @return true/false
    bool DoCrytoOperation(uint8_t const* const payload,
                          uint16_t const length,
                          ara::core::Vector< uint8_t >& macOutput) noexcept;

private:
    /// @brief key slot name
    ara::core::String keySlotName_;
    /// @brief algorithm name
    ara::core::String algorithmName_;
    /// @brief crypto specifier
    ara::core::InstanceSpecifier cryptoSpecifier_;
    /// @brief auth context
    ara::crypto::cryp::MessageAuthnCodeCtx::Uptr secOcCryptoCtx_;
    /// @brief key context
    ara::crypto::cryp::SymmetricKey::Uptrc keySrc_;
    /// @brief initialized or not
    bool initialized_;
};

/// @brief
class SecOCCryptoManager
{
public:
    /// @brief
    /// @param[in] keySlotName
    /// @param[in] algorithmName
    /// @param[in] dataId
    /// @return
    static bool Initialize(ara::core::String const& keySlotName,
                           ara::core::String const& algorithmName,
                           uint16_t dataId) noexcept
    {
        return Impl::Instance().ImplInitialize(keySlotName, algorithmName, dataId);
    }
    /// @brief
    /// @param[in] payload
    /// @param[in] length
    /// @param[in] macOutput
    /// @param[in] dataId
    /// @return
    static bool DoCryptoAuth(uint8_t const* const payload,
                             uint16_t const length,
                             ara::core::Vector< uint8_t >& macOutput,
                             uint16_t dataId) noexcept
    {
        return Impl::Instance().ImplDoCryptoAuth(payload, length, macOutput, dataId);
    }

private:
    /// @brief impl class
    class Impl
    {
    public:
        /// @brief get instance
        /// @return impl instance
        static Impl& Instance() noexcept
        {
            static Impl s_Instance;
            return s_Instance;
        }

        /// @brief
        /// @param[in] keySlotName
        /// @param[in] algorithmName
        /// @param[in] dataId
        /// @return bool
        bool ImplInitialize(ara::core::String const& keySlotName,
                            ara::core::String const& algorithmName,
                            uint16_t dataId) noexcept
        {
            ara::core::InstanceSpecifier specifier{"specifier"};
            std::shared_ptr< SecOcCryptoOp > const p{
                std::make_shared< SecOcCryptoOp >(keySlotName, algorithmName, specifier)};
            if (p->Initialize()) {
                cryptoOpMaps_[dataId] = p;
                return true;
            }
            return false;
        }

        /// @brief
        /// @param[in] payload
        /// @param[in] length
        /// @param[in] macOutput
        /// @param[in] dataId
        /// @return bool
        bool ImplDoCryptoAuth(uint8_t const* const payload,
                              uint16_t const length,
                              ara::core::Vector< uint8_t >& macOutput,
                              uint16_t dataId) noexcept
        {
            ara::core::Map< uint16_t, std::shared_ptr< SecOcCryptoOp > >::iterator const it{cryptoOpMaps_.find(dataId)};
            if (it != cryptoOpMaps_.end()) {
                return it->second->DoCrytoOperation(payload, length, macOutput);
            }
            return false;
        }

        /// @brief impl()
        Impl() noexcept = default;
        /// @brief ~Impl()
        virtual ~Impl() noexcept = default;
        /// @brief Impl()
        /// @param[in] other
        Impl(Impl const& other) noexcept = default;
        /// @brief Impl operator =
        /// @param[in] other
        /// @return Impl &
        Impl& operator=(Impl const& other) noexcept = default;
        /// @brief Impl()
        /// @param[in] other
        Impl(Impl&& other) noexcept = default;
        /// @brief Impl operartor =
        /// @param[in] other
        /// @return Impl &
        Impl& operator=(Impl&& other) noexcept = default;

    private:
        ara::core::Map< uint16_t, std::shared_ptr< SecOcCryptoOp > > cryptoOpMaps_;
    };
};

}  // namespace secoc
}  // namespace internal
}  // namespace com
}  // namespace ara

#endif
