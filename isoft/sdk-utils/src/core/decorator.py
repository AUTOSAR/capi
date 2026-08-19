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
# @file       decorator.py
# @brief      Collection of decorators
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

import logging
import inspect

__all__ = ["handle_exception", "handle_func_exception"]

# Function decorator for automatically handling exceptions, with configurable return value on exception
def handle_func_exception(result, verbose=True):
    def _decorator(func):
        def wrapper(*args, **kwargs):
            try:
                return func(*args, **kwargs)
            except Exception as e:
                frame = inspect.currentframe().f_back
                filename = inspect.getframeinfo(frame).filename
                lineno = inspect.getframeinfo(frame).lineno
                if verbose:
                    logging.error(f"Exeception in {filename}:{lineno}[{func.__name__}()] -> {e}")
                #print("{file}:{line}->{fun}():{err}".format(file=__file__, fun=sys._getframe().f_code.co_name, line=sys._getframe().f_lineno, err=e))
                return result
        return wrapper
    return _decorator


"""
# Class decorator for automatically handling exceptions; returns None by default on exception
def handle_exception_cls(result, verbose=True):
    def _decorator(cls):
        def _handle_exception(func, verbose=True):
            def wrapper(*args, **kwargs):
                try:
                    return func(*args, **kwargs)
                except Exception as e:
                    frame = inspect.currentframe().f_back
                    filename = inspect.getframeinfo(frame).filename
                    lineno = inspect.getframeinfo(frame).lineno
                    if verbose:
                        logging.error(f"Exeception in {filename}:{lineno}[{func.__name__}()] -> {e}")
                    #print("{file}:{line}->{fun}():{err}".format(file=__file__, fun=sys._getframe().f_code.co_name, line=sys._getframe().f_lineno, err=e))
                    return result
            return wrapper
        for name, value in vars(cls).items():
            if callable(value):
                setattr(cls, name, _handle_exception(value))
        return cls
    return _decorator
"""

"""
@brief Exception handling decorator, used to automatically add try...except to functions or class member functions to catch exceptions
@param result - Specifies the default return value when an exception occurs
@param verbose - Whether to print exception information

"""
def handle_exception(result, verbose=False):
    # Function wrapper
    def _function_wrapper(func):
        def wrapper(*args, **kwargs):
            try:
                return func(*args, **kwargs)
            except Exception as e:
                frame = inspect.currentframe().f_back
                filename = inspect.getframeinfo(frame).filename
                lineno = inspect.getframeinfo(frame).lineno
                if verbose:
                    logging.error(f"Exeception in {filename}:{lineno}[{func.__name__}()] -> {e}")
                    #print("{file}:{line}->{fun}():{err}".format(file=__file__, fun=sys._getframe().f_code.co_name, line=sys._getframe().f_lineno, err=e))
                return result
        return wrapper

    # Class wrapper
    def _class_wrapper(cls):
        for name, value in vars(cls).items():
            if callable(value):
                setattr(cls, name, _function_wrapper(value))
        return cls
    
    def _decorator(obj):
        if inspect.isfunction(obj):
            return _function_wrapper(obj)
        elif inspect.isclass(obj):
            return _class_wrapper(obj)
    return _decorator