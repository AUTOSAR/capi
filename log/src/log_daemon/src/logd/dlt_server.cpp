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
/// @file       dlt_server.cpp
/// @brief      Template class for encapsulating log parameters
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
/// @unit_name = dlt_server
/// @unit_description=Backend of Dlt module, used to support log and command forwarding.
/// @endcode
///
/// ================================================================

#include "dlt_server.h"

#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/writer.h>
#include <sys/time.h>

#include <functional>
#include <memory>
#include <thread>

#include "Utils/src/private_log.h"
#include "Utils/src/usercommand_define.h"
#include "isoft/ara_fsh/platform.h"
#include "isoft/ipccpp/debug.h"
#include "isoft/ipccpp/utility.h"
#include "log_lib/include/ara/log/internal/dlt_helpers.h"
#include "log_lib/include/ara/log/internal/dlt_services.h"
#include "log_lib/include/ara/log/internal/dlt_structures.h"

using ara::log::internal::DltServices;

namespace ara {
namespace log {
namespace internal {
/// @brief  Command definition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00299
/// @trace_id_dd=DD_LOG_01704
/// @needwork = dd
/// @endcode
static std::uint8_t const kDlt_Service_Response_Not_Supported{0x01U};

std::int32_t DltServer::Init(std::string const &ecuId,
                             std::string const &ip,
                             std::uint16_t const &port,
                             std::size_t const &bufferSize,
                             Listener *const lis) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    this->mEcuId_ = ecuId;
    mListener_    = lis;

    buffer_     = std::make_shared< LogBuffer >(bufferSize);
    mTcpServer_ = std::make_shared< TcpServer >(ip, port, this);

    LOGVERBOSE(__func__) << " leave  ";
    return mTcpServer_->Open();
}

std::int32_t DltServer::Destroy() noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    std::int32_t const ret{0};

    if (mTcpServer_ != nullptr) {
        mTcpServer_->CloseStream();
    }
    LOGVERBOSE(__func__) << " leave  ";
    return ret;
}

std::int32_t DltServer::ProcessLogClientData(std::int32_t const &sock, DltMessage *const msg) noexcept
{
    LOGVERBOSE(__func__) << "msg->datasize " << std::int32_t(msg->datasize);
    std::uint32_t id{0U};
    std::uint32_t idTmp{0U};
    DltStandardHeaderExtra extra;

    if (msg->datasize < sizeof(uint32_t)) {
        return -1;
    }

    extra = msg->headerExtra;
    idTmp = *(reinterpret_cast< uint32_t * >(msg->databuffer));
    if (((msg->standardheader->htyp) & kDlt_Htyp_Msbf) > 0U) {
        id = ntohl(idTmp);
    } else {
        id = idTmp;
    }
    std::uint16_t const dltServiceIdCallswCinjection{0xFFFU};
    if ((id > static_cast< std::uint32_t >(DltServices::kDlt_Service_Id)) && (id < dltServiceIdCallswCinjection)) {
        switch (id) {
            case static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Get_Software_Version): {
                _ProcessControlgetSoftwareVersion(sock);
                break;
            }

            case static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Set_Default_Log_Level): {
                DltServiceSetDefaultLogLevel *req{nullptr};
                if (msg->datasize < sizeof(DltServiceSetDefaultLogLevel)) {
                    _ProcessControlResponse(sock, id, kDlt_Service_Response_Not_Supported);
                }
                req = reinterpret_cast< DltServiceSetDefaultLogLevel * >(msg->databuffer);
                if (req != nullptr) {
                    this->defaultlogLevel_ = req->logLevel;

                    if (mListener_ != nullptr) {
                        mListener_->OnCmdEvent(_CmdTojsonArgInt(id, req->logLevel));
                    }
                    if (mListener_ != nullptr) {
                        mListener_->OnAllLogLevel(req->logLevel);
                    }
                    _ProcessControlResponse(sock, id, 0U);
                } else {
                    _ProcessControlResponse(sock, id, kDlt_Service_Response_Not_Supported);
                }
                break;
            }
            case static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Get_Default_Log_Level): {
                _ProcessGetDefaultLogLevel(sock);
                break;
            }
            case static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Set_Log_Level): {
                _ProcessSetLogLevel(sock, msg);
                break;
            }
            case static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Store_Config):
            case static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Reset_To_Factory_Default): {
                if (mListener_ != nullptr) {
                    mListener_->OnCmdEvent(_CmdTojson(id));
                    _ProcessControlResponse(sock, id, 0U);
                } else {
                    _ProcessControlResponse(sock, id, kDlt_Service_Response_Not_Supported);
                }
                break;
            }

            // case DLT_SERVICE_ID_GET_LOCAL_TIME
            case static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Set_Timing_Packets): {
                _ProcessControlResponse(sock, id, kDlt_Service_Response_Not_Supported);
                break;
            }

            case static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Get_Log_Info): {
                _ProcessGetLogInfo(sock, msg);
                break;
            }

            default: {
                _ProcessControlResponse(sock, id, kDlt_Service_Response_Not_Supported);
                break;
            }
        }
    }
    LOGVERBOSE(__func__) << " leave  ";
    return 0;
}

