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
# @file       modify_arxml.py
# @brief      Script to modify ARXML files
# @details
# @date       2024-10-11
# @author     longxiao.liang
# @version    1.2.0
#
# ================================================================

import xml.etree.ElementTree as ET
import xml.dom.minidom as minidom
import sys
import shutil

def add_sub_element_to_core_arxml(source_file_path, target_file_path, parent_tag, child_tag, attribute_value, child_texts):

    attribute_name = "DEST"
    # Read XML file
    tree = ET.parse(source_file_path)
    root = tree.getroot()

    # Find target parent element
    namespace = 'http://autosar.org/schema/r4.0'  # Use namespace
    parent_element = root.find(f'.//{{{namespace}}}{parent_tag}')

    if parent_element is not None:
        is_new_child_inserted = False
        for child_text in child_texts:
            is_child_exist = False
            existing_children = parent_element.findall(f'.//{{{namespace}}}{child_tag}[@{attribute_name}="{attribute_value}"]')
            for existing_child in existing_children: 
                if existing_child.text == child_text:
                    is_child_exist = True
                    break
            if not is_child_exist:
                # Create a new child element, using the namespace
                new_element = ET.Element(f'{{{namespace}}}{child_tag}', {attribute_name: attribute_value})
                new_element.text = child_text

                # Add the new element to the end of the parent element
                parent_element.append(new_element)
                is_new_child_inserted = True
                print(f"New element added: {child_tag}, attribute {attribute_name}={attribute_value}, element content {child_text}")
            else:
                print(f"Child element already exists: {child_tag}, attribute {attribute_name}={attribute_value}, element content {child_text}, will not add again.")
        if is_new_child_inserted:
            # Use minidom to format output XML, and manually handle namespace
            rough_string = ET.tostring(root, 'utf-8')

            # Remove redundant namespace prefixes
            rough_string = rough_string.decode('utf-8').replace('ns0:', '').replace(':ns0', '')

            # Add XML declaration
            xml_declaration = '<?xml version="1.0" encoding="UTF-8"?>\n'
            # Save the modified XML file
            with open(target_file_path, 'wb') as f:
                f.write((xml_declaration + rough_string).encode())
    else:
        print(f"Parent element not found: {parent_tag}")

if __name__ == "__main__":
    # Check the number of command line parameters
    if len(sys.argv) != 3:
        print("Usage: python modify_arxml.py <ARXML_PATH> <MODE>")
        print("MODE: m-modify; r-restore")
        sys.exit(1)

    file_path = sys.argv[1]
    mode = sys.argv[2]

    if "m" in mode.lower():
        source_file_path = file_path
        target_file_path = source_file_path + ".bak"
        shutil.copy(source_file_path, target_file_path)

        parent_tag_exe = "CONTAINED-AR-ELEMENT-REFS"
        parent_tag_process = "CONTAINED-PROCESS-REFS"

        child_tag_exe = "CONTAINED-AR-ELEMENT-REF"
        child_tag_process = "CONTAINED-PROCESS-REF"

        attribute_value_exe = "EXECUTABLE"
        attribute_value_process = "PROCESS"

        child_text_exe = ("/ara/sm_ucmcomm_client/executables/ucmcomm_client", "/ara/sm_demo/executables/SMControlApplication", "/ara/sm_trigger_fg/executables/trigger_fg", "/ara/sm_trigger_ecu_state/executables/trigger_ecu_state")
        child_text_process = ("/ara/sm_ucmcomm_client/processes1/ucmcomm_client","/ara/sm_demo/processes/SMControlApplication1", "/ara/sm_trigger_fg/processes/trigger_fg1", "/ara/sm_trigger_ecu_state/processes/trigger_ecu_state1")

        add_sub_element_to_core_arxml(source_file_path, source_file_path, parent_tag_exe, child_tag_exe, attribute_value_exe, child_text_exe)
        add_sub_element_to_core_arxml(source_file_path, source_file_path, parent_tag_process, child_tag_process, attribute_value_process, child_text_process)
    elif "r" in mode.lower():
        source_file_path = file_path + ".bak"
        target_file_path = file_path
        shutil.move(source_file_path, target_file_path)

