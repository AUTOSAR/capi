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
/// @file       ipc_key_provider.h
/// @brief      AutoSar-Crypto key storage module
/// @details    Key storage provider interface.
/// @date       2022-01-13
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Key Component/Key Provider
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_05001
/// @unit_name=PIpcKeyProvider
/// @unit_description=Key Provider
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_IPC_KEY_PROVIDER_H_
#define ARA_CRYPTO_KEYS_IPC_KEY_PROVIDER_H_
#include <ara/core/map.h>

#include <functional>

#include "ara/crypto/common/entry_point.h"
#include "ara/crypto/common/keys/ipc_key_provider.h"
#include "ara/crypto/internal/initialize.h"
#include "ara/crypto/ipc/isoft_ipc_client.h"
#include "ara/crypto/keys/isoft_ipc_key_provider.h"
#include "ara/crypto/keys/key_storage_provider.h"
#include "ara/crypto/manifest/manifest_instance.h"

namespace ara {
namespace crypto {
namespace keys {
//********************************/    //- @interface IpcKeyProvider : isoft_def version of key management provider
/// @brief Key storage provider interface. Any object is uniquely identified by the combination of its UUID and type.
///         HSMs/TPMs that implement the "non-extractable key" concept shall use their own copy of externally provided crypto objects. Some software crypto providers can share a single key slot if they support the same format.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00016
/// @trace_id_dd=DD_CRYPTO_00485
/// @needwork = ad
/// @endcode
template < typename T_IpcKP_Ctx, typename T_CryptoProvider_Ctx >
class IpcKeyProvider : public KeyStorageProvider
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00017
    /// @trace_id_dd=DD_CRYPTO_00486
    /// @needwork = ad
    /// @endcode
    IpcKeyProvider() noexcept : KeyStorageProvider{}, pIpcKeyProvider_{std::make_unique< T_IpcKP_Ctx >()} {}

