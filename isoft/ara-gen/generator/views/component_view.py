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
"A view to with convenience methods for generating information about a component"
"""
import os

from generator.views.view import View
from generator.intermediate_model.execution_management.component import Component
from generator.views.service_view import ServiceView
from generator.views.error_domain_view import ErrorDomainView
from generator.views.kvs_interface_view import KvsInterfaceView


class ComponentView(View):
    """Specialized View of a SoftwareComponent"""
    def __init__(self, component: Component) -> None:
        self._component = component
        self._required_services = list(set([ServiceView(x) for x in self._component.required_services]))
        self._provided_services = list(set([ServiceView(x) for x in self._component.provided_services]))
        self._services = list(set(self._required_services + self._provided_services))

        all_error_domains = set()
        for service in self._services:
            all_error_domains.update(service.error_domains)
        self._error_domain_views = list(set([ErrorDomainView(x) for x in all_error_domains]))

        super().__init__(component)

    @property
    def get_component(self):
        """Get the component in the view"""
        return self._component

    @property
    def include_list(self):
        """Get the include files list for the service_mapping template"""
        proxies_list = []
        for rs in self._required_services:
            ns = rs.namespaces
            if ns is None:
                ns = []
            include_string = "/".join(ns + ["proxy_impl_" + rs.standard_name + ".h"])
            proxies_list += [include_string]

        adapters_list = []
        for ps in self._provided_services:
            ns = ps.namespaces
            if ns is None:
                ns = []
            include_string = "/".join(ns + ["adapter_" + ps.standard_name + ".h"])
            adapters_list += [include_string]

        return sorted(proxies_list) + sorted(adapters_list)

    def service_is_required(self, serviceview: ServiceView):
        # casting needed to use comparators from ServiceView class
        sv = ServiceView(serviceview.model_object)
        return sv in self._required_services

    def service_is_provided(self, serviceview: ServiceView):
        sv = ServiceView(serviceview.model_object)
        return sv in self._provided_services


    @property
    def services_list(self):
        """Get the service list for the service_mapping template"""
        return sorted(self._services)

    def set_services_list(self, services):
        self._services = services

    @property
    def error_domains(self):
        return sorted(self._error_domain_views)


    @property
    def kvs_interfaces(self):
        return sorted([KvsInterfaceView(kvs_interface) for kvs_interface in self._component.persistency_interfaces])

    @property
    def diagelement_services(self):
        return list(set([ServiceView(x) for x in self._component.diag_element_services]))

    @property
    def diagidentifier_services(self):
        return list(set([ServiceView(x) for x in self._component.diag_identifier_services]))

    @property
    def diagroutine_services(self):
        return list(set([ServiceView(x) for x in self._component.diag_routine_services]))

    @property
    def phm_se_interfaces(self):
        return self._component.phm_supervisedentity_interfaces

    @property
    def phm_hc_interfaces(self):
        return self._component.phm_healthchannel_interfaces

    @property
    def phm_ser_notifical_interfaces(self):
        return self._component.phm_sernotifical_interfaces

    @property
    def phm_hcr_notifical_interfaces(self):
        return self._component.phm_hcrnotifical_interfaces

