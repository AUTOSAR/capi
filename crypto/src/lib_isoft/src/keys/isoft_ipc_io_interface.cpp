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
/// @file       isoft_ipc_io_interface.cpp
/// @brief      AutoSar-Crypto Encryption and Decryption Common Module
/// @details
/// @date       2022-04-12
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Key Components/IO Interface
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PIoInterface_Ipc
/// @unit_description=IPC version of IO interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/keys/isoft_ipc_io_interface.h"

#include "ara/crypto/ipc/isoft_ipc_protocol.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Formal interface of IOInterface, used for saving and loading security objects.
///         Actual saving and loading should be implemented through internal methods known to the trusted pair of crypto provider and storage provider.
///         Each object should be uniquely identified by its type and crypto object unique identifier (COUID). This interface assumes that objects in the container are compressed, i.e., have a minimal optimized size.
/// @name  PIoInterface_Ipc
/// @param ipcClient IPC client
/// @param slotName Key slot name
/// @returns
PIoInterface_Ipc::PIoInterface_Ipc(PIpcClient const &ipcClient, ara::core::StringView const &slotName) noexcept
    : PIoInterface_Ipc{ipcClient, 0U, 0U, slotName}
{
}
/// @brief Constructor with parameters
/// @param ipcClient IPC client
/// @param nIpcSlotID IPC key slot ID
/// @param nIpcIoInterfaceID
/// @param slotName Key slot name
PIoInterface_Ipc::PIoInterface_Ipc(PIpcClient const &ipcClient,
                                   uint32_t const nIpcSlotID,
                                   uint32_t const nIpcIoInterfaceID,
                                   ara::core::StringView const &slotName) noexcept
    : IOInterface{}  // NOLINT
    , ipcClient_{ipcClient}
    , nIpcSlotID_{nIpcSlotID}
    , nIpcIoInterfaceID_{nIpcIoInterfaceID}
    , stSlotName_{slotName}
{
}
/// @brief Return the actual allowed key/seed usage flags as defined by the key slot prototype of this "Actor" and the current content of the container.
///         Volatile containers have no prototype restrictions but can define restrictions for the current instance of the object at runtime.
///         The value returned by this method is the bitwise AND of the common usage flags defined at runtime and the usage flags defined by the UserPermissions prototype for the current "Actor".
///         This method is particularly useful for empty permanent prototype containers.
/// @name  GetAllowedUsage
/// @returns Key/seed usage flags
AllowedUsageFlags PIoInterface_Ipc::GetAllowedUsage() const noexcept
{
    ara::core::Result< uint32_t > const result{_T_IpcGetData< uint32_t >(FUNC_NAME_IoInterface(GetAllowedUsage))};
    if (false == result.HasValue()) {
        return kAllowPrototypedOnly;
    }
    return static_cast< AllowedUsageFlags >(result.Value());
}
/// @brief Return the capacity of the underlying resource.
/// @name  GetCapacity
/// @returns Capacity of the underlying resource
std::size_t PIoInterface_Ipc::GetCapacity() const noexcept
{
    ara::core::Result< std::size_t > const result{_T_IpcGetData< std::size_t >(FUNC_NAME_IoInterface(GetCapacity))};
    if (false == result.HasValue()) {
        return 0U;
    }
    return result.Value();
}
/// @brief Return the crypto object type of the object referenced by this IOInterface.
/// @name  GetCryptoObjectType
/// @returns Crypto object type of the referenced object
CryptoObjectType PIoInterface_Ipc::GetCryptoObjectType() const noexcept
{
    ara::core::Result< uint32_t > const result{_T_IpcGetData< uint32_t >(FUNC_NAME_IoInterface(GetCryptoObjectType))};
    if (false == result.HasValue()) {
        return CryptoObjectType::kUndefined;
    }
    return static_cast< CryptoObjectType >(result.Value());
}
/// @brief Return the COUID of the object stored in the IOInterface.
///         If the container is empty, this method returns CryptoObjectType::kUndefined. The unambiguous identification of a crypto object requires two components: crypto object UID and crypto object type.
/// @name  GetObjectId
/// @returns COUID of the object in the IOInterface
CryptoObjectUid PIoInterface_Ipc::GetObjectId() const noexcept
{
    ara::core::Result< CryptoObjectUid > const result{
        _T_IpcGetData< CryptoObjectUid >(FUNC_NAME_IoInterface(GetObjectId))};
    if (false == result.HasValue()) {
        return CryptoObjectUid{};
    }
    return result.Value();
}
/// @brief Return the size of the object payload stored in the underlying buffer of the IOInterface.
/// If the container is empty, this method returns 0. The return value does not consider the meta-information properties of the object, but their size is fixed and common to all crypto objects independent of their actual type.
/// Based on the implementation details of the object, space for the object's meta-information is automatically provided.
/// @name  GetPayloadSize
/// @returns Size of the object payload in the underlying buffer
std::size_t PIoInterface_Ipc::GetPayloadSize() const noexcept
{
    ara::core::Result< std::size_t > const result{_T_IpcGetData< std::size_t >(FUNC_NAME_IoInterface(GetPayloadSize))};
    if (false == result.HasValue()) {
        return 0U;
    }
    return result.Value();
}
/// @brief Get the vendor-specific ID of the primitive.
/// @name  GetPrimitiveId
/// @returns Vendor-specific ID of the primitive
CryptoAlgId PIoInterface_Ipc::GetPrimitiveId() const noexcept
{
    ara::core::Result< CryptoAlgId > const result{_T_IpcGetData< CryptoAlgId >(FUNC_NAME_IoInterface(GetPrimitiveId))};
    if (false == result.HasValue()) {
        return 0U;
    }
    return result.Value();
}
/// @brief Return the content type restriction of this interface (identifying what type of data this container can hold).
/// @return
/// If KeySlotPrototypeProps::mAllowContentTypeChange == TRUE, it should return kUndefined.
/// If the container has a type restriction different from @c CryptoObjectType::kUndefined, only objects of that type can be saved to this container.
/// Volatile containers have no content type restrictions. CryptoObjectType::kUndefined identifies no restriction.
/// @name GetTypeRestriction
/// @return Content type restriction of the interface
CryptoObjectType PIoInterface_Ipc::GetTypeRestriction() const noexcept
{
    ara::core::Result< uint32_t > const result{_T_IpcGetData< uint32_t >(FUNC_NAME_IoInterface(GetTypeRestriction))};
    if (false == result.HasValue()) {
        return CryptoObjectType::kUndefined;
    }
    return static_cast< CryptoObjectType >(result.Value());
}
/// @brief Return the "exportable" attribute of the object stored in the container. The exportability of an object does not depend on the volatility of its container.
/// @return true if can export false otherwise
bool PIoInterface_Ipc::IsObjectExportable() const noexcept
{
    ara::core::Result< bool > const result{_T_IpcGetData< bool >(FUNC_NAME_IoInterface(IsObjectExportable))};
    if (false == result.HasValue()) {
        return false;
    }
    return result.Value();
}
/// @brief Return the "session" (or "temporary") attribute of the set object, e.g., keyderivationfunctionctx::DeriveKey().
///         A "session" object can only be stored in a VolatileTrustedContainer! If this IOInterface is linked to a KeySlot, it always returns false.
/// @name  IsObjectSession
/// @returns  true if can is session object false otherwise
bool PIoInterface_Ipc::IsObjectSession() const noexcept
{
    ara::core::Result< bool > const result{_T_IpcGetData< bool >(FUNC_NAME_IoInterface(IsObjectSession))};
    if (false == result.HasValue()) {
        return false;
    }
    return result.Value();
}
/// @brief Return the "volatility" attribute of the underlying buffer of this IOInterface.
///         "Session" objects can only be stored in "volatile" containers. The contents of a "volatile" container will be destroyed together with the interface instance.
/// @name  IsVolatile
/// @returns true if can is volatile object false otherwise
bool PIoInterface_Ipc::IsVolatile() const noexcept
{
    ara::core::Result< bool > const result{_T_IpcGetData< bool >(FUNC_NAME_IoInterface(IsVolatile))};
    if (false == result.HasValue()) {
        return false;
    }
    return result.Value();
}
/// @brief Get whether the underlying key storage is valid. If the underlying resource is modified after the IOInterface is opened, the IOInterface will become invalid.
/// @name  IsValid
/// @returns  true if can is Valid object false otherwise
bool PIoInterface_Ipc::IsValid() const noexcept
{
    /// @returns true if the underlying resource can be valid, false otherwise
    ara::core::Result< bool > const result{_T_IpcGetData< bool >(FUNC_NAME_IoInterface(IsValid))};
    if (false == result.HasValue()) {
        return false;
    }
    return result.Value();
}
/// @brief Get whether the underlying KeySlot is writable. If this IOInterface is linked to a volatile trusted container, it always returns true.
/// @name  IsWritable
/// @returns  true if can is Writable object false otherwise
bool PIoInterface_Ipc::IsWritable() const noexcept
{
    ara::core::Result< bool > const result{_T_IpcGetData< bool >(FUNC_NAME_IoInterface(IsWritable))};
    if (false == result.HasValue()) {
        return false;
    }
    return result.Value();
}

