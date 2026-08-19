#!/bin/bash

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
# @file       are-run.sh
# @brief      ARE runtime script
# @details
# @date       by: james.feng
# @author     fengjian
# @version    1.2.0
#
# ================================================================

set -e

# Get the absolute path of the directory where the current script is located (compatible with bash, zsh, sh, and both source and execution methods)
function GetScriptDir() {
    local SOURCE

    # Prefer using BASH_SOURCE (suitable for bash and source environments)
    if [ -n "${BASH_SOURCE:-}" ]; then
        SOURCE="${BASH_SOURCE[0]}"
    # Try using Zsh's special variable
    elif [ -n "${ZSH_VERSION:-}" ]; then
        SOURCE="${(%):-%x}"
    # Fall back to $0 (suitable for most POSIX shells)
    else
        SOURCE="$0"
    fi

    # Resolve symbolic link (if the system supports readlink -f)
    if command -v readlink >/dev/null 2>&1; then
        SOURCE=$(readlink -f "$SOURCE")
    fi
    # Core method to get the directory absolute path: switch to the directory and use pwd
    (cd "$(dirname "$SOURCE")" 2>/dev/null && pwd)
}

export ARA_SYSROOT="$(GetScriptDir)"

# Get the latest version of the ara_ver*.json file; a larger number indicates a newer version
function GetNewestAraConfigFile() {
    max_num=0
    max_file=""

    for file in ${ARA_SYSROOT}/ara/ara_ver*.json; do
        if [ ! -f ${file} ]; then
            echo "No ara_ver*.json files found in ${ARA_SYSROOT}" >&2
            exit 1
        fi
        # Extract the numeric part: remove the prefix "ara_ver" and suffix ".json"
        num=$(echo "$file" | sed 's/.*ara_ver//' | sed 's/\.json//')

        # Compare the numbers
        if [ "$num" -gt "$max_num" ]; then
            max_num=$num
            max_file="$file"
        fi
    done

    echo "${max_file:-}"
}

# Get variable values from ara_ver*.json
function GetValueFromAraConfigFile() {
    ARA_CONFIG_FILE="$(GetNewestAraConfigFile)"
    if [ ! -f "$ARA_CONFIG_FILE" ]; then
        echo "No ara_ver*.json files found" >&2
        exit 1
    fi
    local key="$1"
    # 1. Use grep to find lines containing the key, ignoring differences in spaces and quotes
    # 2. Use cut and tr for simple field splitting and character cleaning
    local value=$(grep "\"$key\"" "$ARA_CONFIG_FILE" |
        cut -d ':' -f 2- |
        tr -d ' "\t\r\n' |
        cut -d ',' -f 1)
    echo "${value:-}"
}

function GetAraConfig() {
    # Get the values of each field in sequence
    # Location of the framework parent directory. Note that the actual framework directory is in ${ARA_FRAMEWORK_DIR}/framework
    export ARA_FRAMEWORK_DIR=$(GetValueFromAraConfigFile "platform_framework")
    if [ -z "${ARA_FRAMEWORK_DIR}" ]; then
        export ARA_FRAMEWORK_DIR="${ARA_SYSROOT}/ara"
    fi
    # Location of the core parent directory. Note that the actual core directory is in ${ARA_FRAMEWORK_DIR}/core
    export ARA_CORE_DIR=$(GetValueFromAraConfigFile "platform_core")
    if [ -z "${ARA_CORE_DIR}" ]; then
        export ARA_CORE_DIR="${ARA_SYSROOT}/ara"
    fi
    # Location of the swcls parent directory. Note that the actual swcls directory is in ${ARA_FRAMEWORK_DIR}/swcls
    export ARA_SWCLS_DIR=$(GetValueFromAraConfigFile "application_swcls")
    if [ -z "${ARA_SWCLS_DIR}" ]; then
        export ARA_SWCLS_DIR="${ARA_SYSROOT}/ara"
    fi
    # Location of the var directory.
    export ARA_VAR_DIR=$(GetValueFromAraConfigFile "platform_var")
    if [ -z "${ARA_VAR_DIR}" ]; then
        export ARA_VAR_DIR="${ARA_SYSROOT}/ara"
    fi
    export ARA_FRAMEWORK_VERSION=$(GetValueFromAraConfigFile "framework_version")
    export ARA_CORE_VERSION=$(GetValueFromAraConfigFile "core_version")
    export SDK_SYSROOT="${ARA_FRAMEWORK_DIR}/../"

    # Output results (example)
    #echo "platform_framework: $ARA_FRAMEWORK_DIR"
    #echo "platform_core: $ARA_CORE_DIR"
    #echo "platform_var: $ARA_VAR_DIR"
    #echo "application_swcls: $ARA_SWCLS_DIR"
    #echo "framework_version: $ARA_FRAMEWORK_VERSION"
    #echo "core_version: $ARA_CORE_VERSION"
}

