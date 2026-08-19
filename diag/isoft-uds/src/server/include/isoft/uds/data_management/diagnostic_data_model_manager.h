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
/// @file       diagnostic_data_model_manager.h
/// @brief      This file provides diagnostic_data_model_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/data_management
/// @interface_level=Module
/// @trace_id_sr=c191fba7-b9b8-4edb-a295-4aa1515ac7a2
/// @unit_name=DiagnosticDataModelManager
/// @unit_description=This file provides diagnostic_data_model_manager functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_INCLUDE_DIAGNOSTICDATA_H_
#define ISOFT_UDS_SERVER_INCLUDE_DIAGNOSTICDATA_H_
#include <memory>
#include <vector>

#include "generic_data_element.h"
#include "generic_data_identifier.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Diagnostic data element data type.
/// @code{.isoft}
/// @note Diagnostic data element data types include integer, floating point, string, etc.
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
enum class DataType
{
    /// @brief 8-bit integer.
    /// @note 8-bit integer data type is used to represent 8-bit integer, range [-128, 127].
    kInt8,
    /// @brief 8-bit unsigned integer.
    /// @note 8-bit unsigned integer data type is used to represent 8-bit unsigned integer, range [0, 255].
    kUint8,
    /// @brief 16-bit integer.
    /// @note 16-bit integer data type is used to represent 16-bit integer, range [-32768, 32767].
    kInt16,
    /// @brief 16-bit unsigned integer.
    /// @note 16-bit unsigned integer data type is used to represent 16-bit unsigned integer, range [0, 65535].
    kUint16,
    /// @brief 32-bit integer.
    /// @note 32-bit integer data type is used to represent 32-bit integer, range [-2147483648, 2147483647].
    kInt32,
    /// @brief 32-bit unsigned integer.
    /// @note 32-bit unsigned integer data type is used to represent 32-bit unsigned integer, range [0, 4294967295].
    kUint32,
    /// @brief Floating point number.
    /// @note Floating point number data type is used to represent floating point number, range [-3.4028234663852886E+38, 3.4028234663852886E+38].
    /// @note Floating point number data type is used to represent floating point number, range [-3.4028234663852886E+38, 3.4028234663852886E+38].
    kFloat
};

/// @brief Diagnostic data element array.
/// @param isVariableSize Whether it is a variable-length array.
/// @param maxNumberOfElements Maximum number of elements.
/// @code{.isoft}
/// @note Diagnostic data element array is used to represent multiple diagnostic data elements, each element has a data type and an array.
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct DiagnosticDataElementArray
{
    /// @brief Whether it is a variable-length array.
    /// @note Whether it is a variable-length array, used to indicate whether the length of the array can change dynamically.
    /// @default false
    /// @note The length of a variable-length array can change dynamically at runtime, while the length of a fixed-length array is determined at compile time.
    bool isVariableSize{false};
    /// @brief Maximum number of elements.
    /// @note Maximum number of elements, used to indicate the maximum number of elements in the array.
    /// @default maxNumberOfElements
    /// @note The maximum number of elements must be greater than or equal to 0.
    std::uint32_t maxNumberOfElements{0U};
};

/// @brief Diagnostic data element provider type.
/// @code{.isoft}
/// @note Diagnostic data element provider types include unused, external provider, DCM session provider, DCM security level provider, etc.
/// @interface_level=unit
/// @needwork = ad
/// @endcode
enum class DiagnosticDataElementProvider
{
    /// @brief Unused.
    /// @note Unused provider is used to indicate that the diagnostic data element is not used.
    kUnused,
    /// @brief External provider.
    /// @note External provider is used to indicate that the diagnostic data element is provided externally.
    kExternal,
    /// @brief DCM session provider.
    /// @note DCM session provider is used to indicate that the diagnostic data element is provided by DCM session.
    kDcmSession,
    /// @brief DCM security level provider.
    /// @note DCM security level provider is used to indicate that the diagnostic data element is provided by DCM security level.
    kDcmSecurityLevel
};

/// @brief Diagnostic data element model.
/// @param dataType Data type.
/// @param array Array.
/// @param provider Provider.
/// @param id ID。
/// @code{.isoft}
/// @note Diagnostic data element model is used to represent detailed information of diagnostic data element, including data type, array, provider, ID, etc.
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct DiagnosticDataElementModel
{
    /// @brief Data type.
    /// @note Data type, used to represent the data type of the diagnostic data element.
    /// @default kInt32
    /// @note 32-bit integer.
    DataType dataType{DataType::kInt32};
    /// @brief Array.
    /// @note Array, used to represent the array of the diagnostic data element.
    /// @default nullptr
    /// @note Array must be variable-length array or fixed-length array.
    std::shared_ptr< DiagnosticDataElementArray > array{nullptr};
    /// @brief Provider.
    /// @note Provider, used to represent the provider of the diagnostic data element.
    /// @default kUnused
    /// @note Unused provider is used to indicate that the diagnostic data element is not used.
    DiagnosticDataElementProvider provider{DiagnosticDataElementProvider::kUnused};
    /// @brief ID。
    /// @note ID, used to uniquely identify a diagnostic data element.
    /// @default 0U
    /// @note ID must be greater than or equal to 0.
    std::uint32_t id{0U};
};

