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
View factory class.
"""

from generator.views.dds.impl_type_view import StructImplTypeView, \
    EnumImplTypeView, MapImplTypeView, AliasedImplTypeView, AliasedBaseImplTypeView, \
    VectorImplTypeView, StringImplTypeView, ArrayImplTypeView, BaseTypeView, VariantImplTypeView


class TypeViewFactory(object):
    @staticmethod
    def create_string_view(type):
        return StringImplTypeView(type)

    @staticmethod
    def create_struct_view(type):
        return StructImplTypeView(type)

    @staticmethod
    def create_enum_view(type):
        return EnumImplTypeView(type)

    @staticmethod
    def create_map_view(type):
        return MapImplTypeView(type)

    @staticmethod
    def create_vector_view(type):
        return VectorImplTypeView(type)

    @staticmethod
    def create_variant_view(type):
        return VariantImplTypeView(type)

    @staticmethod
    def create_array_view(type):
        return ArrayImplTypeView(type)

    @staticmethod
    def create_aliased_impl_type_view(type):
        return AliasedImplTypeView(type)

    @staticmethod
    def create_aliased_base_impl_type_view(type):
        return AliasedBaseImplTypeView(type)

    @staticmethod
    def create_base_type_view(type, cpptype_name):
        return BaseTypeView(type, cpptype_name)
