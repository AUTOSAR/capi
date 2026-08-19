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
# pylint: disable=too-many-lines,too-many-public-methods
import logging
from generator.intermediate_model.communication_management.event import Event
from generator.intermediate_model.communication_management.method import Method
from generator.intermediate_model.communication_management.method import \
    Argument
from generator.intermediate_model.communication_management.method import \
    ErrorContext
# pylint: disable=line-too-long
from generator.intermediate_model.communication_management.error_domain import ApError, ApErrorDomain
from generator.intermediate_model.communication_management.application_error_exception import ApplicationErrorException  # noqa
from generator.intermediate_model.communication_management.field import Field
from generator.intermediate_model.communication_management.transformationprops import TransformationProps
# pylint: enable=line-too-long
from generator.views.error_domain_view import ErrorDomainView
from generator.common.tree_helper import get_element_or_none, to_str, transition_number, get_element_or_die, to_list

from .app_types import AppTypesParser
from .init_value import InitValueClass
from .cpp_impltypes import CppTypesParser

# Datatype tags
from ..common.autosar_mapping import AUTOSARMapping
from ..intermediate_model.communication_management.tld_data_id_definition import TlvDataIdDefinition

_A_DATA_PROT = "ARGUMENT-DATA-PROTOTYPE"
_TYPE_TREF = "TYPE-TREF"
_V_DATA_PROT = "VARIABLE-DATA-PROTOTYPE"

# Interface tags
_APPL_ERR = "APPLICATION-ERROR"
_ARGS = "ARGUMENTS"
_CS_OPER = "CLIENT-SERVER-OPERATION"
_ERR_CON_REF = "ERROR-CONTEXT-REF"
_ERR_CON_REFS = "ERROR-CONTEXT-REFS"
_EVENTS = "EVENTS"
_FIELDS = "FIELDS"
_FIELD = "FIELD"
_GET = "GET"
_METHODS = "METHODS"
_NOTIFIER = "NOTIFIER"
_SET = "SET"


def _is_true(text):
    """Check if text equals true, case insensitive"""
    return text is not None and str(text).strip().lower() == 'true'


