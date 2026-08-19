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
ServiceView corresponding to a Service IM object.
"""
import hashlib

from generator.generator.datatype_container import DataTypeContainer
from generator.intermediate_model import BaseType
from generator.intermediate_model.communication_management.service \
    import Service
from generator.intermediate_model.types.structure_impl_type import StructureImplDataType
from generator.parser.commen_util import get_hash
from generator.views.dds.idl_header_view import IdlHeaderView
from generator.views.dds.model_to_view import ViewManipulations
from generator.views.service_instance_view import ServiceInstanceView
from generator.views.dds.event_view import EventView
from generator.views.dds.field_view import FieldView
from generator.views.dds.method_view import MethodView
from generator.views.dds.event_topic_type_view import EventTopicTypeView
from generator.views.dds.impl_type_view import MethodRequestTopicTypeView, \
    MethodReplyTopicTypeView, UnionImplTypeView


class DdsServiceView(ServiceInstanceView):
    """Specialized View of a Service."""

    def __init__(self, service: Service) -> None:
        super().__init__(service)

    @property
    def instance_id(self):
        return "DDS:{}".format(self._service.service_deployment.instance_id)

    @property
    def deployment_id(self):
        """Get the service id"""
        return self._service.service_deployment.deployment_id

    @property
    def domain_id(self):
        """Get the service domain id"""
        return self._service.service_deployment.domain_id

    @property
    def at_key(self):
        return self._service.service_deployment.at_key

    @property
    def qos_profile(self):
        return self._service.service_deployment.qos_profile

    @property
    def transport_plugins(self):
        return self._service.service_deployment.transport_plugins

    @property
    def events(self):
        """Get the events as EventViews for this service"""
        event_views = list()
        for event in self._service.events:
            event_views.append(EventView(event))
        return event_views

    @property
    def methods(self):
        """Get the methods as MethodViews for this service"""
        method_views = list()
        for method in self._service.methods:
            method_views.append(MethodView(method))
        return method_views

    @property
    def all_fields(self):
        """Get the fields as FieldViews for this service"""
        field_views = list()
        for field in self._service.fields:
            field_views.append(FieldView(field))

        return field_views

    @property
    def fields_with_notifier(self):
        """Get the fields with notifying events as FieldViews for this service"""
        field_views = list()
        for field in self._service.fields:
            if field.has_notifier:
                field_views.append(FieldView(field))

        field_views.sort(key=lambda field: field.name)
        return field_views

    def _get_type_info_include(self, type):
        #return '"' + "/".join(type.namespace + ["type_info_" + type.standard_name + ".h"]) + '"'
        return '"' + "/".join(["ara","com","internal","fastdds"] + ["type_info_" + type.standard_name + ".h"]) + '"'

    @property
    def type_info_includes_hpp(self):
        result = []
        types = self.get_event_topic_types()
        types += self.get_field_notifier_topic_types()
        types += self.get_method_dds_types()['topic']
        types += self.get_field_method_dds_types()['topic']
        for x in types:
            result += x.idl_type_support_includes_fastdds
        # result += [self._get_type_info_include(x) for x in types]
        return sorted(list(set(result)))

    @property
    def type_info_includes(self):
        result = []
        types = self.get_event_topic_types()
        types += self.get_field_notifier_topic_types()
        types += self.get_method_dds_types()['topic']
        types += self.get_field_method_dds_types()['topic']
        result += [self._get_type_info_include(x) for x in types]
        return sorted(list(set(result)))
    @property
    def converter_includes(self):
        result = []
        conversions = self.get_interface_type_conversions()
        conversions += self.get_event_topic_type_conversions()
        conversions += self.get_field_notifier_topic_topic_conversions()
        conversions += self.get_method_dds_type_conversions()
        result += [x['dds_type'].idl_converter_self_include for x in conversions]
        return sorted(list(set(result)))

    def get_methods_desc_type(self):
        return ViewManipulations.to_view(BaseType("int32_t", "cstdint"))

    def get_method_hash_const(self, method):
        return {'type' : self.get_methods_desc_type(),
                'name' : self.deployment_id + "Method_"  + method.name + "_Hash",
                'value' : hex(get_hash("M"+method.name))}

    def get_field_getter_hash_const(self, field):
        return {'type' : self.get_methods_desc_type(),
                'name' : self.deployment_id + "Field_Get"  + field.name + "_Hash",
                'value' : hex(get_hash("FGet" + field.name))}

    def get_field_setter_hash_const(self, field):
        return {'type' : self.get_methods_desc_type(),
                'name' : self.deployment_id + "Field_Set"  + field.name + "_Hash",
                'value' : hex(get_hash("FSet" + field.name))}

    @property
    def common_idl_name(self):
        idlName = "{}_{}_dds_common.idl".format(self.standard_name, self.deployment_id)
        return idlName

    @property
    def common_idl_header(self):
        return IdlHeaderView(self.namespacepath , self.common_idl_name)

    @property
    def method_constants(self):
        return [self.get_method_hash_const(x) for x in self.methods]

    @property
    def field_constants(self):
        result = []
        for field in self.all_fields:
            if field.has_getter:
                result.append(self.get_field_getter_hash_const(field))
            if field.has_setter:
                result.append(self.get_field_setter_hash_const(field))
        return result

    @property
    def method_return_type(self):
        name = self.deployment_id + "Method_Return"
        elements = []
        for method in self.methods:
            result_type = method.get_result_type(self.deployment_id, self.namespaces, self.common_idl_header)
            hash_const = self.get_method_hash_const(method)
            elements.append((hash_const['name'], result_type.model_object, False, None, method.name))

        impltype = StructureImplDataType(name, None, elements, self.namespaces)
        type = UnionImplTypeView(self.get_methods_desc_type(), impltype)
        type.add_referred_view(self.common_idl_header)
        return type

    @property
    def method_call_type(self):
        name = self.deployment_id + "Method_Call"
        elements = []
        for method in self.methods:
            input_type = method.get_input_type(self.deployment_id, self.namespaces)
            hash_const = self.get_method_hash_const(method)
            elements.append((hash_const['name'], input_type.model_object, False, None, method.name))

        impltype = StructureImplDataType(name, None, elements, self.namespaces)
        type = UnionImplTypeView(self.get_methods_desc_type(), impltype)
        type.add_referred_view(self.common_idl_header)
        return type

    @property
    def field_call_type(self):
        name = self.deployment_id + "Field_Call"
        elements = []
        for field in self.all_fields:
            if field.has_getter:
                input_type = field.get_getter_input_type(self.deployment_id, self.namespaces)
                hash_const = self.get_field_getter_hash_const(field)
                elements.append((hash_const['name'], input_type.model_object, False, None, "get" + field.name))
            if field.has_setter:
                input_type = field.get_setter_input_type(self.deployment_id, self.namespaces)
                hash_const = self.get_field_setter_hash_const(field)
                elements.append((hash_const['name'], input_type.model_object, False, None, "set" + field.name))

        impltype = StructureImplDataType(name, None, elements, self.namespaces)
        type = UnionImplTypeView(self.get_methods_desc_type(), impltype)
        type.add_referred_view(self.common_idl_header)
        return type

    @property
    def field_return_type(self):
        name = self.deployment_id + "Field_Return"
        elements = []
        for field in self.all_fields:
            if field.has_getter:
                result_type = field.get_getter_result_type(self.deployment_id, self.namespaces, self.common_idl_header)
                hash_const = self.get_field_getter_hash_const(field)
                elements.append((hash_const['name'], result_type.model_object, False, None, "get" + field.name))
            if field.has_setter:
                result_type = field.get_setter_result_type(self.deployment_id, self.namespaces, self.common_idl_header)
                hash_const = self.get_field_setter_hash_const(field)
                elements.append((hash_const['name'], result_type.model_object, False, None, "set" + field.name))

        impltype = StructureImplDataType(name, None, elements, self.namespaces)
        type = UnionImplTypeView(self.get_methods_desc_type(), impltype)
        type.add_referred_view(self.common_idl_header)
        return type

    @property
    def method_request_topic_type(self):
        name = self.deployment_id + "Method_Request"
        return MethodRequestTopicTypeView(name, self.method_call_type.model_object, self.get_desc_path())

    @property
    def field_request_topic_type(self):
        name = self.deployment_id + "Field_Request"
        return MethodRequestTopicTypeView(name, self.field_call_type.model_object, self.get_desc_path())

    @property
    def method_reply_topic_type(self):
        name = self.deployment_id + "Method_Reply"
        return MethodReplyTopicTypeView(name, self.method_return_type.model_object, self.get_desc_path())

    @property
    def field_reply_topic_type(self):
        name = self.deployment_id + "Field_Reply"
        return MethodReplyTopicTypeView(name, self.field_return_type.model_object, self.get_desc_path())

    def get_event_topic_types(self):
        typeNameOb = {}
        for eventview in self.events:
            type_name = eventview.type_view.qualified_idl_name + "EventType"
            if type_name not in typeNameOb:
                type_view = EventTopicTypeView(eventview.type_view.type_name + "EventType" , eventview.type_view.model_object, self.at_key, self.get_desc_path())
                typeNameOb[type_name] = type_view
            else:
                type_view = typeNameOb[type_name]
            if self._service.service_deployment is not None:
                eventDdsName = "::".join([""] + self._service.lowNamespaces + ["fastdds", "{}{}Desc".format(self._service.service_deployment.name, eventview.name)])
                type_view.set_event_dds_name(eventDdsName)

        return [typeNameOb[item] for item in typeNameOb]

    # @uptrace{SWS_CM_11131, 269abd01748928a0461bed9f285bba467e88616d}
    def get_field_notifier_topic_types(self):
        typeNameOb = {}
        for fieldview in self.fields_with_notifier:
            type_name = fieldview.type_view.qualified_idl_name + "FieldNotifierType"
            if type_name not in typeNameOb:
                type_view = EventTopicTypeView(fieldview.type_view.type_name + "FieldNotifierType", fieldview.type_view.model_object, self.at_key, self.get_desc_path())
                typeNameOb[type_name] = type_view
            else:
                type_view = typeNameOb[type_name]
            if self._service.service_deployment is not None:
                eventDdsName = "::".join([""] + self._service.lowNamespaces + ["fastdds", "{}{}EventDesc".format(self._service.service_deployment.name, fieldview.name)])
                type_view.set_event_dds_name(eventDdsName)

        return [typeNameOb[item] for item in typeNameOb]

    def get_desc_path(self):
        if self._service.service_deployment is not None:
            return "/".join(self._service.lowNamespaces + ["fastdds", self._service.service_deployment.standard_name + "_service_desc.h"])
        return None

    def get_method_dds_types(self):
        topic_types = [self.method_request_topic_type, self.method_reply_topic_type]
        all_types = [self.method_call_type, self.method_return_type, *topic_types]
        for method in self.methods:
            all_types.append(method.get_input_type(self.deployment_id, self.namespaces))
            all_types.append(method.get_output_type(self.deployment_id, self.namespaces))
            all_types.append(method.get_result_type(self.deployment_id, self.namespaces, self.common_idl_header))
        return {'all' : all_types, 'topic' : topic_types}

    def get_field_method_dds_types(self):
        topic_types = [self.field_request_topic_type, self.field_reply_topic_type]
        all_types = [self.field_call_type, self.field_return_type, *topic_types]
        for field in self.all_fields:
            if field.has_getter:
                all_types.append(field.get_getter_input_type(self.deployment_id, self.namespaces))
                all_types.append(field.get_getter_output_type(self.deployment_id, self.namespaces))
                all_types.append(field.get_getter_result_type(self.deployment_id, self.namespaces, self.common_idl_header))
            if field.has_setter:
                all_types.append(field.get_setter_input_type(self.deployment_id, self.namespaces))
                all_types.append(field.get_setter_output_type(self.deployment_id, self.namespaces))
                all_types.append(field.get_setter_result_type(self.deployment_id, self.namespaces, self.common_idl_header))
        return {'all' : all_types, 'topic' : topic_types}

    @staticmethod
    def _make_event_topic_type_conversions(types):
        return [{'dds_type' : type.data_type, 'ara_type' : type.data_type}
                for type in types if type.data_type.converter_to_be_generated]

    def get_interface_type_conversions(self):
        return [{'dds_type' : type_view, 'ara_type' : type_view}
                for type_view in DataTypeContainer([self]).types if type_view.converter_to_be_generated]

    def get_event_topic_type_conversions(self):
        return DdsServiceView._make_event_topic_type_conversions(self.get_event_topic_types())

    def get_field_notifier_topic_topic_conversions(self):
        return DdsServiceView._make_event_topic_type_conversions(self.get_field_notifier_topic_types())

    def get_method_dds_type_conversions(self):
        conversions = []
        for method in self.methods:
            output = method.get_output_type(self.deployment_id, self.namespaces)
            ara_output = method.get_ara_output_type(self.namespaces + [self.name])
            if ara_output:
                conversions.append({'dds_type' : output, 'ara_type' : ara_output})
        return conversions

    @property
    def blacklisted_versions(self):
        """Get the blacklisted versions of the service."""
        key = "blacklisted_versions"
        if self._service.service_deployment.required is not None:
            if key in self._service.service_deployment.required:
                if self._service.service_deployment.required[key] is not None:
                    return self._service.service_deployment.required[key]
        return []
