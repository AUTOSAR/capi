/// Modified by iSOFT Infrastructure Software Co., Ltd. on 2024-07-20
///
/* ioapi.h -- IO base function header for compress/uncompress .zip
   part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

         Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

         Modifications for Zip64 support
         Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )

         For more info read MiniZip_info.txt

         Changes

    Oct-2009 - Defined ZPOS64_T to fpos_t on windows and u_int64_t on linux. (might need to find a better why for this)
    Oct-2009 - Change to fseeko64, ftello64 and fopen64 so large files would work on linux.
               More if/def section may be needed to support other platforms
    Oct-2009 - Defined fxxxx64 calls to normal fopen/ftell/fseek so they would compile on windows.
                          (but you should use iowin32.c for windows instead)

*/
///
/// @code{.isoft}
/// @module_path=/UCM/UCMLib
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00026
/// @unit_name=ioapi
/// @unit_description=Provide functions for compressing/decompressing .zip files
/// @endcode
///
/// ===========================================================================================

#ifndef _ZLIBIOAPI64_H
#define _ZLIBIOAPI64_H

#if (!defined(_WIN32)) && (!defined(WIN32)) && (!defined(__APPLE__))

    // Linux needs this to support file operation on files larger then 4+GB
    // But might need better if/def to select just the platforms that needs them.

    #ifndef __USE_FILE_OFFSET64
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_
        /// @trace_id_dd=DD_UCM_
        /// @needwork = no
        /// @endcode
        #define __USE_FILE_OFFSET64
    #endif
    #ifndef __USE_LARGEFILE64
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_
        /// @trace_id_dd=DD_UCM_
        /// @needwork = no
        /// @endcode
        #define __USE_LARGEFILE64
    #endif
    #ifndef _LARGEFILE64_SOURCE
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_
        /// @trace_id_dd=DD_UCM_
        /// @needwork = no
        /// @endcode
        #define _LARGEFILE64_SOURCE
    #endif
    #ifndef _FILE_OFFSET_BIT
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_
        /// @trace_id_dd=DD_UCM_
        /// @needwork = no
        /// @endcode
        #define _FILE_OFFSET_BIT 64
    #endif

#endif

#include <cstdio>
#include <cstdlib>

#include "zlib.h"

#if defined(USE_FILE32API)
    #define fopen64  fopen
    #define ftello64 ftell
    #define fseeko64 fseek
#else
    #if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__HAIKU__)                      \
        || defined(MINIZIP_FOPEN_NO_64)
        #define fopen64  fopen
        #define ftello64 ftello
        #define fseeko64 fseeko
    #endif
    #ifdef _MSC_VER
        #define fopen64 fopen
        #if (_MSC_VER >= 1400) && (!(defined(NO_MSCVER_FILE64_FUNC)))
            #define ftello64 _ftelli64
            #define fseeko64 _fseeki64
        #else  // old MSC
            #define ftello64 ftell
            #define fseeko64 fseek
        #endif
    #endif
#endif

///#ifndef ZPOS64_T
///  #ifdef _WIN32
///                #define ZPOS64_T fpos_t
///  #else
///    #include <stdint.h>
///    #define ZPOS64_T uint64_t
///  #endif
///#endif

#ifdef HAVE_MINIZIP64_CONF_H
    #include "mz64conf.h"
#endif

/* a type chosen by DEFINE */
#ifdef HAVE_64BIT_INT_CUSTOM
typedef 64BIT_INT_CUSTOM_TYPE ZPOS64_T;
#else
    #ifdef HAS_STDINT_H
        #include "stdint.h"
typedef uint64_t ZPOS64_T;
    #else

        #if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 ZPOS64_T;
        #else
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
typedef u_int64_t ZPOS64_T;
        #endif
    #endif
#endif

/* Maximum unsigned 32-bit value used as placeholder for zip64 */
#ifndef MAXU32
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    #define MAXU32 (0xffffffff)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ZLIB_FILEFUNC_SEEK_CUR (1)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ZLIB_FILEFUNC_SEEK_END (2)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ZLIB_FILEFUNC_SEEK_SET (0)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ZLIB_FILEFUNC_MODE_READ (1)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ZLIB_FILEFUNC_MODE_WRITE (2)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ZLIB_FILEFUNC_MODE_READWRITEFILTER (3)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ZLIB_FILEFUNC_MODE_EXISTING (4)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ZLIB_FILEFUNC_MODE_CREATE (8)

#ifndef ZCALLBACK
    #if (defined(WIN32) || defined(_WIN32) || defined(WINDOWS) || defined(_WINDOWS)) && defined(CALLBACK)              \
        && defined(USEWINDOWS_CALLBACK)
        #define ZCALLBACK CALLBACK
    #else
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UCM_
        /// @trace_id_dd=DD_UCM_
        /// @needwork = no
        /// @endcode
        #define ZCALLBACK
    #endif
