#!/usr/bin/env python3

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

# ================================================================
#
# File description:
# ----------------
# @file       __init__.py
# @brief
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

from .misc import *
from .elf import *

# Dynamically define __all__
def _generate_all():
    # Get the global symbol table of the current module
    current_globals = globals()
    # Define filtering rules: for example, exclude names starting with an underscore and the module object itself
    excluded_items = {'__name__', '__doc__', '__package__', '__loader__', '__spec__', '__file__', '__cached__', '__builtins__'}
    # Generate the __all__ list: include keys that do not start with a single underscore and are not in the exclusion list
    generated_all = [name for name in current_globals
                     if not name.startswith('_') and name not in excluded_items]
    return generated_all

# Execute the function and assign the result to __all__
__all__ = _generate_all()
__version__ = "0.1.0"

del _generate_all