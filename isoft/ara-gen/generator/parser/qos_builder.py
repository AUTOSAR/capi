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
import os

import xmltodict

from generator.common.ar_element import ArElement
from generator.common.tree_helper import get_element_or_none, to_list, short_name, get_element_or_die
from generator.parser import commen_util
import lxml.etree as et


class QosBuilder:
    def __init__(self, model):
        self.model = model
        self._log = logging.getLogger(__name__)

    def get_qos(self, ar_process, qos_path):
        if qos_path is not None and str(qos_path).endswith(".xml"):
            if not os.path.exists(qos_path):
                self._log.info("File does not exist. %s", qos_path)
                return None
            qosOb = {
                "isHaveQos": False,
                "ipv4": "",
                "ipv6": ""
            }
            machine = commen_util.find_machine_by_process(self, ar_process.get_fqn())
            endPoint = commen_util.get_machine_network_point(self, machine)
            self._set_ip_address(endPoint, qosOb)
            dscpOb = {}
            mapping = self.model.find_elements_of_type("SERVICE-INSTANCE-TO-PORT-PROTOTYPE-MAPPING")
            for item in mapping:
                process_ref = get_element_or_none(item, "PROCESS-REF")
                if process_ref is not None and process_ref.text == ar_process.get_fqn():
                    service_instance_ref = get_element_or_none(item, "SERVICE-INSTANCE-REF")
                    if service_instance_ref is not None and service_instance_ref.attrib["DEST"] in ["DDS-REQUIRED-SERVICE-INSTANCE", "DDS-PROVIDED-SERVICE-INSTANCE"]:
                        serviceInstance = self.model.find_referable(service_instance_ref.text)
                        qosProfile = get_element_or_none(serviceInstance, "QOS-PROFILE")
                        if qosProfile is not None:
                            protocolAllList = []
                            protocolOnlyList = []
                            profile = qosProfile.text
                            qosProfileKey = "{}@All".format(profile)
                            providePortList = []
                            requiredIpObList = []
                            if qosProfileKey not in qosOb:
                                qosProfileOb = {}
                                qosOb["isHaveQos"] = True
                                qosOb["isR"] = False
                                qosProfileOb["profile"] = profile
                                qosProfileOb["only"] = protocolOnlyList
                                qosProfileOb["protocol"] = protocolAllList
                                qosOb[qosProfileKey] = qosProfileOb
                                qosProfileOb["providePortList"] = providePortList
                                qosProfileOb["requiredIpObList"] = requiredIpObList
                            else:
                                qosProfileOb = qosOb[qosProfileKey]
                                protocolOnlyList = qosProfileOb["only"]
                                protocolAllList = qosProfileOb["protocol"]
                                providePortList = qosProfileOb["providePortList"]
                                requiredIpObList = qosProfileOb["requiredIpObList"]
                            deploymentRef = get_element_or_none(serviceInstance, "SERVICE-INTERFACE-DEPLOYMENT-REF")
                            if deploymentRef is not None:
                                deploymentEle = self.model.find_referable(deploymentRef.text)
                                self.get_protocol(qosOb, deploymentEle, protocolOnlyList, protocolAllList)
                            sdg = get_element_or_none(serviceInstance, "ADMIN-DATA/SDGS/SDG")
                            providePort = {
                                "isHave": "false",
                                "port4": 0,
                                "port6": 0
                            }
                            requiredIpOb = {
                                "isHave": "false",
                                "address4": 0,
                                "address6": 0,
                                "port4": 0,
                                "port6": 0
                            }
                            if sdg is not None:
                                if sdg.attrib["GID"] == "iSOFT:com:DdsProvidedTransportConfig":
                                    sdList = sdg.find_elements_of_type("SD")
                                    for sd in sdList:
                                        if sd.attrib["GID"] == "iSOFT:com:DdsProvidedTransportConfig:EnableDdsProvidedTransportConfig":
                                            providePort["isHave"] = sd.text
                                            providePortList.append(providePort)
                                        elif sd.attrib["GID"] == "iSOFT:com:DdsProvidedTransportConfig:TCPv4Port":
                                            providePort["port4"] = sd.text
                                        elif sd.attrib["GID"] == "iSOFT:com:DdsProvidedTransportConfig:TCPv6Port":
                                            providePort["port6"] = sd.text
                                        elif sd.attrib["GID"] == "iSOFT:com:DdsProvidedTransportConfig:DSCP":
                                            if profile in dscpOb :
                                                assert dscpOb[profile] == sd.text, "[CM  ] CODE-009: {} configured DSCP is inconsistent [{}, {}]. LocalPath:{}. ".format(profile, dscpOb[profile], sd.text, sdg.get_path())
                                            else:
                                                dscpOb[profile] = sd.text
                                elif sdg.attrib["GID"] == "iSOFT:com:DdsRequiredTransportConfig":
                                    sdList = sdg.find_elements_of_type("SD")
                                    for sd in sdList:
                                        if sd.attrib["GID"] == "iSOFT:com:DdsRequiredTransportConfig:EnableDdsRequiredTransportConfig":
                                            requiredIpOb["isHave"] = sd.text
                                            requiredIpObList.append(requiredIpOb)
                                        elif sd.attrib["GID"] == "iSOFT:com:DdsRequiredTransportConfig:RemoteIpv4Address":
                                            requiredIpOb["address4"] = sd.text
                                        elif sd.attrib["GID"] == "iSOFT:com:DdsRequiredTransportConfig:RemoteIpv6Address":
                                            requiredIpOb["address6"] = sd.text
                                        elif sd.attrib["GID"] == "iSOFT:com:DdsRequiredTransportConfig:RemoteTCPv4Port":
                                            requiredIpOb["port4"] = sd.text
                                        elif sd.attrib["GID"] == "iSOFT:com:DdsRequiredTransportConfig:RemoteTCPv6Port":
                                            requiredIpOb["port6"] = sd.text
                                        elif sd.attrib["GID"] == "iSOFT:com:DdsRequiredTransportConfig:DSCP":
                                            if profile in dscpOb:
                                                assert dscpOb[profile] == sd.text, "[CM  ] CODE-009: {} configured DSCP is inconsistent [{}, {}]. LocalPath:{}. ".format(profile, dscpOb[profile], sd.text, sdg.get_path())
                                            else:
                                                dscpOb[profile] = sd.text

                            # domainId = get_element_or_none(serviceInstance, "DOMAIN-ID")
                            # if domainId is not None and "domainId" not in qosProfileOb:
                            #     qosProfileOb["domainId"] = domainId
                            #     profile = profile + domainId.text
                            # if "port" not in qosProfileOb:
                            #     qosProfileOb["port"] = commen_util.get_hash(profile+short_name(process_ref.text))
                            qosType = "data_writer"
                            if service_instance_ref.attrib["DEST"] == "DDS-REQUIRED-SERVICE-INSTANCE":
                                qosType = "data_reader"
                            connectRef = self._get_si_to_machine_mappings(service_instance_ref)
                            if connectRef is not None:
                                connectorEle = self.model.find_referable(connectRef.text)
                                unicastEndpointRef = get_element_or_none(connectorEle, "UNICAST-NETWORK-ENDPOINT-REF")
                                if unicastEndpointRef is not None:
                                    endpointEle = self.model.find_referable(unicastEndpointRef.text)
                                    self._set_ip_address(endpointEle, qosOb)
                            qosProps = serviceInstance.find_elements_of_type("DDS-EVENT-QOS-PROPS") + serviceInstance.find_elements_of_type("DDS-FIELD-QOS-PROPS")
                            for qosProp in qosProps:
                                ref = get_element_or_none(qosProp, "EVENT-REF")
                                if ref is None:
                                    ref = get_element_or_none(qosProp, "FIELD-REF")
                                qos = get_element_or_none(qosProp, "QOS-PROFILE")
                                if qos is not None:
                                    qosAllKey = "{}@All".format(qos.text)
                                    eventOnlyList = []
                                    if qosAllKey in qosOb:
                                        eventOnlyList = qosOb[qosAllKey]["protocol"]
                                    else:
                                        qosKey = "{}@{}".format(qos.text, qosType)
                                        if qosKey not in qosOb:
                                            qosOb["isHaveQos"] = True
                                            qosOb[qosKey] = eventOnlyList
                                        else:
                                            eventOnlyList = qosOb[qosKey]
                                    if ref is not None:
                                        ele = self.model.find_referable(ref.text)
                                        self.get_protocol(qosOb, ele, eventOnlyList, protocolAllList)
            if qosOb["isHaveQos"]:
                qosStr = et.fromstring(open(qos_path, mode="rb").read(), ArElement.parser())
                string = qosStr.to_pretty_string().encode('utf-8')
                my_dict = xmltodict.parse(string)
                returnDict = {"dds": {}}
                if '@xmlns' in my_dict["dds"]:
                    returnDict["dds"]["@xmlns"] = my_dict["dds"]["@xmlns"]
                profilesTemplate = my_dict["dds"]["profiles"]
                profilesOb = {}
                transport_descriptor = []
                for key in qosOb:
                    keys = str(key).split("@")
                    if len(keys) == 2:
                        allOb = qosOb[key]
                        if keys[1] == "All":
                            if "transport_descriptors" in profilesTemplate:
                                transport_descriptors = profilesTemplate["transport_descriptors"]
                                if "transport_descriptor" in transport_descriptors:
                                    transport_descriptor_ = transport_descriptors["transport_descriptor"]
                                    if isinstance(transport_descriptor_, dict):
                                        transport_descriptor.append(transport_descriptor_)
                                    elif isinstance(transport_descriptor_, list):
                                        transport_descriptor.extend(transport_descriptor_)
                            protocolList = allOb["protocol"]
                            transport_id_list = []
                            locator_list = []
                            for protocol in protocolList:
                                protocolLower = protocol.lower()
                                transport_id = "{}_{}".format(protocolLower, keys[0])
                                transport_id_list.append(transport_id)
                                address = qosOb["ipv4"]
                                if protocol.endswith("v6"):
                                    address = qosOb["ipv6"]
                                if protocolLower.startswith("udp"):
                                    transport_ob = {
                                        "transport_id": transport_id,
                                        "type": protocol,
                                        "interfaceWhiteList": {
                                          "address": address
                                        },
                                        "sendBufferSize": "0",
                                        "receiveBufferSize": "0",
                                        "non_blocking_send": True
                                    }
                                    if keys[0] in dscpOb:
                                        transport_ob["DSCP"] = dscpOb[keys[0]]
                                    transport_descriptor.append(transport_ob)
                                elif protocolLower.startswith("tcp"):
                                    requiredIpObList = allOb["requiredIpObList"]
                                    for requiredIpOb in requiredIpObList:
                                        if str(requiredIpOb["isHave"]).lower() == "true":
                                            if protocol.endswith("v4"):
                                                inAddress = requiredIpOb["address4"]
                                                inPort = requiredIpOb["port4"]
                                            else:
                                                inAddress = requiredIpOb["address6"]
                                                inPort = requiredIpOb["port6"]
                                            protocolOb = {
                                                protocolLower: {
                                                    "address": inAddress,
                                                    "physical_port": inPort
                                                }
                                            }
                                            if protocolOb not in locator_list:
                                                locator_list.append(protocolOb)
                                    transport_ob = {
                                        "transport_id": transport_id,
                                        "type": protocol,
                                        "interfaceWhiteList": {
                                            "address": address
                                        },
                                        "sendBufferSize": "0",
                                        "receiveBufferSize": "0"
                                    }
                                    if keys[0] in dscpOb:
                                        transport_ob["DSCP"] = dscpOb[keys[0]]
                                    providePortList = allOb["providePortList"]
                                    portList = []
                                    for providePort in providePortList:
                                        if str(providePort["isHave"]).lower() == "true":
                                            portNum = providePort["port4"]
                                            if protocol.endswith("v6"):
                                                portNum = providePort["port6"]
                                            if portNum not in portList:
                                                portList.append(portNum)
                                    if len(portList) > 0:
                                        transport_ob["listening_ports"] = {
                                            "port": portList
                                        }
                                    transport_ob["wan_addr"] = address
                                    transport_descriptor.append(transport_ob)
                            if len(transport_descriptor) > 0:
                                profilesOb["transport_descriptors"] = {
                                    "transport_descriptor": transport_descriptor
                                }
                            if "participant" in profilesTemplate:
                                participantOb = None
                                participant = profilesTemplate["participant"]
                                if isinstance(participant, list):
                                    for participantItem in participant:
                                        if participantItem["@profile_name"] == keys[0]:
                                            participantOb = participantItem
                                            break
                                elif isinstance(participant, dict):
                                    if participant["@profile_name"] == keys[0]:
                                        participantOb = participant
                                if participantOb:
                                    if "rtps" in participantOb:
                                        rtps_ = participantOb["rtps"]
                                        if rtps_ is not None and "userTransports" in rtps_:
                                            userTransports = rtps_["userTransports"]
                                            if userTransports is not None and "transport_id" in userTransports:
                                                transport_id_ = userTransports["transport_id"]
                                                if isinstance(transport_id_, str):
                                                    transport_id_list.append(transport_id_)
                                                elif isinstance(transport_id_, list):
                                                    transport_id_list.extend(transport_id_)
                                        if rtps_ is not None and "builtin" in rtps_:
                                            builtin = rtps_["builtin"]
                                            if builtin is not None and "initialPeersList" in builtin:
                                                initialPeersList = builtin["initialPeersList"]
                                                if initialPeersList is not None and "locator" in initialPeersList:
                                                    locator = initialPeersList["locator"]
                                                    if isinstance(locator, dict):
                                                        locator_list.append(locator)
                                                    elif isinstance(locator, list):
                                                        locator_list.extend(locator)
                                    rtps = {}
                                    if len(locator_list) > 0:
                                        rtps["builtin"] = {
                                            "initialPeersList": {
                                                "locator": locator_list
                                            }
                                        }
                                    if len(transport_id_list) > 0:
                                        rtps["userTransports"] = {
                                            "transport_id": transport_id_list
                                        }
                                        rtps["useBuiltinTransports"] = True

                                    # if "domainId" in allOb:
                                    #     rtps["domainId"] = allOb["domainId"]
                                    participantOb["rtps"] = rtps
                                    participantList = []
                                    if "participant" in profilesOb:
                                        participantList = profilesOb["participant"]
                                    else:
                                        profilesOb["participant"] = participantList
                                    participantList.append(participantOb)
                            self.set_data_w_r("data_reader", profilesOb, profilesTemplate, keys, allOb, qosOb)
                            self.set_data_w_r("data_writer", profilesOb, profilesTemplate, keys, allOb, qosOb)
                        else:
                            self.set_data_w_r(keys[1], profilesOb, profilesTemplate, keys, allOb, qosOb)
                returnDict["dds"]["profiles"] = profilesOb
                xmlStr = xmltodict.unparse(returnDict, pretty=True)
                return {
                    "qosJson": qosOb,
                    "qosXml": xmlStr
                }

    def set_data_w_r(self, data_type, profilesOb, profilesTemplate, keys, allOb, qosOb):
        if data_type in profilesTemplate:
            data_ob = None
            data_reader = profilesTemplate[data_type]
            if isinstance(data_reader, list):
                for item in data_reader:
                    if item["@profile_name"] == keys[0]:
                        data_ob = item
                        break
            elif isinstance(data_reader, dict):
                if data_reader["@profile_name"] == keys[0]:
                    data_ob = data_reader
            locatorList = []
            protocolList = []
            if isinstance(allOb, dict):
                protocolList = allOb["only"]
            elif isinstance(allOb, list):
                protocolList = allOb
            for protocol in protocolList:
                address = qosOb["ipv4"]
                if protocol.endswith("v6"):
                    address = qosOb["ipv6"]
                locatorList.append({
                    protocol.lower(): {
                        "address": address
                    }
                })

            if data_ob:
                if len(locatorList) > 0:
                    data_ob["unicastLocatorList"] = {
                        "locator": locatorList
                    }
                dataList = []
                if data_type in profilesOb:
                    dataList = profilesOb[data_type]
                else:
                    profilesOb[data_type] = dataList
                dataList.append(data_ob)

    def get_protocol(self, qosOb, ele, onlyList, protocolList):
        protocolMap = get_element_or_none(ele, "TRANSPORT-PROTOCOLS/TRANSPORT-PROTOCOL")
        if protocolMap is None:
            protocolMap = get_element_or_none(ele, "NOTIFIER/TRANSPORT-PROTOCOLS/TRANSPORT-PROTOCOL")
        if protocolMap is not None:
            protocolMap = to_list(protocolMap)
            for protocol in protocolMap:
                protocolStr = protocol.text
                if not (protocolStr.endswith("v4") or protocolStr.endswith("v6")):
                    if qosOb["ipv4"] != "":
                        self.add_protocol_list(protocolStr+"v4", protocolList, onlyList)
                    if qosOb["ipv6"] != "":
                        self.add_protocol_list(protocolStr+"v6", protocolList, onlyList)
                else:
                    self.add_protocol_list(protocolStr, protocolList, onlyList)

    def add_protocol_list(self, protocolStr, protocolList, onlyList):
        if protocolStr not in protocolList:
            protocolList.append(protocolStr)
        if onlyList is not None:
            if protocolStr not in onlyList:
                onlyList.append(protocolStr)

    def _set_ip_address(self, endPoint, qosOb):
        if endPoint is not None:
            ipAddress = get_element_or_none(endPoint, "NETWORK-ENDPOINT-ADDRESSES/IPV-4-CONFIGURATION/IPV-4-ADDRESS")
            if ipAddress is not None:
                qosOb["ipv4"] = ipAddress.text
            ipV6Address = get_element_or_none(endPoint, "NETWORK-ENDPOINT-ADDRESSES/IPV-6-CONFIGURATION/IPV-6-ADDRESS")
            if ipV6Address is not None:
                qosOb["ipv6"] = ipV6Address.text

    def _get_si_to_machine_mappings(self, instanceRef):
        si_to_machine_mappings = self.model.find_elements_of_type("DDS-SERVICE-INSTANCE-TO-MACHINE-MAPPING")
        for item in si_to_machine_mappings:
            refs = get_element_or_none(item, "SERVICE-INSTANCE-REFS")
            if refs is not None:
                instance_refs = refs.find_elements_of_type('SERVICE-INSTANCE-REF')
                for instance in instance_refs:
                    if instance.text == instanceRef:
                        connector_ref = get_element_or_none(item, "COMMUNICATION-CONNECTOR-REF")
                        return connector_ref
        return None

