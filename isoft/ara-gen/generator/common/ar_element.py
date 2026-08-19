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
import re
import functools
import sys
import lxml.etree as et
from copy import deepcopy
import io

from generator.common.method_util import GLOBAL_LOCAL_PATH

splitableOb = {}


class ArXMLParser(et.XMLParser):
    sourceName = ""


class ArElement(et.ElementBase):

    ns_mapping = {"AR": "http://autosar.org/schema/r4.0"}
    sourceName = ""

    @classmethod
    def parser(cls, sourceName=""):

        ar_parser_lookup = et.ElementDefaultClassLookup(element=cls)
        ar_parser = ArXMLParser(huge_tree=True,remove_comments=True, remove_blank_text=True)
        ar_parser.sourceName = sourceName
        ar_parser.set_element_class_lookup(ar_parser_lookup)

        return ar_parser

    @functools.lru_cache(maxsize=5120)
    def add_namespaces(self, string, ns_string):
        # we might use `parts = string.split("/")`
        # but it breaks on xpaths like .//NAME[text()="/a/b/c/d"]
        # also xpath unable to work with empty namespaces, so just not insert AR is not an option
        # note that the order of options in findall is important
        # we first try to capture more exotic things with []
        parts = re.findall(r"./|[^/]+\[.*\]|[^/]+", string)
        for i, part in enumerate(parts):
            if part in ["./", ".", "..", ""]:
                pass
            else:
                parts[i] = "{}:{}".format(ns_string, part)

        return "/".join(parts)

    @functools.lru_cache(maxsize=5120)
    def findall(self, path):
        return super().findall(self.add_namespaces(path, "AR"), self.ns_mapping)

    @functools.lru_cache(maxsize=5120)
    def find(self, path):
        return super().find(self.add_namespaces(path, "AR"), self.ns_mapping)

    def xpath(self, path):
        return super().xpath(self.add_namespaces(path, "AR"), namespaces=self.ns_mapping)

    @staticmethod
    def from_other(other):
        result = deepcopy(other)
        if other is not None:
            # we need everything but kids
            result[:] = []
        return result

    @property
    @functools.lru_cache(maxsize=5120)
    def pure_tag(self):
        return et.QName(self).localname

    @functools.lru_cache(maxsize=5120)
    def get_text(self, path):
        if path == "":
            # note that etree.tostring understanding of "encoding" parameter differs
            # from its understanding in decode() and encode() functions. Passing encoding='utf-8' will produce bytes, not string
            result = et.tostring(self, encoding=str, method='xml')
            result = re.sub(r"[\s]+", "", result)

        else:
            tags = self.findall(path)
            result = tags[0].text if tags else None

        return result

    def get_splitable_id(self, splitkeys):
        if self in splitableOb:
            return splitableOb[self]
        else:
            result = {
                "tag": self.pure_tag
            }
            for splitkey in splitkeys:
                result[splitkey] = self.get_text(splitkey)
            if '' not in splitkeys:
                splitableOb[self] = result
            return result

    def get_nonsplitable_id(self):
        name = self.get_text("SHORT-NAME")
        text = self.text if not name else None
        return {
            "tag": self.pure_tag,
            "content": text,
            "name": name
        }

    @property
    def sourceName(self):
        return self.getroottree().parser.sourceName

    @property
    @functools.lru_cache(maxsize=5120)
    def fqn(self):
        result = self.short_name
        parent = self.getparent()
        if parent is not None:
            result = parent.fqn + "/" + self.short_name
        return result.replace("//","/")

    @property
    @functools.lru_cache(maxsize=5120)
    def short_name(self):
        result = ""
        name = self.find("SHORT-NAME")
        if name is not None:
            result = name.text
        return result

    def get_path(self):
        localPath = ""
        fqn = self.fqn
        if fqn.endswith("/"):
            fqn = fqn[:-1]
        if fqn in GLOBAL_LOCAL_PATH:
            localPathList = GLOBAL_LOCAL_PATH[fqn]
            localPath = ",".join(localPathList)
        return localPath

    def to_pretty_string(self):
        # we have to do this trick with reparsing for easy formatting,
        # fortunately it does not take much time even for 20M+ arxmls
        string = et.tostring(self, encoding=str)
        file_to_reparse = io.StringIO(string)
        parser = et.XMLParser(huge_tree=True,remove_blank_text=True)
        tree = et.parse(file_to_reparse, parser)
        pretty_string = et.tostring(tree, encoding='utf-8', pretty_print=True, xml_declaration=True).decode('utf-8')
        return pretty_string

    def to_file(self, filename):
        with open(filename, 'w', encoding='utf-8') as f:
            f.write(self.to_pretty_string())

    def __str__(self):
        """To make debugging easier"""
        return "<{}:{}>".format(self.pure_tag, self.fqn)

    def __repr__(self):
        return self.__str__()

    def get_hanging_references(self):
        # Just check that the reference points to something existing, disregarding the type of referable.

        # Currently there are references in model formally pointing to one element but in fact pointing to its child
        # For example see deployment descriptions for fields.
        # <EVENT-REF DEST="SOMEIP-EVENT-DEPLOYMENT">/a/b/c/field_deployment_descriptor/notifier</EVENT-REF>
        # note that type here is SOMEIP-EVENT-DEPLOYMENT, but it will actually point to SOMEIP-FIELD-DEPLOYMENT/NOTIFIER
        # which is quite different

        hanging_refs = set()
        fqns = set()

        existing_nodes = self.findall(".//*")
        for node in existing_nodes:
            fqns.add(node.fqn)

        # xpath's ends-with is not available unfortunately, so
        # below is the version made out of sticks and ropes
        refs = self.xpath(".//*[substring(local-name(), string-length(local-name()) - 3) = '-REF']")
        refs += self.xpath(".//*[substring(local-name(), string-length(local-name()) - 4) = '-TREF']")
        for ref in refs:
            if ref.pure_tag not in ["DATA-PROTOTYPE-IN-SERVICE-INTERFACE-REF"]:
                ref_text = ref.text.strip()
                if ref_text not in fqns:
                    hanging_refs.add(ref_text)

        return sorted(list(hanging_refs))
