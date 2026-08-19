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


SDK_SYSROOT="$(dirname $(realpath -e ${BASH_SOURCE[0]}))"
echo ${SDK_SYSROOT}
WORKSPACE="${HOME}/.isoft"
MACHINE=""
APPLICATION=""

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color
log_debug() { echo -e "${BLUE}[DEBUG]${NC} $*"; }
log_info() { echo -e "${GREEN}[INFO]${NC} $*"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $*" >&2; }
log_error() { echo -e "${RED}[ERROR]${NC} $*" >&2; }

# 显示帮助信息
print_help() {
    cat <<EOF
Usage: $0 [OPTIONS]

Update Machine Configuration

OPTIONS:
    -w <workspace-dir>  Set WORKSPACE directory (default: ${HOME}/.isoft)
    -m <machine-name>   Set machine name, must be Machine1 or Machine2
    -h, --help      Show this help message

Example:
    $0 -m Machine1  # Use Machine1
    $0 -m Machine2  # Use Machine2
EOF
}

# 参数解析
while [[ $# -gt 0 ]]; do
    case $1 in
        -w|--workspace)
            WORKSPACE="$2"
            shift 2
            ;;
        -m|--machine)
            MACHINE="$2"
            shift 2
            ;;
        -p|--ipaddr)
            IPADDR="$2"
            shift 2
            ;;
        -a|--application)
            APPLICATION=$(realpath -e "$2")
            shift 2
            ;;
        -s|--sysroot)
            ARA_SYSROOT=$(realpath "$2")
            shift 2
            ;;
        -h|--help)
            print_help
            exit 0
            ;;
        *)
            log_error "Unknown option $1"
            print_help
            exit 1
            ;;
    esac
done

# 检查 configMachine 是否存在
CONFIG_MACHINE="${SDK_SYSROOT}/ara-tools/configMachine"
if [ ! -f "$CONFIG_MACHINE" ]; then
    log_error "configMachine not found at ${CONFIG_MACHINE}"
    exit 1
fi

SAMPLES_DIR="${SDK_SYSROOT}/ara/framework/1.0.0/share/samples"
if [ ! -d "${SAMPLES_DIR}" ]; then
    log_error "${SAMPLES_DIR} not exist. check SDK_SYSROOT"
    exit 1
fi


# 创建必要的子目录
MACHINE_TMP_DIR="${WORKSPACE}/tmp/machines/${MACHINE}"
# 如果没有指定ARA_SYSROOT
#ARA_SYSROOT="${WORKSPACE}/machines/${MACHINE}"

mkdir -p "${ARA_SYSROOT}"
mkdir -p "${MACHINE_TMP_DIR}"

UpdateMachine() {
    echo "========================================="
    echo "Updating machine configuration"
    echo "  WORKSPACE: ${WORKSPACE}"
    echo "  MACHINE:   ${MACHINE}"
    echo "========================================="
    
    ${CONFIG_MACHINE} --keep-ara-gen-output \
        -m /ISOFT/Development/${MACHINE} \
        -r ${ARA_SYSROOT} \
        -w ${MACHINE_TMP_DIR} \
        -e ${HOME}/.isoft/tmp/ara_binout \
        --withSDK ${SDK_SYSROOT} -d -v -v \
        --canUseExistingExe \
        ${SDK_SYSROOT}/ara/framework/1.0.0/share/ara-arxmls \
        ${APPLICATION}/model
    
    if [ $? -eq 0 ]; then
        echo "✅ Machine configuration updated successfully"
        return 0
    else
        echo "❌ Error: Machine configuration update failed"
        return 1
    fi

}

function ReplaceMachineIp() {
    local SCAN_DIR="$1"
    local NEW_IP="$2"

    # 参数校验
    if [ $# -ne 2 ]; then
        echo "用法: replace_json_ip 目标目录 新IP地址"
        return 1
    fi

    # 目录校验
    if [ ! -d "${SCAN_DIR}" ]; then
        echo "错误：目录不存在" >&2
        return 1
    fi

    # 匹配KEY（你的原始配置）
    local KEYS="host|address|reliable|unreliable|etherConnector|ipv4address|unicastIP|ipv4IpAddress|ethernetConnector|ipv4Address"
    # 核心正则（你的原始正则）
    local REG="\"(${KEYS})\"\\s*:\\s*\"([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})(:[0-9]+)?\""

    # 执行替换（你的原始逻辑）
    grep -rl -E "${REG}" --include="*.json" "${SCAN_DIR}" | while read -r file; do
        sed -i -E "s/${REG}/\"\1\": \"${NEW_IP}\3\"/g" "${file}"
    done
}

# 更新机器
UpdateMachine
# 替换IP地址
ReplaceMachineIp ${ARA_SYSROOT}/ara ${IPADDR}
# 拷贝运行脚本
cp -v ${SDK_SYSROOT}/run.sh ${ARA_SYSROOT}/

exit $?