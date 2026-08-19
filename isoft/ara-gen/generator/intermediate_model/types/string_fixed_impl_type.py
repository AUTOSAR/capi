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
String implementation type.
"""

from .impl_type import ImplDataType


class FixedStringImplDataType(ImplDataType):
    """Describes a string type"""
    # @uptrace{TPS_MANI_03144)
    def __init__(self, name, type_emitter, array_size, namespace):
        super().__init__(name, type_emitter, [], namespace)
        self._array_size = array_size

    @property
    def array_size(self):
        """Get the array_size of the ArrayDataType."""
        return self._array_size
