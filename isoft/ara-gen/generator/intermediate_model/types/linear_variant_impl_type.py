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
Linear variant implementation type.
"""

from typing import List
from .impl_type import ImplDataType


class LinearVariantImplDataType(ImplDataType):
    """Describes a linear one dimensional vector data-type
    (C++ style declaration)"""

    def __init__(self, name, type_emitter, element_data_type, namespace: List[str] = None) -> None:
        if element_data_type is None:
            raise AttributeError("{} must have a data-type to be constructed.".
                                 format(self.__class__.__name__))
        super().__init__(name, type_emitter, element_data_type, namespace=namespace)
        self._converter_to_be_generated = True

    @property
    def element_data_type_list(self):
        return self.referred_types
