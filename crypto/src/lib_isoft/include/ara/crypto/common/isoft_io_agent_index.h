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
/// @file       isoft_io_agent_index.h
/// @brief      AutoSar-AP encryption and decryption common module
/// @details    Formal interface of IOInterface, used to manage agent codes.
/// @date       2026-01-21
/// @author     Che Jinzhao
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
/// @module_path=/CRYPTO/Key Manager/IO Interface
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=IoAgentIndex
/// @unit_description=Server-side proxy interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/common/isoft_thread_lock.h"

/// @brief Used to handle the global Io Agent Index
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_04346
/// @trace_id_dd=DD_CRYPTO_08742
/// @needwork = ad
/// @endcode
class IoAgentIndex
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08743
    /// @needwork = dda
    /// @endcode
    IoAgentIndex() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08744
    /// @needwork = dda
    /// @endcode
    ~IoAgentIndex() noexcept = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08745
    /// @needwork = dda
    /// @endcode
    IoAgentIndex(IoAgentIndex&& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08746
    /// @needwork = dda
    /// @endcode
    IoAgentIndex(IoAgentIndex const& other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another object instance of this class
    /// @return PSvrIoInterfaceAgent_Hsmbst&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08747
    /// @needwork = dda
    /// @endcode
    IoAgentIndex& operator=(IoAgentIndex&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other Another object instance of this class
    /// @return PSvrIoInterfaceAgent_Hsmbst&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08748
    /// @needwork = dda
    /// @endcode
    IoAgentIndex& operator=(IoAgentIndex const& other) = delete;

public:
    /// @brief Get IO agent sequence number
    /// @return mIoAgentIndex_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08749
    /// @needwork = dda
    /// @endcode
    uint32_t GetIoAgentIndex() noexcept
    {
        ara::crypto::PAutoLock const autoLock{mThreadLockIndex_};
        mIoAgentIndex_ += 1U;
        return mIoAgentIndex_;
    }

private:
    /// @brief Global IoInterfaceID
    /// @name   mIoAgentIndex_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08750
    /// @needwork = dda
    /// @endcode
    uint32_t mIoAgentIndex_{0U};
    /// @brief Thread lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08751
    /// @needwork = dda
    /// @endcode
    ara::crypto::PThreadLock mThreadLockIndex_{};
};