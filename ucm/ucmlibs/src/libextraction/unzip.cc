/// Modified by iSOFT Infrastructure Software Co., Ltd. on 2024-07-20
///
/// unzip.c -- IO for uncompress .zip files using zlib
/// Version 1.1, February 14h, 2010
/// part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )
/// Modifications of Unzip for Zip64
/// Modifications for Zip64 support on both zip and unzip
/// For more info read MiniZip_info.txt
/// ------------------------------------------------------------------------------------
/// Decryption code comes from crypt.c by Info-ZIP but has been greatly reduced in terms of
/// compatibility with older software. The following is from the original crypt.c.
/// Code woven in by Terry Thorsen 1/2003.
/// See the accompanying file LICENSE, version 2000-Apr-09 or later
/// (the contents of which are also included in zip.h) for terms of use.
/// If, for some reason, all these files are missing, the Info-ZIP license
/// also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
/// crypt.c (full version) by Info-ZIP.      Last revised:  [see crypt.h]
/// The encryption/decryption parts of this source code (as opposed to the
/// non-echoing password parts) were originally written in Europe.  The
/// whole source package can be freely distributed, including from the USA.
/// (Prior to January 2000, re-export from the US was a violation of US law.)
/// This encryption code is a direct transcription of the algorithm from
/// Roger Schlafly, described by Phil Katz in the file appnote.txt.  This
/// file (appnote.txt) is distributed with the PKZIP program (even in the
/// version without encryption capabilities).
/// ------------------------------------------------------------------------------------
/// Changes in unzip.c
/// 2007-2008 - Even Rouault - Addition of cpl_unzGetCurrentFileZStreamPos
/// 2007-2008 - Even Rouault - Decoration of symbol names unz* -> cpl_unz*
/// 2007-2008 - Even Rouault - Remove old C style function prototypes
/// 2007-2008 - Even Rouault - Add unzip support for ZIP64
/// Oct-2009 - Mathias Svensson - Removed cpl_* from symbol names (Even Rouault added them but since this is now moved to a new project (minizip64) I renamed them again).
/// Oct-2009 - Mathias Svensson - Fixed problem if uncompressed size was > 4G and compressed size was <4G
/// should only read the compressed/uncompressed size from the Zip64 format if
/// the size from normal header was 0xFFFFFFFF
/// Oct-2009 - Mathias Svensson - Applied some bug fixes from patches received from Gilles Vollant
/// Oct-2009 - Mathias Svensson - Applied support to unzip files with compression method BZIP2 (bzip2 lib is required)
/// Patch created by Daniel Borca
/// Jan-2010 - back to unzip and minizip 1.0 name scheme, with compatibility layer
/// Copyright (C) 1998 - 2010 Gilles Vollant, Even Rouault, Mathias Svensson
///

/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/UCMLib
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00026
/// @unit_name=unzip
/// @unit_description=Provide functionality for decompressing .zip files
/// @endcode
///
/// ================================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifndef NOUNCRYPT
    #define NOUNCRYPT
#endif

#include "unzip.h"
#include "zlib.h"

#if defined(__GNUC__)
    #pragma GCC diagnostic ignored "-Wold-style-cast"  //myadd
#endif

#ifdef STDC
    #include <cstddef>
#endif
#ifdef NO_ERRNO_H
extern int errno;
#else
    #include <cerrno>
#endif

#ifndef local
    #define local static
#endif
/* compile with -Dlocal if your debugger can't find static symbols */

#ifndef CASESENSITIVITYDEFAULT_NO
    #if !defined(unix) && !defined(CASESENSITIVITYDEFAULT_YES)
        #define CASESENSITIVITYDEFAULT_NO
    #endif
#endif

#ifndef UNZ_BUFSIZE
    #define UNZ_BUFSIZE (16384)
#endif

#ifndef UNZ_MAXFILENAMEINZIP
    #define UNZ_MAXFILENAMEINZIP (256)
#endif

#ifndef ALLOC
    #define ALLOC(size) (malloc(size))
#endif

#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)

const char kUnzCopyright[] = " unzip 1.01 Copyright 1998-2004 Gilles Vollant - http://www.winimage.com/zLibDll";

/* Unz_File_Info64_Internal contain internal info about a file in zipfile*/
typedef struct Unz_File_Info64_Internal_s
{
    ZPOS64_T offsetCurfile; /* relative offset of local header 8 bytes */
} Unz_File_Info64_Internal;

/* file_in_zip_read_info_s contain internal information about a file in zipfile,
    when reading and decompress it */
typedef struct
{
    char* readBuffer; /* internal buffer for compressed data */
    z_stream stream;  /* zLib stream structure for inflate */

#ifdef HAVE_BZIP2
    bz_stream bstream; /* bzLib stream structure for bziped */
#endif

    ZPOS64_T posInZipfile;   /* position in byte on the zipfile, for fseek*/
    uLong streamInitialised; /* flag set if stream structure is initialised*/

    ZPOS64_T offsetLocalExtrafield; /* offset of the local extra field */
    uInt sizeLocalExtrafield;       /* size of the local extra field */
    ZPOS64_T posLocalExtrafield;    /* position in the local extra field in read*/
    ZPOS64_T totalOut64;

    uLong crc32;                   /* crc32 of all data uncompressed */
    uLong crc32Wait;               /* crc32 we must obtain after decompress all */
    ZPOS64_T restReadCompressed;   /* number of byte to be decompressed */
    ZPOS64_T restReadUncompressed; /*number of byte to be obtained after decomp*/
    Zlib_Filefunc64_32_Def zFilefunc;
    voidpf filestream;             /* io structure of the zipfile */
    uLong compressionMethod;       /* compression method (0==store) */
    ZPOS64_T byteBeforeTheZipfile; /* byte before the zipfile, (>0 for sfx)*/
    int raw;
} File_In_Zip64_Read_Info_S;

/* Unz64_S contain internal information about the zipfile
*/
typedef struct
{
    Zlib_Filefunc64_32_Def zFilefunc;
    int is64bitOpenFunction;
    voidpf filestream;             /* io structure of the zipfile */
    Unz_Global_Info64 gi;          /* public global information */
    ZPOS64_T byteBeforeTheZipfile; /* byte before the zipfile, (>0 for sfx)*/
    ZPOS64_T numFile;              /* number of the current file in the zipfile*/
    ZPOS64_T posInCentralDir;      /* pos of the current file in the central dir*/
    ZPOS64_T currentFileOk;        /* flag about the usability of the current file*/
    ZPOS64_T centralPos;           /* position of the beginning of the central dir*/

    ZPOS64_T sizeCentralDir;   /* size of the central directory  */
    ZPOS64_T offsetCentralDir; /* offset of start of central directory with
                                   respect to the starting disk number */

    Unz_File_Info64 curFileInfo;                  /* public info about the current file in zip*/
    Unz_File_Info64_Internal curFileInfoInternal; /* private info about it*/
    File_In_Zip64_Read_Info_S* pfileInZipRead;    /* structure about the current
                                        file if we are decompressing it */
    int encrypted;

    int isZip64;

#ifndef NOUNCRYPT
    unsigned long keys[3]; /* keys defining the pseudo-random sequence */
    const z_crc_t* pcrc_32_tab;
#endif
} Unz64_S;

#ifndef NOUNCRYPT
    #include "crypt.h"
#endif

/* ===========================================================================
   Reads a long in LSB order from the given gz_stream. Sets
*/

local int Unz64local_getShort(const Zlib_Filefunc64_32_Def* pZlibFileFuncDef, voidpf filestream, uLong* pX)
{
    unsigned char c[2];
    int err = static_cast< int >(ZREAD64(*pZlibFileFuncDef, filestream, c, 2));
    if (err == 2) {
        constexpr int16_t kNUM_8{8};
        *pX = c[0] | (static_cast< uLong >(c[1]) << kNUM_8);
        return UNZ_OK;
    }
    *pX = 0;
    if (ZERROR64(*pZlibFileFuncDef, filestream)) {
        return UNZ_ERRNO;
    }
    return UNZ_EOF;
}

local int Unz64local_getLong(const Zlib_Filefunc64_32_Def* pZlibFileFuncDef, voidpf filestream, uLong* pX)
{
    unsigned char c[4];
    int err = static_cast< int >(ZREAD64(*pZlibFileFuncDef, filestream, c, 4));
    if (err == 4) {
        constexpr int16_t kNUM_8{8};
        constexpr int16_t kNUM_16{16};
        constexpr int16_t kNUM_24{24};
        *pX = c[0] | (static_cast< uLong >(c[1]) << kNUM_8) | (static_cast< uLong >(c[2]) << kNUM_16)
              | (static_cast< uLong >(c[3]) << kNUM_24);
        return UNZ_OK;
    }
    *pX = 0;
    if (ZERROR64(*pZlibFileFuncDef, filestream)) {
        return UNZ_ERRNO;
    }
    return UNZ_EOF;
}

local int Unz64local_getLong64(const Zlib_Filefunc64_32_Def* pZlibFileFuncDef, voidpf filestream, ZPOS64_T* pX)
{
    constexpr int16_t kNUM_8{8};
    unsigned char c[kNUM_8];
    int err = static_cast< int >(ZREAD64(*pZlibFileFuncDef, filestream, c, kNUM_8));

    constexpr int16_t kNUM_5{5};
    constexpr int16_t kNUM_6{6};
    constexpr int16_t kNUM_7{7};
    constexpr int16_t kNUM_16{16};
    constexpr int16_t kNUM_24{24};
    constexpr int16_t kNUM_32{32};
    constexpr int16_t kNUM_40{40};
    constexpr int16_t kNUM_48{48};
    constexpr int16_t kNUM_56{56};
    if (err == kNUM_8) {
        *pX = c[0] | (static_cast< ZPOS64_T >(c[1]) << kNUM_8) | (static_cast< ZPOS64_T >(c[2]) << kNUM_16)
              | (static_cast< ZPOS64_T >(c[3]) << kNUM_24) | (static_cast< ZPOS64_T >(c[4]) << kNUM_32)
              | (static_cast< ZPOS64_T >(c[kNUM_5]) << kNUM_40) | (static_cast< ZPOS64_T >(c[kNUM_6]) << kNUM_48)
              | (static_cast< ZPOS64_T >(c[kNUM_7]) << kNUM_56);
        return UNZ_OK;
    }
    *pX = 0;
    if (ZERROR64(*pZlibFileFuncDef, filestream)) {
        return UNZ_ERRNO;
    }
    return UNZ_EOF;
}

