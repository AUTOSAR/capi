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
/// @file       isoft_svr_cert_loader.h
/// @brief      AutoSar-Crypto key storage module
/// @details
/// @date       2023-09-24
/// @author     chang zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/certificate manager/certificate slot storage
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=PSvrCertLoader
/// @unit_description=Certificate slot storage
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_SVR_CERT_LOADER_H_
#define ARA_CRYPTO_KEYS_PUHUA_SVR_CERT_LOADER_H_

#include "ara/core/map.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/common/isoft_io_interface_mem.h"
#include "ara/crypto/ipc/isoft_ipc_protocol.h"
#include "ara/crypto/keys/isoft_updates_observer.h"
#include "ara/crypto/keys/keyslot.h"
#include "ara/crypto/x509/isoft_io_interface_file.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
/// @brief Certificate manager
class PX509_Manager;
//********************************/
/// @brief Key slot loader on the IPC server side
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02840
/// @trace_id_dd=DD_CRYPTO_05713
/// @needwork = ad
/// @endcode
class PSvrCertLoader
{
public:
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02840
    /// @trace_id_dd=DD_CRYPTO_06463
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PSvrCertLoader >;

public:
    /// @brief Create a new certificate loader
    /// @param keySlotManager Key slot manager
    /// @param stCertName Certificate name
    /// @return PSvrCertLoader instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02841
    /// @trace_id_dd=DD_CRYPTO_05714
    /// @needwork = ad
    /// @endcode
    static PSvrCertLoader::Uptr NewCertSlotLoader(PX509_Manager &keySlotManager,
                                                  ara::core::StringView const &stCertName) noexcept;
    /// @brief Get certificate storage pointer
    /// @return Certificate storage pointer
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02842
    /// @trace_id_dd=DD_CRYPTO_05715
    /// @needwork = ad
    /// @endcode
    static ara::per::SharedHandle< ara::per::FileStorage > GetCertStoragePtr() noexcept;
    /// @brief Get certificate IO operation pointer
    /// @param cryptoProvider Crypto provider
    /// @param stCertName Certificate name
    /// @return PIoInterface_File instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02843
    /// @trace_id_dd=DD_CRYPTO_05716
    /// @needwork = ad
    /// @endcode
    static PIoInterface_File::Uptr NewCertSlotIo(cryp::CryptoProvider &cryptoProvider,
                                                 ara::core::StringView const &stCertName) noexcept;

public:
    /// @brief Constructor
    /// @param keySlotManager Key slot manager
    /// @param stStorageName Persistent storage name
    /// @param stCertName Certificate name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02844
    /// @trace_id_dd=DD_CRYPTO_05717
    /// @needwork = ad
    /// @endcode
    explicit PSvrCertLoader(PX509_Manager &keySlotManager,
                            ara::core::StringView const &stStorageName,
                            ara::core::StringView const &stCertName) noexcept;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02845
    /// @trace_id_dd=DD_CRYPTO_05718
    /// @needwork = ad
    /// @endcode
    virtual ~PSvrCertLoader() noexcept = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02846
    /// @trace_id_dd=DD_CRYPTO_05719
    /// @needwork = ad
    /// @endcode
    PSvrCertLoader(PSvrCertLoader const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02847
    /// @trace_id_dd=DD_CRYPTO_05720
    /// @needwork = ad
    /// @endcode
    PSvrCertLoader(PSvrCertLoader &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02848
    /// @trace_id_dd=DD_CRYPTO_05721
    /// @needwork = ad
    /// @endcode
    PSvrCertLoader &operator=(PSvrCertLoader const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02849
    /// @trace_id_dd=DD_CRYPTO_05722
    /// @needwork = ad
    /// @endcode
    PSvrCertLoader &operator=(PSvrCertLoader &&other) = delete;

private:
    /// @brief Certificate manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05723
    /// @needwork = dda
    /// @endcode
    PX509_Manager &ipcProcessManager_;
    /// @brief Storage repository name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05724
    /// @needwork = dda
    /// @endcode
    ara::core::String stStorageName_{};
    /// @brief Certificate name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05725
    /// @needwork = dda
    /// @endcode
    ara::core::String stCertName_{};
    /// @brief File IO interface pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05726
    /// @needwork = dda
    /// @endcode
    PIoInterface_File::Uptr pIoInterface_{};

public:
    /// @brief Save the incoming certificate via IPC
    /// @param pData Data: starting memory address
    /// @param nLen Data length: in bytes
    /// @returns true if save cert sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02850
    /// @trace_id_dd=DD_CRYPTO_05727
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > SaveCertData(uint8_t const *const pData, uint16_t const nLen) const noexcept;
    /// @brief Load the certificate via IPC and save it
    /// @returns Certificate data information
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02851
    /// @trace_id_dd=DD_CRYPTO_05728
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > LoadCertData() const noexcept;
    /// @brief Delete certificate via IPC
    /// @return true if remove cert sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02852
    /// @trace_id_dd=DD_CRYPTO_05729
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > RemoveCert() const noexcept;
    /// @brief Set root certificate
    /// @param nIndex Index
    /// @return true if set cert exinfo sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02853
    /// @trace_id_dd=DD_CRYPTO_05730
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > SetCertExInfo(uint16_t const nIndex) const noexcept;
    /// @brief Check whether CSR has been set to pending
    /// @return true if has already set pending false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02854
    /// @trace_id_dd=DD_CRYPTO_05731
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > IsCsrSetPending() const noexcept;

protected:
    /// @brief Return the PIOInterface used by itself
    /// @returns PIoInterface_File instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05732
    /// @needwork = dda
    /// @endcode
    PIoInterface_File::Uptr _MakeIoInterface() const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_SVR_CERT_LOADER_H_