/// @brief Diagnostic data parameter model.
/// @param offset Offset.
/// @param element Element model.
/// @code{.isoft}
/// @note Diagnostic data parameter model is used to represent detailed information of diagnostic data element, including offset, element model, etc.
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct DiagnosticDataParamterModel
{
    /// @brief Offset.
    /// @note Offset, used to indicate the offset of the diagnostic data element in the data.
    /// @default 0U
    /// @note Offset must be greater than or equal to 0.
    std::uint32_t offset{0U};
    /// @brief Element model.
    /// @note Element model, used to represent detailed information of the diagnostic data element.
    /// @default element
    /// @note Element model must be a valid diagnostic data element model.
    DiagnosticDataElementModel element{};
};

/// @brief Define how to read diagnostic data.
/// @code{.isoft}
/// @note Define how to read diagnostic data includes calling DID's Read method to read data, using Element's Read method to read data, etc.
/// @interface_level=unit
/// @needwork = ad
/// @endcode
enum class DiagnosticDataReadMethod
{
    /// @brief Call DID's Read method to read data.
    /// @note Calling DID's Read method to read data is used to call DID's Read method, the detailed information of the read data for diagnostic data element must be obtained through the read method.
    /// @default kUseReadMothod
    /// @note The read method must be a valid read method.
    kUseReadMothod,
    /// @brief Call Element's Read method to read data.
    /// @note Calling Element's Read method to read data is used to call Element's Read method, the detailed information of the read data for diagnostic data element must be obtained through reading the element.
    /// @default kUseReadElementMothod
    /// @note The read element must be a valid read element.
    kUseReadElementMothod
};

/// @brief Diagnostic data model.
/// @param id ID。
/// @param nSize Size.
/// @param details Parameter model.
/// @param readType Read method.
/// @code{.isoft}
/// @note Diagnostic data model is used to represent detailed information of diagnostic data element, including ID, size, parameter model, read method, etc.
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct DiagnosticDataModel
{
    /// @brief ID。
    /// @note ID, used to uniquely identify a diagnostic data element.
    /// @default 0U
    /// @note ID must be greater than or equal to 0.
    std::uint16_t id{0U};
    /// @brief Size.
    /// @note Size, used to indicate the size of the diagnostic data element.
    /// @default 0U
    /// @note Size must be greater than or equal to 0.
    std::uint32_t nSize{0U};
    /// @brief Parameter model.
    /// @note Parameter model, used to represent detailed information of the diagnostic data element.
    /// @default details
    /// @note Parameter model must be a valid diagnostic data parameter model.
    std::vector< DiagnosticDataParamterModel > details{};
    /// @brief How to read diagnostic data.
    /// @note How to read diagnostic data, used to indicate that the detailed information of the diagnostic data element must be obtained through reading the element.
    /// @default kUseReadMothod
    /// @note The read element must be a valid read element.
    DiagnosticDataReadMethod readType{DiagnosticDataReadMethod::kUseReadMothod};
};

/// @brief Dynamic diagnostic data identifier.
/// @note Dynamic diagnostic data identifier is used to uniquely identify a dynamic diagnostic data element.
/// @default 0U
/// @note Dynamic diagnostic data identifier must be greater than or equal to 0.
using DiagnosticDynamicDataIdentifier = std::uint16_t;

/// @brief Diagnostic data model manager.
/// @param staticData Static diagnostic data model.
/// @param edrElement Read element.
/// @param dynamicData Dynamic diagnostic data identifier.
/// @param dataInterfacePtr Data interface pointer.
/// @param dataElementInterfacePtr Data element interface pointer.
/// @param storeDynamicDataModel Whether to store dynamic diagnostic data model.
/// @code{.isoft}
/// @note Diagnostic data model manager is used to manage models of diagnostic data elements, including static diagnostic data models, dynamic diagnostic data models, etc.
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct DiagnosticDataModelManager
{
    /// @brief Static diagnostic data model.
    /// @note Static diagnostic data model, used to represent detailed information of the diagnostic data element.
    /// @default staticData
    /// @note Static diagnostic data model must be a valid diagnostic data model.
    std::vector< DiagnosticDataModel > staticData{};
    /// @brief Extended data model definition.
    /// @note Extended data model definition, used to represent detailed information of the diagnostic data element.
    /// @default edrElement
    /// @note Extended data model definition must be a valid read element.
    std::vector< DiagnosticDataParamterModel > edrElement{};
    /// @brief Dynamic diagnostic data identifier.
    /// @note Dynamic diagnostic data identifier, used to uniquely identify a dynamic diagnostic data element.
    /// @default dynamicData
    /// @note Dynamic diagnostic data identifier must be greater than or equal to 0.
    std::vector< DiagnosticDynamicDataIdentifier > dynamicData{};
    /// @brief Data interface pointer.
    /// @note Data interface pointer, used to call DID's Read method, the detailed information of the read data for diagnostic data element must be obtained through the read method.
    /// @default dataInterfacePtr
    /// @note Data interface pointer must be a valid data interface pointer.
    std::shared_ptr< GenericDataIdentifierInterface > dataInterfacePtr{nullptr};
    /// @brief Data element interface pointer.
    /// @note Data element interface pointer, used to call Element's Read method, the detailed information of the read data for diagnostic data element must be obtained through reading the element.
    /// @default dataElementInterfacePtr
    /// @note Data element interface pointer must be a valid data element interface pointer.
    std::shared_ptr< GenericDataElementInterface > dataElementInterfacePtr{nullptr};
    /// @brief Whether to store dynamic diagnostic data model.
    /// @note Whether to store dynamic diagnostic data model, used to indicate whether to store dynamic diagnostic data model.
    /// @default false
    /// @note storeDynamicDataModel must be a valid boolean value.
    bool storeDynamicDataModel{false};
};
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  ///ISOFT_UDS_SERVER_INCLUDE_DIAGNOSTICDATA_H_