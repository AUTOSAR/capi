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
import logging

from . import commen_util
from generator.common.tree_helper import get_element_or_none, short_name, get_element_or_0, get_element_or_false, \
    get_element_or_default_1, get_element_or_str, to_list, transition_number, get_element_or_die
from .instance_specifier import InstanceSpecifierBuilder

"""
获取nm 模块Json
"""


class NMDeploymentBuilder:
    def __init__(self, model):
        self._log = logging.getLogger(__name__)
        self.model = model

    def get_nm_deployments(self, ar_machine):
        nmBean = {
            "defaultnmtimeouttime": 20,
            "defaultnmmsgcycletime": 5,
            "defaultnmrepeatmessagetime": 10,
            "defaultnmmsgcycleoffset": 10,
            "defaultnmpdunidposition": -1,
            "defaultnmpducbvposition": -1,
            "defaultnmpninfolength": -1,
            "defaultnmpninfooffset": -1,
            "defaultnmuserdatalength": 0,
            "defaultnmpnresettime": 0,
            "defaultnmpnenabled": False,
            "defaultnmnodeid": 1,
            "defaultnmuserdataenabled": False,
            "defaultnmwaitbussleeptime": 10,
            "defaultpncpreparesleeptime": 0
        }
        self.get_diag(nmBean, ar_machine)
        self.get_nm_logic_handle(nmBean, ar_machine)
        ethernetconnectorarray = []
        machine_design_ref = get_element_or_none(ar_machine, "MACHINE-DESIGN-REF")
        nodes = self.model.find_elements_of_type("UDP-NM-NODE")
        for node in nodes:
            machine_ref = get_element_or_none(node, "MACHINE-REF")
            connector_ref = get_element_or_none(node, "COMMUNICATION-CONNECTOR-REF")
            if connector_ref is not None and machine_design_ref == machine_ref:
                for item in ethernetconnectorarray:
                    if item["ethernetcommunicationconnectorfqn"] == connector_ref.text:
                        assert False, "[NM  ] CODE-002: {} is referenced by multiple UdpNmCluster.NmNodes.UdpNmNode. Fqn:{}. LocalPath:{}. ".format(connector_ref.text, node.get_fqn(), node.get_path())
                assert machine_ref.text in connector_ref.text, "[NM  ] CODE-003: {} and {} in {} do not match. LocalPath:{}. ".format(machine_ref.text, connector_ref.text, node.get_fqn(), node.get_path())
                connector = self.model.find_referable(connector_ref.text)
                connector_Bean = {
                    "ethernetcommunicationconnectorfqn": connector_ref.text,
                    "ethernetcommunicationconnector": get_element_or_none(connector, "SHORT-NAME"),
                    "maximumtransmissionunit": get_element_or_0(connector, "MAXIMUM-TRANSMISSION-UNIT")
                }
                data_mask = get_element_or_none(connector, "PNC-FILTER-DATA-MASK")
                data_mask = transition_number(data_mask, change=2, defaultStr="00")
                connector_Bean["pncfilterdatamask"] = data_mask[2:]
                network_edndpoint = get_element_or_none(connector, "UNICAST-NETWORK-ENDPOINT-REF")
                if network_edndpoint is not None:
                    network_edndpoints = self.model.find_referable(network_edndpoint.text)
                    if network_edndpoints is not None:
                        connector_Bean["ipv4address"] = get_element_or_none(network_edndpoints, "NETWORK-ENDPOINT-ADDRESSES/IPV-4-CONFIGURATION/IPV-4-ADDRESS")
                        connector_Bean["networkmask"] = get_element_or_none(network_edndpoints, "NETWORK-ENDPOINT-ADDRESSES/IPV-4-CONFIGURATION/NETWORK-MASK")
                self.get_nm_common(connector_Bean, machine_design_ref, connector.get_fqn())
                ethernetconnectorarray.append(connector_Bean)
        nmBean["ethernetconnectorarray"] = ethernetconnectorarray
        design = self.model.find_referable(machine_design_ref.text)
        if machine_design_ref is not None and machine_design_ref == design.get_fqn():
            nmBean["defaultpncpreparesleeptime"] = get_element_or_0(design, "PNC-PREPARE-SLEEP-TIMER")
            reset_time = get_element_or_none(design, "PN-RESET-TIMER")
            if reset_time is not None:
                nmBean["defaultnmpnresettime"] = reset_time
        nmMapping = {}
        self.get_nm_after_run_time_info(nmMapping, ar_machine)
        self.get_nm_handle_function(nmBean, nmMapping)

        return {"nmBean": nmBean,
                "nmMapping": nmMapping}

    def get_diag(self, nmBean, ar_machine):
        mapping = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING")
        for item in mapping:
            machineRef = get_element_or_none(item, "MACHINE-REF")
            if machineRef is not None and machineRef == ar_machine.get_fqn():
                processRef = get_element_or_die(item, "PROCESS-REF")
                process = self.model.find_referable(processRef.text)
                exeRef = get_element_or_none(process, "EXECUTABLE-REF")
                if exeRef is not None:
                    exe = self.model.find_referable(exeRef.text)
                    category = get_element_or_none(exe, "CATEGORY")
                    if exe.SHORT_NAME == "nmd" and category is not None and category.text == "PLATFORM_LEVEL":
                        all_instance = []
                        InstanceSpecifierBuilder(self.model).get_exe_instance(all_instance, exe)
                        for instanceItem in all_instance:
                            dest = instanceItem["instanceDest"]
                            if dest is not None:
                                if "DIAGNOSTIC-DATA-IDENTIFIER-GENERIC-INTERFACE" == dest:
                                    nmBean["didInstance"] = instanceItem["instanceRef"]
                                elif "DIAGNOSTIC-MONITOR-INTERFACE" == dest:
                                    nmBean["diagMonitorInstance"] = instanceItem["instanceRef"]
                        break

    def get_nm_after_run_time_info(self, nmBean, ar_machine):
        sdgList = to_list(get_element_or_none(ar_machine, "ADMIN-DATA/SDGS/SDG"))
        nmAfterRunTimeInfo = {
            "fgOffTime": 0.5,
            "networkOffTime": 0.5
        }
        for sdg in sdgList:
            if sdg.attrib["GID"] == "iSOFT:sm:NmHandleToFunctionGroupStateTimeControl":
                sdList = to_list(get_element_or_none(sdg, "SD"))
                for sd in sdList:
                    if sd.attrib["GID"] == "iSOFT:sm:NmHandleToFunctionGroupStateTimeControl:FGOffTime":
                        nmAfterRunTimeInfo["fgOffTime"] = float(sd)
                    elif sd.attrib["GID"] == "iSOFT:sm:NmHandleToFunctionGroupStateTimeControl:NetworkOffTime":
                        nmAfterRunTimeInfo["networkOffTime"] = float(sd)
                break
        nmBean["nmAfterRunTimeInfo"] = nmAfterRunTimeInfo

    def get_nm_handle_function(self, nmBean, nmMapping):
        nmhandle_to_function_group_state_mapping = []
        nmHandleInstances = []
        sdgs_map = self.model.find_elements_of_type("SDGS")
        for sdgs in sdgs_map:
            sdg = get_element_or_none(sdgs, "SDG")
            if sdg is not None:
                sdg_map = to_list(sdg)
                for sdg_item in sdg_map:
                    if "GID" in sdg_item.attrib:
                        if "iSOFT:sm:NmHandleToFunctionGroupStateMapping" == sdg_item.attrib["GID"]:
                            attributes_sdg = get_element_or_none(sdg_item, "SDG")
                            if attributes_sdg is not None and "iSOFT:sm:NmHandleToFunctionGroupStateMappingSubclass:Attributes" == attributes_sdg.attrib["GID"]:
                                typeStr = ""
                                typeSd = get_element_or_none(attributes_sdg, "SD")
                                if typeSd is not None and "iSOFT:sm:NmHandleToFunctionGroupStateMappingSubclass:Type" == typeSd.attrib["GID"]:
                                    typeStr = typeSd.text
                                sdg_elements = get_element_or_none(attributes_sdg, "SDG")
                                sdg_element_map = to_list(sdg_elements)
                                function_group_state_item = None
                                functionGroupStateList = []
                                for sdg_element in sdg_element_map:
                                    if "iSOFT:sm:NmHandleToFunctionGroupStateMappingSubclass:NmHandle" == sdg_element.attrib["GID"]:
                                        nmhandle = get_element_or_none(sdg_element, "SDX-REF")
                                        for nmlogichandl in nmBean["nmlogichandlearray"]:
                                            if nmlogichandl["_path"] == nmhandle.text:
                                                nmHandleStr = nmlogichandl["nmlogichandleindex"]
                                                function_group_state_item = {
                                                    "nmhandle": nmHandleStr,
                                                    "mappingDirection": typeStr
                                                }
                                                if "sm_required_instance_id" in nmlogichandl:
                                                    nmHandleInstance = {
                                                        "nmhandle": nmHandleStr,
                                                        "sm_required_instance_id": nmlogichandl["sm_required_instance_id"]
                                                    }
                                                    if nmHandleInstance not in nmHandleInstances:
                                                        nmHandleInstances.append(nmHandleInstance)
                                                    del nmlogichandl["sm_required_instance_id"]
                                                break
                                    elif "iSOFT:sm:NmHandleToFunctionGroupStateMappingSubclass:FunctionGroup" == sdg_element.attrib["GID"]:
                                        function_group = get_element_or_none(sdg_element, "SDG")
                                        function_groups = to_list(function_group)
                                        type_path = ""
                                        mode_declaration = ""
                                        for function_item in function_groups:
                                            if "iSOFT:sm:NmHandleToFunctionGroupStateMappingSubclass:ContextModeDeclarationGroupPrototype" == function_item.attrib["GID"]:
                                                type_path = get_element_or_str(function_item, "SDX-REF")
                                            elif "iSOFT:sm:NmHandleToFunctionGroupStateMappingSubclass:TargetModeDeclaration" == function_item.attrib["GID"]:
                                                mode_declaration = get_element_or_str(function_item, "SDX-REF")
                                            if type_path != "" and mode_declaration != "":
                                                functionGroupStateItem = {
                                                    "name": type_path,
                                                    "state": short_name(mode_declaration)
                                                }
                                                if typeStr in ["nmHandleActiveToFunctionGroupState", "nmHandleInactiveToFunctionGroupState"]:
                                                    for stateItem in functionGroupStateList:
                                                        assert type_path != stateItem["name"], "[NM  ] CODE-004: Type is '{}', it is not allowed to configure multiple states of a network corresponding to the same functional group. Path:{}. LocalPath:{}".format(typeStr, sdg_item.get_fqn(), sdg_item.get_path())
                                                if functionGroupStateItem not in functionGroupStateList:
                                                    functionGroupStateList.append(functionGroupStateItem)

                                if function_group_state_item is not None:
                                    function_group_state_item["functionGroupStates"] = functionGroupStateList
                                    nmhandle_to_function_group_state_mapping.append(function_group_state_item)
        nmMapping["nmHandleInstances"] = nmHandleInstances
        nmMapping["nmHandleToFunctionGroupStateMapping"] = nmhandle_to_function_group_state_mapping

    def is_machine_process(self, ar_machine, processFqn):
        if processFqn is not None:
            proc_to_machine_mappings = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING")
            for mapping in proc_to_machine_mappings:
                ar_machine_ref = get_element_or_none(mapping, "MACHINE-REF")
                if ar_machine_ref is not None and ar_machine_ref.text == ar_machine.get_fqn():
                    if processFqn.text == mapping.PROCESS_REF:
                        return True
        return False

    """
    获取logic_handle
    """

    def get_nm_logic_handle(self, nmBean, ar_machine):
        nmlogichandlearray = []
        instantiation_mapping = ar_machine.find_elements_of_type("NM-INSTANTIATION")
        network_str = []
        for instantiation in instantiation_mapping:
            network_handle = instantiation.find_elements_of_type("NM-NETWORK-HANDLE")
            for network in network_handle:
                isHave = False
                for item in nmlogichandlearray:
                    if item["_path"] == network.get_fqn():
                        isHave = True
                        break
                if not isHave:
                    handle_bean = {
                        "_path": network.get_fqn(),
                        "nmlogichandleindex": get_element_or_die(network, "SHORT-NAME")
                    }
                    network_refs = network.find_elements_of_type("PARTIAL-NETWORK-REF")
                    self.get_nm_ln_instance_specifier(handle_bean, ar_machine)
                    connector_refs = get_element_or_none(network, "VLAN-REFS/VLAN-REF")
                    ethernetconnectorarray = []
                    if connector_refs is not None:
                        connector_refs = to_list(connector_refs)
                        for connector_ref in connector_refs:
                            ethernetconnectorarray.append({
                                "ethernetcommunicationconnector": short_name(connector_ref.text)
                            })
                    handle_bean["ethernetconnectorarray"] = ethernetconnectorarray
                    partialnetworkarray = []
                    for ref in network_refs:
                        ident = self.model.find_referable(ref.text)
                        if ident is not None:
                            partialnetworkarray.append({
                                "pnindex": get_element_or_die(ident, "SHORT-NAME")
                            })
                            network_str.append(ident.get_fqn())
                    handle_bean["partialnetworkarray"] = partialnetworkarray
                    nmlogichandlearray.append(handle_bean)
        network_str = set(network_str)
        network_str = list(network_str)
        for network in network_str:
            self.get_nm_partial_network(nmBean, ar_machine, network)
        if len(network_str) > 0:
            ne = str(network_str[0])
            system_ma = self.model.find_elements_of_type("SYSTEM")
            for system in system_ma:
                system_name = system.get_fqn()
                if system_name in ne:
                    length = get_element_or_0(system, "PNC-VECTOR-LENGTH")
                    if length != 0:
                        nmBean["defaultnmpninfolength"] = length
                    offset = get_element_or_0(system, "PNC-VECTOR-OFFSET")
                    if offset != 0:
                        nmBean["defaultnmpninfooffset"] = offset
                    sd = get_element_or_none(system, "ADMIN-DATA/SDGS/SDG/SD")
                    if sd is not None and sd.attrib["GID"] == "iSOFT:nm:SystemCustomization:SkipTimerCheck":
                        nmBean["skiptimerCheck"] = sd
                    break
        nmBean["nmlogichandlearray"] = nmlogichandlearray

    def get_nm_ln_instance_specifier(self, handle_bean, ar_machine):
        net = "NetworkState_" + handle_bean["nmlogichandleindex"]
        port_mapping = self.model.find_elements_of_type("P-PORT-PROTOTYPE")
        for port in port_mapping:
            port_name = get_element_or_none(port, "SHORT-NAME")
            if net == port_name:
                port_path = port.get_fqn()
                p_port_mapping = self.model.find_elements_of_type("SERVICE-INSTANCE-TO-PORT-PROTOTYPE-MAPPING")
                for port_item in p_port_mapping:
                    target_port_ref = get_element_or_none(port_item, "PORT-PROTOTYPE-IREF/TARGET-PORT-PROTOTYPE-REF")
                    if target_port_ref is not None and target_port_ref == port_path:
                        process_ref = get_element_or_none(port_item, "PROCESS-REF")
                        isTrue = self.is_machine_process(ar_machine, process_ref)
                        if isTrue:
                            instance_specifier = commen_util.get_instance_specifier(self, port_item)
                            handle_bean["lninstancespecifier"] = instance_specifier
                            instance_id = ""
                            service_instance_ref = get_element_or_none(port_item, "SERVICE-INSTANCE-REF")
                            if service_instance_ref is not None:
                                service_instance_element = self.model.find_referable(service_instance_ref.text)
                                if service_instance_element is not None:
                                    some_id = get_element_or_none(service_instance_element, "SERVICE-INSTANCE-ID")
                                    if some_id is not None:
                                        instance_id = "NSOMEIP:{}".format(some_id)
                            handle_bean["sm_required_instance_id"] = instance_id
                            return

    """
    获取partial_network
    """

    def get_nm_partial_network(self, nmBean, ar_machine, pnc_ident_name):
        if "partialnetworkarray" in nmBean:
            partialnetworkarray = nmBean["partialnetworkarray"]
        else:
            partialnetworkarray = []
        pnc_mapping = self.model.find_elements_of_type("PNC-MAPPING")
        for pnc in pnc_mapping:
            if pnc is not None:
                partial_bean = {}
                ident_mapping = pnc.find_elements_of_type("IDENT")
                for ident in ident_mapping:
                    if ident is not None and ident.get_fqn() == pnc_ident_name:
                        isHave = False
                        for item in partialnetworkarray:
                            if item["_path"] == ident.get_fqn():
                                isHave = True
                                break
                        if not isHave:
                            partial_bean["_path"] = ident.get_fqn()
                            partial_bean["pnindex"] = get_element_or_none(ident, "SHORT-NAME")
                            partial_bean["pncidentifier"] = get_element_or_0(pnc, "PNC-IDENTIFIER")
                            ref_mapping = pnc.find_elements_of_type("PHYSICAL-CHANNEL-REF")
                            ethernetconnectorarray = []
                            for ref in ref_mapping:
                                physical = self.model.find_referable(ref.text)
                                endpoint_mapping = physical.find_elements_of_type("NETWORK-ENDPOINT")
                                for endpoint in endpoint_mapping:
                                    machine_design_ref = get_element_or_none(ar_machine, "MACHINE-DESIGN-REF")
                                    design = self.model.find_referable(machine_design_ref.text)
                                    connector_mapping = design.find_elements_of_type("ETHERNET-COMMUNICATION-CONNECTOR")
                                    for connector in connector_mapping:
                                        endpoint_ref_str = get_element_or_none(connector, "UNICAST-NETWORK-ENDPOINT-REF")
                                        if endpoint_ref_str is not None and endpoint_ref_str == endpoint.get_fqn():
                                            ethernetconnectorarray.append({
                                                "ethernetcommunicationconnector": get_element_or_die(connector, "SHORT-NAME")
                                            })
                            partial_bean["ethernetconnectorarray"] = ethernetconnectorarray
                            partialnetworkarray.append(partial_bean)
        nmBean["partialnetworkarray"] = partialnetworkarray

    """
    获取connector 公共参数
    """

    def get_nm_common(self, connector_Bean, machine_design_ref, connector_name):
        nm_clusters = self.model.find_elements_of_type('UDP-NM-CLUSTER')
        for nm_cluster in nm_clusters:
            nodes = nm_cluster.find_elements_of_type("UDP-NM-NODE")
            for node in nodes:
                machine_ref = get_element_or_none(node, "MACHINE-REF")
                connector_ref = get_element_or_none(node, "COMMUNICATION-CONNECTOR-REF")
                if machine_design_ref is not None and machine_design_ref == machine_ref:
                    if connector_name is not None and connector_name == connector_ref:
                        connector_Bean["nmnodeid"] = get_element_or_default_1(node, "NM-NODE-ID")
                        connector_Bean["nmmsgcycleoffset"] = get_element_or_default_1(node, "NM-MSG-CYCLE-OFFSET")
                        connector_Bean["allnmmessageskeepawake"] = get_element_or_false(node, "ALL-NM-MESSAGES-KEEP-AWAKE")
                        connector_Bean["nmpnhandlemultiplenetworkrequests"] = get_element_or_false(node, "NM-PN-HANDLE-MULTIPLE-NETWORK-REQUESTS")
                        connector_Bean["nmmsgcycletime"] = get_element_or_default_1(nm_cluster, "NM-MSG-CYCLE-TIME")
                        connector_Bean["nmnetworktimeout"] = get_element_or_default_1(nm_cluster, "NM-NETWORK-TIMEOUT")
                        connector_Bean["nmnidposition"] = get_element_or_default_1(nm_cluster, "NM-NID-POSITION")
                        connector_Bean["nmrepeatmessagetime"] = get_element_or_default_1(nm_cluster, "NM-REPEAT-MESSAGE-TIME")
                        connector_Bean["nmuserdatalength"] = get_element_or_default_1(nm_cluster, "NM-USER-DATA-LENGTH")
                        connector_Bean["nmuserdataoffset"] = get_element_or_default_1(nm_cluster, "NM-USER-DATA-OFFSET")
                        connector_Bean["nmwaitbussleeptime"] = get_element_or_default_1(nm_cluster, "NM-WAIT-BUS-SLEEP-TIME")
                        connector_Bean["nmimmediatenmtransmissions"] = get_element_or_0(nm_cluster, "NM-IMMEDIATE-NM-TRANSMISSIONS")
                        connector_Bean["nmimmediatenmcycletime"] = get_element_or_default_1(nm_cluster, "NM-IMMEDIATE-NM-CYCLE-TIME")
                        connector_Bean["nmcbvposition"] = get_element_or_default_1(nm_cluster, "NM-CBV-POSITION")
                        connector_Bean["nmpncparticipation"] = get_element_or_false(nm_cluster, "NM-PNC-PARTICIPATION")
                        connector_Bean["udpport"] = get_element_or_default_1(nm_cluster, "NETWORK-CONFIGURATION/UDP-PORT")
                        connector_Bean["ipv4multicastipaddress"] = get_element_or_str(nm_cluster, "NETWORK-CONFIGURATION/IPV-4-MULTICAST-IP-ADDRESS")
                        connector_Bean["udpnmclustername"] = get_element_or_str(nm_cluster, "SHORT-NAME")
                        sd = get_element_or_none(node, "ADMIN-DATA/SDGS/SDG/SD")
                        if sd is not None and sd.attrib["GID"] == "iSOFT:nm:UdpNmNodeExtension:UserDataString":
                            connector_Bean["userDataFilled"] = str(sd.text)
                        sdg = get_element_or_none(nm_cluster, "ADMIN-DATA/SDGS/SDG")
                        if sdg is not None and sdg.attrib["GID"] == "iSOFT:nm:UdpNmClusterExtension":
                            sdList = sdg.find_elements_of_type("SD")
                            for sdItem in sdList:
                                if "iSOFT:nm:UdpNmClusterExtension:SendImmediateInRepeatMessage" == sdItem.attrib["GID"]:
                                    connector_Bean["sendImmediateInRepeatMessage"] = sdItem
                                elif "iSOFT:nm:UdpNmClusterExtension:RepeatMessageRequestPnCheck" == sdItem.attrib["GID"]:
                                    connector_Bean["rrcPnCheck"] = sdItem
                                elif "iSOFT:nm:UdpNmClusterExtension:SetActiveWakeupBit" == sdItem.attrib["GID"]:
                                    connector_Bean["setActiveWakeupBit"] = sdItem
                                elif "iSOFT:nm:UdpNmClusterExtension:NmStateInUserDataOffset" == sdItem.attrib["GID"]:
                                    connector_Bean["nmStateInUserDataOffset"] = transition_number(sdItem.text)
                                elif "iSOFT:nm:UdpNmClusterExtension:BusSleepToRepeatMessageState" == sdItem.attrib["GID"]:
                                    connector_Bean["busSleepToRepeatMessageState"] = transition_number(sdItem.text)
                                elif "iSOFT:nm:UdpNmClusterExtension:PrepareBusSleepToRepeatMessageState" == sdItem.attrib["GID"]:
                                    connector_Bean["PrepareBusSleepToRepeatMessageState"] = transition_number(sdItem.text)
                                elif "iSOFT:nm:UdpNmClusterExtension:RepeatMessageStateToNormalOperationState" == sdItem.attrib["GID"]:
                                    connector_Bean["repeatMessageStateToNormalOperationState"] = transition_number(sdItem.text)
                                elif "iSOFT:nm:UdpNmClusterExtension:ReadySleepStateToNormalOperationState" == sdItem.attrib["GID"]:
                                    connector_Bean["readySleepStateToNormalOperationState"] = transition_number(sdItem.text)
                                elif "iSOFT:nm:UdpNmClusterExtension:ReadySleepStateToRepeatMessageState" == sdItem.attrib["GID"]:
                                    connector_Bean["readySleepStateToRepeatMessageState"] = transition_number(sdItem.text)
                                elif "iSOFT:nm:UdpNmClusterExtension:NormalOperationStateToRepeatMessageState" == sdItem.attrib["GID"]:
                                    connector_Bean["normalOperationStateToRepeatMessageState"] = transition_number(sdItem.text)
                        break

            else:
                continue
            break
