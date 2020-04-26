/*
===========================================================================
    Copyright (C) 2010-2013  Ninja and TheKelm
    Copyright (C) 1999-2005 Id Software, Inc.

    This file is part of CoD4X18-Server source code.

    CoD4X18-Server source code is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    CoD4X18-Server source code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
===========================================================================
*/

#pragma once

#include "qshared.hpp"
#include "cvar.hpp"
#include "contrib/minizip/unzip.h"
/* #define fs_searchpaths (searchpath_t*)*((int*)(0x13f9da28)) */

#define	DEMOGAME			"demota"

// every time a new demo pk3 file is built, this checksum must be updated.
// the easiest way to get it is to just run the game and see what it spits out
#define	DEMO_PAK_CHECKSUM	437558517u

// if this is defined, the executable positively won't work with any paks other
// than the demo pak, even if productid is present.  This is only used for our
// last demo release to prevent the mac and linux users from using the demo
// executable with the production windows pak before the mac/linux products
// hit the shelves a little later
// NOW defined in build files
//#define PRE_RELEASE_TADEMO


// referenced flags
// these are in loop specific order so don't change the order
#define FS_GENERAL_REF	0x01
#define FS_UI_REF		0x02
#define FS_CGAME_REF	0x04
#define FS_QAGAME_REF	0x08

#define MAX_ZPATH	256
#define	MAX_SEARCH_PATHS	4096
#define MAX_FILEHASH_SIZE	1024
//#define MAX_OSPATH 256
#define MAX_FILE_HANDLES 64


typedef struct fileInPack_s {
	unsigned long		pos;		// file info position in zip
	char			*name;		// name of the file
	struct	fileInPack_s*	next;		// next file in the hash
} fileInPack_t;

typedef struct {	//Verified
	char			pakFilename[MAX_OSPATH];	// c:\quake3\baseq3\pak0.pk3
	char			pakBasename[MAX_OSPATH];	// pak0
	char			pakGamename[MAX_OSPATH];	// baseq3
	unzFile			handle;						// handle to zip file +0x300
	int			checksum;					// regular checksum
	int			pure_checksum;				// checksum for pure
	int			unk1;
	int			numfiles;					// number of files in pk3
	int			referenced;					// referenced file flags
	int			hashSize;					// hash table size (power of 2)		+0x318
	fileInPack_t*	*hashTable;					// hash table	+0x31c
	fileInPack_t*	buildBuffer;				// buffer with the filenames etc. +0x320
} pack_t;

typedef struct {	//Verified
	char		path[MAX_OSPATH];		// c:\quake3
	char		gamedir[MAX_OSPATH];	// baseq3
} directory_t;

typedef struct searchpath_s {	//Verified
	struct searchpath_s *next;
	pack_t		*pack;		// only one of pack / dir will be non NULL
	directory_t	*dir;
	qboolean	localized;
	int		val_2;
	int		val_3;
	int		langIndex;
} searchpath_t;

typedef union qfile_gus {
	FILE*		o;
	unzFile	z;
} qfile_gut;

typedef struct qfile_us {
	qfile_gut	file;
	qboolean	unique;
} qfile_ut;

//Added manual buffering as the stdio file buffering has corrupted the written files

typedef struct {
	qfile_ut	handleFiles;
	qboolean	handleSync;
	int		fileSize;
	int		zipFilePos;
	qboolean	zipFile;
	qboolean	streamed;
	char		name[MAX_ZPATH];
	//Not used by filesystem api
	void*		writebuffer;
	int		bufferSize;
	int		bufferPos; //For buffered writes, next write position for logfile buffering
	int		rbufferPos; //next read position
} fileHandleData_t; //0x11C (284) Size


// TTimo - https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=540
// wether we did a reorder on the current search path when joining the server

// last valid game folder used

#ifdef FS_MISSING
FILE*		missingFiles = NULL;
#endif

#define FileWrapper_GetFileSize FS_fplength


