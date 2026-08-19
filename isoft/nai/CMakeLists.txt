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
# @file       CMakeLists.txt
# @brief
# @details
# @date       2020-11-27
# @author     xn
# @version    1.2.0
#
# ================================================================

cmake_minimum_required (VERSION 3.0)


include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckTypeSize)
include(CheckStructHasMember)
include(CheckCSourceCompiles)

macro(check_function_in_library func lib var)
    set(_ORIG_CMAKE_REQUIRED_LIBRARIES "${CMAKE_REQUIRED_LIBRARIES}")
    set(CMAKE_REQUIRED_LIBRARIES)
    check_function_exists("${func}" "_FUNC_IN_LIB_GLOBAL_${func}")
    set(CMAKE_REQUIRED_LIBRARIES "${_ORIG_CMAKE_REQUIRED_LIBRARIES}")

    if ("${_FUNC_IN_LIB_GLOBAL_${func}}")
        set(${var} FALSE)
    else()
        check_library_exists("${lib}" "${func}" "" ${var})
    endif()
endmacro()

macro(check_type_exists type var)
    set(_C_SOURCE "
        #include <stdio.h>
        #include <stdlib.h>
    ")
    foreach(_C_HEADER ${CMAKE_EXTRA_INCLUDE_FILES})
        set(_C_SOURCE "${_C_SOURCE}
        #include <${_C_HEADER}>")
    endforeach(_C_HEADER)

    set(_C_SOURCE "${_C_SOURCE}
        int main() {
            ${type} var_exists;
            (void)var_exists;
            return 0;
        }
    ")
    check_c_source_compiles("${_C_SOURCE}" ${var})
endmacro()



if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif()

if (POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif()
if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW) # for CMAKE_C_VISIBILITY_PRESET
endif()
if (POLICY CMP0067)
    cmake_policy(SET CMP0067 NEW) # for try_compile
endif()
if (POLICY CMP0075)
    cmake_policy(SET CMP0075 OLD) # for CMAKE_REQUIRED_LIBRARIES warning
endif()
if (POLICY CMP0135)
    cmake_policy(SET CMP0135 OLD) # for download without timestamp
endif()


project(nai LANGUAGES C VERSION 1.1.0) # project version


option(CMAKE_POSITION_INDEPENDENT_CODE "Build position independent libraries" ON)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(BUILD_TESTING "" OFF)
option(BUILD_WITH_DBGINFO "Build with debug information" ON)
option(BUILD_WITH_REGEX "To enable the regular expression" OFF)
option(BUILD_WITH_SSL "To enable the ssl" OFF)
option(BUILD_WITH_HTTP "To enable the http" OFF)
option(BUILD_WITH_AIO_LINUX "To enable the aio of linux" ON)
option(COMPAT_POSIX_MPROTECT "Make mprotect behavior compatible with posix on win32" OFF)


# os 
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(__darwin__ 1)
    if (NOT _DARWIN_C_SOURCE)
        set(_DARWIN_C_SOURCE 1)
    endif()
endif()
if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    set(__freebsd__ 1)
    set(__BSD_VISIBLE 1)
    if (NOT _XOPEN_SOURCE)
        set(_XOPEN_SOURCE 700)
    endif()
endif()
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR 
    ${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(__linux__ 1)
    if (NOT _XOPEN_SOURCE)
        set(_XOPEN_SOURCE 700)
    endif()
    if (${CMAKE_SYSTEM_NAME} MATCHES "Android")
        set(__android__ 1)
    endif()
endif()
if (${CMAKE_SYSTEM_NAME} MATCHES "Solaris")
    set(__solaris__ 1)
endif()
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(__windows__ 1)
endif()
if (${CMAKE_SYSTEM_NAME} MATCHES "QNX")
    set(__qnx__ 1)
    set(QNX 1)
    set(_QNX_SOURCE 1)
    if (NOT _XOPEN_SOURCE)
        set(_XOPEN_SOURCE 700)
    endif()
endif()
if (${CMAKE_SYSTEM_NAME} MATCHES "SylixOS")
    set(__sylixos__ 1)
endif()



# check if _GNU_SOURCE is available.
if (NOT _GNU_SOURCE)
    check_symbol_exists(_GNU_SOURCE "features.h" _GNU_SOURCE_1)
    check_symbol_exists(__GNU_LIBRARY__ "features.h" _GNU_SOURCE_2)
    if (_GNU_SOURCE_1 OR _GNU_SOURCE_2)
        set(_GNU_SOURCE 1)
    endif()
endif()


set(sys_defines)
if (_GNU_SOURCE)
    list(APPEND sys_defines -D_GNU_SOURCE=${_GNU_SOURCE})
endif()
if (_BSD_SOURCE)
    list(APPEND sys_defines -D_BSD_SOURCE=${_BSD_SOURCE})
endif()
if (_QNX_SOURCE)
    list(APPEND sys_defines -D_QNX_SOURCE=${_QNX_SOURCE})
endif()
if (_POSIX_SOURCE)
    list(APPEND sys_defines -D_POSIX_SOURCE=${_POSIX_SOURCE})
endif()
if (_XOPEN_SOURCE)
    list(APPEND sys_defines -D_XOPEN_SOURCE=${_XOPEN_SOURCE})
endif()
if (_POSIX_C_SOURCE)
    list(APPEND sys_defines -D_POSIX_C_SOURCE=${_POSIX_C_SOURCE})
endif()
if (_DARWIN_C_SOURCE)
    list(APPEND sys_defines -D_DARWIN_C_SOURCE=${_DARWIN_C_SOURCE})
endif()
if (_DEFAULT_SOURCE)
    list(APPEND sys_defines -D_DEFAULT_SOURCE=${_DEFAULT_SOURCE})
endif()
if (_LARGEFILE64_SOURCE)
    list(APPEND sys_defines -D_LARGEFILE64_SOURCE=${_LARGEFILE64_SOURCE})
endif()
if (__BSD_VISIBLE)
    list(APPEND sys_defines -D__BSD_VISIBLE=${__BSD_VISIBLE})
endif()


# threads library
find_package(Threads REQUIRED)
set(NAI_HAVE_THREADS ON)
set(thread ${CMAKE_THREAD_LIBS_INIT})


# check library
if (NOT WIN32)
check_function_in_library(dlopen dl             NAI_HAVE_LIBDL)
check_function_in_library(clock_gettime rt      NAI_HAVE_LIBRT_1)
check_function_in_library(aio_suspend rt        NAI_HAVE_LIBRT_2)
check_function_in_library(socket socket         NAI_HAVE_LIBSOCKET)
check_function_in_library(res_init resolv       NAI_HAVE_LIBRESOLV_1)
check_function_in_library(res_ninit resolv      NAI_HAVE_LIBRESOLV_2)
check_function_in_library(io_uring_queue_init uring 
                                                NAI_HAVE_LIBURING)

if (NAI_HAVE_LIBRT_1 OR NAI_HAVE_LIBRT_2)
    set(NAI_HAVE_LIBRT ON)
endif()
if (NAI_HAVE_LIBRESOLV_1 OR NAI_HAVE_LIBRESOLV_2)
    set(NAI_HAVE_LIBRESOLV ON)
endif()
endif()


set(sys_libraries)
if (NAI_HAVE_THREADS)
    list(APPEND sys_libraries ${thread})
endif()
if (NAI_HAVE_LIBDL)
    list(APPEND sys_libraries "dl")
endif()
if (NAI_HAVE_LIBRT)
    list(APPEND sys_libraries "rt")
endif()
if (NAI_HAVE_LIBSOCKET)
    list(APPEND sys_libraries "socket")
endif()
if (NAI_HAVE_LIBRESOLV)
    list(APPEND sys_libraries "resolv")
endif()
if (NAI_HAVE_LIBURING)
    list(APPEND sys_libraries "uring")
endif()


if (NOT CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 99)
endif()
if (NOT MSVC)
    set(CMAKE_REQUIRED_FLAGS -std=gnu${CMAKE_C_STANDARD})
endif()


add_definitions(${sys_defines})
set(CMAKE_REQUIRED_DEFINITIONS ${sys_defines})
set(CMAKE_REQUIRED_LIBRARIES ${sys_libraries})
message(STATUS "required definitions:  ${sys_defines}")
message(STATUS "required libraries: ${sys_libraries}")
message(STATUS "c standard: ${CMAKE_C_STANDARD}")



# check type size
check_type_size("float"                         NAI_SIZEOF_FLOAT)
check_type_size("double"                        NAI_SIZEOF_DOUBLE)
check_type_size("int"                           NAI_SIZEOF_INT)
check_type_size("long"                          NAI_SIZEOF_LONG)
check_type_size("long long"                     NAI_SIZEOF_LONG_LONG)
check_type_size("unsigned int"                  NAI_SIZEOF_UNSIGNED_INT)
check_type_size("unsigned long"                 NAI_SIZEOF_UNSIGNED_LONG)
check_type_size("unsigned long long"            NAI_SIZEOF_UNSIGNED_LONG_LONG)
check_type_size("size_t"                        NAI_SIZEOF_SIZE_T)
check_type_size("ptrdiff_t"                     NAI_SIZEOF_PTRDIFF_T)


# check types
check_type_exists("off64_t"                     NAI_HAVE_OFF64_T)



# check headers
check_include_file(alloca.h                     NAI_HAVE_ALLOCA_H)
check_include_file(fcntl.h                      NAI_HAVE_FCNTL_H)
check_include_file(limits.h                     NAI_HAVE_LIMITS_H)
check_include_file(malloc.h                     NAI_HAVE_MALLOC_H)
check_include_file(ifaddrs.h                    NAI_HAVE_IFADDRS_H)
check_include_file(netdb.h                      NAI_HAVE_NETDB_H)
check_include_file(net/if.h                     NAI_HAVE_NET_IF_H)
check_include_file(net/if_dl.h                  NAI_HAVE_NET_IF_DL_H)
check_include_file(netinet/udp.h                NAI_HAVE_NETINET_UDP_H)
check_include_file(dlfcn.h                      NAI_HAVE_DLFCN_H)
check_include_file(pthread.h                    NAI_HAVE_PTHREAD_H)
check_include_file(sched.h                      NAI_HAVE_SCHED_H)
check_include_file(semaphore.h                  NAI_HAVE_SEMAPHORE_H)
check_include_file(signal.h                     NAI_HAVE_SIGNAL_H)
check_include_file(stddef.h                     NAI_HAVE_STDDEF_H)
check_include_file(stdint.h                     NAI_HAVE_STDINT_H)
check_include_file(stdlib.h                     NAI_HAVE_STDLIB_H)
check_include_file(string.h                     NAI_HAVE_STRING_H)
check_include_file(sys/file.h                   NAI_HAVE_SYS_FILE_H)
check_include_file(sys/mman.h                   NAI_HAVE_SYS_MMAN_H)
check_include_file(sys/inotify.h                NAI_HAVE_SYS_INOTIFY_H)
check_include_file(sys/ioctl.h                  NAI_HAVE_SYS_IOCTL_H)
check_include_file(sys/resource.h               NAI_HAVE_SYS_RESOURCE_H)
check_include_file(sys/socket.h                 NAI_HAVE_SYS_SOCKET_H)
check_include_file(sys/select.h                 NAI_HAVE_SYS_SELECT_H)
check_include_file(sys/sendfile.h               NAI_HAVE_SYS_SENDFILE_H)
check_include_file(sys/sendfilev.h              NAI_HAVE_SYS_SENDFILEV_H)
check_include_file(sys/stat.h                   NAI_HAVE_SYS_STAT_H)
check_include_file(sys/sysinfo.h                NAI_HAVE_SYS_SYSINFO_H)
check_include_file(sys/time.h                   NAI_HAVE_SYS_TIME_H)
check_include_file(sys/types.h                  NAI_HAVE_SYS_TYPES_H)
check_include_file(sys/uio.h                    NAI_HAVE_SYS_UIO_H)
check_include_file(sys/un.h                     NAI_HAVE_SYS_UN_H)
check_include_file(sys/wait.h                   NAI_HAVE_SYS_WAIT_H)
check_include_file(time.h                       NAI_HAVE_TIME_H)
check_include_file(uintptr.h                    NAI_HAVE_UINTPTR_H)
check_include_file(unistd.h                     NAI_HAVE_UNISTD_H)
check_include_file(aio.h                        NAI_HAVE_AIO_H)
check_include_file(liburing.h                   NAI_HAVE_LIBURING_H)

if (BUILD_WITH_AIO_LINUX)
    check_include_file(linux/aio_abi.h          NAI_HAVE_AIO_LINUX_H)
else()
    set(NAI_HAVE_AIO_LINUX_H OFF)
endif()


# check functions
check_symbol_exists(static_assert assert.h      NAI_HAVE_STATIC_ASSERT)
check_symbol_exists(alloca alloca.h             NAI_HAVE_ALLOCA)
check_symbol_exists(memalign malloc.h           NAI_HAVE_MEMALIGN)

check_symbol_exists(clock_gettime time.h        NAI_HAVE_CLOCK_GETTIME)
check_symbol_exists(gettimeofday sys/time.h     NAI_HAVE_GETTIMEOFDAY)
check_symbol_exists(nanosleep time.h            NAI_HAVE_NANOSLEEP)
check_symbol_exists(utime utime.h               NAI_HAVE_UTIME)
check_symbol_exists(utimes sys/time.h           NAI_HAVE_UTIMES)
check_symbol_exists(gmtime_r time.h             NAI_HAVE_GMTIME_R)
check_symbol_exists(localtime_r time.h          NAI_HAVE_LOCALTIME_R)
check_symbol_exists(timer_create time.h         NAI_HAVE_TIMER_CREATE)
check_symbol_exists(timerfd_create sys/timerfd.h NAI_HAVE_TIMERFD_CREATE)

check_symbol_exists(fork unistd.h               NAI_HAVE_FORK)
check_symbol_exists(execv unistd.h              NAI_HAVE_EXECV)

check_symbol_exists(pthread_yield pthread.h     NAI_HAVE_PTHREAD_YIELD)
check_symbol_exists(pthread_spin_init pthread.h NAI_HAVE_PTHREAD_SPINLOCK)
check_symbol_exists(pthread_getaffinity_np pthread.h NAI_HAVE_PTHREAD_GETAFFINITY_NP)
check_symbol_exists(pthread_setaffinity_np pthread.h NAI_HAVE_PTHREAD_SETAFFINITY_NP)
check_symbol_exists(pthread_attr_getaffinity_np pthread.h NAI_HAVE_PTHREAD_ATTR_GETAFFINITY_NP)
check_symbol_exists(pthread_attr_setaffinity_np pthread.h NAI_HAVE_PTHREAD_ATTR_SETAFFINITY_NP)
check_symbol_exists(pthread_condattr_setclock pthread.h NAI_HAVE_PTHREAD_CONDATTR_SETCLOCK)
check_symbol_exists(posix_memalign stdlib.h     NAI_HAVE_POSIX_MEMALIGN)
check_symbol_exists(sched_yield sched.h         NAI_HAVE_SCHED_YIELD)
check_symbol_exists(sched_setscheduler sched.h  NAI_HAVE_SCHED_SETSCHEDULER)
check_symbol_exists(sched_getaffinity sched.h   NAI_HAVE_SCHED_GETAFFINITY)
check_symbol_exists(sched_setaffinity sched.h   NAI_HAVE_SCHED_SETAFFINITY)
check_symbol_exists(sched_get_priority_min sched.h  NAI_HAVE_SCHED_CLASS_PRIORITY)

check_symbol_exists(sem_timedwait semaphore.h   NAI_HAVE_SEM_TIMEDWAIT)
check_symbol_exists(sigaction signal.h          NAI_HAVE_SIGACTION)
check_symbol_exists(setsid unistd.h             NAI_HAVE_SETSID)
check_symbol_exists(getpwnam pwd.h              NAI_HAVE_GETPWNAM)
check_symbol_exists(getpwnam_r pwd.h            NAI_HAVE_GETPWNAM_R)
check_symbol_exists(getgrnam grp.h              NAI_HAVE_GETGRNAM)
check_symbol_exists(getgrnam_r grp.h            NAI_HAVE_GETGRNAM_R)
check_symbol_exists(getrlimit sys/resource.h    NAI_HAVE_GETRLIMIT)
check_symbol_exists(setrlimit sys/resource.h    NAI_HAVE_SETRLIMIT)
check_symbol_exists(get_nprocs sys/sysinfo.h    NAI_HAVE_GET_NPROCS)
check_symbol_exists(getpagesize unistd.h        NAI_HAVE_GETPAGESIZE)

check_symbol_exists(getaddrinfo netdb.h         NAI_HAVE_GETADDRINFO)
check_symbol_exists(gethostbyname_r netdb.h     NAI_HAVE_GETHOSTBYNAME_R)

check_symbol_exists(aio_readv aio.h             NAI_HAVE_AIO_READV)
check_symbol_exists(aio_writev aio.h            NAI_HAVE_AIO_WRITEV)

check_symbol_exists(openat fcntl.h              NAI_HAVE_OPENAT)
check_symbol_exists(stat64 sys/stat.h           NAI_HAVE_STAT64)
check_symbol_exists(fstatat sys/stat.h          NAI_HAVE_FSTATAT)
check_symbol_exists(fstat64 sys/stat.h          NAI_HAVE_FSTAT64)
check_symbol_exists(ftruncate64 unistd.h        NAI_HAVE_FTRUNCATE64)
check_symbol_exists(lseek64 unistd.h            NAI_HAVE_LSEEK64)
check_symbol_exists(pread unistd.h              NAI_HAVE_PREAD)
check_symbol_exists(preadv sys/uio.h            NAI_HAVE_PREADV)
check_symbol_exists(pwrite unistd.h             NAI_HAVE_PWRITE)
check_symbol_exists(pwritev sys/uio.h           NAI_HAVE_PWRITEV)
check_symbol_exists(readv sys/uio.h             NAI_HAVE_READV)
check_symbol_exists(writev sys/uio.h            NAI_HAVE_WRITEV)
check_symbol_exists(dup2 unistd.h               NAI_HAVE_DUP2)
check_symbol_exists(dup3 unistd.h               NAI_HAVE_DUP3)
check_symbol_exists(pipe2 unistd.h              NAI_HAVE_PIPE2)
check_symbol_exists(accept4 sys/socket.h        NAI_HAVE_ACCEPT4)
check_symbol_exists(recvmsg sys/socket.h        NAI_HAVE_RECVMSG)
check_symbol_exists(recvmmsg sys/socket.h       NAI_HAVE_RECVMMSG)
check_symbol_exists(sendmsg sys/socket.h        NAI_HAVE_SENDMSG)
check_symbol_exists(sendmmsg sys/socket.h       NAI_HAVE_SENDMMSG)
check_symbol_exists(sendfile sys/socket.h       NAI_HAVE_SENDFILE_FREEBSD)
check_symbol_exists(sendfile sys/sendfile.h     NAI_HAVE_SENDFILE_LINUX)
check_symbol_exists(sendfilev sys/sendfilev.h   NAI_HAVE_SENDFILEV)
check_symbol_exists(if_nametoindex net/if.h     NAI_HAVE_IF_NAMETOINDEX)
check_symbol_exists(if_indextoname net/if.h     NAI_HAVE_IF_INDEXTONAME)

check_symbol_exists(inotify_init1 sys/inotify.h NAI_HAVE_INOTIFY_INIT1)

check_symbol_exists(kqueue sys/event.h          NAI_HAVE_KQUEUE)
check_symbol_exists(epoll_create sys/epoll.h    NAI_HAVE_EPOLL)
check_symbol_exists(eventfd sys/eventfd.h       NAI_HAVE_EVENTFD)
check_symbol_exists(port_create port.h          NAI_HAVE_PORT)
check_symbol_exists(poll poll.h                 NAI_HAVE_POLL)
check_symbol_exists(WSAPoll winsock2.h          NAI_HAVE_POLL)
check_symbol_exists(select sys/select.h         NAI_HAVE_SELECT)
# check_symbol_exists(sys/devpoll.h             NAI_HAVE_DEVPOLL)


check_symbol_exists(UDP_SEGMENT netinet/udp.h   NAI_HAVE_UDP_SEGMENT)


check_c_source_compiles("
    #include <time.h>
    #include <semaphore.h>
    int main()
    {
      sem_timedwait(0, 0);
      return 0;
    }"
    NAI_HAVE_SEM_TIMEDWAIT)


check_include_files("sys/param.h;sys/cpuset.h"  NAI_HAVE_SYS_CPUSET_H)
check_symbol_exists(cpuset_getaffinity 
    "sys/param.h;sys/cpuset.h"                  NAI_HAVE_CPUSET_GETAFFINITY)
check_symbol_exists(cpuset_setaffinity 
    "sys/param.h;sys/cpuset.h"                  NAI_HAVE_CPUSET_SETAFFINITY)


if ((NOT __darwin__) OR NAI_HAVE_LIBRESOLV)
check_include_files("netinet/in.h;resolv.h"     NAI_HAVE_RESOLV_H)
check_symbol_exists(res_init 
    "netinet/in.h;resolv.h"                     NAI_HAVE_RES_INIT)
check_symbol_exists(_res 
    "netinet/in.h;resolv.h"                     NAI_HAVE_RES_STATE)

if (NOT __qnx__) # res_getservers is found, but can't be linked on qnx
check_symbol_exists(res_getservers 
    "netinet/in.h;resolv.h"                     NAI_HAVE_RES_GETSERVERS)
endif()
endif()


if (WIN32)
set(NAI_HAVE_SOCKADDR_IN6 1)
else()
check_struct_has_member("struct sockaddr_un" 
    sun_path "sys/un.h"                         NAI_HAVE_SOCKADDR_UN)
check_struct_has_member("struct sockaddr_in6" 
    sin6_addr "netinet/in.h"                    NAI_HAVE_SOCKADDR_IN6)
check_struct_has_member("struct sockaddr_in" 
    sin_len "netinet/in.h"                      NAI_HAVE_SOCKADDR_LEN)
endif()

check_struct_has_member("struct tm" 
    tm_gmtoff "time.h"                          NAI_HAVE_TM_GMTOFF)



# os bits
math(EXPR osbits "8*${CMAKE_SIZEOF_VOID_P}")


# configure output dir
if (NOT MSVC)
    set(config_dir "${CMAKE_BUILD_TYPE}")
    set(config_empty "")
    if (config_dir STREQUAL config_empty)
        set(config_dir "Release")
    endif()
else()
    #set(config_dir $<$<CONFIG:Debug>:Debug>)
    #set(config_dir ${config_dir}$<$<CONFIG:Release>:Release>)
    #set(config_dir ${config_dir}$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>)
    #set(config_dir ${config_dir}$<$<CONFIG:MinSizeRel>:MinSizeRel>)
endif()
set(output_dir /x${osbits})
set(output_subdir ${output_dir}/${config_dir})


if (UNIX)
  set(NAI_HAVE_LIBM TRUE)
endif()


# force use /MT and disable warnings 
if (MSVC)
    foreach(flags
        CMAKE_C_FLAGS 
        CMAKE_C_FLAGS_DEBUG 
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL 
        CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS 
        CMAKE_CXX_FLAGS_DEBUG 
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL 
        CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${flags} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flags} "${${flags}}")
        endif(${flags} MATCHES "/MD")
        set(${flags} "${${flags}} /wd4819")
    endforeach(flags)
endif(MSVC)

# set complie options
if (BUILD_WITH_DBGINFO)
    set(CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELWITHDEBINFO})
    set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO})
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE ${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO})
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE ${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO})
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin${output_subdir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin${output_subdir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib${output_subdir})
set(CMAKE_PDB_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/pdb${output_subdir})


execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_PDB_OUTPUT_DIRECTORY})


