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

from generator.intermediate_model import Machine
from generator.parser import commen_util
from generator.common.autosar_mapping import PathError
from generator.parser.crypto_builder import CryptoBuilder
from generator.parser.iam_builder import IAMBuilder
from generator.parser.idsm_builder import IDSMBuilder
from generator.parser.log_trace_builder import LogTraceBuilder
from generator.parser.nm_deployment_builder import NMDeploymentBuilder
from generator.common.tree_helper import get_element_or_none, short_name, to_str, to_list, get_element_or_str, get_element_or_0, transition_number, get_element_or_die
from generator.views.machine_view import MachineView


class MachineBuilder:
    def __init__(self, model, _processBuilder, _phmdeploymentbuilder):
        self._log = logging.getLogger(__name__)
        self.model = model
        self._processBuilder = _processBuilder
        self._phmdeploymentbuilder = _phmdeploymentbuilder
        self._iamBuilder = IAMBuilder(self.model)
        self._idsmBuilder = IDSMBuilder(self.model)
        self._cryptoBuilder = CryptoBuilder(self.model)
        self._logTraceBuilder = LogTraceBuilder(self.model)
        self._nmdeploymentbuilder = NMDeploymentBuilder(self.model)

    def get_machines_views(self, machines_list=None):
        ar_machines = self.model.find_elements_of_type('MACHINE')
        machine_views = []
        for ar_machine in ar_machines:
            machine_fqn = ar_machine.get_fqn()
            if machine_fqn in machines_list:
                machine = Machine(machine_fqn, configuration=self.read_machine_config(ar_machine))
                self._processBuilder.get_machine_design(ar_machine, machine)
                self.get_machine_processes(ar_machine, machine)

                machine_view = MachineView(machine)
                machine_view._nm_contribution = self._nmdeploymentbuilder.get_nm_deployments(ar_machine)
                machine_view.iam_phm = self._iamBuilder.get_iam_for_phm_infos(ar_machine)
                machine_view.iam_crypto = self._iamBuilder.get_iam_for_crypto_infos(ar_machine)
                machine_view.iam_com = self._iamBuilder.get_iam_com(ar_machine, machine_view.provided_services_list)
                machine_view.iam_raw = self._iamBuilder.get_iam_raw(ar_machine)
                machine_view.iam_idsm = self._iamBuilder.get_iam_for_idsm(ar_machine)
                machine_view._crypto_contribution = self._cryptoBuilder.get_all_crypto(ar_machine)
                machine_view.idsm = self._idsmBuilder.get_idsm_contribute(ar_machine)
                machine_view.emd_log = self._logTraceBuilder.get_log_config_emd(ar_machine)
                machine_view.log_global_config = self._logTraceBuilder.get_log_global_config(ar_machine)
                machine_view.phm_contribution = self._phmdeploymentbuilder.get_phm_deployments(machine_fqn)
                machine_view.ide_info = self.get_ide_info(ar_machine)
                machine_view.networkConfig = self.get_network_config(ar_machine)
                machine_views.append(machine_view)

        return machine_views

    def get_ide_info(self, ar_machine):
        machineDesignRef = get_element_or_none(ar_machine, "MACHINE-DESIGN-REF")
        if machineDesignRef is not None:
            machineDesign = self.model.find_referable(machineDesignRef.text)
            ipList = []
            connectorList = machineDesign.find_elements_of_type('ETHERNET-COMMUNICATION-CONNECTOR')
            for connector in connectorList:
                endpointRef = get_element_or_none(connector, "UNICAST-NETWORK-ENDPOINT-REF")
                endpoint_info = self.model.find_referable(endpointRef.text)
                ipaddress = commen_util.get_network_address(endpoint_info, isAssert=False)
                if ipaddress != "" and ipaddress not in ipList:
                    ipList.append(ipaddress)
            if len(ipList) > 0:
                return {
                    'ipAddresses': ipList
                }
        return None


    def get_network_config(self, ar_machine):
        sdgList = get_element_or_none(ar_machine, "ADMIN-DATA/SDGS/SDG")
        if sdgList is not None:
            for sdg in sdgList:
                if sdg.attrib["GID"] == "iSOFT:com:SomeipdNetworkConfig":
                    config = {}
                    sdList = to_list(get_element_or_none(sdg, "SD"))
                    for sd in sdList:
                        if sd.attrib["GID"] == "iSOFT:com:SomeipdNetworkConfig:SendTimeout":
                            config["sendTimeout"] = sd.text
                        elif sd.attrib["GID"] == "iSOFT:com:SomeipdNetworkConfig:RecvTimeout":
                            config["recvTimeout"] = sd.text
                        elif sd.attrib["GID"] == "iSOFT:com:SomeipdNetworkConfig:QueueMixLimit":
                            config["queueMixLimit"] = sd.text
                        elif sd.attrib["GID"] == "iSOFT:com:SomeipdNetworkConfig:TcpKeepaliveEnable":
                            config["tcpEnable"] = "1" if sd.text.lower() == "true" else "0"
                        elif sd.attrib["GID"] == "iSOFT:com:SomeipdNetworkConfig:TcpKeepaliveIdle":
                            config["tcpIdle"] = sd.text
                        elif sd.attrib["GID"] == "iSOFT:com:SomeipdNetworkConfig:TcpKeepaliveInterval":
                            config["tcpInterval"] = sd.text
                        elif sd.attrib["GID"] == "iSOFT:com:SomeipdNetworkConfig:TcpKeepaliveCnt":
                            config["tcpCnt"] = sd.text
                    return config
        return None


    def get_machine_processes(self, ar_machine, machine):
        """Get the processes contained in a ar_machine"""
        ar_processes = []
        proc_to_machine_mappings = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING")
        if proc_to_machine_mappings:
            for mapping in proc_to_machine_mappings:
                ar_machine_ref = get_element_or_none(mapping, "MACHINE-REF")
                if ar_machine_ref is not None and ar_machine_ref.text == ar_machine.get_fqn():
                    try:
                        ar_process = self.model.find_referable(mapping.PROCESS_REF)
                    except PathError:
                        self._log.warning("PROCESS Ref {} used in machine {} is not found! Mapping will be ignored".
                                          format(mapping.PROCESS_REF, ar_machine))
                        continue
                    ar_processes.append(ar_process)
        for ar_process in ar_processes:
            si_mapping = self._processBuilder.read_si_mappings(ar_process)
            machine.si_mappings.append(si_mapping)
            executable_ref = get_element_or_none(ar_process, "EXECUTABLE-REF")
            if executable_ref is not None and short_name(executable_ref.text) == "nsomeipd":
                nsomeipdProcessName = ar_process.SHORT_NAME
                machine.nsomeipdProcessName = nsomeipdProcessName

    def read_machine_config(self, ar_machine):
        result = {}
        ar_default_timeout = get_element_or_none(ar_machine, "DEFAULT-APPLICATION-TIMEOUT")
        result["default"] = self.read_default_timeout(ar_default_timeout)
        processor_core_mapping = ar_machine.find_elements_of_type("PROCESSOR-CORE")
        processor = []
        for processor_core in processor_core_mapping:
            core_id = transition_number(get_element_or_0(processor_core, "CORE-ID"))
            processor.append(core_id)
        result["processor"] = to_list(processor)
        behavior = get_element_or_str(ar_machine, "TRUSTED-PLATFORM-EXECUTABLE-LAUNCH-BEHAVIOR")
        # if behavior == "MONITOR-MODE":
        #     behavior = "Monitor"
        # elif behavior == "NO-TRUSTED-PLATFORM-SUPPORT":
        #     behavior = "NoSupport"
        # elif behavior == "STRICT-MODE":
        #     behavior = "Strict"
        # else:
        #     behavior = ""
        result["launchBehavior"] = behavior
        optional_mapping = ar_machine.find_elements_of_type("TAG-WITH-OPTIONAL-VALUE")
        variables = []
        for optional in optional_mapping:
            key = to_str(get_element_or_str(optional, "KEY"))
            value = to_str(get_element_or_str(optional, "VALUE"))
            variables.append(key + "=" + value)
        result["environmentVariable"] = to_list(variables)
        ar_log_trace_instantiation = get_element_or_none(ar_machine,
                                                         "MODULE-INSTANTIATIONS/LOG-AND-TRACE-INSTANTIATION")
        if ar_log_trace_instantiation is not None:
            ar_log_trace_instantiation = to_list(ar_log_trace_instantiation)
            ecu_id = None
            for log_item in ar_log_trace_instantiation:
                ar_dlt_ecu_id = get_element_or_none(log_item, "DLT-ECU-ID")
                if ar_dlt_ecu_id:
                    if ecu_id is None:
                        ecu_id = ar_dlt_ecu_id
                        result["ecu_id"] = ar_dlt_ecu_id.text
                    else:
                        assert ar_dlt_ecu_id == ecu_id, "DLT-ECU-ID different {}::{}".format(ar_dlt_ecu_id, ecu_id)

        resource_groups = []
        ar_os_instantiations = get_element_or_none(ar_machine, "MODULE-INSTANTIATIONS/OS-MODULE-INSTANTIATION")
        ar_os_instantiations = to_list(ar_os_instantiations)
        for ar_os_instantiation in ar_os_instantiations:
            ar_resource_groups = get_element_or_none(ar_os_instantiation, "RESOURCE-GROUPS/RESOURCE-GROUP")
            ar_resource_groups = to_list(ar_resource_groups)
            for ar_resource_group in ar_resource_groups:
                resource_groups.append({
                    "name": ar_resource_group.SHORT_NAME,
                    "cpu_usage": ar_resource_group.CPU_USAGE,
                    "mem_usage": ar_resource_group.MEM_USAGE
                })
        result["resource_groups"] = sorted(resource_groups, key=lambda x: x["name"])
        ucm_identifier = get_element_or_none(ar_machine, "MODULE-INSTANTIATIONS/UCM-MODULE-INSTANTIATION/IDENTIFIER")
        result["ucm_identifier"] = to_str(ucm_identifier)
        result["time_bases"] = self.read_time_base(ar_machine, result)
        return result

    def read_default_timeout(self, ar_timeout):
        ar_enter_timeout = get_element_or_none(ar_timeout, "ENTER-TIMEOUT-VALUE")
        if ar_enter_timeout is None:
            ar_enter_timeout = 0
        ar_exit_timeout = get_element_or_none(ar_timeout, "EXIT-TIMEOUT-VALUE")
        if ar_exit_timeout is None:
            ar_exit_timeout = 0
        return {
            "enter_timeout_ns": ar_enter_timeout,
            "exit_timeout_ns": ar_exit_timeout
        }

    def read_time_base(self, ar_machine, result):
        time_bases = []
        global_time_domains = []
        sync_time = []
        time_sync_module_instantiation = get_element_or_none(ar_machine, "MODULE-INSTANTIATIONS/TIME-SYNC-MODULE-INSTANTIATION")
        if time_sync_module_instantiation is not None:
            time_sync_module_instantiation = to_list(time_sync_module_instantiation)
            for time_sync_module_item in time_sync_module_instantiation:
                ar_time_base = get_element_or_none(time_sync_module_item, "TIME-BASES")
                if ar_time_base is not None:
                    ar_synchronized_time_base_consumers = get_element_or_none(ar_time_base, "SYNCHRONIZED-TIME-BASE-CONSUMER")
                    if ar_synchronized_time_base_consumers is not None:
                        ar_synchronized_time_base_consumers = to_list(ar_synchronized_time_base_consumers)
                        for ar_synchronized_time_base_consumer in ar_synchronized_time_base_consumers:
                            time_base = {
                                "instanceSpecifier": ar_synchronized_time_base_consumer.get_fqn()[1:],
                                "type": "consumer"
                            }
                            self.get_time_base_storage(ar_synchronized_time_base_consumer.get_fqn(), time_base)
                            ar_network_time_consumer_ref = get_element_or_none(ar_synchronized_time_base_consumer, "NETWORK-TIME-CONSUMER-REF")
                            if ar_network_time_consumer_ref is not None:
                                sync_time.append(ar_network_time_consumer_ref.text)
                                time_base["networkTimeConsumer"] = ar_network_time_consumer_ref.text
                                globalTimeDomain = ar_network_time_consumer_ref.text[0: ar_network_time_consumer_ref.text.rindex('/')]
                                ar_global_time_domain = self.model.find_referable(globalTimeDomain)
                                ar_global_time_eth_master = get_element_or_none(ar_global_time_domain, "GLOBAL-TIME-MASTERS/GLOBAL-TIME-ETH-MASTER")
                                globalTimeDomainMaster = globalTimeDomain + "/" + ar_global_time_eth_master.SHORT_NAME
                                global_time_domains.append(globalTimeDomain)
                                ar_synchronized_time_base_provider = []
                                ar_synchronized_time_base_provider_list = self.model.find_elements_of_type("SYNCHRONIZED-TIME-BASE-PROVIDER")
                                for provider in ar_synchronized_time_base_provider_list:
                                    provider_ref = get_element_or_none(provider, "NETWORK-TIME-PROVIDER-REF")
                                    if provider_ref is not None and provider_ref.text == globalTimeDomainMaster:
                                        ar_synchronized_time_base_provider.append(provider)

                                if len(ar_synchronized_time_base_provider) > 0:
                                    if len(ar_synchronized_time_base_provider) > 1:
                                        fqnList = [item.get_fqn() for item in ar_synchronized_time_base_provider]
                                        localPathList = [item.get_path() for item in ar_synchronized_time_base_provider]
                                        errorStr = "[TSYNC] CODE-001: GlobalTimeEthMaster is referenced by more than one SynchronizedTimeBaseProvider.networkTimeProviderRef.FqnList: {}. LocalPath: {}".format(", ".join(fqnList), localPathList)
                                        assert False, errorStr
                                    ar_time_sync_correction = get_element_or_none(ar_synchronized_time_base_provider[0], "TIME-SYNC-CORRECTION")
                                    if ar_time_sync_correction is not None:
                                        time_base["timeSyncCorrection"] = self.get_time_sync_correction(ar_time_sync_correction)

                            time_bases.append(time_base)
                    ar_synchronized_time_base_providers = get_element_or_none(ar_time_base, "SYNCHRONIZED-TIME-BASE-PROVIDER")
                    if ar_synchronized_time_base_providers is not None:
                        ar_synchronized_time_base_providers = to_list(ar_synchronized_time_base_providers)
                        for ar_synchronized_time_base_provider in ar_synchronized_time_base_providers:
                            time_base = {
                                "instanceSpecifier": ar_synchronized_time_base_provider.get_fqn()[1:],
                                "type": "provider"
                            }
                            self.get_time_base_storage(ar_synchronized_time_base_provider.get_fqn(), time_base)
                            ar_network_time_provider_ref = get_element_or_none(ar_synchronized_time_base_provider, "NETWORK-TIME-PROVIDER-REF")

                            if ar_network_time_provider_ref is not None:
                                sync_time.append(ar_network_time_provider_ref.text)
                                time_base["networkTimeProvider"] = ar_network_time_provider_ref.text
                                globalTimeDomain = ar_network_time_provider_ref.text[0: ar_network_time_provider_ref.text.rindex('/')]
                                global_time_domains.append(globalTimeDomain)
                            ar_time_sync_correction = get_element_or_none(ar_synchronized_time_base_provider, "TIME-SYNC-CORRECTION")
                            if ar_time_sync_correction is not None:
                                time_base["timeSyncCorrection"] = self.get_time_sync_correction(ar_time_sync_correction)
                            time_bases.append(time_base)

        global_time_domains = to_list(set(global_time_domains))
        globalTimeDomains = []
        for global_time_domain in global_time_domains:
            globalTimeDomains.append(self.model.find_referable(global_time_domain))
        result["golbalTimeDomains"] = self.read_global_time_domain(globalTimeDomains, sync_time)
        return time_bases

    def get_time_sync_correction(self, ar_time_sync_correction):
        timeSyncCorrection = {
            "allowProviderRateCorrection": get_element_or_die(ar_time_sync_correction, "ALLOW-PROVIDER-RATE-CORRECTION"),
            "offsetCorrectionAdaptionInterval": get_element_or_die(ar_time_sync_correction, "OFFSET-CORRECTION-ADAPTION-INTERVAL").text,
            "offsetCorrectionJumpThreshold": get_element_or_die(ar_time_sync_correction, "OFFSET-CORRECTION-JUMP-THRESHOLD").text,
            "rateCorrectionsPerMeasurementDuration": transition_number(get_element_or_die(ar_time_sync_correction, "RATE-CORRECTIONS-PER-MEASUREMENT-DURATION")),
            "rateDeviationMeasurementDuration": get_element_or_die(ar_time_sync_correction, "RATE-DEVIATION-MEASUREMENT-DURATION").text
        }
        return timeSyncCorrection

    def read_global_time_domain(self, ar_global_time_domains, sync_time):
        result = []
        if ar_global_time_domains:
            for ar_global_time_domain in ar_global_time_domains:
                globalTimeDomain = {
                    "name": ar_global_time_domain.SHORT_NAME,
                    "debounceTime": get_element_or_die(ar_global_time_domain, "DEBOUNCE-TIME").text,
                    "domainId": get_element_or_die(ar_global_time_domain, "DOMAIN-ID").text,
                    "syncLossTimeout": get_element_or_die(ar_global_time_domain, "SYNC-LOSS-TIMEOUT").text
                }
                sdgList = to_list(get_element_or_none(ar_global_time_domain, "ADMIN-DATA/SDGS/SDG"))
                for sdgItem in sdgList:
                    if sdgItem.attrib["GID"] == "iSOFT:tsync:GlobalTimeDomainExtension":
                        sdList = sdgItem.find_elements_of_type("SD")
                        for sdItem in sdList:
                            if "iSOFT:tsync:GlobalTimeDomainExtension:TimeStampType" == sdItem.attrib["GID"]:
                                globalTimeDomain["timeStampType"] = sdItem.text
                            elif "iSOFT:tsync:GlobalTimeDomainExtension:MasterSlaveConflictDetection" == sdItem.attrib["GID"]:
                                globalTimeDomain["masterSlaveConflictDetection"] = sdItem.text
                            elif "iSOFT:tsync:GlobalTimeDomainExtension:CrcTimeFlagsTxSecured" == sdItem.attrib["GID"]:
                                globalTimeDomain["crcTimeFlagsTxSecured"] = sdItem.text
                            elif "iSOFT:tsync:GlobalTimeDomainExtension:GlobalTimeSequenceCounterJumpWidth" == sdItem.attrib["GID"]:
                                globalTimeDomain["globalTimeSequenceCounterJumpWidth"] = sdItem.text
                            elif "iSOFT:tsync:GlobalTimeDomainExtension:UseSteadyClock" == sdItem.attrib["GID"]:
                                globalTimeDomain["steadyClock"] = sdItem.text
                            elif "iSOFT:tsync:GlobalTimeDomainExtension:GlobalTimePropagationDelay" == sdItem.attrib["GID"]:
                                globalTimeDomain["globalTimePropagationDelay"] = sdItem.text
                        break
                ar_global_time_gateways = get_element_or_none(ar_global_time_domain, "GATEWAYS/GLOBAL-TIME-GATEWAY")
                if ar_global_time_gateways is not None:
                    ar_global_time_gateways = to_list(ar_global_time_gateways)
                    gateways = []
                    for ar_global_time_gateway in ar_global_time_gateways:
                        gateway = {
                            "host": get_element_or_die(ar_global_time_gateway, "HOST-REF").text,
                            "master": get_element_or_die(ar_global_time_gateway, "MASTER-REF").text,
                            "slave": get_element_or_die(ar_global_time_gateway, "SLAVE-REF").text
                        }
                        gateways.append(gateway)
                    globalTimeDomain["gateways"] = gateways
                ar_global_time_correction_props = get_element_or_none(ar_global_time_domain,
                                                                      "GLOBAL-TIME-CORRECTION-PROPS")
                if ar_global_time_correction_props is not None:
                    globalTimeCorrectionProps = {
                        "offsetCorrectionAdaptionInterval": get_element_or_die(ar_global_time_correction_props, "OFFSET-CORRECTION-ADAPTION-INTERVAL"),
                        "offsetCorrectionJumpThreshold": get_element_or_die(ar_global_time_correction_props, "OFFSET-CORRECTION-JUMP-THRESHOLD"),
                        "rateCorrectionMeasurementDuration": get_element_or_die(ar_global_time_correction_props, "RATE-CORRECTION-MEASUREMENT-DURATION"),
                        "rateCorrectionPerMeasurementDuration": transition_number(get_element_or_die(ar_global_time_correction_props, "RATE-CORRECTIONS-PER-MEASUREMENT-DURATION"))
                    }
                    globalTimeDomain["globalTimeCorrectionProps"] = globalTimeCorrectionProps
                ar_global_time_domain_props = get_element_or_none(ar_global_time_domain,
                                                                  "GLOBAL-TIME-DOMAIN-PROPERTYS/ETH-GLOBAL-TIME-DOMAIN-PROPS")
                if ar_global_time_domain_props is not None:
                    globalTimeDomainProperty = {
                        "destinationPhysicalAddress": get_element_or_die(ar_global_time_domain_props, "DESTINATION-PHYSICAL-ADDRESS")
                    }
                    ar_vlan_priority = transition_number(get_element_or_none(ar_global_time_domain_props, "VLAN-PRIORITY"), defaultInt=None)
                    if ar_vlan_priority is not None:
                        globalTimeDomainProperty["vlanPriority"] = ar_vlan_priority
                    globalTimeDomainProperty["messageCompliance"] = get_element_or_none(ar_global_time_domain_props, "MESSAGE-COMPLIANCE")
                    ar_fup_data_id_lists_info = get_element_or_none(ar_global_time_domain_props, "FUP-DATA-ID-LISTS/FUP-DATA-ID-LIST")
                    fup_data_id_lists = []
                    ar_fup_data_id_lists = to_list(ar_fup_data_id_lists_info)
                    for ar_fup_data_id_list in ar_fup_data_id_lists:
                        fup_data_id_list = ar_fup_data_id_list.text
                        fup_data_id_lists.append(fup_data_id_list)
                    globalTimeDomainProperty["fupDataIDList"] = fup_data_id_lists
                    ar_crc_flags = get_element_or_none(ar_global_time_domain_props, "CRC-FLAGS")
                    if ar_crc_flags is not None:
                        crcFlags = {
                            "crcCorrectionField": get_element_or_die(ar_crc_flags, "CRC-CORRECTION-FIELD"),
                            "crcDomainNumber": get_element_or_die(ar_crc_flags, "CRC-DOMAIN-NUMBER"),
                            "crcMessageLength": get_element_or_die(ar_crc_flags, "CRC-MESSAGE-LENGTH"),
                            "crcPreciseOriginTimestamp": get_element_or_die(ar_crc_flags, "CRC-PRECISE-ORIGIN-TIMESTAMP"),
                            "crcSequenceId": get_element_or_die(ar_crc_flags, "CRC-SEQUENCE-ID"),
                            "crcSourcePortIdentity": get_element_or_die(ar_crc_flags, "CRC-SOURCE-PORT-IDENTITY")
                        }
                        globalTimeDomainProperty["crcFlags"] = crcFlags
                    ar_managed_coupling_port = get_element_or_none(ar_global_time_domain_props, "MANAGED-COUPLING-PORTS/ETH-GLOBAL-TIME-MANAGED-COUPLING-PORT")
                    if ar_managed_coupling_port is not None:
                        managedCouplingPort = {
                            "pdelayLatencyThreshold": get_element_or_die(ar_managed_coupling_port, "PDELAY-LATENCY-THRESHOLD"),
                            "pdelayRequestPeriod": get_element_or_die(ar_managed_coupling_port, "PDELAY-REQUEST-PERIOD"),
                            "pdelayRespAndRespFollowUpTimeout": get_element_or_die(ar_managed_coupling_port, "PDELAY-RESP-AND-RESP-FOLLOW-UP-TIMEOUT"),
                            "pdelayResponseEnabled": get_element_or_die(ar_managed_coupling_port, "PDELAY-RESPONSE-ENABLED")
                        }

                        ar_coupling_port = get_element_or_none(ar_managed_coupling_port, "COUPLING-PORT-REF")
                        if ar_coupling_port is not None:
                            managedCouplingPort["couplingPort"] = ar_coupling_port.text
                        globalTimeDomainProperty["managedCouplingPort"] = managedCouplingPort
                    globalTimeDomain["globalTimeDomainProperty"] = globalTimeDomainProperty
                ar_global_time_eth_master = get_element_or_none(ar_global_time_domain, "GLOBAL-TIME-MASTERS/GLOBAL-TIME-ETH-MASTER")
                if ar_global_time_eth_master is not None:
                    globalTimeEthMaster = {
                        "name": ar_global_time_eth_master.SHORT_NAME,
                        "immediateResumeTime": get_element_or_die(ar_global_time_eth_master, "IMMEDIATE-RESUME-TIME"),
                        "isSystemWideGlobalTime": get_element_or_die(ar_global_time_eth_master, "IS-SYSTEM-WIDE-GLOBAL-TIME-MASTER"),
                        "syncPeriod": get_element_or_die(ar_global_time_eth_master, "SYNC-PERIOD"),
                        "crcSecured": get_element_or_die(ar_global_time_eth_master, "CRC-SECURED")
                    }
                    self.time_set_ethernet(ar_global_time_eth_master, globalTimeEthMaster, sync_time)
                    ar_sub_tlv_config = get_element_or_none(ar_global_time_eth_master, "SUB-TLV-CONFIG")
                    if ar_sub_tlv_config is not None:
                        subTlvConfig = {
                            "ofsSubTlv": get_element_or_die(ar_sub_tlv_config, "OFS-SUB-TLV"),
                            "statusSubTlv": get_element_or_die(ar_sub_tlv_config, "STATUS-SUB-TLV"),
                            "timeSubTlv": get_element_or_die(ar_sub_tlv_config, "TIME-SUB-TLV"),
                            "userDataSubTlv": get_element_or_die(ar_sub_tlv_config, "USER-DATA-SUB-TLV")
                        }
                        globalTimeEthMaster["subTlvConfig"] = subTlvConfig
                    globalTimeDomain["globalTimeEthMaster"] = globalTimeEthMaster
                ar_global_time_eth_slaves = get_element_or_none(ar_global_time_domain, "SLAVES/GLOBAL-TIME-ETH-SLAVE")
                globalTimeEthSlaves = []
                if ar_global_time_eth_slaves is not None:
                    for ar_global_time_eth_slave in ar_global_time_eth_slaves:
                        globalTimeEthSlave = {
                            "name": ar_global_time_eth_slave.SHORT_NAME,
                            "followUpTimeoutValue": get_element_or_die(ar_global_time_eth_slave, "FOLLOW-UP-TIMEOUT-VALUE"),
                            "timeLeapFutureThreshold": get_element_or_die(ar_global_time_eth_slave, "TIME-LEAP-FUTURE-THRESHOLD"),
                            "timeLeapPastThreshold": get_element_or_die(ar_global_time_eth_slave, "TIME-LEAP-PAST-THRESHOLD"),
                            "crcValidated": get_element_or_die(ar_global_time_eth_slave, "CRC-VALIDATED")
                        }
                        self.time_set_ethernet(ar_global_time_eth_slave, globalTimeEthSlave, sync_time)
                        ar_time_leap_healing_counter = get_element_or_none(ar_global_time_eth_slave, "TIME-LEAP-HEALING-COUNTER")
                        if ar_time_leap_healing_counter is not None:
                            globalTimeEthSlave["timeLeapHealingCounter"] = ar_time_leap_healing_counter.text
                        globalTimeEthSlaves.append(globalTimeEthSlave)
                    globalTimeDomain["globalTimeEthSlaves"] = globalTimeEthSlaves
                ar_sub_domain_refs = get_element_or_none(ar_global_time_domain, "SUB-DOMAIN-REFS/SUB-DOMAIN-REF")
                if ar_sub_domain_refs is not None:
                    globalTimeSubDomains = []
                    ar_sub_domain_refs = to_list(ar_sub_domain_refs)
                    for ar_sub_domain_ref in ar_sub_domain_refs:
                        sub_domain_ref = ar_sub_domain_ref.text
                        globalTimeSubDomains.append(sub_domain_ref)
                    globalTimeDomain["globalTimeSubDomains"] = globalTimeSubDomains
                ar_offset_time_domain_ref = get_element_or_none(ar_global_time_domain, "OFFSET-TIME-DOMAIN-REF")
                if ar_offset_time_domain_ref is not None:
                    globalTimeDomain["offsetTimeDomain"] = ar_offset_time_domain_ref.text
                ar_network_segment_id = get_element_or_none(ar_global_time_domain, "NETWORK-SEGMENT-ID/NETWORK-SEGMENT-ID")
                if ar_network_segment_id is not None:
                    globalTimeDomain["networkSegmentId"] = ar_network_segment_id
                else:
                    globalTimeDomain["networkSegmentId"] = 0
                ar_pdu_triggering = get_element_or_none(ar_global_time_domain, "GLOBAL-TIME-PDU-TRIGGERING-REF")
                if ar_pdu_triggering is not None:
                    globalTimeDomain["pduTriggering"] = ar_pdu_triggering.text
                result.append(globalTimeDomain)
        return result

    def time_set_ethernet(self, timeEth, globalOb, syncTime):
        if timeEth is not None and timeEth.get_fqn() in syncTime:
            connector_ref = get_element_or_none(timeEth, "COMMUNICATION-CONNECTOR-REF")
            if connector_ref is not None:
                connector_ele = self.model.find_referable(connector_ref.text)
                endpoint_ref = get_element_or_none(connector_ele, "UNICAST-NETWORK-ENDPOINT-REF")
                if endpoint_ref is not None:
                    endpoint_ele = self.model.find_referable(endpoint_ref.text)
                    ipv4id = get_element_or_none(endpoint_ele, "NETWORK-ENDPOINT-ADDRESSES/IPV-4-CONFIGURATION/IPV-4-ADDRESS")
                    if ipv4id is not None:
                        globalOb["ethernetConnector"] = ipv4id.text
                    channel_ref = endpoint_ref.text[0: endpoint_ref.text.rindex('/')]
                    channel_ele = self.model.find_referable(channel_ref)
                    vlanId = get_element_or_none(channel_ele, "VLAN/VLAN-IDENTIFIER")
                    if vlanId is not None:
                        globalOb["vlanId"] = transition_number(vlanId.text)

    def get_time_base_storage(self, nameFqn, timeBaseOb):
        mapping = self.model.find_elements_of_type("TIME-BASE-PROVIDER-TO-PERSISTENCY-MAPPING")
        for item in mapping:
            provider_ref = get_element_or_none(item, "TIME-BASE-PROVIDER-REF")
            if provider_ref is not None and provider_ref.text == nameFqn:
                per_ref = get_element_or_none(item, "PERSISTENCY-DEPLOYMENT-ELEMENT-REF")
                if per_ref is not None:
                    per_storage = per_ref.text[0: per_ref.text.rindex('/')]
                    dest = per_ref.attrib["DEST"]
                    if dest == "PERSISTENCY-FILE":
                        per_file = self.model.find_referable(per_ref.text)
                        file_name = get_element_or_none(per_file, "FILE-NAME")
                        if file_name is not None:
                            timeBaseOb["storageFile"] = file_name.text
                        file_storage_mapping = self.model.find_elements_of_type("PERSISTENCY-PORT-PROTOTYPE-TO-FILE-STORAGE-MAPPING")
                        for file_storage in file_storage_mapping:
                            file_storage_ref = get_element_or_none(file_storage, "FILE-STORAGE-REF")
                            if file_storage_ref is not None and file_storage_ref.text == per_storage:
                                timeBaseOb["storageInstanceSpecifier"] = commen_util.get_instance_specifier(self, file_storage)
                                return
                    elif dest == "PERSISTENCY-KEY-VALUE-PAIR":
                        kv_storage_mapping = self.model.find_elements_of_type("PERSISTENCY-PORT-PROTOTYPE-TO-KEY-VALUE-STORAGE-MAPPING")
                        for kv_storage in kv_storage_mapping:
                            kv_storage_ref = get_element_or_none(kv_storage, "KEY-VALUE-STORAGE-REF")
                            if kv_storage_ref is not None and kv_storage_ref.text == per_storage:
                                timeBaseOb["storageKVKey"] = short_name(per_ref.text)
                                timeBaseOb["storageInstanceSpecifier"] = commen_util.get_instance_specifier(self, kv_storage)
                                return
