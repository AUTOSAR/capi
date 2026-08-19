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

from generator.intermediate_model.software_package.software_package import SoftwarePackage
from generator.common.tree_helper import get_element_or_none, to_str, transition_number, get_element_or_die, get_element_or_str, to_list
from generator.parser.commen_util import get_diagnostic_address_from_software


class SoftwareBuilder:
    def __init__(self, model):
        self._log = logging.getLogger(__name__)
        self.model = model

    def get_software_packages(self, software_package_list):
        software_packages = []
        for software_package_item in software_package_list:
            ar_software_package = self.model.find_referable(software_package_item)
            if ar_software_package is not None:
                software_packages.append(
                    SoftwarePackage(
                        software_package_item,
                        software_cluster_info=self.read_software_cluster_info(ar_software_package, None),
                        software_package_info=self.read_software_package_info(ar_software_package)
                    )
                )
        return software_packages

    def get_software_clusters(self, software_cluster_list):
        software_clusters = []
        for softwareClusterFqn in software_cluster_list:
            ar_software_cluster = self.model.find_referable(softwareClusterFqn)
            if ar_software_cluster is not None:
                software_clusters.append(
                    self.read_software_cluster_info(None, ar_software_cluster)
                )
        return software_clusters

    def read_software_package_info(self, ar_software_package):
        result = {"actionType": to_str(get_element_or_none(ar_software_package, "ACTION-TYPE")).lower(),
                  "category": to_str(get_element_or_none(ar_software_package, "CATEGORY")),
                  "compressedSoftwarePackageSize": transition_number(get_element_or_none(ar_software_package, "COMPRESSED-SOFTWARE-PACKAGE-SIZE"),defaultInt=""),
                  "deltaPackageApplicableVersion": to_str(get_element_or_none(ar_software_package, "DELTA-PACKAGE-APPLICABLE-VERSION")),
                  "minUCMSupportedVersion": to_str(get_element_or_none(ar_software_package, "MINIMUM-SUPPORTED-UCM-VERSION")),
                  "maxUCMSupportedVersion": to_str(get_element_or_none(ar_software_package, "MAXIMUM-SUPPORTED-UCM-VERSION")),
                  "packagerID": transition_number(get_element_or_none(ar_software_package, "PACKAGER-ID"),defaultInt=""),
                  "shortName": ar_software_package.SHORT_NAME,
                  "fqn": ar_software_package.get_fqn(),
                  "uncompressedSoftwarePackageSize": transition_number(get_element_or_none(ar_software_package, "UNCOMPRESSED-SOFTWARE-CLUSTER-SIZE"),defaultInt="")}
        ar_software_cluster_ref = get_element_or_none(ar_software_package, "SOFTWARE-CLUSTER-REF")
        if ar_software_cluster_ref is not None:
            result["softwareCluster"] = ar_software_cluster_ref
        else:
            result["softwareCluster"] = ""
        signatureRef = get_element_or_none(ar_software_package, "PACKAGER-SIGNATURE-REF")
        vendorCertificate = self.get_software_signature(signatureRef)
        if vendorCertificate is not None:
            result["packagerSignature"] = vendorCertificate
        activation_action_sdg = get_element_or_none(ar_software_package, "ADMIN-DATA/SDGS/SDG")
        if activation_action_sdg is not None:
            if "iSOFT:ucm:SoftwarePackage" == activation_action_sdg.attrib["GID"]:
                activation_action_sd = get_element_or_none(activation_action_sdg, "SD")
                if activation_action_sd is not None:
                    for sd in activation_action_sd:
                        if "iSOFT:ucm:SoftwarePackage:ActivationAction" == sd.attrib["GID"]:
                            result["activationAction"] = sd.text
                        elif "iSOFT:ucm:SoftwarePackage:BasePackagePath" == sd.attrib["GID"]:
                            result["basePackagePath"] = sd.text

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

    def read_software_cluster_info(self, ar_software_package, ar_software_cluster_param):
        ar_software_cluster = None
        package_name = None
        if ar_software_package is not None:
            ar_software_cluster_ref = get_element_or_die(ar_software_package, "SOFTWARE-CLUSTER-REF")
            ar_software_cluster = self.model.find_referable(ar_software_cluster_ref.text)
            package_name = ar_software_package.SHORT_NAME
        if ar_software_cluster_param is not None:
            ar_software_cluster = ar_software_cluster_param
        category = get_element_or_none(ar_software_cluster, "CATEGORY")
        if category is None:
            category = "APPLICATION_LAYER"

        result = {"category": category,
                  "shortName": ar_software_cluster.SHORT_NAME,
                  "packageShortName": package_name,
                  "vendorID": transition_number(get_element_or_none(ar_software_cluster, "VENDOR-ID")),
                  "version": to_str(get_element_or_die(ar_software_cluster, "VERSION")),
                  "typeApproval": to_str(get_element_or_none(ar_software_cluster, "TYPE-APPROVAL"))
                  }
        sd = get_element_or_none(ar_software_cluster, "ADMIN-DATA/SDGS/SDG/SD")
        if sd is not None and sd.attrib["GID"] == "iSOFT:ucm:SoftwareCluster:ExtraPackageDir":
            result["extraPackageDir"] = sd.text
        signatureRef = get_element_or_none(ar_software_cluster, "VENDOR-SIGNATURE-REF")
        vendorCertificate = self.get_software_signature(signatureRef)
        if vendorCertificate is not None:
            result["vendorSignature"] = vendorCertificate
        category = get_element_or_str(ar_software_cluster, "CATEGORY")
        if category == "ISOFT_FRAMEWORK":
            result["fqn"] = "/ISOFT/Development/SoftwareClusters/framework"
        else:
            result["fqn"] = ar_software_cluster.get_fqn()

        try:
            result['uuid'] = ar_software_cluster.attrib["UUID"]
        except:
            result['uuid'] = ""

        diagAddress = get_diagnostic_address_from_software(ar_software_cluster)
        if diagAddress is not None:
            result["diagnosticAddress"] = diagAddress

        functionGroupSetList = []
        functionGroupSets = []
        modeDeclarationGroupsList = []
        modeDeclarationGroups = []
        executableList = []
        containedMachines = []

        arElements = get_element_or_none(ar_software_cluster, "CONTAINED-AR-ELEMENT-REFS/CONTAINED-AR-ELEMENT-REF")
        if arElements is not None:
            arElements = to_list(arElements)
            for arElement in arElements:
                if arElement.attrib["DEST"] == "EXECUTABLE":
                    executableList.append(arElement.text)
                elif arElement.attrib["DEST"] == "MACHINE":
                    containedMachines.append(arElement.text)
                elif arElement.attrib["DEST"] == "FUNCTION-GROUP-SET":
                    self.get_function_group_sets(arElement.text, functionGroupSetList, functionGroupSets, modeDeclarationGroupsList, modeDeclarationGroups)
                # elif arElement.attrib["DEST"] == "MODE-DECLARATION-GROUP":
                #     self.get_mode_declaration_group(arElement.text, modeDeclarationGroupsList, modeDeclarationGroups)

        result["claimedFunctionGroups"] = self.get_sc_claimed_function_groups(ar_software_cluster)

        contained_process_refs = get_element_or_none(ar_software_cluster, "CONTAINED-PROCESS-REFS")
        if contained_process_refs is not None:
            containedProcessList = []
            contained_process_map = contained_process_refs.find_elements_of_type("CONTAINED-PROCESS-REF")
            for contained_process in contained_process_map:
                containedProcessList.append(contained_process.text)
                self.get_process_function_group_sets(ar_software_cluster, contained_process.text, functionGroupSetList, functionGroupSets, modeDeclarationGroupsList, modeDeclarationGroups, result["claimedFunctionGroups"])

            result["containedProcess"] = containedProcessList

        result["executableList"] = executableList
        result["containedMachines"] = containedMachines
        if len(functionGroupSets) > 0:
            result["functionGroupSets"] = {
                "functionGroups": functionGroupSets
            }
        result["functionGroupSetList"] = functionGroupSetList
        result["modeDeclarationGroupsList"] = modeDeclarationGroupsList
        result["modeDeclarationGroups"] = {
            "ModeDeclarationGroups": modeDeclarationGroups
        }
        licenses = []
        ar_license_refs = get_element_or_none(ar_software_cluster,
                                                             "LICENSE-REFS/LICENSE-REF")
        ar_license_refs = to_list(ar_license_refs)
        for ar_license_ref in ar_license_refs:
            licenses.append(ar_license_ref.text)
        result["licenses"] = licenses

        ar_conflicts = get_element_or_none(ar_software_cluster, "CONFLICTS-TO")
        if ar_conflicts is not None:
            conflictsTo = {}
            ar_category = get_element_or_none(ar_conflicts, "CATEGORY")
            if ar_category is not None:
                if ar_category == "FUNCTIONAL_DEPENDENCY":
                    conflictsTo["category"] = "functionalDependency"
                if ar_category == "STRUCTURAL_DEPENDENCY":
                    conflictsTo["category"] = "structuralDependency"
                ar_category = get_element_or_none(ar_conflicts, "CATEGORY")
            ar_operator = get_element_or_none(ar_conflicts, "OPERATOR")
            if ar_operator is not None:
                if ar_operator == "LOGICAL-AND":
                    conflictsTo["operator"] = "and"
                if ar_operator == "LOGICAL-OR":
                    conflictsTo["operator"] = "or"
            conflictsTo["parts"] = self.read_parts_info(get_element_or_none(ar_conflicts, "PARTS"), [])
            result["conflictsTo"] = conflictsTo
        ar_depends_on = get_element_or_none(ar_software_cluster, "DEPENDS-ON")
        if ar_depends_on is not None:
            dependsOn = {}
            ar_category = get_element_or_none(ar_depends_on, "CATEGORY")
            if ar_category is not None:
                if ar_category == "FUNCTIONAL_DEPENDENCY":
                    dependsOn["category"] = "functionalDependency"
                if ar_category == "STRUCTURAL_DEPENDENCY":
                    dependsOn["category"] = "structuralDependency"
            ar_operator = get_element_or_none(ar_depends_on, "OPERATOR")
            if ar_operator is not None:
                if ar_operator == "LOGICAL-AND":
                    dependsOn["operator"] = "and"
                if ar_operator == "LOGICAL-OR":
                    dependsOn["operator"] = "or"
            dependsOn["parts"] = self.read_parts_info(get_element_or_none(ar_depends_on, "PARTS"), [])
            result["dependsOn"] = dependsOn
        return result

    def get_sc_claimed_function_groups(self, ar_software_cluster):
        claimedFunctionGroups = []
        ar_claimed_function_group_refs = get_element_or_none(ar_software_cluster, "CLAIMED-FUNCTION-GROUP-REFS/CLAIMED-FUNCTION-GROUP-REF")
        ar_claimed_function_group_refs = to_list(ar_claimed_function_group_refs)
        for ar_claimed_function_group_ref in ar_claimed_function_group_refs:
            claimedFunctionGroups.append(ar_claimed_function_group_ref.text)
        return claimedFunctionGroups

    def read_parts_info(self, ar_parts, parts):
        if ar_parts is not None:
            ar_software_cluster_dependency_formulas = get_element_or_none(ar_parts, "SOFTWARE-CLUSTER-DEPENDENCY-FORMULA")
            ar_software_cluster_dependency_formulas = to_list(ar_software_cluster_dependency_formulas)
            for ar_software_cluster_dependency_formula in ar_software_cluster_dependency_formulas:
                software_cluster_dependency_formula_info = {}
                ar_category = get_element_or_none(ar_software_cluster_dependency_formula, "CATEGORY")
                if ar_category is not None:
                    if ar_category == "FUNCTIONAL_DEPENDENCY":
                        software_cluster_dependency_formula_info["category"] = "functionalDependency"
                    if ar_category == "STRUCTURAL_DEPENDENCY":
                        software_cluster_dependency_formula_info["category"] = "structuralDependency"
                ar_operator = get_element_or_none(ar_software_cluster_dependency_formula, "OPERATOR")
                if ar_operator is not None:
                    if ar_operator == "LOGICAL-AND":
                        software_cluster_dependency_formula_info["operator"] = "and"
                    if ar_operator == "LOGICAL-OR":
                        software_cluster_dependency_formula_info["operator"] = "or"
                software_cluster_dependency_formula_info["parts"] = self.read_parts_info(get_element_or_none(ar_software_cluster_dependency_formula, "PARTS"), [])
                software_cluster_dependency_formula_info["type"] = "software_cluster_dependency_formula_info"
                parts.append(software_cluster_dependency_formula_info)
            ar_software_cluster_dependency_compare_conditions = ar_parts.find_elements_of_type("SOFTWARE-CLUSTER-DEPENDENCY-COMPARE-CONDITION")
            for ar_software_cluster_dependency_compare_condition in ar_software_cluster_dependency_compare_conditions:
                software_cluster_dependency_compare_condition_info = {}
                ar_compare_type = get_element_or_none(ar_software_cluster_dependency_compare_condition, "COMPARE-TYPE")
                if ar_compare_type is not None:
                    if ar_compare_type == "IS-EQUAL":
                        software_cluster_dependency_compare_condition_info["compareType"] = "isEqual"
                    if ar_compare_type == "IS-GREATER-THAN":
                        software_cluster_dependency_compare_condition_info["compareType"] = "isGreaterThan"
                    if ar_compare_type == "IS-GREATER-THAN-OR-EQUAL":
                        software_cluster_dependency_compare_condition_info["compareType"] = "isGreaterThanOrEqual"
                    if ar_compare_type == "IS-LESS-THAN":
                        software_cluster_dependency_compare_condition_info["compareType"] = "isLessThan"
                    if ar_compare_type == "IS-LESS-THAN-OR-EQUAL":
                        software_cluster_dependency_compare_condition_info["compareType"] = "isLessThanOrEqual"
                ar_consider_build_number = get_element_or_none(ar_software_cluster_dependency_compare_condition, "CONSIDER-BUILD-NUMBER")
                if ar_consider_build_number:
                    software_cluster_dependency_compare_condition_info["considerBuildNumber"] = ar_consider_build_number.text == "true"
                ar_software_cluster_ref = get_element_or_none(ar_software_cluster_dependency_compare_condition, "SOFTWARE-CLUSTER-REF")
                ar_software_cluster = self.model.find_referable(ar_software_cluster_ref.text)
                category = get_element_or_str(ar_software_cluster, "CATEGORY")
                if category == "ISOFT_FRAMEWORK":
                    software_cluster_dependency_compare_condition_info["softwareCluster"] = "/ISOFT/Development/SoftwareClusters/framework"
                else:
                    software_cluster_dependency_compare_condition_info["softwareCluster"] = ar_software_cluster.get_fqn()
                software_cluster_dependency_compare_condition_info["version"] = get_element_or_str(ar_software_cluster_dependency_compare_condition, "VERSION")
                software_cluster_dependency_compare_condition_info["type"] = "software_cluster_dependency_compare_condition_info"
                parts.append(software_cluster_dependency_compare_condition_info)
        return parts

    def get_process_function_group_sets(self, ar_software_cluster, processFqn, functionGroupSetList, functionGroupSets, modeDeclarationGroupsList, modeDeclarationGroups, cFGList):
        ar_process = self.model.find_referable(processFqn)
        ar_md_startup_configs = get_element_or_none(ar_process, "STATE-DEPENDENT-STARTUP-CONFIGS/STATE-DEPENDENT-STARTUP-CONFIG")
        ar_md_startup_configs = to_list(ar_md_startup_configs)
        for ar_md_startup_config in ar_md_startup_configs:
            ar_fg_modes = get_element_or_none(ar_md_startup_config, "FUNCTION-GROUP-STATE-IREFS/FUNCTION-GROUP-STATE-IREF")
            ar_fg_modes = to_list(ar_fg_modes)
            for ar_fg_mode in ar_fg_modes:
                ar_mode_group_ref = get_element_or_die(ar_fg_mode, "CONTEXT-MODE-DECLARATION-GROUP-PROTOTYPE-REF")
                # assert ar_mode_group_ref.text in cFGList, "[BASE] CODE-011: The ModeDeclarationGroupPrototype [{}] in the process [{}] startup configuration is not referenced by the SoftwareCluster[{}]. LocalPath:{}. ".format(ar_mode_group_ref.text, processFqn, ar_software_cluster.get_fqn(), ar_process.get_path())
                ar_mode_group = str(ar_mode_group_ref).split('/')
                setRef = ""
                for item in ar_mode_group[1:-1]:
                    setRef += "/" + item
                if setRef != "":
                    self.get_function_group_sets(setRef, functionGroupSetList, functionGroupSets, modeDeclarationGroupsList, modeDeclarationGroups)

    def get_function_group_sets(self, setRef, functionGroupSetList, functionGroupSets, modeDeclarationGroupsList, modeDeclarationGroups):
        if setRef not in functionGroupSetList:
            functionGroupSetList.append(setRef)
            setEle = self.model.find_referable(setRef)
            groupMap = get_element_or_none(setEle, "FUNCTION-GROUPS/MODE-DECLARATION-GROUP-PROTOTYPE")
            if groupMap is not None:
                groupMap = to_list(groupMap)
                # functionGroups = []
                for group in groupMap:
                    mode_declaration_group_ref = get_element_or_none(group, "TYPE-TREF")
                    if mode_declaration_group_ref is not None:
                        mode_machine_declaration_group = self.model.find_referable(mode_declaration_group_ref.text)
                        ar_machine_mode_declarations = get_element_or_none(mode_machine_declaration_group, "MODE-DECLARATIONS/MODE-DECLARATION")
                        modes = []
                        isHaveOff = False
                        for ar_machine_mode in ar_machine_mode_declarations:
                            if ar_machine_mode.SHORT_NAME.lower() == "off":
                                isHaveOff = True
                            modes.append(ar_machine_mode.SHORT_NAME)
                        assert isHaveOff, "[BASE] CODE-012: ModeDeclarationGroup[{}] referenced by FunctionGroup.ModeDeclarationGroupPrototype[{}] must exist off. Path:{}. LocalPath:{}.".format(mode_machine_declaration_group.SHORT_NAME, group.SHORT_NAME, group.get_fqn(), group.get_path())
                        groupOb = {
                            "name": group.get_fqn(),
                            "states": modes
                        }
                        if groupOb not in functionGroupSets:
                            functionGroupSets.append(groupOb)
                        self.get_mode_declaration_group(mode_declaration_group_ref.text, modeDeclarationGroupsList, modeDeclarationGroups)

    def get_mode_declaration_group(self, groupRef, modeDeclarationGroupsList, modeDeclarationGroups):
        if groupRef not in modeDeclarationGroupsList:
            modeDeclarationGroupsList.append(groupRef)
            mode_machine_declaration_group = self.model.find_referable(groupRef)
            ar_machine_mode_declarations = get_element_or_none(mode_machine_declaration_group, "MODE-DECLARATIONS/MODE-DECLARATION")
            modes = []
            for ar_machine_mode in ar_machine_mode_declarations:
                modes.append(ar_machine_mode.SHORT_NAME)
            modeDeclarationGroups.append({
                "FQN": groupRef,
                "Modes": modes
            })
