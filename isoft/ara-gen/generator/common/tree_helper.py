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
import os
import re
import lxml.etree as et
import functools

# pylint: disable=too-many-lines,too-many-public-methods
from lxml.objectify import StringElement, IntElement, FloatElement

from generator.parser.exception_handling import handle_exceptions
from generator.common.autosar_mapping import PathError


def get_schema_from_file(filename):
    assert os.path.isfile(filename), "file {} does not exist".format(filename)
    tree = et.parse(open(filename, encoding='utf-8'))
    return get_schema_file_from_tree(tree)


def get_schema_from_tree(tree):
    schema_locations = list(tree.xpath("//*/@xsi:schemaLocation", namespaces={'xsi': "http://www.w3.org/2001/XMLSchema-instance"}))
    if len(schema_locations) == 0:
        raise AttributeError("XML validation failed! No schema defined.")
    if len(schema_locations) > 1:
        raise ValueError("XML validation failed! Multiple schemas defined.")

    schema_location_split = schema_locations[0].split()
    if len(schema_location_split) != 2:
        raise ValueError("XML validation failed! Invalid schemaLocation syntax.")
    return schema_location_split[1]


def get_schema_file_from_tree(tree, check_availability=True):
    file_name = get_schema_from_tree(tree)
    file_directory = os.path.dirname(os.path.realpath(__file__))
    schema_file = os.path.join(file_directory,
                               "schema",
                               file_name)
    if check_availability and not os.path.isfile(schema_file):
        raise ValueError("XML validation failed! The schema {0} is not supported.".format(file_name))
    return schema_file


@functools.lru_cache(maxsize=5120)
def get_element(from_where, path):
    ns_mapping = {'AR': 'http://autosar.org/schema/r4.0'}
    p = "./AR:" + path.replace("/", "/AR:")
    result = from_where.find(p, ns_mapping)
    if hasattr(result, '_setText'):
        result._setText(result.text.strip())
    return result


@handle_exceptions(
    error_string_template="ARXML element fetching ERROR: '{arguments[1]}' must exist " +
    "for '{arguments[0]}'")
def get_element_or_die(from_where, path):
    result = get_element(from_where, path)
    if result is None:
        assert False, "[INIT] CODE-006: Unable to get element {} from {}. Fqn:{}. LocalPath:{}".format(get_hump(path), get_hump(from_where.short_tag(), True),from_where.get_fqn(), from_where.get_path())
    return result


def get_element_or_none(from_where, path):
    try:
        return get_element(from_where, path)
    except Exception:
        return None


def get_element_or_0(from_where, path):
    try:
        element = get_element(from_where, path)
        if element is not None:
            return element
        else:
            return 0
    except Exception:
        return 0


def get_element_or_default_1(from_where, path):
    try:
        element = get_element(from_where, path)
        if element is not None:
            return element
        else:
            return -1
    except Exception:
        return -1


def get_element_or_false(from_where, path):
    try:
        element = get_element(from_where, path)
        if element is not None:
            return element
        else:
            return False
    except Exception:
        return False


def get_element_or_str(from_where, path):
    try:
        element = get_element(from_where, path)
        if element is not None:
            return element
        else:
            return ""
    except Exception:
        return ""


def short_name(fqn):
    """Get short name of fully qualified name"""
    return str(fqn).split('/')[-1]


def short_name_2(fqn):
    maps = str(fqn).split('/')
    if len(maps) >= 2:
        return maps[-2]
    else:
        return ""


def to_list(x):
    return x if x is not None else []


def to_str(x):
    return x.text if x else ""


def to_nanoseconds(ar_value):
    result = None
    if ar_value:
        try:
            result = int(round(float(ar_value.text) * 1000000000))
        except:
            result = None

    return result


def to_milliseconds(ar_value):
    result = None
    if ar_value == 0:
        return 0
    if ar_value:
        try:
            result = int(round(float(ar_value.text) * 1000))
        except:
            result = None

    return result


def int_or_unmodified(input_value):
    try:
        result = positive_int(input_value)
    except ValueError:
        result = str(input_value)
    return result


def positive_int(input_value):
    s = str(input_value)
    try:
        #first try to autodetect the base
        result = int(s, 0)
    except ValueError as e:
        if s.startswith('0'):
            #octal in ARXML is 0123, python expects it to be 0o123
            result = int(s, 8)
        else:
            raise e
    return result


def transition_number(number, change=10, defaultInt=0, defaultStr="0"):
    if number == -1 or number == 0:
        return number
    if isinstance(number, FloatElement):
        return number
    if change in [2, 8, 10, 16]:
        if number is not None:
            num_str = None
            if isinstance(number, StringElement) or isinstance(number, IntElement):
                num_str = to_str(number)
            elif isinstance(number, int):
                num_str = str(number)
            elif isinstance(number, str):
                num_str = number
            if num_str is not None and num_str != "":
                if re.match('^[+]?[1-9][0-9]*', num_str):
                    num_type = 10
                    if change == 10:
                        return int(num_str)
                elif re.match('^0[oO]?[0-7]+', num_str):
                    num_type = 8
                    if change == 8:
                        return num_str
                elif re.match('^0[xX][0-9a-fA-F]+', num_str):
                    num_type = 16
                    if change == 16:
                        return num_str
                elif re.match('^0[bB][0-1]+', num_str):
                    num_type = 2
                    if change == 2:
                        return num_str
                else:
                    return num_str
                if change == 2:
                    return bin(int(num_str, num_type))
                elif change == 8:
                    return oct(int(num_str, num_type))
                elif change == 10:
                    return int(num_str, num_type)
                elif change == 16:
                    return hex(int(num_str, num_type))
    if change == 10:
        return defaultInt
    else:
        return defaultStr


def get_hump(serializer, isBig=False):
    last = ""
    if serializer is not None:
        serializerList = str(serializer).replace("_","-" ).split("-")
        for index, item in enumerate(serializerList):
            if index == 0 and not isBig:
                last += item.lower()
            else:
                last += item.capitalize()
    return last

