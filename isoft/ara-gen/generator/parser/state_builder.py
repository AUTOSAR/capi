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

from generator.generator.template_renderer import TemplateRenderer
from generator.common.tree_helper import get_element_or_none, short_name, get_element_or_die


class StateBuilder:
    def __init__(self, model, args):
        self._log = logging.getLogger(__name__)
        self._render = TemplateRenderer(None, args)
        self.model = model
        self._args = args
        self.generateList = []

    def get_module_instantiation(self, process):
        machine_mapping = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING")
        for machine_mapping_item in machine_mapping:
            process_ref = get_element_or_none(machine_mapping_item, "PROCESS-REF")
            if process_ref is not None and process_ref.text == process:
                machine_ref = get_element_or_none(machine_mapping_item, "MACHINE-REF")
                moduleInstantiation = get_element_or_none(machine_mapping_item, "NON-OS-MODULE-INSTANTIATION-REF")
                if moduleInstantiation is not None:
                    return [moduleInstantiation.text, machine_ref]
        return None

    def get_state_machine(self, ar_process):
        moduleInstantiationRef = self.get_module_instantiation(ar_process.get_fqn())
        if moduleInstantiationRef is not None:
            moduleInstantiation = self.model.find_referable(moduleInstantiationRef[0])
            sdgList = get_element_or_none(moduleInstantiation, "ADMIN-DATA/SDGS/SDG")
            if sdgList is not None:
                groupList = []
                notificationList = []
                actionItemList = []
                requestList = []
                for sdg in sdgList:
                    if "iSOFT:sm:GenericModuleInstantiation" == sdg.attrib["GID"]:
                        sdgLi = get_element_or_none(sdg, "SDG")
                        if sdgLi is not None:
                            for item in sdgLi:
                                if "iSOFT:sm:GenericModuleInstantiation:Notification" == item.attrib["GID"]:
                                    notificationOb = {}
                                    caption = get_element_or_none(item, "SDG-CAPTION")
                                    if caption is not None:
                                        notificationOb["fqn"] = caption.get_fqn()
                                    sdList = item.find_elements_of_type("SD")
                                    for sd in sdList:
                                        if "iSOFT:sm:StateManagementStateNotification:TriggerOutIdentifier" == sd.attrib["GID"]:
                                            notificationOb["notificationPort"] = "NSOMEIP:{}".format(sd.text)
                                        elif "iSOFT:sm:StateManagementStateNotification:StateMachineIdentifier" == sd.attrib["GID"]:
                                            notificationOb["stateMachineService_instance_id"] = "NSOMEIP:{}".format(sd.text)
                                    sdgList = item.find_elements_of_type("SDG")
                                    for sdgItem in sdgList:
                                        if "iSOFT:sm:StateManagementStateNotification:StateMachine" == sdgItem.attrib["GID"]:
                                            stateMachineOb = {}
                                            stateCaption = get_element_or_none(sdgItem, "SDG-CAPTION")
                                            if stateCaption is not None:
                                                stateMachineOb["fqn"] = stateCaption.get_fqn()
                                                if stateCaption.get_fqn() not in groupList:
                                                    groupList.append(stateCaption.get_fqn())
                                            sdgM = get_element_or_none(sdgItem, "SDG")
                                            if sdgM is not None and sdgM.attrib["GID"] == "iSOFT:sm:StateMachine":
                                                groupRef = get_element_or_none(sdgM, "SDX-REF")
                                                if groupRef is not None:
                                                    groupEle = self.model.find_referable(groupRef.text)
                                                    modeList = []
                                                    modeDecList = groupEle.find_elements_of_type("MODE-DECLARATION")
                                                    for mode in modeDecList:
                                                        modeList.append(mode.SHORT_NAME)
                                                    stateMachineOb["states"] = modeList
                                            notificationOb["stateMachine"] = stateMachineOb
                                            break
                                    notificationList.append(notificationOb)
                                elif "iSOFT:sm:GenericModuleInstantiation:ActionItemList" == item.attrib["GID"]:
                                    actionOb = {}
                                    caption = get_element_or_none(item, "SDG-CAPTION")
                                    if caption is not None:
                                        actionOb["fqn"] = caption.get_fqn()
                                    affectedStateOb = {}
                                    actionItems = []

                                    stateRef = get_element_or_none(item, "SDX-REF")
                                    if stateRef is not None:
                                        affectedStateOb["stateMachineFQN"] = stateRef.text
                                    sdgList = item.find_elements_of_type("SDG")
                                    for sdgItem in sdgList:
                                        if "iSOFT:sm:StateManagementActionList:ActionItem" == sdgItem.attrib["GID"]:
                                            actionItemOb = {}
                                            itemTypeSd = get_element_or_none(sdgItem, "SD")
                                            isMachineFg = False
                                            if itemTypeSd is not None and "iSOFT:sm:StateManagementActionItem:ItemType" == itemTypeSd.attrib["GID"]:
                                                actionItemOb["type"] = itemTypeSd.text
                                            setFunctionGroupStateOb = {}
                                            sdgs = sdgItem.find_elements_of_type("SDG")
                                            for actionSdg in sdgs:
                                                if "iSOFT:sm:StateManagementActionItem:SetFunctionGroupStateContextModeDeclarationGroupPrototype" == actionSdg.attrib["GID"]:
                                                    actionRef = get_element_or_none(actionSdg, "SDX-REF")
                                                    if actionRef is not None:
                                                        isMachineFg = str(actionRef).endswith("/MachineFG")
                                                        setFunctionGroupStateOb["FunctionGroupFQN"] = actionRef.text
                                                elif "iSOFT:sm:StateManagementActionItem:SetFunctionGroupStateTargetModeDeclaration" == actionSdg.attrib["GID"]:
                                                    stateRef = get_element_or_none(actionSdg, "SDX-REF")
                                                    if stateRef is not None:
                                                        setFunctionGroupStateOb["state"] = short_name(stateRef.text)
                                                        setFunctionGroupStateOb["_statePath"] = "/".join(str(stateRef).split("/")[:-1])
                                                        if isMachineFg and short_name(stateRef.text).lower() == "off":
                                                            assert False, "[SM  ] CODE-002: The Off status of the function group MachineFG cannot be used as an action list item.Fqn:{}. LocalPath:{}".format(actionOb["fqn"] if "fqn" in actionOb else "" , moduleInstantiation.get_path())
                                            if len(setFunctionGroupStateOb) > 0:
                                                if "_statePath" in setFunctionGroupStateOb and "FunctionGroupFQN" in setFunctionGroupStateOb:
                                                    functionGroupFqn = setFunctionGroupStateOb["FunctionGroupFQN"]
                                                    functionGroup = self.model.find_referable(functionGroupFqn)
                                                    mode_declaration_group_ref = get_element_or_none(functionGroup, "TYPE-TREF")
                                                    if mode_declaration_group_ref is not None and mode_declaration_group_ref.text == setFunctionGroupStateOb["_statePath"]:
                                                        actionItemOb["setFunctionGroupState"] = setFunctionGroupStateOb
                                                    else:
                                                        assert False, "[SM  ] CODE-004: {} and {} are not related. LocalPath:{}. ".format(functionGroupFqn, setFunctionGroupStateOb["_statePath"], moduleInstantiation.get_path())
                                                actionItemOb["setFunctionGroupState"] = setFunctionGroupStateOb
                                            actionItems.append(actionItemOb)
                                        elif "iSOFT:sm:StateManagementActionList:AffectedState" == sdgItem.attrib["GID"]:
                                            stateRef = get_element_or_none(sdgItem, "SDX-REF")
                                            if stateRef is not None:
                                                self.judge_state(groupList, stateRef.text, affectedStateOb, "stateMachineFQN", moduleInstantiation)
                                                affectedStateOb["state"] = short_name(stateRef.text)

                                    actionOb["actionItems"] = actionItems
                                    actionOb["affectedState"] = affectedStateOb
                                    actionItemList.append(actionOb)
                                elif "iSOFT:sm:GenericModuleInstantiation:Request" == item.attrib["GID"]:
                                    requestOb = {}
                                    caption = get_element_or_none(item, "SDG-CAPTION")
                                    if caption is not None:
                                        requestOb["fqn"] = caption.get_fqn()
                                    requestType = get_element_or_none(item, "SD")
                                    if requestType is not None and "iSOFT:sm:StateManagementStateRequest:RequestType" == requestType.attrib["GID"]:
                                        requestOb["type"] = requestType.text
                                    sdgRuleList = item.find_elements_of_type("SDG")
                                    ruleList = []
                                    for sdgItem in sdgRuleList:
                                        if "iSOFT:sm:StateManagementStateRequest:Rule" == sdgItem.attrib["GID"]:
                                            ruleOb = {}
                                            stateRef = get_element_or_none(sdgItem, "SDX-REF")
                                            if stateRef is not None:
                                                ruleOb["contextStateMachine"] = stateRef.text
                                            ruleSdList = sdgItem.find_elements_of_type("SD")
                                            for ruleSd in ruleSdList:
                                                if "iSOFT:sm:StateManagementRequestRule:CompareValue" == ruleSd.attrib["GID"]:
                                                    ruleOb["compareValue"] = str(ruleSd.text)
                                            ruleSdgList = sdgItem.find_elements_of_type("SDG")
                                            for ruleSdg in ruleSdgList:
                                                if "iSOFT:sm:StateManagementRequestRule:AssumedCurrentState" == ruleSdg.attrib["GID"]:
                                                    stateRef = get_element_or_none(ruleSdg, "SDX-REF")
                                                    if stateRef is not None:
                                                        ruleOb["assumedCurrentState"] = short_name(stateRef.text)
                                                elif "iSOFT:sm:StateManagementRequestRule:NextState" == ruleSdg.attrib["GID"]:
                                                    stateRef = get_element_or_none(ruleSdg, "SDX-REF")
                                                    if stateRef is not None:
                                                        self.judge_state(groupList, stateRef.text, ruleOb, "contextStateMachine", moduleInstantiation)
                                                        ruleOb["nextState"] = short_name(stateRef.text)
                                            ruleList.append(ruleOb)
                                    requestOb["rules"] = ruleList
                                    requestList.append(requestOb)
                isShutDown = False
                isRestart = False
                for item in actionItemList:
                    affectedState = item["affectedState"]
                    if affectedState["state"].lower() == "restart":
                        isRestart = True
                    elif affectedState["state"].lower() == "shutdown":
                        isShutDown = True
                assert isShutDown and isRestart, "[SM  ] CODE-003: {} must be configured with Restart and ShutDown. LocalPath:{}. ".format(moduleInstantiationRef, moduleInstantiation.get_path())
                if len(notificationList + actionItemList + requestList) > 0:
                    return {
                        "notifications": notificationList,
                        "actionItemLists": actionItemList,
                        "requests": requestList
                    }
        return None

    def judge_state(self, groupList, stateRef, ob, stateMachineName, moduleInstantiation):
        isTrue = False
        if stateMachineName in ob:
            linkGroupStr = ob[stateMachineName]
            assert linkGroupStr in groupList, "[SM  ] CODE-006: {} is not in [{}]. Fqn:{}. LocalPath:{}. ".format(linkGroupStr, ",".join(groupList), moduleInstantiation.get_fqn(), moduleInstantiation.get_path())
            contextStateMachine = self.model.find_referable(ob[stateMachineName])
            parent = contextStateMachine.getparent()
            if parent is not None:
                sdgM = get_element_or_none(parent, "SDG")
                if sdgM is not None and sdgM.attrib["GID"] == "iSOFT:sm:StateMachine":
                    groupRef = get_element_or_none(sdgM, "SDX-REF")
                    if groupRef is not None:
                        if str(stateRef).startswith(groupRef.text + "/"):
                            isTrue = True
        assert isTrue, "[SM  ] CODE-007: {} is not in {}. Fqn:{}. LocalPath:{}. ".format(stateRef, ob[stateMachineName] if stateMachineName in ob else "", moduleInstantiation.get_fqn(), moduleInstantiation.get_path())

    def get_state_json(self, ar_process):
        moduleInstantiationRef = self.get_module_instantiation(ar_process.get_fqn())
        if moduleInstantiationRef is not None:
            moduleInstantiation = self.model.find_referable(moduleInstantiationRef[0])
            in_function = []
            in_out_function = []
            out_function = []
            sdgAllList = get_element_or_none(moduleInstantiation, "ADMIN-DATA/SDGS/SDG")
            if sdgAllList is not None:
                for sdgAll in sdgAllList:
                    if "iSOFT:sm:GenericModuleInstantiation" == sdgAll.attrib["GID"]:
                        sdgLi = get_element_or_none(sdgAll, "SDG")
                        if sdgLi is not None:
                            for item in sdgLi:
                                if "iSOFT:sm:GenericModuleInstantiation:FGServerInstanceConfig" == item.attrib["GID"]:
                                    function_group_ref = None
                                    sdgs = item.find_elements_of_type("SDG")
                                    for sdg in sdgs:
                                        if "iSOFT:sm:FGServerInstanceConfigDefinition:FunctionGroup" == sdg.attrib["GID"]:
                                            sdx_ref = get_element_or_none(sdg, "SDX-REF")
                                            if sdx_ref is not None:
                                                function_group_ref = sdx_ref.text
                                                break
                                    if function_group_ref is not None:
                                        sds = item.find_elements_of_type("SD")
                                        for sd in sds:
                                            if "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInIdentifier" == sd.attrib["GID"]:
                                                function_item = {
                                                    "function_group_fqn": function_group_ref,
                                                    "instance_id": "NSOMEIP:{}".format(sd.text)
                                                }
                                                if function_item not in in_function:
                                                    in_function.append(function_item)
                                            elif "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInOutIdentifier" == sd.attrib["GID"]:
                                                function_item = {
                                                    "function_group_fqn": function_group_ref,
                                                    "instance_id": "NSOMEIP:{}".format(sd.text)
                                                }
                                                if function_item not in in_out_function:
                                                    in_out_function.append(function_item)
                                            elif "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerOutIdentifier" == sd.attrib["GID"]:
                                                function_item = {
                                                    "function_group_fqn": function_group_ref,
                                                    "instance_id": "NSOMEIP:{}".format(sd.text)
                                                }
                                                if function_item not in out_function:
                                                    out_function.append(function_item)
            if len(in_function+in_out_function+out_function) == 0:
                return None
            else:
                return {
                    "provided_trigger_in_function_group_service_instances": in_function,
                    "provided_trigger_in_out_function_group_service_instances": in_out_function,
                    "provided_trigger_out_function_group_service_instances": out_function
                }
        return None

    def get_state_xml(self, processes_list):
        generatorPortList = []
        instanceList = []
        for process in processes_list:
            ar_process = self.model.find_referable(process)
            process_design = get_element_or_none(ar_process, "DESIGN-REF")
            moduleInstantiationRef = self.get_module_instantiation(ar_process.get_fqn())
            exe_ref = get_element_or_die(ar_process, "EXECUTABLE-REF")
            exe = self.model.find_referable(exe_ref.text)
            swc_ref = get_element_or_none(exe, "ROOT-SW-COMPONENT-PROTOTYPE/APPLICATION-TYPE-TREF")
            if swc_ref is not None:
                if moduleInstantiationRef is not None:
                    moduleInstantiation = self.model.find_referable(moduleInstantiationRef[0])
                    machineName = "Machine"
                    if moduleInstantiationRef[1] is not None:
                        machineName = short_name(moduleInstantiationRef[1].text)
                    sdgAllList = get_element_or_none(moduleInstantiation, "ADMIN-DATA/SDGS/SDG")
                    if sdgAllList is not None:
                        for sdgAll in sdgAllList:
                            if "iSOFT:sm:GenericModuleInstantiation" == sdgAll.attrib["GID"]:
                                sdgLi = get_element_or_none(sdgAll, "SDG")
                                if sdgLi is not None:
                                    for item in sdgLi:
                                        if "iSOFT:sm:GenericModuleInstantiation:Notification" == item.attrib["GID"]:
                                            stateMachineOb = {}
                                            outOb = {}
                                            captionName = ""
                                            caption = get_element_or_none(item, "SDG-CAPTION")
                                            if caption is not None:
                                                captionName = caption.SHORT_NAME
                                            sdList = item.find_elements_of_type("SD")
                                            for sd in sdList:
                                                if "iSOFT:sm:StateManagementStateNotification:TriggerOutIdentifier" == sd.attrib["GID"]:
                                                    outOb["id"] = sd.text
                                                elif "iSOFT:sm:StateManagementStateNotification:TriggerOutPort" == sd.attrib["GID"]:
                                                    outOb["port"] = sd.text
                                                elif "iSOFT:sm:StateManagementStateNotification:StateMachineIdentifier" == sd.attrib["GID"]:
                                                    stateMachineOb["id"] = sd.text
                                                elif "iSOFT:sm:StateManagementStateNotification:StateMachinePort" == sd.attrib["GID"]:
                                                    stateMachineOb["port"] = sd.text
                                            sdgList = item.find_elements_of_type("SDG")
                                            for sdg in sdgList:
                                                if "iSOFT:sm:StateManagementStateNotification:TriggerOutSomeipSdServerServiceInstanceConfig" == sdg.attrib["GID"]:
                                                    stateRef = get_element_or_none(sdg, "SDX-REF")
                                                    if stateRef is not None:
                                                        outOb["sd_config"] = stateRef.text
                                                elif "iSOFT:sm:StateManagementStateNotification:TriggerOutConnector" == sdg.attrib["GID"]:
                                                    stateRef = get_element_or_none(sdg, "SDX-REF")
                                                    if stateRef is not None:
                                                        outOb["connector"] = stateRef.text
                                                elif "iSOFT:sm:StateManagementStateNotification:StateMachineSomeipSdServerServiceInstanceConfig" == sdg.attrib["GID"]:
                                                    stateRef = get_element_or_none(sdg, "SDX-REF")
                                                    if stateRef is not None:
                                                        stateMachineOb["sd_config"] = stateRef.text
                                                elif "iSOFT:sm:StateManagementStateNotification:StateMachineConnector" == sdg.attrib["GID"]:
                                                    stateRef = get_element_or_none(sdg, "SDX-REF")
                                                    if stateRef is not None:
                                                        stateMachineOb["connector"] = stateRef.text

                                            if len(stateMachineOb) == 4:
                                                stateMachineOb["process"] = process
                                                stateMachineOb["process_name"] = short_name(process)
                                                if process_design is not None:
                                                    stateMachineOb["process_design"] = process_design.text
                                                stateMachineOb["function_deployment_ref"] = "/SomeipDeployments/StateMachineService"
                                                stateMachineOb["swc"] = swc_ref.text
                                                stateMachineOb["port_name"] = "{}_{}_StateMachineService".format(machineName, captionName)
                                                if caption is not None:
                                                    path = str(caption.get_fqn()).split("/")
                                                    functionPath = path[1:]
                                                    functionPath.append(machineName)
                                                    serviceInstancePath = {
                                                        "machineName": machineName,
                                                        "functionAllPath": "/".join(functionPath),
                                                        "functionPath": functionPath
                                                    }
                                                    portList = [{
                                                        "name": stateMachineOb["port_name"],
                                                        "interface": "/ara/sm/AAInterfaces/StateMachineService"
                                                    }]
                                                    self.generator_port_list(serviceInstancePath, [stateMachineOb], instanceList, swc_ref, portList, generatorPortList)
                                            if len(outOb) == 4:
                                                outOb["process"] = process
                                                outOb["process_name"] = short_name(process)
                                                if process_design is not None:
                                                    outOb["process_design"] = process_design.text
                                                outOb["function_deployment_ref"] = "/SomeipDeployments/TriggerOut_StateMachine"
                                                outOb["event_group"] = {
                                                    "name": "TriggerOut_provided_event_group",
                                                    "event_ref": "/SomeipDeployments/TriggerOut_StateMachine/Notifier_eventgroup"
                                                }
                                                outOb["swc"] = swc_ref.text
                                                outOb["port_name"] = "{}_{}_TriggerOut_StateMachine".format(machineName, captionName)
                                                if caption is not None:
                                                    path = str(caption.get_fqn()).split("/")
                                                    functionPath = path[1:]
                                                    functionPath.append(machineName)
                                                    serviceInstancePath = {
                                                        "machineName": machineName,
                                                        "functionAllPath": "/".join(functionPath),
                                                        "functionPath": functionPath
                                                    }
                                                    portList = [{
                                                        "name": outOb["port_name"],
                                                        "interface": "/ara/sm/AAInterfaces/TriggerOut_StateMachine"
                                                    }]
                                                    self.generator_port_list(serviceInstancePath, [outOb], instanceList, swc_ref, portList, generatorPortList)
                                        elif "iSOFT:sm:GenericModuleInstantiation:FGServerInstanceConfig" == item.attrib["GID"]:
                                            sdInGid = ["iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInIdentifier",
                                                       "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInConnector",
                                                       "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInSomeipSdServerServiceInstanceConfig",
                                                       "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInPort"]
                                            sdInOutGid = ["iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInOutIdentifier",
                                                          "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInOutSomeipSdServerServiceInstanceConfig",
                                                          "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInOutConnector",
                                                          "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInOutPort"]
                                            sdOutGid = ["iSOFT:sm:FGServerInstanceConfigDefinition:TriggerOutIdentifier",
                                                        "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerOutSomeipSdServerServiceInstanceConfig",
                                                        "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerOutConnector",
                                                        "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerOutPort"]
                                            inOb = {}
                                            inOutOb = {}
                                            outOb = {}
                                            sds = item.find_elements_of_type("SD")
                                            for sd in sds:
                                                if "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInIdentifier" == sd.attrib["GID"]:
                                                    inOb["id"] = sd.text
                                                    sdInGid.remove(sd.attrib["GID"])
                                                elif "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInPort" == sd.attrib["GID"]:
                                                    inOb["port"] = sd.text
                                                    sdInGid.remove(sd.attrib["GID"])
                                                elif "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInOutIdentifier" == sd.attrib["GID"]:
                                                    inOutOb["id"] = sd.text
                                                    sdInOutGid.remove(sd.attrib["GID"])
                                                elif "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInOutPort" == sd.attrib["GID"]:
                                                    inOutOb["port"] = sd.text
                                                    sdInOutGid.remove(sd.attrib["GID"])
                                                elif "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerOutIdentifier" == sd.attrib["GID"]:
                                                    outOb["id"] = sd.text
                                                    sdOutGid.remove(sd.attrib["GID"])
                                                elif "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerOutPort" == sd.attrib["GID"]:
                                                    outOb["port"] = sd.text
                                                    sdOutGid.remove(sd.attrib["GID"])

                                            sdgs = item.find_elements_of_type("SDG")
                                            for sdg in sdgs:
                                                if "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInSomeipSdServerServiceInstanceConfig" == sdg.attrib["GID"]:
                                                    sdx_ref = get_element_or_none(sdg, "SDX-REF")
                                                    if sdx_ref is not None:
                                                        inOb["sd_config"] = sdx_ref.text
                                                        sdInGid.remove(sdg.attrib["GID"])
                                                elif "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInConnector" == sdg.attrib["GID"]:
                                                    sdx_ref = get_element_or_none(sdg, "SDX-REF")
                                                    if sdx_ref is not None:
                                                        inOb["connector"] = sdx_ref.text
                                                        sdInGid.remove(sdg.attrib["GID"])
                                                elif "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInOutSomeipSdServerServiceInstanceConfig" == sdg.attrib["GID"]:
                                                    sdx_ref = get_element_or_none(sdg, "SDX-REF")
                                                    if sdx_ref is not None:
                                                        inOutOb["sd_config"] = sdx_ref.text
                                                        sdInOutGid.remove(sdg.attrib["GID"])
                                                elif "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerInOutConnector" == sdg.attrib["GID"]:
                                                    sdx_ref = get_element_or_none(sdg, "SDX-REF")
                                                    if sdx_ref is not None:
                                                        inOutOb["connector"] = sdx_ref.text
                                                        sdInOutGid.remove(sdg.attrib["GID"])
                                                elif "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerOutSomeipSdServerServiceInstanceConfig" == sdg.attrib["GID"]:
                                                    sdx_ref = get_element_or_none(sdg, "SDX-REF")
                                                    if sdx_ref is not None:
                                                        outOb["sd_config"] = sdx_ref.text
                                                        sdOutGid.remove(sdg.attrib["GID"])
                                                elif "iSOFT:sm:FGServerInstanceConfigDefinition:TriggerOutConnector" == sdg.attrib["GID"]:
                                                    sdx_ref = get_element_or_none(sdg, "SDX-REF")
                                                    if sdx_ref is not None:
                                                        outOb["connector"] = sdx_ref.text
                                                        sdOutGid.remove(sdg.attrib["GID"])
                                            assert len(sdInGid) == 0 or len(sdInGid) == 4, "[SM  ] CODE-005:  TriggerIn_FunctionGroup Incomplete configuration.FQN:{}".format(moduleInstantiation.get_fqn())
                                            assert len(sdInOutGid) == 0 or len(sdInOutGid) == 4, "[SM  ] CODE-005:  TriggerInOut_FunctionGroup Incomplete configuration.FQN:{}".format(moduleInstantiation.get_fqn())
                                            assert len(sdOutGid) == 0 or len(sdOutGid) == 4, "[SM  ] CODE-005:  TriggerOut_FunctionGroup Incomplete configuration.FQN:{}".format(moduleInstantiation.get_fqn())
                                            portList = []
                                            path = str(moduleInstantiationRef[0]).split("/")
                                            functionPath = path[1:-1]
                                            functionPath.append(machineName)
                                            serviceInstancePath = {
                                                "machineName": machineName,
                                                "functionAllPath": "/".join(functionPath),
                                                "functionPath": functionPath
                                            }
                                            interfaceDeploymentList = []
                                            if "id" in inOb:
                                                inOb["process"] = process
                                                inOb["process_name"] = short_name(process)
                                                if process_design is not None:
                                                    inOb["process_design"] = process_design.text
                                                inOb["function_deployment_ref"] = "/SomeipDeployments/TriggerIn_FunctionGroup"
                                                inOb["swc"] = swc_ref.text
                                                inOb["port_name"] = "{}_{}_{}_TriggerIn_FunctionGroup".format(machineName, moduleInstantiation.SHORT_NAME, inOb["id"])
                                                portList.append({
                                                    "name": inOb["port_name"],
                                                    "interface": "/ara/sm/AAInterfaces/TriggerIn_FunctionGroup"
                                                })
                                                interfaceDeploymentList.append(inOb)
                                            if "id" in inOutOb:
                                                inOutOb["process"] = process
                                                inOutOb["process_name"] = short_name(process)
                                                if process_design is not None:
                                                    inOutOb["process_design"] = process_design.text
                                                inOutOb["function_deployment_ref"] = "/SomeipDeployments/TriggerInOut_FunctionGroup"
                                                inOutOb["event_group"] = {
                                                    "name": "TriggerInOut_provided_event_group",
                                                    "event_ref": "/SomeipDeployments/TriggerInOut_FunctionGroup/Notifier_eventgroup"
                                                }
                                                inOutOb["swc"] = swc_ref.text
                                                inOutOb["port_name"] = "{}_{}_{}_TriggerInOut_FunctionGroup".format(machineName, moduleInstantiation.SHORT_NAME, inOutOb["id"])
                                                portList.append({
                                                    "name": inOutOb["port_name"],
                                                    "interface": "/ara/sm/AAInterfaces/TriggerInOut_FunctionGroup"
                                                })
                                                interfaceDeploymentList.append(inOutOb)
                                            if "id" in outOb:
                                                outOb["process"] = process
                                                outOb["process_name"] = short_name(process)
                                                if process_design is not None:
                                                    outOb["process_design"] = process_design.text
                                                outOb["function_deployment_ref"] = "/SomeipDeployments/TriggerOut_FunctionGroup"
                                                outOb["event_group"] = {
                                                    "name": "TriggerOut_provided_event_group",
                                                    "event_ref": "/SomeipDeployments/TriggerOut_FunctionGroup/Notifier_eventgroup"
                                                }
                                                outOb["swc"] = swc_ref.text
                                                outOb["port_name"] = "{}_{}_{}_TriggerOut_FunctionGroup".format(machineName, moduleInstantiation.SHORT_NAME, outOb["id"])
                                                portList.append({
                                                    "name": outOb["port_name"],
                                                    "interface": "/ara/sm/AAInterfaces/TriggerOut_FunctionGroup"
                                                })
                                                interfaceDeploymentList.append(outOb)
                                            self.generator_port_list(serviceInstancePath, interfaceDeploymentList, instanceList, swc_ref, portList, generatorPortList)
        for instanceItem in instanceList:
            self._generate_instance_mapping_xml(instanceItem)
        for generatorPort in generatorPortList:
            self._generate_port_xml(generatorPort["list"], generatorPort["path"])
        return self.generateList

    def generator_port_list(self, serviceInstancePath, interfaceDeploymentList, instanceList, swc_ref, portList, generatorPortList):
        item = {
            "serviceInstancePath": serviceInstancePath,
            "interfaceDeploymentList": interfaceDeploymentList
        }
        self._generate_mapping_xml(item)
        for it in interfaceDeploymentList:
            if "process" in it:
                del it["process"]
            if "process_design" in it:
                del it["process_design"]
            if "process_name" in it:
                del it["process_name"]
            if "swc" in it:
                del it["swc"]
        instanceItem = {
            "serviceInstancePath": serviceInstancePath,
            "interfaceDeploymentList": interfaceDeploymentList
        }
        if instanceItem not in instanceList:
            instanceList.append(instanceItem)
        for generatorPortItem in generatorPortList:
            if generatorPortItem["path"] == swc_ref.text:
                generatorPortItem["list"].extend(portList)
                return
        generatorPort = {
            "path": swc_ref.text,
            "list": portList
        }
        generatorPortList.append(generatorPort)

    def _generate_mapping_xml(self, item):
        template = self._render.load_template("state/state_port_mapping.j2", trim_blocks=True)
        self.generateList.append(template.render(item=item))

    def _generate_instance_mapping_xml(self, item):
        template = self._render.load_template("state/state_instance_deployment.j2", trim_blocks=True)
        self.generateList.append(template.render(item=item))

    def _generate_port_xml(self, portList, swc):
        path = str(swc).split("/")
        if len(path) > 2:
            name = {
                "lastName": path[-1],
                "nameList": path[1:-1]
            }
            template = self._render.load_template("state/state_instance_port.j2", trim_blocks=True)
            self.generateList.append(template.render(name=name, portList=portList))