# exports compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


##############################################################################
# depends


include(ExternalProject)


# thridparty path
set(thirdparty_dir ${PROJECT_SOURCE_DIR}/thirdparty)
set(thirdparty_install ${thirdparty_dir}/install)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${thirdparty_install})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${thirdparty_install}/include)


# compile args
list(APPEND EXTERNAL_MAKE_ARGS
    CC=${CMAKE_C_COMPILER}
    CFLAGS=${CMAKE_C_FLAGS}
    # CXX=${CMAKE_CXX_COMPILER}
    # CXXFLAGS=${CMAKE_CXX_FLAGS}
)

list(APPEND EXTERNAL_CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_C_FLAGS_DEBUG=${CMAKE_C_FLAGS_DEBUG}
    -DCMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}
    -DCMAKE_C_FLAGS_MINSIZEREL=${CMAKE_C_FLAGS_MINSIZEREL}
    -DCMAKE_C_FLAGS_RELWITHDEBINFO=${CMAKE_C_FLAGS_RELWITHDEBINFO}
    # -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    # -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
    # -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
    # -DCMAKE_CXX_FLAGS_MINSIZEREL=${CMAKE_CXX_FLAGS_MINSIZEREL}
    # -DCMAKE_CXX_FLAGS_RELWITHDEBINFO=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
    -DCMAKE_SYSROOT=${CMAKE_SYSROOT}
    -DCMAKE_FIND_ROOT_PATH=${CMAKE_FIND_ROOT_PATH}
    -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
    -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE}
    -DCMAKE_C_VISIBILITY_PRESET=hidden
    -DCMAKE_VISIBILITY_INLINES_HIDDEN=ON
    -DCMAKE_POLICY_DEFAULT_CMP0063=NEW
)

