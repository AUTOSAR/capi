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

import os
import logging
from generator.views.service_view import ServiceView
from generator.views.component_view import ComponentView
from generator.parser.exception_handling import handle_method_exceptions
from generator.generator.template_renderer import TemplateRenderer


class NpcBinding:
    def __init__(self, renderer, output_dir, machine_dir,processes_dir) -> None:
        self._renderer = renderer
        self._output_dir = output_dir
        self._log = logging.getLogger(__name__)
        self.npc_root_dir = output_dir#os.path.join(output_dir, 'nsomeip')
        self.machine_config_root_dir = machine_dir
        self.processes_root_dir = processes_dir

    def npc_service_dir(self, service: ServiceView):
        if service.lowNamespaces is not None:
            repath = os.path.join(self.npc_root_dir ,*service.lowNamespaces)
            return os.path.join(repath,"npc")
        else:
            return self.npc_root_dir

    def write_guarded_file(self, template, service_view, filename, **kwargs):
        output_dir = self.npc_service_dir(service_view)
        self._renderer.write_output(
            template,
            output_dir,
            filename,
            header_base=self._renderer.include_guard(service_view.namespaces+["npc"], filename),
            **kwargs
        )

    def load_template(self, template_name):
        return self._renderer.load_template(template_name, trim_blocks=True)

    @handle_method_exceptions(error_string_template="NPC service description generation ERROR: {exception_text}")
    def generate_service_desc(self, service_interface):
        if service_interface.user_defined_type == "npc":
            """Generate the cpp and h files for service description"""
            si_desc_template_h = self.load_template("npc_binding/service_desc_xxx_h.j2")

            output_filename = "{}_service_desc.h".format(service_interface.service_deployment.standard_name)
            self._log.debug("Writing %s content to file", output_filename)
            self.write_guarded_file(si_desc_template_h, service_interface, output_filename, service=service_interface)

    @handle_method_exceptions(error_string_template="NPC JSON config generation ERROR: {exception_text}")
    def generate_npc_json(self, machine, services):
        """Generate the npc json file required by genivi npc"""
        npc_template = self.load_template("npc_binding/app_npc_json.j2")

        output_filename = "{}_npc.json".format(machine.name)
        self._renderer.write_output(
            npc_template,
            self.machine_config_root_dir,
            output_filename,
            machine=machine,
            services=services
        )

    def generate_proxy(self, service: ServiceView):
        if service.user_defined_type == "npc":
            """Generate the proxy h file required by application"""
            proxy_npc_template = self.load_template("npc_binding/proxy_npc_xxx_h.j2")

            output_filename = "{}_proxy_impl.h".format(service.service_deployment.standard_name)
            self._log.debug("Writing content to file %s", output_filename)
            self.write_guarded_file(proxy_npc_template, service, output_filename, service=service)

    def generate_si_adapter(self, service: ServiceView):
        if service.user_defined_type == "npc":
            """Generate the adapter_<name>.h file for the service interface"""
            si_adapter_template = self.load_template("npc_binding/skeleton_impl_h.j2")
            output_filename = "{}_skeleton_impl.h".format(service.service_deployment.standard_name)
            self._log.debug("Writing content to file %s", output_filename)
            self.write_guarded_file(si_adapter_template, service, output_filename, service=service)

    def generate_machine_config(self, machine, provided_services):
        self.generate_npc_json(machine, provided_services)

