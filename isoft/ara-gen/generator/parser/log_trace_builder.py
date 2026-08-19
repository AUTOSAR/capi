#  Disclaimer
#
#  This work (specification and/or software implementation) and the material
#  contained in it, as released by AUTOSAR, is for the purpose of information
#  only. AUTOSAR and the companies that have contributed to it shall not be
#  liable for any use of the work.
#
#  The material contained in this work is protected by copyright and other
#  types of intellectual property rights. The commercial exploitation of the
#  material contained in this work requires a license to such intellectual
#  property rights.
#
#  This work may be utilized or reproduced without any modification, in any
#  form or by any means, for informational purposes only. For any other
#  purpose, no part of the work may be utilized or reproduced, in any form
#  or by any means, without permission in writing from the publisher.
#
#  The work has been developed for automotive applications only. It has
#  neither been developed, nor tested for non-automotive applications.
#
#  The word AUTOSAR and the AUTOSAR logo are registered trademarks.
#  --------------------------------------------------------------------------
from generator.parser import commen_util
from generator.common.tree_helper import get_element_or_none, to_list, get_element_or_die


class LogTraceBuilder:

    def __init__(self, model):
        self.model = model
        self.allFile = {}

    def get_log_config(self, ar_process):
        logTrace = {
            "loggingBehavior": False
        }

        ar_executable_ref = get_element_or_none(ar_process, "EXECUTABLE-REF")
        if ar_executable_ref is not None:
            ar_executable = self.model.find_referable(ar_executable_ref.text)
            ar_execution_state_logging = get_element_or_none(ar_executable, "LOGGING-BEHAVIOR")
            if ar_execution_state_logging:
                uses_logging = ar_execution_state_logging.text.upper() == "USES-LOGGING"
                logTrace["loggingBehavior"] = uses_logging
        dlt_channel_mapping = self.model.find_elements_of_type("DLT-LOG-CHANNEL-TO-PROCESS-MAPPING")
        log_channel_ref_list = []
        for dlt_channel_item in dlt_channel_mapping:
            log_channel_ref = get_element_or_none(dlt_channel_item, "DLT-LOG-CHANNEL-REF")
            process_ref = get_element_or_none(dlt_channel_item, "PROCESS-REF")
            if process_ref is not None and log_channel_ref is not None:
                if process_ref.text == ar_process.get_fqn():
                    log_channel_ref_list.append(log_channel_ref.text)
        machine = commen_util.find_machine_by_process(self, ar_process.get_fqn())
        self.get_channel(machine, logTrace, log_channel_ref_list, False)
        return logTrace

    def get_channel(self, ar_machine, logTrace, log_channel_ref_list, changeBehavior):
        channelOb = self.get_dlt_path_ob(ar_machine)
        channelList = []
        for log_channel_ref in log_channel_ref_list:
            if log_channel_ref is not None:
                dlt_channel = self.model.find_referable(log_channel_ref)
                aid = get_element_or_none(dlt_channel, "APPLICATION-ID")
                if aid is not None:
                    if "appId" not in logTrace:
                        logTrace["appId"] = aid
                    assert aid == logTrace["appId"], "[LOG ] CODE-002: The DltLogChannel.applicationId is inconsistent --{}/{}. LocalPath: {}. ".format(aid, logTrace["appId"], dlt_channel.get_path())
                aDes = get_element_or_none(dlt_channel, "APPLICATION-DESCRIPTION")
                if aDes is not None:
                    if "appDesc" not in logTrace:
                        logTrace["appDesc"] = aDes
                    assert aDes == logTrace["appDesc"], "[LOG ] CODE-001: The DltLogChannel.applicationDescription is inconsistent --{}/{}. LocalPath: {}. ".format(aDes, logTrace["appDesc"], dlt_channel.get_path())

                levelOb = {
                    "OFF": 0,
                    "FATAL": 1,
                    "ERROR": 2,
                    "WARN": 3,
                    "INFO": 4,
                    "DEBUG": 5,
                    "VERBOSE": 6,
                }

                modeOb = {
                    "NETWORK": 1,
                    "FILE": 2,
                    "CONSOLE": 4,
                }

                channel_item = {
                    "_path": dlt_channel.get_fqn()
                }
                modeId = 0
                log_modes = get_element_or_none(dlt_channel, "LOG-TRACE-LOG-MODES")
                if log_modes is not None:
                    mode_map = log_modes.find_elements_of_type("LOG-TRACE-LOG-MODE")
                    for mode in mode_map:
                        modeId = modeId + modeOb[mode.text]
                    channel_item["modes"] = modeId
                if changeBehavior and modeId > 0:
                    logTrace["loggingBehavior"] = True
                level = get_element_or_none(dlt_channel, "LOG-TRACE-DEFAULT-LOG-LEVEL")
                if level is not None:
                    channel_item["level"] = levelOb[level.text]
                else:
                    channel_item["level"] = 0
                filePath = get_element_or_none(dlt_channel, "LOG-TRACE-FILE-PATH")
                if filePath is not None:
                    channel_item["fileName"] = filePath
                    channel_item["fileShared"] = False
                lastFilePath = self.get_file_path(dlt_channel)
                if lastFilePath is not None and lastFilePath in channelOb:
                    channel_item["fileShared"] = len(channelOb[lastFilePath]) > 1

                ctxID = get_element_or_none(dlt_channel, "CONTEXT-ID")
                if ctxID is not None:
                    channel_item["ctxId"] = ctxID
                ctxDesc = get_element_or_none(dlt_channel, "CONTEXT-DESCRIPTION")
                if ctxDesc is not None:
                    channel_item["ctxDesc"] = str(ctxDesc)

                verbose_mode = get_element_or_none(dlt_channel, "NON-VERBOSE-MODE")
                if verbose_mode is not None:
                    channel_item["nonVerboseMode"] = verbose_mode
                else:
                    channel_item["nonVerboseMode"] = True
                session_id = get_element_or_none(dlt_channel, "SESSION-ID")
                if session_id is not None:
                    channel_item["sessionId"] = session_id
                if "level" in channel_item and "ctxId" in channel_item:
                    if channel_item["ctxId"] == "#DFT" and "defaultLevel" not in logTrace:
                        logTrace["defaultLevel"] = channel_item["level"]
                channelList.append(channel_item)

                sdgLogOb = self.get_sdg_log(ar_machine)
                for channel in channelList:
                    if "_path" in channel and channel["_path"] in sdgLogOb:
                        fileOb = sdgLogOb[channel["_path"]]
                        del channel["_path"]
                        for att in fileOb:
                            channel[att] = fileOb[att]

        for channel in channelList:
            if "_path" in channel:
                del channel["_path"]
        logTrace["channels"] = channelList

    def get_file_path(self, channel):
        fileMode = False
        log_modes = get_element_or_none(channel, "LOG-TRACE-LOG-MODES")
        if log_modes is not None:
            mode_map = log_modes.find_elements_of_type("LOG-TRACE-LOG-MODE")
            for mode in mode_map:
                if mode.text == "FILE":
                    fileMode = True
                    break
        filePath = get_element_or_none(channel, "LOG-TRACE-FILE-PATH")
        if filePath is not None and fileMode:
            if not str(filePath).endswith(".dlt"):
                applicationId = get_element_or_none(channel, "APPLICATION-ID")
                if applicationId is not None:
                    filePath = "{}/{}.dlt".format(filePath, applicationId.text).replace("//", "/")
            return str(filePath)

    def get_sdg_log(self, ar_machine):
        sdgLogOb = {}
        if ar_machine is not None:
            sdgs = get_element_or_none(ar_machine, "ADMIN-DATA/SDGS/SDG")
            if sdgs is not None:
                sdgs = to_list(sdgs)
                for sdgItem in sdgs:
                    if "iSOFT:log:DltLogChannelToFileExtensionOperationMapping" == sdgItem.attrib["GID"]:
                        sdgList = get_element_or_none(sdgItem, "SDG")
                        logFileRef = []
                        fileOb = {}
                        if sdgList is not None:
                            for sdg in sdgList:
                                if "iSOFT:log:DltLogChannelToFileExtensionOperationMapping:LogChannel" == sdg.attrib["GID"]:
                                    sdx_ref = get_element_or_none(sdg, "SDX-REF")
                                    for item in to_list(sdx_ref):
                                        logFileRef.append(item.text)
                                elif "iSOFT:log:DltLogChannelToFileExtensionOperationMappingSubclass:Attributes" == sdg.attrib["GID"]:
                                    sd = get_element_or_none(sdg, "SD")
                                    sdList = to_list(sd)
                                    for item in sdList:
                                        if "iSOFT:log:LogFileExtensionOperation:FileBuffer" == item.attrib["GID"]:
                                            fileOb["bufferSize"] = item
                                        elif "iSOFT:log:LogFileExtensionOperation:PlainText" == item.attrib["GID"]:
                                            fileOb["plainText"] = item
                                    ruleSds = get_element_or_none(sdg, "SDG/SD")
                                    if ruleSds is not None:
                                        for ruleSd in ruleSds:
                                            if "iSOFT:log:LogFileRotateRule:FileSize" == ruleSd.attrib["GID"]:
                                                if str(ruleSd) != "":
                                                    fileOb["fileSize"] = ruleSd
                                            elif "iSOFT:log:LogFileRotateRule:FileCount" == ruleSd.attrib["GID"]:
                                                if str(ruleSd) != "":
                                                    fileOb["fileCount"] = ruleSd
                                        if ("fileSize" in fileOb and "fileCount" not in fileOb) or ("fileSize" not in fileOb and "fileCount" in fileOb):
                                            assert False, "[LOG ] CODE-006: ['iSOFT:log:LogFileRotateRule:FileSize', 'iSOFT:log:LogFileRotateRule:FileCount'] need to be configured together.LocalPath:{}".format(sdg.get_path())
                            for ref in logFileRef:
                                if "plainText" not in fileOb:
                                    fileOb["plainText"] = False
                                sdgLogOb[ref] = fileOb
        return sdgLogOb

    def get_log_config_emd(self, ar_machine):
        log_channel_map = ar_machine.find_elements_of_type("DLT-LOG-CHANNEL")
        channel_map = []
        for item in log_channel_map:
            app_id = get_element_or_none(item, "APPLICATION-ID")
            if app_id is not None and app_id.text == "EMD":
                channel_map.append(item.get_fqn())
        if len(channel_map) > 0:
            logTrace = {
                "loggingBehavior": False
            }
            self.get_channel(ar_machine, logTrace, channel_map, True)
            return logTrace
        else:
            return {"logTrace": {
                "modes": [],
                "channels": []
            }}

    def get_dlt_path_ob(self, ar_machine):
        if ar_machine is None:
            return {}
        processList = []
        machine_mapping = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING")
        for machine_mapping_item in machine_mapping:
            process_ref = get_element_or_die(machine_mapping_item, "PROCESS-REF")
            machine_ref = get_element_or_none(machine_mapping_item, "MACHINE-REF")
            if machine_ref is not None and machine_ref.text == ar_machine.get_fqn():
                processList.append(process_ref.text)
        channelOb = {}
        dlt_channel_mapping = self.model.find_elements_of_type("DLT-LOG-CHANNEL-TO-PROCESS-MAPPING")
        for item in dlt_channel_mapping:
            log_channel_ref = get_element_or_none(item, "DLT-LOG-CHANNEL-REF")
            process_ref = get_element_or_none(item, "PROCESS-REF")
            if process_ref is not None and log_channel_ref is not None:
                channelEle = self.model.find_referable(log_channel_ref.text)
                filePath = self.get_file_path(channelEle)
                if filePath is not None:
                    if filePath not in channelOb:
                        channelOb[filePath] = [process_ref.text]
                    elif process_ref.text not in channelOb[filePath]:
                        channelOb[filePath].append(process_ref.text)
        return channelOb

    def get_log_global_config(self, ar_machine):
        logGlobalConfigOb = {}
        logChannelAll = []
        logChannelFilePathOb = {}
        ar_log_trace_instantiation = get_element_or_none(ar_machine, "MODULE-INSTANTIATIONS/LOG-AND-TRACE-INSTANTIATION")
        if ar_log_trace_instantiation is not None:
            ar_log_trace_instantiation = to_list(ar_log_trace_instantiation)
            for log_item in ar_log_trace_instantiation:
                logChannels = log_item.find_elements_of_type("DLT-LOG-CHANNEL")
                logChannelAll.extend(logChannels)
                path = log_item.get_path()
                ar_dlt_ecu_id = get_element_or_none(log_item, "DLT-ECU-ID")
                if ar_dlt_ecu_id is not None:
                    if "ecuId" in logGlobalConfigOb:
                        assert ar_dlt_ecu_id == logGlobalConfigOb["ecuId"], "[LOG ] CODE-003: DltLogChannel.dltEcuId different {}::{}.LocalPath:{}".format(ar_dlt_ecu_id, logGlobalConfigOb["ecuId"], path)
                    else:
                        logGlobalConfigOb["ecuId"] = ar_dlt_ecu_id
                queueSize = get_element_or_none(log_item, "QUEUE-SIZE")
                if queueSize is not None:
                    if "queueSize" in logGlobalConfigOb:
                        assert queueSize == logGlobalConfigOb["queueSize"], "[LOG ] CODE-003: DltLogChannel.queueSize different {}::{}.LocalPath:{}".format(queueSize, logGlobalConfigOb["queueSize"], path)
                    else:
                        logGlobalConfigOb["queueSize"] = queueSize

                sessionIdSupport = get_element_or_none(log_item, "SESSION-ID-SUPPORT")
                if sessionIdSupport is not None:
                    if "sessionIdSupport" in logGlobalConfigOb:
                        assert sessionIdSupport == logGlobalConfigOb["sessionIdSupport"], "[LOG ] CODE-003: DltLogChannel.sessionIdSupport different {}::{}.LocalPath:{}".format(sessionIdSupport, logGlobalConfigOb["sessionIdSupport"], path)
                    else:
                        logGlobalConfigOb["sessionIdSupport"] = sessionIdSupport

                timebaseName = get_element_or_none(log_item, "TIME-BASE-RESOURCE-REF")
                if timebaseName is not None:
                    assert timebaseName.attrib["DEST"] == "SYNCHRONIZED-TIME-BASE-CONSUMER", "[LOG ] CODE-004: LogAndTraceInstantiation[{}].timeBaseResourceRef must be SynchronizedTimeBaseConsumer. FQN:{}. LocalPath:{}. ".format(log_item.SHORT_NAME, log_item.get_fqn(), path)
                    timebaseName = timebaseName.text[1:]
                    if "timebaseName" in logGlobalConfigOb:
                        assert timebaseName == logGlobalConfigOb["timebaseName"], "[LOG ] CODE-003: DltLogChannel.timeBaseResourceRef different {}::{}.LocalPath:{}".format(timebaseName, logGlobalConfigOb["timebaseName"], path)
                    else:
                        logGlobalConfigOb["timebaseName"] = timebaseName

        for channel in logChannelAll:
            filePath = self.get_file_path(channel)
            if filePath is not None:
                if filePath in logChannelFilePathOb:
                    filePathRefs = logChannelFilePathOb[filePath]
                    filePathRefs.append(channel.get_fqn())
                else:
                    logChannelFilePathOb[filePath] = [channel.get_fqn()]

        sdgLogOb = self.get_sdg_log(ar_machine)
        for filePathKey in logChannelFilePathOb:
            refList = logChannelFilePathOb[filePathKey]
            filePathOb = None
            filePathRef = ""
            for refItem in refList:
                if filePathOb is None:
                    filePathRef = refItem
                    if refItem in sdgLogOb:
                        filePathOb = sdgLogOb[refItem]
                    else:
                        filePathOb = {}
                else:
                    if refItem in sdgLogOb:
                        newOb = sdgLogOb[refItem]
                    else:
                        newOb = {}
                    if filePathOb != newOb:
                        item = self.model.find_referable(refItem)
                        localPath = item.get_path()
                        assert False, "[LOG ] CODE-005: The same DltLogChannel.logTraceFilePath({}) must be configured with the same ['iSOFT:log:LogFileExtensionOperation:FileBuffer','iSOFT:log:LogFileRotateRule:FileSize','iSOFT:log:LogFileRotateRule:FileCount'].Path:['{}', '{}'].LocalPath:{}".format(filePathKey, refItem, filePathRef, localPath)

        connectorArray = []
        sdgs = get_element_or_none(ar_machine, "ADMIN-DATA/SDGS/SDG")
        if sdgs is not None:
            sdgs = to_list(sdgs)
            for sdgItem in sdgs:
                if "iSOFT:log:LogDaemonEthernetConfiguration" == sdgItem.attrib["GID"]:
                    sdgList = get_element_or_none(sdgItem, "SDG")
                    if sdgList is not None:
                        for sdg in sdgList:
                            connectorItem = {}
                            if "iSOFT:log:LogDaemonEthernetConfigurationSubclass:Attributes" == sdg.attrib["GID"]:
                                connectorSdg = get_element_or_none(sdg, "SDG")
                                if connectorSdg is not None and "iSOFT:log:EthernetConfigurationUnit:Connector" == connectorSdg.attrib["GID"]:
                                    sdx_ref = get_element_or_none(connectorSdg, "SDX-REF")
                                    if sdx_ref is not None:
                                        connectorEle = self.model.find_referable(sdx_ref.text)
                                        endpoint_ref = get_element_or_none(connectorEle, "UNICAST-NETWORK-ENDPOINT-REF")
                                        if endpoint_ref is not None:
                                            point = self.model.find_referable(endpoint_ref.text)
                                            ipaddress = commen_util.get_network_address(point)
                                            connectorItem["etherConnector"] = ipaddress
                                sd = get_element_or_none(sdg, "SD")
                                if sd is not None and "iSOFT:log:EthernetConfigurationUnit:ServicePort" == sd.attrib["GID"]:
                                    connectorItem["tcpPort"] = sd
                                connectorArray.append(connectorItem)
        logGlobalConfigOb["connectorArray"] = connectorArray
        return logGlobalConfigOb

