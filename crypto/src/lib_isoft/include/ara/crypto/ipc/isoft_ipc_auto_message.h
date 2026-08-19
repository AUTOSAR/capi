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
/// @file       isoft_ipc_auto_message.h
/// @brief      AutoSar-Crypto Key Storage Module
/// @details    IPC message wrapper class with automatic memory allocation/release
/// @date       2022-08-17
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-08-17  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Function Modules
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Common_api
/// @unit_description=IPC message wrapper class with automatic memory allocation/release
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_IPC_AUTO_MESSAGE_H_
#define ARA_CRYPTO_KEYS_PUHUA_IPC_AUTO_MESSAGE_H_

#include <ara/core/string_view.h>
#include <isoft/ipccpp/buffer.h>
#include <isoft/ipccpp/packet.h>

#include <cstdint>
#include <functional>
#include <memory>

namespace ara {
namespace crypto {
namespace keys {
// PRQA S 2502 QAC /// @qac: False positive
/// @qac Suppress warning [2502]: This name hides a similar kind of declaration.
namespace isoft_def {
// PRQA L:QAC
//********************************/
/// @brief IPC message wrapper class with automatic memory allocation/release
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02000
/// @trace_id_dd=DD_CRYPTO_04694
/// @needwork = ad
/// @endcode
class PIpcAutoPacket final
{
private:
    /// @brief IPC data packet
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04695
    /// @needwork = dda
    /// @endcode
    isoft::ipc::IPCPacket* pIpcPacket_{nullptr};
    /// @brief Whether to auto-delete
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04696
    /// @needwork = dda
    /// @endcode
    bool bAutoDelPacket_{false};

public:
    /// @brief Parameterized constructor
    /// @param pIpcPacket IPC data packet
    /// @param nNewLen New IPC data packet length
    /// @param bAutoDelPacket Whether to auto-release
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04697
    /// @needwork = dda
    /// @endcode
    explicit PIpcAutoPacket(isoft::ipc::IPCPacket* const pIpcPacket,
                            uint16_t const nNewLen,
                            bool const bAutoDelPacket = false) noexcept;
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04698
    /// @needwork = dda
    /// @endcode
    PIpcAutoPacket() noexcept = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04699
    /// @needwork = dda
    /// @endcode
    PIpcAutoPacket(PIpcAutoPacket&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04700
    /// @needwork = dda
    /// @endcode
    PIpcAutoPacket& operator=(PIpcAutoPacket&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04701
    /// @needwork = dda
    /// @endcode
    PIpcAutoPacket& operator=(PIpcAutoPacket const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04702
    /// @needwork = dda
    /// @endcode
    PIpcAutoPacket(PIpcAutoPacket const& other) = delete;
    /// @brief Destructor
    /// @name     ~PIpcAutoPacket
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04703
    /// @needwork = dda
    /// @endcode
    ~PIpcAutoPacket() noexcept;
    /// @brief Returns the data in the first buffer of IPCPacket (may contain many, so this function is only suitable for small data packets)
    /// @name  data
    /// @returns Starting address of data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04704
    /// @needwork = dda
    /// @endcode
    uint8_t* data() const noexcept;  // NOLINT
    /// @brief Gets extra data length
    /// @return Extra data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04705
    /// @needwork = dda
    /// @endcode
    uint32_t GetExtSize() const noexcept;
    /// @brief Sets the length of the first node in the Buff chain
    /// @name  SetFirstSize
    /// @param nSize Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04706
    /// @needwork = dda
    /// @endcode
    void SetFirstSize(int32_t const nSize) const noexcept;
    /// @brief Gets total data length
    /// @return Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04707
    /// @needwork = dda
    /// @endcode
    uint32_t GetTotalSize() const noexcept;
    /// @brief Adds data to pIpcPacket_
    /// @param pVoidData Starting address of data
    /// @param nDataLen Data length
    /// @return Length of added data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04708
    /// @needwork = dda
    /// @endcode
    uint32_t AddDataToIpc(void const* const pVoidData, uint16_t const nDataLen) const noexcept;
    /// @brief Processes multi-packet data using a callback function
    /// @param pfunc Callback function
    /// @return Length of processed multi-packet data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04709
    /// @needwork = dda
    /// @endcode
    uint32_t DealBuffData(
        std::function< uint32_t(uint32_t nIndex, uint8_t const* pData, uint32_t nLen) > const& pfunc) const noexcept;

public:
    /// @brief Re-initialize
    /// @name ReInit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04710
    /// @needwork = dda
    /// @endcode
    void ReInit() noexcept;
    /// @brief Gets the payload content within the IPC data packet
    /// @name GetIpcBody
    /// @returns Starting address of packet content
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04711
    /// @needwork = dda
    /// @endcode
    uint8_t* GetIpcBody() const noexcept;
    /// @brief Gets the payload content within the IPC data packet
    /// @name GetIpcBody
    /// @returns Starting address of packet content
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04712
    /// @needwork = dda
    /// @endcode
    template < typename T_Value >
    T_Value* GetIpcBody() const noexcept
    {
        return static_cast< T_Value* >(static_cast< void* >(GetIpcBody()));
    }
    /// @brief Binds an externally allocated IPC data packet
    /// @name AttachPacket
    /// @param pIpcPacket IPC data packet
    /// @param bAutoDel Whether to auto-release
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04713
    /// @needwork = dda
    /// @endcode
    void AttachPacket(isoft::ipc::IPCPacket* const pIpcPacket, bool const bAutoDel) noexcept;
    /// @brief Gets the IPC data packet
    /// @name  GetIpcPacket
    /// @returns Pointer to IPC data packet
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04714
    /// @needwork = dda
    /// @endcode
    inline isoft::ipc::IPCPacket* GetIpcPacket() const noexcept { return pIpcPacket_; }
    /// @brief Creates an empty IPC data packet
    /// @name   CreatePacket
    /// @param nNewLen New IPC data packet length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04715
    /// @needwork = dda
    /// @endcode
    void CreatePacket(uint16_t const nNewLen) noexcept;
    /// @brief Gets the function name within the IPC data packet
    /// @name GetFuncName
    /// @returns Function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04716
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetFuncName() const noexcept;
    /// @brief Initializes the IPC data packet header
    /// @name InitIpcHead
    /// @param stFuncName Function name used for IPC call
    /// @param nPacSize IPC data packet length
    /// @returns true if init sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04717
    /// @needwork = dda
    /// @endcode
    bool InitIpcHead(ara::core::StringView const& stFuncName, uint16_t const nPacSize) noexcept;
    /// @brief Initializes the IPC data packet header
    /// @name InitIpcHead
    /// @param pIpcMsg IPC message
    /// @param nPacSize IPC data packet length
    /// @returns true if init sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04718
    /// @needwork = dda
    /// @endcode
    bool InitIpcHead(void const* const pIpcMsg, uint16_t const nPacSize) noexcept;
    /// @brief Gets process ID
    /// @name GetProcessId
    /// @returns Process ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04719
    /// @needwork = dda
    /// @endcode
    uint64_t GetProcessId() const noexcept;
    /// @brief Gets session ID
    /// @name GetSessionId
    /// @returns Session ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04720
    /// @needwork = dda
    /// @endcode
    uint64_t GetSessionId() const noexcept;

protected:
    /// @brief Create new Packet
    /// @name _NewPacketBuff
    /// @param nNewLen New IPC data packet length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04721
    /// @needwork = dda
    /// @endcode
    void _NewPacketBuff(uint16_t const nNewLen) noexcept;
    /// @brief Release Packet
    /// @name      _DelPacket
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04722
    /// @needwork = dda
    /// @endcode
    void _DelPacket() noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IPC_AUTO_MESSAGE_H_
