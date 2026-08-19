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
/// @file       isoft_auto_increace_id.h
/// @brief      AutoSar-AP encryption and decryption common module
/// @details    Puhua thread lock wrapper class
/// @date       2025-09-29
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
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_COMMON_PH_AUTO_INCREACE_ID_H_
#define ARA_CRYPTO_COMMON_PH_AUTO_INCREACE_ID_H_

#include <pthread.h>

#include "ara/crypto/common/isoft_thread_lock.h"

namespace ara {
namespace crypto {
//********************************/

/// @brief Used to handle the global Io Agent Index
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_04343
/// @trace_id_dd=DD_CRYPTO_08712
/// @needwork = ad
/// @endcode
class PAutoIncreaceID
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08713
    /// @needwork = dda
    /// @endcode
    PAutoIncreaceID() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08714
    /// @needwork = dda
    /// @endcode
    ~PAutoIncreaceID() noexcept = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08715
    /// @needwork = dda
    /// @endcode
    PAutoIncreaceID(PAutoIncreaceID &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08716
    /// @needwork = dda
    /// @endcode
    PAutoIncreaceID(PAutoIncreaceID const &other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another object instance of this class
    /// @return PSvrIoInterfaceAgent_Hsmbst&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08717
    /// @needwork = dda
    /// @endcode
    PAutoIncreaceID &operator=(PAutoIncreaceID &&other) = delete;
    /// @brief Default copy assignment operator
    /// @param other Another object instance of this class
    /// @return PSvrIoInterfaceAgent_Hsmbst&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08718
    /// @needwork = dda
    /// @endcode
    PAutoIncreaceID &operator=(PAutoIncreaceID const &other) = delete;

public:
    /// @brief Get auto-increment ID: 64-bit
    /// @return Auto-increment ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08719
    /// @needwork = dda
    /// @endcode
    uint64_t GetAutoIncreaceID() noexcept
    {
        ara::crypto::PAutoLock const autoLock{mThreadLock_};
        nIndexID_ += 1U;
        /// The first three bytes are the prefix bits, the last five bytes are the data bits, indexId is the maximum transaction ID of the current prefix
        uint64_t const indexId{0x00FFFFFFFFFFFFFFUL};
        if (nIndexID_ > indexId) {
            nIndexID_ = 1U;
        }
        return nIndexID_;
    }
    /// @brief Get auto-increment ID: 32-bit
    /// @return Auto-increment ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08719
    /// @needwork = dda
    /// @endcode
    uint32_t GetAutoIncreaceID_32() noexcept
    {
        ara::crypto::PAutoLock const autoLock{mThreadLock_};
        nIndexID_ += 1U;
        /// The first three bytes are the prefix bits, the last five bytes are the data bits, indexId is the maximum transaction ID of the current prefix
        uint64_t const indexId{0xFFFFFFFFU};
        if (nIndexID_ > indexId) {
            nIndexID_ = 1U;
        }
        return static_cast< uint32_t >(nIndexID_);
    }

private:
    /// @brief Auto-increment ID
    /// @name   nIndexID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08720
    /// @needwork = dda
    /// @endcode
    uint64_t nIndexID_{0U};
    /// @brief Thread lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08721
    /// @needwork = dda
    /// @endcode
    ara::crypto::PThreadLock mThreadLock_{};
};
//********************************/
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_COMMON_PH_AUTO_INCREACE_ID_H_
