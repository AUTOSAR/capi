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
Network binding class.
"""

# pylint: disable=too-many-lines,too-many-public-methods
import logging

from unicodedata import category

from generator.intermediate_model.communication_management.ara_com_deployment.\
    someip.someip_service_deployment import SomeIpServiceDeployment  # noqa
from generator.intermediate_model.communication_management.ara_com_deployment.\
    user_defined.user_defined_service_deployment import UserDefinedServiceDeployment  # noqa
from generator.intermediate_model.communication_management.ara_com_deployment.dds.\
    dds_service_deployment import DdsServiceDeployment  # noqa
from generator.parser import commen_util
from generator.parser.commen_util import get_instance_specifier
from generator.parser.someip_svc_deployment_builder import SomeIpServiceDeploymentBuilder
from generator.parser.dds_deployment_builder import DdsServiceDeploymentBuilder
from generator.parser.user_defined_svc_deployment_builder import UserDefinedServiceDeploymentBuilder

from generator.common.tree_helper import get_element_or_die, get_element_or_none, get_element_or_0, \
    get_element_or_str, transition_number, short_name, to_list


def is_instance_mapped_to_machine(mapping, ar_si_fqn):
    instance_refs = mapping.find_elements_of_type('SERVICE-INSTANCE-REF')
    for instance in instance_refs:
        if str(instance) == ar_si_fqn:
            return True
    return False


class NetworkBindingBuilder:
    """
    Handles parsing the ARXML part, related to network binding, and provides service deployment objects.
    """

    def __init__(self, model):
        self._log = logging.getLogger(__name__)
        self.model = model

        self.someip_svc_deployment = SomeIpServiceDeploymentBuilder(self.model)
        self.dds_svc_deployment = DdsServiceDeploymentBuilder(self.model)
        self.user_defined_svc_deployment = UserDefinedServiceDeploymentBuilder(self.model)

    def make_deployment(self, ar_depl, ar_si):
        result = None
        if ar_depl.short_tag() == "SOMEIP-SERVICE-INTERFACE-DEPLOYMENT":
            result = self._make_someip_deployment(ar_depl, ar_si)
        if ar_depl.short_tag() == "DDS-SERVICE-INTERFACE-DEPLOYMENT":
            result = self._make_dds_deployment(ar_depl, ar_si)
        if ar_depl.short_tag() == "USER-DEFINED-SERVICE-INTERFACE-DEPLOYMENT":
            result = self._make_user_defined_deployment(ar_depl, ar_si)
        return result

    def make_deployment_only(self, ar_depl, isRequired):
        result = None
        if ar_depl.short_tag() == "SOMEIP-SERVICE-INTERFACE-DEPLOYMENT":
            result = self._make_someip_deployment_only(ar_depl, isRequired)
        if ar_depl.short_tag() == "DDS-SERVICE-INTERFACE-DEPLOYMENT":
            result = self._make_dds_deployment_only(ar_depl, isRequired)
        if ar_depl.short_tag() == "USER-DEFINED-SERVICE-INTERFACE-DEPLOYMENT":
            result = self._make_user_defined_deployment_only(ar_depl, isRequired)
        return result

    def _get_si_to_machine_mappings(self, ar_si_fqn):
        si_to_machine_mappings = self.model.find_elements_of_type(
            'SOMEIP-SERVICE-INSTANCE-TO-MACHINE-MAPPING',
            accept=lambda e: is_instance_mapped_to_machine(e, ar_si_fqn))
        if not si_to_machine_mappings:
            si_to_machine_mappings = self.model.find_elements_of_type(
                'DDS-SERVICE-INSTANCE-TO-MACHINE-MAPPING',
                accept=lambda e: is_instance_mapped_to_machine(e, ar_si_fqn))
        if not si_to_machine_mappings:
            si_to_machine_mappings = self.model.find_elements_of_type(
                'USER-DEFINED-SERVICE-INSTANCE-TO-MACHINE-MAPPING',
                accept=lambda e: is_instance_mapped_to_machine(e, ar_si_fqn))
        assert len(si_to_machine_mappings) == 1, "[CM  ] CODE-012: ServiceInstance[{}] needs to be mapped to a specific machine. ".format(ar_si_fqn)
        return si_to_machine_mappings[0]

    def _get_service_instance_fqn(self, ar_endpoint_mapping):
        si_fqn = get_element_or_die(ar_endpoint_mapping, "SERVICE-INSTANCE-REF").text
        return si_fqn

    def _get_si_mappings(self, ar_si_to_app_endpoint_mappings):
        tags_for_provided_si = [
            "PROVIDED-SOMEIP-SERVICE-INSTANCE",
            "DDS-PROVIDED-SERVICE-INSTANCE",
            "PROVIDED-USER-DEFINED-SERVICE-INSTANCE"
        ]
        tags_for_required_si = [
            "REQUIRED-SOMEIP-SERVICE-INSTANCE",
            "DDS-REQUIRED-SERVICE-INSTANCE",
            "REQUIRED-USER-DEFINED-SERVICE-INSTANCE"
        ]

        result = {"provided": [], "required": []}
        for endpoint_mapping in ar_si_to_app_endpoint_mappings:
            si_ref = get_element_or_none(endpoint_mapping, "SERVICE-INSTANCE-REF")
            if si_ref is not None:
                ar_si = self.model.find_referable(si_ref.text)
                deployment_ref = get_element_or_none(ar_si, "SERVICE-INTERFACE-DEPLOYMENT-REF")
                if deployment_ref is not None:
                    si_ref_destination = si_ref.get("DEST")
                    mapping = {
                        "si_fqn": self._get_service_instance_fqn(endpoint_mapping),
                        "instance_specifier": get_instance_specifier(self, endpoint_mapping)
                    }
                    netPort = None
                    portRef = get_element_or_none(endpoint_mapping, "PORT-PROTOTYPE-IREF/TARGET-PORT-PROTOTYPE-REF")
                    if portRef is not None:
                        portName = short_name(portRef.text)
                        if portName.startswith("NetworkState_"):
                            netPort = portName.replace("NetworkState_", "")
                    if si_ref_destination in tags_for_provided_si:
                        if netPort is not None:
                            instanceId = get_element_or_none(ar_si, "SERVICE-INSTANCE-ID")
                            if instanceId is not None:
                                mapping["instance_id"] = "NSOMEIP:{}".format(instanceId)
                                mapping["netPort"] = netPort
                        result["provided"].append(mapping)
                    if si_ref_destination in tags_for_required_si:
                        if netPort is not None:
                            instanceId = get_element_or_none(ar_si, "REQUIRED-SERVICE-INSTANCE-ID")
                            if instanceId is not None:
                                mapping["instance_id"] = "NSOMEIP:{}".format(instanceId)
                                mapping["netPort"] = netPort
                        result["required"].append(mapping)

        return result

    def get_si_mappings_from_ar_process(self, ar_process):
        ar_si_to_app_endpoint_mappings = self.model.find_elements_of_type('SERVICE-INSTANCE-TO-PORT-PROTOTYPE-MAPPING', accept=lambda e: str(e.PROCESS_REF) == ar_process.get_fqn())
        return self._get_si_mappings(ar_si_to_app_endpoint_mappings)

    def _make_someip_deployment(self, ar_deployment, ar_si):
        isP = True if str(ar_si.short_tag()).__contains__("PROVIDED") else False
        ports_config = {}
        if ar_si is not None:
            if str(ar_si.short_tag()).endswith("SOMEIP-SERVICE-INSTANCE"):
                ports_config = self.someip_svc_deployment.get_net_ports_config(self._get_si_to_machine_mappings(ar_si.get_fqn()))
        if isP:
            si_deployment = self.someip_svc_deployment.get_provided_si_deployment(ar_si)
            deployment = self.someip_svc_deployment.get_instance_deployment(ar_deployment, ar_si, ports_config, isPR="P")
            instanceId = si_deployment["instance_id"]
            interfaceId = transition_number(deployment["service_id"])
            checkId = str(instanceId) + "-" + str(interfaceId)
            if checkId not in self.someip_svc_deployment.uniqueCheckOb:
                self.someip_svc_deployment.uniqueCheckOb[checkId] = ar_si.get_fqn()
            else:
                assert self.someip_svc_deployment.uniqueCheckOb[checkId] == ar_si.get_fqn(), "[CM  ] CODE-018: ProvidedSomeipServiceInstance[{}, {}] shall be unique in respect of serviceInstanceId, serviceInterfaceId. LocalPath:{}. ".format(ar_si.get_fqn(), self.someip_svc_deployment.uniqueCheckOb[checkId], ar_si.get_path())
        else:
            si_deployment = self.someip_svc_deployment.get_required_si_deployment(ar_si)
            deployment = self.someip_svc_deployment.get_instance_deployment(ar_deployment, ar_si, ports_config, isPR="R")
            if "unICastConfigList" in ports_config and len(ports_config["unICastConfigList"]) > 0:
                unICastConfig = ports_config["unICastConfigList"][0]
                ip = None
                if "ipV4" in unICastConfig:
                    ip = unICastConfig["ipV4"]
                elif "ipV6" in unICastConfig:
                    ip = unICastConfig["ipV6"]
                if "tcp" in unICastConfig and ip is not None:
                    ports_config["unicastTcp"] = unICastConfig["tcp"]
                    ports_config["unicastIp"] = ip
                    ports_config["unicastTcpIp"] = "{}:{}".format(ip, unICastConfig["tcp"])
                if "udp" in unICastConfig and ip is not None:
                    ports_config["unicastUdp"] = unICastConfig["udp"]
                    ports_config["unicastIp"] = ip
                    ports_config["unicastUdpIp"] = "{}:{}".format(ip, unICastConfig["udp"])
                ports_config["strategy"] = unICastConfig["strategy"]
        deployment_config = {**si_deployment, **deployment}
        service_deployment = SomeIpServiceDeployment()
        service_deployment.ports = ports_config
        service_deployment.deployment_id = deployment_config["service_id"]
        service_deployment.instance_id = deployment_config["instance_id"]
        service_deployment.sd_config = deployment_config["sd_config"]
        service_deployment.major_version = deployment_config["service_major_version"]
        service_deployment.minor_version = deployment_config["service_minor_version"]
        service_deployment.load_balancing_priority = deployment_config["load_balancing_priority"]
        service_deployment.load_balancing_weight = deployment_config["load_balancing_weight"]
        service_deployment.have_event_notifier = deployment_config["have_event_notifier"]
        service_deployment.have_method_get_set = deployment_config["have_method_get_set"]
        service_deployment.is_show = deployment_config["is_show"]
        service_deployment.name = ar_deployment.SHORT_NAME
        service_deployment.deployment_fqn = ar_deployment.get_fqn()
        service_deployment.standard_name = service_deployment.name.lower()
        if isP:
            service_deployment.provided = deployment_config
        else:
            service_deployment.version_driven_find_behavior = deployment_config["version_driven_find_behavior_"]
            service_deployment.required_minor_version = deployment_config["required_minimum_minor_version"]
            service_deployment.required = deployment_config
            service_deployment.bindPorts = self.get_required_bind_info(ar_deployment.get_fqn())
        return service_deployment

    def get_required_bind_info(self, deploymentFqn):
        ar_provided_someip_service_instance_services = self.model.find_elements_of_type("PROVIDED-SOMEIP-SERVICE-INSTANCE")
        for ar_provided_someip_service_instance_service in ar_provided_someip_service_instance_services:
            service_interface_deployment_ref = get_element_or_str(ar_provided_someip_service_instance_service,"SERVICE-INTERFACE-DEPLOYMENT-REF")
            if service_interface_deployment_ref.text == deploymentFqn:
                provided_someip_service_instance_fqn = ar_provided_someip_service_instance_service.get_fqn()
                si_to_machine_mappings = self.model.find_elements_of_type("SOMEIP-SERVICE-INSTANCE-TO-MACHINE-MAPPING")
                for si_to_machine_mapping in si_to_machine_mappings:
                    serviceInstanceRefs = to_list(get_element_or_none(si_to_machine_mapping, 'SERVICE-INSTANCE-REFS/SERVICE-INSTANCE-REF'))
                    for serviceInstanceRef in serviceInstanceRefs:
                        if serviceInstanceRef.text == provided_someip_service_instance_fqn:
                            ports_config = {}
                            ar_tcp_port = transition_number(get_element_or_none(si_to_machine_mapping, "TCP-PORT"),defaultInt=None)
                            ar_udp_port = transition_number(get_element_or_none(si_to_machine_mapping, "UDP-PORT"),defaultInt=None)
                            if ar_tcp_port is not None:
                                ports_config["tcp"] = ar_tcp_port
                            if ar_udp_port is not None:
                                ports_config["udp"] = ar_udp_port
                            connectorRef = get_element_or_none(si_to_machine_mapping, "COMMUNICATION-CONNECTOR-REF")
                            if connectorRef is not None:
                                connectorEle = self.model.find_referable(connectorRef.text)
                                unicastEndpointRef = get_element_or_none(connectorEle, "UNICAST-NETWORK-ENDPOINT-REF")
                                if unicastEndpointRef is not None:
                                    endpointEle = self.model.find_referable(unicastEndpointRef.text)
                                    ipAddress = commen_util.get_network_address(endpointEle)
                                    ports_config["ipAddress"] = ipAddress
                            return ports_config
        return {}

    def _make_dds_deployment(self, ar_deployment, ar_si):
        isP = True if str(ar_si.short_tag()).__contains__("PROVIDED") else False
        deployment = self.dds_svc_deployment.get_instance_deployment(ar_deployment)
        if isP:
            si_deployment = self.dds_svc_deployment.get_provided_si_deployment(ar_si)
        else:
            si_deployment = self.dds_svc_deployment.get_required_si_deployment(ar_si)
        deployment_config = {**si_deployment, **deployment}
        service_deployment = DdsServiceDeployment()
        service_deployment.deployment_id = deployment_config["service_id"]
        service_deployment.instance_id = deployment_config["instance_id"]
        service_deployment.domain_id = deployment_config["domain_id"]
        service_deployment.qos_profile = deployment_config["qos_profile"]
        service_deployment.at_key = deployment_config["atKey"]
        for plugin in deployment_config["transport_plugins"]:
            service_deployment.add_transport_plugin(plugin)
        service_deployment.name = ar_deployment.SHORT_NAME
        service_deployment.deployment_fqn = ar_deployment.get_fqn()
        service_deployment.standard_name = service_deployment.name.lower()
        if isP:
            service_deployment.provided = deployment_config
        else:
            service_deployment.required = deployment_config
        return service_deployment

    def _make_user_defined_deployment(self, ar_deployment, ar_si):
        isP = True if str(ar_si.short_tag()).__contains__("PROVIDED") else False
        ports_config = self.someip_svc_deployment.get_net_ports_config(self._get_si_to_machine_mappings(ar_si.get_fqn()))
        deployment = self.user_defined_svc_deployment.get_instance_deployment(ar_deployment)
        if deployment["depType"] == "icc":
            si_deployment = self.user_defined_svc_deployment.get_icc_user_deployment(ar_si, isP)
        else:
            if isP:
                si_deployment = self.user_defined_svc_deployment.get_provided_user_deployment(ar_si, deployment)
            else:
                si_deployment = self.user_defined_svc_deployment.get_required_user_deployment(ar_si)
        deployment_config = {**si_deployment, **deployment}
        service_deployment = UserDefinedServiceDeployment()
        service_deployment.depType = deployment_config["depType"]
        service_deployment.deployment_fqn = ar_deployment.get_fqn()
        service_deployment.ports = ports_config
        service_deployment.instance_id = deployment_config["instance_id"]
        if service_deployment.depType == "icc":
            service_deployment.deployment_id = deployment_config["iccService"]["serviceId"]
            service_deployment.major_version = deployment_config["iccService"]["major"]
            service_deployment.minor_version = deployment_config["iccService"]["minor"]
            service_deployment.iccService = deployment_config["iccService"]
            service_deployment.iccOb = deployment_config["iccOb"]
        else:
            service_deployment.deployment_id = deployment_config["ipcService"]["serviceId"]
            service_deployment.major_version = deployment_config["ipcService"]["major"]
            service_deployment.minor_version = deployment_config["ipcService"]["minor"]
            service_deployment.ipcService = deployment_config["ipcService"]
            service_deployment.ipcShm = deployment_config["ipcShm"]
        service_deployment.name = ar_deployment.SHORT_NAME
        service_deployment.standard_name = service_deployment.name.lower()
        if isP:
            service_deployment.provided = deployment_config
        else:
            service_deployment.required = deployment_config
        return service_deployment

    def _make_someip_deployment_only(self, ar_deployment, isRequired):
        deployment = self.someip_svc_deployment.get_instance_deployment(ar_deployment)
        if isRequired:
            serviceIn = self.model.find_elements_of_type("REQUIRED-SOMEIP-SERVICE-INSTANCE")
        else:
            serviceIn = self.model.find_elements_of_type("PROVIDED-SOMEIP-SERVICE-INSTANCE")
        isShow = True
        for item in serviceIn:
            ref = get_element_or_none(item, "SERVICE-INTERFACE-DEPLOYMENT-REF")
            if ref is not None and ref.text == ar_deployment.get_fqn():
                sdg_caption = get_element_or_none(item, "ADMIN-DATA/SDGS/SDG/SDG-CAPTION")
                if sdg_caption is not None:
                    short_name_sdg = get_element_or_none(sdg_caption, "SHORT-NAME")
                    if short_name_sdg is not None and "DiagnosticInterfaceDeployment" == short_name_sdg:
                        isShow = False
                        break
        service_deployment = SomeIpServiceDeployment()
        service_deployment.deployment_id = deployment["service_id"]
        service_deployment.major_version = deployment["service_major_version"]
        service_deployment.minor_version = deployment["service_minor_version"]
        if isRequired:
            service_deployment.required = deployment
        else:
            service_deployment.provided = deployment
        service_deployment.is_show = isShow
        service_deployment.name = ar_deployment.SHORT_NAME
        service_deployment.deployment_fqn = ar_deployment.get_fqn()
        service_deployment.standard_name = service_deployment.name.lower()

        return service_deployment

    def _make_dds_deployment_only(self, ar_deployment, isRequired):
        deployment = self.dds_svc_deployment.get_instance_deployment(ar_deployment)
        service_deployment = DdsServiceDeployment()
        service_deployment.deployment_id = deployment["service_id"]
        service_deployment.at_key = deployment["atKey"]
        if isRequired:
            service_deployment.required = deployment
        else:
            service_deployment.provided = deployment
        service_deployment.name = ar_deployment.SHORT_NAME
        service_deployment.deployment_fqn = ar_deployment.get_fqn()
        service_deployment.standard_name = service_deployment.name.lower()
        return service_deployment

    def _make_user_defined_deployment_only(self, ar_deployment, isRequired):
        deployment = self.user_defined_svc_deployment.get_instance_deployment(ar_deployment)
        service_deployment = UserDefinedServiceDeployment()
        service_deployment.deployment_fqn = ar_deployment.get_fqn()
        service_deployment.depType = deployment["depType"]
        if service_deployment.depType == "icc":
            service_deployment.deployment_id = deployment["iccService"]["serviceId"]
            service_deployment.iccService = deployment["iccService"]
        else:
            service_deployment.deployment_id = deployment["ipcService"]["serviceId"]
            service_deployment.major_version = deployment["ipcService"]["major"]
            service_deployment.minor_version = deployment["ipcService"]["minor"]
            service_deployment.ipcService = deployment["ipcService"]
        if isRequired:
            service_deployment.required = deployment
        else:
            service_deployment.provided = deployment
        service_deployment.name = ar_deployment.SHORT_NAME
        service_deployment.deployment_fqn = ar_deployment.get_fqn()
        service_deployment.standard_name = service_deployment.name.lower()
        return service_deployment

    def populate_deployment(self, service, deployment):
        """Populate service with deployment information"""
        if type(deployment) is SomeIpServiceDeployment:
            self.someip_svc_deployment.populate_deployment(service, deployment)
        elif type(deployment) is UserDefinedServiceDeployment:
            self.user_defined_svc_deployment.populate_deployment(service, deployment)
        elif type(deployment) is DdsServiceDeployment:
            self.dds_svc_deployment.populate_deployment(service, deployment)

