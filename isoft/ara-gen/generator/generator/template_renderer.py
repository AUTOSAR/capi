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
Generator
"""

import logging
import os
import sys

import inspect
import jinja2
import functools

from generator.generator.generator_settings import GeneratorSettings

class TemplateRenderer:

    def __init__(self, settings, args):
        self._settings = settings
        self._log = logging.getLogger(__name__)
        self._printed_files = []
        self._first_file = True
        self.args = args

    @staticmethod
    def include_guard(namespaces_list, output_filename):
        result = "_".join(namespaces_list + [output_filename, ""])
        result = result.replace(".", "_")
        result = result.replace(":", "_")
        result = result.replace("-", "_")
        return result.upper()

    def write_output(self, template, output_dir, filename, **kwargs):
        """
        Writes the content of a template render to a file unless
        the command line options to only list files is set. Then it
        registers what file would have been written.
        """
        relative_file_path = os.path.join(output_dir, filename)
        output_dir_absolute = os.path.abspath(output_dir)
        full_path = os.path.abspath(relative_file_path)
        if self._settings.list_files:
            # No generation, just print filename (if requested)
            if full_path not in self._printed_files:
                self._printed_files.append(full_path)
                # if self._first_file:
                #     self._first_file = False
                #     sys.stdout.write(full_path)
                # else:
                sys.stdout.write(';' + full_path)
        elif self._should_generate():
            if self.args.single_process:
                self.write_file(template, full_path, output_dir_absolute, **kwargs)
            else:
                try:
                    self.write_file(template, full_path, output_dir_absolute, **kwargs)
                except Exception as e:
                    self._log.info("write_output error: %s,full_path=%s", e.args, full_path)
        else:
            self._log.info("Not re-generating, because it already exists: %s", full_path)

    def write_file(self, template, full_path, output_dir_absolute, **kwargs):
        if os.path.isfile(full_path):
            os.remove(full_path)
        self._log.info("Generating %s", full_path)
        os.makedirs(output_dir_absolute, exist_ok=True)
        if isinstance(template, str):
            output = template
        else:
            output = template.render(**kwargs)
        with open(full_path, 'w', encoding='utf-8') as f:
            f.write(output)
            f.close()

    @functools.lru_cache(maxsize=640)
    def load_template(self, name, trim_blocks=False):
        generator_path = os.path.dirname(inspect.getfile(TemplateRenderer))
        template_folder = os.path.join(os.path.dirname(generator_path), "templates")
        template_env = jinja2.Environment(
            loader=jinja2.FileSystemLoader(template_folder),
            trim_blocks=trim_blocks)

        return template_env.get_template(name)

    def _should_generate(self):
        return not self._settings.dry_run and not self._settings.list_files

