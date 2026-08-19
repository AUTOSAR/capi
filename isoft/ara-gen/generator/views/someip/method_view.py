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
MethodView corresponding to a Method IM object.
"""
from generator.common.tree_helper import transition_number
from generator.views.method_view import MethodView
from generator.intermediate_model.communication_management.method import Method


class MethodView(MethodView):
    """Specialized View of a method."""

    def __init__(self, method: Method) -> None:
        super().__init__(method)

    @property
    def deployment_id(self):
        """Get the method deployment id"""
        return hex(int(self._method.deployment.deployment_id))

    @property
    def method_id(self):
        """Get the method deployment id"""
        return transition_number(self.deployment_id)

    @property
    def is_reliable(self):
        return str(self._method.deployment.proto).upper() == 'TCP'

    @property
    def deployment(self):
        return self._method.deployment

