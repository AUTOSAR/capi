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
import copy
import logging
import struct

from generator.common.tree_helper import get_element_or_none, get_element_or_str, short_name, to_list, \
    get_element_or_false, get_element_or_0, get_element_or_default_1, transition_number, to_str, get_element_or_die, to_milliseconds, get_hump
from generator.parser import commen_util, AR_NAMESPACE_LEN
from generator.parser.commen_util import get_diagnostic_address_from_software, find_machine_by_process, get_machine_network_point, get_network_address
from generator.parser.instance_specifier import InstanceSpecifierBuilder
from generator.views.process_view import ProcessView


class DiagnosisBuilder:
    def __init__(self, model):
        self._log = logging.getLogger(__name__)
        self.model = model
        self.machineNetConfig = {}
        self.interfaceNum = {}
        self.interfaceOb = {}
        self.clientIdList = []
        self.processIdList = []
        self.maxNum = 0
        self.conversionListOb = {}
        self.scToP = {}

    def get_machine_net_config(self):
        machineList = self.model.find_elements_of_type("MACHINE")
        for machineEle in machineList:
            netConfig = {}
            sdgList = get_element_or_none(machineEle, "ADMIN-DATA/SDGS/SDG")
            if sdgList is not None:
                for sdg in sdgList:
                    if sdg.attrib["GID"] == "iSOFT:diag:NetConfig":
                        sdgIp = get_element_or_none(sdg, "SDG")
                        if sdgIp is not None and sdgIp.attrib["GID"] == "iSOFT:diag:NetConfig:multicastIP":
                            sdxRef = get_element_or_none(sdgIp, "SDX-REF")
                            if sdxRef is not None:
                                network = self.model.find_referable(sdxRef.text)
                                ipv4Address = get_element_or_none(network, "NETWORK-ENDPOINT-ADDRESSES/IPV-4-CONFIGURATION/IPV-4-ADDRESS")
                                if ipv4Address is not None:
                                    netConfig["multicastIP"] = ipv4Address
                        sd = get_element_or_none(sdg, "SD")
                        if sd is not None and sd.attrib["GID"] == "iSOFT:diag:NetConfig:multicastPort":
                            netConfig["multicastPort"] = sd
                    elif sdg.attrib["GID"] == "iSOFT:diag:NetConfigForDm":
                        sdgIp = get_element_or_none(sdg, "SDG")
                        if sdgIp is not None and sdgIp.attrib["GID"] == "iSOFT:diag:NetConfigForDm:unicastIP":
                            sdxRef = get_element_or_none(sdgIp, "SDX-REF")
                            if sdxRef is not None:
                                network = self.model.find_referable(sdxRef.text)
                                ipv4Address = get_element_or_none(network, "NETWORK-ENDPOINT-ADDRESSES/IPV-4-CONFIGURATION/IPV-4-ADDRESS")
                                if ipv4Address is not None:
                                    netConfig["unicastIP"] = ipv4Address
                        sd = get_element_or_none(sdg, "SD")
                        if sd is not None and sd.attrib["GID"] == "iSOFT:diag:NetConfigForDm:unicastPort":
                            netConfig["unicastPort"] = sd
            self.machineNetConfig[machineEle.get_fqn()] = netConfig

    def get_diag_by_cluster(self, software_cluster_list, processes):
        diagnosis = {}
        dmcList = []
        serverTable = []
        self.get_machine_net_config()
        self.judge_process_in_software_cluster()
        diagnosis["dapi"] = self.get_dapi(processes)
        for software_cluster_ref in software_cluster_list:
            if self.judge_is_need(software_cluster_ref, "dmd"):
                if "dmd_doip" not in diagnosis:
                    doip = self.get_doip_by_cluster(software_cluster_ref, False)
                    if doip is not None:
                        diagnosis["dmd_doip"] = doip
            elif self.judge_is_need(software_cluster_ref, "doipd"):
                if "doipd_doip" not in diagnosis:
                    doip = self.get_doip_by_cluster(software_cluster_ref, True)
                    if doip is not None:
                        diagnosis["doipd_doip"] = doip
        for software_cluster_ref in software_cluster_list:
            dmc = self.get_dmc_by_cluster(software_cluster_ref)
            if dmc is not None:
                dmcList.append(dmc)
                dmc_item = dmc["dmc"]
                if dmc_item is not None:
                    machine = dmc_item["machine"]
                    serverTable.append({
                        "machineShortName": machine["shortName"],
                        "softwareClusterShortName": short_name(software_cluster_ref)
                    })
        diagnosis["dmc"] = dmcList
        if len(serverTable) > 0:
            netConfig = self.get_exe_dmd_to_machine()
            if len(netConfig) > 0:
                diagnosis["dmd"] = {
                    "netConfig": netConfig,
                    "serverTable": serverTable,
                    "processIdList": self.processIdList
                }
        return diagnosis

    def get_exe_dmd_to_machine(self):
        mapping = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING")
        for item in mapping:
            process_ref = get_element_or_die(item, "PROCESS-REF")
            ar_process = self.model.find_referable(process_ref.text)
            exe = get_element_or_none(ar_process, "EXECUTABLE-REF")
            if exe is not None and "dmd" == short_name(exe.text):
                return self.get_process_net_config(process_ref.text, True)
        return {}

    def get_process_net_config(self, processFqn, isDMD=False):
        machineEle = None
        processId = None
        if processFqn is not None:
            machineEle = find_machine_by_process(self, processFqn)
            processId = ProcessView.get_process_id(short_name(processFqn))
            if processId not in self.processIdList:
                self.processIdList.append(processId)
        if machineEle is not None and len(self.machineNetConfig[machineEle.get_fqn()]) > 0:
            machineNetConfig = self.machineNetConfig[machineEle.get_fqn()]
            if isDMD:
                netConfig = copy.copy(machineNetConfig)
            else:
                netConfig = {}
                if "multicastIP" in machineNetConfig:
                    netConfig["multicastIP"] = machineNetConfig["multicastIP"]
                if "multicastPort" in machineNetConfig:
                    netConfig["multicastPort"] = machineNetConfig["multicastPort"]
            if processId is not None:
                netConfig["appId"] = processId
            return netConfig
        return {}

    def get_dmc_name(self, software_cluster, machineOb):
        software_cluster_process_ref = get_element_or_none(software_cluster, "CONTAINED-PROCESS-REFS/CONTAINED-PROCESS-REF")
        if software_cluster_process_ref is not None:
            software_cluster_process_ref_list = to_list(software_cluster_process_ref)
            for cluster_process_ref in software_cluster_process_ref_list:
                machine_mapping = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING", PROCESS_REF=cluster_process_ref)
                for machine_mapping_item in machine_mapping:
                    machine_ref = get_element_or_none(machine_mapping_item, "MACHINE-REF")
                    process_ref = get_element_or_none(machine_mapping_item, "PROCESS-REF")
                    if machine_ref is not None:
                        machineOb["shortName"] = short_name(machine_ref)
                        machineOb["fqn"] = machine_ref.text
                        dmc_name = "dmd_dmc_" + short_name(machine_ref) + "_" + software_cluster.SHORT_NAME+".json"
                        return dmc_name

    def set_diagnostic_session_list(self, diagnosticSessionList, diagnostic_session_ref):
        if diagnostic_session_ref is not None:
            session_element = self.model.find_referable(diagnostic_session_ref.text)
            if session_element is not None:
                for diagnosticSessionItem in diagnosticSessionList:
                    if diagnostic_session_ref.text == diagnosticSessionItem["_path"]:
                        diagnosticSession = diagnosticSessionItem["id"]
                        return diagnosticSession

                diagnosticSession = transition_number(get_element_or_default_1(session_element, "ID"), defaultInt=-1)
                diagnosticSessionOb = {
                    "id": transition_number(get_element_or_default_1(session_element, "ID"), defaultInt=-1),
                    "_path": diagnostic_session_ref,
                    "shortName": get_element_or_str(session_element, "SHORT-NAME")
                }
                p2ServerMax = to_milliseconds(get_element_or_none(session_element, "P-2-SERVER-MAX"))
                diagnosticSessionOb["p2ServerMax"] = p2ServerMax if p2ServerMax is not None else 0
                p2StarServerMax = to_milliseconds(get_element_or_none(session_element, "P-2-STAR-SERVER-MAX"))
                diagnosticSessionOb["p2StarServerMax"] = int(p2StarServerMax / 10) if p2StarServerMax is not None else 0
                diagnosticSessionList.append(diagnosticSessionOb)
                return diagnosticSession
        return -1

    def get_dmc_by_cluster(self, software_cluster_ref):
        software_cluster = self.model.find_referable(software_cluster_ref)
        diagnosticAddress = get_diagnostic_address_from_software(software_cluster)
        if diagnosticAddress is None:
            return None
        softwareCluster = {
            "shortName": software_cluster.SHORT_NAME,
            "diagnosticAddress": diagnosticAddress
        }
        machineOb = {}
        dmc_name = self.get_dmc_name(software_cluster, machineOb)
        if dmc_name is None:
            return None

        commonPropertiesOb = {}
        accessPermissionList = []
        diagnosticSessionList = []
        securityLevelList = []

        protocolOb = {}
        serviceTableList = []
        conversationOb = {
            "maxNum": self.maxNum
        }
        indicatorList = []
        operationCycleList = []
        enableConditionGroupList = []
        enableConditionList = []
        troubleCodeUdsList = []
        dtcPropsList = []
        agingList = []
        freezeFrameList = []
        memoryDestinationList = []
        extendedDataRecordList = []
        dataIdentifierSetList = []
        diagnosticEventList = []
        clearConditionGroupList = []
        clearConditionList = []
        diagnosticTroubleCodeGroupList = []
        dataIdentifierList = []
        routineList = []
        serviceValidation = {
            "manufacturer":[],
            "supplier":[]
        }
        manufacturerList = []
        supplierList = []
        environmentalConditionList = []
        diagnosticAuthRoleList = []
        externalAuthenticationList = []
        authenticationTimeout = None
        if software_cluster_ref in self.conversionListOb:
            conversationOb["serviceInstanceId"] = self.conversionListOb[software_cluster_ref]
        diagnostic_extract_ref = get_element_or_none(software_cluster, "DIAGNOSTIC-EXTRACT-REF")
        if diagnostic_extract_ref is not None:
            contribution_set_element = self.model.find_referable(diagnostic_extract_ref.text)
            if contribution_set_element is not None:
                common_properties = get_element_or_none(contribution_set_element, "COMMON-PROPERTIES/DIAGNOSTIC-COMMON-PROPS-VARIANTS/DIAGNOSTIC-COMMON-PROPS-CONDITIONAL")
                if common_properties is not None:
                    if get_element_or_none(common_properties, "AGING-REQUIRES-TESTED-CYCLE") is not None:
                        commonPropertiesOb["agingRequiresTestedCycle"] = get_element_or_false(common_properties, "AGING-REQUIRES-TESTED-CYCLE")
                    if get_element_or_none(common_properties, "CLEAR-DTC-LIMITATION") is not None:
                        commonPropertiesOb["clearDtcLimitation"] = get_element_or_str(common_properties, "CLEAR-DTC-LIMITATION")
                    if get_element_or_none(common_properties, "DEFAULT-ENDIANNESS") is not None:
                        commonPropertiesOb["defaultEndianness"] = get_element_or_str(common_properties, "DEFAULT-ENDIANNESS")
                    if get_element_or_none(common_properties, "ENVIRONMENT-DATA-CAPTURE") is not None:
                        commonPropertiesOb["environmentDataCapture"] = get_element_or_str(common_properties, "ENVIRONMENT-DATA-CAPTURE")
                    if get_element_or_none(common_properties, "MAX-NUMBER-OF-REQUEST-CORRECTLY-RECEIVED-RESPONSE-PENDING") is not None:
                        commonPropertiesOb["maxNumberOfRequestCorrectlyReceivedResponsePending"] = get_element_or_0(common_properties, "MAX-NUMBER-OF-REQUEST-CORRECTLY-RECEIVED-RESPONSE-PENDING")
                    if get_element_or_none(common_properties, "OCCURRENCE-COUNTER-PROCESSING") is not None:
                        commonPropertiesOb["occurrenceCounterProcessing"] = get_element_or_str(common_properties, "OCCURRENCE-COUNTER-PROCESSING")
                    if get_element_or_none(common_properties, "RESET-CONFIRMED-BIT-ON-OVERFLOW") is not None:
                        commonPropertiesOb["resetConfirmedBitOnOverflow"] = get_element_or_false(common_properties, "RESET-CONFIRMED-BIT-ON-OVERFLOW")
                    if get_element_or_none(common_properties, "RESPONSE-ON-ALL-REQUEST-SIDS") is not None:
                        commonPropertiesOb["responseOnAllRequestSids"] = get_element_or_false(common_properties, "RESPONSE-ON-ALL-REQUEST-SIDS")
                    if get_element_or_none(common_properties, "RESPONSE-ON-SECOND-DECLINED-REQUEST") is not None:
                        commonPropertiesOb["responseOnSecondDeclinedRequest"] = get_element_or_false(common_properties, "RESPONSE-ON-SECOND-DECLINED-REQUEST")
                    if get_element_or_none(common_properties, "SECURITY-DELAY-TIME-ON-BOOT") is not None:
                        commonPropertiesOb["securityDelayTimeOnBoot"] = to_milliseconds(get_element_or_none(common_properties, "SECURITY-DELAY-TIME-ON-BOOT"))
                    if get_element_or_none(common_properties, "STATUS-BIT-HANDLING-TEST-FAILED-SINCE-LAST-CLEAR") is not None:
                        commonPropertiesOb["statusBitHandlingTestFailedSinceLastClear"] = get_element_or_str(common_properties, "STATUS-BIT-HANDLING-TEST-FAILED-SINCE-LAST-CLEAR")
                    if get_element_or_none(common_properties, "STATUS-BIT-STORAGE-TEST-FAILED") is not None:
                        commonPropertiesOb["statusBitStorageTestFailed"] = get_element_or_false(common_properties, "STATUS-BIT-STORAGE-TEST-FAILED")
                    debounceAlgorithmPropsList = []
                    debounce_algorithm_props_map = common_properties.find_elements_of_type("DIAGNOSTIC-DEBOUNCE-ALGORITHM-PROPS")
                    for debounce_algorithm_props in debounce_algorithm_props_map:
                        debounce_algorithm_props_ob = {"id": len(debounceAlgorithmPropsList), "_path": debounce_algorithm_props.get_fqn()}
                        debounceBehavior = get_element_or_none(debounce_algorithm_props, "DEBOUNCE-BEHAVIOR")
                        if debounceBehavior is not None:
                            debounce_algorithm_props_ob["debounceBehavior"] = debounceBehavior
                        counter_storage = get_element_or_none(debounce_algorithm_props, "DEBOUNCE-COUNTER-STORAGE")
                        if counter_storage is not None:
                            debounce_algorithm_props_ob["debounceCounterStorage"] = counter_storage
                        monitor_internal = get_element_or_none(debounce_algorithm_props, "DEBOUNCE-ALGORITHM/DIAG-EVENT-DEBOUNCE-MONITOR-INTERNAL")
                        if monitor_internal is not None:
                            debounce_algorithm_props_ob["debounceAlgorithm"] = {"kind": "DIAG-EVENT-DEBOUNCE-MONITOR-INTERNAL"}
                        counter_based = get_element_or_none(debounce_algorithm_props, "DEBOUNCE-ALGORITHM/DIAG-EVENT-DEBOUNCE-COUNTER-BASED")
                        if counter_based is not None:
                            counter_based_ob = {
                                "kind": "DIAG-EVENT-DEBOUNCE-COUNTER-BASED"
                            }
                            if get_element_or_none(counter_based, "COUNTER-BASED-FDC-THRESHOLD-STORAGE-VALUE") is not None:
                                counter_based_ob["counterBasedFdcThresholdStorageValue"] = get_element_or_0(counter_based, "COUNTER-BASED-FDC-THRESHOLD-STORAGE-VALUE")
                            if get_element_or_none(counter_based, "COUNTER-DECREMENT-STEP-SIZE") is not None:
                                counter_based_ob["counterDecrementStepSize"] = get_element_or_0(counter_based, "COUNTER-DECREMENT-STEP-SIZE")
                            if get_element_or_none(counter_based, "COUNTER-FAILED-THRESHOLD") is not None:
                                counter_based_ob["counterFailedThreshold"] = get_element_or_0(counter_based, "COUNTER-FAILED-THRESHOLD")
                            if get_element_or_none(counter_based, "COUNTER-INCREMENT-STEP-SIZE") is not None:
                                counter_based_ob["counterIncrementStepSize"] = get_element_or_0(counter_based, "COUNTER-INCREMENT-STEP-SIZE")
                            if get_element_or_none(counter_based, "COUNTER-JUMP-DOWN") is not None:
                                counter_based_ob["counterJumpDown"] = get_element_or_0(counter_based, "COUNTER-JUMP-DOWN")
                            if get_element_or_none(counter_based, "COUNTER-JUMP-DOWN-VALUE") is not None:
                                counter_based_ob["counterJumpDownValue"] = get_element_or_0(counter_based, "COUNTER-JUMP-DOWN-VALUE")
                            if get_element_or_none(counter_based, "COUNTER-JUMP-UP") is not None:
                                counter_based_ob["counterJumpUp"] = get_element_or_0(counter_based, "COUNTER-JUMP-UP")
                            if get_element_or_none(counter_based, "COUNTER-JUMP-UP-VALUE") is not None:
                                counter_based_ob["counterJumpUpValue"] = get_element_or_0(counter_based, "COUNTER-JUMP-UP-VALUE")
                            if get_element_or_none(counter_based, "COUNTER-PASSED-THRESHOLD") is not None:
                                counter_based_ob["counterPassedThreshold"] = get_element_or_0(counter_based, "COUNTER-PASSED-THRESHOLD")
                            debounce_algorithm_props_ob["debounceAlgorithm"] = counter_based_ob
                        time_based = get_element_or_none(debounce_algorithm_props, "DEBOUNCE-ALGORITHM/DIAG-EVENT-DEBOUNCE-TIME-BASED")
                        if time_based is not None:
                            time_based_ob = {
                                "kind": "DIAG-EVENT-DEBOUNCE-TIME-BASED"
                            }
                            if get_element_or_none(time_based, "TIME-BASED-FDC-THRESHOLD-STORAGE-VALUE") is not None:
                                time_based_ob["timeBasedFdcThresholdStorageValue"] = get_element_or_0(time_based, "TIME-BASED-FDC-THRESHOLD-STORAGE-VALUE")
                            if get_element_or_none(time_based, "TIME-FAILED-THRESHOLD") is not None:
                                time_based_ob["timeFailedThreshold"] = get_element_or_0(time_based, "TIME-FAILED-THRESHOLD")
                            if get_element_or_none(time_based, "TIME-PASSED-THRESHOLD") is not None:
                                time_based_ob["timePassedThreshold"] = get_element_or_0(time_based, "TIME-PASSED-THRESHOLD")
                            debounce_algorithm_props_ob["debounceAlgorithm"] = time_based_ob

                        debounceAlgorithmPropsList.append(debounce_algorithm_props_ob)
                    commonPropertiesOb["debounceAlgorithmProps"] = debounceAlgorithmPropsList
                service_tables = contribution_set_element.find_elements_of_type("SERVICE-TABLES")
                for service_table_item in service_tables:
                    service_table_items = service_table_item.find_elements_of_type("DIAGNOSTIC-SERVICE-TABLE-REF-CONDITIONAL")
                    for service_table_item_i in service_table_items:
                        service_table_item_ref = get_element_or_none(service_table_item_i, "DIAGNOSTIC-SERVICE-TABLE-REF")
                        if service_table_item_ref is not None:
                            service_table = self.model.find_referable(service_table_item_ref.text)
                            serviceTableOb = {}
                            if service_table is not None:
                                protocol_kind = get_element_or_str(service_table, "PROTOCOL-KIND")
                                diagnostic_protocol_map = self.model.find_elements_of_type("DIAGNOSTIC-PROTOCOL")
                                for diagnostic_protocol in diagnostic_protocol_map:
                                    protocol_kind_name = get_element_or_str(diagnostic_protocol, "PROTOCOL-KIND")
                                    if protocol_kind_name == protocol_kind:
                                        if get_element_or_none(diagnostic_protocol, "PRIORITY") is not None:
                                            protocolOb["priority"] = transition_number(get_element_or_default_1(diagnostic_protocol, "PRIORITY"))
                                        if get_element_or_none(diagnostic_protocol, "SEND-RESP-PEND-ON-TRANS-TO-BOOT") is not None:
                                            protocolOb["sendRespPendOnTransToBoot"] = get_element_or_false(diagnostic_protocol, "SEND-RESP-PEND-ON-TRANS-TO-BOOT")
                                        protocolOb["protocolKind"] = protocol_kind_name
                                        break
                                serviceList = []
                                service_instance_map = service_table.find_elements_of_type("SERVICE-INSTANCE-REF")
                                for service_instance in service_instance_map:
                                    dest = service_instance.attrib["DEST"]
                                    element = self.model.find_referable(service_instance.text)
                                    if element is not None:
                                        if dest in ["DIAGNOSTIC-ROUTINE-CONTROL"]:
                                            access_id = -1
                                        else:
                                            access_permission = get_element_or_die(element, "ACCESS-PERMISSION-REF")
                                            access_id = self.get_access_permission(access_permission, accessPermissionList, diagnosticSessionList, securityLevelList, software_cluster, environmentalConditionList, dataIdentifierList)

                                        if "DIAGNOSTIC-SESSION-CONTROL" == dest:
                                            diagnosticSession = self.set_diagnostic_session_list(diagnosticSessionList, get_element_or_none(element, "DIAGNOSTIC-SESSION-REF"))
                                            class_name = get_element_or_none(element, "SESSION-CONTROL-CLASS-REF")
                                            if class_name is not None:
                                                serviceOb = self.get_service_ob(serviceList, class_name)
                                                instance_item_ob = {
                                                    "accessPermission": access_id,
                                                    "diagnosticSession": diagnosticSession
                                                }
                                                self.set_instance(serviceOb, instance_item_ob)
                                                self.set_service_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-ECU-RESET" == dest:
                                            category = get_element_or_none(element, "CATEGORY")
                                            instance_item_ob = {
                                                "accessPermission": access_id
                                            }
                                            if category is not None:
                                                instance_item_ob["category"] = category
                                            functionNumber = transition_number(get_element_or_none(element, "CUSTOM-SUB-FUNCTION-NUMBER"),defaultInt=None)
                                            if functionNumber is not None:
                                                instance_item_ob["customSubFunctionNumber"] = functionNumber

                                            self.get_diagnostic_instance_identifier(service_instance, instance_item_ob, software_cluster)
                                            class_name = get_element_or_none(element, "ECU-RESET-CLASS-REF")
                                            if class_name is not None:
                                                serviceOb = self.get_service_ob(serviceList, class_name)
                                                resetClass = self.model.find_referable(class_name.text)
                                                if resetClass is not None:
                                                    respondToReset = get_element_or_none(resetClass, "RESPOND-TO-RESET")
                                                    if respondToReset is not None:
                                                        serviceOb["respondToReset"] = respondToReset
                                                self.set_instance(serviceOb, instance_item_ob)
                                                self.set_service_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-CLEAR-DIAGNOSTIC-INFORMATION" == dest:
                                            class_name = get_element_or_none(element, "CLEAR-DIAGNOSTIC-INFORMATION-CLASS-REF")
                                            if class_name is not None:
                                                serviceOb = self.get_service_ob(serviceList, class_name)
                                                instance_item_ob = {
                                                    "accessPermission": access_id
                                                }
                                                self.set_instance(serviceOb, instance_item_ob)
                                                self.set_service_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-READ-DTC-INFORMATION" == dest:
                                            category = get_element_or_none(element, "CATEGORY")
                                            class_name = get_element_or_none(element, "READ-DTC-INFORMATION-CLASS-REF")
                                            if class_name is not None:
                                                serviceOb = self.get_service_ob(serviceList, class_name)
                                                instance_item_ob = {
                                                    "accessPermission": access_id
                                                }
                                                if category is not None:
                                                    instance_item_ob["category"] = category
                                                self.set_instance(serviceOb, instance_item_ob)
                                                self.set_service_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-READ-DATA-BY-IDENTIFIER" == dest:
                                            class_name = get_element_or_none(element, "READ-CLASS-REF")
                                            serviceOb = self.get_service_ob(serviceList, class_name)
                                            class_element = self.model.find_referable(class_name.text)
                                            if get_element_or_none(class_element, "MAX-DID-TO-READ") is not None:
                                                serviceOb["maxDidToRead"] = transition_number(get_element_or_0(class_element, "MAX-DID-TO-READ"))

                                            data_identifier_ref = get_element_or_none(element, "DATA-IDENTIFIER-REF")
                                            data_identifier_e = self.model.find_referable(data_identifier_ref.text)
                                            identifier_e_id = transition_number(get_element_or_none(data_identifier_e, "ID"),defaultInt=None)
                                            instance_item_ob = {
                                                "accessPermission": access_id
                                            }
                                            if identifier_e_id is not None:
                                                instance_item_ob["dataIdentifier"] = identifier_e_id
                                            self.set_diagnostic_service_some_ip(instance_item_ob, data_identifier_ref.text, software_cluster)
                                            self.set_instance(serviceOb, instance_item_ob)
                                            self.set_service_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-SECURITY-ACCESS" == dest:
                                            class_name = get_element_or_none(element, "SECURITY-ACCESS-CLASS-REF")
                                            serviceOb = self.get_service_ob(serviceList, class_name)
                                            class_element = self.model.find_referable(class_name.text)
                                            if get_element_or_none(class_element, "SHARED-TIMER") is not None:
                                                serviceOb["sharedTimer"] = get_element_or_0(class_element, "SHARED-TIMER")
                                            instance_item_ob = {
                                                "accessPermission": access_id
                                            }
                                            if get_element_or_none(element, "REQUEST-SEED-ID") is not None:
                                                instance_item_ob["requestSeedId"] = transition_number(get_element_or_0(element, "REQUEST-SEED-ID"))

                                            security_levels = get_element_or_none(element, "SECURITY-LEVEL-REF")
                                            if security_levels is not None:
                                                level_id = self.set_security_level_list(securityLevelList, security_levels, software_cluster)
                                                instance_item_ob["securityLevel"] = level_id
                                            self.set_instance(serviceOb, instance_item_ob)
                                            self.set_service_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-COM-CONTROL" == dest:
                                            category = get_element_or_none(element, "CATEGORY")
                                            class_name = get_element_or_none(element, "COM-CONTROL-CLASS-REF")
                                            serviceOb = self.get_service_ob(serviceList, class_name)
                                            instance_item_ob = {
                                                "accessPermission": access_id
                                            }
                                            if category is not None:
                                                instance_item_ob["category"] = category
                                            self.get_diagnostic_instance_identifier(service_instance, instance_item_ob, software_cluster)
                                            self.set_instance(serviceOb, instance_item_ob)
                                            self.set_service_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-READ-DATA-BY-PERIODIC-ID" == dest:
                                            class_name = get_element_or_none(element, "READ-DATA-CLASS-REF")
                                            serviceOb = self.get_service_ob(serviceList, class_name)
                                            class_element = self.model.find_referable(class_name.text)
                                            periodic_element_map = class_element.find_elements_of_type("DIAGNOSTIC-PERIODIC-RATE")
                                            periodicRateList = []
                                            for periodic_element in periodic_element_map:
                                                periodicRateList.append({
                                                    "period": to_milliseconds(transition_number(get_element_or_0(periodic_element, "PERIOD"))),
                                                    "periodicRateCategory": get_element_or_str(periodic_element, "PERIODIC-RATE-CATEGORY")
                                                })
                                            if get_element_or_none(class_element, "MAX-PERIODIC-DID-TO-READ") is not None:
                                                serviceOb["maxPeriodicDidToRead"] = transition_number(get_element_or_0(class_element, "MAX-PERIODIC-DID-TO-READ"))
                                            if get_element_or_none(class_element, "SCHEDULER-MAX-NUMBER") is not None:
                                                serviceOb["schedulerMaxNumber"] = transition_number(get_element_or_0(class_element, "SCHEDULER-MAX-NUMBER"))
                                            serviceOb["periodicRate"] = periodicRateList
                                            data_identifier_ref = get_element_or_none(element, "DATA-IDENTIFIER-REF")
                                            data_identifier_e = self.model.find_referable(data_identifier_ref.text)
                                            identifier_e_id = transition_number(get_element_or_none(data_identifier_e, "ID"),defaultInt=None)
                                            instance_item_ob = {
                                                "accessPermission": access_id
                                            }
                                            if identifier_e_id is not None:
                                                instance_item_ob["dataIdentifier"] = identifier_e_id
                                            self.set_instance(serviceOb, instance_item_ob)
                                            self.set_service_list(serviceList, serviceOb)
                                            if data_identifier_ref is not None:
                                                if "DIAGNOSTIC-DATA-IDENTIFIER" == data_identifier_ref.attrib["DEST"]:
                                                    serviceOb1 = self.get_service_ob(serviceList, "DIAGNOSTIC-READ-DATA-BY-IDENTIFIER-CLASS", False)
                                                    instance_item_ob1 = {
                                                        "accessPermission": access_id
                                                    }
                                                    if identifier_e_id is not None:
                                                        instance_item_ob1["dataIdentifier"] = identifier_e_id
                                                    self.set_diagnostic_service_some_ip(instance_item_ob1, data_identifier_ref.text, software_cluster)
                                                    self.set_instance(serviceOb1, instance_item_ob1)
                                                    self.set_service_list(serviceList, serviceOb1)

                                        elif "DIAGNOSTIC-DYNAMICALLY-DEFINE-DATA-IDENTIFIER" == dest:
                                            data_identifier = get_element_or_none(element, "DATA-IDENTIFIER-REF")
                                            data_identifier_element = self.model.find_referable(data_identifier.text)
                                            dataIdentifier = transition_number(get_element_or_0(data_identifier_element, "ID"))
                                            max_source = transition_number(get_element_or_0(element, "MAX-SOURCE-ELEMENT"))
                                            class_name = get_element_or_none(element, "DYNAMICALLY-DEFINE-DATA-IDENTIFIER-CLASS-REF")
                                            serviceOb = self.get_service_ob(serviceList, class_name)
                                            class_element = self.model.find_referable(class_name.text)
                                            checkPerSourceId = get_element_or_false(class_element, "CHECK-PER-SOURCE-ID")
                                            configurationHandling = get_element_or_str(class_element, "CONFIGURATION-HANDLING")
                                            sub_function_map = class_element.find_elements_of_type("SUBFUNCTION")
                                            subfunction = []
                                            for sub_function in sub_function_map:
                                                subfunction.append(sub_function.text)
                                            serviceOb["checkPerSourceId"] = checkPerSourceId
                                            serviceOb["configurationHandling"] = configurationHandling
                                            serviceOb["subfunction"] = subfunction
                                            instance_item_ob = {
                                                "accessPermission": access_id,
                                                "dataIdentifier": dataIdentifier,
                                                "maxSourceElement": max_source
                                            }
                                            self.set_instance(serviceOb, instance_item_ob)
                                            self.set_service_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-WRITE-DATA-BY-IDENTIFIER" == dest:
                                            class_name = get_element_or_none(element, "WRITE-CLASS-REF")
                                            serviceOb = self.get_service_ob(serviceList, class_name)
                                            data_identifier_ref = get_element_or_none(element, "DATA-IDENTIFIER-REF")
                                            data_identifier_e = self.model.find_referable(data_identifier_ref.text)
                                            identifier_e_id = transition_number(get_element_or_0(data_identifier_e, "ID"))
                                            instance_item_ob = {
                                                "accessPermission": access_id
                                            }
                                            if identifier_e_id is not None:
                                                instance_item_ob["dataIdentifier"] = identifier_e_id
                                            self.set_diagnostic_service_some_ip(instance_item_ob, data_identifier_ref.text, software_cluster)
                                            self.set_instance(serviceOb, instance_item_ob)
                                            self.set_service_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-ROUTINE-CONTROL" == dest:
                                            class_name = get_element_or_none(element, "ROUTINE-CONTROL-CLASS-REF")
                                            serviceOb = self.get_service_ob(serviceList, class_name)
                                            routine_ref = get_element_or_none(element, "ROUTINE-REF")
                                            routine_e = self.model.find_referable(routine_ref.text)
                                            routine_e_id = transition_number(get_element_or_die(routine_e, "ID"))
                                            self.set_diagnostic_routine(routine_e, routineList, accessPermissionList, diagnosticSessionList, securityLevelList, software_cluster, environmentalConditionList, dataIdentifierList)
                                            instance_item_ob = {
                                                "accessPermission": access_id,
                                                "routine": routine_e_id
                                            }
                                            self.get_diagnostic_instance_identifier(service_instance, instance_item_ob, software_cluster)
                                            self.set_instance(serviceOb, instance_item_ob)
                                            self.set_service_list(serviceList, serviceOb)
                                        elif dest in ["DIAGNOSTIC-REQUEST-DOWNLOAD", "DIAGNOSTIC-REQUEST-UPLOAD"]:
                                            instance_item_ob = {
                                                "accessPermission": access_id
                                            }
                                            sdg = get_element_or_none(element, "ADMIN-DATA/SDGS/SDG")
                                            if "DIAGNOSTIC-REQUEST-DOWNLOAD" == dest:
                                                class_name = get_element_or_none(element, "REQUEST-DOWNLOAD-CLASS-REF")
                                                if sdg is not None and sdg.attrib["GID"] == "iSOFT:diag:DiagnosticRequestDownload":
                                                    sd = get_element_or_none(sdg, "SD")
                                                    if sd is not None and sd.attrib["GID"] == "iSOFT:diag:DiagnosticRequestDownload:maxNumberOfBlockLength":
                                                        instance_item_ob["maxNumberOfBlockLength"] = sd
                                            else:
                                                class_name = get_element_or_none(element, "REQUEST-UPLOAD-CLASS-REF")
                                                if sdg is not None and sdg.attrib["GID"] == "iSOFT:diag:DiagnosticRequestUpload":
                                                    sd = get_element_or_none(sdg, "SD")
                                                    if sd is not None and sd.attrib["GID"] == "iSOFT:diag:DiagnosticRequestUpload:maxNumberOfBlockLength":
                                                        instance_item_ob["maxNumberOfBlockLength"] = sd
                                            serviceOb = self.get_service_ob(serviceList, class_name)

                                            self.get_diagnostic_instance_identifier(service_instance, instance_item_ob, software_cluster)
                                            self.set_instance(serviceOb, instance_item_ob)
                                            self.set_service_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-REQUEST-FILE-TRANSFER" == dest:
                                            class_name = get_element_or_none(element, "REQUEST-FILE-TRANSFER-CLASS-REF")
                                            serviceOb = self.get_service_ob(serviceList, class_name)
                                            instance_item_ob = {
                                                "accessPermission": access_id
                                            }
                                            sdg = get_element_or_none(element, "ADMIN-DATA/SDGS/SDG")
                                            if sdg is not None and sdg.attrib["GID"] == "iSOFT:diag:DiagnosticRequestFileTransfer":
                                                sd = get_element_or_none(sdg, "SD")
                                                if sd is not None and sd.attrib["GID"] == "iSOFT:diag:DiagnosticRequestFileTransfer:maxNumberOfBlockLength":
                                                    instance_item_ob["maxNumberOfBlockLength"] = sd
                                            self.get_diagnostic_instance_identifier(service_instance, instance_item_ob, software_cluster)
                                            self.set_instance(serviceOb, instance_item_ob)
                                            self.set_service_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-CLEAR-DIAGNOSTIC-INFORMATION" == dest:
                                            class_name = get_element_or_none(element, "CLEAR-DIAGNOSTIC-INFORMATION-CLASS-REF")
                                            serviceOb = self.get_service_ob(serviceList, class_name)
                                            instance_item_ob = {
                                                "accessPermission": access_id
                                            }
                                            self.set_instance(serviceOb, instance_item_ob)
                                            self.set_service_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-CONTROL-DTC-SETTING" == dest:
                                            category = get_element_or_str(element, "CATEGORY")
                                            class_name = get_element_or_none(element, "DTC-SETTING-CLASS-REF")
                                            serviceOb = self.get_service_ob(serviceList, class_name)
                                            class_element = self.model.find_referable(class_name.text)
                                            serviceOb["controlOptionRecordPresent"] = get_element_or_false(class_element, "CONTROL-OPTION-RECORD-PRESENT")
                                            instance_item_ob = {
                                                "accessPermission": access_id,
                                                "category": category
                                            }
                                            self.set_instance(serviceOb, instance_item_ob)
                                            self.set_service_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-CUSTOM-SERVICE-INSTANCE" == dest:
                                            class_name = get_element_or_none(element, "CUSTOM-SERVICE-CLASS-REF")
                                            if class_name is not None:
                                                class_element = self.model.find_referable(class_name.text)
                                                custom_service_id = transition_number(get_element_or_0(class_element, "CUSTOM-SERVICE-ID"))
                                                if custom_service_id == 54:
                                                    serviceOb = {
                                                        "diagnosticServiceClass": "DIAGNOSTIC-DATA-TRANSFER-CLASS",
                                                        "instance": [{
                                                            "accessPermission": access_id
                                                        }]
                                                    }
                                                    self.set_service_list(serviceList, serviceOb)
                                                elif custom_service_id == 55:
                                                    serviceOb = {
                                                        "diagnosticServiceClass": "DIAGNOSTIC-TRANSFER-EXIT-CLASS",
                                                        "instance": [{
                                                            "accessPermission": access_id
                                                        }]
                                                    }
                                                    self.set_service_list(serviceList, serviceOb)
                                                elif custom_service_id == 41:
                                                    serviceOb = {
                                                        "diagnosticServiceClass": "DIAGNOSTIC-AUTHENTICATION-CLASS",
                                                        "accessPermission": access_id,
                                                        "diagnosticAuthTransmitCertificate":[]
                                                    }
                                                    self.get_diagnostic_instance_identifier(service_instance, serviceOb, software_cluster, custom_service_id)
                                                    service_instance_ele = self.model.find_referable(service_instance.text)
                                                    serviceClass = get_element_or_none(service_instance_ele, "CUSTOM-SERVICE-CLASS-REF")
                                                    if serviceClass is not None:
                                                        serviceClass_element = self.model.find_referable(serviceClass.text)
                                                        sdgList = get_element_or_none(serviceClass_element, "ADMIN-DATA/SDGS/SDG/SDG")
                                                        for sdg in to_list(sdgList):
                                                            if sdg.attrib["GID"] == "iSOFT:diag:DiagnosticCustomServiceClass::diagnosticRole":
                                                                shortName = get_element_or_none(sdg, "SDG-CAPTION/SHORT-NAME")
                                                                if shortName is not None:
                                                                    diagnosticAuthRoleItem = {
                                                                        "shortName": shortName
                                                                    }
                                                                    sd = get_element_or_none(sdg, "SD")
                                                                    for sdItem in to_list(sd):
                                                                        if sdItem.attrib["GID"] == "iSOFT:diag:DiagnosticRole::isDefault":
                                                                            diagnosticAuthRoleItem["isDefault"] = sdItem
                                                                        elif sdItem.attrib["GID"] == "iSOFT:diag:DiagnosticRole::bitPosition":
                                                                            diagnosticAuthRoleItem["bitPosition"] = sdItem
                                                                    if diagnosticAuthRoleItem not in diagnosticAuthRoleList:
                                                                        diagnosticAuthRoleList.append(diagnosticAuthRoleItem)
                                                            elif sdg.attrib["GID"] == "iSOFT:diag:DiagnosticCustomServiceClass::certificateEvaluation":
                                                                certificateEvaluationOb = {}
                                                                sd = get_element_or_none(sdg, "SD")
                                                                for sdItem in to_list(sd):
                                                                    if sdItem.attrib["GID"] == "iSOFT:diag:AuthTransmitCertificateEvaluation::function":
                                                                        certificateEvaluationOb["function"] = sdItem.text
                                                                    elif sdItem.attrib["GID"] == "iSOFT:diag:AuthTransmitCertificateEvaluation::evaluationId":
                                                                        certificateEvaluationOb["evaluationId"] = sdItem
                                                                if len(certificateEvaluationOb)> 0 and certificateEvaluationOb not in serviceOb["diagnosticAuthTransmitCertificate"]:
                                                                    serviceOb["diagnosticAuthTransmitCertificate"].append(certificateEvaluationOb)
                                                    self.get_class_all_max(class_name, serviceOb)
                                                    self.set_service_list(serviceList, serviceOb)
                                                else:
                                                    serviceOb = {
                                                        "diagnosticServiceClass": class_name.attrib["DEST"],
                                                        "customServiceId": custom_service_id,
                                                        "instance": []
                                                    }
                                                    self.get_class_all_max(class_name, serviceOb)
                                                    instance_item_ob = {
                                                        "accessPermission": access_id
                                                    }
                                                    self.get_diagnostic_instance_identifier(service_instance, instance_item_ob, software_cluster, custom_service_id)
                                                    self.set_instance(serviceOb, instance_item_ob)
                                                    self.set_custom_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-TRANSFER-EXIT" == dest:
                                            class_name = get_element_or_none(element, "TRANSFER-EXIT-CLASS-REF")
                                            if class_name is not None:
                                                serviceOb = {
                                                    "diagnosticServiceClass": class_name.attrib["DEST"],
                                                    "instance": [{
                                                        "accessPermission": access_id
                                                    }]
                                                }
                                                self.get_class_all_max(class_name, serviceOb)
                                                self.set_service_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-DATA-TRANSFER" == dest:
                                            class_name = get_element_or_none(element, "DATA-TRANSFER-CLASS-REF")
                                            if class_name is not None:
                                                serviceOb = {
                                                    "diagnosticServiceClass": class_name.attrib["DEST"],
                                                    "instance": [{
                                                        "accessPermission": access_id
                                                    }]
                                                }
                                                self.get_class_all_max(class_name, serviceOb)
                                                self.set_service_list(serviceList, serviceOb)
                                        elif "DIAGNOSTIC-RESPONSE-ON-EVENT" == dest:
                                            class_name = get_element_or_none(element, "RESPONSE-ON-EVENT-CLASS-REF")
                                            serviceOb = self.get_service_ob(serviceList, class_name)
                                            class_element = self.model.find_referable(class_name.text)
                                            sdgList = to_list(get_element_or_none(class_element, "ADMIN-DATA/SDGS/SDG"))
                                            for sdg in sdgList:
                                                if sdg.attrib["GID"] == "iSOFT:diag:DiagnosticResponseOnEventClass":
                                                    sdList = to_list(get_element_or_none(sdg, "SD"))
                                                    for sdItem in sdList:
                                                        if sdItem.attrib["GID"] == "iSOFT:diag:DiagnosticResponseOnEventClass:storeEventEnabled":
                                                            serviceOb["storeEventSupport"] = sdItem
                                                        elif sdItem.attrib["GID"] == "iSOFT:diag:DiagnosticResponseOnEventClass:responseOnEventSchedulerRate":
                                                            serviceOb["responseOnEventSchedulerRate"] = to_milliseconds(sdItem)
                                                        elif sdItem.attrib["GID"] == "iSOFT:diag:DiagnosticResponseOnEventClass:maxNumberOfStoredDTCStatusChangedEvents":
                                                            serviceOb["maxNumberOfStoredDTCStatusChangedEvents"] = sdItem
                                                        elif sdItem.attrib["GID"] == "iSOFT:diag:DiagnosticResponseOnEventClass:maxNumChangeOfDataIdentfierEvents":
                                                            serviceOb["maxNumChangeOfDataIdentfierEvents"] = sdItem
                                                        elif sdItem.attrib["GID"] == "iSOFT:diag:DiagnosticResponseOnEventClass:maxNumComparisionOfValueEvents":
                                                            serviceOb["maxNumComparisionOfValueEvents"] = sdItem
                                                        elif sdItem.attrib["GID"] == "iSOFT:diag:DiagnosticResponseOnEventClass:maxSupportedDIDLength":
                                                            serviceOb["maxSupportedDIDLength"] = sdItem
                                            serviceOb["interMessageTime"] = to_milliseconds(get_element_or_0(class_element, "INTER-MESSAGE-TIME"))
                                            event_window_map = element.find_elements_of_type("DIAGNOSTIC-EVENT-WINDOW")
                                            event_window_list = []
                                            for event_window in event_window_map:
                                                event_window_list.append({
                                                    "eventWindowTime": get_element_or_str(event_window, "EVENT-WINDOW-TIME"),
                                                    "storageStateEvaluation": get_element_or_false(event_window, "STORAGE-STATE-EVALUATION")
                                                })
                                            change_trigger = get_element_or_none(element, "EVENTS/DIAGNOSTIC-DATA-CHANGE-TRIGGER")
                                            change_trigger_list = []
                                            if change_trigger is not None:
                                                event_item = {
                                                    "initialEventStatus": get_element_or_str(change_trigger, "INITIAL-EVENT-STATUS")
                                                }
                                                data_identifier_ref = get_element_or_none(change_trigger, "DATA-IDENTIFIER-REF")
                                                if data_identifier_ref is not None:
                                                    data_identifier_el = self.model.find_referable(data_identifier_ref.text)
                                                    event_item["dataIdentifier"] = transition_number(get_element_or_0(data_identifier_el, "ID"))
                                                change_trigger_list.append(event_item)
                                            dtc_change_trigger = get_element_or_none(element, "EVENTS/DIAGNOSTIC-DTC-CHANGE-TRIGGER")
                                            if dtc_change_trigger is not None:
                                                event_item = {
                                                    "initialEventStatus": get_element_or_str(dtc_change_trigger, "INITIAL-EVENT-STATUS")
                                                }
                                                dtc_status_mask = transition_number(get_element_or_none(dtc_change_trigger, "DTC-STATUS-MASK"),defaultInt=None)
                                                if dtc_status_mask is not None:
                                                    event_item["dtcStatusMask"] = dtc_status_mask
                                                change_trigger_list.append(event_item)
                                            category = get_element_or_str(element, "CATEGORY")
                                            instance_item_ob = {
                                                "accessPermission": access_id,
                                                "category": category,
                                                "event": change_trigger_list,
                                                "eventWindow": event_window_list
                                            }
                                            sdgItem = get_element_or_none(element, "ADMIN-DATA/SDGS/SDG")
                                            if sdgItem is not None and sdgItem.attrib["GID"] == "iSOFT:diag:DiagnosticResponseOnEvent":
                                                sdList = to_list(get_element_or_none(sdgItem, "SD"))
                                                for sdItem in sdList:
                                                    if sdItem.attrib["GID"] == "iSOFT:diag:DiagnosticResponseOnEvent:infiniteTimeToResponse":
                                                        instance_item_ob["infiniteTimeToResponse"] = sdItem
                                                    elif sdItem.attrib["GID"] == "iSOFT:diag:DiagnosticResponseOnEvent:powerWindowTime":
                                                        instance_item_ob["powerWindowTime"] = sdItem
                                            self.set_instance(serviceOb, instance_item_ob)
                                            self.set_service_list(serviceList, serviceOb)
                                protocolOb["serviceTable"] = len(serviceTableList)
                                serviceTableOb["id"] = len(serviceTableList)
                                serviceTableOb["protocolKind"] = get_element_or_str(service_table, "PROTOCOL-KIND")
                                serviceTableOb["service"] = serviceList
                            serviceTableList.append(serviceTableOb)
                set_elements = get_element_or_none(contribution_set_element, "ELEMENTS")
                if set_elements is not None:
                    conditional_element_map = set_elements.find_elements_of_type("DIAGNOSTIC-COMMON-ELEMENT-REF-CONDITIONAL")
                    for conditional_element in conditional_element_map:
                        common_element = get_element_or_none(conditional_element, "DIAGNOSTIC-COMMON-ELEMENT-REF")
                        if common_element is not None:
                            dest = common_element.attrib["DEST"]
                            diagnostic_event_element = self.model.find_referable(common_element.text)
                            if "DIAGNOSTIC-EVENT" == dest:
                                if diagnostic_event_element is not None:
                                    diagnosticEventItem = {}
                                    if get_element_or_none(diagnostic_event_element, "ASSOCIATED-EVENT-IDENTIFICATION") is not None:
                                        diagnosticEventItem["associatedEventIdentification"] = transition_number(get_element_or_none(diagnostic_event_element, "ASSOCIATED-EVENT-IDENTIFICATION"),defaultInt=None)
                                    if get_element_or_none(diagnostic_event_element, "CLEAR-EVENT-ALLOWED-BEHAVIOR") is not None:
                                        diagnosticEventItem["clearEventAllowedBehavior"] = get_element_or_none(diagnostic_event_element, "CLEAR-EVENT-ALLOWED-BEHAVIOR")
                                    if get_element_or_none(diagnostic_event_element, "CONFIRMATION-THRESHOLD") is not None:
                                        diagnosticEventItem["confirmationThreshold"] = get_element_or_none(diagnostic_event_element, "CONFIRMATION-THRESHOLD")
                                    if get_element_or_none(diagnostic_event_element, "EVENT-CLEAR-ALLOWED") is not None:
                                        diagnosticEventItem["eventClearAllowed"] = get_element_or_none(diagnostic_event_element, "EVENT-CLEAR-ALLOWED")
                                    if get_element_or_none(diagnostic_event_element, "PRESTORAGE-FREEZE-FRAME") is not None:
                                        diagnosticEventItem["prestorageFreezeFrame"] = get_element_or_none(diagnostic_event_element, "PRESTORAGE-FREEZE-FRAME")
                                    if get_element_or_none(diagnostic_event_element, "PRESTORED-FREEZEFRAME-STORED-IN-NVM") is not None:
                                        diagnosticEventItem["prestoredFreezeframeStoredInNvm"] = get_element_or_none(diagnostic_event_element, "PRESTORED-FREEZEFRAME-STORED-IN-NVM")
                                    if get_element_or_none(diagnostic_event_element, "RECOVERABLE-IN-SAME-OPERATION-CYCLE") is not None:
                                        diagnosticEventItem["recoverableInSameOperationCycle"] = get_element_or_none(diagnostic_event_element, "RECOVERABLE-IN-SAME-OPERATION-CYCLE")

                                    connectedIndicatorList = []
                                    connected_indicator_map = diagnostic_event_element.find_elements_of_type("DIAGNOSTIC-CONNECTED-INDICATOR")
                                    for connected_indicator in connected_indicator_map:
                                        connectedIndicatorOb = {}
                                        if get_element_or_none(connected_indicator, "BEHAVIOR") is not None:
                                            connectedIndicatorOb["behavior"] = get_element_or_none(connected_indicator, "BEHAVIOR")
                                        if get_element_or_none(connected_indicator, "HEALING-CYCLE-COUNTER-THRESHOLD") is not None:
                                            connectedIndicatorOb["healingCycleCounterThreshold"] = get_element_or_none(connected_indicator, "HEALING-CYCLE-COUNTER-THRESHOLD")
                                        indicator_ref = get_element_or_none(connected_indicator, "INDICATOR-REF")
                                        indicator_element = self.model.find_referable(indicator_ref.text)
                                        indicator_id = -1
                                        for indicator in indicatorList:
                                            if indicator["_path"] == indicator_ref:
                                                indicator_id = indicator["id"]
                                                break
                                        if indicator_id == -1:
                                            indicator_id = len(indicatorList)
                                            indicatorItem = {
                                                "id": indicator_id,
                                                "_path": indicator_ref,
                                                "type": get_element_or_str(indicator_element, "TYPE")
                                            }
                                            mapping = self.model.find_elements_of_type("DIAGNOSTIC-INDICATOR-PORT-MAPPING")
                                            for item in mapping:
                                                map_instance_ref = get_element_or_none(item, "INDICATOR-REF")
                                                if map_instance_ref is not None and map_instance_ref == indicator_ref:
                                                    self.get_diagnostic_interface_service_instance_id(item, indicatorItem)
                                                    break
                                            indicatorList.append(indicatorItem)
                                        connectedIndicatorOb["indicator"] = indicator_id
                                        healing_cycle_ref = get_element_or_none(connected_indicator, "HEALING-CYCLE-REF")
                                        connectedIndicatorOb["healingCycle"] = self.set_operation_cycle_list(operationCycleList, healing_cycle_ref, software_cluster)
                                        connectedIndicatorList.append(connectedIndicatorOb)
                                    diagnosticEventItem["connectedIndicator"] = connectedIndicatorList
                                    diagnostic_event_mapping = {}
                                    debounceAlgorithm_map = self.model.find_elements_of_type("DIAGNOSTIC-EVENT-TO-DEBOUNCE-ALGORITHM-MAPPING")
                                    for debounceAlgorithm_item in debounceAlgorithm_map:
                                        diagnostic_event_ref = get_element_or_none(debounceAlgorithm_item, "DIAGNOSTIC-EVENT-REF")
                                        if diagnostic_event_ref is not None and diagnostic_event_ref == common_element:
                                            debounce_algorithm_ref = get_element_or_none(debounceAlgorithm_item, "DEBOUNCE-ALGORITHM-REF")
                                            if commonPropertiesOb["debounceAlgorithmProps"] is not None:
                                                for item in commonPropertiesOb["debounceAlgorithmProps"]:
                                                    if item["_path"] == debounce_algorithm_ref:
                                                        diagnostic_event_mapping["debounceAlgorithm"] = item["id"]
                                                        break
                                    enableConditionGroup_map = self.model.find_elements_of_type("DIAGNOSTIC-EVENT-TO-ENABLE-CONDITION-GROUP-MAPPING")
                                    for enableConditionGroup_item in enableConditionGroup_map:
                                        diagnostic_event_ref = get_element_or_none(enableConditionGroup_item, "DIAGNOSTIC-EVENT-REF")
                                        if diagnostic_event_ref is not None and diagnostic_event_ref == common_element:
                                            group_ref = get_element_or_none(enableConditionGroup_item, "ENABLE-CONDITION-GROUP-REF")
                                            group_element = self.model.find_referable(group_ref.text)
                                            group_id = -1
                                            for group_item in enableConditionGroupList:
                                                if group_item["_path"] == group_ref:
                                                    group_id = group_item["id"]
                                                    break
                                            if group_id == -1:
                                                group_id = len(enableConditionGroupList)
                                                group_ob = {
                                                    "id": group_id,
                                                    "_path": group_ref
                                                }
                                                enableConditionItemList = []
                                                enable_conditions = get_element_or_none(group_element, "ENABLE-CONDITIONS/DIAGNOSTIC-ENABLE-CONDITION-REF-CONDITIONAL")
                                                for enable_condition_con in to_list(enable_conditions):
                                                    enable_condition = get_element_or_none(enable_condition_con, "DIAGNOSTIC-ENABLE-CONDITION-REF")
                                                    if enable_condition is not None:
                                                        enable_condition_element = self.model.find_referable(enable_condition.text)
                                                        enable_condition_id = -1
                                                        for enable_condition_item in enableConditionList:
                                                            if enable_condition_item["_path"] == enable_condition.text:
                                                                enable_condition_id = enable_condition_item["id"]
                                                                break
                                                        if enable_condition_id == -1:
                                                            enable_condition_id = len(enableConditionList)
                                                            enable_condition_ob = {
                                                                "id": enable_condition_id,
                                                                "_path": enable_condition.text,
                                                                "initValue": get_element_or_str(enable_condition_element, "INIT-VALUE")
                                                            }
                                                            mapping = self.model.find_elements_of_type("DIAGNOSTIC-ENABLE-CONDITION-PORT-MAPPING")
                                                            for item in mapping:
                                                                map_instance_ref = get_element_or_none(item, "ENABLE-CONDITION-REF")
                                                                if map_instance_ref is not None and map_instance_ref == enable_condition.text:
                                                                    self.get_diagnostic_interface_service_instance_id(item, enable_condition_ob)
                                                                    break
                                                            enableConditionList.append(enable_condition_ob)

                                                        enableConditionItemList.append(enable_condition_id)
                                                group_ob["enableCondition"] = enableConditionItemList
                                                enableConditionGroupList.append(group_ob)
                                            diagnostic_event_mapping["enableConditionGroup"] = group_id
                                    operationCycle_map = self.model.find_elements_of_type("DIAGNOSTIC-EVENT-TO-OPERATION-CYCLE-MAPPING")
                                    for operationCycle_item in operationCycle_map:
                                        diagnostic_event_ref = get_element_or_none(operationCycle_item, "DIAGNOSTIC-EVENT-REF")
                                        if diagnostic_event_ref is not None and diagnostic_event_ref == common_element:
                                            operation_cycle_ref = get_element_or_none(operationCycle_item, "OPERATION-CYCLE-REF")
                                            diagnostic_event_mapping["operationCycle"] = self.set_operation_cycle_list(operationCycleList, operation_cycle_ref, software_cluster)

                                    trouble_uds_map = self.model.find_elements_of_type("DIAGNOSTIC-EVENT-TO-TROUBLE-CODE-UDS-MAPPING")
                                    for trouble_uds_item in trouble_uds_map:
                                        trouble_uds_ref = get_element_or_none(trouble_uds_item, "DIAGNOSTIC-EVENT-REF")
                                        if trouble_uds_ref is not None and trouble_uds_ref == common_element:
                                            uds_ref = get_element_or_none(trouble_uds_item, "TROUBLE-CODE-UDS-REF")
                                            code_uds_element = self.model.find_referable(uds_ref.text)
                                            uds_id = -1
                                            for troubleCodeUds_item in troubleCodeUdsList:
                                                if troubleCodeUds_item["_path"] == uds_ref:
                                                    uds_id = troubleCodeUds_item["id"]
                                                    break
                                            if uds_id == -1:
                                                uds_id = len(troubleCodeUdsList)
                                                troubleCodeUdsOb = {
                                                    "id": uds_id,
                                                    "_path": uds_ref
                                                }
                                                if get_element_or_none(code_uds_element, "CONSIDER-PTO-STATUS") is not None:
                                                    troubleCodeUdsOb["considerPtoStatus"] = get_element_or_none(code_uds_element, "CONSIDER-PTO-STATUS")
                                                if get_element_or_none(code_uds_element, "EVENT-OBD-READINESS-GROUP") is not None:
                                                    troubleCodeUdsOb["eventObdReadinessGroup"] = get_element_or_none(code_uds_element, "EVENT-OBD-READINESS-GROUP")
                                                if get_element_or_none(code_uds_element, "FUNCTIONAL-UNIT") is not None:
                                                    troubleCodeUdsOb["functionalUnit"] = transition_number(get_element_or_none(code_uds_element, "FUNCTIONAL-UNIT"),defaultInt=None)
                                                if get_element_or_none(code_uds_element, "SEVERITY") is not None:
                                                    troubleCodeUdsOb["severity"] = transition_number(get_element_or_none(code_uds_element, "SEVERITY"))
                                                if get_element_or_none(code_uds_element, "UDS-DTC-VALUE") is not None:
                                                    troubleCodeUdsOb["udsDtcValue"] = transition_number(get_element_or_none(code_uds_element, "UDS-DTC-VALUE"))
                                                if get_element_or_none(code_uds_element, "WWH-OBD-DTC-CLASS") is not None:
                                                    troubleCodeUdsOb["wwhObdDtcClass"] = get_element_or_none(code_uds_element, "WWH-OBD-DTC-CLASS")

                                                condition_group_mapping = self.model.find_elements_of_type("DIAGNOSTIC-TROUBLE-CODE-UDS-TO-CLEAR-CONDITION-GROUP-MAPPING")
                                                for condition_group_item in condition_group_mapping:
                                                    trouble_code_uds_ref = get_element_or_none(condition_group_item, "TROUBLE-CODE-UDS-REF")
                                                    if trouble_code_uds_ref == uds_ref:
                                                        clear_condition_group_ref = get_element_or_none(condition_group_item, "CLEAR-CONDITION-GROUP-REF")
                                                        if clear_condition_group_ref is not None:
                                                            troubleCodeUdsOb["clearConditionGroup"] = self.set_clear_condition_group(clear_condition_group_ref, clearConditionGroupList, clearConditionList, software_cluster)

                                                dtc_props_ref = get_element_or_none(code_uds_element, "DTC-PROPS-REF")
                                                dtc_props_element = self.model.find_referable(dtc_props_ref.text)
                                                dtc_props_id = -1
                                                for dtcProps_item in dtcPropsList:
                                                    if dtcProps_item["_path"] == dtc_props_ref:
                                                        dtc_props_id = dtcProps_item["id"]
                                                        break
                                                if dtc_props_id == -1:
                                                    dtc_props_id = len(dtcPropsList)
                                                    dtcPropsOb = {
                                                        "id": dtc_props_id,
                                                        "_path": dtc_props_ref
                                                    }
                                                    if get_element_or_none(dtc_props_element, "ENVIRONMENT-CAPTURE-TO-REPORTING") is not None:
                                                        dtcPropsOb["environmentCaptureToReporting"] = get_element_or_none(dtc_props_element, "ENVIRONMENT-CAPTURE-TO-REPORTING")
                                                    if get_element_or_none(dtc_props_element, "IMMEDIATE-NV-DATA-STORAGE") is not None:
                                                        dtcPropsOb["immediateNvDataStorage"] = get_element_or_none(dtc_props_element, "IMMEDIATE-NV-DATA-STORAGE")
                                                    if get_element_or_none(dtc_props_element, "MAX-NUMBER-FREEZE-FRAME-RECORDS") is not None:
                                                        dtcPropsOb["maxNumberFreezeFrameRecords"] = transition_number(get_element_or_none(dtc_props_element, "MAX-NUMBER-FREEZE-FRAME-RECORDS"),defaultInt=None)
                                                    if get_element_or_none(dtc_props_element, "PRIORITY") is not None:
                                                        priority = transition_number(get_element_or_none(dtc_props_element, "PRIORITY"))
                                                        assert 0 <= priority <= 255, "[DIAG] CODE-005: DiagnosticTroubleCodeProps.priority must be in the range of [0, 255].FQN:{}. LocalPath:{}. ".format(dtc_props_element.get_fqn(), dtc_props_element.get_path())
                                                        dtcPropsOb["priority"] = priority
                                                    if get_element_or_none(dtc_props_element, "SIGNIFICANCE") is not None:
                                                        dtcPropsOb["significance"] = get_element_or_none(dtc_props_element, "SIGNIFICANCE")

                                                    legislatedFreezeFrameContentWwhObd_ref_mapping = get_element_or_none(dtc_props_element, "LEGISLATED-FREEZE-FRAME-CONTENT-WWH-OBDS/DIAGNOSTIC-DATA-IDENTIFIER-SET-REF-CONDITIONAL")
                                                    for legislatedFreezeFrameContentWwhObd_ref_item in to_list(legislatedFreezeFrameContentWwhObd_ref_mapping):
                                                        legislatedFreezeFrameContentWwhObd_ref = get_element_or_none(legislatedFreezeFrameContentWwhObd_ref_item, "DIAGNOSTIC-DATA-IDENTIFIER-SET-REF")
                                                        legislatedFreezeFrameContentWwhObdId = self.set_data_identifier_set_list(dataIdentifierSetList, legislatedFreezeFrameContentWwhObd_ref)
                                                        if legislatedFreezeFrameContentWwhObdId is not None:
                                                            dtcPropsOb["legislatedFreezeFrameContentWwhObd"] = legislatedFreezeFrameContentWwhObdId

                                                    snapshotRecordContent_ref_map = get_element_or_none(dtc_props_element, "SNAPSHOT-RECORD-CONTENTS/DIAGNOSTIC-DATA-IDENTIFIER-SET-REF-CONDITIONAL")
                                                    for snapshotRecordContent_ref_item in to_list(snapshotRecordContent_ref_map):
                                                        snapshotRecordContent_ref = get_element_or_none(snapshotRecordContent_ref_item, "DIAGNOSTIC-DATA-IDENTIFIER-SET-REF")
                                                        snapshotRecordContentId = self.set_data_identifier_set_list(dataIdentifierSetList, snapshotRecordContent_ref)
                                                        if snapshotRecordContentId is not None:
                                                            dtcPropsOb["snapshotRecordContent"] = snapshotRecordContentId

                                                    extendedDataRecordItemList = []
                                                    extended_data_records = get_element_or_none(dtc_props_element, "EXTENDED-DATA-RECORDS/DIAGNOSTIC-EXTENDED-DATA-RECORD-REF-CONDITIONAL")
                                                    if extended_data_records is not None:
                                                        for extended_data_record in extended_data_records:
                                                            data_record_ref = get_element_or_none(extended_data_record, "DIAGNOSTIC-EXTENDED-DATA-RECORD-REF")
                                                            if data_record_ref is not None:
                                                                extendedDataRecordRecordNumber = -1
                                                                data_record_element = self.model.find_referable(data_record_ref.text)
                                                                for extendedDataRecordItem in extendedDataRecordList:
                                                                    if extendedDataRecordItem["_path"] == data_record_ref:
                                                                        extendedDataRecordRecordNumber = extendedDataRecordItem["recordNumber"]
                                                                        break
                                                                if extendedDataRecordRecordNumber == -1:
                                                                    extendedDataRecordId = len(extendedDataRecordList)
                                                                    extendedDataRecordRecordNumber = transition_number(get_element_or_0(data_record_element, "RECORD-NUMBER"))
                                                                    extendedDataRecordOb = {
                                                                        "id": extendedDataRecordId,
                                                                        "_path": data_record_ref,
                                                                        "recordNumber": extendedDataRecordRecordNumber
                                                                    }
                                                                    if get_element_or_none(data_record_element, "TRIGGER") is not None:
                                                                        extendedDataRecordOb["trigger"] = get_element_or_none(data_record_element, "TRIGGER")
                                                                    extendedDataRecordOb["update"] = get_element_or_false(data_record_element, "UPDATE")
                                                                    if get_element_or_none(data_record_element, "CUSTOM-TRIGGER") is not None:
                                                                        extendedDataRecordOb["customTrigger"] = get_element_or_none(data_record_element, "CUSTOM-TRIGGER")
                                                                    recordElementList = []
                                                                    diagnostic_parameter_map = get_element_or_none(data_record_element, "RECORD-ELEMENTS/DIAGNOSTIC-PARAMETER")
                                                                    diagnostic_parameter_list = to_list(diagnostic_parameter_map)
                                                                    for diagnostic_parameter_item in diagnostic_parameter_list:
                                                                        recordElementOb = {"bitOffset": transition_number(get_element_or_0(diagnostic_parameter_item, "BIT-OFFSET"))}
                                                                        data_element = get_element_or_none(diagnostic_parameter_item, "DATA-ELEMENTS/DIAGNOSTIC-DATA-ELEMENT")
                                                                        recordElementOb["dataElement"] = self.get_data_element(data_element, recordElementList)
                                                                        support_info_element = transition_number(get_element_or_none(diagnostic_parameter_item, "SUPPORT-INFO/SUPPORT-INFO-BIT"), defaultInt=None)
                                                                        recordElementOb["supportInfo"] = {
                                                                            "supportInfoBit": support_info_element
                                                                        }
                                                                        data_element_fqn = data_element.get_fqn()
                                                                        provided_data_mapping = self.model.find_elements_of_type("DIAGNOSTIC-PROVIDED-DATA-MAPPING")
                                                                        for provided_data_element in provided_data_mapping:
                                                                            if data_element_fqn == get_element_or_str(provided_data_element, "DATA-ELEMENT-REF"):
                                                                                recordElementOb["dataProvider"] = get_element_or_str(provided_data_element, "DATA-PROVIDER")
                                                                                break
                                                                        mapping = self.model.find_elements_of_type("DIAGNOSTIC-SERVICE-DATA-IDENTIFIER-PORT-MAPPING")
                                                                        for item in mapping:
                                                                            map_instance_ref = get_element_or_none(item, "DIAGNOSTIC-DATA-ELEMENT-REF")
                                                                            if map_instance_ref is not None and map_instance_ref == data_element_fqn:
                                                                                self.get_diagnostic_interface_service_instance_id(item, recordElementOb)
                                                                                break
                                                                        recordElementList.append(recordElementOb)
                                                                    extendedDataRecordOb["recordElement"] = recordElementList
                                                                    extendedDataRecordList.append(extendedDataRecordOb)
                                                                extendedDataRecordItemList.append(extendedDataRecordRecordNumber)
                                                    dtcPropsOb["extendedDataRecord"] = extendedDataRecordItemList

                                                    freezeFrameItemList = []
                                                    freeze_frames = get_element_or_none(dtc_props_element, "FREEZE-FRAMES/DIAGNOSTIC-FREEZE-FRAME-REF-CONDITIONAL")
                                                    if freeze_frames is not None:
                                                        freeze_frame_ref_map = to_list(freeze_frames)
                                                        for freeze_frame_item in freeze_frame_ref_map:
                                                            freeze_frame_ref = get_element_or_none(freeze_frame_item, "DIAGNOSTIC-FREEZE-FRAME-REF")
                                                            if freeze_frame_ref is not None:
                                                                freeze_frame_element = self.model.find_referable(freeze_frame_ref.text)
                                                                recordNumber = -1
                                                                for freezeFrameItem in freezeFrameList:
                                                                    if freezeFrameItem["_path"] == freeze_frame_ref:
                                                                        recordNumber = freezeFrameItem["recordNumber"]
                                                                        break
                                                                if recordNumber == -1:
                                                                    freezeFrameId = len(freezeFrameList)
                                                                    recordNumber = transition_number(get_element_or_none(freeze_frame_element, "RECORD-NUMBER"),defaultInt=None)
                                                                    freezeFrameOb = {
                                                                        "id": freezeFrameId,
                                                                        "_path": freeze_frame_ref
                                                                    }
                                                                    if recordNumber is not None:
                                                                        freezeFrameOb["recordNumber"] = recordNumber
                                                                    if get_element_or_none(freeze_frame_element, "TRIGGER") is not None:
                                                                        freezeFrameOb["trigger"] = get_element_or_none(freeze_frame_element, "TRIGGER")
                                                                    if get_element_or_none(freeze_frame_element, "UPDATE") is not None:
                                                                        freezeFrameOb["update"] = get_element_or_none(freeze_frame_element, "UPDATE")
                                                                    if get_element_or_none(freeze_frame_element, "CUSTOM-TRIGGER") is not None:
                                                                        freezeFrameOb["customTrigger"] = get_element_or_none(freeze_frame_element, "CUSTOM-TRIGGER")
                                                                    freezeFrameList.append(freezeFrameOb)
                                                                freezeFrameItemList.append(recordNumber)
                                                    dtcPropsOb["freezeFrame"] = freezeFrameItemList

                                                    aging_ref = get_element_or_none(dtc_props_element, "AGING-REF")
                                                    if aging_ref is not None:
                                                        aging_element = self.model.find_referable(aging_ref.text)
                                                        agingId = -1
                                                        for aging_item in agingList:
                                                            if aging_item["_path"] == aging_ref:
                                                                agingId = aging_item["id"]
                                                                break
                                                        if agingId == -1:
                                                            agingId = len(agingList)
                                                            agingOb = {
                                                                "id": agingId,
                                                                "_path": aging_ref,
                                                                "increase": True
                                                            }
                                                            if get_element_or_none(aging_element, "THRESHOLD") is not None:
                                                                agingOb["threshold"] = get_element_or_none(aging_element, "THRESHOLD")
                                                            direction = get_element_or_none(aging_element, "ADMIN-DATA/SDGS/SDG/SD")
                                                            if direction is not None and "iSOFT:diag:Aging:Direction" == direction.attrib["GID"]:
                                                                if direction.text == "DOWN":
                                                                    agingOb["increase"] = False
                                                            cycle_ref_con = get_element_or_none(aging_element,"AGING-CYCLES/DIAGNOSTIC-OPERATION-CYCLE-REF-CONDITIONAL")
                                                            for cycle_ref_item in to_list(cycle_ref_con):
                                                                cycle_ref = get_element_or_none(cycle_ref_item, "DIAGNOSTIC-OPERATION-CYCLE-REF")
                                                                if cycle_ref is not None:
                                                                    agingOb["agingCycle"] = self.set_operation_cycle_list(operationCycleList, cycle_ref, software_cluster)
                                                            agingList.append(agingOb)
                                                        dtcPropsOb["aging"] = agingId

                                                    memoryDestinationItemList = []
                                                    memory_destination_elements = get_element_or_none(dtc_props_element, "MEMORY-DESTINATION-REFS")
                                                    if memory_destination_elements is not None:
                                                        memory_destination_ref_map = memory_destination_elements.find_elements_of_type("MEMORY-DESTINATION-REF")
                                                        for memory_destination_item in memory_destination_ref_map:
                                                            memory_destination_element = self.model.find_referable(memory_destination_item.text)
                                                            memory_id = -1
                                                            for memoryDestinationItem in memoryDestinationList:
                                                                if memoryDestinationItem["_path"] == memory_destination_item:
                                                                    memory_id = memoryDestinationItem["memoryId"]
                                                                    break

                                                            if memory_id == -1:
                                                                if "DIAGNOSTIC-MEMORY-DESTINATION-PRIMARY" == memory_destination_element.short_tag():
                                                                    memory_id = 256
                                                                else:
                                                                    memory_id = transition_number(get_element_or_0(memory_destination_element, "MEMORY-ID"))
                                                                memory_ob = {
                                                                    "memoryId": memory_id,
                                                                    "_path": memory_destination_item
                                                                }
                                                                if get_element_or_none(memory_destination_element, "DTC-STATUS-AVAILABILITY-MASK") is not None:
                                                                    memory_ob["dtcStatusAvailabilityMask"] = transition_number(get_element_or_none(memory_destination_element, "DTC-STATUS-AVAILABILITY-MASK"))
                                                                if get_element_or_none(memory_destination_element, "EVENT-DISPLACEMENT-STRATEGY") is not None:
                                                                    memory_ob["eventDisplacementStrategy"] = get_element_or_none(memory_destination_element, "EVENT-DISPLACEMENT-STRATEGY")
                                                                if get_element_or_none(memory_destination_element, "MAX-NUMBER-OF-EVENT-ENTRIES") is not None:
                                                                    memory_ob["maxNumberOfEventEntries"] = get_element_or_none(memory_destination_element, "MAX-NUMBER-OF-EVENT-ENTRIES")
                                                                if get_element_or_none(memory_destination_element, "MEMORY-ENTRY-STORAGE-TRIGGER") is not None:
                                                                    memory_ob["memoryEntryStorageTrigger"] = get_element_or_none(memory_destination_element, "MEMORY-ENTRY-STORAGE-TRIGGER")
                                                                if get_element_or_none(memory_destination_element, "TYPE-OF-DTC-SUPPORTED") is not None:
                                                                    memory_ob["typeOfDtcSupported"] = get_element_or_none(memory_destination_element, "TYPE-OF-DTC-SUPPORTED")
                                                                if get_element_or_none(memory_destination_element, "TYPE-OF-FREEZE-FRAME-RECORD-NUMERATION") is not None:
                                                                    memory_ob["typeOfFreezeFrameRecordNumeration"] = get_element_or_none(memory_destination_element, "TYPE-OF-FREEZE-FRAME-RECORD-NUMERATION")

                                                                typeOfDtcSupported = get_element_or_none(memory_destination_element, "TYPE-OF-DTC-SUPPORTED")
                                                                if typeOfDtcSupported:
                                                                    memory_ob["typeOfDtcSupported"] = typeOfDtcSupported

                                                                mapping = self.model.find_elements_of_type("DIAGNOSTIC-MEMORY-DESTINATION-PORT-MAPPING")
                                                                for item in mapping:
                                                                    map_instance_ref = get_element_or_none(item, "MEMORY-DESTINATION-REF")
                                                                    if map_instance_ref is not None and map_instance_ref == memory_destination_item:
                                                                        self.get_diagnostic_interface_service_instance_id(item, memory_ob)
                                                                        break
                                                                memoryDestinationList.append(memory_ob)
                                                            memoryDestinationItemList.append(memory_id)
                                                        dtcPropsOb["memoryDestination"] = memoryDestinationItemList
                                                    dtcPropsList.append(dtcPropsOb)
                                                troubleCodeUdsList.append(troubleCodeUdsOb)
                                                troubleCodeUdsOb["dtcProps"] = dtc_props_id
                                            diagnostic_event_mapping["troubleCodeUds"] = uds_id
                                    diagnosticEventItem["mapping"] = diagnostic_event_mapping
                                    diagnostic_event_port_mapping = self.model.find_elements_of_type("DIAGNOSTIC-EVENT-PORT-MAPPING")
                                    instanceOb = {}
                                    serviceInstance = {}
                                    clientInstance = {}
                                    for diagnostic_event_port_item in diagnostic_event_port_mapping:
                                        map_instance_ref = get_element_or_none(diagnostic_event_port_item, "DIAGNOSTIC-EVENT-REF")
                                        if map_instance_ref is not None and map_instance_ref == common_element:
                                            interface = self.get_diagnostic_interface_class(diagnostic_event_port_item)
                                            self.get_diagnostic_interface_service_instance_id(diagnostic_event_port_item, instanceOb)
                                            if interface is not None:
                                                in_name = None
                                                if "DIAGNOSTIC-EVENT-INTERFACE" == interface:
                                                    in_name = "event"
                                                elif "DIAGNOSTIC-MONITOR-INTERFACE" == interface:
                                                    in_name = "monitor"
                                                if in_name is not None:
                                                    if len(instanceOb) == 2:
                                                        serviceInstance[in_name] = instanceOb["serviceInstanceId"]
                                                        clientInstance[in_name] = instanceOb["clientInstanceId"]
                                    diagnosticEventItem["serviceInstanceId"] = serviceInstance
                                    diagnosticEventItem["clientInstanceId"] = clientInstance
                                    diagnosticEventList.append(diagnosticEventItem)
                            elif dest in ["DIAGNOSTIC-DATA-IDENTIFIER", "DIAGNOSTIC-DYNAMIC-DATA-IDENTIFIER"]:
                                self.set_data_identifier(diagnostic_event_element, dataIdentifierList, software_cluster)
                            elif "DIAGNOSTIC-MEMORY-DESTINATION-PRIMARY" == dest:
                                isHave = False
                                for memoryDestinationItem in memoryDestinationList:
                                    if memoryDestinationItem["_path"] == common_element:
                                        isHave = True
                                        break
                                if not isHave:
                                    memory_ob = {
                                        "memoryId": 256,
                                        "_path": common_element.text
                                    }
                                    if get_element_or_none(diagnostic_event_element, "DTC-STATUS-AVAILABILITY-MASK") is not None:
                                        memory_ob["dtcStatusAvailabilityMask"] = transition_number(get_element_or_none(diagnostic_event_element, "DTC-STATUS-AVAILABILITY-MASK"))
                                    if get_element_or_none(diagnostic_event_element, "EVENT-DISPLACEMENT-STRATEGY") is not None:
                                        memory_ob["eventDisplacementStrategy"] = get_element_or_none(diagnostic_event_element, "EVENT-DISPLACEMENT-STRATEGY")
                                    if get_element_or_none(diagnostic_event_element, "MAX-NUMBER-OF-EVENT-ENTRIES") is not None:
                                        memory_ob["maxNumberOfEventEntries"] = get_element_or_none(diagnostic_event_element, "MAX-NUMBER-OF-EVENT-ENTRIES")
                                    if get_element_or_none(diagnostic_event_element, "MEMORY-ENTRY-STORAGE-TRIGGER") is not None:
                                        memory_ob["memoryEntryStorageTrigger"] = get_element_or_none(diagnostic_event_element, "MEMORY-ENTRY-STORAGE-TRIGGER")
                                    if get_element_or_none(diagnostic_event_element, "TYPE-OF-DTC-SUPPORTED") is not None:
                                        memory_ob["typeOfDtcSupported"] = get_element_or_none(diagnostic_event_element, "TYPE-OF-DTC-SUPPORTED")
                                    if get_element_or_none(diagnostic_event_element, "TYPE-OF-FREEZE-FRAME-RECORD-NUMERATION") is not None:
                                        memory_ob["typeOfFreezeFrameRecordNumeration"] = get_element_or_none(diagnostic_event_element, "TYPE-OF-FREEZE-FRAME-RECORD-NUMERATION")
                                    mapping = self.model.find_elements_of_type("DIAGNOSTIC-MEMORY-DESTINATION-PORT-MAPPING")
                                    for item in mapping:
                                        map_instance_ref = get_element_or_none(item, "MEMORY-DESTINATION-REF")
                                        if map_instance_ref is not None and map_instance_ref == common_element.text:
                                            self.get_diagnostic_interface_service_instance_id(item, memory_ob)
                                            break

                                    memoryDestinationList.append(memory_ob)
                            elif "DIAGNOSTIC-ROUTINE" == dest:
                                self.set_diagnostic_routine(diagnostic_event_element, routineList, accessPermissionList, diagnosticSessionList, securityLevelList, software_cluster, environmentalConditionList, dataIdentifierList)
                            elif "DIAGNOSTIC-SERVICE-GENERIC-MAPPING" == dest:
                                dependency_ref = get_element_or_none(diagnostic_event_element, "SWC-SERVICE-DEPENDENCY-IN-EXECUTABLE-IREF/TARGET-SWC-SERVICE-DEPENDENCY-REF")
                                if dependency_ref is not None:
                                    dependency_item = self.model.find_referable(dependency_ref.text)
                                    if dependency_item is not None:
                                        callback_type = get_element_or_none(dependency_item, "SERVICE-NEEDS/DIAGNOSTIC-COMMUNICATION-MANAGER-NEEDS/SERVICE-REQUEST-CALLBACK-TYPE")
                                        if callback_type is not None:
                                            ob = {}
                                            self.get_diagnostic_interface_service_instance_id(diagnostic_event_element, ob)
                                            if "REQUEST-CALLBACK-TYPE-MANUFACTURER" == callback_type.text:
                                                if ob not in manufacturerList:
                                                    manufacturerList.append(ob)
                                            elif "REQUEST-CALLBACK-TYPE-SUPPLIER" == callback_type.text:
                                                if ob not in supplierList:
                                                    supplierList.append(ob)
                            elif "DIAGNOSTIC-CLEAR-CONDITION" == dest:
                                self.set_clear_condition_item(common_element, clearConditionList, software_cluster)
                            elif "DIAGNOSTIC-CLEAR-CONDITION-GROUP" == dest:
                                self.set_clear_condition_group(common_element, clearConditionGroupList, clearConditionList, software_cluster)
                            elif "DIAGNOSTIC-TROUBLE-CODE-GROUP" == dest:
                                self.set_trouble_code_group(diagnostic_event_element, diagnosticTroubleCodeGroupList, troubleCodeUdsList)
                            elif "DIAGNOSTIC-ACCESS-PERMISSION" == dest:
                                self.get_access_permission(common_element, accessPermissionList, diagnosticSessionList, securityLevelList, software_cluster, environmentalConditionList, dataIdentifierList)
                            elif "DIAGNOSTIC-ENVIRONMENTAL-CONDITION" == dest:
                                self.set_environmental_condition_list(environmentalConditionList, diagnostic_event_element, dataIdentifierList, software_cluster)

                        serviceValidation["manufacturer"] = manufacturerList
                        serviceValidation["supplier"] = supplierList

                sdgAllList = to_list(get_element_or_none(contribution_set_element, "ADMIN-DATA/SDGS/SDG"))
                for sdgAll in sdgAllList:
                    if sdgAll.attrib["GID"] == "iSOFT:diag:DiagnosticContributionSet":
                        sdList = to_list(get_element_or_none(sdgAll, "SD"))
                        for sd in sdList:
                            if sd.attrib["GID"] == "iSOFT:diag:DiagnosticContributionSet:authenticationTimeout":
                                authenticationTimeout = to_milliseconds(sd)
                            elif sd.attrib["GID"] == "iSOFT:diag:DiagnosticContributionSet:P4Server_max_0x3E":
                                commonPropertiesOb["p4ServerMax_0x3E"] = sd
                        sdgList = get_element_or_none(sdgAll, "SDG")
                        for sdg in to_list(sdgList):
                            if sdg.attrib["GID"] == "iSOFT:diag:DiagnosticContributionSet:externalAuthentication":
                                externalAuthenticationOb = {}
                                sd = get_element_or_none(sdg, "SD")
                                for sdItem in to_list(sd):
                                    if sdItem.attrib["GID"] == "iSOFT:diag:ExternalAuthenticationIdentification:sourceAddressMask":
                                        externalAuthenticationOb["sourceAddressMask"] = sdItem
                                    elif sdItem.attrib["GID"] == "iSOFT:diag:ExternalAuthenticationIdentification:sourceAddressCode":
                                        externalAuthenticationOb["sourceAddressCode"] = sdItem
                                if len(externalAuthenticationOb) > 0 and externalAuthenticationOb not in externalAuthenticationList:
                                    externalAuthenticationList.append(externalAuthenticationOb)

        diagnosticAuthentication = {
            "diagnosticAuthRole": diagnosticAuthRoleList,
            "externalAuthentication": externalAuthenticationList
        }
        if authenticationTimeout is not None:
            diagnosticAuthentication["authenticationTimeout"] = authenticationTimeout
        externalAuthenticationService = self.get_external_authentication_service(software_cluster)
        if externalAuthenticationService is not None:
            diagnosticAuthentication["externalAuthenticationService"] = externalAuthenticationService


        return {
            "dmc_name": dmc_name,
            "dmc": {
                "machine": machineOb,
                "softwareCluster": softwareCluster,
                "commonProperties": commonPropertiesOb,
                "serviceTable": serviceTableList,
                "protocol": protocolOb,
                "diagnosticAuthentication": diagnosticAuthentication,
                "diagnosticSession": diagnosticSessionList,
                "securityLevel": securityLevelList,
                "accessPermission": accessPermissionList,
                "environmentalCondition": environmentalConditionList,
                "routine": routineList,
                "conversation": conversationOb,
                "serviceValidation": serviceValidation,
                "diagnosticEvent": diagnosticEventList,
                "enableConditionGroup": enableConditionGroupList,
                "enableCondition": enableConditionList,
                "operationCycle": operationCycleList,
                "indicator": indicatorList,
                "dataIdentifier": dataIdentifierList,
                "diagnosticTroubleCodeGroup": diagnosticTroubleCodeGroupList,
                "troubleCodeUds": troubleCodeUdsList,
                "dtcProps": dtcPropsList,
                "aging": agingList,
                "freezeFrame": freezeFrameList,
                "extendedDataRecord": extendedDataRecordList,
                "memoryDestination": memoryDestinationList,
                "dataIdentifierSet": dataIdentifierSetList,
                "clearConditionGroup": clearConditionGroupList,
                "clearCondition": clearConditionList
            }
        }

    def get_external_authentication_service(self, software_cluster):
        processList = get_element_or_none(software_cluster, "CONTAINED-PROCESS-REFS/CONTAINED-PROCESS-REF")
        for processItem in to_list(processList):
            processEle = self.model.find_referable(processItem.text)
            exe_ref = get_element_or_none(processEle, "EXECUTABLE-REF")
            if exe_ref is not None:
                exe = self.model.find_referable(exe_ref.text)
                swc_ref = get_element_or_none(exe, "ROOT-SW-COMPONENT-PROTOTYPE/APPLICATION-TYPE-TREF")
                if swc_ref is not None:
                    swc = self.model.find_referable(swc_ref.text)
                    p_ports = swc.find_elements_of_type("P-PORT-PROTOTYPE")
                    for port in p_ports:
                        dest = self.get_port_dest(port.get_fqn())
                        if dest == "DIAGNOSTIC-EXTERNAL-AUTHENTICATION-INTERFACE":
                            inOb = self.diag_instance_ob(dest, port.get_fqn())
                            return inOb["serviceInstanceId"]
        return None

    def set_diagnostic_routine(self, element, routineList, accessPermissionList, diagnosticSessionList, securityLevelList, software_cluster, environmentalConditionList, dataIdentifierList):
        fqn = element.get_fqn()
        for item in routineList:
            if item["_path"] == fqn:
                return
        eleId = transition_number(get_element_or_die(element, "ID"))
        for item in routineList:
            if item["id"] == eleId:
                oldEle = self.model.find_referable(item["_path"])
                assert False, "[DIAG] CODE-004: DiagnosticRoutine[{}, {}].id[{}] must be unique. LocalPath:[{}, {}]".format(fqn, item["_path"], str(eleId), element.get_path(), oldEle.get_path())
        routineOb = {
            "id": eleId,
            "_path": element.get_fqn(),
            "routineInfo": transition_number(get_element_or_default_1(element, "ROUTINE-INFO"), defaultInt=-1)
        }
        sdRoutine = get_element_or_none(element, "ADMIN-DATA/SDGS/SDG/SD")
        if sdRoutine is not None and sdRoutine.attrib["GID"] == "iSOFT:diag:DiagnosticRoutineExtension:P4Server_max":
            routineOb["p4ServerMax"] = sdRoutine
        start_element = get_element_or_die(element, "START")
        accessPermissionRef = get_element_or_die(start_element, "ACCESS-PERMISSION-REF")
        sessionList = []
        levelList = []
        self.check_access_permission(element, accessPermissionRef, sessionList, levelList, True)
        access_id = self.get_access_permission(accessPermissionRef, accessPermissionList, diagnosticSessionList, securityLevelList, software_cluster, environmentalConditionList, dataIdentifierList)
        routineOb["start"] = {
            "accessPermission": access_id
        }
        sd = get_element_or_none(start_element, "ADMIN-DATA/SDGS/SDG/SD")
        if sd is not None and sd.attrib["GID"] == "iSOFT:diag:DiagnosticStartRoutineExtension:P4Server_max":
            routineOb["start"]["p4ServerMax"] = sd
        stop_element = get_element_or_none(element, "STOP")
        if stop_element is not None:
            accessPermissionRef = get_element_or_die(stop_element, "ACCESS-PERMISSION-REF")
            self.check_access_permission(element, accessPermissionRef, sessionList, levelList)
            access_id = self.get_access_permission(accessPermissionRef, accessPermissionList, diagnosticSessionList, securityLevelList, software_cluster, environmentalConditionList, dataIdentifierList)
            routineOb["stop"] = {
                "accessPermission": access_id
            }
            sd = get_element_or_none(stop_element, "ADMIN-DATA/SDGS/SDG/SD")
            if sd is not None and sd.attrib["GID"] == "iSOFT:diag:DiagnosticStopRoutineExtension:P4Server_max":
                routineOb["stop"]["p4ServerMax"] = sd
        result_element = get_element_or_none(element, "REQUEST-RESULT")
        if result_element is not None:
            accessPermissionRef = get_element_or_die(result_element, "ACCESS-PERMISSION-REF")
            self.check_access_permission(element, accessPermissionRef, sessionList, levelList)
            access_id = self.get_access_permission(accessPermissionRef, accessPermissionList, diagnosticSessionList, securityLevelList, software_cluster, environmentalConditionList, dataIdentifierList)
            routineOb["requestResult"] = {
                "accessPermission": access_id
            }
            sd = get_element_or_none(result_element, "ADMIN-DATA/SDGS/SDG/SD")
            if sd is not None and sd.attrib["GID"] == "iSOFT:diag:DiagnosticRequestRoutineResultsExtension:P4Server_max":
                routineOb["requestResult"]["p4ServerMax"] = sd
        routineList.append(routineOb)

    def check_access_permission(self, element, accessPermissionRef, sessionList, levelList, isStart=False):
        access_permission = self.model.find_referable(accessPermissionRef.text)
        diagnostic_session_map = access_permission.find_elements_of_type("DIAGNOSTIC-SESSION-REF")
        if diagnostic_session_map is not None and len(diagnostic_session_map) > 0:
            newSessionList = [str(diagnostic_session) for diagnostic_session in diagnostic_session_map]
            newSessionList.sort()
        else:
            newSessionList = []
        security_level_map = access_permission.find_elements_of_type("SECURITY-LEVEL-REF")
        if security_level_map is not None and len(security_level_map) > 0:
            newLevelList = [str(security_level) for security_level in security_level_map]
            newLevelList.sort()
        else:
            newLevelList = []
        if isStart:
            sessionList.extend(newSessionList)
            levelList.extend(newLevelList)
        else:
            assert sessionList == newSessionList and levelList == newLevelList, ("[DIAG] CODE-003: DiagnosticRoutine.[Start, Stop, RequestResult].AccessPermissionRef quoted DiagnosticAccessPermission, DiagnosticAccessPermission.[diagnosticSessionRef, securityLevelRef] must be consistent. Fqn:{}. LocalPath:{}"
                                                                                 .format(element.get_fqn(), element.get_path()))


    def set_environmental_condition_list(self, environmentalConditionList, diagnostic_event_element, dataIdentifierList, software_cluster):
        for item in environmentalConditionList:
            if item["_path"] == diagnostic_event_element.get_fqn():
                return item["id"]
        formula = get_element_or_none(diagnostic_event_element, "FORMULA")
        formulaOb = self.get_condition(formula, dataIdentifierList, software_cluster)
        enId = len(environmentalConditionList)
        environmentalConditionList.append({
            "id": enId,
            "_path": diagnostic_event_element.get_fqn(),
            "formula": formulaOb
        })
        return enId

    def get_condition(self, conditionEle, dataIdentifierList, software_cluster):
        conditionOb = {}
        nrcValue = get_element_or_none(conditionEle, "NRC-VALUE")
        if nrcValue is not None:
            conditionOb["nrcValue"] = nrcValue
        op = get_element_or_none(conditionEle, "OP")
        if op is not None:
            conditionOb["OP"] = to_str(op)
        parts = get_element_or_none(conditionEle, "PARTS")
        if parts is not None:
            partList = []
            partChildren = parts.getchildren()
            for partItem in partChildren:
                if partItem.short_tag() == "DIAGNOSTIC-ENV-DATA-CONDITION":
                    partOb = {
                        "index": len(partList)
                    }
                    dataConditionOb = {}
                    compareType = get_element_or_none(partItem, "COMPARE-TYPE")
                    if compareType is not None:
                        dataConditionOb["comPareType"] = compareType
                    comPareValueOb = {}
                    dataIdentifierOb = {}
                    dataElementFqn = ""
                    valueList = []
                    compareValueEle = get_element_or_none(partItem, "COMPARE-VALUE")
                    if compareValueEle is not None:
                        self.get_compare_value(comPareValueOb, valueList, compareValueEle)
                    if "isArray" not in comPareValueOb:
                        comPareValueOb["isArray"] = False

                    dataElementRef = get_element_or_none(partItem, "DATA-ELEMENT-REF")
                    if dataElementRef is not None:
                        dataElementFqn = dataElementRef.text
                        dataIdentifier = "/".join(dataElementRef.text.split("/")[0: -1])
                        identifierOb = None
                        for item in dataIdentifierList:
                            if item["_path"] == dataIdentifier:
                                identifierOb = item
                                break
                        if identifierOb is None:
                            dataIdentifierEle = self.model.find_referable(dataIdentifier)
                            identifierOb = self.set_data_identifier(dataIdentifierEle, dataIdentifierList, software_cluster)
                        if identifierOb is not None:
                            dataIdentifierOb["id"] = identifierOb["id"]
                            if "parameter" in identifierOb:
                                parameterList = identifierOb["parameter"]
                                for parameter in parameterList:
                                    if "dataElement" in parameter:
                                        dataElement = parameter["dataElement"]
                                        if dataElement["_path"] == dataElementRef.text:
                                            comPareValueOb["type"] = dataElement["baseType"]
                                            dataIdentifierOb["dataElement"] = dataElement["id"]
                                            break
                    if "type" in comPareValueOb:
                        valueType = comPareValueOb["type"]
                        byteStream = None
                        if "8" in valueType:
                            byteStream = "b"
                        elif "16" in valueType:
                            byteStream = "h"
                        elif "32" in valueType:
                            byteStream = "i"
                        elif "float" == valueType:
                            byteStream = "f"
                        if byteStream is not None:
                            comPareValueOb["value"] = self.get_value_byte_stream(valueList, byteStream)

                    dataConditionOb["comPareValue"] = comPareValueOb
                    # dataConditionOb["dataIdentifier"] = dataIdentifierOb
                    dataConditionOb["dataElement"] = dataElementFqn
                    partOb["envDataCondition"] = dataConditionOb
                    partList.append(partOb)
                elif partItem.short_tag() == "DIAGNOSTIC-ENV-CONDITION-FORMULA":
                    conditionForOb = self.get_condition(partItem, dataIdentifierList, software_cluster)
                    partList.append({
                        "index": len(partList),
                        "envConditionFormula": conditionForOb
                    })

            conditionOb["part"] = partList

        return conditionOb

    def get_value_byte_stream(self, valueList, byteStream):
        valuePackList = []
        for valueItem in valueList:
            pack = struct.pack(">{}".format(byteStream), valueItem)
            # packList = str(pack).replace("'", "").split("\\x")
            valuePackList.extend(pack)
        return valuePackList

    def get_compare_value(self, comPareValue, valueList, valueEle):
        numberValue = get_element_or_none(valueEle, "NUMERICAL-VALUE-SPECIFICATION/VALUE")
        if numberValue is not None:
            valueList.append(numberValue)
        constantRef = get_element_or_none(valueEle, "CONSTANT-REFERENCE/CONSTANT-REF")
        if constantRef is not None:
            constantEle = self.model.find_referable(constantRef.text)
            valueSpec = get_element_or_none(constantEle, "VALUE-SPEC")
            if valueSpec is not None:
                self.get_compare_value(comPareValue, valueList, valueSpec)
        arrayElements = get_element_or_none(valueEle, "ARRAY-VALUE-SPECIFICATION/ELEMENTS")
        if arrayElements is not None:
            comPareValue["isArray"] = True
            arrayElements = to_list(arrayElements)
            for item in arrayElements:
                self.get_compare_value(comPareValue, valueList, item)

    def set_data_identifier(self, diagnostic_event_element, dataIdentifierList, software_cluster):
        for item in dataIdentifierList:
            if item["_path"] == diagnostic_event_element.get_fqn():
                return item
        dest = diagnostic_event_element.short_tag()
        identifierOb = {
            "diagnosticdataIdentifierClass": dest,
            "_path": diagnostic_event_element.get_fqn(),
            "id": transition_number(get_element_or_0(diagnostic_event_element, "ID"))
        }
        did_size = transition_number(get_element_or_none(diagnostic_event_element, "DID-SIZE"), defaultInt=None)
        if did_size is not None:
            identifierOb["didSize"] = did_size
        representVin = get_element_or_none(diagnostic_event_element, "REPRESENTS-VIN")
        if representVin is not None:
            identifierOb["representVin"] = representVin
        data_elements = get_element_or_none(diagnostic_event_element, "DATA-ELEMENTS")
        if data_elements is not None:
            diagnostic_parameter_map = data_elements.find_elements_of_type("DIAGNOSTIC-PARAMETER")
            dataElementItemList = []
            for diagnostic_parameter in diagnostic_parameter_map:
                dataElementOb = {
                    "bitOffset": transition_number(get_element_or_0(diagnostic_parameter, "BIT-OFFSET"))
                }
                support_info = get_element_or_none(diagnostic_parameter, "SUPPORT-INFO")
                if support_info is not None:
                    supportInfo = {
                        "supportInfoBit": transition_number(get_element_or_default_1(support_info, "SUPPORT-INFO-BIT"), defaultInt=-1)
                    }
                    dataElementOb["supportInfo"] = supportInfo
                diagnostic_data_element = get_element_or_none(diagnostic_parameter, "DATA-ELEMENTS/DIAGNOSTIC-DATA-ELEMENT")
                if diagnostic_data_element is not None:
                    dataElementOb["dataElement"] = self.get_data_element(diagnostic_data_element, dataElementItemList)
                    diagnostic_data_ref = diagnostic_data_element.get_fqn()
                    provided_data_map = self.model.find_elements_of_type("DIAGNOSTIC-PROVIDED-DATA-MAPPING")
                    for provided_data_item in provided_data_map:
                        data_element_ref = get_element_or_str(provided_data_item, "DATA-ELEMENT-REF")
                        if diagnostic_data_ref == data_element_ref:
                            dataElementOb["dataProvider"] = get_element_or_str(provided_data_item, "DATA-PROVIDER")
                            break
                    mapping = self.model.find_elements_of_type("DIAGNOSTIC-SERVICE-DATA-IDENTIFIER-PORT-MAPPING")
                    for item in mapping:
                        map_instance_ref = get_element_or_none(item, "DIAGNOSTIC-DATA-ELEMENT-REF")
                        if map_instance_ref is not None and map_instance_ref == diagnostic_data_ref:
                            self.get_diagnostic_interface_service_instance_id(item, dataElementOb)
                            break
                dataElementItemList.append(dataElementOb)
            identifierOb["parameter"] = dataElementItemList
        support_info_byte = get_element_or_none(diagnostic_event_element, "SUPPORT-INFO-BYTE")
        if support_info_byte is not None:
            supportInfoByteOb = {
                "position": transition_number(get_element_or_0(support_info_byte, "POSITION")),
                "size": transition_number(get_element_or_0(support_info_byte, "SIZE"))
            }
            identifierOb["supportInfoByte"] = supportInfoByteOb
        dataIdentifierList.append(identifierOb)
        return identifierOb

    def set_trouble_code_group(self, code_group_item, diagnosticTroubleCodeGroupList, troubleCodeUdsList):
        group_number = get_element_or_default_1(code_group_item, "GROUP-NUMBER")
        if group_number != -1:
            for diagnosticTroubleCodeGroupItem in diagnosticTroubleCodeGroupList:
                if group_number == diagnosticTroubleCodeGroupItem["groupNumber"]:
                    diagnosticTroubleCodeGroupList.remove(diagnosticTroubleCodeGroupItem)
                    break
            diagnosticTroubleCodeGroupOb = {
                "groupNumber": group_number
            }
            dtc_item = get_element_or_none(code_group_item, "DTCS")
            troubleCodeUdsItemList = []
            if dtc_item is not None:
                trouble_code_ref_conditional_map = get_element_or_none(dtc_item, "DIAGNOSTIC-TROUBLE-CODE-REF-CONDITIONAL")
                for trouble_code_ref_conditional in to_list(trouble_code_ref_conditional_map):
                    trouble_code_ref = get_element_or_none(trouble_code_ref_conditional, "DIAGNOSTIC-TROUBLE-CODE-REF")
                    if trouble_code_ref is not None:
                        for troubleCodeUds_item in troubleCodeUdsList:
                            if troubleCodeUds_item["_path"] == trouble_code_ref:
                                uds_id = troubleCodeUds_item["id"]
                                troubleCodeUdsItemList.append(uds_id)
                                break
            diagnosticTroubleCodeGroupOb["troubleCodeUds"] = troubleCodeUdsItemList
            diagnosticTroubleCodeGroupList.append(diagnosticTroubleCodeGroupOb)

    def set_clear_condition_group(self, clear_condition_group_ref, clearConditionGroupList, clearConditionList, software_cluster):
        clear_condition_group_id = -1
        clear_condition_group_element = self.model.find_referable(clear_condition_group_ref.text)
        for clearConditionGroupItem in clearConditionGroupList:
            if clearConditionGroupItem["_path"] == clear_condition_group_ref:
                clear_condition_group_id = clearConditionGroupItem["id"]
                break
        if clear_condition_group_id == -1:
            clear_condition_group_id = len(clearConditionGroupList)
            clearConditionItemList = []
            clear_condition_refs = get_element_or_none(clear_condition_group_element, "CLEAR-CONDITION-REFS")
            clear_condition_ref_map = clear_condition_refs.find_elements_of_type("CLEAR-CONDITION-REF")
            for clear_condition_ref in clear_condition_ref_map:
                clearConditionItemList.append(self.set_clear_condition_item(clear_condition_ref, clearConditionList, software_cluster=software_cluster))
            clearConditionGroupList.append({
                "id": clear_condition_group_id,
                "_path": clear_condition_group_ref,
                "clearCondition": clearConditionItemList
            })
        return clear_condition_group_id

    def set_clear_condition_item(self, clear_condition_ref, clearConditionList, software_cluster):
        clear_condition_element = self.model.find_referable(clear_condition_ref.text)
        clear_condition_id = -1
        for clearConditionItem in clearConditionList:
            if clearConditionItem["_path"] == clear_condition_ref:
                clear_condition_id = clearConditionItem["id"]
                break
        if clear_condition_id == -1:
            clear_condition_id = len(clearConditionList)
            clear_condition_ob = {
                "id": clear_condition_id,
                "_path": clear_condition_ref,
                "initValue": get_element_or_false(clear_condition_element, "INIT-VALUE")
            }
            mapping = self.model.find_elements_of_type("DIAGNOSTIC-CLEAR-CONDITION-PORT-MAPPING")
            for item in mapping:
                map_instance_ref = get_element_or_none(item, "CLEAR-CONDITION-REF")
                if map_instance_ref is not None and map_instance_ref == clear_condition_ref:
                    self.get_diagnostic_interface_service_instance_id(item, clear_condition_ob)
                    break
            clearConditionList.append(clear_condition_ob)
        return clear_condition_id

    def get_data_element(self, data_element, dataList):
        dataElement = {}
        if data_element is not None:
            dataElement["id"] = len(dataList)
            dataElement["_path"] = data_element.get_fqn()
            arraySizeSemantics = get_element_or_none(data_element, "ARRAY-SIZE-SEMANTICS")
            if arraySizeSemantics is not None:
                dataElement["arraySizeSemantics"] = arraySizeSemantics
            maxNumberOfElements = get_element_or_none(data_element, "MAX-NUMBER-OF-ELEMENTS")
            if maxNumberOfElements is not None:
                dataElement["maxNumberOfElements"] = transition_number(maxNumberOfElements)
            scalingInfoSize = transition_number(get_element_or_none(data_element, "SCALING-INFO-SIZE"), defaultInt=None)
            if scalingInfoSize is not None:
                dataElement["scalingInfoSize"] = scalingInfoSize
            swBaseType = get_element_or_none(data_element, "SW-DATA-DEF-PROPS/SW-DATA-DEF-PROPS-VARIANTS/SW-DATA-DEF-PROPS-CONDITIONAL/BASE-TYPE-REF")
            if swBaseType is not None:
                swBaseTypeEle = self.model.find_referable(swBaseType.text)
                typeSizeE = get_element_or_none(swBaseTypeEle, "BASE-TYPE-SIZE")
                typeEncoding = get_element_or_none(swBaseTypeEle, "BASE-TYPE-ENCODING")
                if typeSizeE is not None and typeEncoding is not None:
                    typeSize = transition_number(typeSizeE)
                    typeEncoding = to_str(typeEncoding).upper()
                    if typeEncoding == "2C":
                        if typeSize in [8, 16, 32]:
                            dataElement["baseType"] = "int{}".format(typeSize)
                        else:
                            assert False, "[DIAG] CODE-001: DiagnosticDataElement quoted SwBaseType. SwBaseType.baseTypeEncoding and SwBaseType.baseTypeSize does not meet the rules '2C(for 8, 16, or 32 bit)'.FQN:'{}'.LocalPath:'{}'".format(swBaseTypeEle.get_fqn(), swBaseTypeEle.get_path())
                    elif typeEncoding == "NONE":
                        if typeSize in [8, 16, 32]:
                            dataElement["baseType"] = "uint{}".format(typeSize)
                        else:
                            assert False, "[DIAG] CODE-001: DiagnosticDataElement quoted SwBaseType. SwBaseType.baseTypeEncoding and SwBaseType.baseTypeSize does not meet the rules 'NONE(for 8, 16, or 32 bit)'.FQN:'{}'.LocalPath:'{}'".format(swBaseTypeEle.get_fqn(), swBaseTypeEle.get_path())
                    elif typeEncoding == "IEEE754":
                        if typeSize == 32:
                            dataElement["baseType"] = "float"
                        else:
                            assert False, "[DIAG] CODE-001: DiagnosticDataElement quoted SwBaseType. SwBaseType.baseTypeEncoding and SwBaseType.baseTypeSize does not meet the rules 'IEEE754(for 32 bit)'.FQN:'{}'.LocalPath:'{}'".format(swBaseTypeEle.get_fqn(), swBaseTypeEle.get_path())
                baseOrder = get_element_or_none(swBaseTypeEle, "BYTE-ORDER")
                if baseOrder is not None:
                    dataElement["order"] = to_str(baseOrder)
        return dataElement

    def get_doip_by_cluster(self, software_cluster_ref, isDoipd):
        doip_ob = {}
        software_cluster = self.model.find_referable(software_cluster_ref)
        if software_cluster is not None:
            protocol = {}
            protocol_map = self.model.find_elements_of_type("DIAGNOSTIC-PROTOCOL")
            for protocol_element in protocol_map:
                protocol["priority"] = transition_number(get_element_or_str(protocol_element, "PRIORITY"), defaultInt="")
                protocol["protocolKind"] = get_element_or_str(protocol_element, "PROTOCOL-KIND")

            vin = self.get_doip_interface_vin(isDoipd)
            if vin is not None:
                doip_ob["vin"] = vin
            interface = {}
            line_list = []
            identification_list = []
            mode_list = []

            rPortMapping = self.model.find_elements_of_type("R-PORT-PROTOTYPE")
            pPortMapping = self.model.find_elements_of_type("P-PORT-PROTOTYPE")

            self.get_doip_interface(interface, "DIAGNOSTIC-DO-IP-TRIGGER-VEHICLE-ANNOUNCEMENT-INTERFACE", rPortMapping + pPortMapping)
            self.get_doip_interface(interface, "DIAGNOSTIC-DO-IP-ACTIVATION-LINE-INTERFACE", rPortMapping + pPortMapping, True)
            self.get_doip_interface(interface, "DIAGNOSTIC-DO-IP-GROUP-IDENTIFICATION-INTERFACE", rPortMapping + pPortMapping)
            self.get_doip_interface(interface, "DIAGNOSTIC-DO-IP-POWER-MODE-INTERFACE", rPortMapping + pPortMapping)
            if len(line_list) > 0:
                interface["DIAGNOSTIC-DO-IP-ACTIVATION-LINE-INTERFACE"] = line_list
            if len(identification_list) > 0:
                interface["DIAGNOSTIC-DO-IP-GROUP-IDENTIFICATION-INTERFACE"] = identification_list
            if len(mode_list) > 0:
                interface["DIAGNOSTIC-DO-IP-POWER-MODE-INTERFACE"] = mode_list
            doip_ob["interface"] = interface

            if isDoipd:
                routeList = []
                ar_machines = self.model.find_elements_of_type('MACHINE')
                for machine_item in ar_machines:
                    machine_design_ref = get_element_or_none(machine_item, "MACHINE-DESIGN-REF")
                    if machine_design_ref is not None:
                        machine_design = self.model.find_referable(machine_design_ref.text)
                        endpoint_ref = get_element_or_die(machine_design, "COMMUNICATION-CONNECTORS/ETHERNET-COMMUNICATION-CONNECTOR/UNICAST-NETWORK-ENDPOINT-REF")
                        endpoint_info = self.model.find_referable(endpoint_ref.text)
                        ipaddress = commen_util.get_network_address(endpoint_info)
                        diagnosticAddressList = []
                        self.get_diagnostic_address(machine_item, diagnosticAddressList)
                        routeList.append({
                            "ip": ipaddress,
                            "addressing": diagnosticAddressList
                        })

            doip_ref = get_element_or_none(software_cluster, "MODULE-INSTANTIATION-REFS/MODULE-INSTANTIATION-REF")
            if doip_ref is not None:
                doip = self.model.find_referable(doip_ref.text)
                if doip is not None:
                    networkConfigurationList = []
                    communicationConnector = []
                    unicastNetworkEndpoint = []
                    DoIpInstantiation = {}

                    if get_element_or_none(doip, "EID") is not None:
                        eid = str(get_element_or_str(doip, "EID"))[2:]
                        eid_list = bytearray.fromhex(eid)
                        DoIpInstantiation["eid"] = list(eid_list)
                    if get_element_or_none(doip, "GID") is not None:
                        gid = str(get_element_or_str(doip, "GID"))[2:]
                        gid_list = bytearray.fromhex(gid)
                        DoIpInstantiation["gid"] = list(gid_list)
                    if isDoipd:
                        DoIpInstantiation["route"] = routeList
                    if get_element_or_none(doip, "ENTITY-STATUS-MAX-BYTE-FIELD-USE") is not None:
                        DoIpInstantiation["entityStatusMaxByteFieldUse"] = get_element_or_false(doip, "ENTITY-STATUS-MAX-BYTE-FIELD-USE")
                    if get_element_or_none(doip, "GID-INVALIDITY-PATTERN") is not None:
                        DoIpInstantiation["gidInvalidityPattern"] = transition_number(get_element_or_0(doip, "GID-INVALIDITY-PATTERN"))
                    if get_element_or_none(doip, "LOGICAL-ADDRESS") is not None:
                        DoIpInstantiation["logicalAddress"] = transition_number(get_element_or_str(doip, "LOGICAL-ADDRESS"))
                    if get_element_or_none(doip, "MAX-REQUEST-BYTES") is not None:
                        DoIpInstantiation["maxRequestBytes"] = transition_number(get_element_or_0(doip, "MAX-REQUEST-BYTES"))
                    if get_element_or_none(doip, "VIN-INVALIDITY-PATTERN") is not None:
                        DoIpInstantiation["vinInvalidityPattern"] = transition_number(get_element_or_0(doip, "VIN-INVALIDITY-PATTERN"))
                    DoIpInstantiation["doipVersion"] = 3
                    DoIpInstantiation["noParamVehicleIdentificationSwitch"] = False
                    DoIpInstantiation["serverCrt"] = ""
                    DoIpInstantiation["serverKey"] = ""
                    DoIpInstantiation["ca"] = ""
                    sdgList = get_element_or_none(doip, "ADMIN-DATA/SDGS/SDG")
                    if sdgList is not None:
                        for sdg in sdgList:
                            if sdg.attrib["GID"] == "iSOFT:diag:DoIPInstantiationExtension":
                                sdList = get_element_or_none(sdg, "SD")
                                if sdList is not None:
                                    for sd in sdList:
                                        if sd.attrib["GID"] == "iSOFT:diag:DoIPInstantiationExtension:DoIPVersion":
                                            DoIpInstantiation["doipVersion"] = sd
                                        elif sd.attrib["GID"] == "iSOFT:diag:DoIPInstantiationExtension:NoParamVehicleIdentificationSwitch":
                                            DoIpInstantiation["noParamVehicleIdentificationSwitch"] = sd
                                        elif sd.attrib["GID"] == "iSOFT:diag:DoIPInstantiationExtension:ServerCrt":
                                            DoIpInstantiation["serverCrt"] = sd.text
                                        elif sd.attrib["GID"] == "iSOFT:diag:DoIPInstantiationExtension:ServerKey":
                                            DoIpInstantiation["serverKey"] = sd.text
                                        elif sd.attrib["GID"] == "iSOFT:diag:DoIPInstantiationExtension:Ca":
                                            DoIpInstantiation["ca"] = sd.text
                                break
                    requestConfigurationList = []
                    request_map = doip.find_elements_of_type("DO-IP-REQUEST-CONFIGURATION")
                    for request in request_map:
                        requestConfigurationOb = {}
                        if get_element_or_none(request, "END-ADDRESS") is not None:
                            requestConfigurationOb["endAddress"] = transition_number(get_element_or_str(request, "END-ADDRESS"))
                        if get_element_or_none(request, "REQUEST-TYPE") is not None:
                            requestConfigurationOb["requestType"] = get_element_or_str(request, "REQUEST-TYPE")
                        if get_element_or_none(request, "START-ADDRESS") is not None:
                            requestConfigurationOb["startAddress"] = transition_number(get_element_or_str(request, "START-ADDRESS"))
                        requestConfigurationList.append(requestConfigurationOb)
                    DoIpInstantiation["requestConfiguration"] = requestConfigurationList

                    network_interface_map = doip.find_elements_of_type("DO-IP-NETWORK-CONFIGURATION")
                    network_interface_list = []
                    needNum = 0
                    for network_interface in network_interface_map:
                        network_interface_ob = {}
                        if get_element_or_none(network_interface, "EID-USE-MAC") is not None:
                            network_interface_ob["eidUseMac"] = get_element_or_none(network_interface, "EID-USE-MAC")
                        if get_element_or_none(network_interface, "IS-ACTIVATION-LINE-DEPENDENT") is not None:
                            network_interface_ob["isActivationLineDependent"] = get_element_or_none(network_interface, "IS-ACTIVATION-LINE-DEPENDENT")
                        if get_element_or_none(network_interface, "MAX-INITIAL-VEHICLE-ANNOUNCEMENT-TIME") is not None:
                            network_interface_ob["maxInitialVehicleAnnouncementTime"] = get_element_or_none(network_interface, "MAX-INITIAL-VEHICLE-ANNOUNCEMENT-TIME")
                        if get_element_or_none(network_interface, "MAX-TESTER-CONNECTIONS") is not None:
                            network_interface_ob["maxTesterConnections"] = transition_number(get_element_or_none(network_interface, "MAX-TESTER-CONNECTIONS"))
                            needNum += network_interface_ob["maxTesterConnections"]
                        if get_element_or_none(network_interface, "NETWORK-INTERFACE-ID") is not None:
                            network_interface_ob["networkInterfaceId"] = transition_number(get_element_or_none(network_interface, "NETWORK-INTERFACE-ID"))
                        if get_element_or_none(network_interface, "TCP-ALIVE-CHECK-RESPONSE-TIMEOUT") is not None:
                            network_interface_ob["tcpAliveCheckResponseTimeout"] = get_element_or_none(network_interface, "TCP-ALIVE-CHECK-RESPONSE-TIMEOUT")
                        if get_element_or_none(network_interface, "TCP-GENERAL-INACTIVITY-TIME") is not None:
                            network_interface_ob["tcpGeneralInactivityTime"] = get_element_or_none(network_interface, "TCP-GENERAL-INACTIVITY-TIME")
                        if get_element_or_none(network_interface, "TCP-INITIAL-INACTIVITY-TIME") is not None:
                            network_interface_ob["tcpInitialInactivityTime"] = get_element_or_none(network_interface, "TCP-INITIAL-INACTIVITY-TIME")
                        if get_element_or_none(network_interface, "VEHICLE-ANNOUNCEMENT-COUNT") is not None:
                            network_interface_ob["vehicleAnnouncementCount"] = transition_number(get_element_or_none(network_interface, "VEHICLE-ANNOUNCEMENT-COUNT"))
                        if get_element_or_none(network_interface, "VEHICLE-ANNOUNCEMENT-INTERVAL") is not None:
                            network_interface_ob["vehicleAnnouncementInterval"] = get_element_or_none(network_interface, "VEHICLE-ANNOUNCEMENT-INTERVAL")
                        if get_element_or_none(network_interface, "VEHICLE-IDENTIFICATION-SYNC-STATUS") is not None:
                            network_interface_ob["vehicleIdentificationSyncStatus"] = get_element_or_none(network_interface, "VEHICLE-IDENTIFICATION-SYNC-STATUS")
                        networkConfiguration = get_element_or_none(network_interface, "NETWORK-CONFIGURATION-REF")

                        networkId = -1
                        for network in networkConfigurationList:
                            if networkConfiguration == network["_networkConfigurationPath"]:
                                networkId = network["id"]
                                break
                        if networkId == -1:
                            network_element = self.model.find_referable(networkConfiguration.text)

                            communication_ref = get_element_or_none(network_element, "COMMUNICATION-CONNECTOR-REF")
                            communication_element = self.model.find_referable(communication_ref.text)

                            end_point_ref = get_element_or_none(communication_element, "UNICAST-NETWORK-ENDPOINT-REF")
                            end_point_element = self.model.find_referable(end_point_ref.text)
                            networkEndpointAddress = []
                            Ipv4Configuration = {}
                            ipv4Address = get_element_or_none(end_point_element, "NETWORK-ENDPOINT-ADDRESSES/IPV-4-CONFIGURATION/IPV-4-ADDRESS")
                            if ipv4Address is not None:
                                Ipv4Configuration["ipv4Address"] = ipv4Address
                            ipv4AddressSource = get_element_or_none(end_point_element, "NETWORK-ENDPOINT-ADDRESSES/IPV-4-CONFIGURATION/IPV-4-ADDRESS-SOURCE")
                            if ipv4AddressSource is not None:
                                Ipv4Configuration["ipv4AddressSource"] = ipv4AddressSource
                            networkMask = get_element_or_none(end_point_element, "NETWORK-ENDPOINT-ADDRESSES/IPV-4-CONFIGURATION/NETWORK-MASK")
                            if networkMask is not None:
                                Ipv4Configuration["networkMask"] = networkMask
                            networkEndpointAddress.append({
                                "id": len(networkEndpointAddress),
                                "Ipv4Configuration": Ipv4Configuration
                            })
                            end_point_id = len(unicastNetworkEndpoint)
                            unicastNetworkEndpoint.append({
                                "id": end_point_id,
                                "networkEndpointAddress": networkEndpointAddress
                            })
                            communication_id = len(communicationConnector)
                            communicationConnector.append({
                                "id": communication_id,
                                "unicastNetworkEndpoint": end_point_id
                            })
                            networkId = len(networkConfigurationList)
                            networkConfigurationOb = {
                                "id": networkId,
                                "_networkConfigurationPath": networkConfiguration,
                                "communicationConnector": communication_id
                            }
                            if get_element_or_none(network_element, "TCP-PORT") is not None:
                                networkConfigurationOb["tcpPort"] = transition_number(get_element_or_none(network_element, "TCP-PORT"))
                            if get_element_or_none(network_element, "UDP-PORT") is not None:
                                networkConfigurationOb["udpPort"] = transition_number(get_element_or_none(network_element, "UDP-PORT"))
                            networkConfigurationList.append(networkConfigurationOb)
                        network_interface_ob["networkConfiguration"] = networkId
                        network_interface_list.append(network_interface_ob)
                    DoIpInstantiation["networkInterface"] = network_interface_list
                    if self.maxNum == 0:
                        self.maxNum = needNum
                    doip_ob["protocol"] = protocol
                    doip_ob["DoIpInstantiation"] = DoIpInstantiation
                    doip_ob["networkConfiguration"] = networkConfigurationList
                    doip_ob["communicationConnector"] = communicationConnector
                    doip_ob["unicastNetworkEndpoint"] = unicastNetworkEndpoint
                    return doip_ob

        return None

    def get_diagnostic_address(self, machine, diagnosticAddressList):
        machine_mapping = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING")
        ar_software_clusters = self.model.find_elements_of_type('SOFTWARE-CLUSTER')
        for item in machine_mapping:
            itemMachineRef = get_element_or_none(item, "MACHINE-REF")
            if itemMachineRef is not None and itemMachineRef.text == machine.get_fqn():
                process_ref = get_element_or_die(item, "PROCESS-REF")
                for software_clusters in ar_software_clusters:
                    process_refs = get_element_or_none(software_clusters, "CONTAINED-PROCESS-REFS")
                    if process_refs is not None:
                        process_map = process_refs.find_elements_of_type("CONTAINED-PROCESS-REF")
                        if process_ref.text in process_map:
                            diagnosticAddressMap = get_element_or_none(software_clusters, "DIAGNOSTIC-ADDRESSS/SOFTWARE-CLUSTER-DOIP-DIAGNOSTIC-ADDRESS")
                            if diagnosticAddressMap is not None:
                                diagnosticAddressMap = to_list(diagnosticAddressMap)
                                for diagnosticAddress in diagnosticAddressMap:
                                    diagnostic_address = get_element_or_none(diagnosticAddress, "DIAGNOSTIC-ADDRESS")
                                    if diagnostic_address is not None and diagnostic_address not in diagnosticAddressList:
                                        diagnosticAddressList.append(diagnostic_address)

    def get_doip_interface(self, interface, interfaceType, portMapping, isList=False):
        mapping = self.model.find_elements_of_type(interfaceType)
        inOb = {}
        inList = []
        for item in mapping:
            itemFqn = item.get_fqn()
            port_prototype_fqn = None
            for port_item in portMapping:
                interface_tref = get_element_or_none(port_item, "REQUIRED-INTERFACE-TREF")
                if interface_tref is None:
                    interface_tref = get_element_or_none(port_item, "PROVIDED-INTERFACE-TREF")
                if interface_tref is not None and itemFqn == interface_tref.text:
                    port_prototype_fqn = port_item.get_fqn()
                    break

            if port_prototype_fqn is not None:
                dest = self.get_port_dest(port_prototype_fqn)
                inOb = self.diag_instance_ob(dest, port_prototype_fqn)
                if isList:
                    inList.append(inOb)
                elif len(inOb) > 0:
                    break
            else:
                break
        if isList and len(inList) > 0:
            interface[interfaceType] = inList
        elif not isList and len(inOb) > 0:
            interface[interfaceType] = inOb

    def judge_is_need(self, software_cluster_item, exeName):
        if "core" == short_name(software_cluster_item).lower():
            item = self.model.find_referable(software_cluster_item)
            process_refs = get_element_or_none(item, "CONTAINED-PROCESS-REFS")
            if process_refs is not None:
                process_map = process_refs.find_elements_of_type("CONTAINED-PROCESS-REF")
                for process in process_map:
                    ar_process = self.model.find_referable(process.text)
                    exe = get_element_or_none(ar_process, "EXECUTABLE-REF")
                    if exe is not None and exeName == short_name(exe.text):
                        return True
        return False

    def judge_process_in_software_cluster(self):
        software_cluster_mapping = self.model.find_elements_of_type("SOFTWARE-CLUSTER")
        for software_cluster in software_cluster_mapping:
            process_refs = get_element_or_none(software_cluster, "CONTAINED-PROCESS-REFS")
            if process_refs is not None:
                process_map = process_refs.find_elements_of_type("CONTAINED-PROCESS-REF")
                for process in process_map:
                    self.scToP[process.text] = software_cluster.get_fqn()

    def set_data_identifier_set_list(self, dataIdentifierSetList,  ref):
        if ref is not None:
            data_identifier_set_element = self.model.find_referable(ref.text)
            data_identifier_refs = get_element_or_none(data_identifier_set_element, "DATA-IDENTIFIER-REFS")
            if data_identifier_refs is not None:
                data_identifier_ref_map = data_identifier_refs.find_elements_of_type("DATA-IDENTIFIER-REF")
                dataIdentifierSetId = -1
                for dataIdentifierSetItem in dataIdentifierSetList:
                    if dataIdentifierSetItem["_path"] == ref.text:
                        dataIdentifierSetId = dataIdentifierSetItem["id"]
                        break
                if dataIdentifierSetId == -1:
                    dataIdentifierSetId = len(dataIdentifierSetList)
                    dataIdentifierSetOb = {
                        "id": dataIdentifierSetId,
                        "_path": ref.text
                    }
                    dataIdentifierList = []
                    for data_identifier_ref in data_identifier_ref_map:
                        diagnostic_data_identifier = self.model.find_referable(data_identifier_ref.text)
                        dataIdentifierList.append(transition_number(get_element_or_0(diagnostic_data_identifier, "ID")))
                    dataIdentifierSetOb["dataIdentifier"] = dataIdentifierList
                    dataIdentifierSetList.append(dataIdentifierSetOb)
                return dataIdentifierSetId
        return None

    def set_operation_cycle_list(self, operationCycleList, healing_cycle_ref, software_cluster):
        healing_cycle_element = self.model.find_referable(healing_cycle_ref.text)
        cycle_id = -1
        for cycle in operationCycleList:
            if cycle["_path"] == healing_cycle_ref:
                cycle_id = cycle["id"]
                break
        if cycle_id == -1:
            cycle_id = len(operationCycleList)
            cycleOb = {
                "id": cycle_id,
                "_path": healing_cycle_ref
            }
            if get_element_or_none(healing_cycle_element, "AUTOMATIC-END") is not None:
                cycleOb["automaticEnd"] = get_element_or_none(healing_cycle_element, "AUTOMATIC-END")
            if get_element_or_none(healing_cycle_element, "CYCLE-AUTOSTART") is not None:
                cycleOb["cycleAutostart"] = get_element_or_none(healing_cycle_element, "CYCLE-AUTOSTART")
            if get_element_or_none(healing_cycle_element, "CYCLE-STATUS-STORAGE") is not None:
                cycleOb["cycleStatusStorage"] = get_element_or_none(healing_cycle_element, "CYCLE-STATUS-STORAGE")
            if get_element_or_none(healing_cycle_element, "TYPE") is not None:
                cycleOb["type"] = get_element_or_none(healing_cycle_element, "TYPE")

            mapping = self.model.find_elements_of_type("DIAGNOSTIC-OPERATION-CYCLE-PORT-MAPPING")
            for item in mapping:
                map_instance_ref = get_element_or_none(item, "OPERATION-CYCLE-REF")
                if map_instance_ref is not None and map_instance_ref == healing_cycle_ref:
                    self.get_diagnostic_interface_service_instance_id(item, cycleOb)
                    break
            operationCycleList.append(cycleOb)
        return cycle_id

    def get_diagnostic_instance_identifier(self, service_instance,  instance_item_ob, software_cluster, custom_service_id=0):
        mapping = self.model.find_elements_of_type("DIAGNOSTIC-SERVICE-GENERIC-MAPPING")
        interfaceList = []
        for item in mapping:
            map_instance_ref = get_element_or_none(item, "DIAGNOSTIC-SERVICE-INSTANCE-REF")
            if map_instance_ref is not None and map_instance_ref == service_instance:
                if custom_service_id in [54, 55]:
                    interfaceItem = {}
                    self.get_diagnostic_interface_service_instance_id(item, interfaceItem)
                    diagnosticInterfaceClass = self.get_diagnostic_interface_class(item)
                    if diagnosticInterfaceClass is not None:
                        interfaceItem["diagnosticInterfaceClass"] = diagnosticInterfaceClass
                    interfaceList.append(interfaceItem)
                else:
                    self.get_diagnostic_interface_service_instance_id(item, instance_item_ob)
                    diagnosticInterfaceClass = self.get_diagnostic_interface_class(item)
                    if diagnosticInterfaceClass is not None:
                        instance_item_ob["diagnosticInterfaceClass"] = diagnosticInterfaceClass
                    return
        if custom_service_id in [54, 55]:
            instance_item_ob["interface"] = interfaceList

    def set_security_level_list(self, securityLevelList, security_levels, software_cluster):
        for securityLevelOn in securityLevelList:
            if security_levels == securityLevelOn["_path"]:
                return securityLevelOn["id"]

        security_level = self.model.find_referable(security_levels.text)
        level_id = len(securityLevelList) + 1
        securityLevelOb = {
            "id": level_id,
            "_path": security_levels,
            "shortName": get_element_or_str(security_level, "SHORT-NAME")
        }
        if get_element_or_none(security_level, "ACCESS-DATA-RECORD-SIZE") is not None:
            securityLevelOb["accessDataRecordSize"] = transition_number(get_element_or_none(security_level, "ACCESS-DATA-RECORD-SIZE"),defaultInt=None)
        if get_element_or_none(security_level, "KEY-SIZE") is not None:
            securityLevelOb["keySize"] = transition_number(get_element_or_none(security_level, "KEY-SIZE"),defaultInt=None)
        if get_element_or_none(security_level, "NUM-FAILED-SECURITY-ACCESS") is not None:
            securityLevelOb["numFailedSecurityAccess"] = transition_number(get_element_or_none(security_level, "NUM-FAILED-SECURITY-ACCESS"),defaultInt=None)
        if get_element_or_none(security_level, "SECURITY-DELAY-TIME") is not None:
            securityLevelOb["securityDelayTime"] = to_milliseconds(get_element_or_none(security_level, "SECURITY-DELAY-TIME"))
        if get_element_or_none(security_level, "SEED-SIZE") is not None:
            securityLevelOb["seedSize"] = transition_number(get_element_or_none(security_level, "SEED-SIZE"),defaultInt=None)

        security_level_port_mapping = self.model.find_elements_of_type("DIAGNOSTIC-SECURITY-LEVEL-PORT-MAPPING")
        for security_level_port in security_level_port_mapping:
            security_level_ref = get_element_or_none(security_level_port, "SECURITY-LEVEL-REF")
            if security_level_ref is not None and security_level_ref == security_level.get_fqn():
                self.get_diagnostic_interface_service_instance_id(security_level_port, securityLevelOb)
                securityLevelList.append(securityLevelOb)
                break
        return level_id

    def get_access_permission(self, path, accessPermissionList, diagnosticSessionList, securityLevelList, software_cluster, environmentalConditionList, dataIdentifierList):
        for access in accessPermissionList:
            if path == access["_path"]:
                return access["id"]
        access_permission = self.model.find_referable(path.text)
        access_id = len(accessPermissionList)
        accessPermissionOb = {
            "id": access_id,
            "_path": access_permission.get_fqn()
        }
        diagnosticSessionIdList = []
        diagnostic_session_map = access_permission.find_elements_of_type("DIAGNOSTIC-SESSION-REF")
        for diagnostic_session in diagnostic_session_map:
            diagnosticSessionId = self.set_diagnostic_session_list(diagnosticSessionList, diagnostic_session)
            diagnosticSessionIdList.append(diagnosticSessionId)
        accessPermissionOb["diagnosticSession"] = diagnosticSessionIdList

        securityLevelIdList = []
        security_level_map = access_permission.find_elements_of_type("SECURITY-LEVEL-REF")
        for security_levels in security_level_map:
            level_id = self.set_security_level_list(securityLevelList, security_levels, software_cluster=software_cluster)
            securityLevelIdList.append(level_id)
        accessPermissionOb["securityLevel"] = securityLevelIdList
        enRef = get_element_or_none(access_permission, "ENVIRONMENTAL-CONDITION-REF")
        if enRef is not None:
            ev_ele = self.model.find_referable(enRef.text)
            enId = self.set_environmental_condition_list(environmentalConditionList, ev_ele, dataIdentifierList, software_cluster)
            accessPermissionOb["environmentalCondition"] = enId
        sdgs = get_element_or_none(access_permission, "ADMIN-DATA/SDGS")
        if sdgs is not None:
            sdgList = sdgs.getchildren()
            for sdg in sdgList:
                if str(sdg.tag)[AR_NAMESPACE_LEN:]=="SDG" and "GID" in sdg.attrib and sdg.attrib["GID"] == "iSOFT:diag:DiagnosticAccessPermission":
                    authenticationEnabledList = []
                    accessPermissionOb["authenticationEnabled"] = authenticationEnabledList
                    sdxRefList = to_list(get_element_or_none(sdg, "SDX-REF"))
                    for sdxRef in sdxRefList:
                        authenticationEnabledList.append(short_name(sdxRef.text))
        accessPermissionList.append(accessPermissionOb)
        return access_id

    def get_service_ob(self, serviceList, class_name, isClass=True):
        if isClass:
            classDest = class_name.attrib["DEST"]
        else:
            classDest = class_name
        serviceOb = {
            "diagnosticServiceClass": classDest,
            "instance": []
        }
        if isClass:
            self.get_class_all_max(class_name, serviceOb)
        for service in serviceList:
            if classDest == service["diagnosticServiceClass"]:
                serviceOb = service
                break
        return serviceOb

    def get_class_all_max(self, class_name, serviceOb):
        class_element = self.model.find_referable(class_name.text)
        sdgList = to_list(get_element_or_none(class_element, "ADMIN-DATA/SDGS/SDG"))
        for sdg in sdgList:
            gid = "iSOFT:diag:{}Extension".format(get_hump(class_name.attrib["DEST"], True))
            if class_name.attrib["DEST"] == "DIAGNOSTIC-READ-DTC-INFORMATION-CLASS":
                gid = "iSOFT:diag:DiagnosticReadDTCInformationClassExtension"
            if sdg.attrib["GID"] == gid:
                sdList = to_list(get_element_or_none(sdg, "SD"))
                for sd in sdList:
                    if sd.attrib["GID"] == gid + ":P4Server_max":
                        serviceOb["p4ServerMax"] = sd
                    elif sd.attrib["GID"] == gid + ":P4Server_max_0x36":
                        serviceOb["p4ServerMax_0x36"] = sd
                    elif sd.attrib["GID"] == gid + ":P4Server_max_0x37":
                        serviceOb["p4ServerMax_0x37"] = sd

    def set_instance(self, serviceOb, instanceOb):
        if "accessPermission" in instanceOb and instanceOb["accessPermission"] == -1:
            del instanceOb["accessPermission"]
        if "instance" in serviceOb:
            if instanceOb not in serviceOb["instance"]:
                serviceOb["instance"].append(instanceOb)
        else:
            serviceOb["instance"] = [serviceOb]

    def set_service_list(self, serviceList, serviceOb):
        for service in serviceList:
            if serviceOb["diagnosticServiceClass"] == service["diagnosticServiceClass"]:
                return
        serviceList.append(serviceOb)

    def set_custom_list(self, serviceList, serviceOb):
        for service in serviceList:
            if "customServiceId" in service:
                if serviceOb["customServiceId"] == service["customServiceId"]:
                    return
        serviceList.append(serviceOb)

    def set_diagnostic_service_some_ip(self, identifierOb, ref, software_cluster):
        mapping = self.model.find_elements_of_type("DIAGNOSTIC-SERVICE-DATA-IDENTIFIER-PORT-MAPPING")
        for item in mapping:
            map_instance_ref = get_element_or_none(item, "DIAGNOSTIC-DATA-IDENTIFIER-REF")
            if map_instance_ref is not None and map_instance_ref == ref:
                self.get_diagnostic_interface_service_instance_id(item, identifierOb)
                identifierOb["diagnosticInterfaceClass"] = self.get_diagnostic_interface_class(item)
        mapping = self.model.find_elements_of_type("DIAGNOSTIC-SERVICE-GENERIC-MAPPING")
        for item in mapping:
            map_instance_ref = get_element_or_none(item, "DIAGNOSTIC-SERVICE-INSTANCE-REF")
            if map_instance_ref is not None:
                read_data_by_identifier_element = self.model.find_referable(map_instance_ref.text)
                identifier_ref = get_element_or_none(read_data_by_identifier_element, "DATA-IDENTIFIER-REF")
                if identifier_ref is not None and identifier_ref == ref:
                    self.get_diagnostic_interface_service_instance_id(item, identifierOb)
                    identifierOb["diagnosticInterfaceClass"] = self.get_diagnostic_interface_class(item)

    def get_diagnostic_interface_class(self, item):
        dependency_ref = get_element_or_none(item, "SWC-SERVICE-DEPENDENCY-IN-EXECUTABLE-IREF/TARGET-SWC-SERVICE-DEPENDENCY-REF")
        if dependency_ref is not None:
            dependency_item = self.model.find_referable(dependency_ref.text)
            if dependency_item is not None:
                port_prototype_ref = get_element_or_none(dependency_item, "ASSIGNED-PORTS/ROLE-BASED-PORT-ASSIGNMENT/PORT-PROTOTYPE-REF")
                if port_prototype_ref is not None:
                    return self.get_port_dest(port_prototype_ref)

    def get_diagnostic_interface_service_instance_id(self, item, ob):
        dependency_ref = get_element_or_none(item, "SWC-SERVICE-DEPENDENCY-IN-EXECUTABLE-IREF/TARGET-SWC-SERVICE-DEPENDENCY-REF")
        if dependency_ref is not None:
            dependency_item = self.model.find_referable(dependency_ref.text)
            if dependency_item is not None:
                port_prototype_ref = get_element_or_none(dependency_item, "ASSIGNED-PORTS/ROLE-BASED-PORT-ASSIGNMENT/PORT-PROTOTYPE-REF")
                if port_prototype_ref is not None:
                    dest = self.get_port_dest(port_prototype_ref)
                    inOb = self.diag_instance_ob(dest, port_prototype_ref.text)
                    ob["serviceInstanceId"] = inOb["serviceInstanceId"]
                    ob["clientInstanceId"] = inOb["clientInstanceId"]
        return None

    def judge_is_this(self, port_prototype_item, processFqn, software_cluster):
        processRef = get_element_or_none(port_prototype_item, "PROCESS-REF")
        if processFqn is not None:
            return processRef.text == processFqn
        elif software_cluster is not None:
            process_refs = get_element_or_none(software_cluster, "CONTAINED-PROCESS-REFS")
            if process_refs is not None:
                process_mapping = process_refs.find_elements_of_type("CONTAINED-PROCESS-REF")
                for process_ref in process_mapping:
                    if process_ref.text == processRef.text:
                        return True
        else:
            return True
        return False

    def get_doip_interface_vin(self, isDoipd):
        diagnostic_data_identifier_map = self.model.find_elements_of_type("DIAGNOSTIC-DATA-IDENTIFIER")
        for item in diagnostic_data_identifier_map:
            represents_vin = get_element_or_false(item, "REPRESENTS-VIN")
            if represents_vin:
                vin = {
                    "dataIdentifier": transition_number(get_element_or_none(item, "ID"),defaultInt=None)
                }
                diagnostic_read_data_by_identifier = self.model.find_elements_of_type("DIAGNOSTIC-READ-DATA-BY-IDENTIFIER")
                for diagnostic_item in diagnostic_read_data_by_identifier:
                    data_identifier_ref = get_element_or_none(diagnostic_item, "DATA-IDENTIFIER-REF")
                    if data_identifier_ref is not None and data_identifier_ref.text == item.get_fqn():
                        generic_mapping = self.model.find_elements_of_type("DIAGNOSTIC-SERVICE-GENERIC-MAPPING")
                        for generic_item in generic_mapping:
                            instance_ref = get_element_or_none(generic_item, "DIAGNOSTIC-SERVICE-INSTANCE-REF")
                            if instance_ref is not None and instance_ref.text == diagnostic_item.get_fqn():
                                vin["diagnosticInterfaceClass"] = self.get_diagnostic_interface_class(generic_item)
                                self.get_diagnostic_interface_service_instance_id(generic_item, vin)
                                if isDoipd:
                                    vin["clientInstanceId"] = 1
                                else:
                                    vin["clientInstanceId"] = 2
                                return vin

    def find_position(self, d, key):
        for pos, (k, v) in enumerate(d.items()):
            if k == key:
                return pos
        return -1

    def diag_instance_ob(self, required_dest, portFqn):
        if portFqn in self.clientIdList:
            clientId = self.clientIdList.index(portFqn) + 1
        else:
            self.clientIdList.append(portFqn)
            clientId = len(self.clientIdList)
        if portFqn not in self.interfaceOb:
            serviceInstanceId = self.init_service_instance_id(required_dest)
            self.interfaceOb[portFqn] = {
                "serviceInstanceId": serviceInstanceId,
                "clientInstanceId": clientId,
            }
        intOb = self.interfaceOb[portFqn]
        return intOb

    def get_dapi_diagnosticInterface(self, diagnosticInterface, executable, process):
        all_instance = []
        InstanceSpecifierBuilder(self.model).get_exe_instance(all_instance, executable)
        for item in all_instance:
            required_dest = item["instanceDest"]
            if required_dest is not None:
                intOb = self.diag_instance_ob(required_dest, item["portFqn"])
                if required_dest == "DIAGNOSTIC-DO-IP-TRIGGER-VEHICLE-ANNOUNCEMENT-INTERFACE":
                    diagnosticInterface.append({
                        "specifier": "DefaultDoIpTriggerVehicleAnnouncementInstanceSpecifier",
                        "serviceInstanceId": intOb["serviceInstanceId"],
                        "clientInstanceId": intOb["clientInstanceId"]
                    })
                elif self.diag_interface(required_dest):
                    diagnosticInterface.append({
                        "specifier": item["instanceRef"],
                        "serviceInstanceId": intOb["serviceInstanceId"],
                        "clientInstanceId": intOb["clientInstanceId"]
                    })
        if process in self.scToP:
            scFqn = self.scToP[process]
            if scFqn not in self.conversionListOb:
                required_dest = "DIAGNOSTIC-CONVERSATION-INTERFACE"
                serviceInstanceId = self.init_service_instance_id(required_dest)
                self.conversionListOb[scFqn] = serviceInstanceId
            else:
                serviceInstanceId = self.conversionListOb[scFqn]
            self.clientIdList.append(process)
            clientId = len(self.clientIdList)
            diagnosticInterface.append({
                "specifier": "DefaultConversationInstanceSpecifier",
                "serviceInstanceId": serviceInstanceId,
                "clientInstanceId": clientId
            })

    def init_service_instance_id(self, required_dest):
        pos = self.find_position(self.interfaceNum, required_dest)
        if pos == -1:
            self.interfaceNum[required_dest] = 1
            serviceInstanceId = len(self.interfaceNum) * 10000 + 1
        else:
            self.interfaceNum[required_dest] = self.interfaceNum[required_dest] + 1
            serviceInstanceId = (pos + 1) * 10000 + self.interfaceNum[required_dest]
        return serviceInstanceId

    def diag_interface(self, required_dest):
        if required_dest is not None and required_dest in ["DIAGNOSTIC-CONDITION-INTERFACE", "DIAGNOSTIC-DATA-ELEMENT-INTERFACE", "DIAGNOSTIC-DATA-IDENTIFIER-GENERIC-INTERFACE",
                                                           "DIAGNOSTIC-DATA-IDENTIFIER-INTERFACE", "DIAGNOSTIC-DO-IP-ACTIVATION-LINE-INTERFACE", "DIAGNOSTIC-DO-IP-GROUP-IDENTIFICATION-INTERFACE",
                                                           "DIAGNOSTIC-DO-IP-POWER-MODE-INTERFACE", "DIAGNOSTIC-DO-IP-TRIGGER-VEHICLE-ANNOUNCEMENT-INTERFACE", "DIAGNOSTIC-DOWNLOAD-INTERFACE",
                                                           "DIAGNOSTIC-DTC-INFORMATION-INTERFACE", "DIAGNOSTIC-ECU-RESET-INTERFACE", "DIAGNOSTIC-EVENT-INTERFACE",
                                                           "DIAGNOSTIC-GENERIC-UDS-INTERFACE", "DIAGNOSTIC-INDICATOR-INTERFACE", "DIAGNOSTIC-MONITOR-INTERFACE",
                                                           "DIAGNOSTIC-OPERATION-CYCLE-INTERFACE", "DIAGNOSTIC-ROUTINE-GENERIC-INTERFACE", "DIAGNOSTIC-ROUTINE-INTERFACE",
                                                           "DIAGNOSTIC-SECURITY-LEVEL-INTERFACE", "DIAGNOSTIC-SERVICE-VALIDATION-INTERFACE", "DIAGNOSTIC-UPLOAD-INTERFACE",
                                                           "DIAGNOSTIC-COM-CONTROL-INTERFACE", "DIAGNOSTIC-FILETRANSFER-INTERFACE", "DIAGNOSTIC-AUTHENTICATION-INTERFACE", "DIAGNOSTIC-EXTERNAL-AUTHENTICATION-INTERFACE"]:
            return True
        return False

    def set_interface_ob_id_port(self, port_prototype_item):
        service_instance_ref = get_element_or_none(port_prototype_item, "SERVICE-INSTANCE-REF")
        if service_instance_ref is not None:
            service_instance_element = self.model.find_referable(service_instance_ref.text)
            admin = get_element_or_none(service_instance_element, "ADMIN-DATA")
            if admin is not None:
                sdgs = get_element_or_none(admin, "SDGS")
                if sdgs is not None:
                    sdg = get_element_or_none(sdgs, "SDG")
                    if sdg is not None:
                        sd_map = to_list(get_element_or_none(sdg, "SD"))
                        for sd_item in sd_map:
                            if "iSOFT:diag:ProvidedInstance:Identifier" == sd_item.attrib["GID"] or "iSOFT:diag:RequiredInstance:Identifier" == sd_item.attrib["GID"]:
                                return "NSOMEIP:{}".format(sd_item)

    def get_port_dest(self, port_prototype_ref):
        portPrototypeElement = self.model.find_referable(str(port_prototype_ref))
        interface_ref = get_element_or_none(portPrototypeElement, "REQUIRED-INTERFACE-TREF")
        if interface_ref is None:
            interface_ref = get_element_or_none(portPrototypeElement, "PROVIDED-INTERFACE-TREF")
        required_dest = None
        if interface_ref is not None:
            required_dest = interface_ref.attrib["DEST"]
        if required_dest is None:
            required_dest = get_element_or_none(portPrototypeElement, "ADMIN-DATA/SDGS/SDG/SD")
        return required_dest

    def get_dapi(self, processes):
        dapi_List = []
        for process in processes:
            ar_process = self.model.find_referable(process)
            diagnosticInterface = []
            executable_ref = get_element_or_none(ar_process, "EXECUTABLE-REF")
            executable = self.model.find_referable(executable_ref.text)
            self.get_dapi_diagnosticInterface(diagnosticInterface, executable, process)
            if len(diagnosticInterface) > 0:
                netConfig = self.get_process_net_config(process)
                if len(netConfig) > 0:
                    dapi = {
                        "dapi_name": short_name(process)+"_dapi.json",
                        "dapi": {
                            "netConfig": netConfig,
                            "diagnosticInterface": diagnosticInterface
                        }
                    }
                    dapi_List.append(dapi)

        return dapi_List

    def get_diag_element(self, ar_ports):
        ar_diag_data_element_ports = ar_ports.find_elements_of_type("PROVIDED-INTERFACE-TREF", DEST="DIAGNOSTIC-DATA-ELEMENT-INTERFACE")
        diag_element = []
        for ar_diag_data_element_port in ar_diag_data_element_ports:
            ar_diag_data_element = self.model.find_referable(ar_diag_data_element_port.text)
            item = self.get_comment_attr(ar_diag_data_element)
            self.get_read_item(item, ar_diag_data_element)
            diag_element.append(item)
        return diag_element

    def get_diag_identifier(self, ar_ports):
        ar_diag_data_element_ports = ar_ports.find_elements_of_type("PROVIDED-INTERFACE-TREF", DEST="DIAGNOSTIC-DATA-IDENTIFIER-INTERFACE")
        diag_identifier = []
        for ar_diag_data_element_port in ar_diag_data_element_ports:
            ar_diag_data_element = self.model.find_referable(ar_diag_data_element_port.text)
            item = self.get_comment_attr(ar_diag_data_element)
            self.get_read_item(item, ar_diag_data_element)
            writeElement = get_element_or_none(ar_diag_data_element, "WRITE")
            if writeElement is not None:
                data_prototype = []
                arguments = get_element_or_none(writeElement, "ARGUMENTS")
                if arguments is not None:
                    data_prototype_list = arguments.find_elements_of_type("ARGUMENT-DATA-PROTOTYPE")
                    for data_prototype_item in data_prototype_list:
                        data_item = {
                            "name": data_prototype_item.SHORT_NAME
                        }
                        data_type = get_element_or_none(data_prototype_item, "TYPE-TREF")
                        if data_type is not None:
                            data_item["writeType"] = self.set_data_type_name(data_type)
                        data_prototype.append(data_item)
                item["write"] = data_prototype
            diag_identifier.append(item)
        return diag_identifier

    def get_diag_routine(self, ar_ports):
        ar_diag_data_element_ports = ar_ports.find_elements_of_type("PROVIDED-INTERFACE-TREF",
                                                                    DEST="DIAGNOSTIC-ROUTINE-INTERFACE")
        diag_routine = []
        for ar_diag_data_element_port in ar_diag_data_element_ports:
            ar_diag_data_element = self.model.find_referable(ar_diag_data_element_port.text)
            item = self.get_comment_attr(ar_diag_data_element)
            methodList = []

            start = get_element_or_none(ar_diag_data_element, "START")
            if start is not None:
                start_ob = self.get_routine_item(start, item)
                if start_ob is not None:
                    methodList.append({
                        "name": "Start",
                        "lowerName": "start",
                        "ob": start_ob
                    })
            stop = get_element_or_none(ar_diag_data_element, "STOP")
            if stop is not None:
                stop_ob = self.get_routine_item(stop, item)
                if stop_ob is not None:
                    methodList.append({
                        "name": "Stop",
                        "lowerName": "stop",
                        "ob": stop_ob
                    })
            request_result = get_element_or_none(ar_diag_data_element, "REQUEST-RESULT")
            if request_result is not None:
                request_result_ob = self.get_routine_item(request_result, item)
                if request_result_ob is not None:
                    methodList.append({
                        "name": "RequestResults",
                        "lowerName": "requestResults",
                        "ob": request_result_ob
                    })
            item["methodList"] = methodList
            diag_routine.append(item)
        return diag_routine

    def set_data_type_name(self, dataTypeRef):
        if dataTypeRef is not None:
            dataTypeElement = self.model.find_referable(dataTypeRef.text)
            if dataTypeElement is not None:
                type_emitter = get_element_or_none(dataTypeElement, "TYPE-EMITTER")
                if type_emitter is not None and "TYPE_EMITTER_ARA" == type_emitter:
                    category = to_str(get_element_or_none(dataTypeElement, "CATEGORY")).upper()
                    if category == "VECTOR":
                        cpp = get_element_or_none(dataTypeElement, "TEMPLATE-ARGUMENTS/CPP-TEMPLATE-ARGUMENT/TEMPLATE-TYPE-REF")
                        if cpp is not None:
                            cppType = self.set_data_type_name(cpp)
                            return "ara::core::Vector<{}>".format(cppType)
                    if category == "ARRAY":
                        size = get_element_or_0(dataTypeElement, "ARRAY-SIZE")
                        cpp = get_element_or_none(dataTypeElement, "TEMPLATE-ARGUMENTS/CPP-TEMPLATE-ARGUMENT/TEMPLATE-TYPE-REF")
                        if cpp is not None:
                            cppType = self.set_data_type_name(cpp)
                            return "ara::core::Array<{}, {}>".format(cppType, size)
                    return dataTypeElement.SHORT_NAME
                dataType = dataTypeElement.SHORT_NAME
                if "int" in dataType:
                    return "std::" + dataType
                else:
                    return dataType

    def get_comment_attr(self, ar_diag_data_element):
        item = {
            "name": ar_diag_data_element.SHORT_NAME,
            "lowerName": self.change_to_lower_name(ar_diag_data_element.SHORT_NAME)
        }
        namespaces = []
        namespacesElement = get_element_or_none(ar_diag_data_element, "NAMESPACES")
        if namespacesElement is not None:
            symbol_props = namespacesElement.find_elements_of_type("SYMBOL-PROPS")
            for symbol_prop in symbol_props:
                symbol = get_element_or_none(symbol_prop, "SYMBOL")
                if symbol is not None:
                    namespaces.append(symbol.text)
        item["namespaces"] = namespaces
        return item

    def get_routine_item(self, element, item):
        arguments = get_element_or_none(element, "ARGUMENTS")
        if arguments is not None:
            item_ob = {}
            argument_data_prototype = arguments.find_elements_of_type("ARGUMENT-DATA-PROTOTYPE")
            inList = []
            for prototype_item in argument_data_prototype:
                direction = get_element_or_str(prototype_item, "DIRECTION")
                type_ref = get_element_or_str(prototype_item, "TYPE-TREF")
                if "OUT" == direction:
                    outItem = {
                        "name": short_name(type_ref.text)
                    }
                    out_element = self.model.find_referable(type_ref.text)
                    if out_element is not None:
                        output_type_list = []
                        sub_elements = get_element_or_none(out_element, "SUB-ELEMENTS")
                        if sub_elements is not None:
                            type_elements = sub_elements.find_elements_of_type("CPP-IMPLEMENTATION-DATA-TYPE-ELEMENT")
                            for type_item in type_elements:
                                output_item = {
                                    "name": type_item.SHORT_NAME
                                }
                                type_reference_ref = get_element_or_none(type_item, "TYPE-REFERENCE/TYPE-REFERENCE-REF")
                                if type_reference_ref is not None:
                                    output_item["outputType"] = self.set_data_type_name(type_reference_ref)
                                output_type_list.append(output_item)
                        outItem["outType"] = output_type_list
                    item_ob["out"] = outItem
                elif "IN" == direction:
                    inList.append({
                        "name": prototype_item.SHORT_NAME,
                        "inType": self.set_data_type_name(type_ref)
                    })
            item_ob["inList"] = inList
            return item_ob

    def get_read_item(self, item, ar_diag_data_element):
        readElement = get_element_or_none(ar_diag_data_element, "READ")
        if readElement is not None:
            read_out_ref = get_element_or_none(readElement, "ARGUMENTS/ARGUMENT-DATA-PROTOTYPE/TYPE-TREF")
            if read_out_ref is not None:
                readItem = {
                    "name": short_name(read_out_ref.text)
                }
                read_out_element = self.model.find_referable(read_out_ref.text)
                if read_out_element is not None:
                    output_type_list = []
                    sub_elements = get_element_or_none(read_out_element, "SUB-ELEMENTS")
                    if sub_elements is not None:
                        type_elements = sub_elements.find_elements_of_type("CPP-IMPLEMENTATION-DATA-TYPE-ELEMENT")
                        for type_item in type_elements:
                            output_item = {
                                "name": type_item.SHORT_NAME
                            }
                            type_reference_ref = get_element_or_none(type_item, "TYPE-REFERENCE/TYPE-REFERENCE-REF")
                            if type_reference_ref is not None:
                                output_item["outputType"] = self.set_data_type_name(type_reference_ref)
                            output_type_list.append(output_item)
                    readItem["readOut"] = output_type_list
                item["read"] = readItem

    def change_to_lower_name(self, name):
        result = ""
        first = False
        for s in name:
            if s.isupper() and first:
                result = result + "_" + s.lower()
            else:
                result = result + s.lower()
                first = True
        return result