    /// @brief Constructor
    /// @param pIpcKeyProvider Can be: default key provider, HSM_NXP key provider, HSM_BST key provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04333
    /// @trace_id_dd=DD_CRYPTO_08684
    /// @needwork = ad
    /// @endcode
    explicit IpcKeyProvider(typename T_IpcKP_Ctx::Uptr&& pIpcKeyProvider) noexcept
        : KeyStorageProvider{}, pIpcKeyProvider_{std::move(pIpcKeyProvider)}
    {
    }

    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00019
    /// @trace_id_dd=DD_CRYPTO_00488
    /// @needwork = ad
    /// @endcode
    ~IpcKeyProvider() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00020
    /// @trace_id_dd=DD_CRYPTO_00489
    /// @needwork = ad
    /// @endcode
    IpcKeyProvider(IpcKeyProvider const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00021
    /// @trace_id_dd=DD_CRYPTO_00490
    /// @needwork = ad
    /// @endcode
    IpcKeyProvider(IpcKeyProvider&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00022
    /// @trace_id_dd=DD_CRYPTO_00491
    /// @needwork = ad
    /// @endcode
    IpcKeyProvider& operator=(IpcKeyProvider const& other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00023
    /// @trace_id_dd=DD_CRYPTO_00492
    /// @needwork = ad
    /// @endcode
    IpcKeyProvider& operator=(IpcKeyProvider&& other) = delete;

public:
    /// @brief Load key slots. These functions load the information associated with KeySlot into the KeySlot object.
    /// @brief Load a key slot. The functions loads the information associated with a KeySlot into a KeySlot object.
    /// @param iSpecify  the target key-slot instance specifier
    /// @returns an unique smart pointer to allocated key slot
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnreservedResource  if the InstanceSpecifier is incorrect : the slot is not
    /// allocated
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30115}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00024
    /// @trace_id_dd=DD_CRYPTO_00493
    /// @needwork = ad
    /// @endcode
    ara::core::Result< KeySlot::Uptr > LoadKeySlot(ara::core::InstanceSpecifier& iSpecify) noexcept override
    {
        PH_CheckInit_RetErr(ara::core::Result< KeySlot::Uptr >);

        ara::core::String const stSlotName{
            manifest::PManifestInstance::Get()->TransName_PortToSlot(iSpecify.ToString())};
        if (stSlotName.empty()) {
            return ara::core::Result< KeySlot::Uptr >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
        }
        ara::crypto::isoft_def::LogInfo() << "stSlotName =" << stSlotName.data();
        /// No provider found, return a default provider
        uint32_t const cryptoProviderCode{_FindProvider(stSlotName)};
        if (static_cast< uint32_t >(IpcKeyProviderCode::kIpcKeyProvider) == cryptoProviderCode) {
            return GetIpcKeyProvider()->LoadKeySlot(iSpecify);
        }

        /// @errors: SWS_CRYPT_10005 kAccessViolation
        return ara::core::Result< KeySlot::Uptr >::FromError(SecurityErrorDomain::Errc::kAccessViolation);
    }

public:
    /// @brief Start a new transaction for updating key slots.
    ///        To make a keyslot part of a transaction scope, the reserveSpareSlot model parameter of the keyslot must be set to true.
    ///        Transactions are dedicated to updating related key slots simultaneously (in an all-or-nothing atomic manner). All key slots that should be updated by the transaction must be opened and provided to this function.
    ///        Any changes to slots within the scope are executed by calling commit().
    /// @brief Begin new transaction for key slots update.
    ///         In order for a keyslot to be part of a transaction scope, the reserveSpareSlot model parameter of the
    ///         keyslot has to be set to true. A transaction is dedicated for updating related key slots simultaneously
    ///         (in an atomic, all-or-nothing, way). All key slots that should be updated by the transaction have to be
    ///         opened and provided to this function. Any changes to the slots in scope are executed by calling
    ///         commit().
    /// @param targetSlots  a list of KeySlots that should be updated during this transaction.
    /// @returns a unique ID assigned to this transaction
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30123}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kUnreservedResource     if @c targetSlots list has a slot that has not been
    ///        configured with the reserveSpareSlot parameter in the manifest
    /// @error: SecurityErrorDomain::kBusyResource           if @c targetSlots list has key slots that are already
    ///        involved to another pending transaction or opened in writing mode
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00025
    /// @trace_id_dd=DD_CRYPTO_00494
    /// @trace_id_sr=SR_CRYPTO_05002
    /// @needwork = ad
    /// @endcode
    ara::core::Result< TransactionId > BeginTransaction(TransactionScope const& targetSlots) noexcept override
    {
        PH_CheckInit_RetErr(ara::core::Result< TransactionId >);
        if (targetSlots.empty()) {
            return ara::core::Result< TransactionId >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
        }

        ara::core::Result< cryp::CryptoProvider::Uptr > resProvider{targetSlots[0U]->MyProvider()};
        if (!resProvider.HasValue()) {
            return ara::core::Result< TransactionId >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
        }

        // Code of the 0th
        uint32_t const cryptoProviderCode{FindProvider(resProvider.Value().get())};
        if (cryptoProviderCode == static_cast< uint32_t >(IpcKeyProviderCode::kFaultIpcKeyProvider)) {
            return ara::core::Result< TransactionId >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
        }

        // Traverse targetSlots to see if they belong to the same group of keyslots
        for (std::size_t i{1U}; i < targetSlots.size(); i++) {
            resProvider = targetSlots[i]->MyProvider();
            if (FindProvider(resProvider.Value().get()) != cryptoProviderCode) {
                return ara::core::Result< TransactionId >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
            }
        }

        // Return the corresponding start transaction function according to the Code
        if (static_cast< uint32_t >(IpcKeyProviderCode::kIpcKeyProvider) == cryptoProviderCode) {
            return GetIpcKeyProvider()->BeginTransaction(targetSlots);
        }

        return ara::core::Result< TransactionId >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    /// @brief Commit the changes of the transaction to the Key Storage.
    ///        Any changes to key slots are invisible during transaction execution. The commit command permanently saves all changes made during the transaction in the Key Storage.
    /// @brief Commit changes of the transaction to Key Storage.
    ///         Any changes of key slots made during a transaction are invisible up to the commit execution.
    ///         The commit command permanently saves all changes made during the transaction in Key Storage
    /// @param id  an ID of a transaction that should be commited
    /// @return has value if CommitTransaction sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30124}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if provided @c id is invalid, i.e. this ID is unknown or
    /// correspondent transaction already was finished (commited or rolled back)
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00026
    /// @trace_id_dd=DD_CRYPTO_00495
    /// @trace_id_sr=SR_CRYPTO_05002
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > CommitTransaction(TransactionId id) noexcept override
    {
        PH_CheckInit_RetErr(ara::core::Result< void >);
        // If isoft
        if ((id & GetIpcKeyProvider()->GetTransactionIndexPrefix())
            == GetIpcKeyProvider()->GetTransactionIndexPrefix()) {
            return GetIpcKeyProvider()->CommitTransaction(id);
        }

        return ara::core::Result< void >(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    /// @brief Roll back all changes performed during the transaction in the key storage.
    ///        The rollback command permanently cancels all changes made during the transaction in the key storage. Rolling back a transaction is completely invisible to all applications.
    /// @brief Rollback all changes executed during the transaction in Key Storage.
    ///          The rollback command permanently cancels all changes made during the transaction in Key Storage.
    ///          A rolled back transaction is completely invisible for all applications.
    /// @param id  an ID of a transaction that should be rolled back
    /// @return has value if RollbackTransaction sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30125}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if provided @c id is invalid, i.e. this ID is unknown or
    /// correspondent transaction already was finished (commited or rolled back)
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00027
    /// @trace_id_dd=DD_CRYPTO_00496
    /// @trace_id_sr=SR_CRYPTO_05002
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > RollbackTransaction(TransactionId id) noexcept override
    {
        PH_CheckInit_RetErr(ara::core::Result< void >);
        // If isoft
        if ((id & GetIpcKeyProvider()->GetTransactionIndexPrefix())
            == GetIpcKeyProvider()->GetTransactionIndexPrefix()) {
            return GetIpcKeyProvider()->RollbackTransaction(id);
        }

        return ara::core::Result< void >(SecurityErrorDomain::Errc::kInvalidArgument);
    }

public:
    /// @brief Get the pointer to the registered update observer. If no observer has been registered yet, this method will return nullptr!
    /// Unique pointer to the registered update observer interface (returns a copy of the internal unique pointer, i.e., the key storage provider continues to retain ownership)
    /// @brief Get pointer of registered Updates Observer. The method returns @c nullptr if no observers have been
    /// registered yet!
    /// @returns    unique pointer to the registered Updates Observer interface (copy of an internal unique pointer is
    /// returned, i.e. the %Key Storage provider continues to keep the ownership)
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30131}
    /// @uptrace={RS_CRYPTO_02401}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00028
    /// @trace_id_dd=DD_CRYPTO_00497
    /// @trace_id_sr=SR_CRYPTO_05003
    /// @needwork = ad
    /// @endcode
    UpdatesObserver::Uptr GetRegisteredObserver() const noexcept override
    {
        PH_CheckInit_RetValue(nullptr);
        if (pIpcKeyProvider_.operator bool()) {
            return pIpcKeyProvider_->GetRegisteredObserver();
        }

        return {nullptr};
    }
    /// @brief Consumer registers an update observer.
    ///        Only one UpdatesObserver instance can be registered per application process, so this method always unregisters the previous observer and returns its unique pointer.
    ///        If (nullptr == observer), then this method only unregisters the previous observer! If no observer has been registered, this method will return nullptr!
    /// @brief Register consumer Updates Observer.
    ///         Only one instance of the @c UpdatesObserver may be registered by an application process,
    ///         therefore this method always unregister previous observer and return its unique pointer.
    ///         If (nullptr == observer) then the method only unregister the previous observer! The method returns @c
    ///         nullptr if no observers have been registered yet!
    /// @param observer  optional pointer to a client-supplied @c UpdatesObserver instance that should be registered
    /// inside %Key Storage implementation and called every time, when an opened for usage/loading key slot is updated
    /// externally (by its "Owner" application)
    /// @returns unique pointer to previously registered Updates Observer interface (the pointer ownership is "moved
    ///           out" to the caller code)
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30130}
    /// @uptrace={RS_CRYPTO_02401}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00029
    /// @trace_id_dd=DD_CRYPTO_00498
    /// @trace_id_sr=SR_CRYPTO_05003
    /// @needwork = ad
    /// @endcode
    UpdatesObserver::Uptr RegisterObserver(UpdatesObserver::Uptr observer = {nullptr}) noexcept override
    {
        PH_CheckInit_RetValue(nullptr);
        if (pIpcKeyProvider_.operator bool()) {
            return pIpcKeyProvider_->RegisterObserver(std::move(observer));
        }

        return {nullptr};
    }
    /// @brief Unsubscribe the update observer from change monitoring of the specified slot.
    /// @brief Unsubscribe the Update Observer from changes monitoring of the specified slot.
    /// @param slot  number of a slot that should be unsubscribed from the updates observing
    /// @return has value if UnsubscribeObserver sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30126}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if the specified slot is not monitored now (i.e. if it was not
    /// successfully opened via @c OpenAsUser() or it was already unsubscribed by this method)
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00030
    /// @trace_id_dd=DD_CRYPTO_00499
    /// @trace_id_sr=SR_CRYPTO_05003
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > UnsubscribeObserver(KeySlot& slot) noexcept override
    {
        PH_CheckInit_RetErr(ara::core::Result< void >);
        ara::core::Result< cryp::CryptoProvider::Uptr > const resProvider{slot.MyProvider()};
        if (!resProvider.HasValue()) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
        }

        uint32_t const cryptoProviderCode{FindProvider(resProvider.Value().get())};
        if (cryptoProviderCode == static_cast< uint32_t >(IpcKeyProviderCode::kFaultIpcKeyProvider)) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
        }

        // Return the corresponding unsubscribe update observer according to the Code.
        if (static_cast< uint32_t >(IpcKeyProviderCode::kIpcKeyProvider) == cryptoProviderCode) {
            return pIpcKeyProvider_->UnsubscribeObserver(slot);
        }

        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }

public:
    /// @brief Identifiers for key providers of different versions
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00031
    /// @trace_id_dd=DD_CRYPTO_00500
    /// @needwork = ad
    /// @endcode
    // PRQA S 2025 QAC /// @qac: false positive
    enum class IpcKeyProviderCode : uint32_t
    // PRQA L:QAC
    {
        /// @brief Error code
        kFaultIpcKeyProvider = 0U,
        /// @brief Generate the isoft_def version of the key provider
        kIpcKeyProvider = 1U,
        /// @brief Generate the Hsm-Nxp version of the key provider        // kHsmNxpIpcKeyProvider = 2U,
        /// @brief Generate the Hsm-Bst version of the key provider        // kHsmBstIpcKeyProvider = 3U,
        /// @brief Current ipcKeyProvider not found
        kNotFoundIpcKeyProvider = 4U
    };

private:
    /// @brief Default key provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00501
    /// @needwork = dda
    /// @endcode
    typename T_IpcKP_Ctx::Uptr pIpcKeyProvider_;

public:
    /// @brief Get the default key provider
    /// @return Default key provider instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00032
    /// @trace_id_dd=DD_CRYPTO_00503
    /// @needwork = ad
    /// @endcode
    T_IpcKP_Ctx* GetIpcKeyProvider() const noexcept { return pIpcKeyProvider_.get(); }

