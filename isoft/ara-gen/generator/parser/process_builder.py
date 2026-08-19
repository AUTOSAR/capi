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
import zlib

from generator.intermediate_model import Executable, Process, Service, Machine
from generator.intermediate_model.communication_management.ara_com_deployment.someip.someip_service_deployment import SomeIpServiceDeployment
from generator.parser import commen_util
from generator.parser.cg_builder import CGBuilder
from generator.parser.commen_util import get_instance_specifier
from generator.parser.crypto_builder import CryptoBuilder
from generator.parser.e2e_builder import E2EBuilder
from generator.parser.log_trace_builder import LogTraceBuilder
from generator.parser.qos_builder import QosBuilder
from generator.parser.signal_builder import SignalBuilder
from generator.parser.state_builder import StateBuilder
from generator.common.tree_helper import get_element_or_none, short_name, to_str, get_element_or_die, get_element_or_0, transition_number, get_element_or_false, get_element_or_str, to_list, short_name_2, to_nanoseconds
from generator.views.machine_view import MachineView


class ProcessBuilder:
    def __init__(self, model, args, _network_binding_builder, _interface_builder, _phm_builder):
        self._log = logging.getLogger(__name__)
        self.model = model
        self._args = args
        self._cryptoBuilder = CryptoBuilder(self.model)
        self._e2eBuilder = E2EBuilder(self.model)
        self._logTraceBuilder = LogTraceBuilder(self.model)
        self._cgBuilder = CGBuilder(self.model)
        self._stateBuilder = StateBuilder(self.model, self._args)
        self._qosBuilder = QosBuilder(self.model)
        self._network_binding_builder = _network_binding_builder
        self._interface_builder = _interface_builder
        self._phm_builder = _phm_builder

    def get_process_info(self, ar_process):
        fqn = ar_process.get_fqn()
        si_mappings = self.read_si_mappings(ar_process)
        executable = self.get_executable(ar_process)
        swclInfo = self.get_swcl_info(fqn)
        process = Process(
            fqn,
            function=get_element_or_str(ar_process, "FUNCTION-CLUSTER-AFFILIATION"),
            executable=executable,
            em_info=self.read_em_info(ar_process),
            deterministic_client=self.read_deterministic_client(ar_process),
            si_mappings=si_mappings,
            persistency_mappings=self.read_persistency_mappings(ar_process, executable, swclInfo),
            time_base_mappings=self.read_time_base_mappings(ar_process),
            machine=self.read_machine_info(fqn, si_mappings),
            recovery_to_p_port_mappings=self._phm_builder.get_recovery_notification_mapping(fqn),
            recovery_notification_infos=self.get_recovery_notification_info(fqn),
            swcl_info=swclInfo,
            crypto=self._cryptoBuilder.get_crypto_map(ar_process),
            e2e=self._e2eBuilder.get_e2e_someip(ar_process, si_mappings),
            logTrace=self._logTraceBuilder.get_log_config(ar_process),
            signalbindinfo=self.get_signalbind_info(si_mappings),
            rawmappings=self.get_raw_info(ar_process),
            cg=self._cgBuilder.get_machine_cg(ar_process)
        )
        process.state_manager = self._stateBuilder.get_state_json(ar_process)
        process.state_machine = self._stateBuilder.get_state_machine(ar_process)
        process.qos = self._qosBuilder.get_qos(ar_process, self._args.qos_path)
        process.deterministic = self.get_exec_deterministic(ar_process)
        process.ipcName = self.get_ipc_name(ar_process)
        process.ipcConfiguration = self.get_ipc_configuration(ar_process)
        process.clientId = self.get_client_id(ar_process)
        process.networkConfig = self.get_network_config(ar_process)
        return process

    def get_client_id(self, ar_process):
        sdgList = get_element_or_none(ar_process, "ADMIN-DATA/SDGS/SDG")
        if sdgList is not None:
            for sdg in sdgList:
                if sdg.attrib["GID"] == "iSOFT:exec:ClientIdInfo":
                    sd = get_element_or_none(sdg, "SD")
                    if sd is not None and sd.attrib["GID"] == "iSOFT:exec:ClientIdInfo:ClientId":
                        return transition_number(sd.text)
        return None

    def get_network_config(self, ar_process):
        sdgList = get_element_or_none(ar_process, "ADMIN-DATA/SDGS/SDG")
        if sdgList is not None:
            for sdg in sdgList:
                if sdg.attrib["GID"] == "iSOFT:com:SomeipNetworkConfig":
                    config = {}
                    sdList = to_list(get_element_or_none(sdg, "SD"))
                    for sd in sdList:
                        if sd.attrib["GID"] == "iSOFT:com:SomeipNetworkConfig:SendTimeout":
                            config["sendTimeout"] = sd.text
                        elif sd.attrib["GID"] == "iSOFT:com:SomeipNetworkConfig:RecvTimeout":
                            config["recvTimeout"] = sd.text
                        elif sd.attrib["GID"] == "iSOFT:com:SomeipNetworkConfig:QueueMixLimit":
                            config["queueMixLimit"] = sd.text
                        elif sd.attrib["GID"] == "iSOFT:com:SomeipNetworkConfig:TcpKeepaliveEnable":
                            config["tcpEnable"] = "1" if sd.text.lower() == "true" else "0"
                        elif sd.attrib["GID"] == "iSOFT:com:SomeipNetworkConfig:TcpKeepaliveIdle":
                            config["tcpIdle"] = sd.text
                        elif sd.attrib["GID"] == "iSOFT:com:SomeipNetworkConfig:TcpKeepaliveInterval":
                            config["tcpInterval"] = sd.text
                        elif sd.attrib["GID"] == "iSOFT:com:SomeipNetworkConfig:TcpKeepaliveCnt":
                            config["tcpCnt"] = sd.text
                    return config
        return None


    def get_executable(self, ar_process):
        """Get the executables related to a process"""
        ar_executable = self.model.find_referable(ar_process.EXECUTABLE_REF)
        ar_process_design_ref = get_element_or_none(ar_process, "DESIGN-REF")
        if ar_process_design_ref is not None:
            ar_process_design = self.model.find_referable(ar_process_design_ref.text)
            ar_executable_ref_from_design = get_element_or_none(ar_process_design, "EXECUTABLE-REF")
            if ar_executable_ref_from_design is not None:
                ar_executable_from_design = self.model.find_referable(ar_executable_ref_from_design.text)
                if ar_executable_from_design is not ar_executable:
                    self._log.error("The PROCESS %s and its related PROCESS-DESIGN %s reference different executables!",
                                    ar_process, ar_process_design)
        exeversion = to_str(get_element_or_none(ar_executable, "VERSION"))
        executable = Executable(ar_process.SHORT_NAME, ar_executable.SHORT_NAME, exeversion, fqn=ar_executable.get_fqn())
        return executable

    def read_si_mappings(self, ar_process):
        si_mappings = self._network_binding_builder.get_si_mappings_from_ar_process(ar_process)
        for mapping in si_mappings['provided']:
            mapping['service'] = self.make_service_by_instance(mapping['si_fqn'])
        for mapping in si_mappings['required']:
            mapping['service'] = self.make_service_by_instance(mapping['si_fqn'])
            mapping['service'].set_machine(self.get_machine_byservicefqn(mapping['service'].fqn))
        return si_mappings

    def get_machine_byservicefqn(self, servicefqn):
        ar_adaptive_app_swc_types = self.model.find_elements_of_type("ADAPTIVE-APPLICATION-SW-COMPONENT-TYPE")
        for ar_adaptive_app_swc_type in ar_adaptive_app_swc_types:
            provided_interface_refs = ar_adaptive_app_swc_type.find_elements_of_type("PROVIDED-INTERFACE-TREF")
            for provided_interface_ref in provided_interface_refs:
                if provided_interface_ref and provided_interface_ref.text == servicefqn:
                    swcfqn = ar_adaptive_app_swc_type.get_fqn()
                    ar_executables = self.model.find_elements_of_type("EXECUTABLE")
                    for ar_executable in ar_executables:
                        swc_ref = get_element_or_str(ar_executable, "ROOT-SW-COMPONENT-PROTOTYPE/APPLICATION-TYPE-TREF")
                        if swcfqn == swc_ref.text:
                            executable_fqn = ar_executable.get_fqn()
                            ar_processes = self.model.find_elements_of_type("PROCESS")
                            for ar_process in ar_processes:
                                executable_ref = get_element_or_str(ar_process, "EXECUTABLE-REF")
                                if executable_fqn == executable_ref.text:
                                    process_fqn = ar_process.get_fqn()
                                    return self.read_machine_info(process_fqn)
        return None

    def read_machine_info(self, processfqn, si_mapping_ob=None):
        ar_mappings = self.model.find_elements_of_type(
            'PROCESS-TO-MACHINE-MAPPING',
            accept=lambda e: str(
                e.PROCESS_REF) == processfqn)
        for ar_mapping in ar_mappings:
            ar_machine_ref = get_element_or_none(ar_mapping, "MACHINE-REF")
            if ar_machine_ref is not None:
                ar_machine = self.model.find_referable(ar_machine_ref.text)
                machine = Machine(ar_machine.get_fqn(), configuration=None)
                self.get_machine_design(ar_machine, machine)
                if si_mapping_ob is not None:
                    machine.si_mappings = [si_mapping_ob]
                return MachineView(machine)
        return None

    def get_multiple_host(self, ar_machine):
        multipleHostList = []
        machine_design_ref = get_element_or_die(ar_machine, 'MACHINE-DESIGN-REF')
        ar_machine_design = self.model.find_referable(str(machine_design_ref))
        connectorList = ar_machine_design.find_elements_of_type("ETHERNET-COMMUNICATION-CONNECTOR")
        discoveryList = ar_machine_design.find_elements_of_type("SOMEIP-SERVICE-DISCOVERY")
        channelList = []
        if len(connectorList) > 1 and len(discoveryList) > 1:
            networkRef = get_element_or_die(connectorList[0], "UNICAST-NETWORK-ENDPOINT-REF")
            channelList.append("/".join(str(networkRef).split("/")[:-1]))
            for connector in connectorList[1:]:
                networkRef = get_element_or_die(connector, "UNICAST-NETWORK-ENDPOINT-REF")
                channelRef = "/".join(str(networkRef).split("/")[:-1])
                if channelRef not in channelList:
                    channelList.append(channelRef)
                    for discovery in discoveryList[1:]:
                        discovery_ref = get_element_or_none(discovery, 'MULTICAST-SD-IP-ADDRESS-REF')
                        if discovery_ref is not None:
                            channel_ref = "/".join(str(discovery_ref).split("/")[:-1])
                            if channelRef == channel_ref:
                                networkEle = self.model.find_referable(networkRef.text)
                                host = commen_util.get_network_address(networkEle)
                                multipleEle = self.model.find_referable(discovery_ref.text)
                                sd_multicast = commen_util.get_network_address(multipleEle)
                                sd_unicast = get_element_or_none(discovery, "SOMEIP-SERVICE-DISCOVERY-PORT")
                                if host != "" and sd_multicast != "" and sd_unicast is not None:
                                    hostOb = {
                                        "host": host,
                                        "sd_multicast": sd_multicast,
                                        "sd_unicast": sd_unicast.text
                                    }
                                    if hostOb not in multipleHostList:
                                        multipleHostList.append(hostOb)
        return multipleHostList


    def get_ipc_name(self, ar_process):
        sdgList = get_element_or_none(ar_process, "ADMIN-DATA/SDGS/SDG")
        if sdgList is not None:
            for sdg in sdgList:
                if sdg.attrib["GID"] == "iSOFT:exec:ExecIpcInfo":
                    sd = get_element_or_none(sdg, "SD")
                    if sd is not None and sd.attrib["GID"] == "iSOFT:exec:ExecIpcInfo:IpcName":
                        return sd.text
        return None

    def get_ipc_configuration(self, ar_process):
        sdgList = get_element_or_none(ar_process, "ADMIN-DATA/SDGS/SDG")
        if sdgList is not None:
            for sdg in sdgList:
                if sdg.attrib["GID"] == "iSOFT:com:IpcConfiguration":
                    ipcConfiguration = {}
                    sdList = get_element_or_none(sdg, "SD")
                    if sdList is not None:
                        for sd in sdList:
                            if sd.attrib["GID"] == "iSOFT:com:IpcConfiguration:Mode":
                                ipcConfiguration["mode"] = sd
                            elif sd.attrib["GID"] == "iSOFT:com:IpcConfiguration:NetName":
                                ipcConfiguration["netName"] = sd
                            elif sd.attrib["GID"] == "iSOFT:com:IpcConfiguration:AppName":
                                ipcConfiguration["appName"] = sd
                            elif sd.attrib["GID"] == "iSOFT:com:IpcConfiguration:AppId":
                                ipcConfiguration["appId"] = transition_number(sd)
                            elif sd.attrib["GID"] == "iSOFT:com:IpcConfiguration:ServiceNetType":
                                ipcConfiguration["serviceNetType"] = sd
                            elif sd.attrib["GID"] == "iSOFT:com:IpcConfiguration:ServiceAccessHandle":
                                ipcConfiguration["serviceAccessHandle"] = sd
                            elif sd.attrib["GID"] == "iSOFT:com:IpcConfiguration:DiscoveryNetType":
                                ipcConfiguration["discoveryNetType"] = sd
                            elif sd.attrib["GID"] == "iSOFT:com:IpcConfiguration:SendTimeout":
                                ipcConfiguration["sendTimeout"] = sd
                            elif sd.attrib["GID"] == "iSOFT:com:IpcConfiguration:DiscoveryAccessHandle":
                                ipcConfiguration["discoveryAccessHandle"] = sd
                        return ipcConfiguration
        return {}

    def get_machine_design(self, ar_machine, machine):
        machine_design_refs = ar_machine.find_elements_of_type('MACHINE-DESIGN-REF', DEST='MACHINE-DESIGN')
        if machine_design_refs is None or len(machine_design_refs) == 0:
            self._log.error("No MachineDesignRef found for %s!", machine.name)
            assert False, "[BASE] CODE-001: No machineDesignRef found for Machine. FQN:{}. LocalPath:{}".format(ar_machine.get_fqn(), ar_machine.get_path())
        elif len(machine_design_refs) > 1:
            self._log.warning("More than one MACHINE-DESIGN-REF found for %s! The first match is used", machine.name())
        ar_machine_design = self.model.find_referable(str(machine_design_refs[0]))
        unicast_addr = self.get_unicast_address(ar_machine_design)
        machine.ip_address = unicast_addr
        # machine.network_mask = unicast_addr["netmask"]
        # machine.gateway = unicast_addr["gateway"]
        machine.sd_address = self.get_sd_ip(ar_machine_design)
        machine.sd_port = self.get_sd_port(ar_machine_design)
        machine.usd_sd_option = self.get_sd_option(ar_machine_design)
        if self.get_maxtaransmissunit(ar_machine_design) > 0:
            machine.maxtransmissunit = self.get_maxtaransmissunit(ar_machine_design) - 60
        machine.multiple_host = self.get_multiple_host(ar_machine)

    def get_unicast_address(self, machine):
        """ Returns unicast address: ip address and network mask """
        unicast_ref = get_element_or_die(machine, "COMMUNICATION-CONNECTORS/ETHERNET-COMMUNICATION-CONNECTOR/UNICAST-NETWORK-ENDPOINT-REF")
        endpoint_info = self.model.find_referable(unicast_ref.text)
        return commen_util.get_network_address(endpoint_info)

    def get_maxtaransmissunit(self, machine_design):
        maxtaransmissunit = transition_number(get_element_or_0(machine_design, "COMMUNICATION-CONNECTORS/ETHERNET-COMMUNICATION-CONNECTOR/MAXIMUM-TRANSMISSION-UNIT"))
        return maxtaransmissunit

    def get_sd_ip(self, machine):
        """ Returns multicast (service discovery) ip address """
        multicast_sd_ref = get_element_or_none(machine, "SERVICE-DISCOVER-CONFIGS/SOMEIP-SERVICE-DISCOVERY/MULTICAST-SD-IP-ADDRESS-REF")
        if not multicast_sd_ref:
            return None
        endpoint_info = self.model.find_referable(multicast_sd_ref.text)
        ip_address = commen_util.get_network_address(endpoint_info)
        return ip_address

    def get_sd_option(self, machine):
        """ Returns multicast (service discovery) ip address """
        multicast_sd_ref = get_element_or_none(machine, "SERVICE-DISCOVER-CONFIGS/SOMEIP-SERVICE-DISCOVERY/MULTICAST-SD-IP-ADDRESS-REF")
        if not multicast_sd_ref:
            return -1
        endpoint_info = self.model.find_referable(multicast_sd_ref.text)

        sdgList = to_list(get_element_or_none(endpoint_info, "ADMIN-DATA/SDGS/SDG"))
        for sdg in sdgList:
            if sdg.attrib["GID"] == "iSOFT:com:SomeipSdConfiguration":
                sd = get_element_or_none(sdg, "SD")
                if sd is not None and sd.attrib["GID"] == "iSOFT:com:SomeipSdConfiguration:EnableIpv4SdEndpointOption":
                    if sd.text == "false":
                        return 0
                    elif sd.text == "true":
                        return 1
        return -1

    def get_sd_port(self, machine):
        """ Returns service discovery port number """
        sd_port = get_element_or_none(machine, "SERVICE-DISCOVER-CONFIGS/SOMEIP-SERVICE-DISCOVERY/SOMEIP-SERVICE-DISCOVERY-PORT")
        if not sd_port:
            return None
        return sd_port.text

    def make_service_by_instance(self, ar_si_fqn):
        ar_si = self.model.find_referable(ar_si_fqn)
        ar_depl = self.model.find_referable(ar_si.SERVICE_INTERFACE_DEPLOYMENT_REF)
        ar_interface = self.model.find_referable(ar_depl.SERVICE_INTERFACE_REF)
        service = Service(ar_interface.get_fqn())
        self._interface_builder.populate_interface(ar_interface, service)
        deployment = self._network_binding_builder.make_deployment(ar_depl, ar_si)
        self._network_binding_builder.populate_deployment(service, deployment)
        service.service_deployment = deployment
        service.is_show = deployment.is_show
        if type(deployment) is SomeIpServiceDeployment:
            signalbuilder = SignalBuilder(self.model)
            for signal in signalbuilder.get_signals_byservicedeploymentid(service.service_deployment.deployment_id):
                service.add_signal(signal)
            for etosmapping in signalbuilder.get_etosmappings_byservice(service.fqn):
                service.add_etosmapping(etosmapping)
            for stoemapping in signalbuilder.get_stoemappings_byservice(service.fqn):
                service.add_stoemapping(stoemapping)
            service.adjust_signaluserevent()
        return service

    def get_exec_deterministic(self, ar_process):
        mapping = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING", PROCESS_REF=ar_process.get_fqn())
        deterministic = {}
        for item in mapping:
            sdgList = get_element_or_none(item, "ADMIN-DATA/SDGS/SDG")
            if sdgList is not None:
                for sdgItem in sdgList:
                    if sdgItem.attrib["GID"] == "iSOFT:exec:DeterministicSyncMaster":
                        masterSDG = [
                            "SDG-CAPTION/SHORT-NAME",
                            "iSOFT:exec:DeterministicSyncMaster:MaxRunCycleLoop",
                            "iSOFT:exec:DeterministicSyncMOutOfN:MinimumNumberOfRequests",
                            "iSOFT:exec:DeterministicSyncMOutOfN:NumberOfConnectedClients"
                        ]
                        master = {}
                        sdgCaption = get_element_or_none(sdgItem, "SDG-CAPTION")
                        if sdgCaption is not None:
                            masterSDG.remove("SDG-CAPTION/SHORT-NAME")
                            master["fullQualifiedName"] = sdgCaption.get_fqn()
                        sd = get_element_or_none(sdgItem, "SD")
                        if sd is not None and sd.attrib["GID"] == "iSOFT:exec:DeterministicSyncMaster:MaxRunCycleLoop":
                            try:
                                masterSDG.remove(sd.attrib["GID"])
                                master["maxRunCycleLoop"] = int(sd.text)
                            except Exception:
                                assert False, "[EM  ] CODE-001: iSOFT:exec:DeterministicSyncMaster:MaxRunCycleLoop must be int. Path:{}. LocalPath:{}. ".format(item.get_fqn(), item.get_path())
                        sdg = get_element_or_none(sdgItem, "SDG")
                        if sdg is not None and sdg.attrib["GID"] == "iSOFT:exec:DeterministicSyncMasterSubclass:Attributes":
                            master["verificationMethod"] = "deterministicSyncMOutOfN"
                            sdList = sdg.find_elements_of_type("SD")
                            deterministicSyncMOutOfN = {}
                            for sdItem in sdList:
                                if sdItem.attrib["GID"] == "iSOFT:exec:DeterministicSyncMOutOfN:MinimumNumberOfRequests":
                                    try:
                                        masterSDG.remove(sdItem.attrib["GID"])
                                        number1 = int(sdItem.text)
                                        if number1 < 1 or number1 > 10:
                                            raise ValueError
                                        deterministicSyncMOutOfN["minimumNumberOfRequests"] = number1
                                    except Exception:
                                        assert False, "[EM  ] CODE-001: iSOFT:exec:DeterministicSyncMOutOfN:MinimumNumberOfRequests must be an integer and the value range is 1-10. Path:{}. LocalPath:{}. ".format(item.get_fqn(), item.get_path())
                                elif sdItem.attrib["GID"] == "iSOFT:exec:DeterministicSyncMOutOfN:NumberOfConnectedClients":
                                    try:
                                        masterSDG.remove(sdItem.attrib["GID"])
                                        number1 = int(sdItem.text)
                                        if number1 < 1 or number1 > 10:
                                            raise ValueError
                                        deterministicSyncMOutOfN["numberOfConnectedClients"] = number1
                                    except Exception:
                                        assert False, "[EM  ] CODE-001: iSOFT:exec:DeterministicSyncMOutOfN:NumberOfConnectedClients must be an integer and the value range is 1-10. Path:{}. LocalPath:{}. ".format(item.get_fqn(), item.get_path())
                            master["deterministicSyncMOutOfN"] = deterministicSyncMOutOfN
                        assert len(masterSDG) == 0, "[EM  ] CODE-001: 'iSOFT:exec:DeterministicSyncMaster' needs to be configured {}. Path:{}. LocalPath:{}. ".format(masterSDG, item.get_fqn(), item.get_path())
                        deterministic["master"] = master
                    elif sdgItem.attrib["GID"] == "iSOFT:exec:ExecDeterministicClient":
                        clientSDG = [
                            "iSOFT:exec:ExecDeterministicClient:CycleTimeValue",
                            "iSOFT:exec:ExecDeterministicClient:NumberOfWorkers",
                            "DeterministicSyncMaster"
                        ]
                        client = {}
                        sdList = sdgItem.find_elements_of_type("SD")
                        for sdItem in sdList:
                            if sdItem.attrib["GID"] == "iSOFT:exec:ExecDeterministicClient:CycleTimeValue":
                                try:
                                    clientSDG.remove(sdItem.attrib["GID"])
                                    client["cycleTimeValue"] = float(sdItem.text)
                                except Exception:
                                    assert False, "[EM  ] CODE-002: iSOFT:exec:ExecDeterministicClient:CycleTimeValue must be float. Path:{}. LocalPath:{}. ".format(item.get_fqn(), item.get_path())
                            elif sdItem.attrib["GID"] == "iSOFT:exec:ExecDeterministicClient:NumberOfWorkers":
                                try:
                                    clientSDG.remove(sdItem.attrib["GID"])
                                    number1 = int(sdItem.text)
                                    if number1 < 1 or number1 > 20:
                                        raise ValueError
                                    client["numberOfWorkers"] = number1
                                except Exception:
                                    assert False, "[EM  ] CODE-002: iSOFT:exec:ExecDeterministicClient:NumberOfWorkers must be an integer and the value range is 1-20. Path:{}. LocalPath:{}. ".format(item.get_fqn(), item.get_path())
                        sdxRef = get_element_or_none(sdgItem, "SDX-REF")
                        if sdxRef is not None:
                            clientSDG.remove("DeterministicSyncMaster")
                            client["deterministicSyncMaster"] = sdxRef.text
                            nameList = sdxRef.text.split("/")
                            mappingRef = "/".join(nameList[0: -1])
                            mapping = self.model.find_referable(mappingRef)
                            if mapping is not None:
                                processRef = get_element_or_none(mapping, "PROCESS-REF")
                                if processRef is not None:
                                    process = self.model.find_referable(processRef.text)
                                    exeRef = get_element_or_none(process, "EXECUTABLE-REF")
                                    if exeRef is not None:
                                        client["deterSyncMasterIpcName"] = short_name(exeRef.text)
                        assert len(clientSDG) == 0, "[EM  ] CODE-002: 'iSOFT:exec:ExecDeterministicClient' needs to be configured {}. Path:{}. LocalPath:{}. ".format(clientSDG, item.get_fqn(), item.get_path())
                        deterministic["client"] = client
        return deterministic

    def get_recovery_notification_info(self, processfqn):
        ar_mappings = self.model.find_elements_of_type(
            'RECOVERY-NOTIFICATION-TO-P-PORT-PROTOTYPE-MAPPING',
            accept=lambda e: str(
                e.PROCESS_REF) == processfqn)
        resultmaping = []
        for ar_mapping in ar_mappings:
            recovery_action = get_element_or_none(ar_mapping, "RECOVERY-ACTION-IREF")
            instance_specifier = commen_util.get_instance_specifier_port(self, ar_mapping, recovery_action)
            if recovery_action is not None:
                ar_port_ref = get_element_or_die(recovery_action, "TARGET-P-PORT-PROTOTYPE-REF")
                ar_port = self.model.find_referable(ar_port_ref.text)

                interface = get_element_or_none(ar_port, "PROVIDED-INTERFACE-TREF")

                interface_class = ""
                if interface.get("DEST") == "PHM-SUPERVISION-RECOVERY-NOTIFICATION-INTERFACE":
                    interface_class = "PhmSupervisionRecoveryNotificationInterface"
                if interface.get("DEST") == "PHM-HEALTH-CHANNEL-RECOVERY-NOTIFICATION-INTERFACE":
                    interface_class = "PhmHealthChannelRecoveryNotificationInterface"

                recoverynotificationinfo = {
                    "instance_specifier": instance_specifier,
                    "interface": interface,
                    "interface_class": interface_class
                }

                resultmaping.append(recoverynotificationinfo)
        return resultmaping

    def get_swcl_info(self, proceessfqn):
        ar_software_clusters = self.model.find_elements_of_type('SOFTWARE-CLUSTER')
        for ar_software_cluster in ar_software_clusters:
            arprocessrefs = ar_software_cluster.find_elements_of_type("CONTAINED-PROCESS-REF")
            for arprocessref in arprocessrefs:
                if arprocessref.text == proceessfqn:
                    softwareclusterfqn = ar_software_cluster.get_fqn()
                    softwareclusterversion = get_element_or_none(ar_software_cluster, 'VERSION')
                    softwareclustername = softwareclusterfqn.split("/")[-1]
                    swcl_info = {
                        "softwareclustername": softwareclustername
                    }
                    if softwareclusterversion is not None:
                        swcl_info["softwareclusterversion"] = softwareclusterversion.text
                    else:
                        swcl_info["softwareclusterversion"] = "1.0.0"
                    return swcl_info
        return {"softwareclustername": "Unkown", "softwareclusterversion": "1.0.0"}

    def get_raw_info(self, processfqn):
        ethernet_raw_data_stream_mapping = []
        ar_mappings = self.model.find_elements_of_type(
            'ETHERNET-RAW-DATA-STREAM-MAPPING')
        for ar_mapping in ar_mappings:
            ar_proess_ref = get_element_or_die(ar_mapping, "PROCESS-REF")
            ar_process = self.model.find_referable(ar_proess_ref.text)
            if ar_process == processfqn:
                instance_specifier = commen_util.get_instance_specifier(self, ar_mapping)
                socketOption = []
                ar_socketoptions = ar_mapping.find_elements_of_type("SOCKET-OPTION")
                for ar_socketoption in ar_socketoptions:
                    socketOption.append(ar_socketoption.text)
                tcpPort = transition_number(get_element_or_0(ar_mapping, "TCP-PORT"))
                udpPort = transition_number(get_element_or_0(ar_mapping, "UDP-PORT"))
                multicastUdp = transition_number(get_element_or_0(ar_mapping, "MULTICAST-UDP-PORT"))
                communicationconnectorref = get_element_or_die(ar_mapping, "COMMUNICATION-CONNECTOR-REF")
                ar_communicationconnector = self.model.find_referable(communicationconnectorref.text)
                maximumTransmissionUnit = transition_number(get_element_or_0(ar_communicationconnector, "MAXIMUM-TRANSMISSION-UNIT"))
                pathMtuTimeout = transition_number(get_element_or_0(ar_communicationconnector, "PNC-FILTER-DATA-MASK"))
                ipAddress = ""
                unicastnetworkendpointref = get_element_or_none(ar_communicationconnector, "UNICAST-NETWORK-ENDPOINT-REF")
                if unicastnetworkendpointref is not None:
                    ar_unicastnetworkendpoint = self.model.find_referable(unicastnetworkendpointref.text)
                    ipAddress = get_element_or_str(ar_unicastnetworkendpoint, "NETWORK-ENDPOINT-ADDRESSES/IPV-4-CONFIGURATION/IPV-4-ADDRESS")
                ethernet_raw__mapping = {
                    "instance_specifier": instance_specifier,
                    "socketOption": socketOption,
                    "tcpPort": tcpPort,
                    "udpPort": udpPort,
                    "multicastUdp": multicastUdp,
                    "maximumTransmissionUnit": maximumTransmissionUnit,
                    "pathMtuTimeout": pathMtuTimeout,
                    "ipAddress": ipAddress,
                    "ca": "",
                    "crt": "",
                    "key": "",
                    "password": ""
                }
                ethernet_raw_data_stream_mapping.append(ethernet_raw__mapping)
        return {"ethernet_raw_data_stream_mapping": ethernet_raw_data_stream_mapping}

    def get_signalbind_info(self, si_mapping):
        signalbuilder = SignalBuilder(self.model)
        return signalbuilder.get_signalbindinfo(si_mapping)

    def read_em_info(self, ar_process):

        result = {"platform_application": None, "timer_granularity": None, "execution_state_reporting": None}
        ar_executable_ref = get_element_or_die(ar_process, "EXECUTABLE-REF")
        result["restartNumber"] = transition_number(get_element_or_0(ar_process, "NUMBER-OF-RESTART-ATTEMPTS"))
        result["pre_mapping"] = get_element_or_false(ar_process, "PRE-MAPPING")
        result["functionClusterAffiliation"] = get_element_or_none(ar_process, "FUNCTION-CLUSTER-AFFILIATION")

        machine_mapping = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING")
        processor = []
        for machine_set in machine_mapping:
            set_process_ref = get_element_or_none(machine_set, "PROCESS-REF")
            if set_process_ref is not None and set_process_ref.text == ar_process.get_fqn():
                set_machine = get_element_or_none(machine_set, "MACHINE-REF")
                not_run_map = []
                run_map = []
                not_run_on = get_element_or_none(machine_set, "SHALL-NOT-RUN-ON-REFS")
                if not_run_on is not None:
                    not_run_on_ref_map = not_run_on.find_elements_of_type("SHALL-NOT-RUN-ON-REF")
                    for not_run in not_run_on_ref_map:
                        not_run_map.append(not_run)
                run_on = get_element_or_none(machine_set, "SHALL-RUN-ON-REFS")
                if run_on is not None:
                    run_on_ref_map = run_on.find_elements_of_type("SHALL-RUN-ON-REF")
                    for run in run_on_ref_map:
                        run_map.append(run)
                if set_machine is not None:
                    machine_element = self.model.find_referable(set_machine.text)
                    processors = get_element_or_none(machine_element, "PROCESSORS")
                    if processors is not None:
                        processor_map = processors.find_elements_of_type("PROCESSOR")
                        for index, processor_item in enumerate(processor_map):
                            processorOb = {
                                "id": index
                            }
                            processorItem = []
                            cores = get_element_or_none(processor_item, "CORES")
                            if cores is not None:
                                processor_core_map = processor_item.find_elements_of_type("PROCESSOR-CORE")
                                for processor_core in processor_core_map:
                                    coreFqn = processor_core.get_fqn()
                                    if len(run_map) > 0:
                                        if coreFqn in run_map:
                                            processorItem.append(transition_number(get_element_or_0(processor_core, "CORE-ID")))
                                    else:
                                        if coreFqn not in not_run_map:
                                            processorItem.append(transition_number(get_element_or_0(processor_core, "CORE-ID")))
                            processorOb["core"] = processorItem

                            if len(processorItem) > 0:
                                processor.append(processorOb)
                                result["processor"] = processor
        ar_executable = self.model.find_referable(ar_executable_ref.text)
        ar_category = get_element_or_none(ar_executable, "CATEGORY")
        ar_timer_granularity = get_element_or_0(ar_executable, "MINIMUM-TIMER-GRANULARITY")
        ar_execution_state_reporting = get_element_or_none(ar_executable, "REPORTING-BEHAVIOR")
        ar_execution_state_logging = get_element_or_none(ar_executable, "LOGGING-BEHAVIOR")
        if ar_category:
            result["platform_application"] = ar_category.text.upper() == "PLATFORM_LEVEL"
        else:
            result["platform_application"] = False
        result["timer_granularity"] = ar_timer_granularity
        if ar_execution_state_logging:
            result["uses_logging"] = ar_execution_state_logging.text.upper() == "USES-LOGGING"
        else:
            result["uses_logging"] = False
        if ar_execution_state_reporting:
            result["execution_state_reporting"] = ar_execution_state_reporting.text
        else:
            result["execution_state_reporting"] = ""
        build_type = get_element_or_str(ar_executable, "BUILD-TYPE")
        if build_type == "BUILD-TYPE-RELEASE":
            result["buildType"] = "Release"
        else:
            result["buildType"] = "Debug"
        result["version"] = get_element_or_str(ar_executable, "VERSION")
        result["name"] = get_element_or_str(ar_executable, "SHORT-NAME")
        result["fqn"] = ar_executable.get_fqn()

        resourceConsumption = {
            "heap_usage_key": 0,
            "memory_usage_key": 0
        }
        heap_usage_mapping = ar_process.find_elements_of_type("WORST-CASE-HEAP-USAGE")
        for heap_usage in heap_usage_mapping:
            resourceConsumption["heap_usage_key"] = transition_number(get_element_or_0(heap_usage, "MEMORY-CONSUMPTION"))
            break
        memory_usage_mapping = ar_process.find_elements_of_type("SYSTEM-MEMORY-USAGE")
        for memory_usage in memory_usage_mapping:
            resourceConsumption["memory_usage_key"] = transition_number(get_element_or_0(memory_usage, "MEMORY-CONSUMPTION"))
            break
        ar_md_startup_configs = get_element_or_none(ar_process, "STATE-DEPENDENT-STARTUP-CONFIGS/STATE-DEPENDENT-STARTUP-CONFIG")
        ar_md_startup_configs = to_list(ar_md_startup_configs)
        startup_configs = []
        mode_group_ref = None
        for ar_md_startup_config in ar_md_startup_configs:

            executionDependency = []
            dependency_mapping = ar_md_startup_config.find_elements_of_type("EXECUTION-DEPENDENCY")
            for dependency in dependency_mapping:
                process_name = get_element_or_str(dependency, "PROCESS-STATE-IREF/CONTEXT-MODE-DECLARATION-GROUP-PROTOTYPE-REF")
                process_name = short_name_2(process_name)
                declaration_name = get_element_or_str(dependency,
                                                      "PROCESS-STATE-IREF/TARGET-MODE-DECLARATION-REF")
                declaration_name = short_name(declaration_name).lower()
                if process_name != "" and declaration_name != "":
                    executionDependency.append(process_name + "." + declaration_name)

            fg_modes = []
            machine_states = []
            ar_fg_modes = get_element_or_none(ar_md_startup_config, "FUNCTION-GROUP-STATE-IREFS/FUNCTION-GROUP-STATE-IREF")
            ar_fg_modes = to_list(ar_fg_modes)
            for ar_fg_mode in ar_fg_modes:
                ar_mode_group_ref = get_element_or_die(ar_fg_mode, "CONTEXT-MODE-DECLARATION-GROUP-PROTOTYPE-REF")
                ar_mode_ref = get_element_or_die(ar_fg_mode, "TARGET-MODE-DECLARATION-REF")
                ar_mode_group = self.model.find_referable(ar_mode_group_ref.text)
                ar_type_ref = get_element_or_die(ar_mode_group, "TYPE-TREF")
                ar_type = self.model.find_referable(ar_type_ref.text)
                ar_mode = self.model.find_referable(ar_mode_ref.text)
                mode = ar_mode.SHORT_NAME
                mode_group = ar_type.SHORT_NAME
                if mode_group == "MachineState":
                    machine_states.append(mode)
                else:
                    fg_modes.append({"mode_group": ar_mode_group_ref, "mode": mode})
                    if mode_group_ref is None:
                        mode_group_ref = ar_mode_group_ref
                    else:
                        assert mode_group_ref == ar_mode_group_ref, "[SM  ] CODE-001: The process of {} must reference the same ModeDeclarationGroupPrototype".format(ar_process.get_fqn())

            resource_group = ""
            ar_resource_group_ref = get_element_or_none(ar_md_startup_config, "RESOURCE-GROUP-REF")
            if ar_resource_group_ref:
                ar_resource_group = self.model.find_referable(ar_resource_group_ref.text)
                resource_group = ar_resource_group.SHORT_NAME

            ar_startup_config_ref = get_element_or_none(ar_md_startup_config, "STARTUP-CONFIG-REF")
            if ar_startup_config_ref:
                ar_startup_config = self.model.find_referable(ar_startup_config_ref.text)
                self_terminating_process = get_element_or_none(ar_startup_config, "TERMINATION-BEHAVIOR")
                if (self_terminating_process):
                    self_terminating_process = self_terminating_process.text.upper() == "PROCESS-IS-SELF-TERMINATING"
                scheduling_policy = get_element_or_str(ar_startup_config, "SCHEDULING-POLICY")
                scheduling_priority = get_element_or_0(ar_startup_config, "SCHEDULING-PRIORITY")
                behavior = get_element_or_str(ar_startup_config, "TERMINATION-BEHAVIOR")
                timeout_element = get_element_or_none(ar_startup_config, "TIMEOUT")
                timeoutOb = None
                if timeout_element is not None:
                    timeoutOb = {"enterTimeout": get_element_or_0(timeout_element, "ENTER-TIMEOUT-VALUE"),
                                 "exitTimeout": get_element_or_0(timeout_element, "EXIT-TIMEOUT-VALUE")}

                optional_mapping = ar_startup_config.find_elements_of_type("TAG-WITH-OPTIONAL-VALUE")
                variables = []
                for optional in optional_mapping:
                    key = to_str(get_element_or_str(optional, "KEY"))
                    value = to_str(get_element_or_str(optional, "VALUE"))
                    variables.append(key + "=" + value)

                execution_error_num = 0
                error_ref = get_element_or_none(ar_startup_config, "EXECUTION-ERROR-REF")
                if error_ref is not None:
                    execution_error = self.model.find_referable(error_ref.text)
                    if execution_error is not None:
                        execution_error_num = transition_number(get_element_or_0(execution_error, "EXECUTION-ERROR"))

                arguments = self._read_command_line(ar_startup_config)
                environment_vars = self.read_timeout(ar_startup_config)
                md_startup_config = {
                    "name": ar_startup_config.SHORT_NAME,
                    "fg_modes": fg_modes,
                    "executionDependency": executionDependency,
                    "machine_states": machine_states,
                    "resource_group": resource_group,
                    "scheduling_policy": scheduling_policy,
                    "scheduling_priority": scheduling_priority,
                    "behavior": behavior,
                    "timeoutOb": timeoutOb,
                    "environment_vars": environment_vars,
                    "arguments": arguments,
                    "resourceConsumption": resourceConsumption,
                    "environmentVariable": variables,
                    "execution_error_num": execution_error_num,
                    "self_terminating_process": self_terminating_process
                }

                startup_configs.append(md_startup_config)

        result["startup_configs"] = startup_configs

        return result

    def read_deterministic_client(self, ar_process):
        ref = get_element_or_none(ar_process, "DETERMINISTIC-CLIENT-REF")
        if ref is not None:
            element = self.model.find_referable(ref.text)
            ob = {}
            timeValue = get_element_or_none(element, "CYCLE-TIME-VALUE")
            if timeValue is not None:
                ob["cycleTimeValue"] = timeValue
            numberWorker = transition_number(get_element_or_none(element, "NUMBER-OF-WORKERS"), defaultInt=None)
            if numberWorker is not None:
                ob["numberOfWorkers"] = numberWorker
            return ob
        else:
            return None

    def read_persistency_mappings(self, ar_process, executable, swcl_info):
        return {
            "kvs": self._read_persistency_kvs_mappings(ar_process, executable, swcl_info),
            "file": self._read_persistency_file_mappings(ar_process),
        }

    def get_version_value_encoding(self, ar_value, ar_key):
        key_crc32 = zlib.crc32(bytes(ar_key, encoding='utf-8'))
        i = 0
        value_encoding = ""
        while i < len(ar_value):
            nRandom = (1 + 1) * 16 + (i + 1)
            value_encoding += self.encodingFunc(ord(ar_value[i]), key_crc32, nRandom)
            i += 1
        return value_encoding

    def _read_persistency_kvs_mappings(self, ar_process, executable, swcl_info):
        result = []

        ar_mappings = self.model.find_elements_of_type(
            'PERSISTENCY-PORT-PROTOTYPE-TO-KEY-VALUE-STORAGE-MAPPING',
            accept=lambda e: str(
                e.PROCESS_REF) == ar_process.get_fqn())
        appVersion = executable.exe_version
        app_key_value_crc32 = zlib.crc32(bytes("AppVersion" + appVersion, encoding='utf-8'))
        app_key_crc32 = self.get_version_value_encoding(appVersion, "AppVersion")
        app_value_length = len(str(appVersion))
        perVersion = swcl_info["softwareclusterversion"]
        per_key_value_crc32 = zlib.crc32(bytes("PerVersion" + perVersion, encoding='utf-8'))
        per_key_crc32 = self.get_version_value_encoding(perVersion, "PerVersion")
        per_value_length = len(str(perVersion))

        for ar_mapping in ar_mappings:
            ar_kvs_ref = get_element_or_die(ar_mapping, "KEY-VALUE-STORAGE-REF")
            ar_kvs = self.model.find_referable(ar_kvs_ref.text)
            minimum_sustained_size = get_element_or_none(ar_kvs, "MINIMUM-SUSTAINED-SIZE")
            ar_port_ref = get_element_or_die(ar_mapping, "PORT-PROTOTYPE-IREF/TARGET-PORT-PROTOTYPE-REF")
            ar_port = self.model.find_referable(ar_port_ref.text)
            if ar_port_ref.get("DEST") == "P-PORT-PROTOTYPE":
                ar_port_interface_ref = get_element_or_die(ar_port, "PROVIDED-INTERFACE-TREF")
            if ar_port_ref.get("DEST") == "R-PORT-PROTOTYPE":
                ar_port_interface_ref = get_element_or_die(ar_port, "REQUIRED-INTERFACE-TREF")
            if ar_port_ref.get("DEST") == "PR-PORT-PROTOTYPE":
                ar_port_interface_ref = get_element_or_die(ar_port, "PROVIDED-REQUIRED-INTERFACE-TREF")
            ar_port_interface = self.model.find_referable(ar_port_interface_ref.text)
            if minimum_sustained_size is None:
                minimum_sustained_size = get_element_or_none(ar_port_interface, "MINIMUM-SUSTAINED-SIZE")
                if minimum_sustained_size is None:
                    minimum_sustained_size = 4096
            redundancy = self.redundancy_or_scope(get_element_or_none(ar_port_interface, "REDUNDANCY"))
            maximum_allowed_size = get_element_or_none(ar_kvs, "MAXIMUM-ALLOWED-SIZE")
            uri = get_element_or_none(ar_kvs, "URI")
            update_strategy = get_element_or_none(ar_kvs, "UPDATE-STRATEGY")
            interface_update_strategy = get_element_or_none(ar_port_interface, "UPDATE-STRATEGY")

            redundancy_info = {
                "redundancy_m_out_of_n": [],
                "redundancy_hash": [],
                "redundancy_crc": []
            }
            self._read_persistency_redundancy(ar_kvs, redundancy_info, self.redundancy_or_scope)
            if len(redundancy_info["redundancy_m_out_of_n"]) + len(redundancy_info["redundancy_hash"]) + len(redundancy_info["redundancy_crc"]) == 0:
                self._read_persistency_redundancy(ar_port_interface, redundancy_info, self.redundancy_or_scope)
            per_info = self._read_per_info(ar_kvs, ar_port, ar_port_interface)
            no_crypto = []
            element_crypto = []

            self._get_element_crypto_mapping(ar_kvs_ref, [], per_info, no_crypto, element_crypto)
            # storage.versionPer 2211 ar_kvs -> KEY-VALUE-STORAGE
            lastStrategy = update_strategy
            if lastStrategy is None:
                lastStrategy = interface_update_strategy
            result.append(
                {
                    "instance_specifier": get_instance_specifier(self, ar_mapping),
                    "name": ar_kvs.SHORT_NAME,
                    "storageIns": ar_kvs_ref.text[1:],
                    "port_type_raw": ar_port_ref.get("DEST"),
                    "no_crypto": no_crypto,
                    "element_crypto": element_crypto,
                    "storage": {
                        "name": ar_port_interface.SHORT_NAME,
                        "uri": to_str(uri),
                        "versionPer": None,
                        "update_strategy": to_str(lastStrategy),
                        "maximum_allowed_size": to_str(maximum_allowed_size),
                        "redundancy": redundancy,
                        "minimum_sustained_size": minimum_sustained_size,
                        "redundancy_handlings": redundancy_info
                    },
                    "per_info": per_info,
                    "appVersion": {
                        "version": appVersion,
                        "length": app_value_length,
                        "key_crc32": app_key_crc32,
                        "key_value_crc32": app_key_value_crc32,
                        "brackets_before": "{",
                        "brackets_after": "}"
                    },
                    "perVersion": {
                        "version": perVersion,
                        "length": per_value_length,
                        "key_crc32": per_key_crc32,
                        "key_value_crc32": per_key_value_crc32,
                        "brackets_before": "{",
                        "brackets_after": "}"
                    }
                }
            )

        return result

    def _read_persistency_file_mappings(self, ar_process):
        result = []

        ar_mappings = self.model.find_elements_of_type(
            'PERSISTENCY-PORT-PROTOTYPE-TO-FILE-STORAGE-MAPPING',
            accept=lambda e: str(
                e.PROCESS_REF) == ar_process.get_fqn())

        for ar_mapping in ar_mappings:
            ar_filearray_ref = get_element_or_die(ar_mapping, "FILE-STORAGE-REF")
            ar_filearray = self.model.find_referable(ar_filearray_ref.text)
            minimum_sustained_size = get_element_or_none(ar_filearray, "MINIMUM-SUSTAINED-SIZE")
            maximum_allowed_size = get_element_or_none(ar_filearray, "MAXIMUM-ALLOWED-SIZE")
            uri = get_element_or_none(ar_filearray, "URI")
            update_strategy = get_element_or_none(ar_filearray, "UPDATE-STRATEGY")
            ar_port_ref = get_element_or_die(ar_mapping, "PORT-PROTOTYPE-IREF/TARGET-PORT-PROTOTYPE-REF")
            ar_port = self.model.find_referable(ar_port_ref.text)
            if ar_port_ref.get("DEST") == "P-PORT-PROTOTYPE":
                ar_port_interface_ref = get_element_or_die(ar_port, "PROVIDED-INTERFACE-TREF")
            if ar_port_ref.get("DEST") == "R-PORT-PROTOTYPE":
                ar_port_interface_ref = get_element_or_die(ar_port, "REQUIRED-INTERFACE-TREF")
            if ar_port_ref.get("DEST") == "PR-PORT-PROTOTYPE":
                ar_port_interface_ref = get_element_or_die(ar_port, "PROVIDED-REQUIRED-INTERFACE-TREF")
            ar_port_interface = self.model.find_referable(ar_port_interface_ref.text)
            if minimum_sustained_size is None:
                minimum_sustained_size = get_element_or_none(ar_port_interface, "MINIMUM-SUSTAINED-SIZE")
                if minimum_sustained_size is None:
                    minimum_sustained_size = 4096
            redundancy = self.redundancy_or_scope(get_element_or_none(ar_port_interface, "REDUNDANCY"))
            max_number_of_files = transition_number(get_element_or_die(ar_port_interface, "MAX-NUMBER-OF-FILES"))
            redundancy_info = {
                "redundancy_m_out_of_n": [],
                "redundancy_hash": [],
                "redundancy_crc": []
            }
            self._read_persistency_redundancy(ar_filearray, redundancy_info, self.redundancy_or_scope)
            if len(redundancy_info["redundancy_m_out_of_n"]) + len(redundancy_info["redundancy_hash"]) + len(redundancy_info["redundancy_crc"]) == 0:
                self._read_persistency_redundancy(ar_port_interface, redundancy_info, self.redundancy_or_scope)

            persistency_file_elements = []
            interface_update_strategy = get_element_or_none(ar_port_interface, "UPDATE-STRATEGY")
            self._read_persistency_file_elements(ar_filearray, update_strategy, interface_update_strategy, persistency_file_elements)
            self._read_persistency_file_elements(ar_port_interface, update_strategy, interface_update_strategy, persistency_file_elements)

            element_crypto = []
            no_crypto = []
            self._get_element_crypto_mapping(ar_filearray_ref, persistency_file_elements, [], no_crypto, element_crypto)
            # storage.versionPer 2211 ar_filearray-->FILE-STORAGE
            lastStrategy = update_strategy
            if lastStrategy is None:
                lastStrategy = interface_update_strategy
            result.append(
                {
                    "instance_specifier": get_instance_specifier(self, ar_mapping),
                    "storageIns": ar_filearray_ref.text[1:],
                    "port_type_raw": ar_port_ref.get("DEST"),
                    "no_crypto": no_crypto,
                    "element_crypto": element_crypto,
                    "storage": {
                        "name": ar_port_interface.SHORT_NAME,
                        "uri": to_str(uri),
                        "versionPer": None,
                        "update_strategy": to_str(lastStrategy),
                        "maximum_allowed_size": to_str(maximum_allowed_size),
                        "redundancy": redundancy,
                        "minimum_sustained_size": minimum_sustained_size,
                        "redundancy_handlings": redundancy_info
                    },
                    "fileStorage": {
                        "max_number_of_files": max_number_of_files,
                        "init_file_list": persistency_file_elements
                    }

                }
            )

        return result

    def _read_persistency_file_elements(self, ar_storage, update_strategy, interface_update_strategy, fileList):
        if ar_storage is not None:
            ar_persistency_file_elements = get_element_or_none(ar_storage, "FILES/PERSISTENCY-FILE")
            if ar_persistency_file_elements is None:
                ar_persistency_file_elements = get_element_or_none(ar_storage, "FILE-ELEMENTS/PERSISTENCY-FILE-ELEMENT")
            ar_persistency_file_elements = to_list(ar_persistency_file_elements)
            for ar_persistency_file_element in ar_persistency_file_elements:
                shortName = ar_persistency_file_element.SHORT_NAME
                fileName = to_str(get_element_or_die(ar_persistency_file_element, "FILE-NAME"))
                contentUri = get_element_or_str(ar_persistency_file_element, "CONTENT-URI")
                new_list = copy.copy(fileList)
                haveIn = False
                for fileItem in new_list:
                    if fileName == fileItem["fileName"]:
                        haveIn = True
                        if shortName != fileItem["_shortName"]:
                            assert False, "[PER ] CODE-001: [{}, {}] fileName is the same, shortName must be the same.LocalPath:\"{}\",\"{}\".".format(ar_persistency_file_element.get_fqn(), fileItem["fqn"], ar_persistency_file_element.get_path(), fileItem["path"])
                            # fileList.remove(fileItem)
                        if fileItem["contentUri"] == "":
                            fileItem["contentUri"] = contentUri
                        if not fileItem["isFile"]:
                            updateStrategy = get_element_or_none(ar_persistency_file_element, "UPDATE-STRATEGY")
                            if updateStrategy is None:
                                updateStrategy = update_strategy
                            if updateStrategy is None:
                                updateStrategy = interface_update_strategy
                            if updateStrategy is None:
                                updateStrategy = ""
                            fileItem["updateStrategy"] = updateStrategy
                if not haveIn:
                    isFile = True
                    updateStrategy = get_element_or_none(ar_persistency_file_element, "UPDATE-STRATEGY")
                    if updateStrategy is None:
                        isFile = False
                        updateStrategy = update_strategy
                    if updateStrategy is None:
                        isFile = False
                        updateStrategy = interface_update_strategy
                    if updateStrategy is None:
                        isFile = False
                        updateStrategy = ""
                    fileList.append({
                        "_shortName": shortName,
                        "updateStrategy": updateStrategy,
                        "contentUri": contentUri,
                        "fileName": fileName,
                        "fqn": ar_persistency_file_element.get_fqn(),
                        "path": ar_persistency_file_element.get_path(),
                        "isFile": isFile
                    })

    def _read_persistency_redundancy(self, ar_storage, redundancy_info, redundancy_or_scope):
        if len(redundancy_info["redundancy_m_out_of_n"]) == 0:
            ar_redundancy_mns = get_element_or_none(ar_storage, "REDUNDANCY-HANDLINGS/PERSISTENCY-REDUNDANCY-M-OUT-OF-N")
            ar_redundancy_mns = to_list(ar_redundancy_mns)
            redundancy_mn = []
            for ar_redundancy_mn in ar_redundancy_mns:
                redundancy_mn.append({
                    "M": transition_number(get_element_or_die(ar_redundancy_mn, "M")),
                    "N": transition_number(get_element_or_die(ar_redundancy_mn, "N")),
                    "scope": redundancy_or_scope(get_element_or_none(ar_redundancy_mn, "SCOPE"))
                })
            redundancy_info["redundancy_m_out_of_n"] = redundancy_mn
        if len(redundancy_info["redundancy_crc"]) == 0:
            ar_redundancy_crcs = get_element_or_none(ar_storage, "REDUNDANCY-HANDLINGS/PERSISTENCY-REDUNDANCY-CRC")
            ar_redundancy_crcs = to_list(ar_redundancy_crcs)
            redundancy_crc = []
            for ar_redundancy_crc in ar_redundancy_crcs:
                redundancy_crc.append({
                    "algorithm_family": self.change_redundancy_family(ar_redundancy_crc),
                    "length": to_str(get_element_or_die(ar_redundancy_crc, "LENGTH")),
                    "scope": redundancy_or_scope(get_element_or_none(ar_redundancy_crc, "SCOPE"))
                })
            redundancy_info["redundancy_crc"] = redundancy_crc

        if len(redundancy_info["redundancy_hash"]) == 0:
            ar_redundancy_hash_list = get_element_or_none(ar_storage, "REDUNDANCY-HANDLINGS/PERSISTENCY-REDUNDANCY-HASH")
            ar_redundancy_hash_list = to_list(ar_redundancy_hash_list)
            redundancy_hash = []
            for ar_redundancy_hash in ar_redundancy_hash_list:
                redundancy_hash.append({
                    "algorithm_family": to_str(get_element_or_die(ar_redundancy_hash, "ALGORITHM-FAMILY")),
                    "length": to_str(get_element_or_die(ar_redundancy_hash, "LENGTH")),
                    "scope": redundancy_or_scope(get_element_or_none(ar_redundancy_hash, "SCOPE")),
                    "vector_length": transition_number(get_element_or_none(ar_redundancy_hash, "INITIALIZATION-VECTOR-LENGTH"), defaultInt=None)
                })
            redundancy_info["redundancy_hash"] = redundancy_hash

    def change_redundancy_family(self, ar_redundancy):
        ob = {
            "CRC_J1850": 8,
            "CRC_8H2F": 8,
            "CRC_CCITT_FALSE": 16,
            "CRC_16ARC": 16,
            "CRC_ETHERNET": 32,
            "CRC_32P4": 32,
            "CRC_0x42F0E1EBA9EA3693": 64
        }
        lengthList = {
            "CRC_J1850": "CRC8",
            "CRC_8H2F": "CRC8",
            "CRC_CCITT_FALSE": "CRC16",
            "CRC_16ARC": "CRC16",
            "CRC_ETHERNET": "CRC32",
            "CRC_32P4": "CRC32",
            "CRC_0x42F0E1EBA9EA3693": "CRC64"
        }
        family = to_str(get_element_or_die(ar_redundancy, "ALGORITHM-FAMILY"))
        length = transition_number(get_element_or_die(ar_redundancy, "LENGTH"))
        if family in ob:
            if length == ob[family]:
                return lengthList[family]
        assert False, "[PER ] CODE-002: The PersistencyRedundancyCrc[{}] attribute [algorithmFamily({}):length({})] combination must be combined in [ CRC_J1850:8, CRC_8H2F:8, CRC_CCITT_FALSE:16, CRC_16ARC:16, CRC_ETHERNET:32, CRC_32P4:32, CRC_0x42F0E1EBA9EA3693:64 ]".format(ar_redundancy.get_fqn(), family, length)

    def _get_element_crypto_mapping(self, ar_ref, persistency_file_elements, per_info, no_crypto, element_crypto):
        no_crypto_mapping = self.model.find_elements_of_type('PERSISTENCY-DEPLOYMENT-TO-CRYPTO-KEY-SLOT-MAPPING', PERSISTENCY_DEPLOYMENT_REF=ar_ref.text)
        for item in no_crypto_mapping:
            cryptoKeySlot = get_element_or_str(item, "CRYPTO-KEY-SLOT-REF")
            if len(cryptoKeySlot) > 0:
                cryptoKeySlot = to_str(cryptoKeySlot)[1:]
            persistencyDeployment = get_element_or_str(item, "PERSISTENCY-DEPLOYMENT-REF")
            if len(persistencyDeployment) > 0:
                persistencyDeployment = to_str(persistencyDeployment)[1:]
            no_crypto.append({
                "keySlotIns": cryptoKeySlot,
                "keySlotUsage": get_element_or_str(item, "KEY-SLOT-USAGE"),
                "persistencyDeployment": persistencyDeployment,
                "verificationHash": get_element_or_str(item, "VERIFICATION-HASH")
            })
        element_crypto_mapping = self.model.find_elements_of_type('PERSISTENCY-DEPLOYMENT-ELEMENT-TO-CRYPTO-KEY-SLOT-MAPPING')
        for item in element_crypto_mapping:
            persistencyDeploymentElement = get_element_or_none(item, "PERSISTENCY-DEPLOYMENT-ELEMENT-REF")
            if persistencyDeploymentElement is not None:
                fqn = persistencyDeploymentElement.text
                elementIns = ""
                for file_element in persistency_file_elements:
                    if fqn == file_element["fqn"]:
                        elementIns = file_element["fileName"]
                        break
                for per_info_item in per_info:
                    if fqn == per_info_item["key_fqn"]:
                        elementIns = per_info_item["key_name"]
                        break
                if elementIns != "":
                    cryptoKeySlot = get_element_or_str(item, "CRYPTO-KEY-SLOT-REF")
                    if len(cryptoKeySlot) > 0:
                        cryptoKeySlot = to_str(cryptoKeySlot)[1:]
                    element_crypto.append({
                        "keySlotIns": cryptoKeySlot,
                        "keySlotUsage": get_element_or_str(item, "KEY-SLOT-USAGE"),
                        "storageIns": ar_ref.text[1:],
                        "persistencyDeploymentElement": elementIns,
                        "verificationHash": get_element_or_str(item, "VERIFICATION-HASH")
                    })

    def redundancy_or_scope(self, inType):
        if inType is None:
            return ""
        elif inType == "NONE":
            return "none"
        elif inType in ["REDUNDANT", "PERSISTENCY-REDUNDANCY-HANDLING-SCOPE-STORAGE"]:
            return "redundant"
        elif inType in ["REDUNDANT-PER-ELEMENT", "PERSISTENCY-REDUNDANCY-HANDLING-SCOPE-ELEMENT"]:
            return "redundantPerElement"

    def _read_per_info(self, ar_kvs, port, interface):
        per_error = []
        per_info = []
        comSpec = get_element_or_none(port, "REQUIRED-COM-SPECS/PERSISTENCY-DATA-REQUIRED-COM-SPEC")
        data_element = get_element_or_none(interface, "DATA-ELEMENTS/PERSISTENCY-DATA-ELEMENT")
        interface_update_strategy = get_element_or_none(interface, "UPDATE-STRATEGY")
        kvs_update_strategy = get_element_or_none(ar_kvs, "UPDATE-STRATEGY")
        index = 2
        if data_element is not None:
            data_element = to_list(data_element)
            for data_element_item in data_element:
                fqn = data_element_item.get_fqn()
                kv_item = None
                if comSpec is not None:
                    comSpec = to_list(comSpec)
                    for item in comSpec:
                        dataEleRef = get_element_or_die(item, "DATA-ELEMENT-REF")
                        if fqn == dataEleRef.text:
                            kv_item = item
                            break
                if kv_item is not None:
                    self.add_per_info(data_element_item, kv_item, index, per_info, per_error, interface_update_strategy, kvs_update_strategy, False)
                    index += 1
                else:
                    per_error.append({
                        "key_name": data_element_item.SHORT_NAME,
                        "error_msg": "[PER ] CODE-004: INIT-VALUE no configuration or misconfiguration. FQN:{}. LocalPath:{}.".format(fqn, data_element_item.get_path()),
                    })
        ar_key_value_pairs = get_element_or_none(ar_kvs, "KEY-VALUE-PAIRS/PERSISTENCY-KEY-VALUE-PAIR")
        if ar_key_value_pairs is not None:
            ar_key_value_pairs = to_list(ar_key_value_pairs)
            for ar_key_value_pair in ar_key_value_pairs:
                self.add_per_info(ar_key_value_pair, ar_key_value_pair, index, per_info, per_error, interface_update_strategy, kvs_update_strategy, True)
                index += 1
        for per in per_info:
            if "update_strategy" not in per or per["update_strategy"] is None:
                per_error.append({
                    "key_name": per["key_name"],
                    "error_msg": "[PER ] CODE-005: UPDATE-STRATEGY no configuration or misconfiguration. FQN:{}.".format(per["key_fqn"]),
                })
        assert len(per_error) == 0, "\n".join([item["error_msg"] for item in per_error])
        return per_info

    def add_per_info(self, ar_key_value_pair, comSpec, index, per_info, per_error, interface_update_strategy, kvs_update_strategy, isStorage):
        ar_update_strategy = get_element_or_none(ar_key_value_pair, "UPDATE-STRATEGY")
        ar_value_data_type_ref = get_element_or_none(ar_key_value_pair, "VALUE-DATA-TYPE-REF")
        if ar_value_data_type_ref is None:
            ar_value_data_type_ref = get_element_or_none(ar_key_value_pair, "TYPE-TREF")
        fqn = ar_key_value_pair.get_fqn()
        ar_key = ar_key_value_pair.SHORT_NAME
        itemPerInfo = {}
        isIn = False
        for item in per_info:
            if item["key_name"] == ar_key:
                itemPerInfo = item
                isIn = True
                break
        if len(per_error) > 0:
            new_per_error = copy.copy(per_error)
            for error in new_per_error:
                if error["key_name"] == ar_key:
                    per_error.remove(error)
        key_crc32 = zlib.crc32(bytes(ar_key, encoding='utf-8'))
        if ar_value_data_type_ref is not None:
            if ar_value_data_type_ref.text == "/ISOFT/SharedTypes/String":
                ar_text_value_specification = get_element_or_none(comSpec, "INIT-VALUE/TEXT-VALUE-SPECIFICATION")
                if ar_text_value_specification is not None:
                    ar_value = get_element_or_none(ar_text_value_specification, "VALUE")
                    if ar_value is not None:
                        ar_value = to_str(ar_value)
                        i = 0
                        value_encoding = ""
                        while i < len(ar_value):
                            nRandom = (index + 1) * 16 + (i + 1)
                            value_encoding += self.encodingFunc(ord(ar_value[i]), key_crc32, nRandom)
                            i += 1
                        key_value_crc32 = zlib.crc32(bytes(ar_key + ar_value, encoding='utf-8'))
                        self.set_kvs_update_strategy(itemPerInfo, isStorage, ar_update_strategy, kvs_update_strategy, interface_update_strategy)
                        itemPerInfo["key_name"] = ar_key
                        itemPerInfo["key_fqn"] = fqn
                        itemPerInfo["value_crc32"] = value_encoding
                        itemPerInfo["brackets_before"] = "{"
                        itemPerInfo["brackets_after"] = "}"
                        itemPerInfo["key_value_crc32"] = key_value_crc32
                        itemPerInfo["value_type"] = "c"
                        itemPerInfo["value_length"] = len(ar_value)
                        if not isIn:
                            per_info.append(itemPerInfo)
                    elif not isIn:
                        per_error.append({
                            "key_name": ar_key,
                            "error_msg": "[PER ] CODE-004: INIT-VALUE no configuration or misconfiguration. FQN:{}. LocalPath:{}.".format(fqn, ar_key_value_pair.get_path()),
                        })
                elif not isIn:
                    per_error.append({
                        "key_name": ar_key,
                        "error_msg": "[PER ] CODE-004: INIT-VALUE no configuration or misconfiguration. FQN:{}. LocalPath:{}.".format(fqn, ar_key_value_pair.get_path()),
                    })
            else:
                ar_value = get_element_or_none(comSpec, "INIT-VALUE/NUMERICAL-VALUE-SPECIFICATION/VALUE")
                if ar_value is not None:
                    ar_num_array = []
                    self.changeNumToArray(itemPerInfo, ar_value_data_type_ref, ar_value, ar_num_array)
                    self.set_kvs_update_strategy(itemPerInfo, isStorage, ar_update_strategy, kvs_update_strategy, interface_update_strategy)
                    itemPerInfo["key_name"] = ar_key
                    itemPerInfo["key_fqn"] = fqn
                    value_crc32 = ""
                    i = 0
                    for num in ar_num_array:
                        nRandom = (index + 1) * 16 + (i + 1)
                        value_crc32 += self.encodingFunc(num, key_crc32, nRandom)
                        i += 1
                    itemPerInfo["value_crc32"] = value_crc32
                    itemPerInfo["brackets_before"] = "{"
                    itemPerInfo["brackets_after"] = "}"
                    itemPerInfo["key_value_crc32"] = self._encodingNumKeyValueCrc32(key_crc32, ar_num_array)
                    if not isIn:
                        per_info.append(itemPerInfo)
                elif not isIn:
                    per_error.append({
                        "key_name": ar_key,
                        "error_msg": "[PER ] CODE-004: INIT-VALUE no configuration or misconfiguration. FQN:{}. LocalPath:{}.".format(fqn, ar_key_value_pair.get_path()),
                    })
        elif not isIn:
            per_error.append({
                "key_name": ar_key,
                "error_msg": "[PER ] CODE-006: TYPE-REF no configuration. FQN:{}. LocalPath:{}.".format(fqn, ar_key_value_pair.get_path()),
            })

    def set_kvs_update_strategy(self, itemPerInfo, isStorage, ar_update_strategy, kvs_update_strategy, interface_update_strategy):
        if ar_update_strategy is not None:
            itemPerInfo["update_strategy"] = ar_update_strategy
        elif kvs_update_strategy is not None:
            itemPerInfo["update_strategy"] = kvs_update_strategy
        elif interface_update_strategy is not None:
            itemPerInfo["update_strategy"] = interface_update_strategy

    def _encodingNumKeyValueCrc32(self, key_crc32, ar_num_array):
        crc32_table = [
            0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
            0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
            0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
            0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
            0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
            0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
            0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
            0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
            0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
            0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
            0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
            0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
            0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
            0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
            0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
            0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
            0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
            0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
            0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
            0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
            0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
            0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
            0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
            0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
            0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
            0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
            0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
            0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
            0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
            0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
            0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
            0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
            0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
            0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
            0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
            0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
            0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
            0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
            0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
            0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
            0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
            0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
            0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
            0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
            0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
            0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
            0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
            0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
            0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
            0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
            0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
            0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
            0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
            0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
            0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
            0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
            0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
            0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
            0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
            0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
            0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
            0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
            0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
            0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D]
        crc = key_crc32 ^ 0xFFFFFFFF
        for num in ar_num_array:
            crc = crc32_table[(crc ^ num) & 0xFF] ^ (crc >> 8)
        return crc ^ 0xFFFFFFFF

    def changeNumToArray(self, result, ar_value_data_type_ref, ar_value, ar_num_array):
        if ar_value_data_type_ref.text == "/AUTOSAR/StdTypes/bool":
            result["value_type"] = 9
            result["value_length"] = 1
            self.changeNumToRound(ar_value, ar_num_array, 1)
        if ar_value_data_type_ref.text == "/AUTOSAR/StdTypes/int8_t":
            result["value_type"] = 1
            result["value_length"] = 1
            self.changeNumToRound(ar_value, ar_num_array, 1)
        if ar_value_data_type_ref.text == "/AUTOSAR/StdTypes/int16_t":
            result["value_type"] = 2
            result["value_length"] = 2
            self.changeNumToRound(ar_value, ar_num_array, 2)
        if ar_value_data_type_ref.text == "/AUTOSAR/StdTypes/int32_t":
            result["value_type"] = 3
            result["value_length"] = 4
            self.changeNumToRound(ar_value, ar_num_array, 4)
        if ar_value_data_type_ref.text == "/AUTOSAR/StdTypes/int64_t":
            result["value_type"] = 4
            result["value_length"] = 8
            self.changeNumToRound(ar_value, ar_num_array, 8)
        if ar_value_data_type_ref.text == "/AUTOSAR/StdTypes/uint8_t":
            result["value_type"] = 5
            result["value_length"] = 1
            self.changeNumToRound(ar_value, ar_num_array, 1)
        if ar_value_data_type_ref.text == "/AUTOSAR/StdTypes/uint16_t":
            result["value_type"] = 6
            result["value_length"] = 2
            self.changeNumToRound(ar_value, ar_num_array, 2)
        if ar_value_data_type_ref.text == "/AUTOSAR/StdTypes/uint32_t":
            result["value_type"] = 7
            result["value_length"] = 4
            self.changeNumToRound(ar_value, ar_num_array, 4)
        if ar_value_data_type_ref.text == "/AUTOSAR/StdTypes/uint64_t":
            result["value_type"] = 8
            result["value_length"] = 8
            self.changeNumToRound(ar_value, ar_num_array, 8)
        if ar_value_data_type_ref.text == "/AUTOSAR/StdTypes/float":
            result["value_type"] = "a"
            result["value_length"] = 4
            self.changeNumToRound(ar_value, ar_num_array, 4)
        if ar_value_data_type_ref.text == "/AUTOSAR/StdTypes/double":
            result["value_type"] = "b"
            result["value_length"] = 8
            self.changeNumToRound(ar_value, ar_num_array, 8)

    def changeNumToRound(self, ar_value, ar_num_array, time):
        i = 1
        while i <= time:
            remainder_value = ar_value % 256
            ar_num_array.append(remainder_value)
            ar_value = int((ar_value - remainder_value) / 256)
            i += 1

    def encodingFunc(self, input_value, key_crc32, nRandom):
        input_value ^= (key_crc32 % 256)
        stResult = ""
        nKey1 = 16 - (nRandom % 16)
        nKey2 = nRandom % 16
        stResult += str(self._EncodeChar((int(input_value / 16) ^ nKey1)))
        stResult += str(self._EncodeChar((int(input_value % 16) ^ nKey2)))
        return stResult

    def _EncodeChar(self, chInput):
        chInput %= 16
        result = ""
        if chInput >= 10:
            result = chr(97 + (chInput - 10))
        else:
            result = chInput
        return result

    def _read_command_line(self, ar_startup_config):
        arguments = []
        ar_args_mapping = ar_startup_config.find_elements_of_type("PROCESS-ARGUMENT")
        for ar_arg in ar_args_mapping:
            argument_str = get_element_or_str(ar_arg, "ARGUMENT")
            arguments.append(argument_str)
        return arguments

    def get_time_base_instance_specifier(self, ar_endpoint_mapping):
        port_prototype = get_element_or_none(ar_endpoint_mapping, "TIME-SYNC-P-PORT-PROTOTYPE-IREF")
        if port_prototype is None:
            port_prototype = get_element_or_none(ar_endpoint_mapping, "TIME-SYNC-R-PORT-PROTOTYPE-IREF")
        return commen_util.get_instance_specifier_port(self, ar_endpoint_mapping, port_prototype)

    def read_time_base_mappings(self, ar_process):
        time_base_mappings = []
        time_sync_process_mappings = self.model.find_elements_of_type("TIME-SYNC-PORT-PROTOTYPE-TO-TIME-BASE-MAPPING",
                                                                      PROCESS_REF=ar_process.get_fqn())
        if time_sync_process_mappings:
            for mapping in time_sync_process_mappings:
                time_base_mapping = {
                    "instanceSpecifier": self.get_time_base_instance_specifier(mapping)
                }
                timeBase = get_element_or_none(mapping, "TIME-BASE-RESOURCE-REF")
                if timeBase is not None:
                    time_base_mapping["timeBase"] = timeBase.text[1:]
                else:
                    time_base_mapping["timeBase"] = ""
                time_base_mappings.append(time_base_mapping)
        return time_base_mappings

    def read_timeout(self, ar_timeout):
        ar_enter_timeout = get_element_or_none(ar_timeout, "TIMEOUT/ENTER-TIMEOUT-VALUE")
        if ar_enter_timeout is None:
            ar_enter_timeout = 0
        ar_exit_timeout = get_element_or_none(ar_timeout, "TIMEOUT/EXIT-TIMEOUT-VALUE")
        if ar_exit_timeout is None:
            ar_exit_timeout = 0
        return {
            "enter_timeout_ns": to_nanoseconds(ar_enter_timeout),
            "exit_timeout_ns": to_nanoseconds(ar_exit_timeout)
        }
