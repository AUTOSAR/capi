==================================================================
CAPI Adaptive Platform Stack - Software Architecture Documentation
==================================================================

Introduction
============

This document provides a detailed technical description of the software architecture of the AUTOSAR Adaptive Platform stack that is developed as a part  of the AUTOSAR Common Adaptive Platform Implementation (CAPI).
CAPI also provides tools for developing, configuring and using the AUTOSAR Adaptive Platform Stack.
These tools are not described in the document at hand.

Objectives
----------

This document is an architecture description of CAPI in accordance to [ISO42010:2022]_ and has the following main objectives:

- Identify the stakeholders of CAPI and their concerns.
- Identify the system scope and provide overview information of CAPI.
- Provide definitions for all used architecture viewpoints and a mapping of all stakeholder concerns to those viewpoints.
- Provide an architecture view and its architecture models for each architecture viewpoint used in this architecture description.
- Provide correspondence rules and correspondences among the contents of this architecture description.
- Provide an architecture rationale (explanation, justification, reasoning for decisions made) on a high level.
  A more in-depth documentation of decisions is provided in Architecture Decision Records (ADRs) [ADR]_.
- Provide a record of known inconsistencies and gaps among the architecture description.

Scope
-----

This document applies to CAPI.
It is recommended to get an overview of CAPI for all members of AUTOSAR working groups who want to contribute to CAPI, and for stack vendors and application developers who want to use CAPI in their projects.

Document Structure
------------------

This document is organized as follows.
The introduction is followed by a section that  provides an overview of the main goals and requirements for CAPI, the top quality goals of its architecture, and a list of stakeholders that are affected by it.
Section 3 lists requirements that constrain design and implementation decisions or decisions about the development process.
Section 4 is the base for discovering trade-offs and sensitivity points in the architecture of CAPI by introducing a quality attribute tree followed by the most important quality scenarios.
The system context in which CAPI is intended to be used is outlined in section 5.
Section 6 summarizes the fundamental decisions and solution strategies, that shape the architecture of CAPI such as technology decisions or architectural patterns.
Sections 7 to 10 explain the software architecture from different view points.
First, section 7 explains the decomposition of CAPI and the responsibilities and interdependencies of its building blocks.
Then, section 8 illustrates important runtime scenarios.
Section 9, the development view, bridges the gap between the software architecture and the source code.
Finally, Section 10 shows different scenarios how applications based on CAPI may be deployed.
Section 11 provides an overview of the cross-cutting concepts and patterns used by several building blocks of CAPI.
This is followed by a list of known technical debts and risks.
Finally, we define terms and acronyms before listing the references.

Architecture decisions are document separately in the form of architecture decision records (ADRs).
Please find them in [ADR]_.
Additionally, we use process decision records (PDRs) to describe decisions regarding the development process [PDR]_.

The software architecture documentation contains sections for all architectural aspects.
Some sections may be empty until there is content to document for the respective topic.
This is intended and there is no need or pressure to "fill-in" all sections.

Overview and Goals
==================

CAPI is a community-source implementation of the AUTOSAR Adaptive Platform Standard.
For a high-level overview of the project and its background see the introduction of /README.rst and https://www.autosar.org/capi.

Overarching Goals
-----------------

**Provide a useful implementation of automotive middleware to the AUTOSAR partners**

Advantages:

- Provide direct value to the partners
- Motivation for contribution
- Point of standardization 

**CAPI is the implementation of the AUTOSAR Adaptive Platform**

Advantages:

- Utilize all the effort, know-how, and conflict resolution work done already
- Utilize existing know-how within partners about the utilization of the AUTOSAR Adaptive Platform
- Utilize existing code / tools / products within partners that already use the AUTOSAR Adaptive Platform
- In line with AUTOSAR strategy (in contrast to theoretical possibility of developing something not in line with the standard)
- Chance to improve the AUTOSAR Adaptive Platform standard

**Increase the overall attractiveness of the AUTOSAR Adaptive Platform**

Advantages:

- Incentivize use of the AUTOSAR Adaptive Platform
- Incentivize contribution
- Fosters standardization
- Contribute to overall AUTOSAR vision
- "AUTOSAR will be the global established standard for software and methodology" (AUTOSAR_EXP_Introduction_Part1)
- Concentrate more effort on one common solution

Requirements Overview
---------------------

Quality Goals
-------------

Stakeholders
------------

+-------------------+--------------------------------+----------------------------------------------------------------------------------------+
| Role / Name       | Email                          | Expectations                                                                           |
+===================+================================+========================================================================================+
|                   |                                |                                                                                        |
+-------------------+--------------------------------+----------------------------------------------------------------------------------------+

