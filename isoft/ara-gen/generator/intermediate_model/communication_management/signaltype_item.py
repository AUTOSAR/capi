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
type_item class.
"""




class Signaltypeitem(object):

    def __init__(self, name,signaltype,length):

        self._is_signal = True
        self._name=name
        self._signaltype = signaltype
        self._length = length
        self._packing_byte_order = ""
        self._start_position = 0
        self._eventnamevalue = ""#userevent时有效

    @property
    def name(self):
        return self._name

    @property
    def type(self):
        return self._signaltype

    @property
    def value(self):
        return self._length

    @property
    def length(self):
        return  self._length

    @property
    def is_signal(self):
        return self._is_signal

    def set_is_signal(self,value):
        self._is_signal = value

    @property
    def packing_byte_order(self):
        return self._packing_byte_order

    def set_packing_byte_order(self, order):
        self._packing_byte_order = order

    @property
    def start_position(self):
        return self._start_position

    def set_start_position(self, pos):
        self._start_position = pos

    @property
    def eventnamevalue(self):
        return self._eventnamevalue

    def set_eventnamevalue(self, eventnamevalue):
        self._eventnamevalue = eventnamevalue

    @property
    def byteordertype(self):
        if self._packing_byte_order == "MOST-SIGNIFICANT-BYTE-FIRST":
            return "true"
        if self._packing_byte_order == "MOST-SIGNIFICANT-BYTE-LAST":
            return "false"
        else:
            return "true"
    @property
    def datatype(self):

        if  self._signaltype == "ARRAY":
            return " ara::core::String"
        if self._signaltype == "PRIMITIVE":
            if self._length == 1:
                return "bool"
            if self._length == 8:
                return "uint8_t"
            if self._length == 16:
                return "uint16_t"
            if self._length == 32:
                return "uint32_t"
            if self._length == 64:
                return "uint64_t"


