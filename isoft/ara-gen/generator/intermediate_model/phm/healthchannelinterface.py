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
from .channelstatus import ChannelStatus



class HealthChannelInterface(object):
    """Describes a HealthChannelInterface that can be provided by
    or requested from a component"""
    def __init__(self, fqn: str) -> None:
        if fqn == "":
            raise AttributeError("A HealthChannelInterface must have a name")

        self._fqn = fqn
        self.namespaces = list()  # type: List[str]
        self._exposed_api_content = {}   # type: Dict
        self._exposed_api_content["channelstatuses"] = Container()
        self._identifier = []
        self.service_deployment = None  # type: PHMDeployment

    def __str__(self):
        return "HealthChannelInterface(%s)->API(%s)" % (self.name, self._exposed_api_content)

    def __repr__(self):
        return self.__str__()

    @property
    def name(self) -> str:
        """Get the HealthChannelInterface name"""
        return self._fqn.split("/")[-1]

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

    def addidentifier(self, identifiers):
        for identifier in identifiers:
            if identifier not in self._identifier:
                self._identifier.append(identifier)

    def add_channelstatus(self, channelstatus: ChannelStatus):
        """Add an ChannelStatus to this Service."""
        self._exposed_api_content["channelstatuses"].add(channelstatus)

    @property
    def channelstatuses(self):
        """Get the ChannelStatuses of this Service."""
        return self._exposed_api_content["channelstatuses"]
    @property
    def fqn(self):
        return self._fqn


