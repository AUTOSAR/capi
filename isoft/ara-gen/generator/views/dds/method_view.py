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
MethodView corresponding to a Method IM object.
"""
from generator.intermediate_model import BaseType
from generator.intermediate_model.types.impl_type import ImplDataType
from generator.intermediate_model.types.structure_impl_type import StructureImplDataType
from generator.views.dds.idl_header_view import IdlHeaderView
from generator.views.method_view import MethodView as MethodViewBase
from generator.views.argument_view import ArgumentView as ArgumentViewBase
from generator.intermediate_model.communication_management.method import Method
from generator.views.dds.impl_type_view import UnionImplTypeView, StructImplTypeView
from generator.views.dds.model_to_view import ViewManipulations

class ArgumentView(ArgumentViewBase):
    def __init__(self, argument) -> None:
        super().__init__(argument)
        self.type_view = ViewManipulations.convert_to_view_tree(argument.impl_type)

class MethodView(MethodViewBase):
    """Specialized View of a method."""

    def __init__(self, method: Method) -> None:
        super().__init__(method)

    def _make_argument_view(self, arg):
        return ArgumentView(arg)

    @property
    def request_topic(self):
        return self._method.deployment.request_topic

    @property
    def reply_topic(self):
        return self._method.deployment.reply_topic

    @property
    def qos_profile(self):
        return self._method.deployment.qos_profile

    @property
    def transport_protocols(self):
        return self._method.deployment.transport_protocols

    @staticmethod
    def get_unused_member_type():
        return StructureImplDataType("UnusedMember", "TYPE_EMITTER_DDS_RPC", [], ["dds", "rpc"])

    @staticmethod
    def get_error_code_type():
        return StructureImplDataType("ErrorCode", "TYPE_EMITTER_ARA_CORE", [], ['ara', 'core'])

    @staticmethod
    def make_result_type(namespace, include, name, output_type):
        elements = [("DDS::RETCODE_OK", output_type.model_object, False, None, "result"),
                    ("DDS::RETCODE_ERROR", MethodView.get_error_code_type(), False, None, "error")]
        impltype = StructureImplDataType(name, None, elements, namespace)
        type = UnionImplTypeView(ViewManipulations.to_view(BaseType("int32_t", "cstdint")), impltype)
        type.add_referred_view(include)
        type.add_referred_view(IdlHeaderView("","dds/DdsDcpsInfrastructure.idl"))
        return type

    @staticmethod
    def make_result_type_forfiled(namespace, include, name, output_type):
        elements = [("DDS::RETCODE_OK", output_type.model_object, False, None, "result_"),
                    ("DDS::RETCODE_ERROR", MethodView.get_error_code_type(), False, None, "error")]
        impltype = StructureImplDataType(name, None, elements, namespace)
        type = UnionImplTypeView(ViewManipulations.to_view(BaseType("int32_t", "cstdint")), impltype)
        type.add_referred_view(include)
        type.add_referred_view(IdlHeaderView("", "dds/DdsDcpsInfrastructure.idl"))
        return type

    @staticmethod
    def make_entry_type(namespace, name, elements):
        type = StructImplTypeView(StructureImplDataType(name, None, elements, namespace))
        for sub_element in type._subelements:
            type.add_referred_view(sub_element[1])
        return type

    def get_input_type(self, service_id, namespace):
        name = service_id + "Method_" + self.name + "_In"
        elements = [(x.model_object.name, x.model_object.impl_type, False, None) for x in self.get_input_args]
        if not len(elements):
            elements.append(("dummy", MethodView.get_unused_member_type(), False, None))
        return MethodView.make_entry_type(namespace, name, elements)

    def get_output_type(self, service_id, namespace):
        name = service_id + "Method_" + self.name + "_Out"
        elements = [(x.model_object.name, x.model_object.impl_type, False, None) for x in self.get_output_args]
        if not len(elements):
            elements.append(("dummy", self.get_unused_member_type(), False, None))
        return MethodView.make_entry_type(namespace, name, elements)

    def get_ara_output_type(self, namespace):
        name =  self.name + "Output"
        elements = [(x.model_object.name, x.model_object.impl_type, False, None) for x in self.get_output_args]
        if not len(elements):
            return None
        type = StructImplTypeView(StructureImplDataType(name, "TYPE_EMITTER_METHOD", elements, namespace))
        for sub_element in type._subelements:
            type.add_referred_view(sub_element[1])
        return type

    def get_result_type(self, service_id, namespace, include):
        name = service_id + "Method_" + self.name + "_Result"
        return MethodView.make_result_type(namespace, include, name, self.get_output_type(service_id, namespace))

    @property
    def firstlowername(self):
        return self.name[:1].lower() + self.name[1:]