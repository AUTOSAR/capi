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
/// @file       service_validation_service.cpp
/// @brief      This file implements service_validation_service functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=src
/// @trace_id_sr=8c25caf1-6707-49df-99c4-260a8a629818
/// @unit_name=ServiceValidationService
/// @unit_description=This file provides service_validation_service functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_validation_service.h"

#include "isoft/uds/type_int.h"

namespace isoft {
namespace uds {
namespace server {

ServiceValidationService::ServiceValidationService()
    : manufacturerTable_{}, supplierTable_{}, requestManufacturerConfirmTable_{}, requestSupplierConfirmTable_{}
{
}


void ServiceValidationService::AddManufacturerServiceValidation(
    std::shared_ptr< ServiceValidationInterface > const& serviceValidation) noexcept
{
    manufacturerTable_.push_back(serviceValidation);
}


void ServiceValidationService::AddSupplierServiceValidation(
    std::shared_ptr< ServiceValidationInterface > const& serviceValidation) noexcept
{
    supplierTable_.push_back(serviceValidation);
}

bool ServiceValidationService::ManufacturerServiceValidate(std::shared_ptr< RequestContext > const& request) noexcept
{
    MessagePtr requestPtr{request->GetRequest()};
    std::vector< std::uint8_t > body{requestPtr->GetBody()};
    MetaInfoMap metaInfo{request->GetMetaInfoMap()};
    isoft::uds::Result< void > result;
    std::vector< ServiceValidationInterfacePtr > validatePassTable;
    std::string const requestHandle{metaInfo.at("kRequestHandle")};
    for (auto&& manufacturer : manufacturerTable_) {
        result = manufacturer->Validate(body, metaInfo);
        if (!result.HasValue()) {
            std::ignore = requestManufacturerConfirmTable_.insert({requestHandle, validatePassTable});
            std::uint8_t const nrc{static_cast< std::uint8_t >(result.Error())};
            if (nrc == kInt8_0xFFU) {
                ManufacturerConfirmation(ConfirmationStatusType::kNoProcessingNoResponse, metaInfo);
            } else {
                request->Respond(nrc);
            }
            return false;
        }
        validatePassTable.push_back(manufacturer);
    }
    if (validatePassTable.empty()) {
        return true;
    }
    std::ignore = requestManufacturerConfirmTable_.insert({requestHandle, validatePassTable});
    return true;
}

bool ServiceValidationService::SupplierServiceValidate(std::shared_ptr< RequestContext > const& request) noexcept
{
    MessagePtr requestPtr{request->GetRequest()};
    std::vector< std::uint8_t > body{requestPtr->GetBody()};
    MetaInfoMap metaInfo{request->GetMetaInfoMap()};
    isoft::uds::Result< void > result;
    std::vector< ServiceValidationInterfacePtr > validatePassTable;
    std::string const requestHandle{metaInfo.at("kRequestHandle")};
    for (auto&& supplier : supplierTable_) {
        result = supplier->Validate(body, metaInfo);
        if (!result.HasValue()) {
            std::ignore = requestSupplierConfirmTable_.insert({requestHandle, validatePassTable});
            std::uint8_t const nrc{static_cast< std::uint8_t >(result.Error())};
            if (nrc == kInt8_0xFFU) {
                SupplierConfirmation(ConfirmationStatusType::kNoProcessingNoResponse, metaInfo);
                ManufacturerConfirmation(ConfirmationStatusType::kNoProcessingNoResponse, metaInfo);
            } else {
                request->Respond(nrc);
            }
            return false;
        }
        validatePassTable.push_back(supplier);
    }
    if (validatePassTable.empty()) {
        return true;
    }
    std::ignore = requestSupplierConfirmTable_.insert({requestHandle, validatePassTable});
    return true;
}


void ServiceValidationService::ManufacturerConfirmation(ConfirmationStatusType status, MetaInfoMap& metaInfo) noexcept
{
    isoft::uds::Result< void > result;
    std::string const requestHandle{metaInfo.at("kRequestHandle")};
    decltype(auto) findResult = requestManufacturerConfirmTable_.find(requestHandle);
    if (findResult != requestManufacturerConfirmTable_.end()) {
        std::vector< ServiceValidationInterfacePtr > table{requestManufacturerConfirmTable_.at(requestHandle)};

        for (auto it = table.rbegin(); it != table.rend(); it++) {
            result = (*it)->Confirmation(status, metaInfo);
        }
        std::ignore = requestManufacturerConfirmTable_.erase(requestHandle);
    }
}


void ServiceValidationService::SupplierConfirmation(ConfirmationStatusType status, MetaInfoMap& metaInfo) noexcept
{
    isoft::uds::Result< void > result;
    std::string const requestHandle{metaInfo.at("kRequestHandle")};
    decltype(auto) findResult = requestSupplierConfirmTable_.find(requestHandle);
    if (findResult != requestSupplierConfirmTable_.end()) {
        std::vector< ServiceValidationInterfacePtr > table{requestSupplierConfirmTable_.at(requestHandle)};
        for (auto it = table.rbegin(); it != table.rend(); it++) {
            result = (*it)->Confirmation(status, metaInfo);
        }
        std::ignore = requestSupplierConfirmTable_.erase(requestHandle);
    }
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