Architecture Constraints
========================

Quality Requirements
====================

Quality Attributes
------------------

Quality Scenarios
-----------------

System Scope and Context
========================

Business Context
----------------

Toolchain Provided by CAPI:

+----------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Name                       | Description                                                                                                                                                                                  |
+============================+==============================================================================================================================================================================================+
| **ara-gen**                | Code generator. Generates Proxy/Skeleton C++ code and Daemon build configs from ARXML models.                                                                                                |
+----------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| **ara-tools**              | Runtime toolset, installed to ``${ARA_SYSROOT}/ara-tools/``.                                                                                                                                 |
+----------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| **arxmls**                 | ARXML model definition library. Contains all platform component ARXML configurations and ``components.json`` with dependency and classification definitions.                                 |
+----------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| **sdk-utils**              | sdk-utils provides complete toolchain from source build to SDK/ARE packaging. SDK build and packaging tools. Provides ``build``/``pack``/``publish``/``update``/``are`` subcommands,         |
|                            | automating download, compilation, installation of dependencies, and packaging as self-extracting ``.run`` files.                                                                             |
+----------------------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

External Communication Partners:

+----------------------------+------------------------------------------------------------+------------------------------------------------------+
| Name                       | Description                                                |  Input / Output                                      |
+============================+============================================================+======================================================+
| Adaptive Applications (AA) | User-authored Adaptive Applications                        |  Consume platform services via ara::* API            |
+----------------------------+------------------------------------------------------------+------------------------------------------------------+
| Linux Kernel / POSIX       | Basis of CAPI                                              |  Control of system functions (e.g. processes, I/O)   |
+----------------------------+------------------------------------------------------------+------------------------------------------------------+

Solution Strategy
=================

Architectural Approach
----------------------

CAPI is the reference implementation of the AUTOSAR Adaptive Platform standard.
Accordingly, the EXP Software Architecture [AP_EXP_SWArchitecture]_ builds the basis of the CAPI architecture.

Daemon vs Library Separation Principle
**************************************

Daemon/Library separation: Modules with independent runtime requirements are compiled as Daemons; pure function libraries are compiled as Libraries

Rule: Modules that need to call ``ara::core::Initialize()`` and hold their own event loop are compiled as standalone daemons.

Decomposition Strategy
----------------------

CAPI follows the structure described in EXP Software Architecture [AP_EXP_SWArchitecture]_.

Technology
----------

Implementation in C++17 in alignment with the C++ version of the AUTOSAR AP standard.
CAPI is open to integrate RUST parts in the future.
Currently, there are no RUST parts in CAPI and the interaction mechanisms between CAPI and RUST are not yet defined.

Design Principles
-----------------

Building Block View
===================

Overall System View
-------------------

::

    +----------------------------------------------------------------------------------------------------------------------+
    |  AP Functional Clusters                                                                                              |
    |----------------------------------------------------------------------------------------------------------------------|
    | +-----------+  +-----------+  +---------+  +----------+  +----------+  +-----------+  +----------+  +----------+     |
    | | ara::core |  | ara::exec |  | ara::sm |  | ara::phm |  | ara::com |  | ara::diag |  | ara::log |  | ara::per |     |
    | +-----------+  +-----------+  +---------+  +----------+  +----------+  +-----------+  +----------+  +----------+     |
    |         +-------------+  +----------+  +-----------+  +---------+  +------------+  +----------+  +---------+         |
    |         | ara::crypto |  | ara::iam |  | ara::idsm |  | ara::nm |  | ara::tsync |  | ara::ucm |  | ara::fw |         |
    |         +-------------+  +----------+  +-----------+  +---------+  +------------+  +----------+  +---------+         |
    +----------------------------------------------------------------------------------------------------------------------+
                                                            |
                                                            V
    +----------------------------------------------------------------------------------------------------------------------+
    |  Infrastructure                                                                                                      |
    +----------------------------------------------------------------------------------------------------------------------+