std::int32_t DltServer::_SendBufferBackToClient(std::int32_t const &sock,
                                                void *const dataBuffer,
                                                std::int32_t const &messageSize) noexcept
{
    LOGVERBOSE(__func__) << " enter[  ";
    ssize_t dataSend{0};
    while (dataSend < messageSize) {
        ssize_t const ret{send(sock, static_cast< uint8_t * >(dataBuffer) + dataSend, messageSize - dataSend, 0)};
        if (ret < 0) {
            return -1;
        }
        dataSend += ret;
    }
    LOGVERBOSE(__func__) << " leave  ";
    return 0;
}

std::int32_t DltServer::_SocketSendMsgToClient(std::int32_t const &sock,
                                               void *const data1,
                                               std::int32_t const &size1,
                                               void *const data2,
                                               std::int32_t const &size2,
                                               bool const &serialheader) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    std::int32_t ret{0};
    if (serialheader) {
        std::int8_t const dltSerialHeader[kDlt_Id_Size]{'D', 'L', 'S', 1};
        ret = _SendBufferBackToClient(sock, const_cast< std::int8_t * >(dltSerialHeader),
                                      static_cast< int32_t >(sizeof(dltSerialHeader)));
        if (ret != 0) {
            return ret;
        }
    }
    if ((data1 != nullptr) && (size1 > 0)) {
        ret = _SendBufferBackToClient(sock, data1, size1);
        if (ret != 0) {
            return ret;
        }
    }
    if ((data2 != nullptr) && (size2 > 0)) {
        ret = _SendBufferBackToClient(sock, data2, size2);
    }
    LOGVERBOSE(__func__) << " leave  ";
    return ret;
}

std::int32_t DltServer::_SendControlMsg(std::int32_t const &sock,
                                        DltMessage *const msg,
                                        std::string const &apid,
                                        std::string const &ctid) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    int32_t len{0};

    if ((msg == nullptr)) {
        return -1;
    }

    msg->standardheader       = reinterpret_cast< DltStandardHeader * >(msg->headerbuffer);
    msg->standardheader->htyp = kDlt_Htyp_Weid | kDlt_Htyp_Wtms | kDlt_Htyp_Ueh | kDlt_Htyp_Protocol_Version_N1;

#if (BYTE_ORDER == BIG_ENDIAN)
    msg->standardheader->htyp = (msg->standardheader->htyp | kDlt_Htyp_Msbf);
