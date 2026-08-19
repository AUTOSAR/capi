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
Application-specific error class.
"""


class ApplicationErrorException(object):
    """Describes an application error that can happen in a Service Method"""

    def __init__(self, fqn: str, error_code, error_contexts) -> None:
        self._fqn = fqn
        self._error_code = error_code
        self._error_contexts = error_contexts

    def __str__(self):
        """To make debugging easier"""
        return self.name

    def __repr__(self):
        return self.__str__()

    @property
    def fqn(self):
        return self._fqn

    @property
    def name(self):
        """Get the name of the application error"""
        return self._fqn.split("/")[-1]

    @property
    def error_code(self):
        """Get the error code of the application error"""
        return self._error_code

    @property
    def error_contexts(self):
        """Get the error context of the application error"""
        return self._error_contexts

