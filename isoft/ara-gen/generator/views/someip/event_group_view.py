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
EventGroupView corresponding to a EventGroupDeployment IM object.
"""
from generator.common.tree_helper import transition_number
from generator.views.view import View

from generator.intermediate_model.communication_management.ara_com_deployment.someip.event_group_deployment \
    import EventGroupDeployment


class EventGroupView(View):
    """Specialized View of an EventGroupDeployment."""

    def __init__(self, event_group: EventGroupDeployment) -> None:
        self._event_group = event_group
        super().__init__(event_group)

    @property
    def event_group_id(self):
        """Get the event group deployment id"""
        return hex(self._event_group.event_group_id)

    @property
    def event_group(self):
        return self._event_group

    @property
    def events(self):
        """Get the events in the event group deployment"""
        events = list()
        for event in self._event_group.events:
            events.append(hex(event))

        return events
