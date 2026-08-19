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
FieldView corresponding to a Field IM object.
"""
from generator.common.tree_helper import transition_number
from generator.views.field_view import FieldView
from generator.views.model_to_view import ViewManipulations

from generator.intermediate_model.communication_management.field \
    import Field


class FieldView(FieldView):
    """Specialized View of an field."""

    def __init__(self, field: Field) -> None:
        super().__init__(field)

    @property
    def get_method_id(self):
        """Get the getter method id if the Field has a getter method"""
        retval = ""
        if self._field.has_getter:
            if self._field.deployment["getter"]:
                retval = hex(int(self._field.deployment["getter"]["id"]))
            else:
                raise ValueError("Field '{}' declared as having getter, but no deployment info on it".format(self._field.name))
        return retval

    @property
    def event_id(self):
        """Gets the  event id if this Field has a notifier"""
        retval = ""
        if self._field.has_notifier:
            if self._field.deployment["notifier"]:
                retval = hex(int(self._field.deployment["notifier"]["id"])+32768)
            else:
                raise ValueError("Field '{}' declared as having notifier, but no deployment info on it".format(self._field.name))
        return retval

    @property
    def has_reliable_notifier(self):
        """Gets the  event id if this Field has a reliable notifier"""
        if self._field.has_notifier:
            if self._field.deployment["notifier"]:
                proto = str(self._field.deployment["notifier"]["proto"])
                return proto.upper() == 'TCP'
            else:
                raise ValueError("Field '{}' declared as having notifier, but no deployment info on it".format(self._field.name))
        return False

    @property
    def has_reliable_getter(self):
        """Checks if this Field has a reliable getter"""
        if self._field.has_getter:
            if self._field.deployment["getter"]:
                proto = str(self._field.deployment["getter"]["proto"])
                return proto.upper() == 'TCP'
            else:
                raise ValueError("Field '{}' declared as having getter, but no deployment info on it".format(self._field.name))
        return False

    @property
    def has_reliable_setter(self):
        """Checks if this Field has a reliable setter"""
        if self._field.has_setter:
            if self._field.deployment["setter"]:
                proto = str(self._field.deployment["setter"]["proto"])
                return proto.upper() == 'TCP'
            else:
                raise ValueError("Field '{}' declared as having setter, but no deployment info on it".format(self._field.name))
        return False

    @property
    def set_method_id(self):
        """Get the setter method id if the Field has a setter method"""
        retval = ""
        if self._field.has_setter:
            if self._field.deployment["setter"]:
                retval = hex(int(self._field.deployment["setter"]["id"]))
            else:
                raise ValueError("Field '{}' declared as having setter, but no deployment info on it".format(self._field.name))
        return retval

    @property
    def notifier_id(self):
        retval = self.event_id
        if retval != "":
            return transition_number(retval)
        else:
            return None

    @property
    def setter_id(self):
        retval = self.set_method_id
        if retval != "":
            return transition_number(retval)
        else:
            return None

    @property
    def getter_id(self):
        retval = self.get_method_id
        if retval != "":
            return transition_number(retval)
        else:
            return None

    @property
    def filed_getter(self):
        if self._field.deployment is not None and "getter" in self._field.deployment:
            return self._field.deployment["getter"]
        return None

    @property
    def field_setter(self):
        if self._field.deployment is not None and "setter" in self._field.deployment:
            return self._field.deployment["setter"]
        return None

    @property
    def field_notifier(self):
        if self._field.deployment is not None and "notifier" in self._field.deployment:
            return self._field.deployment["notifier"]
        return None