    /// @brief Determine whether the key slot belongs to the key provider
    /// @param nSlotName Key slot name
    /// @param ipcKeyProvider IPC key provider
    /// @return is CryptoProvider 1 Puhua key provider 2 HSM NXP key provider 3 HSM Black Sesame key provider 4 Not found
    /// @throw ???
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00034
    /// @trace_id_dd=DD_CRYPTO_00505
    /// @needwork = ad
    /// @endcode
    template < typename T >
    static uint32_t IsMyProvider(ara::core::StringView const& nSlotName, T const* const ipcKeyProvider) noexcept(
        noexcept(isoft_def::PIpcAutoPacket()) && noexcept(ara::core::InstanceSpecifier(ara::core::StringView{})))
    {
        uint8_t* pdata{nullptr};
        uint32_t ndataLen{0U};

        isoft_def::PIpcAutoPacket aswMsg;
        bool const bDealIpc{ipcKeyProvider->GetClient()->DealIpcRequest(
            FUNC_NAME_KeySlot(MyProvider), aswMsg,
            std::move([nSlotName](isoft_def::PIpcAutoPacket const* const pReqMsg) -> uint16_t {
                if (pReqMsg != nullptr) {
                    isoft_def::keyslot::PIpcReq_MyProvider ipcReq;
                    ipcReq.nIpcSlotNameLen = static_cast< uint32_t >(nSlotName.size());
                    std::ignore            = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                    std::ignore = pReqMsg->AddDataToIpc(nSlotName.data(), static_cast< uint16_t >(nSlotName.size()));
                }
                return static_cast< uint16_t >(sizeof(isoft_def::keyslot::PIpcReq_MyProvider) + nSlotName.size());
            }))};
        if (true == bDealIpc) {
            isoft_def::keyslot::PIpcAsw_MyProvider* const pIpcAsw{
                static_cast< isoft_def::keyslot::PIpcAsw_MyProvider* >(static_cast< void* >(aswMsg.GetIpcBody()))};
            if (0 == pIpcAsw->GetErrorID()) {
                pdata    = pIpcAsw->GetData();
                ndataLen = pIpcAsw->GetDataLen();
            }
        }

        if ((pdata == nullptr) || (ndataLen == 0U)) {
            return static_cast< uint32_t >(IpcKeyProviderCode::kNotFoundIpcKeyProvider);
        }

        ara::core::InstanceSpecifier const iSpecify{T_StringView(T_TransVoid(pdata), static_cast< size_t >(ndataLen))};
        ara::crypto::cryp::CryptoProvider::Uptr const pCryptoProvider{LoadCryptoProvider(iSpecify)};
        return FindProvider(pCryptoProvider.get());
    }

protected:
    /// @brief Find the key provider by key name
    /// @param nSlotName Key slot name
    /// @return 1 Puhua key provider 2 HSM NXP key provider 3 HSM Black Sesame key provider 4 Not found
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00506
    /// @needwork = dda
    /// @endcode
    uint32_t _FindProvider(ara::core::StringView const& nSlotName) const noexcept
    {
        uint32_t ipcKeyProviderCode{0U};
        ipcKeyProviderCode = IsMyProvider< T_IpcKP_Ctx >(nSlotName, GetIpcKeyProvider());
        // Check if it belongs to isoft's ipcKeyProvider
        if (ipcKeyProviderCode != static_cast< uint32_t >(IpcKeyProviderCode::kNotFoundIpcKeyProvider)) {
            // Found (it could be really found, or the corresponding server was found but according to the server's return result, no provider was found)
            return ipcKeyProviderCode;
        }
        // Not found at all! Reason: the corresponding server is not started
        return ipcKeyProviderCode;
    }
    /// @brief Determine whether the crypto provider belongs to the key provider
    /// @param pCryptoProvider Pointer to the crypto provider
    /// @return 1 Puhua key provider 2 HSM NXP key provider 3 HSM Black Sesame key provider 4 Not found
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00507
    /// @needwork = dda
    /// @endcode
    static uint32_t FindProvider(ara::crypto::cryp::CryptoProvider* const pCryptoProvider) noexcept
    {
        // If it is a isoft version key provider, it will return in the if statement
        T_CryptoProvider_Ctx const* const pTCryptoProvider{dynamic_cast< T_CryptoProvider_Ctx* >(pCryptoProvider)};
        if (pTCryptoProvider != nullptr) {
            return static_cast< uint32_t >(IpcKeyProviderCode::kIpcKeyProvider);
        }

        return static_cast< uint32_t >(IpcKeyProviderCode::kFaultIpcKeyProvider);
    }
};
//********************************/
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_IPC_KEY_PROVIDER_H_
