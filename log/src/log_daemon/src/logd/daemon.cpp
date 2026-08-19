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
/// @file       daemon.cpp
/// @brief      Management class for daemon processes
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltLogd
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00003,LOG_SR_00009
/// @unit_name = daemon
/// @unit_description=Backend of Dlt module, used to support log and command forwarding.
/// @endcode
///
/// ================================================================

#include "daemon.h"

#include <csignal>
#include <fstream>
#include <sstream>

#include "Utils/src/usercommand_define.h"
#ifdef HAS_ARA_LOG
    #undef HAS_ARA_LOG
#endif
#ifdef ARA_WITH_EXEC
    #include <ara/exec/execution_client.h>
#endif
#include <isoft/ara_fsh/process.h>
#include <isoft/ipccpp/debug.h>
#include <isoft/ipccpp/server.h>
#include <isoft/ipccpp/utility.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/writer.h>

/// @brief Configuration information
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00285
/// @trace_id_dd=DD_LOG_01539
/// @needwork = ad
/// @endcode
#include "Utils/src/private_log.h"
#include "log_command_trans.h"
#include "log_lib/include/ara/log/internal/dlt_services.h"
#include "log_trans.h"

using ara::log::internal::DltServices;
static ara::log::internal::Daemon *g_Local_Daemon{nullptr};
extern "C" void SigPipeHandler(std::int32_t sigNum) noexcept
{
    LOGVERBOSE("logd Caught SIGPIPE signal") << sigNum;
    LOGVERBOSE(__func__) << " leave  ";
    // Handle signals
}
extern "C" void SigTimeHandler(std::int32_t sigNum) noexcept
{
    std::ignore = sigNum;
    if (g_Local_Daemon != nullptr) {
        g_Local_Daemon->SendheartBeat();
    }
    LOGERROR(__func__) << " leave  " << sigNum;
    // Handle signals
}
namespace ara {
namespace log {
namespace internal {
/// @brief
/// @return
std::int32_t Daemon::Init() noexcept
{
    // timer_t timerid;
    // struct sigevent sev = {.sigev_value = {0}, .sigev_signo = SIGALRM, .sigev_notify = SIGEV_SIGNAL};
    // timer_create(CLOCK_REALTIME, &sev, &timerid);

    // struct itimerspec its = {{3, 0}, {1, 0}};  // Trigger every 1 second
    // timer_settime(timerid, 0, &its, NULL);
    // sighandler_t old_handler{signal(SIGALRM, SigTimeHandler)};

    // if (old_handler == SIG_ERR) {
    //     perror("Failed to register SIGINT");
    //     return 1;
    // } else if (old_handler == SIG_DFL) {
    //     printf("Previous handler was default.\n");
    // } else {
    //     printf("Previous handler address exsit\n");
    // }

    /// Prevent crash caused by pipe signal
    std::ignore = signal(SIGPIPE, SigPipeHandler);

    mainLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();
    if (mainLoop_ == nullptr) {
        LOGVERBOSE(__func__) << "mainloop is null";
        return EXIT_FAILURE;
    }

    mTerminatingHandler_ = std::make_shared< isoft::naicpp::TerminatingHandler >([this]() {
        // std::cout << "#LOGD# signal term callback " << std::endl;
        std::ignore = this->Destroy();
    });
    if (0 != mTerminatingHandler_->Register(mainLoop_)) {
        LOGVERBOSE(__func__) << "_register error _";
        return (EXIT_FAILURE);
    }

    std::string const configName{isoft::ara_fsh::Platform().GetPlatformEtcDir() + "/log_global_config.json"};

    DaemonConfig const daemonConfig{_ReadConfig(configName)};

    PrivateLogger::SetPrivateLogLevel(static_cast< PriLogLevelType >(daemonConfig.internalLogLevel));

    dltServer_  = std::make_unique< DltServer >();
    std::ignore = dltServer_->Init(daemonConfig.ecuid, daemonConfig.serverip, daemonConfig.serverport,
                                   daemonConfig.queueSize, this);

    mLogTrans_  = std::make_unique< LogTrans >();
    std::ignore = mLogTrans_->Init(this);

    mLogCommandTrans_ = std::make_shared< LogCommandTrans >();
    std::ignore       = mLogCommandTrans_->Init();
    dltServer_->SetAAInfos(mLogCommandTrans_->GetAAclientInfos());

    {
#ifdef ARA_WITH_EXEC
        ara::exec::ExecutionClient const ec;
        static_cast< void >(ec.ReportExecutionState(ara::exec::ExecutionState::kRunning));
#endif
    }
    LOGVERBOSE(__func__) << " leave  ";
    g_Local_Daemon = this;

    return EXIT_SUCCESS;
}
/// @brief
void Daemon::Run() noexcept
{
    LOGVERBOSE(__func__) << " enter  ] ";
    if (mainLoop_) {
        std::ignore = mainLoop_->Run(true);
    }
    LOGVERBOSE(__func__) << " stop running   ";

    if (this->mLogCommandTrans_) {
        this->mLogCommandTrans_->DeIniting();
        this->mLogCommandTrans_ = nullptr;
    }

    LOGVERBOSE(__func__) << " enter ";
    if (this->mLogTrans_ != nullptr) {
        std::ignore = this->mLogTrans_->Destroy();
        mLogTrans_  = nullptr;
    }

    if (this->dltServer_ != nullptr) {
        this->dltServer_->Destroy();
        this->dltServer_ = nullptr;
    }

    LOGVERBOSE(__func__) << " leave  ";
}

/// @brief
/// @return
std::int32_t Daemon::Destroy() noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    g_Local_Daemon = nullptr;

