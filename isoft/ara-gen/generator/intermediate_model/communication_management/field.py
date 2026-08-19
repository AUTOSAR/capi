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
Field class.
"""

from .transformationprops import TransformationProps
class Field(object):
    """Describes a Field in a Service"""
    def __init__(self, name, init_value="",
                 has_notifier=False, has_getter=False, has_setter=False, deployment=None, impl_type=None):
        if name == "":
            raise AttributeError("A Field must have a name")
        self._name = name
        self.has_notifier = has_notifier
        self.has_getter = has_getter
        self.has_setter = has_setter
        self.init_value = init_value
        self.deployment = deployment
        self.impl_type = impl_type
        self._transformationprops = None

    def __str__(self):
        return "Field(%s)" % self.name

    def __repr__(self):
        return self.__str__()

    @property
    def name(self):
        """Get the name of this Field."""
        return self._name

    @property
    def transformationprops(self):
        return self._transformationprops
    def set_transformationprops(self,transformationprops):

        self._transformationprops = transformationprops


