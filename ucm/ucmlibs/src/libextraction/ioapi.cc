/// Modified by iSOFT Infrastructure Software Co., Ltd. on 2024-07-20
///
/// @code{.isoft}
/// @module_path=/UCM/UCMLib
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00026
/// @unit_name=ioapi
/// @unit_description=Provide functions for compressing/decompressing .zip files
/// @endcode
///
/// ================================================================
///
/// ioapi.h -- IO base function header for compress/uncompress .zip
/// part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )
/// Modifications for Zip64 support
/// For more info read MiniZip_info.txt
///
/// ================================================================

#if defined(_WIN32) && (!(defined(_CRT_SECURE_NO_WARNINGS)))
    #define _CRT_SECURE_NO_WARNINGS
#endif

#if defined(__APPLE__) || defined(IOAPI_NO_64) || defined(__HAIKU__) || defined(MINIZIP_FOPEN_NO_64)
    // In darwin and perhaps other BSD variants off_t is a 64 bit value, hence no need for specific 64 bit functions
    #define FOPEN_FUNC(filename, mode)          fopen(filename, mode)
    #define FTELLO_FUNC(stream)                 ftello(stream)
    #define FSEEKO_FUNC(stream, offset, origin) fseeko(stream, offset, origin)
#else
    #define FOPEN_FUNC(filename, mode)          fopen64(filename, mode)
    #define FTELLO_FUNC(stream)                 ftello64(stream)
    #define FSEEKO_FUNC(stream, offset, origin) fseeko64(stream, offset, origin)
#endif

#include "ioapi.h"

#if defined(__GNUC__)
    #pragma GCC diagnostic ignored "-Wold-style-cast"  //myadd
#endif

voidpf Call_Zopen64(const Zlib_Filefunc64_32_Def* pfilefunc, const void* filename, int mode)
{
    if (pfilefunc->zfileFunc64.zopen64File != nullptr) {
        return (*(pfilefunc->zfileFunc64.zopen64File))(pfilefunc->zfileFunc64.opaque, filename, mode);
    }
    return (*(pfilefunc->zopen32File))(pfilefunc->zfileFunc64.opaque, reinterpret_cast< const char* >(filename), mode);
}

int64_t Call_Zseek64(const Zlib_Filefunc64_32_Def* pfilefunc, voidpf filestream, ZPOS64_T offset, int origin)
{
    if (pfilefunc->zfileFunc64.zseek64File != nullptr) {
        return (*(pfilefunc->zfileFunc64.zseek64File))(pfilefunc->zfileFunc64.opaque, filestream, offset, origin);
    }
    uLong offsetTruncated = static_cast< uLong >(offset);
    if (offsetTruncated != offset) {
        return -1;
    }
    return (*(pfilefunc->zseek32File))(pfilefunc->zfileFunc64.opaque, filestream, offsetTruncated, origin);
}

ZPOS64_T Call_Ztell64(const Zlib_Filefunc64_32_Def* pfilefunc, voidpf filestream)
{
    if (pfilefunc->zfileFunc64.zseek64File != nullptr) {
        return (*(pfilefunc->zfileFunc64.ztell64File))(pfilefunc->zfileFunc64.opaque, filestream);
    }
    uLong tellULong = static_cast< uLong >((*(pfilefunc->ztell32File))(pfilefunc->zfileFunc64.opaque, filestream));
    if ((tellULong) == MAXU32) {
        return static_cast< ZPOS64_T >(-1);
    }
    return tellULong;
}

void Fill_Zlib_Filefunc64_32_Def_From_Filefunc32(Zlib_Filefunc64_32_Def* pFilefunc64Sep32,
                                                 const Zlib_Filefunc_Def* pFilefunc32)
{
    pFilefunc64Sep32->zfileFunc64.zopen64File = nullptr;
    pFilefunc64Sep32->zopen32File             = pFilefunc32->zopenFile;
    pFilefunc64Sep32->zfileFunc64.zreadFile   = pFilefunc32->zreadFile;
    pFilefunc64Sep32->zfileFunc64.zwriteFile  = pFilefunc32->zwriteFile;
    pFilefunc64Sep32->zfileFunc64.ztell64File = nullptr;
    pFilefunc64Sep32->zfileFunc64.zseek64File = nullptr;
    pFilefunc64Sep32->zfileFunc64.zcloseFile  = pFilefunc32->zcloseFile;
    pFilefunc64Sep32->zfileFunc64.zerrorFile  = pFilefunc32->zerrorFile;
    pFilefunc64Sep32->zfileFunc64.opaque      = pFilefunc32->opaque;
    pFilefunc64Sep32->zseek32File             = pFilefunc32->zseekFile;
    pFilefunc64Sep32->ztell32File             = pFilefunc32->ztellFile;
}

static voidpf ZCALLBACK Fopen_File_Func(voidpf opaque, const char* filename, int mode)
{
    FILE* file            = nullptr;
    const char* modeFopen = nullptr;
    (void)opaque;
    if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ) {
        modeFopen = "rb";
    } else if ((mode & ZLIB_FILEFUNC_MODE_EXISTING) != 0) {
        modeFopen = "r+b";
    } else if ((mode & ZLIB_FILEFUNC_MODE_CREATE) != 0) {
        modeFopen = "wb";
    }

    if ((filename != nullptr) && (modeFopen != nullptr)) {
        file = fopen(filename, modeFopen);
    }
    return file;
}

