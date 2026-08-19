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
View base class.
"""


class View(object):
    """
    Base View class for any model object.
    """

    def __init__(self, model_object):
        self.model_object = model_object

    def __getattr__(self, item):
        return getattr(self.model_object, item)

    def __str__(self):
        return "View_for{%s}" % str(self.model_object)

    def __repr__(self):
        return self.__str__()

    @property
    def standard_name(self):
        return self.model_object.name.lower()


def getstate(self):
    return self.model_object


def setstate(self, state):
    self.model_object = state


View.__getstate__ = getstate
View.__setstate__ = setstate
