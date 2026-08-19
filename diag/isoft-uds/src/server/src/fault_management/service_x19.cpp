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
/// @file       service_x19.cpp
/// @brief      This file implements service_x19 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=225207ed-d7c2-4e18-8249-5e4f4df423ed
/// @unit_name=service_x19.h
/// @unit_description=This file provides service_x19 functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/service_x19.h"

#include <functional>

#include "common/log.h"
#include "common/serialize.h"
#include "fault_management/dtc_filter.h"
#include "fault_management/types.h"
#include "isoft/uds/fault_management/dtc_information_interface.h"
#include "isoft/uds/fault_management/fault_management.h"
#include "isoft/uds/type_int.h"

namespace isoft {
namespace uds {
namespace server {
namespace service_x19 {
std::uint8_t const kMinRequestSize{2U};


std::uint32_t GetDtc(std::vector< std::uint8_t >& data) noexcept
{
    std::uint8_t const dtcSize{3U};
    std::uint8_t const begin{2U};
    std::uint32_t value{0U};
    for (size_t i{begin}; (i < (begin + dtcSize)) && (i < data.size()); i++) {
        value = (value << CHAR_BIT) | static_cast< std::uint32_t >(data[i]);
    }
    return value;
}




inline void Push(std::vector< std::uint8_t >& data, std::uint16_t const& input)
{
    std::uint8_t highByte{0U};
    highByte = static_cast< std::uint8_t >(input >> CHAR_BIT);
    std::uint8_t lowByte{0U};
    lowByte = static_cast< std::uint8_t >(input);
    data.emplace_back(highByte);
    data.emplace_back(lowByte);
}




inline void PushDtc(std::vector< std::uint8_t >& data, std::uint32_t const& input)
{
    std::uint8_t offsetByte{0U};
    std::int8_t const offset{kInt32_16};
    offsetByte = static_cast< std::uint8_t >(input >> offset);
    std::uint8_t highByte{0U};
    highByte = static_cast< std::uint8_t >(input >> CHAR_BIT);
    std::uint8_t lowByte{0U};
    lowByte = static_cast< std::uint8_t >(input);
    data.emplace_back(offsetByte);
    data.emplace_back(highByte);
    data.emplace_back(lowByte);
}
}  // namespace service_x19

std::int32_t ServiceX19::Initialize(ReadDTCInformationInstanceConfig& config,
                                    std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                                    std::shared_ptr< SessionManager > const& sessionManagerPtr,
                                    std::shared_ptr< SecurityLevelManager > const& securityLevelManagerPtr) noexcept
{
    if (config.instances.empty()) {
        LogWarn() << "ServiceX19::Initialize|config is empty!";
        return -1;
    }

    if (sessionManagerPtr.get() == nullptr) {
        LogWarn() << "ServiceX19::Initialize|sessionManagerPtr is nullptr!";
        return -1;
    }

    if (securityLevelManagerPtr.get() == nullptr) {
        LogWarn() << "ServiceX19::Initialize| securityLevelManagerPtr is nullptr!";
        return -1;
    }
    sessionValidator_       = std::make_shared< ServerLevelSessionValidator< std::uint8_t > >();
    securityLevelValidator_ = std::make_shared< ServerLevelSecurityLevelValidator< std::uint8_t > >();
    for (auto const& instanceConfig : config.instances) {
        std::shared_ptr< Instance > instancePtr = std::make_shared< Instance >();
        if (instancePtr.get() == nullptr) {
            LogWarn() << "ServiceX19::Initialize|allow memory fails in new struct Instance";
            return -1;
        }

        instancePtr->subfunctionNumber = static_cast< std::uint8_t >(instanceConfig.subfunctionNumber);
        std::int32_t result{0};
        if (instanceConfig.accessPermissionAuth.enable) {
            std::shared_ptr< SubfunctionLevelAuthValidator< std::uint8_t > > authValidator
                = std::make_shared< SubfunctionLevelAuthValidator< std::uint8_t > >();
            if (authValidator.get() == nullptr) {
                LogWarn() << "allow memory fails in new SubfunctionLevelAuthValidator instance";
                return -1;
            }
            result = static_cast< int32_t >(
                authValidator->Initialize(static_cast< uint8_t >(instanceConfig.subfunctionNumber),
                                          instanceConfig.accessPermissionAuth.authRoleList, authManagerPtr));
            if (result != 0) {
                LogWarn() << "auth config initialize fails!";
                return -1;
            }
            if (authValidator_.get() == nullptr) {
                authValidator_ = std::make_shared< ServerLevelAuthValidator< std::uint8_t > >();
            }
            result = authValidator_->RegisterSubfunctionLevel(static_cast< uint8_t >(instanceConfig.subfunctionNumber),
                                                              authValidator);
            if (result != 0) {
                LogWarn() << "auth id repeatability! Please check ReadDTCInformationSubfunction.accessPermissionAuth";
                return -1;
            }
            instancePtr->authValidator = authValidator;
        }
        instancePtr->sessionValidator = std::make_shared< SubfunctionLevelSessionValidator< std::uint8_t > >();
        if (instancePtr->sessionValidator.get() == nullptr) {
            LogWarn() << "ServiceX19::Initialize|allow memory fails in new SubfunctionLevelSessionValidator instance";
            return -1;
        }

        result = static_cast< int32_t >(
            instancePtr->sessionValidator->Initialize(static_cast< uint8_t >(instanceConfig.subfunctionNumber),
                                                      instanceConfig.accessPermissionSession, sessionManagerPtr));
        if (result != 0) {
            LogWarn() << "ServiceX19::Initialize|session config initialize fails result =" << result;
            return -1;
        }

        result = sessionValidator_->RegisterSubfunctionLevel(static_cast< uint8_t >(instanceConfig.subfunctionNumber),
                                                             instancePtr->sessionValidator);
        if (result != 0) {
            LogWarn() << "ServiceX19::Initialize|session id repeatability! Please check "
                         "ControlDTCSettingInstanceConfig.accessPermissionSession result ="
                      << result;
            return -1;
        }

        instancePtr->securityLevelValidator
            = std::make_shared< SubfunctionLevelSecurityLevelValidator< std::uint8_t > >();
        if (instancePtr->securityLevelValidator.get() == nullptr) {
            LogWarn() << "allow memory fails in new SubfunctionLevelSecurityLevelValidator instance";
            return -1;
        }
        result = static_cast< int32_t >(instancePtr->securityLevelValidator->Initialize(
            static_cast< uint8_t >(instanceConfig.subfunctionNumber), instanceConfig.accessPermissionSecurityLevel,
            securityLevelManagerPtr));
        if (result != 0) {
            LogWarn() << "ServiceX19::Initialize|security level config initialize fails!";
            return -1;
        }

        result = securityLevelValidator_->RegisterSubfunctionLevel(
            static_cast< uint8_t >(instanceConfig.subfunctionNumber), instancePtr->securityLevelValidator);
        if (result != 0) {
            LogWarn() << "ServiceX19::Initialize|security level id repeatability! Please check "
                         "ReadDiagnosticDataByIdentifier.accessPermissionSecurityLevel";
            return -1;
        }

        auto findInstanceResult = instanceTable_.find(static_cast< uint8_t >(instanceConfig.subfunctionNumber));
        if (findInstanceResult != instanceTable_.end()) {
            LogWarn() << "ServiceX19::Initialize|ControlDTCSettingInstanceConfig.subfunctionNumber:"
                      << static_cast< uint8_t >(instanceConfig.subfunctionNumber) << " alreay exist";
            return -1;
        }
        std::ignore = instanceTable_.insert({static_cast< uint8_t >(instanceConfig.subfunctionNumber), instancePtr});
    }
    p4ServerMax_ = config.p4ServerMax;
    LogVerbose() << "ServiceX19::Initialize|success";
    return 0;
}


static std::uint8_t GetSubfunction(std::shared_ptr< RequestContext >& context)
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX19::GetSubfunction|context us nullptr";
        return 0U;
    }
    decltype(auto) requestBody = context->GetRequest()->GetBody();
    if (requestBody.size() < 2U) {
        LogWarn() << "ServiceX19::GetSubfunction|requestBody size error";
        return 0U;
    }
    std::uint8_t const subfunctionMask{0x7FU};
    return static_cast< std::uint8_t >(requestBody[1U] & subfunctionMask);
}

static std::shared_ptr< Message > CreateRespondMessage(std::shared_ptr< RequestContext >& context,
                                                       std::uint8_t const& dtcStatusAvailabilityMask,
                                                       std::uint8_t const& dtcFormatIdentifier,
                                                       std::uint16_t const& dtcCount) noexcept
{
    std::shared_ptr< Message > response{context->CreateResponse()};
    decltype(auto) body{response->GetBody()};
    body.emplace_back(GetSubfunction(context));
    body.emplace_back(dtcStatusAvailabilityMask);
    body.emplace_back(dtcFormatIdentifier);
    service_x19::Push(body, dtcCount);
    return response;
}

static std::shared_ptr< Message > CreateRespondMessage(
    std::shared_ptr< RequestContext >& context,
    std::uint8_t const dtcStatusAvailabilityMask,
    std::vector< DTCAndStatusRecord > const& dtcAndStatusRecord) noexcept
{
    std::shared_ptr< Message > response{context->CreateResponse()};
    decltype(auto) body{response->GetBody()};
    body.emplace_back(GetSubfunction(context));
    body.emplace_back(dtcStatusAvailabilityMask);
    for (DTCAndStatusRecord const each : dtcAndStatusRecord) {
        service_x19::PushDtc(body, each.dtcValue);
        body.emplace_back(each.status);
    }
    return response;
}

static std::shared_ptr< Message > CreateRespondMessage(
    std::shared_ptr< RequestContext >& context,
    DTCAndStatusRecord const& dtcAndStatusRecord,
    std::vector< SnapshotDataRecordType > const& snapshotDataRecord) noexcept
{
    std::shared_ptr< Message > response{context->CreateResponse()};
    decltype(auto) body{response->GetBody()};
    body.emplace_back(GetSubfunction(context));
    service_x19::PushDtc(body, dtcAndStatusRecord.dtcValue);
    body.emplace_back(dtcAndStatusRecord.status);
    for (decltype(auto) record : snapshotDataRecord) {
        body.emplace_back(record.snapshotRecordNumber);
        body.emplace_back(static_cast< std::uint8_t >(record.snapshotDataIdentifiers.size()));
        for (decltype(auto) sr : record.snapshotDataIdentifiers) {
            service_x19::Push(body, sr.dataIdentifier);
            std::ignore = body.insert(body.cend(), sr.data.begin(), sr.data.end());
        }
    }
    return response;
}

static std::shared_ptr< Message > CreateRespondMessage(std::shared_ptr< RequestContext >& context,
                                                       DTCAndStatusRecord const& dtcAndStatusRecord,
                                                       std::vector< DTCExtDataRecord > const& dtcExtDataRecord) noexcept
{
    std::shared_ptr< Message > response{context->CreateResponse()};
    decltype(auto) body{response->GetBody()};
    body.emplace_back(GetSubfunction(context));
    service_x19::PushDtc(body, dtcAndStatusRecord.dtcValue);
    body.emplace_back(dtcAndStatusRecord.status);
    for (decltype(auto) record : dtcExtDataRecord) {
        body.emplace_back(record.recordNumber);
        std::ignore = body.insert(body.cend(), record.data.begin(), record.data.end());
    }
    return response;
}

static std::shared_ptr< Message > CreateRespondMessage(
    std::shared_ptr< RequestContext >& context,
    std::vector< DTCFaultDetectionCounterRecord > const& dtcFaultDetectionCounterRecord) noexcept
{
    std::shared_ptr< Message > response{context->CreateResponse()};
    decltype(auto) body{response->GetBody()};
    body.emplace_back(GetSubfunction(context));
    for (decltype(auto) record : dtcFaultDetectionCounterRecord) {
        service_x19::PushDtc(body, record.dtcValue);
        body.emplace_back(static_cast< std::uint8_t >(record.fdc));
    }
    return response;
}

static std::shared_ptr< Message > CreateRespondMessage(
    std::shared_ptr< RequestContext >& context,
    std::uint8_t const memorySelection,
    std::uint8_t const dtcStatusAvailabilityMask,
    std::vector< DTCAndStatusRecord > const& dtcAndStatusRecord) noexcept
{
    std::shared_ptr< Message > response{context->CreateResponse()};
    decltype(auto) body{response->GetBody()};
    body.emplace_back(GetSubfunction(context));
    body.emplace_back(memorySelection);
    body.emplace_back(dtcStatusAvailabilityMask);
    for (decltype(auto) each : dtcAndStatusRecord) {
        service_x19::PushDtc(body, each.dtcValue);
        body.emplace_back(each.status);
    }
    return response;
}

static std::shared_ptr< Message > CreateRespondMessage(
    std::shared_ptr< RequestContext >& context,
    std::uint8_t const memorySelection,
    DTCAndStatusRecord const& dtcAndStatusRecord,
    std::vector< SnapshotDataRecordType > const& snapshotDataRecord) noexcept
{
    std::shared_ptr< Message > response{context->CreateResponse()};
    decltype(auto) body{response->GetBody()};
    body.emplace_back(GetSubfunction(context));
    body.emplace_back(memorySelection);
    service_x19::PushDtc(body, dtcAndStatusRecord.dtcValue);
    body.emplace_back(dtcAndStatusRecord.status);
    for (decltype(auto) record : snapshotDataRecord) {
        body.emplace_back(record.snapshotRecordNumber);
        body.emplace_back(static_cast< std::uint8_t >(record.snapshotDataIdentifiers.size()));
        for (decltype(auto) sr : record.snapshotDataIdentifiers) {
            service_x19::Push(body, sr.dataIdentifier);
            std::ignore = body.insert(body.cend(), sr.data.begin(), sr.data.end());
        }
    }
    return response;
}

static std::shared_ptr< Message > CreateRespondMessage(
    std::shared_ptr< RequestContext >& context,
    std::vector< DTCAndSnapshotNumber > const& dtcAndSnapshotNumberTable) noexcept
{
    std::shared_ptr< Message > response{context->CreateResponse()};
    decltype(auto) body{response->GetBody()};
    body.emplace_back(GetSubfunction(context));
    for (auto&& dtcAndSnapshotNumber : dtcAndSnapshotNumberTable) {
        for (auto&& snapshotIdentifier : dtcAndSnapshotNumber.snapshotIdentifierTable) {
            service_x19::PushDtc(body, dtcAndSnapshotNumber.dtcValue);
            body.emplace_back(snapshotIdentifier);
        }
    }
    return response;
}

static std::shared_ptr< Message > CreateRespondMessage(std::shared_ptr< RequestContext >& context,
                                                       std::uint8_t const memorySelection,
                                                       DTCAndStatusRecord const& dtcAndStatusRecord,
                                                       std::vector< DTCExtDataRecord > const& dtcExtDataRecord) noexcept
{
    std::shared_ptr< Message > response{context->CreateResponse()};
    decltype(auto) body{response->GetBody()};
    body.emplace_back(GetSubfunction(context));
    body.emplace_back(memorySelection);
    service_x19::PushDtc(body, dtcAndStatusRecord.dtcValue);
    body.emplace_back(dtcAndStatusRecord.status);
    for (decltype(auto) record : dtcExtDataRecord) {
        body.emplace_back(record.recordNumber);
        std::ignore = body.insert(body.cend(), record.data.begin(), record.data.end());
    }
    return response;
}

std::vector< ValidatorPtr > ServiceX19::GetSidValidator() noexcept
{
    std::vector< ValidatorPtr > validatorTable{};
    if (authValidator_.get() != nullptr) {
        validatorTable.push_back(authValidator_);
    }
    if (sessionValidator_.get() != nullptr) {
        validatorTable.push_back(sessionValidator_);
    }
    if (securityLevelValidator_.get() != nullptr) {
        validatorTable.push_back(securityLevelValidator_);
    }
    return validatorTable;
}

std::size_t ServiceX19::GetMinimumSize(std::uint8_t subfunction) noexcept
{
    size_t minSize{0U};
    switch (static_cast< ReadDTCInformationSubfunction >(subfunction)) {
        case ReadDTCInformationSubfunction::kReportNumberOfDtcByStatusMask:
        case ReadDTCInformationSubfunction::kReportDtcByStatusMask: {
            minSize = 3U;
        } break;
        case ReadDTCInformationSubfunction::kReportDtcSnapshotRecordByDtcNumber:
        case ReadDTCInformationSubfunction::kReportDtcExtDataRecordByDtcNumber: {
            minSize = kInt8_6U;
        } break;
        case ReadDTCInformationSubfunction::kReportNumberOfDtcBySeverityMaskRecord:
        case ReadDTCInformationSubfunction::kReportUserDefMemoryDtcByStatusMask: {
            minSize = 4U;
        } break;
        case ReadDTCInformationSubfunction::kReportSupportedDtcs:
        case ReadDTCInformationSubfunction::kReportDtcSnapshotIdentification:
        case ReadDTCInformationSubfunction::kReportDtcFaultDetectionCounter: {
            minSize = 2U;
        } break;
        case ReadDTCInformationSubfunction::kReportUserDefMemoryDtcSnapshotRecordByDtcNumber:
        case ReadDTCInformationSubfunction::kReportUserDefMemoryDtcExtDataRecordByDtcNumber: {
            minSize = kInt8_7U;
        } break;
        default: {
            LogWarn() << "ServiceX19::GetMinimumSize|kSubfunctionNotSupported|0x" << ara::log::LogHex8{subfunction};
        } break;
    }

    return minSize;
}

bool ServiceX19::IsSupport(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return true;
    }
    return false;
}

