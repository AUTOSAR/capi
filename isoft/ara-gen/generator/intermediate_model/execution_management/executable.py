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
Executable class.
"""
# Unused import check suppressed, because only used in type annotation, which is not found by PyLint
from typing import List  # pylint: disable=unused-import

from generator.views.component_view import ComponentView
from .component import Component  # pylint: disable=unused-import


class Executable(object):
    '''Executable'''
    def __init__(self, name: str,exename:str,exeversion:str,fqn:str) -> None:
        self.name = name
        self._exename = exename
        self._components = list()  # type: List[Component]
        self._exeversion = exeversion
        self._swcl_info = None
        self._build_type = "Debug"
        self._fqn = fqn
        self._serviceall ={}
        self.dltMessage = {}

    def __str__(self):
        return "Executable(%s)->Components(%s)" % (self.name, self._components)

    def __repr__(self):
        return self.__str__()

    @property
    def components(self):
        """Getter"""
        return self._components

    def add_component(self, component):
        """Adds a component to the executables list of components"""
        self._components.append(component)

    @property
    def exe_name(self):
        return self._exename

    @property
    def exe_fqn(self):
        return self._fqn

    @property
    def exe_version(self):
        if self._exeversion == "":
            return "1.0.0"
        else:
            return self._exeversion

    @property
    def component_views(self):
        return  list(set([ComponentView(component) for component in self.components]))

    @property
    def swcl_info(self):
        return self._swcl_info

    def set_swcl_info(self,swcl_info):
        self._swcl_info = swcl_info

    def setserviceall(self,serviceall):
        self._serviceall = serviceall

    @property
    def serviceall(self):
        return self._serviceall

    def set_build_type(self, buildType):
        self._build_type = buildType

    @property
    def build_type(self):
        return self._build_type
