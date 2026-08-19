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
# @file       ara.py
# @brief      Retrieve ara repository information
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

import sys
import json5, json
import os
import re
from pathlib import Path
from typing import Dict
from dataclasses import dataclass, field
import git

__SDK_UTILS_SRC_DIR = str(Path(__file__).resolve().parent.parent)
if __SDK_UTILS_SRC_DIR not in sys.path:
    sys.path.insert(0, __SDK_UTILS_SRC_DIR)

from core import Logger, global_logger

__all__ = ["ApallInfo"]

@dataclass(frozen=False)
class ApallInfo:
    """
        Retrieve apall source information
    """

    VERSION:str = None              # Software version
    BRANCH:str = None               # Branch name
    COMMIT_HASH:str = None          # Commit hash
    COMMIT_DATE:str = None          # Commit date
    COMMIT_LOG:str = None           # Commit log
    SUBMODULE_COMMIT_HASH:Dict = None   # Submodule commit hashes

    def __init__(self, src_dir:Path):
        self._source_path = Path(src_dir)


    def load(self) -> Dict:
        """Get apall information

        Returns:
            Dict: On success, information dictionary; on failure, None;
        """
        try:
            release_file = Path(self._source_path) / "release.json"
            if release_file.exists():
                # If the release.json file exists, get information directly from it
                with open(release_file, "rt") as f:
                    return json5.load(f)
            else:
                # Get apall information from the local git repository
                ai = ApallInfo(self._source_path)
                return ai._load_from_git()
            
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None
    
    
    def dump(self, info:Dict) -> bool:
        """Write the specified apall information to the release.json file

        Returns:
            bool: True, success; False, failure;
        """
        try:
            release_file = Path(self._source_path) / "release.json"
            with open(release_file, "wt") as f:
                info_str = json.dumps(info, indent=4, ensure_ascii=False, sort_keys=True)
                f.write(info_str)

            return True
            
        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None
        

    def _load_from_git(self) -> Dict:
        """Get apall information from the local git repository

        Returns:
            Dict: On success, information dictionary; on failure, None;
        """
        try:
            if not self._source_path.exists():
                raise Exception("apall source path {self._source_path} is not exists")

            # Open the existing repository locally
            self._repo = git.Repo(self._source_path)

            info = {}
            # Get SDK_VERSION from CMakeLists.txt
            info["version"] = self.get_version()

            # Get the date of the last commit
            log_message = self._repo.git.execute(["git", "log", "-1", "--pretty=format:%cd", "--date=format:%Y%m%d%H%M%S"])
            info["commit_date"] = log_message.strip()

            # Get the hash of the last commit
            hash_str = self._repo.git.execute(["git", "log", "-1", "--pretty=format:%H"])
            info["commit_hash"] = hash_str.strip()
        
            # Get related log information
            log_apall = self._repo.git.execute(["git", "log", "-1", "--no-merges", "--pretty=format:'%B'"])
            info["commit_log"] = log_apall
            
            # Get the N model version number
            n_model_version = self.get_nmodel_version()
            if n_model_version is not None:
                info["n_model_version"] = n_model_version
            return info

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None


    def get_version(self) -> str:
        try:
            # Define the list of files to search
            file_paths = ["cmake/Config.cmake", "CMakeLists.txt"]
    
            # Define the regular expression pattern
            pattern = r'(?i)\s*set\s*\(\s*SDK_VERSION\s*\s+(["\']?)([^"\'\)\s]+)\1\s*\)'
    
            found_version = None
    
            # Search in the two files separately
            for file_path in file_paths:
                if not os.path.exists(self._source_path / file_path):
                    continue

                with open(self._source_path / file_path, 'r', encoding='utf-8') as file:
                    content = file.read()

                # Search for matches
                match = re.search(pattern, content)
                if match:
                    found_version = match.group(2)
    
            # Process the search results
            return found_version

        except Exception as e:
            global_logger.error(f"{__file__}:{sys._getframe().f_lineno} -> {sys._getframe().f_code.co_name}:{e}")
            return None

    def get_nmodel_version(self) -> str:
        try:
            nmv_path = os.path.join(self._source_path, 'isoft/n-model-definition/model_version.json')
            with open(nmv_path, "r") as f:
                data = json.load(f)
                mv = data["modelVersion"]
                version = f"{mv['major']}.{mv['minor']}"
        except Exception as e:
            # print("{file}:{line}->{fun}():{err}".format(file=__file__, fun=sys._getframe().f_code.co_name, line=sys._getframe().f_lineno, err=e))
            pass
        else:
            return version
def _tests():
    apall = ApallInfo("/home/fj/git/ap-all")
    info = apall.load()
    info_str = json5.dumps(info, indent=4, ensure_ascii=False, sort_keys=True)
    print(info_str)

    info["version"] = apall.get_version()
    apall.dump(info)
    info = apall.load()
    info_str = json5.dumps(info, indent=4, ensure_ascii=False, sort_keys=True)
    print(info_str)

if __name__ == "__main__":
    _tests()