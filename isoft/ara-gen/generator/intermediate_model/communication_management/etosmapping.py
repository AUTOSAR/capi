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

from .sub_eandsmapping import SubEvenAndSignalMapping

class EventToSignalMapping(object):

    def __init__(self):

        self._event = ""
        self._signals = []
        self._submappings = [] #SubEvenAndSignalMapping
        self._order_submappings = []
        self._userevent = False
        self._eventtype = ""
        self._EinS = False  #是否和SignalToEventMapping重复
    @property
    def event(self):
        return self._event

    def set_event(self,event):
        self._event = event

    @property
    def EinS(self):
        return self._EinS

    def set_EinS(self, value):
        self._EinS = value
    @property
    def eventtypevalue(self):
        return self._eventtypevalue

    def set_eventtypevalue(self, eventtypevalue):
        self._eventtypevalue = eventtypevalue

    @property
    def userevent(self):
        return self._userevent

    def set_userevent(self, userevent):
        self._userevent = userevent

    @property
    def signals(self):
        return self._signals

    def add_signals(self, signal):
        if signal not in self._signals:
            self._signals.append(signal)

    @property
    def userevent(self):
        return self._userevent

    def set_userevent(self, userevent):
        self._userevent = userevent

    @property
    def submapping(self):
        return self._submappings

    def add_submapping(self, submapping:SubEvenAndSignalMapping):
        existeventandsignal = False
        for _order_submapping in self._order_submappings:
            if _order_submapping["event"] == submapping.event and _order_submapping["signal"] == submapping.signal:
                existeventandsignal = True
                break
        if not existeventandsignal:
            item = {
                "event": submapping.event,
                "signal": submapping.signal,
                "transmissiontrigger": submapping.transmissiontrigger,
                "userevent":False,
                "eventtype":"",
                "submappinglist": []
            }
            item["submappinglist"].append(submapping)
            self._order_submappings.append(item)
        else:
            existsubmaping = False
            for _order_submapping in self._order_submappings:
                for submapp in _order_submapping["submappinglist"]:
                    if submapping.signaltypevalue == submapp.signaltypevalue and submapping.eventtypevalue == submapp.eventtypevalue:
                        existsubmaping = True
                        return
            if not existsubmaping:
                for _order_submapping in self._order_submappings:
                    if _order_submapping["event"] == submapping.event and _order_submapping[
                        "signal"] == submapping.signal:
                        _order_submapping["submappinglist"].append(submapping)
                        if submapping.transmissiontrigger:
                            _order_submapping["transmissiontrigger"] = True
        # isexist = False
        # for sub_mapping in self._submappings:
        #     if submapping.event==sub_mapping.event and submapping.signal==sub_mapping.signal and submapping.signaltypevalue == sub_mapping.signaltypevalue and submapping.eventtypevalue == sub_mapping.eventtypevalue:
        #         isexist = true
        # if not isexist:
        #     self._submappings.append(submapping)

    @property
    def order_submapping(self):
        return self._order_submappings

    @property
    def eventtype(self):
        return self._eventtype

    def set_eventtype(self, eventtype):
        self._eventtype = eventtype