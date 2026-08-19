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

# Common helper functions for package-specific source cleanup scripts.
#
# This file must be sourced by a package-specific script. It is not intended
# to be executed directly.
#
# Package scripts keep all package-specific decisions, such as source-tree
# markers and paths to remove. This helper only provides the shared logging,
# validation, removal, and verification behavior.

readonly CLEANUP_LOG_PREFIX="PACKAGE-SOURCE-CLEANUP"

# Public interface for package-specific cleanup scripts:
#
#   cleanup_initialize
#   cleanup_require_no_arguments
#   cleanup_require_file
#   cleanup_require_directory
#   cleanup_require_text
#   cleanup_remove_expected_path
#
# All other functions in this file are implementation details and should not
# be called directly by package-specific scripts.


# ---------------------------------------------------------------------------
# Internal implementation helpers
# ---------------------------------------------------------------------------

cleanup_log() {
    printf '[%s][%s][INFO] %s\n' \
        "${CLEANUP_LOG_PREFIX}" \
        "${CLEANUP_PACKAGE_NAME:-unknown}" \
        "$*"
}

cleanup_fail() {
    printf '[%s][%s][ERROR] %s\n' \
        "${CLEANUP_LOG_PREFIX}" \
        "${CLEANUP_PACKAGE_NAME:-unknown}" \
        "$*" >&2

    exit 1
}

# Validate that a relative cleanup path cannot escape the physical source root.
# Resolving its parent also prevents intermediate symlinks from redirecting rm.
cleanup_validate_removal_path() {
    local path="$1"
    local path_parent
    local resolved_parent
    local source_root

    [[ -n "${path}" ]] \
        || cleanup_fail "Internal error: cleanup path must not be empty."

    case "${path}" in
        /*)
            cleanup_fail "Internal error: absolute cleanup paths are not supported: \"${path}\""
            ;;
        .|..|./*|*/./*|*/.|../*|*/../*|*/..|*/)
            cleanup_fail "Internal error: unsafe cleanup path: \"${path}\""
            ;;
    esac

    source_root="$(pwd -P)" \
        || cleanup_fail "Unable to determine the source root."

    [[ "${source_root}" != "/" ]] \
        || cleanup_fail "Refusing to remove path \"${path}\" from the filesystem root directory."

    path_parent="$(dirname -- "${path}")"
    resolved_parent="$(cd -P -- "${path_parent}" 2>/dev/null && pwd -P)" \
        || cleanup_fail "Unable to resolve parent directory \"${path_parent}\" for cleanup path \"${path}\"."

    case "${resolved_parent}/" in
        "${source_root}/"*)
            ;;
        *)
            cleanup_fail "Cleanup path \"${path}\" resolves outside source root \"${source_root}\"."
            ;;
    esac

    cleanup_log "Validated cleanup path \"${path}\" in source root \"${source_root}\""
}

# ---------------------------------------------------------------------------
# Public interface for package-specific cleanup scripts
# ---------------------------------------------------------------------------

cleanup_initialize() {
    local package_name="$1"

    [[ -n "${package_name}" ]] \
        || cleanup_fail "Internal error: package name must not be empty."

    CLEANUP_PACKAGE_NAME="${package_name}"
}

cleanup_require_no_arguments() {
    local argument_count="$1"

    (( argument_count == 0 )) \
        || cleanup_fail "This package-specific cleanup script does not accept arguments."
}

cleanup_require_file() {
    local path="$1"

    [[ -f "${path}" ]] \
        || cleanup_fail "Required source file was not found: \"${path}\""
}

cleanup_require_directory() {
    local path="$1"

    [[ -d "${path}" ]] \
        || cleanup_fail "Required source directory was not found: \"${path}\""
}

cleanup_require_text() {
    local path="$1"
    local expected_text="$2"

    cleanup_require_file "${path}"

    grep -Fq -- "${expected_text}" "${path}" \
        || cleanup_fail "Expected source information was not found in \"${path}\": ${expected_text}"
}

# Remove an expected bundled source path and verify that it no longer exists.
cleanup_remove_expected_path() {
    local path="$1"

    cleanup_validate_removal_path "${path}"

    [[ -e "${path}" || -L "${path}" ]] \
        || cleanup_fail "Expected bundled source path to remove does not exist: \"${path}\""

    cleanup_log "Removing path: \"${path}\""

    rm -rf -- "${path}"

    [[ ! -e "${path}" && ! -L "${path}" ]] \
        || cleanup_fail "Path still exists after deletion: \"${path}\""

    cleanup_log "Removed and verified path: \"${path}\""
}
