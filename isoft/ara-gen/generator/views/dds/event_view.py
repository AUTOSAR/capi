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

from generator.views.event_view import EventView
from generator.views.dds.model_to_view import ViewManipulations

from generator.intermediate_model.communication_management.event \
    import Event


class EventView(EventView):
    """Specialized View of an Event."""

    def __init__(self, event: Event) -> None:
        super().__init__(event)
        self.type_view = ViewManipulations.convert_to_view_tree(event.impl_type)

    @property
    def topic_name(self):
        return self._event.event_deployment.topic

    @property
    def qos_profile(self):
        return self._event.event_deployment.qos_profile

    @property
    def transport_protocols(self):
        return self._event.event_deployment.transport_protocols
