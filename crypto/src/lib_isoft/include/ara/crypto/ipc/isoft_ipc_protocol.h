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
/// @file       isoft_ipc_protocol.h
/// @brief      AutoSar-Crypto Key Storage Module
/// @details    IPC communication data packets for KeyProvider providers
/// @date       2022-08-01
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-08-01  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Function Modules
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Common_api
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_IPC_PROTOCOL_H_
#define ARA_CRYPTO_KEYS_PUHUA_IPC_PROTOCOL_H_

#include <ara/core/string_view.h>

#include <cstdint>

#include "ara/core/string.h"
#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/ipc/isoft_ipc_name.h"
#include "ara/crypto/keys/key_slot_content_props.h"
#include "ara/crypto/keys/key_slot_prototype_props.h"

#ifndef STRINGIFY
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03247
    /// @trace_id_dd=DD_CRYPTO_06489
    /// @trace_id_sr=SR_CRYPTO_06005
    /// @needwork = ad
    /// @endcode
    #define STRINGIFY(x) #x
#endif
/// @brief Build class member function identifier within IPC data packet: PIpcKeyProvider
/// @param stFuncName Function name used for IPC call
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03248
/// @trace_id_dd=DD_CRYPTO_06490
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
#define FUNC_NAME_KeyProvider(stFuncName) ara::core::StringView("PIpcKeyProvider::" STRINGIFY(stFuncName))
/// @brief Build class member function identifier within IPC data packet: PIpcX509Provider
/// @param stFuncName Function name used for IPC call
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03249
/// @trace_id_dd=DD_CRYPTO_06491
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
#define FUNC_NAME_CertProvider(stFuncName) ara::core::StringView("PIpcX509Provider::" STRINGIFY(stFuncName))
/// @brief Build class member function identifier within IPC data packet: PIpcKeySlot
/// @param stFuncName Function name used for IPC call
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03250
/// @trace_id_dd=DD_CRYPTO_06492
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
#define FUNC_NAME_KeySlot(stFuncName) ara::core::StringView("PIpcKeySlot::" STRINGIFY(stFuncName))
/// @brief Build class member function identifier within IPC data packet: PIoInterface_Ipc
/// @param stFuncName Function name used for IPC call
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03251
/// @trace_id_dd=DD_CRYPTO_06493
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
#define FUNC_NAME_IoInterface(stFuncName) ara::core::StringView("PIoInterface_Ipc::" STRINGIFY(stFuncName))
/// @brief Build class member function identifier within IPC data packet: PSymmetricBlockStream_Ipc
/// @param stFuncName Function name used for IPC call
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03252
/// @trace_id_dd=DD_CRYPTO_06494
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
#define FUNC_NAME_Symmetric(stFuncName) ara::core::StringView("PSymmetricBlockStream_Ipc::" STRINGIFY(stFuncName))
/// @brief Build class member function identifier within IPC data packet: PWrap_Ipc
/// @param stFuncName Function name used for IPC call
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03253
/// @trace_id_dd=DD_CRYPTO_06495
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
#define FUNC_NAME_Wrap(stFuncName) ara::core::StringView("PWrap_Ipc::" STRINGIFY(stFuncName))
/// @brief Build class member function identifier within IPC data packet: PMac_Ipc
/// @param stFuncName Function name used for IPC call
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03254
/// @trace_id_dd=DD_CRYPTO_06496
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
#define FUNC_NAME_Mac(stFuncName) ara::core::StringView("PMac_Ipc::" STRINGIFY(stFuncName))
/// @brief Build class member function identifier within IPC data packet: PAsymmetric_Ipc
/// @param stFuncName Function name used for IPC call
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03255
/// @trace_id_dd=DD_CRYPTO_06497
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
#define FUNC_NAME_Asymmetric(stFuncName) ara::core::StringView("PAsymmetric_Ipc::" STRINGIFY(stFuncName))
/// @brief Build class member function identifier within IPC data packet: PRng_Ipc
/// @param stFuncName Function name used for IPC call
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03256
/// @trace_id_dd=DD_CRYPTO_06498
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
#define FUNC_NAME_RNG(stFuncName) ara::core::StringView("PRng_Ipc::" STRINGIFY(stFuncName))
/// @brief Build class member function identifier within IPC data packet: PSeed_Ipc
/// @param stFuncName Function name used for IPC call
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03257
/// @trace_id_dd=DD_CRYPTO_06499
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
#define FUNC_NAME_Seed(stFuncName) ara::core::StringView("PSeed_Ipc::" STRINGIFY(stFuncName))
/// @brief Build class member function identifier within IPC data packet: PPrivateKey_Ipc
/// @param stFuncName Function name used for IPC call
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03258
/// @trace_id_dd=DD_CRYPTO_06500
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
#define FUNC_NAME_PrivateKey(stFuncName) ara::core::StringView("PPrivateKey_Ipc::" STRINGIFY(stFuncName))
/// @brief Build class member function identifier within IPC data packet: PPublicKey_Ipc
/// @param stFuncName Function name used for IPC call
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03259
/// @trace_id_dd=DD_CRYPTO_06501
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
#define FUNC_NAME_PublicKey(stFuncName) ara::core::StringView("PPublicKey_Ipc::" STRINGIFY(stFuncName))

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Provides IPC service for KeyProvider externally
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01574
/// @trace_id_dd=DD_CRYPTO_04102
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
enum class EnumIpcCmdID : uint32_t
{
    /// @brief IPC service type: Undefined
    kIpcCmdID_NoDefine = 0,
    /// @brief IPC service type: PIpcKeyProvider
    kIpcCmdID_KeyProvider = 1000,
    /// @brief IPC service type: LoadKeySlot
    kIpcCmdID_LoadKeySlot = 1001,
    /// @brief IPC service type: BeginTransaction
    kIpcCmdID_BeginTransaction = 1002,
    /// @brief IPC service type: CommitTransaction
    kIpcCmdID_CommitTransaction = 1003,
    /// @brief IPC service type: RollbackTransaction
    kIpcCmdID_RollbackTransaction = 1004,
    /// @brief IPC service type: GetRegisteredObserver
    kIpcCmdID_GetRegisteredObserver = 1005,
    /// @brief IPC service type: RegisterObserver
    kIpcCmdID_RegisterObserver = 1006,
    /// @brief IPC service type: UnsubscribeObserver
    kIpcCmdID_UnsubscribeObserver = 1007,
};
//********************************/
#pragma pack(push)
#pragma pack(1)
// Composition of a standard request packet: PIpcPac_Head + stFuncName + Payload   //2022-08-15
// hanjingjing tentatively defines the data packet identifier used by the processing program as stFuncName (nCmdID_ degenerates to reserved item)
/// @brief IPC communication data packet: Header
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01575
/// @trace_id_dd=DD_CRYPTO_04103
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcPac_Head final
{
public:
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01576
    /// @trace_id_dd=DD_CRYPTO_04104
    /// @needwork = ad
    /// @endcode
    ~PIpcPac_Head() = default;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01577
    /// @trace_id_dd=DD_CRYPTO_04105
    /// @needwork = ad
    /// @endcode
    PIpcPac_Head() = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01578
    /// @trace_id_dd=DD_CRYPTO_04106
    /// @needwork = ad
    /// @endcode
    PIpcPac_Head(PIpcPac_Head &&other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01579
    /// @trace_id_dd=DD_CRYPTO_04107
    /// @needwork = ad
    /// @endcode
    PIpcPac_Head &operator=(PIpcPac_Head &&other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01580
    /// @trace_id_dd=DD_CRYPTO_04108
    /// @needwork = ad
    /// @endcode
    PIpcPac_Head &operator=(PIpcPac_Head const &other) = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01581
    /// @trace_id_dd=DD_CRYPTO_04109
    /// @needwork = ad
    /// @endcode
    PIpcPac_Head(PIpcPac_Head const &other) = delete;
    /// @brief Process ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04110
    /// @needwork = dda
    /// @endcode
    uint64_t nProcessID{0};
    /// @brief Packet checksum: tentative
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04111
    /// @needwork = dda
    /// @endcode
    int32_t nCrc{0};
    /// @brief Data length: includes PIpcPac_Head itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04112
    /// @needwork = dda
    /// @endcode
    uint16_t nPacSize{0};
    /// @brief Function name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04113
    /// @needwork = dda
    /// @endcode
    uint16_t nFuncNameLen{0};
    /// @brief Get the function name
    /// @return function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01582
    /// @trace_id_dd=DD_CRYPTO_04114
    /// @needwork = ad
    /// @endcode
    inline ara::core::StringView GetFuncName() const noexcept
    {
        return ara::core::StringView(
            static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this)) + sizeof(PIpcPac_Head),
            static_cast< std::size_t >(nFuncNameLen));
    }
    /// @brief Get the header length of the IPC communication data packet
    /// @name   GetHeadLen
    /// @returns header length of the IPC communication data packet
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01584
    /// @trace_id_dd=DD_CRYPTO_04116
    /// @needwork = ad
    /// @endcode
    inline uint16_t GetHeadLen() const noexcept { return static_cast< uint16_t >(sizeof(PIpcPac_Head) + nFuncNameLen); }
    /// @brief Get the body of the IPC communication data packet
    /// @name   GetBody
    /// @returns starting address of the IPC communication data packet body
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01583
    /// @trace_id_dd=DD_CRYPTO_04115
    /// @needwork = ad
    /// @endcode
    inline uint8_t const *GetBody() const noexcept { return T_TransBytes(this) + sizeof(PIpcPac_Head) + nFuncNameLen; }
    /// @brief Return the data packet body of a specific type
    /// @return data packet body of a specific type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01585
    /// @trace_id_dd=DD_CRYPTO_04117
    /// @needwork = ad
    /// @endcode
    template < typename T_Value >
    T_Value const *GetBody() const noexcept
    {
        uint8_t const *const pBody{GetBody()};
        return static_cast< T_Value const * >(static_cast< void const * >(pBody));
    }
};
/// @brief IPC communication data packet: logical header
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01586
/// @trace_id_dd=DD_CRYPTO_04118
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_LogicHead
{
public:
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01587
    /// @trace_id_dd=DD_CRYPTO_04119
    /// @needwork = ad
    /// @endcode
    virtual ~PIpcAsw_LogicHead() = default;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01588
    /// @trace_id_dd=DD_CRYPTO_04120
    /// @needwork = ad
    /// @endcode
    PIpcAsw_LogicHead() = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01589
    /// @trace_id_dd=DD_CRYPTO_04121
    /// @needwork = ad
    /// @endcode
    PIpcAsw_LogicHead(PIpcAsw_LogicHead const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01590
    /// @trace_id_dd=DD_CRYPTO_04122
    /// @needwork = ad
    /// @endcode
    PIpcAsw_LogicHead(PIpcAsw_LogicHead &&other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01591
    /// @trace_id_dd=DD_CRYPTO_04123
    /// @needwork = ad
    /// @endcode
    PIpcAsw_LogicHead &operator=(PIpcAsw_LogicHead const &other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01592
    /// @trace_id_dd=DD_CRYPTO_04124
    /// @needwork = ad
    /// @endcode
    PIpcAsw_LogicHead &operator=(PIpcAsw_LogicHead &&other) = delete;

private:
    /// @brief Error code, 0 means no error
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04125
    /// @needwork = dda
    /// @endcode
    int32_t nErrorID_{0U};

public:
    /// @brief Get the error code
    /// @return nErrorID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01593
    /// @trace_id_dd=DD_CRYPTO_04126
    /// @needwork = ad
    /// @endcode
    int32_t GetErrorID() const noexcept { return nErrorID_; }
    /// @brief Set the error code
    /// @param nErrorID error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01594
    /// @trace_id_dd=DD_CRYPTO_04127
    /// @needwork = ad
    /// @endcode
    void SetErrorID(int32_t const nErrorID) noexcept { nErrorID_ = nErrorID; }
};
//********************************/
/// @brief Namespace of the key provider
namespace keyprovider {
/// @brief Get the name within IPC: key provider name
/// @return key provider name
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01595
/// @trace_id_dd=DD_CRYPTO_04128
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline char8_t const *GetName_PIpcKeyProvider() noexcept { return "PIpcKeyProvider"; }
/// @brief Get the name within IPC: PIpcKeyProvider::UpdateObserver
/// @return name within IPC
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01596
/// @trace_id_dd=DD_CRYPTO_04129
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_UpdateObserver() noexcept
{
    return ara::core::StringView{"PIpcKeyProvider::UpdateObserver"};
}
/// @brief IPC communication request packet: LoadKeySlot
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01597
/// @trace_id_dd=DD_CRYPTO_04130
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_LoadKeySlot final
{
private:
    /// @brief KeySlot name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04131
    /// @needwork = dda
    /// @endcode
    uint16_t nNameLen_{0};
    /// @brief Process ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04110
    /// @needwork = dda
    /// @endcode
    uint64_t nPid_{0};

public:
    /// @brief Get the name
    /// @return key slot port name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01598
    /// @trace_id_dd=DD_CRYPTO_04132
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetName() const noexcept
    {
        return ara::core::StringView(static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this))
                                         + sizeof(PIpcReq_LoadKeySlot),
                                     static_cast< std::size_t >(nNameLen_));
    }
    /// @brief Get the name length
    /// @return key slot port name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01599
    /// @trace_id_dd=DD_CRYPTO_04133
    /// @needwork = ad
    /// @endcode
    uint16_t GetNameLen() const noexcept { return nNameLen_; }
    /// @brief Get the name length
    /// @return key slot port name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01599
    /// @trace_id_dd=DD_CRYPTO_04133
    /// @needwork = ad
    /// @endcode
    uint64_t GetPid() const noexcept { return nPid_; }
    /// @brief Set the name length
    /// @param nNameLen name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01600
    /// @trace_id_dd=DD_CRYPTO_04134
    /// @needwork = ad
    /// @endcode
    void SetNameLen(uint16_t const nNameLen) noexcept { nNameLen_ = nNameLen; }
    /// @brief Set the PID
    /// @param PID process ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01600
    /// @trace_id_dd=DD_CRYPTO_04134
    /// @needwork = ad
    /// @endcode
    void SetPid(uint64_t const nPid) noexcept { nPid_ = nPid; }
};
/// @brief IPC communication reply packet: LoadKeySlot
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01601
/// @trace_id_dd=DD_CRYPTO_04135
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_LoadKeySlot final : public PIpcAsw_LogicHead
{
private:
    /// @brief KeySlot identifier ID: the first packet uses KeyName, otherwise SlotID is used to identify the key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04136
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID_{0};

public:
    /// @brief Get the slot ID
    /// @return key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01602
    /// @trace_id_dd=DD_CRYPTO_04137
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcSlotID() const noexcept { return nIpcSlotID_; }
    /// @brief Set the slot ID
    /// @param nIpcSlotID IPC key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01603
    /// @trace_id_dd=DD_CRYPTO_04138
    /// @needwork = ad
    /// @endcode
    void SetIpcSlotID(uint32_t const nIpcSlotID) noexcept { nIpcSlotID_ = nIpcSlotID; }
};
//***************/
/// @brief IPC communication request packet: BeginTransaction
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01604
/// @trace_id_dd=DD_CRYPTO_04139
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_BeginTransaction final
{
public:
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01605
    /// @trace_id_dd=DD_CRYPTO_04140
    /// @needwork = ad
    /// @endcode
    ~PIpcReq_BeginTransaction() = default;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01606
    /// @trace_id_dd=DD_CRYPTO_04141
    /// @needwork = ad
    /// @endcode
    PIpcReq_BeginTransaction() = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01607
    /// @trace_id_dd=DD_CRYPTO_04142
    /// @needwork = ad
    /// @endcode
    PIpcReq_BeginTransaction(PIpcReq_BeginTransaction const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01608
    /// @trace_id_dd=DD_CRYPTO_04143
    /// @needwork = ad
    /// @endcode
    PIpcReq_BeginTransaction(PIpcReq_BeginTransaction &&other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01609
    /// @trace_id_dd=DD_CRYPTO_04144
    /// @needwork = ad
    /// @endcode
    PIpcReq_BeginTransaction &operator=(PIpcReq_BeginTransaction const &other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01610
    /// @trace_id_dd=DD_CRYPTO_04145
    /// @needwork = ad
    /// @endcode
    PIpcReq_BeginTransaction &operator=(PIpcReq_BeginTransaction &&other) = delete;
    /// @brief Number of key slot handles owned by the transaction; the specific data follows this packet, each SlotID is a uint32_t
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04146
    /// @needwork = dda
    /// @endcode
    uint32_t nSlotCount{0};
};
/// @brief IPC communication reply packet: BeginTransaction
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01611
/// @trace_id_dd=DD_CRYPTO_04147
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_BeginTransaction final : public PIpcAsw_LogicHead
{
private:
    /// @brief Returned transaction number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04148
    /// @needwork = dda
    /// @endcode
    uint64_t nTransactionId_{0};

public:
    /// @brief Get the transaction number
    /// @return transaction index number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01612
    /// @trace_id_dd=DD_CRYPTO_04149
    /// @needwork = ad
    /// @endcode
    uint64_t GetTransactionId() const noexcept { return nTransactionId_; }
    /// @brief Set the transaction number
    /// @param nTransactionId transaction ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01613
    /// @trace_id_dd=DD_CRYPTO_04150
    /// @needwork = ad
    /// @endcode
    void SetTransactionId(uint64_t const nTransactionId) noexcept { nTransactionId_ = nTransactionId; }
};
//***************/
/// @brief IPC communication request packet: CommitTransaction
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03161
/// @trace_id_dd=DD_CRYPTO_06364
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcReq_CommitTransaction = PIpcAsw_BeginTransaction;
/// @brief IPC communication reply packet: CommitTransaction
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01614
/// @trace_id_dd=DD_CRYPTO_04151
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_CommitTransaction final : public PIpcAsw_LogicHead
{
private:
    /// @brief Success count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04152
    /// @needwork = dda
    /// @endcode
    uint32_t nSuccessCount_{0};

public:
    /// @brief Get the success count
    /// @return success count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01615
    /// @trace_id_dd=DD_CRYPTO_04153
    /// @needwork = ad
    /// @endcode
    uint32_t GetSuccessCount() const noexcept { return nSuccessCount_; }
    /// @brief Set the success count
    /// @param nSuccessCount success count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01616
    /// @trace_id_dd=DD_CRYPTO_04154
    /// @needwork = ad
    /// @endcode
    void SetSuccessCount(uint32_t const nSuccessCount) noexcept { nSuccessCount_ = nSuccessCount; }
};
//***************/
/// @brief IPC communication request packet: RollbackTransaction
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03162
/// @trace_id_dd=DD_CRYPTO_06365
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcReq_RollbackTransaction = PIpcAsw_BeginTransaction;
/// @brief IPC communication reply packet: RollbackTransaction
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03163
/// @trace_id_dd=DD_CRYPTO_06366
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcAsw_RollbackTransaction = PIpcAsw_CommitTransaction;
//***************/
/// @brief IPC communication request packet: GetRegisteredObserver
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01617
/// @trace_id_dd=DD_CRYPTO_04155
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_GetRegisteredObserver final
{
public:
    /// @brief Check whether the corresponding subscription registration exists, equal to PIpcPac_Head.nProcessID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04156
    /// @needwork = dda
    /// @endcode
    uint64_t nRegProcessID{0};
};
/// @brief IPC communication reply packet: GetRegisteredObserver
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01618
/// @trace_id_dd=DD_CRYPTO_04157
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_GetRegisteredObserver final : public PIpcAsw_LogicHead
{
private:
    /// @brief Registered Pid, equal to PIpcPac_Head.nProcessID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04158
    /// @needwork = dda
    /// @endcode
    uint64_t nRegProcessID_{0};

public:
    /// @brief Get the registered Pid
    /// @return registered Pid
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01619
    /// @trace_id_dd=DD_CRYPTO_04159
    /// @needwork = ad
    /// @endcode
    uint64_t GetRegProcessID() const noexcept { return nRegProcessID_; }
    /// @brief Set the registered Pid
    /// @param regProcessId registered Pid
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01620
    /// @trace_id_dd=DD_CRYPTO_04160
    /// @needwork = ad
    /// @endcode
    void SetRegProcessID(uint64_t const regProcessId) noexcept { nRegProcessID_ = regProcessId; }
};
//***************/
/// @brief IPC communication request packet: RegisterObserver
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01621
/// @trace_id_dd=DD_CRYPTO_04161
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_RegisterObserver final
{
public:
    /// @brief 0 means to cancel the already registered one; non-zero must equal PIpcPac_Head.nProcessID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04162
    /// @needwork = dda
    /// @endcode
    uint64_t nActionPid{0};
};
/// @brief IPC communication reply packet: RegisterObserver
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01622
/// @trace_id_dd=DD_CRYPTO_04163
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_RegisterObserver final : public PIpcAsw_LogicHead
{
private:
    /// @brief Registered Pid, equal to PIpcPac_Head.nProcessID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04164
    /// @needwork = dda
    /// @endcode
    uint64_t nActionPid_{0};

public:
    /// @brief Get the registered Pid
    /// @return registered Pid
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01623
    /// @trace_id_dd=DD_CRYPTO_04165
    /// @needwork = ad
    /// @endcode
    uint64_t GetActionPid() const noexcept { return nActionPid_; }
    /// @brief Set the registered Pid
    /// @param actionPID active process ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01624
    /// @trace_id_dd=DD_CRYPTO_04166
    /// @needwork = ad
    /// @endcode
    void SetActionPid(uint64_t const actionPID) noexcept { nActionPid_ = actionPID; }
};
//***************/
/// @brief IPC communication request packet: UnsubscribeObserver
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01625
/// @trace_id_dd=DD_CRYPTO_04167
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_UnsubscribeObserver final
{
public:
    /// @brief Key slot number (handle) to unsubscribe from observation
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04168
    /// @needwork = dda
    /// @endcode
    uint32_t nKeySlotID{0};
};
/// @brief IPC communication reply packet: UnsubscribeObserver
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01626
/// @trace_id_dd=DD_CRYPTO_04169
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_UnsubscribeObserver final : public PIpcAsw_LogicHead
{
private:
    /// @brief Key slot number (handle) that was unsubscribed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04170
    /// @needwork = dda
    /// @endcode
    uint32_t nKeySlotID_{0};

public:
    /// @brief Get the key slot number that was unsubscribed
    /// @return nKeySlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01627
    /// @trace_id_dd=DD_CRYPTO_04171
    /// @needwork = ad
    /// @endcode
    uint32_t GetKeySlotID() const noexcept { return nKeySlotID_; }
    /// @brief Set the key slot number that was unsubscribed
    /// @param keySlotID key slot number that was unsubscribed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01628
    /// @trace_id_dd=DD_CRYPTO_04172
    /// @needwork = ad
    /// @endcode
    void SetKeySlotID(uint32_t const keySlotID) noexcept { nKeySlotID_ = keySlotID; }
};
//***************/
/// @brief IPC communication request packet: UpdateObserver
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01629
/// @trace_id_dd=DD_CRYPTO_04173
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_UpdateOsbserver final
{
private:
    /// @brief Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04174
    /// @needwork = dda
    /// @endcode
    uint32_t nUpdateSlotID_{0};
    /// @brief Name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04175
    /// @needwork = dda
    /// @endcode
    uint16_t nNameLen_{0};

public:
    /// @brief Get the name
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01630
    /// @trace_id_dd=DD_CRYPTO_04176
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetName() const noexcept
    {
        return ara::core::StringView{static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this))
                                         + sizeof(PIpcAsw_UpdateOsbserver),
                                     static_cast< std::size_t >(nNameLen_)};
    }
    /// @brief Get the key slot ID
    /// @return nUpdateSlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01631
    /// @trace_id_dd=DD_CRYPTO_04177
    /// @needwork = ad
    /// @endcode
    uint32_t GetUpdateSlotID() const noexcept { return nUpdateSlotID_; }
    /// @brief Set the key slot ID
    /// @param nUpdateSlotID key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01632
    /// @trace_id_dd=DD_CRYPTO_04178
    /// @needwork = ad
    /// @endcode
    void SetUpdateSlotID(uint32_t const nUpdateSlotID) noexcept { nUpdateSlotID_ = nUpdateSlotID; }
    /// @brief Get the name length
    /// @return nNameLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01633
    /// @trace_id_dd=DD_CRYPTO_04179
    /// @needwork = ad
    /// @endcode
    uint16_t GetNameLen() const noexcept { return nNameLen_; }
    /// @brief Set the name length
    /// @param nNameLen name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01634
    /// @trace_id_dd=DD_CRYPTO_04180
    /// @needwork = ad
    /// @endcode
    void SetNameLen(uint16_t const nNameLen) noexcept { nNameLen_ = nNameLen; }
};
//***************/
/// @brief IPC communication request packet: FindKeySlot
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01635
/// @trace_id_dd=DD_CRYPTO_04181
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_FindKeySlot final
{
public:
    /// @brief Key slot ID // KeySlot identifier ID: first packet uses KeyName, otherwise SlotID is used to identify the key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04182
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID{0};

public:
};
/// @brief IPC communication reply packet: FindKeySlot
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01636
/// @trace_id_dd=DD_CRYPTO_04183
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_FindKeySlot final : public PIpcAsw_LogicHead
{
private:
    /// @brief KeySlot identifier ID: first packet uses KeyName, otherwise SlotID is used to identify the key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04184
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID_{0};
    /// @brief KeySlot name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04185
    /// @needwork = dda
    /// @endcode
    uint16_t nNameLen_{0};

public:
    /// @brief Get the name
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01637
    /// @trace_id_dd=DD_CRYPTO_04186
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetName() const noexcept
    {
        return ara::core::StringView(static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this))
                                         + sizeof(PIpcAsw_FindKeySlot),
                                     static_cast< std::size_t >(nNameLen_));
    }
    /// @brief Get the KeySlot identifier ID
    /// @return nIpcSlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01638
    /// @trace_id_dd=DD_CRYPTO_04187
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcSlotID() const noexcept { return nIpcSlotID_; }
    /// @brief Set the KeySlot identifier ID
    /// @param nIpcSlotID IPC key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01639
    /// @trace_id_dd=DD_CRYPTO_04188
    /// @needwork = ad
    /// @endcode
    void SetIpcSlotID(uint32_t const nIpcSlotID) noexcept { nIpcSlotID_ = nIpcSlotID; }
    /// @brief Get the KeySlot name length
    /// @return nNameLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01640
    /// @trace_id_dd=DD_CRYPTO_04189
    /// @needwork = ad
    /// @endcode
    uint16_t GetNameLen() const noexcept { return nNameLen_; }
    /// @brief Set the KeySlot name length
    /// @param nNameLen name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01641
    /// @trace_id_dd=DD_CRYPTO_04190
    /// @needwork = ad
    /// @endcode
    void SetNameLen(uint16_t const nNameLen) noexcept { nNameLen_ = nNameLen; }
};
/// @brief IPC communication request packet: GetHsmKeySlotID
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01642
/// @trace_id_dd=DD_CRYPTO_04191
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_GetHsmKeySlotID final : public PIpcAsw_LogicHead
{
private:
    /// @brief Key primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04192
    /// @needwork = dda
    /// @endcode
    uint32_t nKeyAlgId_{0U};
    /// @brief Key type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04193
    /// @needwork = dda
    /// @endcode
    uint32_t nKeyType_{0U};
    /// @brief Key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04194
    /// @needwork = dda
    /// @endcode
    uint32_t nKeyLen_{0U};
    /// @brief Whether shared
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04195
    /// @needwork = dda
    /// @endcode
    bool nShareSecert_{false};

public:
    /// @brief Get the key primitive ID
    /// @return nKeyAlgId_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01643
    /// @trace_id_dd=DD_CRYPTO_04196
    /// @needwork = ad
    /// @endcode
    uint32_t GetKeyAlgId() const noexcept { return nKeyAlgId_; }
    /// @brief Set the key primitive ID
    /// @param nKeyAlgId key primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01644
    /// @trace_id_dd=DD_CRYPTO_04197
    /// @needwork = ad
    /// @endcode
    void SetKeyAlgId(uint32_t const nKeyAlgId) noexcept { nKeyAlgId_ = nKeyAlgId; }
    /// @brief Get the key type
    /// @return nKeyType_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01645
    /// @trace_id_dd=DD_CRYPTO_04198
    /// @needwork = ad
    /// @endcode
    uint32_t GetKeyType() const noexcept { return nKeyType_; }
    /// @brief Set the key type
    /// @param nKeyType key type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01646
    /// @trace_id_dd=DD_CRYPTO_04199
    /// @needwork = ad
    /// @endcode
    void SetKeyType(uint32_t const nKeyType) noexcept { nKeyType_ = nKeyType; }
    /// @brief Get the key length
    /// @return nKeyLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01647
    /// @trace_id_dd=DD_CRYPTO_04200
    /// @needwork = ad
    /// @endcode
    uint32_t GetKeyLen() const noexcept { return nKeyLen_; }
    /// @brief Set the key length
    /// @param nKeyLen key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01648
    /// @trace_id_dd=DD_CRYPTO_04201
    /// @needwork = ad
    /// @endcode
    void SetKeyLen(uint32_t const nKeyLen) noexcept { nKeyLen_ = nKeyLen; }
    /// @brief Get whether shared
    /// @return nShareSecert_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01649
    /// @trace_id_dd=DD_CRYPTO_04202
    /// @needwork = ad
    /// @endcode
    bool GetShareSecert() const noexcept { return nShareSecert_; }
    /// @brief Set whether shared
    /// @param nShareSecert whether shared
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01650
    /// @trace_id_dd=DD_CRYPTO_04203
    /// @needwork = ad
    /// @endcode
    void SetShareSecert(bool const nShareSecert) noexcept { nShareSecert_ = nShareSecert; }
};
/// @brief IPC communication reply packet: GetHsmKeySlotID
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01651
/// @trace_id_dd=DD_CRYPTO_04204
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_GetHsmKeySlotID final : public PIpcAsw_LogicHead
{
private:
    /// @brief Cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04205
    /// @needwork = dda
    /// @endcode
    uint8_t nHsmSlotID_{0U};
    /// @brief Group ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04206
    /// @needwork = dda
    /// @endcode
    uint8_t nHsmGroupID_{0U};
    /// @brief Catalog ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04207
    /// @needwork = dda
    /// @endcode
    uint8_t nHsmCataLogID_{0U};

public:
    /// @brief Get the cryptographic primitive ID
    /// @return nHsmSlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01652
    /// @trace_id_dd=DD_CRYPTO_04208
    /// @needwork = ad
    /// @endcode
    uint8_t GetHsmSlotID() const noexcept { return nHsmSlotID_; }
    /// @brief Set the cryptographic primitive ID
    /// @param nHsmSlotID HSM key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01653
    /// @trace_id_dd=DD_CRYPTO_04209
    /// @needwork = ad
    /// @endcode
    void SetHsmSlotID(uint8_t const nHsmSlotID) noexcept { nHsmSlotID_ = nHsmSlotID; }
    /// @brief Get the group ID
    /// @return nHsmGroupID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01654
    /// @trace_id_dd=DD_CRYPTO_04210
    /// @needwork = ad
    /// @endcode
    uint8_t GetHsmGroupID() const noexcept { return nHsmGroupID_; }
    /// @brief Set the group ID
    /// @param nHsmGroupID HSM group ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01655
    /// @trace_id_dd=DD_CRYPTO_04211
    /// @needwork = ad
    /// @endcode
    void SetHsmGroupID(uint8_t const nHsmGroupID) noexcept { nHsmGroupID_ = nHsmGroupID; }
    /// @brief Get the catalog ID
    /// @return nHsmCataLogID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01656
    /// @trace_id_dd=DD_CRYPTO_04212
    /// @needwork = ad
    /// @endcode
    uint8_t GetHsmCataLogID() const noexcept { return nHsmCataLogID_; }
    /// @brief Set the catalog ID
    /// @param nCataLogID catalog ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01657
    /// @trace_id_dd=DD_CRYPTO_04213
    /// @needwork = ad
    /// @endcode
    void SetHsmCataLogID(uint8_t const nCataLogID) noexcept { nHsmCataLogID_ = nCataLogID; }
};
/// @brief IPC communication request packet: NotifyHsmdIsGenKeySuccess
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01658
/// @trace_id_dd=DD_CRYPTO_04214
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_NotifiyHsmdIsGenKeySucess final : public PIpcAsw_LogicHead
{
private:
    /// @brief Key ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04215
    /// @needwork = dda
    /// @endcode
    uint8_t nSlotID_{0U};
    /// @brief Group ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04216
    /// @needwork = dda
    /// @endcode
    uint8_t nGroupID_{0U};
    /// @brief Catalog ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04217
    /// @needwork = dda
    /// @endcode
    uint8_t nCataLogID_{0U};
    /// @brief Whether successful
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04218
    /// @needwork = dda
    /// @endcode
    uint8_t isSucess_{0U};

public:
    /// @brief Get the key ID
    /// @return nSlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01659
    /// @trace_id_dd=DD_CRYPTO_04219
    /// @needwork = ad
    /// @endcode
    uint8_t GetSlotID() const noexcept { return nSlotID_; }
    /// @brief Set the key ID
    /// @param nSlotID key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01660
    /// @trace_id_dd=DD_CRYPTO_04220
    /// @needwork = ad
    /// @endcode
    void SetSlotID(uint8_t const nSlotID) noexcept { nSlotID_ = nSlotID; }
    /// @brief Get the group ID
    /// @return nGroupID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01661
    /// @trace_id_dd=DD_CRYPTO_04221
    /// @needwork = ad
    /// @endcode
    uint8_t GetGroupID() const noexcept { return nGroupID_; }
    /// @brief Set the group ID
    /// @param nGroupID group ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01662
    /// @trace_id_dd=DD_CRYPTO_04222
    /// @needwork = ad
    /// @endcode
    void SetGroupID(uint8_t const nGroupID) noexcept { nGroupID_ = nGroupID; }
    /// @brief Get the catalog ID
    /// @return nCataLogID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01663
    /// @trace_id_dd=DD_CRYPTO_04223
    /// @needwork = ad
    /// @endcode
    uint8_t GetCataLogID() const noexcept { return nCataLogID_; }
    /// @brief Set the catalog ID
    /// @param nCataLogID catalog ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01664
    /// @trace_id_dd=DD_CRYPTO_04224
    /// @needwork = ad
    /// @endcode
    void SetCataLogID(uint8_t const nCataLogID) noexcept { nCataLogID_ = nCataLogID; }
    /// @brief Get whether successful
    /// @return isSucess_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01665
    /// @trace_id_dd=DD_CRYPTO_04225
    /// @needwork = ad
    /// @endcode
    uint8_t GetSucess() const noexcept { return isSucess_; }
    /// @brief Set whether successful
    /// @param nSucess whether successful
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01666
    /// @trace_id_dd=DD_CRYPTO_04226
    /// @needwork = ad
    /// @endcode
    void SetSucess(uint8_t const nSucess) noexcept { isSucess_ = nSucess; }
};
/// @brief IPC communication reply packet: NotifyHsmdIsGenKeySuccess
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03164
/// @trace_id_dd=DD_CRYPTO_06367
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcAsw_NotifiyHsmdIsGenKeySucess = PIpcAsw_LogicHead;
//***************/
}  // namespace keyprovider
//********************************/

