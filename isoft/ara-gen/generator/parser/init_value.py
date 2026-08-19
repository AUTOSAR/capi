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
from generator.common.tree_helper import get_element_or_none, get_element_or_str


class InitValueClass:
    def __init__(self, model):
        self.model = model
        self.controlList = {
            "StringImplDataType": "TEXT-VALUE-SPECIFICATION",
            "MapImplDataType": "APPLICATION-ASSOC-MAP-VALUE-SPECIFICATION",
            "StructureImplDataType": "RECORD-VALUE-SPECIFICATION",
            "ArrayImplDataType": "ARRAY-VALUE-SPECIFICATION",
            "BaseType": "NUMERICAL-VALUE-SPECIFICATION"
        }

    def get_init_value(self, fqn, portItem, impl_type):
        if impl_type is None or portItem is None:
            return ""
        fieldComSpecList = portItem.find_elements_of_type("FIELD-SENDER-COM-SPEC")
        for fieldComSpec in fieldComSpecList:
            dataElementRef = get_element_or_none(fieldComSpec, "DATA-ELEMENT-REF")
            if dataElementRef is not None and dataElementRef.text == fqn:
                initValue = self.find_init_value(impl_type, fieldComSpec)
                if initValue != "":
                    return "{" + initValue + "}"

        return ""

    def find_init_value(self, impl_type, fieldComSpec):
        result = ""
        if fieldComSpec is not None:
            dataElementRef = get_element_or_str(fieldComSpec, "DATA-ELEMENT-REF")
            classname = impl_type.__class__.__name__
            if classname == 'StringImplDataType':
                textMap = fieldComSpec.find_elements_of_type("TEXT-VALUE-SPECIFICATION")
                value = []
                for textItem in textMap:
                    textCon = get_element_or_none(textItem, "VALUE")
                    if textCon is not None:
                        value.append(textCon.text)
                if len(value) > 0:
                    result = "\"{}\"".format(",".join(value))
            elif classname == 'MapImplDataType':
                mapping = fieldComSpec.find_elements_of_type("APPLICATION-ASSOC-MAP-ELEMENT-VALUE-SPECIFICATION")
                resultList = []
                for item in mapping:
                    key_data_type = impl_type.key_data_type
                    value_data_type = impl_type.value_data_type
                    mapKey = get_element_or_none(item, "KEY")
                    mapValue = get_element_or_none(item, "VALUE")
                    if mapKey is not None or mapValue is not None:
                        resultKey = self.find_init_value(key_data_type, mapKey)
                        resultValue = self.find_init_value(value_data_type, mapValue)
                        if resultKey != "" or resultValue != "":
                            if resultKey == "":
                                resultKey = "{}"
                            if resultValue == "":
                                resultValue = "{}"
                            resultList.append("{" + resultKey + "," + resultValue + "}")
                result = ",".join(resultList)
            elif classname == 'LinearVectorImplDataType':
                element_data_type = impl_type.element_data_type
                result = self.find_init_value(element_data_type, fieldComSpec)
            elif classname == "LinearVariantImplDataType":
                element_data_type_list = impl_type.element_data_type_list
                resultList = []
                for element_data_type in element_data_type_list:
                    resultList.append(self.find_init_value(element_data_type, fieldComSpec))
                result = ",".join(resultList)
            elif classname == "StructureImplDataType":
                record_fields = get_element_or_none(fieldComSpec, "INIT-VALUE/RECORD-VALUE-SPECIFICATION/FIELDS")
                result = self.get_record_value(record_fields, impl_type, dataElementRef)
            elif classname == 'ArrayImplDataType':
                array_value = get_element_or_none(fieldComSpec, "INIT-VALUE/ARRAY-VALUE-SPECIFICATION")
                result = self.get_array_value(array_value, impl_type)
            elif classname == 'ImplDataType':
                referred_types = impl_type.referred_types
                if len(referred_types) > 0:
                    referred_type = referred_types[0]
                    result = self.find_init_value(referred_type, fieldComSpec)
            elif classname == 'BaseType':
                numberMap = fieldComSpec.find_elements_of_type("NUMERICAL-VALUE-SPECIFICATION")
                value = []
                for numberItem in numberMap:
                    numberCon = get_element_or_none(numberItem, "VALUE")
                    if numberCon is not None:
                        value.append(numberCon.text)
                if len(value) > 0:
                    result = "{}".format(",".join(value))
        return result

    def get_array_value(self, array_value, impl_type):
        result = ""
        if array_value is not None:
            count = get_element_or_none(array_value, "INTENDED-PARTIAL-INITIALIZATION-COUNT")
            referred_types = impl_type.referred_types
            if len(referred_types) > 0:
                referred_type = referred_types[0]
                result = self.find_init_value(referred_type, array_value)
        return result

    def get_record_value(self, record_fields, impl_type, dataElementRef):
        result = ""
        if record_fields is not None:
            partChildren = record_fields.getchildren()
            sub_elements = impl_type.sub_elements
            resultList = []
            for i, sub_element in enumerate(sub_elements):
                referred_type = sub_element[1]
                if len(partChildren) > i:
                    value_type = partChildren[i]
                    try:
                        self.judge_type(referred_type, value_type)
                        assert False, "[BASE] CODE-003: Initial value type error. FieldPath:{}, Path:{}. LocalPath:{}".format(dataElementRef, value_type.get_fqn(), value_type.get_path())
                    except ValueError:
                        resultCon = self.get_all_value(referred_type, value_type, dataElementRef)
                        if resultCon == "":
                            resultCon = "{}"
                        resultList.append(resultCon)
            result = ",".join(resultList)
        return result

    def get_all_value(self, impl_type, value_type, dataElementRef):
        result = ""
        short_tag = value_type.short_tag()
        if short_tag == "TEXT-VALUE-SPECIFICATION":
            textCon = get_element_or_none(value_type, "VALUE")
            if textCon is not None:
                result = "\""+textCon.text+"\""
        elif short_tag == "APPLICATION-ASSOC-MAP-VALUE-SPECIFICATION":
            result = "{"+self.find_init_value(impl_type, value_type)+"}"
        elif short_tag == "ARRAY-VALUE-SPECIFICATION":
            result = "{"+self.get_array_value(value_type, impl_type)+"}"
        elif short_tag == "RECORD-VALUE-SPECIFICATION":
            record_fields = get_element_or_none(value_type, "FIELDS")
            result = "{"+self.get_record_value(record_fields, impl_type, dataElementRef)+"}"
        elif short_tag == "NUMERICAL-VALUE-SPECIFICATION":
            numberCon = get_element_or_none(value_type, "VALUE")
            if numberCon is not None:
                result = numberCon.text
        return result

    def judge_type(self, impl_type, value_type):
        if impl_type is not None and value_type is not None:
            classname = impl_type.__class__.__name__
            short_tag = value_type.short_tag()
            if classname == "LinearVectorImplDataType":
                self.judge_type(impl_type.element_data_type, value_type)
            elif classname == "ImplDataType":
                referred_types = impl_type.referred_types
                if len(referred_types) > 0:
                    referred = referred_types[0]
                    self.judge_type(referred, value_type)
            else:
                tag = self.controlList[classname]
                if short_tag == tag:
                    raise ValueError






