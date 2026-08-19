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
Method deployment class.
"""

from generator.intermediate_model.utils.container import Container
from ..deployment import Deployment


class MethodDeployment(Deployment):
    """Describes deployment information necessary for an Method"""
    def __init__(self, request_topic, reply_topic):
        super().__init__()
        self._transport_protocols = Container()
        self.qos_profile = ""
        self.request_topic = request_topic
        self.reply_topic = reply_topic

    @property
    def transport_protocols(self) -> Container:
        return self._transport_protocols

    def add_transport_protocol(self, protocol: str):
        self._transport_protocols.append(protocol)

