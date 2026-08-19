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

from generator.intermediate_model.utils.container import Container
from ..service_deployment import ServiceDeployment
from ...ara_com_deployment.someip.event_group_deployment import EventGroupDeployment


class SomeIpServiceDeployment(ServiceDeployment):
    """Describes deployment information necessary for a Service"""

    def __init__(self):
        super().__init__()
        self.deployment_fqn = ""
        self.deployment_id = ""
        self.instance_id = ""
        self.ports = None
        self.sd_config = {}
        self._event_group_deployments = Container()
        self.required = None
        self.provided = None
        self.major_version = None
        self.minor_version = None
        self.load_balancing_priority = None
        self.version_driven_find_behavior = None
        self.required_minor_version = None
        self.load_balancing_weight = None
        self.is_show = True
        self.bindPorts = None
        self.name = ""
        self.standard_name = ""
        self.have_event_notifier = False
        self.have_method_get_set = False

    @property
    def event_groups(self) -> Container:
        """Get the event groups in the service"""
        return self._event_group_deployments

    def add_event_group_deployment(self, event_group: EventGroupDeployment):
        """Add an event group to the service"""
        self._event_group_deployments.append(event_group)