/* My own strcmpi / strcasecmp */
local int strcmpcasenosensitive_internal(const char* fileName1, const char* fileName2)
{
    constexpr int16_t kNUM_32{0x20};
    for (;;) {
        char c1 = *(fileName1++);
        char c2 = *(fileName2++);
        if ((c1 >= 'a') && (c1 <= 'z')) {
            ////c1 -= 0x20;
            c1 = static_cast< char >((c1 - kNUM_32));  //myadd
        }
        if ((c2 >= 'a') && (c2 <= 'z')) {
            ////c2 -= 0x20;
            c2 = static_cast< char >((c2 - kNUM_32));  //myadd
        }
        if (c1 == '\0') {
            return ((c2 == '\0') ? 0 : -1);
        }
        if (c2 == '\0') {
            return 1;
        }
        if (c1 < c2) {
            return -1;
        }
        if (c1 > c2) {
            return 1;
        }
    }
}

#ifdef CASESENSITIVITYDEFAULT_NO
    #define CASESENSITIVITYDEFAULTVALUE 2
#else
    #define CASESENSITIVITYDEFAULTVALUE 1
#endif

#ifndef STRCMPCASENOSENTIVEFUNCTION
    #define STRCMPCASENOSENTIVEFUNCTION strcmpcasenosensitive_internal
#endif

/*
   Compare two filenames (fileName1,fileName2).
   If iCaseSensitivity = 1, comparison is case sensitive (like strcmp)
   If iCaseSensitivity = 2, comparison is not case sensitive (like strcmpi
                                                                or strcasecmp)
   If iCaseSensitivity = 0, case sensitivity is default of your operating system
        (like 1 on Unix, 2 on Windows)

*/
extern int ZEXPORT UnzStringFileNameCompare(const char* fileName1, const char* fileName2, int iCaseSensitivity)
{
    if (iCaseSensitivity == 0) {
        iCaseSensitivity = CASESENSITIVITYDEFAULTVALUE;
    }

    if (iCaseSensitivity == 1) {
        return strcmp(fileName1, fileName2);
    }

    return STRCMPCASENOSENTIVEFUNCTION(fileName1, fileName2);
}

#ifndef BUFREADCOMMENT
    #define BUFREADCOMMENT (0x400)
#endif

#ifndef CENTRALDIRINVALID
    #define CENTRALDIRINVALID ((ZPOS64_T)(-1))
#endif

/*
  Locate the Central directory of a zipfile (at the end, just before
    the global comment)
*/
local ZPOS64_T Unz64local_SearchCentralDir(const Zlib_Filefunc64_32_Def* pZlibFileFuncDef, voidpf filestream)
{
    constexpr u_int32_t kNUM_65535{0xffff};
    unsigned char* buf{nullptr};
    ZPOS64_T uSizeFile{0};
    ZPOS64_T uBackRead{0};
    ZPOS64_T uMaxBack  = kNUM_65535; /* maximum size of global comment */
    ZPOS64_T uPosFound = CENTRALDIRINVALID;

    if (ZSEEK64(*pZlibFileFuncDef, filestream, 0, ZLIB_FILEFUNC_SEEK_END) != 0) {
        return CENTRALDIRINVALID;
    }

    uSizeFile = ZTELL64(*pZlibFileFuncDef, filestream);

    if (uMaxBack > uSizeFile) {
        uMaxBack = uSizeFile;
    }

    buf = reinterpret_cast< unsigned char* >(ALLOC(BUFREADCOMMENT + 4));
    if (buf == nullptr) {
        return CENTRALDIRINVALID;
    }

    uBackRead = 4;
    while (uBackRead < uMaxBack) {
        uLong uReadSize{0};
        ZPOS64_T uReadPos{0};
        int i{0};
        if (uBackRead + BUFREADCOMMENT > uMaxBack) {
            uBackRead = uMaxBack;
        } else {
            uBackRead += BUFREADCOMMENT;
        }
        uReadPos = uSizeFile - uBackRead;

        uReadSize = ((BUFREADCOMMENT + 4) < (uSizeFile - uReadPos)) ? (BUFREADCOMMENT + 4)
                                                                    : static_cast< uLong >((uSizeFile - uReadPos));
        if (ZSEEK64(*pZlibFileFuncDef, filestream, uReadPos, ZLIB_FILEFUNC_SEEK_SET) != 0) {
            break;
        }

        if (ZREAD64(*pZlibFileFuncDef, filestream, buf, uReadSize) != uReadSize) {
            break;
        }

        constexpr u_int16_t kNUM_5{0x05};
        constexpr u_int16_t kNUM_6{0x06};
        constexpr u_int16_t kNUM_75{0x4b};
        constexpr u_int16_t kNUM_80{0x50};
        for (i = static_cast< int >(uReadSize - 3); (i--) > 0;) {
            if (((*(buf + i)) == kNUM_80) && ((*(buf + i + 1)) == kNUM_75) && ((*(buf + i + 2)) == kNUM_5)
                && ((*(buf + i + 3)) == kNUM_6)) {
                uPosFound = uReadPos + static_cast< unsigned >(i);
                break;
            }
        }

        if (uPosFound != CENTRALDIRINVALID) {
            break;
        }
    }
    free(buf);
    return uPosFound;
}

/*
  Locate the Central directory 64 of a zipfile (at the end, just before
    the global comment)
*/
local ZPOS64_T Unz64local_SearchCentralDir64(const Zlib_Filefunc64_32_Def* pZlibFileFuncDef, voidpf filestream)
{
    constexpr u_int32_t kNUM_65535{0xffff};
    unsigned char* buf{nullptr};
    ZPOS64_T uSizeFile{0};
    ZPOS64_T uBackRead{0};
    ZPOS64_T uMaxBack  = kNUM_65535; /* maximum size of global comment */
    ZPOS64_T uPosFound = CENTRALDIRINVALID;
    uLong uL{0};
    ZPOS64_T relativeOffset{0};

    if (ZSEEK64(*pZlibFileFuncDef, filestream, 0, ZLIB_FILEFUNC_SEEK_END) != 0) {
        return CENTRALDIRINVALID;
    }

    uSizeFile = ZTELL64(*pZlibFileFuncDef, filestream);

    if (uMaxBack > uSizeFile) {
        uMaxBack = uSizeFile;
    }

    buf = reinterpret_cast< unsigned char* >(ALLOC(BUFREADCOMMENT + 4));
    if (buf == nullptr) {
        return CENTRALDIRINVALID;
    }

    uBackRead = 4;
    while (uBackRead < uMaxBack) {
        uLong uReadSize{0};
        ZPOS64_T uReadPos{0};
        int i{0};
        if (uBackRead + BUFREADCOMMENT > uMaxBack) {
            uBackRead = uMaxBack;
        } else {
            uBackRead += BUFREADCOMMENT;
        }
        uReadPos = uSizeFile - uBackRead;

        uReadSize = ((BUFREADCOMMENT + 4) < (uSizeFile - uReadPos)) ? (BUFREADCOMMENT + 4)
                                                                    : static_cast< uLong >((uSizeFile - uReadPos));
        if (ZSEEK64(*pZlibFileFuncDef, filestream, uReadPos, ZLIB_FILEFUNC_SEEK_SET) != 0) {
            break;
        }

        if (ZREAD64(*pZlibFileFuncDef, filestream, buf, uReadSize) != uReadSize) {
            break;
        }

        constexpr u_int16_t kNUM_6{0x06};
        constexpr u_int16_t kNUM_7{0x07};
        constexpr u_int16_t kNUM_75{0x4b};
        constexpr u_int16_t kNUM_80{0x50};
        for (i = static_cast< int >(uReadSize - 3); (i--) > 0;) {
            if (((*(buf + i)) == kNUM_80) && ((*(buf + i + 1)) == kNUM_75) && ((*(buf + i + 2)) == kNUM_6)
                && ((*(buf + i + 3)) == kNUM_7)) {
                uPosFound = uReadPos + static_cast< unsigned >(i);
                break;
            }
        }

        if (uPosFound != CENTRALDIRINVALID) {
            break;
        }
    }
    free(buf);
    if (uPosFound == CENTRALDIRINVALID) {
        return CENTRALDIRINVALID;
    }

    /* Zip64 end of central directory locator */
    if (ZSEEK64(*pZlibFileFuncDef, filestream, uPosFound, ZLIB_FILEFUNC_SEEK_SET) != 0) {
        return CENTRALDIRINVALID;
    }

    /* the signature, already checked */
    if (Unz64local_getLong(pZlibFileFuncDef, filestream, &uL) != UNZ_OK) {
        return CENTRALDIRINVALID;
    }

    /* number of the disk with the start of the zip64 end of central directory */
    if (Unz64local_getLong(pZlibFileFuncDef, filestream, &uL) != UNZ_OK) {
        return CENTRALDIRINVALID;
    }
    if (uL != 0) {
        return CENTRALDIRINVALID;
    }

    /* relative offset of the zip64 end of central directory record */
    if (Unz64local_getLong64(pZlibFileFuncDef, filestream, &relativeOffset) != UNZ_OK) {
        return CENTRALDIRINVALID;
    }

    /* total number of disks */
    if (Unz64local_getLong(pZlibFileFuncDef, filestream, &uL) != UNZ_OK) {
        return CENTRALDIRINVALID;
    }
    if (uL != 1) {
        return CENTRALDIRINVALID;
    }

    /* Goto end of central directory record */
    if (ZSEEK64(*pZlibFileFuncDef, filestream, relativeOffset, ZLIB_FILEFUNC_SEEK_SET) != 0) {
        return CENTRALDIRINVALID;
    }

    /* the signature */
    if (Unz64local_getLong(pZlibFileFuncDef, filestream, &uL) != UNZ_OK) {
        return CENTRALDIRINVALID;
    }

    constexpr u_int32_t kNUM_101075792{0x06064b50};
    if (uL != kNUM_101075792) {
        return CENTRALDIRINVALID;
    }

    return relativeOffset;
}

