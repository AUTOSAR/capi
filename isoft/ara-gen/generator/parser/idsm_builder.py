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

from generator.parser import commen_util
from generator.common.tree_helper import get_element_or_none, get_element_or_str, get_element_or_0, to_list, \
    short_name, get_element_or_false,transition_number


class IDSMBuilder:

    def __init__(self, model):
        self._log = logging.getLogger(__name__)
        self.model = model

    def get_idsm_contribute(self, machine):
        idsm_module_instantiation = get_element_or_none(machine, "MODULE-INSTANTIATIONS/IDSM-MODULE-INSTANTIATION")
        if idsm_module_instantiation is not None:
            idsm_instance = self.find_idsm_instance(idsm_module_instantiation.get_fqn())
            if idsm_instance is not None:
                securityEventFilterChains = []
                securityEventContextProps = []
                securityEventDefinitions = []
                dtcToSecurityEventContextProps = []
                securityEventContextMappingApplications = []
                securityEventContextMappingBswModules = []
                securityEventContextMappingFunctionalClusters = []
                securityEventContextMappingCommConnectors = []
                didToSecurityEventContextProps = []
                securityEventReportedMappingProcess = []
                securityEventReportedMappingPort = []
                idsm_instance_ob = {"shortName": short_name(idsm_instance.get_fqn()),
                                    "fqn": idsm_instance.get_fqn()}
                block_states = get_element_or_none(idsm_instance, "BLOCK-STATES/BLOCK-STATE")
                if block_states is not None:
                    blockState = []
                    block_state_map = to_list(block_states)
                    for block_state in block_state_map:
                        blockState.append(block_state.get_fqn())
                    idsm_instance_ob["blockState"] = blockState
                ecu_instance_conditionals = get_element_or_none(idsm_instance, "ECU-INSTANCES/ECU-INSTANCE-REF-CONDITIONAL")
                for ecu_instance_conditional in to_list(ecu_instance_conditionals):
                    ecu_instance_ref = get_element_or_none(ecu_instance_conditional, "ECU-INSTANCE-REF")
                    if ecu_instance_ref is not None:
                        idsm_instance_ob["ecuInstance"] = ecu_instance_ref.text
                idsm_instance_id = get_element_or_none(idsm_instance, "IDSM-INSTANCE-ID")
                if idsm_instance_id is not None:
                    idsm_instance_ob["idsmInstanceId"] = transition_number(idsm_instance_id)

                idsmModuleInstantiation = {}
                sdg = get_element_or_none(idsm_instance, "ADMIN-DATA/SDGS/SDG")
                if sdg is not None:
                    sds = get_element_or_none(sdg, "SD")
                    if sds is not None:
                        memoryLimitation = {}
                        sds = to_list(sds)
                        for sd_item in sds:
                            if "iSOFT:idsm:Limitation:Event" == sd_item.attrib["GID"]:
                                memoryLimitation["event"] = sd_item
                            elif "iSOFT:idsm:Limitation:Context" == sd_item.attrib["GID"]:
                                memoryLimitation["context"] = sd_item
                        idsm_instance_ob["memoryLimitation"] = memoryLimitation
                netOb = {
                    "tcpPort": 0,
                    "ipv4IpAddress": ""
                }
                sdgIp = get_element_or_none(idsm_module_instantiation, "ADMIN-DATA/SDGS/SDG")
                if sdgIp is not None and "iSOFT:idsm:Idsr" == sdgIp.attrib["GID"]:
                    sd = get_element_or_none(sdgIp, "SD")
                    if sd is not None and "iSOFT:idsm:Idsr:Port" == sd.attrib["GID"]:
                        netOb["tcpPort"] = sd
                    sdg = get_element_or_none(sdgIp, "SDG")
                    if sdg is not None and sdg.attrib["GID"] == "iSOFT:idsm:Idsr:IpAddr":
                        sdxRef = get_element_or_none(sdg, "SDX-REF")
                        if sdxRef is not None:
                            network = self.model.find_referable(sdxRef.text)
                            ipv4Address = get_element_or_none(network, "NETWORK-ENDPOINT-ADDRESSES/IPV-4-CONFIGURATION/IPV-4-ADDRESS")
                            if ipv4Address is not None:
                                netOb["ipv4IpAddress"] = ipv4Address

                idsmModuleInstantiation["networkInterface"] = netOb
                idsmModuleInstantiation["timeBase"] = ""
                time_bases = get_element_or_none(idsm_module_instantiation, "TIME-BASES/TIME-BASE-RESOURCE-REF-CONDITIONAL")
                for time_base in to_list(time_bases):
                    time_base_resource = get_element_or_none(time_base, "TIME-BASE-RESOURCE-REF")
                    if time_base_resource is not None:
                        assert time_base_resource.attrib["DEST"] == "SYNCHRONIZED-TIME-BASE-CONSUMER", "[IDSM] CODE-001: IdsmModuleInstantiation[{}].TimeBases.TimeBaseResourceRefConditional.timeBaseResourceRef must be SynchronizedTimeBaseConsumer. FQN:{}. LocalPath:{}. ".format(idsm_module_instantiation.SHORT_NAME, idsm_module_instantiation.get_fqn(), idsm_module_instantiation.get_path())
                        idsmModuleInstantiation["timeBase"] = time_base_resource.text[1:]

                idsm_instance_ob["idsmModuleInstantiation"] = idsmModuleInstantiation
                idsm_rate_limitation_ref_cons = get_element_or_none(idsm_instance, "RATE-LIMITATION-FILTERS/IDSM-RATE-LIMITATION-REF-CONDITIONAL")
                for idsm_rate_limitation_ref_con in to_list(idsm_rate_limitation_ref_cons):
                    idsm_rate_limitation_ref = get_element_or_none(idsm_rate_limitation_ref_con, "IDSM-RATE-LIMITATION-REF")
                    if idsm_rate_limitation_ref is not None:
                        idsm_rate_limitation = self.model.find_referable(idsm_rate_limitation_ref.text)
                        idsm_instance_ob["rateLimitationFilter"] = {
                            "maxEventsInInterval": transition_number(get_element_or_0(idsm_rate_limitation, "MAX-EVENTS-IN-INTERVAL")),
                            "timeInterval": get_element_or_0(idsm_rate_limitation, "TIME-INTERVAL")
                        }
                idsm_traffic_limitation_ref_cons = get_element_or_none(idsm_instance, "TRAFFIC-LIMITATION-FILTERS/IDSM-TRAFFIC-LIMITATION-REF-CONDITIONAL")
                for idsm_traffic_limitation_ref_con in to_list(idsm_traffic_limitation_ref_cons):
                    idsm_traffic_limitation_ref = get_element_or_none(idsm_traffic_limitation_ref_con, "IDSM-TRAFFIC-LIMITATION-REF")
                    if idsm_traffic_limitation_ref is not None:
                        idsm_traffic_limitation = self.model.find_referable(idsm_traffic_limitation_ref.text)
                        idsm_instance_ob["trafficLimitationFilter"] = {
                            "maxBytesInInterval": transition_number(get_element_or_0(idsm_traffic_limitation, "MAX-BYTES-IN-INTERVAL")),
                            "timeInterval": get_element_or_0(idsm_traffic_limitation, "TIME-INTERVAL")
                        }
                apKeySolt = get_element_or_str(idsm_instance, "SIGNATURE-SUPPORT-AP/KEY-SLOT-REF")
                apCryptoPrimitive = get_element_or_str(idsm_instance, "SIGNATURE-SUPPORT-AP/CRYPTO-PRIMITIVE")
                idsm_instance_ob["signatureSupportAp"] = {
                    "cryptoPrimitive": apCryptoPrimitive,
                    "keySlot": str(apKeySolt)[1:]
                }

                idsm_instance_ob["signatureSupportCp"] = ""
                idsm_instance_ob["timestampFormat"] = get_element_or_str(idsm_instance, "TIMESTAMP-FORMAT")

                idsmInstanceFqn = idsm_instance.get_fqn()
                application_mapping = self.model.find_elements_of_type("SECURITY-EVENT-CONTEXT-MAPPING-APPLICATION")

                for application_item in application_mapping:
                    idsmInstanceRefCon = get_element_or_none(application_item, "IDSM-INSTANCES/IDSM-INSTANCE-REF-CONDITIONAL")
                    for idsmInstanceRefConItem in to_list(idsmInstanceRefCon):
                        idsmInstanceRef = get_element_or_none(idsmInstanceRefConItem, "IDSM-INSTANCE-REF")
                        if idsmInstanceRef is not None and idsmInstanceFqn == idsmInstanceRef.text:
                            applicationOb = {"shortName": application_item.SHORT_NAME,
                                             "fqn": application_item.get_fqn(),
                                             "affectedApplication": get_element_or_str(application_item, "AFFECTED-APPLICATION")}
                            self.get_mapping_other(applicationOb, application_item, securityEventFilterChains, securityEventContextProps, securityEventDefinitions, dtcToSecurityEventContextProps, didToSecurityEventContextProps, securityEventReportedMappingPort, securityEventReportedMappingProcess)
                            securityEventContextMappingApplications.append(applicationOb)
                bswMapping = self.model.find_elements_of_type("SECURITY-EVENT-CONTEXT-MAPPING-BSW-MODULE")
                for item in bswMapping:
                    idsmInstanceRefCon = get_element_or_none(item, "IDSM-INSTANCES/IDSM-INSTANCE-REF-CONDITIONAL")
                    for idsmInstanceRefConItem in to_list(idsmInstanceRefCon):
                        idsmInstanceRef = get_element_or_none(idsmInstanceRefConItem, "IDSM-INSTANCE-REF")
                        if idsmInstanceRef is not None and idsmInstanceFqn == idsmInstanceRef.text:
                            ob = {"shortName": item.SHORT_NAME,
                                  "fqn": item.get_fqn(),
                                  "affectedBswModule": get_element_or_str(item, "AFFECTED-BSW-MODULE")}
                            self.get_mapping_other(ob, item, securityEventFilterChains, securityEventContextProps, securityEventDefinitions, dtcToSecurityEventContextProps, didToSecurityEventContextProps, securityEventReportedMappingPort, securityEventReportedMappingProcess)
                            securityEventContextMappingBswModules.append(ob)
                clusterMapping = self.model.find_elements_of_type("SECURITY-EVENT-CONTEXT-MAPPING-FUNCTIONAL-CLUSTER")
                for item in clusterMapping:
                    idsmInstanceRefCon = get_element_or_none(item, "IDSM-INSTANCES/IDSM-INSTANCE-REF-CONDITIONAL")
                    for idsmInstanceRefConItem in to_list(idsmInstanceRefCon):
                        idsmInstanceRef = get_element_or_none(idsmInstanceRefConItem, "IDSM-INSTANCE-REF")
                        if idsmInstanceRef is not None and idsmInstanceFqn == idsmInstanceRef.text:
                            ob = {"shortName": item.SHORT_NAME,
                                  "fqn": item.get_fqn(),
                                  "affectedFunctionalCluster": get_element_or_str(item, "AFFECTED-FUNCTIONAL-CLUSTER")}
                            self.get_mapping_other(ob, item, securityEventFilterChains, securityEventContextProps, securityEventDefinitions, dtcToSecurityEventContextProps, didToSecurityEventContextProps, securityEventReportedMappingPort, securityEventReportedMappingProcess)
                            securityEventContextMappingFunctionalClusters.append(ob)
                commMapping = self.model.find_elements_of_type("SECURITY-EVENT-CONTEXT-MAPPING-COMM-CONNECTOR")
                for item in commMapping:
                    idsmInstanceRefCon = get_element_or_none(item, "IDSM-INSTANCES/IDSM-INSTANCE-REF-CONDITIONAL")
                    for idsmInstanceRefConItem in to_list(idsmInstanceRefCon):
                        idsmInstanceRef = get_element_or_none(idsmInstanceRefConItem, "IDSM-INSTANCE-REF")
                        if idsmInstanceRef is not None and idsmInstanceFqn == idsmInstanceRef.text:
                            ob = {"shortName": item.SHORT_NAME,
                                  "fqn": item.get_fqn()}
                            commList = []
                            conditional = get_element_or_none(item, "COMM-CONNECTORS/COMMUNICATION-CONNECTOR-REF-CONDITIONAL")
                            if conditional is not None:
                                conditional = to_list(conditional)
                                for cItem in conditional:
                                    ref = get_element_or_none(cItem, "COMMUNICATION-CONNECTOR-REF")
                                    if ref is not None:
                                        commList.append(ref.text)
                            ob["commConnector"] = commList
                            self.get_mapping_other(ob, item, securityEventFilterChains, securityEventContextProps, securityEventDefinitions, dtcToSecurityEventContextProps, didToSecurityEventContextProps, securityEventReportedMappingPort, securityEventReportedMappingProcess)
                            securityEventContextMappingCommConnectors.append(ob)
                self.set_other_event_definitions(securityEventDefinitions)
                return {
                    "idsmInstance": idsm_instance_ob,
                    "securityEventFilterChains": securityEventFilterChains,
                    "securityEventDefinitions": securityEventDefinitions,
                    "didToSecurityEventContextProps": didToSecurityEventContextProps,
                    "dtcToSecurityEventContextProps": dtcToSecurityEventContextProps,
                    "securityEventContextProps": securityEventContextProps,
                    "securityEventReportedMappingProcess": securityEventReportedMappingProcess,
                    "securityEventReportedMappingPort": securityEventReportedMappingPort,
                    "securityEventContextMappingBswModules": securityEventContextMappingBswModules,
                    "securityEventContextMappingFunctionalClusters": securityEventContextMappingFunctionalClusters,
                    "securityEventContextMappingCommConnectors": securityEventContextMappingCommConnectors,
                    "securityEventContextMappingApplications": securityEventContextMappingApplications

                }

    def get_mapping_other(self, ob, item, securityEventFilterChains, securityEventContextProps, securityEventDefinitions, dtcToSecurityEventContextProps, didToSecurityEventContextProps, securityEventReportedMappingPort, securityEventReportedMappingProcess):
        eventFilterRefCon = get_element_or_none(item, "FILTER-CHAINS/SECURITY-EVENT-FILTER-CHAIN-REF-CONDITIONAL")
        for eventFilterRefConItem in to_list(eventFilterRefCon):
            eventFilterRef = get_element_or_none(eventFilterRefConItem, "SECURITY-EVENT-FILTER-CHAIN-REF")
            if eventFilterRef is not None:
                ob["filterChain"] = eventFilterRef.text
                self.set_event_filter(eventFilterRef, securityEventFilterChains)
            else:
                ob["filterChain"] = ""
        mappedSecurityEvent = []
        securityEventMapping = get_element_or_none(item, "MAPPED-SECURITY-EVENTS/SECURITY-EVENT-CONTEXT-PROPS")
        if securityEventMapping is not None:
            securityEventMapping = to_list(securityEventMapping)
            for securityEventItem in securityEventMapping:
                mappedSecurityEvent.append(securityEventItem.get_fqn())
                self.set_security_event_context_props(securityEventItem, securityEventContextProps, securityEventDefinitions, securityEventReportedMappingPort, securityEventReportedMappingProcess)
                self.set_dtc_to_security_event_context_props(securityEventItem, dtcToSecurityEventContextProps)
                self.set_did_security_event_context_props(securityEventItem, didToSecurityEventContextProps)

        ob["mappedSecurityEvent"] = mappedSecurityEvent

    def set_did_security_event_context_props(self, securityEventItem, didToSecurityEventContextProps):
        fqn = securityEventItem.get_fqn()
        for item in didToSecurityEventContextProps:
            if item["securityEventContextPropsFqn"] == fqn:
                return
        ob = {
            "securityEventContextPropsFqn": fqn
        }
        mode_mapping = self.model.find_elements_of_type("DIAGNOSTIC-SECURITY-EVENT-REPORTING-MODE-MAPPING")
        for modeItem in mode_mapping:
            security_event_ref = get_element_or_none(modeItem, "SECURITY-EVENT-REF")
            if security_event_ref is not None and security_event_ref.text == fqn:
                data_element_ref = get_element_or_none(modeItem, "DATA-ELEMENT-REF")
                if data_element_ref is not None:
                    data_identifier_mapping = self.model.find_elements_of_type("DIAGNOSTIC-DATA-IDENTIFIER")
                    for data_identifier_item in data_identifier_mapping:
                        data_element_mapping = get_element_or_none(data_identifier_item, "DATA-ELEMENTS/DIAGNOSTIC-PARAMETER/DATA-ELEMENTS/DIAGNOSTIC-DATA-ELEMENT")
                        if data_element_mapping is not None:
                            data_element_mapping = to_list(data_element_mapping)
                            for data_element_item in data_element_mapping:
                                if data_element_item.get_fqn() == data_element_ref.text:
                                    ob["did"] = transition_number(get_element_or_0(data_identifier_item, "ID"))
                                    didToSecurityEventContextProps.append(ob)
                                    return

    def set_dtc_to_security_event_context_props(self, securityEventItem, dtcToSecurityEventContextProps):
        fqn = securityEventItem.get_fqn()
        for item in dtcToSecurityEventContextProps:
            if item["securityEventContextPropsFqn"] == fqn:
                return
        ob = {
            "securityEventContextPropsFqn": fqn
        }
        self.set_monitor(ob, fqn, dtcToSecurityEventContextProps)

    def set_monitor(self, ob, fqn, dtcToSecurityEventContextProps):
        mapping = self.model.find_elements_of_type("DIAGNOSTIC-EVENT-TO-SECURITY-EVENT-MAPPING")
        for eItem in mapping:
            security_event_props_ref = get_element_or_none(eItem, "SECURITY-EVENT-PROPS-REF")
            if security_event_props_ref is not None and security_event_props_ref.text == fqn:
                diagnostic_event_ref = get_element_or_none(eItem, "DIAGNOSTIC-EVENT-REF")
                if diagnostic_event_ref is not None:
                    self.set_dtc_did(ob, diagnostic_event_ref)
                    diagnostic_event_port_mapping = self.model.find_elements_of_type("DIAGNOSTIC-EVENT-PORT-MAPPING")
                    for diagnostic_event_port_item in diagnostic_event_port_mapping:
                        map_instance_ref = get_element_or_none(diagnostic_event_port_item, "DIAGNOSTIC-EVENT-REF")
                        if map_instance_ref is not None and map_instance_ref == diagnostic_event_ref:
                            interface = self.get_diagnostic_interface_class(diagnostic_event_port_item)
                            if interface is not None and "DIAGNOSTIC-MONITOR-INTERFACE" == interface:
                                dependency_ref = get_element_or_none(diagnostic_event_port_item, "SWC-SERVICE-DEPENDENCY-IN-EXECUTABLE-IREF/TARGET-SWC-SERVICE-DEPENDENCY-REF")
                                if dependency_ref is not None:
                                    dependency_element = self.model.find_referable(dependency_ref.text)
                                    if dependency_element is not None:
                                        port_prototype_ref = get_element_or_none(dependency_element, "ASSIGNED-PORTS/ROLE-BASED-PORT-ASSIGNMENT/PORT-PROTOTYPE-REF")
                                        if port_prototype_ref is not None:
                                            port_prototype_mapping = self.model.find_elements_of_type("SERVICE-INSTANCE-TO-PORT-PROTOTYPE-MAPPING")
                                            for port_prototype_item in port_prototype_mapping:
                                                port_prototype_item_ref = get_element_or_str(port_prototype_item, "PORT-PROTOTYPE-IREF/TARGET-PORT-PROTOTYPE-REF")
                                                if port_prototype_ref == port_prototype_item_ref:
                                                    ob["monitorInstance"] = commen_util.get_instance_specifier(self, port_prototype_item)
                                                    dtcToSecurityEventContextProps.append(ob)
                                                    return

    def set_dtc_did(self, ob, diagnostic_event_ref):
        uds_mapping = self.model.find_elements_of_type("DIAGNOSTIC-EVENT-TO-TROUBLE-CODE-UDS-MAPPING")
        for udsItem in uds_mapping:
            event_ref = get_element_or_none(udsItem, "DIAGNOSTIC-EVENT-REF")
            if event_ref is not None and event_ref.text == diagnostic_event_ref.text:
                code_uds_ref = get_element_or_none(udsItem, "TROUBLE-CODE-UDS-REF")
                if code_uds_ref is not None:
                    code_uds = self.model.find_referable(code_uds_ref.text)
                    dtc_props_ref = get_element_or_none(code_uds, "DTC-PROPS-REF")
                    if dtc_props_ref is not None:
                        dtc_props_ele = self.model.find_referable(dtc_props_ref.text)
                        id_set_ref_con = get_element_or_none(dtc_props_ele, "SNAPSHOT-RECORD-CONTENTS/DIAGNOSTIC-DATA-IDENTIFIER-SET-REF-CONDITIONAL")
                        for identifier_set_item in id_set_ref_con:
                            id_set_ref = get_element_or_none(identifier_set_item, "DIAGNOSTIC-DATA-IDENTIFIER-SET-REF")
                            if id_set_ref is not None:
                                identifier_set_ele = self.model.find_referable(id_set_ref.text)
                                data_identifier_ref = get_element_or_none(identifier_set_ele, "DATA-IDENTIFIER-REFS/DATA-IDENTIFIER-REF")
                                if data_identifier_ref is not None:
                                    data_identifier_ele = self.model.find_referable(data_identifier_ref.text)
                                    ob["did"] = transition_number(get_element_or_0(data_identifier_ele, "ID"))
                                    diagnostic_read_data_mapping = self.model.find_elements_of_type("DIAGNOSTIC-READ-DATA-BY-IDENTIFIER")
                                    for diagnostic_read_data_item in diagnostic_read_data_mapping:
                                        data_identifier_r = get_element_or_none(diagnostic_read_data_item, "DATA-IDENTIFIER-REF")
                                        if data_identifier_r is not None and data_identifier_r.text == data_identifier_ref.text:
                                            generic_mapping = self.model.find_elements_of_type("DIAGNOSTIC-SERVICE-GENERIC-MAPPING")
                                            for generic_item in generic_mapping:
                                                service_instance_ref = get_element_or_none(generic_item, "DIAGNOSTIC-SERVICE-INSTANCE-REF")
                                                if service_instance_ref is not None and service_instance_ref.text == diagnostic_read_data_item.get_fqn():
                                                    dep_ref = get_element_or_none(generic_item, "SWC-SERVICE-DEPENDENCY-IN-EXECUTABLE-IREF/TARGET-SWC-SERVICE-DEPENDENCY-REF")
                                                    if dep_ref is not None:
                                                        dependency_element = self.model.find_referable(dep_ref.text)
                                                        if dependency_element is not None:
                                                            port_prototype_ref = get_element_or_none(dependency_element, "ASSIGNED-PORTS/ROLE-BASED-PORT-ASSIGNMENT/PORT-PROTOTYPE-REF")
                                                            if port_prototype_ref is not None:
                                                                port_prototype_mapping = self.model.find_elements_of_type("SERVICE-INSTANCE-TO-PORT-PROTOTYPE-MAPPING")
                                                                for port_prototype_item in port_prototype_mapping:
                                                                    port_prototype_item_ref = get_element_or_str(port_prototype_item, "PORT-PROTOTYPE-IREF/TARGET-PORT-PROTOTYPE-REF")
                                                                    if port_prototype_ref == port_prototype_item_ref:
                                                                        ob["didInstance"] = commen_util.get_instance_specifier(self, port_prototype_item)
                                                                        return

    def set_security_event_context_props(self, securityEventItem, securityEventContextProps, securityEventDefinitions, securityEventReportedMappingPort, securityEventReportedMappingProcess):
        fqn = securityEventItem.get_fqn()
        for item in securityEventContextProps:
            if item["fqn"] == fqn:
                return
        itemOb = {
            "shortName": securityEventItem.SHORT_NAME,
            "fqn": fqn,
            "contextData": get_element_or_str(securityEventItem, "CONTEXT-DATAS/SECURITY-EVENT-CONTEXT-DATA/VARIATION-POINT/SHORT-LABEL"),
            "defaultReportingMode": get_element_or_str(securityEventItem, "DEFAULT-REPORTING-MODE"),
            "persistentStorage": get_element_or_false(securityEventItem, "PERSISTENT-STORAGE"),
            "sensorInstanceId": transition_number(get_element_or_0(securityEventItem, "SENSOR-INSTANCE-ID")),
            "severity": transition_number(get_element_or_0(securityEventItem, "SEVERITY"))
        }
        securityEventRefCon = get_element_or_none(securityEventItem, "SECURITY-EVENTS/SECURITY-EVENT-DEFINITION-REF-CONDITIONAL")
        for securityEventRefConItem in to_list(securityEventRefCon):
            securityEventRef = get_element_or_str(securityEventRefConItem, "SECURITY-EVENT-DEFINITION-REF")
            itemOb["securityEvent"] = securityEventRef
            securityEventContextProps.append(itemOb)
            if securityEventRef != "":
                self.set_security_event_definitions(securityEventRef.text, securityEventDefinitions)
                self.set_security_event_reported_mapping_port(securityEventRef, securityEventReportedMappingPort, securityEventReportedMappingProcess)

    def set_security_event_reported_mapping_port(self, securityEventRef, securityEventReportedMappingPort, securityEventReportedMappingProcess):
        definition_mapping = self.model.find_elements_of_type("SECURITY-EVENT-REPORT-TO-SECURITY-EVENT-DEFINITION-MAPPING")
        for definition_item in definition_mapping:
            definition_ref = get_element_or_none(definition_item, "SECURITY-EVENT-DEFINITION-REF")
            if definition_ref is not None and definition_ref.text == securityEventRef.text:
                port_ref = get_element_or_str(definition_item, "REPORTED-SECURITY-EVENT-IREF/TARGET-R-PORT-REF")
                isHave = False
                for item in securityEventReportedMappingPort:
                    if item["fqn"] == definition_item.get_fqn():
                        isHave = True
                        break
                if not isHave and port_ref != "":
                    eventPortInstance = self.set_security_event_reported_mapping_process(port_ref, securityEventReportedMappingProcess)
                    securityEventReportedMappingPort.append({
                        "shortName": definition_item.SHORT_NAME,
                        "fqn": definition_item.get_fqn(),
                        "eventPortInstance": eventPortInstance,
                        "securityEventDefinition": securityEventRef
                    })

    def set_security_event_reported_mapping_process(self, port_ref, securityEventReportedMappingProcess):
        security_event_mapping = self.model.find_elements_of_type("SECURITY-EVENT-MAPPING")
        for security_event_item in security_event_mapping:
            item_port_ref = get_element_or_none(security_event_item, "REPORTING-PORT-PROTOTYPE-IREF/TARGET-R-PORT-PROTOTYPE-REF")
            if item_port_ref is not None and item_port_ref.text == port_ref.text:
                isHave = False
                for item in securityEventReportedMappingProcess:
                    if item["fqn"] == security_event_item.get_fqn():
                        isHave = True
                        break
                if not isHave:
                    port_prototype = get_element_or_none(security_event_item, "REPORTING-PORT-PROTOTYPE-IREF")
                    eventPortInstance = commen_util.get_instance_specifier_port(self, security_event_item, port_prototype)
                    process_ref = get_element_or_none(security_event_item, "PROCESS-REF")
                    fg = ""
                    if process_ref is not None:
                        process = self.model.find_referable(process_ref.text)
                        function_group_state_ele = get_element_or_none(process, "STATE-DEPENDENT-STARTUP-CONFIGS/STATE-DEPENDENT-STARTUP-CONFIG/FUNCTION-GROUP-STATE-IREFS/FUNCTION-GROUP-STATE-IREF")
                        if function_group_state_ele is not None:
                            prototype_ref = get_element_or_none(function_group_state_ele, "CONTEXT-MODE-DECLARATION-GROUP-PROTOTYPE-REF")
                            if prototype_ref is not None:
                                fg = short_name(prototype_ref)
                    securityEventReportedMappingProcess.append({
                        "shortName": security_event_item.SHORT_NAME,
                        "fqn": security_event_item.get_fqn(),
                        "id": transition_number(get_element_or_0(security_event_item, "ID")),
                        "process": get_element_or_str(security_event_item, "PROCESS-REF"),
                        "fg": fg,
                        "eventPortInstance": eventPortInstance
                    })

                    return eventPortInstance
        return ""

    def set_other_event_definitions(self, securityEventDefinitions):
        mapping = self.model.find_elements_of_type("SECURITY-EVENT-DEFINITION")
        for item in mapping:
            if item.SHORT_NAME in ["IDSM_INTERNAL_EVENT_COMMUNICATION_ERROR", "IDSM_INTERNAL_EVENT_TRAFFIC_LIMITATION_EXCEEDED", "IDSM_INTERNAL_EVENT_NO_EVENT_BUFFER_AVAILABLE", "IDSM_INTERNAL_EVENT_NO_CONTEXT_DATA_BUFFER_AVAILABLE"]:
                self.set_security_event_definitions(item.get_fqn(), securityEventDefinitions)

    def set_security_event_definitions(self, securityEventRef, securityEventDefinitions):
        if securityEventRef != "":
            for definitionItem in securityEventDefinitions:
                if definitionItem["fqn"] == securityEventRef:
                    return
            definitionElement = self.model.find_referable(securityEventRef)
            securityEventDefinitions.append({
                "shortName": get_element_or_str(definitionElement, "SHORT-NAME"),
                "fqn": securityEventRef,
                "eventSymbolName": get_element_or_str(definitionElement, "EVENT-SYMBOL-NAME/SHORT-NAME"),
                "id": transition_number(get_element_or_0(definitionElement, "ID"))
            })

    def set_event_filter(self, ref, elementList):
        for item in elementList:
            if item["fqn"] == ref.text:
                return
        element = self.model.find_referable(ref.text)
        ob = {
            "fqn": ref
        }
        oneEveryN = get_element_or_none(element, "ONE-EVERY-N/N")
        if oneEveryN is not None:
            ob["oneEveryN"] = oneEveryN
        aggregation = get_element_or_none(element, "AGGREGATION")
        if aggregation is not None:
            ob["aggregation"] = {
                "contextDataSource": get_element_or_str(aggregation, "CONTEXT-DATA-SOURCE"),
                "minimumIntervalLength": get_element_or_0(aggregation, "MINIMUM-INTERVAL-LENGTH")
            }
        stateElement = get_element_or_none(element, "STATE")
        if stateElement is not None:
            stateOb = {}
            blockIfStateActiveAp = []
            ap_mapping = get_element_or_none(stateElement, "BLOCK-IF-STATE-ACTIVE-AP-IREFS/BLOCK-IF-STATE-ACTIVE-AP-IREF")
            if ap_mapping is not None:
                ap_mapping = to_list(ap_mapping)
                for ap_item in ap_mapping:
                    modeDeclaration = ""
                    modeDeclarationRef = get_element_or_none(ap_item, "TARGET-MODE-DECLARATION-REF")
                    if modeDeclarationRef is not None:
                        modeDeclaration = short_name(modeDeclarationRef.text)
                    blockIfStateActiveAp.append({
                        "modeDeclarationGroupPrototype": get_element_or_str(ap_item, "CONTEXT-MODE-DECLARATION-GROUP-PROTOTYPE-REF"),
                        "modeDeclaration":  modeDeclaration
                    })
            stateOb["blockIfStateActiveAp"] = blockIfStateActiveAp
            ob["state"] = stateOb
        threshold = get_element_or_none(element, "THRESHOLD")
        if threshold is not None:
            ob["threshold"] = {
                "intervalLength": get_element_or_0(threshold, "INTERVAL-LENGTH"),
                "thresholdNumber": transition_number(get_element_or_0(threshold, "THRESHOLD-NUMBER"))
            }
        elementList.append(ob)

    def find_idsm_instance(self, fqn):
        idsm_instance_mapping = self.model.find_elements_of_type("IDSM-INSTANCE")
        for idsm_instance in idsm_instance_mapping:
            idsm_module_instantiation_ref = get_element_or_none(idsm_instance, "IDSM-MODULE-INSTANTIATION-REF")
            if idsm_module_instantiation_ref is not None and idsm_module_instantiation_ref.text == fqn:
                return idsm_instance

    def get_diagnostic_interface_class(self, item):
        dependency_ref = get_element_or_none(item, "SWC-SERVICE-DEPENDENCY-IN-EXECUTABLE-IREF/TARGET-SWC-SERVICE-DEPENDENCY-REF")
        if dependency_ref is not None:
            dependency_item = self.model.find_referable(dependency_ref.text)
            if dependency_item is not None:
                port_prototype_ref = get_element_or_none(dependency_item, "ASSIGNED-PORTS/ROLE-BASED-PORT-ASSIGNMENT/PORT-PROTOTYPE-REF")
                if port_prototype_ref is not None:
                    port_prototype_element = self.model.find_referable(port_prototype_ref.text)
                    if port_prototype_element is not None:
                        provided_interface = get_element_or_none(port_prototype_element, "PROVIDED-INTERFACE-TREF")
                        if provided_interface is None:
                            provided_interface = get_element_or_none(port_prototype_element, "REQUIRED-INTERFACE-TREF")
                        if provided_interface is not None:
                            return provided_interface.attrib["DEST"]

