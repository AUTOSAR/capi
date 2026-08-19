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
import functools
import os
import sys

import lxml.etree as et
from copy import deepcopy

from lxml import etree

from generator.common.method_util import remove_nil, GLOBAL_LOCAL_PATH, check_uuid
from generator.parser.schema_element import SchemaElement
from generator.common.ar_element import ArElement


class AUTOSARTypesSet(object):

    empty_description = {
        "own_fields": {"ordered": [], "unordered": []},
        "base_types": {"ordered": [], "unordered": []},
    }

    def __init__(self, xsd_schema_filename):

        xsd_schema_tree = et.parse(open(xsd_schema_filename, encoding='utf-8'), SchemaElement.parser())

        self._types_set = {}
        self.get_classes(xsd_schema_tree)

        schema_file = os.path.basename(os.path.realpath(xsd_schema_filename))

        self._empty_arxml = '''
        <AUTOSAR
            xmlns="http://autosar.org/schema/r4.0"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://autosar.org/schema/r4.0 {schema}">
        </AUTOSAR>'''.format(schema=schema_file)

    @property
    def empty_arxml_root(self):
        return et.XML(self._empty_arxml, ArElement.parser())

    @functools.lru_cache(maxsize=5120)
    def get_possible_fields(self, namespaced_typename):
        class_descriptor = self._types_set[namespaced_typename]
        # it is perfectly fine for ordering purposes to treat unordered "choices" as sequential elements
        own = class_descriptor["own_fields"]["ordered"] + class_descriptor["own_fields"]["unordered"]
        own_traversed = []
        for field in own:
            if field["name"]:
                own_traversed.append(field)
            else:
                traversed_fields = self.get_possible_fields(field["type"])
                own_traversed.extend(traversed_fields)

        bases = class_descriptor["base_types"]["ordered"] + class_descriptor["base_types"]["unordered"]
        result = []
        if bases:
            for base in bases:
                if base == namespaced_typename:
                    result.extend(own_traversed)
                else:
                    result.extend(self.get_possible_fields(base))
        else:
            result = own_traversed

        return result

    def get_tag_type_in_class(self, classname, xml_element):
        tag = xml_element.pure_tag
        all_fields = self.get_possible_fields(classname)
        fields = list(filter(lambda x: x["name"] == tag, all_fields))
        assert len(fields) == 1, "multiple types or no types for {}::{}: {}".format(classname, tag, fields)
        return fields[0]

    def get_tag_position_in_class(self, classname, xml_element):
        tag = xml_element.pure_tag
        all_fields = self.get_possible_fields(classname)
        fields = list(filter(lambda x: x["name"] == tag, all_fields))
        assert len(fields) == 1, "multiple types or no types for {}::{}: {}".format(classname, tag, fields)
        result = all_fields.index(fields[0])
        return result

    def collect_base_types(self, class_name, complextype):
        sequences = complextype.findall("xsd:sequence/xsd:group", complextype.nsmap)
        choices = complextype.findall("xsd:choice/xsd:group", complextype.nsmap)

        if sequences and choices:
            assert False, "too much fun in type declaration, {}".format(complextype)

        class_description = self._types_set.setdefault(class_name, deepcopy(self.empty_description))
        class_description["base_types"] = {
            "ordered": [x.get("ref") for x in sequences],
            "unordered": [x.get("ref") for x in choices],
        }

    def get_base_element_name(self, name):
        if name.endswith("-IREFS"):
            return name[:-6]
        if name.endswith("S"):
            return name[:-1]
        return name

    def process_fields(self, class_name, ordering, aggregation):
        fields = aggregation.xpath("*", namespaces=aggregation.nsmap)
        counter = 1000
        for field in fields:
            tag = field.pure_tag
            name = None
            el_type = None
            keep_order = False
            splitable = False
            splitkeys = []
            if tag == "element":
                name = field.get("name")
                el_type = field.get("type")
                splitable = field.splitable
                keep_order = field.ordered
                splitkeys = field.splitkeys

                if not el_type:
                   el_type = "{}:INLINE-{}".format(class_name, counter)
                   counter += 1
                   structs = field.findall("xsd:complexType", field.nsmap)
                   assert len(structs) == 1, "too fancy element: {}".format(name)
                   inline_class_description = self.get_struct_body(el_type, structs[0])
                   assert len(inline_class_description["own_fields"]["ordered"]) == 0, "ordered content not expected: {}".format(name)
                   for f in inline_class_description["own_fields"]["unordered"]:
                       f["splitable"] = splitable
                       f["splitkeys"] = splitkeys
                   splitkeys = []
            elif tag == "choice":
                el_type = "{}:UNNAMED-CHOICE-{}".format(class_name, counter)
                counter += 1
                self._types_set.setdefault(el_type, deepcopy(self.empty_description))
                self.process_fields(el_type, "unordered", field)
            elif tag == "group":
                el_type = field.get("ref")
            else:
                assert False, "unexpected tag {}".format(tag)

            assert el_type, "unable to get type for {}".format(class_name)
            self._types_set[class_name]["own_fields"][ordering].append(
                {
                    "name": name,
                    "type": el_type,
                    "keep_order": keep_order,
                    "splitable": splitable,
                    "splitkeys": splitkeys,
                }
            )

    def get_struct_body(self, class_name, struct):
        sequences = struct.findall("xsd:sequence", struct.nsmap)
        choices = struct.findall("xsd:choice", struct.nsmap)
        simples = struct.findall("xsd:simpleContent", struct.nsmap)

        assert (len(sequences) + len(choices) + len(simples) == 1), "too much fun in body declaration, {}".format(struct)

        class_description = self._types_set.setdefault(class_name, deepcopy(self.empty_description))

        if sequences:
            self.process_fields(class_name, "ordered", sequences[0])
        elif choices:
            self.process_fields(class_name, "unordered", choices[0])
        elif simples:
            # nothing to do here, simple types do not have subelements
            pass
        else:
            assert False, "Strange!"

        return class_description

    def get_classes(self, xsd_schema):
        root = xsd_schema.getroot()

        groups = root.findall("xsd:group", root.nsmap)
        complex_types = root.findall("xsd:complexType", root.nsmap)

        for ct in complex_types:
            class_name = "AR:" + ct.get("name")
            self.collect_base_types(class_name, ct)

        for group in groups:
            class_name = "AR:" + group.get("name")
            self.get_struct_body(class_name, group)

    # the idea is that tag name is not equal to class name refer for example the following snippet from schema:
    '''
    <xsd:element maxOccurs="1" minOccurs="0" name="CAN-FD-BAUDRATE" type="AR:POSITIVE-UNLIMITED-INTEGER">
    '''
    # tag name in arxml will be CAN-FD-BAUDRATE, but it must be treated as POSITIVE-UNLIMITED-INTEGER.
    # sub-node adjustment sequence
    def order_class(self, root, tag_descriptor):
        kids = root.findall("*")
        classname = tag_descriptor["type"]
        for kid in kids:
            tag_descr = self.get_tag_type_in_class(classname, kid)
            self.order_class(kid, tag_descr)
        if not tag_descriptor["keep_order"]:
            def sort_predicate(x):
                return self.get_tag_position_in_class(classname, x)

            root[:] = sorted(kids, key=sort_predicate)
        else:
            pass

    # for top level element we assume tag name represents class.
    # typically it is AUTOSAR
    def order_tree(self, root_element):
        class_name = "AR:" + root_element.pure_tag
        initial_descriptor = {
            "type": class_name,
            "keep_order": False
        }
        self.order_class(root_element, initial_descriptor)

    def _merge_two_trees(self, class_name, left_root, right_root):
        for child in right_root:
            self.set_short_name_path(child)
            tag_info = self.get_tag_type_in_class(class_name, child)
            if tag_info["splitable"] or child.pure_tag in ["ADMIN-DATA"]:
                def search_predicate(x):
                    return x.get_splitable_id(tag_info["splitkeys"]) == child.get_splitable_id(tag_info["splitkeys"])

                same_tag = next(filter(search_predicate, left_root), None)

                if same_tag is None or str(child.pure_tag).endswith("-REF-CONDITIONAL"):
                    same_tag = ArElement.from_other(child)
                    left_root.append(same_tag)

                self._merge_two_trees(tag_info["type"], same_tag, child)

            else:

                def search_predicate(x):
                    return x.get_nonsplitable_id() == child.get_nonsplitable_id()

                same_tag = next(filter(search_predicate, left_root), None)

                if same_tag is None or child.pure_tag in ["SDG", "SD"]:
                    self.child_set_path(child)
                    left_root.append(child)
                else:
                    # "same" element present, nothing to do if it is SHORT-NAME
                    # otherwise report violation
                    assert child.pure_tag == "SHORT-NAME", (
                        "[INIT] CODE-003: ARXML merging error,Non-splittable tag {} already present,competitor is {}.LocalPath:{}"
                    ).format(same_tag, child, child.get_path())
                    pass

    def child_set_path(self, child):
        for item in child:
            if item.pure_tag == "SHORT-NAME":
                self.set_short_name_path(item)
                break
            else:
                self.child_set_path(item)

    def set_short_name_path(self, child):
        if child.pure_tag == "SHORT-NAME":
            parent = child.getparent()
            fqn = parent.fqn
            adminData = False
            if parent.pure_tag == "AR-PACKAGE":
                childList = parent.getchildren()
                if len(childList) >= 2:
                    childItem = childList[1]
                    if childItem.pure_tag == "ADMIN-DATA":
                        adminData = True
            if parent.pure_tag not in ["AR-PACKAGE"] or adminData:
                if fqn not in GLOBAL_LOCAL_PATH:
                    GLOBAL_LOCAL_PATH[fqn] = [child.sourceName]
                else:
                    GLOBAL_LOCAL_PATH[fqn].append(child.sourceName)

    def merge_files(self, input_files):
        tree_roots = []
        uuidList = []
        repeatUuidList = []

        for file_path in input_files:
            tree = remove_nil(file_path)
            check_uuid(tree, uuidList, repeatUuidList)
            node = et.fromstring(etree.tostring(tree), ArElement.parser(sourceName=file_path))
            tree_roots.append(node)
        #tree_roots = [et.parse(open(f, encoding='utf-8'), ArElement.parser()).getroot() for f in input_files
        # tree_roots = [et.fromstring(open(f,mode="rb").read(), ArElement.parser()) for f in input_files] # 速度稍快
        if len(repeatUuidList) > 0:
            for repeatUuid in repeatUuidList:
                print(repeatUuid)
            sys.exit(1)
        return self.merge_tree_roots(tree_roots)

    def merge_files_xml(self, old_tree_roots, xmlStr):
        tree_roots = []
        if len(xmlStr) > 0:
            for xml in xmlStr:
                tree_roots.append(et.fromstring(str.encode(xml), ArElement.parser()))
        for t in tree_roots:
            self._merge_two_trees("AR:AUTOSAR", old_tree_roots, t)
        return old_tree_roots

    def merge_tree_roots(self, tree_roots):
        result = self.empty_arxml_root

        for t in tree_roots:
            self._merge_two_trees("AR:AUTOSAR", result, t)

        return result

