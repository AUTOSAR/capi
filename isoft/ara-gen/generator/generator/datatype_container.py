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
View class for collecting data for used data types, both basetypes
and implementation data types.
"""

from typing import List


class DataTypeContainer(object):
    """
    Collects views to provide information for generating data type header files
    """
    def __init__(self, interfaces: List) -> None:
        self._interfaces = interfaces
        self._types_set = []
        self._get_types()

    @property
    def types(self):
        return self._types_set

    def _update_types_set(self, view):
        if view.to_be_generated:
            if view in self._types_set:
                viewName = view.__class__.__name__
                if viewName == "StructImplTypeView":
                    isTlv = view.is_tlv
                    if isTlv:
                        for index, viewItem in enumerate(self._types_set):
                            if viewItem == view:
                                self._types_set[index] = view
                                break
            else:
                for v in view.referred_views:
                    self._update_types_set(v)
                self._types_set.append(view)

    def _get_types(self):
        for interface in self._interfaces:
            for tv in interface.type_views:
                self._update_types_set(tv)