//********************************/
/// @brief Set the key slot number
/// @name  SetSlotID
/// @param nIpcSlotID IPC key slot ID
/// @param nInterfaceID IO interface ID
void PIoInterface_Ipc::SetSlotID(uint32_t const nIpcSlotID, uint32_t const nInterfaceID) noexcept
{
    nIpcSlotID_        = nIpcSlotID;
    nIpcIoInterfaceID_ = nInterfaceID;
}
//********************************/
/// @brief All IPC functions that use PIpcReq_InterfaceCommon to send packets and PIpcAsw_LogicHead to receive packets
/// @name  _IpcGetData
/// @param aswMsg IPC-managed response message
/// @param stFuncName Function name used for IPC call
/// @returns  true if get data sucess false otherwise
bool PIoInterface_Ipc::_IpcGetData(PIpcAutoPacket &aswMsg, ara::core::StringView const &stFuncName) const noexcept
{
    bool const bDealIpc{
        ipcClient_.DealIpcRequest(stFuncName, aswMsg, [this](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                iointerface::PIpcReq_InterfaceCommon ipcReq;
                ipcReq.nIpcSlotID     = nIpcSlotID_;
                ipcReq.nIoInterfaceID = nIpcIoInterfaceID_;
                std::ignore           = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return sizeof(iointerface::PIpcReq_InterfaceCommon);
        })};
    if (false == bDealIpc) {
        return false;
    }
    iointerface::PIpcAsw_InterfaceCommon *const pIpcAsw{
        static_cast< iointerface::PIpcAsw_InterfaceCommon * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return false;
    }
    return true;
}
template < typename T_Type >
/// @brief Template for IPC functions that use PIpcReq_InterfaceBase to send packets and PIpcAsw_LogicHead to receive packets
/// @name  _T_IpcGetData
/// @param stFuncName Function name used for IPC call
/// @returns  true if get data sucess false otherwise
/// All IPC functions that use PIpcReq_InterfaceCommon to send packets and PIpcAsw_LogicHead to receive packets
ara::core::Result< T_Type > PIoInterface_Ipc::_T_IpcGetData(ara::core::StringView const &stFuncName) const noexcept
{
    PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{_IpcGetData(aswMsg, stFuncName)};
    if (false == bDealIpc) {
        return ara::core::Result< T_Type >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    iointerface::PIpcAsw_InterfaceCommon *const pIpcAsw{
        static_cast< iointerface::PIpcAsw_InterfaceCommon * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    if (pIpcAsw->GetDataLen() != sizeof(T_Type)) {
        return ara::core::Result< T_Type >::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    T_Type nReturn;
    // A powerful deserialization function can be written here, making this solution universal
    std::ignore = memcpy(&nReturn, pIpcAsw->GetData(), static_cast< std::size_t >(pIpcAsw->GetDataLen()));
    return ara::core::Result< T_Type >::FromValue(nReturn);
}
/// @brief Template for IPC functions that use PIpcReq_InterfaceBase to send packets and PIpcAsw_LogicHead to receive packets
/// @name  _T_IpcGetData<uint32_t>
/// @returns  template ara::core::Result<uint32_t>
/// @throws
/// @param stFuncName Function name used for IPC call
template ara::core::Result< uint32_t > PIoInterface_Ipc::_T_IpcGetData< uint32_t >(
    ara::core::StringView const &stFuncName) const noexcept;
/// @brief Template for IPC functions that use PIpcReq_InterfaceBase to send packets and PIpcAsw_LogicHead to receive packets
/// @name  _T_IpcGetData<uint64_t>
/// @returns  template ara::core::Result<uint64_t>
/// @throws
/// @param stFuncName Function name used for IPC call
template ara::core::Result< uint64_t > PIoInterface_Ipc::_T_IpcGetData< uint64_t >(
    ara::core::StringView const &stFuncName) const noexcept;
/// @brief Template for IPC functions that use PIpcReq_InterfaceBase to send packets and PIpcAsw_LogicHead to receive packets
/// @name  _T_IpcGetData<CryptoObjectUid>
/// @throws
/// @param stFuncName Function name used for IPC call
/// @return
template ara::core::Result< CryptoObjectUid > PIoInterface_Ipc::_T_IpcGetData< CryptoObjectUid >(
    ara::core::StringView const &stFuncName) const noexcept;
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
