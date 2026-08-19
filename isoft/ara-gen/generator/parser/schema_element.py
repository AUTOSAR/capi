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

import lxml.etree as et
import re

class SchemaElement(et.ElementBase):

    @classmethod
    def parser(cls):
        parser_lookup = et.ElementDefaultClassLookup(element=cls)
        parser = et.XMLParser(huge_tree=True,remove_comments=True, remove_blank_text=True)
        parser.set_element_class_lookup(parser_lookup)
        return parser

    def _get_stereotypes(self):
        # list of element's stereotypes, like ["atpSplitable", "atpObject" etc]
        result = []
        stereotypes = self.findall("xsd:annotation/xsd:appinfo[@source='stereotypes']", self.nsmap)
        for stereotype in stereotypes:
            stereotypes_list = stereotype.text
            result.extend(re.split("[;,]", stereotypes_list))

        return result


    def _get_tags(self):
        # forms dictionary of present tags
        # in form {"tag_x": ["val1", "val2", ... "valN"], "tag_y": ["val1", "val2", .. "val_M"], ... etc}
        result = {}
        tags = self.findall("xsd:annotation/xsd:appinfo[@source='tags']", self.nsmap)
        for tag in tags:
            tags_list = re.findall('(.*?)="(.*?)";?', tag.text)
            for tag_body in tags_list:
                result.update({tag_body[0]: re.split(r",\s?", tag_body[1])})
        return result


    @property
    def splitkeys(self):

        def convert_splitkey_name(name, role):
            # convert splitkey in MetaModel format to ARXML format
            # i.e. from "variationPoint.shortLabel" to "VARIATION-POINT/SHORT-LABEL"

            name_components = name.split('.')
            if name_components[0] == role:
                del name_components[0]

            tags = [re.sub('(?!^)([A-Z]+)', r'-\1', x).upper() for x in name_components]
            return "/".join(tags)

        splitkeys = []
        if self.splitable:
            tags = self._get_tags()
            splitkeys_mm = tags.get("atp.Splitkey", [])
            role = self.role_name

            splitkeys = [convert_splitkey_name(x, role) for x in splitkeys_mm]

        return splitkeys


    @property
    def splitable(self):
        return "atpSplitable" in self._get_stereotypes()


    @property
    def ordered(self):
        type_tags = self._get_tags()
        flag = type_tags.get("pureMM.isOrdered", [])
        return flag == ["true"]


    @property
    def pure_tag(self):
        return et.QName(self).localname

    @property
    def role_name(self):
        type_tags = self._get_tags()
        qualified_names = type_tags.get("mmt.qualifiedName", [])
        assert len(qualified_names) > 0, "No qualified name for element {}".format(self.pure_tag)
        assert len(qualified_names) == 1, "Multiple qualified names for element {}".format(self.pure_tag)
        qualified_name = qualified_names[0]
        return qualified_name.split(".")[-1]
