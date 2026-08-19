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

from functools import wraps
import sys
import traceback
import logging

from generator.common.method_util import GLOBAL_JUDGE


def handle_exceptions(errors=(Exception, ), error_func=sys.exit,
                      error_string_template="{exception_text}{arguments}{named_arguments}"):
    def real_decorator(function):
        @wraps(function)
        def wrapper(*args, **kwargs):
            try:
                return function(*args, **kwargs)
            except errors as error:
                set_log_info(error, error_string_template, args, kwargs)
                return error_func(
                    1
                )
        return wrapper
    return real_decorator


def handle_method_exceptions(errors=(Exception, ), error_func=sys.exit,
                             error_string_template="{exception_text}{arguments}{named_arguments}"):
    def real_decorator(function):
        @wraps(function)
        def wrapper(self, *args, **kwargs):
            try:
                return function(self, *args, **kwargs)
            except errors as error:
                set_log_info(error, error_string_template, args, kwargs)
                return error_func(
                    1
                )
        return wrapper
    return real_decorator

def set_log_info(error, error_string_template, args, kwargs):
    log = logging.getLogger(__name__)
    if GLOBAL_JUDGE["debug"]:
        log.error(str(traceback.format_exc()))
        log.error(error_string_template.format(
            exception_text=error.args[0],
            arguments=args,
            named_arguments=kwargs
        ) + "\nara-gen commandline:" + " ".join(sys.argv))
    else:
        log.error(str(error))