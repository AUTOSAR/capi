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

from generator.views.view import View
from generator.intermediate_model.communication_management.error_domain import ApErrorDomain
from generator.intermediate_model.communication_management.error_domain import ApError

class ErrorDomainView(View):
    def __init__(self, error_domain: ApErrorDomain) -> None:
        super().__init__(error_domain)
    def __str__(self):
        return "<ErrorDomainView: {0} {1} (errors: {2})>".format(self.namespace, self.name, len(self.errors))
    def __repr__(self):
        return self.__str__()

    def __lt__(self, other):
        return self.qualified_cpp_name.lower() < other.qualified_cpp_name.lower()

    def __eq__(self, other):
        if type(other) is ErrorDomainView:
            return self.qualified_cpp_name.lower() == other.qualified_cpp_name.lower()
        return False

    def __hash__(self):
        return hash(self.qualified_cpp_name.lower())

    @property
    def qualified_cpp_name(self) -> str:
        return "::".join([self.qualified_cpp_namespace_prefix] + [self.name])

    @property
    def qualified_cpp_namespace_prefix(self) -> str:
        return "::".join(self.namespace)

    @property
    def include_name(self):
        errorpath = ""
        for p in self.namespace:
            errorpath = errorpath + p + "/"
        errorpath = '"' + errorpath + 'error_domain_{0}.h"'.format(self.standard_name)
        return errorpath
       # return '"error_domain_{0}.h"'.format(self.standard_name)

class ApErrorView(View):
    def __init__(self, error: ApError) -> None:
        super().__init__(error)
        self._domain_view = ErrorDomainView(error.domain)

    def __str__(self):
        return "<ApErrorView: {0}>".format(self.name)
    def __repr__(self):
        return self.__str__()

    @property
    def qualified_cpp_name(self) -> str:
        return "::".join([self.qualified_cpp_namespace_prefix] + [self.name])

    @property
    def qualified_cpp_namespace_prefix(self) -> str:
        return self._domain_view.qualified_cpp_namespace_prefix