#endif

    msg->standardheader->mcnt = 0U;

    /// TODO Need ECUDID
    std::ignore = memcpy(static_cast< void * >(msg->headerExtra.ecu), mEcuId_.data(), kDlt_Id_Size);

    struct timeval tv
    {
    };
    std::ignore = gettimeofday(&tv, nullptr);
    // 0.1 ms PRS_Dlt_00309
    std::int32_t const toMs{10000};
    std::int32_t const toNs{100000};
    msg->headerExtra.tmsp = tv.tv_sec * toMs + tv.tv_usec / toNs;

    if (DLT_IS_HTYP_WEID(msg->standardheader->htyp)) {
        std::ignore = memcpy(msg->headerbuffer + sizeof(DltStandardHeader), static_cast< void * >(msg->headerExtra.ecu),
                             kDlt_Id_Size);
    }
    std::size_t weidOffset{0U};
    std::size_t wsidOffset{0U};
    if (DLT_IS_HTYP_WEID(msg->standardheader->htyp)) {
        weidOffset = kDlt_Size_Weid;
    }
    if (DLT_IS_HTYP_WSID(msg->standardheader->htyp)) {
        wsidOffset = kDlt_Size_Wsid;
    }

    if (DLT_IS_HTYP_WSID(msg->standardheader->htyp)) {
        msg->headerExtra.seid = htobe32(msg->headerExtra.seid);
        std::ignore = memcpy(msg->headerbuffer + sizeof(DltStandardHeader) + weidOffset, &(msg->headerExtra.seid),
                             kDlt_Size_Wsid);
    }

    if (DLT_IS_HTYP_WTMS(msg->standardheader->htyp)) {
        msg->headerExtra.tmsp = htobe32(msg->headerExtra.tmsp);
        std::ignore           = memcpy(msg->headerbuffer + sizeof(DltStandardHeader) + weidOffset + wsidOffset,
                             &(msg->headerExtra.tmsp), kDlt_Size_Wtms);
    }

    msg->extendedheader = reinterpret_cast< DltExtendedHeader * >(
        msg->headerbuffer + sizeof(DltStandardHeader) + DLT_STANDARD_HEADER_EXTRA_SIZE(msg->standardheader->htyp));

    std::uint8_t const localntRes{0x26U};

    msg->extendedheader->msin = localntRes;

    msg->extendedheader->noar = 1U;

    if (apid.empty()) {
        std::ignore = std::memcpy(static_cast< void * >(msg->extendedheader->apid), "DCA0", kDlt_Id_Size);
    } else {
        std::ignore = std::memcpy(static_cast< void * >(msg->extendedheader->apid), apid.data(), apid.size());
    }
    if (ctid.empty()) {
        std::ignore = std::memcpy(static_cast< void * >(msg->extendedheader->ctid), "CC10", kDlt_Id_Size);
    } else {
        std::ignore = std::memcpy(static_cast< void * >(msg->extendedheader->ctid), ctid.data(), ctid.size());
    }

    msg->headersize = static_cast< uint32_t >(sizeof(DltStandardHeader) + sizeof(DltExtendedHeader)
                                              + DLT_STANDARD_HEADER_EXTRA_SIZE(msg->standardheader->htyp));

    len = static_cast< std::int32_t >(msg->headersize + msg->datasize);

    if (len > UINT16_MAX) {
        return -1;
    }

    msg->standardheader->len = htobe16(((uint16_t)len));
    LOGVERBOSE(__func__) << " leave  ";
    return _SocketSendMsgToClient(sock, static_cast< void * >(msg->headerbuffer),
                                  static_cast< std::int32_t >(msg->headersize), msg->databuffer,
                                  static_cast< std::int32_t >(msg->datasize), false);
}

void DltServer::_ProcessControlResponse(std::int32_t const &sock,
                                        std::uint32_t const &serviceId,
                                        std::uint8_t const &status) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    DltMessage msg;
    DltServiceResponse *resp{nullptr};

    if (_InitDltMessage(&msg) == -1) {
        return;
    }

    msg.datasize = sizeof(DltServiceResponse);
    if ((msg.databuffer != nullptr) && (msg.databuffersize < msg.datasize)) {
        free(msg.databuffer);
        msg.databuffer = nullptr;
    }
    if (msg.databuffer == nullptr) {
        msg.databuffer     = static_cast< uint8_t * >(malloc(msg.datasize));
        msg.databuffersize = msg.datasize;
    }
    if (msg.databuffer == nullptr) {
        return;
    }

    resp            = reinterpret_cast< DltServiceResponse * >(msg.databuffer);
    resp->serviceId = serviceId;
    resp->status    = status;

    std::ignore = _SendControlMsg(sock, &msg, "", "");

    _FreeDltMessage(&msg);
    LOGVERBOSE(__func__) << " leave  ";
}

