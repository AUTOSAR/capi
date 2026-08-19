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
/// @file       isoft_io_interface_file.h
/// @brief      AutoSar-Crypto key storage module
/// @details    Key slot loader on the IPC server side
/// @date       2023-09-24
/// @author     chang zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <td>2023-09-27  <td>1.0.0      <td>chang zheng      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/certificate manager/IO Interface
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=PIoInterface_File
/// @unit_description=Interface for saving and loading security objects
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_FILE_H_
#define ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_FILE_H_

#include <ara/per/file_storage.h>

#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/common/isoft_io_interface_mem.h"
#include "ara/crypto/cryp/crypto_provider.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_block.h"
#include "ara/crypto/keys/keyslot.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Formal interface of IOInterface, used to save and load security objects.
///         Actual saving and loading should be implemented through known internal methods in the trusted pair of crypto provider and storage provider.
///         Each object should be uniquely identified by its type and cryptographic object unique identifier (COUID). This interface assumes that objects in the container are compressed, i.e., have a minimal optimized size.
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02826
/// @trace_id_dd=DD_CRYPTO_05693
/// @needwork = ad
/// @endcode
class PIoInterface_File
{
public:
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02826
    /// @trace_id_dd=DD_CRYPTO_06462
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PIoInterface_File >;

public:
    /// @brief Constructor
    /// @name   PIoInterface_File
    /// @param cryptoProvider Crypto provider
    /// @param stStorageName Persistent storage name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02827
    /// @trace_id_dd=DD_CRYPTO_05694
    /// @needwork = ad
    /// @endcode
    PIoInterface_File(ara::crypto::cryp::CryptoProvider& cryptoProvider,
                      ara::core::StringView const& stStorageName) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02828
    /// @trace_id_dd=DD_CRYPTO_05695
    /// @needwork = ad
    /// @endcode
    PIoInterface_File() = delete;
    /// @brief Default virtual destructor
    /// @name  ~PIoInterface_File
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02829
    /// @trace_id_dd=DD_CRYPTO_05696
    /// @needwork = ad
    /// @endcode
    ~PIoInterface_File() noexcept = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02830
    /// @trace_id_dd=DD_CRYPTO_05697
    /// @needwork = ad
    /// @endcode
    PIoInterface_File(PIoInterface_File const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02831
    /// @trace_id_dd=DD_CRYPTO_05698
    /// @needwork = ad
    /// @endcode
    PIoInterface_File(PIoInterface_File&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02832
    /// @trace_id_dd=DD_CRYPTO_05699
    /// @needwork = ad
    /// @endcode
    PIoInterface_File& operator=(PIoInterface_File const& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02833
    /// @trace_id_dd=DD_CRYPTO_05700
    /// @needwork = ad
    /// @endcode
    PIoInterface_File& operator=(PIoInterface_File&& other) = delete;

public:  // PIoInterface custom interface
    /// @brief Initialize the IO interface
    /// @name   InitIoInterface
    /// @param stCertName Certificate name
    /// @returns true if init iointerface sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02834
    /// @trace_id_dd=DD_CRYPTO_05701
    /// @needwork = ad
    /// @endcode
    bool InitIoInterface(ara::core::StringView const& stCertName) noexcept;
    /// @brief Read key data
    /// @name   ReadCertData
    /// @param vecData Return result
    /// @returns true if read cert data sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02835
    /// @trace_id_dd=DD_CRYPTO_05702
    /// @needwork = ad
    /// @endcode
    bool ReadCertData(ara::core::Vector< ara::core::Byte >& vecData) const noexcept;
    /// @brief Save key data
    /// @name   SaveCertData
    /// @param pData Data: starting memory address
    /// @param nLen Data length: in bytes
    /// @return true if save cert data sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02836
    /// @trace_id_dd=DD_CRYPTO_05703
    /// @needwork = ad
    /// @endcode
    bool SaveCertData(uint8_t const* const pData, uint16_t const nLen) noexcept;
    /// @brief Save key data
    /// @name   RemoveCert
    /// @return true if remove sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02837
    /// @trace_id_dd=DD_CRYPTO_05704
    /// @needwork = ad
    /// @endcode
    bool RemoveCert() noexcept;
    /// @brief Set certificate extension
    /// @param nIndex Index
    /// @return true if set cert exinfo sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02838
    /// @trace_id_dd=DD_CRYPTO_05705
    /// @needwork = ad
    /// @endcode
    bool SetCertExInfo(uint16_t const nIndex) const noexcept;
    /// @brief Check whether CSR is set to pending
    /// @return true if csr has already set pending false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02839
    /// @trace_id_dd=DD_CRYPTO_05706
    /// @needwork = ad
    /// @endcode
    bool IsCsrSetPending() const noexcept;

private:
    /// @brief Provider used for encryption/decryption of this object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05707
    /// @needwork = dda
    /// @endcode
    ara::crypto::cryp::CryptoProvider& cryptoProvider_;
    /// @brief File storage repository
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05708
    /// @needwork = dda
    /// @endcode
    mutable ara::per::SharedHandle< ara::per::FileStorage > pFileStorage_{};
    /// @brief File storage repository name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05709
    /// @needwork = dda
    /// @endcode
    ara::core::String stStorageName_{};
    /// @brief File name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05710
    /// @needwork = dda
    /// @endcode
    ara::core::String stFileName_{};
    /// @brief Key used to encrypt the key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05711
    /// @needwork = dda
    /// @endcode
    internal::PAutoBuff certData_;
    /// @brief Initialize and load SlotData from the KV database
    /// @name   _initLoadCertData
    /// @returns true if read cert info from kv sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05712
    /// @needwork = dda
    /// @endcode
    bool _initLoadCertData() noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_FILE_H_