if (CMAKE_CROSSCOMPILING)
    list(APPEND EXTERNAL_CMAKE_ARGS 
        -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME})
    list(APPEND EXTERNAL_MAKE_ARGS 
        --host=${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_FULL_NAME})
endif()

# message(STATUS "external cmake args: " ${EXTERNAL_CMAKE_ARGS})
# message(STATUS "external make args: " ${EXTERNAL_MAKE_ARGS})

# add cmake scripts
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/misc/cmake ${CMAKE_MODULE_PATH})


# shell suffix
set(exe_suffix "")
set(cmd_suffix "")
if (WIN32)
    set(exe_suffix ".exe")
    set(cmd_suffix ".cmd")
endif(WIN32)


set(sys ${sys_libraries})


# home
set(HOME $ENV{HOME})


# set options
set(NAI_WITH_HTTP ${BUILD_WITH_HTTP})


# openssl library
if (BUILD_WITH_SSL)

    if (NOT SSL_FOUND AND FALSE)
        find_package(WolfSSL QUIET)
        if (WolfSSL_FOUND)
            unset(WolfSSL_VERSION_OK CACHE)
            check_c_source_compiles("
                #include <wolfssl/version.h>
                #if (LIBWOLFSSL_VERSION_HEX < 0x5002000)
                #error fail
                #endif
                int main()
                {
                  return 0;
                }"
                WolfSSL_VERSION_OK)
        endif()
        if (WolfSSL_VERSION_OK)
            set(SSL_FOUND ${WolfSSL_FOUND})
            set(SSL_INCLUDE_DIR ${WolfSSL_INCLUDE_DIR})
            set(SSL_LIBRARIES ${WolfSSL_LIBRARIES})
            set(SSL_LIBRARY ${WolfSSL_LIBRARY})
            set(CRYPTO_LIBRARY ${WolfSSL_LIBRARY})
            set(NAI_HAVE_WOLFSSL ON)
            message(WARNING "the license of wolfssl is GPL")
        endif()
    endif()

    # TODO: LibreSSL does not supported DTLS_get_data_mtu, should be 
    # implemented by function nai_dtls_get_mtu in nai_ssl.c, so we disable LibreSSL for now.
    if (NOT SSL_FOUND AND FALSE)
        find_package(LibreSSL QUIET)
        if (LIBRESSL_FOUND)
            set(SSL_FOUND ${LIBRESSL_FOUND})
            set(SSL_INCLUDE_DIR ${LIBRESSL_INCLUDE_DIR})
            set(SSL_LIBRARIES ${LIBRESSL_LIBRARIES})
            set(SSL_LIBRARY ${LIBRESSL_SSL_LIBRARY})
            set(CRYPTO_LIBRARY ${LIBRESSL_CRYPTO_LIBRARY})
            set(NAI_HAVE_LIBRESSL ON)
        endif()
    endif()
    # if (NOT SSL_FOUND)
    #     set(SSL_FOUND ON)
    #     set(SSL_INCLUDE_DIR "${HOME}/.local/include")
    #     set(SSL_LIBRARIES "${HOME}/.local/lib/libssl.so")
    #     set(SSL_LIBRARY "${HOME}/.local/lib/libssl.so")
    #     set(CRYPTO_LIBRARY "${HOME}/.local/lib/libcrypto.so")
    #     set(NAI_HAVE_LIBRESSL ON)
    # endif()

    if (NOT SSL_FOUND)
        find_package(OpenSSL QUIET)
        if (OPENSSL_FOUND)
            set(SSL_FOUND ${OPENSSL_FOUND})
            set(SSL_INCLUDE_DIR ${OPENSSL_INCLUDE_DIR})
            set(SSL_LIBRARIES ${OPENSSL_LIBRARIES})
            set(SSL_LIBRARY ${OPENSSL_SSL_LIBRARY})
            foreach(lib ${SSL_LIBRARIES})
                set(CRYPTO_LIBRARY ${lib})
            endforeach()
            set(NAI_HAVE_OPENSSL ON)
        endif()
    endif()

    message(STATUS "ssl libs: include=${SSL_INCLUDE_DIR},lib=${SSL_LIBRARIES}")
