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
# @file       object.py
# @brief      Collection of base objects
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

import json
from core.decorator import handle_exception

import logging
import inspect

"""
APSDK metaclass, used to automatically handle exception information
"""
class AMeta(type):
    def __new__(cls, name, bases, attrs):
        for attr_name, attr_value in attrs.items():
            if callable(attr_value):
                attrs[attr_name] = cls.handle_exception(attr_value, result=None)
        return super().__new__(cls, name, bases, attrs)

    def handle_exception(func, result, verbose=True):
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


    
class ConfigClassMeta(type):
    def __new__(cls, name, bases, attrs):
        # Assume the dictionary is named _data in the class attributes
        function_dict = attrs.get('_data', {})
        print(function_dict)
        for dict_key, dict_value in function_dict.items():
            # Define a function; the function body simply prints the key and value
            def new_func(self, key=dict_key):
                return dict_value
            # Bind the new function to the class; the function name is the key of the dictionary
            attrs[dict_key] = new_func
        return super().__new__(cls, name, bases, attrs)


"""
Base configuration object class
Provides basic methods for object operations
"""
@handle_exception(False)
class ConfigObject:
    def __init__(self):
        self._data = {}

    def _add_function_from_dict_key(self):
        for dict_key, dict_value in self._data.items():
            @property
            def getter(self, key=dict_key):
                return self.get(key)
            setattr(self._class__, dict_key, getter)


    """
    Get the value for the specified KEY. If no KEY is specified, get all KEY-VALUE pairs.
    TODO: Get VALUE based on KEY path
    """
    @handle_exception(None, verbose=False)
    def get(self, key=None):
        if None is key:
            return self._data
        else:
            return self._data[key]
    
    """
    Set the value for the specified KEY
    TODO: Set VALUE based on KEY path
    """
    def set(self, key, value):
        self._data[key] = value
        self._add_function_from_dict_key()
        return True

    """
    Load configuration from a Json file
    """
    def load(self, path):
        with open(path, 'r') as f:
            self._data = json.load(f)
        return True

    """
    Write its own configuration to a Json file
    """
    def dump(self, path):
        with open(path, 'w') as f:
            self._data = json.load(f)
            json.dump(self._data, f, indent=4)
        return True
        
"""
#class Test(metaclass=AMeta):
@handle_exception(None)
class Test:
    def hello(self):
        print("helloworld")
        #raise Exception("xxxxx")
        return True

    @handle_exception(False)
    def world(self):
        print("12324556")
        raise Exception("zzzzzzzzz")
        return True


if __name__ == "__main__":
    t = Test()
    print(t.hello())
    print(t.world())
"""