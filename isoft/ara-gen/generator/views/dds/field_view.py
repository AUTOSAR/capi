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
from generator.views.dds.method_view import MethodView
from generator.views.field_view import FieldView
from generator.intermediate_model.communication_management.field import Field
from generator.views.dds.model_to_view import ViewManipulations

class FieldView(FieldView):
    """Specialized View of an field."""

    def __init__(self, field: Field) -> None:
        super().__init__(field)
        self.type_view = ViewManipulations.convert_to_view_tree(field.impl_type)

    @property
    def notifier_topic_name(self):
        retval = ""
        if self._field.has_notifier:
            if self._field.deployment.notifier is not None:
                retval = self._field.deployment.notifier.topic
            else:
                raise ValueError("Field '{}' declared as having notifier, but no deployment info on it".format(self._field.name))
        return retval

    @property
    def notifier_qos_profile(self):
        retval = ""
        if self._field.has_notifier:
            if self._field.deployment.notifier is not None:
                retval = self._field.deployment.notifier.qos_profile
            else:
                raise ValueError("Field '{}' declared as having notifier, but no deployment info on it".format(self._field.name))
        return retval

    @property
    def getter_request_topic_name(self):
        retval = ""
        if self._field.has_getter:
            if self._field.deployment.getter is not None:
                retval = self._field.deployment.getter.request_topic
            else:
                raise ValueError("Field '{}' declared as having getter, but no deployment info on it".format(self._field.name))
        return retval

    @property
    def getter_reply_topic_name(self):
        retval = ""
        if self._field.has_getter:
            if self._field.deployment.getter is not None:
                retval = self._field.deployment.getter.reply_topic
            else:
                raise ValueError("Field '{}' declared as having getter, but no deployment info on it".format(self._field.name))
        return retval

    @property
    def getter_qos_profile(self):
        retval = ""
        if self._field.has_getter:
            if self._field.deployment.getter is not None:
                retval = self._field.deployment.getter.qos_profile
            else:
                raise ValueError("Field '{}' declared as having getter, but no deployment info on it".format(self._field.name))
        return retval

    @property
    def setter_request_topic_name(self):
        retval = ""
        if self._field.has_setter:
            if self._field.deployment.setter is not None:
                retval = self._field.deployment.setter.request_topic
            else:
                raise ValueError("Field '{}' declared as having getter, but no deployment info on it".format(self._field.name))
        return retval

    @property
    def setter_reply_topic_name(self):
        retval = ""
        if self._field.has_setter:
            if self._field.deployment.setter is not None:
                retval = self._field.deployment.setter.reply_topic
            else:
                raise ValueError("Field '{}' declared as having getter, but no deployment info on it".format(self._field.name))
        return retval

    @property
    def setter_qos_profile(self):
        retval = ""
        if self._field.has_setter:
            if self._field.deployment.setter is not None:
                retval = self._field.deployment.setter.qos_profile
            else:
                raise ValueError("Field '{}' declared as having getter, but no deployment info on it".format(self._field.name))
        return retval

    def get_getter_input_type(self, service_id, namespace):
        name = service_id + "Field_Get" + self.name + "_In"
        elements = [("dummy", MethodView.get_unused_member_type(), False, None)]
        return MethodView.make_entry_type(namespace, name, elements)

    def get_setter_input_type(self, service_id, namespace):
        name = service_id + "Field_Set" + self.name + "_In"
        elements = [(self.name, self.type_view.model_object, False, None)]
        return MethodView.make_entry_type(namespace, name, elements)

    def get_getter_output_type(self, service_id, namespace):
        name = service_id + "Field_Get" + self.name + "_Out"
        elements = [("return_", self.type_view.model_object, False, None)]
        return MethodView.make_entry_type(namespace, name, elements)

    def get_setter_output_type(self, service_id, namespace):
        name = service_id + "Field_Set" + self.name + "_Out"
        elements = [("return_", self.type_view.model_object, False, None)]
        return MethodView.make_entry_type(namespace, name, elements)

    def get_getter_result_type(self, service_id, namespace, include):
        name = service_id + "Field_Get" + self.name + "_Result"
        return MethodView.make_result_type_forfiled(namespace, include, name, self.get_getter_output_type(service_id, namespace))

    def get_setter_result_type(self, service_id, namespace, include):
        name = service_id + "Field_Set" + self.name + "_Result"
        return MethodView.make_result_type_forfiled(namespace, include, name, self.get_setter_output_type(service_id, namespace))
