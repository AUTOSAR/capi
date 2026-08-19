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
/// @file       isoft_keys_process_io_interface.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2022-08-17
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/IO Interface
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PKeysProcess_IoInterface
/// @unit_description=IO Interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_keys_process_io_interface.h"

#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/ksp/isoft_keys_manager.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
/// @brief Extended use of isoft's logging system
using ara::crypto::isoft_def::LogInfo;
//********************************/
/// @brief KeyProvider IPC server-side: IO Interface
/// @param lpcProcessManager LCP command processing manager
PKeysProcess_IoInterface::PKeysProcess_IoInterface(PKeys_Manager &lpcProcessManager) noexcept
    : PKeysProcess_T_Base< PKeysProcess_IoInterface >{this, lpcProcessManager}
{
    _InsertMapCmd(FUNC_NAME_IoInterface(GetAllowedUsage), &PKeysProcess_IoInterface::GetAllowedUsage);
    _InsertMapCmd(FUNC_NAME_IoInterface(GetCapacity), &PKeysProcess_IoInterface::GetCapacity);
    _InsertMapCmd(FUNC_NAME_IoInterface(GetCryptoObjectType), &PKeysProcess_IoInterface::GetCryptoObjectType);
    _InsertMapCmd(FUNC_NAME_IoInterface(GetPayloadSize), &PKeysProcess_IoInterface::GetPayloadSize);
    _InsertMapCmd(FUNC_NAME_IoInterface(GetObjectId), &PKeysProcess_IoInterface::GetObjectId);
    _InsertMapCmd(FUNC_NAME_IoInterface(GetPrimitiveId), &PKeysProcess_IoInterface::GetPrimitiveId);
    _InsertMapCmd(FUNC_NAME_IoInterface(GetTypeRestriction), &PKeysProcess_IoInterface::GetTypeRestriction);
    _InsertMapCmd(FUNC_NAME_IoInterface(IsObjectExportable), &PKeysProcess_IoInterface::IsObjectExportable);
    _InsertMapCmd(FUNC_NAME_IoInterface(IsObjectSession), &PKeysProcess_IoInterface::IsObjectSession);
    _InsertMapCmd(FUNC_NAME_IoInterface(IsVolatile), &PKeysProcess_IoInterface::IsVolatile);
    _InsertMapCmd(FUNC_NAME_IoInterface(IsValid), &PKeysProcess_IoInterface::IsValid);
    _InsertMapCmd(FUNC_NAME_IoInterface(IsWritable), &PKeysProcess_IoInterface::IsWritable);
}
//********************************/
/// @brief Returns the actual allowed key/seed usage flags defined by the "Actor's" key slot prototype and the current content of the container.
/// @name  GetAllowedUsage
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  Key/seed usage flags
PKeysProcess_Base::PResultLen PKeysProcess_IoInterface::GetAllowedUsage(PIpcPac_Head const *const pReqHead,
                                                                        PIpcAutoPacket &aswMsg) const noexcept
{
    ara::core::Result< PIoInterface * > const result{_FindIoInterface(pReqHead)};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }
    return DealLogic< uint32_t >(pReqHead, aswMsg, result.Value()->GetAllowedUsage());
}
/// @brief Returns the capacity of the underlying resource, in bytes.
/// @name  GetCapacity
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  Capacity of the underlying resource
PKeysProcess_Base::PResultLen PKeysProcess_IoInterface::GetCapacity(PIpcPac_Head const *const pReqHead,
                                                                    PIpcAutoPacket &aswMsg) const noexcept
{
    ara::core::Result< PIoInterface * > const result{_FindIoInterface(pReqHead)};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }
    return DealLogic< std::size_t >(pReqHead, aswMsg, result.Value()->GetCapacity());
}
/// @brief Returns the cryptobjecttype of the object referenced by this IOInterface.
/// @name  GetCryptoObjectType
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  Cryptobjecttype of the referenced object
PKeysProcess_Base::PResultLen PKeysProcess_IoInterface::GetCryptoObjectType(PIpcPac_Head const *const pReqHead,
                                                                            PIpcAutoPacket &aswMsg) const noexcept
{
    ara::core::Result< PIoInterface * > const result{_FindIoInterface(pReqHead)};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }
    PIoInterface *const pFindIoInterface{result.Value()};
    return DealLogic< CryptoObjectType >(pReqHead, aswMsg, pFindIoInterface->GetCryptoObjectType());
}
/// @brief Returns the COUID of the object stored to the IOInterface.
/// @name  GetObjectId
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  COUID of the IOInterface object
PKeysProcess_Base::PResultLen PKeysProcess_IoInterface::GetObjectId(PIpcPac_Head const *const pReqHead,
                                                                    PIpcAutoPacket &aswMsg) const noexcept
{
    ara::core::Result< PIoInterface * > const result{_FindIoInterface(pReqHead)};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }
    return DealLogic< CryptoObjectUid >(pReqHead, aswMsg, result.Value()->GetObjectId());
}
/// @brief Returns the size of the object payload stored in the underlying buffer of the IOInterface.
/// @name  GetPayloadSize
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  Object payload size in the underlying buffer
PKeysProcess_Base::PResultLen PKeysProcess_IoInterface::GetPayloadSize(PIpcPac_Head const *const pReqHead,
                                                                       PIpcAutoPacket &aswMsg) const noexcept
{
    ara::core::Result< PIoInterface * > const result{_FindIoInterface(pReqHead)};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }
    return DealLogic< std::size_t >(pReqHead, aswMsg, result.Value()->GetPayloadSize());
}
/// @brief Gets the vendor-specific ID of the primitive.
/// @name  GetPrimitiveId
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  Vendor-specific ID
PKeysProcess_Base::PResultLen PKeysProcess_IoInterface::GetPrimitiveId(PIpcPac_Head const *const pReqHead,
                                                                       PIpcAutoPacket &aswMsg) const noexcept
{
    ara::core::Result< PIoInterface * > const result{_FindIoInterface(pReqHead)};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }
    return DealLogic< CryptoAlgId >(pReqHead, aswMsg, result.Value()->GetPrimitiveId());
}
/// @brief Returns the content type restriction of this interface (identifies what type of data this container can hold).
/// @name  GetTypeRestriction
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  Interface content type restriction
PKeysProcess_Base::PResultLen PKeysProcess_IoInterface::GetTypeRestriction(PIpcPac_Head const *const pReqHead,
                                                                           PIpcAutoPacket &aswMsg) const noexcept
{
    ara::core::Result< PIoInterface * > const result{_FindIoInterface(pReqHead)};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }
    return DealLogic< CryptoObjectType >(pReqHead, aswMsg, result.Value()->GetTypeRestriction());
}
/// @brief Returns the "exportable" attribute of the object stored in the container. An object's exportability does not depend on the volatility of its container.
/// @name  IsObjectExportable
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  true if can Exportable false otherwise
PKeysProcess_Base::PResultLen PKeysProcess_IoInterface::IsObjectExportable(PIpcPac_Head const *const pReqHead,
                                                                           PIpcAutoPacket &aswMsg) const noexcept
{
    ara::core::Result< PIoInterface * > const result{_FindIoInterface(pReqHead)};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }
    return DealLogic< bool >(pReqHead, aswMsg, result.Value()->IsObjectExportable());
}
/// @brief Returns the "session" (or "temporary") attribute set for the object, e.g., keyderivationfunctionctx::DeriveKey().
/// @name  IsObjectSession
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns true if is Session Object false otherwise
PKeysProcess_Base::PResultLen PKeysProcess_IoInterface::IsObjectSession(PIpcPac_Head const *const pReqHead,
                                                                        PIpcAutoPacket &aswMsg) const noexcept
{
    ara::core::Result< PIoInterface * > const result{_FindIoInterface(pReqHead)};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }
    return DealLogic< bool >(pReqHead, aswMsg, result.Value()->IsObjectSession());
}
/// @brief Returns the "volatile" attribute of the underlying buffer of this IOInterface.
/// @name  IsVolatile
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  true if is Volatile Object false otherwise
PKeysProcess_Base::PResultLen PKeysProcess_IoInterface::IsVolatile(PIpcPac_Head const *const pReqHead,
                                                                   PIpcAutoPacket &aswMsg) const noexcept
{
    ara::core::Result< PIoInterface * > const result{_FindIoInterface(pReqHead)};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }
    return DealLogic< bool >(pReqHead, aswMsg, result.Value()->IsVolatile());
}
/// @brief Checks if the underlying keystore is valid. If the underlying resource is modified after the IOInterface is opened, the IOInterface becomes invalid.
/// @name  IsValid
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  true if is Valid Object false otherwise
PKeysProcess_Base::PResultLen PKeysProcess_IoInterface::IsValid(PIpcPac_Head const *const pReqHead,
                                                                PIpcAutoPacket &aswMsg) const noexcept
{
    ara::core::Result< PIoInterface * > const result{_FindIoInterface(pReqHead)};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }
    return DealLogic< bool >(pReqHead, aswMsg, result.Value()->IsValid());
}
/// @brief Checks if the underlying KeySlot is writable. If this IOInterface is linked to a volatile trusted container, it always returns true.
/// @name  IsWritable
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  true if is Writable Object false otherwise
PKeysProcess_Base::PResultLen PKeysProcess_IoInterface::IsWritable(PIpcPac_Head const *const pReqHead,
                                                                   PIpcAutoPacket &aswMsg) const noexcept
{
    ara::core::Result< PIoInterface * > const result{_FindIoInterface(pReqHead)};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }
    return DealLogic< bool >(pReqHead, aswMsg, result.Value()->IsWritable());
}
//********************************/
/// @brief Finds iointerface
/// @name  _FindIoInterface
/// @param pReqHead IPC request packet header
/// @returns  ara::core::Result<PIoInterface *>
ara::core::Result< PIoInterface * > PKeysProcess_IoInterface::_FindIoInterface(
    PIpcPac_Head const *const pReqHead) const noexcept
{
    if (nullptr == pReqHead) {
        return ara::core::Result< PIoInterface * >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    iointerface::PIpcReq_InterfaceCommon const *const pPacReq{
        pReqHead->GetBody< iointerface::PIpcReq_InterfaceCommon >()};
    // Processing logic: First find the key slot
    PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoader(pPacReq->nIpcSlotID)};
    if (nullptr == pFindSlotLoader) {
        return ara::core::Result< PIoInterface * >::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }
    PSvrIoInterfaceAgent *const pFindAgent{
        pFindSlotLoader->FindIoInterface(pPacReq->nIpcSlotID, pPacReq->nIoInterfaceID)};
    if (nullptr == pFindAgent) {
        return ara::core::Result< PIoInterface * >::FromError(SecurityErrorDomain::Errc::kIpcIoInterfaceNotExist);
    }
    PIoInterface *const pFindIoInterface{pFindAgent->GetIoInterface()};
    return ara::core::Result< PIoInterface * >::FromValue(pFindIoInterface);
}
/// @brief Processing logic
/// @name  DealLogic
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @param data Template type: data
/// @returns  Template generic function return value
/// @code{.isoft}
/// @tparam T_Type
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_06099
/// @needwork = dda
/// @endcode
template < typename T_Type >
PKeysProcess_Base::PResultLen PKeysProcess_IoInterface::DealLogic(PIpcPac_Head const *const pReqHead,
                                                                  PIpcAutoPacket &aswMsg,
                                                                  T_Type const &data) noexcept
{
    uint16_t const nAswLen{static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(iointerface::PIpcAsw_InterfaceCommon)
                                                   + sizeof(T_Type))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    iointerface::PIpcAsw_InterfaceCommon *const pPacAsw{
        static_cast< iointerface::PIpcAsw_InterfaceCommon * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(sizeof(T_Type));
    T_Type *const pData{static_cast< T_Type * >(static_cast< void * >(pPacAsw + 1))};
    *pData = data;
    return PResultLen::FromValue(pReqHead->nPacSize);
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