/// @brief Namespace: x509provider
namespace x509provider {
/// @brief Get the name: certificate manager name
/// @return  certificate manager name
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01667
/// @trace_id_dd=DD_CRYPTO_04227
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView const GetName_PIpcX509Provider() noexcept
{
    return ara::core::StringView{"PIpcX509Provider"};
}
//***************/
/// @brief IPC communication request packet: check whether the certificate slot exists
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01668
/// @trace_id_dd=DD_CRYPTO_04228
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_IsCertExist final
{
public:
    /// @brief KeySlot name length
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04229
    /// @needwork = dda
    /// @endcode
    uint16_t nNameLen{0};

public:
    /// @brief Get the name
    /// @return certificate port name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01669
    /// @trace_id_dd=DD_CRYPTO_04230
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetName() const noexcept
    {
        return ara::core::StringView(static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this))
                                         + sizeof(PIpcReq_IsCertExist),
                                     static_cast< std::size_t >(nNameLen));
    }
};
/// @brief IPC communication reply packet: IsCertExist
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01670
/// @trace_id_dd=DD_CRYPTO_04231
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_IsCertExist final : public PIpcAsw_LogicHead
{
private:
    /// @brief Data length in the found certificate slot: -1 indicates not found, 0 indicates found but empty, other positive integers indicate data length in the certificate slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04232
    /// @needwork = dda
    /// @endcode
    uint16_t nFindLen_{0};

public:
    /// @brief Get the data length in the certificate slot
    /// @return nFindLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01671
    /// @trace_id_dd=DD_CRYPTO_04233
    /// @needwork = ad
    /// @endcode
    uint16_t GetFindLen() const noexcept { return nFindLen_; }
    /// @brief Set the data length in the certificate slot
    /// @param nFindLen data length in the found certificate slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01672
    /// @trace_id_dd=DD_CRYPTO_04234
    /// @needwork = ad
    /// @endcode
    void SetFindLen(uint16_t const nFindLen) noexcept { nFindLen_ = nFindLen; }
};
//***************/
/// @brief IPC communication request packet: LoadCert
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01673
/// @trace_id_dd=DD_CRYPTO_04235
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_LoadCert final
{
public:
    /// @brief KeySlot name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04236
    /// @needwork = dda
    /// @endcode
    uint16_t nNameLen{0};

public:
    /// @brief Get the name
    /// @return certificate port name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01674
    /// @trace_id_dd=DD_CRYPTO_04237
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetName() const noexcept
    {
        return ara::core::StringView(
            static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this)) + sizeof(PIpcReq_LoadCert),
            static_cast< std::size_t >(nNameLen));
    }
};
/// @brief IPC communication reply packet: LoadCert
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01675
/// @trace_id_dd=DD_CRYPTO_04238
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_LoadCert final : public PIpcAsw_LogicHead
{
private:
    /// @brief CertSlot identifier ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04239
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcCertID_{0};
    /// @brief Certificate format type: PEM/DER, etc.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04240
    /// @needwork = dda
    /// @endcode
    uint32_t nCertType_{0};
    /// @brief Certificate length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04241
    /// @needwork = dda
    /// @endcode
    uint16_t nCertLen_{0};
    /// @brief Deletion flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04242
    /// @needwork = dda
    /// @endcode
    uint32_t nDeleteFlag_{0};

public:
    /// @brief Get the CertSlot identifier ID
    /// @return nIpcCertID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01676
    /// @trace_id_dd=DD_CRYPTO_04243
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcCertID() const noexcept { return nIpcCertID_; }
    /// @brief Set the CertSlot identifier ID
    /// @param nIpcCertID CertSlot identifier ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01677
    /// @trace_id_dd=DD_CRYPTO_04244
    /// @needwork = ad
    /// @endcode
    void SetIpcCertID(uint32_t const nIpcCertID) noexcept { nIpcCertID_ = nIpcCertID; }
    /// @brief Get the certificate format type
    /// @return nCertType_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01678
    /// @trace_id_dd=DD_CRYPTO_04245
    /// @needwork = ad
    /// @endcode
    uint32_t GetCertType() const noexcept { return nCertType_; }
    /// @brief Set the certificate format type
    /// @param nCertType certificate format type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01679
    /// @trace_id_dd=DD_CRYPTO_04246
    /// @needwork = ad
    /// @endcode
    void SetCertType(uint32_t const nCertType) noexcept { nCertType_ = nCertType; }
    /// @brief Get the certificate length
    /// @return nCertLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01680
    /// @trace_id_dd=DD_CRYPTO_04247
    /// @needwork = ad
    /// @endcode
    uint16_t GetCertLen() const noexcept { return nCertLen_; }
    /// @brief Set the certificate length
    /// @param nCertLen certificate length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01681
    /// @trace_id_dd=DD_CRYPTO_04248
    /// @needwork = ad
    /// @endcode
    void SetCertLen(uint16_t const nCertLen) noexcept { nCertLen_ = nCertLen; }
    /// @brief Get the deletion flag
    /// @return nDeleteFlag_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01682
    /// @trace_id_dd=DD_CRYPTO_04249
    /// @needwork = ad
    /// @endcode
    uint32_t GetDeleteFlag() const noexcept { return nDeleteFlag_; }
    /// @brief Set the deletion flag
    /// @param nDeleteFlag deletion flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01683
    /// @trace_id_dd=DD_CRYPTO_04250
    /// @needwork = ad
    /// @endcode
    void SetDeleteFlag(uint32_t const nDeleteFlag) noexcept { nDeleteFlag_ = nDeleteFlag; }
};
//***************/
/// @brief IPC communication request packet: SaveCert
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01684
/// @trace_id_dd=DD_CRYPTO_04251
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_SaveCert final
{
public:
    /// @brief CertSlot identifier ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04252
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcCertID{0};
    /// @brief KeySlot name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04253
    /// @needwork = dda
    /// @endcode
    uint16_t nNameLen{0};
    /// @brief KeySlot data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04254
    /// @needwork = dda
    /// @endcode
    uint16_t nDataLen{0};

public:
    /// @brief Get the name
    /// @return certificate port name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01685
    /// @trace_id_dd=DD_CRYPTO_04255
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetName() const noexcept
    {
        return ara::core::StringView(
            static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this)) + sizeof(PIpcReq_SaveCert),
            static_cast< std::size_t >(nNameLen));
    }
    /// @brief Get the data
    /// @return starting address of the request attached data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01686
    /// @trace_id_dd=DD_CRYPTO_04256
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetData() const noexcept { return T_TransBytes(this) + sizeof(PIpcReq_SaveCert) + nNameLen; }
};
/// @brief IPC communication reply packet: SaveCert
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01687
/// @trace_id_dd=DD_CRYPTO_04257
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_SaveCert final : public PIpcAsw_LogicHead
{
private:
    /// @brief KeySlot identifier ID: first packet uses KeyName, otherwise SlotID is used to identify the key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04258
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcCertID_{0};

public:
    /// @brief Get the CertSlot identifier ID
    /// @return nIpcCertID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01688
    /// @trace_id_dd=DD_CRYPTO_04259
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcCertID() const noexcept { return nIpcCertID_; }
    /// @brief Set the CertSlot identifier ID
    /// @param nIpcCertID CertSlot identifier ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01689
    /// @trace_id_dd=DD_CRYPTO_04260
    /// @needwork = ad
    /// @endcode
    void SetIpcCertID(uint32_t const nIpcCertID) noexcept { nIpcCertID_ = nIpcCertID; }
};
//***************/
/// @brief IPC communication request packet: RemoveCert
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01690
/// @trace_id_dd=DD_CRYPTO_04261
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_RemoveCert final
{
public:
    /// @brief KeySlot name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04262
    /// @needwork = dda
    /// @endcode
    uint16_t nNameLen{0};

public:
    /// @brief Get the name
    /// @return certificate port name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01691
    /// @trace_id_dd=DD_CRYPTO_04263
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetName() const noexcept
    {
        return T_StringView(T_TransChar(this) + sizeof(PIpcReq_RemoveCert), static_cast< std::size_t >(nNameLen));
    }
};
/// @brief IPC communication reply packet: RemoveCert
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03165
/// @trace_id_dd=DD_CRYPTO_06368
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcAsw_RemoveCert = PIpcAsw_SaveCert;
//***************/
/// @brief IPC communication request packet: FindCert
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01692
/// @trace_id_dd=DD_CRYPTO_04264
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_FindCert final : public PIpcAsw_LogicHead
{
private:
    /// @brief CertSlot identifier ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04265
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcCertID_{0};
    /// @brief Certificate format type: PEM/DER, etc.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04266
    /// @needwork = dda
    /// @endcode
    uint32_t nCertType_{0};
    /// @brief Internal index of the current certificate in storage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04267
    /// @needwork = dda
    /// @endcode
    std::size_t nCertIndex_{0};
    /// @brief Certificate name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04268
    /// @needwork = dda
    /// @endcode
    uint16_t nCertNameLen_{0};
    /// @brief Certificate data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04269
    /// @needwork = dda
    /// @endcode
    uint16_t nCertDataLen_{0};
    /// @brief Deletion flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04270
    /// @needwork = dda
    /// @endcode
    uint32_t nDeleteFlag_{0};

public:
    /// @brief Get the certificate name
    /// @return certificate port name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01693
    /// @trace_id_dd=DD_CRYPTO_04271
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetCertName() const noexcept
    {
        return ara::core::StringView(
            static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this)) + sizeof(PIpcAsw_FindCert),
            static_cast< std::size_t >(nCertNameLen_));
    }
    /// @brief Get the certificate data
    /// @return starting address of the certificate data information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01694
    /// @trace_id_dd=DD_CRYPTO_04272
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetCertData() const noexcept
    {
        return static_cast< uint8_t const * >(static_cast< void const * >(this)) + sizeof(PIpcAsw_FindCert)
               + nCertNameLen_;
    }
    /// @brief Get the certificate ID
    /// @return nIpcCertID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01695
    /// @trace_id_dd=DD_CRYPTO_04273
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcCertID() const noexcept { return nIpcCertID_; }
    /// @brief Set the certificate ID
    /// @param nIpcCertID CertSlot identifier ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01696
    /// @trace_id_dd=DD_CRYPTO_04274
    /// @needwork = ad
    /// @endcode
    void SetIpcCertID(uint32_t const nIpcCertID) noexcept { nIpcCertID_ = nIpcCertID; }
    /// @brief Get the certificate type
    /// @return nCertType_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01697
    /// @trace_id_dd=DD_CRYPTO_04275
    /// @needwork = ad
    /// @endcode
    uint32_t GetCertType() const noexcept { return nCertType_; }
    /// @brief Set the certificate type
    /// @param nCertType certificate type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01698
    /// @trace_id_dd=DD_CRYPTO_04276
    /// @needwork = ad
    /// @endcode
    void SetCertType(uint32_t const nCertType) noexcept { nCertType_ = nCertType; }
    /// @brief Get the certificate index
    /// @return nCertIndex_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01699
    /// @trace_id_dd=DD_CRYPTO_04277
    /// @needwork = ad
    /// @endcode
    size_t GetCertIndex() const noexcept { return nCertIndex_; }
    /// @brief Set the certificate index
    /// @param nCertIndex certificate index
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01700
    /// @trace_id_dd=DD_CRYPTO_04278
    /// @needwork = ad
    /// @endcode
    void SetCertIndex(size_t const nCertIndex) noexcept { nCertIndex_ = nCertIndex; }
    /// @brief Get the certificate name length
    /// @return nCertNameLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01701
    /// @trace_id_dd=DD_CRYPTO_04279
    /// @needwork = ad
    /// @endcode
    uint16_t GetCertNameLen() const noexcept { return nCertNameLen_; }
    /// @brief Set the certificate name length
    /// @param nCertNameLen certificate name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01702
    /// @trace_id_dd=DD_CRYPTO_04280
    /// @needwork = ad
    /// @endcode
    void SetCertNameLen(uint16_t const nCertNameLen) noexcept { nCertNameLen_ = nCertNameLen; }
    /// @brief Get the certificate data length
    /// @return nCertDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01703
    /// @trace_id_dd=DD_CRYPTO_04281
    /// @needwork = ad
    /// @endcode
    uint16_t GetCertDataLen() const noexcept { return nCertDataLen_; }
    /// @brief Set the certificate data length
    /// @param nCertDataLen certificate data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01704
    /// @trace_id_dd=DD_CRYPTO_04282
    /// @needwork = ad
    /// @endcode
    void SetCertDataLen(uint16_t const nCertDataLen) noexcept { nCertDataLen_ = nCertDataLen; }
    /// @brief Get the deletion flag
    /// @return nDeleteFlag_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01705
    /// @trace_id_dd=DD_CRYPTO_04283
    /// @needwork = ad
    /// @endcode
    uint32_t GetDeleteFlag() const noexcept { return nDeleteFlag_; }
    /// @brief Set the deletion flag
    /// @param nDeleteFlag deletion flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01706
    /// @trace_id_dd=DD_CRYPTO_04284
    /// @needwork = ad
    /// @endcode
    void SetDeleteFlag(uint32_t const nDeleteFlag) noexcept { nDeleteFlag_ = nDeleteFlag; }
};
//***************/
/// @brief IPC communication request packet: FindCertByDn
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01707
/// @trace_id_dd=DD_CRYPTO_04285
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_FindCertByDn final
{
public:
    /// @brief Effective time point
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04286
    /// @needwork = dda
    /// @endcode
    time_t validityTime{0};
    /// @brief Internal index of the current certificate in storage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04287
    /// @needwork = dda
    /// @endcode
    std::size_t nCertIndex{-1LU};
    /// @brief subjectDn string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04288
    /// @needwork = dda
    /// @endcode
    uint16_t nSubjectDnLen{0};
    /// @brief issuerDn string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04289
    /// @needwork = dda
    /// @endcode
    uint16_t nIssuerDnLen{0};

public:
    /// @brief Get the subjectDn string
    /// @return subjectDn string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01708
    /// @trace_id_dd=DD_CRYPTO_04290
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetSubjectDn() const noexcept
    {
        return ara::core::StringView(static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this))
                                         + sizeof(PIpcReq_FindCertByDn),
                                     static_cast< std::size_t >(nSubjectDnLen));
    }
    /// @brief Get the issuerDn string
    /// @return issuerDn string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01709
    /// @trace_id_dd=DD_CRYPTO_04291
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetIssuerDn() const noexcept
    {
        return ara::core::StringView(static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this))
                                         + sizeof(PIpcReq_FindCertByDn) + nSubjectDnLen,
                                     static_cast< std::size_t >(nIssuerDnLen));
    }
};
//***************/
/// @brief IPC communication request packet: FindCertByKeyIds
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01710
/// @trace_id_dd=DD_CRYPTO_04292
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_FindCertByKeyIds final
{
public:
    /// @brief SubjectKeyId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04293
    /// @needwork = dda
    /// @endcode
    uint16_t nSubjectKeyIdLen{0};
    /// @brief AuthorityKeyId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04294
    /// @needwork = dda
    /// @endcode
    uint16_t nAuthorityKeyIdLen{0};

public:
    /// @brief Get the SubjectKeyId
    /// @return SubjectKeyId information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01711
    /// @trace_id_dd=DD_CRYPTO_04295
    /// @needwork = ad
    /// @endcode
    ara::crypto::ReadOnlyMemRegion GetSubjectKeyId() const noexcept
    {
        return ara::crypto::ReadOnlyMemRegion(
            static_cast< uint8_t const * >(static_cast< void const * >(this)) + sizeof(PIpcReq_FindCertByKeyIds),
            static_cast< std::size_t >(nSubjectKeyIdLen));
    }
    /// @brief Get the AuthorityKeyId
    /// @return AuthorityKeyId information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01712
    /// @trace_id_dd=DD_CRYPTO_04296
    /// @needwork = ad
    /// @endcode
    ara::crypto::ReadOnlyMemRegion GetAuthorityKeyId() const noexcept
    {
        return ara::crypto::ReadOnlyMemRegion(static_cast< uint8_t const * >(static_cast< void const * >(this))
                                                  + sizeof(PIpcReq_FindCertByKeyIds) + nSubjectKeyIdLen,
                                              static_cast< std::size_t >(nAuthorityKeyIdLen));
    }
};
//***************/
/// @brief IPC communication request packet: FindCertBySn
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01713
/// @trace_id_dd=DD_CRYPTO_04297
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_FindCertBySn final
{
public:
    /// @brief SN string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04298
    /// @needwork = dda
    /// @endcode
    uint16_t nSnLen{0};
    /// @brief issuerDn string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04299
    /// @needwork = dda
    /// @endcode
    uint16_t nIssuerDnLen{0};

public:
    /// @brief Get the SN string
    /// @return SN string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01714
    /// @trace_id_dd=DD_CRYPTO_04300
    /// @needwork = ad
    /// @endcode
    ara::crypto::ReadOnlyMemRegion GetSerialNumber() const noexcept
    {
        return ara::crypto::ReadOnlyMemRegion(
            static_cast< uint8_t const * >(static_cast< void const * >(this)) + sizeof(PIpcReq_FindCertBySn),
            static_cast< std::size_t >(nSnLen));
    }
    /// @brief Get the issuerDn string
    /// @return issuerDn string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01715
    /// @trace_id_dd=DD_CRYPTO_04301
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetIssuerDn() const noexcept
    {
        return ara::core::StringView(static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this))
                                         + sizeof(PIpcReq_FindCertBySn) + nSnLen,
                                     static_cast< std::size_t >(nIssuerDnLen));
    }
};
//***************/
/// @brief IPC communication request packet: IsTrustMaster
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01716
/// @trace_id_dd=DD_CRYPTO_04302
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_IsTrustMaster final
{
};
/// @brief IPC communication reply packet: IsTrustMaster
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01717
/// @trace_id_dd=DD_CRYPTO_04303
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_IsTrustMaster final : public PIpcAsw_LogicHead
{
private:
    /// @brief Whether successful
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04304
    /// @needwork = dda
    /// @endcode
    bool isSucess_{false};

public:
    /// @brief Get whether successful
    /// @return isSucess
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01718
    /// @trace_id_dd=DD_CRYPTO_04305
    /// @needwork = ad
    /// @endcode
    bool GetIsSucess() const noexcept { return isSucess_; }
    /// @brief Set whether successful
    /// @param nIsSucess whether successful
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01719
    /// @trace_id_dd=DD_CRYPTO_04306
    /// @needwork = ad
    /// @endcode
    void SetIsSucess(bool const nIsSucess) noexcept { isSucess_ = nIsSucess; }
};
//***************/
/// @brief IPC communication request packet: SetCertExInfo
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01720
/// @trace_id_dd=DD_CRYPTO_04307
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_SetCertExInfo final
{
public:
    /// @brief KeySlot name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04308
    /// @needwork = dda
    /// @endcode
    uint16_t nNameLen{0};
    /// @brief Which byte of the extension field to set, starting from 0
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04309
    /// @needwork = dda
    /// @endcode
    uint16_t nIndex{0};

public:
    /// @brief Get the name
    /// @return certificate port name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01721
    /// @trace_id_dd=DD_CRYPTO_04310
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetName() const noexcept
    {
        return ara::core::StringView(static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this))
                                         + sizeof(PIpcReq_SetCertExInfo),
                                     static_cast< std::size_t >(nNameLen));
    }
};
//***************/
/// @brief IPC communication request packet: RevokeCert
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01722
/// @trace_id_dd=DD_CRYPTO_04311
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_RevokeCert final
{
public:
    /// @brief KeySlot name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04312
    /// @needwork = dda
    /// @endcode
    uint16_t nNameLen{0};
    /// @brief IssuerSerialNum name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04313
    /// @needwork = dda
    /// @endcode
    uint16_t nIssureSerialNumLen{0};

public:
    /// @brief Get the name
    /// @return  certificate port name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01723
    /// @trace_id_dd=DD_CRYPTO_04314
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetName() const noexcept
    {
        return ara::core::StringView(
            static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this)) + sizeof(PIpcReq_LoadCert),
            static_cast< std::size_t >(nNameLen));
    }
    /// @brief Get the SerialNum
    /// @return SerialNum
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01724
    /// @trace_id_dd=DD_CRYPTO_04315
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetSerialNum() const noexcept
    {
        return ara::core::StringView(static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this))
                                         + sizeof(PIpcReq_LoadCert) + nNameLen,
                                     static_cast< std::size_t >(nIssureSerialNumLen));
    }
};
/// @brief IPC communication reply packet: used to hold the result of revoke cert: true or false
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01725
/// @trace_id_dd=DD_CRYPTO_04316
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_RevokeCert final : public PIpcAsw_LogicHead
{
private:
    /// @brief Whether revocation succeeded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04317
    /// @needwork = dda
    /// @endcode
    bool bRevokeSuccess_{true};

public:
    /// @brief Get whether revocation succeeded
    /// @return bRevokeSuccess_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01726
    /// @trace_id_dd=DD_CRYPTO_04318
    /// @needwork = ad
    /// @endcode
    bool GetRevokeSuccess() const noexcept { return bRevokeSuccess_; }
    /// @brief Set whether revocation succeeded
    /// @param nRevokeSuccess whether revocation succeeded
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01727
    /// @trace_id_dd=DD_CRYPTO_04319
    /// @needwork = ad
    /// @endcode
    void SetRevokeSuccess(bool const nRevokeSuccess) noexcept { bRevokeSuccess_ = nRevokeSuccess; }
};
//***************/
/// @brief IPC communication request packet: GenLoadCert
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01728
/// @trace_id_dd=DD_CRYPTO_04320
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_GenLoadCert final
{
public:
    /// @brief KeySlot name length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04321
    /// @needwork = dda
    /// @endcode
    uint16_t nNameLen{0};

public:
    /// @brief Get the name
    /// @return certificate port name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01729
    /// @trace_id_dd=DD_CRYPTO_04322
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetName() const noexcept
    {
        return ara::core::StringView(static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this))
                                         + sizeof(PIpcReq_GenLoadCert),
                                     static_cast< std::size_t >(nNameLen));
    }
};
/// @brief IPC communication reply packet: used to hold the generated CRL result
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01730
/// @trace_id_dd=DD_CRYPTO_04323
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_GenCRL final : public PIpcAsw_LogicHead
{
private:
    /// @brief CRL data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04324
    /// @needwork = dda
    /// @endcode
    uint16_t nDataLen_{0};

public:
    /// @brief Get the CRL data length
    /// @return nDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01731
    /// @trace_id_dd=DD_CRYPTO_04325
    /// @needwork = ad
    /// @endcode
    uint16_t GetDataLen() const noexcept { return nDataLen_; }
    /// @brief Set the CRL data length
    /// @param nDataLen CRL data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01732
    /// @trace_id_dd=DD_CRYPTO_04326
    /// @needwork = ad
    /// @endcode
    void SetDataLen(uint16_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
};
//***************/
/// @brief IPC communication reply packet: SetCertExInfo
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03166
/// @trace_id_dd=DD_CRYPTO_06369
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcAsw_SetCertExInfo = PIpcAsw_IsTrustMaster;
//***************/
/// @brief IPC communication request packet: GetAsRootOfTrust
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03167
/// @trace_id_dd=DD_CRYPTO_06370
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcReq_GetAsRootOfTrust = PIpcReq_IsTrustMaster;
/// @brief IPC communication reply packet: GetAsRootOfTrust
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03168
/// @trace_id_dd=DD_CRYPTO_06371
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_GetAsRootOfTrust final : public PIpcAsw_LogicHead
{
};
//***************/
/// @brief IPC communication request packet: IsCsrSetPending
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03169
/// @trace_id_dd=DD_CRYPTO_06372
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcReq_IsCsrSetPending = PIpcReq_RemoveCert;
/// @brief IPC communication reply packet: IsCsrSetPending
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03170
/// @trace_id_dd=DD_CRYPTO_06373
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcAsw_IsCsrSetPending = PIpcAsw_IsTrustMaster;
//***************/
/// @brief IPC communication request packet: GetCsrNames
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03171
/// @trace_id_dd=DD_CRYPTO_06374
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcReq_GetCsrNames = PIpcReq_GetAsRootOfTrust;
/// @brief IPC communication reply packet: GetCsrNames
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03172
/// @trace_id_dd=DD_CRYPTO_06375
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcAsw_GetCsrNames = PIpcAsw_GetAsRootOfTrust;
//***************/
/// @brief IPC communication request packet: OcspRequest
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01733
/// @trace_id_dd=DD_CRYPTO_04327
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_OcspReuest final
{
public:
    /// @brief OCSP request information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04328
    /// @needwork = dda
    /// @endcode
    uint16_t nOcspResInfoLen;
    /// @brief Issuer certificate number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04329
    /// @needwork = dda
    /// @endcode
    uint16_t nIssureCertNum;
    /// @brief Get the OCSP request information
    /// @return starting address of the OCSP request attached information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01734
    /// @trace_id_dd=DD_CRYPTO_04330
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetOcspResInfo() const noexcept
    {
        return static_cast< uint8_t const * >(static_cast< void const * >(this)) + sizeof(PIpcReq_OcspReuest);
    }
    /// @brief Return the issuer certificate number
    /// @return starting address of the issuer certificate number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01735
    /// @trace_id_dd=DD_CRYPTO_04331
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetIssureCert() const noexcept
    {
        return static_cast< uint8_t const * >(static_cast< void const * >(this)) + sizeof(PIpcReq_OcspReuest)
               + nOcspResInfoLen;
    }
};
/// @brief IPC communication reply packet: OcspRequest
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01736
/// @trace_id_dd=DD_CRYPTO_04332
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_OcspReuest final : public PIpcAsw_LogicHead
{
private:
    /// @brief OCSP return information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04333
    /// @needwork = dda
    /// @endcode
    uint16_t nOcspResponseInfoLen_;

public:
    /// @brief Get the OCSP return information
    /// @return nOcspResponseInfoLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01737
    /// @trace_id_dd=DD_CRYPTO_04334
    /// @needwork = ad
    /// @endcode
    uint16_t GetOcspResponseInfoLen() const noexcept { return nOcspResponseInfoLen_; }
    /// @brief Set the OCSP return information
    /// @param nDataLen data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01738
    /// @trace_id_dd=DD_CRYPTO_04335
    /// @needwork = ad
    /// @endcode
    void SetOcspResponseInfoLen(uint16_t const nDataLen) noexcept { nOcspResponseInfoLen_ = nDataLen; }
    /// @brief Get the OCSP return information
    /// @return starting address of the OCSP return information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01739
    /// @trace_id_dd=DD_CRYPTO_04336
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetOcspResponseInfo() const noexcept
    {
        return static_cast< uint8_t const * >(static_cast< void const * >(this)) + sizeof(PIpcAsw_OcspReuest);
    }
};
//***************/
/// @brief IPC communication request packet: GetCertSlotNames
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03173
/// @trace_id_dd=DD_CRYPTO_06376
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcReq_GetCertSlotNames = PIpcReq_IsCsrSetPending;
/// @brief IPC communication reply packet: GetCertSlotNames
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01740
/// @trace_id_dd=DD_CRYPTO_04337
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_GetCertSlotNames final : public PIpcAsw_LogicHead
{
private:
    /// @brief Key slot name one
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04338
    /// @needwork = dda
    /// @endcode
    uint16_t nSlotNameOne_;
    /// @brief Key slot name two
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04339
    /// @needwork = dda
    /// @endcode
    uint16_t nSlotNameTwo_;

public:
    /// @brief Get key slot name one.
    /// @return nSlotNameOne_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01741
    /// @trace_id_dd=DD_CRYPTO_04340
    /// @needwork = ad
    /// @endcode
    uint16_t GetSlotNameOne() const noexcept { return nSlotNameOne_; }
    /// @brief Set key slot name one.
    /// @param nSlotNameOne Key slot name one
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01742
    /// @trace_id_dd=DD_CRYPTO_04341
    /// @needwork = ad
    /// @endcode
    void SetSlotNameOne(uint16_t const nSlotNameOne) noexcept { nSlotNameOne_ = nSlotNameOne; }
    /// @brief Get key slot name two.
    /// @return nSlotNameTwo_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01743
    /// @trace_id_dd=DD_CRYPTO_04342
    /// @needwork = ad
    /// @endcode
    uint16_t GetSlotNameTwo() const noexcept { return nSlotNameTwo_; }
    /// @brief Set key slot name two.
    /// @param nSlotNameTwo Key slot name two
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01744
    /// @trace_id_dd=DD_CRYPTO_04343
    /// @needwork = ad
    /// @endcode
    void SetSlotNameTwo(uint16_t const nSlotNameTwo) noexcept { nSlotNameTwo_ = nSlotNameTwo; }
    /// @brief Get key slot name one.
    /// @return Key slot name one
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01745
    /// @trace_id_dd=DD_CRYPTO_04344
    /// @needwork = ad
    /// @endcode
    ara::core::String GetSlotNameOneString() const noexcept
    {
        return ara::core::String(static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this))
                                     + sizeof(PIpcAsw_GetCertSlotNames),
                                 static_cast< std::size_t >(nSlotNameOne_));
    }
    /// @brief Get key slot name two.
    /// @return Key slot name two
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01746
    /// @trace_id_dd=DD_CRYPTO_04345
    /// @needwork = ad
    /// @endcode
    ara::core::String GetSlotNameTwoString() const noexcept
    {
        return ara::core::String(static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(this))
                                     + sizeof(PIpcAsw_GetCertSlotNames) + nSlotNameOne_,
                                 static_cast< std::size_t >(nSlotNameTwo_));
    }
};
}  // namespace x509provider
//********************************/
/// @brief Namespace: keyslot
namespace keyslot {
/// @brief Get name: PIpcKeySlot
/// @return PIpcKeySlot
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01747
/// @trace_id_dd=DD_CRYPTO_04346
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView const GetName_PIpcKeySlot() noexcept { return ara::core::StringView{"PIpcKeySlot"}; }
//***************/
/// @brief IPC: key slot data communication base class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01748
/// @trace_id_dd=DD_CRYPTO_04347
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_SlotBase
{
public:
    /// @brief default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01749
    /// @trace_id_dd=DD_CRYPTO_04348
    /// @needwork = ad
    /// @endcode
    virtual ~PIpcReq_SlotBase() = default;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01750
    /// @trace_id_dd=DD_CRYPTO_04349
    /// @needwork = ad
    /// @endcode
    PIpcReq_SlotBase() = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01751
    /// @trace_id_dd=DD_CRYPTO_04350
    /// @needwork = ad
    /// @endcode
    PIpcReq_SlotBase(PIpcReq_SlotBase const &other) = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01752
    /// @trace_id_dd=DD_CRYPTO_04351
    /// @needwork = ad
    /// @endcode
    PIpcReq_SlotBase(PIpcReq_SlotBase &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01753
    /// @trace_id_dd=DD_CRYPTO_04352
    /// @needwork = ad
    /// @endcode
    PIpcReq_SlotBase &operator=(PIpcReq_SlotBase const &other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01754
    /// @trace_id_dd=DD_CRYPTO_04353
    /// @needwork = ad
    /// @endcode
    PIpcReq_SlotBase &operator=(PIpcReq_SlotBase &&other) = delete;

private:
    /// @brief KeySlot identifier ID: except for the first packet which uses KeyName, other cases use SlotID to identify the key slot.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04354
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID_{0};

public:
    /// @brief Get the KeySlot identifier ID.
    /// @return nIpcSlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01755
    /// @trace_id_dd=DD_CRYPTO_04355
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcSlotID() const noexcept { return nIpcSlotID_; }
    /// @brief Set the KeySlot identifier ID.
    /// @param nIpcSlotID IPC key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01756
    /// @trace_id_dd=DD_CRYPTO_04356
    /// @needwork = ad
    /// @endcode
    void SetIpcSlotID(uint32_t const nIpcSlotID) noexcept { nIpcSlotID_ = nIpcSlotID; }
};
//***************/
/// @brief IPC communication request packet: Clear key slot content
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03174
/// @trace_id_dd=DD_CRYPTO_06377
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcReq_Clear = PIpcReq_SlotBase;
/// @brief IPC communication reply packet: Clear key slot content
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03175
/// @trace_id_dd=DD_CRYPTO_06378
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcAsw_Clear = PIpcAsw_LogicHead;
//***************/
/// @brief IPC communication request packet: Check if key slot is empty
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03176
/// @trace_id_dd=DD_CRYPTO_06379
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcReq_IsEmpty = PIpcReq_SlotBase;
/// @brief IPC communication reply packet: Check if key slot is empty
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01757
/// @trace_id_dd=DD_CRYPTO_04357
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_IsEmpty final : public PIpcAsw_LogicHead
{
private:
    /// @brief Whether empty
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04358
    /// @needwork = dda
    /// @endcode
    uint16_t bEmpty_{0};

public:
    /// @brief Get whether empty.
    /// @return bEmpty_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01758
    /// @trace_id_dd=DD_CRYPTO_04359
    /// @needwork = ad
    /// @endcode
    uint16_t GetEmpty() const noexcept { return bEmpty_; }
    /// @brief Set whether empty.
    /// @param bEmpty Whether empty
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01759
    /// @trace_id_dd=DD_CRYPTO_04360
    /// @needwork = ad
    /// @endcode
    void SetEmpty(uint16_t const bEmpty) noexcept { bEmpty_ = bEmpty; }
};
//***************/
/// @brief IPC communication request packet: Get key slot content attributes
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03177
/// @trace_id_dd=DD_CRYPTO_06380
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcReq_GetContentProps = PIpcReq_SlotBase;
/// @brief IPC communication reply packet: Get key slot content attributes
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01760
/// @trace_id_dd=DD_CRYPTO_04361
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_GetContentProps final : public PIpcAsw_LogicHead
{
private:
    /// @brief Key slot context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04362
    /// @needwork = dda
    /// @endcode
    KeySlotContentProps slotCont_;

public:
    /// @brief Get the key slot context.
    /// @return slotCont_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01761
    /// @trace_id_dd=DD_CRYPTO_04363
    /// @needwork = ad
    /// @endcode
    KeySlotContentProps GetKeySlotContentProps() const noexcept { return slotCont_; }
    /// @brief Set the key slot context.
    /// @param slotCont Key slot context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01762
    /// @trace_id_dd=DD_CRYPTO_04364
    /// @needwork = ad
    /// @endcode
    void SetKeySlotContentProps(KeySlotContentProps const &slotCont) noexcept { slotCont_ = slotCont; }
};
//***************/
/// @brief IPC communication request packet: Get key slot attributes
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03178
/// @trace_id_dd=DD_CRYPTO_06381
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcReq_GetPrototypedProps = PIpcReq_SlotBase;
/// @brief IPC communication reply packet: Get key slot attributes
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01763
/// @trace_id_dd=DD_CRYPTO_04365
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_GetPrototypedProps final : public PIpcAsw_LogicHead
{
private:
    /// @brief Key slot attributes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04366
    /// @needwork = dda
    /// @endcode
    KeySlotPrototypeProps slotProp_;

public:
    /// @brief Get the key slot attributes.
    /// @return slotProp_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01764
    /// @trace_id_dd=DD_CRYPTO_04367
    /// @needwork = ad
    /// @endcode
    KeySlotPrototypeProps GetKeySlotPrototypeProps() const noexcept { return slotProp_; }
    /// @brief Set the key slot attributes.
    /// @param keySlotPrototypeProps Key slot attributes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01765
    /// @trace_id_dd=DD_CRYPTO_04368
    /// @needwork = ad
    /// @endcode
    void SetKeySlotPrototypeProps(KeySlotPrototypeProps const &keySlotPrototypeProps) noexcept
    {
        slotProp_ = keySlotPrototypeProps;
    }
};
//***************/
/// @brief IPC communication request packet: Open key slot
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01766
/// @trace_id_dd=DD_CRYPTO_04369
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_Open final : public PIpcReq_SlotBase
{
private:
    /// @brief Whether to subscribe
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04370
    /// @needwork = dda
    /// @endcode
    bool subscribeForUpdates_{false};
    /// @brief Whether writable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04371
    /// @needwork = dda
    /// @endcode
    bool writeable_{false};

public:
    /// @brief Get whether to subscribe.
    /// @return subscribeForUpdates_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01767
    /// @trace_id_dd=DD_CRYPTO_04372
    /// @needwork = ad
    /// @endcode
    bool GetSubscribeForUpdates() const noexcept { return subscribeForUpdates_; }
    /// @brief Set whether to subscribe.
    /// @param nSubscribeForUpdates Whether to subscribe
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01768
    /// @trace_id_dd=DD_CRYPTO_04373
    /// @needwork = ad
    /// @endcode
    void SetSubscribeForUpdates(bool const nSubscribeForUpdates) noexcept
    {
        subscribeForUpdates_ = nSubscribeForUpdates;
    }
    /// @brief Get whether writable.
    /// @return writeable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01769
    /// @trace_id_dd=DD_CRYPTO_04374
    /// @needwork = ad
    /// @endcode
    bool GetWriteable() const noexcept { return writeable_; }
    /// @brief Set whether writable.
    /// @param nWriteable Whether writable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01770
    /// @trace_id_dd=DD_CRYPTO_04375
    /// @needwork = ad
    /// @endcode
    void SetWriteable(bool const nWriteable) noexcept { writeable_ = nWriteable; }
};
/// @brief IPC communication reply packet: Open key slot
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01771
/// @trace_id_dd=DD_CRYPTO_04376
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_Open final : public PIpcAsw_LogicHead
{
private:
    /// @brief IO interface number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04377
    /// @needwork = dda
    /// @endcode
    uint32_t nIoInterfaceID_{0};

public:
    /// @brief Get the IO interface number.
    /// @return nIoInterfaceID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01772
    /// @trace_id_dd=DD_CRYPTO_04378
    /// @needwork = ad
    /// @endcode
    uint32_t GetIoInterfaceID() const noexcept { return nIoInterfaceID_; }
    /// @brief Set the IO interface number.
    /// @param nIoInterfaceID IO interface ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01773
    /// @trace_id_dd=DD_CRYPTO_04379
    /// @needwork = ad
    /// @endcode
    void SetIoInterfaceID(uint32_t const nIoInterfaceID) noexcept { nIoInterfaceID_ = nIoInterfaceID; }
};
//***************/
/// @brief IPC communication request packet: Close key slot
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01774
/// @trace_id_dd=DD_CRYPTO_04380
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_Close final : public PIpcReq_SlotBase
{
private:
    /// @brief Whether to switch to read-only mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04381
    /// @needwork = dda
    /// @endcode
    bool bReadOnly_{false};

public:
    /// @brief Get whether to switch to read-only mode.
    /// @return bReadOnly_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01775
    /// @trace_id_dd=DD_CRYPTO_04382
    /// @needwork = ad
    /// @endcode
    bool GetReadOnly() const noexcept { return bReadOnly_; }
    /// @brief Set whether to switch to read-only mode.
    /// @param nReadOnly Whether to switch to read-only mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01776
    /// @trace_id_dd=DD_CRYPTO_04383
    /// @needwork = ad
    /// @endcode
    void SetReadOnly(bool const nReadOnly) noexcept { bReadOnly_ = nReadOnly; }
};
/// @brief IPC communication reply packet: Close key slot
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01777
/// @trace_id_dd=DD_CRYPTO_04384
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_Close final : public PIpcAsw_LogicHead
{
private:
    /// @brief Whether closed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04385
    /// @needwork = dda
    /// @endcode
    bool bClose_{false};

public:
    /// @brief Get whether closed.
    /// @return bClose_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01778
    /// @trace_id_dd=DD_CRYPTO_04386
    /// @needwork = ad
    /// @endcode
    bool GetClose() const noexcept { return bClose_; }
    /// @brief Set whether closed.
    /// @param nClose Whether closed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01779
    /// @trace_id_dd=DD_CRYPTO_04387
    /// @needwork = ad
    /// @endcode
    void SetClose(bool const nClose) noexcept { bClose_ = nClose; }
};
//***************/
/// @brief IPC communication request packet: Save a copy
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01780
/// @trace_id_dd=DD_CRYPTO_04388
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_SaveCopy_Slot final : public PIpcReq_SlotBase
{
private:
    /// @brief IO interface number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04389
    /// @needwork = dda
    /// @endcode
    uint32_t nIoInterfaceID_{0};
    /// @brief Slot ID of the data to copy
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04390
    /// @needwork = dda
    /// @endcode
    uint32_t nSaveCopySlotID_{0};
    /// @brief Crypto primitive ID of KeyKey
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04391
    /// @needwork = dda
    /// @endcode
    CryptoAlgId keyKeyAlgID_{kAlgIdAny};

public:
    /// @brief Get the IO interface number.
    /// @return nIoInterfaceID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01781
    /// @trace_id_dd=DD_CRYPTO_04392
    /// @needwork = ad
    /// @endcode
    uint32_t GetIoInterfaceID() const noexcept { return nIoInterfaceID_; }
    /// @brief Set the IO interface number.
    /// @param nIoInterfaceID IO interface ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01782
    /// @trace_id_dd=DD_CRYPTO_04393
    /// @needwork = ad
    /// @endcode
    void SetIoInterfaceID(uint32_t const nIoInterfaceID) noexcept { nIoInterfaceID_ = nIoInterfaceID; }
    /// @brief Get the slot ID of the data to copy.
    /// @return nSaveCopySlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01783
    /// @trace_id_dd=DD_CRYPTO_04394
    /// @needwork = ad
    /// @endcode
    uint32_t GetSaveCopySlotID() const noexcept { return nSaveCopySlotID_; }
    /// @brief Set the slot ID of the data to copy.
    /// @param nSaveCopySlotID Slot ID of the data to copy
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01784
    /// @trace_id_dd=DD_CRYPTO_04395
    /// @needwork = ad
    /// @endcode
    void SetSaveCopySlotID(uint32_t const nSaveCopySlotID) noexcept { nSaveCopySlotID_ = nSaveCopySlotID; }
    /// @brief Get the crypto primitive ID of KeyKey.
    /// @return keyKeyAlgID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01785
    /// @trace_id_dd=DD_CRYPTO_04396
    /// @needwork = ad
    /// @endcode
    CryptoAlgId GetKeyKeyAlgID() const noexcept { return keyKeyAlgID_; }
    /// @brief Set the crypto primitive ID of KeyKey.
    /// @param nKeyKeyAlgID Crypto primitive ID of KeyKey
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01786
    /// @trace_id_dd=DD_CRYPTO_04397
    /// @needwork = ad
    /// @endcode
    void SetKeyKeyAlgID(CryptoAlgId const nKeyKeyAlgID) noexcept { keyKeyAlgID_ = nKeyKeyAlgID; }
};
/// @brief IPC communication reply packet: Save a copy
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01787
/// @trace_id_dd=DD_CRYPTO_04398
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_SaveCopy_Slot final : public PIpcAsw_LogicHead
{
private:
    /// @brief Key slot number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04399
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID_{0};
    /// @brief IO interface number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04400
    /// @needwork = dda
    /// @endcode
    uint32_t nIoInterfaceID_{0};

public:
    /// @brief Get the key slot number.
    /// @return nIpcSlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01788
    /// @trace_id_dd=DD_CRYPTO_04401
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcSlotID() const noexcept { return nIpcSlotID_; }
    /// @brief Set the key slot number.
    /// @param nIpcSlotID IPC key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01789
    /// @trace_id_dd=DD_CRYPTO_04402
    /// @needwork = ad
    /// @endcode
    void SetIpcSlotID(uint32_t const nIpcSlotID) noexcept { nIpcSlotID_ = nIpcSlotID; }
    /// @brief Get the IO interface number.
    /// @return nIoInterfaceID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01790
    /// @trace_id_dd=DD_CRYPTO_04403
    /// @needwork = ad
    /// @endcode
    uint32_t GetIoInterfaceID() const noexcept { return nIoInterfaceID_; }
    /// @brief Set the IO interface number.
    /// @param nIoInterfaceID IO interface ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01791
    /// @trace_id_dd=DD_CRYPTO_04404
    /// @needwork = ad
    /// @endcode
    void SetIoInterfaceID(uint32_t const nIoInterfaceID) noexcept { nIoInterfaceID_ = nIoInterfaceID; }
};
//***************/
/// @brief IPC communication request packet: Save key slot data from memory
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01792
/// @trace_id_dd=DD_CRYPTO_04405
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_SaveCopy_Mem final : public PIpcReq_SlotBase
{
private:
    /// @brief Attributes of the key slot content
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04406
    /// @needwork = dda
    /// @endcode
    KeySlotContentProps keyCont_;
    /// @brief Attributes of the key slot itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04407
    /// @needwork = dda
    /// @endcode
    KeySlotPrototypeProps slotProp_;
    /// @brief Encryption algorithm for the plain key; KeyKey may be randomly generated automatically.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04408
    /// @needwork = dda
    /// @endcode
    CryptoAlgId cryptoAlgID_{kAlgIdAny};
    /// @brief Key length of the encryption/decryption key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04409
    /// @needwork = dda
    /// @endcode
    uint16_t nKeyKeyLen_{0};
    /// @brief Length of the key data stored in the key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04410
    /// @needwork = dda
    /// @endcode
    uint16_t nKeyDataLen_{0};

public:
    /// @brief Get the attributes of the key slot content.
    /// @return keyCont_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01793
    /// @trace_id_dd=DD_CRYPTO_04411
    /// @needwork = ad
    /// @endcode
    KeySlotContentProps GetKeySlotContentProps() const noexcept { return keyCont_; }
    /// @brief Set the attributes of the key slot content.
    /// @param nkeyCont Attributes of the key slot content
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01794
    /// @trace_id_dd=DD_CRYPTO_04412
    /// @needwork = ad
    /// @endcode
    void SetKeySlotContentProps(KeySlotContentProps const &nkeyCont) noexcept { keyCont_ = nkeyCont; }
    /// @brief Get the attributes of the key slot itself.
    /// @return slotProp_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01795
    /// @trace_id_dd=DD_CRYPTO_04413
    /// @needwork = ad
    /// @endcode
    KeySlotPrototypeProps GetKeySlotPrototypeProps() const noexcept { return slotProp_; }
    /// @brief Set the attributes of the key slot itself.
    /// @param nKeySlotPrototypeProps Attributes of the key slot itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01796
    /// @trace_id_dd=DD_CRYPTO_04414
    /// @needwork = ad
    /// @endcode
    void SetKeySlotPrototypeProps(KeySlotPrototypeProps const &nKeySlotPrototypeProps) noexcept
    {
        slotProp_ = nKeySlotPrototypeProps;
    }
    /// @brief Get the encryption algorithm for the plain key.
    /// @return cryptoAlgID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01797
    /// @trace_id_dd=DD_CRYPTO_04415
    /// @needwork = ad
    /// @endcode
    CryptoAlgId GetCryptoAlgID() const noexcept { return cryptoAlgID_; }
    /// @brief Set the encryption algorithm for the plain key.
    /// @param nCryptoAlgID Encryption algorithm for the plain key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01798
    /// @trace_id_dd=DD_CRYPTO_04416
    /// @needwork = ad
    /// @endcode
    void SetCryptoAlgID(CryptoAlgId const nCryptoAlgID) noexcept { cryptoAlgID_ = nCryptoAlgID; }
    /// @brief Get the key length of the encryption/decryption key slot.
    /// @return nKeyKeyLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01799
    /// @trace_id_dd=DD_CRYPTO_04417
    /// @needwork = ad
    /// @endcode
    uint16_t GetKeyKeyLen() const noexcept { return nKeyKeyLen_; }
    /// @brief Set the key length of the encryption/decryption key slot.
    /// @param nKeyKeyLen KeyKey length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01800
    /// @trace_id_dd=DD_CRYPTO_04418
    /// @needwork = ad
    /// @endcode
    void SetKeyKeyLen(uint16_t const nKeyKeyLen) noexcept { nKeyKeyLen_ = nKeyKeyLen; }
    /// @brief Get the length of the key data stored in the key slot.
    /// @return nKeyDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01801
    /// @trace_id_dd=DD_CRYPTO_04419
    /// @needwork = ad
    /// @endcode
    uint16_t GetKeyDataLen() const noexcept { return nKeyDataLen_; }
    /// @brief Set the length of the key data stored in the key slot.
    /// @param nKeyDataLen Length of the key data stored in the key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01802
    /// @trace_id_dd=DD_CRYPTO_04420
    /// @needwork = ad
    /// @endcode
    void SetKeyDataLen(uint16_t const nKeyDataLen) noexcept { nKeyDataLen_ = nKeyDataLen; }
};
/// @brief IPC communication reply packet: Save key slot data from memory
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01803
/// @trace_id_dd=DD_CRYPTO_04421
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_SaveCopy_Mem final : public PIpcAsw_LogicHead
{
private:
    /// @brief IPC key slot number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04422
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID_{0};

public:
    /// @brief Get the IPC key slot number.
    /// @return nIpcSlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01804
    /// @trace_id_dd=DD_CRYPTO_04423
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcSlotID() const noexcept { return nIpcSlotID_; }
    /// @brief Set the IPC key slot number.
    /// @param nIpcSlotID IPC key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01805
    /// @trace_id_dd=DD_CRYPTO_04424
    /// @needwork = ad
    /// @endcode
    void SetIpcSlotID(uint32_t const nIpcSlotID) noexcept { nIpcSlotID_ = nIpcSlotID; }
};
//***************/
/// @brief IPC communication request packet: MyProvider
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01806
/// @trace_id_dd=DD_CRYPTO_04425
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_MyProvider final
{
public:
    /// @brief IPC key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04426
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotNameLen{0};
    /// @brief Get the key slot name data.
    /// @name GetData
    /// @return uint8_t const *
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01807
    /// @trace_id_dd=DD_CRYPTO_04427
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetData() const noexcept { return T_TransBytes(this) + sizeof(PIpcReq_MyProvider); }
};
/// @brief IPC communication reply packet: MyProvider
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01808
/// @trace_id_dd=DD_CRYPTO_04428
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_MyProvider final : public PIpcAsw_LogicHead
{
private:
    /// @brief Data length
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04429
    /// @needwork = dda
    /// @endcode
    uint32_t nDataLen_{0U};

public:
    /// @brief Get the data.
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01809
    /// @trace_id_dd=DD_CRYPTO_04430
    /// @needwork = ad
    /// @endcode
    uint8_t *GetData() noexcept { return T_TransBytes(this) + sizeof(PIpcAsw_MyProvider); }
    /// @brief Get the data length.
    /// @return nDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01810
    /// @trace_id_dd=DD_CRYPTO_04431
    /// @needwork = ad
    /// @endcode
    uint32_t GetDataLen() const noexcept { return nDataLen_; }
    /// @brief Set the data length.
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01811
    /// @trace_id_dd=DD_CRYPTO_04432
    /// @needwork = ad
    /// @endcode
    void SetDataLen(uint32_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
};
//***************/
}  // namespace keyslot

namespace iointerface {
/// @brief Get the class name in IPC communication data: PIoInterface_Ipc
/// @return PIoInterface_Ipc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01812
/// @trace_id_dd=DD_CRYPTO_04433
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView const GetkName_PIoInterface_Ipc() noexcept
{
    return ara::core::StringView{"PIoInterface_Ipc"};
}
//***************/
/// @brief IPC communication request packet: InterfaceCommon
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01813
/// @trace_id_dd=DD_CRYPTO_04434
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_InterfaceCommon final
{
public:
    /// @brief IPC key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04435
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID{0};
    /// @brief IO interface number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04436
    /// @needwork = dda
    /// @endcode
    uint32_t nIoInterfaceID{0};
};
/// @brief IPC communication reply packet: InterfaceCommon
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01814
/// @trace_id_dd=DD_CRYPTO_04437
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_InterfaceCommon final : public PIpcAsw_LogicHead
{
private:
    /// @brief Subsequent data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04438
    /// @needwork = dda
    /// @endcode
    uint16_t nDataLen_{0};

public:
    /// @brief Get the data.
    /// @return Starting address of data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01815
    /// @trace_id_dd=DD_CRYPTO_04439
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetData() const noexcept { return T_TransBytes(this) + sizeof(PIpcAsw_InterfaceCommon); }
    /// @brief Get the data length.
    /// @return nDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01816
    /// @trace_id_dd=DD_CRYPTO_04440
    /// @needwork = ad
    /// @endcode
    uint16_t GetDataLen() const noexcept { return nDataLen_; }
    /// @brief Set the data length.
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01817
    /// @trace_id_dd=DD_CRYPTO_04441
    /// @needwork = ad
    /// @endcode
    void SetDataLen(uint16_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
};
//***************/
}  // namespace iointerface
//***************/

/// @brief Namespace: encryption context
namespace cryptoctx {
/// @brief Get the class name in IPC communication data: PSymmetricBlockStream_Ipc
/// @return PSymmetricBlockStream_Ipc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01818
/// @trace_id_dd=DD_CRYPTO_04442
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView const GetName_PSymmetricBlockStream_Ipc() noexcept
{
    return ara::core::StringView{"PSymmetricBlockStream_Ipc"};
}
/// @brief Get the class name in IPC communication data: PMac_Ipc
/// @return PMac_Ipc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01819
/// @trace_id_dd=DD_CRYPTO_04443
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView const GetName_PMac_Ipc() noexcept { return ara::core::StringView{"PMac_Ipc"}; }
/// @brief Get the class name in IPC communication data: PWrap_Ipc
/// @return PWrap_Ipc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01820
/// @trace_id_dd=DD_CRYPTO_04444
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView const GetName_PWrap_Ipc() noexcept { return ara::core::StringView{"PWrap_Ipc"}; }
/// @brief Get the class name in IPC communication data: PAsymmetric_Ipc
/// @return PAsymmetric_Ipc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01821
/// @trace_id_dd=DD_CRYPTO_04445
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView const GetName_PAsymmetric_Ipc() noexcept
{
    return ara::core::StringView{"PAsymmetric_Ipc"};
}
/// @brief Get the class name in IPC communication data: PRng_Ipc
/// @return PRng_Ipc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01822
/// @trace_id_dd=DD_CRYPTO_04446
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView const GetName_PRng_Ipc() noexcept { return ara::core::StringView{"PRng_Ipc"}; }
/// @brief Get the class name in IPC communication data: PSeed_Ipc
/// @return PSeed_Ipc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01823
/// @trace_id_dd=DD_CRYPTO_04447
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView const GetName_PSeed_Ipc() noexcept { return ara::core::StringView{"PSeed_Ipc"}; }
/// @brief Get the class name in IPC communication data: PPrivateKey_Ipc
/// @return PPrivateKey_Ipc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01824
/// @trace_id_dd=DD_CRYPTO_04448
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView const GetName_PPrivateKey_Ipc() noexcept
{
    return ara::core::StringView{"PPrivateKey_Ipc"};
}
/// @brief Get the class name in IPC communication data: PPublicKey_Ipc
/// @return PPublicKey_Ipc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01825
/// @trace_id_dd=DD_CRYPTO_04449
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView const GetName_PPublicKey_Ipc() noexcept { return ara::core::StringView{"PPublicKey_Ipc"}; }
//***************/
/// @brief IPC communication request packet: Setkey
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01826
/// @trace_id_dd=DD_CRYPTO_04450
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_Setkey final
{
public:
    /// @brief IPC key slot number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04451
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID{0};
};
/// @brief IPC communication reply packet: Setkey
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03179
/// @trace_id_dd=DD_CRYPTO_06382
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcAsw_Setkey = PIpcAsw_LogicHead;
//***************/
/// @brief IPC communication request packet: docipher
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01827
/// @trace_id_dd=DD_CRYPTO_04452
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_docipher
{
public:
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01828
    /// @trace_id_dd=DD_CRYPTO_04453
    /// @needwork = ad
    /// @endcode
    virtual ~PIpcReq_docipher() = default;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01829
    /// @trace_id_dd=DD_CRYPTO_04454
    /// @needwork = ad
    /// @endcode
    PIpcReq_docipher() = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01830
    /// @trace_id_dd=DD_CRYPTO_04455
    /// @needwork = ad
    /// @endcode
    PIpcReq_docipher(PIpcReq_docipher const &other) = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01831
    /// @trace_id_dd=DD_CRYPTO_04456
    /// @needwork = ad
    /// @endcode
    PIpcReq_docipher(PIpcReq_docipher &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01832
    /// @trace_id_dd=DD_CRYPTO_04457
    /// @needwork = ad
    /// @endcode
    PIpcReq_docipher &operator=(PIpcReq_docipher const &other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01833
    /// @trace_id_dd=DD_CRYPTO_04458
    /// @needwork = ad
    /// @endcode
    PIpcReq_docipher &operator=(PIpcReq_docipher &&other) = delete;

private:
    /// @brief IPC key slot number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04459
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID_{0};
    /// @brief Encryption direction: encrypt or decrypt
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04460
    /// @needwork = dda
    /// @endcode
    uint32_t nTransform_{0};
    /// @brief Block length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04461
    /// @needwork = dda
    /// @endcode
    uint32_t nBlocksize_{0};
    /// @brief Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04462
    /// @needwork = dda
    /// @endcode
    uint32_t nDataLen_{0};
    /// @brief Initialization vector length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04463
    /// @needwork = dda
    /// @endcode
    uint32_t nIvLen_{0};
    /// @brief Seek offset
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04464
    /// @needwork = dda
    /// @endcode
    int64_t nOffset_{0};
    /// @brief Current offset position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04465
    /// @needwork = dda
    /// @endcode
    uint32_t nCurrentPos_{0U};
    /// @brief Length of data already encrypted
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04466
    /// @needwork = dda
    /// @endcode
    uint32_t nFinishBytes_{0U};
    /// @brief Whether to offset from the current position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04467
    /// @needwork = dda
    /// @endcode
    bool nFromBegin_{true};
    /// @brief Whether the data is block-aligned
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04468
    /// @needwork = dda
    /// @endcode
    bool nAlgedData_{false};

public:
    /// @brief Get the packet body data.
    /// @return Starting address of packet body data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01834
    /// @trace_id_dd=DD_CRYPTO_04469
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetData() const noexcept { return T_TransBytes(this) + sizeof(PIpcReq_docipher); }
    /// @brief Get the IV data.
    /// @return Starting address of IV data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01835
    /// @trace_id_dd=DD_CRYPTO_04470
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetIvData() const noexcept { return T_TransBytes(this) + sizeof(PIpcReq_docipher) + nDataLen_; }
    /// @brief Get the IPC key slot number.
    /// @return nIpcSlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01836
    /// @trace_id_dd=DD_CRYPTO_04471
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcSlotID() const noexcept { return nIpcSlotID_; }
    /// @brief Set the IPC key slot number.
    /// @param nIpcSlotID IPC key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01837
    /// @trace_id_dd=DD_CRYPTO_04472
    /// @needwork = ad
    /// @endcode
    void SetIpcSlotID(uint32_t const nIpcSlotID) noexcept { nIpcSlotID_ = nIpcSlotID; }
    /// @brief Get the encryption direction: encrypt or decrypt.
    /// @return nTransform_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01838
    /// @trace_id_dd=DD_CRYPTO_04473
    /// @needwork = ad
    /// @endcode
    uint32_t GetTransform() const noexcept { return nTransform_; }
    /// @brief Set the encryption direction: encrypt or decrypt.
    /// @param nTransform Encryption direction: encrypt or decrypt
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01839
    /// @trace_id_dd=DD_CRYPTO_04474
    /// @needwork = ad
    /// @endcode
    void SetTransform(uint32_t const nTransform) noexcept { nTransform_ = nTransform; }
    /// @brief Get the block length.
    /// @return nBlocksize_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01840
    /// @trace_id_dd=DD_CRYPTO_04475
    /// @needwork = ad
    /// @endcode
    uint32_t GetBlocksize() const noexcept { return nBlocksize_; }
    /// @brief Set the block length.
    /// @param nBlocksize Block length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01841
    /// @trace_id_dd=DD_CRYPTO_04476
    /// @needwork = ad
    /// @endcode
    void SetBlocksize(uint32_t const nBlocksize) noexcept { nBlocksize_ = nBlocksize; }
    /// @brief Get the data length.
    /// @return nDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01842
    /// @trace_id_dd=DD_CRYPTO_04477
    /// @needwork = ad
    /// @endcode
    uint32_t GetDataLen() const noexcept { return nDataLen_; }
    /// @brief Set the data length.
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01843
    /// @trace_id_dd=DD_CRYPTO_04478
    /// @needwork = ad
    /// @endcode
    void SetDataLen(uint32_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
    /// @brief Get the initialization vector length.
    /// @return nIvLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01844
    /// @trace_id_dd=DD_CRYPTO_04479
    /// @needwork = ad
    /// @endcode
    uint32_t GetIvLen() const noexcept { return nIvLen_; }
    /// @brief Set the initialization vector length.
    /// @param nIvLen Initialization vector length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01845
    /// @trace_id_dd=DD_CRYPTO_04480
    /// @needwork = ad
    /// @endcode
    void SetIvLen(uint32_t const nIvLen) noexcept { nIvLen_ = nIvLen; }
    /// @brief Get the seek offset.
    /// @return nOffset_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01846
    /// @trace_id_dd=DD_CRYPTO_04481
    /// @needwork = ad
    /// @endcode
    int64_t GetOffset() const noexcept { return nOffset_; }
    /// @brief Set the seek offset.
    /// @param nOffset Offset
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01847
    /// @trace_id_dd=DD_CRYPTO_04482
    /// @needwork = ad
    /// @endcode
    void SetOffset(int64_t const nOffset) noexcept { nOffset_ = nOffset; }
    /// @brief Get the current offset position.
    /// @return nCurrentPos_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01848
    /// @trace_id_dd=DD_CRYPTO_04483
    /// @needwork = ad
    /// @endcode
    uint32_t GetCurrentPos() const noexcept { return nCurrentPos_; }
    /// @brief Set the current offset position.
    /// @param nCurrentPos Current offset position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01849
    /// @trace_id_dd=DD_CRYPTO_04484
    /// @needwork = ad
    /// @endcode
    void SetCurrentPos(uint32_t const nCurrentPos) noexcept { nCurrentPos_ = nCurrentPos; }
    /// @brief Get the length of data already encrypted.
    /// @return nFinishBytes_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01850
    /// @trace_id_dd=DD_CRYPTO_04485
    /// @needwork = ad
    /// @endcode
    uint32_t GetFinishBytes() const noexcept { return nFinishBytes_; }
    /// @brief Set the length of data already encrypted.
    /// @param nFinishBytes Length of data already encrypted
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01851
    /// @trace_id_dd=DD_CRYPTO_04486
    /// @needwork = ad
    /// @endcode
    void SetFinishBytes(uint32_t const nFinishBytes) noexcept { nFinishBytes_ = nFinishBytes; }
    /// @brief Get whether to offset from the current position.
    /// @return nFromBegin_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01852
    /// @trace_id_dd=DD_CRYPTO_04487
    /// @needwork = ad
    /// @endcode
    bool GetFromBegin() const noexcept { return nFromBegin_; }
    /// @brief Set whether to offset from the current position.
    /// @param nFromBegin Whether to offset from the current position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01853
    /// @trace_id_dd=DD_CRYPTO_04488
    /// @needwork = ad
    /// @endcode
    void SetFromBegin(bool const nFromBegin) noexcept { nFromBegin_ = nFromBegin; }
    /// @brief Get whether the data is block-aligned.
    /// @return nAlgedData_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01854
    /// @trace_id_dd=DD_CRYPTO_04489
    /// @needwork = ad
    /// @endcode
    bool GetAlgedData() const noexcept { return nAlgedData_; }
    /// @brief Set whether the data is block-aligned.
    /// @param nAlgedData Whether the data is block-aligned
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01855
    /// @trace_id_dd=DD_CRYPTO_04490
    /// @needwork = ad
    /// @endcode
    void SetAlgedData(bool const nAlgedData) noexcept { nAlgedData_ = nAlgedData; }
};

/// @brief IPC communication reply packet: Transform
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01856
/// @trace_id_dd=DD_CRYPTO_04491
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_docipher final : public PIpcAsw_LogicHead
{
private:
    /// @brief Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04492
    /// @needwork = dda
    /// @endcode
    uint32_t nDataLen_{0};
    /// @brief Current position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04493
    /// @needwork = dda
    /// @endcode
    uint32_t nCurrentPos_{0U};
    /// @brief End bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04494
    /// @needwork = dda
    /// @endcode
    uint32_t nFinishBytes_{0U};

public:
    /// @brief Get the data area.
    /// @return Starting address of data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01857
    /// @trace_id_dd=DD_CRYPTO_04495
    /// @needwork = ad
    /// @endcode
    uint8_t *GetData() noexcept { return T_TransBytes(this) + sizeof(PIpcAsw_docipher); }
    /// @brief Get the data area.
    /// @return Starting address of data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetData() const noexcept { return T_TransBytes(this) + sizeof(PIpcAsw_docipher); }
    /// @brief Get the data length.
    /// @return nDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01858
    /// @trace_id_dd=DD_CRYPTO_04496
    /// @needwork = ad
    /// @endcode
    uint32_t GetDataLen() const noexcept { return nDataLen_; }
    /// @brief Set the data length.
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01859
    /// @trace_id_dd=DD_CRYPTO_04497
    /// @needwork = ad
    /// @endcode
    void SetDataLen(uint32_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
    /// @brief Get the end bytes.
    /// @return nFinishBytes_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01860
    /// @trace_id_dd=DD_CRYPTO_04498
    /// @needwork = ad
    /// @endcode
    uint32_t GetFinishBytes() const noexcept { return nFinishBytes_; }
    /// @brief Set the end bytes.
    /// @param nFinishBytes End bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01861
    /// @trace_id_dd=DD_CRYPTO_04499
    /// @needwork = ad
    /// @endcode
    void SetFinishBytes(uint32_t const nFinishBytes) noexcept { nFinishBytes_ = nFinishBytes; }
    /// @brief Get the current position.
    /// @return nCurrentPos_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01862
    /// @trace_id_dd=DD_CRYPTO_04500
    /// @needwork = ad
    /// @endcode
    uint32_t GetCurrentPos() const noexcept { return nCurrentPos_; }
    /// @brief Set the current position.
    /// @param nCurrentPos Current position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01863
    /// @trace_id_dd=DD_CRYPTO_04501
    /// @needwork = ad
    /// @endcode
    void SetCurrentPos(uint32_t const nCurrentPos) noexcept { nCurrentPos_ = nCurrentPos; }
};

/// @brief IPC communication reply packet: common
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01864
/// @trace_id_dd=DD_CRYPTO_04502
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_common final : public PIpcAsw_LogicHead
{
private:
    /// @brief Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04503
    /// @needwork = dda
    /// @endcode
    uint32_t nDataLen_{0};

public:
    /// @brief Return the data area.
    /// @return Starting address of data area
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01865
    /// @trace_id_dd=DD_CRYPTO_04504
    /// @needwork = ad
    /// @endcode
    uint8_t *GetData() noexcept { return T_TransBytes(this) + sizeof(PIpcAsw_common); }
    /// @brief Get the data length.
    /// @return nDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01866
    /// @trace_id_dd=DD_CRYPTO_04505
    /// @needwork = ad
    /// @endcode
    uint32_t GetDataLen() const noexcept { return nDataLen_; }
    /// @brief Set the data length.
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01867
    /// @trace_id_dd=DD_CRYPTO_04506
    /// @needwork = ad
    /// @endcode
    void SetDataLen(uint32_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
};

/// @brief IPC communication request packet: Wrap
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01868
/// @trace_id_dd=DD_CRYPTO_04507
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_Wrap
{
public:
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01869
    /// @trace_id_dd=DD_CRYPTO_04508
    /// @needwork = ad
    /// @endcode
    virtual ~PIpcReq_Wrap() = default;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01870
    /// @trace_id_dd=DD_CRYPTO_04509
    /// @needwork = ad
    /// @endcode
    PIpcReq_Wrap() = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01871
    /// @trace_id_dd=DD_CRYPTO_04510
    /// @needwork = ad
    /// @endcode
    PIpcReq_Wrap(PIpcReq_Wrap const &other) = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01872
    /// @trace_id_dd=DD_CRYPTO_04511
    /// @needwork = ad
    /// @endcode
    PIpcReq_Wrap(PIpcReq_Wrap &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01873
    /// @trace_id_dd=DD_CRYPTO_04512
    /// @needwork = ad
    /// @endcode
    PIpcReq_Wrap &operator=(PIpcReq_Wrap const &other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01874
    /// @trace_id_dd=DD_CRYPTO_04513
    /// @needwork = ad
    /// @endcode
    PIpcReq_Wrap &operator=(PIpcReq_Wrap &&other) = delete;

private:
    /// @brief IPC key slot number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04514
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID_{0};
    /// @brief Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04515
    /// @needwork = dda
    /// @endcode
    uint32_t nKeySlotId_{0};
    /// @brief Encryption direction: encrypt or decrypt
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04516
    /// @needwork = dda
    /// @endcode
    uint32_t nTransform_{0};
    /// @brief Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04517
    /// @needwork = dda
    /// @endcode
    uint32_t nDataLen_{0};
    /// @brief Block length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04518
    /// @needwork = dda
    /// @endcode
    uint32_t nBlocksize_{0};

public:
    /// @brief Return the data area.
    /// @return Starting address of data area
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01875
    /// @trace_id_dd=DD_CRYPTO_04519
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetData() const noexcept { return T_TransBytes(this) + sizeof(PIpcReq_Wrap); }
    /// @brief Get the IPC key slot number.
    /// @return nIpcSlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01876
    /// @trace_id_dd=DD_CRYPTO_04520
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcSlotID() const noexcept { return nIpcSlotID_; }
    /// @brief Set the IPC key slot number.
    /// @param nIpcSlotID IPC key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01877
    /// @trace_id_dd=DD_CRYPTO_04521
    /// @needwork = ad
    /// @endcode
    void SetIpcSlotID(uint32_t const nIpcSlotID) noexcept { nIpcSlotID_ = nIpcSlotID; }
    /// @brief Get the key slot ID.
    /// @return nKeySlotId_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01878
    /// @trace_id_dd=DD_CRYPTO_04522
    /// @needwork = ad
    /// @endcode
    uint32_t GetKeySlotId() const noexcept { return nKeySlotId_; }
    /// @brief Set the key slot ID.
    /// @param nKeySlotId Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01879
    /// @trace_id_dd=DD_CRYPTO_04523
    /// @needwork = ad
    /// @endcode
    void SetKeySlotId(uint32_t const nKeySlotId) noexcept { nKeySlotId_ = nKeySlotId; }
    /// @brief Get the encryption direction: encrypt or decrypt.
    /// @return nTransform_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01880
    /// @trace_id_dd=DD_CRYPTO_04524
    /// @needwork = ad
    /// @endcode
    uint32_t GetTransform() const noexcept { return nTransform_; }
    /// @brief Set the encryption direction: encrypt or decrypt.
    /// @param nTransform Encryption direction: encrypt or decrypt
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01881
    /// @trace_id_dd=DD_CRYPTO_04525
    /// @needwork = ad
    /// @endcode
    void SetTransform(uint32_t const nTransform) noexcept { nTransform_ = nTransform; }
    /// @brief Get the data length.
    /// @return nDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01882
    /// @trace_id_dd=DD_CRYPTO_04526
    /// @needwork = ad
    /// @endcode
    uint32_t GetDataLen() const noexcept { return nDataLen_; }
    /// @brief Set the data length.
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01883
    /// @trace_id_dd=DD_CRYPTO_04527
    /// @needwork = ad
    /// @endcode
    void SetDataLen(uint32_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
    /// @brief Get the block length.
    /// @return nBlocksize_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01884
    /// @trace_id_dd=DD_CRYPTO_04528
    /// @needwork = ad
    /// @endcode
    uint32_t GetBlocksize() const noexcept { return nBlocksize_; }
    /// @brief Set the block length.
    /// @param nBlocksize Block length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01885
    /// @trace_id_dd=DD_CRYPTO_04529
    /// @needwork = ad
    /// @endcode
    void SetBlocksize(uint32_t const nBlocksize) noexcept { nBlocksize_ = nBlocksize; }
};

/// @brief IPC communication request packet: RsaLogic
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01886
/// @trace_id_dd=DD_CRYPTO_04530
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_RsaLogic
{
public:
    /// @brief default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01887
    /// @trace_id_dd=DD_CRYPTO_04531
    /// @needwork = ad
    /// @endcode
    virtual ~PIpcReq_RsaLogic() = default;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01888
    /// @trace_id_dd=DD_CRYPTO_04532
    /// @needwork = ad
    /// @endcode
    PIpcReq_RsaLogic() = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01889
    /// @trace_id_dd=DD_CRYPTO_04533
    /// @needwork = ad
    /// @endcode
    PIpcReq_RsaLogic(PIpcReq_RsaLogic const &other) = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01890
    /// @trace_id_dd=DD_CRYPTO_04534
    /// @needwork = ad
    /// @endcode
    PIpcReq_RsaLogic(PIpcReq_RsaLogic &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01891
    /// @trace_id_dd=DD_CRYPTO_04535
    /// @needwork = ad
    /// @endcode
    PIpcReq_RsaLogic &operator=(PIpcReq_RsaLogic const &other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01892
    /// @trace_id_dd=DD_CRYPTO_04536
    /// @needwork = ad
    /// @endcode
    PIpcReq_RsaLogic &operator=(PIpcReq_RsaLogic &&other) = delete;

private:
    /// @brief IPC key slot number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04537
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID_{0};
    /// @brief Padding flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04538
    /// @needwork = dda
    /// @endcode
    uint32_t nPadding_{0};
    /// @brief Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04539
    /// @needwork = dda
    /// @endcode
    uint32_t nDataLen_{0};
    /// @brief hashID required for PSS
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04540
    /// @needwork = dda
    /// @endcode
    uint32_t nHashId_{0};
    /// @brief Salt length value required for PSS padding, default maximum
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04541
    /// @needwork = dda
    /// @endcode
    uint32_t nSaltLen_{0};
    /// @brief Length of signature data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04542
    /// @needwork = dda
    /// @endcode
    uint32_t nDigestDataLen_{0};

public:
    /// @brief Get the data area.
    /// @return Starting address of data area
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01893
    /// @trace_id_dd=DD_CRYPTO_04543
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetData() const noexcept { return T_TransBytes(this) + sizeof(PIpcReq_RsaLogic); }
    /// @brief Get the IPC key slot number.
    /// @return nIpcSlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01894
    /// @trace_id_dd=DD_CRYPTO_04544
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcSlotID() const noexcept { return nIpcSlotID_; }
    /// @brief Set the IPC key slot number.
    /// @param nIpcSlotID IPC key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01895
    /// @trace_id_dd=DD_CRYPTO_04545
    /// @needwork = ad
    /// @endcode
    void SetIpcSlotID(uint32_t const nIpcSlotID) noexcept { nIpcSlotID_ = nIpcSlotID; }
    /// @brief Get the padding flag.
    /// @return nPadding_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01896
    /// @trace_id_dd=DD_CRYPTO_04546
    /// @needwork = ad
    /// @endcode
    uint32_t GetPadding() const noexcept { return nPadding_; }
    /// @brief Set the padding flag.
    /// @param nPadding Data padding scheme
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01897
    /// @trace_id_dd=DD_CRYPTO_04547
    /// @needwork = ad
    /// @endcode
    void SetPadding(uint32_t const nPadding) noexcept { nPadding_ = nPadding; }
    /// @brief Get the data length.
    /// @return nDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01898
    /// @trace_id_dd=DD_CRYPTO_04548
    /// @needwork = ad
    /// @endcode
    uint32_t GetDataLen() const noexcept { return nDataLen_; }
    /// @brief Set the data length.
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01899
    /// @trace_id_dd=DD_CRYPTO_04549
    /// @needwork = ad
    /// @endcode
    void SetDataLen(uint32_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
    /// @brief Get the hashID required for PSS.
    /// @return nHashId_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01900
    /// @trace_id_dd=DD_CRYPTO_04550
    /// @needwork = ad
    /// @endcode
    uint32_t GetHashId() const noexcept { return nHashId_; }
    /// @brief Set the hashID required for PSS.
    /// @param nHashId Hash algorithm crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01901
    /// @trace_id_dd=DD_CRYPTO_04551
    /// @needwork = ad
    /// @endcode
    void SetHashId(uint32_t const nHashId) noexcept { nHashId_ = nHashId; }
    /// @brief Get the salt length value required for PSS padding.
    /// @return nSaltLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01902
    /// @trace_id_dd=DD_CRYPTO_04552
    /// @needwork = ad
    /// @endcode
    uint32_t GetSaltLen() const noexcept { return nSaltLen_; }
    /// @brief Set the salt length value required for PSS padding.
    /// @param nSaltLen Salt length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01903
    /// @trace_id_dd=DD_CRYPTO_04553
    /// @needwork = ad
    /// @endcode
    void SetSaltLen(uint32_t const nSaltLen) noexcept { nSaltLen_ = nSaltLen; }
    /// @brief Get the length of the signature data.
    /// @return nDigestDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01904
    /// @trace_id_dd=DD_CRYPTO_04554
    /// @needwork = ad
    /// @endcode
    uint32_t GetDigestDataLen() const noexcept { return nDigestDataLen_; }
    /// @brief Set the length of the signature data.
    /// @param nDigestDataLen Digest data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01905
    /// @trace_id_dd=DD_CRYPTO_04555
    /// @needwork = ad
    /// @endcode
    void SetDigestDataLen(uint32_t const nDigestDataLen) noexcept { nDigestDataLen_ = nDigestDataLen; }
};

/// @brief IPC communication request packet: Common
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01906
/// @trace_id_dd=DD_CRYPTO_04556
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_Common
{
public:
    /// @brief default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01907
    /// @trace_id_dd=DD_CRYPTO_04557
    /// @needwork = ad
    /// @endcode
    virtual ~PIpcReq_Common() = default;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01908
    /// @trace_id_dd=DD_CRYPTO_04558
    /// @needwork = ad
    /// @endcode
    PIpcReq_Common() = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01909
    /// @trace_id_dd=DD_CRYPTO_04559
    /// @needwork = ad
    /// @endcode
    PIpcReq_Common(PIpcReq_Common const &other) = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01910
    /// @trace_id_dd=DD_CRYPTO_04560
    /// @needwork = ad
    /// @endcode
    PIpcReq_Common(PIpcReq_Common &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01911
    /// @trace_id_dd=DD_CRYPTO_04561
    /// @needwork = ad
    /// @endcode
    PIpcReq_Common &operator=(PIpcReq_Common const &other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01912
    /// @trace_id_dd=DD_CRYPTO_04562
    /// @needwork = ad
    /// @endcode
    PIpcReq_Common &operator=(PIpcReq_Common &&other) = delete;

private:
    /// @brief IPC key slot number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04563
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID_{0};
    /// @brief Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04564
    /// @needwork = dda
    /// @endcode
    uint32_t nDataLen_{0};

public:
    /// @brief Get the data area.
    /// @return Starting address of data area
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01913
    /// @trace_id_dd=DD_CRYPTO_04565
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetData() const noexcept { return T_TransBytes(this) + sizeof(PIpcReq_Common); }
    /// @brief Get the IPC key slot number.
    /// @return nIpcSlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01914
    /// @trace_id_dd=DD_CRYPTO_04566
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcSlotID() const noexcept { return nIpcSlotID_; }
    /// @brief Set the IPC key slot number.
    /// @param nIpcSlotID IPC key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01915
    /// @trace_id_dd=DD_CRYPTO_04567
    /// @needwork = ad
    /// @endcode
    void SetIpcSlotID(uint32_t const nIpcSlotID) noexcept { nIpcSlotID_ = nIpcSlotID; }
    /// @brief Get the data length.
    /// @return nDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01916
    /// @trace_id_dd=DD_CRYPTO_04568
    /// @needwork = ad
    /// @endcode
    uint32_t GetDataLen() const noexcept { return nDataLen_; }
    /// @brief Set the data length.
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01917
    /// @trace_id_dd=DD_CRYPTO_04569
    /// @needwork = ad
    /// @endcode
    void SetDataLen(uint32_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
};

/// @brief IPC communication request packet: Rng
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01918
/// @trace_id_dd=DD_CRYPTO_04570
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_Rng
{
public:
    /// @brief default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01919
    /// @trace_id_dd=DD_CRYPTO_04571
    /// @needwork = ad
    /// @endcode
    virtual ~PIpcReq_Rng() = default;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01920
    /// @trace_id_dd=DD_CRYPTO_04572
    /// @needwork = ad
    /// @endcode
    PIpcReq_Rng() = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01921
    /// @trace_id_dd=DD_CRYPTO_04573
    /// @needwork = ad
    /// @endcode
    PIpcReq_Rng(PIpcReq_Rng const &other) = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01922
    /// @trace_id_dd=DD_CRYPTO_04574
    /// @needwork = ad
    /// @endcode
    PIpcReq_Rng(PIpcReq_Rng &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01923
    /// @trace_id_dd=DD_CRYPTO_04575
    /// @needwork = ad
    /// @endcode
    PIpcReq_Rng &operator=(PIpcReq_Rng const &other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01924
    /// @trace_id_dd=DD_CRYPTO_04576
    /// @needwork = ad
    /// @endcode
    PIpcReq_Rng &operator=(PIpcReq_Rng &&other) = delete;

private:
    /// @brief IPC key slot number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04577
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID_{0};
    /// @brief Requested length of random number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04578
    /// @needwork = dda
    /// @endcode
    uint32_t nGenerateLen_{0};

public:
    /// @brief Get the data area.
    /// @return Starting address of data area
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01925
    /// @trace_id_dd=DD_CRYPTO_04579
    /// @needwork = ad
    /// @endcode
    uint8_t *GetData() noexcept { return T_TransBytes(this) + sizeof(PIpcReq_Rng); }
    /// @brief Get the IPC key slot number.
    /// @return nIpcSlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01926
    /// @trace_id_dd=DD_CRYPTO_04580
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcSlotID() const noexcept { return nIpcSlotID_; }
    /// @brief Set the IPC key slot number.
    /// @param nIpcSlotID IPC key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01927
    /// @trace_id_dd=DD_CRYPTO_04581
    /// @needwork = ad
    /// @endcode
    void SetIpcSlotID(uint32_t const nIpcSlotID) noexcept { nIpcSlotID_ = nIpcSlotID; }
    /// @brief Get the requested length of random number.
    /// @return nGenerateLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01928
    /// @trace_id_dd=DD_CRYPTO_04582
    /// @needwork = ad
    /// @endcode
    uint32_t GetGenerateLen() const noexcept { return nGenerateLen_; }
    /// @brief Set the requested length of random number.
    /// @param nGenerateLen Requested length of random number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01929
    /// @trace_id_dd=DD_CRYPTO_04583
    /// @needwork = ad
    /// @endcode
    void SetGenerateLen(uint32_t const nGenerateLen) noexcept { nGenerateLen_ = nGenerateLen; }
};

/// @brief IPC communication request packet: AgreeKey
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01930
/// @trace_id_dd=DD_CRYPTO_04584
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_AgreeKey
{
public:
    /// @brief default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01931
    /// @trace_id_dd=DD_CRYPTO_04585
    /// @needwork = ad
    /// @endcode
    virtual ~PIpcReq_AgreeKey() = default;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01932
    /// @trace_id_dd=DD_CRYPTO_04586
    /// @needwork = ad
    /// @endcode
    PIpcReq_AgreeKey() = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01933
    /// @trace_id_dd=DD_CRYPTO_04587
    /// @needwork = ad
    /// @endcode
    PIpcReq_AgreeKey(PIpcReq_AgreeKey const &other) = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01934
    /// @trace_id_dd=DD_CRYPTO_04588
    /// @needwork = ad
    /// @endcode
    PIpcReq_AgreeKey(PIpcReq_AgreeKey &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01935
    /// @trace_id_dd=DD_CRYPTO_04589
    /// @needwork = ad
    /// @endcode
    PIpcReq_AgreeKey &operator=(PIpcReq_AgreeKey const &other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01936
    /// @trace_id_dd=DD_CRYPTO_04590
    /// @needwork = ad
    /// @endcode
    PIpcReq_AgreeKey &operator=(PIpcReq_AgreeKey &&other) = delete;

private:
    /// @brief IPC key slot number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04591
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID_{0};
    /// @brief Public key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04592
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcPubSlotID_{0};
    /// @brief Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04593
    /// @needwork = dda
    /// @endcode
    uint32_t nDataLen_{0};

public:
    /// @brief Get the data area.
    /// @return Starting address of data area
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01937
    /// @trace_id_dd=DD_CRYPTO_04594
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetData() const noexcept { return T_TransBytes(this) + sizeof(PIpcReq_AgreeKey); }
    /// @brief Get the IPC key slot number.
    /// @return nIpcSlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01938
    /// @trace_id_dd=DD_CRYPTO_04595
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcSlotID() const noexcept { return nIpcSlotID_; }
    /// @brief Set the IPC key slot number.
    /// @param nIpcSlotID IPC key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01939
    /// @trace_id_dd=DD_CRYPTO_04596
    /// @needwork = ad
    /// @endcode
    void SetIpcSlotID(uint32_t const nIpcSlotID) noexcept { nIpcSlotID_ = nIpcSlotID; }
    /// @brief Get the public key slot ID.
    /// @return nIpcPubSlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01940
    /// @trace_id_dd=DD_CRYPTO_04597
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcPubSlotID() const noexcept { return nIpcPubSlotID_; }
    /// @brief Set the public key slot ID.
    /// @param nIpcPubSlotID Public key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01941
    /// @trace_id_dd=DD_CRYPTO_04598
    /// @needwork = ad
    /// @endcode
    void SetIpcPubSlotID(uint32_t const nIpcPubSlotID) noexcept { nIpcPubSlotID_ = nIpcPubSlotID; }
    /// @brief Get the data length.
    /// @return nDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01942
    /// @trace_id_dd=DD_CRYPTO_04599
    /// @needwork = ad
    /// @endcode
    uint32_t GetDataLen() const noexcept { return nDataLen_; }
    /// @brief Set the data length.
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01943
    /// @trace_id_dd=DD_CRYPTO_04600
    /// @needwork = ad
    /// @endcode
    void SetDataLen(uint32_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
};

/// @brief IPC communication reply packet: Rng
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01944
/// @trace_id_dd=DD_CRYPTO_04601
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_Rng final : public PIpcAsw_LogicHead
{
public:
    /// @brief Get the data area.
    /// @return Starting address of data area
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01945
    /// @trace_id_dd=DD_CRYPTO_04602
    /// @needwork = ad
    /// @endcode
    uint8_t *GetData() noexcept { return T_TransBytes(this) + sizeof(PIpcAsw_Rng); }
};
/// @brief IPC communication request packet: Secret seed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01946
/// @trace_id_dd=DD_CRYPTO_04603
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_Seed
{
public:
    /// @brief default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01947
    /// @trace_id_dd=DD_CRYPTO_04604
    /// @needwork = ad
    /// @endcode
    virtual ~PIpcReq_Seed() = default;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01948
    /// @trace_id_dd=DD_CRYPTO_04605
    /// @needwork = ad
    /// @endcode
    PIpcReq_Seed() = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01949
    /// @trace_id_dd=DD_CRYPTO_04606
    /// @needwork = ad
    /// @endcode
    PIpcReq_Seed(PIpcReq_Seed const &other) = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01950
    /// @trace_id_dd=DD_CRYPTO_04607
    /// @needwork = ad
    /// @endcode
    PIpcReq_Seed(PIpcReq_Seed &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01951
    /// @trace_id_dd=DD_CRYPTO_04608
    /// @needwork = ad
    /// @endcode
    PIpcReq_Seed &operator=(PIpcReq_Seed const &other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01952
    /// @trace_id_dd=DD_CRYPTO_04609
    /// @needwork = ad
    /// @endcode
    PIpcReq_Seed &operator=(PIpcReq_Seed &&other) = delete;

private:
    /// @brief IPC key slot number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04610
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID_{0};
    /// @brief Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04611
    /// @needwork = dda
    /// @endcode
    uint32_t nDataLen_{0};
    /// @brief Offset during Seek
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04612
    /// @needwork = dda
    /// @endcode
    int64_t nSteps_{0};

public:
    /// @brief Get the data area.
    /// @return Starting address of data area
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01953
    /// @trace_id_dd=DD_CRYPTO_04613
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetData() const noexcept { return T_TransBytes(this) + sizeof(PIpcReq_Common); }
    /// @brief Get the IPC key slot number.
    /// @return nIpcSlotID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01954
    /// @trace_id_dd=DD_CRYPTO_04614
    /// @needwork = ad
    /// @endcode
    uint32_t GetIpcSlotID() const noexcept { return nIpcSlotID_; }
    /// @brief Set the IPC key slot number.
    /// @param nIpcSlotID IPC key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01955
    /// @trace_id_dd=DD_CRYPTO_04615
    /// @needwork = ad
    /// @endcode
    void SetIpcSlotID(uint32_t const nIpcSlotID) noexcept { nIpcSlotID_ = nIpcSlotID; }
    /// @brief Get the data length.
    /// @return nDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01956
    /// @trace_id_dd=DD_CRYPTO_04616
    /// @needwork = ad
    /// @endcode
    uint32_t GetDataLen() const noexcept { return nDataLen_; }
    /// @brief Set the data length.
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01957
    /// @trace_id_dd=DD_CRYPTO_04617
    /// @needwork = ad
    /// @endcode
    void SetDataLen(uint32_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
    /// @brief Get the offset during Seek.
    /// @return nSteps_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01958
    /// @trace_id_dd=DD_CRYPTO_04618
    /// @needwork = ad
    /// @endcode
    int64_t GetSteps() const noexcept { return nSteps_; }
    /// @brief Set the offset during Seek.
    /// @param nSteps Offset during Seek
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01959
    /// @trace_id_dd=DD_CRYPTO_04619
    /// @needwork = ad
    /// @endcode
    void SetSteps(int64_t const nSteps) noexcept { nSteps_ = nSteps; }
};
/// @brief IPC communication reply packet: Seed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03180
/// @trace_id_dd=DD_CRYPTO_06383
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
using PIpcAsw_Seed = PIpcAsw_common;

/// @brief IPC communication reply packet: GetPublic
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01960
/// @trace_id_dd=DD_CRYPTO_04620
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_GetPublic final : public PIpcAsw_LogicHead
{
private:
    /// @brief Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04621
    /// @needwork = dda
    /// @endcode
    uint32_t nDataLen_{0};
    /// @brief Usage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04622
    /// @needwork = dda
    /// @endcode
    uint32_t nAllowedUsage_{0};

public:
    /// @brief Get the data area.
    /// @return Starting address of data area
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01961
    /// @trace_id_dd=DD_CRYPTO_04623
    /// @needwork = ad
    /// @endcode
    uint8_t *GetData() noexcept { return T_TransBytes(this) + sizeof(PIpcAsw_GetPublic); }
    /// @brief Get the data length.
    /// @return nDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01962
    /// @trace_id_dd=DD_CRYPTO_04624
    /// @needwork = ad
    /// @endcode
    uint32_t GetDataLen() const noexcept { return nDataLen_; }
    /// @brief Set the data length.
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01963
    /// @trace_id_dd=DD_CRYPTO_04625
    /// @needwork = ad
    /// @endcode
    void SetDataLen(uint32_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
    /// @brief Get the usage.
    /// @return nAllowedUsage_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01964
    /// @trace_id_dd=DD_CRYPTO_04626
    /// @needwork = ad
    /// @endcode
    uint32_t GetAllowedUsage() const noexcept { return nAllowedUsage_; }
    /// @brief Set the usage.
    /// @param nAllowedUsage Usage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01965
    /// @trace_id_dd=DD_CRYPTO_04627
    /// @needwork = ad
    /// @endcode
    void SetAllowedUsage(uint32_t const nAllowedUsage) noexcept { nAllowedUsage_ = nAllowedUsage; }
};

/// @brief IPC communication reply packet: DecapsulateKey
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01966
/// @trace_id_dd=DD_CRYPTO_04628
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_DecapsulateKey final : public PIpcAsw_LogicHead
{
private:
    /// @brief RSA length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04629
    /// @needwork = dda
    /// @endcode
    uint32_t nRsaLen_{0};
    /// @brief Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04630
    /// @needwork = dda
    /// @endcode
    uint32_t nDataLen_{0};

public:
    /// @brief Get the data area.
    /// @return Starting address of data area
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01967
    /// @trace_id_dd=DD_CRYPTO_04631
    /// @needwork = ad
    /// @endcode
    uint8_t *GetData() noexcept { return T_TransBytes(this) + sizeof(PIpcAsw_DecapsulateKey); }
    /// @brief Get the data length.
    /// @return nDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01968
    /// @trace_id_dd=DD_CRYPTO_04632
    /// @needwork = ad
    /// @endcode
    uint32_t GetDataLen() const noexcept { return nDataLen_; }
    /// @brief Set the data length.
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01969
    /// @trace_id_dd=DD_CRYPTO_04633
    /// @needwork = ad
    /// @endcode
    void SetDataLen(uint32_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
    /// @brief Get the RSA length.
    /// @return nRsaLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01970
    /// @trace_id_dd=DD_CRYPTO_04634
    /// @needwork = ad
    /// @endcode
    uint32_t GetRsaLen() const noexcept { return nRsaLen_; }
    /// @brief Set the RSA length.
    /// @param nRsaLen RSA length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01971
    /// @trace_id_dd=DD_CRYPTO_04635
    /// @needwork = ad
    /// @endcode
    void SetRsaLen(uint32_t const nRsaLen) noexcept { nRsaLen_ = nRsaLen; }
};
//***************/
/// @brief IPC communication request packet: DoMac
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01972
/// @trace_id_dd=DD_CRYPTO_04636
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcReq_DoMac final
{
public:
    /// @brief m // KeySlot identifier ID: except for the first packet which uses KeyName, other cases use SlotID to identify the key slot.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04637
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID{0};
    /// @brief Encryption direction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04638
    /// @needwork = dda
    /// @endcode
    uint32_t nTransform{0};
    /// @brief MAC context algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04639
    /// @needwork = dda
    /// @endcode
    uint32_t nAlgID{0};
    /// @brief Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04640
    /// @needwork = dda
    /// @endcode
    uint32_t nDataLen{0};

public:
    /// @brief Get the data area.
    /// @return Starting address of data area
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01973
    /// @trace_id_dd=DD_CRYPTO_04641
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetData() const noexcept { return T_TransBytes(this) + sizeof(PIpcReq_DoMac); }
};
/// @brief IPC communication reply packet: DoMac
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01974
/// @trace_id_dd=DD_CRYPTO_04642
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PIpcAsw_DoMac final : public PIpcAsw_LogicHead
{
private:
    /// @brief Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04643
    /// @needwork = dda
    /// @endcode
    uint32_t nDataLen_{0};

public:
    /// @brief Get the data area.
    /// @return Starting address of data area
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01975
    /// @trace_id_dd=DD_CRYPTO_04644
    /// @needwork = ad
    /// @endcode
    uint8_t *GetData() noexcept { return T_TransBytes(this) + sizeof(PIpcAsw_DoMac); }
    /// @brief Get the data length.
    /// @return nDataLen_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01976
    /// @trace_id_dd=DD_CRYPTO_04645
    /// @needwork = ad
    /// @endcode
    uint32_t GetDataLen() const noexcept { return nDataLen_; }
    /// @brief Set the data length.
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01977
    /// @trace_id_dd=DD_CRYPTO_04646
    /// @needwork = ad
    /// @endcode
    void SetDataLen(uint32_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
};

}  // namespace cryptoctx
//********************************/
#pragma pack(pop)
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IPC_PROTOCOL_H_
