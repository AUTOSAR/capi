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
/// @file       isoft_x509_manager.h
/// @brief      AutoSar-Crypto key storage module
/// @details    KeyProvider provider's IPC server-side key slot manager.
/// @date       2023-09-24
/// @author     chang zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </td>2023-09-24  <td>1.0.0      <td>chang zheng      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/certificate manager/certificate manager
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=PX509_Manager
/// @unit_description=Certificate manager
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_X509_MANAGER_H_
#define ARA_CRYPTO_KEYS_PUHUA_X509_MANAGER_H_

#include <ara/core/string.h>

#include "ara/crypto/ipc/isoft_ipc_auto_message.h"
#include "ara/crypto/ipc/isoft_ipc_protocol.h"
#include "ara/crypto/ipc/isoft_ipc_serverx509.h"
#include "ara/crypto/keys/updates_observer.h"
#include "ara/crypto/manifest/manifest_crypto_data.h"
#include "ara/crypto/manifest/manifest_ksp_config.h"
#include "ara/crypto/x509/isoft_svr_cert_loader.h"
#include "ara/crypto/x509/isoft_x509_process_base.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief KeyProvider provider's IPC server-side key slot manager.
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02855
/// @trace_id_dd=DD_CRYPTO_05733
/// @needwork = ad
/// @endcode
class PX509_Manager
{
public:
    /// @brief Certificate loader MAP list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02855
    /// @trace_id_dd=DD_CRYPTO_06464
    /// @needwork = dd
    /// @endcode
    using MAP_KeySlot = ara::core::Map< uint32_t, PSvrCertLoader::Uptr >;
    /// @brief Certificate IPC handler MAP list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02855
    /// @trace_id_dd=DD_CRYPTO_06465
    /// @needwork = dd
    /// @endcode
    using MAP_IpcProcess = ara::core::Map< ara::core::StringView, PX509Process_Base::Uptr >;
    /// @brief Certificate transaction MAP list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02855
    /// @trace_id_dd=DD_CRYPTO_06466
    /// @needwork = dd
    /// @endcode
    using MAP_Transaction = ara::core::Map< uint32_t, ara::core::Vector< uint32_t > >;

public:
    /// @brief Constructor
    /// @name      PX509_Manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02856
    /// @trace_id_dd=DD_CRYPTO_05734
    /// @needwork = ad
    /// @endcode
    PX509_Manager();
    /// @brief Destructor
    /// @name      ~PX509_Manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02857
    /// @trace_id_dd=DD_CRYPTO_05735
    /// @needwork = ad
    /// @endcode
    virtual ~PX509_Manager() noexcept;
    /// @brief Default copy constructor note: changed to default here, may cause errors on lower version compilers (declared to take const reference, but implicit declaration would take non-const)
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02858
    /// @trace_id_dd=DD_CRYPTO_05736
    /// @needwork = ad
    /// @endcode
    PX509_Manager(PX509_Manager const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02859
    /// @trace_id_dd=DD_CRYPTO_05737
    /// @needwork = ad
    /// @endcode
    PX509_Manager(PX509_Manager &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02860
    /// @trace_id_dd=DD_CRYPTO_05738
    /// @needwork = ad
    /// @endcode
    PX509_Manager &operator=(PX509_Manager const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02861
    /// @trace_id_dd=DD_CRYPTO_05739
    /// @needwork = ad
    /// @endcode
    PX509_Manager &operator=(PX509_Manager &&other) = delete;
    /// @brief Execute
    /// @name   Running
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02862
    /// @trace_id_dd=DD_CRYPTO_05740
    /// @needwork = ad
    /// @endcode
    void Running() const noexcept;

public:
    /// @brief Handle IPC messages
    /// @name   ProcessIpcMsg
    /// @param pReq IPC request data
    /// @param nReqLen IPC request data length
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02863
    /// @trace_id_dd=DD_CRYPTO_05741
    /// @needwork = ad
    /// @endcode
    int32_t ProcessIpcMsg(uint8_t *const pReq, uint16_t const nReqLen, PIpcAutoPacket &aswMsg) noexcept;
    /// @brief Handle IPC errors
    /// @name   ProcessCmd_Error
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @param nErrorCode Error code
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02864
    /// @trace_id_dd=DD_CRYPTO_05742
    /// @needwork = ad
    /// @endcode
    static int32_t ProcessCmd_Error(PIpcPac_Head const *const pReqHead,
                                    PIpcAutoPacket &aswMsg,
                                    SecurityErrorDomain::Errc const nErrorCode) noexcept;
    /// @brief Get the referenced provider
    /// @name   GetCryptoProvider
    /// @returns Crypto provider reference
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02865
    /// @trace_id_dd=DD_CRYPTO_05743
    /// @needwork = ad
    /// @endcode
    cryp::CryptoProvider &GetCryptoProvider() const noexcept;

public:
    /// @brief Check whether the certificate exists: return value -1 means not found, 0 means found but empty, other positive values indicate the data length in the certificate slot
    /// @name  IsCertExist
    /// @param stCertName Certificate name
    /// @return -1 means not found, 0 means found but empty, other positive values indicate the data length in the certificate slot
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02866
    /// @trace_id_dd=DD_CRYPTO_05744
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint32_t > IsCertExist(ara::core::StringView const &stCertName) noexcept;
    /// @brief Load certificate
    /// @name  LoadCert
    /// @param stCertName Certificate name
    /// @param vecData Return result
    /// @return Certificate data length if present, otherwise error
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02867
    /// @trace_id_dd=DD_CRYPTO_05745
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint32_t > LoadCert(ara::core::StringView const &stCertName,
                                           ara::core::Vector< uint8_t > &vecData) noexcept;
    /// @brief Save certificate
    /// @name  SaveCert
    /// @param stCertName Certificate name
    /// @param pCertData Certificate data
    /// @param nDataLen Data length
    /// @return has value if save cert sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02868
    /// @trace_id_dd=DD_CRYPTO_05746
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint32_t > SaveCert(ara::core::StringView const &stCertName,
                                           uint8_t const *const pCertData,
                                           uint16_t const nDataLen) noexcept;
    /// @brief Delete certificate
    /// @name  RemoveCert
    /// @param stCertName Certificate name
    /// @return 0 sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02869
    /// @trace_id_dd=DD_CRYPTO_05747
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint32_t > RemoveCert(ara::core::StringView const &stCertName) noexcept;
    /// @brief Find the corresponding crypto provider by slot name
    /// @param stSlotName Key slot name
    /// @return Corresponding crypto provider for the slot name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02870
    /// @trace_id_dd=DD_CRYPTO_05748
    /// @needwork = ad
    /// @endcode
    ara::core::String FindCryptoProviderBySlotName(ara::core::StringView const &stSlotName) noexcept;
    /// @brief Delete certificate
    /// @name  IsTrustMaster
    /// @param stProcessName Process name
    /// @return true if is trustmaster false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02871
    /// @trace_id_dd=DD_CRYPTO_05749
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > IsTrustMaster(ara::core::StringView const &stProcessName) noexcept;
    /// @brief Set certificate extension information
    /// @param stCertName Certificate name
    /// @param nIndex Index
    /// @return true if set cert exinfo sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02872
    /// @trace_id_dd=DD_CRYPTO_05750
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > SetCertExInfo(ara::core::StringView const &stCertName, uint16_t const nIndex) noexcept;
    /// @brief Check whether CSR is set to pending
    /// @param stCertName Certificate name
    /// @return true if has already set pending
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02873
    /// @trace_id_dd=DD_CRYPTO_05751
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > IsCsrSetPending(ara::core::StringView const &stCertName) noexcept;
    /// @brief Get the actual key slot name
    /// @param stSlotName Key slot name
    /// @return Actual key slot name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02874
    /// @trace_id_dd=DD_CRYPTO_05752
    /// @needwork = ad
    /// @endcode
    ara::core::Vector< ara::core::String > FindRealSlotNames(ara::core::StringView const &stSlotName) noexcept;

private:
    /// @brief Crypto configuration file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05753
    /// @needwork = dda
    /// @endcode
    manifest::PManifestKspConfig manifestKspConfig_{};
    /// @brief Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05754
    /// @needwork = dda
    /// @endcode
    cryp::CryptoProvider::Uptr pCryptoProvider_{};
    /// @brief Key slot manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05755
    /// @needwork = dda
    /// @endcode
    MAP_KeySlot mapKeySlot_{};
    /// @brief IPC packet handler
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05756
    /// @needwork = dda
    /// @endcode
    MAP_IpcProcess mapIpcProcess_{};
    /// @brief Transaction management
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05757
    /// @needwork = dda
    /// @endcode
    MAP_Transaction mapTransation_{};
    /// @brief Certificate IPC server
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05758
    /// @needwork = dda
    /// @endcode
    mutable PIpcServerX509 ipcServer_;

protected:
    /// @brief Get the configuration file name
    /// @name  GetConfigFileName
    /// @return Configuration file name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05759
    /// @needwork = dda
    /// @endcode
    static ara::core::String GetConfigFileName() noexcept;
    /// @brief Get the default CryptoProvider
    /// @name  DefCryptoProvider
    /// @return Default CryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05760
    /// @needwork = dda
    /// @endcode
    static cryp::CryptoProvider::Uptr DefCryptoProvider() noexcept;
    /// @brief Find the corresponding certificate slot configuration in the configuration file
    /// @name  _FindConfigCertSlot
    /// @param stSlotName Key slot name
    /// @return Corresponding key content attribute information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05761
    /// @needwork = dda
    /// @endcode
    keys::KeySlotPrototypeProps::Uptr _FindConfigCertSlot(ara::core::StringView const &stSlotName) const noexcept;
    /// @brief Find the corresponding certificate slot configuration in the configuration file
    /// @name  _FindConfigCert
    /// @param stCertName Certificate name
    /// @return Corresponding certificate slot configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05762
    /// @needwork = dda
    /// @endcode
    manifest::PConfig_CryptoCertificate const *_FindConfigCert(ara::core::StringView const &stCertName) const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_X509_MANAGER_H_