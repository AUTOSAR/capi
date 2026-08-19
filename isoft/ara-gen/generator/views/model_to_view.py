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

from generator.views.type_view_factory import TypeViewFactory

class ViewManipulations(object):
    # cpp types according to SWS_AdaptivePlatformTypes
    _basetypes_dict = {'boolean': 'bool',
                       'cpp_bool': 'bool',
                       'std_int8_t': 'std::int8_t',
                       'std_int16_t': 'std::int16_t',
                       'std_int32_t': 'std::int32_t',
                       'std_int64_t': 'std::int64_t',
                       'std_int8_least_t': 'std::int_least8_t',
                       'std_int16_least_t': 'std::int_least16_t',
                       'std_int32_least_t': 'std::int_least32_t',
                       'std_uint8_t': 'std::uint8_t',
                       'std_uint16_t': 'std::uint16_t',
                       'std_uint32_t': 'std::uint32_t',
                       'std_uint64_t': 'std::uint64_t',
                       'std_uint8_least_t': 'std::uint_least8_t',
                       'std_uint16_least_t': 'std::uint_least16_t',
                       'std_uint32_least_t': 'std::uint_least32_t',
                       'std_float32_t': 'float',
                       'std_float64_t': 'double'}

    @staticmethod
    def _std_type_name(path):
        """
        Extract the basetype name from a type path. If no path then return
        the string itself
        """
        if "/" in path:
            path_list = path.split("/")
            type_name = str(path_list[len(path_list) - 1])
        else:
            type_name = path
        return type_name


    @staticmethod
    def _cppname(basetype):
        btype_name = ViewManipulations._std_type_name(str(basetype.name))
        return ViewManipulations._basetypes_dict[btype_name]


    @staticmethod
    def get_view_kind(view):
        classname = view.__class__.__name__
        if classname == 'StringImplTypeView':
            kind = 'string'
        elif classname == 'FixedStringImplTypeView':
            kind = 'fixed_string'
        elif classname == 'StructImplTypeView':
            kind = 'structure'
        elif classname == 'EnumImplTypeView':
            kind = 'enum'
        elif classname == 'MapImplTypeView':
            kind = 'map'
        elif classname == 'VectorImplTypeView':
            kind = 'vector'
        elif classname == 'VariantImplTypeView':
            kind = 'variant'
        elif classname == 'ArrayImplTypeView':
            kind = 'array'
        elif classname == 'AliasedImplTypeView':
            kind = 'aliased_impltype'
        elif classname == 'AliasedBaseImplTypeView':
            kind = 'aliased_basetype'
        elif classname == 'BaseTypeView':
            kind = 'basetype'
        else:
            kind = 'unknown'

        return kind

    @staticmethod
    def convert_to_view_tree(_type):
        return ViewManipulations()._convert_to_view_tree(_type)

    @staticmethod
    def to_view( _type):
        return ViewManipulations()._to_view(_type)

    def _to_view(self, _type):
        result = None
        classname = _type.__class__.__name__
        if classname == 'StringImplDataType':
            result = self._view_factory().create_string_view(_type)
        elif classname == 'FixedStringImplDataType':
            result = self._view_factory().create_fixed_string_view(_type)
        elif classname == 'StructureImplDataType':
            result = self._view_factory().create_struct_view(_type)
        elif classname == 'EnumImplDataType':
            result = self._view_factory().create_enum_view(_type)
        elif classname == 'MapImplDataType':
            result = self._view_factory().create_map_view(_type)
        elif classname == 'LinearVectorImplDataType':
            result = self._view_factory().create_vector_view(_type)
        elif classname == 'LinearVariantImplDataType':
            result = self._view_factory().create_variant_view(_type)
        elif classname == 'ArrayImplDataType':
            result = self._view_factory().create_array_view(_type)
        elif classname == 'ImplDataType':
            assert _type.referred_types, \
                '{} must have type it based on'.format(_type.name)
            result = self._view_factory().create_aliased_impl_type_view(_type)
            if not _type.type_is_new:
                if _type.referred_types[0].__class__.__name__ == 'BaseType':
                    result = self._view_factory().create_aliased_base_impl_type_view(_type)
        elif classname == 'BaseType':
            if _type.type_is_new:
                result = self._view_factory().create_base_type_view(_type, None)
            else:
                result = self._view_factory().create_base_type_view(_type, ViewManipulations._cppname(_type))
        else:
            assert False, "unexpected type {}".format(classname)

        return result

    def _convert_to_view_tree(self, _type):
        view = self.to_view(_type)
        for t in _type.referred_types:
            vt = self._convert_to_view_tree(t)
            view.add_referred_view(vt)
        return view

    def _view_factory(self):
        return TypeViewFactory