#endif

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    typedef voidpf(ZCALLBACK* Open_File_Func)(voidpf opaque, const char* filename, int mode);
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork
    typedef uLong(ZCALLBACK* Read_File_Func)(voidpf opaque, voidpf stream, void* buf, uLong size);
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork
    typedef uLong(ZCALLBACK* Write_File_Func)(voidpf opaque, voidpf stream, const void* buf, uLong size);
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork
    typedef int(ZCALLBACK* Close_File_Func)(voidpf opaque, voidpf stream);
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork
    typedef int(ZCALLBACK* TestError_File_Func)(voidpf opaque, voidpf stream);

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork
    typedef int64_t(ZCALLBACK* Tell_File_Func)(voidpf opaque, voidpf stream);
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork
    typedef int64_t(ZCALLBACK* Seek_File_Func)(voidpf opaque, voidpf stream, uLong offset, int origin);

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00001
    /// @needwork = no
    /// @endcode
    /* here is the "old" 32 bits structure */
    typedef struct Zlib_Filefunc_Def_s
    {
        Open_File_Func zopenFile;
        Read_File_Func zreadFile;
        Write_File_Func zwriteFile;
        Tell_File_Func ztellFile;
        Seek_File_Func zseekFile;
        Close_File_Func zcloseFile;
        TestError_File_Func zerrorFile;
        voidpf opaque;
    } Zlib_Filefunc_Def;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork
    typedef ZPOS64_T(ZCALLBACK* Tell64_File_Func)(voidpf opaque, voidpf stream);
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork
    typedef int64_t(ZCALLBACK* Seek64_File_Func)(voidpf opaque, voidpf stream, ZPOS64_T offset, int origin);
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork
    typedef voidpf(ZCALLBACK* Open64_File_Func)(voidpf opaque, const void* filename, int mode);

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00002
    /// @needwork = no
    /// @endcode
    typedef struct Zlib_Filefunc64_Def_s
    {
        Open64_File_Func zopen64File;
        Read_File_Func zreadFile;
        Write_File_Func zwriteFile;
        Tell64_File_Func ztell64File;
        Seek64_File_Func zseek64File;
        Close_File_Func zcloseFile;
        TestError_File_Func zerrorFile;
        voidpf opaque;
    } Zlib_Filefunc64_Def;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00003
    /// @needwork = dd
    /// @endcode
    void Fill_Fopen64_Filefunc(Zlib_Filefunc64_Def* pZlibFilefuncDef);
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00004
    /// @needwork = dd
    /// @endcode
    void Fill_Fopen_Filefunc(Zlib_Filefunc_Def* pZlibFilefuncDef);

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00005
    /// @needwork = dd
    /// @endcode
    /* now internal definition, only for zip.c and unzip.h */
    typedef struct Zlib_Filefunc64_32_Def_s
    {
        Zlib_Filefunc64_Def zfileFunc64;
        Open_File_Func zopen32File;
        Tell_File_Func ztell32File;
        Seek_File_Func zseek32File;
    } Zlib_Filefunc64_32_Def;

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ZREAD64(filefunc, filestream, buf, size)                                                                       \
    ((*((filefunc).zfileFunc64.zreadFile))((filefunc).zfileFunc64.opaque, filestream, buf, size))

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ZWRITE64(filefunc, filestream, buf, size)                                                                      \
    ((*((filefunc).zfileFunc64.zwriteFile))((filefunc).zfileFunc64.opaque, filestream, buf, size))
///#define ZTELL64(filefunc,filestream)            ((*((filefunc).ztell64File)) ((filefunc).opaque,filestream))
///#define ZSEEK64(filefunc,filestream,pos,mode)   ((*((filefunc).zseek64File)) ((filefunc).opaque,filestream,pos,mode))
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ZCLOSE64(filefunc, filestream)                                                                                 \
    ((*((filefunc).zfileFunc64.zcloseFile))((filefunc).zfileFunc64.opaque, filestream))
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ZERROR64(filefunc, filestream)                                                                                 \
    ((*((filefunc).zfileFunc64.zerrorFile))((filefunc).zfileFunc64.opaque, filestream))

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00006
    /// @needwork = dd
    /// @endcode
    voidpf Call_Zopen64(const Zlib_Filefunc64_32_Def* pfilefunc, const void* filename, int mode);
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00007
    /// @needwork = dd
    /// @endcode
    int64_t Call_Zseek64(const Zlib_Filefunc64_32_Def* pfilefunc, voidpf filestream, ZPOS64_T offset, int origin);
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00008
    /// @needwork = dd
    /// @endcode
    ZPOS64_T Call_Ztell64(const Zlib_Filefunc64_32_Def* pfilefunc, voidpf filestream);

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00009
    /// @needwork = dd
    /// @endcode
    void Fill_Zlib_Filefunc64_32_Def_From_Filefunc32(Zlib_Filefunc64_32_Def* pFilefunc64Sep32,
                                                     const Zlib_Filefunc_Def* pFileFunc32);

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ZOPEN64(filefunc, filename, mode) (Call_Zopen64((&(filefunc)), (filename), (mode)))
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ZTELL64(filefunc, filestream) (Call_Ztell64((&(filefunc)), (filestream)))
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ZSEEK64(filefunc, filestream, pos, mode) (Call_Zseek64((&(filefunc)), (filestream), (pos), (mode)))

#ifdef __cplusplus
}
#endif

#endif