function MountCgroup() {
    local MP_CGROUP="/sys/fs/cgroup"
    local MP_CGROUP_CPUACCT="${MP_CGROUP}/cpu,cpuacct"
    local MP_CGROUP_CPUSET="${MP_CGROUP}/cpuset"
    local MP_CGROUP_PIDS="${MP_CGROUP}/pids"
    local MP_CGROUP_MEMORY="${MP_CGROUP}/memory"

    if ! mount | grep "${MP_CGROUP} type" &>/dev/null; then
        if ! mount -t tmpfs tmpfs ${MP_CGROUP} -o rw,nosuid,nodev,noexec,mode=755; then
            return 1
        fi

    fi

    if ! mount | grep ${MP_CGROUP_CPUACCT} &>/dev/null; then
        mkdir -p ${MP_CGROUP_CPUACCT}
        if ! mount -t cgroup cgroup ${MP_CGROUP_CPUACCT} -o rw,nosuid,nodev,noexec,relatime,cpu,cpuacct; then
            return 1
        fi
    fi

    if ! mount | grep ${MP_CGROUP_CPUSET} &>/dev/null; then
        mkdir -p ${MP_CGROUP_CPUSET}
        if ! mount -t cgroup cgroup ${MP_CGROUP_CPUSET} -o rw,nosuid,nodev,noexec,relatime,cpuset; then
            return 1
        fi
    fi

    if ! mount | grep ${MP_CGROUP_PIDS} &>/dev/null; then
        mkdir -p ${MP_CGROUP_PIDS}
        if ! mount -t cgroup cgroup ${MP_CGROUP_PIDS} -o rw,nosuid,nodev,noexec,relatime,pids; then
            return 1
        fi
    fi

    if ! mount | grep ${MP_CGROUP_MEMORY} &>/dev/null; then
        mkdir -p ${MP_CGROUP_MEMORY}
        if ! mount -t cgroup cgroup ${MP_CGROUP_MEMORY} -o rw,nosuid,nodev,noexec,relatime,memory; then
            return 1
        fi
    fi

    return 0
}

function AARun() {
    export LD_LIBRARY_PATH="${SDK_SYSROOT}/ara/framework/${ARA_FRAMEWORK_VERSION}/lib:${SDK_SYSROOT}/usr/lib"
    # Set communication identifier
    export ISOFT_ARA_FSH_SYSROOT="${ARA_SYSROOT}"
    # Set configuration directory
    local EXE_DIR=$(dirname $(realpath $1))
    export ISOFT_ARA_FSH_PROC_CONFIG_DIR="${EXE_DIR}/../../etc/process"
    # Set the runtime directory; not required, only necessary when the default /run directory lacks read/write permissions.
    export ISOFT_ARA_RUNTIME_DIR="/tmp"
    $*
}

