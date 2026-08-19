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

from .signaltype_item import Signaltypeitem

class Signal(object):

    def __init__(self, name):
        if name == "":
            raise AttributeError("A signal must have a name")
        self._name = name
        self._typeitems = []  #Signaltypeitem
        self._serviceid = 0
        self._eventid = 0
        self._conmuni_direct = []
        self._hasdynamiclenght = False
        self._signalsreftype = ""  #“GROUP”   “SIGNALS”
        self._userevent = False


    @property
    def name(self):
        return self._name

    @property
    def serviceid(self):
        return self._serviceid

    def set_serviceid(self,serviceid):
        self._serviceid = serviceid

    @property
    def eventid(self):
        return self._eventid

    @property
    def eventid_hex(self):
        return hex(self._eventid)

    def set_eventid(self, eventid):
        self._eventid = eventid

    @property
    def hasdynamiclenght(self):
        return self._hasdynamiclenght

    def set_hasdynamiclenght(self, hasdynamiclenght):
        self._hasdynamiclenght = hasdynamiclenght

    @property
    def userevent(self):
        return self._userevent

    def set_userevent(self, userevent):
        self._userevent = userevent

    @property
    def signalsreftype(self):
        return self._signalsreftype

    def set_signalsreftype(self, signalsreftype):
        self._signalsreftype = signalsreftype

    @property
    def typeitems(self):
        return self._typeitems



    def add_typeitem(self, typeitem:Signaltypeitem):
        # if self._hasdynamiclenght:
        #     if typeitem.type =="Array":
        #         self._typeitems.append(typeitem)
        #     else:
        #         self._typeitems.insert(0,typeitem)
        # else:
        if self.signalsreftype == "GROUP":

            offset = 0
            for typeitem in self.typeitems:
                offset = offset+typeitem.length
            typeitem.set_start_position(offset)
        else:
            self._typeitems.append(typeitem)
            self._typeitems.sort(key=lambda typeitem: typeitem.start_position)

    def add_conmuni_direct(self, direct):
        if direct not in self._conmuni_direct:
            self._conmuni_direct.append(direct)