endif()
if (NOT BUILD_WITH_SSL)
    set(SSL_FOUND FALSE)
elseif (SSL_FOUND)
    set(ssl "ssl")
    add_library(${ssl} UNKNOWN IMPORTED)
    set_target_properties(${ssl} PROPERTIES 
        INTERFACE_INCLUDE_DIRECTORIES ${SSL_INCLUDE_DIR})
    set_target_properties(${ssl} PROPERTIES 
        IMPORTED_LOCATION ${SSL_LIBRARY})

    set(crypto "crypto")
    add_library(${crypto} UNKNOWN IMPORTED)
    set_target_properties(${crypto} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${SSL_INCLUDE_DIR})
    set_target_properties(${crypto} PROPERTIES
        IMPORTED_LOCATION ${CRYPTO_LIBRARY})
endif()


# pcre library
if (BUILD_WITH_REGEX)
    find_package(PCRE QUIET)
    message(STATUS "pcre libs: ${PCRE_LIBRARIES}")
    message(STATUS "pcre libs: ${PCRE_STATIC_LIBRARIES}")
endif()
if (NOT BUILD_WITH_REGEX)
    set(PCRE_FOUND FALSE)
elseif (PCRE_FOUND)
    set(NAI_HAVE_PCRE ON)
    set(pcre "pcre")
    add_library(${pcre} UNKNOWN IMPORTED)
    set_target_properties(${pcre} PROPERTIES 
        INTERFACE_INCLUDE_DIRECTORIES ${PCRE_INCLUDE_DIRS})
    if (PCRE_LIBRARIES)
        set_target_properties(${pcre} PROPERTIES
            IMPORTED_LOCATION ${PCRE_LIBRARIES})
    else()
        set_target_properties(${pcre} PROPERTIES
            IMPORTED_LOCATION ${PCRE_STATIC_LIBRARIES})
    endif()