/*
  Open a Zip file. path contain the full pathname (by example,
     on a Windows NT computer "c:\\test\\zlib114.zip" or on an Unix computer
     "zlib/zlib114.zip".
     If the zipfile cannot be opened (file doesn't exist or in not valid), the
       return value is nullptr.
     Else, the return value is a UnzFile Handle, usable with other function
       of this unzip package.
*/
local UnzFile UnzOpenInternal(const void* path,
                              Zlib_Filefunc64_32_Def* pZlibFileFunc64Sep32Def,
                              int is64bitOpenFunction)
{
    Unz64_S us;
    Unz64_S* s{nullptr};
    ZPOS64_T centralPos{0};
    uLong uL{0};

    uLong numberDisk{0};       /* number of the current disk, used for
                                   spanning ZIP, unsupported, always 0*/
    uLong numberDiskWithCD{0}; /* number the disk with central dir, used
                                   for spanning ZIP, unsupported, always 0*/
    ZPOS64_T numberEntryCD{0}; /* total number of entries in
                                   the central dir
                                   (same than numberEntry on nospan) */

    int err = UNZ_OK;

    if (kUnzCopyright[0] != ' ') {
        return nullptr;
    }

    us.zFilefunc.zseek32File = nullptr;
    us.zFilefunc.ztell32File = nullptr;
    if (pZlibFileFunc64Sep32Def == nullptr) {
        Fill_Fopen64_Filefunc(&us.zFilefunc.zfileFunc64);
    } else {
        us.zFilefunc = *pZlibFileFunc64Sep32Def;
    }
    us.is64bitOpenFunction = is64bitOpenFunction;

    us.filestream = ZOPEN64(us.zFilefunc, path, ZLIB_FILEFUNC_MODE_READ | ZLIB_FILEFUNC_MODE_EXISTING);
    if (us.filestream == nullptr) {
        return nullptr;
    }

    centralPos = Unz64local_SearchCentralDir64(&us.zFilefunc, us.filestream);
    if (centralPos != CENTRALDIRINVALID) {
        uLong uS{0};
        ZPOS64_T uL64{0};

        us.isZip64 = 1;

        if (ZSEEK64(us.zFilefunc, us.filestream, centralPos, ZLIB_FILEFUNC_SEEK_SET) != 0) {
            err = UNZ_ERRNO;
        }

        /* the signature, already checked */
        if (Unz64local_getLong(&us.zFilefunc, us.filestream, &uL) != UNZ_OK) {
            err = UNZ_ERRNO;
        }

        /* size of zip64 end of central directory record */
        if (Unz64local_getLong64(&us.zFilefunc, us.filestream, &uL64) != UNZ_OK) {
            err = UNZ_ERRNO;
        }

        /* version made by */
        if (Unz64local_getShort(&us.zFilefunc, us.filestream, &uS) != UNZ_OK) {
            err = UNZ_ERRNO;
        }

        /* version needed to extract */
        if (Unz64local_getShort(&us.zFilefunc, us.filestream, &uS) != UNZ_OK) {
            err = UNZ_ERRNO;
        }

        /* number of this disk */
        if (Unz64local_getLong(&us.zFilefunc, us.filestream, &numberDisk) != UNZ_OK) {
            err = UNZ_ERRNO;
        }

        /* number of the disk with the start of the central directory */
        if (Unz64local_getLong(&us.zFilefunc, us.filestream, &numberDiskWithCD) != UNZ_OK) {
            err = UNZ_ERRNO;
        }

        /* total number of entries in the central directory on this disk */
        if (Unz64local_getLong64(&us.zFilefunc, us.filestream, &us.gi.numberEntry) != UNZ_OK) {
            err = UNZ_ERRNO;
        }

        /* total number of entries in the central directory */
        if (Unz64local_getLong64(&us.zFilefunc, us.filestream, &numberEntryCD) != UNZ_OK) {
            err = UNZ_ERRNO;
        }

        if ((numberEntryCD != us.gi.numberEntry) || (numberDiskWithCD != 0) || (numberDisk != 0)) {
            err = UNZ_BADZIPFILE;
        }

        /* size of the central directory */
        if (Unz64local_getLong64(&us.zFilefunc, us.filestream, &us.sizeCentralDir) != UNZ_OK) {
            err = UNZ_ERRNO;
        }

        /* offset of start of central directory with respect to the
          starting disk number */
        if (Unz64local_getLong64(&us.zFilefunc, us.filestream, &us.offsetCentralDir) != UNZ_OK) {
            err = UNZ_ERRNO;
        }

        us.gi.sizeComment = 0;
    } else {
        centralPos = Unz64local_SearchCentralDir(&us.zFilefunc, us.filestream);
        if (centralPos == CENTRALDIRINVALID) {
            err = UNZ_ERRNO;
        }

        us.isZip64 = 0;

        if (ZSEEK64(us.zFilefunc, us.filestream, centralPos, ZLIB_FILEFUNC_SEEK_SET) != 0) {
            err = UNZ_ERRNO;
        }

        /* the signature, already checked */
        if (Unz64local_getLong(&us.zFilefunc, us.filestream, &uL) != UNZ_OK) {
            err = UNZ_ERRNO;
        }

        /* number of this disk */
        if (Unz64local_getShort(&us.zFilefunc, us.filestream, &numberDisk) != UNZ_OK) {
            err = UNZ_ERRNO;
        }

        /* number of the disk with the start of the central directory */
        if (Unz64local_getShort(&us.zFilefunc, us.filestream, &numberDiskWithCD) != UNZ_OK) {
            err = UNZ_ERRNO;
        }

        /* total number of entries in the central dir on this disk */
        if (Unz64local_getShort(&us.zFilefunc, us.filestream, &uL) != UNZ_OK) {
            err = UNZ_ERRNO;
        }
        us.gi.numberEntry = uL;

        /* total number of entries in the central dir */
        if (Unz64local_getShort(&us.zFilefunc, us.filestream, &uL) != UNZ_OK) {
            err = UNZ_ERRNO;
        }
        numberEntryCD = uL;

        if ((numberEntryCD != us.gi.numberEntry) || (numberDiskWithCD != 0) || (numberDisk != 0)) {
            err = UNZ_BADZIPFILE;
        }

        /* size of the central directory */
        if (Unz64local_getLong(&us.zFilefunc, us.filestream, &uL) != UNZ_OK) {
            err = UNZ_ERRNO;
        }
        us.sizeCentralDir = uL;

        /* offset of start of central directory with respect to the
            starting disk number */
        if (Unz64local_getLong(&us.zFilefunc, us.filestream, &uL) != UNZ_OK) {
            err = UNZ_ERRNO;
        }
        us.offsetCentralDir = uL;

        /* zipfile comment length */
        if (Unz64local_getShort(&us.zFilefunc, us.filestream, &us.gi.sizeComment) != UNZ_OK) {
            err = UNZ_ERRNO;
        }
    }

    if ((centralPos < us.offsetCentralDir + us.sizeCentralDir) && (err == UNZ_OK)) {
        err = UNZ_BADZIPFILE;
    }

    if (err != UNZ_OK) {
        ZCLOSE64(us.zFilefunc, us.filestream);
        return nullptr;
    }

    us.byteBeforeTheZipfile = centralPos - (us.offsetCentralDir + us.sizeCentralDir);
    us.centralPos           = centralPos;
    us.pfileInZipRead       = nullptr;
    us.encrypted            = 0;

    s = reinterpret_cast< Unz64_S* >(ALLOC(sizeof(Unz64_S)));
    if (s != nullptr) {
        *s = us;
        UnzGoToFirstFile(reinterpret_cast< UnzFile >(s));
    }
    return reinterpret_cast< UnzFile >(s);
}

extern UnzFile ZEXPORT UnzOpen2(const char* path, Zlib_Filefunc_Def* pZlibFileFunc32Def)
{
    if (pZlibFileFunc32Def != nullptr) {
        Zlib_Filefunc64_32_Def zlibFileFunc64Sep32DefFill;
        Fill_Zlib_Filefunc64_32_Def_From_Filefunc32(&zlibFileFunc64Sep32DefFill, pZlibFileFunc32Def);
        return UnzOpenInternal(path, &zlibFileFunc64Sep32DefFill, 0);
    }
    return UnzOpenInternal(path, nullptr, 0);
}

extern UnzFile ZEXPORT UnzOpen2_64(const void* path, Zlib_Filefunc64_Def* pZlibFileFuncDef)
{
    if (pZlibFileFuncDef != nullptr) {
        Zlib_Filefunc64_32_Def zlibFileFunc64Sep32DefFill;
        zlibFileFunc64Sep32DefFill.zfileFunc64 = *pZlibFileFuncDef;
        zlibFileFunc64Sep32DefFill.ztell32File = nullptr;
        zlibFileFunc64Sep32DefFill.zseek32File = nullptr;
        return UnzOpenInternal(path, &zlibFileFunc64Sep32DefFill, 1);
    }
    return UnzOpenInternal(path, nullptr, 1);
}

extern UnzFile ZEXPORT UnzOpen(const char* path) { return UnzOpenInternal(path, nullptr, 0); }

extern UnzFile ZEXPORT UnzOpen64(const void* path) { return UnzOpenInternal(path, nullptr, 1); }

/*
  Close a ZipFile opened with unzOpen.
  If there is files inside the .Zip opened with unzOpenCurrentFile (see later),
    these files MUST be closed with unzCloseCurrentFile before call unzClose.
  return UNZ_OK if there is no problem. */
extern int ZEXPORT UnzClose(UnzFile file)
{
    Unz64_S* s{nullptr};
    if (file == nullptr) {
        return UNZ_PARAMERROR;
    }
    s = reinterpret_cast< Unz64_S* >(file);

    if (s->pfileInZipRead != nullptr) {
        UnzCloseCurrentFile(file);
    }

    ZCLOSE64(s->zFilefunc, s->filestream);
    free(s);
    return UNZ_OK;
}

/*
  Write info about the ZipFile in the *pglobal_info structure.
  No preparation of the structure is needed
  return UNZ_OK if there is no problem. */
extern int ZEXPORT UnzGetGlobalInfo64(UnzFile file, Unz_Global_Info64* pglobalInfo)
{
    Unz64_S* s{nullptr};
    if (file == nullptr) {
        return UNZ_PARAMERROR;
    }
    s            = reinterpret_cast< Unz64_S* >(file);
    *pglobalInfo = s->gi;
    return UNZ_OK;
}

