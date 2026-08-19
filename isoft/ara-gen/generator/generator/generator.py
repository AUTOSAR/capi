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
"""
Generator
"""
import importlib
import json
import logging
import os

from generator.common import aragen_args
from generator.generator.datatype_container import DataTypeContainer
from generator.generator.generator_settings import GeneratorSettings
from generator.generator.model_adjust import ModelAdjust
from generator.generator.template_renderer import TemplateRenderer
from generator.intermediate_model.communication_management.ara_com_deployment.dds. \
    dds_service_deployment import DdsServiceDeployment  # noqa
from generator.intermediate_model.communication_management.ara_com_deployment. \
    someip.someip_service_deployment import SomeIpServiceDeployment  # noqa
from generator.intermediate_model.communication_management.ara_com_deployment. \
    user_defined.user_defined_service_deployment import UserDefinedServiceDeployment  # noqa
from generator.intermediate_model.utils.bean2json_encoder import JsonEncoder
from generator.parser.exception_handling import handle_method_exceptions
from generator.views.aragen_helper_info_view import AragenHelperInfoView
from generator.views.component_view import ComponentView
from generator.views.dds.dds_service_view import DdsServiceView
from generator.views.model_to_view import ViewManipulations
from generator.views.service_view import ServiceView
from generator.views.someip.someip_service_view import SomeIpServiceView
from generator.views.user_defined.user_defined_service_view import UserDefinedServiceView
from generator.views.view_factory import ViewFactory


