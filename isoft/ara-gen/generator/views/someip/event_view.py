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
EventView corresponding to a Event IM object.
"""
from generator.common.tree_helper import transition_number
from generator.views.event_view import EventView

from generator.intermediate_model.communication_management.event \
    import Event


class EventView(EventView):
    """Specialized View of an Event."""

    def __init__(self, event: Event) -> None:
        super().__init__(event)
        self.eventGroupId = []

    @property
    def deployment_id(self):
        """Get the event deployment id"""
        return hex(self._event.event_deployment.deployment_id+32768)

    @property
    def event_id(self):
        return transition_number(self.deployment_id)

    @property
    def is_reliable(self):
        """Specifies whether the communication is reliable"""
        return str(self._event.event_deployment.proto).upper() == 'TCP'

    @property
    def event_deployment(self):
        return self._event.event_deployment

    @property
    def event_group_id(self):
        return self.eventGroupId
