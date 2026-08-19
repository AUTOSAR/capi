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
/// @file       i_redd_calculate.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Encryption and decryption operation class used by the PER persistence module
/// @date       2022-12-22
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Encryption Management
/// @interface_level=unit
/// @trace_id_sr=
/// @unit_name=IReddAlgorithm
/// @unit_description=Encryption and decryption operation class used by the PER persistence module
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2022-12-22 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#ifndef ARA_PER_CRYPTO_I_REDD_CALCULATE_H_
#define ARA_PER_CRYPTO_I_REDD_CALCULATE_H_

#include <ara/core/utility.h>
#include <ara/core/vector.h>

#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/common/isoft_data_type.h"
#include "ara/per/internal/isoftkv/page_common_struct.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Base class for calculating ReddCrc/ReddHash
class IReddAlgorithm
{
public:
    /// @brief Default constructor
    IReddAlgorithm() = default;
    /// @brief
    virtual ~IReddAlgorithm() = default;
    /// @brief Copy constructor
    /// @param other Another object instance of this class
    IReddAlgorithm(IReddAlgorithm const& other) = default;
    /// @brief Move constructor
    /// @param other Another object instance of this class
    IReddAlgorithm(IReddAlgorithm&& other) noexcept = default;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    IReddAlgorithm& operator=(IReddAlgorithm const& other) = default;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    IReddAlgorithm& operator=(IReddAlgorithm&& other) noexcept = default;

public:
    /// @brief Get the Redd type supported by the derived class
    /// @return
    virtual EReddType GetReddType() const noexcept = 0;
    /// @brief Get the AlgID of the algorithm
    /// @return
    virtual uint64_t GetAlgID() const noexcept = 0;
    /// @brief Reset all data
    virtual void Reset() noexcept = 0;
    /// @brief Calculate Redd data
    /// @param pBSrcData Data to be calculated
    /// @param nSrcLen Length of data to be calculated
    /// @return Whether successful
    virtual bool CalculateReddData(uint8_t const* const pBSrcData, uint32_t const nSrcLen) noexcept = 0;
    /// @brief Return the execution result
    /// @return Vector of uint8_t type
    virtual ara::core::Vector< uint8_t > GetResult() const noexcept = 0;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
#endif
