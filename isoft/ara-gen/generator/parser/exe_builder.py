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

from generator.intermediate_model import Executable, Service, Component
from generator.intermediate_model.communication_management.ara_com_deployment.dds.dds_service_deployment import DdsServiceDeployment
from generator.intermediate_model.communication_management.ara_com_deployment.someip.someip_service_deployment import SomeIpServiceDeployment
from generator.intermediate_model.persistency.kvs_interface import KvsInterface
from generator.intermediate_model.phm.channelstatus import ChannelStatus
from generator.intermediate_model.phm.checkpoint import CheckPoint
from generator.intermediate_model.phm.hcrnotificalinterface import HCRNotificalInterface
from generator.intermediate_model.phm.healthchannelinterface import HealthChannelInterface
from generator.intermediate_model.phm.sernotificalinterface import SERNotificalInterface
from generator.intermediate_model.phm.supervisedentityinterface import SupervisedEntityInterface
from generator.parser.signal_builder import SignalBuilder
from generator.common.tree_helper import get_element_or_str, get_element_or_none, short_name, get_element_or_die, get_element_or_0, to_list, transition_number,to_str
from generator.views.component_view import ComponentView

_APPL_TYPE_TREF = "APPLICATION-TYPE-TREF"
_ADAPTIVE_APPL_TYPE = "ADAPTIVE-APPLICATION-SW-COMPONENT-TYPE"
_COMPOSITION_SW_COMPONENT_TYPE = "COMPOSITION-SW-COMPONENT-TYPE"


def _is_true(text):
    """Check if text equals true, case insensitive"""
    return text is not None and str(text).strip().lower() == 'true'


