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
from generator.views.model_to_view import ViewManipulations

from generator.intermediate_model.communication_management.method import Argument


class ArgumentView(View):
    """Specialized View of a method argument"""

    def __init__(self, argument: Argument) -> None:
        self._argument = argument
        self.type_view = ViewManipulations.convert_to_view_tree(argument.impl_type)
        super().__init__(argument)

    def __str__(self):
        """To make debugging easier"""
        return self.type_declaration

    def __repr__(self):
        return self.__str__()

    @property
    def type_declaration(self):
        """
        Get the full type declaration including namespace of
        the impl type of this argument.
        """
        return self.type_view.qualified_cpp_name

    @property
    def type_declaration_fastdds(self):
        """
        Get the full type declaration including namespace of
        the impl type of this argument.
        """
        return self.type_view.qualified_cpp_name_fastdds

    @property
    def type_include(self):
        """
        Get the include statement for the type declared in the argument.
        """
        return self.type_view.self_include