extern int ZEXPORT UnzGetGlobalInfo(UnzFile file, Unz_Global_Info* pglobalInfo32)
{
    Unz64_S* s{nullptr};
    if (file == nullptr) {
        return UNZ_PARAMERROR;
    }
    s = reinterpret_cast< Unz64_S* >(file);
    /* to do : check if numberEntry is not truncated */
    pglobalInfo32->numberEntry = static_cast< uLong >(s->gi.numberEntry);
    pglobalInfo32->sizeComment = s->gi.sizeComment;
    return UNZ_OK;
}
/*
   Translate date/time from Dos format to Tm_Unz (readable more easily)
*/
local void Unz64local_DosDateToTmuDate(ZPOS64_T ulDosDate, Tm_Unz* ptm)
{
    ZPOS64_T uDate{0};
    constexpr u_int16_t kNUM_16{16};
    constexpr u_int16_t kNUM_31{0x1f};
    constexpr u_int16_t kNUM_480{0x1E0};
    constexpr u_int16_t kNUM_32{0x20};
    constexpr u_int16_t kNUM_65024{0x0FE00};
    constexpr u_int16_t kNUM_512{0x0200};
    constexpr u_int16_t kNUM_1980{1980};
    constexpr u_int16_t kNUM_63488{0xF800};
    constexpr u_int16_t kNUM_2048{0x800};
    constexpr u_int16_t kNUM_2016{0x7E0};

    uDate       = static_cast< ZPOS64_T >((ulDosDate >> kNUM_16));
    ptm->tmMday = static_cast< int >((uDate & kNUM_31));
    ptm->tmMon  = static_cast< int >(((((uDate)&kNUM_480) / kNUM_32) - 1));
    ptm->tmYear = static_cast< int >((((uDate & kNUM_65024) / kNUM_512) + kNUM_1980));

    ptm->tmHour = static_cast< int >(((ulDosDate & kNUM_63488) / kNUM_2048));
    ptm->tmMin  = static_cast< int >(((ulDosDate & kNUM_2016) / kNUM_32));
    ptm->tmSec  = static_cast< int >((2 * (ulDosDate & kNUM_31)));
}

/*
  Get Info about the current file in the zipfile, with internal only info
*/
local int Unz64local_GetCurrentFileInfoInternal(UnzFile file,
                                                Unz_File_Info64* pileInfo,
                                                Unz_File_Info64_Internal* pfileInfoInternal,
                                                char* szFileName,
                                                uLong fileNameBufferSize,
                                                void* extraField,
                                                uLong extraFieldBufferSize,
                                                char* szComment,
                                                uLong commentBufferSize)
{
    Unz64_S* s{nullptr};
    Unz_File_Info64 fileInfo;
    Unz_File_Info64_Internal fileInfoInternal;
    int err = UNZ_OK;
    uLong uMagic{0};
    ////long lSeek=0;
    uLong lSeek = 0;  //myadd
    uLong uL{0};

    if (file == nullptr) {
        return UNZ_PARAMERROR;
    }
    s = reinterpret_cast< Unz64_S* >(file);
    if (ZSEEK64(s->zFilefunc, s->filestream, s->posInCentralDir + s->byteBeforeTheZipfile, ZLIB_FILEFUNC_SEEK_SET)
        != 0) {
        err = UNZ_ERRNO;
    }

    constexpr u_int32_t kNUM_33639248{0x02014b50};
    constexpr u_int16_t kNUM_65535{0xffff};
    /* we check the magic */
    if (err == UNZ_OK) {
        if (Unz64local_getLong(&s->zFilefunc, s->filestream, &uMagic) != UNZ_OK) {
            err = UNZ_ERRNO;
        } else if (uMagic != kNUM_33639248) {
            err = UNZ_BADZIPFILE;
        }
    }

    if (Unz64local_getShort(&s->zFilefunc, s->filestream, &fileInfo.version) != UNZ_OK) {
        err = UNZ_ERRNO;
    }

    if (Unz64local_getShort(&s->zFilefunc, s->filestream, &fileInfo.versionNeeded) != UNZ_OK) {
        err = UNZ_ERRNO;
    }

    if (Unz64local_getShort(&s->zFilefunc, s->filestream, &fileInfo.flag) != UNZ_OK) {
        err = UNZ_ERRNO;
    }

    if (Unz64local_getShort(&s->zFilefunc, s->filestream, &fileInfo.compressionMethod) != UNZ_OK) {
        err = UNZ_ERRNO;
    }

    if (Unz64local_getLong(&s->zFilefunc, s->filestream, &fileInfo.dosDate) != UNZ_OK) {
        err = UNZ_ERRNO;
    }

    Unz64local_DosDateToTmuDate(fileInfo.dosDate, &fileInfo.tmuDate);

    if (Unz64local_getLong(&s->zFilefunc, s->filestream, &fileInfo.crc) != UNZ_OK) {
        err = UNZ_ERRNO;
    }

    if (Unz64local_getLong(&s->zFilefunc, s->filestream, &uL) != UNZ_OK) {
        err = UNZ_ERRNO;
    }
    fileInfo.compressedSize = uL;

    if (Unz64local_getLong(&s->zFilefunc, s->filestream, &uL) != UNZ_OK) {
        err = UNZ_ERRNO;
    }
    fileInfo.uncompressedSize = uL;

    if (Unz64local_getShort(&s->zFilefunc, s->filestream, &fileInfo.sizeFilename) != UNZ_OK) {
        err = UNZ_ERRNO;
    }

    if (Unz64local_getShort(&s->zFilefunc, s->filestream, &fileInfo.sizeFileExtra) != UNZ_OK) {
        err = UNZ_ERRNO;
    }

    if (Unz64local_getShort(&s->zFilefunc, s->filestream, &fileInfo.sizeFileComment) != UNZ_OK) {
        err = UNZ_ERRNO;
    }

    if (Unz64local_getShort(&s->zFilefunc, s->filestream, &fileInfo.diskNumStart) != UNZ_OK) {
        err = UNZ_ERRNO;
    }

    if (Unz64local_getShort(&s->zFilefunc, s->filestream, &fileInfo.internalFa) != UNZ_OK) {
        err = UNZ_ERRNO;
    }

    if (Unz64local_getLong(&s->zFilefunc, s->filestream, &fileInfo.externalFa) != UNZ_OK) {
        err = UNZ_ERRNO;
    }

    // relative offset of local header
    if (Unz64local_getLong(&s->zFilefunc, s->filestream, &uL) != UNZ_OK) {
        err = UNZ_ERRNO;
    }
    fileInfoInternal.offsetCurfile = uL;

    lSeek += fileInfo.sizeFilename;
    if ((err == UNZ_OK) && (szFileName != nullptr)) {
        uLong uSizeRead{0};
        if (fileInfo.sizeFilename < fileNameBufferSize) {
            *(szFileName + fileInfo.sizeFilename) = '\0';
            uSizeRead                             = fileInfo.sizeFilename;
        } else {
            uSizeRead = fileNameBufferSize;
        }

        if ((fileInfo.sizeFilename > 0) && (fileNameBufferSize > 0)) {
            if (ZREAD64(s->zFilefunc, s->filestream, szFileName, uSizeRead) != uSizeRead) {
                err = UNZ_ERRNO;
            }
        }
        lSeek -= uSizeRead;
    }

    // Read extrafield
    if ((err == UNZ_OK) && (extraField != nullptr)) {
        ZPOS64_T uSizeRead{0};
        if (fileInfo.sizeFileExtra < extraFieldBufferSize) {
            uSizeRead = fileInfo.sizeFileExtra;
        } else {
            uSizeRead = extraFieldBufferSize;
        }

        if (lSeek != 0) {
            if (ZSEEK64(s->zFilefunc, s->filestream, (ZPOS64_T)lSeek, ZLIB_FILEFUNC_SEEK_CUR) == 0) {
                lSeek = 0;
            } else {
                err = UNZ_ERRNO;
            }
        }

        if ((fileInfo.sizeFileExtra > 0) && (extraFieldBufferSize > 0)) {
            if (ZREAD64(s->zFilefunc, s->filestream, extraField, (uLong)uSizeRead) != uSizeRead) {
                err = UNZ_ERRNO;
            }
        }

        lSeek += fileInfo.sizeFileExtra - static_cast< uLong >(uSizeRead);
    } else {
        lSeek += fileInfo.sizeFileExtra;
    }

    if ((err == UNZ_OK) && (fileInfo.sizeFileExtra != 0)) {
        uLong acc = 0;

        // since lSeek now points to after the extra field we need to move back
        lSeek -= fileInfo.sizeFileExtra;

        if (lSeek != 0) {
            if (ZSEEK64(s->zFilefunc, s->filestream, (ZPOS64_T)lSeek, ZLIB_FILEFUNC_SEEK_CUR) == 0) {
                lSeek = 0;
            } else {
                err = UNZ_ERRNO;
            }
        }

        while (acc < fileInfo.sizeFileExtra) {
            uLong headerId{0};
            uLong dataSize{0};

            if (Unz64local_getShort(&s->zFilefunc, s->filestream, &headerId) != UNZ_OK) {
                err = UNZ_ERRNO;
            }

            if (Unz64local_getShort(&s->zFilefunc, s->filestream, &dataSize) != UNZ_OK) {
                err = UNZ_ERRNO;
            }

            /* ZIP64 extra fields */
            if (headerId == 0x0001) {
                if (fileInfo.uncompressedSize == MAXU32) {
                    if (Unz64local_getLong64(&s->zFilefunc, s->filestream, &fileInfo.uncompressedSize) != UNZ_OK) {
                        err = UNZ_ERRNO;
                    }
                }

                if (fileInfo.compressedSize == MAXU32) {
                    if (Unz64local_getLong64(&s->zFilefunc, s->filestream, &fileInfo.compressedSize) != UNZ_OK) {
                        err = UNZ_ERRNO;
                    }
                }

                if (fileInfoInternal.offsetCurfile == MAXU32) {
                    /* Relative Header offset */
                    if (Unz64local_getLong64(&s->zFilefunc, s->filestream, &fileInfoInternal.offsetCurfile) != UNZ_OK) {
                        err = UNZ_ERRNO;
                    }
                }

                if (fileInfo.diskNumStart == kNUM_65535) {
                    /* Disk Start Number */
                    if (Unz64local_getLong(&s->zFilefunc, s->filestream, &fileInfo.diskNumStart) != UNZ_OK) {
                        err = UNZ_ERRNO;
                    }
                }

            } else {
                if (ZSEEK64(s->zFilefunc, s->filestream, dataSize, ZLIB_FILEFUNC_SEEK_CUR) != 0) {
                    err = UNZ_ERRNO;
                }
            }

            acc += 2 + 2 + dataSize;
        }
    }

    if ((err == UNZ_OK) && (szComment != nullptr)) {
        uLong uSizeRead{0};
        if (fileInfo.sizeFileComment < commentBufferSize) {
            *(szComment + fileInfo.sizeFileComment) = '\0';
            uSizeRead                               = fileInfo.sizeFileComment;
        } else {
            uSizeRead = commentBufferSize;
        }

        if (lSeek != 0) {
            if (ZSEEK64(s->zFilefunc, s->filestream, (ZPOS64_T)lSeek, ZLIB_FILEFUNC_SEEK_CUR) == 0) {
                lSeek = 0;
            } else {
                err = UNZ_ERRNO;
            }
        }

        if ((fileInfo.sizeFileComment > 0) && (commentBufferSize > 0)) {
            if (ZREAD64(s->zFilefunc, s->filestream, szComment, uSizeRead) != uSizeRead) {
                err = UNZ_ERRNO;
            }
        }
        lSeek += fileInfo.sizeFileComment - uSizeRead;
    } else {
        lSeek += fileInfo.sizeFileComment;
    }

    if ((err == UNZ_OK) && (pileInfo != nullptr)) {
        *pileInfo = fileInfo;
    }

    if ((err == UNZ_OK) && (pfileInfoInternal != nullptr)) {
        *pfileInfoInternal = fileInfoInternal;
    }

    return err;
}