void DltServer::_ProcessControlgetSoftwareVersion(std::int32_t const &sock) noexcept
{
    LOGVERBOSE(__func__) << ": enter";
    DltMessage msg;
    std::uint32_t len{0U};
    DltServiceGetSoftwareVersionResponse *resp{nullptr};
    if (_InitDltMessage(&msg) == -1) {
        _ProcessControlResponse(sock, static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Get_Software_Version),
                                kDlt_Service_Response_Not_Supported);
        return;
    }
    /// TODO Need a system macro definition
    std::string const ecuVersion{"1.0.0"};
    len = static_cast< std::uint32_t >(ecuVersion.size());
    msg.datasize
        = static_cast< std::uint32_t >(sizeof(std::uint32_t) + sizeof(std::uint8_t) + sizeof(std::uint32_t) + len);

    if ((msg.databuffer != nullptr) && (msg.databuffersize < msg.datasize)) {
        free(msg.databuffer);
        msg.databuffer = nullptr;
    }

    if (msg.databuffer == nullptr) {
        msg.databuffer     = static_cast< std::uint8_t * >(malloc(msg.datasize));
        msg.databuffersize = msg.datasize;
    }

    if (msg.databuffer == nullptr) {
        _ProcessControlResponse(sock, static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Get_Software_Version),
                                kDlt_Service_Response_Not_Supported);
        return;
    }

    resp            = reinterpret_cast< DltServiceGetSoftwareVersionResponse * >(msg.databuffer);
    resp->serviceId = static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Get_Software_Version);
    resp->status    = 0;
    resp->length    = len;
    std::ignore     = memcpy(msg.databuffer + msg.datasize - len, ecuVersion.data(), len);
    std::ignore     = _SendControlMsg(sock, &msg, "", "");
    _FreeDltMessage(&msg);
    LOGVERBOSE(__func__) << " leave";
}

void DltServer::_ProcessGetDefaultLogLevel(std::int32_t const &sock) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    DltMessage msg;
    DltServiceGetDefaultLogLevelResponse *resp{nullptr};
    if (_InitDltMessage(&msg) == -1) {
        _ProcessControlResponse(sock, static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Get_Default_Log_Level),
                                kDlt_Service_Response_Not_Supported);
        return;
    }

    msg.datasize = sizeof(DltServiceGetDefaultLogLevelResponse);

    if ((msg.databuffer != nullptr) && (msg.databuffersize < msg.datasize)) {
        free(msg.databuffer);
        msg.databuffer = nullptr;
    }
    if (msg.databuffer == nullptr) {
        msg.databuffer     = static_cast< uint8_t * >(malloc(msg.datasize));
        msg.databuffersize = msg.datasize;
    }

    if (msg.databuffer == nullptr) {
        _ProcessControlResponse(sock, static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Get_Default_Log_Level),
                                kDlt_Service_Response_Not_Supported);
        return;
    }

    resp            = reinterpret_cast< DltServiceGetDefaultLogLevelResponse * >(msg.databuffer);
    resp->serviceId = static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Get_Default_Log_Level);
    resp->status    = 0;
    resp->logLevel  = defaultlogLevel_;

    std::ignore = _SendControlMsg(sock, &msg, "", "");
    _FreeDltMessage(&msg);
    LOGVERBOSE(__func__) << " leave  ";
}

