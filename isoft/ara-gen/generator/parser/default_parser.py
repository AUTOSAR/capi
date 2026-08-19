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

from generator.generator.template_renderer import TemplateRenderer
from generator.parser.cg_builder import CGBuilder
from generator.parser.diagnosis_builder import DiagnosisBuilder
from generator.parser.exe_builder import ExecutableBuilder
from generator.parser.machine_builder import MachineBuilder
from generator.parser.nm_xml_builder import NmXmlBuilder
from generator.parser.phm_build import PhmBuilder
from generator.parser.process_builder import ProcessBuilder
from generator.parser.instance_specifier import InstanceSpecifierBuilder
from generator.parser.software_builder import SoftwareBuilder
from generator.parser.state_builder import StateBuilder
from generator.parser.vehicle_builder import VehicleBuilder
from generator.views.process_view import ProcessView
from generator.views.software_package_view import SoftwarePackageView
from generator.views.vehicle_package_view import VehiclePackageView
from generator.common.tree_helper import get_element_or_die, get_element_or_none, to_list
from generator.parser.interface_builder import InterfaceBuilder
from generator.parser.network_binding_builder import NetworkBindingBuilder


# Application tags
_APPL_TYPE_TREF = "APPLICATION-TYPE-TREF"
_ADAPTIVE_APPL_TYPE = "ADAPTIVE-APPLICATION-SW-COMPONENT-TYPE"


