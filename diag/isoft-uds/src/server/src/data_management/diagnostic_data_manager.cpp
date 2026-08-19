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
/// @file       diagnostic_data_manager.cpp
/// @brief      This file implements diagnostic_data_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=data_management
/// @trace_id_sr=52c4bd77-d69d-4f55-b41b-508f4e1ef4c6
/// @unit_name=DiagnosticDataManager
/// @unit_description=This file provides diagnostic_data_manager functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "diagnostic_data_manager.h"

#include <cstdint>

#include "common/log.h"
#include "common/serialize.h"
#include "isoft/uds/type_int.h"

namespace isoft {
namespace uds {
namespace server {

DiagnosticDataManager::DiagnosticDataManager(std::shared_ptr< PersistenceService > &persistenceService) noexcept
    : diagnosticDataModelTable_{}
    , diagnosticDynamicDataModelTable_{}
    , edrElementTable_{}
    , dynamicAvailableIdentifier_{}
    , persistenceService_{persistenceService}
    , dataInterfacePtr_{nullptr}
    , dataElementInterfacePtr_{nullptr}
    , storeDynamicDataModel_{false}
    , currentSessionId_{}
    , currentSecurityLevelId_{}
{
}

std::int32_t DiagnosticDataManager::Initialize(
    DiagnosticDataModelManager &modelManager,
    std::shared_ptr< SessionManager > &sessionManagerPtr,
    std::shared_ptr< SecurityLevelManager > &securityLevelManagerPtr) noexcept
{
    if (modelManager.staticData.empty()) {
        LogWarn() << "static data config is empty!Please add static data config";
        return -1;
    }
    NotifyOfSessionChangeCallback sessionChangeCallback{
        [this](SessionId, SessionId newSessionId) { currentSessionId_ = newSessionId; }};
    if (sessionManagerPtr.get() != nullptr) {
        std::ignore = sessionManagerPtr->AddNotifyOfSessionChange(sessionChangeCallback);
    }
    NotifyOfSecurityLevelChangeCallback securityLevelCallback{
        [this](isoft::uds::server::SecurityLevelId id) { currentSecurityLevelId_ = id; }};
    if (securityLevelManagerPtr.get() != nullptr) {
        std::ignore = securityLevelManagerPtr->AddNotifyOfSecurityLevelChange(securityLevelCallback);
    }
    for (auto &&didModel : modelManager.staticData) {
        bool isValidDataIdentifier{true};
        isValidDataIdentifier = _validateDataIdentifier(didModel.id);
        if (!isValidDataIdentifier) {
            return -1;
        }
        if (didModel.nSize == 0) {
            LogWarn() << "diagnostic data size must greater than zero! did:" << didModel.id;
            return -2;
        }
        
        for (auto &&paramterModel : didModel.details) {
            if (paramterModel.offset % sizeof(std::uint8_t) != 0U) {
                LogWarn() << "Did:" << didModel.id << " offset:" << paramterModel.offset
                          << "value of DiagnosticDataParamterModel.bitOffset shall only be set to a multiple of 8.";
                return -3;
            }
            if (paramterModel.element.dataType < DataType::kInt8 || paramterModel.element.dataType > DataType::kFloat) {
                LogWarn() << "Did:" << didModel.id << " offset:" << paramterModel.offset
                          << "element data type not support!Please verify DataType enum type";
                //return -4;
            }
            if (paramterModel.element.array.get() == nullptr) {
                LogInfo() << "Did:" << didModel.id << " offset:" << paramterModel.offset << "element.array is nullptr";
            } else {
                if (paramterModel.element.array->maxNumberOfElements <= 0) {
                    LogWarn() << "Did:" << didModel.id << " offset:" << paramterModel.offset
                              << " element.array.maxNumberOfElements exists then its value shall be greater than 0.";
                    return -kInt32_5;
                }
            }
            if (didModel.readType == DiagnosticDataReadMethod::kUseReadMothod
                && paramterModel.element.provider != DiagnosticDataElementProvider::kUnused) {
                LogWarn() << "Did:" << didModel.id << " offset:" << paramterModel.offset
                          << " readType:" << static_cast< std::uint8_t >(didModel.readType)
                          << " element.provider must DiagnosticDataElementProvider::kUnused";
                return -kInt32_8;
            }

            
        }
        auto findDidResult = diagnosticDataModelTable_.find(didModel.id);
        if (findDidResult != diagnosticDataModelTable_.end()) {
            LogWarn() << "did:" << didModel.id << " is exists!Please check config!";
            return -kInt32_6;
        }
        std::ignore = diagnosticDataModelTable_.insert({didModel.id, didModel});
    }
    for (auto &&dynamicId : modelManager.dynamicData) {
        std::uint16_t const minDynamicId{kMinPeriodicDataIdentifier};
        std::uint16_t const maxDynamicId{kMaxDynamicallyDefinedDataIdentifier};
        if (dynamicId < minDynamicId || dynamicId > maxDynamicId) {
            LogWarn() << "invalid value of dynamic id:" << dynamicId;
            return -kInt32_6;
        }
    }

    for (auto &&edrElement : modelManager.edrElement) {
        std::ignore = edrElementTable_.insert({edrElement.element.id, edrElement});
    }

    for (auto &&dynamicId : modelManager.dynamicData) {
        std::ignore = dynamicAvailableIdentifier_.insert(dynamicId);
    }
    dataInterfacePtr_        = modelManager.dataInterfacePtr;
    dataElementInterfacePtr_ = modelManager.dataElementInterfacePtr;
    storeDynamicDataModel_   = modelManager.storeDynamicDataModel;
    if (modelManager.storeDynamicDataModel) {
        _loadStoreDynamicModel();
    }
    return 0;
}

Result< DiagnosticDataElementModel > DiagnosticDataManager::GetDiagnosticDataElementModel(std::uint32_t id) noexcept
{
    std::int32_t const error{-1};
    for (auto &&diagnosticDataModel : diagnosticDataModelTable_) {
        auto findResult
            = std::find_if(diagnosticDataModel.second.details.begin(), diagnosticDataModel.second.details.end(),
                           [&id](isoft::uds::server::DiagnosticDataParamterModel const &input) {
                               if (input.element.id == id) {
                                   return true;
                               }
                               return false;
                           });
        if (findResult != diagnosticDataModel.second.details.end()) {
            return Result< DiagnosticDataElementModel >::FromValue(findResult->element);
        }
    }
    auto edrElementResult = edrElementTable_.find(id);
    if (edrElementResult != edrElementTable_.end()) {
        return Result< DiagnosticDataElementModel >::FromValue(edrElementResult->second.element);
    }
    return Result< DiagnosticDataElementModel >(error);
}

bool DiagnosticDataManager::HasDataIdentifier(std::uint16_t id) noexcept
{
    auto findDiagnosticDataResult = diagnosticDataModelTable_.find(id);
    if (findDiagnosticDataResult != diagnosticDataModelTable_.end()) {
        LogInfo() << "DiagnosticDataManager::HasDataIdentifier| find  data id:" << id << " success!";
        return true;
    }
    auto findDiagnosticDynamicDataResult = dynamicAvailableIdentifier_.find(id);
    if (findDiagnosticDynamicDataResult != dynamicAvailableIdentifier_.end()) {
        LogInfo() << "DiagnosticDataManager::HasDataIdentifier| find  data id:" << id << " success!";
        return true;
    }
    LogWarn() << "DiagnosticDataManager::HasDataIdentifier| find data id:" << id << " error!";
    return false;
}

Result< std::list< DiagnosticData > > DiagnosticDataManager::Read(
    std::vector< std::uint16_t > &dataIdentifierTable,
    MetaInfoMap const &meataInfo,
    std::shared_ptr< CancellationState > const &cancellationState) noexcept
{
    std::vector< std::uint16_t > supportDataIdentifierTable;
    std::vector< std::uint16_t > supportDynamicDataIdentifierTable;
    std::vector< std::uint16_t > supportDataElementTable;
    for (auto &&dataIdentifier : dataIdentifierTable) {
        if (IsDynamic(dataIdentifier)) {
            supportDynamicDataIdentifierTable.push_back(dataIdentifier);
            continue;
        }
        auto findDataIdentifierModelResult = diagnosticDataModelTable_.find(dataIdentifier);
        if (findDataIdentifierModelResult == diagnosticDataModelTable_.end()) {
            continue;
        }
        if (findDataIdentifierModelResult->second.readType == DiagnosticDataReadMethod::kUseReadMothod) {
            supportDataIdentifierTable.push_back(dataIdentifier);
        } else if (findDataIdentifierModelResult->second.readType == DiagnosticDataReadMethod::kUseReadElementMothod) {
            supportDataElementTable.push_back(dataIdentifier);
        }
    }
    std::list< DiagnosticData > readDataTable;
    std::int32_t err{0};
    if (!supportDataIdentifierTable.empty()) {
        Result< std::list< DiagnosticData > > readDataIdentifierByInterfaceResult;
        LogInfo() << "DiagnosticDataManager::Read| call Read method!";
        readDataIdentifierByInterfaceResult
            = dataInterfacePtr_->Read(dataIdentifierTable, meataInfo, CancellationHandler(cancellationState));
        if (readDataIdentifierByInterfaceResult.HasValue()) {
            readDataTable = readDataIdentifierByInterfaceResult.Value();
        } else {
            err         = readDataIdentifierByInterfaceResult.Error();
            std::ignore = err;
        }
    }
    if (!supportDynamicDataIdentifierTable.empty()) {
        Result< std::list< DiagnosticData > > readDynamicDataIdentifierResult;
        readDynamicDataIdentifierResult = _readDynamic(supportDynamicDataIdentifierTable, meataInfo, cancellationState);
        if (readDynamicDataIdentifierResult.HasValue()) {
            std::list< DiagnosticData > readDataIdentifierTable{readDynamicDataIdentifierResult.Value()};
            std::ignore = readDataTable.insert(readDataTable.cend(), readDataIdentifierTable.cbegin(),
                                               readDataIdentifierTable.cend());
        } else {
            err = readDynamicDataIdentifierResult.Error();
        }
    }
    if (!supportDataElementTable.empty()) {
        Result< std::list< DiagnosticData > > readDataIdentifierByElementResult;
        readDataIdentifierByElementResult
            = _readByElement(dataIdentifierTable, meataInfo, CancellationHandler(cancellationState));
        if (readDataIdentifierByElementResult.HasValue()) {
            std::list< DiagnosticData > readDataIdentifierByElementTable{readDataIdentifierByElementResult.Value()};
            std::ignore = readDataTable.insert(readDataTable.cend(), readDataIdentifierByElementTable.cbegin(),
                                               readDataIdentifierByElementTable.cend());
        } else {
            err = readDataIdentifierByElementResult.Error();
        }
    }
    Result< std::list< DiagnosticData > > result;
    if (err != 0) {
        result.ErrorOr(err);
    } else {
        result.ValueOr(readDataTable);
    }
    return result;
}

Result< DiagnosticDataElement > DiagnosticDataManager::ReadDataElement(
    std::uint32_t id,
    MetaInfoMap const &meataInfo,
    std::shared_ptr< CancellationState > const &cancellationState) noexcept
{
    Result< DiagnosticDataElement > result;
    std::vector< std::uint32_t > idTable;
    idTable.push_back(id);
    auto readResult = ReadDataElement(idTable, meataInfo, cancellationState);
    if (readResult.HasValue() && !readResult.Value().empty()) {
        result.ValueOr(readResult.Value().front());
    } else {
        std::int32_t err = readResult.Error();
        if (err == 0) {
            LogInfo() << "DiagnosticDataManager::ReadDataElement| read element is zero!";
            err = static_cast< std::int32_t >(NrcErrc::kGeneralReject);
        }
        result.ErrorOr(err);
    }
    return result;
}
Result< std::list< DiagnosticDataElement > > DiagnosticDataManager::ReadDataElement(
    std::vector< std::uint32_t > const &idTable,
    MetaInfoMap const &meataInfo,
    std::shared_ptr< CancellationState > const &cancellationState) noexcept
{
    std::vector< std::uint32_t > externalTable;
    std::list< DiagnosticDataElement > interanlElementTable;
    for (auto &&id : idTable) {
        isoft::uds::Result< isoft::uds::server::DiagnosticDataElementModel > findElement
            = GetDiagnosticDataElementModel(id);
        if (!findElement.HasValue()) {
            LogInfo() << "DiagnosticDataManager::ReadDataElement|not find element. id:" << id;
            continue;
        }
        if (findElement.Value().provider == DiagnosticDataElementProvider::kExternal) {
            externalTable.push_back(id);    
        } else if (findElement.Value().provider == DiagnosticDataElementProvider::kDcmSession) {
            DiagnosticDataElement element;
            element.id = id;
            element.record.push_back(currentSessionId_);
            interanlElementTable.push_back(element);
        } else if (findElement.Value().provider == DiagnosticDataElementProvider::kDcmSession) {
            DiagnosticDataElement element;
            element.id = id;
            element.record.push_back(currentSecurityLevelId_);
            interanlElementTable.push_back(element);
        }
    }
    Result< std::list< DiagnosticDataElement > > readExternalElementResult;
    readExternalElementResult
        = dataElementInterfacePtr_->Read(externalTable, meataInfo, CancellationHandler(cancellationState));
    if (readExternalElementResult.HasValue()) {
        if (!interanlElementTable.empty() && !readExternalElementResult.Value().empty()) {
            std::ignore = readExternalElementResult.Value().insert(
                readExternalElementResult.Value().cend(), interanlElementTable.cbegin(), interanlElementTable.cend());
        }
    } else {
        if (!interanlElementTable.empty()) {
            readExternalElementResult.ValueOr(interanlElementTable);
        }
    }
    return readExternalElementResult;
}

Result< std::list< DiagnosticDataElement > > DiagnosticDataManager::ReadEdrElement(
    std::vector< std::uint32_t > const &idTable,
    MetaInfoMap const &meataInfo,
    std::shared_ptr< CancellationState > const &cancellationState) noexcept
{
    std::vector< std::uint32_t > externalTable;
    for (auto &&id : idTable) {
        auto findEdrElement = edrElementTable_.find(id);
        if (findEdrElement == edrElementTable_.end()) {
            continue;
        }
        if (findEdrElement->second.element.provider == DiagnosticDataElementProvider::kExternal) {
            externalTable.push_back(id);
        }
    }
    return dataElementInterfacePtr_->Read(externalTable, meataInfo, CancellationHandler(cancellationState));
}

Result< void > DiagnosticDataManager::Write(DiagnosticData const &data,
                                            MetaInfoMap const &meataInfo,
                                            std::shared_ptr< CancellationState > const &cancellationState) noexcept
{
    return dataInterfacePtr_->Write(data, meataInfo, CancellationHandler(cancellationState));
}

std::uint8_t DiagnosticDataManager::Defined(DiagnosticDynamicDataModel &data, bool store) noexcept
{
    for (auto &&sourceDataModel : data.record) {
        std::size_t sourceDataSize{static_cast< std::size_t >(sourceDataModel.nPos + sourceDataModel.nSize)};
        auto findSourceDataModel = diagnosticDataModelTable_.find(sourceDataModel.id);
        if (findSourceDataModel == diagnosticDataModelTable_.end()) {
            LogWarn() << "DiagnosticDataManager::Defined| not find did:" << sourceDataModel.id;
            return -1;
        }
        if (findSourceDataModel->second.nSize < sourceDataSize) {
            LogWarn() << "DiagnosticDataManager::Defined| did:" << sourceDataModel.id << " length short!";
            return -1;
        }
    }

    for (auto &&id : dynamicAvailableIdentifier_) {
        if (data.id == id) {
            auto findDynamicModel = diagnosticDynamicDataModelTable_.find(id);
            if (findDynamicModel == diagnosticDynamicDataModelTable_.end()) {
                std::ignore = diagnosticDynamicDataModelTable_.insert({id, data});
            } else {
                std::ignore = findDynamicModel->second.record.insert(findDynamicModel->second.record.cend(),
                                                                     data.record.cbegin(), data.record.cend());
            }
            if (store) {
                _saveStoreDynamicModel();
            }
            return 0;
        }
    }
    LogWarn() << "DiagnosticDataManager::Defined| not found dynamic identifier in dynamic identifier list! dynamic "
                 "identifier:"
              << data.id;
    return -1;
}

std::size_t DiagnosticDataManager::GetCountOfDiagnosticDynamicDataModelSource(std::uint16_t did) noexcept
{
    auto findDynamicModel = diagnosticDynamicDataModelTable_.find(did);
    if (findDynamicModel == diagnosticDynamicDataModelTable_.end()) {
        return 0;
    }
    return findDynamicModel->second.record.size();
}

std::uint8_t DiagnosticDataManager::ClearDefined(std::uint16_t did) noexcept
{
    auto findDynamicModel = diagnosticDynamicDataModelTable_.find(did);
    if (findDynamicModel == diagnosticDynamicDataModelTable_.end()) {
        return -1;
    }
    std::ignore = diagnosticDynamicDataModelTable_.erase(findDynamicModel);
    _clearStoreDynamicModel(did);
    _saveStoreDynamicModel();
    return 0;
}

std::uint8_t DiagnosticDataManager::ClearAllDefined() noexcept
{
    for (auto &&diagnosticDynamicDataModel : diagnosticDynamicDataModelTable_) {
        _clearStoreDynamicModel(diagnosticDynamicDataModel.first);
    }

    diagnosticDynamicDataModelTable_.clear();
    _saveStoreDynamicModel();
    return 0;
}

std::size_t DiagnosticDataManager::GetDataSize(std::uint16_t did) noexcept
{
    auto findModel = diagnosticDataModelTable_.find(did);
    if (findModel == diagnosticDataModelTable_.end()) {
        auto findDynamicModel = diagnosticDynamicDataModelTable_.find(did);
        if (findDynamicModel == diagnosticDynamicDataModelTable_.end()) {
            LogWarn() << "DiagnosticDataManager::GetDataSize| get data size fail! did:" << did;
            return 0;
        }
        std::size_t dataSize{0};
        for (auto &&sourceModel : findDynamicModel->second.record) {
            dataSize += sourceModel.nSize;
        }
        return dataSize;
    }
    return findModel->second.nSize;
}

std::vector< std::reference_wrapper< DiagnosticDynamicDataModel > >
DiagnosticDataManager::GetAllDynamicDataModel() noexcept
{
    std::vector< std::reference_wrapper< DiagnosticDynamicDataModel > > diagnosticDynamicDataModelTable;
    for (auto &&diagnosticDynamicDataModel : diagnosticDynamicDataModelTable_) {
        diagnosticDynamicDataModelTable.push_back(std::ref(diagnosticDynamicDataModel.second));
    }
    return diagnosticDynamicDataModelTable;
}

bool DiagnosticDataManager::IsDynamic(std::uint16_t id) noexcept
{
    for (auto &&dynamicId : dynamicAvailableIdentifier_) {
        if (dynamicId == id) {
            return true;
        }
    }
    return false;
}

void DiagnosticDataManager::_loadStoreDynamicModel()
{
    std::string dynamicIdTable{"dynamicIdTable"};
    std::vector< std::uint8_t > rawData;
    rawData = persistenceService_->LoadData(dynamicIdTable);
    Serialize deserialize{rawData};
    std::uint16_t dynamicId{0U};
    while (deserialize.Read(dynamicId)) {
        DiagnosticDynamicDataModel dynamicDataModel;
        if (IsDynamic(dynamicId)) {
            std::vector< std::uint8_t > rawModelData;
            std::string dynamicModelKey{"dynamicModel_"};
            std::ignore         = dynamicModelKey.append(std::to_string(static_cast< unsigned int >(dynamicId)));
            rawModelData        = persistenceService_->LoadData(dynamicModelKey);
            dynamicDataModel.id = dynamicId;
            Serialize deserializeDyanmicModel{rawModelData};
            SourceData sourceData{};
            while (deserializeDyanmicModel.Read(sourceData.id)) {
                bool readResult{false};
                readResult = deserializeDyanmicModel.Read(sourceData.nPos);
                if (!readResult) {
                    LogWarn() << "DiagnosticDataManager::LoadDynamicModel| read SourceData.nPos fails!";
                    return;
                }
                readResult = deserializeDyanmicModel.Read(sourceData.nSize);
                if (!readResult) {
                    LogWarn() << "DiagnosticDataManager::LoadDynamicModel| read SourceData.nSize fails!";
                    return;
                }
                dynamicDataModel.record.push_back(sourceData);
            }
        }
        std::ignore = diagnosticDynamicDataModelTable_.insert({dynamicId, dynamicDataModel});
    }
}

void DiagnosticDataManager::_saveStoreDynamicModel()
{
    std::string dynamicIdTable{"dynamicIdTable"};
    std::vector< std::uint8_t > rawData;
    Serialize serialize{rawData};
    for (auto &&diagnosticDynamicDataModel : diagnosticDynamicDataModelTable_) {
        serialize.Write(diagnosticDynamicDataModel.first);
        std::vector< std::uint8_t > rawModelData;
        std::string dynamicModelKey{"dynamicModel_"};
        std::ignore
            = dynamicModelKey.append(std::to_string(static_cast< unsigned int >(diagnosticDynamicDataModel.first)));
        Serialize serializeDyanmicModel{rawModelData};
        for (auto &&sourceData : diagnosticDynamicDataModel.second.record) {
            serializeDyanmicModel.Write(sourceData.id);
            serializeDyanmicModel.Write(sourceData.nPos);
            serializeDyanmicModel.Write(sourceData.nSize);
        }
        LogVerbose() << "DiagnosticDataManager::_saveStoreDynamicModel| ddid:" << diagnosticDynamicDataModel.first;
        std::ignore = persistenceService_->SaveData(dynamicModelKey, rawModelData);
    }

    std::ignore = persistenceService_->SaveData(dynamicIdTable, rawData);
}

void DiagnosticDataManager::_clearStoreDynamicModel(std::uint16_t did)
{
    LogVerbose() << "DiagnosticDataManager::_clearStoreDynamicModel| ddid:" << did;
    std::string dynamicModelKey{"dynamicModel_"};
    std::ignore = dynamicModelKey.append(std::to_string(static_cast< unsigned int >(did)));
    std::ignore = persistenceService_->RemoveData(dynamicModelKey);
}

Result< std::list< DiagnosticData > > DiagnosticDataManager::_readByElement(
    std::vector< std::uint16_t > &dataIdentifierTable,
    MetaInfoMap const &meataInfo,
    CancellationHandler cancellationHandler) noexcept
{
    std::vector< std::uint32_t > externalElementTable;
    for (auto &&dataIdentifier : dataIdentifierTable) {
        auto findDataIdentifierModelResult = diagnosticDataModelTable_.find(dataIdentifier);
        if (findDataIdentifierModelResult == diagnosticDataModelTable_.end()) {
            continue;
        }
        if (findDataIdentifierModelResult->second.readType == DiagnosticDataReadMethod::kUseReadElementMothod) {
            for (auto &&paramterModel : findDataIdentifierModelResult->second.details) {
                if (paramterModel.element.provider == DiagnosticDataElementProvider::kExternal) {
                    externalElementTable.push_back(paramterModel.element.id);
                }
            }
        }
    }
    Result< std::list< DiagnosticDataElement > > readElementResult
        = dataElementInterfacePtr_->Read(externalElementTable, meataInfo, std::move(cancellationHandler));
    if (readElementResult.HasValue()) {
        std::list< DiagnosticData > validDiagnosticDataTable;
        std::list< DiagnosticDataElement > elementList{readElementResult.Value()};
        for (auto &&dataIdentifier : dataIdentifierTable) {
            auto findDataIdentifierModelResult = diagnosticDataModelTable_.find(dataIdentifier);
            if (findDataIdentifierModelResult == diagnosticDataModelTable_.end()) {
                continue;
            }
            if (findDataIdentifierModelResult->second.readType == DiagnosticDataReadMethod::kUseReadElementMothod) {
                DiagnosticData diagnosticData;
                diagnosticData.id = dataIdentifier;
                diagnosticData.record.reserve(findDataIdentifierModelResult->second.nSize);
                for (auto &&paramterModel : findDataIdentifierModelResult->second.details) {
                    if (paramterModel.element.provider == DiagnosticDataElementProvider::kExternal) {
                        for (auto &&element : elementList) {
                            if (element.id == paramterModel.element.id) {
                                std::ignore = diagnosticData.record.insert(
                                    diagnosticData.record.cbegin()
                                        + static_cast< int32_t >(paramterModel.offset / sizeof(std::uint8_t)),
                                    element.record.cbegin(), element.record.cend());
                            }
                        }
                    } else if (paramterModel.element.provider == DiagnosticDataElementProvider::kDcmSession) {
                        std::ignore = diagnosticData.record.insert(
                            diagnosticData.record.cbegin()
                                + static_cast< int32_t >(paramterModel.offset / sizeof(std::uint8_t)),
                            currentSessionId_);
                    } else if (paramterModel.element.provider == DiagnosticDataElementProvider::kDcmSecurityLevel) {
                        std::ignore = diagnosticData.record.insert(
                            diagnosticData.record.cbegin()
                                + static_cast< int32_t >(paramterModel.offset / sizeof(std::uint8_t)),
                            currentSecurityLevelId_);
                    }
                }
                validDiagnosticDataTable.push_back(diagnosticData);
            }
        }
        if (validDiagnosticDataTable.empty()) {
            return Result< std::list< DiagnosticData > >(-1);
        }
        return Result< std::list< DiagnosticData > >(validDiagnosticDataTable);
    }
    return Result< std::list< DiagnosticData > >(readElementResult.Error());
}

bool DiagnosticDataManager::_isInternalElement(std::uint16_t id, std::uint32_t offset) noexcept
{
    auto findDataModelResult = diagnosticDataModelTable_.find(id);
    if (findDataModelResult != diagnosticDataModelTable_.end()) {
        for (auto &&paramterModel : findDataModelResult->second.details) {
            if (paramterModel.offset == offset) {
                if (paramterModel.element.provider == DiagnosticDataElementProvider::kDcmSession
                    || paramterModel.element.provider == DiagnosticDataElementProvider::kDcmSecurityLevel) {
                    return true;
                }
            }
        }
    }
    return false;
}
Result< std::list< DiagnosticData > > DiagnosticDataManager::_readDynamic(
    std::vector< std::uint16_t > &dataIdentifierTable,
    MetaInfoMap const &meataInfo,
    std::shared_ptr< CancellationState > const &cancellationState) noexcept
{
    std::list< DiagnosticData > diagnosticDataList;
    std::int32_t err{0};
    for (auto &&dataIdentifier : dataIdentifierTable) {
        DiagnosticData data{};
        data.id         = dataIdentifier;
        auto findResult = diagnosticDynamicDataModelTable_.find(dataIdentifier);
        if (findResult == diagnosticDynamicDataModelTable_.end()) {
            diagnosticDataList.push_back(data);
            continue;
        }
        DiagnosticDynamicDataModel &dynamicDataModel{findResult->second};
        for (auto &&sourceDataModel : dynamicDataModel.record) {
            std::vector< std::uint16_t > sourceDataIdentifierTable;
            sourceDataIdentifierTable.push_back(sourceDataModel.id);
            Result< std::list< DiagnosticData > > sourceDataIdentifierTableResult;
            sourceDataIdentifierTableResult = Read(sourceDataIdentifierTable, meataInfo, cancellationState);
            if (sourceDataIdentifierTableResult.HasValue()) {
                DiagnosticData &sourceData{sourceDataIdentifierTableResult.Value().front()};
                data.record.insert(data.record.end(), sourceData.record.begin() + sourceDataModel.nPos,
                                   sourceData.record.begin() + sourceDataModel.nPos + sourceDataModel.nSize);
            } else {
                err = sourceDataIdentifierTableResult.Error();
                LogWarn() << "DiagnosticDataManager::_readDynamic| read diagnostic data fails! error:" << err;
            }
        }
        diagnosticDataList.push_back(data);
    }
    return Result< std::list< DiagnosticData > >(diagnosticDataList);
}

bool DiagnosticDataManager::_validateDataIdentifier(std::uint16_t id) noexcept
{

    std::ignore = id;
    return true;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft