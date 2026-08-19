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

from generator.generator.datatype_container import DataTypeContainer
from generator.parser.exception_handling import handle_method_exceptions
from generator.views.dds.model_to_view import ViewManipulations


class FastDdsBinding:
    def __init__(self, renderer, output_dir, machine_dir, processes_dir) -> None:
        self._renderer = renderer
        self.internalNs = ["ara", "com", "internal", "fastdds"]
        self._output_dir = output_dir
        self._log = logging.getLogger(__name__)
        self.fastdds_root_dir = output_dir  # os.path.join(output_dir, 'fastdds')
        self.machine_config_root_dir = machine_dir
        self.processes_root_dir = processes_dir
        self.converter_includes = []

    def fastdds_service_dir(self, service):
        if service.lowNamespaces is not None:
            repath = os.path.join(self.fastdds_root_dir, *service.lowNamespaces)
            return os.path.join(repath, "fastdds")
        else:
            return self.fastdds_root_dir

    def includes_dir_for(self, namespaces_list):
        if namespaces_list is None:
            self._log.debug("NS list is none")
            namespaces_list = []
        lowered_ns_list = [x.lower() for x in namespaces_list]
        return os.path.join(self.fastdds_root_dir, *lowered_ns_list)

    def write_guarded_file(self, template, ns, filename, **kwargs):
        output_dir = self.includes_dir_for(ns)
        output_dir = output_dir+"/fastdds"
        self._renderer.write_output(
            template,
            output_dir,
            filename,
            header_base=self._renderer.include_guard(ns, filename),
            **kwargs
        )

    def write_guarded_file_idl(self, template, ns, filename, **kwargs):
        if ns is None:
            self._log.debug("NS list is none")
            ns = []
        lowered_ns_list = [x.lower() for x in ns]
        output_dir =  os.path.join(self.fastdds_root_dir+"/fastdds_idl/", *lowered_ns_list)
        self._renderer.write_output(
            template,
            output_dir,
            filename,
            header_base=self._renderer.include_guard(ns, filename),
            **kwargs
        )

    def write_guarded_file_internal(self, template, ns, filename, **kwargs):
        output_dir = self.fastdds_root_dir+"/".join([""] + self.internalNs)
        self._renderer.write_output(
            template,
            output_dir,
            filename,
            header_base=self._renderer.include_guard(self.internalNs, filename),
            **kwargs
        )

    def write_guarded_file_idl_dds(self, template, ns, filename, **kwargs):
        output_dir = self.includes_dir_for(ns)
        output_dir = output_dir + "/fastdds_idl/dds"
        self._renderer.write_output(
            template,
            output_dir,
            filename,
            header_base=self._renderer.include_guard(ns, filename),
            **kwargs
        )

    def write_guarded_file_idl_dds_ns(self, template, ns, filename, **kwargs):
        if ns is None:
            self._log.debug("NS list is none")
            ns = []
        lowered_ns_list = [x.lower() for x in ns]
        output_dir =  os.path.join(self.fastdds_root_dir+"/fastdds_idl/dds/", *lowered_ns_list)
        self._renderer.write_output(
            template,
            output_dir,
            filename,
            header_base=self._renderer.include_guard(ns, filename),
            **kwargs
        )

    def write_guarded_file_idl_dds_core(self, template, ns, filename, **kwargs):
        output_dir = self.includes_dir_for(ns)
        output_dir = output_dir + "/fastdds_idl/dds/ara/core"
        self._renderer.write_output(
            template,
            output_dir,
            filename,
            header_base=self._renderer.include_guard(ns, filename),
            **kwargs
        )

    def write_guarded_file_fastddsmapping(self, template, ns, filename, **kwargs):
        output_dir = self.includes_dir_for(ns)
        output_dir = output_dir+"/fastdds_mapping"
        self._renderer.write_output(
            template,
            output_dir,
            filename,
            header_base=self._renderer.include_guard(ns, filename),
            **kwargs
        )

    def write_guarded_h_file(self, template, service_view, filename, **kwargs):
        output_dir = self.fastdds_service_dir(service_view)
        self._renderer.write_output(
            template,
            output_dir,
            filename,
            header_base=self._renderer.include_guard(service_view.namespaces+["fastdds"], filename),
            **kwargs
        )

    def load_template(self, template_name):
        return self._renderer.load_template(template_name, trim_blocks=True)

    def addfastdds(self, includes):
        result = []
        for include in includes:
            re = include.replace("type_","fastdds/type_")
            result.append(re)
        return result

    @handle_method_exceptions(error_string_template="FASTDDS service description generation ERROR: {exception_text}")
    def generate_service_desc(self, service_interface):
        """Generate the cpp and h files for service description"""
        si_desc_template_h = self.load_template("fastdds_binding/service_desc_xxx_h.j2")
        output_filename = "{}_service_desc.h".format(service_interface.service_deployment.standard_name)
        self._log.debug("Writing %s content to file", output_filename)
        include = service_interface.type_info_includes_hpp + self.converter_includes
        self.write_guarded_h_file(si_desc_template_h, service_interface, output_filename, service=service_interface,includes=include)

    @handle_method_exceptions(error_string_template="FASTDDS JSON config generation ERROR: {exception_text}")
    def generate_fastdds_json(self, machine, services):
        """Generate the fastdds json file required by genivi fastdds"""
        fastdds_template = self.load_template("fastdds_binding/app_fastdds_json.j2")

        output_filename = "{}_fastdds.json".format(machine.name)
        self._renderer.write_output(
            fastdds_template,
            self.machine_config_root_dir,
            output_filename,
            machine=machine,
            services=services
        )

    def generate_proxy(self, service):
        """Generate the proxy h file required by application"""
        proxy_fastdds_template = self.load_template("fastdds_binding/proxy_fastdds_xxx_h.j2")

        output_filename = "{}_proxy_impl.h".format(service.service_deployment.standard_name)
        self._log.debug("Writing content to file %s", output_filename)
        includes = self.converter_includes + service.type_info_includes
        self.write_guarded_h_file(proxy_fastdds_template, service, output_filename, service=service,includes=includes)

    def generate_si_adapter(self, service):
        """Generate the adapter_<name>.h file for the service interface"""
        si_adapter_template = self.load_template("fastdds_binding/skeleton_impl_h.j2")
        output_filename = "{}_skeleton_impl.h".format(service.service_deployment.standard_name)
        self._log.debug("Writing content to file %s", output_filename)
        includes = self.converter_includes + service.type_info_includes
        self.write_guarded_h_file(si_adapter_template, service, output_filename, service=service,includes=includes)

    def generate_machine_config(self, machine, provided_services):
        # fastdds does not need to know which services are required
        self.generate_fastdds_json(machine, provided_services)
       # self.generate_app_fastdds_json(machine, provided_services)

    def _generate_service_common_idls(self, services):
        template = self.load_template("fastdds_binding/service_common_xxx_idl.j2")
        for service in services:
            self.write_guarded_file_idl(template, service.namespaces, service.common_idl_name, service=service)

    def _generate_type_idls(self, type_views):
        """Generate idl files for the whole types tree"""
        template_map = {
            'structure':             "fastdds_binding/idl_types/struct_impl_type_idl.j2",
            'event_topic':           "fastdds_binding/idl_types/event_topic_type_idl.j2",
            'method_request_topic':  "fastdds_binding/idl_types/method_request_topic_type_idl.j2",
            'method_reply_topic':    "fastdds_binding/idl_types/method_reply_topic_type_idl.j2",
            'union':                 "fastdds_binding/idl_types/union_impl_type_idl.j2",
            'enum':                  "fastdds_binding/idl_types/enum_impl_type_idl.j2",
            'map':                   "fastdds_binding/idl_types/map_impl_type_idl.j2",
            'vector':                "fastdds_binding/idl_types/vector_impl_type_idl.j2",
            'variant':               "fastdds_binding/idl_types/variant_impl_type_idl.j2",
            'array':                 "fastdds_binding/idl_types/array_impl_type_idl.j2",
            'aliased_impltype':      "fastdds_binding/idl_types/aliased_impl_type_idl.j2",
            'aliased_basetype':      "fastdds_binding/idl_types/aliased_base_type_idl.j2",
            'string':                "fastdds_binding/idl_types/string_impl_type_idl.j2",
            'fixed_string':          "fastdds_binding/idl_types/string_fixed_impl_type_idl.j2",
            'basetype':              "",  # no need to generate anything for basetypes
        }

        for tv in type_views:
            kind = ViewManipulations.get_view_kind(tv)
            assert kind in template_map, "unknown view type {0}({1})".format(kind, tv)
            template_filename = template_map[kind]
            if template_filename:
                template = self.load_template(template_filename)
                typename = tv.type_name.lower()
                output_filename = "impl_type_{0}.idl".format(typename)
                if tv.interface_type:
                    self.write_guarded_file_idl_dds_ns(template, tv.namespace, output_filename, type_view=tv)
                else:
                    self.write_guarded_file_idl(template, tv.namespace, output_filename, type_view=tv)

        ddsmappingtemplate = self.load_template("fastdds_binding/idl_types/dds_type_mapping_h.j2")
        isHaveMapping = False
        if len(type_views)>0:
            new_type_views = []
            for type_view_item in type_views:
                if "StringImplTypeView" in type_view_item.__class__.__name__:
                    pass
                else:
                    new_type_views.append(type_view_item)
            if len(new_type_views) > 0:
                isHaveMapping = True
                self.write_guarded_file_internal(ddsmappingtemplate, new_type_views[0].namespace, "dds_type_mapping.h", type_views=new_type_views)
        if not isHaveMapping:
            self.write_guarded_file_internal(ddsmappingtemplate, [], "dds_type_mapping.h", type_views=[])

        template1 = self.load_template("fastdds_binding/idl_types/ara_core_idl.j2")
        template2 = self.load_template("fastdds_binding/idl_types/dds_base_types_idl.j2")
        template3 = self.load_template("fastdds_binding/idl_types/dds_rpc_idl.j2")
        template4 = self.load_template("fastdds_binding/idl_types/type_converter_ara_core_h.j2")

        output_filename1 = "ara_core.idl"
        output_filename2 = "dds_base_types.idl"
        output_filename3 = "dds_rpc.idl"
        output_filename4 = "type_converter_ara_core.h"

        if len(type_views)>0:
            self.write_guarded_file_idl_dds_core(template1, [], output_filename1, type_view=None)
            self.write_guarded_file_idl_dds(template2, [], output_filename2, type_view=None)
            self.write_guarded_file_idl_dds(template3, [], output_filename3, type_view=None)
            self.write_guarded_file_internal(template4, type_views[0].namespace, output_filename4, type_view=None)

    @handle_method_exceptions(error_string_template="idls generation ERROR: {exception_text}")
    def generate_idls(self, services):
        """Generate idl files for the whole types tree"""
        interface_types = DataTypeContainer(services).types
        for item in interface_types:
            item.set_interface_type(True)
            referred_views = item.referred_views
            for referred in referred_views:
                if hasattr(referred, "set_interface_type"):
                    referred.set_interface_type(True)
            if hasattr(item, "element_type"):
                if hasattr(item.element_type, "set_interface_type"):
                    item.element_type.set_interface_type(True)
            if hasattr(item, "element_type_list"):
                for element_type in item.element_type_list:
                    if hasattr(element_type, "set_interface_type"):
                        element_type.set_interface_type(True)
            if hasattr(item, "keytype"):
                if hasattr(item.keytype, "set_interface_type"):
                    item.keytype.set_interface_type(True)
            if hasattr(item, "valuetype"):
                if hasattr(item.valuetype, "set_interface_type"):
                    item.valuetype.set_interface_type(True)
        event_types = []
        method_types = []
        for service in services:
            event_types += service.get_event_topic_types()
            event_types += service.get_field_notifier_topic_types()
            method_types += service.get_method_dds_types()['all']
            method_types += service.get_field_method_dds_types()['all']
        self._generate_service_common_idls(services)
        type_view = event_types + method_types
        for item in type_view:
            referred_views = item.referred_views
            for referred in referred_views:
                if referred in interface_types:
                    referred.set_interface_type(True)
        self._generate_type_idls(interface_types + type_view)

    def isInconversions(self,conversions,conversion):
        ddstypename = "".join(conversion["dds_type"].namespace)+conversion["dds_type"].type_name
        aratypename = "".join(conversion["ara_type"].namespace)+conversion["ara_type"].type_name
        for con in conversions:
            ddstypename1 = "".join(con["dds_type"].namespace) + con["dds_type"].type_name
            aratypename1 = "".join(con["ara_type"].namespace) + con["ara_type"].type_name
            if ddstypename == ddstypename1 and aratypename == aratypename1:
                return True
        return False

    @handle_method_exceptions(error_string_template="ara-idl type converters generation ERROR: {exception_text}")
    def generate_type_converters(self, services):
        """Generate ara-idl types converters"""
        struct_template = self.load_template("fastdds_binding/type_converter_struct_xxx_h.j2")
        enum_template = self.load_template("fastdds_binding/type_converter_enum_xxx_h.j2")
        variant_template = self.load_template("fastdds_binding/type_converter_variant_xxx_h.j2")
        self.converter_includes = []
        conversions = []
        for service in services:
            conversions += service.get_event_topic_type_conversions()
            conversions += service.get_field_notifier_topic_topic_conversions()
            conversions += service.get_method_dds_type_conversions()
            conversions += service.get_interface_type_conversions()

        newconversions = []
        for con in conversions:
            if not self.isInconversions(newconversions,con):
                newconversions.append(con)
        for conv in newconversions:
            kind = ViewManipulations.get_view_kind(conv['dds_type'])
            if kind == 'structure':
                template = struct_template
            elif kind == 'enum':
                template = enum_template
            elif kind == 'variant':
                template = variant_template
            else:
                assert False, "unknown view type {0}({1})".format(kind, conv['dds_type'])

            output_filename = "type_converter_{0}.h".format(conv['dds_type'].type_name.lower())
            self.write_guarded_file_internal(
                template,
                conv['dds_type'].namespace,
                output_filename,
                dds_type=conv['dds_type'],
                ara_type=conv['ara_type'],
                services=services
            )
            type_converter_path = '"{}"'.format("/".join(self.internalNs + [output_filename]))
            if type_converter_path not in self.converter_includes:
                self.converter_includes.append(type_converter_path)
        ddstypeconvertertemplate = self.load_template("fastdds_binding/idl_types/dds_type_converter_h.j2")
        if len(services)>0:
            self.write_guarded_file_internal(ddstypeconvertertemplate, [], "dds_type_converter.h", conversions=newconversions,services=services)

    def _generate_event_topic_types_info(self, type_views):
        """Generate header files for the topic types"""
        template = self.load_template("fastdds_binding/type_info_xxx_h.j2")
        for tv in type_views:
            output_filename = "type_info_{0}.h".format(tv.standard_name)
            self.write_guarded_file_internal(template, tv.namespace, output_filename, dds_type=tv, ara_type=tv.data_type)

    def _generate_method_topic_types_info(self, type_views):
        template = self.load_template("fastdds_binding/type_info_xxx_h.j2")
        for tv in type_views:
            output_filename = "type_info_{0}.h".format(tv.standard_name)
            self.write_guarded_file_internal(template, tv.namespace, output_filename, dds_type=tv)

    @handle_method_exceptions(error_string_template="idl type info headers generation ERROR: {exception_text}")
    def generate_type_info_headers(self, services):
        """Generate idl type info header files for the whole types tree"""
        event_types = []
        method_types = []
        for service in services:
            event_types += service.get_event_topic_types()
            method_types += service.get_method_dds_types()['topic']
            method_types += service.get_field_method_dds_types()['topic']
            event_types += service.get_field_notifier_topic_types()
        eventOb = {}
        for item in event_types:
            standard_name = item.standard_name
            if standard_name in eventOb:
                for info_dds_name in item.info_dds_names:
                    eventOb[standard_name].set_event_dds_name(info_dds_name)
                for desc_path in item.desc_paths:
                    eventOb[standard_name].set_desc_path(desc_path)
            else:
                eventOb[standard_name] = item
        new_event_types = [eventOb[standard_name] for standard_name in eventOb]
        self._generate_event_topic_types_info(new_event_types)

        self._generate_method_topic_types_info(method_types)
