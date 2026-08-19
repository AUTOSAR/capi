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
from generator.common.tree_helper import get_element_or_none, short_name


class InstanceSpecifierBuilder:

    def __init__(self, model):
        self.model = model

    def get_all_instance(self, exes):
        all_instance = []
        for exe in exes:
            ar_executable = self.model.find_referable(exe)
            self.get_exe_instance(all_instance, ar_executable)
        all_instance.sort(key=lambda t: t["interfaceName"])
        all_ob = {}
        for item in all_instance:
            interfaceName = item["interfaceName"]
            instanceRef = item["instanceRef"]
            instanceName = self.first_name(instanceRef)
            if interfaceName in all_ob:
                interfaceOb = all_ob[interfaceName]
                if instanceName in interfaceOb:
                    instanceNameList = interfaceOb[instanceName]
                    isHave = False
                    for name in instanceNameList:
                        if instanceRef == name["instanceRef"]:
                            isHave = True
                            break
                    if not isHave:
                        interfaceOb[instanceName].append({
                            "instanceRef": instanceRef,
                            "instanceDest": item["instanceDest"]
                        })
                else:
                    interfaceOb[instanceName] = [{
                            "instanceRef": instanceRef,
                            "instanceDest": item["instanceDest"]
                        }]
            else:
                all_ob[interfaceName] = {
                    instanceName: [{
                            "instanceRef": instanceRef,
                            "instanceDest": item["instanceDest"]
                        }],
                }
        return all_ob

    def get_exe_instance(self, all_instance, ar_executable):
        root_ele = get_element_or_none(ar_executable, "ROOT-SW-COMPONENT-PROTOTYPE")
        if root_ele is not None:
            ar_component_ref = get_element_or_none(root_ele, "APPLICATION-TYPE-TREF")
            if ar_component_ref is not None:
                swcE = self.model.find_referable(ar_component_ref.text)
                ports = get_element_or_none(swcE, "PORTS")
                if ports is not None:
                    pProt = ports.find_elements_of_type("P-PORT-PROTOTYPE")
                    for item in pProt:
                        returnList = self.get_port_dest(item)
                        all_instance.append({
                            "interfaceName": returnList[0],
                            "instanceRef": "{}/{}/{}".format(ar_executable.SHORT_NAME, root_ele.SHORT_NAME, item.SHORT_NAME),
                            "instanceDest": returnList[1],
                            "portFqn": item.get_fqn()
                        })
                    rProt = ports.find_elements_of_type("R-PORT-PROTOTYPE")
                    for item in rProt:
                        returnList = self.get_port_dest(item)
                        all_instance.append({
                            "interfaceName": returnList[0],
                            "instanceRef": "{}/{}/{}".format(ar_executable.SHORT_NAME, root_ele.SHORT_NAME, item.SHORT_NAME),
                            "instanceDest": returnList[1],
                            "portFqn": item.get_fqn()
                        })

                    prProt = ports.find_elements_of_type("PR-PORT-PROTOTYPE")
                    for item in prProt:
                        returnList = self.get_port_dest(item)
                        all_instance.append({
                            "interfaceName": returnList[0],
                            "instanceRef": "{}/{}/{}".format(ar_executable.SHORT_NAME, root_ele.SHORT_NAME, item.SHORT_NAME),
                            "instanceDest": returnList[1],
                            "portFqn": item.get_fqn()
                        })

    def get_port_dest(self, portPrototypeElement):
        interface_ref = get_element_or_none(portPrototypeElement, "REQUIRED-INTERFACE-TREF")
        if interface_ref is None:
            interface_ref = get_element_or_none(portPrototypeElement, "PROVIDED-INTERFACE-TREF")
        if interface_ref is None:
            interface_ref = get_element_or_none(portPrototypeElement, "PROVIDED-REQUIRED-INTERFACE-TREF")
        required_dest = ""
        interface_name = "DIAG"
        if interface_ref is not None:
            required_dest = interface_ref.attrib["DEST"]
            interface_name = short_name(interface_ref.text)
        if required_dest == "" :
            required_d = get_element_or_none(portPrototypeElement, "ADMIN-DATA/SDGS/SDG/SD")
            if required_d is not None and required_d.attrib["GID"] in ["iSOFT:diag:RequiredInterface:Class", "iSOFT:diag:ProvidedInterface:Class"]:
                required_dest = required_d

        return [interface_name, required_dest]

    def first_name(self, all_name):
        return str(all_name).split('/')[0]

