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
Method class.
"""
# Unused import check suppressed, because only used in type annotation, which is not found by PyLint
from typing import List  # pylint: disable=unused-import
from .application_error_exception import ApplicationErrorException  # pylint: disable=unused-import

from ..types.impl_type import ImplDataType
from .transformationprops import TransformationProps

class ErrorContext(object):
    """Describes an Argument within a method"""

    def __init__(self, impl_type: ImplDataType, referable_fqn: str) -> None:
        self._impl_type = impl_type
        self._referable_fqn = referable_fqn
        self.deployment = None


    def __str__(self):
        """To make debugging easier"""
        return "ErrorContext with fqn: {}".format(self._referable_fqn)

    def __repr__(self):
        return self.__str__()

    @property
    def referable_fqn(self):
        return self._referable_fqn

    @property
    def impl_type(self):
        """Get the implmentation type of the Argument."""
        return self._impl_type

    @property
    def name(self):
        """Get the name of the Argument."""
        return self._referable_fqn.split("/")[-1]


class Argument(object):
    """Describes an Argument within a method"""

    def __init__(self, name: str, impl_type: ImplDataType, arg_is_error: bool) -> None:
        self._name = name
        self._impl_type = impl_type
        self._is_error = arg_is_error

    def __str__(self):
        """To make debugging easier"""
        return "Argument {} for Method {}".format(self._name, self._method_fqn)

    def __repr__(self):
        return self.__str__()

    @property
    def name(self):
        """Get the name of the Argument."""
        return self._name

    @property
    def impl_type(self):
        """Get the implmentation type of the Argument."""
        return self._impl_type

    @property
    def is_error(self):
        """Is this an output arg mentioned in an error context"""
        return self._is_error

class Method(object):
    """Describes a Method within a service"""
    def __init__(self, name: str) -> None:
        if name == "":
            raise AttributeError("A Method must have a name")
        self._name = name
        self._input_arguments = []  # type: List[Argument]
        self._output_arguments = []  # type: List[Argument]
        self._errors = []  # type: List[ApError]
        self._deployment = None
        self.is_fire_and_forget = None
        self._transformationprops = None
        self.isArgsTag = False

    def __str__(self):
        """To make debugging easier"""
        return "Method: " + self.name

    def __repr__(self):
        return self.__str__()

    @property
    def name(self):
        """Get the name of the Method."""
        return self._name

    @property
    def input_arguments(self):
        """Get the Method's input arguments."""
        return self._input_arguments

    @property
    def output_arguments(self):
        """Get the Method's input arguments."""
        return self._output_arguments

    def add_argument(self, arg: Argument, direction):
        if direction == 'in':
            self._input_arguments.append(arg)
        elif direction == 'out':
            self._output_arguments.append(arg)
        elif direction == 'inout':
            self._input_arguments.append(arg)
            self._output_arguments.append(arg)

    @property
    def errors(self):
        return self._errors

    def add_error(self, error):
        if error is not None and error not in self._errors:
            self._errors.append(error)

    @property
    def transformationprops(self):
        return self._transformationprops

    def set_transformationprops(self, transformationprops):
        self._transformationprops = transformationprops