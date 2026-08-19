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

from generator.intermediate_model.vehicle_package.vehicle_package import VehiclePackage
from generator.common.tree_helper import get_element_or_die, to_list, get_element_or_none, to_str, short_name, get_element_or_str


class VehicleBuilder:
    def __init__(self, model, _softwareBuilder):
        self._log = logging.getLogger(__name__)
        self.model = model
        self._softwareBuilder = _softwareBuilder

    def get_vehicle_packages(self, vehicle_package_list):
        vehicle_packages = []
        for fqn_item in vehicle_package_list:
            ar_vehicle_package = self.model.find_referable(fqn_item)
            if ar_vehicle_package is not None:
                vehicle_packages.append(
                    VehiclePackage(
                        fqn_item,
                        vehicle_package_info=self.read_vehicle_package_info(ar_vehicle_package)
                    )
                )
        return vehicle_packages

    def read_vehicle_package_info(self, ar_vehicle_package):
        result = {}
        software_packages = []
        driverNotifications = []
        ar_vehicle_driver_notifications = get_element_or_die(ar_vehicle_package, "DRIVER-NOTIFICATIONS/VEHICLE-DRIVER-NOTIFICATION")
        ar_vehicle_driver_notifications = to_list(ar_vehicle_driver_notifications)
        for ar_vehicle_driver_notification in ar_vehicle_driver_notifications:
            vehicle_driver_notification = {}
            vehicle_driver_notification["notificationState"] = to_str(get_element_or_none(ar_vehicle_driver_notification, "NOTIFICATION-STATE"))
            ar_approval_required = get_element_or_none(ar_vehicle_driver_notification,
                                                       "APPROVAL-REQUIRED")
            if ar_approval_required is not None:
                vehicle_driver_notification[
                    "approvalRequired"] = ar_approval_required.text == "true"
            driverNotifications.append(vehicle_driver_notification)
        result["driverNotification"] = driverNotifications
        result["shortName"] = ar_vehicle_package.SHORT_NAME
        signatureRef = get_element_or_none(ar_vehicle_package, "PACKAGER-SIGNATURE-REF")
        packagerSignature = self.get_software_signature(signatureRef)
        if packagerSignature is not None:
            result["packagerSignature"] = packagerSignature

        result["repository"] = to_str(get_element_or_none(ar_vehicle_package, "REPOSITORY"))
        vehicleRolloutSteps = []
        ar_vehicle_rollout_steps = get_element_or_die(ar_vehicle_package,
                                                      "ROLLOUT-QUALIFICATIONS/VEHICLE-ROLLOUT-STEP")
        ar_vehicle_rollout_steps = to_list(ar_vehicle_rollout_steps)
        for ar_vehicle_rollout_step in ar_vehicle_rollout_steps:
            vehicle_rollout_step = {}
            vehicle_rollout_step["shortName"] = ar_vehicle_rollout_step.SHORT_NAME
            vehicle_rollout_step["safetyPolicy"] = to_str(get_element_or_none(ar_vehicle_rollout_step, "SAFETY-POLICY"))
            ucmProcessing = []
            ar_ucm_steps = get_element_or_none(ar_vehicle_rollout_step,
                                               "UCM-PROCESSINGS/UCM-STEP")
            ar_ucm_steps = to_list(ar_ucm_steps)
            for ar_ucm_step in ar_ucm_steps:
                ucm_step = {}
                ucm_step["shortName"] = ar_ucm_step.SHORT_NAME
                ucm_ref = get_element_or_none(ar_ucm_step, "UCM-REF")
                if ucm_ref is not None:
                    ucm_step["ucm"] = short_name(ucm_ref)
                softwarePackageStep = []
                ar_software_package_steps = get_element_or_none(ar_ucm_step,
                                                                "SOFTWARE-PACKAGE-STEPS/SOFTWARE-PACKAGE-STEP")
                ar_software_package_steps = to_list(ar_software_package_steps)
                for ar_software_package_step in ar_software_package_steps:
                    software_package_step = {}
                    software_package_step["shortName"] = ar_software_package_step.SHORT_NAME
                    ar_activation_switch = to_str(get_element_or_none(ar_software_package_step,
                                                                      "ACTIVATION-SWITCH"))
                    if ar_activation_switch is not None:
                        software_package_step[
                            "activationSwitch"] = ar_activation_switch == "true"
                    ar_process_ref = get_element_or_none(ar_software_package_step,
                                                         "PROCESS-REF")
                    if ar_process_ref is not None:
                        software_package_step[
                            "process"] = ar_process_ref.text
                    transfer = {}
                    ar_software_package_storing = get_element_or_none(ar_software_package_step,
                                                                      "TRANSFERS/SOFTWARE-PACKAGE-STORING")
                    if ar_software_package_storing is not None:
                        transfer["storing"] = to_str(get_element_or_none(ar_software_package_storing,
                                                                         "STORING"))
                        ar_transfer_refs = get_element_or_none(ar_software_package_storing,
                                                               "TRANSFER-REFS/TRANSFER-REF")
                        ar_transfer = []
                        ar_transfer_refs = to_list(ar_transfer_refs)
                        for ar_transfer_ref in ar_transfer_refs:
                            ar_transfer.append(ar_transfer_ref.text)
                            software_packages.append(ar_transfer_ref.text)
                        transfer["transfer"] = ar_transfer
                    software_package_step["transfer"] = transfer

                    softwarePackageStep.append(software_package_step)
                ucm_step["softwarePackageStep"] = softwarePackageStep
                ucmProcessing.append(ucm_step)

            vehicle_rollout_step["ucmProcessing"] = ucmProcessing

            vehicleRolloutSteps.append(vehicle_rollout_step)
        result["rolloutQualification"] = vehicleRolloutSteps
        software_packages = list(set(software_packages))
        software_package_infos = []
        software_cluster_infos = []
        for software_package in software_packages:
            ar_software_packages = self.model.find_referable(software_package)
            software_package_infos.append(self._softwareBuilder.read_software_package_info(ar_software_packages))
            software_cluster_infos.append(self._softwareBuilder.read_software_cluster_info(ar_software_packages, None))
        result["software_package_infos"] = software_package_infos
        result["software_cluster_infos"] = software_cluster_infos
        ar_ucm_descriptions = get_element_or_die(ar_vehicle_package,
                                                 "UCMS/UCM-DESCRIPTION")
        ar_ucm_descriptions = to_list(ar_ucm_descriptions)
        ucm = []
        for ar_ucm_description in ar_ucm_descriptions:
            ucm_description = {}
            ucm_description["shortName"] = ar_ucm_description.SHORT_NAME
            ucm_description["identifier"] = to_str(get_element_or_none(ar_ucm_description,
                                                                       "IDENTIFIER"))
            ar_ucm_module_instantiation_ref = get_element_or_none(ar_ucm_description,
                                                                  "UCM-MODULE-INSTANTIATION-REF")
            if ar_ucm_module_instantiation_ref is not None:
                ucm_description["ucmModuleInstantiation"] = ar_ucm_module_instantiation_ref.text
                instantiation_element = self.model.find_referable(ar_ucm_module_instantiation_ref.text)
                if instantiation_element is not None:
                    ucm_description["ucmId"] = get_element_or_str(instantiation_element, "IDENTIFIER")
            ucm.append(ucm_description)
        result["ucm"] = ucm
        ar_ucm_master_fallback_refs = get_element_or_die(ar_vehicle_package,
                                                         "UCM-MASTER-FALLBACK-REFS/UCM-MASTER-FALLBACK-REF")
        ar_ucm_master_fallback_refs = to_list(ar_ucm_master_fallback_refs)
        ucmMasterFallback = []
        for ar_ucm_master_fallback_ref in ar_ucm_master_fallback_refs:
            ucmMasterFallback.append(short_name(ar_ucm_master_fallback_ref))
        result["ucmMasterFallback"] = ucmMasterFallback
        return result

    def get_software_signature(self, signatureRef):
        if signatureRef is not None:
            mapping = self.model.find_elements_of_type("COM-CERTIFICATE-TO-CRYPTO-CERTIFICATE-MAPPING")
            for item in mapping:
                serviceRef = get_element_or_none(item, "CRYPTO-SERVICE-CERTIFICATE-REF")
                if serviceRef is not None and serviceRef.text == signatureRef.text:
                    certificateRef = get_element_or_none(item, "CRYPTO-CERTIFICATE-REF")
                    if certificateRef is not None:
                        certificateEle = self.model.find_referable(certificateRef.text)
                        sd = get_element_or_none(certificateEle, "ADMIN-DATA/SDGS/SDG/SD")
                        if sd is not None and sd.attrib["GID"] == "iSOFT:ucm:CryptoCertificate:Path":
                            return sd.text
