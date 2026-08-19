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
Event class.
"""

from ..types.impl_type import ImplDataType
from .transformationprops import TransformationProps

class Event(object):
    """Describes an Event in a Service"""

    def __init__(self, name, impl_type=None, event_deployment=None):
        if name == "":
            raise AttributeError("An event must have a name")
        self.name = name
        self.impl_type = impl_type
        self.event_deployment = event_deployment
        self._transformationprops = None

    def __str__(self):
        return "Event(%s)" % self.name

    def __repr__(self):
        return self.__str__()

    @property
    def transformationprops(self):
        return self._transformationprops

    def set_transformationprops(self, transformationprops):
        self._transformationprops = transformationprops