void DltServer::_ProcessGetLogInfo(std::int32_t const &sock, DltMessage *const msg) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    DltServiceGetLogInfoRequest *req{nullptr};
    DltMessage resp;

    std::uint16_t const countAppIds{static_cast< std::uint16_t >(aainfos_->size())};
    std::size_t offset{0U};
    std::uint32_t const sid{static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Get_Log_Info)};
    req = reinterpret_cast< DltServiceGetLogInfoRequest * >(msg->databuffer);

    std::int8_t const uIntSix{6};
    std::int8_t const uIntSeven{7};
    std::int8_t const uIntEight{8};

    std::uint8_t const uIntTwo{2U};
    std::uint8_t const uIntThree{3U};
    std::size_t const uIntFour{4U};

    if (req->options == uIntSix) {
        resp.datasize       = 0U;
        resp.databuffer     = static_cast< uint8_t * >(malloc(kCommand_Read_Buffer));
        resp.databuffersize = kCommand_Read_Buffer;
        std::ignore         = memset(resp.databuffer, 0, kCommand_Read_Buffer);
        // service id
        std::ignore = memcpy(resp.databuffer, &sid, sizeof(uint32_t));
        offset += sizeof(std::uint32_t);
        /// status
        std::int8_t status{0};
        if (aainfos_->empty() == false) {
            status = uIntSix;
        } else {
            status = uIntEight;
        }

        std::ignore = memcpy(resp.databuffer + offset, &status, sizeof(int8_t));
        offset += sizeof(int8_t);

        std::ignore = memcpy(resp.databuffer + offset, &countAppIds, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        for (AAClientInfoMap::iterator it{aainfos_->begin()}; it != aainfos_->end(); it++) {
            AAClientChannelMap &mpptr{it->second->channels};
            std::ignore = memcpy(resp.databuffer + offset, it->first.c_str(), kDlt_Id_Size);
            offset += kDlt_Id_Size;
            std::uint16_t const ctxids{static_cast< std::uint16_t >(mpptr.size())};
            std::ignore = memcpy(resp.databuffer + offset, &ctxids, sizeof(std::uint16_t));
            offset += sizeof(std::uint16_t);
            for (AAClientChannelMap::iterator chiter{mpptr.begin()}; chiter != mpptr.end(); chiter++) {
                std::ignore = memcpy(resp.databuffer + offset, chiter->first.c_str(), kDlt_Id_Size);
                offset += kDlt_Id_Size;
                std::int8_t const ll{static_cast< std::int8_t >(chiter->second->logLevel)};
                std::int8_t const ts{1};
                std::ignore = memcpy(resp.databuffer + offset, &ll, sizeof(std::int8_t));
                offset += sizeof(std::int8_t);
                std::ignore = memcpy(resp.databuffer + offset, &ts, sizeof(std::int8_t));
                offset += sizeof(std::int8_t);
            }
        }
        std::uint8_t const rCharType{static_cast< uint8_t >('r')};
        std::uint8_t const eCharType{static_cast< uint8_t >('e')};
        std::uint8_t const mCharType{static_cast< uint8_t >('m')};
        std::uint8_t const oCharType{static_cast< uint8_t >('o')};
        resp.databuffer[offset + 0U]        = rCharType;
        resp.databuffer[offset + 1U]        = eCharType;
        resp.databuffer[offset + uIntTwo]   = mCharType;
        resp.databuffer[offset + uIntThree] = oCharType;
        resp.datasize                       = offset + uIntFour;
        std::ignore                         = _SendControlMsg(sock, &resp, "", "");
        _FreeDltMessage(&resp);

    } else if (req->options == uIntSeven) {
        std::size_t buffersize{0U};
        for (AAClientInfoMap::iterator it{aainfos_->begin()}; it != aainfos_->end(); it++) {
            buffersize += it->second->appId.size();
            buffersize += it->second->appDesc.size();
            AAClientChannelMap &mpptr{it->second->channels};
            for (AAClientChannelMap::iterator cit{mpptr.begin()}; cit != mpptr.end(); cit++) {
                buffersize += cit->second->contextId.size();
                buffersize += cit->second->contextDescription.size();
            }
        }

        if (buffersize > kCommand_Read_Buffer) {
            std::uint32_t const getLogInfoStatusRespDataOverflow{9U};
            _ProcessControlResponse(sock, static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Get_Log_Info),
                                    getLogInfoStatusRespDataOverflow);
            return;
        }
        resp.datasize       = 0U;
        resp.databuffer     = static_cast< uint8_t * >(malloc(static_cast< size_t >(kCommand_Read_Buffer)));
        resp.databuffersize = kCommand_Read_Buffer;
        std::ignore         = memset(resp.databuffer, 0, kCommand_Read_Buffer);
        // service id
        std::ignore = memcpy(resp.databuffer, &sid, sizeof(std::uint32_t));
        offset += sizeof(std::uint32_t);
        /// status
        std::int8_t status{0};
        if (!aainfos_->empty()) {
            status = req->options;
        } else {
            status = uIntEight;
        }

        std::ignore = memcpy(resp.databuffer + offset, &status, sizeof(int8_t));
        offset += sizeof(int8_t);
        // Number of apps
        std::ignore = memcpy(resp.databuffer + offset, &countAppIds, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        for (AAClientInfoMap::iterator it{aainfos_->begin()}; it != aainfos_->end(); it++) {
            std::ignore = memcpy(resp.databuffer + offset, it->first.c_str(), kDlt_Id_Size);
            offset += kDlt_Id_Size;
            AAClientChannelMap &mpptr{it->second->channels};
            std::uint16_t const ctxids{static_cast< std::uint16_t >(mpptr.size())};
            std::ignore = memcpy(resp.databuffer + offset, &ctxids, sizeof(std::uint16_t));
            offset += sizeof(std::uint16_t);
            for (AAClientChannelMap::iterator chiter{mpptr.begin()}; chiter != mpptr.end(); chiter++) {
                std::ignore = memcpy(resp.databuffer + offset, chiter->first.c_str(), kDlt_Id_Size);
                offset += kDlt_Id_Size;
                std::uint8_t const ll{chiter->second->logLevel};
                std::uint8_t const ts{1U};
                std::ignore = memcpy(resp.databuffer + offset, &ll, sizeof(std::uint8_t));
                offset += sizeof(std::uint8_t);
                std::ignore = memcpy(resp.databuffer + offset, &ts, sizeof(std::uint8_t));
                offset += sizeof(std::uint8_t);

                std::uint16_t const contentdeslen{
                    static_cast< std::uint16_t >(chiter->second->contextDescription.length())};
                std::ignore = memcpy(resp.databuffer + offset, &contentdeslen, sizeof(std::uint16_t));
                offset += sizeof(std::uint16_t);

                std::ignore
                    = memcpy(resp.databuffer + offset, chiter->second->contextDescription.c_str(), contentdeslen);
                offset += contentdeslen;
            }
            /// App description
            std::uint16_t const appdeslen{static_cast< std::uint16_t >(it->second->appDesc.length())};
            std::ignore = memcpy(resp.databuffer + offset, &appdeslen, sizeof(std::uint16_t));
            offset += sizeof(std::uint16_t);
            std::ignore = memcpy(resp.databuffer + offset, it->second->appDesc.c_str(), appdeslen);
            offset += appdeslen;
        }
        std::uint8_t const rCharType{static_cast< uint8_t >('r')};
        std::uint8_t const eCharType{static_cast< uint8_t >('e')};
        std::uint8_t const mCharType{static_cast< uint8_t >('m')};
        std::uint8_t const oCharType{static_cast< uint8_t >('o')};
        resp.databuffer[offset + 0U]        = rCharType;
        resp.databuffer[offset + 1U]        = eCharType;
        resp.databuffer[offset + uIntTwo]   = mCharType;
        resp.databuffer[offset + uIntThree] = oCharType;
        resp.datasize                       = offset + uIntFour;

        std::ignore = _SendControlMsg(sock, &resp, "", "");
        _FreeDltMessage(&resp);

    } else {
        _ProcessControlResponse(sock, static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Get_Log_Info),
                                kDlt_Service_Response_Not_Supported);
    }
    LOGVERBOSE(__func__) << " leave  ";
}