+--------------------+---------------------------------------------------------------------------------------------------+
| Name               | Responsibility                                                                                    |
+====================+===================================================================================================+
| ara::core          | Runtime initialization and deinitialization                                                       |
+--------------------+---------------------------------------------------------------------------------------------------+
| ara::exec          | Execution management                                                                              |
+--------------------+---------------------------------------------------------------------------------------------------+
| ara::com           | Communication                                                                                     | 
+--------------------+---------------------------------------------------------------------------------------------------+
| ara::phm           | Health management                                                                                 |   
+--------------------+---------------------------------------------------------------------------------------------------+
| ara::per           | Persistence                                                                                       |  
+--------------------+---------------------------------------------------------------------------------------------------+
| ara::nm            | Network management                                                                                | 
+--------------------+---------------------------------------------------------------------------------------------------+
| ara::ucm           | Update management                                                                                 |
+--------------------+---------------------------------------------------------------------------------------------------+
| ara::iam           | Access control                                                                                    |
+--------------------+---------------------------------------------------------------------------------------------------+
| ara::sm            | State management                                                                                  |
+--------------------+---------------------------------------------------------------------------------------------------+
| ara::diag          | Diagnostics                                                                                       |
+--------------------+---------------------------------------------------------------------------------------------------+
| ara::log           | Logging                                                                                           |
+--------------------+---------------------------------------------------------------------------------------------------+
| ara::crypto        | Crypto                                                                                            |
+--------------------+---------------------------------------------------------------------------------------------------+
| ara::tsync         | Time synchronization                                                                              |
+--------------------+---------------------------------------------------------------------------------------------------+
| ara::fw            | Firewall                                                                                          |
+--------------------+---------------------------------------------------------------------------------------------------+
| ara::idsm          | Intrusion detection                                                                               |
+--------------------+---------------------------------------------------------------------------------------------------+
| Infrastructure     | OS abstraction and communication infrastructure                                                   |
+--------------------+---------------------------------------------------------------------------------------------------+

Module Dependency Relationships:

- **BASE_LIBRARIES**: All FC modules depend on Infrastructure
- **ara-com**: The heaviest dependency load (depends on Infrastructure + e2e + nsomeip + npc + log + crc + iam + crypto + exec)
- **ara-nm**: The broadest dependency span (depends on Infrastructure + log + core + exec + com + diag + nm-ethoper)

Level 2
-------

Complex or complicated building blocks from the overall system view are further detailed in the following subsections.

Infrastructure
**************

::

     +---------------------+
     | Infrastructure      |
     +---------------------+
     | Communication Stack |
     |    OS Abstraction   |
     +---------------------+

+---------------------+--------------------------------------------------------------------------------------------------+
| Name                | Responsibility                                                                                   |
+=====================+==================================================================================================+
| Communication Stack | Unix Domain Socket lifecycle management, IPC framework, SOME-IP                                  |
+---------------------+--------------------------------------------------------------------------------------------------+
| OS Abstraction      | Cross-platform OS abstraction and C++ event loop wrapper                                         |
+---------------------+--------------------------------------------------------------------------------------------------+

ara::core - Runtime Initialization
**********************************

**ara-core** (header-only): Runtime initialization

- ``ara::core::Initialize()`` — Sequentially initializes all FC modules (log→per→crypto→com→diag)
- ``ara::core::Deinitialize()`` — Reverse-order deinitialization
- Uses conditional compilation macros (``HAS_ARA_*``) to control module initialization

For the basic types defined in the ara::core namespace, see section `Basic Functions`_ in the `Cross-cutting Concepts`_.

ara::exec — Execution Management
********************************

**Daemon**: ``emd`` (Execution Manager Daemon), reason: System process management must run independently; first process started

**Client libraries**: ``ExecutionClient``, ``StateClient``, ``UpdateClient``

**Core responsibility**: Controls machine startup/shutdown, process lifecycle management, function group state transitions.

**Key components**:

+----------------------+-----------------------------------------------------------------------------+
| Name                 | Responsibility                                                              |
+======================+=============================================================================+
| ``ExecutionManager`` | Main daemon class, manages FunctionGroups/Processes/ResourceGroups          |
+----------------------+-----------------------------------------------------------------------------+
| ``ExecutionClient``  | AA reports execution state: ``ReportExecutionState(kRunning/kTerminating)`` |
+----------------------+-----------------------------------------------------------------------------+
| ``StateClient``      | Requests function group state transitions: ``SetState(functionGroupState)`` |
+----------------------+-----------------------------------------------------------------------------+
| ``FunctionGroup``    | Function group abstraction, uses Preconstruct() two-phase construction      |
+----------------------+-----------------------------------------------------------------------------+

**Communication**: Communicates with AA via three IPC protocols: EMS (Execution Management Service), SMS (State Management Service), UMS (Update Management Service) based on ipc/IPCBase.

ara::sm — State Management
**************************

**Daemon**: ``smd``, reason: State coordination hub, requires independent event loop

**Core architecture**: Event-driven CommManager pattern

**Key components**:

