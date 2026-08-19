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
TopicTypeView is the View for service topic type
"""

from generator.intermediate_model.types.impl_type import ImplDataType
from generator.intermediate_model.types.structure_impl_type import StructureImplDataType
from generator.intermediate_model.types.base_type import BaseType
from generator.views.dds.impl_type_view import StructImplTypeView

# @uptrace{SWS_CM_11016, 48cead4810057324ad1954261252432b4e0e8251}
class EventTopicTypeView(StructImplTypeView):
    def __init__(self, name,  data_type: ImplDataType, atKey, descPath) -> None:
        key = [("instance_id", BaseType("uint16_t", "cstdint"), False, None)]
        data = [("data", data_type, False, None)]
        elements = list(key + data)

        impltype = StructureImplDataType(name, None, elements, data_type.namespace)
        super().__init__(impltype)
        for sub_element in self._subelements:
            self.add_referred_view(sub_element[1])
        self.at_key = atKey
        self._eventDdsNameList = []
        self._desc_paths = [descPath]

    @property
    def key_name(self):
        return self._subelements[0][0]

    @property
    def key_type(self):
        return self._subelements[0][1]

    @property
    def data_name(self):
        return self._subelements[1][0]

    @property
    def data_type(self):
        return self._subelements[1][1]

    def set_event_dds_name(self, eventDdsName):
        if eventDdsName not in self._eventDdsNameList:
            self._eventDdsNameList.append(eventDdsName)

    @property
    def info_dds_names(self):
        return self._eventDdsNameList

    def set_desc_path(self, desc_path):
        if desc_path not in self._desc_paths:
            self._desc_paths.append(desc_path)

    @property
    def desc_paths(self):
        return self._desc_paths