/*
  Write info about the ZipFile in the *pglobal_info structure.
  No preparation of the structure is needed
  return UNZ_OK if there is no problem.
*/
extern int ZEXPORT UnzGetCurrentFileInfo64(UnzFile file,
                                           Unz_File_Info64* pileInfo,
                                           char* szFileName,
                                           uLong fileNameBufferSize,
                                           void* extraField,
                                           uLong extraFieldBufferSize,
                                           char* szComment,
                                           uLong commentBufferSize)
{
    return Unz64local_GetCurrentFileInfoInternal(file, pileInfo, nullptr, szFileName, fileNameBufferSize, extraField,
                                                 extraFieldBufferSize, szComment, commentBufferSize);
}

extern int ZEXPORT UnzGetCurrentFileInfo(UnzFile file,
                                         Unz_File_Info* pileInfo,
                                         char* szFileName,
                                         uLong fileNameBufferSize,
                                         void* extraField,
                                         uLong extraFieldBufferSize,
                                         char* szComment,
                                         uLong commentBufferSize)
{
    int err{0};
    Unz_File_Info64 fileInfo64;
    err = Unz64local_GetCurrentFileInfoInternal(file, &fileInfo64, nullptr, szFileName, fileNameBufferSize, extraField,
                                                extraFieldBufferSize, szComment, commentBufferSize);
    if ((err == UNZ_OK) && (pileInfo != nullptr)) {
        pileInfo->version           = fileInfo64.version;
        pileInfo->versionNeeded     = fileInfo64.versionNeeded;
        pileInfo->flag              = fileInfo64.flag;
        pileInfo->compressionMethod = fileInfo64.compressionMethod;
        pileInfo->dosDate           = fileInfo64.dosDate;
        pileInfo->crc               = fileInfo64.crc;

        pileInfo->sizeFilename    = fileInfo64.sizeFilename;
        pileInfo->sizeFileExtra   = fileInfo64.sizeFileExtra;
        pileInfo->sizeFileComment = fileInfo64.sizeFileComment;

        pileInfo->diskNumStart = fileInfo64.diskNumStart;
        pileInfo->internalFa   = fileInfo64.internalFa;
        pileInfo->externalFa   = fileInfo64.externalFa;

        pileInfo->tmuDate = fileInfo64.tmuDate;

        pileInfo->compressedSize   = static_cast< uLong >(fileInfo64.compressedSize);
        pileInfo->uncompressedSize = static_cast< uLong >(fileInfo64.uncompressedSize);
    }
    return err;
}
/*
  Set the current file of the zipfile to the first file.
  return UNZ_OK if there is no problem
*/
extern int ZEXPORT UnzGoToFirstFile(UnzFile file)
{
    int err = UNZ_OK;
    Unz64_S* s{nullptr};
    if (file == nullptr) {
        return UNZ_PARAMERROR;
    }
    s                  = reinterpret_cast< Unz64_S* >(file);
    s->posInCentralDir = s->offsetCentralDir;
    s->numFile         = 0;
    err = Unz64local_GetCurrentFileInfoInternal(file, &s->curFileInfo, &s->curFileInfoInternal, nullptr, 0, nullptr, 0,
                                                nullptr, 0);
    s->currentFileOk = (err == UNZ_OK) ? 1 : 0;
    return err;
}

/*
  Set the current file of the zipfile to the next file.
  return UNZ_OK if there is no problem
  return UNZ_END_OF_LIST_OF_FILE if the actual file was the latest.
*/
extern int ZEXPORT UnzGoToNextFile(UnzFile file)
{
    Unz64_S* s{nullptr};
    int err{0};

    if (file == nullptr) {
        return UNZ_PARAMERROR;
    }
    s = reinterpret_cast< Unz64_S* >(file);
    if (s->currentFileOk == 0) {
        return UNZ_END_OF_LIST_OF_FILE;
    }
    constexpr u_int16_t kNUM_65535{0xffff};
    if (s->gi.numberEntry != kNUM_65535) { /* 2^16 files overflow hack */
        if (s->numFile + 1 == s->gi.numberEntry) {
            return UNZ_END_OF_LIST_OF_FILE;
        }
    }

    s->posInCentralDir += SIZECENTRALDIRITEM + s->curFileInfo.sizeFilename + s->curFileInfo.sizeFileExtra
                          + s->curFileInfo.sizeFileComment;
    s->numFile++;
    err = Unz64local_GetCurrentFileInfoInternal(file, &s->curFileInfo, &s->curFileInfoInternal, nullptr, 0, nullptr, 0,
                                                nullptr, 0);
    s->currentFileOk = (err == UNZ_OK) ? 1 : 0;
    return err;
}

/*
  Try locate the file szFileName in the zipfile.
  For the iCaseSensitivity signification, see UnzStringFileNameCompare

  return value :
  UNZ_OK if the file is found. It becomes the current file.
  UNZ_END_OF_LIST_OF_FILE if the file is not found
*/
extern int ZEXPORT UnzLocateFile(UnzFile file, const char* szFileName, int iCaseSensitivity)
{
    Unz64_S* s{nullptr};
    int err{0};

    /* We remember the 'current' position in the file so that we can jump
     * back there if we fail.
     */
    Unz_File_Info64 curFileInfoSaved;
    Unz_File_Info64_Internal curFileInfoInternalSaved;
    ZPOS64_T numFileSaved{0};
    ZPOS64_T posInCentralDirSaved{0};

    if (file == nullptr) {
        return UNZ_PARAMERROR;
    }

    if (strlen(szFileName) >= UNZ_MAXFILENAMEINZIP) {
        return UNZ_PARAMERROR;
    }

    s = reinterpret_cast< Unz64_S* >(file);
    if (s->currentFileOk == 0) {
        return UNZ_END_OF_LIST_OF_FILE;
    }

    /* Save the current state */
    numFileSaved             = s->numFile;
    posInCentralDirSaved     = s->posInCentralDir;
    curFileInfoSaved         = s->curFileInfo;
    curFileInfoInternalSaved = s->curFileInfoInternal;

    err = UnzGoToFirstFile(file);

    while (err == UNZ_OK) {
        char szCurrentFileName[UNZ_MAXFILENAMEINZIP + 1];
        err = UnzGetCurrentFileInfo64(file, nullptr, szCurrentFileName, sizeof(szCurrentFileName) - 1, nullptr, 0,
                                      nullptr, 0);
        if (err == UNZ_OK) {
            if (UnzStringFileNameCompare(szCurrentFileName, szFileName, iCaseSensitivity) == 0) {
                return UNZ_OK;
            }
            err = UnzGoToNextFile(file);
        }
    }

    /* We failed, so restore the state of the 'current file' to where we
     * were.
     */
    s->numFile             = numFileSaved;
    s->posInCentralDir     = posInCentralDirSaved;
    s->curFileInfo         = curFileInfoSaved;
    s->curFileInfoInternal = curFileInfoInternalSaved;
    return err;
}

/*
///////////////////////////////////////////
// Contributed by Ryan Haksi (mailto://cryogen@infoserve.net)
// I need random access
//
// Further optimization could be realized by adding an ability
// to cache the directory in memory. The goal being a single
// comprehensive file read to put the file I need in a memory.
*/

/*
typedef struct Unz_File_Pos_s
{
    ZPOS64_T posInZipDirectory;   // offset in file
    ZPOS64_T numOfFile;            // # of file
} Unz_File_Pos;
*/

extern int ZEXPORT UnzGetFilePos64(UnzFile file, Unz64_File_Pos* filePos)
{
    Unz64_S* s{nullptr};

    if (file == nullptr || filePos == nullptr) {
        return UNZ_PARAMERROR;
    }
    s = reinterpret_cast< Unz64_S* >(file);
    if (s->currentFileOk == 0) {
        return UNZ_END_OF_LIST_OF_FILE;
    }

    filePos->posInZipDirectory = s->posInCentralDir;
    filePos->numOfFile         = s->numFile;

    return UNZ_OK;
}

extern int ZEXPORT UnzGetFilePos(UnzFile file, Unz_File_Pos* filePos)
{
    Unz64_File_Pos filePos64;
    int err = UnzGetFilePos64(file, &filePos64);
    if (err == UNZ_OK) {
        filePos->posInZipDirectory = static_cast< uLong >(filePos64.posInZipDirectory);
        filePos->numOfFile         = static_cast< uLong >(filePos64.numOfFile);
    }
    return err;
}

extern int ZEXPORT UnzGoToFilePos64(UnzFile file, const Unz64_File_Pos* filePos)
{
    Unz64_S* s{nullptr};
    int err{0};

    if (file == nullptr || filePos == nullptr) {
        return UNZ_PARAMERROR;
    }
    s = reinterpret_cast< Unz64_S* >(file);

    /* jump to the right spot */
    s->posInCentralDir = filePos->posInZipDirectory;
    s->numFile         = filePos->numOfFile;

    /* set the current file */
    err = Unz64local_GetCurrentFileInfoInternal(file, &s->curFileInfo, &s->curFileInfoInternal, nullptr, 0, nullptr, 0,
                                                nullptr, 0);
    /* return results */
    s->currentFileOk = (err == UNZ_OK) ? 1 : 0;
    return err;
}

extern int ZEXPORT UnzGoToFilePos(UnzFile file, Unz_File_Pos* filePos)
{
    Unz64_File_Pos filePos64;
    if (filePos == nullptr) {
        return UNZ_PARAMERROR;
    }

    filePos64.posInZipDirectory = filePos->posInZipDirectory;
    filePos64.numOfFile         = filePos->numOfFile;
    return UnzGoToFilePos64(file, &filePos64);
}

/*
// Unzip Helper Functions - should be here?
///////////////////////////////////////////
*/

