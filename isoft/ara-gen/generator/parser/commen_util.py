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
import hashlib

from generator.common.tree_helper import get_element_or_none, short_name, get_element_or_die, transition_number, to_list


def get_instance_specifier(self, endpoint):
    port_prototype = get_element_or_none(endpoint, "PORT-PROTOTYPE-IREF")
    return get_instance_specifier_port(self, endpoint, port_prototype)


def get_instance_specifier_port(self, endpoint, port_prototype):
    if port_prototype is not None:
        ar_port_ref = get_element_or_none(port_prototype, "TARGET-PORT-PROTOTYPE-REF")
        if ar_port_ref is None:
            ar_port_ref = get_element_or_none(port_prototype, "TARGET-R-PORT-PROTOTYPE-REF")
        if ar_port_ref is None:
            ar_port_ref = get_element_or_none(port_prototype, "TARGET-P-PORT-PROTOTYPE-REF")
        if ar_port_ref is None:
            ar_port_ref = get_element_or_none(port_prototype, "CONTEXT-R-PORT-PROTOTYPE-REF")
        if ar_port_ref is None:
            ar_port_ref = get_element_or_none(port_prototype, "CONTEXT-P-PORT-PROTOTYPE-REF")
        if ar_port_ref is None:
            ar_port_ref = get_element_or_none(port_prototype, "CONTEXT-PORT-PROTOTYPE-REF")
        assert ar_port_ref is not None, "[CM  ] CODE-011: {}[{}] must have PORT-PROTOTYPE-REF. LocalPath:{}".format(port_prototype.get_fqn(), port_prototype.short_tag(), port_prototype.get_path())
        ar_process_ref = get_element_or_none(endpoint, "PROCESS-REF")
        if ar_process_ref is not None:
            ar_process = self.model.find_referable(ar_process_ref.text)
            ar_executable_ref = get_element_or_die(ar_process, "EXECUTABLE-REF")
            ar_executable = self.model.find_referable(ar_executable_ref.text)
            ar_root = get_element_or_die(ar_executable, "ROOT-SW-COMPONENT-PROTOTYPE")
            hierarchy = ""
            ar_context_map = port_prototype.find_elements_of_type("CONTEXT-COMPONENT-PROTOTYPE-REF")
            for ar_context_ref in ar_context_map:
                hierarchy += "/{}".format(short_name(ar_context_ref.text))
            result = "{}/{}{}/{}".format(ar_executable.SHORT_NAME, ar_root.SHORT_NAME, hierarchy, short_name(ar_port_ref.text))
            return result

    return ""


def get_network_address(point, isAssert=True):
    ipAddress = get_element_or_none(point, "NETWORK-ENDPOINT-ADDRESSES/IPV-4-CONFIGURATION/IPV-4-ADDRESS")
    if ipAddress is not None:
        return ipAddress.text
    else:
        ipAddress = get_element_or_none(point, "NETWORK-ENDPOINT-ADDRESSES/IPV-6-CONFIGURATION/IPV-6-ADDRESS")
        if ipAddress is not None:
            sdgList = to_list(get_element_or_none(point, "ADMIN-DATA/SDGS/SDG"))
            for sdg in sdgList:
                sd = get_element_or_none(sdg, "SD")
                if sd is not None and "iSOFT:com:NetworkEndpoint:AdapterName" == sd.attrib["GID"]:
                    return "{}%{}".format(ipAddress.text, sd.text)
    if isAssert:
        assert False, "[NM  ] CODE-001: NetworkEndpoint need to configure one of IPV4 or IPV6.FQN:{}.LocalPath:{}".format(point.get_fqn(), point.get_path())
    else:
        return ""


def find_machine_by_process(self, process_ref):
    if process_ref is None:
        return None
    mapping = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING", PROCESS_REF=process_ref)
    for item in mapping:
        machine = get_element_or_none(item, "MACHINE-REF")
        if machine is not None:
            return self.model.find_referable(machine.text)
    return None

def find_machine_fqn_by_process(self, process_ref):
    if process_ref is None or str(process_ref) == "":
        return ""
    mapping = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING", PROCESS_REF=process_ref)
    for item in mapping:
        machine = get_element_or_none(item, "MACHINE-REF")
        if machine is not None:
            return machine.text
    return ""


def get_machine_network_point(self, machine):
    if machine is not None:
        machineDe = get_element_or_none(machine, "MACHINE-DESIGN-REF")
        if machineDe is not None:
            machineDesign = self.model.find_referable(machineDe.text)
            endPointRef = get_element_or_die(machineDesign, "COMMUNICATION-CONNECTORS/ETHERNET-COMMUNICATION-CONNECTOR/UNICAST-NETWORK-ENDPOINT-REF")
            endpoint_info = self.model.find_referable(endPointRef.text)
            return endpoint_info
    return None


def get_diagnostic_address_from_software(software_cluster):
    diagnostic_addresss = get_element_or_none(software_cluster, "DIAGNOSTIC-ADDRESSS")
    if diagnostic_addresss is not None:
        diagnostic_address_map = software_cluster.find_elements_of_type("SOFTWARE-CLUSTER-DOIP-DIAGNOSTIC-ADDRESS")
        diagnosticAddress = {
            "physical": -1,
            "functional": []
        }
        errorStr = "[DIAG] CODE-002: SoftwareCluster[{}].DiagnosticAddresss.SoftwareClusterDoipDiagnosticAddress.addressSemantics = PHYSICAL-ADDRESS must have one and only one.".format(software_cluster.get_fqn())
        for diagnostic_address in diagnostic_address_map:
            address_semantics = get_element_or_none(diagnostic_address, "ADDRESS-SEMANTICS")
            diagnostic_address_num = transition_number(get_element_or_none(diagnostic_address, "DIAGNOSTIC-ADDRESS"), defaultInt=None)
            if "PHYSICAL-ADDRESS" == address_semantics:
                assert diagnosticAddress["physical"] == -1, errorStr
                diagnosticAddress["physical"] = diagnostic_address_num
            elif "FUNCTIONAL-ADDRESS" == address_semantics:
                diagnosticAddress["functional"].append(diagnostic_address_num)
        assert diagnosticAddress["physical"] != -1, errorStr
        return diagnosticAddress


def get_hash(value):
    hash_algo = hashlib.new('sha1')
    hash_algo.update(value.encode('UTF-8'))
    result = 0
    digest = bytearray(hash_algo.digest())
    for i in digest[0:2]:
        result = result * 256 + int(i)
    return result