class Generator:
    """
    Generator for intermediate model.
    """

    _LIB_BINDING_PACKAGE = 'generator.generator.lib_binding'
    _LIB_BINDING = {
        'someip': {
            'nsomeip2': {
                    'module': _LIB_BINDING_PACKAGE + '.nsomeip2_binding',
                    'class': 'Nsomeip2Binding',
            }
        },
        'user_defined': {
            'npc': {
                'module': _LIB_BINDING_PACKAGE + '.npc_binding',
                'class': 'NpcBinding',
            },
            'icc': {
                'module': _LIB_BINDING_PACKAGE + '.icc_binding',
                'class': 'IccBinding',
            }
        },
        'dds': {
            'fastdds': {
                'module': _LIB_BINDING_PACKAGE + '.fastdds_binding',
                'class': 'FastDdsBinding',
            }
        }
    }

    def __init__(self, args):
        self._settings = GeneratorSettings(args)
        self._log = logging.getLogger(__name__)
        self._renderer = TemplateRenderer(self._settings, args)
        self._skeleton_interface_header = None
        self._base_types = set()
        self._impl_types = set()
        self._args = args
        self.generation_root = self._settings.output_dir
        self.includes_root_dir = os.path.join(self.generation_root, 'includes')
        self.machines_root_dir = os.path.join(self.generation_root, 'machines')
        self.processes_root_dir = os.path.join(self.generation_root, 'processes')
        self.software_packages_root_dir = os.path.join(self.generation_root, 'software-packages')
        self.software_root_dir = os.path.join(self.generation_root, 'software-clusters')
        self.vehicle_root_dir = os.path.join(self.generation_root, 'vehicles')
        self.net_bindings_root_dir = os.path.join(self.generation_root, 'net-bindings')

        self._someip_bindings = []
        for name in self._settings.someip_libs:
            module = importlib.import_module(Generator._LIB_BINDING['someip'][name]['module'])
            Binding = getattr(module, Generator._LIB_BINDING['someip'][name]['class'])
            binding = Binding(self._renderer, self.net_bindings_root_dir, self.machines_root_dir,self.processes_root_dir)
            self._someip_bindings.append(binding)

        self._userdef_bindings = []
        for name in self._settings.user_defined_libs:
            module = importlib.import_module(Generator._LIB_BINDING['user_defined'][name]['module'])
            Binding = getattr(module, Generator._LIB_BINDING['user_defined'][name]['class'])
            binding = Binding(self._renderer, self.net_bindings_root_dir, self.machines_root_dir,self.processes_root_dir)
            self._userdef_bindings.append(binding)

        self._dds_bindings = []
        for name in self._settings.dds_libs:
            module = importlib.import_module(Generator._LIB_BINDING['dds'][name]['module'])
            Binding = getattr(module, Generator._LIB_BINDING['dds'][name]['class'])
            binding = Binding(self._renderer, self.net_bindings_root_dir, self.machines_root_dir,self.processes_root_dir)
            self._dds_bindings.append(binding)

    def includes_dir_for(self, namespaces_list):
        lowered_ns_list = [x.lower() for x in namespaces_list]
        return os.path.join(self.includes_root_dir, *lowered_ns_list)

    def _split_services(self, services):
        someip_services = [service for service in services if type(service) is SomeIpServiceView]
        userdef_services = [service for service in services if type(service) is UserDefinedServiceView]
        dds_services = [service for service in services if type(service) is DdsServiceView]
        return {"someip": someip_services, "userdef": userdef_services, "dds": dds_services}

    def get_process_service_instances(self, process):
        mappings = process.get_si_mappings()
        provided_instances = [x["service"] for x in mappings["provided"]]
        required_instances = [x["service"] for x in mappings["required"]]
        all_instances = provided_instances + required_instances
        return {
            "provided": self._split_services(provided_instances),
            "required": self._split_services(required_instances),
            "all": self._split_services(all_instances),
        }

    def _generate_provided_service_bindings(self, bindings, services):
        for binding in bindings:
            for service in services:
                binding.generate_service_desc(service)
                binding.generate_si_adapter(service)

    def _generate_required_service_bindings(self, bindings, services):
        for binding in bindings:
            for service in services:
                binding.generate_service_desc(service)
                binding.generate_proxy(service)

    @staticmethod
    def _unique_service_id(service_instances_list):
        seen = {}
        result = [seen.setdefault(x.service_id, x) for x in service_instances_list if x.service_id not in seen]
        return result

    def _generate_network_binding_sm(self, process, component):
        service_instances = self.get_process_service_instances(process)
        srv = self._unique_service_id(service_instances["all"]["dds"])
        if srv:
            for binding in self._dds_bindings:
                binding.generate_sm_definition(component, srv)

    def _generate_network_binding_config(self, machine):
        provided_service_instances = self._split_services(machine.provided_services_list)
        required_service_instances = self._split_services(machine.required_services_list)
        all_service_instances = self._split_services(machine.services_list)
        if all_service_instances["someip"]:
            for binding in self._someip_bindings:
                binding.generate_machine_config(machine, provided_service_instances["someip"])
        # if all_service_instances["userdef"]:
            for binding in self._userdef_bindings:
                binding.generate_machine_config(machine, provided_service_instances["userdef"])
        if all_service_instances["dds"]:
            for binding in self._dds_bindings:
                binding.generate_machine_config(machine, provided_service_instances["dds"])

    def _generate_library_interfaces(self, service_instances):
        # no need for library-specific interfaces for someip and userdef yet
        for binding in self._dds_bindings:
            if len(service_instances["all"]["dds"]) > 0:
                binding.generate_idls(service_instances["all"]["dds"])
                binding.generate_type_info_headers(service_instances["all"]["dds"])
                binding.generate_type_converters(service_instances["all"]["dds"])

    def _generate_swc(self, components):
        error_domains = set()
        for component in components:
            self._log.info('Generating proxies, skeletons and type headers for swc "%s"', component.name)
            self._generate_interface(component)
            error_domains.update(component.error_domains)
            if ModelAdjust.is_has_raw(component.interfaces):
                error_domains.update(component.error_domains)
                self.write_runtime_cpp(sorted(error_domains), component)
            if len(component.services_list) > 0:
                error_domains.update(component.error_domains)
                self.write_runtime_cpp(sorted(error_domains), component)
        self._generate_error_domain_headers(error_domains)

    def _generate_interface(self, component):
        self.generate_type_headers(component)
        for service in component.services_list:
            self.generate_si_definition(service)
            if component.service_is_required(service):
                self.generate_proxy_h_cpp(service)
            if component.service_is_provided(service):
                self.generate_skeleton_h_cpp(service)
        showList = []
        for service in component.services_list:
            if service.is_show:
                showList.append(service)
        component.set_services_list(showList)
        for phm_se_interface in component.phm_se_interfaces:
            self.generate_phm_se_interface_h(phm_se_interface)
        for phm_hc_interface in component.phm_hc_interfaces:
            self.generate_phm_hc_interface_h(phm_hc_interface)
        self.generate_diag_h_app(component)
        self.generate_sg_h(component)

    def _generate_em_manifests(self, process):
        output_dir = os.path.join(self.processes_root_dir)
        output_filename = "{}_manifest.json".format(process.name)
        em_template = self._renderer.load_template("execution_manager_manifest.j2", trim_blocks=True)
        self._renderer.write_output(em_template, output_dir, output_filename, process=process)

    def _generate_diagnosis(self, diag):
        if diag is not None:
            if "dmd" in diag:
                output_filename = "dmd_dmd.json"
                template = json.dumps(diag["dmd"], cls=JsonEncoder, indent=4)
                self._renderer.write_output(template, self.processes_root_dir, output_filename)
            if "dmd_doip" in diag:
                output_filename = "dmd_doip.json"
                template = json.dumps(diag["dmd_doip"], cls=JsonEncoder, indent=4)
                self._renderer.write_output(template, self.processes_root_dir, output_filename)
            if "doipd_doip" in diag:
                output_filename = "doipd_doip.json"
                template = json.dumps(diag["doipd_doip"], cls=JsonEncoder, indent=4)
                self._renderer.write_output(template, self.processes_root_dir, output_filename)
            if "dmc" in diag:
                dmcList = diag["dmc"]
                for dmc in dmcList:
                    output_filename = dmc["dmc_name"]
                    template = json.dumps(dmc["dmc"], cls=JsonEncoder, indent=4)
                    self._renderer.write_output(template, self.processes_root_dir, output_filename)
            if "dapi" in diag:
                dapiList = diag["dapi"]
                for dapi in dapiList:
                    output_filename = dapi["dapi_name"]
                    template = json.dumps(dapi["dapi"], cls=JsonEncoder, indent=4)
                    self._renderer.write_output(template, self.processes_root_dir, output_filename)

    def _generate_si_manifest(self, process):
        output_dir = os.path.join(self.processes_root_dir)
        output_filename = "{}_service_instance_manifest.json".format(process.name)
        template = self._renderer.load_template("service/service_instance_manifest.j2", trim_blocks=True)
        mappings = process.get_si_mappings()
        self._renderer.write_output(template, output_dir, output_filename, mappings=mappings)

    def getbindinfo(self, someip_services_requirds):
        binds = []
        for someip_services_requird in someip_services_requirds:
            if "tcp" in someip_services_requird.ports and "unicastIp" in someip_services_requird.ports and "unicastTcp" in someip_services_requird.ports:
                itemTcp = {"type": "tcp",
                           "port": someip_services_requird.ports["tcp"],
                           "portEnd": someip_services_requird.ports["tcpEnd"],
                           "ipaddress": someip_services_requird.ports["unicastIp"],
                           "bindport": someip_services_requird.ports["unicastTcp"]}
                if itemTcp not in binds:
                    binds.append(itemTcp)
            if "udp" in someip_services_requird.ports and "unicastIp" in someip_services_requird.ports and "unicastUdp" in someip_services_requird.ports:
                itemUdp = {"type": "udp",
                           "port": someip_services_requird.ports["udp"],
                           "portEnd": someip_services_requird.ports["udpEnd"],
                           "ipaddress": someip_services_requird.ports["unicastIp"],
                           "bindport": someip_services_requird.ports["unicastUdp"]}
                if itemUdp not in binds:
                    binds.append(itemUdp)
        if len(binds) == 0:
            for someip_services_requird in someip_services_requirds:
                if someip_services_requird.machine is not None and "ipAddress" in someip_services_requird.service_deployment.bindPorts:
                    machineOb = someip_services_requird.machine
                    ipAddressList = [machineOb.ip_address]
                    for multiple_host in machineOb.multiple_host:
                        ipAddressList.append(multiple_host["host"])
                    ipAddress = someip_services_requird.service_deployment.bindPorts["ipAddress"]
                    if ipAddress in ipAddressList:
                        if "tcp" in someip_services_requird.ports and "tcp" in someip_services_requird.service_deployment.bindPorts:
                            item = {"type": "tcp",
                                    "port": someip_services_requird.ports["tcp"],
                                    "portEnd": someip_services_requird.ports["tcpEnd"],
                                    "ipaddress": ipAddress,
                                    "bindport": someip_services_requird.service_deployment.bindPorts["tcp"]}
                            if item not in binds:
                                binds.append(item)
                        if "udp" in someip_services_requird.ports and "udp" in someip_services_requird.service_deployment.bindPorts:
                            item1 = {"type": "udp",
                                     "port": someip_services_requird.ports["udp"],
                                     "portEnd": someip_services_requird.ports["udpEnd"],
                                     "ipaddress": ipAddress,
                                     "bindport": someip_services_requird.service_deployment.bindPorts["udp"]}
                            if item1 not in binds:
                                binds.append(item1)

        bindresulets = []
        for bindinfo in binds:
            self.add_bind_result(bindinfo, bindresulets)
        return bindresulets

    def add_bind_result(self, bindinfo, bindresulets):
        find = False
        for bindresulet in bindresulets:
            if bindinfo["type"] == bindresulet["type"] and bindinfo["ipaddress"] == bindresulet["ipaddress"] and bindinfo["bindport"] == bindresulet["bindport"]:
                find = True
                portItem = {"start": bindinfo["port"], "end": bindinfo["portEnd"]}
                if portItem not in bindresulets:
                    bindresulet["port"].append(portItem)
        if not find:
            port = [{"start": bindinfo["port"], "end": bindinfo["portEnd"]}]
            item = {"type": bindinfo["type"], "port": port,
                    "ipaddress": bindinfo["ipaddress"],
                    "bindport": bindinfo["bindport"]}
            bindresulets.append(item)

        return bindresulets

    def _generate_app_nsomeip_json(self, process):
        """Generate the nsomeip2 json file required by genivi nsomeip2"""
        mappings = process.get_si_mappings()
        services=[m['service'] for m in mappings['provided']]
        someip_services = [service for service in services if type(service) is SomeIpServiceView]
        services_required = [m['service'] for m in mappings['required']]
        someip_services_requirds = [service for service in services_required if type(service) is SomeIpServiceView]
        bindinfos = self.getbindinfo(someip_services_requirds)
        signalbindservices = []
        aragenhelperinfo = self.get_aragenhelperinfo(process=process)
        for someip_services_requird in someip_services_requirds:
            for signalservice in process.signalbindinfo.services:
                if someip_services_requird.name == signalservice.name and someip_services_requird.deployment_id == signalservice.service_deployment.deployment_id:
                    # signalbindservices.append(someip_services_requird)
                    someip_services_requird.set_forsignal(True)
        for name in self._settings.someip_libs:
            if name == "nsomeip2" and aragenhelperinfo.isnsomeip:
                nsomeip2_template = self._renderer.load_template("nsomeip2_binding/process_nsomeip_json.j2")
                output_dir = os.path.join(self.processes_root_dir)
                output_filename = "{}_nsomeip.json".format(process.name)
                self._renderer.write_output(
                    nsomeip2_template,
                    output_dir,
                    output_filename,
                    process=process,
                    machine=process.machine,
                    services=someip_services,
                    bindinfos=bindinfos,
                    signalbindinfo= process.signalbindinfo,
                    servicesrequirds = someip_services_requirds
                )

    def _generate_process_npc_json(self, process):
        """Generate the nsomeip2 json file required by genivi nsomeip2"""
        mappings = process.get_si_mappings()
        services = [m['service'] for m in mappings['provided']]
        someip_services = [service for service in services if type(service) is UserDefinedServiceView and service.user_defined_type == "npc"]

        aragenhelperinfo = self.get_aragenhelperinfo(process=process)
        for name in self._settings.user_defined_libs:
            if name == "npc" and aragenhelperinfo.isNPC:
                nsomeip2_template = self._renderer.load_template("npc_binding/process_npc_json.j2")
                output_dir = os.path.join(self.processes_root_dir)
                output_filename = "{}_npc.json".format(process.name)
                self._renderer.write_output(
                    nsomeip2_template,
                    output_dir,
                    output_filename,
                    process=process,
                    machine=process.machine,
                    services=someip_services,
                    servicemun=len(someip_services)
                )

    def _generate_process_icc_json(self, process):
        mappings = process.get_si_mappings()
        services = [m['service'] for m in mappings['provided']+mappings['required']]
        all_services = [service for service in services if type(service) is UserDefinedServiceView and service.user_defined_type == "icc"]
        if len(all_services) > 0:
            aragenhelperinfo = self.get_aragenhelperinfo(process=process)
            for name in self._settings.user_defined_libs:
                if name == "icc" and aragenhelperinfo.isIcc:
                    icc_template = self._renderer.load_template("icc_binding/process_icc_json.j2")
                    output_dir = os.path.join(self.processes_root_dir)
                    output_filename = "{}_icc.json".format(process.name)
                    self._renderer.write_output(
                        icc_template,
                        output_dir,
                        output_filename,
                        process=process,
                        services=all_services
                    )

    def _generate_phm_processjson(self,process):

        phmprocessjson_template = self._renderer.load_template("phm_process_json.j2")

        output_filename = "{}_phm.json".format(process.name)
        self._renderer.write_output(
            phmprocessjson_template,
            self.processes_root_dir,
            output_filename,
            process=process
        )

    def _generate_project_root_cmakelist(self, filelist):

        output_filename = "CMakeLists.txt"
        cmake_template = self._renderer.load_template("project/CMakeLists_txt.j2", trim_blocks=True)
        self._renderer.write_output(cmake_template, self.generation_root + "/", output_filename, filelist=filelist, version=aragen_args.__version__)

    def _generate_project_root_files_cmakelist(self, executable, swcl_info, component_views, isSmd=False):
        outPath = self.generation_root + "/files"
        if isSmd and not os.path.exists(outPath):
            outPath = self.generation_root + "/gen"

        aragenhelperinfo = self.get_aragenhelperinfo(component_views=component_views, executable=executable)
        output_filename = "aragen-helper.cmake"
        cmake_template = self._renderer.load_template("project/files/aragen-helper_cmake.j2", trim_blocks=True)
        self._renderer.write_output(cmake_template, outPath, output_filename, aragenhelperinfo=aragenhelperinfo, executable=executable, swcl_info=swcl_info, version=aragen_args.__version__)

    def get_aragenhelperinfo(self, component_views=None, executable=None, process=None):
        if component_views is None:
            component_views = []
        aragenhelperinfo = AragenHelperInfoView()

        for component in component_views:

            if ModelAdjust.is_has_tsync(component.interfaces):
                aragenhelperinfo.add_ARA_GEN_ARA_LIB("ara::tsync")
                aragenhelperinfo.findpakage.append("ara-tsync")

            if ModelAdjust.is_has_per(component.interfaces) or component.kvs_interfaces:
                aragenhelperinfo.add_ARA_GEN_ARA_LIB("ara::per")
                aragenhelperinfo.findpakage.append("ara-per")

            if ModelAdjust.is_has_crypto(component.interfaces):
                aragenhelperinfo.add_ARA_GEN_ARA_LIB("ara::ara_crypto")
                aragenhelperinfo.findpakage.append("isoft-crc")
                aragenhelperinfo.findpakage.append("ara-crypto")

            if ModelAdjust.is_has_phm(component.interfaces):
                aragenhelperinfo.add_ARA_GEN_ARA_LIB("ara::phm::phm_client")
                aragenhelperinfo.findpakage.append("ara-phm-client")

            if ModelAdjust.is_has_fw(component.interfaces):
                aragenhelperinfo.add_ARA_GEN_ARA_LIB("ara::ara_fw_client")
                aragenhelperinfo.findpakage.append("ara-firewall")

            if ModelAdjust.is_has_diag(component.interfaces):
                aragenhelperinfo.add_ARA_GEN_ARA_LIB("ara::diag")
                aragenhelperinfo.findpakage.append("ara-diag")
            if ModelAdjust.is_has_raw(component.interfaces):
                aragenhelperinfo.set_issaw(True)

            if ModelAdjust.is_has_cg(component):
                aragenhelperinfo.findpakage.append("ara-com-cg")
                aragenhelperinfo.set_cg(True)
        if process:
            mappings = process.get_si_mappings()
            provided_instances = [x["service"] for x in mappings["provided"]]
            required_instances = [x["service"] for x in mappings["required"]]
            all_instances = provided_instances + required_instances
            if len(all_instances):
                aragenhelperinfo.set_hascomservice(True)
            for service in all_instances:
                if type(service) is DdsServiceView:
                    for name in self._settings.dds_libs:
                        if name == "fastdds":
                            aragenhelperinfo.set_isfastdds(True)
                if type(service) is SomeIpServiceView:
                    for name in self._settings.someip_libs:
                        if name == "nsomeip2":
                            aragenhelperinfo.set_isnsomeip(True)
                if type(service) is UserDefinedServiceView:
                    for name in self._settings.user_defined_libs:
                        if name == "npc":
                            aragenhelperinfo.set_isNPC(True)
                        elif name == "icc":
                            aragenhelperinfo.set_isIcc(True)
        elif executable:
            mappings = executable.serviceall
            provided_instances = mappings["pServiceList"]
            required_instances = mappings["rServiceList"]
            all_instances = provided_instances + required_instances
            if len(all_instances):
                aragenhelperinfo.set_hascomservice(True)

            for service in all_instances:
                # if type(service) is SomeIpServiceView:
                if type(service.service_deployment) is DdsServiceDeployment:
                    for name in self._settings.dds_libs:
                        if name == "fastdds":
                            aragenhelperinfo.set_isfastdds(True)
                if type(service.service_deployment) is SomeIpServiceDeployment:
                    for name in self._settings.someip_libs:
                        if name == "nsomeip2":
                            aragenhelperinfo.set_isnsomeip(True)
                if type(service.service_deployment) is UserDefinedServiceDeployment:
                    depType = service.service_deployment.depType
                    if depType == "npc":
                        aragenhelperinfo.set_isNPC(True)
                    elif depType == "icc":
                        aragenhelperinfo.set_isIcc(True)

        return aragenhelperinfo

    def _generate_project_root_src_instance(self, instance, isSmd=False):
        outPath = self.generation_root + "/files"
        if isSmd and not os.path.exists(outPath):
            outPath = self.generation_root + "/gen"
        output_filename = "instance_specifier.txt"
        instance_template = self._renderer.load_template("project/files/instance_specifier.j2", trim_blocks=True)
        self._renderer.write_output(instance_template, outPath, output_filename, instance_spencifier=instance)

    def _generate_project_root_src_cmakelist(self):

        output_filename = "CMakeLists.txt"
        cmake_template = self._renderer.load_template("project/src/CMakeLists_txt.j2", trim_blocks=True)
        self._renderer.write_output(cmake_template, self.generation_root+"/src", output_filename, version=aragen_args.__version__)

    def _generate_project_root_src_main(self):

        output_filename = "main.cpp"
        cmake_template = self._renderer.load_template("project/src/main_cpp.j2", trim_blocks=True)
        self._renderer.write_output(cmake_template, self.generation_root+"/src", output_filename)

    def _generate_project_root_test_cmakelist(self):
        output_filename = "CMakeLists.txt"
        cmake_template = self._renderer.load_template("project/test/CMakeLists_txt.j2", trim_blocks=True)
        self._renderer.write_output(cmake_template, self.generation_root + "/test", output_filename, version=aragen_args.__version__)

    def _generate_project_root_gitignore(self):
        output_filename = ".gitignore"
        cmake_template = self._renderer.load_template("project/gitignore.j2", trim_blocks=True)
        self._renderer.write_output(cmake_template, self.generation_root, output_filename)

    def _generate_project_root_test_testcpp(self):
        output_filename = "test1.cpp"
        cmake_template = self._renderer.load_template("project/test/test1_cpp.j2", trim_blocks=True)
        self._renderer.write_output(cmake_template, self.generation_root + "/test", output_filename)

    def _generate_processes(self, processes):
        for process in processes:
            self._log.info('Generating manifests for process "%s"', process.name)
            self._generate_em_manifests(process)
            self._generate_storage_manifests(process)
            self._generate_per_manifests(process)
            self._generate_time_base_mapping_manifests(process)
            self._generate_si_manifest(process)
            self._generate_app_nsomeip_json(process)
            self._generate_process_npc_json(process)
            self._generate_process_icc_json(process)
            self._generate_phm_processjson(process)
            self._generate_crypto_process(process)
            self._generate_e2e(process)
            self._generate_log_trace(process)
            self._generate_raw(process)
            self._generate_cg_manifest(process)
            self._generate_state_management_manifest(process)
            self._generate_cm(process)
            self._generate_qos(process)


    def _generate_software(self, softwarePackages):
        for softwarePackage in softwarePackages:
            if softwarePackage.software_cluster_info is not None:
                self._generate_software_cluster(softwarePackage.software_cluster_info, self.software_packages_root_dir)
            if softwarePackage.software_package_info is not None:
                self._generate_software_package(softwarePackage.software_package_info)

    def _generate_software_cluster_list(self, softwareClusters):
        for softwareCluster in softwareClusters:
            if softwareCluster is not None:
                self._generate_software_cluster(softwareCluster, self.software_root_dir)

    def _generate_vehicle(self, vehiclePackages):
        for vehiclePackage in vehiclePackages:
            self._generate_vehicle_package_info(vehiclePackage.vehicle_package_info)
            self._generate_vehicle_software_cluster_infos(vehiclePackage.vehicle_package_info)
            self._generate_vehicle_software_package_infos(vehiclePackage.vehicle_package_info)

    def _generate_vehicle_package_info(self, vehicle_package):
        output_filename = "ucm_vps.json"
        em_template = self._renderer.load_template("ucm_vps.j2", trim_blocks=True)
        self._renderer.write_output(em_template, os.path.join(self.vehicle_root_dir, vehicle_package["shortName"]), output_filename, vehicle_package=vehicle_package)

    def _generate_vehicle_software_cluster_infos(self, vehicle_package):
        output_filename = "ucm_swcls.json"
        em_template = self._renderer.load_template("ucm_swcls.j2", trim_blocks=True)
        self._renderer.write_output(em_template, os.path.join(self.vehicle_root_dir, vehicle_package["shortName"]), output_filename, vehicle_package=vehicle_package)

    def _generate_vehicle_software_package_infos(self, vehicle_package):
        output_filename = "ucm_swps.json"
        em_template = self._renderer.load_template("ucm_swps.j2", trim_blocks=True)
        self._renderer.write_output(em_template, os.path.join(self.vehicle_root_dir, vehicle_package["shortName"]), output_filename, vehicle_package=vehicle_package)

    def _generate_software_package(self, software_package):
        output_filename = "{}_swpkg_manifest.json".format(software_package["shortName"])
        em_template = self._renderer.load_template("swpkg_manifest.j2", trim_blocks=True)
        self._renderer.write_output(em_template, self.software_packages_root_dir, output_filename, softwarePackage=software_package)

    def _generate_software_cluster(self, software_cluster, output_dir):
        shortName = software_cluster["shortName"]
        if software_cluster["packageShortName"] is not None:
            shortName = software_cluster["packageShortName"]
        output_filename = "{}_swcl_manifest.json".format(shortName)
        em_template = self._renderer.load_template("swcl_manifest.j2", trim_blocks=True)
        self._renderer.write_output(em_template, output_dir, output_filename, softwareCluster=software_cluster)

        if "functionGroupSets" in software_cluster:
            output_filename1 = "{}_function_groups.json".format(shortName)
            template1 = json.dumps(software_cluster["functionGroupSets"], cls=JsonEncoder, indent=4)
            self._renderer.write_output(template1, output_dir, output_filename1)

        # output_filename2 = "{}_modedeclarationgroups.json".format(software_cluster["shortName"])
        # template2 = json.dumps(software_cluster["modeDeclarationGroups"], cls=JsonEncoder, indent=4)
        # self._renderer.write_output(template2, self.software_packages_root_dir, output_filename2)

    def _generate_machine_manifest(self, machine):
        output_filename = "{}_machine_manifest.json".format(machine.name)
        em_template = self._renderer.load_template("machine_manifest.j2", trim_blocks=True)
        self._renderer.write_output(em_template, self.machines_root_dir, output_filename, configuration=machine.configuration)

    def _generate_storage_manifests(self, process):
        output_dir = os.path.join(self.processes_root_dir)
        no_mapping = []
        element_mapping = []
        for item in process.persistency_mappings["kvs"]:
            no_mapping.extend(item["no_crypto"])
            element_mapping.extend(item["element_crypto"])
        for item in process.persistency_mappings["file"]:
            no_mapping.extend(item["no_crypto"])
            element_mapping.extend(item["element_crypto"])

        output_filename = "{}_storage_manifest.json".format(process.name)
        em_template = self._renderer.load_template("storage_manifest.j2", trim_blocks=True)
        self._renderer.write_output(em_template, output_dir, output_filename, process=process, no_mapping=no_mapping, element_mapping=element_mapping)

    def _generate_per_manifests(self, process):
        for kvs in process.persistency_mappings["kvs"]:
            output_dir = os.path.join(self.processes_root_dir)
            output_filename = "{}_{}_init.per".format(process.name, kvs["name"])
            em_template = self._renderer.load_template("ap_per.j2", trim_blocks=True)
            self._renderer.write_output(em_template, output_dir, output_filename, kvs=kvs)

    def _generate_time_base_mapping_manifests(self, process):
        output_filename = "{}_time_base_map.json".format(process.name)
        em_template = self._renderer.load_template("time_base_map.j2", trim_blocks=True)
        self._renderer.write_output(em_template, self.processes_root_dir, output_filename,process=process)

    def _generate_crypto_process(self, process):
        output_filename = "{}_crypto_manifest.json".format(process.name)
        template = json.dumps(process.crypto, cls=JsonEncoder, indent=4)
        self._renderer.write_output(template, self.processes_root_dir, output_filename, process=process)

    def _generate_e2e(self, process):
        if process is not None and process.e2e is not None:
            e2e = process.e2e
            if "propMap" in e2e:
                output_filename = "{}_e2e_statemachines.json".format(process.name)
                template = json.dumps(e2e["propMap"], cls=JsonEncoder, indent=4)
                self._renderer.write_output(template, self.processes_root_dir, output_filename)
            if "dataIdMap" in e2e:
                output_filename = "{}_e2e_dataid_mapping.json".format(process.name)
                template = json.dumps(e2e["dataIdMap"], cls=JsonEncoder, indent=4)
                self._renderer.write_output(template, self.processes_root_dir, output_filename)

    def _generate_log_trace(self, process):
        if process is not None and process.logTrace is not None:
            output_filename = "{}_log.json".format(process.name)
            template = json.dumps(process.logTrace, cls=JsonEncoder, indent=4)
            self._renderer.write_output(template, self.processes_root_dir, output_filename)

    def _generate_raw(self,process):
        if process is not None and process.rawmappings["ethernet_raw_data_stream_mapping"]:
            output_filename = "{}_raw.json".format(process.name)
            template = json.dumps(process.rawmappings, cls=JsonEncoder, indent=4)
            self._renderer.write_output(template, self.processes_root_dir, output_filename)

    def _generate_cg_manifest(self, process):
        if process.cg:
            output_filename = "{}_cg_manifest.json".format(process.name)
            template = json.dumps(process.cg, cls=JsonEncoder, indent=4)
            self._renderer.write_output(template, self.processes_root_dir, output_filename)

    def _generate_cm(self, process):
        if process.signalbindinfo:
            if process.signalbindinfo.secOC:
                output_filename = "{}_secoc.json".format(process.name)
                template = json.dumps(process.signalbindinfo.secOC, cls=JsonEncoder, indent=4)
                self._renderer.write_output(template, self.processes_root_dir, output_filename)
            if process.signalbindinfo.secOC_binding:
                output_filename = "{}_secoc_binding.json".format(process.name)
                template = json.dumps(process.signalbindinfo.secOC_binding, cls=JsonEncoder, indent=4)
                self._renderer.write_output(template, self.processes_root_dir, output_filename)

    def _generate_qos(self, process):
        if process.qos:
            qos = process.qos
            if "qosJson" in qos:
                output_filename = "{}_qos.json".format(process.name)
                template = json.dumps(qos["qosJson"], cls=JsonEncoder, indent=4)
                self._renderer.write_output(template, self.processes_root_dir, output_filename)
            if "qosXml" in qos:
                output_filename = "{}_rtps.xml".format(process.name)
                self._renderer.write_output(qos["qosXml"], self.processes_root_dir, output_filename)

    def _generate_state_management_manifest(self, process):
        if process.state_manager:
            output_filename = "{}_trigger_fg_service_instance_manifest.json".format(process.name)
            template = json.dumps(process.state_manager, cls=JsonEncoder, indent=4)
            self._renderer.write_output(template, self.processes_root_dir, output_filename)
        if process.state_machine:
            output_filename = "{}_state_machine.json".format(process.name)
            template = json.dumps(process.state_machine, cls=JsonEncoder, indent=4)
            self._renderer.write_output(template, self.processes_root_dir, output_filename)

    def _generate_ide(self, machine):
        if machine.ide_info is not None:
            output_filename = "{}_ide_machine_info.json".format(machine.name)
            template = json.dumps(machine.ide_info, cls=JsonEncoder, indent=4)
            self._renderer.write_output(template, self.machines_root_dir, output_filename)

    def _generate_time_sync_manifests(self, machine):
        output_filename = "{}_time_sync.json".format(machine.name)
        em_template = self._renderer.load_template("time_sync.j2", trim_blocks=True)
        self._renderer.write_output(em_template, self.machines_root_dir, output_filename,configuration=machine.configuration)

    def _generate_dlt_config(self, machine):
        output_filename = "{}_dlt.conf".format(machine.name)
        log_config_template = self._renderer.load_template("dlt_conf.j2", trim_blocks=True)
        self._renderer.write_output(log_config_template, self.machines_root_dir, output_filename, machine=machine)

    def _generate_network_config(self, machine):
        output_filename = "{}_etc_network_interfaces".format(machine.name)
        template = self._renderer.load_template("etc_network_interfaces.j2", trim_blocks=True)
        self._renderer.write_output(template, self.machines_root_dir, output_filename, host=machine)

    def _generate_nm(self, machine):
        output_filename = "{}_nm.json".format(machine.name)

        template = json.dumps(machine.nm_contribution["nmBean"], cls=JsonEncoder, indent=4)
        self._renderer.write_output(template, self.machines_root_dir, output_filename)

        output_filename1 = "{}_nm_mapping.json".format(machine.name)
        template1 = json.dumps(machine.nm_contribution["nmMapping"], cls=JsonEncoder, indent=4)
        self._renderer.write_output(template1, self.machines_root_dir, output_filename1)

    def _generate_phm_contribute(self, machine):
        if machine.phm_contribution is not None:
            output_filename = "{}_phm_contribute.json".format(machine.name)
            template = json.dumps(machine.phm_contribution, cls=JsonEncoder, indent=4)
            self._renderer.write_output(template, self.machines_root_dir, output_filename)

    def _generate_log(self, machine):
        if machine.emd_log is not None:
            output_filename = "{}_log-emd.json".format(machine.name)
            template = json.dumps(machine.emd_log, cls=JsonEncoder, indent=4)
            self._renderer.write_output(template, self.machines_root_dir, output_filename)
        if machine.log_global_config is not None:
            output_filename = "{}_log_global_config.json".format(machine.name)
            template = json.dumps(machine.log_global_config, cls=JsonEncoder, indent=4)
            self._renderer.write_output(template, self.machines_root_dir, output_filename)

    def _generate_idsm(self, machine):
        if machine.idsm is not None:
            output_filename = "{}_idsm_contribute.json".format(machine.name)
            template = json.dumps(machine.idsm, cls=JsonEncoder, indent=4)
            self._renderer.write_output(template, self.machines_root_dir, output_filename)

    def _generate_iam_someip(self, machine):
        if machine.iam_com is not None:
            output_filename = "{}_iam_com.json".format(machine.name)
            template = json.dumps(machine.iam_com, cls=JsonEncoder, indent=4)
            self._renderer.write_output(template, self.machines_root_dir, output_filename)

    def _generate_iam_raw(self, machine):
        if machine.iam_raw is not None:
            output_filename = "{}_iam_raw.json".format(machine.name)
            template = json.dumps(machine.iam_raw, cls=JsonEncoder, indent=4)
            self._renderer.write_output(template, self.machines_root_dir, output_filename)

    def _generate_iam_idsm(self, machine):
        if machine.iam_idsm is not None:
            output_filename = "{}_iam_idsm.json".format(machine.name)
            template = json.dumps(machine.iam_idsm, cls=JsonEncoder, indent=4)
            self._renderer.write_output(template, self.machines_root_dir, output_filename)

    def _generate_crypto(self, machine):
        output_filename = "{}_crypto_manifest.json".format(machine.name)
        template = json.dumps(machine.crypto_contribution, cls=JsonEncoder, indent=4)
        self._renderer.write_output(template, self.machines_root_dir, output_filename)

    def _generate_iam_phm(self, machine):
        output_filename = "{}_iam_phm.json".format(machine.name)
        template = json.dumps(machine.iam_phm, cls=JsonEncoder, indent=4)
        self._renderer.write_output(template, self.machines_root_dir, output_filename)

    def _generate_iam_crypto(self, machine):
        output_filename = "{}_iam_crypto.json".format(machine.name)
        template = json.dumps(machine.iam_crypto, cls=JsonEncoder, indent=4)
        self._renderer.write_output(template, self.machines_root_dir, output_filename)

    def _generate_build_info(self, buildInfo):
        if "exe" in buildInfo:
            self._generate_exe(buildInfo["exe"])
        if "components" in buildInfo:
            self._generate_swc(buildInfo["components"])
        if "fwInterfaceList" in buildInfo:
            for fw_interface in buildInfo["fwInterfaceList"]:
                self.generate_fw_interface_h(fw_interface)

    def _generate_exe(self, exe):
        all_required_service_instances = []
        all_provided_service_instances = []
        # if "exeProcessList" in exe:
        #     exeProcessList = exe["exeProcessList"]
        #     for exeP in exeProcessList:
        #         provided_instances = [ViewFactory.create_service_view(mapping['service']) for mapping in exeP['provided']]
        #         required_instances = [ViewFactory.create_service_view(mapping['service']) for mapping in exeP['required']]
        #         all_instances = provided_instances + required_instances
        #         service_instances = {
        #             "provided": self._split_services(provided_instances),
        #             "required": self._split_services(required_instances),
        #             "all": self._split_services(all_instances),
        #         }
        #         self._generate_library_interfaces(service_instances)
        #         all_required_service_instances.append(service_instances["required"])
        #         all_provided_service_instances.append(service_instances["provided"])
        if "serviceAll" in exe:
            serviceAll = exe["serviceAll"]
            provided_instances = [ViewFactory.create_service_view(item) for item in serviceAll['pServiceList']]
            required_instances = [ViewFactory.create_service_view(item) for item in serviceAll['rServiceList']]
            all_instances = provided_instances + required_instances
            service_instances = {
                "provided": self._split_services(provided_instances),
                "required": self._split_services(required_instances),
                "all": self._split_services(all_instances),
            }
            self._generate_library_interfaces(service_instances)
            all_required_service_instances.append(service_instances["required"])
            all_provided_service_instances.append(service_instances["provided"])

        for required_service_instances in all_required_service_instances:
            self._generate_required_service_bindings(self._someip_bindings, required_service_instances["someip"])
            self._generate_required_service_bindings(self._userdef_bindings, required_service_instances["userdef"])
            self._generate_required_service_bindings(self._dds_bindings, required_service_instances["dds"])

        for provided_service_instances in all_provided_service_instances:
            self._generate_provided_service_bindings(self._someip_bindings, provided_service_instances["someip"])
            self._generate_provided_service_bindings(self._userdef_bindings, provided_service_instances["userdef"])
            self._generate_provided_service_bindings(self._dds_bindings, provided_service_instances["dds"])

        if "dltMessage" in exe:
            dltMessage = exe["dltMessage"]
            if len(dltMessage["dltMessageList"]) > 0:
                output_filename = "define.h"
                element_template = self._renderer.load_template("dlt_message_define_h.j2", trim_blocks=True)
                self.write_standard_header(element_template, ["ara", "log", "internal", "dltmessage"], output_filename,
                                           dltMessageList=dltMessage["dltMessageList"])
            if len(dltMessage["dltMessageName"]) > 0:
                for dltItem in dltMessage["dltMessageName"]:
                    output_filename = "messages.h"
                    element_template = self._renderer.load_template("dlt_message_h.j2", trim_blocks=True)
                    self.write_standard_header(element_template, dltItem["namespaceList"], output_filename, dlt=dltItem)

    def _generate_machines(self, machines):
        for machine in machines:
            self._log.info('Generating Configuration for machine "%s"', machine.name)
            self._generate_network_binding_config(machine)
            self._generate_machine_manifest(machine)
            self._generate_time_sync_manifests(machine)
            self._generate_network_config(machine)
            self._generate_dlt_config(machine)
            self._generate_nm(machine)
            self._generate_phm_contribute(machine)
            self._generate_iam_someip(machine)
            self._generate_iam_idsm(machine)
            self._generate_iam_raw(machine)
            self._generate_crypto(machine)
            self._generate_iam_phm(machine)
            self._generate_iam_crypto(machine)
            self._generate_idsm(machine)
            self._generate_log(machine)
            self._generate_ide(machine)

    def _generate_project(self, model, filelist):

        executable = model["executable"]
        if executable is not None:
            component_views = executable.component_views
            swcl_info = executable.swcl_info
            exeName = executable.name
            self._generate_project_root_src_instance(model["instance"], exeName=="smd")
            self._generate_project_root_files_cmakelist(executable, swcl_info, component_views, exeName=="smd")
            if 'PROJECT' in self._settings.generate:
                self._generate_project_root_cmakelist(filelist)
                self._generate_project_root_src_cmakelist()
                self._generate_project_root_src_main()
                self._generate_project_root_test_cmakelist()
                self._generate_project_root_gitignore()
                self._generate_project_root_test_testcpp()

    def generate(self, model,filelist):
        """
        Generates proxy/skeleton files from the intermediate model,
        the generator expects the model to be a list of MachineViews.
        """

        self._log.debug('generation started for intermediate model "%s"', model)
        if 'PROJECT' not in self._settings.generate and 'PROJECT_UPDATE' not in self._settings.generate:
            if "processes" in model and model["processes"]:
                self._generate_processes(model["processes"])

            if "softwarePackages" in model and model["softwarePackages"]:
                self._generate_software(model["softwarePackages"])

            if "softwareClusters" in model and model["softwareClusters"]:
                self._generate_software_cluster_list(model["softwareClusters"])

            if "diag" in model and model["diag"]:
                self._generate_diagnosis(model["diag"])

            if "vehiclePackages" in model and model["vehiclePackages"]:
                self._generate_vehicle(model["vehiclePackages"])

            if "machines" in model and model["machines"]:
                self._generate_machines(model["machines"])

            if "build_info" in model and model["build_info"]:
                self._generate_build_info(model["build_info"])

        elif 'PROJECT' in self._settings.generate or 'PROJECT_UPDATE' in self._settings.generate:
            self._generate_project(model, filelist)

    def _generate_error_domain_headers(self, error_domains):
        if error_domains:
            error_domain_template = self._renderer.load_template("service/error_domain_xxx_h.j2")
            for error_domain in error_domains:
                output_filename = "error_domain_{0}.h".format(error_domain.standard_name)
                self.write_standard_header(error_domain_template, error_domain.namespace, output_filename,
                                           error_domain=error_domain)

    def write_standard_header(self, template, namespaces, filename, **kwargs):
        output_dir = self.includes_dir_for(namespaces)
        self._renderer.write_output(
            template,
            output_dir,
            filename,
            header_base=self._renderer.include_guard(namespaces, filename),
            **kwargs
        )

    @handle_method_exceptions(error_string_template="type headers generation ERROR: {exception_text}")
    def generate_type_headers(self, component):
        """Generate header files for the whole types tree"""
        template_map = {
            'structure':         "type/struct_impl_type_h.j2",
            'enum':              "type/enum_impl_type_h.j2",
            'map':               "type/map_impl_type_h.j2",
            'vector':            "type/vector_impl_type_h.j2",
            'variant':           "type/variant_impl_type_h.j2",
            'array':             "type/array_impl_type_h.j2",
            'aliased_impltype':  "type/aliased_impl_type_h.j2",
            'aliased_basetype':  "type/aliased_base_type_h.j2",
            'string':            "type/string_impl_type_h.j2",
            'fixed_string':      "type/string_fixed_impl_type_h.j2",
            'basetype':          "type/base_type_h.j2",
        }

        type_views = DataTypeContainer(component.services_list + component.kvs_interfaces).types

        for tv in list(type_views):
            kind = ViewManipulations.get_view_kind(tv)
            if len(tv.referred_views) == 1 and kind == "aliased_impltype":
                if tv.referred_views[0].__class__.__name__ == 'StringImplTypeView':
                    kind = 'string'
                elif tv.referred_views[0].__class__.__name__ == 'FixedStringImplTypeView':
                    kind = 'fixed_string'

            assert kind in template_map, "unknown view type {0}({1})".format(kind, tv)
            template = self._renderer.load_template(template_map[kind], trim_blocks=True)
            typename = tv.type_name.lower()
            if kind == "basetype":
                output_filename = "base_type_{0}.h".format(typename)
            else:
                output_filename = "impl_type_{0}.h".format(typename)
            self.write_standard_header(template, tv.namespace, output_filename, type_view=tv)

    def generate_si_definition(self, service_interface: ServiceView):
        """
        Generate the <name>.h file for the service interface
        """
        si_def_template_h = self._renderer.load_template("service/service_interface_definition_h.j2", True)
        output_filename = service_interface.standard_name + "_common.h"
        self.write_standard_header(si_def_template_h, service_interface.namespaces,
                                   output_filename,
                                   service=service_interface)
        si_def_template_cpp = self._renderer.load_template("service/service_interface_definition_cpp.j2", True)
        output_filename = service_interface.standard_name + "_common.cpp"
        self._renderer.write_output(si_def_template_cpp,
                                    os.path.join(self.net_bindings_root_dir, service_interface.namespacepath),
                                    output_filename, service=service_interface)

    def generate_proxy_h_cpp(self, service: ServiceView):
        """Generate the proxy h file required by application"""
        proxy_template = self._renderer.load_template("service/proxy_service_h.j2", True)
        output_filename = service.standard_name + "_proxy.h"
        self._log.debug("Writing content to file %s", output_filename)
        self.write_standard_header(proxy_template, service.namespaces,
                                   output_filename,
                                   service=service,
                                   soaElementCppDefStyle=self._args.soa_element_cpp_def_style)

    def generate_skeleton_h_cpp(self, service: ServiceView, machine=None):
        """Generate the skeleton h file required by application"""
        skeleton_template = self._renderer.load_template("service/skeleton_service_h.j2", True)
        output_filename = service.standard_name + "_skeleton.h"
        self._log.debug("Writing content of %s to file", output_filename)
        self.write_standard_header(skeleton_template, service.namespaces,
                                   output_filename,
                                   machine=machine,
                                   service=service,
                                   soaElementCppDefStyle=self._args.soa_element_cpp_def_style)

    def issameservice(self, service1, service2):
        includename1 = ""
        for name in service1.namespaces:
            includename1 = includename1 + name
        includename1 = includename1 + service1.name
        includename2 = ""
        for name in service1.namespaces:
            includename2 = includename2 + name
        includename2 = includename2 + service2.name
        if includename1 == includename2:
            return True
        else:
            return False

    def split_PR_service(self,component: ComponentView,p_services,r_services,pr_services):

        for pservice in component.provided_services:
            isrservice = False
            for rservice in component.required_services:
                if self.issameservice(rservice ,pservice):
                    isrservice = True
                    break
            if isrservice:
                pr_services.append(pservice)
            else:
                p_services.append(pservice)

        for rservice in component.required_services:
            isprservice = False
            for prservice in pr_services:
                if self.issameservice(prservice,rservice):
                    isprservice = True
                    break
            if not isprservice:
                r_services.append(rservice)

        for pservice in component.provided_services:
            for p_service in p_services:
                if self.issameservice(pservice,p_service):
                    p_service.set_bind_type_new(pservice.bind_type)

            for pr_service in pr_services:
                if self.issameservice(pservice, pr_service):
                    pr_service.set_bind_type_new(pservice.bind_type)

        for rservice in component.required_services:
            for r_service in r_services:
                if self.issameservice(rservice,r_service):
                    r_service.set_bind_type_new(rservice.bind_type)

            for pr_service in pr_services:
                if self.issameservice(rservice, pr_service):
                    pr_service.set_bind_type_new(rservice.bind_type)

    def is_servicein_serviclest(self,servicelist,service):
        for item in servicelist:
            if self.issameservice(item,service):
                item.add_service_deploymentinfo(service.bind_type,service.service_deployment.name)
                return True
        return False

    def get_sameservie_fordep(self,servicelist):
        result = []
        for service in servicelist:
            if not self.is_servicein_serviclest(result,service):
                service.add_service_deploymentinfo(service.bind_type, service.service_deployment.name)
                result.append(service)
        return result

    def write_runtime_cpp(self, error_domains, component):
        p_services = []
        r_services = []
        pr_services = []

        cg_signs = []

        self.split_PR_service(component, p_services, r_services, pr_services)

        # runtime 特殊处理，将deployment放置列表中
        p_services = self.get_sameservie_fordep(p_services)
        r_services = self.get_sameservie_fordep(r_services)
        pr_services = self.get_sameservie_fordep(pr_services)

        for service in p_services:
            if service.cg_sign:
                for rService in r_services:
                    if service.cg_sign == rService.cg_sign:
                        cg_signs.append(service.cg_sign)
                        break

        cm_def_template_cpp = self._renderer.load_template("nsomeip2_binding/runtime_cpp.j2", True)
        file_output_dir = os.path.join(self.net_bindings_root_dir, component.standard_name)

        self._renderer.write_output(cm_def_template_cpp, file_output_dir,
                                    "runtime.cpp",
                                    services=component.services_list,
                                    error_domains=error_domains
                                    )

        for prservice in pr_services:
            if prservice.is_show:
                cm_def_template_cpp = self._renderer.load_template("nsomeip2_binding/service_pr_runtime_cpp.j2", True)
                file_output_dir = os.path.join(self.net_bindings_root_dir, component.standard_name)
                self._renderer.write_output(cm_def_template_cpp, file_output_dir,
                                            prservice.name + "_runtime.cpp",
                                            service=prservice,
                                            error_domains=error_domains)
        for pservice in p_services:
            if pservice.is_show:
                isHaveCg = pservice.cg_sign.lower() if pservice.cg_sign in cg_signs else None
                cm_def_template_cpp = self._renderer.load_template(
                    "nsomeip2_binding/service_provider_runtime_cpp.j2", True)
                file_output_dir = os.path.join(self.net_bindings_root_dir, component.standard_name)
                self._renderer.write_output(cm_def_template_cpp, file_output_dir,
                                            pservice.name + "_runtime.cpp",
                                            service=pservice,
                                            isHaveCg=isHaveCg,
                                            error_domains=error_domains)

        for rservice in r_services:
            if rservice.is_show:
                isHaveCg = rservice.cg_sign.lower() if rservice.cg_sign in cg_signs else None
                cm_def_template_cpp = self._renderer.load_template(
                    "nsomeip2_binding/service_required_runtime_cpp.j2", True)
                file_output_dir = os.path.join(self.net_bindings_root_dir, component.standard_name)
                self._renderer.write_output(cm_def_template_cpp, file_output_dir,
                                            rservice.name + "_runtime.cpp",
                                            service=rservice,
                                            isHaveCg=isHaveCg,
                                            error_domains=error_domains)

    def generate_phm_se_interface_h(self,phm_se_interface):
        output_filename = "{}.h".format(phm_se_interface.name)
        phm_se_template = self._renderer.load_template("phm_se_interface_h.j2", trim_blocks=True)
        self.write_standard_header(phm_se_template, phm_se_interface.namespaces, output_filename,
                                    seinterface=phm_se_interface)

    def generate_phm_hc_interface_h(self,phm_hc_interface):
        output_filename = "{}.h".format(phm_hc_interface.name)
        phm_hc_template = self._renderer.load_template("phm_hc_interface_h.j2", trim_blocks=True)
        self.write_standard_header(phm_hc_template, phm_hc_interface.namespaces, output_filename,
                                    hcinterface=phm_hc_interface)

    def generate_fw_interface_h(self,fw_interface):
        output_filename = "{}_fw_states.h".format(fw_interface["name"])
        fw_template = self._renderer.load_template("fw_states_h.j2", trim_blocks=True)
        self.write_standard_header(fw_template, ["ara","fw","states"], output_filename, fw=fw_interface)

    def generate_sg_h(self, component):
        cgList = []
        for service in component.required_services:
            addList = []
            if service.cg_sign is not None and "Client" in service.name:
                for pservice in component.provided_services:
                    if pservice.cg_sign is not None and service.cg_sign == pservice.cg_sign:
                        addList.append(service)
                        addList.append(pservice)
                        if addList not in cgList:
                            cgList.append(addList)
                        break
        if len(cgList) > 0:
            for item in cgList:
                output_filename = "cg_type_info_{}.h".format(item[0].cg_sign.lower())
                element_template = self._renderer.load_template("cg_type_info_h.j2", trim_blocks=True)
                self.write_standard_header(element_template, ["ara", "com", "internal", "cg"], output_filename, rService=item[0], pService=item[1])

    def generate_diag_h_app(self, component):
        for element_item in component.diag_element:
            output_filename = "{}.h".format(element_item["lowerName"])
            element_template = self._renderer.load_template("diag/xxx_element_h.j2", trim_blocks=True)
            self.write_standard_header(element_template, ["ara", "diag"], output_filename,
                                       item=element_item)

            output_filename_app = "{}.cpp".format(element_item["lowerName"])
            element_template = self._renderer.load_template("diag/xxx_element_cpp.j2", trim_blocks=True)
            self._renderer.write_output(element_template, self.net_bindings_dir_for(element_item["namespaces"]),
                                        output_filename_app, item=element_item)

        for identifier_item in component.diag_identifier:
            output_filename = "{}.h".format(identifier_item["lowerName"])
            element_template = self._renderer.load_template("diag/xxx_identifier_h.j2", trim_blocks=True)
            self.write_standard_header(element_template, ["ara", "diag"], output_filename,
                                       item=identifier_item)

            output_filename_app = "{}.cpp".format(identifier_item["lowerName"])
            element_template_app = self._renderer.load_template("diag/xxx_identifier_cpp.j2", trim_blocks=True)
            self._renderer.write_output(element_template_app, self.net_bindings_dir_for(identifier_item["namespaces"]),
                                        output_filename_app, item=identifier_item)

        for routine_item in component.diag_routine:
            output_filename = "{}.h".format(routine_item["lowerName"])
            element_template = self._renderer.load_template("diag/xxx_routine_h.j2", trim_blocks=True)
            self.write_standard_header(element_template, ["ara", "diag"], output_filename,
                                       item=routine_item)

            output_filename_app = "{}.cpp".format(routine_item["lowerName"])
            element_template = self._renderer.load_template("diag/xxx_routine_cpp.j2", trim_blocks=True)
            self._renderer.write_output(element_template, self.net_bindings_dir_for(routine_item["namespaces"]),
                                        output_filename_app, item=routine_item)

    def net_bindings_dir_for(self, namespaces_list):
        lowered_ns_list = [x.lower() for x in namespaces_list]
        return os.path.join(self.net_bindings_root_dir, *lowered_ns_list)