    if (this->mLogCommandTrans_ != nullptr) {
        this->mLogCommandTrans_->SetStoping();
    }
    LOGVERBOSE(__func__) << " enter ";
    if (this->mLogTrans_ != nullptr) {
        std::ignore = mLogTrans_->SetStoping();
    }

    if (this->mLogTrans_) {
        this->mLogTrans_->ClosePipe();
    }

    LOGVERBOSE(__func__) << " exit ";

    return 0;
}

void Daemon::SendheartBeat() noexcept
{
    std::string retstr{};
    rapidjson::StringBuffer buf;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buf};
    std::ignore = writer.StartObject();
    std::ignore = writer.Key("cmd");
    std::ignore = writer.Int(kUpdateHeartBeat);
    std::ignore = writer.EndObject();

    std::string jsonstr = retstr.append(buf.GetString()).append("\n");

    if (mLogCommandTrans_) {
        char *const datastr{const_cast< char * >(jsonstr.c_str())};
        std::ignore = mLogCommandTrans_->SendByAppid("TIDM", reinterpret_cast< uint8_t * >(datastr), jsonstr.length());
    }
}

bool Daemon::WaitingTimeout() noexcept
{
    if (this->mLogTrans_ != nullptr) {
        this->mLogTrans_->StopWaiting();
    }

    return false;
}

void Daemon::OnLogBuffer(uint8_t *mesg, std::size_t const &mesgSize) noexcept
{
    if (mesg != nullptr) {
        std::ignore = dltServer_->SendLogMessage(mesg, mesgSize);
    }
}

Daemon::DaemonConfig Daemon::_ReadConfig(std::string const &configPath) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    Daemon::DaemonConfig retConfig;
    std::ifstream const ifs{configPath};
    std::stringstream buffer;
    buffer << ifs.rdbuf();