void DltServer::_ProcessSetLogLevel(std::int32_t const &sock, DltMessage *const msg)
{
    LOGVERBOSE(__func__) << " enter ";

    DltServiceSetLogLevel *req{nullptr};

    req = reinterpret_cast< DltServiceSetLogLevel * >(msg->databuffer);
    std::uint32_t const serviceid{static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Set_Log_Level)};
    std::string const apid(reinterpret_cast< Char8_T * >(req->apid), kDlt_Id_Size);
    std::string const ctid(reinterpret_cast< Char8_T * >(req->ctid), kDlt_Id_Size);

    std::int8_t const apidLength{static_cast< std::int8_t >(strlen(apid.c_str()))};
    std::int8_t const ctidLength{static_cast< std::int8_t >(strlen(ctid.c_str()))};

    if ((apidLength == 0) && (ctidLength == 0)) {
        if (mListener_ != nullptr) {
            mListener_->OnCmdEvent(_CmdTojsonArgInt(
                static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Set_Default_Log_Level), req->logLevel));
        }

    } else if ((apidLength != 0) && (ctidLength == 0)) {
        std::ignore = LOGVERBOSE("only appid ") << apid;
        if (mListener_ != nullptr) {
            mListener_->OnAppLogLevel(apid, req->logLevel);
        }

        rapidjson::StringBuffer buf;
        rapidjson::Writer< rapidjson::StringBuffer > writer{buf};
        std::ignore = writer.StartObject();
        std::ignore = writer.Key("cmd");
        std::ignore = writer.Uint(static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Set_Log_Level));
        std::ignore = writer.Key("appid");
        std::ignore = writer.String(apid.c_str());
        std::ignore = writer.Key("loglevel");
        std::ignore = writer.Int(req->logLevel);
        std::ignore = writer.EndObject();
        if (mListener_ != nullptr) {
            mListener_->OnCmdEvent(std::string(buf.GetString()).append("\n"));
        }

    } else if ((apidLength != 0) && (ctidLength != 0)) /*only app id case*/
    {
        LOGVERBOSE(" have  appid:  ") << apid << "  contextid : " << ctid;
        if (mListener_ != nullptr) {
            mListener_->OnChannelLogLevel(apid, ctid, req->logLevel);
        }

        rapidjson::StringBuffer buf;
        rapidjson::Writer< rapidjson::StringBuffer > writer{buf};
        std::ignore = writer.StartObject();
        std::ignore = writer.Key("cmd");
        std::ignore = writer.Uint(static_cast< std::uint32_t >(DltServices::kDlt_Service_Id_Set_Log_Level));
        std::ignore = writer.Key("appid");
        std::ignore = writer.String(apid.c_str());
        std::ignore = writer.Key("contextid");
        std::ignore = writer.String(ctid.c_str());
        std::ignore = writer.Key("loglevel");
        std::ignore = writer.Int(req->logLevel);
        std::ignore = writer.EndObject();
        if (mListener_ != nullptr) {
            mListener_->OnCmdEvent(std::string(buf.GetString()).append("\n"));
        }
    } else {
        _ProcessControlResponse(sock, serviceid, kDlt_Service_Response_Not_Supported);
        return;
    }
    _ProcessControlResponse(sock, serviceid, 0U);
    LOGVERBOSE(__func__) << " leave  ";
}