else()
    set(pcre_ep "libpcre")
    #set(pcre_url "https://ftp.pcre.org/pub/pcre/pcre-8.44.zip")
    set(pcre_url "https://ftp.halifax.rwth-aachen.de/osdn/sfnet/p/pc/pcre/pcre/8.44/pcre-8.44.zip")
    set(pcre_md5 "49b898e80e195921c31f4001ba8e1248")
    set(pcre_libdir ${thirdparty_install}/lib/${output_dir})
    set(pcre_install ${CMAKE_INSTALL_PREFIX})
    if (BUILD_SHARED_LIBS)
        set(pcre_install ${thirdparty_install})
    endif()

    externalproject_add(${pcre_ep}
        URL ${pcre_url}
        URL_MD5 ${pcre_md5}
        PREFIX "pcre"
        SOURCE_DIR ${thirdparty_dir}/pcre
        INSTALL_DIR ${pcre_install}
        CMAKE_ARGS ${pcre_args} 
            -DBUILD_TESTS=OFF -DBUILD_SHARED_LIBS=OFF 
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${pcre_libdir}/${config_dir}
            -DPCRE_BUILD_TESTS=OFF
            -DPCRE_BUILD_PCRECPP=OFF
            -DPCRE_SUPPORT_JIT=ON
            -DPCRE_SUPPORT_UTF=ON
            ${EXTERNAL_CMAKE_ARGS}
    )

    set(pcre_name pcre)
    set(pcred_name pcre)
    if (MSVC)
        set(pcred_name pcred)
    endif()

    set(NAI_HAVE_PCRE ON)
    set(pcre "pcre")
    set(pcre_libname ${CMAKE_STATIC_LIBRARY_PREFIX}${pcre_name}${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(pcred_libname ${CMAKE_STATIC_LIBRARY_PREFIX}${pcred_name}${CMAKE_STATIC_LIBRARY_SUFFIX})
    add_library(${pcre} STATIC IMPORTED)
    add_dependencies(${pcre} ${pcre_ep})
    set_target_properties(${pcre} PROPERTIES 
        INTERFACE_INCLUDE_DIRECTORIES ${thirdparty_install}/include)
    set_target_properties(${pcre} PROPERTIES 
        IMPORTED_LOCATION_DEBUG ${pcre_libdir}/Debug/${pcred_libname})
    set_target_properties(${pcre} PROPERTIES 
        IMPORTED_LOCATION_RELEASE ${pcre_libdir}/Release/${pcre_libname})
    set_target_properties(${pcre} PROPERTIES 
        IMPORTED_LOCATION_RELWITHDEBINFO ${pcre_libdir}/RelWithDebInfo/${pcre_libname})
    set_target_properties(${pcre} PROPERTIES 
        IMPORTED_LOCATION_MINSIZEREL ${pcre_libdir}/MinSizeRel/${pcre_libname})
endif()


# check gcc atomic
if (NOT WIN32)
    if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU" AND 
        NOT CMAKE_C_COMPILER_VERSION VERSION_LESS 4.1)
        set(NAI_HAVE_GCC_ATOMIC TRUE)
    elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
        set(NAI_HAVE_GCC_ATOMIC TRUE)
    elseif (${CMAKE_C_COMPILER_ID} STREQUAL "QCC")
        set(NAI_HAVE_GCC_ATOMIC TRUE)
    else()
        check_c_source_compiles("
            int main()
            {
              int a;
              __sync_lock_test_and_set(&a, 1);
              __sync_fetch_and_add(&a, 1);
              if(!__sync_bool_compare_and_swap(&a, 2, 3))
                return -1;
              return 0;
            }"
            NAI_HAVE_GCC_ATOMIC)
    endif()
endif()

# use atomic_ops library
if (NOT WIN32 AND NOT NAI_HAVE_GCC_ATOMIC)


find_package(ATOMIC_OPS QUIET)
message(STATUS "atomic_ops libs: ${ATOMIC_OPS_LIBRARIES}")
message(STATUS "atomic_ops libs: ${ATOMIC_OPS_STATIC_LIBRARIES}")
if (ATOMIC_OPS_FOUND)
    set(NAI_HAVE_ATOMIC_OPS ON)
    set(atomic "atomic")
    add_library(${atomic} UNKNOWN IMPORTED)
    set_target_properties(${atomic} PROPERTIES 
        INTERFACE_INCLUDE_DIRECTORIES ${ATOMIC_OPS_INCLUDE_DIRS})
    if (ATOMIC_OPS_LIBRARIES)
        set_target_properties(${atomic} PROPERTIES 
            IMPORTED_LOCATION ${ATOMIC_OPS_LIBRARIES})
    else()
        set_target_properties(${atomic} PROPERTIES 
            IMPORTED_LOCATION ${ATOMIC_OPS_STATIC_LIBRARIES})
    endif()
else()
    # install atomic_ops
    set(atomic_ep "libatomic")
    set(atomic_url "https://github.com/bdwgc/libatomic_ops/releases/download/v7.10.0/libatomic_ops-7.10.0.tar.gz")
    set(atomic_md5 "1de9631daa0781a8c5a8457053d57cf0")
    set(atomic_libdir ${thirdparty_install}/lib/${output_dir})
    set(atomic_install ${CMAKE_INSTALL_PREFIX})
    if (BUILD_SHARED_LIBS)
        set(atomic_install ${thirdparty_install})
    endif()

    externalproject_add(${atomic_ep}
        URL ${atomic_url}
        URL_MD5 ${atomic_md5}
        PREFIX "atomic_ops"
        SOURCE_DIR ${thirdparty_dir}/atomic_ops
        INSTALL_DIR ${atomic_install}
        BUILD_COMMAND make
        INSTALL_COMMAND make install
        CONFIGURE_COMMAND <SOURCE_DIR>/configure
            --prefix=<INSTALL_DIR>
            --libdir=${atomic_libdir}/${config_dir}
            --disable-gpl
            ${EXTERNAL_MAKE_ARGS}
    )

    set(NAI_HAVE_ATOMIC_OPS ON)
    set(atomic "atomic_ops")
    set(atomic_libname ${CMAKE_STATIC_LIBRARY_PREFIX}atomic_ops${CMAKE_STATIC_LIBRARY_SUFFIX})
    add_library(${atomic} STATIC IMPORTED)
    add_dependencies(${atomic} ${atomic_ep})
    set_target_properties(${atomic} 
        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${thirdparty_install}/include)
    set_target_properties(${atomic} PROPERTIES 
        IMPORTED_LOCATION_DEBUG ${atomic_libdir}/Debug/${atomic_libname})
    set_target_properties(${atomic} PROPERTIES 
        IMPORTED_LOCATION_RELEASE ${atomic_libdir}/Release/${atomic_libname})
    set_target_properties(${atomic} PROPERTIES 
        IMPORTED_LOCATION_RELWITHDEBINFO ${atomic_libdir}/RelWithDebInfo/${atomic_libname})
    set_target_properties(${atomic} PROPERTIES 
        IMPORTED_LOCATION_MINSIZEREL ${atomic_libdir}/MinSizeRel/${atomic_libname})
endif()


else()
set(ATOMIC_OPS_FOUND FALSE)
endif()


# target for thirdparty
add_custom_target(thirdparty DEPENDS ${atomic_ep} ${pcre_ep})


##############################################################################
# porijects


# open warning 
if (NOT MSVC)
    add_definitions(-Wall)
    add_definitions(-Wextra)
    add_definitions(-Wno-missing-field-initializers)
    add_definitions(-Wdouble-promotion)
    add_definitions(-Winit-self)
    add_definitions(-Wswitch-default)
    add_definitions(-Wuninitialized)
    add_definitions(-Wfloat-equal)
    add_definitions(-Wshadow)
    add_definitions(-Wunreachable-code)
endif()
if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions(-Wabi=${CMAKE_C_STANDARD})
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions(-Wabi)
endif()


# 
set(NAI_COMPAT_POSIX_MPROTECT ${COMPAT_POSIX_MPROTECT})
configure_file(cmake.config.h.in 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/nai/config/nai_config.${osbits}.h 
    @ONLY)


#list includes
file(GLOB    include_FILES  include/nai/*.h
                            include/nai/runtime/*.h
                            include/nai/service/*.h
                            include/nai/os/*.h
                            include/nai/io/*.h
                            include/nai/event/*.h
                            include/nhttp/*.h
                            include/nhttp/client/*.h
                            include/nhttp/server/*.h)

# list nai sources
file(GLOB    event_FILES    src/nai/event/*.c       src/nai/event/*.h)
file(GLOB    io_FILES       src/nai/io/*.c          src/nai/io/*.h)
file(GLOB    rt_FILES       src/nai/runtime/*.c     src/nai/runtime/*.h)
file(GLOB    se_FILES       src/nai/service/*.c     src/nai/service/*.h)
file(GLOB    os_FILES       src/nai/os/*.c          src/nai/os/*.h
                            src/nai/os/win/*.c      src/nai/os/win/*.h
                            src/nai/os/unix/*.c     src/nai/os/unix/*.h)
file(GLOB    wmatch_FILES   src/nai/thirdparty/wildmatch/wildmatch.*)

# list nhttp sources
if (BUILD_WITH_HTTP)
    file(GLOB    https_FILES    src/nhttp/server/*.c    src/nhttp/server/*.h)
    file(GLOB    httpc_FILES    src/nhttp/client/*.c    src/nhttp/client/*.h)
endif()


if (BUILD_SHARED_LIBS)
    set(nai_deps ${thread} 
        ${sys} ${atomic} ${pcre} ${ssl} ${crypto})
else()
    set(nai_public_deps ${thread} 
        ${sys} ${atomic} ${pcre} ${ssl} ${crypto})
endif()

# add nai library
set(nai "nai")
set(nai_source
    ${include_FILES}
    ${event_FILES}
    ${io_FILES}
    ${rt_FILES}
    ${se_FILES}
    ${os_FILES}
    ${wmatch_FILES}
    ${https_FILES}
    ${httpc_FILES}
)
add_library(${nai} ${nai_source})
set_target_properties(${nai} PROPERTIES C_STANDARD ${CMAKE_C_STANDARD})
set_target_properties(${nai} PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(${nai} PROPERTIES VISIBILITY_INLINES_HIDDEN ON)

if (MSVC)
    set_target_properties(${nai} PROPERTIES OUTPUT_NAME "libnai")
endif()

target_link_libraries(${nai} PUBLIC ${nai_public_deps} PRIVATE ${nai_deps})
target_include_directories(${nai} PRIVATE src)
target_include_directories(${nai} PUBLIC 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> 
    $<INSTALL_INTERFACE:include>)


if (BUILD_SHARED_LIBS)
    target_compile_definitions(${nai} PRIVATE NAI_DLL)
endif()


# list nhttp source
# file(GLOB    server_FILES   src/nhttp/server/*.c    src/nhttp/server/*.h)
# file(GLOB    client_FILES   src/nhttp/client/*.c    src/nhttp/client/*.h)
#
# # add nai library
# set(nhttp "nhttp")
# set(nhttp_include include src/nhttp)
# set(nhttp_source
#     ${server_FILES}
#     ${client_FILES}
# )
# add_library(${nhttp} SHARED ${nhttp_source})
# set_target_properties(${nhttp} PROPERTIES PREFIX "lib")
# set_target_properties(${nhttp} PROPERTIES C_VISIBILITY_PRESET hidden)
# set_target_properties(${nhttp} PROPERTIES VISIBILITY_INLINES_HIDDEN ON)
#
#
# target_compile_definitions(${nhttp} PRIVATE NHTTP_DLL)
# target_include_directories(${nhttp} PRIVATE ${nhttp_include})
# target_link_libraries(${nhttp} ${nai})
#


##############################################################################
# install


include(CMakePackageConfigHelpers)


if (NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR lib)
endif()
if (NOT DEFINED CMAKE_INSTALL_BINDIR)
    set(CMAKE_INSTALL_BINDIR bin)
endif()
if (NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
    set(CMAKE_INSTALL_INCLUDEDIR include)
endif()


set(install_includes ${CMAKE_CURRENT_SOURCE_DIR}/include/nai)
if (BUILD_WITH_HTTP)
    set(install_includes ${install_includes} 
        ${CMAKE_CURRENT_SOURCE_DIR}/include/nhttp)
endif()

install(TARGETS ${nai}
    EXPORT "${PROJECT_NAME}-targets"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY 
    ${install_includes}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PATTERN *.h)

install(
    EXPORT "${PROJECT_NAME}-targets"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})


configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/misc/cmake/${PROJECT_NAME}-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    COMPATIBILITY ExactVersion)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})


if (NOT BUILD_SHARED_LIBS)
    if (NAI_HAVE_PCRE)
        configure_package_config_file(
            ${PROJECT_SOURCE_DIR}/misc/cmake/nai-imported-config.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/${pcre}-config.cmake
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
            NO_CHECK_REQUIRED_COMPONENTS_MACRO)

        install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${pcre}-config.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
    endif()

    if (NAI_HAVE_ATOMIC_OPS)
        configure_package_config_file(
            ${PROJECT_SOURCE_DIR}/misc/cmake/nai-imported-config.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/${atomic}-config.cmake
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
            NO_CHECK_REQUIRED_COMPONENTS_MACRO)

        install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${atomic}-config.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
    endif()
endif()


##############################################################################
# testing
if (BUILD_TESTING)
    include(CTest)
    enable_testing()
    
    file(GLOB    test_FILES         src/test/*.c)
    file(GLOB    test_common_FILES  src/test/common/*.c)
    
    set(test_deps)
    if (QNX)
        list(APPEND test_deps "m")
    endif()
    
    set(test_common "test_common")
    add_library(${test_common} STATIC ${test_common_FILES})
    target_link_libraries(${test_common} ${nai} ${test_deps})
    
    foreach(testfile ${test_FILES})
        get_filename_component(test ${testfile} NAME)
        string(REGEX REPLACE "\\.[^.]*$" "" test ${test})
        message(STATUS "add ${test} ${testfile}")
    
        add_executable(${test} ${testfile})
        set_target_properties(${test} PROPERTIES C_STANDARD ${CMAKE_C_STANDARD})
        target_link_libraries(${test} ${test_common})
    
        add_test(NAME ${test} COMMAND ${test})
        set_tests_properties(${test} PROPERTIES PASS_REGULAR_EXPRESSION "passed")
    endforeach()

endif(BUILD_TESTING)

##############################################################################
# output


set(mman)
if (WIN32)
    list(APPEND mman "mmap")
elseif (NAI_HAVE_SYS_MMAN_H)
    list(APPEND mman "mmap")
endif()

set(process)
if (WIN32)
    list(APPEND process "affinity")
else()
    list(APPEND process "fork")
    if (NAI_HAVE_SIGNAL_H)
        list(APPEND process "signal")
    endif()
    if (NAI_HAVE_SETRLIMIT)
        list(APPEND process "rlimit")
    endif()
    if (NAI_HAVE_SCHED_SETAFFINITY OR NAI_HAVE_CPUSET_SETAFFINITY)
        list(APPEND process "affinity")
    endif()
endif()

set(dlopen "on")

set(event)
if (NAI_HAVE_SELECT OR WIN32)
    list(APPEND event "select")
endif()
if (NAI_HAVE_POLL OR WIN32)
    list(APPEND event "poll")
endif()
if (WIN32)
    list(APPEND event "iocp")
    list(APPEND event "wpoll")
endif()
if (NAI_HAVE_EPOLL)
    list(APPEND event "epoll")
endif()
if (NAI_HAVE_LIBURING_H)
    list(APPEND event "io_uring")
endif()
if (NAI_HAVE_KQUEUE)
    list(APPEND event "kqueue")
endif()

set(aio)
if (WIN32)
    list(APPEND aio "file")
    list(APPEND aio "socket")
elseif (NAI_HAVE_LIBURING_H)
    list(APPEND aio "file")
    list(APPEND aio "socket")
elseif (NAI_HAVE_AIO_H)
    list(APPEND aio "file")
    list(APPEND aio "socket")
elseif (NAI_HAVE_AIO_LINUX_H)
    list(APPEND aio "file")
endif()

set(file)
if (NAI_HAVE_OPENAT)
    list(APPEND file "openat")
endif()
if (NAI_HAVE_FSTATAT)
    list(APPEND file "fstatat")
endif()
if (NAI_HAVE_READV)
    list(APPEND file "readv")
endif()
if (NAI_HAVE_PREAD OR WIN32)
    list(APPEND file "pread")
endif()
if (NAI_HAVE_PREADV)
    list(APPEND file "preadv")
endif()
if (NAI_HAVE_WRITEV)
    list(APPEND file "writev")
endif()
if (NAI_HAVE_PWRITE OR WIN32)
    list(APPEND file "pwrite")
endif()
if (NAI_HAVE_PWRITEV)
    list(APPEND file "pwritev")
endif()

set(socket)
if (NAI_HAVE_RECVMSG OR WIN32)
    list(APPEND socket "recvmsg")
endif()
if (NAI_HAVE_RECVMMSG)
    list(APPEND socket "recvmmsg")
endif()
if (NAI_HAVE_SENDMSG OR WIN32)
    list(APPEND socket "sendmsg")
endif()
if (NAI_HAVE_SENDMMSG)
    list(APPEND socket "sendmmsg")
endif()

set(family)
list(APPEND family "in4")
if (NAI_HAVE_SOCKADDR_IN6 OR WIN32)
    list(APPEND family "in6")
endif()
if (NAI_HAVE_SOCKADDR_UN)
    list(APPEND family "unix")
endif()

set(sendfile)
if (WIN32)
    list(APPEND sendfile "sendfilev")
elseif (NAI_HAVE_SENDFILEV)
    list(APPEND sendfile "sendfilev")
elseif (NAI_HAVE_SENDFILE_LINUX OR NAI_HAVE_SENDFILE_FREEBSD)
    list(APPEND sendfile "sendfile")
endif()

function(message_output line)
    string(REPLACE ";" " " txt "${ARGN}")
    message(STATUS ${line} " " ${txt})
endfunction()

# show complie options
message_output("")
message_output("System environment:")
message_output("")
message_output("  build shared libs ..:" ${BUILD_SHARED_LIBS})
message_output("  build testing ......:" ${BUILD_TESTING})
message_output("  build type .........:" ${CMAKE_BUILD_TYPE})
message_output("  system name ........:" ${CMAKE_SYSTEM_NAME})
message_output("  host system name ...:" ${CMAKE_HOST_SYSTEM_NAME})
message_output("  os bits ............: ${osbits}")
message_output("  c compiler .........: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message_output("  cxx compiler .......: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message_output("  debug c flags ......: ${CMAKE_C_FLAGS_DEBUG}")
message_output("  debug cxx flags ....: ${CMAKE_CXX_FLAGS_DEBUG}")
message_output("  debug link flags ...: ${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
message_output("  release c flags ....: ${CMAKE_C_FLAGS_RELEASE}")
message_output("  release cxx flags ..: ${CMAKE_CXX_FLAGS_RELEASE}")
message_output("  release link flags .: ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
message_output("  install prefix .....: ${CMAKE_INSTALL_PREFIX}")
message_output("  definitions ........:" ${sys_defines})
message_output("  dependencies .......:" ${sys} ${thread})
message_output("    pcre .............:" ${PCRE_LIBRARIES})
message_output("    ssl ..............:" ${SSL_LIBRARIES})
message_output("    atomic_ops .......:" ${ATOMIC_OPS_LIBRARIES})
message_output("  output directories .:")
message_output("    bin ..............:" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message_output("    lib ..............:" ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
message_output("    pdb ..............:" ${CMAKE_PDB_OUTPUT_DIRECTORY})
message_output("")
message_output("System capability:")
message_output("")
message_output("  mman .............. :" ${mman})
message_output("  dlopen .............:" ${dlopen})
message_output("  process ............:" ${process})
message_output("  event model ........:" ${event})
message_output("  aio ................:" ${aio})
message_output("  file ...............:" ${file})
message_output("  sendfile ...........:" ${sendfile})
message_output("  socket .............:" ${socket})
message_output("  address family .....:" ${family})
message_output("")


