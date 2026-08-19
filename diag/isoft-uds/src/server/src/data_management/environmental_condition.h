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
/// @file       environmental_condition.h
/// @brief      This file provides environmental_condition functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/data_management
/// @interface_level=software
/// @trace_id_sr=68d4e9d2-c4f1-42ab-9291-69c73f987afb
/// @unit_name=EnvironmentalCondition
/// @unit_description=This file provides environmental_condition functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_ENVIRONMENTAL_CONDITION_H_
#define ISOFT_UDS_SERVER_ENVIRONMENTAL_CONDITION_H_

#include <cstdint>
#include <memory>

#include "common/log.h"
#include "common/request_context.h"
#include "diagnostic_data_manager.h"
#include "isoft/serialize/serialize.h"
#include "isoft/uds/data_management/environmental_condition_model.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Environmental condition class
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
class EnvironmentalCondition
{
    /// @brief Verify data condition
    /// @param[in] dataConditionPtr EnvDataCondition object
    /// @param[in] metaInfo  Meta information
    /// @param[in] cancelEventPtr Cancel event
    /// @return 0: Success >0: Failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t _checkDataCondition(EnvDataCondition& dataCondition,
                                     MetaInfoMap const& metaInfo,
                                     std::shared_ptr< CancellationState > const& cancellationState);

    /// @brief Verify condition formula
    /// @param[in] conditionFormulaPtr  EnvConditionFormula object
    /// @param[in] metaInfo Meta information
    /// @param[in] cancelEvent Cancel event
    /// @return 0: Success >0: Failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint8_t _checkConditionFormula(EnvConditionFormula& conditionFormula,
                                        MetaInfoMap const& metaInfo,
                                        std::shared_ptr< CancellationState > const& cancellationState);

    /// @brief Verify value
    /// @param[in] elementData Data obtained by reading DID
    /// @param[in] compareType  Comparison type
    /// @param[in] compareValue Comparison value, configuration data
    /// @param[in] orderEnum Endianness flag
    /// @return 0: Success >0: Failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t _compareValue(std::vector< std::uint8_t > elementData,
                               DiagnosticCompareTypeEnum const& compareType,
                               ValueSpecification& compareValue) noexcept;

    /// @brief Verify value
    /// @param[in] elementData Data obtained by reading DID
    /// @param[in] compareType  Comparison type
    /// @param[in] compareValue Comparison value, configuration data
    /// @param[in] orderEnum Endianness flag
    /// @return 0: Success >0: Failure
    /// @code{.isoft}
    /// @tparam TValue Array type
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    template < typename TValue >
    std::int32_t _compare(std::vector< TValue > a,
                          std::vector< TValue > b,
                          DiagnosticCompareTypeEnum const& compareType) noexcept
    {
        bool equal{std::equal(a.begin(), a.end(), b.begin(), b.end())};
        if (compareType == DiagnosticCompareTypeEnum::kIsEqual) {
            if (!equal) {
                LogWarn() << "EnvironmentalCondition::CompareValue| not equal!";
                return __LINE__;
            }
            return 0;
        }
        if (compareType == DiagnosticCompareTypeEnum::kIsNotEqual) {
            if (equal) {
                LogWarn() << "EnvironmentalCondition::CompareValue| equal!";
                return __LINE__;
            }
            return 0;
        }

        LogWarn() << "EnvironmentalCondition::CompareValue| compare Type invaild!";
        return __LINE__;
    }

    /// @brief Verify value
    /// @param[in] a Dynamically obtained data
    /// @param[in] b Configured data
    /// @param[in] compareType Comparison type
    /// @param[in] nullptr Verify array type
    /// @return 0: Success
    /// @code{.isoft}
    /// @tparam T Array type
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    template < typename T >
    std::int32_t _compare(T a,
                          T b,
                          DiagnosticCompareTypeEnum const& compareType,
                          std::enable_if_t< std::is_integral< T >::value >* = nullptr) noexcept
    {
        switch (compareType) {
            case DiagnosticCompareTypeEnum::kIsEqual: {
                if (a == b) {
                    return 0;
                }
                return __LINE__;
            }
            case DiagnosticCompareTypeEnum::kIsGreaterOrEqual: {
                if (a >= b) {
                    return 0;
                }
                return __LINE__;
            }
            case DiagnosticCompareTypeEnum::kIsGreaterThan: {
                if (a > b) {
                    return 0;
                }
                return __LINE__;
            }
            case DiagnosticCompareTypeEnum::kIsLessOrEqual: {
                if (a <= b) {
                    return 0;
                }
                return __LINE__;
            }
            case DiagnosticCompareTypeEnum::kIsLessThan: {
                if (a < b) {
                    return 0;
                }
                return __LINE__;
            }
            case DiagnosticCompareTypeEnum::kIsNotEqual: {
                if (a != b) {
                    return 0;
                }
                return __LINE__;
            }
            default:
                break;
        }
        return __LINE__;
    }

    /// @brief Verify value
    /// @param[in] a Dynamically obtained data
    /// @param[in] b Configured data
    /// @param[in] compareType Comparison type
    /// @return 0: Success
    /// @code{.isoft}
    /// @tparam T Array type
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t _compare(float a, float b, DiagnosticCompareTypeEnum const& compareType) noexcept
    {
        switch (compareType) {
            case DiagnosticCompareTypeEnum::kIsEqual: {
                if (std::abs(a - b) < std::numeric_limits< float >::epsilon()) {
                    return 0;
                }
                return __LINE__;
            }
            case DiagnosticCompareTypeEnum::kIsGreaterOrEqual: {
                if (a >= b) {
                    return 0;
                }
                return __LINE__;
            }
            case DiagnosticCompareTypeEnum::kIsGreaterThan: {
                if (a > b) {
                    return 0;
                }
                return __LINE__;
            }
            case DiagnosticCompareTypeEnum::kIsLessOrEqual: {
                if (a <= b) {
                    return 0;
                }
                return __LINE__;
            }
            case DiagnosticCompareTypeEnum::kIsLessThan: {
                if (a < b) {
                    return 0;
                }
                return __LINE__;
            }
            case DiagnosticCompareTypeEnum::kIsNotEqual: {
                if (std::abs(a - b) < std::numeric_limits< float >::epsilon()) {
                    return 0;
                }
                return __LINE__;
            }
            default:
                break;
        }
        return __LINE__;
    }

    /// @brief Deserialize data
    /// @param[out] realValueT Dynamically obtained data
    /// @param[out] compareValueT Configured data
    /// @param[in] realData Dynamically obtained raw data
    /// @param[in] compareValue Configured raw data
    /// @param[in] orderEnum Endianness configuration
    /// @code{.isoft}
    /// @tparam T Data type
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    template < typename T >
    void _deserialize(T& realValueT,
                      T& compareValueT,
                      std::vector< std::uint8_t >& realData,
                      ValueSpecification& compareValue) noexcept
    {
        isoft::serialize::Buffer< std::vector< uint8_t > > realDataBuffer{realData};
        isoft::serialize::Buffer< std::vector< uint8_t > > compareValueBuffer{compareValue.data};
        std::ignore = isoft::serialize::Deserialize(realDataBuffer, realValueT);
        std::ignore = isoft::serialize::Deserialize(compareValueBuffer, compareValueT);
    }

    /// @brief Deserialize data
    /// @param[out] realValue Dynamically obtained data
    /// @param[out] rawRealData Configured data
    /// @param[in] compareValue Dynamically obtained raw data
    /// @param[in] rawCompareData Configured raw data
    /// @code{.isoft}
    /// @tparam T Data type
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    template < typename T >
    void _deserialize(std::vector< T >& realValue,
                      std::vector< std::uint8_t >& rawRealData,
                      std::vector< T >& compareValue,
                      std::vector< std::uint8_t >& rawCompareData) noexcept
    {
        isoft::serialize::Buffer< std::vector< uint8_t > > realDataBuffer{rawRealData};
        std::size_t len = isoft::serialize::Deserialize(realDataBuffer, realValue);
        LogWarn() << "EnvironmentalCondition::Deserialize| length of raw result of read DID data :" << len;
        isoft::serialize::Buffer< std::vector< uint8_t > > compareValueBuffer{rawCompareData};
        std::size_t point{compareValueBuffer.First()};
        while (point != compareValueBuffer.Last()) {
            compareValue.emplace_back();
            std::ignore = isoft::serialize::DeserializePart(compareValueBuffer, point, compareValue.back());
        }
        LogWarn() << "EnvironmentalCondition::Deserialize| length of config DID data :" << rawCompareData.size();
    }

    /// @brief Environmental condition model
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    EnvironmentalConditionModel model_;

    /// @brief Environmental condition data manager pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DiagnosticDataManager > diagnosticDataManagerPtr_;

    /// @brief Initialize environmental condition model
    /// @param[in] model Environmental condition model
    /// @param[in] diagnosticDataManagerPtr Environmental condition data manager pointer
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t _initialize(EnvConditionFormula& model,
                             std::shared_ptr< DiagnosticDataManager >& diagnosticDataManagerPtr) noexcept;

    /// @brief Initialize environmental condition model
    /// @param[in] model Environmental condition model
    /// @param[in] diagnosticDataManagerPtr Environmental condition data manager pointer
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t _initialize(EnvDataCondition const& model,
                             std::shared_ptr< DiagnosticDataManager >& diagnosticDataManagerPtr) noexcept;

