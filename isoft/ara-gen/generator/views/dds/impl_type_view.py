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
TopicTypeView is the View for service topic type
"""
from typing import List
from generator.intermediate_model.types.impl_type import ImplDataType
from generator.intermediate_model.types.structure_impl_type import StructureImplDataType
from generator.intermediate_model.types.base_type import BaseType

from generator.views.impltype_view import ImplTypeView as ImplTypeViewBase
from generator.views.impltype_view import StringImplTypeView as StringImplTypeViewBase
from generator.views.impltype_view import StructImplTypeView as StructImplTypeViewBase
from generator.views.impltype_view import EnumImplTypeView as EnumImplTypeViewBase
from generator.views.impltype_view import MapImplTypeView as MapImplTypeViewBase
from generator.views.impltype_view import AliasedImplTypeView as AliasedImplTypeViewBase
from generator.views.impltype_view import VectorImplTypeView as VectorImplTypeViewBase
from generator.views.impltype_view import VariantImplTypeView as VariantImplTypeViewBase
from generator.views.impltype_view import ArrayImplTypeView as ArrayImplTypeViewBase
from generator.views.impltype_view import AliasedBaseImplTypeView as AliasedBaseImplTypeViewBase
from generator.views.basetype_view import BaseTypeView as BaseTypeViewBase


class ImplTypeView(ImplTypeViewBase):
    def __init__(self, impltype) -> None:
        super().__init__(impltype)
        self._converter_to_be_generated = False

    @property
    def converter_to_be_generated(self):
        return self._converter_to_be_generated

    @property
    def idl_type_name(self):
        return self.type_name

    @property
    def idl_self_include(self) -> str:
        if self._impltype.to_be_generated:
            return '"' + "/".join(self.lowNamespace + ["impl_type_" + self.standard_name + ".idl"]) + '"'
        elif self._impltype._type_emitter == "TYPE_EMITTER_DDS_RPC":
            return '"dds_idl/dds_rpc.idl"'
        elif self._impltype._type_emitter == "TYPE_EMITTER_ARA_CORE":
            return '"dds_idl/ara_core.idl"'
        return None

    @property
    def idl_self_include_fastdds(self) -> str:
        if self._impltype.to_be_generated:
            if self.interface_type:
                return '"dds/' + "/".join(self.lowNamespace +["impl_type_" + self.standard_name + ".idl"]) + '"'
            else:
                return '"' + "/".join(self.lowNamespace + ["impl_type_" + self.standard_name + ".idl"]) + '"'
        elif self._impltype._type_emitter == "TYPE_EMITTER_DDS_RPC":
            return '"dds/dds_rpc.idl"'
        elif self._impltype._type_emitter == "TYPE_EMITTER_ARA_CORE":
            return '"dds/ara/core/ara_core.idl"'
        return None


    @property
    def hide_in_namespace(self) -> str:
        return self._impltype.to_be_generated or self._impltype._type_emitter == "TYPE_EMITTER_ARA_CORE"

    @property
    def idl_converter_self_include(self):
        if self.converter_to_be_generated:
            #return '"' + "/".join(self.namespace + ["type_converter_" + self.standard_name + ".h"]) + '"'
            return '"' + "/".join(["ara","com","internal","fastdds"] + ["type_converter_" + self.standard_name + ".h"]) + '"'
        return None

    @property
    def idl_converter_self_include_fastdds(self):
        if self.converter_to_be_generated:
            return '"' + "/".join(["ara","com","internal","fastdds"]+["type_converter_" + self.standard_name + ".h"]) + '"'
        return None

    @property
    def qualified_idl_type_cpp_name(self) -> str:
        return "::".join(["", "dds_types"] + self.qualified_namespace[1:] + [self.name])

    @property
    def qualified_idl_type_cpp_name_fastdds(self) -> str:
        if self.interface_type:
            return "::".join(["","dds"]+self.qualified_namespace + [self.name])
        return "::".join(self.qualified_namespace +[self.name])

    @property
    def qualified_idl_type_cpp_name_fastdds_ROS2(self) -> str:
        if self.qualified_namespace:
            return "::".join(self.qualified_namespace + ["dds_"]+[self.name])+"_"
        else:
            return self.name
    @property
    def qualified_idl_name(self) -> str:
        # dds_namespace = [""]
        # if self.hide_in_namespace:
        #     dds_namespace.append("dds_types")
        # dds_namespace += self.qualified_namespace[1:]
        dds_namespace = []
        if self.hide_in_namespace:
            dds_namespace.append("dds")
        dds_namespace += self.qualified_namespace
        return "::".join(dds_namespace + [self.idl_type_name])

    @property
    def qualified_idl_name_fastdds(self) -> str:
        # dds_namespace = [""]
        # if self.hide_in_namespace:
        #     dds_namespace.append("fastdds")


        classname = self.__class__.__name__
        if classname == "MapImplTypeView":
            if self.value_data_type.final_type.__class__.__name__ == "BaseType" and self.key_data_type.final_type.__class__.__name__ == "BaseType":
                return "map<"+self.key_data_type.name.replace("_t","")+","+"::".join(self.value_data_type.namespace + [self.value_data_type.name.replace("_t","")])+"> "
            elif self.value_data_type.final_type.__class__.__name__ == "BaseType":
                return "map<" + "::".join(
                    ["dds"] + self.key_data_type.namespace +[self.key_data_type.name.replace("_t", "")]) + "," + "::".join(
                    self.value_data_type.namespace + [self.value_data_type.name.replace("_t", "")]) + "> "
            elif self.key_data_type.final_type.__class__.__name__ == "BaseType":
                if self.value_data_type.final_type.__class__.__name__ == "MapImplDataType":
                    return "map<" + "::".join(
                        [self.key_data_type.name.replace("_t", "")]) + "," + "map<"+self.value_data_type.key_data_type.name.replace("_t","")+","+"::".join(self.value_data_type.value_data_type.namespace + [self.value_data_type.value_data_type.name.replace("_t","")])+"> " + "> "
                else:
                    return "map<" + "::".join(
                        [self.key_data_type.name.replace("_t", "")]) + "," + "::".join(
                        ["dds"] + self.value_data_type.namespace + [self.value_data_type.name.replace("_t", "")]) + "> "
            else:
                return "map<" +  "::".join(
                    ["dds"] + self.key_data_type.namespace+[self.key_data_type.name.replace("_t", "")]) + "," + "::".join(
                    ["dds"] + self.value_data_type.namespace + [self.value_data_type.name.replace("_t", "")]) + "> "


        dds_namespace = self.qualified_namespace
        if "::".join(dds_namespace) != "dds::rpc":
            dds_namespace = self.qualified_namespace
            if self.interface_type:
                dds_namespace=["dds"]+dds_namespace

        return "::".join(dds_namespace + [self.idl_type_name])

    @property
    def idl_includes(self) -> List[str]:
        result = []
        for v in self.referred_views:
            if v.idl_self_include and not v.idl_self_include in result:
                result += [v.idl_self_include]
        return sorted(result)

    @property
    def idl_includes_fastdds(self) -> List[str]:
        result = []
        for v in self.referred_views:
            if v.idl_self_include_fastdds and not v.idl_self_include_fastdds in result:
                result += [v.idl_self_include_fastdds]
        return sorted(result)

    @property
    def idl_converter_includes(self) -> List[str]:
        result = []
        for v in self.referred_views:
            if v.idl_converter_self_include and not v.idl_converter_self_include in result:
                result += [v.idl_converter_self_include]
        return sorted(result)

    @property
    def idl_converter_includes_fastdds(self) -> List[str]:
        result = []
        for v in self.referred_views:
            if v.idl_converter_self_include_fastdds and not v.idl_converter_self_include_fastdds in result:
                result += [v.idl_converter_self_include_fastdds]
        return sorted(result)

    @property
    def idl_type_support_includes(self) -> List[str]:
        return [
            '"' + "/".join(self.lowNamespace + ["impl_type_{0}TypeSupportS.h".format(self.type_name.lower())]) + '"',
            '"' + "/".join(self.lowNamespace + ["impl_type_{0}TypeSupportImpl.h".format(self.type_name.lower())]) + '"'
        ]

    @property
    def idl_type_support_includes_fastdds(self) -> List[str]:
        classname = self.__class__.__name__
        if classname == 'EnumImplTypeView':
            if self.interface_type:
                return [
                    '"dds/' + "/".join(self.lowNamespace + ["impl_type_{0}.hpp".format(self.type_name.lower())]) + '"'
                ]
            else:
                return [
                '"' + "/".join(self.lowNamespace + ["impl_type_{0}.hpp".format(self.type_name.lower())]) + '"'
        ]
        else:
            if self.interface_type:
                return [
                    '"dds/' + "/".join(self.lowNamespace + ["impl_type_{0}.hpp".format(self.type_name.lower())]) + '"',
                    '"dds/' + "/".join(self.lowNamespace + ["impl_type_{0}PubSubTypes.hpp".format(self.type_name.lower())]) + '"'
                ]
            else:
                return [
                '"' + "/".join(self.lowNamespace + ["impl_type_{0}.hpp".format(self.type_name.lower())]) + '"',
                '"' + "/".join(self.lowNamespace + ["impl_type_{0}PubSubTypes.hpp".format(self.type_name.lower())]) + '"'
                ]

class BaseTypeView(BaseTypeViewBase):
    def __init__(self, basetype: BaseType, cpptype_name) -> None:
        super().__init__(basetype, cpptype_name)

    @property
    def idl_type_name(self):
        # @uptrace{SWS_CM_11041, 3c57b9d4612b0f15a3c6f119ffd1654192482ff2}
        cpp_to_idl_types_map = {
            "bool"     : "boolean",
            "uint8_t"  : "uint8",
            "uint16_t" : "uint16",
            "uint32_t" : "uint32",
            "uint64_t" : "uint64",
            "int8_t"   : "int8",
            "int16_t"  : "int16",
            "int32_t"  : "int32",
            "int64_t"  : "int64",
            "float"    : "float",
            "double"   : "double",
            "std::uint8_t"  : "uint8",
            "std::uint16_t" : "uint16",
            "std::uint32_t" : "uint32",
            "std::uint64_t" : "uint64",
            "std::int8_t"   : "int8",
            "std::int16_t"  : "int16",
            "std::int32_t"  : "int32",
            "std::int64_t"  : "int64",
        }
        if self._basetype.type_is_new:
            result = cpp_to_idl_types_map[self.type_name]
        else:
            result = cpp_to_idl_types_map[self.cpptype_name]

        return result

    @property
    def idl_self_include(self) -> str:
        return '"dds_idl/dds_base_types.idl"'
    @property
    def idl_self_include_fastdds(self):
        return '"dds/dds_base_types.idl"'

    @property
    def converter_to_be_generated(self):
        return False

    @property
    def idl_converter_self_include(self):
        return None

    @property
    def idl_converter_self_include_fastdds(self):
        return None

    @property
    def idl_type_support_includes(self) -> List[str]:
        return []

    @property
    def idl_type_support_includes_fastdds(self) -> List[str]:
        return []

    @property
    def qualified_idl_type_cpp_name(self) -> str:
        return self.qualified_cpp_name

    @property
    def qualified_idl_type_cpp_name_fastdds(self) -> str:
        return self.qualified_cpp_name

    @property
    def qualified_cpp_name_fastdds(self) -> str:
        return self.qualified_cpp_name

    @property
    def qualified_idl_name(self) -> str:
        return self.idl_type_name

    @property
    def qualified_idl_name_fastdds(self) -> str:
        return self.idl_type_name







class StructImplTypeView(StructImplTypeViewBase, ImplTypeView):
    def __init__(self, impltype) -> None:
        super().__init__(impltype)
        self._converter_to_be_generated = True
        from generator.views.dds.model_to_view import ViewManipulations
        self._subelements = [(x[0], ViewManipulations.to_view(x[1])) for x in self._impltype.sub_elements]
    @property
    def subelemtype_decl_list_idl(self):
        #ignore optionality for now
        subelemtype_decl_list = []
        for _subelemtype in self._subelements:
            subelemtype_decl_list.append(_subelemtype[1].qualified_idl_name + ' ' + _subelemtype[0] + ';')
        return subelemtype_decl_list

    @property
    def subelemtype_decl_list_idl_fastdds(self):
        # ignore optionality for now
        subelemtype_decl_list = []
        for _subelemtype in self._subelements:
            subelemtype_decl_list.append(_subelemtype[1].qualified_idl_name_fastdds + ' ' + _subelemtype[0] + ';')
        return subelemtype_decl_list



class EnumImplTypeView(EnumImplTypeViewBase, ImplTypeView):
    def __init__(self, impltype) -> None:
        super().__init__(impltype)
        self._converter_to_be_generated = True

    @property
    def literal_decl_list_idl(self):
        result = []
        for lit in self._impltype.literal_value_map:
            declaration = lit[0]
            if lit[1] is not None:
                declaration += '/* = {}U */'.format(lit[1])
            result.append(declaration)
        return result

class AliasedImplTypeView(AliasedImplTypeViewBase, ImplTypeView):
    def __init__(self, impltype) -> None:
        super().__init__(impltype)

    @property
    def aliasedtype_idl(self):
        aliased_type_view = self.referred_views[0]
        return aliased_type_view.qualified_idl_name

class StringImplTypeView(StringImplTypeViewBase, ImplTypeView):
    def __init__(self, impltype) -> None:
        super().__init__(impltype)

class VectorImplTypeView(VectorImplTypeViewBase, ImplTypeView):
    def __init__(self, impltype) -> None:
        super().__init__(impltype)


class VariantImplTypeView(VariantImplTypeViewBase, ImplTypeView):
    def __init__(self, impltype) -> None:
        super().__init__(impltype)
        self._converter_to_be_generated = True


class ArrayImplTypeView(ArrayImplTypeViewBase, ImplTypeView):
    def __init__(self, impltype) -> None:
        super().__init__(impltype)

class AliasedBaseImplTypeView(AliasedBaseImplTypeViewBase, ImplTypeView):
    def __init__(self, impltype) -> None:
        super().__init__(impltype)

class MapImplTypeView(MapImplTypeViewBase, ImplTypeView):
    def __init__(self, impltype) -> None:
        super().__init__(impltype)

class UnionImplTypeView(ImplTypeView):
    """ImplTypeView with added data for structure subelements"""
    def __init__(self, desc_type, impltype) -> None:
        super().__init__(impltype)
        self._discriminator = desc_type
        from generator.views.dds.model_to_view import ViewManipulations
        self._subelements = [(x[0], ViewManipulations.to_view(x[1]), x[4]) for x in self._impltype.sub_elements]
        for sub_element in self._subelements:
            self.add_referred_view(sub_element[1])

    @property
    def self_include(self):
        return None

    @property
    def discriminator_type(self):
        return self._discriminator

    @property
    def default_type(self):
        return 0

    @property
    def subelemnames(self):
        """Get the subelements name list"""
        subelemnames = []
        for _subelemtype in self._subelements:
            subelemnames.append(_subelemtype[0])
        return subelemnames

    @property
    def subelemtype_decl_list(self):
        subelemtype_decl_list = []
        for _subelemtype in self._subelements:
            subelemtype_decl_list.append(
                "case " + _subelemtype[0] + ": " + _subelemtype[1].qualified_idl_name + " " + _subelemtype[2] + ";")
        return subelemtype_decl_list

    @property
    def subelemtype_decl_list_fastdds(self):
        subelemtype_decl_list = []
        for _subelemtype in self._subelements:
            subelemtype0 = _subelemtype[0]
            if subelemtype0 == "DDS::RETCODE_OK":
                subelemtype0 = subelemtype0.replace("DDS","dds")
            if subelemtype0 == "DDS::RETCODE_ERROR":
                subelemtype0 = subelemtype0.replace("DDS", "dds")
            subelemtype1qualified_idl_name_fastdds = _subelemtype[1].qualified_idl_name_fastdds
            if subelemtype1qualified_idl_name_fastdds == "ara::core::ErrorCode":
                subelemtype1qualified_idl_name_fastdds = "dds::ara::core::ErrorCode"
            subelemtype_decl_list.append(
                "case " + subelemtype0 + ": " + subelemtype1qualified_idl_name_fastdds + " " + _subelemtype[2] + ";")
        return subelemtype_decl_list


class MethodRequestTopicTypeView(StructImplTypeView):
    def __init__(self, name,  call_type: ImplDataType, descPath) -> None:
        header = StructureImplDataType("RequestHeader", "TYPE_EMITTER_DDS_RPC", [], ["dds", "rpc"])
        elements = [
            ("header", header, False, None),
            ("data", call_type, False, None)
        ]
        impltype = StructureImplDataType(name, None, elements, call_type.namespace)
        super().__init__(impltype)
        for sub_element in self._subelements:
            self.add_referred_view(sub_element[1])

        self._desc_path = descPath

    @property
    def desc_paths(self):
        return [self._desc_path]

    @property
    def info_dds_names(self):
        return ["void"]

class MethodReplyTopicTypeView(StructImplTypeView):
    def __init__(self, name,  return_type: ImplDataType, descPath) -> None:
        header = StructureImplDataType("ReplyHeader", "TYPE_EMITTER_DDS_RPC", [], ["dds", "rpc"])
        elements = [
            ("header", header, False, None),
            ("data", return_type, False, None)
        ]
        impltype = StructureImplDataType(name, None, elements, return_type.namespace)
        super().__init__(impltype)
        for sub_element in self._subelements:
            self.add_referred_view(sub_element[1])

        self._desc_path = descPath

    @property
    def desc_paths(self):
        return [self._desc_path]

    @property
    def info_dds_names(self):
        return ["void"]