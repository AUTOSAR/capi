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
import copy
import logging

from generator.common.tree_helper import get_element_or_die, get_element_or_none, to_list  # , short_name
from generator.intermediate_model.types.array_impl_type import ArrayImplDataType
from generator.intermediate_model.types.base_type import BaseType
from generator.intermediate_model.types.enum_impl_type import EnumImplDataType
from generator.intermediate_model.types.impl_type import ImplDataType
from generator.intermediate_model.types.linear_vector_impl_type import LinearVectorImplDataType
from generator.intermediate_model.types.map_impl_type import MapImplDataType
from generator.intermediate_model.types.string_impl_type import StringImplDataType
from generator.intermediate_model.types.structure_impl_type import StructureImplDataType
from .exception_handling import handle_method_exceptions
from ..intermediate_model.types.linear_variant_impl_type import LinearVariantImplDataType
from ..intermediate_model.types.string_fixed_impl_type import FixedStringImplDataType


class CppTypesParser:

    def __init__(self, arxml_tree, app_type_parser):
        self._log = logging.getLogger(__name__)
        self.arxml_tree = arxml_tree
        self._apptype_parser = app_type_parser
        # self._tp_to_si_elements = self._parse_transformation_props()
        self.trans_formation_props = None
        self.needVerifyTlv = True
        self.cppLoopOb = {}

    @staticmethod
    def get_literal_values(compumethod):
        """
        Takes a Compumethod of catogory TEXTTABLE generates a list over the
        literals and values for an enum
        """

        literals = list()
        compu_scales = compumethod.find_elements_of_type("COMPU-SCALE")
        for compuscale in compu_scales:
            # literal symbols could be named either by SYMBOL, COMPU-CONST/VT
            # or SHORT-LABEL. Order is as per SWS_CM_00425

            # @uptrace{SWS_CM_00425, 0000000000000000000000000000000000000000}
            # TODO: support for "suffix" mentioned in SWS_CM_00425 and then put fingerprint
            literal_name = get_element_or_none(compuscale, "SYMBOL")

            if literal_name is None:
                literal_name = get_element_or_none(compuscale, "COMPU-CONST/VT")

            if literal_name is None:
                literal_name = get_element_or_none(compuscale, "SHORT-LABEL")

            if literal_name is None:
                # @uptrace{SWS_CM_00426, 3d42bb2c579563826c4fa115be543ecc90707dad}
                assert False, "Unable to fetch enum value name for {}".format(compuscale.get_fqn())

            # Only set literalvalue if lowerlimit and upperlimit are equal and both have "closed" interval type.
            # Set to "absent" (i.e. None) if both lowerlimit and upperlimit are absent.
            # Otherwise remove the literal from the table because it fails into category "non-point" and shall be simply skipped.
            literal_value = None
            lower_limit = get_element_or_none(compuscale, "LOWER-LIMIT")
            upper_limit = get_element_or_none(compuscale, "UPPER-LIMIT")
            # @uptrace{SWS_CM_10376, 1607d268482d6087d892d68a8ab07563cfa9f698}
            if (lower_limit is None) and (upper_limit is None):
                # normal case, use the name and do not use the value.
                pass
            elif (lower_limit is not None) and (upper_limit is not None):
                if (lower_limit.get('INTERVAL-TYPE').upper() != "CLOSED"):
                    #non-point literal, flush it from table
                    literal_name = ""
                elif (upper_limit.get('INTERVAL-TYPE').upper() != "CLOSED"):
                    #non-point literal, flush it from table
                    literal_name = ""
                elif lower_limit != upper_limit:
                    #non-point literal, flush it from table
                    literal_name = ""
                else:
                    #everything is fine, pick up the value
                    literal_value = lower_limit
            else:
                #non-point literal, flush it from table
                literal_name = ""

            if literal_name != "":
                literals.append((literal_name, literal_value))

        return literals


    @staticmethod
    def parse_namespaces(namespaces):
        symbol_props = namespaces.find_elements_of_type("SYMBOL-PROPS")
        namespace_list = []

        for symbol_prop in symbol_props:
            if symbol_prop.SYMBOL:
                namespace_list.append(symbol_prop.SYMBOL)
            else:
                namespace_list.append(symbol_prop.SHORT_NAME)
        # return [x.lower() for x in namespace_list]
        return namespace_list

    def get_namespaces(self, cpp_type):
        result = []
        if hasattr(cpp_type, 'NAMESPACES'):
            namespaces = cpp_type.NAMESPACES
            if namespaces is not None:
                result = self.parse_namespaces(namespaces)
        return result

    def _get_size_of_struct_length_field(self, ar_struct_type):
        # Find the appropriate transformation props for this structure
        # and return with the size_of_struct_length field
        if self.trans_formation_props is not None:
            if self.trans_formation_props.date_ref is not None:
                ar_event_method_or_field = self.arxml_tree.find_referable(self.trans_formation_props.date_ref)
                typeRefList = []
                arguments = get_element_or_none(ar_event_method_or_field, "ARGUMENTS/ARGUMENT-DATA-PROTOTYPE")
                if arguments is not None:
                    for argument in arguments:
                        type_ref = get_element_or_none(argument, "TYPE-TREF")
                        if type_ref is not None:
                            typeRefList.append(type_ref)
                else:
                    type_ref = get_element_or_die(ar_event_method_or_field, "TYPE-TREF")
                    typeRefList.append(type_ref)
                for typeRef in typeRefList:
                    # The interface might reference an ApplicationDataType
                    # which needs to be resolved before comparing with the given ar_struct_type
                    type_kind = typeRef.get('DEST')
                    if type_kind.startswith('APPLICATION'):
                        app_type_ref = typeRef
                        ar_interface = typeRef.getparent().getparent()
                        typeRef = self._apptype_parser.get_cpp_from_app_type(app_type_ref, ar_interface)
                        assert typeRef is not None, \
                            "Could not resolve {0} {1}. The mapping to {2} is broken.".format(
                                type_kind, app_type_ref, "STD-CPP-IMPLEMENTATION-DATA-TYPE")
                    if ar_struct_type.get_fqn() == typeRef.text:
                        return self.trans_formation_props.sizeofstructLengthField if self.trans_formation_props.sizeofstructLengthField else 0
                    try:
                        self.judge_struct_type(typeRef, ar_struct_type.get_fqn())
                    except ValueError:
                        return self.trans_formation_props.sizeofstructLengthField if self.trans_formation_props.sizeofstructLengthField else 0
        return 0

    def judge_struct_type(self, type_ref, ar_struct_type_fqn):
        if ar_struct_type_fqn == type_ref.text:
            raise ValueError
        else:
            ar_type = self.arxml_tree.find_referable(type_ref.text)
            subelems = get_element_or_none(ar_type, "SUB-ELEMENTS/CPP-IMPLEMENTATION-DATA-TYPE-ELEMENT")
            if subelems is not None:
                subelems = to_list(subelems)
                for elem in subelems:
                    subelemtyperef = get_element_or_none(elem, "TYPE-REFERENCE/TYPE-REFERENCE-REF")
                    if subelemtyperef is not None:
                        self.judge_struct_type(subelemtyperef, ar_struct_type_fqn)
            else:
                template_args = get_element_or_none(ar_type, "TEMPLATE-ARGUMENTS/CPP-TEMPLATE-ARGUMENT")
                if template_args is not None:
                    for template_arg in template_args:
                        template_ref = get_element_or_none(template_arg, "TEMPLATE-TYPE-REF")
                        if template_ref is not None:
                            self.judge_struct_type(template_ref, ar_struct_type_fqn)

    def _get_tlv_data_id(self, ar_cpp_impl_data_type):
        if self.trans_formation_props is not None:
            for data_id in self.trans_formation_props.data_ids:
                if data_id.impl_type == ar_cpp_impl_data_type.get_fqn():
                    return data_id.id
        return None

    def _get_structure_cpp_type(self, ar_type, namespace, parent, newList):
        subelements = list()
        subelems = get_element_or_die(ar_type, "SUB-ELEMENTS/CPP-IMPLEMENTATION-DATA-TYPE-ELEMENT")
        data_id_counter = 0
        for elem in subelems:
            subelemtyperef = get_element_or_die(elem, "TYPE-REFERENCE/TYPE-REFERENCE-REF")
            ar_element_type = self.arxml_tree.find_referable(subelemtyperef.text)
            element_type = self.get_cpp_type(ar_element_type, False, parent, newList)
            ar_optional = get_element_or_none(elem, "IS-OPTIONAL")
            is_optional = ar_optional and ar_optional.text.upper() == "TRUE"
            data_id = self._get_tlv_data_id(elem)
            if data_id is not None:
                data_id_counter += 1
            subelement_tuple = ((str(elem.SHORT_NAME)), element_type, is_optional, data_id)
            subelements.append(subelement_tuple)

        structure_cpp_type = StructureImplDataType(ar_type.SHORT_NAME,
                                                   self._get_cpp_type_emitter(ar_type),
                                                   subelements,
                                                   namespace)

        structure_cpp_type.size_of_struct_length_field = self._get_size_of_struct_length_field(ar_type)

        if self.needVerifyTlv:
            # Check TLV data ids. All sub elements shall be tlv encoded if there is at least one member with data id.
            if len(subelems) != data_id_counter and data_id_counter != 0:
                raise AssertionError("All sub elements shall be referenced by a tlv data id. Location: " + ar_type.get_fqn())

            # Check length field size availability. The size of struct length field size is mandatory in case of TLV.
            if data_id_counter > 0 and structure_cpp_type.size_of_struct_length_field == 0:
                raise AssertionError("Size of struct length field size shall be configured for: " + ar_type.get_fqn())

            # Check data id availability if there is any optional member.
            if structure_cpp_type.has_optional and data_id_counter == 0:
                raise AssertionError("All sub elements shall be referenced by a tlv data id. Location: " + ar_type.get_fqn())

        return structure_cpp_type

    def _get_invalid_value(self, ar_type, real_type):

        if real_type is None:
            typename = "StringImplDataType"
            assert ar_type.CATEGORY == "STRING", "unexpected usage of invalid_value getter, type category = {}".format(ar_type.CATEGORY)
        else:
            typename = real_type.__class__.__name__

        result = None
        if typename == "BaseType":
            value = get_element_or_none(ar_type,"SW-DATA-DEF-PROPS/SW-DATA-DEF-PROPS-VARIANTS/SW-DATA-DEF-PROPS-CONDITIONAL/INVALID-VALUE/NUMERICAL-VALUE-SPECIFICATION/VALUE")
            if value is not None:
                result = value.text
        elif typename == "StringImplDataType":
            value = get_element_or_none(ar_type,"SW-DATA-DEF-PROPS/SW-DATA-DEF-PROPS-VARIANTS/SW-DATA-DEF-PROPS-CONDITIONAL/INVALID-VALUE/TEXT-VALUE-SPECIFICATION/VALUE")
            if value is not None:
                result = value.text
        else:
            self._log.debug("no support for InvalidValue for type %s", typename)

        return result

    @handle_method_exceptions(error_string_template="{exception_text}")
    def _get_typeref_cpp_type(self, ar_type, namespace, parent, newList):
        conditionalList = get_element_or_none(ar_type, "SW-DATA-DEF-PROPS/SW-DATA-DEF-PROPS-VARIANTS/SW-DATA-DEF-PROPS-CONDITIONAL")
        compumethod_ref = []
        if conditionalList is not None:
            conditionalList = to_list(conditionalList)
            for conditional in conditionalList:
                compu_method_ref = get_element_or_none(conditional, "COMPU-METHOD-REF")
                if compu_method_ref is not None:
                    compumethod_ref.append(compu_method_ref)
        if len(compumethod_ref) > 0:
            assert len(compumethod_ref) == 1, "[BASE] CODE-009: Only one table of values expected for {}. LocalPath:{}. ".format(ar_type.get_fqn(), ar_type.get_path())
            assert compumethod_ref[0].get('DEST') == 'COMPU-METHOD', "[BASE] CODE-009: SwDataDefPropsConditional.compuMethodRef must point to CompuMethod {}. LocalPath:{}. ".format(ar_type.get_fqn(), ar_type.get_path())
            compumethod = self.arxml_tree.find_referable(compumethod_ref[0].text)
            assert compumethod is not None, "[BASE] CODE-009: unable to resolve SwDataDefPropsConditional.compuMethodRef for {}. LocalPath:{}. ".format(ar_type.get_fqn(), ar_type.get_path())

            self._log.debug("compumethod NAME='%s' CATEGORY='%s'",compumethod.SHORT_NAME, compumethod.CATEGORY)
            categoryStr = get_element_or_none(compumethod, "CATEGORY")
            if categoryStr is None or categoryStr == "TEXTTABLE":
                literal_table = self.get_literal_values(compumethod)

                enum_type_ref = get_element_or_die(ar_type, "TYPE-REFERENCE-REF")
                assert enum_type_ref.get('DEST') == 'STD-CPP-IMPLEMENTATION-DATA-TYPE', "[BASE] CODE-009: StdCppImplementationDataType.typeReferenceRef must point to StdCppImplementationDataType {}. LocalPath:{}. ".format(ar_type.get_fqn(), ar_type.get_path())

                basetyperef = self.get_cpp_type(enum_type_ref, False, parent, newList)
                self._log.debug("enum CPP TYPE NAME='%s'", basetyperef.name)

                return EnumImplDataType(ar_type.SHORT_NAME,
                                        self._get_cpp_type_emitter(ar_type),
                                        basetyperef,
                                        literal_table,
                                        namespace)
        # just typedef
        typeref_cpptype_ref = get_element_or_die(ar_type, "TYPE-REFERENCE-REF")
        assert typeref_cpptype_ref.get('DEST') == 'STD-CPP-IMPLEMENTATION-DATA-TYPE', "[BASE] CODE-008: StdCppImplementationDataType.typeReferenceRef must point to StdCppImplementationDataType {}. LocalPath:{}. ".format(ar_type.get_fqn(), ar_type.get_path())

        typeref_cpp_type = self.get_cpp_type(typeref_cpptype_ref[0], False, parent, newList)
        self._log.debug("typeref cpp TYPE NAME='%s' typeref BASE_TYPE='%s'", ar_type.SHORT_NAME, typeref_cpp_type.name)

        return ImplDataType(ar_type.SHORT_NAME, self._get_cpp_type_emitter(ar_type),[typeref_cpp_type], namespace, self._get_invalid_value(ar_type, typeref_cpp_type.final_type))

    def _get_vector_cpp_type(self, ar_type, namespace, parent, newList):
        template_args = get_element_or_die(ar_type, "TEMPLATE-ARGUMENTS/CPP-TEMPLATE-ARGUMENT")
        assert len(template_args) == 1, "[BASE] CODE-007: One template argument expected for {}. LocalPath:{}. ".format(ar_type.get_fqn(), ar_type.get_path())
        element_type_ref = get_element_or_die(template_args[0], "TEMPLATE-TYPE-REF")
        ar_element_type = self.arxml_tree.find_referable(element_type_ref.text)
        element_type = self.get_cpp_type(ar_element_type, False, parent, newList)
        allocator = None
        return LinearVectorImplDataType(ar_type.SHORT_NAME,
                                        self._get_cpp_type_emitter(ar_type),
                                        element_type,
                                        allocator,
                                        namespace)

    def _get_variant_cpp_type(self, ar_type, namespace, parent, newList):
        template_args = get_element_or_die(ar_type, "TEMPLATE-ARGUMENTS/CPP-TEMPLATE-ARGUMENT")
        element_type_list = []
        for template_arg in template_args:
            element_type_ref = get_element_or_die(template_arg, "TEMPLATE-TYPE-REF")
            ar_element_type = self.arxml_tree.find_referable(element_type_ref.text)
            element_type = self.get_cpp_type(ar_element_type, False, parent, newList)
            element_type_list.append(element_type)
        return LinearVariantImplDataType(ar_type.SHORT_NAME, self._get_cpp_type_emitter(ar_type), element_type_list, namespace)

    def _get_array_cpp_type(self, ar_type, namespace, parent, newList):
        array_size = get_element_or_die(ar_type, "ARRAY-SIZE")
        template_args = get_element_or_die(ar_type, "TEMPLATE-ARGUMENTS/CPP-TEMPLATE-ARGUMENT")
        assert len(template_args) == 1, "[BASE] CODE-006: One template argument expected for {}. LocalPath:{}. ".format(ar_type.get_fqn(), ar_type.get_path())
        element_type_ref = get_element_or_die(template_args[0], "TEMPLATE-TYPE-REF")
        ar_element_type = self.arxml_tree.find_referable(element_type_ref.text)
        element_type = self.get_cpp_type(ar_element_type, False, parent, newList)
        return ArrayImplDataType(ar_type.SHORT_NAME,
                                 self._get_cpp_type_emitter(ar_type),
                                 element_type,
                                 array_size,
                                 namespace)

    def _get_map_cpp_type(self, ar_type, namespace, parent, newList):
        template_args = get_element_or_die(ar_type, "TEMPLATE-ARGUMENTS/CPP-TEMPLATE-ARGUMENT")
        assert len(template_args) == 2, "[Code 50]Two template arguments expected for {}. LocalPath:{}. ".format(ar_type.get_fqn(), ar_type.get_path())
        key_type_ref   = get_element_or_die(template_args[0], "TEMPLATE-TYPE-REF")
        value_type_ref = get_element_or_die(template_args[1], "TEMPLATE-TYPE-REF")
        ar_key_type   = self.arxml_tree.find_referable(key_type_ref.text)
        ar_value_type = self.arxml_tree.find_referable(value_type_ref.text)
        key_type = self.get_cpp_type(ar_key_type, False, parent, newList)
        value_type = self.get_cpp_type(ar_value_type, False, parent, newList)
        return MapImplDataType(ar_type.SHORT_NAME,
                               self._get_cpp_type_emitter(ar_type),
                               key_type,
                               value_type,
                               namespace)

    def _get_cpp_type_emitter(self, ar_type):
        type_emitter = ""
        if hasattr(ar_type, "TYPE_EMITTER"):
            type_emitter = str(ar_type.TYPE_EMITTER).strip()
        return type_emitter

    def _get_string_type(self, ar_type, namespace, parent, newList):
        return StringImplDataType(
            ar_type.SHORT_NAME,
            self._get_cpp_type_emitter(ar_type),
            1,
            namespace,
            self._get_invalid_value(ar_type, None)
        )

    def _get_fixed_string_type(self, ar_type, namespace, parent, newList):
        array_size = get_element_or_die(ar_type, "ARRAY-SIZE")
        return FixedStringImplDataType(
            ar_type.SHORT_NAME,
            self._get_cpp_type_emitter(ar_type),
            array_size,
            namespace
        )

    def _get_value_type(self, ar_type, namespace, parent, newList):
        template_args = get_element_or_none(ar_type, "TEMPLATE-ARGUMENTS/CPP-TEMPLATE-ARGUMENT")
        if template_args is not None:
            type_emitter = self._get_cpp_type_emitter(ar_type)
            if type_emitter == "":
                type_emitter = "TYPE_EMITTER_ARA"
            element_type_ref = get_element_or_die(template_args[0], "TEMPLATE-TYPE-REF")
            typeref_cpp_type = self.get_cpp_type(element_type_ref, False, parent, newList)
            return ImplDataType(ar_type.SHORT_NAME,
                                type_emitter,
                                [typeref_cpp_type],
                                namespace,
                                self._get_invalid_value(ar_type, typeref_cpp_type.final_type))
        else:
            return BaseType(
                ar_type.SHORT_NAME,
                self._get_cpp_type_emitter(ar_type)
            )

    def set_trans_formation_props(self, trans_formation_props):
        self.trans_formation_props = trans_formation_props

    def set_need_verify_tlv(self, needVerifyTlv):
        self.needVerifyTlv = needVerifyTlv

    def get_cpp_type(self, ar_type_ref, isFirst=False, parent=None, errorFqnList=None):
        typeRef = str(ar_type_ref)
        ar_type = self.arxml_tree.find_referable(str(ar_type_ref))
        newList = copy.copy(errorFqnList)
        if not isFirst:
            if typeRef in errorFqnList:
                errorFqnList.append(typeRef)
                assert False, "[BASE] CODE-010: Data type refers to an infinite loop, circular relation [{}]. LocalPath:{}. ".format(", ".join(errorFqnList), parent.get_path())
            newList.append(typeRef)
        else:
            parent = ar_type
            newList = [typeRef]
        namespaces = self.get_namespaces(ar_type)
        if ar_type.CATEGORY == 'STRUCTURE':
            return self._get_structure_cpp_type(ar_type, namespaces, parent, newList)
        elif ar_type.CATEGORY == 'TYPE_REFERENCE':
            return self._get_typeref_cpp_type(ar_type, namespaces, parent, newList)
        elif ar_type.CATEGORY == 'VECTOR':
            return self._get_vector_cpp_type(ar_type, namespaces, parent, newList)
        elif ar_type.CATEGORY == 'VARIANT':
            return self._get_variant_cpp_type(ar_type, namespaces, parent, newList)
        elif ar_type.CATEGORY == 'ARRAY':
            return self._get_array_cpp_type(ar_type, namespaces, parent, newList)
        elif ar_type.CATEGORY == 'ASSOCIATIVE_MAP':
            return self._get_map_cpp_type(ar_type, namespaces, parent, newList)
        elif ar_type.CATEGORY == 'STRING':
            return self._get_string_type(ar_type, namespaces, parent, newList)
        elif ar_type.CATEGORY == 'FIXED_STRING':
            return self._get_fixed_string_type(ar_type, namespaces, parent, newList)
        elif ar_type.CATEGORY == 'VALUE':
            return self._get_value_type(ar_type, namespaces, parent, newList)
        else:
            assert False, "[BASE] CODE-004: Unknown type category. {} must in ['STRUCTURE', 'TYPE_REFERENCE', 'VECTOR', 'VARIANT', 'ARRAY', 'ASSOCIATIVE_MAP', 'STRING', 'FIXED_STRING', 'VALUE'].FQN:{},LocalPath={}".format(ar_type.CATEGORY, str(ar_type_ref), ar_type.get_path())

