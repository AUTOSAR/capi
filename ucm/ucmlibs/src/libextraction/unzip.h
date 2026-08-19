/// Modified by iSOFT Infrastructure Software Co., Ltd. on 2024-07-20
///
/* unzip.h -- IO for uncompress .zip files using zlib
   Version 1.1, February 14h, 2010
   part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

         Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

         Modifications of Unzip for Zip64
         Copyright (C) 2007-2008 Even Rouault

         Modifications for Zip64 support on both zip and unzip
         Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )

         For more info read MiniZip_info.txt

         ---------------------------------------------------------------------------------

        Condition of use and distribution are the same than zlib :

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  ---------------------------------------------------------------------------------

        Changes

        See header of unzip64.c

*/

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
/// ===========================================================================================

#ifndef _unz64_H
#define _unz64_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _ZLIB_H
    #include "zlib.h"
#endif

#ifndef _ZLIBIOAPI_H
    #include "ioapi.h"
#endif

#ifdef HAVE_BZIP2
    #include "bzlib.h"
#endif

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00005
/// @trace_id_dd=DD_UCM_00303
/// @needwork = dd
/// @endcode
#define Z_BZIP2ED 12

#if defined(STRICTUNZIP) || defined(STRICTZIPUNZIP)
    /* like the STRICT of WIN32, we define a pointer that cannot be converted
    from (void*) without cast */
    typedef struct TagUnzFile__
    {
        int unused;
    } UnzFile__;
    typedef UnzFile__ *UnzFile;