std::shared_ptr< Validator > ServiceX19::GetSubfunctionAuth(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->authValidator;
    }
    LogInfo() << "ServiceX19::GetSubfunctionAuth|Please config auth validator in subfunction:" << subfunction;
    return nullptr;
}

std::shared_ptr< Validator > ServiceX19::GetSubfunctionSession(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->sessionValidator;
    }
    LogWarn() << "ServiceX19::GetSubfunctionSession|Please config session validator in subfunction:" << subfunction;
    return nullptr;
}

bool ServiceX19::SupportSecurityLevel() noexcept
{
    if (securityLevelValidator_.get() != nullptr) {
        return true;
    }
    return false;
}

std::shared_ptr< Validator > ServiceX19::GetSubfunctionSecurityLevel(std::uint8_t subfunction) noexcept
{
    auto findInstacneResult = instanceTable_.find(subfunction);
    if (findInstacneResult != instanceTable_.end()) {
        return findInstacneResult->second->securityLevelValidator;
    }
    LogWarn() << "ServiceX19::GetSubfunctionSecurityLevel|Please config security level validator in subfunction:"
              << subfunction;
    return nullptr;
}

void ServiceX19::ParseAndResponse(std::shared_ptr< RequestContext > context) noexcept
{
    if (context.get() == nullptr) {
        LogWarn() << "ServiceX19::Process|context is nullptr";
        return;
    }

    decltype(auto) request{context->GetRequest()};
    decltype(auto) requestBody{request->GetBody()};
    std::size_t const requestSize{requestBody.size()};

    ///  [SWS_DM_00098]
    if (requestSize < service_x19::kMinRequestSize) {
        context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
        LogWarn() << "ServiceX19::Process|kIncorrectMessageLengthOrInvalidFormat";
        return;
    }

    std::shared_ptr< DtcFilter > const& interface = faultManager_.GetReadDtcInformationInterface();
    if (interface.get() == nullptr) {
        LogWarn() << "ServiceX19::Process|interface is nullptr";
        return;
    }

    std::uint8_t const subfunction = GetSubfunction(context);
    LogVerbose() << "ServiceX19::ParseAndResponse|subfunction =" << static_cast< uint16_t >(subfunction);
    switch (static_cast< ReadDTCInformationSubfunction >(subfunction)) {
        case ReadDTCInformationSubfunction::kReportNumberOfDtcByStatusMask: {
            std::uint8_t const kReportNumberOfDtcByStatusMaskSize{3U};
            if (requestSize != kReportNumberOfDtcByStatusMaskSize) {
                context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
                LogWarn() << "ServiceX19::Process|kIncorrectMessageLengthOrInvalidFormat";
                return;
            }
            std::uint8_t dtcStatusAvailabilityMask{0U};
            std::uint8_t dtcFormatIdentifier{0U};
            std::uint16_t dtcCount{0U};
            int32_t const result = interface->GetNumberOfDTCByStatusMask(requestBody[2U], dtcStatusAvailabilityMask,
                                                                         dtcFormatIdentifier, dtcCount);
            if (result == 0) {
                std::shared_ptr< Message > response{
                    CreateRespondMessage(context, dtcStatusAvailabilityMask, dtcFormatIdentifier, dtcCount)};
                context->Respond(std::move(response));
            } else {
                context->Respond(NrcErrc::kRequestOutOfRange);
                LogWarn() << "ServiceX19::Process|" << result;
            }
            break;
        }
        case ReadDTCInformationSubfunction::kReportDtcByStatusMask: {
            std::uint8_t const kReportDtcByStatusMaskSize{3U};
            if (requestSize != kReportDtcByStatusMaskSize) {
                context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
                LogWarn() << "ServiceX19::Process|kIncorrectMessageLengthOrInvalidFormat";
                return;
            }
            std::uint8_t dtcStatusAvailabilityMask{0U};
            std::vector< DTCAndStatusRecord > dtcAndStatusRecord{};
            int32_t const result
                = interface->GetDTCByStatusMask(requestBody[2U], dtcStatusAvailabilityMask, dtcAndStatusRecord);
            if (result == 0) {
                std::shared_ptr< Message > response{
                    CreateRespondMessage(context, dtcStatusAvailabilityMask, dtcAndStatusRecord)};
                context->Respond(std::move(response));
            } else {
                context->Respond(NrcErrc::kRequestOutOfRange);
                LogWarn() << "ServiceX19::Process|" << result;
            }
            break;
        }
        case ReadDTCInformationSubfunction::kReportDtcSnapshotIdentification: {
            std::uint8_t const kReportDtcSnapshotRecordByDtcNumberSize{2U};
            if (requestSize != kReportDtcSnapshotRecordByDtcNumberSize) {
                context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
                LogWarn() << "ServiceX19::Process|kIncorrectMessageLengthOrInvalidFormat";
                return;
            }
            std::vector< DTCAndSnapshotNumber > dtcAndSnapshotNumberTable;
            int32_t const result = interface->GetDTCSnapshotNumber(dtcAndSnapshotNumberTable);
            if (result == 0) {
                std::shared_ptr< Message > response{CreateRespondMessage(context, dtcAndSnapshotNumberTable)};
                context->Respond(std::move(response));

            } else {
                context->Respond(NrcErrc::kRequestOutOfRange);
                LogWarn() << "ServiceX19::Process|" << result;
            }
            break;
        }
        case ReadDTCInformationSubfunction::kReportDtcSnapshotRecordByDtcNumber: {
            std::uint8_t const kReportDtcSnapshotRecordByDtcNumberSize{kInt8_6U};
            if (requestSize != kReportDtcSnapshotRecordByDtcNumberSize) {
                context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
                LogWarn() << "ServiceX19::Process|kIncorrectMessageLengthOrInvalidFormat";
                return;
            }
            std::uint32_t const dtcMaskRecord{service_x19::GetDtc(requestBody)};
            DTCAndStatusRecord dtcAndStatusRecord{};
            std::vector< SnapshotDataRecordType > snapshotDataRecord;
            int32_t const result = interface->GetDTCSnapshotRecordByDTCNumber(dtcMaskRecord, requestBody[5U],
                                                                              dtcAndStatusRecord, snapshotDataRecord);
            if (result == 0) {
                std::shared_ptr< Message > response{
                    CreateRespondMessage(context, dtcAndStatusRecord, snapshotDataRecord)};
                context->Respond(std::move(response));

            } else {
                context->Respond(NrcErrc::kRequestOutOfRange);
                LogWarn() << "ServiceX19::Process|" << result;
            }
            break;
        }
        case ReadDTCInformationSubfunction::kReportDtcExtDataRecordByDtcNumber: {
            std::uint8_t const kReportDtcExtDataRecordByDtcNumberSize{kInt8_6U};
            if (requestSize != kReportDtcExtDataRecordByDtcNumberSize) {
                context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
                LogWarn() << "ServiceX19::Process|kIncorrectMessageLengthOrInvalidFormat";
                return;
            }
            std::uint32_t const dtcMaskRecord{service_x19::GetDtc(requestBody)};
            DTCAndStatusRecord dtcAndStatusRecord{};
            std::vector< DTCExtDataRecord > dtcExtDataRecord;
            int32_t const result = interface->GetDTCExtDataRecordByDTCNumber(dtcMaskRecord, requestBody[5U],
                                                                             dtcAndStatusRecord, dtcExtDataRecord);
            if (result == 0) {
                std::shared_ptr< Message > response{
                    CreateRespondMessage(context, dtcAndStatusRecord, dtcExtDataRecord)};
                context->Respond(std::move(response));
            } else {
                context->Respond(NrcErrc::kRequestOutOfRange);
                LogWarn() << "ServiceX19::Process|" << result;
            }
            break;
        }
        case ReadDTCInformationSubfunction::kReportNumberOfDtcBySeverityMaskRecord: {
            std::uint8_t const kReportNumberOfDtcBySeverityMaskRecordSize{4U};
            if (requestSize != kReportNumberOfDtcBySeverityMaskRecordSize) {
                context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
                LogWarn() << "ServiceX19::Process|kIncorrectMessageLengthOrInvalidFormat";
                return;
            }
            std::uint8_t dtcStatusAvailabilityMask{0U};
            std::uint8_t dtcFormatIdentifier{0U};
            std::uint16_t dtcCount{0U};
            int32_t const result = interface->GetNumberOfDTCBySeverityMaskRecord(
                requestBody[2U], requestBody[3U], dtcStatusAvailabilityMask, dtcFormatIdentifier, dtcCount);
            if (result == 0) {
                std::shared_ptr< Message > response{
                    CreateRespondMessage(context, dtcStatusAvailabilityMask, dtcFormatIdentifier, dtcCount)};
                context->Respond(std::move(response));
            } else {
                context->Respond(NrcErrc::kRequestOutOfRange);
                LogWarn() << "ServiceX19::Process|" << result;
            }
            break;
        }
        case ReadDTCInformationSubfunction::kReportSupportedDtcs: {
            std::uint8_t const kReportSupportedDtcsSize{2U};
            if (requestSize != kReportSupportedDtcsSize) {
                context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
                LogWarn() << "ServiceX19::Process|kIncorrectMessageLengthOrInvalidFormat";
                return;
            }
            std::uint8_t dtcStatusAvailabilityMask{0U};
            std::vector< DTCAndStatusRecord > dtcAndStatusRecord{};
            int32_t const result = interface->GetSupportedDTC(dtcStatusAvailabilityMask, dtcAndStatusRecord);
            if (result == 0) {
                std::shared_ptr< Message > response{
                    CreateRespondMessage(context, dtcStatusAvailabilityMask, dtcAndStatusRecord)};
                context->Respond(std::move(response));
            } else {
                context->Respond(NrcErrc::kRequestOutOfRange);
                LogWarn() << "ServiceX19::Process|" << result;
            }
            break;
        }
        case ReadDTCInformationSubfunction::kReportDtcFaultDetectionCounter: {
            std::uint8_t const kReportDtcFaultDetectionCounterSize{2U};
            if (requestSize != kReportDtcFaultDetectionCounterSize) {
                context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
                LogWarn() << "ServiceX19::Process|kIncorrectMessageLengthOrInvalidFormat";
                return;
            }
            std::vector< DTCFaultDetectionCounterRecord > dtcFaultDetectionCounterRecord;
            int32_t const result{std::move(interface->GetDTCFaultDetectionCounter(dtcFaultDetectionCounterRecord))};
            if (result == 0) {
                std::shared_ptr< Message > response{CreateRespondMessage(context, dtcFaultDetectionCounterRecord)};
                context->Respond(std::move(response));
            } else {
                context->Respond(NrcErrc::kRequestOutOfRange);
                LogWarn() << "ServiceX19::Process|" << result;
            }
            break;
        }
        case ReadDTCInformationSubfunction::kReportUserDefMemoryDtcByStatusMask: {
            std::uint8_t const kReportUserDefMemoryDtcByStatusMaskSize{4U};
            if (requestSize != kReportUserDefMemoryDtcByStatusMaskSize) {
                context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
                LogWarn() << "ServiceX19::Process|kIncorrectMessageLengthOrInvalidFormat";
                return;
            }
            std::uint8_t dtcStatusAvailabilityMask{0U};
            std::vector< DTCAndStatusRecord > dtcAndStatusRecord{};
            int32_t const result{std::move(interface->GetUserDefMemoryDTCByStatusMask(
                requestBody[2U], requestBody[3U], dtcStatusAvailabilityMask, dtcAndStatusRecord))};
            if (result == 0) {
                std::shared_ptr< Message > response{
                    CreateRespondMessage(context, requestBody[3U], dtcStatusAvailabilityMask, dtcAndStatusRecord)};
                context->Respond(std::move(response));
            } else {
                context->Respond(NrcErrc::kRequestOutOfRange);
                LogWarn() << "ServiceX19::Process|" << result;
            }
            break;
        }
        case ReadDTCInformationSubfunction::kReportUserDefMemoryDtcSnapshotRecordByDtcNumber: {
            std::uint8_t const kReportUserDefMemoryDtcSnapshotRecordByDtcNumberSize{kInt8_7U};
            if (requestSize != kReportUserDefMemoryDtcSnapshotRecordByDtcNumberSize) {
                context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
                LogWarn() << "ServiceX19::Process|kIncorrectMessageLengthOrInvalidFormat";
                return;
            }
            std::uint32_t const dtcMaskRecord{service_x19::GetDtc(requestBody)};
            DTCAndStatusRecord dtcAndStatusRecord{};
            std::vector< SnapshotDataRecordType > snapshotDataRecord;
            int32_t const result{std::move(interface->GetUserDefMemoryDTCSnapshotRecordByDTCNumber(
                dtcMaskRecord, requestBody[5U], requestBody[kInt8_6U], dtcAndStatusRecord, snapshotDataRecord))};
            if (result == 0) {
                std::shared_ptr< Message > response{
                    CreateRespondMessage(context, requestBody[kInt8_6U], dtcAndStatusRecord, snapshotDataRecord)};
                context->Respond(std::move(response));
            } else {
                context->Respond(NrcErrc::kRequestOutOfRange);
                LogWarn() << "ServiceX19::Process|" << result;
            }
            break;
        }
        case ReadDTCInformationSubfunction::kReportUserDefMemoryDtcExtDataRecordByDtcNumber: {
            std::uint8_t const kReportUserDefMemoryDtcExtDataRecordByDtcNumberSize{kInt8_7U};
            if (requestSize != kReportUserDefMemoryDtcExtDataRecordByDtcNumberSize) {
                context->Respond(NrcErrc::kIncorrectMessageLengthOrInvalidFormat);
                LogWarn() << "ServiceX19::Process|kIncorrectMessageLengthOrInvalidFormat";
                return;
            }
            std::uint32_t const dtcMaskRecord{service_x19::GetDtc(requestBody)};
            DTCAndStatusRecord dtcAndStatusRecord{};
            std::vector< DTCExtDataRecord > dtcExtDataRecord;
            int32_t const result{std::move(interface->GetUserDefMemoryDTCExtDataRecordByDTCNumber(
                dtcMaskRecord, requestBody[5U], requestBody[kInt8_6U], dtcAndStatusRecord, dtcExtDataRecord))};
            if (result == 0) {
                std::shared_ptr< Message > response{
                    CreateRespondMessage(context, requestBody[kInt8_6U], dtcAndStatusRecord, dtcExtDataRecord)};
                context->Respond(std::move(response));
            } else {
                context->Respond(NrcErrc::kRequestOutOfRange);
                LogWarn() << "ServiceX19::Process|" << result;
            }
            break;
        }
        default: {
            context->Respond(NrcErrc::kSubfunctionNotSupported);
            LogWarn() << "ServiceX19::Process|kSubfunctionNotSupported|0x" << ara::log::LogHex8{subfunction};
        } break;
    }
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