std::int32_t DltServer::_InitDltMessage(DltMessage *const msg)
{
    LOGVERBOSE(__func__) << " enter ";
    if (msg == nullptr) {
        return -1;
    }

    msg->headersize = 0U;
    msg->datasize   = 0U;

    msg->databuffer     = nullptr;
    msg->databuffersize = 0U;

    msg->standardheader = nullptr;
    msg->extendedheader = nullptr;

    msg->foundSerialheader = 0;
    LOGVERBOSE(__func__) << " leave  ";
    return 0;
}

void DltServer::_FreeDltMessage(DltMessage *const msg)
{
    LOGVERBOSE(__func__) << " enter ";
    if (msg->databuffer != nullptr) {
        free(msg->databuffer);
        msg->databuffer     = nullptr;
        msg->databuffersize = 0U;
    }
    LOGVERBOSE(__func__) << " leave  ";
}

std::string DltServer::_CmdTojson(std::uint32_t const &cmdid) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    std::string retstr{};
    rapidjson::StringBuffer buf;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buf};
    std::ignore = writer.StartObject();
    std::ignore = writer.Key("cmd");
    std::ignore = writer.Uint(cmdid);
    std::ignore = writer.EndObject();
    LOGVERBOSE(__func__) << " leave  ";
    return retstr.append(buf.GetString()).append("\n");
}

std::string DltServer::_CmdTojsonArgInt(std::uint32_t const &cmdid, std::int32_t const &arg) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    std::string retstr{};
    rapidjson::StringBuffer buf;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buf};
    std::ignore = writer.StartObject();
    std::ignore = writer.Key("cmd");
    std::ignore = writer.Uint(cmdid);
    std::ignore = writer.Key("data");
    std::ignore = writer.Int(arg);
    std::ignore = writer.EndObject();
    LOGVERBOSE(__func__) << " leave  ";
    return retstr.append(buf.GetString()).append("\n");
}

void DltServer::SetAAInfos(std::shared_ptr< AAClientInfoMap > const &infoPtr)
{
    LOGVERBOSE(__func__) << " enter ";
    aainfos_ = infoPtr;
    LOGVERBOSE(__func__) << " leave  ";
}

void DltServer::OnCommandMessage(std::int32_t const &clientFd, DltMessage *msg)
{
    LOGVERBOSE(__func__) << " enter ";
    std::ignore = ProcessLogClientData(clientFd, msg);
    LOGVERBOSE(__func__) << " leave  ";
}