class ExecutableBuilder:
    def __init__(self, model, _diagnosisBuilder, _interface_builder, _network_binding_builder, _phmdeploymentbuilder):
        self._log = logging.getLogger(__name__)
        self.model = model
        self._diagnosisBuilder = _diagnosisBuilder
        self._interface_builder = _interface_builder
        self._network_binding_builder = _network_binding_builder
        self._phmdeploymentbuilder = _phmdeploymentbuilder

    def get_executables(self, executables):  # Generate c ++
        if len(executables) == 1:
            ar_executable = self.model.find_referable(executables[0])
            exeversion = to_str(get_element_or_none(ar_executable, "VERSION"))
            build_type = get_element_or_none(ar_executable, "BUILD-TYPE")
            exeBuild = "Debug"
            if build_type is not None and "BUILD-TYPE-RELEASE" == build_type.text:
                exeBuild = "Release"
            executable = Executable(ar_executable.SHORT_NAME, ar_executable.SHORT_NAME, exeversion, fqn=ar_executable.get_fqn())
            executable.set_swcl_info(self._get_exe_swcl_info(ar_executable.get_fqn()))
            executable.set_build_type(exeBuild)
            exe_swc = self._find_exe_swc(executables)
            for component in self.get_create_c_components(exe_swc):
                executable.add_component(component)

            serviceList = self.get_exe_service_list(executables)
            executable.setserviceall({
                "rServiceList": serviceList[0],
                "pServiceList": serviceList[1]
            })
            executable.dltMessageList = self.get_dlt_message(executables)
            return executable

    def get_build_info(self, executables):
        exe = {
            "dltMessage": self.get_dlt_message(executables)
        }
        serviceList = self.get_exe_service_list(executables, True)
        exe["serviceAll"] = {
            "rServiceList": serviceList[0],
            "pServiceList": serviceList[1]
        }
        exe_swc = self._find_exe_swc(executables)
        fwInterfaceList = self._get_fw_interface(executables)
        componentViewList = self.get_component_view(exe_swc, serviceList)
        return {
            "exe": exe,
            "fwInterfaceList": fwInterfaceList,
            "components": componentViewList
        }

    def get_exe_service_list(self, executables, isBuild=False):
        portList = self.get_exe_p_r_port(executables)

        deploymentMapping = self.model.find_elements_of_type("SOMEIP-SERVICE-INTERFACE-DEPLOYMENT") + \
                            self.model.find_elements_of_type("DDS-SERVICE-INTERFACE-DEPLOYMENT") + \
                            self.model.find_elements_of_type("USER-DEFINED-SERVICE-INTERFACE-DEPLOYMENT")
        rServiceList = []
        pServiceList = []
        for deploymentE in deploymentMapping:
            interface_ref = get_element_or_none(deploymentE, "SERVICE-INTERFACE-REF")
            if interface_ref is not None:
                if interface_ref.text in portList[0]:
                    ar_interface = self.model.find_referable(interface_ref.text)
                    service = Service(ar_interface.get_fqn())
                    deployment = self._network_binding_builder.make_deployment_only(deploymentE, False)
                    needVerifyTlv = not isinstance(deployment, DdsServiceDeployment)
                    if isBuild:
                        self._interface_builder.populate_interface(ar_interface, service, portList[0][interface_ref.text], needVerifyTlv=needVerifyTlv)
                    else:
                        self._interface_builder.populate_interface(ar_interface, service, needVerifyTlv=needVerifyTlv)
                    self._network_binding_builder.populate_deployment(service, deployment)
                    service.service_deployment = deployment
                    service.is_show = deployment.is_show
                    if isBuild:
                        self.set_service_signal_builder(service, deployment)
                    pServiceList.append(service)
                if interface_ref.text in portList[1]:
                    ar_interface = self.model.find_referable(interface_ref.text)
                    service = Service(ar_interface.get_fqn())
                    deployment = self._network_binding_builder.make_deployment_only(deploymentE, True)
                    needVerifyTlv = not isinstance(deployment, DdsServiceDeployment)
                    if isBuild:
                        self._interface_builder.populate_interface(ar_interface, service, portList[1][interface_ref.text], needVerifyTlv=needVerifyTlv)
                    else:
                        self._interface_builder.populate_interface(ar_interface, service, needVerifyTlv=needVerifyTlv)
                    self._network_binding_builder.populate_deployment(service, deployment)
                    service.service_deployment = deployment
                    service.is_show = deployment.is_show
                    if isBuild:
                        self.set_service_signal_builder(service, deployment)
                    rServiceList.append(service)
        return [rServiceList, pServiceList]

    def set_service_signal_builder(self, service, deployment):
        if type(deployment) is SomeIpServiceDeployment:
            signalbuilder = SignalBuilder(self.model)
            for signal in signalbuilder.get_signals_byservicedeploymentid(service.service_deployment.deployment_id):
                service.add_signal(signal)
            for etosmapping in signalbuilder.get_etosmappings_byservice(service.fqn):
                service.add_etosmapping(etosmapping)
            for stoemapping in signalbuilder.get_stoemappings_byservice(service.fqn):
                service.add_stoemapping(stoemapping)
            service.adjust_signaluserevent()

    def get_exe_p_r_port(self, executables):
        pInterface = {}
        rInterface = {}
        exe_swc = self._find_exe_swc(executables)
        for swc in exe_swc:
            swcE = self.model.find_referable(swc)
            if swcE is not None:
                ports = get_element_or_none(swcE, "PORTS")
                if ports is not None:
                    pProt = ports.find_elements_of_type("P-PORT-PROTOTYPE")
                    for item in pProt:
                        interface_ref = get_element_or_none(item, "PROVIDED-INTERFACE-TREF")
                        if interface_ref is not None and interface_ref.text not in pInterface:
                            pInterface[interface_ref.text] = item
                    rProt = ports.find_elements_of_type("R-PORT-PROTOTYPE")
                    for item in rProt:
                        interface_ref = get_element_or_none(item, "REQUIRED-INTERFACE-TREF")
                        if interface_ref is not None and interface_ref.text not in rInterface:
                            rInterface[interface_ref.text] = item
        return [pInterface, rInterface]

    def get_dlt_message(self, executables):
        dltLogChannelDesignList = []
        for executableItem in executables:
            processDesignMapping = self.model.find_elements_of_type("PROCESS-DESIGN")
            for processDesign in processDesignMapping:
                exeRef = get_element_or_none(processDesign, "EXECUTABLE-REF")
                if exeRef is not None and exeRef.text == executableItem:
                    mapping = self.model.find_elements_of_type("DLT-LOG-CHANNEL-DESIGN-TO-PROCESS-DESIGN-MAPPING")
                    for item in mapping:
                        pdr = get_element_or_none(item, "PROCESS-DESIGN-REF")
                        if pdr is not None and pdr.text == processDesign.get_fqn():
                            cdr = get_element_or_none(item, "DLT-LOG-CHANNEL-DESIGN-REF")
                            if cdr is not None:
                                dltLogChannelDesignList.append(cdr.text)
        dltMessageList = []
        dltMessageNameList = []
        for item in dltLogChannelDesignList:
            dltLogChannelDesign = self.model.find_referable(item)
            dltMessageMapping = get_element_or_none(dltLogChannelDesign, "DLT-MESSAGE-REFS/DLT-MESSAGE-REF")
            if dltMessageMapping is not None:
                dltMessageMapping = to_list(dltMessageMapping)
                for dltMessage in dltMessageMapping:
                    dltMessageSet = "/".join(str(dltMessage).split("/")[:-1])
                    dltMessageSetEle = self.model.find_referable(dltMessageSet)
                    sd = get_element_or_none(dltMessageSetEle, "ADMIN-DATA/SDGS/SDG/SD")
                    if sd is not None and sd.attrib["GID"] == "iSOFT:log:DltMessageNamespace:Namespace":
                        namespaces = str(sd.text + dltMessageSetEle.SHORT_NAME)
                        nameOb = None
                        for nameItem in dltMessageNameList:
                            if nameItem["namespaces"] == namespaces:
                                nameOb = nameItem
                                break
                        if nameOb is None:
                            nameOb = {
                                "namespaces": namespaces,
                                "namespaceList": namespaces.split("::"),
                                "dltMessageList": []
                            }
                            dltMessageNameList.append(nameOb)
                        self.get_dlt_message_list(nameOb["dltMessageList"], dltMessage)
                    else:
                        self.get_dlt_message_list(dltMessageList, dltMessage)

        dltMessageOb = {
            "dltMessageName": dltMessageNameList,
            "dltMessageList": dltMessageList,
            "isHaveLog": len(dltMessageList) > 0 or len(dltMessageNameList) > 0
        }

        return dltMessageOb



    def get_dlt_message_list(self, dltMessageList, dltMessage):
        dltMessageEle = self.model.find_referable(dltMessage.text)
        dltMessageOb = {
            "name": short_name(dltMessage.text),
            "messageId": get_element_or_die(dltMessageEle, "MESSAGE-ID"),
            "typeInfo": get_element_or_die(dltMessageEle, "MESSAGE-TYPE-INFO"),
            "lineNumber": get_element_or_0(dltMessageEle, "MESSAGE-LINE-NUMBER"),
            "sourceFile": get_element_or_str(dltMessageEle, "MESSAGE-SOURCE-FILE")
        }
        argumentNum = 0
        dltArgumentMap = dltMessageEle.find_elements_of_type("DLT-ARGUMENT")
        for dltArgument in dltArgumentMap:
            propsConditionalList = dltArgument.find_elements_of_type("SW-DATA-DEF-PROPS-CONDITIONAL")
            for propsConditional in propsConditionalList:
                unitRef = get_element_or_none(propsConditional, "UNIT-REF")
                if unitRef is not None:
                    argumentNum += 1
                    break
        dltMessageOb["argumentNum"] = argumentNum
        dltMessageList.append(dltMessageOb)

    def get_create_c_components(self, exe_swc):
        ar_components = []
        for component_ref in exe_swc:
            ar_component = self.model.find_referable(str(component_ref))
            ar_components.append(ar_component)
        components = [Component(ar_component.SHORT_NAME, ar_component.get_fqn()) for ar_component in ar_components]
        for ar_component, component in zip(ar_components, components):
            ar_ports = get_element_or_none(ar_component, "PORTS")
            if ar_ports is None:
                continue
            self.set_component_interface(component, ar_ports)
        return components

    def set_component_interface(self, component, ar_ports):
        for per_interface in self._get_persistency_interfaces(ar_ports):
            component.add_persistency_interface(per_interface)
        pport_map = ar_ports.find_elements_of_type("P-PORT-PROTOTYPE")
        for pport in pport_map:
            ar_pport = get_element_or_none(pport, "PROVIDED-INTERFACE-TREF")
            if ar_pport:
                component.add_interface(ar_pport.attrib["DEST"])
            pport_sd = get_element_or_none(pport, "ADMIN-DATA/SDGS/SDG/SD")
            if pport_sd is not None and pport_sd.attrib["GID"] == "iSOFT:diag:ProvidedInterface:Class":
                component.add_interface(pport_sd.text)
        rport_map = ar_ports.find_elements_of_type("R-PORT-PROTOTYPE")
        for rport in rport_map:
            ar_rport = get_element_or_none(rport, "REQUIRED-INTERFACE-TREF")
            if ar_rport:
                component.add_interface(ar_rport.attrib["DEST"])
            rport_sd = get_element_or_none(rport, "ADMIN-DATA/SDGS/SDG/SD")
            if rport_sd is not None and rport_sd.attrib["GID"] == "iSOFT:diag:RequiredInterface:Class":
                component.add_interface(rport_sd.text)
            sdgList = to_list(get_element_or_none(rport, "ADMIN-DATA/SDGS/SDG/SDG"))
            for sdg in sdgList:
                if sdg.attrib["GID"] == "iSOFT:fw:FirewallStateSwitchInterface:firewallStatus":
                    component.add_interface("FirewallStateSwitchInterface")
                    break

        prport_map = ar_ports.find_elements_of_type("PR-PORT-PROTOTYPE")
        for prport in prport_map:
            ar_prport = get_element_or_none(prport, "PROVIDED-REQUIRED-INTERFACE-TREF")
            if ar_prport:
                component.add_interface(ar_prport.attrib["DEST"])

    def get_component_view(self, components_list, serviceList):
        """Get the adaptive autosar swc(s) belonging to an executable or components specified in a list of
         components names or all if nothing is set"""
        ar_component_refs = self.model.find_elements_of_type(_ADAPTIVE_APPL_TYPE)

        ar_components = []
        for component_ref in ar_component_refs:
            ar_component = self.model.find_referable(str(component_ref))
            if components_list is None or ar_component.get_fqn() in components_list or ar_component.SHORT_NAME in components_list:
                ar_components.append(ar_component)

        components = [Component(ar_component.SHORT_NAME, ar_component.get_fqn()) for ar_component in ar_components]
        for ar_component, component in zip(ar_components, components):
            ar_ports = get_element_or_none(ar_component, "PORTS")
            if ar_ports is None:
                continue
            for provided_service in self._get_provided_services(ar_ports, serviceList[1]):
                component.add_provided_service(provided_service)
            for required_service in self._get_required_services(ar_ports, serviceList[0]):
                component.add_required_service(required_service)
            for phm_se_interface in self._get_phm_se_interfaces(ar_ports):
                component.add_phm_supervisedentity_interface(phm_se_interface)
            for phm_hc_interface in self._get_phm_hc_interfaces(ar_ports):
                component.add_phm_healthchannel_interface(phm_hc_interface)
            # for phm_ser_notifical_interface in self._get_phm_ser_notifical_interface(ar_ports):
            #     component.add_phm_sernotifical_interface(phm_ser_notifical_interface)
            # for phm_hcr_notifical_interface in self._get_phm_hcr_notifical_interface(ar_ports):
            #     component.add_phm_hcrnotifical_interface(phm_hcr_notifical_interface)

            for diag_element in self._diagnosisBuilder.get_diag_element(ar_ports):
                component.add_diag_element(diag_element)
            for diag_identifier in self._diagnosisBuilder.get_diag_identifier(ar_ports):
                component.add_diag_identifier(diag_identifier)
            for diag_routine in self._diagnosisBuilder.get_diag_routine(ar_ports):
                component.add_diag_routine(diag_routine)

            self.set_component_interface(component, ar_ports)
        componentViewList = [ComponentView(component) for component in components]
        return componentViewList

    def _get_exe_swcl_info(self, executablefqn):
        ar_software_clusters = self.model.find_elements_of_type('SOFTWARE-CLUSTER')
        for ar_software_cluster in ar_software_clusters:
            software_design_refs = ar_software_cluster.find_elements_of_type("DESIGN-REF")
            for software_design_ref in software_design_refs:

                ar_software_design = self.model.find_referable(software_design_ref.text)
                processdesign_refs = ar_software_design.find_elements_of_type("CONTAINED-PROCESS-REF")

                for processdesign_ref in processdesign_refs:

                    ar_processdesign = self.model.find_referable(processdesign_ref.text)
                    executable_ref = get_element_or_str(ar_processdesign, "EXECUTABLE-REF")

                    if executablefqn in executable_ref.text:
                        softwareclusterfqn = ar_software_cluster.get_fqn()
                        softwareclusterversion = get_element_or_none(ar_software_cluster, 'VERSION')
                        softwareclustername = softwareclusterfqn.split("/")[-1]
                        swcl_info = {
                            "softwareclustername": softwareclustername
                        }
                        if softwareclusterversion is not None:
                            swcl_info["softwareclusterversion"] = softwareclusterversion.text
                        return swcl_info

        return {"softwareclustername": "Unkown", "softwareclusterversion": "1.0.0"}

    def _get_provided_services(self, ar_ports, serviceList):
        """Extract provided services from ports"""
        ar_p_ports = ar_ports.find_elements_of_type('P-PORT-PROTOTYPE')
        services = list()
        for ar_port in ar_p_ports:
            ar_provided_interface_refs = ar_port.find_elements_of_type('PROVIDED-INTERFACE-TREF', DEST='SERVICE-INTERFACE')
            pService = []
            ar_provided_interface_text = [interface_ref.text for interface_ref in ar_provided_interface_refs]
            for serviceItem in serviceList:
                if serviceItem.fqn in ar_provided_interface_text:
                    pService.append(serviceItem)
            services.extend(pService)
        return services

    def _get_required_services(self, ar_ports, serviceList):
        """ Extract required services from ports"""
        ar_p_ports = ar_ports.find_elements_of_type('R-PORT-PROTOTYPE')
        services = list()
        for ar_port in ar_p_ports:
            ar_required_interface_refs = ar_port.find_elements_of_type('REQUIRED-INTERFACE-TREF', DEST='SERVICE-INTERFACE')
            rServices = []
            ar_required_interface_text = [interface_ref.text for interface_ref in ar_required_interface_refs]
            for serviceItem in serviceList:
                if serviceItem.fqn in ar_required_interface_text:
                    rServices.append(serviceItem)
            services.extend(rServices)
        return services

    def _get_persistency_interfaces(self, ar_ports):
        ar_persistency_ports = ar_ports.find_elements_with_attr("DEST", "PERSISTENCY-KEY-VALUE-STORAGE-INTERFACE")
        interfaces = []
        for ar_persistency_port in ar_persistency_ports:
            ar_per_interface = self.model.find_referable(ar_persistency_port.text)
            interfaces.append(
                KvsInterface(
                    ar_per_interface.get_fqn(),
                    self._get_persistency_types(ar_per_interface)
                )
            )

        return interfaces

    def _get_persistency_types(self, ar_per_interface):
        types = []
        ar_dataelements = get_element_or_none(ar_per_interface, "DATA-ELEMENTS/PERSISTENCY-DATA-ELEMENT")
        ar_dataelements = to_list(ar_dataelements)
        for ar_dataelement in ar_dataelements:
            ar_type_ref = get_element_or_die(ar_dataelement, "TYPE-TREF")
            types.append(self._interface_builder.resolve_type_ref(ar_type_ref, ar_per_interface))
        ar_additional_types = get_element_or_none(ar_per_interface, "DATA-TYPE-FOR-SERIALIZATION-REFS/DATA-TYPE-FOR-SERIALIZATION-REF")
        ar_additional_types = to_list(ar_additional_types)
        for ar_additional_type in ar_additional_types:
            types.append(self._interface_builder.resolve_type_ref(ar_additional_type, ar_per_interface))

        return types

    def _get_phm_se_interfaces(self, ar_ports):
        """ Extract required services from ports"""

        ar_r_ports = ar_ports.find_elements_of_type('R-PORT-PROTOTYPE')
        SupervisedEntityInterfaces = []
        interfacerefs = []
        for ar_port in ar_r_ports:
            ar_required_interface_refs = ar_port.find_elements_of_type(
                'REQUIRED-INTERFACE-TREF',
                DEST='PHM-SUPERVISED-ENTITY-INTERFACE')
            for interface_ref in ar_required_interface_refs:
                if str(interface_ref) not in interfacerefs:
                    interfacerefs.append(str(interface_ref))
        for interfaceref in interfacerefs:
            seinterface = SupervisedEntityInterface(interfaceref)
            ar_seinterface = self.model.find_referable(interfaceref)
            self._phmdeploymentbuilder.populate_interface(ar_seinterface, seinterface)
            for checkpoint in self.get_checkpoints(ar_seinterface):
                seinterface.add_checkpoint(checkpoint)
                seinterface.addidentifier(self.get_se_interface_identifier_bycheckpoint(checkpoint))
            SupervisedEntityInterfaces.append(seinterface)

        return SupervisedEntityInterfaces

    def _get_phm_hc_interfaces(self, ar_ports):
        """ Extract required services from ports"""
        ar_r_ports = ar_ports.find_elements_of_type('R-PORT-PROTOTYPE')
        HealthChannelInterfaces = []
        interfacerefs = []
        for ar_port in ar_r_ports:
            ar_required_interface_refs = ar_port.find_elements_of_type(
                'REQUIRED-INTERFACE-TREF',
                DEST='PHM-HEALTH-CHANNEL-INTERFACE')
            for interface_ref in ar_required_interface_refs:
                if str(interface_ref) not in interfacerefs:
                    interfacerefs.append(str(interface_ref))
        for interfaceref in interfacerefs:

            seinterface = HealthChannelInterface(interfaceref)
            ar_seinterface = self.model.find_referable(interfaceref)
            self._phmdeploymentbuilder.populate_interface(ar_seinterface, seinterface)

            for channelstatus in self.get_channelstatus(ar_seinterface):
                seinterface.add_channelstatus(channelstatus)
                seinterface.addidentifier(self.get_hc_interface_identifier_bychannelstatus(channelstatus))
            HealthChannelInterfaces.append(seinterface)

        return HealthChannelInterfaces

    def get_channelstatus(self, ar_interface):
        """Get the channelstatuss contained in an interface"""
        if_channelstatuss = get_element_or_none(ar_interface, "STATUSS")
        if if_channelstatuss is None:
            return []
        ar_channel_status_list = if_channelstatuss.find_elements_of_type("PHM-HEALTH-CHANNEL-STATUS")
        statusIdList = []
        for ar_channel_status in ar_channel_status_list:
            statusId = transition_number(get_element_or_none(ar_channel_status, "STATUS-ID"), defaultInt=None)
            if statusId is not None:
                assert statusId not in statusIdList, "[PHM ] CODE-004: The PhmHealthChannelStatus[{}].statusId[{}] contained in PhmHealthChannelInterface[{}] cannot be repeated. LocalPath:{}. ".format(ar_channel_status.get_fqn(), str(statusId), ar_interface.get_fqn(), ar_channel_status.get_path())
                statusIdList.append(statusId)
        channelstatuss = [ChannelStatus(ar_channelstatus.SHORT_NAME, transition_number(get_element_or_0(ar_channelstatus, "STATUS-ID")), _is_true(get_element_or_none(ar_channelstatus, "TRIGGERS-RECOVERY-NOTIFICATION")), ar_channelstatus.get_fqn()) for ar_channelstatus in ar_channel_status_list]

        return channelstatuss

    def get_checkpoints(self, ar_interface):
        """Get the checkpoints contained in an interface"""
        if_checkpoints = get_element_or_none(ar_interface, "CHECKPOINTS")
        if if_checkpoints is None:
            return []
        ar_checkpoints = if_checkpoints.find_elements_of_type("PHM-CHECKPOINT")
        checkpointIdList = []
        for ar_checkpoint in ar_checkpoints:
            checkpointId = transition_number(get_element_or_none(ar_checkpoint, "CHECKPOINT-ID"), defaultInt=None)
            if checkpointId is not None:
                assert checkpointId not in checkpointIdList, "[PHM ] CODE-005: The PhmCheckpoint[{}].checkpointId[{}] contained in PhmSupervisedEntityInterface[{}] cannot be repeated. LocalPath:{}. ".format(ar_checkpoint.get_fqn(), str(checkpointId), ar_interface.get_fqn(), ar_checkpoint.get_path())
                checkpointIdList.append(checkpointId)
        checkpoints = [CheckPoint(ar_checkpoint.SHORT_NAME, transition_number(get_element_or_none(ar_checkpoint, "CHECKPOINT-ID"), defaultInt=None), ar_checkpoint.get_fqn()) for ar_checkpoint in ar_checkpoints]

        return checkpoints

    def get_hc_interface_identifier_bychannelstatus(self, channelstatus):
        identifiers = []
        channelstatusfqn = channelstatus.fqn
        ar_hcchannelstatuses = self.model.find_elements_of_type('HEALTH-CHANNEL-EXTERNAL-STATUS')
        for ar_hcchannelstatus in ar_hcchannelstatuses:
            ar_hcex_reportedstatuses = ar_hcchannelstatus.find_elements_of_type("HEALTH-CHANNEL-EXTERNAL-REPORTED-STATUS")
            for ar_hcex_reportedstatuse in ar_hcex_reportedstatuses:
                tar_phm_channelstatus_ref = get_element_or_str(ar_hcex_reportedstatuse, "STATUS-REF")
                if channelstatusfqn == tar_phm_channelstatus_ref:
                    identifier = self._phmdeploymentbuilder.get_phmhealthchannel_identifier(ar_hcchannelstatus)
                    identifiers.append(identifier)
        return identifiers

    def get_se_interface_identifier_bycheckpoint(self, checkpoint):
        identifiers = []
        checkpointfqn = checkpoint.fqn
        ar_checkpoints = self.model.find_elements_of_type('SUPERVISION-CHECKPOINT')
        for ar_checkpoint in ar_checkpoints:
            tar_phm_checkpoint_ref = get_element_or_str(ar_checkpoint, "PHM-CHECKPOINT-IREF/TARGET-PHM-CHECKPOINT-REF")
            if checkpointfqn == tar_phm_checkpoint_ref:
                identifier = self._phmdeploymentbuilder.get_checkpoint_identifier(ar_checkpoint)
                identifiers.append(identifier)
        return identifiers

    def _get_phm_ser_notifical_interface(self, ar_ports):
        """ Extract required services from ports"""
        ar_p_ports = ar_ports.find_elements_of_type('R-PORT-PROTOTYPE')
        phmsernotificalinterfaces = list()
        for ar_port in ar_p_ports:
            ar_phm_ser_notifical_interface_refs = ar_port.find_elements_of_type(
                'REQUIRED-INTERFACE-TREF',
                DEST='PHM-SUPERVISION-RECOVERY-NOTIFICATION-INTERFACE')
            ar_phm_ser_notifical_interfaces = [
                self.model.find_referable(str(interface_ref))
                for interface_ref in ar_phm_ser_notifical_interface_refs]
            phm_ser_notifical_interfaces = [SERNotificalInterface(str(ar_required_interface))
                                            for ar_required_interface in
                                            ar_phm_ser_notifical_interfaces]

            for ar_interface, service in zip(ar_phm_ser_notifical_interfaces,
                                             phm_ser_notifical_interfaces):
                self._phmdeploymentbuilder.populate_interface(ar_interface, service)

            phmsernotificalinterfaces.extend(phm_ser_notifical_interfaces)
        return phmsernotificalinterfaces

    def _get_phm_hcr_notifical_interface(self, ar_ports):
        """ Extract required services from ports"""
        ar_p_ports = ar_ports.find_elements_of_type('R-PORT-PROTOTYPE')
        phmhcrnotificalinterfaces = list()
        for ar_port in ar_p_ports:
            ar_phm_hcr_notifical_interface_refs = ar_port.find_elements_of_type(
                'REQUIRED-INTERFACE-TREF',
                DEST='PHM-HEALTH-CHANNEL-RECOVERY-NOTIFICATION-INTERFACE')
            ar_phm_hcr_notifical_interfaces = [
                self.model.find_referable(str(interface_ref))
                for interface_ref in ar_phm_hcr_notifical_interface_refs]
            phm_hcr_notifical_interfaces = [HCRNotificalInterface(str(ar_required_interface))
                                            for ar_required_interface in
                                            ar_phm_hcr_notifical_interfaces]

            for ar_interface, service in zip(ar_phm_hcr_notifical_interfaces,
                                             phm_hcr_notifical_interfaces):
                self._phmdeploymentbuilder.populate_interface(ar_interface, service)
            phmhcrnotificalinterfaces.extend(phm_hcr_notifical_interfaces)
        return phmhcrnotificalinterfaces

    def _find_exe_swc(self, executables):
        result = []
        for executable in executables:
            ar_executable = self.model.find_referable(executable)
            ar_component_refs = ar_executable.find_elements_of_type(_APPL_TYPE_TREF, DEST=_ADAPTIVE_APPL_TYPE)
            if ar_component_refs:
                for ar_component_ref in ar_component_refs:
                    result.append(ar_component_ref.text)
        return result

    def _get_fw_interface(self, executables):
        fwList = []
        for executable in executables:
            exe = self.model.find_referable(executable)
            ar_root = get_element_or_none(exe, "ROOT-SW-COMPONENT-PROTOTYPE")
            if ar_root is not None:
                swc_ref = get_element_or_none(ar_root, "APPLICATION-TYPE-TREF")
                if swc_ref is not None:
                    swc = self.model.find_referable(swc_ref.text)
                    r_ports = swc.find_elements_of_type("R-PORT-PROTOTYPE")
                    for port in r_ports:
                        shortName = get_element_or_die(port, "SHORT-NAME")
                        fwOb = {
                            "name": shortName,
                            "identifier": "{}/{}/{}".format(exe.SHORT_NAME, ar_root.SHORT_NAME, shortName),
                            "enumList": []
                        }
                        sdgList = to_list(get_element_or_none(port, "ADMIN-DATA/SDGS/SDG/SDG"))
                        for sdg in sdgList:
                            enumOb = {}
                            sdList = to_list(get_element_or_none(sdg, "SD"))
                            for sd in sdList:
                                if sd.attrib["GID"] == "iSOFT:fw:FirewallStatusEnum:fwStatusName":
                                    if len(sd.text) > 0:
                                        enumOb["name"] = str(sd)[0].upper() + str(sd)[1:]
                                elif sd.attrib["GID"] == "iSOFT:fw:FirewallStatusEnum:fwStatusValue":
                                    enumOb["enumValue"] = sd.text
                            if len(enumOb) == 2:
                                fwOb["enumList"].append(enumOb)
                        if len(fwOb["enumList"]) > 0:
                            fwList.append(fwOb)
        return fwList


