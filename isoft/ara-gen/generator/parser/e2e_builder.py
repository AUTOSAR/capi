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
from generator.common.tree_helper import get_element_or_none, short_name, get_element_or_0, to_list, get_element_or_str, get_element_or_false, get_element_or_die, transition_number
from ..views.user_defined.user_defined_service_view import UserDefinedServiceView

"""
获取iam 模块Json
"""


class E2EBuilder:

    def __init__(self, model):
        self._log = logging.getLogger(__name__)
        self.model = model
        self.p_service = []
        self.isNeedE2E = False

    def get_e2e_someip(self, ar_process, si_mappings):
        self.p_service = []
        self.isNeedE2E = False
        for item in si_mappings["provided"]:
            service = item["service"]
            if service.service_deployment is not None:
                classname = service.service_deployment.__class__.__name__
                if classname == 'UserDefinedServiceDeployment':
                    self.p_service.append(UserDefinedServiceView(service))
        End2EndEventProtectionProps = []
        End2EndMethodProtectionProps = []
        End2EndEventDataIdMapping = []
        End2EndMethodDataIdMapping = []
        port_mapping = self.model.find_elements_of_type("SERVICE-INSTANCE-TO-PORT-PROTOTYPE-MAPPING")
        for port_item in port_mapping:
            process_ref = get_element_or_none(port_item, "PROCESS-REF")
            if process_ref is not None and process_ref == ar_process.get_fqn():
                service_instance_ref = get_element_or_none(port_item, "SERVICE-INSTANCE-REF")
                if service_instance_ref is not None:
                    service_instance_item = self.model.find_referable(service_instance_ref.text)
                    self.get_data_id(service_instance_item, End2EndEventProtectionProps, End2EndMethodProtectionProps, End2EndEventDataIdMapping, End2EndMethodDataIdMapping)
        if self.isNeedE2E:
            self.verify_source_id()
        return {
            "propMap": {
                "End2EndEventProtectionProps": End2EndEventProtectionProps,
                "End2EndMethodProtectionProps": End2EndMethodProtectionProps
            },
            "dataIdMap": {
                "End2EndEventDataIdMapping": End2EndEventDataIdMapping,
                "End2EndMethodDataIdMapping": End2EndMethodDataIdMapping
            }

        }

    def get_data_id(self, service_instance_item, End2EndEventProtectionProps, End2EndMethodProtectionProps, End2EndEventDataIdMapping, End2EndMethodDataIdMapping):
        event_protection = get_element_or_none(service_instance_item, "E-2-E-EVENT-PROTECTION-PROPSS")
        event_list = []
        if event_protection is not None:
            event_list = event_protection.find_elements_of_type("END-2-END-EVENT-PROTECTION-PROPS")
            self.set_list(event_list, End2EndEventProtectionProps)
        method_protection = get_element_or_none(service_instance_item, "E-2-E-METHOD-PROTECTION-PROPSS")
        method_list = []
        if method_protection is not None:
            method_list = method_protection.find_elements_of_type("END-2-END-METHOD-PROTECTION-PROPS")
            self.set_list(method_list, End2EndMethodProtectionProps)
        if len(event_list + method_list) > 0:
            self.isNeedE2E = True
            self.set_data_id_mapping(service_instance_item, event_list + method_list, End2EndEventDataIdMapping, End2EndMethodDataIdMapping)

    def get_all_e2e_someip(self):
        End2EndEventProtectionProps = []
        End2EndMethodProtectionProps = []
        End2EndEventDataIdMapping = []
        End2EndMethodDataIdMapping = []
        someip_mapping = self.model.find_elements_of_type("SOMEIP-SERVICE-INSTANCE-TO-MACHINE-MAPPING")
        for someip_item in someip_mapping:
            service_instance_ref = get_element_or_none(someip_item, "SERVICE-INSTANCE-REFS/SERVICE-INSTANCE-REF")
            if service_instance_ref is not None:
                service_instance_list = to_list(service_instance_ref)
                for service_instance_item_ref in service_instance_list:
                    service_instance_item = self.model.find_referable(service_instance_item_ref.text)
                    self.get_data_id(service_instance_item, End2EndEventProtectionProps, End2EndMethodProtectionProps, End2EndEventDataIdMapping, End2EndMethodDataIdMapping)
        return {
            "propMap": {
                "End2EndEventProtectionProps": End2EndEventProtectionProps,
                "End2EndMethodProtectionProps": End2EndMethodProtectionProps
            },
            "dataIdMap": {
                "End2EndEventDataIdMapping": End2EndEventDataIdMapping,
                "End2EndMethodDataIdMapping": End2EndMethodDataIdMapping
            }

        }

    def set_data_id_mapping(self, service_instance_item, allList, event_mapping, method_mapping):
        if service_instance_item is not None:
            short_tag = service_instance_item.short_tag()
            if short_tag in ["REQUIRED-USER-DEFINED-SERVICE-INSTANCE", "PROVIDED-USER-DEFINED-SERVICE-INSTANCE"]:
                instance_id = None
            elif short_tag in ["REQUIRED-SOMEIP-SERVICE-INSTANCE", "DDS-REQUIRED-SERVICE-INSTANCE"]:
                instance_id = transition_number(get_element_or_none(service_instance_item, "REQUIRED-SERVICE-INSTANCE-ID"),defaultInt=None)
            else:
                instance_id = transition_number(get_element_or_die(service_instance_item, "SERVICE-INSTANCE-ID"))
            service_instance_id = transition_number(instance_id, defaultInt=-1)
            service_id = 0
            deployment_ref = get_element_or_none(service_instance_item, "SERVICE-INTERFACE-DEPLOYMENT-REF")
            service = None
            if deployment_ref is not None:
                deployment_element = self.model.find_referable(deployment_ref.text)
                if deployment_element is not None:
                    service = self.get_service(deployment_element)
                    if service is not None:
                        service_instance_id = transition_number(service.userdef_specific_instance_id)
                        service_id = transition_number(service.service_interface_id)
                    else:
                        service_id = transition_number(get_element_or_0(deployment_element, "SERVICE-INTERFACE-ID"))
            for list_item in allList:
                item_ob = {
                    "serviceId": service_id,
                    "instanceId": service_instance_id
                }
                event_id = None
                event_deployment_ref = get_element_or_none(list_item, "EVENT-REF")
                if event_deployment_ref is not None:
                    event_element = self.model.find_referable(event_deployment_ref.text)
                    if service is not None:
                        event_ref = get_element_or_none(event_element, "EVENT-REF")
                        for event in service.events:
                            if event_ref is not None and short_name(event_ref.text) == event.get_event.name:
                                event_id = transition_number(event.deployment_id)
                                break
                    else:
                        event_id = transition_number(get_element_or_none(event_element, "EVENT-ID")) + 32768
                    if event_id is not None:
                        item_ob["eventId"] = event_id
                method_id = None
                method_deployment_ref = get_element_or_none(list_item, "METHOD-REF")
                if method_deployment_ref is not None:
                    method_element = self.model.find_referable(method_deployment_ref.text)
                    if service is not None:
                        method_ref = get_element_or_none(method_element, "METHOD-REF")
                        for method in service.methods:
                            if method_ref is not None and short_name(method_ref.text) == method.get_method.name:
                                method_id = transition_number(method.deployment_id)
                                break
                    else:
                        method_element = self.model.find_referable(method_deployment_ref.text)
                        method_id = transition_number(get_element_or_none(method_element, "METHOD-ID"))
                    if method_id is not None:
                        item_ob["methodId"] = method_id
                sourceId = get_element_or_0(list_item, "SOURCE-ID")
                item_ob["sourceId"] = transition_number(sourceId)
                data_ids = get_element_or_none(list_item, "DATA-IDS")
                if data_ids is not None:
                    data_id_list = data_ids.find_elements_of_type("DATA-ID")
                    item_ob["dataId"] = data_id_list
                if "dataId" in item_ob and "eventId" in item_ob:
                    if item_ob not in event_mapping:
                        event_mapping.append(item_ob)
                if "dataId" in item_ob and "methodId" in item_ob:
                    if item_ob not in event_mapping:
                        method_mapping.append(item_ob)

    def get_service(self, deploymentEle):
        deployment_short_tag = deploymentEle.short_tag()
        if deployment_short_tag == "USER-DEFINED-SERVICE-INTERFACE-DEPLOYMENT":
            fqn = deploymentEle.get_fqn()
            for item in self.p_service:
                classname = item.__class__.__name__
                if classname == "UserDefinedServiceView":
                    if item.interface_deployment_fqn == fqn:
                        return item
        return None

    def set_list(self, event_list, map_list):
        for event_item in event_list:
            event_ob = {
                "dataLength": transition_number(get_element_or_0(event_item, "DATA-LENGTH")),
                "maxDataLength": transition_number(get_element_or_0(event_item, "MAX-DATA-LENGTH")),
                "minDataLength": transition_number(get_element_or_0(event_item, "MIN-DATA-LENGTH")),
                "dataUpdatePeriod": get_element_or_0(event_item, "DATA-UPDATE-PERIOD")
            }
            data_ids = get_element_or_none(event_item, "DATA-IDS")
            if data_ids is not None:
                data_id_list = data_ids.find_elements_of_type("DATA-ID")
                if len(data_id_list) == 1:
                    event_ob["dataId"] = data_id_list[0]
                    event_ob["dataIdList"] = []
                else:
                    dataIdList = []
                    for data_id in data_id_list:
                        dataIdList.append(data_id)
                    event_ob["dataId"] = 0
                    event_ob["dataIdList"] = dataIdList

            profile_configuration_ref = get_element_or_none(event_item, "E-2-E-PROFILE-CONFIGURATION-REF")
            if profile_configuration_ref is not None:
                profile_configuration_element = self.model.find_referable(profile_configuration_ref.text)
                if profile_configuration_element is not None:
                    event_ob["dataIdMode"] = transition_number(get_element_or_str(profile_configuration_element, "DATA-ID-MODE"),defaultInt="")
                    event_ob["maxDeltaCounter"] = transition_number(get_element_or_0(profile_configuration_element, "MAX-DELTA-COUNTER"))
                    event_ob["maxErrorStateInit"] = transition_number(get_element_or_0(profile_configuration_element, "MAX-ERROR-STATE-INIT"))
                    event_ob["maxErrorStateInvalid"] = transition_number(get_element_or_0(profile_configuration_element, "MAX-ERROR-STATE-INVALID"))
                    event_ob["maxErrorStateValid"] = transition_number(get_element_or_0(profile_configuration_element, "MAX-ERROR-STATE-VALID"))
                    event_ob["minOkStateInit"] = transition_number(get_element_or_0(profile_configuration_element, "MIN-OK-STATE-INIT"))
                    event_ob["minOkStateInvalid"] = transition_number(get_element_or_0(profile_configuration_element, "MIN-OK-STATE-INVALID"))
                    event_ob["minOkStateValid"] = transition_number(get_element_or_0(profile_configuration_element, "MIN-OK-STATE-VALID"))
                    event_ob["windowSizeInit"] = transition_number(get_element_or_0(profile_configuration_element, "WINDOW-SIZE-INIT"))
                    event_ob["windowSizeInvalid"] = transition_number(get_element_or_0(profile_configuration_element, "WINDOW-SIZE-INVALID"))
                    event_ob["windowSizeValid"] = transition_number(get_element_or_0(profile_configuration_element, "WINDOW-SIZE-VALID"))
                    event_ob["profileName"] = get_element_or_str(profile_configuration_element, "PROFILE-NAME")
                    event_ob["clearFromValidToInvalid"] = get_element_or_false(profile_configuration_element, "CLEAR-FROM-VALID-TO-INVALID")
            isAdd = True
            for item in map_list:
                if item == event_ob:
                    isAdd = False
                    break
            if isAdd:
                map_list.append(event_ob)

    def verify_source_id(self):
        requestServiceInstanceMapping = self.model.find_elements_of_type("REQUIRED-USER-DEFINED-SERVICE-INSTANCE") + self.model.find_elements_of_type("REQUIRED-SOMEIP-SERVICE-INSTANCE") + self.model.find_elements_of_type("DDS-REQUIRED-SERVICE-INSTANCE")
        requiredSourceId = {}
        requiredMethodOb = {}
        for item in requestServiceInstanceMapping:
            self.app_774_protection(item, requiredSourceId, requiredMethodOb)
        providedSourceId = {}
        providedMethodOb = {}
        providedServiceInstanceMapping = self.model.find_elements_of_type("PROVIDED-USER-DEFINED-SERVICE-INSTANCE") + self.model.find_elements_of_type("PROVIDED-SOMEIP-SERVICE-INSTANCE") + self.model.find_elements_of_type("DDS-PROVIDED-SERVICE-INSTANCE")
        for item in providedServiceInstanceMapping:
            self.app_774_protection(item, providedSourceId, providedMethodOb)
        for methodRef in requiredMethodOb:
            if methodRef in providedMethodOb:
                requiredMethodItem = requiredMethodOb[methodRef]
                providedMethodItem = providedMethodOb[methodRef]
                if requiredMethodItem != providedMethodItem:
                    protectionItem = requiredSourceId[requiredMethodOb[methodRef]["sourceId"]]
                    assert False, "[CM  ] CODE-014: The configuration of E2E for [{}] is matched one-to-one with the Server on the Client side. Path:{}. LocalPath:{}.".format(methodRef, protectionItem.get_fqn(), protectionItem.get_path())

    def app_774_protection(self, item, sourceIdOb, methodOb):
        methodProtection = item.find_elements_of_type("END-2-END-METHOD-PROTECTION-PROPS")
        for protectionItem in methodProtection:
            sourceId = transition_number(get_element_or_0(protectionItem, "SOURCE-ID"))
            assert 0 <= sourceId <= 268435455, "[CM  ] CODE-015: SourceID[{}] value range [0,268435455].Path:{}. LocalPath:{}.".format(str(sourceId), protectionItem.get_fqn(), protectionItem.get_path())
            assert str(sourceId) not in sourceIdOb, "[CM  ] CODE-016: SourceID[{}] must be unique. SourceID is duplicated in [{}, {}].LocalPath:[{}, {}]".format(str(sourceId), sourceIdOb[str(sourceId)].get_fqn(), protectionItem.get_fqn(), sourceIdOb[str(sourceId)].get_path(), protectionItem.get_path())
            sourceIdOb[str(sourceId)] = protectionItem
            methodRef = get_element_or_none(protectionItem, "METHOD-REF")
            if methodRef is not None:
                methodItemOb = {
                    "sourceId": str(sourceId),
                    "dataLength": transition_number(get_element_or_0(protectionItem, "DATA-LENGTH")),
                    "maxDataLength": transition_number(get_element_or_0(protectionItem, "MAX-DATA-LENGTH")),
                    "minDataLength": transition_number(get_element_or_0(protectionItem, "MIN-DATA-LENGTH")),
                    "dataUpdatePeriod": get_element_or_0(protectionItem, "DATA-UPDATE-PERIOD")
                }
                data_ids = get_element_or_none(protectionItem, "DATA-IDS")
                if data_ids is not None:
                    data_id_list = data_ids.find_elements_of_type("DATA-ID")
                    methodItemOb["dataId"] = data_id_list
                configurationRef = get_element_or_str(protectionItem, "E-2-E-PROFILE-CONFIGURATION-REF")
                methodItemOb["configurationRef"] = configurationRef
                methodOb[methodRef.text] = methodItemOb
