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
/// @file       isoft_x509_process_base.h
/// @brief      Base class for logical processing of KeyProvider provider's IPC server-side key providers
/// @details
/// @date       2023-09-24
/// @author     chang zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/certificate manager/certificate manager
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=PX509Process_Base
/// @unit_description=Base class for logical processing of server-side certificate providers
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_PROCESS_BASE_H_
#define ARA_CRYPTO_KEYS_PUHUA_PROCESS_BASE_H_

#include "ara/crypto/cryp/crypto_provider.h"
#include "ara/crypto/ipc/isoft_ipc_auto_message.h"
#include "ara/crypto/ipc/isoft_ipc_protocol.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Base class for logical processing of KeyProvider provider's IPC server-side key providers
class PX509_Manager;
//********************************/
/// @brief PX509Process_Base
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02807
/// @trace_id_dd=DD_CRYPTO_05669
/// @needwork = ad
/// @endcode
class PX509Process_Base
{
public:
    /// @brief Exclusive smart pointer type alias
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02807
    /// @trace_id_dd=DD_CRYPTO_06458
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PX509Process_Base >;
    /// @brief Result type alias for return results
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02807
    /// @trace_id_dd=DD_CRYPTO_06459
    /// @needwork = dd
    /// @endcode
    using PResultLen = ara::core::Result< uint16_t >;

public:
    /// @brief Constructor with parameters
    /// @name   PX509Process_Base
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02808
    /// @trace_id_dd=DD_CRYPTO_05670
    /// @needwork = ad
    /// @endcode
    explicit PX509Process_Base(PX509_Manager& lpcProcessManager) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02809
    /// @trace_id_dd=DD_CRYPTO_05671
    /// @needwork = ad
    /// @endcode
    PX509Process_Base() = delete;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02810
    /// @trace_id_dd=DD_CRYPTO_05672
    /// @needwork = ad
    /// @endcode
    virtual ~PX509Process_Base() noexcept = default;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02811
    /// @trace_id_dd=DD_CRYPTO_05673
    /// @needwork = ad
    /// @endcode
    PX509Process_Base& operator=(PX509Process_Base const& other) noexcept = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02812
    /// @trace_id_dd=DD_CRYPTO_05674
    /// @needwork = ad
    /// @endcode
    PX509Process_Base& operator=(PX509Process_Base&& other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02813
    /// @trace_id_dd=DD_CRYPTO_05675
    /// @needwork = ad
    /// @endcode
    PX509Process_Base(PX509Process_Base&& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02814
    /// @trace_id_dd=DD_CRYPTO_05676
    /// @needwork = ad
    /// @endcode
    PX509Process_Base(PX509Process_Base const& other) noexcept = delete;
    /// @brief Handle IPC messages
    /// @name   ProcessIpcMsg
    /// @param pReq IPC request data
    /// @param nReqLen IPC request data length
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02815
    /// @trace_id_dd=DD_CRYPTO_05677
    /// @needwork = ad
    /// @endcode
    virtual PResultLen ProcessIpcMsg(uint8_t* pReq, uint16_t nReqLen, PIpcAutoPacket& aswMsg) noexcept = 0;

public:
    /// @brief Get the referenced provider
    /// @name   GetCryptoProvider
    /// @returns Crypto provider reference
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02816
    /// @trace_id_dd=DD_CRYPTO_05678
    /// @needwork = ad
    /// @endcode
    cryp::CryptoProvider& GetCryptoProvider() const noexcept;

protected:
    /// @brief Certificate manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05679
    /// @needwork = dda
    /// @endcode
    PX509_Manager& lpcProcessManager_;  // NOLINT

protected:
    /// @brief Handle IPC errors
    /// @name   ProcessCmd_Error
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @param nErrorCode Error code
    /// @return  0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05680
    /// @needwork = dda
    /// @endcode
    static PResultLen ProcessCmd_Error(PIpcPac_Head const* const pReqHead,
                                       PIpcAutoPacket& aswMsg,
                                       SecurityErrorDomain::Errc const nErrorCode) noexcept;
    /// @brief Handle IPC errors
    /// @name   ProcessCmd_Error
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @param nErrorCode Error code
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05681
    /// @needwork = dda
    /// @endcode
    static PResultLen ProcessCmd_Error(PIpcPac_Head const* const pReqHead,
                                       PIpcAutoPacket& aswMsg,
                                       int32_t const nErrorCode) noexcept;
};
//********************************/
/// @brief IPC message handler within the certificate manager
/// @code{.isoft}
/// @tparam T_Process
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02817
/// @trace_id_dd=DD_CRYPTO_05682
/// @needwork = ad
/// @endcode
template < typename T_Process >
class PX509Process_T_Base : public PX509Process_Base
{
public:
    /// @brief Command processing function
    /// @name  CB_ProcessCmd
    /// @param pPacReq IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return Packet length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02807
    /// @trace_id_dd=DD_CRYPTO_06460
    /// @needwork = dd
    /// @endcode
    using CB_ProcessCmd
        = PX509Process_Base::PResultLen (T_Process::*)(PIpcPac_Head const* pPacReq, PIpcAutoPacket& aswMsg) const;
    /// @brief Type alias for the map list of IPC command processing functions
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02807
    /// @trace_id_dd=DD_CRYPTO_06461
    /// @needwork = dd
    /// @endcode
    using MAP_ProcessCmd = ara::core::Map< ara::core::StringView, CB_ProcessCmd >;

public:
    /// @brief Constructor
    /// @name   PX509Process_T_Base
    /// @param pObject IPC message handler object
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02818
    /// @trace_id_dd=DD_CRYPTO_05683
    /// @needwork = ad
    /// @endcode
    explicit PX509Process_T_Base(T_Process* const pObject, PX509_Manager& lpcProcessManager) noexcept
        : PX509Process_Base{lpcProcessManager}, mapProcessCmd_{}, pObject_{pObject} {};
    /// @brief Default destructor
    /// @name   ~PX509Process_T_Base
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02819
    /// @trace_id_dd=DD_CRYPTO_05684
    /// @needwork = ad
    /// @endcode
    ~PX509Process_T_Base() noexcept override = default;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02820
    /// @trace_id_dd=DD_CRYPTO_05685
    /// @needwork = ad
    /// @endcode
    PX509Process_T_Base() noexcept = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02821
    /// @trace_id_dd=DD_CRYPTO_05686
    /// @needwork = ad
    /// @endcode
    PX509Process_T_Base& operator=(PX509Process_T_Base const& other) noexcept = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02822
    /// @trace_id_dd=DD_CRYPTO_05687
    /// @needwork = ad
    /// @endcode
    PX509Process_T_Base& operator=(PX509Process_T_Base&& other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02823
    /// @trace_id_dd=DD_CRYPTO_05688
    /// @needwork = ad
    /// @endcode
    PX509Process_T_Base(PX509Process_T_Base&& other) noexcept = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02824
    /// @trace_id_dd=DD_CRYPTO_05689
    /// @needwork = ad
    /// @endcode
    PX509Process_T_Base(PX509Process_T_Base const& other) noexcept = delete;
    /// @brief Handle IPC messages
    /// @name  ProcessIpcMsg
    /// @param pReq IPC request data
    /// @param nReqLen IPC request data length
    /// @param aswMsg Response message managed by IPC hosting
    /// @return 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02825
    /// @trace_id_dd=DD_CRYPTO_05690
    /// @needwork = ad
    /// @endcode
    PResultLen ProcessIpcMsg(uint8_t* pReq, uint16_t nReqLen, PIpcAutoPacket& aswMsg) noexcept override
    {
        std::ignore = nReqLen;
        PIpcPac_Head* const pPacHead{static_cast< PIpcPac_Head* >(static_cast< void* >(pReq))};
        ara::core::StringView const stFuncName{pPacHead->GetFuncName()};
        typename MAP_ProcessCmd::iterator const itFind{std::move(mapProcessCmd_.find(stFuncName))};
        if (itFind == mapProcessCmd_.end()) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);
        }
        CB_ProcessCmd const pFindProcess{itFind->second};
        return std::move((pObject_->*pFindProcess)(pPacHead, aswMsg));  // Pass error code
        // return ( (T_Process*)(this)->*pFindProcess)(pPacHead, aswMsg);
    }

private:
    /// @brief Map list of IPC command processing functions
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05691
    /// @needwork = dda
    /// @endcode
    MAP_ProcessCmd mapProcessCmd_{};

private:
    /// @brief IPC command handler object pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05692
    /// @needwork = dda
    /// @endcode
    T_Process* pObject_{nullptr};

protected:
    /// @brief Insert data into the map
    /// @param key Process identifier
    /// @param fun Callback function for handling IPC messages
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08700
    /// @needwork = dda
    /// @endcode
    void _InsertMapCmd(ara::core::StringView const& key, CB_ProcessCmd const fun) noexcept
    {
        mapProcessCmd_[key] = fun;
    }
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_PROCESS_BASE_H_
