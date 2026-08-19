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
/// @file       isoft_auto_buff.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    Adaptive buffer class used by the Crypto module
/// @date       2022-02-17
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
/// @unit_description=Buff wrapper for automatically managed heap memory
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_AUTO_BUFF_H_
#define ARA_CRYPTO_CRYP_PUHUA_AUTO_BUFF_H_

#include <cstdint>

namespace ara {
namespace crypto {
namespace internal {  /// @qac Possibly cannot modify [2502]: This name hides a similar kind of declaration.

//********************************/
/// @brief Buff wrapper class for automatically managed heap memory
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00057
/// @trace_id_dd=DD_CRYPTO_00597
/// @needwork = ad
/// @endcode
class PAutoBuff final
{
private:
    /// @brief Data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00598
    /// @needwork = dda
    /// @endcode
    uint8_t* pBuffData_{nullptr};
    /// @brief Capacity
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00599
    /// @needwork = dda
    /// @endcode
    uint32_t nBuffSize_{0U};
    /// @brief Size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00600
    /// @needwork = dda
    /// @endcode
    uint32_t nDataLen_{0U};

public:
    /// @brief Constructor: default
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00602
    /// @needwork = dda
    /// @endcode
    PAutoBuff() noexcept = default;
    /// @brief Constructor: initialize only the buffer length
    /// @name   PAutoBuff
    /// @param nBuffSize capacity of the BUFF
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00603
    /// @needwork = dda
    /// @endcode
    explicit PAutoBuff(uint32_t const nBuffSize) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00604
    /// @needwork = dda
    /// @endcode
    ~PAutoBuff() noexcept;
    /// @brief Copy constructor: deleted
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00605
    /// @needwork = dda
    /// @endcode
    PAutoBuff(PAutoBuff const& other) = delete;
    /// @brief Move assignment operator: deleted
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00606
    /// @needwork = dda
    /// @endcode
    PAutoBuff(PAutoBuff&& other) = delete;
    /// @brief Copy assignment operator: deleted
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00607
    /// @needwork = dda
    /// @endcode
    PAutoBuff& operator=(PAutoBuff const& other) noexcept = delete;
    /// @brief Override move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00608
    /// @needwork = dda
    /// @endcode
    PAutoBuff& operator=(PAutoBuff&& other) & noexcept;

public:
    /// @brief Return the data area
    /// @name   Data
    /// @param nIndex index
    /// @returns starting address of the data area
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00609
    /// @needwork = dda
    /// @endcode
    uint8_t* Data(uint32_t const nIndex = 0U) const noexcept;
    /// @brief Return the data length
    /// @name  size
    /// @returns data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00610
    /// @needwork = dda
    /// @endcode
    inline uint32_t size() const noexcept { return nDataLen_; }  // NOLINT
    /// @brief Get data at a specific position
    /// @name   at
    /// @param nIndex index
    /// @returns data at the specific position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00611
    /// @needwork = dda
    /// @endcode
    uint8_t at(uint32_t const nIndex) const noexcept;  // NOLINT
    /// @brief Get the buffer length
    /// @name  GetBuffLen
    /// @returns buffer length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00612
    /// @needwork = dda
    /// @endcode
    inline uint32_t GetBuffLen() const noexcept { return nBuffSize_; }
    /// @brief Reset the buffer: may allocate new memory
    /// @name   ResetBuff
    /// @param nBuffSize capacity of the BUFF
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00613
    /// @needwork = dda
    /// @endcode
    void ResetBuff(uint32_t nBuffSize) noexcept;
    /// @brief Reset the data area
    /// @name   ResetData
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00614
    /// @needwork = dda
    /// @endcode
    void ResetData() noexcept;
    /// @brief Set the data area starting from the head of the data area
    /// @name   SetData
    /// @param pData data: starting memory address
    /// @param nLen data length in bytes
    /// @param bForceNew whether to force creation of new buffer
    /// @returns true if set data sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00615
    /// @needwork = dda
    /// @endcode
    bool SetData(uint8_t const* const pData, uint32_t const nLen, bool bForceNew = true) noexcept;
    /// @brief Append data starting from the tail of the data area
    /// @name   AddData
    /// @param pData data: starting memory address
    /// @param nLen data length in bytes
    /// @returns ture if add data sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00616
    /// @needwork = dda
    /// @endcode
    bool AddData(uint8_t const* const pData, uint32_t const nLen) noexcept;
    /// @brief Add multiple copies of the same data
    /// @name  AddData
    /// @param pData data: starting memory address
    /// @param nDataLen data length
    /// @returns ture if add data sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00617
    /// @needwork = dda
    /// @endcode
    bool AddData(uint8_t const pData, uint32_t const nDataLen) noexcept;
    /// @brief Set the data length
    /// @name  SetDataSize
    /// @param nSize data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00618
    /// @needwork = dda
    /// @endcode
    inline void SetDataSize(uint32_t const nSize) noexcept { nDataLen_ = nSize; }
    /// @brief Set the data length
    /// @param nDataLen data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00619
    /// @needwork = dda
    /// @endcode
    inline void SetDataLen(uint32_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
    /// @brief Increase the data length
    /// @param nDataLen data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00620
    /// @needwork = dda
    /// @endcode
    void AddDataLen(int32_t const nDataLen) noexcept;

protected:
    /// @brief Release memory
    /// @name   _DelBuff
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00622
    /// @needwork = dda
    /// @endcode
    void _DelBuff() noexcept;
};
//********************************/
}  // namespace internal
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_AUTO_BUFF_H_
