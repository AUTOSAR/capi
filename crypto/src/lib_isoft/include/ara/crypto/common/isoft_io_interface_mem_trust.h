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
/// @file       isoft_io_interface_mem_trust.h
/// @brief      AutoSar-AP encryption and decryption common module
/// @details    Formal interface of IOInterface, used to save and load security objects: implementation version of KV database.
/// @date       2022-04-12
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
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Common_api
/// @unit_description=IO interface exported from trusted container
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_MEM_TRUST_H_
#define ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_MEM_TRUST_H_
#include "ara/crypto/common/isoft_io_interface_mem.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
/// @brief Trusted container
class PVolatileTrustedContainer;
/// @brief This class is only exported from the trusted container and is used to temporarily store the written key
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00005
/// @trace_id_dd=DD_CRYPTO_00182
/// @needwork = ad
/// @endcode
class PIoInterface_Mem_Trust : public PIoInterface_Mem
{
private:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00183
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem_Trust() noexcept : PIoInterface_Mem{true} {}
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00184
    /// @needwork = dda
    /// @endcode
    ~PIoInterface_Mem_Trust() override = default;

public:
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00185
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem_Trust(PIoInterface_Mem_Trust const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00186
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem_Trust(PIoInterface_Mem_Trust&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00187
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem_Trust& operator=(PIoInterface_Mem_Trust const& other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00188
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem_Trust& operator=(PIoInterface_Mem_Trust&& other) = delete;
    /// @brief Friend class
    // PRQA S 2107 QAC /// @qac: This is a friend declaration.
    friend class PVolatileTrustedContainer;
#// PRQA L:QAC

public:
    /// @brief Determine whether the iointerface is a session object
    /// @return true if seesion object false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00189
    /// @needwork = dda
    /// @endcode
    inline bool IsObjectSession() const noexcept override { return false; }
};

/// @brief This class is only used on the IPC server side
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00006
/// @trace_id_dd=DD_CRYPTO_00190
/// @needwork = ad
/// @endcode
class PIoInterface_Mem_Server : public PIoInterface_Mem
{
public:
    /// @brief Exclusive smart pointer type alias
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00006
    /// @trace_id_dd=DD_CRYPTO_06238
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PIoInterface_Mem_Server >;
    /// @brief Parameterized constructor
    /// @param bVolatile Whether it is volatile
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00191
    /// @needwork = dda
    /// @endcode
    explicit PIoInterface_Mem_Server(bool const bVolatile) noexcept : PIoInterface_Mem{bVolatile} {}
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00192
    /// @needwork = dda
    /// @endcode
    ~PIoInterface_Mem_Server() override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00193
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem_Server(PIoInterface_Mem_Server const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00194
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem_Server(PIoInterface_Mem_Server&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00195
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem_Server& operator=(PIoInterface_Mem_Server const& other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00196
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem_Server& operator=(PIoInterface_Mem_Server&& other) = delete;

public:
    /// @brief Determine whether the iointerface is a session object
    /// @return true if session object false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00197
    /// @needwork = dda
    /// @endcode
    inline bool IsObjectSession() const noexcept override { return false; }
};

}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_MEM_TRUST_H_
