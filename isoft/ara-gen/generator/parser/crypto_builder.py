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
import sys

from . import commen_util
from generator.common.tree_helper import get_element_or_none, get_element_or_false, get_element_or_str, get_element_or_0, to_str, transition_number, get_element_or_die

"""
获取crypto 模块Json
"""


class CryptoBuilder:
    def __init__(self, model):
        self._log = logging.getLogger(__name__)
        self.model = model

    def get_all_crypto(self, machine):
        cryptoCertificateToCryptoKeySlotMapping = []
        cryptoKeySlots = []
        cryptoCertificates = []
        cryptoProviders = []
        cryptoServiceCertificates = []
        cryptoTrustMasterProcesses = []
        crypto_service_certificate = self.model.find_elements_of_type("CRYPTO-SERVICE-CERTIFICATE")
        for csc_item in crypto_service_certificate:
            cryptoServiceCertificates.append({
                "serviceCertificateInstance": csc_item.get_fqn()[1:],
                "cryptoServiceCertificate": {
                    "algorithmFamily": get_element_or_str(csc_item, "ALGORITHM-FAMILY"),
                    "format": get_element_or_str(csc_item, "FORMAT"),
                    "maximumLength": transition_number(get_element_or_0(csc_item, "MAXIMUM-LENGTH"),defaultInt=0),
                    "nextHigherCertificate": get_element_or_str(csc_item, "NEXT-HIGHER-CERTIFICATE-REF"),
                }
            })
        self.add_crypto_trust_master(machine, cryptoTrustMasterProcesses)

        instantiation_element = get_element_or_none(machine, "MODULE-INSTANTIATIONS")
        if instantiation_element is not None:
            crypto_module_list = get_element_or_none(instantiation_element, "CRYPTO-MODULE-INSTANTIATION")
            if crypto_module_list is not None:
                for crypto_module in crypto_module_list:
                    crypto_certificate_map = crypto_module.find_elements_of_type("CRYPTO-CERTIFICATE")
                    for crypto_certificate_item in crypto_certificate_map:
                        self.add_crypto_certificate(cryptoCertificates, crypto_certificate_item)

            instantiation_map = instantiation_element.find_elements_of_type("CRYPTO-MODULE-INSTANTIATION")
            for instantiation_item in instantiation_map:
                crypto_provider = get_element_or_none(instantiation_item, "CRYPTO-PROVIDERS")
                if crypto_provider is not None:
                    crypto_provider_map = crypto_provider.find_elements_of_type("CRYPTO-PROVIDER")
                    for crypto_provider_item in crypto_provider_map:
                        self.add_crypto_providers(cryptoProviders, crypto_provider_item)
                        key_slots = crypto_provider_item.find_elements_of_type("KEY-SLOTS")
                        for key_slot_item in key_slots:
                            crypto_key_slot_map = key_slot_item.find_elements_of_type("CRYPTO-KEY-SLOT")
                            for crypto_key_slot_item in crypto_key_slot_map:
                                key_slot_app_ob = {}
                                key_slot_quote = False
                                slotType = self.set_key_slot_map(None, crypto_key_slot_item.get_fqn(), cryptoKeySlots)
                                key_slot_prototype = self.model.find_elements_of_type("CRYPTO-KEY-SLOT-TO-PORT-PROTOTYPE-MAPPING")
                                for key_slot_prototype_item in key_slot_prototype:
                                    processFqn = get_element_or_die(key_slot_prototype_item, "PROCESS-REF")
                                    machine_key = commen_util.find_machine_by_process(self, processFqn)
                                    if machine_key is not None and machine_key.get_fqn() == machine.get_fqn():
                                        key_slot_ref2 = get_element_or_none(key_slot_prototype_item, "KEY-SLOT-REF")
                                        if key_slot_ref2 is not None and key_slot_ref2.text == crypto_key_slot_item.get_fqn():
                                            key_slot_quote = True
                                            if "APPLICATION" == slotType:
                                                if key_slot_ref2.text in key_slot_app_ob:
                                                    oldFqn = key_slot_app_ob[key_slot_ref2.text]
                                                    oldFqn.append(key_slot_prototype_item.get_fqn())
                                                    errorStr = "[CRYPTO] CODE-001: CryptoKeySlot[{}].SlotType == 'APPLICATION' is referenced by more than one CryptoKeySlotToPortPrototypeMapping[{}]. LocalPath:{}.".format(key_slot_ref2.text, ",".join(oldFqn), key_slot_prototype_item.get_path())
                                                    assert False, errorStr
                                                key_slot_app_ob[key_slot_ref2.text] = [key_slot_prototype_item.get_fqn()]
                                            self.set_key_slot_map(processFqn, key_slot_ref2.text, cryptoKeySlots)
                                if not key_slot_quote:
                                    self._log.info("CRYPTO-KEY-SLOT = '{}',No mapping references".format(crypto_key_slot_item.get_fqn()))

                slot_map = get_element_or_none(instantiation_item, "CERTIFICATE-TO-KEY-SLOT-MAPPINGS")
                if slot_map is not None:
                    slot_list = instantiation_item.find_elements_of_type("CRYPTO-CERTIFICATE-TO-CRYPTO-KEY-SLOT-MAPPING")
                    for slot_item in slot_list:
                        slotOb = {}
                        crypto_certificate_ref = get_element_or_none(slot_item, "CRYPTO-CERTIFICATE-REF")
                        slotOb["certificateInstance"] = crypto_certificate_ref.text[1:]
                        key_slot_ref = get_element_or_none(slot_item, "CRYPTO-KEY-SLOT-REFS")
                        if key_slot_ref is not None:
                            key_slot_list = slot_item.find_elements_of_type("CRYPTO-KEY-SLOT-REF")
                            cryptoKeySlot = []
                            for key_slot_item in key_slot_list:
                                cryptoKeySlot.append({
                                    "keySlotInstance": key_slot_item.text[1:]
                                })
                            slotOb["keySlots"] = cryptoKeySlot
                        cryptoCertificateToCryptoKeySlotMapping.append(slotOb)
                        if crypto_certificate_ref is not None:
                            self.set_certificate_to_port_prototype(None, crypto_certificate_ref, None, None, cryptoCertificates)

        return {
            "CryptoKeySlots": cryptoKeySlots,
            "CryptoProviders": cryptoProviders,
            "CryptoCertificates": cryptoCertificates,
            "CryptoServiceCertificates": cryptoServiceCertificates,
            "CryptoCertificateToCryptoKeySlotMapping": cryptoCertificateToCryptoKeySlotMapping,
            "CryptoTrustMasterProcesses": cryptoTrustMasterProcesses
        }

    def add_crypto_trust_master(self, machine, cryptoTrustMasterProcesses):
        mapping = self.model.find_elements_of_type("PROCESS-TO-MACHINE-MAPPING")
        for item in mapping:
            machine_ref = get_element_or_none(item, "MACHINE-REF")
            if machine_ref is not None and machine_ref.text == machine.get_fqn():
                process_ref = get_element_or_die(item, "PROCESS-REF")
                process = self.model.find_referable(process_ref.text)
                exe_ref = get_element_or_none(process, "EXECUTABLE-REF")
                if exe_ref is not None:
                    exe = self.model.find_referable(exe_ref.text)
                    swc_ref = get_element_or_none(exe, "ROOT-SW-COMPONENT-PROTOTYPE/APPLICATION-TYPE-TREF")
                    if swc_ref is not None:
                        swc = self.model.find_referable(swc_ref.text)
                        r_ports = swc.find_elements_of_type("R-PORT-PROTOTYPE")
                        for port in r_ports:
                            interfaceRef = get_element_or_none(port, "REQUIRED-INTERFACE-TREF")
                            if interfaceRef is not None and interfaceRef.attrib["DEST"] == "CRYPTO-TRUST-MASTER-INTERFACE":
                                masterProcessOb = {
                                    "processName": process_ref
                                }
                                if masterProcessOb not in cryptoTrustMasterProcesses:
                                    cryptoTrustMasterProcesses.append(masterProcessOb)
                                break

    def add_crypto_providers(self, cryptoProviders, crypto_provider_item):
        if crypto_provider_item is not None:
            providerInstance = crypto_provider_item.get_fqn()[1:]
            for pro in cryptoProviders:
                if pro["providerInstance"] == providerInstance:
                    return
            key_slots = get_element_or_none(crypto_provider_item, "KEY-SLOTS")
            if key_slots is not None:
                key_slot_map = key_slots.find_elements_of_type("CRYPTO-KEY-SLOT")
                keySlotList = []
                for key in key_slot_map:
                    keySlotList.append({
                        "keySlotInstance": key.get_fqn()[1:]
                    })
                cryptoProviders.append({
                    "providerInstance": providerInstance,
                    "keySlots": keySlotList
                })

    def add_crypto_certificate(self, cryptoCertificates, crypto_certificate_element):
        if crypto_certificate_element is not None:
            certificateInstance = crypto_certificate_element.get_fqn()[1:]
            for cer in cryptoCertificates:
                if cer["certificateInstance"] == certificateInstance:
                    certificateInstance = None
                    break
            if certificateInstance is not None:
                cryptoCertificates.append({
                    "certificateInstance": certificateInstance,
                    "cryptoCertificate": {
                        "shortName": get_element_or_str(crypto_certificate_element, "SHORT-NAME"),
                        "isPrivate": get_element_or_false(crypto_certificate_element, "IS-PRIVATE")
                    }
                })

    def set_key_slot_map(self, processFqn, key_slot_ref2, cryptoKeySlots):
        isChange = False
        keyItemOb = {
            "keySlotInstance": key_slot_ref2[1:]
        }
        for key_item in cryptoKeySlots:
            if key_slot_ref2[1:] == key_item["keySlotInstance"]:
                if processFqn is not None and "APPLICATION" == key_item["keySlot"]["slotType"]:
                    keyItemOb = key_item
                    isChange = True
                else:
                    return key_item["keySlot"]["slotType"]
        key_slot_element = self.model.find_referable(key_slot_ref2)
        if key_slot_element is not None:
            if isChange:
                slotType = get_element_or_str(key_slot_element, "SLOT-TYPE")
                if "APPLICATION" == slotType and processFqn is not None:
                    keySlotOb = keyItemOb["keySlot"]
                    if keySlotOb is not None:
                        appName = ""
                        if "appName" in keySlotOb:
                            appName = keySlotOb["appName"]+";"

                        keySlotOb["appName"] = appName + to_str(processFqn)[1:]
            else:
                keySlotOb = {
                    "shortName": get_element_or_str(key_slot_element, "SHORT-NAME"),
                    "allocateShadowCopy": get_element_or_false(key_slot_element, "ALLOCATE-SHADOW-COPY"),
                    "cryptoAlgId": to_str(get_element_or_str(key_slot_element, "CRYPTO-ALG-ID")),
                    "cryptoObjectType": get_element_or_str(key_slot_element, "CRYPTO-OBJECT-TYPE"),
                    "slotCapacity": transition_number(get_element_or_0(key_slot_element, "SLOT-CAPACITY"),defaultInt=0),
                    "slotType": get_element_or_str(key_slot_element, "SLOT-TYPE")
                }
                allowed_modification = get_element_or_none(key_slot_element, "KEY-SLOT-ALLOWED-MODIFICATION")
                if allowed_modification is not None:
                    keySlotOb["keySlotAllowedModification"] = {
                        "allowContentTypeChange": get_element_or_false(allowed_modification, "ALLOW-CONTENT-TYPE-CHANGE"),
                        "exportability": get_element_or_false(allowed_modification, "EXPORTABILITY"),
                        "maxNumberOfAllowedUpdates": transition_number(get_element_or_0(allowed_modification, "MAX-NUMBER-OF-ALLOWED-UPDATES"),defaultInt=0),
                        "restrictUpdate": get_element_or_false(allowed_modification, "RESTRICT-UPDATE")
                    }
                key_slot_str = ""
                allowed_usages = get_element_or_none(key_slot_element, "KEY-SLOT-CONTENT-ALLOWED-USAGES")
                if allowed_usages is not None:
                    allowed_usage_map = allowed_usages.find_elements_of_type("CRYPTO-KEY-SLOT-CONTENT-ALLOWED-USAGE")
                    for allowed_usage in allowed_usage_map:
                        key_slot_usage = get_element_or_none(allowed_usage, "ALLOWED-KEYSLOT-USAGE")
                        key_slot_str += key_slot_usage + ","
                    keySlotOb["keySlotContentAllowedUsage"] = {
                        "allowedKeyslotUsage": key_slot_str[:-1]
                    }
                fileName = get_element_or_none(key_slot_element, "ADMIN-DATA/SDGS/SDG")
                if fileName is not None and "iSOFT:crypto:KeySlot" == fileName.attrib["GID"]:
                    sd = get_element_or_none(fileName, "SD")
                    if sd is not None and "iSOFT:crypto:KeySlot:FileName" == sd.attrib["GID"]:
                        keySlotOb["fileName"] = sd.text
                keyItemOb["keySlot"] = keySlotOb
        if not isChange:
            cryptoKeySlots.append(keyItemOb)
        return keyItemOb["keySlot"]["slotType"]

    def get_crypto_map(self, ar_process):
        cryptoKeySlotToPortPrototypeMapping = []
        cryptoProviderToPortPrototypeMapping = []
        cryptoCertificateToPortPrototypeMapping = []
        comCertificateToCryptoCertificateMapping = []

        self.set_provider_to_port_prototype(cryptoProviderToPortPrototypeMapping, ar_process)

        key_slot_prototype = self.model.find_elements_of_type("CRYPTO-KEY-SLOT-TO-PORT-PROTOTYPE-MAPPING", PROCESS_REF=ar_process.get_fqn())
        for key_slot_prototype_item in key_slot_prototype:
            key_slot_ref2 = get_element_or_none(key_slot_prototype_item, "KEY-SLOT-REF")
            if key_slot_ref2 is not None:
                self.set_key_slot_to_port_prototype(key_slot_prototype_item, key_slot_ref2, cryptoKeySlotToPortPrototypeMapping)
        prototype_mapping = self.model.find_elements_of_type("CRYPTO-CERTIFICATE-TO-PORT-PROTOTYPE-MAPPING")
        for prototype_item in prototype_mapping:
            process_ref = get_element_or_none(prototype_item, "PROCESS-REF")
            if process_ref is not None and process_ref.text == ar_process.get_fqn():
                crypto_certificate_ref2 = get_element_or_none(prototype_item, "CRYPTO-CERTIFICATE-REF")
                if crypto_certificate_ref2 is not None:
                    instance_specifier = commen_util.get_instance_specifier(self, prototype_item)
                    self.set_certificate_to_port_prototype(prototype_item, crypto_certificate_ref2, instance_specifier, cryptoCertificateToPortPrototypeMapping, None)

        machine = commen_util.find_machine_by_process(self, ar_process.get_fqn())
        if machine is not None:
            instantiation_element = get_element_or_none(machine, "MODULE-INSTANTIATIONS")
            if instantiation_element is not None:
                instantiation_map = instantiation_element.find_elements_of_type("CRYPTO-MODULE-INSTANTIATION")
                for instantiation_item in instantiation_map:
                    slot_map = get_element_or_none(instantiation_item, "CERTIFICATE-TO-KEY-SLOT-MAPPINGS")
                    if slot_map is not None:
                        slot_list = instantiation_item.find_elements_of_type("CRYPTO-CERTIFICATE-TO-CRYPTO-KEY-SLOT-MAPPING")
                        for slot_item in slot_list:
                            crypto_certificate_ref = get_element_or_none(slot_item, "CRYPTO-CERTIFICATE-REF")
                            certificate_mapping = self.model.find_elements_of_type("COM-CERTIFICATE-TO-CRYPTO-CERTIFICATE-MAPPING")
                            for certificate_item in certificate_mapping:
                                crypto_certificate_ref3 = get_element_or_none(certificate_item, "CRYPTO-CERTIFICATE-REF")
                                if crypto_certificate_ref3 is not None and crypto_certificate_ref3.text == crypto_certificate_ref.text:
                                    service_certificate_ob = {
                                        "certificateInstance": crypto_certificate_ref3.text[1:]
                                    }
                                    service_certificate_ref = get_element_or_none(certificate_item, "CRYPTO-SERVICE-CERTIFICATE-REF")
                                    if service_certificate_ref is not None:
                                        service_certificate_ob["serviceCertificateInstance"] = service_certificate_ref.text[1:]
                                    comCertificateToCryptoCertificateMapping.append(service_certificate_ob)
                                    break

        return {
            "CryptoKeySlotToPortPrototypeMapping": cryptoKeySlotToPortPrototypeMapping,
            "CryptoProviderToPortPrototypeMapping": cryptoProviderToPortPrototypeMapping,
            "CryptoCertificateToPortPrototypeMapping": cryptoCertificateToPortPrototypeMapping,
            "ComCertificateToCryptoCertificateMapping": comCertificateToCryptoCertificateMapping
        }

    def set_provider_to_port_prototype(self, cryptoProviderToPortPrototypeMapping, ar_process):
        provider_element_map = self.model.find_elements_of_type("CRYPTO-PROVIDER-TO-PORT-PROTOTYPE-MAPPING")
        for provider_element in provider_element_map:
            if provider_element is not None:
                process_ref = get_element_or_none(provider_element, "PROCESS-REF")
                if process_ref is not None and process_ref.text == ar_process.get_fqn():
                    crypto_provider_ref = get_element_or_none(provider_element, "CRYPTO-PROVIDER-REF")
                    if crypto_provider_ref is not None:
                        instance_specifier = commen_util.get_instance_specifier(self, provider_element)
                        isHave = False
                        for provider_item in cryptoProviderToPortPrototypeMapping:
                            if provider_item["portInstance"] == instance_specifier:
                                isHave = True
                                break
                        if not isHave:
                            providerOb = {"portInstance": instance_specifier,
                                          "providerInstance": crypto_provider_ref.text[1:]
                                          }
                            ar_port_ref = get_element_or_none(provider_element, "PORT-PROTOTYPE-IREF/TARGET-PORT-PROTOTYPE-REF")
                            if ar_port_ref is not None:
                                providerOb["portType"] = ar_port_ref.attrib["DEST"]
                            cryptoProviderToPortPrototypeMapping.append(providerOb)

    def set_key_slot_to_port_prototype(self, key_slot_prototype_item, key_slot_ref2,  cryptoKeySlotToPortPrototypeMapping):
        instance_specifier = commen_util.get_instance_specifier(self, key_slot_prototype_item)
        for item in cryptoKeySlotToPortPrototypeMapping:
            if instance_specifier == item["portInstance"]:
                return
        itemOb = {
            "portInstance": instance_specifier,
            "keySlotInstance": key_slot_ref2.text[1:]
        }
        ar_port_ref = get_element_or_none(key_slot_prototype_item, "PORT-PROTOTYPE-IREF/TARGET-PORT-PROTOTYPE-REF")
        if ar_port_ref is not None:
            itemOb["portType"] = ar_port_ref.attrib["DEST"]
        cryptoKeySlotToPortPrototypeMapping.append(itemOb)

    def set_certificate_to_port_prototype(self, prototype_item, crypto_certificate_ref, instance_specifier, cryptoCertificateToPortPrototypeMapping, cryptoCertificates):
        if cryptoCertificates is not None:
            crypto_certificate_element = self.model.find_referable(crypto_certificate_ref.text)
            if crypto_certificate_element is not None:
                certificateInstance = crypto_certificate_element.get_fqn()[1:]
                for cer in cryptoCertificates:
                    if cer["certificateInstance"] == certificateInstance:
                        certificateInstance = None
                        break
                if certificateInstance is not None:
                    cryptoCertificates.append({
                        "certificateInstance": certificateInstance,
                        "cryptoCertificate": {
                            "shortName": get_element_or_str(crypto_certificate_element, "SHORT-NAME"),
                            "isPrivate": get_element_or_false(crypto_certificate_element, "IS-PRIVATE")
                        }
                    })
        if cryptoCertificateToPortPrototypeMapping is not None:
            for item in cryptoCertificateToPortPrototypeMapping:
                if instance_specifier == item["portInstance"]:
                    return
            itemOb = {
                "portInstance": instance_specifier,
                "writeAccess": get_element_or_false(prototype_item, "WRITE-ACCESS")
            }
            ar_port_ref = get_element_or_none(prototype_item, "PORT-PROTOTYPE-IREF/TARGET-PORT-PROTOTYPE-REF")
            if ar_port_ref is not None:
                itemOb["portType"] = ar_port_ref.attrib["DEST"]
            crypto_certificate_element = self.model.find_referable(crypto_certificate_ref.text)
            if crypto_certificate_element is not None:
                certificateInstance = crypto_certificate_element.get_fqn()[1:]
                if certificateInstance is not None:
                    itemOb["certificateInstance"] = certificateInstance
            cryptoCertificateToPortPrototypeMapping.append(itemOb)



