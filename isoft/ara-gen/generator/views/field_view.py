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

from generator.views.view import View
from generator.views.model_to_view import ViewManipulations

from generator.intermediate_model.communication_management.field \
    import Field


class FieldView(View):
    """Specialized View of an field."""

    def __init__(self, field: Field) -> None:
        self._field = field
        self.type_view = ViewManipulations.convert_to_view_tree(field.impl_type)
        super().__init__(field)

    def __str__(self):
        """To make debugging easier"""
        return "<{}>".format(self.name)

    def __repr__(self):
        return self.__str__()

    @property
    def has_setter(self):
        """Returns true if this Field has a setter method"""
        return self._field.has_setter

    @property
    def has_getter(self):
        """Returns true if this Field has a getter method"""
        return self._field.has_getter

    @property
    def has_notifier(self):
        """Returns true if this Field has a has_notifier event"""
        return self._field.has_notifier

    @property
    def get_field(self):
        """Get the field from the FieldView"""
        return self._field

    @property
    def type_include(self):
        """Get the include statement for the type used in the event."""
        return self.type_view.self_include

    @property
    def get_impl_type_name(self):
        """Get the impltype string"""
        return self.type_view.qualified_cpp_name

    @property
    def init_value(self):
        return self.type_view.init_value

    @property
    def type_declaration(self):
        """
        Get the full type declaration including namespace of
        the impl type of this field.
        """
        return self.type_view.qualified_cpp_name

    @property
    def type_declaration_fastdds(self):
        """
        Get the full type declaration including namespace of
        the impl type of this event.
        """
        return self.type_view.qualified_cpp_name_fastdds
    @property
    def get_mutable(self):
        """Returns "Mutable" if the field is mutable (has a setter) """
        if self._field.has_setter:
            return "Mutable"
        return ""

    @property
    def get_fieldtype(self):

        if self.has_notifier and not self._field.has_setter and not self._field.has_getter:
            return "N"
        if not self.has_notifier and self._field.has_setter and not self._field.has_getter:
            return "S"
        if not self.has_notifier and not self._field.has_setter and self._field.has_getter:
            return "G"
        if self.has_notifier and self._field.has_setter and not self._field.has_getter:
            return "NS"
        if not self.has_notifier and self._field.has_setter and self._field.has_getter:
            return "SG"
        if self.has_notifier and not self._field.has_setter and self._field.has_getter:
            return "NG"
        if self.has_notifier and self._field.has_setter and self._field.has_getter:
            return "NSG"

    @property
    def firstlowername(self):
        return self.name[:1].lower() + self.name[1:]

