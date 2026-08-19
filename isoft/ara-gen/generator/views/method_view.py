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
MethodView corresponding to a Method IM object.
"""

from generator.views.view import View

from generator.intermediate_model.communication_management.method import Method
from generator.views.error_domain_view import ApErrorView
from generator.views.error_domain_view import ErrorDomainView
from generator.views.argument_view import ArgumentView


class MethodView(View):
    """Specialized View of a method."""

    def __init__(self, method: Method) -> None:
        self._method = method
        super().__init__(method)

    def __str__(self):
        """To make debugging easier"""
        return self.name

    def __repr__(self):
        return self.__str__()

    @property
    def get_method(self):
        """Get the method in the view"""
        return self._method

    @property
    def get_args(self):
        """Get all the input and output args of the method"""
        argument_views = []

        for arg in self._method.output_arguments:
            argument_views.append(self._make_argument_view(arg))

        for arg in self._method.input_arguments:
            argument_views.append(self._make_argument_view(arg))
        return argument_views

    @property
    def get_output_args(self):
        """Get the output arguments of the method"""
        argument_views = []
        for arg in self._method.output_arguments:
            if not arg.is_error:
                argument_views.append(self._make_argument_view(arg))
        return argument_views

    @property
    def get_input_args(self):
        """Get the input arguments of the method"""
        argument_views = []
        for arg in self._method.input_arguments:
            if not arg.is_error:
                argument_views.append(self._make_argument_view(arg))
        return argument_views

    def add_component(self, component):
        """Adds a component to the executables list of components"""
        self._components.append(component)

    @property
    def errors(self):
        error_views = []
        for error in self._method.errors:
            error_views += [ApErrorView(error)]
        # not sorted because we want preserve the order
        return error_views

    @property
    def error_domains(self):
        domains = set()
        for error in self._method.errors:
            domains.add(error.domain)
        domain_views = [ErrorDomainView(ed) for ed in domains]
        # sorted to ensure consistent appearance in generated files
        return sorted(domain_views)

    def _make_argument_view(self, arg):
        return ArgumentView(arg)

    @property
    def is_fire_and_forget(self):
        return self._method.is_fire_and_forget

    @property
    def firstlowername(self):
        return self.name[:1].lower() + self.name[1:]