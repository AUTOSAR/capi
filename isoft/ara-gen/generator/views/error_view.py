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
ErrorView corresponding to a Service possible error object.
"""

from generator.views.view import View
from generator.views.argument_view import ArgumentView
from generator.intermediate_model.communication_management.application_error_exception import ApplicationErrorException


class ErrorView(View):
    """Specialized View of a possible error in a method"""

    def __init__(self, error: ApplicationErrorException) -> None:
        self._error = error
        super().__init__(error)

    def __lt__(self, other):
        return self._error.fqn < other._error.fqn

    def __eq__(self, other):
        if type(other) is ErrorView:
            return self._error.fqn == other._error.fqn
        return False

    def __hash__(self):
        return hash(self._error.fqn)

    @property
    def error_contexts(self):
        """Get the error contexts wrapped in ArgumentViews"""
        error_contexts = []
        for error_context in self._error.error_contexts:
            error_contexts.append(ArgumentView(error_context))
        return error_contexts

    @property
    def has_non_default_constr(self):
        """Returns true if the error has any error context references"""
        return len(self._error.error_contexts) > 0

    @property
    def error(self):
        """Get the error wrapped in this view for comparison"""
        return self._error
