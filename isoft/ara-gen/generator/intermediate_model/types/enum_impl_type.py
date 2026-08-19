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
Enumeration implementation type.
"""

from typing import List, Tuple
from .impl_type import ImplDataType, BaseType


class EnumImplDataType(ImplDataType):
    """Describes an enumeration data-type (C++ style declaration)"""
    def __init__(self, name, type_emitter, data_type, literal_value_map: List[Tuple[str, int]], namespace: List[str] = None) -> None:
        super().__init__(name, type_emitter, [data_type], namespace)
        self._literal_value_map = literal_value_map

    @property
    def data_type(self):
        """Get the implementation data type for this EnumImplDataType."""
        return self.referred_types[0]

    @property
    def literal_value_map(self):
        """Get the literal_value_map dict for this EnumImplDataType."""
        return self._literal_value_map