function AreRun() {
    local RUN_FLAG_WITHOUT_CGROUP=$1
    local RUN_FLAG_DEBUG_PORT=$2
    local RUN_ARGS=""

    if [ "xtrue" = "x${RUN_FLAG_WITHOUT_CGROUP}" ]; then
        RUN_ARGS="-r"
    else
        if ! MountCgroup; then
            echo "ERROR on mount cgroup"
            exit
        fi
    fi

    if [ "x0" != "x${RUN_FLAG_DEBUG_PORT}" ]; then
        RUN_ARGS="${RUN_ARGS} -d ${RUN_FLAG_DEBUG_PORT}"
    fi
    
    export LD_LIBRARY_PATH="${SDK_SYSROOT}/lib:${SDK_SYSROOT}/usr/lib:${SDK_SYSROOT}/ara/framework/${ARA_FRAMEWORK_VERSION}/lib"
    ${ARA_FRAMEWORK_DIR}/framework/ara_loader -s ${ARA_SYSROOT} -v ${RUN_ARGS}
}

function PrepareSmToolEnvironment() {
    export ISOFT_ARA_FSH_SYSROOT="${ARA_SYSROOT}"
    export ISOFT_ARA_FSH_PROC_CONFIG_DIR="${ARA_CORE_DIR}/core/${ARA_CORE_VERSION}/etc/shell_request_proc"
    export LD_LIBRARY_PATH="${SDK_SYSROOT}/lib:${SDK_SYSROOT}/usr/lib:${SDK_SYSROOT}/ara/framework/${ARA_FRAMEWORK_VERSION}/lib"

    # State management tool, used to switch functional group states and get/set state machine status
    SM_TOOL=${ARA_CORE_DIR}/core/${ARA_CORE_VERSION}/bin/shell_request_client
}

function AreChangeFunctionGroupState() {
    PrepareSmToolEnvironment
    local FG_STATE=$1
    ${SM_TOOL} -f -s ${FG_STATE}
}

function AreGetFunctionGroupState() {
    PrepareSmToolEnvironment
    local FG_NAME=$1
    ${SM_TOOL} -f -g ${FG_NAME}
}

function AreChangeStateMachine {
    PrepareSmToolEnvironment
    local SM_STATE=$1
    ${SM_TOOL} -S -s ${SM_STATE}
}

function AreGetStateMachine() {
    PrepareSmToolEnvironment
    local SM_FQN=$1
    ${SM_TOOL} -S -g ${SM_FQN}
}

# Get the PID of emd under Linux
function FindEmd4Linux() {
    # Method 1: Simple, but depends on complex commands, not supported in embedded scenarios.
    # local EMD_PID=$(ps -eo pid,command | grep emd | grep ${ARA_SYSROOT} | awk '{print $1}')

    # Method 2: Complex, but meets embedded scenario requirements
    local SYSROOT=$1
    for PID in $(ls /proc); do
        local COMM_PATH="/proc/${PID}/comm"
        if ! [ -f ${COMM_PATH} ]; then
            continue
        fi
        read COMM <${COMM_PATH}
        read CMDLINE <"/proc/${PID}/cmdline"
        if [ "xemd" != "x${COMM}" ]; then
            continue
        fi
        #echo ${PID} ${COMM} ${CMDLINE}
        CMDLINES=($(cat /proc/${PID}/cmdline | tr "\0" " "))
        for ARG in ${CMDLINES[*]}; do
            if [ ${SYSROOT} = ${ARG} ]; then
                echo ${PID}
            fi
        done
    done
}

# Get the PID of emd under QNX
function FindEmd4Qnx() {
    PID=$(pidin | grep "emd" | grep "sbin" | awk '{print $1}')
    echo ${PID}
}

# Identify the operating system name
function IdentifyOsName() {
    # Method 1: Use the uname command (most direct)
    if uname -a | grep -q "QNX"; then
        echo "QNX"
    elif uname -a | grep -q "Linux"; then
        echo "Linux"
    fi
    return 0
    # Method 2: Check for QNX-specific commands
    if command -v pidin >/dev/null 2>&1; then
        echo "QNX"
    else
        echo "Linux"
    fi
}