/*
  Read the local header of the current zipfile
  Check the coherency of the local header and info in the end of central
        directory about this file
  store in *piSizeVar the size of extra info in local header
        (filename and size of extra field data)
*/
local int Unz64local_CheckCurrentFileCoherencyHeader(Unz64_S* s,
                                                     uInt* piSizeVar,
                                                     ZPOS64_T* poffsetLocalExtrafield,
                                                     uInt* psizeLocalExtrafield)
{
    uLong uMagic{0};
    uLong uData{0};
    uLong uFlags{0};
    uLong sizeFilename{0};
    uLong sizeExtraField{0};
    int err = UNZ_OK;

    constexpr u_int32_t kNUM_67324752{0x04034b50};
    constexpr u_int8_t kNUM_8{8};
    constexpr u_int32_t kNUM_4294967295{0xFFFFFFFF};

    *piSizeVar              = 0;
    *poffsetLocalExtrafield = 0;
    *psizeLocalExtrafield   = 0;

    if (ZSEEK64(s->zFilefunc, s->filestream, s->curFileInfoInternal.offsetCurfile + s->byteBeforeTheZipfile,
                ZLIB_FILEFUNC_SEEK_SET)
        != 0) {
        return UNZ_ERRNO;
    }

    if (err == UNZ_OK) {
        if (Unz64local_getLong(&s->zFilefunc, s->filestream, &uMagic) != UNZ_OK) {
            err = UNZ_ERRNO;
        } else if (uMagic != kNUM_67324752) {
            err = UNZ_BADZIPFILE;
        }
    }

    if (Unz64local_getShort(&s->zFilefunc, s->filestream, &uData) != UNZ_OK) {
        err = UNZ_ERRNO;
    }
    /*
    else if ((err==UNZ_OK) && (uData!=s->curFileInfo.wVersion))
        err=UNZ_BADZIPFILE;
*/
    if (Unz64local_getShort(&s->zFilefunc, s->filestream, &uFlags) != UNZ_OK) {
        err = UNZ_ERRNO;
    }

    if (Unz64local_getShort(&s->zFilefunc, s->filestream, &uData) != UNZ_OK) {
        err = UNZ_ERRNO;
    } else if ((err == UNZ_OK) && (uData != s->curFileInfo.compressionMethod)) {
        err = UNZ_BADZIPFILE;
    }

    if ((err == UNZ_OK) && (s->curFileInfo.compressionMethod != 0) &&
        /* #ifdef HAVE_BZIP2 */
        (s->curFileInfo.compressionMethod != Z_BZIP2ED) &&
        /* #endif */
        (s->curFileInfo.compressionMethod != Z_DEFLATED)) {
        err = UNZ_BADZIPFILE;
    }

    if (Unz64local_getLong(&s->zFilefunc, s->filestream, &uData) != UNZ_OK) { /* date/time */
        err = UNZ_ERRNO;
    }

    if (Unz64local_getLong(&s->zFilefunc, s->filestream, &uData) != UNZ_OK) { /* crc */
        err = UNZ_ERRNO;
    } else if ((err == UNZ_OK) && (uData != s->curFileInfo.crc) && ((uFlags & kNUM_8) == 0)) {
        err = UNZ_BADZIPFILE;
    }

    if (Unz64local_getLong(&s->zFilefunc, s->filestream, &uData) != UNZ_OK) { /* size compr */
        err = UNZ_ERRNO;
    } else if (uData != kNUM_4294967295 && (err == UNZ_OK) && (uData != s->curFileInfo.compressedSize)
               && ((uFlags & kNUM_8) == 0)) {
        err = UNZ_BADZIPFILE;
    }

    if (Unz64local_getLong(&s->zFilefunc, s->filestream, &uData) != UNZ_OK) { /* size uncompr */
        err = UNZ_ERRNO;
    } else if (uData != kNUM_4294967295 && (err == UNZ_OK) && (uData != s->curFileInfo.uncompressedSize)
               && ((uFlags & kNUM_8) == 0)) {
        err = UNZ_BADZIPFILE;
    }

    if (Unz64local_getShort(&s->zFilefunc, s->filestream, &sizeFilename) != UNZ_OK) {
        err = UNZ_ERRNO;
    } else if ((err == UNZ_OK) && (sizeFilename != s->curFileInfo.sizeFilename)) {
        err = UNZ_BADZIPFILE;
    }

    *piSizeVar += static_cast< uInt >(sizeFilename);

    if (Unz64local_getShort(&s->zFilefunc, s->filestream, &sizeExtraField) != UNZ_OK) {
        err = UNZ_ERRNO;
    }
    *poffsetLocalExtrafield = s->curFileInfoInternal.offsetCurfile + SIZEZIPLOCALHEADER + sizeFilename;
    *psizeLocalExtrafield   = static_cast< uInt >(sizeExtraField);

    *piSizeVar += static_cast< uInt >(sizeExtraField);

    return err;
}

/*
  Open for reading data the current file in the zipfile.
  If there is no error and the file is opened, the return value is UNZ_OK.
*/
extern int ZEXPORT UnzOpenCurrentFile3(UnzFile file, int* method, int* level, int raw, const char* password)
{
    int err = UNZ_OK;
    uInt iSizeVar{0};
    Unz64_S* s{nullptr};
    File_In_Zip64_Read_Info_S* pfileInZipReadInfo{nullptr};
    ZPOS64_T offsetLocalExtrafield{0}; /* offset of the local extra field */
    uInt sizeLocalExtrafield{0};       /* size of the local extra field */
#ifndef NOUNCRYPT
    char source[12];
#else
    if (password != nullptr) {
        return UNZ_PARAMERROR;
    }
#endif

    if (file == nullptr) {
        return UNZ_PARAMERROR;
    }
    s = reinterpret_cast< Unz64_S* >(file);
    if (s->currentFileOk == 0) {
        return UNZ_PARAMERROR;
    }

    if (s->pfileInZipRead != nullptr) {
        UnzCloseCurrentFile(file);
    }

    if (Unz64local_CheckCurrentFileCoherencyHeader(s, &iSizeVar, &offsetLocalExtrafield, &sizeLocalExtrafield)
        != UNZ_OK) {
        return UNZ_BADZIPFILE;
    }

    pfileInZipReadInfo = reinterpret_cast< File_In_Zip64_Read_Info_S* >(ALLOC(sizeof(File_In_Zip64_Read_Info_S)));
    if (pfileInZipReadInfo == nullptr) {
        return UNZ_INTERNALERROR;
    }

    pfileInZipReadInfo->readBuffer            = reinterpret_cast< char* >(ALLOC(UNZ_BUFSIZE));
    pfileInZipReadInfo->offsetLocalExtrafield = offsetLocalExtrafield;
    pfileInZipReadInfo->sizeLocalExtrafield   = sizeLocalExtrafield;
    pfileInZipReadInfo->posLocalExtrafield    = 0;
    pfileInZipReadInfo->raw                   = raw;

    if (pfileInZipReadInfo->readBuffer == nullptr) {
        free(pfileInZipReadInfo);
        return UNZ_INTERNALERROR;
    }

    pfileInZipReadInfo->streamInitialised = 0;

    if (method != nullptr) {
        *method = static_cast< int >(s->curFileInfo.compressionMethod);
    }

    constexpr u_int8_t kNUM_6{6};
    constexpr u_int8_t kNUM_9{9};
    if (level != nullptr) {
        *level = kNUM_6;
        switch (s->curFileInfo.flag & kNUM_6) {
            case kNUM_6:
                *level = 1;
                break;
            case 4:
                *level = 2;
                break;
            case 2:
                *level = kNUM_9;
                break;
        }
    }

    if ((s->curFileInfo.compressionMethod != 0) &&
        /* #ifdef HAVE_BZIP2 */
        (s->curFileInfo.compressionMethod != Z_BZIP2ED) &&
        /* #endif */
        (s->curFileInfo.compressionMethod != Z_DEFLATED)) {
        err = UNZ_BADZIPFILE;
    }

    pfileInZipReadInfo->crc32Wait            = s->curFileInfo.crc;
    pfileInZipReadInfo->crc32                = 0;
    pfileInZipReadInfo->totalOut64           = 0;
    pfileInZipReadInfo->compressionMethod    = s->curFileInfo.compressionMethod;
    pfileInZipReadInfo->filestream           = s->filestream;
    pfileInZipReadInfo->zFilefunc            = s->zFilefunc;
    pfileInZipReadInfo->byteBeforeTheZipfile = s->byteBeforeTheZipfile;

    pfileInZipReadInfo->stream.total_out = 0;

    if ((s->curFileInfo.compressionMethod == Z_BZIP2ED) && (raw == 0)) {
#ifdef HAVE_BZIP2
        pfileInZipReadInfo->bstream.bzalloc = (void* (*)(void*, int, int))0;
        pfileInZipReadInfo->bstream.bzfree  = (free_func)0;
        pfileInZipReadInfo->bstream.opaque  = (voidpf)0;
        pfileInZipReadInfo->bstream.state   = (voidpf)0;

        pfileInZipReadInfo->stream.zalloc   = (alloc_func)0;
        pfileInZipReadInfo->stream.zfree    = (free_func)0;
        pfileInZipReadInfo->stream.opaque   = (voidpf)0;
        pfileInZipReadInfo->stream.next_in  = (voidpf)0;
        pfileInZipReadInfo->stream.avail_in = 0;

        err = BZ2_bzDecompressInit(&pfileInZipReadInfo->bstream, 0, 0);
        if (err == Z_OK)
            pfileInZipReadInfo->streamInitialised = Z_BZIP2ED;
        else {
            free(pfileInZipReadInfo->readBuffer);
            free(pfileInZipReadInfo);
            return err;
        }
#else
        pfileInZipReadInfo->raw = 1;
#endif
    } else if ((s->curFileInfo.compressionMethod == Z_DEFLATED) && (raw == 0)) {
        pfileInZipReadInfo->stream.zalloc   = nullptr;
        pfileInZipReadInfo->stream.zfree    = nullptr;
        pfileInZipReadInfo->stream.opaque   = nullptr;
        pfileInZipReadInfo->stream.next_in  = nullptr;
        pfileInZipReadInfo->stream.avail_in = 0;

        err = inflateInit2(&pfileInZipReadInfo->stream, -MAX_WBITS);
        if (err == Z_OK) {
            pfileInZipReadInfo->streamInitialised = Z_DEFLATED;
        } else {
            free(pfileInZipReadInfo->readBuffer);
            free(pfileInZipReadInfo);
            return err;
        }
        /* windowBits is passed < 0 to tell that there is no zlib header.
         * Note that in this case inflate *requires* an extra "dummy" byte
         * after the compressed stream in order to complete decompression and
         * return Z_STREAM_END.
         * In unzip, i don't wait absolutely Z_STREAM_END because I known the
         * size of both compressed and uncompressed data
         */
    }
    pfileInZipReadInfo->restReadCompressed   = s->curFileInfo.compressedSize;
    pfileInZipReadInfo->restReadUncompressed = s->curFileInfo.uncompressedSize;

    pfileInZipReadInfo->posInZipfile = s->curFileInfoInternal.offsetCurfile + SIZEZIPLOCALHEADER + iSizeVar;

    pfileInZipReadInfo->stream.avail_in = static_cast< uInt >(0);

    s->pfileInZipRead = pfileInZipReadInfo;
    s->encrypted      = 0;

#ifndef NOUNCRYPT
    if (password != nullptr) {
        int i;
        s->pcrc_32_tab = get_crc_table();
        init_keys(password, s->keys, s->pcrc_32_tab);
        if (ZSEEK64(s->zFilefunc, s->filestream,
                    s->pfileInZipRead->posInZipfile + s->pfileInZipRead->byteBeforeTheZipfile, SEEK_SET)
            != 0)
            return UNZ_INTERNALERROR;
        if (ZREAD64(s->zFilefunc, s->filestream, source, 12) < 12)
            return UNZ_INTERNALERROR;

        for (i = 0; i < 12; i++)
            zdecode(s->keys, s->pcrc_32_tab, source[i]);

        s->pfileInZipRead->posInZipfile += 12;
        s->encrypted = 1;
    }
#endif

    return UNZ_OK;
}

