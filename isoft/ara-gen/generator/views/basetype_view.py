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
BasetypeView corresponding to a basettype declaration object.
"""

from typing import List
from generator.views.view import View
from generator.intermediate_model.types.base_type import BaseType


class BaseTypeView(View):
    """Specialized View of a BaseType declaration."""

    def __init__(self, basetype: BaseType, cpptype_name) -> None:
        super().__init__(basetype)
        self._basetype = basetype
        self._cpptype_name = cpptype_name

    def __eq__(self, other):
        """Overrides the default implementation"""
        if isinstance(other, BaseTypeView):
            return self.type_name == other.type_name and self.cpptype_name == other.cpptype_name
        return NotImplemented

    def __ne__(self, other):
        """Overrides the default implementation"""
        eq_result = self.__eq__(other)
        if eq_result is not NotImplemented:
            return not eq_result
        return NotImplemented

    def __hash__(self):
        """Overrides the default implementation"""
        return hash((self.type_name, self.cpptype_name))

    def __str__(self):
        """To make debugging easier"""
        return "basetype({0}->{1})".format(self.type_name, self.cpptype_name)

    @property
    def type_name(self):
        """Get the basetype name"""
        return self._basetype.name

    @property
    def cpptype_name(self):
        """Get the cpptype name"""
        return self._cpptype_name

    @property
    def self_include(self) -> str:
        result = ""
        if self._basetype.to_be_generated:
            result = '"base_type_{0}.h"'.format(self.type_name.lower())
        else:
            if self._basetype.type_emitter and self._basetype.type_emitter != "FundamentalType": # 特殊处理
                result = '<{0}>'.format(self._basetype.type_emitter)
        return result

    @property
    def includes(self) -> List[str]:
        """Get list of include files for the basetype header file if any"""
        include_files = []
        if self._basetype.type_is_new:
            if self._basetype.type_emitter:
                include_files = ['<{0}>'.format(self._basetype.type_emitter)]
        else:
            if "float" in self.type_name:
                include_files = ['<cfloat>']

        return include_files

    @property
    def referred_views(self):
        return []

    @property
    def qualified_cpp_name(self) -> str:
        return "::".join(self.qualified_namespace + [self.name])

    @property
    def qualified_cpp_name_fastdds(self) -> str:
        return "::".join(self.qualified_namespace + [self.name])

    def cpp_name_in_namespace(self, namespace) -> str:
        return self.qualified_cpp_name

    @property
    def underlying_model_type(self):
        return self._basetype
