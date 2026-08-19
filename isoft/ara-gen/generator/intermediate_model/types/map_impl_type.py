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
Map implementation type.
"""

from .impl_type import ImplDataType
from typing import List

class MapImplDataType(ImplDataType):
    """Describes a map datatype (C++ style declaration)"""
    def __init__(self, name, type_emitter, key_data_type: ImplDataType,
                 value_data_type: ImplDataType, namespace: List[str] = None) -> None:
        super().__init__(name, type_emitter, [key_data_type, value_data_type], namespace=namespace)
        if not key_data_type:
            raise AttributeError("A MapImplDataType must have " +
                                 "a key datatype to construct a map with")
        if not value_data_type:
            raise AttributeError("A MapImplDataType must have " +
                                 "a value datatype to construct a map with")
        self._key_data_type = key_data_type
        self._value_data_type = value_data_type

    @property
    def key_data_type(self) -> ImplDataType:
        """Get the key ImplDataType."""
        return self._key_data_type

    @property
    def value_data_type(self) -> ImplDataType:
        """Get the value ImplDataType."""
        return self._value_data_type
