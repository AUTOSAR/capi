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
/// @file       environmental_condition.cpp
/// @brief      This file implements environmental_condition functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=data_management
/// @trace_id_sr=68d4e9d2-c4f1-42ab-9291-69c73f987afb
/// @unit_name=EnvironmentalCondition
/// @unit_description=This file provides environmental_condition functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "environmental_condition.h"

#include <algorithm>

#include "common/log.h"
namespace isoft {
namespace uds {
namespace server {

EnvironmentalCondition::EnvironmentalCondition() noexcept : model_{}, diagnosticDataManagerPtr_{nullptr} {}


std::int32_t EnvironmentalCondition::_initialize(
    EnvConditionFormula& model, std::shared_ptr< DiagnosticDataManager >& diagnosticDataManagerPtr) noexcept
{
    if (model.parts.empty()) {
        LogWarn() << "EnvironmentalCondition::Initialize| EnvConditionFormula.parts is empty!";
        return -1;
    }
    for (auto&& part : model.parts) {
        if (part.get() == nullptr) {
            LogWarn() << "EnvironmentalCondition::Initialize| EnvConditionFormula.part is nullptr!";
            return -1;
        }
        std::int32_t result{0};
        if (std::is_same< decltype(part), EnvDataCondition >::value) {
            std::shared_ptr< EnvDataCondition > conditionPtr{};
            conditionPtr = std::dynamic_pointer_cast< EnvDataCondition >(part);
            result       = _initialize(*conditionPtr, diagnosticDataManagerPtr);
            if (result != 0) {
                LogWarn() << "EnvironmentalCondition::Initialize| Initialize EnvDataCondition fails!";
                return result;
            }
        } else if (std::is_same< decltype(part), EnvConditionFormula >::value) {
            std::shared_ptr< EnvConditionFormula > formulaPtr{};
            formulaPtr = std::dynamic_pointer_cast< EnvConditionFormula >(part);
            result     = _initialize(*formulaPtr, diagnosticDataManagerPtr);
            if (result != 0) {
                LogWarn() << "EnvironmentalCondition::Initialize| Initialize EnvConditionFormula fails!";
                return result;
            }
        }
    }
    return 0;
}


std::int32_t EnvironmentalCondition::_initialize(
    EnvDataCondition const& model, std::shared_ptr< DiagnosticDataManager >& diagnosticDataManagerPtr) noexcept
{
    Result< DiagnosticDataElementModel > elementModelResult{};
    elementModelResult = diagnosticDataManagerPtr->GetDiagnosticDataElementModel(model.element);
    if (!elementModelResult.HasValue()) {
        LogWarn() << "EnvironmentalCondition::Initialize| element id is invalid! id:" << model.element;
        return -1;
    }
    if (elementModelResult.Value().dataType != model.compareValue.type) {
        LogWarn() << "EnvironmentalCondition::Initialize| EnvDataCondition type mismatch!";
        return -1;
    }
    return 0;
}


std::int32_t EnvironmentalCondition::Initialize(
    EnvironmentalConditionModel& model, std::shared_ptr< DiagnosticDataManager >& diagnosticDataManagerPtr) noexcept
{
    if (diagnosticDataManagerPtr.get() == nullptr) {
        LogWarn() << "EnvironmentalCondition::Initialize| diagnosticDataManagerPtr is nullptr";
        return -1;
    }
    diagnosticDataManagerPtr_ = diagnosticDataManagerPtr;
    if (model.id < 0) {
        LogWarn() << "EnvironmentalCondition::Initialize|EnvironmentalConditionId must > 0!,current id:" << model.id;
        return -1;
    }
    model_ = model;

    return _initialize(model.formula, diagnosticDataManagerPtr);
}

std::uint8_t EnvironmentalCondition::Check(MetaInfoMap const& metaInfo,
                                           std::shared_ptr< CancellationState > const& cancellationState) noexcept
{

    return _checkConditionFormula(model_.formula, metaInfo, cancellationState);
}

std::int32_t EnvironmentalCondition::_checkDataCondition(EnvDataCondition& dataCondition,
                                                         MetaInfoMap const& metaInfo,
                                                         std::shared_ptr< CancellationState > const& cancellationState)
{
    Result< isoft::uds::server::DiagnosticDataElement > readElementResult;
    readElementResult = diagnosticDataManagerPtr_->ReadDataElement(dataCondition.element, metaInfo, cancellationState);
    if (!readElementResult.HasValue()) {
        return static_cast< std::int32_t >(readElementResult.Error());
    }
    isoft::uds::server::DiagnosticDataElement& element{readElementResult.Value()};
    if (element.record.empty()) {
        LogWarn() << "EnvironmentalCondition::_checkDataCondition| read data element failed! data element is empty";
        return __LINE__;
    }

    return _compareValue(element.record, dataCondition.compareType, dataCondition.compareValue);
}

std::uint8_t EnvironmentalCondition::_checkConditionFormula(
    EnvConditionFormula& conditionFormula,
    MetaInfoMap const& metaInfo,
    std::shared_ptr< CancellationState > const& cancellationState)
{
    std::uint8_t resultValue{0U};
    for (std::size_t curIndex = 0U; curIndex < conditionFormula.parts.size(); curIndex++) {
        std::uint32_t uiIndex{static_cast< std::uint32_t >(curIndex)};
        std::shared_ptr< EnvDataCondition > dataConditionPtr{
            std::dynamic_pointer_cast< EnvDataCondition >(conditionFormula.parts[uiIndex])};
        if (dataConditionPtr.get() != nullptr) {
            std::int32_t checkDataCondition{0};
            checkDataCondition = _checkDataCondition(*dataConditionPtr, metaInfo, cancellationState);
            if ((conditionFormula.op == DiagnosticLogicalOperatorEnum::kLogicalAnd)) {
                if (checkDataCondition != 0) {
                    LogWarn() << "EnvironmentalCondition::_checkConditionFormula| index:" << uiIndex
                              << " check data condition fails!";
                    resultValue = conditionFormula.nrcValue;
                    break;
                }
                LogVerbose() << "EnvironmentalCondition::_checkConditionFormula| index:" << uiIndex
                             << " check data condition success!";
            } else if (conditionFormula.op == DiagnosticLogicalOperatorEnum::kLogicalOr) {
                if (checkDataCondition == 0) {
                    resultValue = 0;
                    LogVerbose() << "EnvironmentalCondition::_checkConditionFormula| index:" << uiIndex
                                 << " check data condition success!";
                    break;
                }

                LogWarn() << "EnvironmentalCondition::_checkConditionFormula| index:" << uiIndex
                          << " check data condition fails";
                resultValue = conditionFormula.nrcValue;
            }
            continue;
        }
        std::shared_ptr< EnvConditionFormula > nextConditionFormulaPtr{
            std::dynamic_pointer_cast< EnvConditionFormula >(conditionFormula.parts[uiIndex])};
        if (nextConditionFormulaPtr.get() != nullptr) {
            resultValue = _checkConditionFormula(*nextConditionFormulaPtr, metaInfo, cancellationState);
            if ((resultValue != 0U) && (conditionFormula.op == DiagnosticLogicalOperatorEnum::kLogicalAnd)) {
                LogWarn() << "EnvironmentalCondition::_checkConditionFormula| index:" << uiIndex
                          << " check condition formula fails";
                break;
            }
            if (conditionFormula.op == DiagnosticLogicalOperatorEnum::kLogicalOr) {
                if (resultValue == 0) {
                    break;
                }
            }

        } else {
            LogWarn() << "EnvironmentalCondition::_checkConditionFormula| parts invaild";
        }
    }
    return resultValue;
}

std::int32_t EnvironmentalCondition::_compareValue(std::vector< std::uint8_t > elementData,
                                                   DiagnosticCompareTypeEnum const& compareType,
                                                   ValueSpecification& compareValue) noexcept
{
    switch (compareValue.type) {
        case DataType::kUint8: {
            if (compareValue.isArray) {
               
                return _compare(elementData, compareValue.data, compareType);
            }
            
            return _compare(elementData[0U], compareValue.data[0U], compareType);
        }
        case DataType::kUint16: {
            if (compareValue.isArray) {
                ara::core::Vector< std::uint16_t > realData;
                ara::core::Vector< std::uint16_t > compareData;
                _deserialize(realData, elementData, compareData, compareValue.data);
               
                return _compare(realData, compareData, compareType);
            }
            std::uint16_t uRealValue{0U};
            std::uint16_t uCompareValue{0U};
            _deserialize(uRealValue, uCompareValue, elementData, compareValue);
            
            return _compare(uRealValue, uCompareValue, compareType);
        }
        case DataType::kUint32: {
            if (compareValue.isArray) {
                ara::core::Vector< std::uint32_t > realData;
                ara::core::Vector< std::uint32_t > compareData;
                _deserialize(realData, elementData, compareData, compareValue.data);
  
                return _compare(realData, compareData, compareType);
            }
            std::uint32_t uRealValue{0U};
            std::uint32_t uCompareValue{0U};
            _deserialize(uRealValue, uCompareValue, elementData, compareValue);

            return _compare(uRealValue, uCompareValue, compareType);
        }
        case DataType::kInt8: {
            if (compareValue.isArray) {
                ara::core::Vector< std::int8_t > realData;
                ara::core::Vector< std::int8_t > compareData;
                _deserialize(realData, elementData, compareData, compareValue.data);
           
                return _compare(realData, compareData, compareType);
            }
            std::int8_t iRealValue{0};
            std::int8_t iCompareValue{0};
            _deserialize(iRealValue, iCompareValue, elementData, compareValue);

            return _compare(iRealValue, iCompareValue, compareType);
        }
        case DataType::kInt16: {
            if (compareValue.isArray) {
                ara::core::Vector< std::int16_t > realData;
                ara::core::Vector< std::int16_t > compareData;
                _deserialize(realData, elementData, compareData, compareValue.data);

                return _compare(realData, compareData, compareType);
            }
            std::int16_t iRealValue{0};
            std::int16_t iCompareValue{0};
            _deserialize(iRealValue, iCompareValue, elementData, compareValue);

            return _compare(iRealValue, iCompareValue, compareType);
        }
        case DataType::kInt32: {
            if (compareValue.isArray) {
                ara::core::Vector< std::int32_t > realData;
                ara::core::Vector< std::int32_t > compareData;
                _deserialize(realData, elementData, compareData, compareValue.data);

                return _compare(realData, compareData, compareType);
            }
            std::int32_t iRealValue{0};
            std::int32_t iCompareValue{0};
            _deserialize(iRealValue, iCompareValue, elementData, compareValue);

            return _compare(iRealValue, iCompareValue, compareType);
        }
        case DataType::kFloat: {
            if (compareValue.isArray) {
                ara::core::Vector< float > realData;
                ara::core::Vector< float > compareData;
                _deserialize(realData, elementData, compareData, compareValue.data);

                return _compare(realData, compareData, compareType);
            }
            float fRealValue{0};
            float fCompareValue{0};
            _deserialize(fRealValue, fCompareValue, elementData, compareValue);

            return _compare(fRealValue, fCompareValue, compareType);
        }
        default:
            break;
    }
    return __LINE__;
}  // namespace server

}  // namespace server
}  // namespace uds
}  // namespace isoft