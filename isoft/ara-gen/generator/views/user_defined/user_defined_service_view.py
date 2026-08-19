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
ServiceView corresponding to a Service IM object.
"""

from generator.common.tree_helper import transition_number
from generator.intermediate_model.communication_management.service \
    import Service
from generator.views.someip.someip_service_view import SomeIpServiceView


class UserDefinedServiceView(SomeIpServiceView):
    """Specialized View of a Service."""

    def __init__(self, service: Service) -> None:
        super().__init__(service)

    @property
    def instance_id(self):
        if self.user_defined_type == "icc":
            return "ICC:{}".format(self.icc_ob["instanceId"])
        else:
            return "NPC:{}".format(self.ipc_shm["instanceId"])

    @property
    def userdef_specific_instance_id(self):
        return transition_number(self._service.service_deployment.instance_id)

    @property
    def service_interface_id(self):
        if self.user_defined_type == "icc":
            return self.icc_service["serviceId"]
        else:
            return self.ipc_service["serviceId"]

    @property
    def interface_deployment_fqn(self):
        return self._service.service_deployment.deployment_fqn

    @property
    def user_defined_type(self):
        return self._service.service_deployment.depType

    @property
    def ipc_service(self):
        return self._service.service_deployment.ipcService

    @property
    def ipc_shm(self):
        return self._service.service_deployment.ipcShm

    @property
    def icc_service(self):
        return self._service.service_deployment.iccService

    @property
    def icc_ob(self):
        return self._service.service_deployment.iccOb

