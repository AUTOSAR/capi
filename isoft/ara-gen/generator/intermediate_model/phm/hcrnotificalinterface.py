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
HCRNotificalInterface class.
"""

# Unused import check suppressed, because only used in type annotation, which is not found by PyLint
from typing import List, Dict   # pylint: disable=unused-import


class HCRNotificalInterface(object):
    """Describes a HCRNotificalInterface that can be provided by
    or requested from a component"""
    def __init__(self, fqn: str) -> None:
        if fqn == "":
            raise AttributeError("A HCRNotificalInterface must have a name")

        self._fqn = fqn
        self.namespaces = list()  # type: List[str]



    def __str__(self):
        return "HCRNotificalInterface(%s)" % (self.name)

    def __repr__(self):
        return self.__str__()


    @property
    def name(self) -> str:
        """Get the HCRNotificalInterface name"""
        return self._fqn.split("/")[-1]

    @property
    def fqn(self):
        return self._fqn