+------------------------+---------------------------------------------------------------------------------------------------------------+
| Name                   | Responsibility                                                                                                |
+========================+===============================================================================================================+
| ``EventManager``       | Core event dispatcher, manages lifecycle and event distribution for all CommManagers                          |
+------------------------+---------------------------------------------------------------------------------------------------------------+
| ``EMCommManager```     | Communicates with EM: uses `StateClient` to request function group state, ``ExecutionClient`` to report state |
+------------------------+---------------------------------------------------------------------------------------------------------------+
| ``PHMCommManager``     | Communicates with PHM: executes health recovery actions (RecoveryAction)                                      |
+------------------------+---------------------------------------------------------------------------------------------------------------+
| ``DMCommManager``      | Communicates with Diag: handles ECU reset requests                                                            |
+------------------------+---------------------------------------------------------------------------------------------------------------+
| ``NMCommManager``      | Communicates with NM: coordinates via network management (through ara::com)                                   |
+------------------------+---------------------------------------------------------------------------------------------------------------+
| ``UCMCommManager``     | Communicates with UCM: coordinates software update flows                                                      |
+------------------------+---------------------------------------------------------------------------------------------------------------+
| ``FGStateCommManager`` | Function group state communication: TriggerIn/Out/IO modes                                                    |
+------------------------+---------------------------------------------------------------------------------------------------------------+
| ``StateMachineManager``| State machine management: tracks overall system state                                                         |
+------------------------+---------------------------------------------------------------------------------------------------------------+

ara::com — Communication Management
***********************************

**Daemon**: ``nsomeipd`` (SOME/IP Routing Daemon), reason: Network binding and Service Discovery require independent process

**Core design**: Binding abstraction layer + Runtime singleton pattern

**Layered architecture**:

::

    Skeleton (Provider)          Proxy (Requester)
    OfferService()              FindService()
    ProcessMethodCall()         Subscribe()/GetNewSamples()
            ↓                           ↓
            └──────── Binding Layer ────┘
            ┌──────────────┬──────────────┬──────────────┐
            │ nsompeip     │ npc          │ icc          │ fastdds
            │ (SOME/IP)    │ (Zero-copy)  │ (In-process) │ (DDS)
            └──────────────┴──────────────┴──────────────┘

**Key APIs**:

- ``ServiceProxy::Create()`` — Create client proxy
- ``ServiceProxy::FindService()`` — Synchronous service discovery
- ``ServiceProxy::StartFindService()`` — Asynchronous service discovery
- ``ServiceSkeleton::OfferService()`` — Offer a service
- ``ServiceSkeleton::ProcessNextMethodCall()`` — Process method calls

**Runtime architecture**: ``Runtime::GetInstance()`` singleton manages all ``BindRuntime`` instances, supports nsompeip/npc/icc/fastdds bindings, registered via ``RegisterBindRuntime()``.

ara::log — Logging and Tracing
******************************

**Daemon**: ``logd``, reason: Log collection/filtering/routing must be independent of applications

**Client library**: ``ara::log``

**Protocol**: DLT (Diagnostic Log and Trace) + TCP remote logging

**Key APIs**:

- ``CreateLogger(instanceSpecifier)`` — Create a Logger
- ``Logger::LogInfo() << "my log" << "information"`` — Streaming log  
- ``Logger::LogInfo() << Arg(val, name) << Arg(val2, name2, unit2)`` — Streaming log 
- ``RemoteClientState()`` — Remote client state

**Interfaces**: Library formats logs → IPC → Daemon filters/routes → File/DLT-Viewer/Network.

ara::phm — Platform Health Management
*************************************

**Daemon**: ``phmd``, reason: Health supervision must be independent of supervised processes

**Client libraries**: ``phm_client``, ``recovery_action``

**Supervision types**:

+---------------------+------------------------------------------------------------+
| Supervision         | Function                                                   |
+=====================+============================================================+
| AliveSupervision    | Periodic checkpoint, timeout indicates process abnormality |
+---------------------+------------------------------------------------------------+
| DeadlineSupervision | One-time deadline, indicates task timeout                  |
+---------------------+------------------------------------------------------------+
| LogicalSupervision  | Condition check, indicates logic violation                 |
+---------------------+------------------------------------------------------------+

**State machine**: kDeactivated → kOK → kFailed → kExpired → kStopped

**Recovery mechanism**:

1. AA → ``SupervisedEntity::ReportCheckpoint()`` / ``HealthChannel::ReportHealthStatus()`` → IPC → PHMD
2. PHMD supervision timeout → IPC → SM's ``RecoveryAction::RecoveryHandler()``
3. SM restarts/stops/switches to backup function group via EM

ara::diag — Diagnostics Management
**********************************

**Daemon**: ``diagd`` (Diagnostic Manager), reason: DoIP/UDS protocol stack needs independent external request handling

**Protocol stack**: DoIP (ISO 13400) + UDS (ISO 14229)

**Communication chain**:

::
        
    External Tester ↔ DoIP (TCP 13400/UDP 13400) ↔ DoIPServer ↔ UDS Server
                                                            ↓
                                                    DiagnosticManager
                                                    ├── SessionManagement
                                                    ├── SecurityAccessManagement
                                                    ├── FaultManagement (DTC)
                                                    ├── EcuResetManagement
                                                    ├── RoutineManagement
                                                    └── DataTransferManagement

**UDS services**: Session control, security access, fault management (DTC), ECU reset, routine control, upload/download, communication control, authentication.

ara::crypto — Cryptography Module
*********************************

**Daemons**: ``cryptod`` (key management), ``cryptox509d`` (certificate management), reason: Key isolation and HSM access require independent process

**Backend support**: OpenSSL (software) / NXP HSM / BST HSM (hardware)

**Key APIs**:

- ``CryptoProvider`` / ``CryptoService`` — Crypto service entry points
- Symmetric encryption (AES), asymmetric encryption, hashing (SHA), MAC, signature, random number, KDF, KEM

**Interfaces**: AA API → lib_isoft (crypto backend) → IPC → cryptod/cryptox509d → HSM/file storage.

ara::per — Persistence
**********************

**Pure client library** (no daemon), directly operates on files within the AA process.

**Key APIs**:

- ``ara::per::KeyValueStorage`` — KV storage, supports scalar/String/Array/Vector/custom types
- ``ara::per::FileStorage`` — File storage, supports read/write/append/truncate modes
- ``Recover()`` — Data recovery (M/N redundancy)

ara::nm — Network Management
****************************

**Daemon**: `nmd`, reason: UDP NM protocol requires independent process

**Protocol**: UDP NM (AUTOSAR NM)

**Key APIs**:

- ``ara::nm::NetworkHandle::GetNetworkState()`` → kNoCom / kFullCom
- ``ara::nm::NetworkHandle::SetNetworkRequestedState()``
- ``RegisterNetworkStateChangeNotifier()`` — State change callback

**State machine**: Coordinates network sleep/wake between ECUs via UDP NM messages.

ara::tsync — Time Synchronization
*********************************

**Daemon**: tsyncd, reason: gPTP time master/slave requires independent process

**Protocol**: IEEE 802.1AS / gPTP

**Intra-ECU communication**: Shared memory time distribution

**API**: ``ara::tsync::Clock::now()`` — Get globally synchronized time.

ara::ucm — Update and Configuration Management
**********************************************

**Daemons**: ``pkgmgr`` (ECU-level) + ``vpkgmgr`` (vehicle-level), reason: OTA updates must be independent of the updated process

**OTA flow**:

::

    Cloud → vucm (vehicle-level) → ara::com → ucm (ECU-level)
                                            ├── Package extraction (ZIP)
                                            ├── Crypto verification (signature/hash)
                                            ├── Update state machine (Download→Install→Activate→Rollback)
                                            └── Trigger process restart via exec

ara::fw — Firewall
******************

**Daemon**: ``fwd``, reason: Network packet filtering requires independent process

**Filter chain**: DataLinkFilter → IPv4/IPv6Filter → TCPFilter → DoIP/SomeIp/DDS/PayloadFilter

ara::iam — Identity and Access Management
*****************************************

**Pure static library** (linked into each FC process), authorization checks driven by Manifest configuration.

ara::idsm — Intrusion Detection
*******************************

**Daemon**: ``idsmd``, reason: Security event collection and aggregation require independent process

**Interfaces**: ``ara::idsm::EventReporter::ReportEvent()`` → IPC → idsmd → filter/aggregate → Diag DTC/DID.

Level 3
-------

Complex or complicated building blocks from level 2 are further detailed in the following subsections.

Communication Stack
*******************

nsomeip (SOME/IP) operates on top of two parallel IPC paths: npc (zero-copy shared memory, direct on NAI) and ipc/ipcbase (general-purpose Unix Socket framework)

::

     +---------------------+
     | Communication Stack |
     +---------------------+
     |   nsomeip / npc /   |
     |    ipc / ipcbase    |
     +---------------------+

+-------------+--------------------------------------------------------------------------------------------------------------+--------------------------+
| Name        | Responsibility                                                                                               | Level                    |
+=============+==============================================================================================================+==========================+
| **ipcbase** | Unix Domain Socket lifecycle management. Creates ``SOCK_DGRAM`` type sockets, passes process credentials via |                          |
|             | ``SO_PASSCRED``. Core class ``IPCBase`` provides ``CreateSocket()``/``GetSocket()``/``ResolveName()``.       | L2: Transport            |
+-------------+--------------------------------------------------------------------------------------------------------------+--------------------------+
| **ipc**     | General IPC framework built on Unix Sockets, providing Client/Server pattern. Supports synchronous/          |                          |
|             | asynchronous calls, message fragmentation/reassembly. C interface (``ipc_client_t``/``ipc_server_t``)        |                          |
|             | + C++ wrapper (``IPCClient``/``IPCServer``).                                                                 | L3: Framework            |
+-------------+--------------------------------------------------------------------------------------------------------------+--------------------------+
| **npc**     | Zero-copy IPC library built directly on NAI (OS abstraction). Implements its own shared memory zone          |                          |
|             | management (``npc_zone``) and message port queue (``npc_port``), bypassing the general ipc/ipcbase framework.|                          |
|             | Supports Request/Response/Notification message types.                                                        | L3: Zero-copy            |
+-------------+--------------------------------------------------------------------------------------------------------------+--------------------------+
| **nsomeip** | Full AUTOSAR SOME/IP protocol stack implementation. Supports Service Discovery, Request/Response,            |                          |
|             | Fire&Forget, Event Subscription. Core API: ``nsi_app_init()``/``nsi_app_send()``/``nsi_app_offer_service()``.| L4: Application Protocol |
+-------------+--------------------------------------------------------------------------------------------------------------+--------------------------+

NPC and IPC/IPCBase are **two independent, parallel IPC pathways**, not a layered dependency chain.

- **NPC is self-contained**: `npc_depends = ${nai} ${sys}` — links only to NAI and system libs. Zero `#include` of IPC/IPCBase headers.
- **IPC/IPCBase is the general-purpose path**: IPCBase handles Unix Domain Socket lifecycle; IPC provides the Client/Server framework on top.

