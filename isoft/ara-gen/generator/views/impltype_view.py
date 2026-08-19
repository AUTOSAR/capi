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
ImpltypeView corresponding to a impltype declaration object.
"""

# Unused import check suppressed, because only used in type annotation,
# which is not found by PyLint
from typing import List, Tuple  # pylint: disable=unused-import
from generator.views.basetype_view import BaseTypeView
from generator.views.view import View
from generator.intermediate_model.types.impl_type import ImplDataType

import generator.views.model_to_view


class ImplTypeView(View):
    """Specialized View of a ImplType declaration."""

    def __init__(self, impltype: ImplDataType) -> None:

        super().__init__(impltype)
        self._impltype = impltype
        self._referred_views = []
        self.__id = "/".join(self.namespace + [self.type_name])

    def __eq__(self, other):
        """will be used to check whether particular item is in list"""
        if isinstance(other, ImplTypeView):
            return self.__id == other.__id
        return NotImplemented

    def __ne__(self, other):
        eq_result = self.__eq__(other)
        if eq_result is not NotImplemented:
            return not eq_result
        return NotImplemented

    def __lt__(self, other):
        return self.__id < other.__id

    def __hash__(self):
        """
        Please note the difference with __eq__, it is intentional
        it allows to check for multiple different defines for the same impltype
        """
        return hash(self._full_id)

    def __str__(self):
        """To make debugging easier"""
        return "({})".format(self._full_id)

    @property
    def _id(self):
        return "/".join(self.namespace + [self.type_name])

    @property
    def _referables_id(self):
        result = ""
        for t in self._impltype.referred_types:
            result += "[{0}]".format("/".join(t.namespace + [t.name]))
        return result

    @property
    def _full_id(self):
        return self.__id + "->" + self._referables_id

    @property
    def type_name(self):
        """Get the impltype name"""
        return self._impltype.name

    @property
    def self_include(self) -> str:
        if not self._impltype.to_be_generated:
            return None
        return '"' + "/".join(self.lowNamespace + ["impl_type_" + self.standard_name + ".h"]) + '"'


    @property
    def qualified_cpp_name(self) -> str:
        return "::".join([""]+self.qualified_namespace + [self.name])

    @property
    def qualified_cpp_name_fastdds(self) -> str:
        return "::".join(["","dds"]+self.qualified_namespace + [self.name])

    def cpp_name_in_namespace(self, qualified_namespace) -> str:
        qns = self.qualified_namespace
        if qualified_namespace == qns:
            qns = []
        return "::".join(qns + [self.name])

    @property
    def includes(self) -> List[str]:
        result = []
        for v in self.referred_views:
            if v.self_include and not v.self_include in result:
                result += [v.self_include]
        return sorted(result)

    @property
    def referred_views(self):
        return self._referred_views

    def add_referred_view(self, referred_view):
        self._referred_views.append(referred_view)

    @property
    def underlying_model_type(self):
        return self._impltype

    @property
    def subelemtype_decl_list_fastdds(self):
        return []

class StringImplTypeView(ImplTypeView):
    def __init__(self, impltype) -> None:
        super().__init__(impltype)

class FixedStringImplTypeView(ImplTypeView):
    def __init__(self, impltype) -> None:
        super().__init__(impltype)

    @property
    def arraySize(self):
        """Get the arraysize of the array"""
        return self._impltype.array_size

class StructImplTypeView(ImplTypeView):
    """ImplTypeView with added data for structure subelements"""
    def __init__(self, impltype) -> None:
        super().__init__(impltype)
        self._subelements = [(x[0], generator.views.model_to_view.ViewManipulations.to_view(x[1])) for x in self._impltype.sub_elements]

    @property
    def names_optionals_data_ids(self):
        """Get the subelements name, optional and data id list"""
        names_optionals_data_ids_list = list()
        for _subelemtype in self._impltype.sub_elements:
            struct_member_name = _subelemtype[0]
            optional = _subelemtype[2]
            tlv_data_id = None
            if _subelemtype[3] is not None:
                tlv_data_id = "0x{:04x}".format(_subelemtype[3])
            name_optional_data_id_tuple = (struct_member_name, optional, tlv_data_id)
            names_optionals_data_ids_list.append(name_optional_data_id_tuple)
        return names_optionals_data_ids_list

    @property
    def subelemtype_decl_list(self):
        """
        Get the list of the subelements names for the struct header template
        """
        subelemtype_decl_list = []
        for _subelemtype in self._impltype.sub_elements:
            if _subelemtype[2]:
                decl_string = "ara::core::Optional<" + "::".join(_subelemtype[1].qualified_namespace +
                                                                 [_subelemtype[1].name]) + '> ' + _subelemtype[0] + ';'
            else:
                decl_string = "::".join(_subelemtype[1].qualified_namespace + [_subelemtype[1].name]) + ' ' + _subelemtype[0] + ';'
                if _subelemtype[1].type_emitter and _subelemtype[1].type_emitter != "FundamentalType":
                    decl_string = "::" + decl_string
            subelemtype_decl_list.append(decl_string)

        return subelemtype_decl_list

    @property
    def has_optional(self):
        """Return true if structure has at least one optional field"""
        return self._impltype.has_optional

    @property
    def size_of_struct_length_field(self):
        return self._impltype.size_of_struct_length_field

    @property
    def is_tlv(self):
        return self._impltype.is_tlv

    @property
    def includes(self) -> List[str]:
        result = super().includes
        if self._impltype.size_of_struct_length_field > 0 and "<cstdint>" not in result:
            # <cstdint> is needed for LengthFieldType
            result += ["<cstdint>"]
            result = sorted(result)
        if self.is_tlv and "<set>" not in result:
            # <set> is needed for the return type of the required_fields method
            result += ["<set>"]
            result = sorted(result)
        if self.has_optional:
            # "ara/core/optional.h" is needed for optional fields
            result += ["\"ara/core/optional.h\""]
            result = sorted(result)
        return result




class EnumImplTypeView(ImplTypeView):
    """ImplTypeView with added data for enumeration type declarations"""
    def __init__(self, impltype) -> None:
        super().__init__(impltype)

    @property
    def literal_decl_list(self):
        """Get the list of literal declarations"""
        result = []
        for lit in self._impltype.literal_value_map:
            declaration = lit[0]
            if lit[1] is not None:
                declaration += ' = {}U'.format(lit[1])

            result.append(declaration)

        return result

    @property
    def literal_decl_key_list(self):
        """Get the list of literal declarations key"""
        result = []
        for lit in self._impltype.literal_value_map:
            declaration = lit[0]
            result.append(declaration)

        return result
    @property
    def element_typename(self):
        """Get the element typename in namespace format"""
        element_type_view = self.referred_views[0]
        assert element_type_view.underlying_model_type == self._impltype.data_type, "View misaligned with underlying type"
        return element_type_view.qualified_cpp_name

class MapImplTypeView(ImplTypeView):
    """ImplTypeView with added data for map type declarations"""

    def __init__(self, impltype) -> None:
        super().__init__(impltype)

    @property
    def keytype(self):
        key_type_view = self.referred_views[0]
        assert key_type_view.underlying_model_type == self._impltype.key_data_type, "View misaligned with underlying type"
        return key_type_view

    @property
    def valuetype(self):
        value_type_view = self.referred_views[1]
        assert value_type_view.underlying_model_type == self._impltype.value_data_type, "View misaligned with underlying type"
        return value_type_view


class AliasedImplTypeView(ImplTypeView):
    """ImplTypeView with added data for aliased implementation data types"""

    def __init__(self, impltype: ImplDataType) -> None:
        super().__init__(impltype)

    @property
    def aliasedtype(self):
        aliased_type_view = self.referred_views[0]
        return aliased_type_view.cpp_name_in_namespace(self.qualified_namespace)

class VectorImplTypeView(ImplTypeView):
    """ ImplTypeView with added data for vector element types"""

    def __init__(self, impltype) -> None:
        super().__init__(impltype)

    @property
    def element_type(self):
        if self.referred_views:
            return self.referred_views[0]
        else:
            return None


class VariantImplTypeView(ImplTypeView):

    def __init__(self, impltype) -> None:
        super().__init__(impltype)

    @property
    def sub_element_list(self):
        sub_element_list = []
        for item in self.element_data_type_list:
            sub_element_list.append(generator.views.model_to_view.ViewManipulations.to_view(item))
        return sub_element_list

    @property
    def element_type_list(self):
        if self.referred_views:
            return self.referred_views
        else:
            return None


class ArrayImplTypeView(VectorImplTypeView):
    """ ImplTypeView with added data for array element types"""

    def __init__(self, impltype) -> None:
        super().__init__(impltype)

    @property
    def arraysize(self):
        """Get the arraysize of the array"""
        return self._impltype.array_size

class AliasedBaseImplTypeView(AliasedImplTypeView):
    def __init__(self, impltype) -> None:
        super().__init__(impltype)

