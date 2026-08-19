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
/// @file       isoft_per_redd_calculate.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Operation class for calculating redundancy results used by the PER persistence module
/// @date       2023-08-01
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Encryption Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2023-08-01 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#ifndef ARA_PER_CRYPTO_PH_PER_REDD_CALCULATE_H_
#define ARA_PER_CRYPTO_PH_PER_REDD_CALCULATE_H_
#ifdef ARA_WITH_CRYPTO
    #include <ara/crypto/cryp/hash_function_ctx.h>
#endif
#include "ara/per/internal/crypto/i_redd_calculate.h"
#include "ara/per/internal/isoftkv/config_muster.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @code{.isoft}
/// @unit_name=PReddAlgorithm_Base
/// @endcode
class PReddAlgorithm_Base : public IReddAlgorithm
{
private:
#ifdef ARA_WITH_CRYPTO
    /// @brief Operation object performing the calculation
    ara::crypto::cryp::HashFunctionCtx::Uptr pHashCtx_;
#endif
protected:
    /// @brief Construct the calculation object Ctx
    /// @param checkSum
    /// @return
    bool _BuildCtx(PReddData_CheckSum const &checkSum) noexcept;
#ifdef ARA_WITH_CRYPTO
    /// @brief Get a pointer to the calculation object
    /// @return
    ara::crypto::cryp::HashFunctionCtx *_GetAlgorithmCtx() noexcept { return pHashCtx_.get(); }
#endif
public:
    /// @brief Get the AlgID of the algorithm
    /// @return
    uint64_t GetAlgID() const noexcept override;
    /// @brief Reset all data
    void Reset() noexcept override;
    /// @brief Calculate Redd data
    /// @param pBSrcData Data to be calculated
    /// @param nSrcLen Length of data to be calculated
    /// @return Whether successful
    bool CalculateReddData(uint8_t const *const pBSrcData, uint32_t const nSrcLen) noexcept override;
    /// @brief Return the execution result
    /// @return Vector of uint8_t type
    ara::core::Vector< uint8_t > GetResult() const noexcept override;
};
//********************************/
/// @code{.isoft}
/// @unit_name=PReddAlgorithm_Crc
/// @endcode
class PReddAlgorithm_Crc final : public PReddAlgorithm_Base
{
public:
    /// @brief Constructor
    /// @param reddConfig
    explicit PReddAlgorithm_Crc(PReddDataCrc const &reddConfig);
    /// @brief Get the Redd type supported by the derived class
    /// @return
    EReddType GetReddType() const noexcept override;
};
//********************************/
/// @code{.isoft}
/// @unit_name=PReddAlgorithm_Hash
/// @endcode
class PReddAlgorithm_Hash final : public PReddAlgorithm_Base
{
private:
    // Initialization vector
    ara::core::Vector< uint8_t > vecInit_;

public:
    /// @brief Constructor
    /// @param reddConfig
    explicit PReddAlgorithm_Hash(PReddDataHash const &reddConfig);
    /// @brief Get the Redd type supported by the derived class
    /// @return
    EReddType GetReddType() const noexcept override;
    /// @brief Reset all data
    void Reset() noexcept override;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
#endif
