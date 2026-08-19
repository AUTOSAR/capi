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


class TransformationProps(object):
    """Describes an Event in a Service"""

    def __init__(self):

        self._byteorder = ""#"MOST-SIGNIFICANT-BYTE-FIRS"
        self._sessionhandling =""# "SESSION-HANDLING-ACTIVE"

        self._alignment = None
        self._implementslegacyStringSerialization = None
        self._isDynamicLengthFieldsize = None
        self._sizeofArrayLengthField = None
        self._sizeofFixedArrayLengthField = None
        self._sizeofFixedStringLengthField = None
        self._sizeofstringLengthField = None
        self._sizeofstructLengthField = None
        self._sizeofUnionLengthField = None
        self._sizeofUnionTypeselectorField = None
        self._stringEncoding = None
        self.data_ids = []
        self.date_ref = None  # Event Method Filed fqn


    @property
    def byteorder(self):
        if self._byteorder != "":
            return "k" + self.gethumpString(self._byteorder)
        else:
            return self.gethumpString(self._byteorder)

    @property
    def sessionhandling(self):
        if self._sessionhandling != "":
            return "k" + self.gethumpString(self._sessionhandling)
        else:
            return self.gethumpString(self._sessionhandling)



    def set_byteorder(self,byteorder):
        self._byteorder = byteorder


    def set_sessionhandling(self,sessionhandling):
        self._sessionhandling = sessionhandling

    def gethumpString(self,value):
        list = value.split("-")
        result = ""
        for item in list:
            result = result+item.capitalize()
        return result

    @property
    def alignment(self):
        return self._alignment
    def set_alignment(self,alignment ):
        self._alignment = alignment

    @property
    def implementslegacyStringSerialization(self):
        return self._implementslegacyStringSerialization

    def set_implementslegacyStringSerialization(self, implementslegacyStringSerialization):
        self._implementslegacyStringSerialization = implementslegacyStringSerialization

    @property
    def isDynamicLengthFieldsize(self):
        return self._isDynamicLengthFieldsize

    def set_isDynamicLengthFieldsize(self, isDynamicLengthFieldsize):
        self._isDynamicLengthFieldsize = isDynamicLengthFieldsize

    @property
    def sizeofArrayLengthField(self):
        return self._sizeofArrayLengthField

    def set_sizeofArrayLengthField(self, sizeofArrayLengthField):
        self._sizeofArrayLengthField = sizeofArrayLengthField

    @property
    def sizeofFixedArrayLengthField(self):
        return self._sizeofFixedArrayLengthField

    def set_sizeofFixedArrayLengthField(self, sizeofFixedArrayLengthField):
        self._sizeofFixedArrayLengthField = sizeofFixedArrayLengthField

    @property
    def sizeofFixedStringLengthField(self):
        return self._sizeofFixedStringLengthField

    def set_sizeofFixedStringLengthField(self, sizeofFixedStringLengthField):
        self._sizeofFixedStringLengthField = sizeofFixedStringLengthField

    @property
    def sizeofstringLengthField(self):
        return self._sizeofstringLengthField

    def set_sizeofstringLengthField(self, sizeofstringLengthField):
        self._sizeofstringLengthField = sizeofstringLengthField
    @property
    def sizeofstructLengthField(self):
        return self._sizeofstructLengthField

    def set_sizeofstructLengthField(self, sizeofstructLengthField):
        self._sizeofstructLengthField = sizeofstructLengthField

    @property
    def sizeofUnionLengthField(self):
        return self._sizeofUnionLengthField

    def set_sizeofUnionLengthField(self, sizeofUnionLengthField):
        self._sizeofUnionLengthField = sizeofUnionLengthField

    @property
    def sizeofUnionTypeselectorField(self):
        return self._sizeofUnionTypeselectorField

    def set_sizeofUnionTypeselectorField(self, sizeofUnionTypeselectorField):
        self._sizeofUnionTypeselectorField = sizeofUnionTypeselectorField

    @property
    def stringEncoding(self):
        if self._stringEncoding is not None:
            return "k" + self._stringEncoding
        return self._stringEncoding

    def set_stringEncoding(self, stringEncoding):
        self._stringEncoding = stringEncoding