extern "C"
{
    extern cvar_t* fs_gameDirVar;
    extern cvar_t* fs_debug;
    extern cvar_t* fs_copyfiles;
    extern cvar_t* fs_cdpath;
    extern cvar_t* fs_basepath;
    extern cvar_t* fs_basegame;
    extern cvar_t* fs_ignoreLocalized;
    extern cvar_t* fs_homepath;
    extern cvar_t* fs_restrict;
    extern cvar_t* fs_usedevdir;
    extern cvar_t* loc_warnings;
    extern cvar_t* loc_warningsAsErrors;

    int CCDECL FS_FOpenTextFileWrite(const char* filename);

void FS_CopyFile(char* FromOSPath,char* ToOSPath);
int FS_Read(void* data, int length, fileHandle_t);
long FS_FOpenFileRead(const char* filename, fileHandle_t* returnhandle);
long FS_FOpenFileReadThread1(const char* filename, fileHandle_t* returnhandle);
long FS_FOpenFileReadThread2(const char* filename, fileHandle_t* returnhandle);
fileHandle_t FS_FOpenFileWrite(const char* filename);
fileHandle_t FS_FOpenFileAppend(const char* filename);
qboolean FS_Initialized();
int FS_filelength( fileHandle_t f );

qboolean FS_HomeRemove( const char *path );
qboolean FS_SV_HomeRemove( const char *path );

qboolean FS_FileExists( const char *file );
qboolean FS_SV_FileExists( const char *file );
qboolean FS_SV_HomeFileExists( const char *file );

char* FS_SV_GetFilepath( const char *file, char* buf, int buflen );

void FS_Rename( const char *from, const char *to );
void FS_SV_Rename( const char *from, const char *to );
qboolean FS_FCloseFile( fileHandle_t f );
qboolean FS_FilenameCompare( const char *s1, const char *s2 );
const char *FS_ShiftedStrStr(const char *string, const char *substring, int shift);
long FS_fplength(FILE *h);

qboolean FS_IsExt(const char *filename, const char *ext, int namelen);

void FS_ConvertPath( char *s );

int FS_PathCmp( const char *s1, const char *s2 );
int	FS_FTell( fileHandle_t f );
void	FS_Flush( fileHandle_t f );
void FS_FreeFile( void *buffer );
void FS_FreeFileKeepBuf( );
void FS_FreeFileOSPath( void *buffer );
int FS_ReadLine( void *buffer, int len, fileHandle_t f );
fileHandle_t FS_SV_FOpenFileWrite( const char *filename );
long FS_SV_FOpenFileRead( const char *filename, fileHandle_t *fp );
fileHandle_t FS_SV_FOpenFileAppend( const char *filename );
int FS_Write( const void *buffer, int len, fileHandle_t h );
int FS_ReadFile( const char *qpath, void **buffer );
int FS_ReadFileOSPath( const char *ospath, void **buffer );
int FS_SV_ReadFile( const char *qpath, void **buffer );
int FS_WriteFile( const char *qpath, const void *buffer, int size );

#define FS_SV_WriteFile FS_SV_HomeWriteFile
int FS_SV_HomeWriteFile( const char *qpath, const void *buffer, int size );
int FS_SV_BaseWriteFile( const char *qpath, const void *buffer, int size );
void FS_BuildOSPathForThread(const char *base, const char *game, const char *qpath, char *fs_path, int fs_thread);

void CCDECL FS_Printf( fileHandle_t h, const char *fmt, ... );
int FS_Seek( fileHandle_t f, long offset, int origin );
const char* CCDECL FS_GetBasepath();
qboolean FS_VerifyPak( const char *pak );
void	FS_ForceFlush( fileHandle_t f );
void CCDECL FS_InitFilesystem(void);
void CCDECL FS_Shutdown(qboolean);
void CCDECL FS_ShutdownIwdPureCheckReferences(void);
void CCDECL FS_ShutdownServerIwdNames(void);
void CCDECL FS_ShutdownServerReferencedIwds(void);
void CCDECL FS_ShutdownServerReferencedFFs(void);
const char* CCDECL FS_LoadedIwdPureChecksums(char* wbuf, int len);
void FS_LoadedPaks(char* outsums, char* outnames, int maxlen);
char* CCDECL FS_GetMapBaseName( const char* mapname );
qboolean FS_CreatePath (char *OSPath);
void FS_SV_HomeCopyFile(char* from, char* to);
void FS_Restart(int checksumfeed);

void CCDECL FS_Startup(const char* game);
void FS_InitCvars();
unsigned Com_BlockChecksumKey32( void *buffer, int length, int key );
void FS_PatchFileHandleData();
int FS_LoadStack();
void FS_CopyCvars();
qboolean FS_SV_BaseRemove( const char *path );

void FS_RemoveOSPath(const char *);
qboolean FS_FileExistsOSPath( const char *ospath );
void FS_RenameOSPath( const char *from_ospath, const char *to_ospath );
qboolean FS_SetPermissionsExec(const char* ospath);
bool DB_GetQPathForZone(const char* zoneName, int maxlen, char* opath);
int     FS_FOpenFileByMode( const char *qpath, fileHandle_t *f, fsMode_t mode );
void FS_ReferencedPaks(char *outChkSums, char *outPathNames, int maxlen);
void FS_AddIwdPureCheckReference(searchpath_t *search);
void FS_StripSeperators(char* path);
void FS_StripTrailingSeperator( char *path );
void FS_ReplaceSeparators( char *path );
int FS_CalculateChecksumForFile(const char* filename, int *crc32);
int FS_WriteChecksumInfo(const char* filename, byte* data, size_t maxsize);
int FS_WriteFileOSPath( char *ospath, const void *buffer, int size );
void FS_ClearPakReferences( int flags );
int FS_filelengthForOSPath( const char* ospath );
FILE *CCDECL FS_FileOpenReadText(const char *filename);
int CCDECL FS_FileGetFileSize(FILE *file);
unsigned int CCDECL FS_FileRead(void *ptr, unsigned int len, FILE *stream);

void CCDECL FS_FreeFileList(const char **list);
const char **CCDECL FS_ListFiles(const char *path, const char *extension, int behavior, int *numfiles);
int CCDECL FS_GetModList(char *listbuf, int bufsize);
void CCDECL FS_FileClose(FILE *stream);
qboolean SEH_GetLanguageIndexForName(const char* language, int *langindex);
const char* SEH_GetLanguageName(unsigned int langindex);
int SEH_GetCurrentLanguage( );
void FS_CloseLogFile(fileHandle_t f);
fileHandle_t FS_OpenLogfile(const char* name, char mode);
void FS_WriteLogFlush(fileHandle_t f);
int FS_WriteLog( const void *buffer, int ilen, fileHandle_t h );
void FS_AddGameDirectory_Single(const char *path, const char *dir_nolocal, qboolean localized, int index);

    int CCDECL FS_OpenFileOverwrite(const char *qpath);
    long FS_HashFileName( const char *fname, int hashSize );
    void CCDECL FS_AddUserMapDirIWDs(const char *pszGameFolder);
    int CCDECL FS_GetFileList(const char *path, const char *extension, int behavior, char *listbuf, int bufsize);
    qboolean CCDECL FS_LanguageHasAssets(int iLanguage);
    void DB_BuildOSPath(const char* filename, int ffdir, int len, char* buff);
} // extern "C"

void DB_BuildQPath(const char* filename, int ffdir, int len, char* buff);