OS Abstraction
**************

Socket/File/Thread/MMap/Event Loop

::

    +-------------------------+ 
    |     OS Abstraction      | 
    +-------------------------+ 
    |        nai /            | 
    | naicpp                  | 
    +-------------------------+

+-----------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Name      | Responsibility                                                                                                                                                                                                                         |
+===========+========================================================================================================================================================================================================================================+
| **nai**   | Written in C. Cross-platform OS abstraction. Encapsulates Socket, File I/O, Threading, Memory Mapping, Atomic operations, Thread pools. Provides base types: ``nai_fd_t``, ``nai_pool_t``, ``nai_event_t``.                            |
+-----------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| **naicpp**| C++ event loop wrapper. ``isoft::naicpp::EvLoop`` class provides ``Open()/Run()/Stop()/Close()`` lifecycle management, ``Exec()`` to add execution nodes, ``MakeTimer()`` to create timers. Supports user-thread and own-thread modes. |
+-----------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Runtime View
============

Development View
================

Location of the building blocks and cross-cutting libraries:

::

    capi
    ├── cmake                          # Cross-cutting concepts / build system
    ├── com                            # ara::com — communication management
    ├── core                           # ara::core - runtime initialization
    ├── crypto                         # ara::crypto — cryptography module
    ├── diag                           # ara::diag — diagnostics management
    ├── exec                           # ara::exec — execution management
    ├── fw                             # ara::fw — firewall
    ├── iam                            # ara::iam — identity and access management
    ├── idsm                           # ara::idsm — intrusion detection
    ├── isoft
    │   ├── apcommon-cmake-modules     # Build system configuration files, modules and utilities
    │   ├── ara-fsh                    # Cross-cutting concepts / basic functions / isoft ara fsh
    │   ├── common                     # Cross-cutting concepts / basic functions / isoft common
    │   ├── crc                        # Cross-cutting concepts / basic functions / isoft crc
    │   ├── e2e                        # Cross-cutting concepts / basic functions / isoft e2e
    │   ├── ipc                        # Communication stack / ipc
    │   ├── ipcbase                    # Communication stack / ipcbase
    │   ├── manifestreader             # Cross-cutting concepts / build system
    │   ├── nai                        # OS abstraction / nai
    │   ├── naicpp                     # OS abstraction / naicpp 
    │   ├── npc                        # Communication stack / npc
    │   ├── nsomeip                    # Communication stack / nsomeip
    │   └── serialize                  # Cross-cutting concepts / basic functions / isoft serialize
    ├── log                            # ara::log — logging and tracing
    ├── nm                             # ara::nm — network management
    ├── per                            # ara::per — persistence
    ├── phm                            # ara::phm — platform health management
    ├── sm                             # ara::sm — state management
    ├── tsync                          # ara::tsync — time synchronization
    └── ucm                            # ara::ucm — update and configuration management

