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
/// @file       isoft_convert_alg_id.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    Puhua encryption and decryption context type definition
/// @date       2022-01-11
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
/// @module_path=/CRYPTO/Default Encryption and Decryption/Cryptographic Primitive
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PConvertAlgID
/// @unit_description=Convert cryptographic primitive to string
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_PUHUA_CONVERT_ALG_ID_H_
#define ARA_CRYPTO_PUHUA_CONVERT_ALG_ID_H_

#include <cstdint>

#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/core/vector.h"
#include "ara/crypto/cryp/cryobj/crypto_primitive_id.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Convert cryptographic primitive to string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00060
/// @trace_id_dd=DD_CRYPTO_00655
/// @needwork = ad
/// @endcode
class PConvertAlgID final
{
public:
    /// @brief Type alias for cryptographic primitive conversion lookup table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00060
    /// @trace_id_dd=DD_CRYPTO_06243
    /// @needwork = dd
    /// @endcode
    using VecAlgName = ara::core::Vector< CryptoPrimitiveId::Uptr >;

private:
    /// @brief Cryptographic primitive conversion lookup table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00656
    /// @needwork = dda
    /// @endcode
    VecAlgName vecPuhuaAlgName_{};
    /// @brief Cryptographic primitive conversion lookup table: Hash
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00657
    /// @needwork = dda
    /// @endcode
    VecAlgName vecHashAlgName_{};

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00061
    /// @trace_id_dd=DD_CRYPTO_00658
    /// @needwork = ad
    /// @endcode
    PConvertAlgID() noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00062
    /// @trace_id_dd=DD_CRYPTO_00659
    /// @needwork = ad
    /// @endcode
    ~PConvertAlgID() noexcept;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another CryptoProvider to this instance.
    /// @param other another object instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00063
    /// @trace_id_dd=DD_CRYPTO_00660
    /// @needwork = ad
    /// @endcode
    PConvertAlgID &operator=(PConvertAlgID const &other) = default;
    /// @brief Default move assignment operator
    /// @brief Move-assign another CryptoProvider to this instance.
    /// @param other another object instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00064
    /// @trace_id_dd=DD_CRYPTO_00661
    /// @needwork = ad
    /// @endcode
    PConvertAlgID &operator=(PConvertAlgID &&other) = default;
    /// @brief Copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00065
    /// @trace_id_dd=DD_CRYPTO_00662
    /// @needwork = ad
    /// @endcode
    PConvertAlgID(PConvertAlgID const &other) = delete;
    /// @brief Move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00066
    /// @trace_id_dd=DD_CRYPTO_00663
    /// @needwork = ad
    /// @endcode
    PConvertAlgID(PConvertAlgID &&other) = delete;
    /// @brief Convert string to cryptographic primitive ID
    /// @param stAlgName cryptographic primitive string
    /// @return cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00067
    /// @trace_id_dd=DD_CRYPTO_00664
    /// @needwork = ad
    /// @endcode
    uint64_t ConvertToAlgId(ara::core::StringView const &stAlgName) const noexcept;
    /// @brief Convert cryptographic primitive ID to string
    /// @param nTypeID cryptographic primitive ID
    /// @return cryptographic primitive string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00068
    /// @trace_id_dd=DD_CRYPTO_00665
    /// @needwork = ad
    /// @endcode
    ara::core::StringView ConvertToAlgName(uint64_t const nTypeID) const noexcept;
    /// @brief Check whether the cryptographic primitive is a valid cryptographic primitive
    /// @param nTypeID cryptographic primitive ID
    /// @return true if valid false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00069
    /// @trace_id_dd=DD_CRYPTO_00666
    /// @needwork = ad
    /// @endcode
    bool IsValidAlgID(uint64_t const nTypeID) const noexcept;
    /// @brief Check whether the cryptographic primitive belongs to the Hash class
    /// @param nTypeID cryptographic primitive ID
    /// @return  true if valid false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00070
    /// @trace_id_dd=DD_CRYPTO_00667
    /// @needwork = ad
    /// @endcode
    bool IsValidHashAlgID(uint64_t const nTypeID) const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_PUHUA_CONVERT_ALG_ID_H_