static voidpf ZCALLBACK Fopen64_File_Func(voidpf opaque, const void* filename, int mode)
{
    FILE* file            = nullptr;
    const char* modeFopen = nullptr;
    (void)opaque;
    if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ) {
        modeFopen = "rb";
    } else if ((mode & ZLIB_FILEFUNC_MODE_EXISTING) != 0) {
        modeFopen = "r+b";
    } else if ((mode & ZLIB_FILEFUNC_MODE_CREATE) != 0) {
        modeFopen = "wb";
    }

    if ((filename != nullptr) && (modeFopen != nullptr)) {
        file = FOPEN_FUNC(reinterpret_cast< const char* >(filename), modeFopen);
    }
    return file;
}

static uLong ZCALLBACK FRead_File_Func(voidpf opaque, voidpf stream, void* buf, uLong size)
{
    uLong ret{0};
    (void)opaque;
    ret = static_cast< uLong >(fread(buf, 1, static_cast< size_t >(size), reinterpret_cast< FILE* >(stream)));
    return ret;
}

static uLong ZCALLBACK Fwrite_File_Func(voidpf opaque, voidpf stream, const void* buf, uLong size)
{
    uLong ret{0};
    (void)opaque;
    ret = static_cast< uLong >(fwrite(buf, 1, static_cast< size_t >(size), reinterpret_cast< FILE* >(stream)));
    return ret;
}

static int64_t ZCALLBACK FTell_File_Func(voidpf opaque, voidpf stream)
{
    int64_t ret{0};
    (void)opaque;
    ret = ftell(reinterpret_cast< FILE* >(stream));
    return ret;
}

static ZPOS64_T ZCALLBACK FTell64_File_Func(voidpf opaque, voidpf stream)
{
    ZPOS64_T ret{0};
    (void)opaque;
    ret = static_cast< ZPOS64_T >(FTELLO_FUNC(reinterpret_cast< FILE* >(stream)));
    return ret;
}

static int64_t ZCALLBACK FSeek_File_Func(voidpf opaque, voidpf stream, uLong offset, int origin)
{
    int fSeekOrigin = 0;
    int64_t ret{0};
    (void)opaque;
    switch (origin) {
        case ZLIB_FILEFUNC_SEEK_CUR:
            fSeekOrigin = SEEK_CUR;
            break;
        case ZLIB_FILEFUNC_SEEK_END:
            fSeekOrigin = SEEK_END;
            break;
        case ZLIB_FILEFUNC_SEEK_SET:
            fSeekOrigin = SEEK_SET;
            break;
        default:
            return -1;
    }
    ret = 0;
    if (fseek(reinterpret_cast< FILE* >(stream), static_cast< int64_t >(offset), fSeekOrigin) != 0) {
        ret = -1;
    }
    return ret;
}

static int64_t ZCALLBACK Fseek64_File_Func(voidpf opaque, voidpf stream, ZPOS64_T offset, int origin)
{
    int fSeekOrigin = 0;
    int64_t ret{0};
    (void)opaque;
    switch (origin) {
        case ZLIB_FILEFUNC_SEEK_CUR:
            fSeekOrigin = SEEK_CUR;
            break;
        case ZLIB_FILEFUNC_SEEK_END:
            fSeekOrigin = SEEK_END;
            break;
        case ZLIB_FILEFUNC_SEEK_SET:
            fSeekOrigin = SEEK_SET;
            break;
        default:
            return -1;
    }
    ret = 0;

    if (FSEEKO_FUNC(reinterpret_cast< FILE* >(stream), (z_off64_t)offset, fSeekOrigin) != 0) {
        ret = -1;
    }

    return ret;
}

static int ZCALLBACK Fclose_File_Func(voidpf opaque, voidpf stream)
{
    int ret{0};
    (void)opaque;
    ret = fclose(reinterpret_cast< FILE* >(stream));
    return ret;
}

static int ZCALLBACK Ferror_File_Func(voidpf opaque, voidpf stream)
{
    int ret{0};
    (void)opaque;
    ret = ferror(reinterpret_cast< FILE* >(stream));
    return ret;
}

void Fill_Fopen_Filefunc(Zlib_Filefunc_Def* pZlibFilefuncDef)
{
    pZlibFilefuncDef->zopenFile  = Fopen_File_Func;
    pZlibFilefuncDef->zreadFile  = FRead_File_Func;
    pZlibFilefuncDef->zwriteFile = Fwrite_File_Func;
    pZlibFilefuncDef->ztellFile  = FTell_File_Func;
    pZlibFilefuncDef->zseekFile  = FSeek_File_Func;
    pZlibFilefuncDef->zcloseFile = Fclose_File_Func;
    pZlibFilefuncDef->zerrorFile = Ferror_File_Func;
    pZlibFilefuncDef->opaque     = nullptr;
}

void Fill_Fopen64_Filefunc(Zlib_Filefunc64_Def* pZlibFilefuncDef)
{
    pZlibFilefuncDef->zopen64File = Fopen64_File_Func;
    pZlibFilefuncDef->zreadFile   = FRead_File_Func;
    pZlibFilefuncDef->zwriteFile  = Fwrite_File_Func;
    pZlibFilefuncDef->ztell64File = FTell64_File_Func;
    pZlibFilefuncDef->zseek64File = Fseek64_File_Func;
    pZlibFilefuncDef->zcloseFile  = Fclose_File_Func;
    pZlibFilefuncDef->zerrorFile  = Ferror_File_Func;
    pZlibFilefuncDef->opaque      = nullptr;
}
