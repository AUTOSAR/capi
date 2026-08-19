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
Service deployment class.
"""
from pprint import pprint

from generator.intermediate_model.utils.container import Container
from ..service_deployment import ServiceDeployment

class DdsServiceDeployment(ServiceDeployment):
    """Describes deployment information necessary for a Service"""

    def __init__(self):
        super().__init__()
        self.deployment_id = ""
        self.instance_id = ""
        self.domain_id = ""
        self.qos_profile = ""
        self._transport_plugins = Container()
        self.required = None
        self.provided = None
        self.is_show = True
        self.at_key = True
        self.name = ""
        self.standard_name = ""
        self.deployment_fqn = ""

    def __str__(self):
        return "DdsServiceDeployment({}, {})".format(self.required, self.provided)

    def __repr__(self):
        return self.__str__()


    @property
    def transport_plugins(self) -> Container:
        return self._transport_plugins

    def add_transport_plugin(self, plugin: str):
        self._transport_plugins.append(plugin)