Deployment View
===============

Cross-cutting Concepts
======================

Basic Functions
---------------

The following libraries provide basic functionality, which is used by most other building blocks that are part of the Adaptive Platform stack.

Data processing libraries:

- isoft serialize: Template-based serialization/deserialization library. ``Serialize()``/``Deserialize()`` variadic template functions supporting primitive types, strings, containers, TLV format, big/little endian. Used as in-process library via local function calls.
- isoft e2e: End-to-end communication protection library. Implements AUTOSAR E2E Profiles P01-P44, providing CRC + Counter + DataID protection against message duplication/loss/tampering/delay.
- isoft crc: CRC checksum library. ``isoft::crc::CRC`` class with static methods supporting CRC8/CRC8H2F/CRC16/CRC32/CRC32P4/CRC64, with chunked computation.

Types and configuration:

- ara core-types (static library): Core data types (Result, Optional, Variant, Future ...) as defined in [AP_SWS_Core]_

  - ``ara::core::Result<T,E>`` — Result type with error code
  - ``ara::core::ErrorCode`` / ``ErrorDomain`` — Error handling framework
  - ``ara::core::Future<T>`` / ``Promise<T>`` — Async programming primitives
  - ``ara::core::Optional<T>`` / ``Variant<Ts...>`` — Value types
  - ``ara::core::String`` / ``StringView`` / ``Vector<T>`` / ``Map<K,V>`` / ``Span<T>`` — Containers
  - ``ara::core::InstanceSpecifier`` — Identifiers for modeled aspects 
  - ``ara::core::SteadyClock`` — Monotonic clock

