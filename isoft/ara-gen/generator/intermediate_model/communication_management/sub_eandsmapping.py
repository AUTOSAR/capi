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


class SubEvenAndSignalMapping(object):

    def __init__(self):

        self._event = ""
        self._signal = ""
        self._eventtypevalue = ""
        self._signaltypevalue = ""
        self._transmissiontrigger = False
        self._userevent = False


    @property
    def event(self):
        return self._event

    def set_event(self,event):
        self._event = event

    @property
    def signal(self):
        return self._signal

    def set_signal(self,signal):
        self._signal = signal

    @property
    def eventtypevalue(self):
        return self._eventtypevalue

    def set_eventtypevalue(self,eventtypevalue):
        self._eventtypevalue = eventtypevalue

    @property
    def signaltypevalue(self):
        return self._signaltypevalue

    def set_signaltypevalue(self,signaltypevalue):
        self._signaltypevalue = signaltypevalue

    @property
    def transmissiontrigger(self):
        return self._transmissiontrigger

    def set_transmissiontrigger(self, transmissiontrigger):
        self._transmissiontrigger = transmissiontrigger


