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
from generator.common.tree_helper import get_element_or_die

_PTD_MAPPINGS = "PORT-INTERFACE-TO-DATA-TYPE-MAPPING"


class AppTypesParser:

    def __init__(self, arxml_tree):
        self._log = logging.getLogger(__name__)
        self.arxml_tree = arxml_tree
        self._app_to_cpp_mappings = {}

    def _validate_app_primitive_type(self, ar_app_data_type, ar_cpp_type, mappings):
        cpp_category = get_element_or_die(ar_cpp_type, "CATEGORY")
        if cpp_category not in ["STRING", "VALUE"]:
            return False
        return True

    def _validate_app_array_type(self, ar_app_data_type, ar_cpp_type, mappings):
        cpp_category = get_element_or_die(ar_cpp_type, "CATEGORY")
        app_array_element = get_element_or_die(ar_app_data_type, "ELEMENT")

        app_dynamic_array_size_profile = ar_app_data_type.find_elements_of_type("DYNAMIC-ARRAY-SIZE-PROFILE")
        if len(app_dynamic_array_size_profile) > 0 and app_dynamic_array_size_profile[0] == "VSA_LINEAR":
            if cpp_category != "VECTOR":
                return False
        else:
            if cpp_category != "ARRAY":
                return False

            app_array_element_max_num_elements = get_element_or_die(app_array_element, "MAX-NUMBER-OF-ELEMENTS")
            cpp_template_argument = get_element_or_die(ar_cpp_type, "ARRAY-SIZE")
            if app_array_element_max_num_elements != cpp_template_argument:
                return False

        app_array_element_type_ref = get_element_or_die(app_array_element, "TYPE-TREF")
        app_array_element_type = self.arxml_tree.find_referable(str(app_array_element_type_ref))

        cpp_template_argument = get_element_or_die(ar_cpp_type,
                                                   "TEMPLATE-ARGUMENTS/CPP-TEMPLATE-ARGUMENT")
        if len(cpp_template_argument) != 1:
            return False
        cpp_array_element_type_ref = get_element_or_die(cpp_template_argument[0], "TEMPLATE-TYPE-REF")
        cpp_array_element_type = self.arxml_tree.find_referable(str(cpp_array_element_type_ref))

        return self.validate_app_to_cpp_type(app_array_element_type, cpp_array_element_type, mappings)

    def _validate_app_record_type(self, ar_app_data_type, ar_cpp_type, mappings):
        cpp_category = get_element_or_die(ar_cpp_type, "CATEGORY")
        if cpp_category != "STRUCTURE":
            return False

        app_record_elements = get_element_or_die(ar_app_data_type,
                                                 "ELEMENTS/APPLICATION-RECORD-ELEMENT")

        cpp_impl_data_type_elements = get_element_or_die(ar_cpp_type,
                                                         "SUB-ELEMENTS/CPP-IMPLEMENTATION-DATA-TYPE-ELEMENT")

        if len(app_record_elements) != len(cpp_impl_data_type_elements):
            return False

        for app_record_element, cpp_impl_data_type_element in \
                zip(app_record_elements, cpp_impl_data_type_elements):
            app_record_type_refs = app_record_element.find_elements_of_type("TYPE-TREF")
            if len(app_record_type_refs) != 1:
                return False
            app_element = self.arxml_tree.find_referable(str(app_record_type_refs[0]))

            cpp_element_ref = get_element_or_die(cpp_impl_data_type_element,
                                                 "TYPE-REFERENCE/TYPE-REFERENCE-REF")
            cpp_element = self.arxml_tree.find_referable(str(cpp_element_ref))  

            if self.validate_app_to_cpp_type(app_element, cpp_element, mappings) == False:
                return False
        return True

    def _validate_app_assoc_map_type(self, ar_app_data_type, ar_cpp_type, mappings):
        app_key_type_refs = get_element_or_die(ar_app_data_type, "KEY/TYPE-TREF")
        app_value_type_refs = get_element_or_die(ar_app_data_type, "VALUE/TYPE-TREF")
        if len(app_key_type_refs) != 1 or len(app_value_type_refs) != 1:
            return False
        app_key_type = self.arxml_tree.find_referable(str(app_key_type_refs[0]))
        app_value_type = self.arxml_tree.find_referable(str(app_value_type_refs[0]))

        cpp_template_arguments = get_element_or_die(ar_cpp_type,
                                                   "TEMPLATE-ARGUMENTS/CPP-TEMPLATE-ARGUMENT")
        if len(cpp_template_arguments) != 2:
            return False
        cpp_key_type_ref = get_element_or_die(cpp_template_arguments[0], "TEMPLATE-TYPE-REF")
        cpp_value_type_ref = get_element_or_die(cpp_template_arguments[1], "TEMPLATE-TYPE-REF")
        cpp_key_type = self.arxml_tree.find_referable(str(cpp_key_type_ref))
        cpp_value_type = self.arxml_tree.find_referable(str(cpp_value_type_ref))

        if self.validate_app_to_cpp_type(app_key_type, cpp_key_type, mappings) == False:
            return False
        if self.validate_app_to_cpp_type(app_value_type, cpp_value_type, mappings) == False:
            return False
        return True

    def validate_app_to_cpp_type(self, ar_app_data_type, ar_cpp_type_ref, mappings):
        """Ensure that an application data type aligns with all related cpp implementation data types."""
        ar_app_data_type_kind = ar_app_data_type.short_tag()
        ar_cpp_type = self.arxml_tree.find_referable(str(ar_cpp_type_ref))
        cpp_category = get_element_or_die(ar_cpp_type, "CATEGORY")

        while cpp_category == "TYPE_REFERENCE":
            typeref_cpptype_ref = get_element_or_die(ar_cpp_type, "TYPE-REFERENCE-REF")
            if typeref_cpptype_ref.get('DEST') != 'STD-CPP-IMPLEMENTATION-DATA-TYPE':
                return False
            ar_cpp_type = self.arxml_tree.find_referable(str(typeref_cpptype_ref))
            cpp_category = get_element_or_die(ar_cpp_type, "CATEGORY")

        if ar_app_data_type_kind == 'APPLICATION-PRIMITIVE-DATA-TYPE':
            return self._validate_app_primitive_type(ar_app_data_type, ar_cpp_type, mappings)
        elif ar_app_data_type_kind == 'APPLICATION-ARRAY-DATA-TYPE':
            return self._validate_app_array_type(ar_app_data_type, ar_cpp_type, mappings)
        elif ar_app_data_type_kind == 'APPLICATION-RECORD-DATA-TYPE':
            return self._validate_app_record_type(ar_app_data_type, ar_cpp_type, mappings)
        elif ar_app_data_type_kind == 'APPLICATION-ASSOC-MAP-DATA-TYPE':
            return self._validate_app_assoc_map_type(ar_app_data_type, ar_cpp_type, mappings)
        else:
            return False

    def get_data_type_maps(self, ar_interface):
        """Collect and cache all data mappings related to a specific interface."""
        if ar_interface.get_fqn() not in self._app_to_cpp_mappings:
            mappings = {}
            port_interface_to_data_type_mappings = self.arxml_tree.find_elements_of_type(_PTD_MAPPINGS,
                                                                                    PORT_INTERFACE_REF=ar_interface.get_fqn())
            for port_interface_to_data_type_mapping in port_interface_to_data_type_mappings:
                data_type_mapping_set_refs = port_interface_to_data_type_mapping.find_elements_of_type(
                    'DATA-TYPE-MAPPING-SET-REF', DEST='DATA-TYPE-MAPPING-SET')
                for data_type_mapping_set_ref in data_type_mapping_set_refs:
                    data_type_mapping_set = self.arxml_tree.find_referable(str(data_type_mapping_set_ref))
                    data_type_maps = data_type_mapping_set.find_elements_of_type('DATA-TYPE-MAP')
                    for data_type_map in data_type_maps:
                        ar_application_data_type_refs = \
                            data_type_map.find_elements_of_type('APPLICATION-DATA-TYPE-REF')
                        assert len(ar_application_data_type_refs) == 1, \
                            "Exactly one {0} expected for {1}, found {2}".format(
                                'APPLICATION-DATA-TYPE-REF',
                                data_type_map.get_fqn(),
                                len(ar_application_data_type_refs))
                        ar_application_data_type = \
                            self.arxml_tree.find_referable(str(ar_application_data_type_refs[0]))

                        ar_implementation_data_type_refs = \
                            data_type_map.find_elements_of_type('IMPLEMENTATION-DATA-TYPE-REF')
                        assert len(ar_implementation_data_type_refs) == 1, \
                            "Exactly one {0} expected for {1}, found {2}".format(
                                'IMPLEMENTATION-DATA-TYPE-REF',
                                data_type_map.get_fqn(),
                                len(ar_implementation_data_type_refs))
                        impl_type_kind = ar_implementation_data_type_refs[0].get('DEST')

                        if ar_application_data_type is not None and \
                                impl_type_kind == 'STD-CPP-IMPLEMENTATION-DATA-TYPE':
                            if ar_application_data_type not in mappings:
                                mappings[ar_application_data_type] = ar_implementation_data_type_refs[0]
                            else:
                                self._log.warning("Found multiple {0} for {1}. The first is taken.".format(
                                    "STD-CPP-IMPLEMENTATION-DATA-TYPE", ar_application_data_type))
            self._app_to_cpp_mappings[ar_interface.get_fqn()] = mappings

        return self._app_to_cpp_mappings[ar_interface.get_fqn()]

    def get_cpp_from_app_type(self, ar_app_type_ref, ar_interface):
        """Retrieve the mapped cpp data type for a given app data type and ensure that they match."""
        ar_app_type = self.arxml_tree.find_referable(str(ar_app_type_ref))
        data_type_maps = self.get_data_type_maps(ar_interface)

        if ar_app_type not in data_type_maps:
            return None

        if not self.validate_app_to_cpp_type(ar_app_type, data_type_maps[ar_app_type], data_type_maps):
            return None

        return self.get_data_type_maps(ar_interface)[ar_app_type]
