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
import random

from generator.common.tree_helper import get_element_or_none, get_element_or_die, get_element_or_str, transition_number, to_list, short_name
from generator.generator.template_renderer import TemplateRenderer
from generator.parser.commen_util import get_hash


class NmXmlBuilder:
    def __init__(self, model, args):
        self._log = logging.getLogger(__name__)
        self.model = model
        self._render = TemplateRenderer(None, args)
        self.model = model
        self.generateList = []
        self.portList = []
        self.portInstanceIdOb = {}

    def nm_xml(self, ar_machine_list):
        for ar_machine_fqn in ar_machine_list:
            ar_machine = self.model.find_referable(ar_machine_fqn)
            nm_instantiation = ar_machine.find_elements_of_type("NM-INSTANTIATION")
            handle_list = ar_machine.find_elements_of_type("NM-NETWORK-HANDLE")
            handleFqnList = []
            for handle in handle_list:
                fqn = handle.get_fqn()
                if fqn not in handleFqnList:
                    handleFqnList.append(fqn)
            self.machine_p_port(ar_machine, handleFqnList, nm_instantiation)
        return self.generateList

    def merge_json(self, portOb, isP):
        if isP:
            template = self._render.load_template("nm/nm_p_arxml.j2", trim_blocks=True)
        else:
            template = self._render.load_template("nm/nm_r_arxml.j2", trim_blocks=True)
        self.generateList.append(template.render(port=portOb))

    def machine_p_port(self, ar_machine, handle_list, nm_instantiation):
        instanceIdList = []
        instanceMapping = self.model.find_elements_of_type("PROVIDED-SOMEIP-SERVICE-INSTANCE")
        for instanceItem in instanceMapping:
            instanceId = get_element_or_none(instanceItem, "SERVICE-INSTANCE-ID")
            if instanceId is not None:
                instanceIdList.append(transition_number(instanceId))
        nmNetworkHandleOb = {}
        for nm in nm_instantiation:
            sdgList = get_element_or_none(nm, "ADMIN-DATA/SDGS/SDG")
            if sdgList is not None:
                for sdg in sdgList:
                    if sdg.attrib["GID"] == "iSOFT:nm:NetworkHandleToUserProcessMapping":
                        sdgItemList = get_element_or_none(sdg, "SDG/SDG")
                        networkHandleRef = None
                        networkHandleProcess = []
                        for sdgItem in sdgItemList:
                            if sdgItem.attrib["GID"] == "iSOFT:nm:NetworkHandleToUserProcessSubMapping:NetworkHandle":
                                sdfRef = get_element_or_none(sdgItem, "SDX-REF")
                                if sdfRef is not None and str(sdfRef.text).startswith(ar_machine.get_fqn()):
                                    networkHandleRef = sdfRef.text
                            elif sdgItem.attrib["GID"] == "iSOFT:nm:NetworkHandleToUserProcessSubMapping:UserProcess":
                                sdfRefList = get_element_or_none(sdgItem, "SDX-REF")
                                if sdfRefList is not None:
                                    for sdfRef in sdfRefList:
                                        if sdfRef.text not in networkHandleProcess:
                                            networkHandleProcess.append(sdfRef.text)
                        if networkHandleRef is not None:
                            if networkHandleRef not in nmNetworkHandleOb:
                                nmNetworkHandleOb[networkHandleRef] = networkHandleProcess
                            else:
                                nmNetworkHandleOb[networkHandleRef].extend(networkHandleProcess)
        for handle in handle_list:
            if handle not in nmNetworkHandleOb:
                nmNetworkHandleOb[handle] = []
        nmdProcess = None
        smdProcess = None
        mapping = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING")
        for item in mapping:
            itemMachineRef = get_element_or_none(item, "MACHINE-REF")
            if itemMachineRef is not None and itemMachineRef.text == ar_machine.get_fqn():
                process_ref = get_element_or_die(item, "PROCESS-REF")
                process = self.model.find_referable(process_ref.text)
                exe_ref = get_element_or_none(process, "EXECUTABLE-REF")
                if exe_ref is not None:
                    if short_name(exe_ref.text) == "nmd":
                        nmdProcess = process_ref.text
                    elif short_name(exe_ref.text) == "smd":
                        smdProcess = process_ref.text
        if nmdProcess is not None and smdProcess is not None:
            processToLn = {}
            for networkHandleRef in nmNetworkHandleOb:
                if nmdProcess in processToLn:
                    if networkHandleRef not in processToLn[nmdProcess]:
                        processToLn[nmdProcess].append(networkHandleRef)
                else:
                    processToLn[nmdProcess] = [networkHandleRef]
                if smdProcess in processToLn:
                    if networkHandleRef not in processToLn[smdProcess]:
                        processToLn[smdProcess].append(networkHandleRef)
                else:
                    processToLn[smdProcess] = [networkHandleRef]
                processList = nmNetworkHandleOb[networkHandleRef]
                for processItem in processList:
                    if processItem in processToLn:
                        if networkHandleRef not in processToLn[processItem]:
                            processToLn[processItem].append(networkHandleRef)
                    else:
                        processToLn[processItem] = [networkHandleRef]
            if nmdProcess in processToLn:
                self.need_port(ar_machine, instanceIdList, nmdProcess, processToLn[nmdProcess])
                del processToLn[nmdProcess]
            for processItem in processToLn:
                self.need_port(ar_machine, instanceIdList, processItem, processToLn[processItem])

    def need_port(self, ar_machine, instanceIdList, processItem, processToLnList):
        process = self.model.find_referable(processItem)
        process_design_ref = get_element_or_str(process, "PROCESS-DESIGN-REF")
        exe_ref = get_element_or_none(process, "EXECUTABLE-REF")
        if exe_ref is not None:
            exe_element = self.model.find_referable(exe_ref.text)
            root_swc_ele = get_element_or_none(exe_element, "ROOT-SW-COMPONENT-PROTOTYPE")
            if root_swc_ele is not None:
                swc_ref = get_element_or_none(root_swc_ele, "APPLICATION-TYPE-TREF")
                if swc_ref is not None:
                    swc_ele = self.model.find_referable(swc_ref.text)
                    packageList = str(swc_ele.get_fqn()).split("/")
                    if exe_element.SHORT_NAME == "nmd":
                        p_port_list = swc_ele.find_elements_of_type("P-PORT-PROTOTYPE")
                        self.set_port_list(ar_machine, instanceIdList, swc_ele, packageList, processItem, process_design_ref, root_swc_ele, processToLnList, p_port_list, True)
                    else:
                        r_port_list = swc_ele.find_elements_of_type("R-PORT-PROTOTYPE")
                        self.set_port_list(ar_machine, instanceIdList, swc_ele, packageList, processItem, process_design_ref, root_swc_ele, processToLnList, r_port_list, False)

    def set_port_list(self, ar_machine, instanceIdList, swc_ele, packageList, process_ref, process_design_ref, root_swc_ele, handle_list, port_list, isP):
        configServiceOb = {
            "machine1": "/ara/nm/timing/nm_SomeipServerServiceInstanceConfig1",
            "machine2": "/ara/nm/timing/nm_SomeipServerServiceInstanceConfig2"
        }
        configClientOb = {
            "machine1": "/ara/sm/timing1/SomeipSdClientServiceInstanceConfig",
            "machine2": "/ara/sm/timing2/SomeipSdClientServiceInstanceConfig"
        }
        machineName = str(ar_machine.SHORT_NAME).lower()
        if machineName not in ["machine1", "machine2"]:
            machineName = "machine1"

        lastPortList = []
        portNameList = []
        portFqnList = []
        for port in port_list:
            short_name1 = get_element_or_die(port, "SHORT-NAME")
            if str(short_name1.text).startswith("NetworkState_"):
                portNameList.append(short_name1.text)
                portFqnList.append(port.get_fqn())
        for handleFqn in handle_list:
            handleItem = self.model.find_referable(handleFqn)
            handleName = "NetworkState_{}".format(handleItem.SHORT_NAME)
            portFqn = "{}/{}".format(swc_ele.get_fqn(), handleName)
            if handleName not in portNameList:
                connectorRef = self.get_connector_ref(handleItem, ar_machine)
                if connectorRef is not None:
                    portItem = {
                        "portName": handleName,
                        "processFqn": process_ref,
                        "processDesignFqn": process_design_ref,
                        "rootSwcFqn": root_swc_ele.get_fqn(),
                        "portFqn": portFqn,
                        "connectorFqn": str(connectorRef),
                        "sdConfigServiceFqn": configServiceOb[machineName],
                        "sdConfigClientFqn": configClientOb[machineName],
                        "instanceId": self.get_instance_id(instanceIdList, handleItem.SHORT_NAME),
                        "port": self.get_tcp_udp_port(self.portList, isP)
                    }
                    lastPortList.append(portItem)
            else:
                if handleName not in self.portInstanceIdOb:
                    for portFqn in portFqnList:
                        if str(portFqn).endswith(handleName):
                            portMapping = self.model.find_elements_of_type("SERVICE-INSTANCE-TO-PORT-PROTOTYPE-MAPPING")
                            for mappingItem in portMapping:
                                portRef = get_element_or_none(mappingItem, "PORT-PROTOTYPE-IREF/TARGET-PORT-PROTOTYPE-REF")
                                if portRef is not None and portRef.text == portFqn:
                                    instanceRef = get_element_or_none(mappingItem, "SERVICE-INSTANCE-REF")
                                    if instanceRef is not None:
                                        instanceEle = self.model.find_referable(instanceRef.text)
                                        instanceId = get_element_or_none(instanceEle, "SERVICE-INSTANCE-ID")
                                        if instanceId is not None:
                                            self.portInstanceIdOb[handleItem.SHORT_NAME] = instanceId.text
                                            break
        if len(lastPortList) > 0:
            portOb = {
                "nameList": packageList[1:-1],
                "lastName": packageList[-1],
                "allName":"/".join(packageList[:-1]),
                "machineName": ar_machine.SHORT_NAME,
                "portList": lastPortList
            }
            self.merge_json(portOb, isP)

    def get_tcp_udp_port(self, portList, isP):
        portNum = 51000
        if isP:
            portNum = 50000
        while portNum in portList:
            portNum += 1
        portList.append(portNum)
        return portNum

    def get_instance_id(self, instanceIdList, name):
        if name in self.portInstanceIdOb:
            return self.portInstanceIdOb[name]
        else:
            instanceId = get_hash(name)
            while instanceId in instanceIdList:
                instanceId += 1
            instanceIdList.append(instanceId)
            self.portInstanceIdOb[name] = instanceId
            return instanceId

    def get_connector_ref(self, handleItem, ar_machine):
        connectorRef = get_element_or_none(handleItem, "VLAN-REFS/VLAN-REF")
        if connectorRef is not None:
            return connectorRef
        else:
            machine_design_ref = get_element_or_die(ar_machine, "MACHINE-DESIGN-REF")
            design = self.model.find_referable(machine_design_ref.text)
            connector_mapping = design.find_elements_of_type("ETHERNET-COMMUNICATION-CONNECTOR")
            for connector in connector_mapping:
                return connector.get_fqn()
            return None