void DltServer::OnSocketStatus(bool const &connected) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    bool const iSLen{buffer_->Len() > 0U};
    bool const clientOK{ClientStates() == 1};
    if (connected && iSLen && clientOK) {
        std::lock_guard< decltype(bufferLocker_) > const scopeLockGud{bufferLocker_};
        std::ignore = mTcpServer_->Send(buffer_->Data(), buffer_->Len());
        buffer_->Clear();
    }
    if (ClientStates() > 0) {
        if (mListener_ != nullptr) {
            mListener_->OnCmdEvent(_CmdTojson(kClientConncted));
        }
    } else {
        if (mListener_ != nullptr) {
            mListener_->OnCmdEvent(_CmdTojson(kClientDisConncted));
        }
    }
    LOGVERBOSE(__func__) << " leave  ";
}

std::int32_t DltServer::SendLogMessage(uint8_t *const buffer, std::size_t const &len) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    bool const clientOK{ClientStates() > 0};
    if ((mTcpServer_ != nullptr) && clientOK) {
        std::ignore = mTcpServer_->Send(buffer, len);
    } else {
        std::lock_guard< decltype(bufferLocker_) > const lockGd{bufferLocker_};
        if (buffer_->IsFull(len)) {
            buffer_->Clear();
        }
        std::ignore = buffer_->Push(buffer, len);
    }
    LOGVERBOSE(__func__) << " leave  ";
    return 0;
}

std::int32_t DltServer::ClientStates() const noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    return mTcpServer_->ClientCount();
}

void DltServer::_OnChannelData(std::int32_t const &msgType,
                               uint8_t const *const mesg,
                               std::size_t const &mesgSize) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    if (mesg != nullptr) {
        LOGVERBOSE(__func__) << msgType << mesgSize;
    }
    LOGVERBOSE(__func__) << " leave  ";
}

std::int32_t DltServer::OnSocketRead(std::int32_t const &socketfd) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    if (socketfd > 0) {
        std::shared_ptr< DltCommandReceiver > reciver{nullptr};
        if (recivers_.count(socketfd) > 0U) {
            reciver = recivers_.at(socketfd);
        } else {
            reciver     = std::make_shared< DltCommandReceiver >(this);
            std::ignore = recivers_.emplace(std::make_pair(socketfd, reciver));
            std::uint32_t const socketReadBufferSize{1024U * 4U};
            reciver->Init(socketfd, DltReceiverType::kDlt_Receive_Socket, socketReadBufferSize);
        }

        if (reciver == nullptr) {
            return -1;
        }
        bool const readOk{reciver->DataToBuffer()};
        if (!readOk) {
            LOGVERBOSE(__func__) << " socket disconnected or read failed, fd: " << socketfd;
            std::ignore = recivers_.erase(socketfd);
            return -1;
        }
    }
    LOGVERBOSE(__func__) << " leave  ";
    return 0;
}
void DltServer::_updateLoglevel(std::uint8_t const &loglevel)
{
    LOGVERBOSE(__func__) << " enter ";
    for (AAClientInfoMap::iterator it{aainfos_->begin()}; it != aainfos_->end(); it++) {
        AAClientChannelMap &mpptr{it->second->channels};
        for (AAClientChannelMap::iterator chiter{mpptr.begin()}; chiter != mpptr.end(); chiter++) {
            chiter->second->logLevel = (loglevel);
        }
    }
    LOGVERBOSE(__func__) << " leave  ";
}
void DltServer::_updateLoglevel(std::string const &appid, std::uint8_t const &loglevel) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    for (AAClientInfoMap::iterator it{aainfos_->begin()}; it != aainfos_->end(); it++) {
        AAClientChannelMap &mpptr{it->second->channels};
        if (appid == it->first) {
            for (AAClientChannelMap::iterator chiter{mpptr.begin()}; chiter != mpptr.end(); chiter++) {
                chiter->second->logLevel = (loglevel);
            }
        }
    }
    LOGVERBOSE(__func__) << " leave  ";
}
void DltServer::_updateLoglevel(std::string const &appid,
                                std::string const &ctxid,
                                std::uint8_t const &loglevel) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    for (AAClientInfoMap::iterator it{aainfos_->begin()}; it != aainfos_->end(); it++) {
        AAClientChannelMap &mpptr{it->second->channels};
        if (appid == it->first) {
            for (AAClientChannelMap::iterator chiter{mpptr.begin()}; chiter != mpptr.end(); chiter++) {
                if (chiter->first == ctxid) {
                    chiter->second->logLevel = (loglevel);
                }
            }
        }
    }
    LOGVERBOSE(__func__) << " leave  ";
}

}  // namespace internal

}  // namespace log

}  // namespace ara
