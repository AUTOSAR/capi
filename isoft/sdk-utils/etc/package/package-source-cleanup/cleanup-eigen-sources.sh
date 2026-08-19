#!/usr/bin/env bash

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

# Purpose:
# Remove the bundled BTL benchmark sources from Eigen before Eigen is
# configured and built.
#
# The sources under bench/btl are not used, built, installed, or packaged by
# CAPI. Removing them makes this exclusion explicit and verifies that the
# normal Eigen build succeeds without them.

set -euo pipefail

readonly SCRIPT_DIR="$(
    cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
    pwd -P
)"

# shellcheck source=cleanup-common.sh
source "${SCRIPT_DIR}/cleanup-common.sh"

main() {
    cleanup_initialize "Eigen"
    cleanup_require_no_arguments "$#"

    cleanup_require_file "signature_of_eigen3_matrix_library"
    cleanup_require_directory "Eigen"

    cleanup_remove_expected_path "bench/btl"
}

main "$@"
