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
BaseType class.
"""


class BaseType(object):
    """Describes the base (C++ style declaration) a of datatype"""
    def __init__(self, name, type_emitter):
        self._name = name
        self._type_emitter = type_emitter

    @property
    def name(self):
        """Get the name of this BaseType."""
        return self._name

    @property
    def namespace(self):
        return []

    @property
    def qualified_namespace(self):
        '''
        Types can be "old" (ImplDataType) or "new" (CppImplDataType).
        "new" types can have type emitter, and thus require std:: prefix
        or be a fundametal one, like bool or float and not require it.
        "old" types always require :: prefix.
        under the hood type_emitter equal to None is in use
        to distinguish between "old" and "new" types.
        '''
        if self.type_is_new:
            if self.type_emitter and self.type_emitter != "FundamentalType":
                return['std']
            else:
                # fundamental type like bool or float
                return []
        else:
            return [""]

    @property
    def type_emitter(self):
        result = self._type_emitter
        if not result:
            if self.name in ['int8_t', 'int16_t', 'int32_t', 'int64_t',
                             'uint8_t', 'uint16_t', 'uint32_t', 'uint64_t', ]:
                result = "cstdint"
        return result

    @property
    def type_is_new(self):
        return not self._type_emitter is None

    @property
    def to_be_generated(self):
        # headers for oldstyle basetypes are always generated,
        # headers for new basetypes are not required
        return not self.type_is_new

    @property
    def referred_types(self):
        return []

    @property
    def final_type(self):
        return self
