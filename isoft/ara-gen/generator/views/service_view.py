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

import os
import hashlib

from generator.common.tree_helper import transition_number
from generator.intermediate_model.communication_management.service \
    import Service
from generator.parser.commen_util import get_hash
from generator.views.view import View
from generator.views.error_view import ErrorView
from generator.views.event_view import EventView
from generator.views.field_view import FieldView
from generator.views.method_view import MethodView


CPP_SCOPE_RES_OPERATOR = "::"
CPP_UNDERSCORE_DELIMITER = "_"
CPP_INCLUDE_PATH_SEPARATOR = "/"

class ServiceView(View):
    """Specialized View of a Service."""

    def __init__(self, service: Service) -> None:
        self._service = service
        super().__init__(service)

    def __eq__(self, other):
        return self.fqn == other.fqn

    def __lt__(self, other):
        return self.qualified_cpp_name.lower() < other.qualified_cpp_name.lower()

    def __hash__(self):
        return hash(self.fqn)

    def __str__(self):
        return "ServiceView({})".format(self.fqn)

    def __repr__(self):
        return self.__str__()

    @property
    def absolute_namespace(self):
        """Get the global scoped namespace of this ServiceView."""
        if self.namespaces is not None:
            return CPP_SCOPE_RES_OPERATOR.join(self.namespaces)

    @property
    def service_id(self):
        fqn = self.fqn
        result = get_hash(fqn)
        return hex(result)

    @property
    def events(self):
        """Get the events as EventViews for this service"""
        event_views = list()
        for event in self._service.events:
            event_views.append(EventView(event))
        return event_views

    @property
    def methods(self):
        """Get the methods as MethodViews for this service"""
        method_views = list()
        for method in self._service.methods:
            method_views.append(MethodView(method))
        return method_views

    @property
    def has_fire_and_forget_method(self):
        for method in self._service.methods:
            if method.is_fire_and_forget:
                return True
        return False

    @property
    def has_regular_method(self):
        for method in self._service.methods:
            if not method.is_fire_and_forget:
                return True
        return False

    @property
    def all_fields(self):
        """Get the fields as FieldViews for this service"""
        field_views = list()
        for field in self._service.fields:
            field_views.append(FieldView(field))

        return field_views

    @property
    def type_views(self):
        types = []
        types += [event.type_view for event in self.events]
        types += [field.type_view for field in self.all_fields]
        types += [arg.type_view for method in self.methods for arg in method.get_args]
        return types

    @property
    def includes(self):
        """Get the include statements required by the service interface"""
        includes = set()

        args = []
        for method in self.methods:
            args.extend(method.get_args)

        for arg in args:
            includes.add(arg.type_include)

        for event in self.events:
            includes.add(event.type_include)

        for field in self.all_fields:
            includes.add(field.type_include)

        #filter out empty includes (for bool etc.)
        include_list = list(filter(None, includes))
        include_list.sort()

        error_domains_includes = set()
        for method in self.methods:
            for error_domain in method.error_domains:
                errorpath = ""
                for p in error_domain.namespace:
                    errorpath = errorpath+p+"/"
                if error_domain.include_name.find(errorpath) !=-1:
                    errorpath =  error_domain.include_name
                else:
                    errorpath = '"' +errorpath+ error_domain.include_name.replace('"','')+'"'
                error_domains_includes.add(errorpath)

        error_domains_includes_list = list(error_domains_includes)
        error_domains_includes_list.sort()

        return error_domains_includes_list + include_list

    @property
    def possible_errors(self):
        """Get the possible errors for this service"""
        possible_errors = []
        for error in self._service.errors:
            if error is not None:
                possible_errors.append(ErrorView(error))
        return sorted(possible_errors)

    def get_error_method_name(self, error):
        """Get the name of the method which is referencing this error"""
        if error.error.error_contexts:
            error_context_fqn = error.error.error_contexts[0].referable_fqn
            method_name = error_context_fqn.split("/")[-2]
            return method_name
        return ""

    @property
    def major_version(self):
        if self._service.major_version is not None:
            return hex(int(self._service.major_version))
        return "0x1"

    @property
    def minor_version(self):
        if self._service.minor_version is not None:
            return hex(int(self._service.minor_version))
        return "0x0"

    @property
    def qualified_cpp_name(self):
        """
        Get the fully qualified cpp name of this service,
        IE namespace::name
        """
        fqn = []
        if self.namespaces is not None:
            fqn.extend(self.namespaces)
        fqn.append(self.name)
        return CPP_SCOPE_RES_OPERATOR.join(fqn)

    @property
    def event_impl_type_namespace(self):
        """Get the implementation data type namespace string for an event"""
        event_impl_namespaces = ""
        if self.namespaces is not None:
            event_impl_namespaces = CPP_SCOPE_RES_OPERATOR.join(self.namespaces) + CPP_SCOPE_RES_OPERATOR
        return CPP_SCOPE_RES_OPERATOR + event_impl_namespaces + "events" + CPP_SCOPE_RES_OPERATOR

    @property
    def namespacepath(self):
        """Returns the namespace string as a path with slashes"""
        path = ""
        filtered_namespaces = [x.lower() for x in self.namespaces if x.strip()]
        if filtered_namespaces:
            path = CPP_INCLUDE_PATH_SEPARATOR.join(filtered_namespaces)
        if path:
            path += CPP_INCLUDE_PATH_SEPARATOR
        return path

    @property
    def packagepath(self):
        """to be removed once migration to this aragen is complete"""
        result = self.fqn
        parts = result.split("/")
        parts_modified = [x.lower() for x in parts[:-1]] + [parts[-1]]
        result = "/".join(parts_modified)
        return result

    @property
    def service_mapping_name(self):
        return "__".join(self.namespaces + [self.standard_name] + ["mapping"])
    @property
    def firstlowername(self):
        return self.name[:1].lower() + self.name[1:]

    @property
    def changeto_andlowername(self):
        result = ""
        first = False
        for s in self.name:
            if s.isupper() and first:
                result = result + "_" + s.lower()
            else:
                result = result + s.lower()
                first = True
        return result