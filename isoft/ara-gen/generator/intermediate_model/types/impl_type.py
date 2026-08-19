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
Base of implementation types.
"""
from typing import List
from .base_type import BaseType


class ImplDataType(object):
    """Describes a datatype declaration (C++ style declaration)"""
    def __init__(self, name: str, type_emitter, referred_types, namespace: List[str], invalidvalue=None) -> None:
        if name == "":
            raise AttributeError("An ImplDataType must have a name that " +
                                 "is a valid C++ variable name")
        self._name = name
        self._referred_types = referred_types
        self._namespace = namespace
        self._type_emitter = type_emitter
        self.lowNamespace = [str(x).lower() for x in namespace]
        self.invalidvalue = invalidvalue
        self._interface_type = False

    @property
    def interface_type(self):
        return self._interface_type

    def set_interface_type(self, value):
        self._interface_type = value

    @property
    def name(self):
        """Get the name of the ImplDataType."""
        return self._name

    @property
    def namespace(self):
        """Get the namespace of the ImplDataType"""
        return self._namespace

    @property
    def qualified_namespace(self):
        """Get the namespace of the ImplDataType"""
        if "" in self._namespace:
            raise AttributeError("Namespace contains empty elements. %s" % self._namespace)
        #return [""] + self._namespace
        return self._namespace

    @property
    def referred_types(self):
        return self._referred_types

    def add_referred_type(self, referred_type):
        if not referred_type in self._referred_types:
            self._referred_types.append(referred_type)

    @property
    def type_is_new(self):
        return not self._type_emitter is None

    @property
    def to_be_generated(self):
        # headers for oldstyle types are always generated,
        # headers for cpptypes are generated only if type has correct emitter
        # [TPS_MANI_01176]
        # [TPS_MANI_01177]
        return (not (self.type_is_new)) or (self._type_emitter == "TYPE_EMITTER_ARA")

    @property
    def final_type(self):
        if self.__class__.__name__ == "ImplDataType":
            # TYPE_REFs
            return self._referred_types[0].final_type
        else:
            return self

    def type_emitter(self):
        result = self._type_emitter
        return result
