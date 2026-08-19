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



from . import commen_util
from generator.common.tree_helper import get_element_or_none, get_element_or_0, to_list, \
    get_element_or_die, to_str, short_name, get_element_or_false, transition_number, get_element_or_str

"""
获取iam 模块Json
"""


class IAMBuilder:

    def __init__(self, model):
        self._log = logging.getLogger(__name__)
        self.model = model
        self.p_service = []

    def get_iam_com(self, ar_machine, p_service):
        self.p_service = p_service
        iamSomeip = {}
        iam_instantiation = get_element_or_none(ar_machine, "MODULE-INSTANTIATIONS/IAM-MODULE-INSTANTIATION")
        if iam_instantiation is not None:
            iam_instantiations = to_list(iam_instantiation)
            if len(iam_instantiations) > 1:
                iam_fqns = [item.get_fqn() for item in iam_instantiations]
                assert False, (
                    "[IAM ] CODE-003: IamModuleInstantiation must be unique: {}".format(",".join(iam_fqns))
                )
            local = get_element_or_none(iam_instantiation, "LOCAL-COM-ACCESS-CONTROL-ENABLED")
            remote = get_element_or_none(iam_instantiation, "REMOTE-ACCESS-CONTROL-ENABLED")
            local = False if str(local).lower() in ["0", "false"] else True
            remote = False if str(remote).lower() in ["0", "false"] else True
            iamSomeip["LocalComAccessControlEnabled"] = local
            iamSomeip["RemoteAccessControlEnabled"] = remote
            if local or remote:
                iamSomeip["StrictVerificationMode"] = False
                self.iam_someip_local(ar_machine, iamSomeip, local, remote)
            if not local and not remote:
                return None
        else:
            return None

        return iamSomeip

    def iam_someip_local(self, ar_machine, iamSomeip, local, remote):
        eventGrantList = []
        methodGrantList = []
        remoteEventGrantList = []
        remoteMethodGrantList = []
        offerGrantList = []
        findGrantList = []
        rawDataStreamGrants = []
        grant_refs = get_element_or_none(ar_machine, "MODULE-INSTANTIATIONS/IAM-MODULE-INSTANTIATION/GRANT-REFS")
        if grant_refs is not None:
            grant_ref_map = get_element_or_none(grant_refs, "GRANT-REF")
            if grant_ref_map is not None:
                grant_ref_map = to_list(grant_ref_map)
                for grant_ref in grant_ref_map:
                    dest = grant_ref.attrib["DEST"]
                    if "COM-EVENT-GRANT" == dest:
                        self.set_list(grant_ref, eventGrantList, remoteEventGrantList)
                    elif "COM-METHOD-GRANT" == dest:
                        self.set_list(grant_ref, methodGrantList, remoteMethodGrantList)
                    elif "COM-OFFER-SERVICE-GRANT" == dest:
                        self.set_service_list(grant_ref, offerGrantList)
                    elif "COM-FIND-SERVICE-GRANT" == dest:
                        self.set_service_list(grant_ref, findGrantList)
                    elif "COM-FIELD-GRANT" == dest:
                        self.set_field_list(grant_ref, eventGrantList, remoteEventGrantList, methodGrantList, remoteMethodGrantList)
                    # elif "ETHERNET-RAW-DATA-STREAM-GRANT" == dest:
                    #     self.set_raw_list(grant_ref, rawDataStreamGrants)

        if local:
            iamSomeip["ComEventGrants"] = eventGrantList
            iamSomeip["ComMethodGrants"] = methodGrantList
            iamSomeip["ComOfferServiceGrants"] = offerGrantList
            iamSomeip["ComFindServiceGrants"] = findGrantList
            # iamSomeip["RawDataStreamGrants"] = rawDataStreamGrants
        if remote:
            if len(remoteEventGrantList):
                iamSomeip["ComRemoteEventGrants"] = remoteEventGrantList
            if len(remoteMethodGrantList):
                iamSomeip["ComRemoteMethodGrants"] = remoteMethodGrantList

    def get_iam_raw(self, ar_machine):
        rawOb = {}
        iam_instantiation = get_element_or_none(ar_machine, "MODULE-INSTANTIATIONS/IAM-MODULE-INSTANTIATION")
        if iam_instantiation is not None:
            iam_instantiations = to_list(iam_instantiation)
            if len(iam_instantiations) > 1:
                iam_fqns = [item.get_fqn() for item in iam_instantiations]
                assert False, (
                     "[IAM ] CODE-003: IamModuleInstantiation must be unique: {}".format(",".join(iam_fqns))
                )
            local = get_element_or_none(iam_instantiation, "LOCAL-COM-ACCESS-CONTROL-ENABLED")
            if local is not None:
                rawOb["LocalComAccessControlEnabled"] = local
            if local is not None and local and len(iam_instantiations) > 0:
                rawDataStreamGrants = []
                grant_refs = get_element_or_none(iam_instantiations[0], "GRANT-REFS")
                if grant_refs is not None:
                    grant_ref_map = get_element_or_none(grant_refs, "GRANT-REF")
                    if grant_ref_map is not None:
                        grant_ref_map = to_list(grant_ref_map)
                        for grant_ref in grant_ref_map:
                            dest = grant_ref.attrib["DEST"]
                            if "ETHERNET-RAW-DATA-STREAM-GRANT" == dest:
                                self.set_raw_list(grant_ref, rawDataStreamGrants)
                rawOb["RawDataStreamGrants"] = rawDataStreamGrants
        return rawOb

    def set_raw_list(self, grant_ref, rawDataStreamGrants):
        grant = self.model.find_referable(grant_ref.text)
        streamMappingRef = get_element_or_none(grant, "ETHERNET-RAW-DATA-STREAM-MAPPING-REF")
        if streamMappingRef is not None:
            streamMapping = self.model.find_referable(streamMappingRef.text)
            connectorRef = get_element_or_none(streamMapping, "COMMUNICATION-CONNECTOR-REF")
            ipAddress = ""
            if connectorRef is not None:
                connectorEle = self.model.find_referable(connectorRef.text)
                unicastEndpointRef = get_element_or_none(connectorEle, "UNICAST-NETWORK-ENDPOINT-REF")
                if unicastEndpointRef is not None:
                    endpointEle = self.model.find_referable(unicastEndpointRef.text)
                    ipAddress = commen_util.get_network_address(endpointEle)
            mulCastUdpPort = transition_number(get_element_or_0(streamMapping, "MULTICAST-UDP-PORT"))
            tcpPort = transition_number(get_element_or_0(streamMapping, "TCP-PORT"))
            udpPort = transition_number(get_element_or_0(streamMapping, "UDP-PORT"))
            processPath = get_element_or_str(streamMapping, "PROCESS-REF")
            assert (tcpPort == 0 and udpPort == 0 and mulCastUdpPort > 0) or (tcpPort > 0 and udpPort == 0 and mulCastUdpPort == 0) or (tcpPort == 0 and udpPort > 0 and mulCastUdpPort == 0), "[IAM ] CODE-001: EthernetRawDataStreamMapping.[multicastUdpPort, tcpPort, udpPort].There is one and only one that is not 0.Path:{}.LocalPath:{}".format(
                streamMappingRef.text, streamMapping.get_path())
            rawDataStreamGrantItem = {
                "processname": processPath,
                "ip": ipAddress,
                "udpPort": udpPort,
                "tcpPort": tcpPort,
                "mulCastUdpPort": mulCastUdpPort
            }
            if rawDataStreamGrantItem not in rawDataStreamGrants:
                rawDataStreamGrants.append(rawDataStreamGrantItem)

    def set_field_list(self, ref, eventGrantList, remoteEventGrantList, methodGrantList, remoteMethodGrantList):
        item = self.model.find_referable(ref.text)
        if item is not None:
            item_ob = self.set_service_instance(ref)
            if item_ob is not None:
                service = item_ob["service"]
                del item_ob["service"]
                service_deployment_ref = get_element_or_none(item, "SERVICE-DEPLOYMENT-REF")
                if service_deployment_ref is not None:
                    service_deployment_element = self.model.find_referable(service_deployment_ref.text)
                    if service_deployment_element is not None:
                        field_ref = get_element_or_none(service_deployment_element, "FIELD-REF")
                        if field_ref is not None:
                            fieldEle = self.model.find_referable(field_ref.text)
                            field = None
                            if service is not None:
                                for field_item in service.all_fields:
                                    if short_name(field_ref.text) == field_item.get_field.name:
                                        field = field_item
                                        break
                            remote_ref = get_element_or_none(item, "REMOTE-SUBJECT-REFS/REMOTE-SUBJECT-REF")

                            has_notifier = get_element_or_false(fieldEle, "HAS-NOTIFIER")
                            if has_notifier:
                                notifier = get_element_or_none(service_deployment_element, "NOTIFIER")
                                if notifier is not None:
                                    eventId = None
                                    if field is not None:
                                        eventId = transition_number(field.event_id)
                                    else:
                                        event_id = get_element_or_none(service_deployment_element, "NOTIFIER/EVENT-ID")
                                        if event_id is not None:
                                            eventId = transition_number(event_id) + 32768
                                    if eventId is not None:
                                        item_event_ob = copy.copy(item_ob)
                                        item_event_ob["eventid"] = eventId
                                        if remote_ref is not None:
                                            self.set_remote_list(item_ob["serviceid"], item_ob["instanceid"], eventId, None, remote_ref, remoteEventGrantList)
                                        elif item_event_ob not in eventGrantList:
                                            eventGrantList.append(item_event_ob)
                            role = get_element_or_none(item, "ROLE")
                            if role is not None:
                                has_setter = get_element_or_false(fieldEle, "HAS-SETTER")
                                has_getter = get_element_or_false(fieldEle, "HAS-GETTER")
                                if "SETTER" == role.text:
                                    assert has_setter, "[IAM ] CODE-002: Error,The role SETTER requires a real object, please complete setter deployment.FQN: {}. LocalPath: {}. ".format(ref.text, item.get_path())
                                    if field is not None:
                                        method_id = field.set_method_id
                                    else:
                                        method_id = transition_number(get_element_or_none(service_deployment_element, "SET/METHOD-ID"),defaultInt=None)
                                    self.set_role_method_list(item_ob, method_id, methodGrantList, remote_ref, item_ob["serviceid"], item_ob["instanceid"], remoteMethodGrantList)
                                elif "GETTER" == role.text:
                                    assert has_getter, "[IAM ] CODE-002: Error,The role GETTER requires a real object, please complete getter deployment.FQN: {}. LocalPath: {}. ".format(ref.text, item.get_path())
                                    if field is not None:
                                        method_id = field.get_method_id
                                    else:
                                        method_id = transition_number(get_element_or_none(service_deployment_element, "GET/METHOD-ID"),defaultInt=None)
                                    self.set_role_method_list(item_ob, method_id, methodGrantList, remote_ref, item_ob["serviceid"], item_ob["instanceid"], remoteMethodGrantList)
                                elif "GETTER-SETTER" == role.text:
                                    assert has_setter and has_getter, "[IAM ] CODE-002: Error,The role GETTER-SETTER requires a real object, please complete setter/getter deployment.FQN: {}. LocalPath: {}. ".format(ref.text, item.get_path())
                                    if field is not None:
                                        method_id_set = field.set_method_id
                                    else:
                                        method_id_set = transition_number(get_element_or_none(service_deployment_element, "SET/METHOD-ID"),defaultInt=None)
                                    self.set_role_method_list(item_ob, method_id_set, methodGrantList, remote_ref, item_ob["serviceid"], item_ob["instanceid"], remoteMethodGrantList)
                                    get_event_ob = copy.copy(item_ob)
                                    if field is not None:
                                        method_id_get = field.get_method_id
                                    else:
                                        method_id_get = transition_number(get_element_or_none(service_deployment_element, "GET/METHOD-ID"),defaultInt=None)
                                    self.set_role_method_list(get_event_ob, method_id_get, methodGrantList, remote_ref, item_ob["serviceid"], item_ob["instanceid"], remoteMethodGrantList)

    def set_role_method_list(self, item_ob, method_id, methodGrantList, remote_ref, service_interface_id, service_instance_id, remoteMethodGrantList):
        if method_id is not None:
            item_ob["methodid"] = transition_number(method_id)
            if remote_ref is not None:
                self.set_remote_list(service_interface_id, service_instance_id, None, transition_number(method_id), remote_ref, remoteMethodGrantList)
            elif item_ob not in methodGrantList:
                methodGrantList.append(item_ob)

    def set_list(self, ref, grantList, remoteList):
        item = self.model.find_referable(ref.text)
        if item is not None:
            item_ob = self.set_service_instance(ref)
            if item_ob is not None:
                service = item_ob["service"]
                del item_ob["service"]
                eventId = None
                methodId = None
                service_deployment_ref = get_element_or_none(item, "SERVICE-DEPLOYMENT-REF")
                if service_deployment_ref is not None:
                    service_deployment_element = self.model.find_referable(service_deployment_ref.text)
                    event_ref = get_element_or_none(service_deployment_element, "EVENT-REF")
                    method_ref = get_element_or_none(service_deployment_element, "METHOD-REF")
                    if service is not None:
                        if event_ref is not None:
                            for event in service.events:
                                if short_name(event_ref.text) == event.get_event.name:
                                    eventId = transition_number(event.deployment_id)
                                    item_ob["eventid"] = eventId
                                    break
                        if method_ref is not None:
                            for method in service.methods:
                                if short_name(method_ref.text) == method.get_method.name:
                                    methodId = transition_number(method.deployment_id)
                                    item_ob["methodid"] = methodId
                                    break
                    else:
                        event_id = get_element_or_none(service_deployment_element, "EVENT-ID")
                        if event_id is not None:
                            eventId = transition_number(event_id) + 32768
                            item_ob["eventid"] = eventId
                        method_id = get_element_or_none(service_deployment_element, "METHOD-ID")
                        if method_id is not None:
                            methodId = transition_number(method_id)
                            item_ob["methodid"] = methodId

                    remote_ref = get_element_or_none(item, "REMOTE-SUBJECT-REFS/REMOTE-SUBJECT-REF")
                    if remote_ref is not None:
                        self.set_remote_list(item_ob["serviceid"], item_ob["instanceid"], eventId, methodId, remote_ref, remoteList)
                    elif item_ob not in grantList:
                        grantList.append(item_ob)


    def set_service_instance(self, ref):
        item = self.model.find_referable(ref.text)
        if item is not None:
            service_instance_ref = get_element_or_none(item, "SERVICE-INSTANCE-REF")
            if service_instance_ref is not None:
                design_ref = get_element_or_none(item, "DESIGN-REF")
                process_design_ref = None
                if design_ref is not None:
                    design_ele = self.model.find_referable(design_ref.text)
                    if design_ele is not None:
                        process_design_ref = get_element_or_none(design_ele, "PROCESS-DESIGN-REF")
                processFqn = None
                port_mapping = self.model.find_elements_of_type("SERVICE-INSTANCE-TO-PORT-PROTOTYPE-MAPPING", SERVICE_INSTANCE_REF=service_instance_ref.text)
                for port_item in port_mapping:
                    if process_design_ref is not None:
                        processDR = to_str(get_element_or_none(port_item, "PROCESS-DESIGN-REF"))
                        if processDR == process_design_ref.text:
                            processFqn = get_element_or_none(port_item, "PROCESS-REF")
                            break
                if processFqn is None:
                    for port_item in port_mapping:
                        processFqn = get_element_or_none(port_item, "PROCESS-REF")
                        break

                service_instance_element = self.model.find_referable(service_instance_ref.text)
                if service_instance_element is not None:
                    short_tag = service_instance_element.short_tag()
                    if short_tag in ["REQUIRED-USER-DEFINED-SERVICE-INSTANCE", "PROVIDED-USER-DEFINED-SERVICE-INSTANCE"]:
                        service_instance = -1
                    elif short_tag in ["REQUIRED-SOMEIP-SERVICE-INSTANCE", "DDS-REQUIRED-SERVICE-INSTANCE"]:
                        service_instance = transition_number(get_element_or_none(service_instance_element, "REQUIRED-SERVICE-INSTANCE-ID"),defaultInt=None)
                    else:
                        service_instance = transition_number(get_element_or_die(service_instance_element, "SERVICE-INSTANCE-ID"))
                    if service_instance is not None and str(service_instance) == "ANY":
                        service_instance_id = 65535
                    else:
                        service_instance_id = transition_number(service_instance, defaultInt=-1)
                    deployment_ref = get_element_or_none(service_instance_element, "SERVICE-INTERFACE-DEPLOYMENT-REF")
                    if deployment_ref is not None:
                        deployment_element = self.model.find_referable(deployment_ref.text)
                        service = self.get_service(deployment_element)
                        if service is not None:
                            service_instance_id = transition_number(service.userdef_specific_instance_id)
                            service_interface_id = transition_number(service.service_interface_id)
                        else:
                            service_interface_id = transition_number(get_element_or_none(deployment_element, "SERVICE-INTERFACE-ID"))
                        if processFqn is not None:
                            item_ob = {
                                "processname": processFqn,
                                "serviceid": service_interface_id,
                                "instanceid": service_instance_id,
                                "service": service
                            }
                            return item_ob

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

    def set_service_list(self, ref, grantList):
        item_ob = self.set_service_instance(ref)
        if item_ob is not None:
            del item_ob["service"]
            if item_ob not in grantList:
                grantList.append(item_ob)

    def set_remote_list(self, serviceId, instanceId, eventId, methodId, remote_ref_list, remoteList):
        if remoteList is None or remote_ref_list is None:
            return
        for remote_ref in remote_ref_list:
            item_ob = {
                "serviceid": serviceId,
                "instanceid": instanceId
            }
            if eventId is not None:
                item_ob["eventid"] = eventId
            if methodId is not None:
                item_ob["methodid"] = methodId
            remote_element = self.model.find_referable(remote_ref.text)
            if remote_element is not None:
                connection_props = get_element_or_none(remote_element, "AUTHENTIC-CONNECTION-PROPSS/IP-IAM-AUTHENTIC-CONNECTION-PROPS")
                if connection_props is not None:
                    for connection_prop in connection_props:
                        endport_ref = get_element_or_none(connection_prop, "REMOTE-NETWORK-ENDPOINT-REF")
                        if endport_ref:
                            end_port_element = self.model.find_referable(endport_ref.text)
                            endpoint = get_element_or_none(end_port_element, "NETWORK-ENDPOINTS/NETWORK-ENDPOINT")
                            if endpoint is not None:
                                remoteOb = copy.copy(item_ob)
                                remoteOb["RemoteIp"] = commen_util.get_network_address(endpoint)
                                start = get_element_or_none(connection_prop, "REMOTE-PORT-RANGE-START")
                                if start is not None:
                                    remoteOb["RemotePortStart"] = transition_number(start)
                                end = get_element_or_none(connection_prop, "REMOTE-PORT-RANGE-END")
                                if end is not None:
                                    remoteOb["RemotePortEnd"] = transition_number(end)
                                ip_protocol = get_element_or_none(connection_prop, "IP-PROTOCOL")
                                if ip_protocol is not None and methodId is not None:
                                    remoteOb["Protocol"] = ip_protocol
                                remoteOb["LocalIp"] = ""
                                local_end_port_ref = get_element_or_none(connection_prop, "LOCAL-NETWORK-ENDPOINT-REF")
                                if local_end_port_ref is not None:
                                    local_end_port_element = self.model.find_referable(local_end_port_ref.text)
                                    local_endpoint = get_element_or_none(local_end_port_element, "NETWORK-ENDPOINTS/NETWORK-ENDPOINT")
                                    if local_endpoint is not None:
                                        remoteOb["LocalIp"] = commen_util.get_network_address(local_endpoint)
                                remoteOb["LocalPortStart"] = transition_number(get_element_or_none(connection_prop, "LOCAL-PORT-RANGE-START"), defaultInt=-1)
                                remoteOb["LocalPortEnd"] = transition_number(get_element_or_none(connection_prop, "LOCAL-PORT-RANGE-END"), defaultInt=-1)
                                isAdd = True
                                for item in remoteList:
                                    if item == remoteOb:
                                        isAdd = False
                                        break
                                if isAdd:
                                    remoteList.append(remoteOb)


    def get_iam_for_idsm(self, ar_machine):
        grants = []
        security_event_mapping = self.model.find_elements_of_type("SECURITY-EVENT-MAPPING")
        for item in security_event_mapping:
            process_fqn = get_element_or_none(item, "PROCESS-REF")
            if self.is_machine_process(ar_machine, process_fqn):
                grant = None
                for grantItem in grants:
                    if process_fqn == grantItem["processname"]:
                        grant = grantItem
                        break
                if grant is None:
                    grant = {
                        "processname": process_fqn,
                        "secureeventid": []
                    }
                    grants.append(grant)
                port_ref = get_element_or_none(item, "REPORTING-PORT-PROTOTYPE-IREF/TARGET-R-PORT-PROTOTYPE-REF")
                if port_ref is not None:
                    definition_mapping = self.model.find_elements_of_type("SECURITY-EVENT-REPORT-TO-SECURITY-EVENT-DEFINITION-MAPPING")
                    for definition_item in definition_mapping:
                        new_port_ref = get_element_or_none(definition_item, "REPORTED-SECURITY-EVENT-IREF/TARGET-R-PORT-REF")
                        if new_port_ref is not None and new_port_ref.text == port_ref.text:
                            definition_ref = get_element_or_none(definition_item, "SECURITY-EVENT-DEFINITION-REF")
                            if definition_ref is not None:
                                event_definition = self.model.find_referable(definition_ref.text)
                                eventId = get_element_or_none(event_definition, "ID")
                                if eventId is not None and eventId not in grant["secureeventid"]:
                                    grant["secureeventid"].append(eventId)
        return {
            "IdsmGrants": grants
        }

    def  get_iam_for_phm_infos(self, ar_machine):
        phmCheckPointGrantsList = []
        phmChannelGrantsList = []
        processes = self.get_machine_processes(ar_machine)
        for process in processes:
            processFqn = process.get_fqn()
            checkPointList = []
            channelIdList = []
            management_contribution_mapping = self.model.find_elements_of_type("PLATFORM-HEALTH-MANAGEMENT-CONTRIBUTION")
            for management_contribution in management_contribution_mapping:
                supervision_checkpoint_mapping = management_contribution.find_elements_of_type("SUPERVISION-CHECKPOINT")
                for supervision_checkpoint in supervision_checkpoint_mapping:
                    processName = get_element_or_none(supervision_checkpoint, "PROCESS-REF")
                    if processName is not None and processName.text == processFqn:
                        checkpointIdP = transition_number(get_element_or_none(supervision_checkpoint, "CHECKPOINT-ID"), defaultInt=None)
                        checkpoint_ref = get_element_or_none(supervision_checkpoint, "PHM-CHECKPOINT-IREF/TARGET-PHM-CHECKPOINT-REF")
                        if checkpoint_ref is not None:
                            ar_checkpoint = self.model.find_referable(checkpoint_ref.text)
                            checkpointId = transition_number(get_element_or_none(ar_checkpoint, "CHECKPOINT-ID"), defaultInt=None)
                            if checkpointIdP is not None and checkpointId is not None and checkpointIdP != checkpointId:
                                assert False, "[PHM ] CODE-001: SupervisionCheckpoint[{}].checkpointId and PhmSupervisedEntityInterface.PhmCheckpoint[{}].checkpointId should be configured the same. LocalPath: {}:{}. ".format(supervision_checkpoint.get_fqn(), ar_checkpoint.get_fqn(), supervision_checkpoint.get_path(), ar_checkpoint.get_path())
                            if checkpointId is not None and checkpointId not in checkPointList:
                                checkPointList.append(checkpointId)
                            elif checkpointIdP is not None and checkpointIdP not in checkPointList:
                                checkPointList.append(checkpointIdP)

                external_status_mapping = management_contribution.find_elements_of_type("HEALTH-CHANNEL-EXTERNAL-STATUS")
                for external_status in external_status_mapping:
                    processName = get_element_or_none(external_status, "PROCESS-REF")
                    if processName is not None and processName.text == processFqn:
                        reported_status_list = external_status.find_elements_of_type("HEALTH-CHANNEL-EXTERNAL-REPORTED-STATUS")
                        for item in reported_status_list:
                            statusId = transition_number(get_element_or_none(item, "STATUS-ID"), defaultInt=None)
                            statusRef = get_element_or_none(item, "STATUS-REF")
                            if statusRef is not None:
                                statusEle = self.model.find_referable(statusRef.text)
                                # notification = get_element_or_none(statusEle, "TRIGGERS-RECOVERY-NOTIFICATION")
                                # if notification is not None and notification:
                                interfaceStatusId = transition_number(get_element_or_none(statusEle, "STATUS-ID"), defaultInt=None)
                                if statusId is not None and interfaceStatusId is not None and statusId != interfaceStatusId:
                                    assert False, "[PHM ] CODE-002: PhmHealthChannelStatus[{}].statusId and HealthChannelExternalReportedStatus[{}].statusId should be configured the same. LocalPath: {}:{}. ".format(statusEle.get_fqn(), item.get_fqn(), statusEle.get_path(), item.get_path())
                                if interfaceStatusId is not None and interfaceStatusId not in channelIdList:
                                    channelIdList.append(interfaceStatusId)
                                elif statusId is not None and statusId not in channelIdList:
                                    channelIdList.append(statusId)
            if len(checkPointList) > 0:
                phmCheckPointGrantsList.append({
                    "processname": processFqn,
                    "checkpointid": checkPointList
                })
            if len(channelIdList) > 0:
                phmChannelGrantsList.append({
                    "processname": processFqn,
                    "channelid": channelIdList
                })

        return {"PhmCheckPointGrants": phmCheckPointGrantsList,
                "PhmChannelGrants": phmChannelGrantsList}

    def get_iam_for_crypto_infos(self, ar_machine):
        CryptoGrants = []
        instantiation_element = get_element_or_none(ar_machine, "MODULE-INSTANTIATIONS")
        if instantiation_element is not None:
            instantiation_map = instantiation_element.find_elements_of_type("CRYPTO-MODULE-INSTANTIATION")
            for instantiation_item in instantiation_map:
                crypto_provider = get_element_or_none(instantiation_item, "CRYPTO-PROVIDERS")
                if crypto_provider is not None:
                    crypto_provider_map = crypto_provider.find_elements_of_type("CRYPTO-PROVIDER")
                    for crypto_provider_item in crypto_provider_map:
                        key_slots = crypto_provider_item.find_elements_of_type("KEY-SLOTS")
                        for key_slot_item in key_slots:
                            crypto_key_slot_map = key_slot_item.find_elements_of_type("CRYPTO-KEY-SLOT")
                            for crypto_key_slot_item in crypto_key_slot_map:
                                slot_type = get_element_or_none(crypto_key_slot_item, "SLOT-TYPE")
                                if slot_type is not None:
                                    key_slot_prototype = self.model.find_elements_of_type("CRYPTO-KEY-SLOT-TO-PORT-PROTOTYPE-MAPPING")
                                    for key_slot_prototype_item in key_slot_prototype:
                                        processFqn = get_element_or_none(key_slot_prototype_item, "PROCESS-REF")
                                        key_slot_ref2 = get_element_or_none(key_slot_prototype_item, "KEY-SLOT-REF")
                                        if key_slot_ref2 is not None and key_slot_ref2.text == crypto_key_slot_item.get_fqn() and self.is_machine_process(ar_machine, processFqn):
                                            CryptoGrantItem = None
                                            is_change = False
                                            for CryptoGrant in CryptoGrants:
                                                if CryptoGrant["processname"] == processFqn:
                                                    CryptoGrantItem = CryptoGrant
                                                    is_change = True
                                                    break
                                            if CryptoGrantItem is not None and "slotname" in CryptoGrantItem:
                                                CryptoGrantItem["slotname"].append(key_slot_ref2.text[1:])
                                            else:
                                                CryptoGrantItem = {"processname": processFqn, "slotname": [key_slot_ref2.text[1:]]}
                                            if not is_change:
                                                CryptoGrants.append(CryptoGrantItem)

        return {
            "CryptoGrants": CryptoGrants
        }

    def is_machine_process(self, ar_machine, processFqn):
        if processFqn is None:
            return False
        proc_to_machine_mappings = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING")
        for mapping in proc_to_machine_mappings:
            ar_machine_ref = get_element_or_none(mapping, "MACHINE-REF")
            if ar_machine_ref is not None and ar_machine_ref.text == ar_machine.get_fqn():
                ar_process = self.model.find_referable(mapping.PROCESS_REF)
                if processFqn == ar_process.get_fqn():
                    return True
        return False

    def get_machine_processes(self, ar_machine):
        ar_processes = []
        proc_to_machine_mappings = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING")
        for mapping in proc_to_machine_mappings:
            ar_machine_ref = get_element_or_none(mapping, "MACHINE-REF")
            if ar_machine_ref is not None and ar_machine_ref.text == ar_machine.get_fqn():
                ar_process = self.model.find_referable(mapping.PROCESS_REF)
                ar_processes.append(ar_process)

        return ar_processes