class InterfaceBuilder:

    def __init__(self, model, package_path_fallback):
        self._log = logging.getLogger(__name__)
        self.model = model
        self._package_path_fallback = package_path_fallback
        self._apptype_parser = AppTypesParser(model)
        self._cpptype_parser = CppTypesParser(model, self._apptype_parser)
        self._error_domains = self.get_error_domains()
        self._error_domain_views = self._error_domains_to_views(self._error_domains)
        self._initValue = InitValueClass(model)
        self.dataIdOb = {}

    def _error_domains_to_views(self, error_domains):
        # just put to views, no meaningful processing
        # needed for uniformity with current architecture
        return [ErrorDomainView(ed) for ed in error_domains]

    def _get_service_namespaces(self, ar_interface):
        """Extract the service namespaces from a service-interface ar-object"""
        log = logging.getLogger(__name__)

        if hasattr(ar_interface, 'NAMESPACES'):
            namespaces = ar_interface.NAMESPACES
            if namespaces is not None:
                return CppTypesParser.parse_namespaces(namespaces)
        else:
            if self._package_path_fallback:
                log.warning("No namespaces were found in the following interface %s! Fallback to Packages hierarchy",
                            ar_interface.SHORT_NAME)
                return ar_interface.get_parent_package_hierarchy_as_list()
            else:
                log.warning("No namespaces were found in the following interface %s!",
                            ar_interface.SHORT_NAME)
                return []

    def resolve_type_ref(self, ar_ref, ar_interface, trans_formation_props=None, needVerifyTlv=True):
        type_kind = ar_ref.get('DEST')

        if type_kind.startswith('APPLICATION'):
            app_type_ref = ar_ref
            ar_ref = self._apptype_parser.get_cpp_from_app_type(app_type_ref, ar_interface)
            assert ar_ref is not None, \
                "Could not resolve {0} {1}. The mapping to {2} is broken.".format(
                type_kind, app_type_ref, "STD-CPP-IMPLEMENTATION-DATA-TYPE")
            type_kind = ar_ref.get('DEST')

        if type_kind == 'STD-CPP-IMPLEMENTATION-DATA-TYPE':
            self._cpptype_parser.set_trans_formation_props(trans_formation_props)
            self._cpptype_parser.set_need_verify_tlv(needVerifyTlv)
            result = self._cpptype_parser.get_cpp_type(ar_ref, isFirst=True)
        elif type_kind == "IMPLEMENTATION-DATA-TYPE":
            pass
        else:
            assert False, "[PER ] CODE-003: PersistencyKeyValueStorageInterface.dataTypeForSerializations reference type must be 'StdCppImplementationDataType' or 'ImplementationDataType'. FQN:{}. LocalPath:{}. ".format(ar_interface.get_fqn(), ar_interface.get_path())

        return result

    def get_type(self, ar_entity, ar_interface, trans_formation_props, needVerifyTlv):
        ar_type_refs = ar_entity.find_elements_of_type(_TYPE_TREF)
        assert len(ar_type_refs) == 1, "Exactly one {0} expected for {1}, found {2}".format(
            _TYPE_TREF, ar_entity.get_fqn(), len(ar_type_refs))

        return self.resolve_type_ref(ar_type_refs[0], ar_interface, trans_formation_props, needVerifyTlv)

    @staticmethod
    def _arg_is_an_exception(arg_fqn, service):
        for err in service.errors:
            for ctx in err.error_contexts:
                if ctx.referable_fqn == arg_fqn:
                    return True
        return False

    def get_method_arguments(self, ar_method, method, service, ar_interface, trans_formation_props, needVerifyTlv):
        """Get the method arguments"""
        arguments = get_element_or_none(ar_method, _ARGS)
        if arguments is None:
            return
        ar_args = ar_method.find_elements_of_type(_A_DATA_PROT)
        output = []
        for ar_arg in ar_args:
            arg_impl_type = self.get_type(ar_arg, ar_interface, trans_formation_props, needVerifyTlv)
            arg_direction = ar_arg.DIRECTION.lower()
            arg_is_exception = InterfaceBuilder._arg_is_an_exception(str(ar_arg), service)
            argument = Argument(ar_arg.SHORT_NAME, arg_impl_type, arg_is_exception)
            method.add_argument(argument, arg_direction)

    def find_error(self, error_fqn):
        for error_domain in self._error_domains:
            for error in error_domain.errors:
                if error.fqn == error_fqn:
                    return error
        return None

    def get_method_errors(self, ar_method, method):
        ar_error_refs = get_element_or_none(ar_method, "POSSIBLE-AP-ERROR-REFS/POSSIBLE-AP-ERROR-REF")
        if ar_error_refs is not None:
            for ar_error_ref in ar_error_refs:
                ar_error = self.model.find_referable(ar_error_ref.text)
                error = self.find_error(ar_error.get_fqn())
                method.add_error(error)

        ar_error_set_refs = get_element_or_none(ar_method, "POSSIBLE-AP-ERROR-SET-REFS/POSSIBLE-AP-ERROR-SET-REF")
        if ar_error_set_refs is not None:
            for ar_error_set_ref in ar_error_set_refs:
                ar_error_set = self.model.find_referable(ar_error_set_ref.text)

                ar_error_refs = get_element_or_none(ar_error_set, "AP-APPLICATION-ERROR-REFS/AP-APPLICATION-ERROR-REF")
                if ar_error_refs is not None:
                    for ar_error_ref in ar_error_refs:
                        ar_error = self.model.find_referable(ar_error_ref.text)
                        error = self.find_error(ar_error.get_fqn())
                        method.add_error(error)

    def _get_error_contexts(self, ar_appl_err):
        """Get the error contexts of an application error"""
        ar_err_cont_refs_cont = get_element_or_none(ar_appl_err, _ERR_CON_REFS)
        if ar_err_cont_refs_cont is None:
            return []
        ar_err_cont_refs = ar_err_cont_refs_cont.find_elements_of_type(
            _ERR_CON_REF, DEST=_A_DATA_PROT)

        error_contexts = []
        for ar_error_ref in ar_err_cont_refs:
            ar_error_cont = self.model.find_referable(str(ar_error_ref))
            ar_type_refs = ar_error_cont.find_elements_of_type(
                _TYPE_TREF, DEST="STD-CPP-IMPLEMENTATION-DATA-TYPE")
            if len(ar_type_refs) == 1:
                impl_type = self._cpptype_parser.get_cpp_type(ar_type_refs[0], isFirst=True)
                error_contexts.append(ErrorContext(impl_type, str(ar_error_ref)))
            else:
                # ar_type_refs = ar_error_cont.find_elements_of_type(
                #     _TYPE_TREF, DEST="IMPLEMENTATION-DATA-TYPE")
                # impl_type = self._impltype_parser.get_impl_type(ar_type_refs[0])
                pass
        return error_contexts

    def get_events(self, ar_interface, needVerifyTlv):
        """Get the events contained in an interface"""
        if_events = get_element_or_none(ar_interface, _EVENTS)
        if if_events is None:
            return []
        ar_events = if_events.find_elements_of_type(_V_DATA_PROT)
        events = [Event(ar_event.SHORT_NAME) for ar_event in ar_events]
        for ar_event, event in zip(ar_events, events):
            trans_formation_props = self.get_transformationprops(ar_event.get_fqn(), "event")
            event.impl_type = self.get_type(ar_event, ar_interface, trans_formation_props, needVerifyTlv)
            event.set_transformationprops(trans_formation_props)
        return events

    def get_fields(self, ar_interface, portItem, needVerifyTlv):
        """Get the fields of a service interface"""
        if_fields = get_element_or_none(ar_interface, _FIELDS)
        if if_fields is None:
            return []
        ar_fields = if_fields.find_elements_of_type(_FIELD)
        fields = [Field(ar_field.SHORT_NAME) for ar_field in ar_fields]
        for ar_field, field in zip(ar_fields, fields):
            trans_formation_props = self.get_transformationprops(ar_field.get_fqn(), "field")
            field.impl_type = self.get_type(ar_field, ar_interface, trans_formation_props, needVerifyTlv)
            field.init_value = self._initValue.get_init_value(ar_field.get_fqn(), portItem, field.impl_type)
            field.has_notifier = _is_true(ar_field.HAS_NOTIFIER)
            field.has_getter = _is_true(ar_field.HAS_GETTER)
            field.has_setter = _is_true(ar_field.HAS_SETTER)
            field.set_transformationprops(trans_formation_props)
        return fields

    def get_methods(self, ar_interface, service, needVerifyTlv):
        """Get the methods of a service interface"""
        if_methods = get_element_or_none(ar_interface, _METHODS)
        if if_methods is None:
            return []
        ar_methods = if_methods.find_elements_of_type(_CS_OPER)
        methods = [Method(ar_method.SHORT_NAME) for ar_method in ar_methods]
        for ar_method, method in zip(ar_methods, methods):
            trans_formation_props = self.get_transformationprops(ar_method.get_fqn(), "method")
            self.get_method_arguments(ar_method, method, service, ar_interface, trans_formation_props, needVerifyTlv)
            self.get_method_errors(ar_method, method)
            method.is_fire_and_forget = _is_true(get_element_or_none(ar_method, "FIRE-AND-FORGET"))
            method.set_transformationprops(trans_formation_props)
            sdg = get_element_or_none(ar_method, "ADMIN-DATA/SDGS/SDG")
            if sdg is not None and sdg.attrib["GID"] == "iSOFT:com:Output":
                sd = get_element_or_none(sdg, "SD")
                if sd is not None and sd.attrib["GID"] == "iSOFT:com:Output:AsArgs":
                    method.isArgsTag = sd
        return methods

    def get_transformationprops(self, fqn, typeName):
        transformationProps = None
        transformation_props = self.model.find_elements_of_type("SOMEIP-DATA-PROTOTYPE-TRANSFORMATION-PROPS")
        for item in transformation_props:
            someip_transformation_props_ref = get_element_or_none(item, "SOMEIP-TRANSFORMATION-PROPS-REF")
            isYes = False
            if someip_transformation_props_ref is not None:
                elementInImplDatatypeMapping = item.find_elements_of_type("DATA-PROTOTYPE-IREF")
                for elementInImplDatatype in elementInImplDatatypeMapping:
                    data_ref = get_element_or_none(elementInImplDatatype, "TARGET-DATA-PROTOTYPE-REF")
                    if data_ref is not None:
                        if typeName == "field" and data_ref.attrib["DEST"] == "FIELD" and data_ref.text == fqn:
                            isYes = True
                            break
                        elif typeName == "event" and data_ref.attrib["DEST"] == "VARIABLE-DATA-PROTOTYPE" and data_ref.text == fqn:
                            isYes = True
                            break
                        elif typeName == "method" and data_ref.attrib["DEST"] == "ARGUMENT-DATA-PROTOTYPE":
                            data_short_ref = "/".join(str(data_ref.text).split("/")[:-1])
                            if data_short_ref == fqn:
                                isYes = True
                                break
                if isYes:
                    transformationProps = self.get_transformationprops_sub(someip_transformation_props_ref, transformationProps, fqn)
                    thisMapping = None
                    ar_transformationpropsmappings = self.model.find_elements_of_type("TRANSFORMATION-PROPS-TO-SERVICE-INTERFACE-ELEMENT-MAPPING")
                    for ar_transformationpropsmapping in ar_transformationpropsmappings:
                        if thisMapping is None:
                            if typeName == "field":
                                friedrefs = ar_transformationpropsmapping.find_elements_of_type("FIELD-REF")
                                for friedref in friedrefs:
                                    if fqn == friedref.text:
                                        thisMapping = ar_transformationpropsmapping
                                        break
                            if typeName == "method":
                                methodrefs = ar_transformationpropsmapping.find_elements_of_type("METHOD-REF")
                                for methodref in methodrefs:
                                    if fqn == methodref.text:
                                        thisMapping = ar_transformationpropsmapping
                                        break
                            if typeName == "event":
                                eventrefs = ar_transformationpropsmapping.find_elements_of_type("EVENT-REF")
                                for eventref in eventrefs:
                                    if fqn == eventref.text:
                                        thisMapping = ar_transformationpropsmapping
                                        break
                        else:
                            break
                    if thisMapping is not None:
                        if len(transformationProps.data_ids) == 0:
                            self.get_data_ids(thisMapping, transformationProps)
                        if transformationProps.byteorder == "":
                            transformation_props_ref = get_element_or_none(thisMapping, "TRANSFORMATION-PROPS-REF")
                            if transformation_props_ref is not None:
                                self.get_transformationprops_byte_order(transformation_props_ref, transformationProps)
                        if transformationProps.sizeofstructLengthField is None:
                            transformation_props_ref = get_element_or_none(thisMapping, "TRANSFORMATION-PROPS-REF")
                            if transformation_props_ref is not None:
                                self.get_transformationprops_sizeofstructLengthField(transformation_props_ref, transformationProps)

        if transformationProps is None:
            ar_transformationpropsmappings = self.model.find_elements_of_type("TRANSFORMATION-PROPS-TO-SERVICE-INTERFACE-ELEMENT-MAPPING")
            for ar_transformationpropsmapping in ar_transformationpropsmappings:
                if transformationProps is None:
                    transformationpropsref = get_element_or_none(ar_transformationpropsmapping, "TRANSFORMATION-PROPS-REF")
                    if typeName == "field":
                        friedrefs = ar_transformationpropsmapping.find_elements_of_type("FIELD-REF")
                        for friedref in friedrefs:
                            if fqn == friedref.text:
                                transformationProps = self.get_transformationprops_sub(transformationpropsref, transformationProps, fqn)
                                self.get_data_ids(ar_transformationpropsmapping, transformationProps)
                                break
                    if typeName == "method":
                        methodrefs = ar_transformationpropsmapping.find_elements_of_type("METHOD-REF")
                        for methodref in methodrefs:
                            if fqn == methodref.text:
                                transformationProps = self.get_transformationprops_sub(transformationpropsref, transformationProps, fqn)
                                self.get_data_ids(ar_transformationpropsmapping, transformationProps)
                                break
                    if typeName == "event":
                        eventrefs = ar_transformationpropsmapping.find_elements_of_type("EVENT-REF")
                        for eventref in eventrefs:
                            if fqn == eventref.text:
                                transformationProps = self.get_transformationprops_sub(transformationpropsref, transformationProps, fqn)
                                self.get_data_ids(ar_transformationpropsmapping, transformationProps)
                                break
                else:
                    break

        return transformationProps

    def get_data_ids(self, ar_transformationpropsmapping, transformationProps):
        fqn = ar_transformationpropsmapping.get_fqn()
        if fqn not in self.dataIdOb:
            data_ids = self._get_data_ids(ar_transformationpropsmapping)
            self.dataIdOb[fqn] = data_ids
        transformationProps.data_ids = self.dataIdOb[fqn]

    def get_transformationprops_byte_order(self, transformationPropsRef, transformationProps):
        if transformationPropsRef is not None:
            ar_apsomeiptransformationprops = self.model.find_referable(str(transformationPropsRef))
            byteorder = get_element_or_none(ar_apsomeiptransformationprops, "BYTE-ORDER")
            if byteorder is not None:
                transformationProps.set_byteorder(byteorder.text)

    def get_transformationprops_sizeofstructLengthField(self, transformationPropsRef, transformationProps):
        if transformationPropsRef is not None:
            ar_apsomeiptransformationprops = self.model.find_referable(str(transformationPropsRef))
            sizeofstructLengthField = get_element_or_none(ar_apsomeiptransformationprops, "SIZE-OF-STRUCT-LENGTH-FIELD")
            if sizeofstructLengthField is not None:
                if sizeofstructLengthField not in [0, 1, 2, 4]:
                    assert False, "[BASE] CODE-002: ApSomeipTransformationProps.sizeOfStructLengthField shall be either 0,1,2 or 4."
                transformationProps.set_sizeofstructLengthField(sizeofstructLengthField)

    def get_transformationprops_sub(self, transformationPropsRef, transformationProps, date_ref):
        if transformationPropsRef:
            if transformationProps is None:
                transformationProps = TransformationProps()
                transformationProps.date_ref = date_ref
            ar_apsomeiptransformationprops = self.model.find_referable(str(transformationPropsRef))
            byteorder = get_element_or_none(ar_apsomeiptransformationprops, "BYTE-ORDER")
            sessionhandling = get_element_or_none(ar_apsomeiptransformationprops, "SESSION-HANDLING")
            alignment = get_element_or_none(ar_apsomeiptransformationprops, "ALIGNMENT")
            implementslegacyStringSerialization = get_element_or_none(ar_apsomeiptransformationprops, "IMPLEMENTS-LEGACY-STRING-SERIALIZATION")
            isDynamicLengthFieldsize = get_element_or_none(ar_apsomeiptransformationprops, "IS-DYNAMIC-LENGTH-FIELD-SIZE")
            sizeofArrayLengthField = get_element_or_none(ar_apsomeiptransformationprops, "SIZE-OF-ARRAY-LENGTH-FIELD")
            sizeofstringLengthField = get_element_or_none(ar_apsomeiptransformationprops, "SIZE-OF-STRING-LENGTH-FIELD")
            sizeofstructLengthField = get_element_or_none(ar_apsomeiptransformationprops, "SIZE-OF-STRUCT-LENGTH-FIELD")
            sizeofUnionLengthField = get_element_or_none(ar_apsomeiptransformationprops, "SIZE-OF-UNION-LENGTH-FIELD")
            sizeofUnionTypeselectorField = get_element_or_none(ar_apsomeiptransformationprops, "SIZE-OF-UNION-TYPE-SELECTOR-FIELD")
            stringEncoding = get_element_or_none(ar_apsomeiptransformationprops, "STRING-ENCODING")

            sdList = get_element_or_none(ar_apsomeiptransformationprops, "ADMIN-DATA/SDGS/SDG/SD")
            sdList = to_list(sdList)
            for item in sdList:
                if item.attrib["GID"] == "iSOFT:com:ApSomeipTransformationProps:SizeOfFixedArrayLengthField":
                    transformationProps.set_sizeofFixedArrayLengthField(item)
                elif item.attrib["GID"] == "iSOFT:com:ApSomeipTransformationProps:SizeOfFixedStringLengthField":
                    transformationProps.set_sizeofFixedStringLengthField(item)

            if sessionhandling is not None:
                transformationProps.set_sessionhandling(sessionhandling.text)
            if byteorder is not None:
                transformationProps.set_byteorder(byteorder.text)
            if alignment is not None:
                if alignment not in [8, 16, 32, 64, 128, 256]:
                    assert False, "ApSomeipTransformationProps.alignment shall be either 8,16,32,64,128,or 256."
                transformationProps.set_alignment(alignment)
            if implementslegacyStringSerialization is not None:
                transformationProps.set_implementslegacyStringSerialization(implementslegacyStringSerialization.text)
            if isDynamicLengthFieldsize is not None:
                transformationProps.set_isDynamicLengthFieldsize(isDynamicLengthFieldsize.text)
            if sizeofArrayLengthField is not None:
                if sizeofArrayLengthField not in [0,1,2,4]:
                    assert False, "ApSomeipTransformationProps.sizeOfArrayLengthField shall be either 0,1,2 or 4."
                transformationProps.set_sizeofArrayLengthField(sizeofArrayLengthField)
            if sizeofstringLengthField is not None:
                if sizeofstringLengthField not in [0,1,2,4]:
                    assert False, "[BASE] CODE-002: ApSomeipTransformationProps.sizeOfStringLengthField shall be either 0,1,2 or 4."
                transformationProps.set_sizeofstringLengthField(sizeofstringLengthField)
            if sizeofstructLengthField is not None:
                if sizeofstructLengthField not in [0,1,2,4]:
                    assert False, "[BASE] CODE-002: ApSomeipTransformationProps.sizeOfStructLengthField shall be either 0,1,2 or 4."
                transformationProps.set_sizeofstructLengthField(sizeofstructLengthField)
            if sizeofUnionLengthField is not None:
                if sizeofUnionLengthField not in [0,1,2,4]:
                    assert False, "[BASE] CODE-002: ApSomeipTransformationProps.sizeOfUnionLengthField shall be either 0,1,2 or 4."
                transformationProps.set_sizeofUnionLengthField(sizeofUnionLengthField)
            if sizeofUnionTypeselectorField is not None:
                if sizeofUnionTypeselectorField not in [1,2,4]:
                    assert False, "[BASE] CODE-002: ApSomeipTransformationProps.sizeOfUnionTypeSelectorField shall be either 1,2 or 4."
                transformationProps.set_sizeofUnionTypeselectorField(sizeofUnionTypeselectorField)
            if stringEncoding is not None:
                transformationProps.set_stringEncoding(stringEncoding.text.replace("-", ""))
        return transformationProps

    def _get_data_ids(self, ar_entity: AUTOSARMapping):
        tlv_data_id_definition_set_refs = get_element_or_none(ar_entity, "TLV-DATA-ID-DEFINITION-REFS/TLV-DATA-ID-DEFINITION-REF")
        if tlv_data_id_definition_set_refs is None:
            return []

        ar_id_defs = []
        for tlv_data_id_definition_set_ref in tlv_data_id_definition_set_refs:
            tlv_data_id_definition_set = self.model.find_referable(str(tlv_data_id_definition_set_ref))
            ar_id_defs.extend(get_element_or_none(tlv_data_id_definition_set, "TLV-DATA-ID-DEFINITIONS/TLV-DATA-ID-DEFINITION"))

        tlv_id_defs = [TlvDataIdDefinition(ar_id_def.ID) for ar_id_def in ar_id_defs]
        for ar_def, tlv_def in zip(ar_id_defs, tlv_id_defs):
            tlv_def.impl_type = get_element_or_die(ar_def, "TLV-SUB-ELEMENT-REF")

        return tlv_id_defs

    def get_errors(self, ar_interface):
        """Get the errors of a service interface"""
        ar_appl_errors = ar_interface.find_elements_of_type(_APPL_ERR)
        appl_errors = []
        for ar_appl_err in ar_appl_errors:
            error_contexts = self._get_error_contexts(ar_appl_err)
            appl_errors.append(ApplicationErrorException(
                ar_appl_err.get_fqn(),
                ar_appl_err.ERROR_CODE, error_contexts))
        return appl_errors

    def get_error_domains(self):
        domains = {}
        ar_error_domains = self.model.find_elements_of_type("AP-APPLICATION-ERROR-DOMAIN")
        for ar_error_domain in ar_error_domains:
            namespaces = self._cpptype_parser.get_namespaces(ar_error_domain)
            value = ar_error_domain.VALUE
            value =transition_number(value,change=16)
            fqn = ar_error_domain.get_fqn()
            error_domain = ApErrorDomain(fqn, namespaces, value)
            domains[fqn] = error_domain

        ar_errors = self.model.find_elements_of_type("AP-APPLICATION-ERROR")
        for ar_error in ar_errors:
            domain_ref = ar_error.ERROR_DOMAIN_REF
            fqn = ar_error.get_fqn()
            error_code = ar_error.ERROR_CODE
            desc = to_str(get_element_or_none(ar_error, "DESC/L-2"))
            error = ApError(fqn, desc, error_code)
            corresponding_domain = domains[domain_ref]
            corresponding_domain.add_error(error)

        return list(domains.values())

    def get_error_domain_views(self):
        return self._error_domain_views

    def populate_interface(self, ar_interface, service, portItem=None, needVerifyTlv=True):
        service.namespaces = self._get_service_namespaces(ar_interface)
        service.lowNamespaces =  [x.lower() for x in service.namespaces]
        for error in self.get_errors(ar_interface):
            service.add_error(error)
        for event in self.get_events(ar_interface, needVerifyTlv):
            service.add_event(event)
        for field in self.get_fields(ar_interface, portItem, needVerifyTlv):
            service.add_field(field)
        for method in self.get_methods(ar_interface, service, needVerifyTlv):
            service.add_method(method)
        service.major_version = transition_number(get_element_or_none(ar_interface, "MAJOR-VERSION"),defaultInt=0)
        service.minor_version = transition_number(get_element_or_none(ar_interface, "MINOR-VERSION"),defaultInt=0)
        sdg = get_element_or_none(ar_interface, "ADMIN-DATA/SDGS/SDG")
        if sdg is not None and sdg.attrib["GID"] == "iSOFT:com:CommunicationGroup":
            sd = get_element_or_none(sdg, "SD")
            if sd is not None and sd.attrib["GID"] == "iSOFT:com:CommunicationGroup:GroupName":
                service.cg_sign = sd.text