extern int ZEXPORT UnzOpenCurrentFile(UnzFile file) { return UnzOpenCurrentFile3(file, nullptr, nullptr, 0, nullptr); }

extern int ZEXPORT UnzOpenCurrentFilePassword(UnzFile file, const char* password)
{
    return UnzOpenCurrentFile3(file, nullptr, nullptr, 0, password);
}

extern int ZEXPORT UnzOpenCurrentFile2(UnzFile file, int* method, int* level, int raw)
{
    return UnzOpenCurrentFile3(file, method, level, raw, nullptr);
}

/** Addition for GDAL : START */

extern ZPOS64_T ZEXPORT UnzGetCurrentFileZStreamPos64(UnzFile file)
{
    Unz64_S* s{nullptr};
    File_In_Zip64_Read_Info_S* pfileInZipReadInfo{nullptr};
    s = reinterpret_cast< Unz64_S* >(file);
    if (file == nullptr) {
        return 0;  //UNZ_PARAMERROR;
    }
    pfileInZipReadInfo = s->pfileInZipRead;
    if (pfileInZipReadInfo == nullptr) {
        return 0;  //UNZ_PARAMERROR;
    }
    return pfileInZipReadInfo->posInZipfile + pfileInZipReadInfo->byteBeforeTheZipfile;
}

/** Addition for GDAL : END */

/*
  Read bytes from the current file.
  buf contain buffer where data must be copied
  len the size of buf.

  return the number of byte copied if some bytes are copied
  return 0 if the end of file was reached
  return <0 with error code if there is an error
    (UNZ_ERRNO for IO error, or zLib error for uncompress error)
*/
extern int ZEXPORT UnzReadCurrentFile(UnzFile file, voidp buf, unsigned len)
{
    int err    = UNZ_OK;
    uInt iRead = 0;
    Unz64_S* s{nullptr};
    File_In_Zip64_Read_Info_S* pfileInZipReadInfo{nullptr};
    if (file == nullptr) {
        return UNZ_PARAMERROR;
    }
    s                  = reinterpret_cast< Unz64_S* >(file);
    pfileInZipReadInfo = s->pfileInZipRead;

    if (pfileInZipReadInfo == nullptr) {
        return UNZ_PARAMERROR;
    }

    if (pfileInZipReadInfo->readBuffer == nullptr) {
        return UNZ_END_OF_LIST_OF_FILE;
    }
    if (len == 0) {
        return 0;
    }

    pfileInZipReadInfo->stream.next_out = reinterpret_cast< Bytef* >(buf);

    pfileInZipReadInfo->stream.avail_out = static_cast< uInt >(len);

    if ((len > pfileInZipReadInfo->restReadUncompressed) && (pfileInZipReadInfo->raw == 0)) {
        pfileInZipReadInfo->stream.avail_out = static_cast< uInt >(pfileInZipReadInfo->restReadUncompressed);
    }

    if ((len > pfileInZipReadInfo->restReadCompressed + pfileInZipReadInfo->stream.avail_in)
        && (pfileInZipReadInfo->raw != 0)) {
        pfileInZipReadInfo->stream.avail_out
            = static_cast< uInt >(pfileInZipReadInfo->restReadCompressed) + pfileInZipReadInfo->stream.avail_in;
    }

    while (pfileInZipReadInfo->stream.avail_out > 0) {
        if ((pfileInZipReadInfo->stream.avail_in == 0) && (pfileInZipReadInfo->restReadCompressed > 0)) {
            uInt uReadThis = UNZ_BUFSIZE;
            if (pfileInZipReadInfo->restReadCompressed < uReadThis) {
                uReadThis = static_cast< uInt >(pfileInZipReadInfo->restReadCompressed);
            }
            if (uReadThis == 0) {
                return UNZ_EOF;
            }
            if (ZSEEK64(pfileInZipReadInfo->zFilefunc, pfileInZipReadInfo->filestream,
                        pfileInZipReadInfo->posInZipfile + pfileInZipReadInfo->byteBeforeTheZipfile,
                        ZLIB_FILEFUNC_SEEK_SET)
                != 0) {
                return UNZ_ERRNO;
            }
            if (ZREAD64(pfileInZipReadInfo->zFilefunc, pfileInZipReadInfo->filestream, pfileInZipReadInfo->readBuffer,
                        uReadThis)
                != uReadThis) {
                return UNZ_ERRNO;
            }

#ifndef NOUNCRYPT
            if (s->encrypted) {
                uInt i;
                for (i = 0; i < uReadThis; i++)
                    pfileInZipReadInfo->readBuffer[i]
                        = zdecode(s->keys, s->pcrc_32_tab, pfileInZipReadInfo->readBuffer[i]);
            }
#endif

            pfileInZipReadInfo->posInZipfile += uReadThis;

            pfileInZipReadInfo->restReadCompressed -= uReadThis;

            pfileInZipReadInfo->stream.next_in  = reinterpret_cast< Bytef* >(pfileInZipReadInfo->readBuffer);
            pfileInZipReadInfo->stream.avail_in = uReadThis;
        }

        if ((pfileInZipReadInfo->compressionMethod == 0) || (pfileInZipReadInfo->raw != 0)) {
            uInt uDoCopy{0};
            uInt i{0};

            if ((pfileInZipReadInfo->stream.avail_in == 0) && (pfileInZipReadInfo->restReadCompressed == 0)) {
                return (iRead == 0) ? UNZ_EOF : static_cast< int >(iRead);
            }

            if (pfileInZipReadInfo->stream.avail_out < pfileInZipReadInfo->stream.avail_in) {
                uDoCopy = pfileInZipReadInfo->stream.avail_out;
            } else {
                uDoCopy = pfileInZipReadInfo->stream.avail_in;
            }

            for (i = 0; i < uDoCopy; i++) {
                *(pfileInZipReadInfo->stream.next_out + i) = *(pfileInZipReadInfo->stream.next_in + i);
            }

            pfileInZipReadInfo->totalOut64 = pfileInZipReadInfo->totalOut64 + uDoCopy;

            pfileInZipReadInfo->crc32 = crc32(pfileInZipReadInfo->crc32, pfileInZipReadInfo->stream.next_out, uDoCopy);
            pfileInZipReadInfo->restReadUncompressed -= uDoCopy;
            pfileInZipReadInfo->stream.avail_in -= uDoCopy;
            pfileInZipReadInfo->stream.avail_out -= uDoCopy;
            pfileInZipReadInfo->stream.next_out += uDoCopy;
            pfileInZipReadInfo->stream.next_in += uDoCopy;
            pfileInZipReadInfo->stream.total_out += uDoCopy;
            iRead += uDoCopy;
        } else if (pfileInZipReadInfo->compressionMethod == Z_BZIP2ED) {
#ifdef HAVE_BZIP2
            uLong uTotalOutBefore, uTotalOutAfter;
            const Bytef* bufBefore;
            uLong uOutThis;

            pfileInZipReadInfo->bstream.next_in        = (char*)pfileInZipReadInfo->stream.next_in;
            pfileInZipReadInfo->bstream.avail_in       = pfileInZipReadInfo->stream.avail_in;
            pfileInZipReadInfo->bstream.total_in_lo32  = pfileInZipReadInfo->stream.total_in;
            pfileInZipReadInfo->bstream.total_in_hi32  = 0;
            pfileInZipReadInfo->bstream.next_out       = (char*)pfileInZipReadInfo->stream.next_out;
            pfileInZipReadInfo->bstream.avail_out      = pfileInZipReadInfo->stream.avail_out;
            pfileInZipReadInfo->bstream.total_out_lo32 = pfileInZipReadInfo->stream.total_out;
            pfileInZipReadInfo->bstream.total_out_hi32 = 0;

            uTotalOutBefore = pfileInZipReadInfo->bstream.total_out_lo32;
            bufBefore       = (const Bytef*)pfileInZipReadInfo->bstream.next_out;

            err = BZ2_bzDecompress(&pfileInZipReadInfo->bstream);

            uTotalOutAfter = pfileInZipReadInfo->bstream.total_out_lo32;
            uOutThis       = uTotalOutAfter - uTotalOutBefore;

            pfileInZipReadInfo->totalOut64 = pfileInZipReadInfo->totalOut64 + uOutThis;

            pfileInZipReadInfo->crc32 = crc32(pfileInZipReadInfo->crc32, bufBefore, (uInt)(uOutThis));
            pfileInZipReadInfo->restReadUncompressed -= uOutThis;
            iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);

            pfileInZipReadInfo->stream.next_in   = (Bytef*)pfileInZipReadInfo->bstream.next_in;
            pfileInZipReadInfo->stream.avail_in  = pfileInZipReadInfo->bstream.avail_in;
            pfileInZipReadInfo->stream.total_in  = pfileInZipReadInfo->bstream.total_in_lo32;
            pfileInZipReadInfo->stream.next_out  = (Bytef*)pfileInZipReadInfo->bstream.next_out;
            pfileInZipReadInfo->stream.avail_out = pfileInZipfileInZipReadInfopRead_info->bstream.avail_out;
            pfileInZipReadInfo->stream.total_out = pfileInZipReadInfo->bstream.total_out_lo32;

            if (err == BZ_STREAM_END)
                return (iRead == 0) ? UNZ_EOF : iRead;
            if (err != BZ_OK)
                break;
#endif
        }  // end Z_BZIP2ED
        else {
            ZPOS64_T uTotalOutBefore{0};
            ZPOS64_T uTotalOutAfter{0};
            const Bytef* bufBefore{nullptr};
            ZPOS64_T uOutThis{0};
            int flush = Z_SYNC_FLUSH;

            uTotalOutBefore = pfileInZipReadInfo->stream.total_out;
            bufBefore       = pfileInZipReadInfo->stream.next_out;

            /*
            if ((pfileInZipRead_info->restReadUncompressed ==
                     pfileInZipRead_info->stream.avail_out) &&
                (pfileInZipRead_info->restReadCompressed == 0))
                flush = Z_FINISH;
            */
            err = inflate(&pfileInZipReadInfo->stream, flush);

            if ((err >= 0) && (pfileInZipReadInfo->stream.msg != nullptr)) {
                err = Z_DATA_ERROR;
            }

            uTotalOutAfter = pfileInZipReadInfo->stream.total_out;
            /* Detect overflow, because z_stream.total_out is uLong (32 bits) */
            if (uTotalOutAfter < uTotalOutBefore) {
                constexpr u_int8_t kNUM_32{32};
                uTotalOutAfter += 1LL << kNUM_32; /* Add maximum value of uLong + 1 */
            }
            uOutThis = uTotalOutAfter - uTotalOutBefore;

            pfileInZipReadInfo->totalOut64 = pfileInZipReadInfo->totalOut64 + uOutThis;

            pfileInZipReadInfo->crc32 = crc32(pfileInZipReadInfo->crc32, bufBefore, static_cast< uInt >(uOutThis));

            pfileInZipReadInfo->restReadUncompressed -= uOutThis;

            iRead += static_cast< uInt >(uTotalOutAfter - uTotalOutBefore);

            if (err == Z_STREAM_END) {
                return (iRead == 0) ? UNZ_EOF : static_cast< int >(iRead);
            }
            if (err != Z_OK) {
                break;
            }
        }
    }

    if (err == Z_OK) {
        return static_cast< int >(iRead);
    }
    return err;
}

