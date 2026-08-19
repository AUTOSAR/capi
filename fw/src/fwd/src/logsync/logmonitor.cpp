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
/// @file       logmonitor.cpp
/// @brief      fw kern.log log processing class
/// @details    fw kern.log log processing class
/// @date       2024-12-17
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/IDSM Log Sync
/// @interface_level=module
/// @trace_id_sr=SR_FW_0009
/// @unit_name=Log_Monitor
/// @unit_description=Firewall IDSM data synchronization
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "logmonitor.h"

#include <ara/core/instance_specifier.h>
#include <ara/idsm/event_report.h>
#include <ara/idsm/timestamp_provider.h>
#include <isoft/serialize/serialize.h>

#include <algorithm>
#include <string>
#include <thread>

#include "ara/fw/common/common.h"
#include "ara/fw/internal/cmd_str.h"
#include "ara/fw/internal/log_api.h"
namespace ara {
namespace fw {
namespace internal {

/// @brief
/// @return
bool LogMonitor::_isFieldStart(char8_t c) noexcept { return (c >= 'A' && c <= 'Z'); }

/// @brief
/// @return
bool LogMonitor::_isFieldChar(char8_t c) noexcept { return (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'); }
/// @brief Parse log line by line
/// @param LogLine Log
/// @param strPrefix
/// @return Success/Failure
bool LogMonitor::LogParse(ara::core::String const &logLine) noexcept
{
    ara::core::String keyword{"Firewall-Drop"};
    ara::core::String keywordApp{"FirewallApp"};
    fields_.clear();
    // The line is not empty and has the corresponding keyword
    if ((!logLine.empty())) {
        // Non-APP layer
        if (logLine.find(keyword) != std::string::npos) {
            size_t pos       = keyword.length();
            const size_t end = logLine.length();

            ara::core::String currentField{};
            ara::core::String currentValue{};
            bool inField{false};
            bool inValue{false};

            while (pos < end) {
                char8_t c = logLine[pos];

                if (!inField && !inValue && _isFieldStart(c)) {
                    // Start new field
                    inField = true;
                    currentField.clear();
                    currentField += c;
                } else if (inField && !inValue) {
                    if (c == '=') {
                        // Field name end, start value
                        inField = false;
                        inValue = true;
                        currentValue.clear();
                    } else if (_isFieldStart(c)) {
                        // Continue field name
                        currentField += c;
                    } else {
                        // Invalid character, reset state
                        inField = false;
                        currentField.clear();
                    }
                } else if (!inField && inValue) {
                    if (_isFieldStart(c) && (pos + 1 < end && _isFieldChar(logLine[pos + 1]))) {
                        // New field starts, save current field
                        if (!currentField.empty()) {
                            // Remove extra spaces
                            currentValue.erase(std::remove(currentValue.begin(), currentValue.end(), ' '),
                                               currentValue.end());
                            // Remove non-empty LEN of 2 bytes, temporarily do not add to IDSM
                            // data.
                            if (!currentValue.empty() && currentField != "LEN") {
                                // If key is MAC, truncate. Before is dest, after is src. Ubuntu
                                // data has extra 08:00, remove it.
                                if (currentField == "MAC") {
                                    if (currentValue.size() >= (kPayloadOffset17 + kPayloadOffset18)) {
                                        ara::core::String macDest{currentValue.substr(0, kPayloadOffset17)};
                                        ara::core::String macSrc{
                                            currentValue.substr(kPayloadOffset18, kPayloadOffset17)};
                                        fields_["SRCMAC"]  = macSrc;
                                        fields_["DESTMAC"] = macDest;
                                    }

                                } else {
                                    fields_[currentField] = currentValue;
                                }
                            }
                        }

                        // Start new field
                        inValue = false;
                        inField = true;
                        currentField.clear();
                        currentField += c;
                        currentValue.clear();
                    } else {
                        // Continue current value
                        currentValue += c;
                    }
                }
                ++pos;
            }
            // PROTO handled separately, MAC truncated string processing, convert to
            // Process last field
            if (!currentField.empty() && !currentValue.empty()) {
                fields_[currentField] = currentValue;
            }  /// app layer.
        } else if (logLine.find(keywordApp) != std::string::npos) {
            _parseAppData(logLine);
        } else {
        }
    }
    return !fields_.empty();
}  // namespace internal

/// @brief Serialize data
/// @param vec Data to serialize
/// @param vecPaylod Serialized data
ara::core::Vector< uint8_t > LogMonitor::Serialize(ara::core::Vector< ara::core::String > const &vec) noexcept
{
    LogDebug() << "LogMonitor::Serialize in!";
    ara::core::Vector< uint8_t > vecPaylod{};
    // //for debug.
    // for (auto const& it : vec) {
    //     LogDebug() << "LogMonitor::Serialize: get vec data:" << it;
    // }
    isoft::serialize::Buffer< Chunk > buffer{vecPaylod};
    isoft::serialize::Buffer< Chunk >::result_t const status{isoft::serialize::Serialize(buffer, vec)};
    if (status < 0) {
        vecPaylod.clear();
    }
    if (vecPaylod.empty()) {
        LogWarn() << "LogMonitor::Serialize: get vecPaylod is empty!";
    }
    return vecPaylod;
}

/// @brief Call IDSM interface to send data
/// @param vecPaylod Serialized data
void LogMonitor::CallIdsmApi(ara::core::Vector< uint8_t > const &vecPaylod) noexcept
{
    LogDebug() << "LogMonitor::CallIdsmApi get vecpayload size:" << vecPaylod.size();
    if (!vecPaylod.empty()) {
        // Will be modified after actual NLM confirmation.
        ara::core::InstanceSpecifier portAuth{"fwd/fw_root_swc/rPort_fw"};
        ara::idsm::EventReporter idsmClient{portAuth};
        ara::idsm::ContextDataType contextData{vecPaylod};
        idsmClient.ReportEvent(contextData);
    }
}

void LogMonitor::_saveReadLineNum() const noexcept
{
    std::ofstream stateFile{"./fwReadLine"};
    if (stateFile.is_open()) {
        stateFile << lineNum_;
        LogInfo() << "readlineNum is save in  fwReadLine";
    } else {
        LogWarn() << "can't store readlineNum";
    }
}

void LogMonitor::_loadLastReadLineNum() noexcept
{
    std::ifstream stateFile("./fwReadLine");
    if (stateFile.is_open()) {
        stateFile >> lineNum_;
        LogInfo() << "restart from last time process line " << lineNum_;
    } else {
        LogWarn() << " cant't find fwReadLine file. start from in begin.";
        lineNum_ = 0;
    }
}

/// @brief Execute synchronous log task.
bool LogMonitor::Start() noexcept
{
    /// @brief kern.log
    // std::ifstream file{GetkKernLog()};
    ///@brief redaline
    std::string strLine{};
    /// serilize
    ara::core::Vector< uint8_t > payload{};
    // Structure sent to IDSM API
    ara::core::Vector< ara::core::String > vecStrValue{};
    // Single character data read.
    ara::core::String strToIdsm{};
    // open file.
    if (!file_.is_open()) {
        LogWarn() << "LogMonitor::Start(): open kern.log failed!,please check "
                     "permission and the file is exist!";
        return false;
    }
    // Loop to read file content.
    while (true) {
        payload.clear();
        // Read file content line by line for filtering.
        while (std::getline(file_, strLine)) {
            ara::core::String lineBuf{};
            lineBuf.append(strLine.c_str());
            if (LogParse(lineBuf)) {
                // Get data
                for (auto &it : fields_) {
                    strToIdsm = " " + it.first + "=" + it.second + " ";
                    vecStrValue.push_back(strToIdsm);
                }
                fields_.clear();
            }
        }
        // If there is data, synchronize to IDSM
        if (!vecStrValue.empty()) {
            // After reading the file, send the read data to IDSM API.
            payload = Serialize(vecStrValue);
            // Call IDSM interface.
            CallIdsmApi(payload);
        } else if (file_.eof()) {
            file_.clear();
        } else {
        }
        uint8_t const timeSleep{2U};
        std::this_thread::sleep_for(std::chrono::seconds(timeSleep));
    }
    return true;
}

/// @brief Parse a single field
/// @param fieldStr Input value
void LogMonitor::_parseField(ara::core::String const &fieldStr) noexcept
{
    // Find position of equal sign
    size_t eqPos = fieldStr.find('=');
    if (eqPos == std::string::npos) {
        return;
    }

    // Extract field name and value
    ara::core::String fieldName  = fieldStr.substr(0, eqPos);
    ara::core::String fieldValue = fieldStr.substr(eqPos + 1);
    LogDebug() << "get FirewallApp FieldName:" << fieldName.data();
    LogDebug() << "get FirewallApp FieldValue:" << fieldValue.data();
    if (!fieldValue.empty() && !fieldName.empty()) {
        fields_[fieldName] = fieldValue;
    }
}
/// @brief
/// @param logLine
void LogMonitor::_parseAppData(ara::core::String const &logLine) noexcept
{
    fields_.clear();
    std::string logStr(logLine.c_str());

    // Find the start position of "Firewall-doip-Drop"
    size_t startPos{logStr.find("FirewallApp")};
    if (startPos == std::string::npos) {
        LogWarn() << "_parseAppData: Cannot find FirewallApp in  kern log line";
        return;
    }

    // Extract the part starting from Firewall-doip-Drop
    std::string firewallPart = logStr.substr(startPos);

    startPos = firewallPart.find(":");
    if (startPos == std::string::npos) {
        std::cout << "_parseAppData:not  get :  in kern log line" << std::endl;
        return;
    }
    // Get the data after :.
    std::string strAppParseData{firewallPart.substr(startPos + 1)};
    // Use string stream to split fields
    std::istringstream iss(strAppParseData);
    std::string token{};

    // Loop
    while (std::getline(iss, token, ' ')) {
        ara::core::String buf{};
        buf.append(token.c_str());
        _parseField(buf);
    }
}

//********************************/
}  // namespace internal
}  // namespace fw
}  // namespace ara