#else
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
typedef voidp UnzFile;
#endif

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00005
/// @trace_id_dd=DD_UCM_00306
/// @needwork = dd
/// @endcode
#define UNZ_OK (0)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00005
/// @trace_id_dd=DD_UCM_00307
/// @needwork = dd
/// @endcode
#define UNZ_END_OF_LIST_OF_FILE (-100)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00005
/// @trace_id_dd=DD_UCM_00308
/// @needwork = dd
/// @endcode
#define UNZ_ERRNO (Z_ERRNO)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00005
/// @trace_id_dd=DD_UCM_00308
/// @needwork = dd
/// @endcode
#define UNZ_EOF (0)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00005
/// @trace_id_dd=DD_UCM_00310
/// @needwork = dd
/// @endcode
#define UNZ_PARAMERROR (-102)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00005
/// @trace_id_dd=DD_UCM_00311
/// @needwork = dd
/// @endcode
#define UNZ_BADZIPFILE (-103)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00005
/// @trace_id_dd=DD_UCM_00312
/// @needwork = dd
/// @endcode
#define UNZ_INTERNALERROR (-104)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00005
/// @trace_id_dd=DD_UCM_00313
/// @needwork = dd
/// @endcode
#define UNZ_CRCERROR (-105)

    /* Tm_Unz contain date/time info */
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00314
    /// @needwork = dd
    /// @endcode
    typedef struct Tm_Unz_s
    {
        int tmSec;  /* seconds after the minute - [0,59] */
        int tmMin;  /* minutes after the hour - [0,59] */
        int tmHour; /* hours since midnight - [0,23] */
        int tmMday; /* day of the month - [1,31] */
        int tmMon;  /* months since January - [0,11] */
        int tmYear; /* years - [1980..2044] */
    } Tm_Unz;

    /* Unz_Global_Info structure contain global data about the ZIPfile
   These data comes from the end of central dir */
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00315
    /// @needwork = dd
    /// @endcode
    typedef struct Unz_Global_Info64_s
    {
        ZPOS64_T numberEntry; /* total number of entries in
                                     the central dir on this disk */
        uLong sizeComment;    /* size of the global comment of the zipfile */
    } Unz_Global_Info64;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00316
    /// @needwork = dd
    /// @endcode
    typedef struct Unz_Global_Info_s
    {
        uLong numberEntry; /* total number of entries in
                                     the central dir on this disk */
        uLong sizeComment; /* size of the global comment of the zipfile */
    } Unz_Global_Info;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00317
    /// @needwork = dd
    /// @endcode
    /* Unz_File_Info contain information about a file in the zipfile */
    typedef struct Unz_File_Info64_s
    {
        uLong version;             /* version made by                 2 bytes */
        uLong versionNeeded;       /* version needed to extract       2 bytes */
        uLong flag;                /* general purpose bit flag        2 bytes */
        uLong compressionMethod;   /* compression method              2 bytes */
        uLong dosDate;             /* last mod file date in Dos fmt   4 bytes */
        uLong crc;                 /* crc-32                          4 bytes */
        ZPOS64_T compressedSize;   /* compressed size                 8 bytes */
        ZPOS64_T uncompressedSize; /* uncompressed size               8 bytes */
        uLong sizeFilename;        /* filename length                 2 bytes */
        uLong sizeFileExtra;       /* extra field length              2 bytes */
        uLong sizeFileComment;     /* file comment length             2 bytes */

        uLong diskNumStart; /* disk number start               2 bytes */
        uLong internalFa;   /* internal file attributes        2 bytes */
        uLong externalFa;   /* external file attributes        4 bytes */

        Tm_Unz tmuDate;
    } Unz_File_Info64;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00011
    /// @needwork = dd
    /// @endcode
    typedef struct Unz_File_Info_s
    {
        uLong version;           /* version made by                 2 bytes */
        uLong versionNeeded;     /* version needed to extract       2 bytes */
        uLong flag;              /* general purpose bit flag        2 bytes */
        uLong compressionMethod; /* compression method              2 bytes */
                                 /// @code{.isoft}
                                 /// @interface_level=unit
                                 /// @trace_id_ad=AD_UCM_00005
                                 /// @trace_id_dd=DD_UCM_00012
                                 /// @needwork = dd
                                 /// @endcode
        uLong dosDate;           /* last mod file date in Dos fmt   4 bytes */
                                 /// @code{.isoft}
                                 /// @interface_level=unit
                                 /// @trace_id_ad=AD_UCM_00005
                                 /// @trace_id_dd=DD_UCM_00013
                                 /// @needwork = dd
                                 /// @endcode
        uLong crc;               /* crc-32                          4 bytes */
                                 /// @code{.isoft}
                                 /// @interface_level=unit
                                 /// @trace_id_ad=AD_UCM_00005
                                 /// @trace_id_dd=DD_UCM_00014
                                 /// @needwork = dd
                                 /// @endcode
        uLong compressedSize;    /* compressed size                 4 bytes */
        uLong uncompressedSize;  /* uncompressed size               4 bytes */
        uLong sizeFilename;      /* filename length                 2 bytes */
        uLong sizeFileExtra;     /* extra field length              2 bytes */
        uLong sizeFileComment;   /* file comment length             2 bytes */

        uLong diskNumStart; /* disk number start               2 bytes */
        uLong internalFa;   /* internal file attributes        2 bytes */
        uLong externalFa;   /* external file attributes        4 bytes */

        Tm_Unz tmuDate;
    } Unz_File_Info;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00321
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzStringFileNameCompare(const char *fileName1, const char *fileName2, int iCaseSensitivity);
    /*
   Compare two filenames (fileName1,fileName2).
   If iCaseSensitivity = 1, comparison is case sensitive (like strcmp)
   If iCaseSensitivity = 2, comparison is not case sensitive (like strcmpi
                                or strcasecmp)
   If iCaseSensitivity = 0, case sensitivity is default of your operating system
    (like 1 on Unix, 2 on Windows)
*/

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00319
    /// @needwork = dd
    /// @endcode
    extern UnzFile ZEXPORT UnzOpen(const char *path);

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00320
    /// @needwork = dd
    /// @endcode
    extern UnzFile ZEXPORT UnzOpen64(const void *path);
    /*
    Open a Zip file. path contain the full pathname (by example,
     on a Windows XP computer "c:\\zlib\\zlib113.zip" or on an Unix computer
     "zlib/zlib113.zip".
     If the zipfile cannot be opened (file don't exist or in not valid), the
       return value is nullptr.
     Else, the return value is a UnzFile Handle, usable with other function
       of this unzip package.
     the "64" function take a const void* pointer, because the path is just the
       value passed to the Open64_File_Func callback.
     Under Windows, if UNICODE is defined, using Fill_Fopen64_Filefunc, the path
       is a pointer to a wide unicode string (LPCTSTR is LPCWSTR), so const char*
       does not describe the reality
*/

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00015
    /// @needwork = dd
    /// @endcode
    extern UnzFile ZEXPORT UnzOpen2(const char *path, Zlib_Filefunc_Def *pZlibFileFuncDef);
    /*
   Open a Zip file, like unzOpen, but provide a set of file low level API
      for read/write the zip file (see ioapi.h)
*/

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00016
    /// @needwork = dd
    /// @endcode
    extern UnzFile ZEXPORT UnzOpen2_64(const void *path, Zlib_Filefunc64_Def *pZlibFileFuncDef);
    /*
   Open a Zip file, like unz64Open, but provide a set of file low level API
      for read/write the zip file (see ioapi.h)
*/

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00017
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzClose(UnzFile file);
    /*
  Close a ZipFile opened with unzOpen.
  If there is files inside the .Zip opened with unzOpenCurrentFile (see later),
    these files MUST be closed with unzCloseCurrentFile before call unzClose.
  return UNZ_OK if there is no problem. */

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00018
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzGetGlobalInfo(UnzFile file, Unz_Global_Info *pglobalInfo);

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00019
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzGetGlobalInfo64(UnzFile file, Unz_Global_Info64 *pglobalInfo);
    /*
  Write info about the ZipFile in the *pglobalInfo structure.
  No preparation of the structure is needed
  return UNZ_OK if there is no problem. */

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00020
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzGetGlobalComment(UnzFile file, char *szComment, uLong uSizeBuf);
    /*
  Get the global comment string of the ZipFile, in the szComment buffer.
  uSizeBuf is the size of the szComment buffer.
  return the number of byte copied or an error code <0
*/

    /***************************************************************************/
    /* Unzip package allow you browse the directory of the zipfile */

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00021
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzGoToFirstFile(UnzFile file);
    /*
  Set the current file of the zipfile to the first file.
  return UNZ_OK if there is no problem
*/

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00022
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzGoToNextFile(UnzFile file);
    /*
  Set the current file of the zipfile to the next file.
  return UNZ_OK if there is no problem
  return UNZ_END_OF_LIST_OF_FILE if the actual file was the latest.
*/

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00023
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzLocateFile(UnzFile file, const char *szFileName, int iCaseSensitivity);
    /*
  Try locate the file szFileName in the zipfile.
  For the iCaseSensitivity signification, see UnzStringFileNameCompare

  return value :
  UNZ_OK if the file is found. It becomes the current file.
  UNZ_END_OF_LIST_OF_FILE if the file is not found
*/

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00024
    /// @needwork = dd
    /// @endcode
    /* ****************************************** */
    /* Ryan supplied functions */
    /* Unz_File_Info contain information about a file in the zipfile */
    typedef struct Unz_File_Pos_s
    {
        uLong posInZipDirectory; /* offset in zip file directory */
        uLong numOfFile;         /* # of file */
    } Unz_File_Pos;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00025
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzGetFilePos(UnzFile file, Unz_File_Pos *filePos);

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00026
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzGoToFilePos(UnzFile file, Unz_File_Pos *filePos);

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00027
    /// @needwork = dd
    /// @endcode
    typedef struct Unz64_File_Pos_s
    {
        ZPOS64_T posInZipDirectory; /* offset in zip file directory */
        ZPOS64_T numOfFile;         /* # of file */
    } Unz64_File_Pos;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00028
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzGetFilePos64(UnzFile file, Unz64_File_Pos *filePos);

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00029
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzGoToFilePos64(UnzFile file, const Unz64_File_Pos *filePos);

    /* ****************************************** */

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00030
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzGetCurrentFileInfo64(UnzFile file,
                                               Unz_File_Info64 *pfileInfo,
                                               char *szFileName,
                                               uLong fileNameBufferSize,
                                               void *extraField,
                                               uLong extraFieldBufferSize,
                                               char *szComment,
                                               uLong commentBufferSize);

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00031
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzGetCurrentFileInfo(UnzFile file,
                                             Unz_File_Info *pfileInfo,
                                             char *szFileName,
                                             uLong fileNameBufferSize,
                                             void *extraField,
                                             uLong extraFieldBufferSize,
                                             char *szComment,
                                             uLong commentBufferSize);
    /*
  Get Info about the current file
  if pfile_info!=nullptr, the *pfile_info structure will contain some info about
        the current file
  if szFileName!=nullptr, the filename string will be copied in szFileName
            (fileNameBufferSize is the size of the buffer)
  if extraField!=nullptr, the extra field information will be copied in extraField
            (extraFieldBufferSize is the size of the buffer).
            This is the Central-header version of the extra field
  if szComment!=nullptr, the comment string of the file will be copied in szComment
            (commentBufferSize is the size of the buffer)
*/

    /** Addition for GDAL : START */

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00032
    /// @needwork = dd
    /// @endcode
    extern ZPOS64_T ZEXPORT UnzGetCurrentFileZStreamPos64(UnzFile file);

    /** Addition for GDAL : END */

    /***************************************************************************/
    /* for reading the content of the current zipfile, you can open it, read data
   from it, and close it (you can close it before reading all the file)
   */

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00033
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzOpenCurrentFile(UnzFile file);
    /*
  Open for reading data the current file in the zipfile.
  If there is no error, the return value is UNZ_OK.
*/

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00034
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzOpenCurrentFilePassword(UnzFile file, const char *password);
    /*
  Open for reading data the current file in the zipfile.
  password is a crypting password
  If there is no error, the return value is UNZ_OK.
*/

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00035
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzOpenCurrentFile2(UnzFile file, int *method, int *level, int raw);
    /*
  Same than unzOpenCurrentFile, but open for read raw the file (not uncompress)
    if raw==1
  *method will receive method of compression, *level will receive level of
     compression
  note : you can set level parameter as nullptr (if you did not want known level,
         but you CANNOT set method parameter as nullptr
*/

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00036
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzOpenCurrentFile3(UnzFile file, int *method, int *level, int raw, const char *password);
    /*
  Same than unzOpenCurrentFile, but open for read raw the file (not uncompress)
    if raw==1
  *method will receive method of compression, *level will receive level of
     compression
  note : you can set level parameter as nullptr (if you did not want known level,
         but you CANNOT set method parameter as nullptr
*/

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00037
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzCloseCurrentFile(UnzFile file);
    /*
  Close the file in zip opened with unzOpenCurrentFile
  Return UNZ_CRCERROR if all the file was read but the CRC is not good
*/

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00038
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzReadCurrentFile(UnzFile file, voidp buf, unsigned len);
    /*
  Read bytes from the current file (opened by unzOpenCurrentFile)
  buf contain buffer where data must be copied
  len the size of buf.

  return the number of byte copied if some bytes are copied
  return 0 if the end of file was reached
  return <0 with error code if there is an error
    (UNZ_ERRNO for IO error, or zLib error for uncompress error)
*/

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00039
    /// @needwork = dd
    /// @endcode
    extern z_off_t ZEXPORT Unztell(UnzFile file);

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00040
    /// @needwork = dd
    /// @endcode
    extern ZPOS64_T ZEXPORT Unztell64(UnzFile file);
    /*
  Give the current position in uncompressed data
*/

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00041
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT Unzeof(UnzFile file);
    /*
  return 1 if the end of file was reached, 0 elsewhere
*/

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00042
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzGetLocalExtrafield(UnzFile file, voidp buf, unsigned len);
    /*
  Read extra field from the current file (opened by unzOpenCurrentFile)
  This is the local-header version of the extra field (sometimes, there is
    more info in the local-header version than in the central-header)

  if buf==nullptr, it return the size of the local extra field

  if buf!=nullptr, len is the size of the buffer, the extra header is copied in
    buf.
  the return value is the number of bytes copied in buf, or (if <0)
    the error code
*/

    /***************************************************************************/

    /* Get the current file offset */
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00043
    /// @needwork = dd
    /// @endcode
    extern ZPOS64_T ZEXPORT UnzGetOffset64(UnzFile file);
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00044
    /// @needwork = dd
    /// @endcode
    extern uLong ZEXPORT UnzGetOffset(UnzFile file);

    /* Set the current file offset */
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00045
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzSetOffset64(UnzFile file, ZPOS64_T pos);
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00046
    /// @needwork = dd
    /// @endcode
    extern int ZEXPORT UnzSetOffset(UnzFile file, uLong pos);

#ifdef __cplusplus
}
#endif

#endif /* _unz64_H */
