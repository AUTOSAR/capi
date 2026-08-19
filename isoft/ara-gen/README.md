# aragen

> AUTOSAR Adaptive Platform Code & Configuration Generator — Automatically generates C++ code, process manifests, machine configurations, and network binding files from ARXML models

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Requirements](#requirements)
- [Quick Start](#quick-start)
- [Command-Line Arguments](#command-line-arguments)
- [Generated Artifacts](#generated-artifacts)
- [Examples](#examples)
- [Known Limitations](#known-limitations)

---

## Overview

`aragen` is the core code generation tool in the AUTOSAR Adaptive Platform (AP) toolchain. It reads AUTOSAR ARXML model files, parses machine definitions, process definitions, executables, service interfaces, software clusters, and other elements, then automatically generates:

- **C++ Code** — Service interface Proxy/Skeleton headers, data type headers, runtime code
- **Process Configurations** — Execution manifests, service instance manifests, persistency configs, PHM/E2E/Crypto configs, etc.
- **Machine Configurations** — Machine manifests, time synchronization, network configs, DLT logging configs, NM/IAM/IDSM configs, etc.
- **Network Bindings** — SOME/IP, NPC, ICC, DDS (FastDDS) binding configuration files
- **Software Cluster Info** — Software cluster manifests, function group definitions, vehicle package configs
- **CMake Projects** — Complete CMake build project structure (`-g PROJECT` mode)

The tool uses a multi-process parallel generation architecture — process configs, machine configs, and diagnostic configs are generated simultaneously in separate processes, significantly improving throughput for large-scale models.

## Features

- **Full-Stack Code Generation** — One-command generation of C++ Proxy/Skeleton, type definitions, runtime code, and CMake projects from ARXML models
- **Multi-Network Binding Support** — Built-in generators for SOME/IP (nsomeip2), NPC, ICC, and DDS (FastDDS) network bindings
- **Multi-Process Parallelism** — Process, machine, and diagnostic configurations generated concurrently in independent processes
- **Selective Generation** — Filter generation scope by machine FQN, process FQN, executable FQN, or software cluster FQN
- **ARXML Pre-Validation** — Supports UUID duplicate checking, FQN uniqueness checking, and reference validity checking
- **Information Query Mode** — List machines, processes, software clusters, etc. defined in the model without generating any files
- **DDS IDL Generation** — Automatically generates DDS IDL files and type converters
- **Diagnostic Code Generation** — Generates C++ headers and implementation files for diagnostic elements, identifiers, and routines
- **Crypto & Security** — Generates Crypto manifests, E2E protection configs, IAM/IDSM contribution configs

Tips:
  - FQN: Fully Qualified Name

## Requirements

| Requirement | Version                             |
| ----------- |-------------------------------------|
| Python      | >= 3.8                              |
| OS          | Linux (Ubuntu 20.04 >= recommended) |

## Quick Start

### 1. Generate All Configurations and Code

```bash
./isoft/ara-gen/aragen -o ./output ./isoft/arxmls
```

Reads all .arxml files from the ./input directory — it will recursively traverse all ARXML files under the directory and generate all configurations and code to the ./output directory.
### 2. Generate a C++ Project.
Tips:
  - C++ projects can only be generated for Executables
  - An Executable must contain Ports that reference Interfaces.

```bash
./isoft/ara-gen/aragen -g PROJECT -e /ara/sm_shell_request_client/executables/shell_request_client -o ./output ./isoft/arxmls
```

Generates a complete C++ project structure (CMakeLists.txt, main.cpp, headers, etc.) for the specified executable.

### 3. Generate a Single Process Configuration

```bash
./isoft/ara-gen/aragen -p /ara/sm_shell_request_client/processes1/shell_request_proc -o ./output ./isoft/arxmls
```

### 4. Generate a Single Machine Configuration

```bash
./isoft/ara-gen/aragen -m /ISOFT/Development/Machine1 -o ./output ./isoft/arxmls
```

### 5. Query Model Information

```bash
# List all machines
./isoft/ara-gen/aragen --list-machines ./isoft/arxmls

# List all software clusters with detailed info
./isoft/ara-gen/aragen --list-swcl-info ./isoft/arxmls

# List all processes
./isoft/ara-gen/aragen --list-processes ./isoft/arxmls
```

## Command-Line Arguments

### Core Arguments

| Argument      | Shorthand    | Description                                                                                         | Required  |
| ------------- | ------------ | --------------------------------------------------------------------------------------------------- | :-------: |
| `--output`    | `-o <dir>`   | Output directory                                                                                    | Yes       |
| —             | `<files>`    | Input files/directories (multiple allowed; all `.arxml` files in directories are used)              | Yes       |
| `--generate`  | `-g <type>`  | Generation type: `ALL`, `CODE`, `PROJECT`, `PROJECT_UPDATE`. Default: `ALL`                         | No        |
| `--debug`     | `-d`         | Enable debug mode with more detailed error output                                                   | No        |

### Selective Generation Arguments

| Argument                    | Shorthand      | Description                                                                 |
| --------------------------- | -------------- | --------------------------------------------------------------------------- |
| `--machines`                | `-m <fqn>`     | Generate config for specified machines only (comma-separated FQNs)          |
| `--processes`               | `-p <fqn>`     | Generate config for specified processes only (comma-separated FQNs)         |
| `--executables`             | `-e <fqn>`     | Generate code for specified executables only (comma-separated FQNs)         |
| `--software-components`     | `-s <fqn>`     | Generate code for specified software components only (comma-separated FQNs) |
| `--softwarePackageFqn`      | `-P <fqn>`     | Specify software package FQN                                                |
| `--software-cluster <fqn>`  | —              | Specify software cluster FQN                                                |
| `--vehiclePackageFqn`       | `-V <fqn>`     | Specify vehicle package FQN                                                 |

### Generation Behavior

The behavior of `ALL` depends on whether an executable is selected:

* `ALL` without `-e` generates all configuration files.
* `ALL` with `-e` generates includes and network-binding files for the selected executable.
* `CODE` with `-e` also generates includes and network-binding files for the selected executable.

Code is generated only if the selected executable has a matching interface. 

Example:

```bash
./isoft/ara-gen/aragen \
    isoft/arxmls \
    -g CODE \
    -e /ara/sm_shell_request_client/executables/shell_request_client \
    -o ./output
```

### Network Binding Arguments

| Argument            | Default    | Options               | Description                     |
| ------------------- | ---------- |-----------------------| ------------------------------- |
| `--someip-libs`     | `nsomeip2` | `nsomeip2`, `none`    | SOME/IP network binding library |
| `--ipc-libs`        | `npc`      | `npc`, `none`         | IPC network binding library     |
| `--icc-libs`        | `icc`      | `icc`, `none`         | ICC network binding library     |
| `--dds-libs`        | `fastdds`  | `fastdds`             | DDS network binding library     |
| `--qos-path <path>` | —          | —                     | QoS template file path          |

> Setting `--someip-libs` to `none` (--someip-libs=none) will skip generating that binding type.

### Query Arguments

| Argument                  | Description                                                            |
| ------------------------- | ---------------------------------------------------------------------- |
| `--list-machines`         | List all machine names defined in input files and exit                 |
| `--list-processes`        | List all process names defined in input files and exit                 |
| `--list-swc`              | List all software component names defined in input files and exit      |
| `--list-swp`              | List all software package names defined in input files and exit        |
| `--list-swcl-info`        | List complete information for all software clusters and exit           |
| `--list-files`            | Print file paths that would be generated (without actually generating) |

### Other Arguments

| Argument                              | Description                                                                       |
|---------------------------------------|-----------------------------------------------------------------------------------|
| `-n` / `--dry-run`                    | Dry run mode — execute the full pipeline without generating files                 |
| `--version`                           | Display version number and exit                                                   |
| `--single-process`                    | Single-process mode (for debugging)                                               |
| `--force-pkg-path-fallback`           | Force using package name hierarchy as fallback path when namespace is unavailable |
| `--soa-element-cpp-def-style <style>` | C++ definition style for SOA elements                                             |
| `--check-uuid`                        | Check for duplicate UUIDs in ARXML files                                          |
| `--no-reference-checking`             | Skip reference validity pre-check                                                 |
| `--help`                              | Available parameters and tips                                                     |

## Generated Artifacts

### Default Mode (`-g ALL`)

Generates all of the following:

Tips:
  - Execute the command: `./isoft/ara-gen/aragen -o ./output ./isoft/arxmls`
  - Template only serves as an intermediate node conversion and does not require much attention.

#### Process-Level Configurations (`processes/` directory)

| File                                                  | Template                                   | Description                                      |
| ----------------------------------------------------- | ------------------------------------------ | ------------------------------------------------ |
| `{process}_manifest.json`                             | `execution_manager_manifest.j2`            | Execution management manifest                    |
| `{process}_service_instance_manifest.json`            | `service/service_instance_manifest.j2`     | Service instance manifest                        |
| `{process}_storage_manifest.json`                     | `storage_manifest.j2`                      | Persistency storage manifest                     |
| `{process}_{kvs}_init.per`                            | `ap_per.j2`                                | KVS initialization file                          |
| `{process}_time_base_map.json`                        | `time_base_map.j2`                         | Time base mapping                                |
| `{process}_nsomeip.json`                              | `nsomeip2_binding/process_nsomeip_json.j2` | nsomeip2 communication config                    |
| `{process}_npc.json`                                  | `npc_binding/process_npc_json.j2`          | NPC communication config                         |
| `{process}_icc.json`                                  | `icc_binding/process_icc_json.j2`          | ICC communication config                         |
| `{process}_phm.json`                                  | `phm_process_json.j2`                      | PHM process config                               |
| `{process}_crypto_manifest.json`                      | —                                          | Crypto process manifest                          |
| `{process}_e2e_statemachines.json`                    | —                                          | E2E state machine config                         |
| `{process}_e2e_dataid_mapping.json`                   | —                                          | E2E Data ID mapping                              |
| `{process}_log.json`                                  | —                                          | Logging config                                   |
| `{process}_raw.json`                                  | —                                          | Raw data stream config                           |
| `{process}_cg_manifest.json`                          | —                                          | Communication group manifest                     |
| `{process}_trigger_fg_service_instance_manifest.json` | —                                          | State management trigger function group manifest |
| `{process}_state_machine.json`                        | —                                          | State machine config                             |
| `{process}_secoc.json`                                | —                                          | SecOC config                                     |
| `{process}_secoc_binding.json`                        | —                                          | SecOC binding config                             |
| `{process}_qos.json`                                  | —                                          | QoS JSON config                                  |
| `{process}_rtps.xml`                                  | —                                          | RTPS XML config                                  |

#### Machine-Level Configurations (`machines/` directory)

| File                               | Template                    | Description                 |
| ---------------------------------- | --------------------------- | --------------------------- |
| `{machine}_machine_manifest.json`  | `machine_manifest.j2`       | Machine manifest            |
| `{machine}_time_sync.json`         | `time_sync.j2`              | Time synchronization config |
| `{machine}_dlt.conf`               | `dlt_conf.j2`               | DLT logging config          |
| `{machine}_etc_network_interfaces` | `etc_network_interfaces.j2` | Network interface config    |
| `{machine}_nm.json`                | —                           | Network management config   |
| `{machine}_nm_mapping.json`        | —                           | NM mapping config           |
| `{machine}_phm_contribute.json`    | —                           | PHM contribution config     |
| `{machine}_crypto_manifest.json`   | —                           | Crypto machine manifest     |
| `{machine}_iam_com.json`           | —                           | IAM communication config    |
| `{machine}_iam_raw.json`           | —                           | IAM raw config              |
| `{machine}_iam_idsm.json`          | —                           | IAM IDSM config             |
| `{machine}_iam_phm.json`           | —                           | IAM PHM config              |
| `{machine}_iam_crypto.json`        | —                           | IAM Crypto config           |
| `{machine}_idsm_contribute.json`   | —                           | IDSM contribution config    |
| `{machine}_log-emd.json`           | —                           | Logging EMD config          |
| `{machine}_log_global_config.json` | —                           | Global logging config       |
| `{machine}_ide_machine_info.json`  | —                           | IDE machine info            |

#### Software Clusters & Vehicle Packages (`software-clusters/`, `software-packages/`, `vehicles/` directories)

| File                          | Template            | Description                           |
| ----------------------------- | ------------------- | ------------------------------------- |
| `{swcl}_swcl_manifest.json`   | `swcl_manifest.j2`  | Software cluster manifest             |
| `{swcl}_function_groups.json` | —                   | Function group definitions            |
| `{swpkg}_swpkg_manifest.json` | `swpkg_manifest.j2` | Software package manifest             |
| `ucm_vps.json`                | `ucm_vps.j2`        | Vehicle package info                  |
| `ucm_swcls.json`              | `ucm_swcls.j2`      | Vehicle package software cluster list |
| `ucm_swps.json`               | `ucm_swps.j2`       | Vehicle package software package list |

#### Diagnostic Configurations (`processes/` directory)

| File              | Description                         |
| ----------------- | ----------------------------------- |
| `dmd_dmd.json`    | Diagnostic management daemon config |
| `dmd_doip.json`   | DMD DoIP config                     |
| `doipd_doip.json` | DoIP daemon config                  |

### Project Mode (`-g PROJECT`)

Generates a complete C++ project structure:

```
output/
├── CMakeLists.txt              # Top-level CMake build file
├── .gitignore
├── files/
│   ├── aragen-helper.cmake     # aragen helper CMake config
│   └── instance_specifier.txt  # Instance specifier
├── src/
│   ├── CMakeLists.txt
│   └── main.cpp                # Main program entry
└── test/
    ├── CMakeLists.txt
    └── test1.cpp               # Test file
```

## Examples

### Generate Everything

```bash
./isoft/ara-gen/aragen --single-process -o ./output ./isoft/arxmls
```

### Generate a C++ Project: C++ projects can only be generated for Executables

```bash
./isoft/ara-gen/aragen -g PROJECT -e /ClientApp/exe/clientd -o ./output ./isoft/arxmls ./samples/helloworld-cm
```

### Generate a C++ CODE: An Executable must contain Ports that reference Interfaces.

```bash
./isoft/ara-gen/aragen -g CODE -e /ClientApp/exe/clientd -o ./output/gen ./isoft/arxmls ./samples/helloworld-cm
```

### Generate a Single Process Configuration

```bash
./isoft/ara-gen/aragen -p /ClientDeployment/client_process -o ./output ./isoft/arxmls ./samples/helloworld-cm
```

### Generate a Single Machine Configuration

```bash
./isoft/ara-gen/aragen -m /ISOFT/Development/Machine1 -o ./output ./isoft/arxmls
```

### Specify Network Bindings (Config Only)

```bash
./isoft/ara-gen/aragen -o ./output \
  --someip-libs=nsomeip2 \
  --dds-libs=fastdds \
  --ipc-libs=npc \
  ./isoft/arxmls
```

### Disable All Network Bindings (Config Only)

```bash
./isoft/ara-gen/aragen -o ./output \
  --someip-libs=none \
  --dds-libs=none \
  --ipc-libs=none \
  --icc-libs=none \
  ./isoft/arxmls
```

### Query Model Information

```bash
# List all machines
./isoft/ara-gen/aragen --list-machines ./isoft/arxmls

# List all processes
./isoft/ara-gen/aragen --list-processes ./isoft/arxmls

# List all software components
./isoft/ara-gen/aragen --list-swc ./isoft/arxmls

# List all software packages
./isoft/ara-gen/aragen --list-swp ./isoft/arxmls

# List complete software cluster info (FQN:category:version:dependencies:processes:exe:RootSWC:machines)
./isoft/ara-gen/aragen --list-swcl-info ./isoft/arxmls
```

### Multiple Input Directories

```bash
./isoft/ara-gen/aragen -o ./output ./arxmls/models ./arxmls/machine ./arxmls/applications
```

## Known Limitations

- `ApplicationRecordDataTypes` does not support optional elements
