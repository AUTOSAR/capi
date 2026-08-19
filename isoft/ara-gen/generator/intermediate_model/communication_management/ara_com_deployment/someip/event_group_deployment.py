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
Event-group deployment class.
"""

from ..deployment import Deployment
from ....utils.container import Container


class EventGroupDeployment(Deployment):
    """Describes deployment information necessary for an EventGroup"""
    def __init__(self, event_group_id):
        super().__init__()
        self._id = event_group_id
        self._events = Container()
        self._threshold = 0
        self._subs = []
        self._subscribe_timeo = None
        self._subscribe_renew = None
        self._ttl = None
        self.request_response_delay = None
        self.showConfig = False
        self._multicast_ip = None
        self._muticast_port = ""

    @property
    def event_group_id(self) -> int:
        """Get event group deployment id"""
        return self._id

    @property
    def events(self) -> Container:
        """Get the events in the event group"""
        return self._events

    def add_event(self, event):
        """Add an Event to this EventGroup"""
        self._events.add(event+32768)
    def set_threshold(self,threshold):
        self._threshold = threshold

    @property
    def threshold(self):
        return self._threshold

    def set_subscribe_timeo(self,subscribe_timeo):
        self._subscribe_timeo = subscribe_timeo

    @property
    def subscribe_timeo(self):
        return self._subscribe_timeo

    def set_subscribe_renew(self, subscribe_renew):
        self._subscribe_renew = subscribe_renew

    @property
    def subscribe_renew(self):
        return self._subscribe_renew

    def set_ttl(self, ttl):
        self._ttl = ttl

    @property
    def ttl(self):
        return self._ttl

    def set_subs(self, subs):
        self._subs = subs

    @property
    def getSubs(self):
        return self._subs
    def set_multicast_ip(self,ip):
        self._multicast_ip = ip

    @property
    def multicast_ip(self):
        return self._multicast_ip

    def set_muticast_port(self,port):
        self._muticast_port = port

    @property
    def muticast_port(self):
        return self._muticast_port