- isoft common: Common types and macro definitions (``kSuccess``/``kFailure``/``MANIFEST_PATH``)
- isoft ara fsh: Defines AP runtime file system hierarchy. Directory constants: ``/ara/``, ``/ara/core/``, ``/ara/swcls/``, ``/tmp/``, ``/ipc/``. Core classes: ``isoft::ara_fsh::Platform`` and ``Process``.
- isoft manifestreader: JSON manifest parser, providing ``OpenManifest()``/``ParseManifest()``/``LoadManifestFromFile()``

Logging
-------

All building blocks of the Adaptive Platform Stack use ara::log for logging.

Inter-Module Communication Mechanisms
-------------------------------------

Multi-mode communication: IPC (Unix Socket) + ara::com (SOME/IP) + shared memory + local calls + network protocols

IPC (Unix Domain Socket) — Local Daemon Communication
*****************************************************

**Use case**: Local communication between all daemons and their client libraries, and between daemons.
**Protocol layer**: ipcbase (socket management) → ipc (Client/Server framework) → per-module custom protocols.

::

    AA App ──IPC──▶ logd     (Log reporting, DLT protocol)
    AA App ──IPC──▶ phmd     (Health checkpoint reporting, SV/HC channels)
    AA App ──IPC──▶ emd      (Execution state reporting, EMS)
    AA App ──IPC──▶ emd      (Function group state requests, SMS)
    AA App ──IPC──▶ fwd      (Firewall rule management)
    smd    ──IPC──▶ emd      (StateClient, state transitions)
    smd    ──IPC──▶ phmd     (RecoveryAction, recovery operations)
    smd    ──IPC──▶ diagd    (ECU reset requests)
    smd    ──IPC──▶ ucm      (Update control)

**IPC pattern**: Request-response (synchronous) or event reporting (asynchronous), based on Unix ``SOCK_DGRAM``, with process identity verification via ``SO_PASSCRED``.

ara::com (SOME/IP SOA) — Cross-Process / Network Communication
**************************************************************

**Use case**: Scenarios requiring service discovery and cross-ECU communication.

**Interfaces**: Proxy (Requester) ↔ Skeleton (Provider) ← Binding (nsompeip/npc/icc/fastdds) ← Routing Daemon.

::

    SM          ──ara::com──▶ NM           (FG state via network management coordination)
    Diag Agent  ──ara::com──▶ Diag Server  (Diagnostic services)
    UCM ECU     ──ara::com──▶ UCM Vehicle  (Vehicle-level OTA coordination)
    AA App      ──ara::com──▶ AA App       (Application SOA — method/event/field)


**Communication flow**:

