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
SupervisedEntity class.
"""

# Unused import check suppressed, because only used in type annotation, which is not found by PyLint
from typing import List, Dict   # pylint: disable=unused-import

from ..utils.container import Container
from .checkpoint import CheckPoint



class SupervisedEntityInterface(object):
    """Describes a SupervisedEntityInterface that can be provided by
    or requested from a component"""
    def __init__(self, fqn: str) -> None:
        if fqn == "":
            raise AttributeError("A SupervisedEntityInterface must have a name")

        self._fqn = fqn
        self.namespaces = list()  # type: List[str]
        self._exposed_api_content = {}   # type: Dict
        self._exposed_api_content["checkpoints"] = Container()
        self._identifier = []

    def __str__(self):
        return "SupervisedEntityInterface(%s)->API(%s)" % (self.name, self._exposed_api_content)

    def __repr__(self):
        return self.__str__()

    def get_identifiernamespace(self, identifier):
        temp = identifier.split("/")
        result = []
        for name in temp:
            if name != temp[0] and name != temp[1] and name != temp[-1]:
                result.append(name)
        return result

    def get_identifiername(self,identifier) -> str:
        return identifier.split("/")[-1]

    @property
    def identifier(self):
        """Get the SupervisedEntityInterface name"""
        return self._identifier

    def addidentifier(self,identifiers):
        for identifier in identifiers:
            if identifier not in self._identifier:
                self._identifier.append(identifier)

    @property
    def name(self) -> str:
        """Get the SupervisedEntityInterface name"""
        return self._fqn.split("/")[-1]

    def add_checkpoint(self, checkpoint: CheckPoint):
        """Add an CheckPoint to this Service."""
        self._exposed_api_content["checkpoints"].add(checkpoint)

    @property
    def checkpoints(self):
        """Get the CheckPoints of this Service."""
        return self._exposed_api_content["checkpoints"]
    @property
    def fqn(self):
        return self._fqn


