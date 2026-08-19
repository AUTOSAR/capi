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
ServiceView corresponding to a Service IM object.
"""
from generator.intermediate_model.communication_management.service \
    import Service
from generator.views.service_instance_view import ServiceInstanceView
from generator.views.someip.event_group_view import EventGroupView
from generator.views.someip.event_view import EventView
from generator.views.someip.field_view import FieldView
from generator.views.someip.method_view import MethodView
from generator.common.tree_helper import transition_number

class SomeIpServiceView(ServiceInstanceView):
    """Specialized View of a Service."""

    # To be able to generate all templates we need >20 public methods in ServiceView.
    # pylint: disable=R0904
    # - Too many public methods

    def __init__(self, service: Service) -> None:
        super().__init__(service)
        self._forsignal = False


    @property
    def instance_id(self):
        return "NSOMEIP:{}".format(self._service.service_deployment.instance_id)

    @property
    def someip_specific_instance_id(self):
        return transition_number(self._service.service_deployment.instance_id, change=16)

    @property
    def someip_specific_instance_id_(self):
        return transition_number(self._service.service_deployment.instance_id)

    @property
    def deployment_id(self):
        return transition_number(self._service.service_deployment.deployment_id,change=16)

    @property
    def deployment_id_(self):
        return transition_number(self._service.service_deployment.deployment_id)

    @property
    def deployment(self):
        return self._service.service_deployment

    @property
    def major_version_(self):
        return transition_number(self._service.service_deployment.major_version)

    @property
    def minor_version_(self):
        return transition_number(self._service.service_deployment.minor_version)

    @property
    def ports(self):
        """Get the service port"""
        return self._service.service_deployment.ports

    @property
    def events(self):
        """Get the events as EventViews for this service"""
        event_views = list()
        for event in self._service.events:
            event_views.append(EventView(event))
        return event_views

    @property
    def signalevent(self):
        event_views = list()
        for event in self._service.events:
            if event.event_deployment.serializer == 'SIGNAL-BASED':
                event_views.append(EventView(event))
        return event_views

    @property
    def unsignalevent(self):
        event_views = list()
        for event in self._service.events:
            if event.event_deployment.serializer != 'SIGNAL-BASED':
                event_views.append(EventView(event))
        return event_views

    def get_events_from_group(self):
        """Get all the events in this event group"""
        event_views = self.events
        filtered = list()
        for event_group in self.event_groups:
            for event in event_views:
                if event.deployment_id in event_group.events:
                    if event in filtered:
                        event.event_group_id.append(event_group.event_group_id)
                    else:
                        event.event_group_id.append(event_group.event_group_id)
                        filtered.append(event)
        return filtered

    @property
    def methods(self):
        """Get the methods as MethodViews for this service"""
        method_views = list()
        for method in self._service.methods:
            method_views.append(MethodView(method))
        return method_views

    @property
    def all_fields(self):
        """Get the fields as FieldViews for this service"""
        field_views = list()
        for field in self._service.fields:
            field_views.append(FieldView(field))

        return field_views

    @property
    def fields_with_notifier(self):
        """Get the fields with notifying events as FieldViews for this service"""
        field_views = list()
        for field in self._service.fields:
            if field.has_notifier:
                field_views.append(FieldView(field))

        field_views.sort(key=lambda field: field.name)
        return field_views

    def get_field_event_group(self, field: FieldView):
        """Get the field's event group if available"""
        result = []
        if field.has_notifier:
            for event_group in self._service.service_deployment.event_groups:
                if int(field.event_id, 16) in event_group.events:
                    result.append(event_group.event_group_id)
            if result:
                return result
            raise ValueError("Event '{}' ({}) does not belong to any EVENT-GROUP".format(field.name, field.event_id))
        else:
            return None

    @property
    def event_groups(self):
        """Get the eventgroups for the service"""
        event_group_views = list()
        for event_group in self._service.service_deployment.event_groups:
            event_group_views.append(EventGroupView(event_group))
        return event_group_views

    @property
    def service_version_major(self):
        """Get the major version of the service"""
        if self._service.service_deployment.major_version is not None:
            return hex(int(self._service.service_deployment.major_version))
        return "0x00"

    @property
    def service_version_minor(self):
        """Get the minor version of the service"""
        if self._service.service_deployment.minor_version is not None:
            return hex(int(self._service.service_deployment.minor_version))
        return "0x00000000"

    @property
    def blacklisted_versions(self):
        """Get the blacklisted versions of the service."""
        key = "blacklisted_versions"
        if self._service.service_deployment.required is not None:
            if key in self._service.service_deployment.required:
                if self._service.service_deployment.required[key] is not None:
                    return list(map(lambda bl_pair:
                                ({"major": "0xFF"
                                , "minor": bl_pair["minor"]}
                                if bl_pair["major"] is None
                                else bl_pair),
                                self._service.service_deployment.required[key]))
        return []

    @property
    def required_minimum_minor_version(self):
        """Get required minimum minor version."""
        key = "required_minimum_minor_version"
        if self._service.service_deployment.required is not None:
            if key in self._service.service_deployment.required:
                required_minimum_minor_version = self._service.service_deployment.required[key]
                if required_minimum_minor_version is not None:
                    if(str(required_minimum_minor_version) == "ANY"):
                        return "0xFFFFFFFF"
                    return required_minimum_minor_version
        return "0xFFFFFFFF" # ANY minor is the default value

    @property
    def is_minimum_minor_policy_enabled(self):
        """Get version driven find behavior."""
        key = "version_driven_find_behavior"
        if self._service.service_deployment.required is not None:
            if key in self._service.service_deployment.required:
                version_driven_find_behavior = self._service.service_deployment.required[key]
                if version_driven_find_behavior is not None:
                    if version_driven_find_behavior == "MINIMUM-MINOR-VERSION":
                        return True
                    if version_driven_find_behavior == "EXACT-OR-ANY-MINOR-VERSION":
                        return False
        return False
    @property
    def forsignal(self):
        return self._forsignal

    def set_forsignal(self,forsignal):
        self._forsignal = forsignal