public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00731
    /// @needwork = dda
    /// @endcode
    EnvironmentalCondition(EnvironmentalCondition const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00732
    /// @needwork = dda
    /// @endcode
    EnvironmentalCondition& operator=(EnvironmentalCondition const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00733
    /// @needwork = dda
    /// @endcode
    EnvironmentalCondition(EnvironmentalCondition&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00734
    /// @needwork = dda
    /// @endcode
    EnvironmentalCondition& operator=(EnvironmentalCondition&&) = delete;

    /// @brief Constructor
    /// @param[in] dataIdentifierManager  Data identifier object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    explicit EnvironmentalCondition() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    virtual ~EnvironmentalCondition() noexcept = default;

    /// @brief Initialize environmental condition model
    /// @param[in] model Environmental condition model
    /// @param[in] diagnosticDataManagerPtr Environmental condition data manager pointer
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(EnvironmentalConditionModel& model,
                            std::shared_ptr< DiagnosticDataManager >& diagnosticDataManagerPtr) noexcept;

    /// @brief Check environmental condition
    /// @param[in] metaInfo Meta information
    /// @param[in] cancellationState Cancel state object
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint8_t Check(MetaInfoMap const& metaInfo,
                       std::shared_ptr< CancellationState > const& cancellationState) noexcept;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_ENVIRONMENTAL_CONDITION_H_