class DefaultParser(object):
    """
    Handles parsing the ARXML and providing the intermediate model that it
    represents.
    """

    def __init__(self, model, args):
        self.model = model
        self._log = logging.getLogger(__name__)
        self._args = args
        self._package_path_fallback = args.force_pkg_path_fallback
        self._generate_all = not (args.machines or args.processes or args.software_components or args.executables or args.softwarePackageFqn or args.software_cluster or args.vehiclePackageFqn)
        self._softwareBuilder = SoftwareBuilder(self.model)
        self._vehicleBuilder = VehicleBuilder(self.model, self._softwareBuilder)
        self._interface_builder = InterfaceBuilder(self.model, self._package_path_fallback)
        self._network_binding_builder = NetworkBindingBuilder(self.model)
        self._phmdeploymentbuilder = PhmBuilder(self.model, self._package_path_fallback)
        self._processBuilder = ProcessBuilder(self.model, args, self._network_binding_builder, self._interface_builder, self._phmdeploymentbuilder)
        self._machineBuilder = MachineBuilder(self.model, self._processBuilder, self._phmdeploymentbuilder)
        self._diagnosisBuilder = DiagnosisBuilder(self.model)
        self._cgBuilder = CGBuilder(self.model)
        self._stateBuilder = StateBuilder(self.model, self._args)
        self._exeBuilder = ExecutableBuilder(self.model, self._diagnosisBuilder, self._interface_builder, self._network_binding_builder, self._phmdeploymentbuilder)
        self._nmXmlBuilder = NmXmlBuilder(self.model, self._args)

    def get_processes(self, processes_list):
        ar_processes = self.model.find_elements_of_type('PROCESS')
        processes = []
        for ar_process in ar_processes:
            fqn = ar_process.get_fqn()
            if fqn in processes_list:
                process = self._processBuilder.get_process_info(ar_process)
                processes.append(process)
        return processes

    def _del_port(self, port, keyList):
        newPortList = []
        if "interfaceDeploymentList" in port:
            port_list = port["interfaceDeploymentList"]
            for port_item in port_list:
                if port_item["portPrototypeRef"] not in keyList:
                    newPortList.append(port_item)
                    keyList.append(port_item["portPrototypeRef"])
            port["interfaceDeploymentList"] = newPortList
        return port

    def _validate_against_reference(self, list_to_validate, reference_list):
        invalid_items = []
        valid_items = []
        for item in list_to_validate:
            if item.startswith('/'):
                # fully qualified name provided
                if item in reference_list:
                    self._log.debug("item %s: validated ok", item)
                    valid_items += [item]
                else:
                    self._log.error("[INIT] CODE-007: item %s: validation failed, not found", item)
                    invalid_items += [item]
            else:
                # partial name provided
                alternatives = [x for x in reference_list if x.split("/")[-1]==item]
                if len(alternatives) == 1:
                    self._log.debug("item %s: validated ok", alternatives[0])
                    valid_items += alternatives
                elif len(alternatives) == 0:
                    self._log.error("[INIT] CODE-007: item %s: validation failed, not found", item)
                    invalid_items += [item]
                else:
                    self._log.error("[INIT] CODE-008: item %s: validation failed, found multiple times", item)
                    invalid_items += [item]

        return {"invalid": invalid_items, "valid": valid_items}

    def _find_exe_swc(self, executables):
        result = []
        for executable in executables:
            ar_executable = self.model.find_referable(executable)
            ar_component_refs = ar_executable.find_elements_of_type(_APPL_TYPE_TREF, DEST=_ADAPTIVE_APPL_TYPE)
            if ar_component_refs:
                for ar_component_ref in ar_component_refs:
                    result.append(ar_component_ref.text)
        return result

    def _find_target_executables_or_abort(self, executables):
        reference = self.get_executables_fqn()
        result = self._validate_against_reference(executables, reference)
        if result["invalid"]:
            self._log.error("[INIT] CODE-009: Generation failed for unrecognized executables %s!", result["invalid"])
            sys.exit(
                "[INIT] CODE-009: Generation failed for unrecognized executables {0}! "
                "Possible executables are {1}".format(
                    result["invalid"],
                    reference
                )
            )

        return result["valid"]

    def _find_target_machines_or_abort(self, machines):
        reference = self.get_machines_fqn()
        result = self._validate_against_reference(machines, reference)
        if result["invalid"]:
            self._log.error("[INIT] CODE-009: Generation failed for unrecognized machines %s!", result["invalid"])
            sys.exit(
                "[INIT] CODE-009: Generation failed for unrecognized machines {0}! "
                "Possible machines are {1}".format(
                    result["invalid"],
                    reference
                )
            )

        return result["valid"]

    def _find_target_softwarePackages_or_abort(self, softwarePackageFqn):
        reference = self.get_software_packages_fqn()
        result = self._validate_against_reference(softwarePackageFqn, reference)
        if result["invalid"]:
            self._log.error("[INIT] CODE-009: Generation failed for unrecognized software package %s!", result["invalid"])
            sys.exit(
                "[INIT] CODE-009: Generation failed for unrecognized software package {0}! "
                "Possible software packages are {1}".format(
                    result["invalid"],
                    reference
                )
            )

        return result["valid"]

    def _find_target_softwareClusters_or_abort(self, softwareClustersFqn):
        reference = self.get_software_clusters_fqn()
        result = self._validate_against_reference(softwareClustersFqn, reference)
        if result["invalid"]:
            self._log.error("[INIT] CODE-009: Generation failed for unrecognized software cluster %s!", result["invalid"])
            sys.exit(
                "[INIT] CODE-009: Generation failed for unrecognized software cluster {0}! "
                "Possible software clusters are {1}".format(
                    result["invalid"],
                    reference
                )
            )

        return result["valid"]

    def _find_target_vehiclePackages_or_abort(self, vehiclePackageFqn):
        reference = self.get_vehicle_packages_fqn()
        result = self._validate_against_reference(vehiclePackageFqn, reference)
        if result["invalid"]:
            self._log.error("[INIT] CODE-009: Generation failed for unrecognized vehicle package %s!", result["invalid"])
            sys.exit(
                "[INIT] CODE-009: Generation failed for unrecognized vehicle packages {0}! "
                "Possible vehicle packages are {1}".format(
                    result["invalid"],
                    reference
                )
            )

        return result["valid"]

    def _find_target_sw_components_or_abort(self, sw_components):
        reference = self.get_sw_components_fqn()
        result = self._validate_against_reference(sw_components, reference)
        if result["invalid"]:
            self._log.error("[INIT] CODE-009: Generation failed for unrecognized sw_components %s!", result["invalid"])
            sys.exit(
                "[INIT] CODE-009: Generation failed for unrecognized sw_components {0}! "
                "Possible sw_components are {1}".format(
                    result["invalid"],
                    reference
                )
            )

        return result["valid"]

    def _find_target_processes_or_abort(self, processes_list):
        reference = self.get_processes_fqn()
        result = self._validate_against_reference(processes_list, reference)
        if result["invalid"]:
            self._log.error("[INIT] CODE-009: Generation failed for unrecognized processes %s!", result["invalid"])
            sys.exit(
                "[INIT] CODE-009: Generation failed for unrecognized processes {0}! "
                "Possible processes are {1}".format(
                    result["invalid"],
                    reference
                )
            )

        return result["valid"]

    def get_executables_fqn(self):
        """Get a list of all executables fqn."""
        ar_executables = self.model.find_elements_of_type('EXECUTABLE')
        return [executable.get_fqn() for executable in ar_executables]

    def get_machines_fqn(self):
        """Get a list of all machines fqn."""
        ar_machines = self.model.find_elements_of_type('MACHINE')
        return [machine.get_fqn() for machine in ar_machines]

    def get_software_packages_fqn(self):
        """Get a list of all software fqn."""
        ar_software_packages = self.model.find_elements_of_type('SOFTWARE-PACKAGE')
        return [softwarePackage.get_fqn() for softwarePackage in ar_software_packages]

    def get_software_clusters_fqn(self):
        """Get a list of all software cluster fqn."""
        ar_software_clusters = self.model.find_elements_of_type('SOFTWARE-CLUSTER')
        return [softwareCluster.get_fqn() for softwareCluster in ar_software_clusters]

    def get_vehicle_packages_fqn(self):
        """Get a list of all software fqn."""
        ar_vehicle_packages = self.model.find_elements_of_type('VEHICLE-PACKAGE')
        return [vehiclePackage.get_fqn() for vehiclePackage in ar_vehicle_packages]

    def get_sw_components_fqn(self):
        """Get a list of all Software Components fqn."""
        ar_sw_components = self.model.find_elements_of_type(_ADAPTIVE_APPL_TYPE)
        return [ar_sw_component.get_fqn() for ar_sw_component in ar_sw_components]

    def get_processes_fqn(self):
        ar_processes = self.model.find_elements_of_type('PROCESS')
        return [ar_process.get_fqn() for ar_process in ar_processes]

    def get_swcl_full_info(self):
        result = []
        ar_software_clusters = self.model.find_elements_of_type('SOFTWARE-CLUSTER')
        for ar_software_cluster in ar_software_clusters:
            softwareclusterfqn = ar_software_cluster.get_fqn()
            softwareClusterCategory = get_element_or_none(ar_software_cluster, 'CATEGORY')
            if softwareClusterCategory is None:
                softwareClusterCategory = "APPLICATION_LAYER"
            softwareclusterversion = get_element_or_die(ar_software_cluster, 'VERSION')
            arprocessrefs = ar_software_cluster.find_elements_of_type("CONTAINED-PROCESS-REF")
            parts = []
            newParts = get_element_or_none(ar_software_cluster, "DEPENDS-ON/PARTS")
            self.software_cluster_depend_on(newParts, parts)
            partsStr = ",".join(parts)
            if len(arprocessrefs) > 0:
                for arprocessref in arprocessrefs:
                    ar_process = self.model.find_referable(arprocessref.text)
                    executable = get_element_or_none(ar_process, 'EXECUTABLE-REF')
                    application_type = ""
                    exeFqn = ""
                    if executable is not None:
                        ar_executable = self.model.find_referable(executable.text)
                        exeFqn = ar_executable.get_fqn()
                        application_type = get_element_or_die(ar_executable, 'ROOT-SW-COMPONENT-PROTOTYPE/APPLICATION-TYPE-TREF')
                    ar_processtomachine_mappings = self.model.find_elements_of_type('PROCESS-TO-MACHINE-MAPPING', accept=lambda e: str(e.PROCESS_REF) == ar_process.get_fqn())
                    machine = None
                    for ar_processtomachine_mapping in ar_processtomachine_mappings:
                        machine = get_element_or_none(ar_processtomachine_mapping, 'MACHINE-REF')
                        if machine is not None:
                            result.append(softwareclusterfqn + ":" + softwareClusterCategory + ":" + softwareclusterversion + ":" + partsStr + ":" + ar_process.get_fqn() + ":" + exeFqn + ":" + application_type + ":" + machine)
                    if machine is None:
                        result.append(softwareclusterfqn + ":" + softwareClusterCategory + ":" + softwareclusterversion + ":" + partsStr + ":" + ar_process.get_fqn() + ":" + exeFqn + ":" + application_type + ":")
            else:
                result.append(softwareclusterfqn + ":" + softwareClusterCategory + ":" + softwareclusterversion + ":" + partsStr + "::::")
        return result

    def software_cluster_depend_on(self, ar_parts, parts):
        if ar_parts is not None:
            ar_software_cluster_dependency_formulas = get_element_or_none(ar_parts, "SOFTWARE-CLUSTER-DEPENDENCY-FORMULA")
            ar_software_cluster_dependency_formulas = to_list(ar_software_cluster_dependency_formulas)
            for ar_software_cluster_dependency_formula in ar_software_cluster_dependency_formulas:
                self.software_cluster_depend_on(get_element_or_none(ar_software_cluster_dependency_formula, "PARTS"), parts)
            ar_software_cluster_dependency_compare_conditions = ar_parts.find_elements_of_type("SOFTWARE-CLUSTER-DEPENDENCY-COMPARE-CONDITION")
            for ar_software_cluster_dependency_compare_condition in ar_software_cluster_dependency_compare_conditions:
                ar_software_cluster_ref = get_element_or_none(ar_software_cluster_dependency_compare_condition, "SOFTWARE-CLUSTER-REF")
                ar_software_cluster = self.model.find_referable(ar_software_cluster_ref.text)
                fqn = ar_software_cluster.get_fqn()
                if fqn not in parts:
                    parts.append(fqn)
                newParts = get_element_or_none(ar_software_cluster, "DEPENDS-ON/PARTS")
                self.software_cluster_depend_on(newParts, parts)

    def create_state_some_ip(self):
        processes = self.get_processes_fqn()
        return self._stateBuilder.get_state_xml(processes)

    def create_nm_xml(self):
        machineList = self.get_machines_fqn()
        return self._nmXmlBuilder.nm_xml(machineList)

    def process_singletone_args(self, args, log):
        done = False
        if args.list_machines:
            machines = self.get_machines_fqn()
            log.info("machines in the input data: %s", machines)
            if machines:
                for machine in machines:
                    print(machine)
            else:
                log.warning("No machines were found in the input data!")
            done = True
        if args.list_swc:
            software_components = self.get_sw_components_fqn()
            log.info("Software Components in the input data: %s", software_components)
            if software_components:
                for software_component in software_components:
                    print(software_component)
            else:
                log.warning("No Software Components were found in the input data!")
            done = True
        if args.list_swp:
            software_packages = self.get_software_packages_fqn()
            log.info("Software Packages in the input data: %s", software_packages)
            if software_packages:
                for software_package in software_packages:
                    print(software_package)
            else:
                log.warning("No Software Packages were found in the input data!")
            done = True
        if args.list_processes:
            processes = self.get_processes_fqn()
            log.info("processes in the input data: %s", processes)
            if processes:
                for process in processes:
                    print(process)
            else:
                log.warning("No processes were found in the input data!")
            done = True
        if args.list_swcl_info:
            swclFullInfo = self.get_swcl_full_info()
            if swclFullInfo:
                for item in swclFullInfo:
                    print(item)
            else:
                log.warning("No swcl were found in the input data!")
            done = True
        if done:
            log.info('Finished.')
            sys.exit(0)

    def get_other_(self):
        """
        build and provide the intermediate model.
        """
        softwarePackages = []
        softwareClusters = []
        vehiclePackages = []
        # swc = []
        if self._generate_all:
            softwarePackages = self.get_software_packages_fqn()
            softwareClusters = self.get_software_clusters_fqn()
            vehiclePackages = self.get_vehicle_packages_fqn()
            # swc = self.get_sw_components_fqn()
        else:
            if self._args.softwarePackageFqn:
                softwarePackages = self._find_target_softwarePackages_or_abort(self._args.softwarePackageFqn)

            if self._args.vehiclePackageFqn:
                vehiclePackages = self._find_target_vehiclePackages_or_abort(self._args.vehiclePackageFqn)

            if self._args.software_cluster:
                softwareClusters = self._find_target_softwareClusters_or_abort(self._args.software_cluster)

            # if self._args.software_components:
            #     swc = self._find_target_sw_components_or_abort(self._args.software_components)

        return {
            # "components": self._exeBuilder.get_components_views(swc),
            "softwarePackages": self._get_software_package_views(softwarePackages),
            "softwareClusters": self._get_software_cluster_views(softwareClusters),
            "vehiclePackages": self._get_vehicle_package_views(vehiclePackages),
            "errors": self._interface_builder.get_error_domain_views()
        }

    def get_build_exe_(self):
        executables = self.get_executables_fqn()
        if self._args.executables:
            executables = self._find_target_executables_or_abort(self._args.executables)
        return {
            "build_info": self._exeBuilder.get_build_info(executables)
            # "components": self._exeBuilder.get_components_views(exe_swc),
        }

    def get_project(self):
        executables = []
        if self._args.executables:
            executables = self._find_target_executables_or_abort(self._args.executables)
        return {
            "executable": self._exeBuilder.get_executables(executables),
            "instance": InstanceSpecifierBuilder(self.model).get_all_instance(executables)
        }

    def get_process_(self):
        processes = []
        if self._generate_all:
            processes = self.get_processes_fqn()
        else:
            if self._args.processes:
                processes = self._find_target_processes_or_abort(self._args.processes)
        return {"processes": self._get_processes_views(processes)}

    def get_machine_(self):
        machines = []
        if self._generate_all:
            machines = self.get_machines_fqn()
        else:
            if self._args.machines:
                machines = self._find_target_machines_or_abort(self._args.machines)
        returnM = self._machineBuilder.get_machines_views(machines)
        return {"machines": returnM}

    def get_diag_(self):
        processes = []
        if self._generate_all:
            processes = self.get_processes_fqn()
        else:
            if self._args.processes:
                processes = self._find_target_processes_or_abort(self._args.processes)
        returnDiag = self._get_diag(self.get_software_clusters_fqn(), processes)
        return {"diag": returnDiag}

    def _get_processes_views(self, processes_list):
        processes = [ProcessView(process) for process in self.get_processes(processes_list)]
        return processes

    def _get_software_package_views(self, software_package_list):
        softwarePackages = [SoftwarePackageView(softwarePackage) for softwarePackage in self._softwareBuilder.get_software_packages(software_package_list)]
        return softwarePackages

    def _get_software_cluster_views(self, software_cluster_list):
        softwareClusters = self._softwareBuilder.get_software_clusters(software_cluster_list)
        return softwareClusters

    def _get_diag(self, software_cluster_list, processes):
        diag = self._diagnosisBuilder.get_diag_by_cluster(software_cluster_list, processes)
        return diag

    def _get_vehicle_package_views(self, vehicle_package_list):
        vehiclePackages = [VehiclePackageView(vehiclePackage) for vehiclePackage in self._vehicleBuilder.get_vehicle_packages(vehicle_package_list)]
        return vehiclePackages


