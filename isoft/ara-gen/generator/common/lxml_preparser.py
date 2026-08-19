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
ARXML parser.
"""

import logging
import sys
import os
import glob
from lxml import etree, objectify

from generator.common.autosar_mapping import AUTOSARMapping
from generator.common.arxml_merger import AUTOSARTypesSet
from generator.common.method_util import remove_nil
from generator.common.tree_helper import get_schema_from_file


class LxmlPreparser:
    """
    Creates lxml parsers and checks input ARXMLs against a given schema
    """

    def __init__(self):
        self._log = logging.getLogger(__name__)
        self._schema = None

    def _get_parser(self):
        """
        Convenience function to get the XMLParser from etree
        :return: etree.XMLParser
        """
        xml_parser = etree.XMLParser(huge_tree=True,remove_comments=True, remove_blank_text=True)

        xml_parser.setElementClassLookup(
            objectify.ObjectifyElementClassLookup(tree_class=AUTOSARMapping))

        return xml_parser

    def _set_schema_for_validation(self, schema_file):
        self._schema = etree.XMLSchema(file = open(schema_file, encoding='utf-8'))
        self._schema_file = schema_file

    def _validate(self, filename, showError):
        """
        Validates an ARXML file against [cached] schema
        """
        assert self._schema is not None, "schema must be set before any validation"
        assert os.path.isfile(filename), "file {} does not exist".format(filename)
        # root = etree.parse(open(filename, encoding='utf-8')).getroot()
        tree = remove_nil(filename)
        root = tree.getroot()
        if showError:
            self._log.debug("Started ARXML validation. Schema %s will be used", self._schema_file)

        if self._schema.validate(root):
            if showError:
                self._log.info("ARXML validation successfully finished. SOURCE=%s", filename)
        else:
            if showError:
                self._log.error("[INIT] CODE-001: ARXML validation failed! SCHEMA=%s SOURCE=%s", self._schema_file, filename)
                for error in self._schema.error_log:
                    self._log.error("[INIT] CODE-001: line:%d %s", error.line, error.message)
                sys.exit("XML validation failed!")
            else:
                sys.exit()

    def _validate_root(self, root, showError):
        if self._schema.validate(root):
            self._log.info("ARXML validation successfully finished. SOURCE=%s", "filename")
        else:
            if showError:
                self._log.error("[INIT] CODE-002: ARXML validation failed! SCHEMA=%s SOURCE=%s", self._schema_file, "filename")
                for error in self._schema.error_log:
                    self._log.error("[INIT] CODE-002: line:%d %s", error.line, error.message)
                sys.exit("XML validation failed!")
            else:
                sys.exit()

    def _create_file_list(self, content):
        filelist = list()
        for file_arg in content:
            if os.path.isdir(file_arg):
                # Directory parameter. Create list of all arxml files in the directory
                files = [arg for arg in glob.glob(file_arg + "/*.arxml*")]
            elif '*' in file_arg:
                # Handle wildcard to create list
                files = [arg for arg in glob.glob(file_arg)]
            else:
                # File name
                files = [file_arg]

            for file in files:
                filelist.append(file)
        return filelist

    def _validate_schema_versions(self, files):
        versions = [get_schema_from_file(file) for file in files]
        if not versions.count(versions[0]) == len(versions):
            self._log.error("ARXML validation failed! Multiple schema versions detected.")
            sys.exit("XML validation failed!")

    types_set = None
    merged_root = None

    def parse(self, files, diagStr, showError, skip_reference_checking) -> objectify.ObjectifiedElement:
        """
        Parses ARXML files list
        :param files: list of files to process
        :return: objectify.ObjectifiedElement
        """
        if len(diagStr) == 0:
            real_list_of_files = self._create_file_list(files)
            assert len(real_list_of_files) > 0, \
                ("Wildcards processing lead to empty list of files.\n"
                 "Original list: {}").format(files)

            self._validate_schema_versions(real_list_of_files)

            # pick the first file as schema reference.
            schema_file = get_schema_from_file(real_list_of_files[0])
            self._set_schema_for_validation(schema_file)
            for f in real_list_of_files:
                self._validate(f, showError)
            self.types_set = AUTOSARTypesSet(schema_file)
            self.merged_root = self.types_set.merge_files(real_list_of_files)
        else:
            self.merged_root = self.types_set.merge_files_xml(self.merged_root, diagStr)
        self.types_set.order_tree(self.merged_root)
        self._validate_root(self.merged_root, showError)
        if not skip_reference_checking:
            hanging_references = self.merged_root.get_hanging_references()
            assert len(hanging_references) == 0 or not showError, (
                "[INIT] CODE-004: The following elements do not exist.: {}".format(hanging_references)
            )
            if len(hanging_references) == 0:
                string = self.merged_root.to_pretty_string().encode('utf-8')
                result = objectify.fromstring(string, self._get_parser())
                return result
        else:
            string = self.merged_root.to_pretty_string().encode('utf-8')
            result = objectify.fromstring(string, self._get_parser())
            return result