function AreStop() {
    local OS_NAME="$(IdentifyOsName)"
    local EMD_PID

    case "${OS_NAME}" in
    "Linux")
        EMD_PID=$(FindEmd4Linux ${ARA_SYSROOT})
        ;;
    "QNX")
        EMD_PID=$(FindEmd4Qnx)
        ;;
    *)
        echo "unkown OS name ${OS_NAME}" >&2
        ;;
    esac

    echo "are stop ..."

    if [ -n "${EMD_PID}" ]; then
        kill -15 ${EMD_PID}
    fi
}

# Uninstall ARE
function AreUninstall() {
    echo "are uninstall ..."
    # Read configuration, delete directories such as framework/core/swcls

    remove_dirs() {
        # Software set directory name, e.g., framework
        local DIR_NAME="$1"
        # Parent directory of the software set, e.g., ara-sysroot/ara
        local ROOT_PATH="$2"
        # Path to be deleted
        local RM_PATH
        # If the ROOT_PATH variable is set, it means the software set is in the default location
        if [ -z "${ROOT_PATH}" ]; then
            RM_PATH="${ARA_SYSROOT}/ara/"
        fi
        RM_PATH="${RM_PATH}/${DIR_NAME}"
        rm -rf "${RM_PATH}"
    }

    remove_dirs "framework" "${ARA_FRAMEWORK_DIR}"
    remove_dirs "core" "${ARA_CORE_DIR}"
    remove_dirs "swcls" "${ARA_SWCLS_DIR}"

    if [ -n "${ARA_VAR_DIR}" ]; then
        rm -rf "${ARA_VAR_DIR}"
    else
        rm -rf "${ARA_SYSROOT}/ara/var"
    fi

    # Delete ara-sysroot itself
    rm -rf ${ARA_SYSROOT}/ara
    rm -rf ${ARA_SYSROOT}/run.sh
    # Do not delete forcefully, because this directory might be a common system directory; delete only if empty
    rmdir ${ARA_SYSROOT} &>/dev/null
}

function Help() {
    echo "Usage:"
    echo "Option | Comment"
    echo "   -s  | Stop machine."
    echo "   -r  | Run machine."
    echo "   -R  | Run machine without cgroup."
    echo "   -a  | Run Application."
    echo "   -c  | Change FunctionGroup State."
    echo "   -C  | Get FunctionGroup State."
    echo "   -m  | Change StateMachine State."
    echo "   -M  | Get StateMachine State."
    echo "   -d  | Run machine in DEBUG mode, and set debug port. must with -r/-R."
    echo "   -u  | Uninstall ARE."
}

####################### Main ########################

GetAraConfig

function Main() {
    local RUN_ARG_DEBUG_PORT="0"
    local RUN_ARG_WITHOUT_CGROUP="false"
    local RUN="false"
    local RUN_AA="false"
    local AA_DIR=""
    while getopts "rRa:d:c:C:m:M:su" CmdOpt; do
        #echo "CmdOpt: " ${CmdOpt}
        case $CmdOpt in
        d)
            RUN_ARG_DEBUG_PORT=${OPTARG}
            ;;
        r)
            RUN="true"
            ;;
        R)
            RUN="true"
            RUN_ARG_WITHOUT_CGROUP="true"
            ;;
        a)
            RUN_AA="true"
            AA_DIR=${OPTARG}
            ;;
        c)
            AreChangeFunctionGroupState ${OPTARG}
            exit
            ;;
        C)
            AreGetFunctionGroupState ${OPTARG}
            exit
            ;;
        m)
            AreChangeStateMachine ${OPTARG}
            exit
            ;;
        M)
            AreGetStateMachine ${OPTARG}
            exit
            ;;
        s)
            AreStop
            exit
            ;;
        u)
            AreUninstall
            exit
            ;;
        ?)
            Help
            exit
            ;;
        esac
    done

    if [ "x${RUN}" = "xtrue" ]; then
        AreRun ${RUN_ARG_WITHOUT_CGROUP} ${RUN_ARG_DEBUG_PORT}
    elif [ "x${RUN_AA}" = "xtrue" ]; then
        AARun ${AA_DIR}
    else
        Help
    fi
}

Main $*
