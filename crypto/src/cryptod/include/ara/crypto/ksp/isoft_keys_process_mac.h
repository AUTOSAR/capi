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
/// @file       isoft_keys_process_mac.h
/// @brief      AutoSar-Crypto key storage module
/// @details    KeyProvider provider's IPC server side: logical processing of message authentication codes
/// @date       2022-08-17
/// @author     CHANG ZHENG
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/message authentication code IPC service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_02005
/// @unit_name=PKeysProcess_Mac
/// @unit_description=Message authentication code IPC service
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_MAC_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_MAC_H_

#include "ara/crypto/cryp/mac/isoft_ctx_mac_base.h"
#include "ara/crypto/ksp/isoft_keys_process_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {

/// @brief IPC MAC context management structure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02950
/// @trace_id_dd=DD_CRYPTO_06631
/// @needwork = dd
/// @endcode
struct MacContextManager
{
    /// @brief Because the key must be saved before the context is destroyed, put it together here
    std::pair< MessageAuthnCodeCtx::Uptr, SymmetricKey::Uptrc > macContext{};
    /// @brief Timestamp
    std::time_t timeSecond{0U};
};

/// @brief MAC operation type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02949
/// @trace_id_dd=DD_CRYPTO_05886
/// @needwork = ad
/// @endcode
enum class DoOperateMac : uint32_t
{
    kDoSetKey = 0,  // Clone operation
    kDoInit   = 1,  // Jump operation
    kDoReset  = 2,  // Jump operation
    kDoUpdate = 3,  // Next operation
    kDoFinish = 4,  // XOR operation
};
//********************************/
/// @brief IPC MAC processing logic class
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02950
/// @trace_id_dd=DD_CRYPTO_05887
/// @needwork = ad
/// @endcode
class PKeysProcess_Mac : public keys::isoft_def::PKeysProcess_T_Base< PKeysProcess_Mac >
{
public:
    /// @brief Constructor with parameters
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02951
    /// @trace_id_dd=DD_CRYPTO_05888
    /// @needwork = ad
    /// @endcode
    explicit PKeysProcess_Mac(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02952
    /// @trace_id_dd=DD_CRYPTO_05889
    /// @needwork = ad
    /// @endcode
    ~PKeysProcess_Mac() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02953
    /// @trace_id_dd=DD_CRYPTO_05890
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Mac(PKeysProcess_Mac const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02954
    /// @trace_id_dd=DD_CRYPTO_05891
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Mac(PKeysProcess_Mac &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02955
    /// @trace_id_dd=DD_CRYPTO_05892
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Mac &operator=(PKeysProcess_Mac const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02956
    /// @trace_id_dd=DD_CRYPTO_05893
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Mac &operator=(PKeysProcess_Mac &&other) = delete;

public:
    /// @brief ara::core::Map<uint64_t, std::unique_ptr<MacContextManager> > declaration
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02950
    /// @trace_id_dd=DD_CRYPTO_06472
    /// @needwork = dd
    /// @endcode
    using MAP_ContextManger = ara::core::Map< uint64_t, std::unique_ptr< MacContextManager > >;

private:
    /// @brief mapContextManger_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05894
    /// @needwork = dda
    /// @endcode
    mutable MAP_ContextManger mapContextManger_{};

public:
    /// @brief Set key
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05895
    /// @needwork = dda
    /// @endcode
    PResultLen SetKey(keys::isoft_def::PIpcPac_Head const *const pReqHead,
                      keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Execute init operation
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return has value if DoInit sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05896
    /// @needwork = dda
    /// @endcode
    PResultLen DoInit(keys::isoft_def::PIpcPac_Head const *const pReqHead,
                      keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Execute update operation
    /// @name  DoUpdate
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns  has value if DoUpdate sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05897
    /// @needwork = dda
    /// @endcode
    PResultLen DoUpdate(keys::isoft_def::PIpcPac_Head const *const pReqHead,
                        keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Execute finish operation
    /// @name  DoFinish
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns  has value if DoFinish sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05898
    /// @needwork = dda
    /// @endcode
    PResultLen DoFinish(keys::isoft_def::PIpcPac_Head const *const pReqHead,
                        keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;
    /// @brief Execute reset operation
    /// @name  DoReset
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns  has value if DoReset sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05899
    /// @needwork = dda
    /// @endcode
    PResultLen DoReset(keys::isoft_def::PIpcPac_Head const *const pReqHead,
                       keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept;

private:
    /// @brief Find the corresponding context by PID
    /// @param nPid Process ID
    /// @returns MacContextManager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05900
    /// @needwork = dda
    /// @endcode
    MacContextManager *_findMacContextByID(uint64_t const nPid) const noexcept
    {
        MAP_ContextManger::iterator const itFind{mapContextManger_.find(nPid)};
        if (itFind == mapContextManger_.end()) {
            return nullptr;
        }
        return itFind->second.get();
    }
    /// @brief MAC IPC specific logic processing
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @param doOperater MAC operation type
    /// @return has value if DoOperaterMac sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05901
    /// @needwork = dda
    /// @endcode
    PResultLen _doOperaterMac(keys::isoft_def::PIpcPac_Head const *const pReqHead,
                              keys::isoft_def::PIpcAutoPacket &aswMsg,
                              DoOperateMac const doOperater) const noexcept;
    /// @brief Clean up timed-out context objects (clean up after 1 hour)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05902
    /// @needwork = dda
    /// @endcode
    void _clearMacContextOfTimeOut() const noexcept
    {
        /// Get current time
        std::time_t const t0{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
        MAP_ContextManger::iterator itFind{mapContextManger_.begin()};
        while (itFind != mapContextManger_.end()) {
            if (itFind->second->timeSecond - t0 >= (kInt_60 * kInt_60)) {
                itFind = mapContextManger_.erase(itFind);
            } else {
                itFind++;
            }
        }
    }
    /// @brief Delete context cache object by process ID
    /// @param nPid Process ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05903
    /// @needwork = dda
    /// @endcode
    void _removeMacContextFromPid(uint64_t const nPid) const noexcept
    {
        MAP_ContextManger::iterator itFind{mapContextManger_.begin()};
        while (true) {
            if (itFind == mapContextManger_.end()) {
                break;
            }
            if (itFind->first == nPid) {
                std::ignore = mapContextManger_.erase(itFind);
                break;
            }
            itFind++;
        }
    }
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEYS_PROCESS_MAC_H_