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
/// @file       log_command_excutor.cpp
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltProtocol
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00003
/// @unit_name = log_command_excutor
/// @unit_description=Implementation of command log forwarding on the Dlt lib side
/// @endcode
///
/// ================================================================

#include "log_command_excutor.h"

#include <isoft/ara_fsh/process.h>
#include <isoft/ipccpp/server.h>
#include <isoft/naicpp/evloop.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include <sys/uio.h>
#include <sys/un.h>

#include <csignal>
#include <ctime>
#include <fstream>
#include <thread>

#include "DltLogDefines/application_config.h"
#include "Utils/src/private_log.h"
#include "Utils/src/usercommand_define.h"
#include "ara/log/common.h"
#include "ara/log/internal/dlt_services.h"

namespace ara {
namespace log {
namespace internal {

static bool g_Local_Heart_Beat{true};

void LogCommandExcutor::HandleDltServiceCmd(std::int32_t cmd, rapidjson::Document const &d) noexcept
{
    DltServices const cmdEnum{static_cast< DltServices >(cmd)};
    switch (cmdEnum) {
        case DltServices::kDlt_Service_Id_Set_Default_Log_Level: {
            std::uint8_t const logLevel{static_cast< std::uint8_t >(d["data"].GetUint())};
            config_->defaultLogLevel = (logLevel);
            if (onLogLevelChangedCallback_) {
                std::string const contextid{""};
                onLogLevelChangedCallback_(contextid, logLevel);
            }
            break;
        }
        case DltServices::kDlt_Service_Id_Set_Log_Level: {
            std::uint8_t const logLevel{static_cast< std::uint8_t >(d["loglevel"].GetInt())};
            std::string const appid{d["appid"].GetString()};
            bool const haveNoContextid{d.HasMember("contextid") == false};
            if (d.HasMember("appid") && haveNoContextid) {
                bool const checkCB{static_cast< bool >(onLogLevelChangedCallback_)};
                if (appid == config_->appid && checkCB) {
                    std::string const contextid{""};
                    onLogLevelChangedCallback_(contextid, logLevel);
                    /// TODO: Adapt to control commands of R2311
                    // for (std::pair< std::string, DLTV1::ChannelConfigPtr > cpaire : config_->dltLogChannels) {
                    //     cpaire.second->logLevel = (logLevel);
                    // }
                }
            }
            bool const haveContextid{d.HasMember("contextid")};
            if (d.HasMember("appid") && haveContextid) {
                std::string const contextid{d["contextid"].GetString()};
                bool const checkCB{static_cast< bool >(onLogLevelChangedCallback_)};
                if (appid == config_->appid && checkCB) {
                    onLogLevelChangedCallback_(contextid, logLevel);
                    /// TODO: Adapt to control commands of R2311
                    // DLTV1::ChannelConfigMap &tmpmap{config_->dltLogChannels};
                    // if (tmpmap.count(contextid) > 0U) {
                    //     tmpmap[contextid]->logLevel = (logLevel);
                    // }
                }
            }
            break;
        }
        case DltServices::kDlt_Service_Id_Store_Config: {
            rapidjson::Document document;
            std::ignore = document.SetObject();

            rapidjson::Value appidValue;
            std::ignore
                = appidValue.SetString(config_->appid.c_str(), config_->appid.length(), document.GetAllocator());
            std::ignore = document.AddMember("appId", appidValue, document.GetAllocator());
            std::ignore = document.AddMember("defaultLevel", config_->defaultLogLevel, document.GetAllocator());
            rapidjson::Value configlistValue{rapidjson::kArrayType};
            /// TODO: Adapt to control commands of R2311
            // for (std::pair< std::string, DLTV1::ChannelConfigPtr > const chpair : config_->dltLogChannels) {
            //     DLTV1::ChannelConfigPtr channel{chpair.second};
            //     rapidjson::Value channelValue{rapidjson::kObjectType};
            //     std::ignore = channelValue.AddMember(
            //         "ctxId", rapidjson::Value(channel->contextId.c_str(), document.GetAllocator()).Move(),
            //         document.GetAllocator());
            //     std::ignore = channelValue.AddMember("level", channel->logLevel, document.GetAllocator());
            //     std::ignore = configlistValue.PushBack(channelValue, document.GetAllocator());
            // }
            std::ignore = document.AddMember("channels", configlistValue, document.GetAllocator());

            rapidjson::StringBuffer buffer;
            rapidjson::PrettyWriter< rapidjson::StringBuffer > writer{buffer};
            std::ignore = document.Accept(writer);

            isoft::ara_fsh::Process const proc;
            std::string const etcpath{proc.GetEtcDir()};
            std::ofstream outputFile{etcpath, static_cast< std::ios_base::openmode >(std::ios::out | std::ios::trunc)};
            if (outputFile.is_open()) {
                outputFile << buffer.GetString();
                outputFile.close();
            }

            break;
        }
        case DltServices::kDlt_Service_Id_Reset_To_Factory_Default: {
            /// TODO Save information
            // else {
            // }
            // TODO Need to reload log information? Restart to take effect

            break;
        }
        default: {
            break;
        }
    }
}

void LogCommandExcutor::HandleUserCmd(std::int32_t cmd) noexcept
{
    UserCommand const userCmd{static_cast< UserCommand >(cmd)};
    switch (userCmd) {
        case UserCommand::kRegAppStatusOK:
        case UserCommand::kRegAppStatusFailed: {
            LOGVERBOSE(" UserCommand::kRegAppStatus: ") << cmd;
            if (onRegistStatusChangedCallback_) {
                onRegistStatusChangedCallback_(cmd);
            }

            break;
        }
        case UserCommand::kClientConncted: {
            if (config_) {
                config_->clientstate = ClientState::kConnected;
            }
            if (onClientstateChangedCallback_) {
                onClientstateChangedCallback_(ClientState::kConnected);
            }
            break;
        }
        case UserCommand::kClientDisConncted: {
            if (config_) {
                config_->clientstate = ClientState::kNotConnected;
            }
            if (onClientstateChangedCallback_) {
                onClientstateChangedCallback_(ClientState::kNotConnected);
            }
            break;
        }
        case UserCommand::kUpdateHeartBeat: {
            g_Local_Heart_Beat = true;
            break;
        }
        default: {
            break;
        }
    }
}

// void timer_handler(std::int32_t sig)
// {
//     std::cout << __func__ << sig << std::endl;
//     if (g_Local_Heart_Beat) {
//         g_Local_Heart_Beat = false;
//         return;
//     }
//     // g_Local_Heart_Beat
//     exit(0);
// }
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00324
/// @trace_id_dd=DD_LOG_01720
/// @needwork = ad
/// @endcode
/// @brief
/// @param[in]  config
LogCommandExcutor::LogCommandExcutor(std::shared_ptr< ApplicationConfig > const &config) noexcept
{
    LOGVERBOSE("enter : ") << __func__;
    config_ = config;
}
/// @brief
LogCommandExcutor::~LogCommandExcutor() noexcept { LOGVERBOSE("enter : ") << __func__; }

bool LogCommandExcutor::StartBeatHeartCheck() noexcept
{
    // if(config_->appid == "TIDM") {
    timer_t timerid{nullptr};
    struct sigevent sev
    {
    };
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo  = SIGALRM;
    sev.sigev_value  = {0};

    timer_create(CLOCK_REALTIME, &sev, &timerid);

    struct itimerspec its = {{2, 0}, {1, 0}};  // Trigger every 1 second
    timer_settime(timerid, 0, &its, nullptr);
    // signal(SIGALRM, timer_handler);
    return true;
}

/// @brief Initialize LogCommandExcutor, establish an IPC client connection
/// @return LogCommandExcutorStatus initialization status
/// @details
///   Returns kSuccess on success
///   Returns corresponding error status code on failure
LogCommandExcutorStatus LogCommandExcutor::Init() noexcept
{
    LOGVERBOSE("enter : ") << __func__;

    // === Check configuration ===
    if (nullptr == config_) {
        LOGVERBOSE(__func__) << "config_ is nullptr";
        return LogCommandExcutorStatus::kNotInitialized;
    }

    // === Verify that the send callback has been set ===
    if (!sendRequestCallback_) {
        LOGVERBOSE(__func__) << "sendRequestCallback_ is not set";
        return LogCommandExcutorStatus::kNotInitialized;
    }

    LOGVERBOSE(__func__) << config_->appid << " send callback ready";

    // === Register the application ===
    // clang-format off
    if (config_->appid == "EMD") {
        std::thread t{ [this]() {
            std::int8_t const kDeSix{3};
            std::this_thread::sleep_for(std::chrono::seconds(kDeSix));
            this->RegistApp();
        } };
        pthread_setname_np(t.native_handle(), "CMD_t");
        t.detach();
    } else {
        this->RegistApp();
    }
    // clang-format on

    status_ = true;
    LOGVERBOSE(__func__) << "leave - Init success";
    return LogCommandExcutorStatus::kSuccess;
}
/// @brief Update log channel information
/// @param[in]  appId     Application ID
/// @param[in]  ctxId     Channel (context) ID
/// @param[in]  logLevel  New log level
/// @return LogCommandExcutorStatus update status
/// @details
///   Returns kSuccess on success
///   Returns kNotInitialized if not initialized
///   Returns kInvalidParameter if parameters are invalid
///   Returns kSendFailed if IPC sending fails
LogCommandExcutorStatus LogCommandExcutor::UpdateLogChannel(std::string const &appId,
                                                            std::string const &ctxId,
                                                            std::uint8_t const &logLevel) noexcept
{
    LOGVERBOSE(__func__) << "appid: " << appId << " ctxid:" << ctxId;

    // === Check initialization status ===
    if (!status_ || !sendRequestCallback_) {
        LOGVERBOSE(__func__) << "Not initialized";
        return LogCommandExcutorStatus::kNotInitialized;
    }

    // === Parameter check ===
    if (appId.empty()) {
        LOGVERBOSE(__func__) << "appId is empty";
        return LogCommandExcutorStatus::kInvalidParameter;
    }

    rapidjson::Document document;
    std::ignore = document.SetObject();
    rapidjson::Document::AllocatorType &jsonAllocator{document.GetAllocator()};

    std::ignore = document.AddMember("cmd", static_cast< std::int32_t >(UserCommand::kUpdateLogChannel),
                                     document.GetAllocator());
    std::ignore
        = document.AddMember("appid", rapidjson::Value().SetString(appId.c_str(), jsonAllocator), jsonAllocator);

    // Create an array
    rapidjson::Value jsonArray{rapidjson::kArrayType};

    rapidjson::Value subobject{rapidjson::kObjectType};
    std::ignore
        = subobject.AddMember("contextId", rapidjson::Value().SetString(ctxId.c_str(), jsonAllocator), jsonAllocator);
    std::ignore = subobject.AddMember("logLevel", logLevel, jsonAllocator);
    std::ignore = jsonArray.PushBack(subobject, jsonAllocator);

    // Add the array to the Document
    std::ignore = document.AddMember("contextIdInfoList", jsonArray, jsonAllocator);
    // Convert the Document to a string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
    std::ignore = document.Accept(writer);

    std::string sendstr{buffer.GetString()};
    std::ignore = sendstr.append("\n");

    // === Send IPC request ===
    if (!sendRequestCallback_ || 0 != sendRequestCallback_(sendstr)) {
        LOGVERBOSE(__func__) << "Send IPC request failed";
        return LogCommandExcutorStatus::kSendFailed;
    }

    LOGVERBOSE(__func__) << "UpdateLogChannel success";
    return LogCommandExcutorStatus::kSuccess;
}
/// @brief Destroy LogCommandExcutor, release IPC resources
/// @return LogCommandExcutorStatus destruction status
/// @details
///   Returns kSuccess on success
///   Returns kDestroyFailed on failure
LogCommandExcutorStatus LogCommandExcutor::LogCommandDestroy() noexcept
{
    LOGVERBOSE(__func__) << " enter ";

    status_ = false;
    LOGVERBOSE(__func__) << " leave - Destroy success";
    return LogCommandExcutorStatus::kSuccess;
}

/// @brief
void LogCommandExcutor::RegistApp() noexcept
{
    LOGVERBOSE(__func__) << " : enter--- ";
    rapidjson::Document document;
    std::ignore = document.SetObject();
    rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

    std::ignore = document.AddMember("cmd", static_cast< std::int32_t >(UserCommand::kRegApp), document.GetAllocator());
    std::ignore = document.AddMember("appid", rapidjson::Value().SetString(config_->appid.c_str(), rapidallocator),
                                     rapidallocator);
    std::ignore = document.AddMember(
        "appidDesc", rapidjson::Value().SetString(config_->appdesc.c_str(), rapidallocator), rapidallocator);
    // Create an array
    rapidjson::Value jsonArray{rapidjson::kArrayType};

    /// TODO: Adapt to control commands of R2311
    // for (std::pair< std::string, std::shared_ptr< DLTV1::ChannelConfig > > tmpC : config_->dltLogChannels) {
    //     rapidjson::Value subobject{rapidjson::kObjectType};
    //     std::ignore = subobject.AddMember("contextId", rapidjson::Value().SetString(tmpC.first.c_str(), rapidallocator),
    //                                       rapidallocator);
    //     std::ignore = subobject.AddMember(
    //         "contextDesc", rapidjson::Value().SetString(tmpC.second->contextDescription.c_str(), rapidallocator),
    //         rapidallocator);
    //     std::ignore = subobject.AddMember("logLevel", tmpC.second->logLevel, rapidallocator);
    //     std::ignore = jsonArray.PushBack(subobject, rapidallocator);
    // }

    // Add the array to the Document
    std::ignore = document.AddMember("contextIdInfoList", jsonArray, rapidallocator);
    // Convert the Document to a string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
    std::ignore = document.Accept(writer);

    std::string sendstr{buffer.GetString()};
    std::ignore = sendstr.append("\n");

    if (sendRequestCallback_) {
        std::ignore = sendRequestCallback_(sendstr);
    }
}
/// @brief
/// @param[in]  jsonstr
void LogCommandExcutor::HandleDaemonCmd(std::string const &jsonstr) noexcept
{
    LOGVERBOSE(__func__) << " : enter--- ";
    LOGVERBOSE(__func__) << "  " << config_->appid << jsonstr;
    g_Local_Heart_Beat = true;
    rapidjson::Document d;
    if (d.Parse(jsonstr.c_str()).HasParseError()) {
    } else {
        if (d.HasMember("cmd")) {
            std::int32_t const cmd{d["cmd"].GetInt()};
            std::int32_t const dltMaxServiceId{static_cast< std::int32_t >(DltServices::kDlt_Service_Id_Last_Entry)};
            std::int32_t const userCmdStart{static_cast< std::int32_t >(UserCommand::kRegApp)};
            std::int32_t const userCmdEnd{static_cast< std::int32_t >(UserCommand::kLibCommandEnd)};

            if (cmd >= userCmdStart && cmd <= userCmdEnd) {
                HandleUserCmd(cmd);
            } else if (cmd >= 0 && cmd <= dltMaxServiceId) {
                HandleDltServiceCmd(cmd, d);
            }
        }
    }
}

}  // namespace internal
}  // namespace log
}  // namespace ara
