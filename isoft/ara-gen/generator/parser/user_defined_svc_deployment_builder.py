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
"""
User defined service deployment builder class.
"""

import collections
import logging

from generator.common.tree_helper import get_element, get_element_or_none, short_name, get_element_or_die, to_list
from generator.intermediate_model.communication_management.ara_com_deployment.someip.event_deployment \
    import EventDeployment
from generator.intermediate_model.communication_management.ara_com_deployment.someip.event_group_deployment \
    import EventGroupDeployment
from generator.intermediate_model.communication_management.ara_com_deployment.someip.field_deployment \
    import FieldDeployment
from generator.intermediate_model.communication_management.ara_com_deployment.someip.method_deployment \
    import MethodDeployment
from generator.parser.commen_util import get_hash
from generator.parser.interface_builder import _GET, _SET, _NOTIFIER

class UserDefinedServiceDeploymentBuilder:
    """
    Handles parsing the ARXML part, related to user defined network binding.
    """

    def __init__(self, model):
        self.model = model
        self.depType = ""
        self._log = logging.getLogger(__name__)
        self._user_defined_service_interfaces_ids = []
        self._user_defined_service_interfaces_ids_ob = {}

    def judge_dep_type(self, ar_deployment):
        deplCategory = get_element_or_none(ar_deployment, 'CATEGORY')
        if deplCategory is not None and deplCategory.text == "SERVICE_INTERFACE_DEPLOYMENT_ICC":
            self.depType = "ICC"
        else:
            self.depType = "NPC"

    def _get_event_deployment(self, ar_event_depl, id_counter):
        """
        Extract IPC event deployment information, will be returned as
        a dict with information
        """
        result = dict(
            name=ar_event_depl.get_fqn(),
            id=id_counter,
            proto=self.depType
        )
        if self.depType == "NPC":
            sd = get_element_or_none(ar_event_depl, "ADMIN-DATA/SDGS/SDG/SD")
            if sd is not None and sd.attrib["GID"] == "iSOFT:com:IpcEventDeployment:EventId":
                result['id'] = sd
        return result

    def _get_field_deployment(self, ar_field_depl, id_counter):
        """
        Extract IPC field deployment information, will be returned as
        a dict with information
        """
        result = {
            "getter": {},
            "setter": {},
            "notifier": {}
        }
        # incrementing_id is a local counter to be incremented inside this function. getter, setter and notifier use
        # it to have different ids
        # id_counter itself will be changed in a caller function according to the number of non-empty elements in
        # the resulting dict
        incrementing_id = id_counter
        getter = get_element_or_none(ar_field_depl, _GET)
        if getter is not None:
            result['getter'] = dict(
                name=get_element(getter, "SHORT-NAME"),
                id=incrementing_id,
                proto=self.depType
            )
            incrementing_id += 1
            if self.depType == "NPC":
                sd = get_element_or_none(getter, "ADMIN-DATA/SDGS/SDG/SD")
                if sd is not None and sd.attrib["GID"] == "iSOFT:com:IpcMethodDeployment:MethodId":
                    result['getter']['id'] = sd
        setter = get_element_or_none(ar_field_depl, _SET)
        if setter is not None:
            result['setter'] = dict(
                name=get_element(setter, "SHORT-NAME"),
                id=incrementing_id,
                proto=self.depType
            )
            incrementing_id += 1
            if self.depType == "NPC":
                sd = get_element_or_none(setter, "ADMIN-DATA/SDGS/SDG/SD")
                if sd is not None and sd.attrib["GID"] == "iSOFT:com:IpcMethodDeployment:MethodId":
                    result['setter']['id'] = sd
        notifier = get_element_or_none(ar_field_depl, _NOTIFIER)
        if notifier is not None:
            result['notifier'] = dict(
                name=notifier.get_fqn(),
                id=incrementing_id,
                proto=self.depType
            )
            if self.depType == "NPC":
                sd = get_element_or_none(notifier, "ADMIN-DATA/SDGS/SDG/SD")
                if sd is not None and sd.attrib["GID"] == "iSOFT:com:IpcEventDeployment:EventId":
                    result['notifier']['id'] = sd
        return result

    def _get_method_deployment(self, ar_method_depl, id_counter):
        """
        Extract IPC method deployment information, will be returned as
        a dict with information
        """
        result = dict(
            name=get_element(ar_method_depl, "SHORT-NAME"),
            id=id_counter,
            proto=self.depType
        )
        if self.depType == "NPC":
            sd = get_element_or_none(ar_method_depl, "ADMIN-DATA/SDGS/SDG/SD")
            if sd is not None and sd.attrib["GID"] == "iSOFT:com:IpcMethodDeployment:MethodId":
                result['id'] = sd
        return result

    def _get_eventgrps_deployment(self, events_deployment, fields_deployment):
        """
        Extract IPC event group deployment information, will be returned
        as a dict with information
        """
        inner = collections.OrderedDict()
        result = dict(event_groups=inner)

        for _, event_depl in events_deployment.items():
            if event_depl:
                # Construct an unique name for a newly created eventgroup by appending fixed string to an event's name
                group_name = event_depl["name"] + "EventGroup"
                result['event_groups'][group_name] = dict(
                    id=event_depl["id"],
                    events=[event_depl["id"]]
                )

        for _, field_depl in fields_deployment.items():
            if field_depl:
                notifier = field_depl["notifier"]
                if notifier:
                    # Construct an unique name for a newly created eventgroup by appending fixed to an notifier's name
                    group_name = notifier["name"] + "EventGroup"
                    result['event_groups'][group_name] = dict(
                        id=notifier["id"],
                        events=[notifier["id"]]
                    )

        return result

    def get_instance_deployment(self, ar_deployment):
        """
        Extract IPC deployment information, will be returned as a
        dict with information
        """
        self.judge_dep_type(ar_deployment)
        serviceInterfaceId = None
        fqn = ar_deployment.get_fqn()
        if fqn in self._user_defined_service_interfaces_ids_ob:
            serviceInterfaceId = self._user_defined_service_interfaces_ids_ob[fqn]
        if serviceInterfaceId is None:
            generateId = get_hash(fqn)
            if generateId in self._user_defined_service_interfaces_ids:
                while generateId in self._user_defined_service_interfaces_ids:
                    generateId += 1
            self._user_defined_service_interfaces_ids.append(generateId)
            self._user_defined_service_interfaces_ids_ob[fqn] = generateId
            serviceInterfaceId = generateId
        id_counter = 1
        interface_ref = get_element_or_none(ar_deployment, "SERVICE-INTERFACE-REF")
        interface_ele = self.model.find_referable(interface_ref.text)
        interfaceOb = {
            "events": [],
            "methods": [],
            "fields": []
        }
        events = get_element_or_none(interface_ele, "EVENTS/VARIABLE-DATA-PROTOTYPE")
        if events is not None:
            for item in events:
                interfaceOb["events"].append(item.get_fqn())
        methods = get_element_or_none(interface_ele, "METHODS/CLIENT-SERVER-OPERATION")
        if methods is not None:
            for item in methods:
                interfaceOb["methods"].append(item.get_fqn())
        fields = get_element_or_none(interface_ele, "FIELDS/FIELD")
        if fields is not None:
            for item in fields:
                interfaceOb["fields"].append(item.get_fqn())

        result = dict(service_id=serviceInterfaceId,
                      events={}, methods={}, fields={}, event_groups={})

        eventdeploymens = get_element_or_none(ar_deployment, 'EVENT-DEPLOYMENTS')
        if eventdeploymens is not None:
            event_depls = ar_deployment.EVENT_DEPLOYMENTS.find_elements_of_type(
                "USER-DEFINED-EVENT-DEPLOYMENT")
            for event_depl in event_depls:
                key = get_element_or_die(event_depl, "EVENT-REF").text
                if key in interfaceOb["events"]:
                    interfaceOb["events"].remove(key)
                result['events'][key] = self._get_event_deployment(event_depl, id_counter)
                id_counter += 1
        fielddeploymens = get_element_or_none(ar_deployment, 'FIELD-DEPLOYMENTS')
        if fielddeploymens is not None:
            field_depls = ar_deployment.FIELD_DEPLOYMENTS.find_elements_of_type(
                "USER-DEFINED-FIELD-DEPLOYMENT")
            for field_depl in field_depls:
                key = get_element_or_die(field_depl, "FIELD-REF").text
                if key in interfaceOb["fields"]:
                    interfaceOb["fields"].remove(key)
                fieldsEle = self._get_field_deployment(field_depl, id_counter)
                self.interface_field_deployment(interface_ele, key, fieldsEle, ar_deployment)
                result["fields"][key] = fieldsEle

                # get_field_user_defined_deployment function use an id for every non-empty dict in returned dict, so
                # increment id_counter by a number of used ids, which is total number of elements in returned dict minus
                # number of empty elements
                methods = list(result['fields'][field_depl.FIELD_REF].values())
                id_counter += len(methods) - methods.count({})
        methodsdeploymens = get_element_or_none(ar_deployment, 'METHOD-DEPLOYMENTS')
        if methodsdeploymens is not None:
            method_depls = ar_deployment.METHOD_DEPLOYMENTS.find_elements_of_type(
                "USER-DEFINED-METHOD-DEPLOYMENT")
            for method_depl in method_depls:
                key = get_element_or_die(method_depl, "METHOD-REF").text
                if key in interfaceOb["methods"]:
                    interfaceOb["methods"].remove(key)
                result['methods'][key] = self._get_method_deployment(method_depl, id_counter)
                id_counter += 1

        assert len(interfaceOb["events"]) + len(interfaceOb["fields"]) + len(interfaceOb["methods"]) == 0, \
            "[CM  ] CODE-008: Failed (The interface [{}] should be deployed properly in [{}]).LocalPath:{}:{}.".format(interface_ele.get_fqn(), ar_deployment.get_fqn(), interface_ele.get_path(), ar_deployment.get_path())

        event_group_depls = self._get_eventgrps_deployment(
            result['events'], result['fields'])

        result = {**result, **event_group_depls}
        ipcService = {
            "serviceId": serviceInterfaceId,
            "major": 0,
            "minor": 0
        }
        iccService = {
            "serviceId": serviceInterfaceId,
            "major": 0,
            "minor": 0
        }
        sdgList = to_list(get_element_or_none(ar_deployment, "ADMIN-DATA/SDGS/SDG"))
        for sdg in sdgList:
            if sdg.attrib["GID"] == "iSOFT:com:IpcServiceInterfaceDeployment":
                sdList = get_element_or_none(sdg, "SD")
                if sdList is not None:
                    for sd in sdList:
                        if sd.attrib["GID"] == "iSOFT:com:IpcServiceInterfaceDeployment:Service":
                            ipcService["serviceId"] = sd
                        elif sd.attrib["GID"] == "iSOFT:com:IpcServiceInterfaceDeployment:Major":
                            ipcService["major"] = sd
                        elif sd.attrib["GID"] == "iSOFT:com:IpcServiceInterfaceDeployment:Minor":
                            ipcService["minor"] = sd
            elif sdg.attrib["GID"] == "iSOFT:com:IccServiceInterfaceDeployment":
                sdList = get_element_or_none(sdg, "SD")
                if sdList is not None:
                    for sd in sdList:
                        if sd.attrib["GID"] == "iSOFT:com:IccServiceInterfaceDeployment:Service":
                            iccService["serviceId"] = sd
        result["ipcService"] = ipcService
        result["iccService"] = iccService
        result["depType"] = self.depType.lower()
        return result

    def interface_field_deployment(self, interface, filedFqn, filedEle, ar_deployment):
        interfaceFile = self.model.find_referable(filedFqn)
        configuration = True
        if interfaceFile.HAS_NOTIFIER and "name" not in filedEle["notifier"]:
            configuration = False
        if interfaceFile.HAS_GETTER and "name" not in filedEle["getter"]:
            configuration = False
        if interfaceFile.HAS_SETTER and "name" not in filedEle["setter"]:
            configuration = False
        assert configuration, \
            "[CM  ] CODE-008: Failed (The interface [{}] should be deployed properly in [{}]).LocalPath:{}:{}.".format(interface.get_fqn(), ar_deployment.get_fqn(), interface.get_path(), ar_deployment.get_path())

    def get_required_user_deployment(self, required_si):
        instance_id = get_hash(get_element_or_die(required_si, "SHORT-NAME").text)
        result = {
            "instance_id": "ANY",
            "ipcShm": {"instanceId": "ANY"}
        }
        return result

    def get_provided_user_deployment(self, provided_si, deploymentOb):

        instance_id = get_hash(get_element_or_die(provided_si, "SHORT-NAME").text)
        ipcShm = {
            "shmPath": "{}_{}.shm".format(deploymentOb["service_id"], instance_id),
            "shmSegSize": 41943040,
            "shmSegCount": 4,
            "msgQueueLen": 128,
            "instanceId": instance_id
        }
        sdgList = get_element_or_none(provided_si, "ADMIN-DATA/SDGS/SDG")
        if sdgList is not None:
            for sdg in sdgList:
                if sdg.attrib["GID"] == "iSOFT:com:IpcServiceInstanceDeployment":
                    sdList = get_element_or_none(sdg, "SD")
                    if sdList is not None:
                        for sd in sdList:
                            if sd.attrib["GID"] == "iSOFT:com:IpcServiceInstanceDeployment:ShmPath":
                                ipcShm["shmPath"] = sd
                            elif sd.attrib["GID"] == "iSOFT:com:IpcServiceInstanceDeployment:ShmSegSize":
                                ipcShm["shmSegSize"] = sd
                            elif sd.attrib["GID"] == "iSOFT:com:IpcServiceInstanceDeployment:ShmSegCount":
                                ipcShm["shmSegCount"] = sd
                            elif sd.attrib["GID"] == "iSOFT:com:IpcServiceInstanceDeployment:MsgQueueLen":
                                ipcShm["msgQueueLen"] = sd
                            elif sd.attrib["GID"] == "iSOFT:com:IpcServiceInstanceDeployment:Instance":
                                ipcShm["instanceId"] = sd
                                instance_id = sd
                    break
        result = {
            "instance_id": instance_id,
            "ipcShm": ipcShm
        }

        return result


    def get_icc_user_deployment(self, si, isP):
        instance_id = get_hash(get_element_or_die(si, "SHORT-NAME").text)
        iccOb = {
            "instanceId": instance_id,
            "iccType": "0",
            "iccIdentifier": "",
        }
        if isP:
            gid = "iSOFT:com:ProvidedIccServiceInstanceDeployment"
        else:
            gid = "iSOFT:com:RequiredIccServiceInstanceDeployment"
        sdgList = to_list(get_element_or_none(si, "ADMIN-DATA/SDGS/SDG"))
        for sdg in sdgList:
            if sdg.attrib["GID"] == gid:
                sdList = get_element_or_none(sdg, "SD")
                if sdList is not None:
                    for sd in sdList:
                        if sd.attrib["GID"] == gid + ":IccType":
                            iccOb["iccType"] = sd
                        elif sd.attrib["GID"] == gid + ":IccIdentifier":
                            iccOb["iccIdentifier"] = sd
                        elif sd.attrib["GID"] == gid + ":Instance":
                            iccOb["instanceId"] = sd
                            instance_id = sd
                break

        result = {
            "instance_id": instance_id,
            "iccOb": iccOb
        }
        return result

    def _populate_eventgrp_deployment(self, service_deployment, event_group_depls):
        """Populate an event group deployment"""
        for event_group_depl_dict in event_group_depls.values():
            event_group_id = int(event_group_depl_dict["id"])
            threshold = 0
            if "threshold" in event_group_depl_dict:
                threshold = event_group_depl_dict["threshold"]
            event_group_deployment = EventGroupDeployment(event_group_id)
            event_group_deployment.set_threshold(threshold)
            service_deployment.add_event_group_deployment(
                event_group_deployment)
            for event in event_group_depl_dict["events"]:
                event_group_deployment.add_event(int(event))

    def _populate_event_deployment(self, events, event_deployment):
        """Populate an event deployment"""
        for event_fqn, event_depl in event_deployment.items():
            event_name = short_name(event_fqn)
            for event in events:
                if event.name == event_name:
                    event.event_deployment = EventDeployment()
                    event.event_deployment.deployment_id = int(
                        event_depl["id"])
                    event.event_deployment.proto = event_depl["proto"]

    def _populate_method_deployment(self, methods, method_deployment):
        """Populate a method deployment"""
        for method_fqn, method_depl in method_deployment.items():
            method_name = short_name(method_fqn)
            for method in methods:
                if method.name == method_name:
                    method.deployment = MethodDeployment()
                    method.deployment.deployment_id = method_depl["id"]
                    method.deployment.proto = method_depl["proto"]

    def _populate_field_deployment(self, fields, field_deployment):
        """Populate a field deployment"""
        for field_fqn, field_depl in field_deployment.items():
            field_name = short_name(field_fqn)
            field_getter = field_depl["getter"]
            field_setter = field_depl["setter"]
            field_notifier = field_depl["notifier"]
            for field in fields:
                if field.name == field_name:
                    field.deployment = FieldDeployment()
                    field.deployment["getter"] = field_getter
                    field.deployment["setter"] = field_setter
                    field.deployment["notifier"] = field_notifier

    def populate_deployment(self, service, deployment):
        """Populate a service deployment"""
        if deployment.required:
            self._populate_event_deployment(
                service.events, deployment.required["events"])
            self._populate_method_deployment(
                service.methods, deployment.required["methods"])
            self._populate_field_deployment(
                service.fields, deployment.required["fields"])
            self._populate_eventgrp_deployment(
                deployment,
                deployment.required["event_groups"])
        elif deployment.provided:
            self._populate_event_deployment(
                service.events, deployment.provided["events"])
            self._populate_method_deployment(
                service.methods, deployment.provided["methods"])
            self._populate_field_deployment(
                service.fields, deployment.provided["fields"])
            self._populate_eventgrp_deployment(
                deployment,
                deployment.provided["event_groups"])