    if (buffer.str().empty() == false) {
        rapidjson::Document d;
        std::string const datastr(buffer.str());

        if (d.Parse(datastr.c_str()).HasParseError() == false) {
            if (d.HasMember("ecuId")) {
                retConfig.ecuid = d["ecuId"].GetString();
            }

            bool const hasConn{d.HasMember("connectorArray")};
            bool const isConnAarry{hasConn && d["connectorArray"].IsArray()};
            if (isConnAarry) {
                for (rapidjson::Value &v : d["connectorArray"].GetArray()) {
                    if (v.IsObject() && v.HasMember("etherConnector") && v.HasMember("tcpPort")
                        && v["etherConnector"].IsString() && v["tcpPort"].IsUint()) {
                        std::string const logDaemonIP{v["etherConnector"].GetString()};
                        std::uint16_t const logDaemonPort{static_cast< std::uint16_t >(v["tcpPort"].GetUint())};
                        retConfig.serverip   = logDaemonIP;
                        retConfig.serverport = logDaemonPort;
                    }
                }
            }
            if (d.HasMember("queueSize")) {
                retConfig.queueSize = static_cast< std::size_t >(d["queueSize"].GetUint());
            }

            if (d.HasMember("internalLogLevel")) {
                retConfig.internalLogLevel = static_cast< std::int8_t >(d["internalLogLevel"].GetInt());
            }
        }
    } else {
        std::ignore = LOGERROR("config is wrong ,using default");
    }
    LOGVERBOSE(__func__) << " leave  ";
    return retConfig;
}

void Daemon::OnCmdEvent(std::string const &jsonstr) noexcept
{
    LOGVERBOSE(__func__) << jsonstr;

    rapidjson::Document d;
    if (d.Parse(jsonstr.c_str()).HasParseError()) {
        std::ignore = LOGERROR("OnCmdEvent parse error");
    } else {
        bool const isCmdInt{d["cmd"].IsInt()};
        if (d.HasMember("cmd") && isCmdInt) {
            std::uint32_t const cmd{d["cmd"].GetUint()};
            switch (static_cast< DltServices >(cmd)) {
                case DltServices::kDlt_Service_Id_Set_Log_Level: {
                    std::ignore = LOGVERBOSE("command  DltServices::kDlt_Service_Id_Set_Log_Level");
                    std::string const appid{d["appid"].GetString()};
                    if (mLogCommandTrans_) {
                        char *const datastr{const_cast< char * >(jsonstr.c_str())};
                        std::ignore = mLogCommandTrans_->SendByAppid(appid, reinterpret_cast< uint8_t * >(datastr),
                                                                     jsonstr.length());
                    }
                    break;
                }
                case DltServices::kDlt_Service_Id_Store_Config:
                case DltServices::kDlt_Service_Id_Reset_To_Factory_Default:
                case DltServices::kDlt_Service_Id_Set_Default_Log_Level: {
                    std::ignore = LOGVERBOSE("command  DltServices::kDlt_Service_Id_Set_Default_Log_Level");
                    if (mLogCommandTrans_) {
                        char *const datastr{const_cast< char * >(jsonstr.c_str())};
                        std::ignore
                            = mLogCommandTrans_->SendMessage(reinterpret_cast< uint8_t * >(datastr), jsonstr.length());
                    }

                    break;
                }

                default: {
                    break;
                }
            }
        }
    }
    LOGVERBOSE(__func__) << " leave  ";
}

void Daemon::OnAllLogLevel(std::uint8_t const &loglevel) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    std::ignore   = loglevel;
    mIsAllLogSet_ = true;
    LOGVERBOSE(__func__) << " leave  ";
}

void Daemon::OnAppLogLevel(std::string const &apppID, std::uint8_t const &loglevel) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    std::ignore   = apppID;
    std::ignore   = loglevel;
    mIsAllLogSet_ = false;
    LOGVERBOSE(__func__) << " leave  ";
}

void Daemon::OnChannelLogLevel(std::string const &apppID,
                               std::string const &contextID,
                               std::uint8_t const &loglevel) noexcept
{
    std::ignore   = apppID;
    std::ignore   = contextID;
    std::ignore   = loglevel;
    mIsAllLogSet_ = false;
}

}  // namespace internal
}  // namespace log
}  // namespace ara