/*
  Give the current position in uncompressed data
*/
extern z_off_t ZEXPORT Unztell(UnzFile file)
{
    Unz64_S* s{nullptr};
    File_In_Zip64_Read_Info_S* pfileInZipReadInfo{nullptr};
    if (file == nullptr) {
        return UNZ_PARAMERROR;
    }
    s                  = reinterpret_cast< Unz64_S* >(file);
    pfileInZipReadInfo = s->pfileInZipRead;

    if (pfileInZipReadInfo == nullptr) {
        return UNZ_PARAMERROR;
    }

    return static_cast< z_off_t >(pfileInZipReadInfo->stream.total_out);
}

extern ZPOS64_T ZEXPORT Unztell64(UnzFile file)
{
    Unz64_S* s{nullptr};
    File_In_Zip64_Read_Info_S* pfileInZipReadInfo{nullptr};
    if (file == nullptr) {
        return static_cast< ZPOS64_T >(-1);
    }
    s                  = reinterpret_cast< Unz64_S* >(file);
    pfileInZipReadInfo = s->pfileInZipRead;

    if (pfileInZipReadInfo == nullptr) {
        return static_cast< ZPOS64_T >(-1);
    }

    return pfileInZipReadInfo->totalOut64;
}

/*
  return 1 if the end of file was reached, 0 elsewhere
*/
extern int ZEXPORT Unzeof(UnzFile file)
{
    Unz64_S* s{nullptr};
    File_In_Zip64_Read_Info_S* pfileInZipReadInfo{nullptr};
    if (file == nullptr) {
        return UNZ_PARAMERROR;
    }
    s                  = reinterpret_cast< Unz64_S* >(file);
    pfileInZipReadInfo = s->pfileInZipRead;

    if (pfileInZipReadInfo == nullptr) {
        return UNZ_PARAMERROR;
    }

    if (pfileInZipReadInfo->restReadUncompressed == 0) {
        return 1;
    }
    return 0;
}

/*
Read extra field from the current file (opened by unzOpenCurrentFile)
This is the local-header version of the extra field (sometimes, there is
more info in the local-header version than in the central-header)

  if buf==nullptr, it return the size of the local extra field that can be read

  if buf!=nullptr, len is the size of the buffer, the extra header is copied in
    buf.
  the return value is the number of bytes copied in buf, or (if <0)
    the error code
*/
extern int ZEXPORT UnzGetLocalExtrafield(UnzFile file, voidp buf, unsigned len)
{
    Unz64_S* s{nullptr};
    File_In_Zip64_Read_Info_S* pfileInZipReadInfo{nullptr};
    uInt readNow{0};
    ZPOS64_T sizeToRead{0};

    if (file == nullptr) {
        return UNZ_PARAMERROR;
    }
    s                  = reinterpret_cast< Unz64_S* >(file);
    pfileInZipReadInfo = s->pfileInZipRead;

    if (pfileInZipReadInfo == nullptr) {
        return UNZ_PARAMERROR;
    }

    sizeToRead = (pfileInZipReadInfo->sizeLocalExtrafield - pfileInZipReadInfo->posLocalExtrafield);

    if (buf == nullptr) {
        return static_cast< int >(sizeToRead);
    }

    if (len > sizeToRead) {
        readNow = static_cast< uInt >(sizeToRead);
    } else {
        readNow = static_cast< uInt >(len);
    }

    if (readNow == 0) {
        return 0;
    }

    if (ZSEEK64(pfileInZipReadInfo->zFilefunc, pfileInZipReadInfo->filestream,
                pfileInZipReadInfo->offsetLocalExtrafield + pfileInZipReadInfo->posLocalExtrafield,
                ZLIB_FILEFUNC_SEEK_SET)
        != 0) {
        return UNZ_ERRNO;
    }

    if (ZREAD64(pfileInZipReadInfo->zFilefunc, pfileInZipReadInfo->filestream, buf, readNow) != readNow) {
        return UNZ_ERRNO;
    }

    return static_cast< int >(readNow);
}

/*
  Close the file in zip opened with unzOpenCurrentFile
  Return UNZ_CRCERROR if all the file was read but the CRC is not good
*/
extern int ZEXPORT UnzCloseCurrentFile(UnzFile file)
{
    int err = UNZ_OK;

    Unz64_S* s{nullptr};
    File_In_Zip64_Read_Info_S* pfileInZipReadInfo{nullptr};
    if (file == nullptr) {
        return UNZ_PARAMERROR;
    }
    s                  = reinterpret_cast< Unz64_S* >(file);
    pfileInZipReadInfo = s->pfileInZipRead;

    if (pfileInZipReadInfo == nullptr) {
        return UNZ_PARAMERROR;
    }

    if ((pfileInZipReadInfo->restReadUncompressed == 0) && (pfileInZipReadInfo->raw == 0)) {
        if (pfileInZipReadInfo->crc32 != pfileInZipReadInfo->crc32Wait) {
            err = UNZ_CRCERROR;
        }
    }

    free(pfileInZipReadInfo->readBuffer);
    pfileInZipReadInfo->readBuffer = nullptr;
    if (pfileInZipReadInfo->streamInitialised == Z_DEFLATED) {
        inflateEnd(&pfileInZipReadInfo->stream);
    }
#ifdef HAVE_BZIP2
    else if (pfileInZipReadInfo->streamInitialised == Z_BZIP2ED)
        BZ2_bzDecompressEnd(&pfileInZipReadInfo->bstream);
#endif

    pfileInZipReadInfo->streamInitialised = 0;
    free(pfileInZipReadInfo);

    s->pfileInZipRead = nullptr;

    return err;
}

/*
  Get the global comment string of the ZipFile, in the szComment buffer.
  uSizeBuf is the size of the szComment buffer.
  return the number of byte copied or an error code <0
*/
extern int ZEXPORT UnzGetGlobalComment(UnzFile file, char* szComment, uLong uSizeBuf)
{
    Unz64_S* s{nullptr};
    uLong uReadThis{0};
    if (file == nullptr) {
        return UNZ_PARAMERROR;
    }
    s = reinterpret_cast< Unz64_S* >(file);

    uReadThis = uSizeBuf;
    if (uReadThis > s->gi.sizeComment) {
        uReadThis = s->gi.sizeComment;
    }

    if (ZSEEK64(s->zFilefunc, s->filestream, s->centralPos + 22, ZLIB_FILEFUNC_SEEK_SET) != 0) {
        return UNZ_ERRNO;
    }

    if (uReadThis > 0) {
        *szComment = '\0';
        if (ZREAD64(s->zFilefunc, s->filestream, szComment, uReadThis) != uReadThis) {
            return UNZ_ERRNO;
        }
    }

    if ((szComment != nullptr) && (uSizeBuf > s->gi.sizeComment)) {
        *(szComment + s->gi.sizeComment) = '\0';
    }
    return static_cast< int >(uReadThis);
}

/* Additions by RX '2004 */
extern ZPOS64_T ZEXPORT UnzGetOffset64(UnzFile file)
{
    Unz64_S* s{nullptr};

    if (file == nullptr) {
        return 0;  //UNZ_PARAMERROR;
    }
    s = reinterpret_cast< Unz64_S* >(file);
    if (s->currentFileOk == 0) {
        return 0;
    }
    constexpr u_int16_t kNUM_65535{0xffff};
    if (s->gi.numberEntry != 0 && s->gi.numberEntry != kNUM_65535) {
        if (s->numFile == s->gi.numberEntry) {
            return 0;
        }
    }
    return s->posInCentralDir;
}

extern uLong ZEXPORT UnzGetOffset(UnzFile file)
{
    ZPOS64_T offset64{0};

    if (file == nullptr) {
        return 0;  //UNZ_PARAMERROR;
    }
    offset64 = UnzGetOffset64(file);
    return static_cast< uLong >(offset64);
}

extern int ZEXPORT UnzSetOffset64(UnzFile file, ZPOS64_T pos)
{
    Unz64_S* s{nullptr};
    int err{0};

    if (file == nullptr) {
        return UNZ_PARAMERROR;
    }
    s = reinterpret_cast< Unz64_S* >(file);

    s->posInCentralDir = pos;
    s->numFile         = s->gi.numberEntry; /* hack */
    err = Unz64local_GetCurrentFileInfoInternal(file, &s->curFileInfo, &s->curFileInfoInternal, nullptr, 0, nullptr, 0,
                                                nullptr, 0);
    s->currentFileOk = (err == UNZ_OK) ? 1 : 0;
    return err;
}

extern int ZEXPORT UnzSetOffset(UnzFile file, uLong pos) { return UnzSetOffset64(file, pos); }
