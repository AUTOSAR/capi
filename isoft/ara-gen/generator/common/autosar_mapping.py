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
Class with model access utilities exposed by objectify to the user.
"""

import logging
import sys

from lxml import objectify
from lxml.objectify import StringElement

import functools

import generator.parser
from generator.common.method_util import GLOBAL_LOCAL_PATH

LOG = logging.getLogger(__name__)


class PathError(Exception):
    """Thrown when the referable is not found."""
    pass


class AUTOSARMapping(objectify.ObjectifiedElement):
    """
    This class extends the objects returned by objectify with
    custom convenience functions.
    """

    def get_sub_element(self, item):
        """Get sub element from AUTOSARMapping"""
        value = super().__getattribute__(item)
        return value

    @functools.lru_cache(maxsize=5120)
    def __getattribute__(self, item):
        """Return"""
        value = super().__getattribute__(item)
        if isinstance(value, StringElement):
            return str(value)
        return value


    @functools.lru_cache(maxsize=5120)
    def __getattr__(self, item):
        """
        Convert elements like SHORT-NAME to SHORT_NAME such that they
        may be accessed more conveniently as members
        :param item: Any ARXML tag name, must be a valid child
                     (e.g. application.SHORT_NAME)
        """
        real_attr = item.replace('_', '-')
        if item in ["_setText", "SHORT-NAME", "NAMESPACES", "TYPE_EMITTER", "shape"]:
            super().__getattribute__(real_attr)
        try:
            attr_value = super().__getattribute__(real_attr)
            if attr_value is None:
                # maybe it's an XML attribute instead of sub-element
                attr_value = self.attrib(real_attr)
            if isinstance(attr_value, StringElement):
                return str(attr_value)
            return attr_value
        except AttributeError:
            path = self.get_path()
            LOG.error("[INIT] CODE-006: Unable to get element {} from {}. LocalPath:{}".format(item, self.short_tag(), path))
            sys.exit(1)

    def __contains__(self, path):
        try:
            self.find_referable(path)
            return True
        except PathError:
            return False

    def __str__(self):
        return self.get_fqn()

    def short_tag(self):
        """
        Return the tag without the AR namespace.
        """
        return str(self.tag)[generator.parser.AR_NAMESPACE_LEN:]
        
    @functools.lru_cache(maxsize=5120)
    def _find_elements_of_type(self, ar_type: str,  **constraints):
        """
        Finds elements of the specified type, e.g. 'MACHINE' that satisfy
        specified constraints, e.g. SHORT_NAME='radarMachine'.
        """
        elements = self.findall(".//{}{}".format(generator.parser.AR_NAMESPACE, ar_type))
        result = list()
        for element in elements:
            if not constraints:
                result.append(element)
                continue
            for key, value in constraints.items():
                try:
                    # try getting a sub-element
                    attr_value = getattr(element, key)
                except AttributeError:
                    # otherwise maybe it's an XML attribute
                    attr_value = element.attrib[key]
                if attr_value == value:
                    result.append(element)
        return result

    def find_elements_of_type(self, ar_type: str, accept=lambda e: True, **constraints):
        """
        Finds elements of the specified type, e.g. 'MACHINE' that satisfy
        specified constraints, e.g. SHORT_NAME='radarMachine'.
        """
        result = self._find_elements_of_type(ar_type,**constraints)
        return list(filter(accept, result))

    def find_elements_with_attr(self, attr, value):
        xpath_str = ".//*[@{}='{}']".format(attr, value)
        return self.xpath(xpath_str)

    def parent(self, path):
        parts = path.split("/")
        return "/".join(parts[:-1])
        
    @functools.lru_cache(maxsize=1)
    def _findall_shortname(self):
        r = {}
        for referable in self.findall('.//' + generator.parser.AR_NAMESPACE + 'SHORT-NAME/..'):
            if referable.SHORT_NAME not in r:
                r[referable.SHORT_NAME] = []
            r[referable.SHORT_NAME].append(referable)
        return r

    @functools.lru_cache(maxsize=5120)
    def findall_fqn(self):
        r = {}
        for referable in self.findall('.//' + generator.parser.AR_NAMESPACE + 'SHORT-NAME/..'):
            fqn = referable.get_fqn()
            if fqn not in r:
                r[fqn] = []
            if referable.short_tag() != "AR-PACKAGE":
                r[fqn].append(referable)

        return r

    @functools.lru_cache(maxsize=5120)
    def find_referable(self, path):
        """
        Finds any Referable by it's short name. You can either give a path
        or just the short name.
        But when given just the short name without the package as path
        you might get a false positive.
        The path must not be absolute.
        :param path: short name or relative path
        :return: ObjectifyObject: referable
        """
        LOG.debug('Searching for referable "%s"', path)

        if path[0] == '/':
            LOG.debug('Referable "%s" is a full path referable', path)
            path = path[1:]
        else:
            LOG.debug('Referable "%s" is just a short name, you might get a false positive result!', path)

        name = path.split("/")[-1]
        package_path = "/" + "/".join(path.split("/")[:-1])
      
        t = self._findall_shortname()
        name_list = []
        if name in t:
            name_list = t[name]

        for referable in name_list:
            if package_path != "/":
                # package path given
                if referable.get_package_path() == package_path:
                    return referable
                continue
            else:
                # no package path given
                LOG.\
                  warning('Referable "%s" is found by matching short name, you might get a false positive result',
                          referable.SHORT_NAME)
                return referable

        raise PathError('"{}" Referable not found'.format(path))

    @functools.lru_cache(maxsize=5120)
    def find_element_by_attr(self, attr):
        referables = self.findall(
            './/' + generator.parser.AR_NAMESPACE + attr + '/..'
        )
        return referables

    def get_fqn(self):
        """Get the full qualified name"""
        fqn = self.get_fqn_as_list()
        return AUTOSARMapping.fqn_as_string(fqn)

    def get_path(self):
        fqn = self.get_fqn()
        localPath = self.get_deep_path(fqn)
        return localPath

    def get_deep_path(self, fqn):
        while fqn:
            if fqn in GLOBAL_LOCAL_PATH:
                localPathList = GLOBAL_LOCAL_PATH[fqn]
                localPath = ",".join(localPathList)
                return localPath
            fqn_parts = fqn.split("/")
            if len(fqn_parts) > 1:
                fqn = "/".join(fqn_parts[:-1])
            else:
                fqn = ""
        return ""

    @functools.lru_cache(maxsize=5120)
    def _get_element_hierarchy_as_list(self):
        """
        Provides the full element hierarchy as a list, the caller has to filter it according
        to the needs.
        :return: Returns the hierarchy of the object as list
        """
        result = []
        referable = self
        result.append(referable)

        while True:
            parent = referable.getparent()
            if parent is None or parent.tag == generator.parser.AR_NAMESPACE +\
                    'AUTOSAR':
                break
            result.insert(0, parent)
            referable = parent

        return result

    @functools.lru_cache(maxsize=5120)
    def get_fqn_as_list(self):
        """
        Provides the full qualified name as list
        :return: Returns the full qualified name of the object as list
        """
        result = []
        hierarchy = self._get_element_hierarchy_as_list()

        for element in hierarchy:
            short_name = getattr(element, 'SHORT-NAME', None)
            if short_name is not None:
                result.append(str(short_name))

        return result

    def get_parent_package_hierarchy_as_list(self):
        """
        Provides the names of the parents AR-PACKAGES hierarchy
        :return: Returns the parents AR-PACKAGES hierarchy of the object as list
        """
        result = []
        hierarchy = self._get_element_hierarchy_as_list()

        for element in hierarchy:
            if element.tag == generator.parser.AR_NAMESPACE + 'AR-PACKAGE':
                short_name = getattr(element, 'SHORT-NAME', None)
                if short_name is not None:
                    result.append(str(short_name))

        # return [x.lower() for x in result]
        return result

    @classmethod
    def fqn_as_string(cls, fqn):
        """
        Returns the full qualified name as string
        :param fqn: The full qualified name as list
        :return: Returns the full qualified name as string
        """
        return '/' + '/'.join(fqn)

    def get_package_path(self):
        """
        Calculates the package path for an referable.
        Excluding the short name.
        :return: Returns the package path, fqn without the short name
        """
        fqn = self.get_fqn_as_list()[:-1]
        return AUTOSARMapping.fqn_as_string(fqn)