1. Skeleton: ``OfferService()`` → nsompeipd → broadcast Service Discovery Offer
2. Proxy: ``StartFindService()`` → nsompeipd → listen for Service Discovery → ``FindServiceHandler`` callback
3. Proxy: ``Subscribe()`` → subscribe to events
4. Skeleton: ``Send()`` → send event/field notifications
5. Proxy: ``MethodCall()`` → Skeleton: ``ProcessNextMethodCall()`` → return result

Shared Memory — High-Performance Data Transfer
**********************************************

+-------------------------+------------------------------------------------------------+
| Scenario                | Implementation                                             |
+=========================+============================================================+
| NPC (zero-copy IPC)     | npc_zone shared memory region + npc_port message queue     |
+-------------------------+------------------------------------------------------------+
| tsync time distribution | Shared memory mapping, AA directly reads synchronized time |
+-------------------------+------------------------------------------------------------+
| per KV storage          | M/N redundant page management, file-map based              |
+-------------------------+------------------------------------------------------------+

Local Function Calls — In-Process Libraries
*******************************************

+------------------------+-------------------------------------------------------------------------------+
| Scenario               | Characteristics                                                               |
+========================+===============================================================================+
| ara::iam authorization | Compiled as static library, linked into each FC process, Manifest-driven      |
+------------------------+-------------------------------------------------------------------------------+
| ara::per persistence   | Compiled as client library, directly operates on filesystem within AA process |
+------------------------+-------------------------------------------------------------------------------+

Network Protocols — ECU-to-ECU / External Communication
*******************************************************

+-------------+--------------+-------------------------------------------------------+
| Protocol    | Using Module | Port/Method                                           |
+=============+==============+=======================================================+
| SOME/IP     | com          | UDP/TCP — Service Discovery + method calls            |
+-------------+--------------+-------------------------------------------------------+
| DoIP        | diag         | TCP 13400 + UDP 13400 — External tester connection    |
+-------------+--------------+-------------------------------------------------------+
| UDS on DoIP | diag         | DoIP transports UDS messages                          |
+-------------+--------------+-------------------------------------------------------+
| UDP NM      | nm           | UDP broadcast/multicast — Network management messages |
+-------------+--------------+-------------------------------------------------------+
| gPTP        | tsync        | IEEE 802.1AS — Time synchronization                   |
+-------------+--------------+-------------------------------------------------------+
| DLT         | log          | TCP — Remote log viewing (dlt-viewer)                 |
+-------------+--------------+-------------------------------------------------------+

Build System Flow
-----------------

Directory **isoft/apcommon-cmake-modules** contains common CMake build configs.
``apcommon_defaults()`` macro sets warning flags, C++14/17 standard, C99 standard, fPIC, etc. 

CMake modularity: Three-tier build configuration with Config.cmake (switches) + Modules.cmake (dependencies) + Utils.cmake (utilities)

::

    sdk-utils build → CMake (Config.cmake + Modules.cmake)
        ├── 1. Parse configuration (ARA_ENABLE_*)
        ├── 2. Add base libraries (isoft infrastructure)
        ├── 3. Build AP Libs in dependency order (tsync→log→core→exec→iam→crypto→per→com→diag→nm→sm-utils)
        ├── 4. Build AP Daemons (smd→emd→diagd→phmd→...)
        └── 5. Configure deployment machine (configMachine: ARXML model → runtime config)

    sdk-utils pack → Package as self-extracting .run file:
        ├── sysroot (Platform runtime: core/framework/swcls/var)
        ├── toolchain.cmake (Cross-compilation config)
        ├── build.sh (Application build script)
        ├── config.sh (Machine configuration script)
        └── run.sh (Start/stop script)

Technical Debt and Risks
========================

Glossary
========

This section lists abbreviations and terms that are specific to this document.
A list of general terms for AUTOSAR is provided in the AUTOSAR [FO_TR_Glossary]_.

Acronyms and Abbreviations
--------------------------

Definition of Terms
-------------------

References
==========

.. [ADR] CAPI Architecture Decision Records. *adr/*
.. [PDR] CAPI Process Decision Records. *pdr/*
.. [AP_EXP_SWArchitecture] Explanation of Adaptive Platform Software Architecture, R23-11. AUTOSAR, 2023
.. [AP_SWS_Core] Specification of Adaptive Platform Core, R23-11. AUTOSAR, 2023
.. [FO_TR_Glossary] Glossary, R25-11. AUTOSAR, 2025
.. [ISO42010:2022] ISO/IEC/IEEE 42010:2022 Software, systems and enterprise - Architecture description. ISO, 2022
