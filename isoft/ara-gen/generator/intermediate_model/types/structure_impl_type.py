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
Structure implementation type.
"""

from typing import List, Tuple
from .impl_type import ImplDataType


class StructureImplDataType(ImplDataType):
    """Describes a one-level structure data-type
    (C++ style declaration)"""

    def __init__(self, name: str, type_emitter,
                 sub_elements: List[Tuple[str, ImplDataType, bool, int]], namespace: List[str] = None) -> None:
        super().__init__(name, type_emitter, [], namespace=namespace)
        self._sub_elements = sub_elements
        self.size_of_struct_length_field = 0
        for element in sub_elements:
            self.add_referred_type(element[1])

    @property
    def sub_elements(self):
        """Get the list of subelements of the StructureImplDataType."""
        return self._sub_elements

    @property
    def has_optional(self):
        """Return true if there is at least one optional element among subelements"""
        return len(list(filter(lambda e: e[2], self._sub_elements))) > 0

    @property
    def is_tlv(self):
        """Return true if the first element has a data id. Return false otherwise.
        Enough to check the first element. All elements are tlv encoded or none of them."""
        if self._sub_elements[0][3] is not None:
            return True
        else:
            return False
