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

from . import commen_util
from generator.common.tree_helper import get_element_or_none, to_list, get_element_or_str, get_element_or_0,transition_number


class CGBuilder:
    def __init__(self, model):
        self._log = logging.getLogger(__name__)
        self.model = model

    def get_xml_cg(self):
        cg_list = []
        server_interface_mapping = self.model.find_elements_of_type("SERVICE-INTERFACE")
        for item in server_interface_mapping:
            category = get_element_or_none(item, "CATEGORY")
            if category is not None and category.text == "COMMUNICATION_GROUP":
                interfaceOb = {}
                fqn = item.get_fqn()
                path = str(fqn).split("/")
                if len(path) > 2:
                    interfaceOb["interfaceName"] = path[-1]
                    interfaceOb["nameList"] = path[1:-1]
                interfaceOb["categoryName"] = category
                symbol_mapping = get_element_or_none(item, "NAMESPACES/SYMBOL-PROPS")
                if symbol_mapping is not None:
                    symbolProps = []
                    symbol_mapping = to_list(symbol_mapping)
                    for symbol in symbol_mapping:
                        symbolProps.append({
                            "name": symbol.SHORT_NAME,
                            "symbol": get_element_or_str(symbol, "SYMBOL")
                        })
                    interfaceOb["symbolProps"] = symbolProps
                interfaceOb["majorVersion"] = transition_number(get_element_or_0(item, "MAJOR-VERSION"),defaultInt=0)
                interfaceOb["minorVersion"] = transition_number(get_element_or_0(item, "MINOR-VERSION"),defaultInt=0)
                event_mapping = get_element_or_none(item, "EVENTS/VARIABLE-DATA-PROTOTYPE")
                if event_mapping is not None:
                    events = []
                    event_mapping = to_list(event_mapping)
                    for event in event_mapping:
                        events.append({
                            "name": event.SHORT_NAME,
                            "typeTref": get_element_or_str(event, "TYPE-TREF")
                        })
                    interfaceOb["events"] = events
                method_mapping = get_element_or_none(item, "METHODS/CLIENT-SERVER-OPERATION")
                if method_mapping is not None:
                    methods = []
                    method_mapping = to_list(method_mapping)
                    for method in method_mapping:
                        methodOb = {
                            "name": method.SHORT_NAME
                        }
                        argument_mapping = get_element_or_none(method, "ARGUMENTS/ARGUMENT-DATA-PROTOTYPE")
                        if argument_mapping is not None:
                            arguments = []
                            argument_mapping = to_list(argument_mapping)
                            for argument in argument_mapping:
                                arguments.append({
                                    "name": argument.SHORT_NAME,
                                    "typeTref": get_element_or_str(argument, "TYPE-TREF"),
                                    "direction": get_element_or_str(argument, "DIRECTION")
                                })
                            methodOb["arguments"] = arguments
                        error_refs = get_element_or_none(method, "POSSIBLE-AP-ERROR-REFS")
                        if error_refs is not None:
                            possibles = []
                            error_mapping = error_refs.find_elements_of_type("POSSIBLE-AP-ERROR-REF")
                            for error in error_mapping:
                                possibles.append(error.text)
                            if len(possibles) > 0:
                                methodOb["possibles"] = possibles
                        methods.append(methodOb)
                    interfaceOb["methods"] = methods
                cg_list.append(interfaceOb)
        return cg_list

    def get_machine_cg(self, ar_process):
        cg_server_service_instances = []
        cg_client_service_instances = []
        server_interface_mapping = self.model.find_elements_of_type("SERVICE-INTERFACE")
        for item in server_interface_mapping:
            category = get_element_or_none(item, "CATEGORY")
            if category is not None and category.text == "COMMUNICATION_GROUP_SERVER":
                sdg = get_element_or_none(item, "ADMIN-DATA/SDGS/SDG")
                if sdg is not None and sdg.attrib["GID"] == "iSOFT:com:CommunicationGroup":
                    sd = get_element_or_none(sdg, "SD")
                    if sd is not None and sd.attrib["GID"] == "iSOFT:com:CommunicationGroup:GroupName":
                        clientElement = self.get_client(sd.text)
                        if clientElement is not None:
                            pElement = self.set_p_server(item, ar_process)
                            rElement = self.set_r_client(clientElement, ar_process)
                            if pElement is not None:
                                cg_server_service_instances.append(
                                    {
                                        "instance_specifier": commen_util.get_instance_specifier(self, pElement),
                                        "name": sd.text,
                                        "client:": 0
                                    }
                                )
                            if rElement is not None:
                                cg_client_service_instances.append(
                                    {
                                        "instance_specifier": commen_util.get_instance_specifier(self, rElement),
                                        "name": sd.text,
                                        "client:": 0
                                    }
                                )

        if len(cg_client_service_instances) > 0 or len(cg_server_service_instances) > 0:
            return {
                "cg_server_service_instances": cg_server_service_instances,
                "cg_client_service_instances": cg_client_service_instances
            }
        else:
            return None

    def set_p_server(self, serverElement, ar_process):
        p_ports = self.model.find_elements_of_type("P-PORT-PROTOTYPE")
        for item in p_ports:
            provided = get_element_or_none(item, "PROVIDED-INTERFACE-TREF")
            if provided is not None and provided.text == serverElement.get_fqn():
                mapping = self.model.find_elements_of_type("SERVICE-INSTANCE-TO-PORT-PROTOTYPE-MAPPING")
                for prototype in mapping:
                    prototype_ref = get_element_or_none(prototype, "PORT-PROTOTYPE-IREF/TARGET-PORT-PROTOTYPE-REF")
                    if prototype_ref is not None and prototype_ref.text == item.get_fqn():
                        if self.find_this_machine(prototype, ar_process):
                            return prototype

    def set_r_client(self, serverElement, ar_process):
        r_ports = self.model.find_elements_of_type("R-PORT-PROTOTYPE")
        for item in r_ports:
            ref = get_element_or_none(item, "REQUIRED-INTERFACE-TREF")
            if ref is not None and ref.text == serverElement.get_fqn():
                mapping = self.model.find_elements_of_type("SERVICE-INSTANCE-TO-PORT-PROTOTYPE-MAPPING")
                for prototype in mapping:
                    prototype_ref = get_element_or_none(prototype, "PORT-PROTOTYPE-IREF/TARGET-PORT-PROTOTYPE-REF")
                    if prototype_ref is not None and prototype_ref.text == item.get_fqn():
                        if self.find_this_machine(prototype, ar_process):
                            return prototype

    def find_this_machine(self, element, ar_process):
        process = get_element_or_none(element, "PROCESS-REF")
        if process is not None and ar_process.get_fqn() == process.text:
            return True
        return False

    def get_client(self, groupName):
        server_interface_mapping = self.model.find_elements_of_type("SERVICE-INTERFACE")
        for item in server_interface_mapping:
            category = get_element_or_none(item, "CATEGORY")
            if category is not None and category.text == "COMMUNICATION_GROUP_CLIENT":
                sdg = get_element_or_none(item, "ADMIN-DATA/SDGS/SDG")
                if sdg is not None and sdg.attrib["GID"] == "iSOFT:com:CommunicationGroup":
                    sd = get_element_or_none(sdg, "SD")
                    if sd is not None and sd.attrib["GID"] == "iSOFT:com:CommunicationGroup:GroupName":
                        if groupName == sd.text:
                            return item
