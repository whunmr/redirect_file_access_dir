/*
how_to_test:

E:\lab\detours\samples\simple>nmake test
Microsoft (R) Program Maintenance Utility Version 9.00.21022.08
Copyright (C) Microsoft Corporation.  All rights reserved.
-------- Should load simple32.dll dynamically using withdll.exe----------
        ..\..\bin.X86\withdll.exe -d:..\..\bin.X86\simple32.dll -e:map_driver=E_
to_C ..\..\bin.X86\sleep5.exe
_______________|map_driver=E_to_C
withdll.exe: Starting: `..\..\bin.X86\sleep5.exe'
withdll.exe:   with `E:\lab\detours\bin.X86\simple32.dll'
--------->>>|E_to_C|E|C
simple32.dll: Starting.
32.dll: Detoured.
sleep5.exe: Starting.
access CreateFile --------> C:\lab\detours\samples\simple\xxx.txt
The file 'data2' was opened
access DeleteFile --------> C:\lab\detours\samples\simple\ddd.txt
access AddUsersToEncryptedFile --------> C:\lab\detours\samples\simple\xxx.txt
access CopyFile --------> C:\lab\detours\samples\simple\xxx.txt _and_ C:\lab\det
ours\samples\simple\yyy.txt
access CopyFile --------> C:\xxx.txt _and_ C:\lab\detours\samples\simple\yyy.txt

access CopyFile --------> C:\lab\detours\samples\simple\xxx.txt _and_ C:\yyy.txt

sleep5.exe: Done sleeping.
32.dll: Removed (result=0).
*/


#include <stdio.h>
#include <windows.h>
#include "detours.h"

#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"Lz32.lib")

//1. read mapping from ENV
//2. find absolute path of accessed file
//3. inject all file related apis
#define BUFSIZE 4096
__declspec( thread ) TCHAR pathA[BUFSIZE]=TEXT("");
__declspec( thread ) TCHAR pathB[BUFSIZE]=TEXT("");
__declspec( thread ) TCHAR pathC[BUFSIZE]=TEXT("");
__declspec( thread ) WCHAR pathWA[BUFSIZE]=L"";
__declspec( thread ) WCHAR pathWB[BUFSIZE]=L"";
__declspec( thread ) WCHAR pathWC[BUFSIZE]=L"";
__declspec( thread ) TCHAR** lppPart={NULL};
__declspec( thread ) WCHAR** lppPartW={NULL};
__declspec( thread ) TCHAR srcDriver = '0';
__declspec( thread ) TCHAR dstDriver = '0';

////////////////////////////////////////////////////////////////////////////////
DWORD (WINAPI * TrueGetFullPathName)( _In_  LPCTSTR lpFileName,_In_ DWORD nBufferLength
                                    , _Out_ LPTSTR lpBuffer,_Out_ LPTSTR *lpFilePart) = GetFullPathName;

DWORD (WINAPI * TrueGetFullPathNameW)( _In_  LPCWSTR lpFileName,_In_ DWORD nBufferLength
                            , _Out_ LPWSTR lpBuffer,_Out_ LPWSTR *lpFilePart) = GetFullPathNameW;

////////////////////////////////////////////////////////////////////////////////
bool mapDriverToPathW(LPCWSTR lpFileName, LPWSTR buf) {
  if (0 != TrueGetFullPathNameW(lpFileName, BUFSIZE, buf, lppPartW)) {
    if (toupper(buf[0]) == srcDriver) {
      buf[0] = dstDriver;
      return true;
    }
  }
  return false;
}

bool mapDriverToPath(LPCTSTR lpFileName, LPTSTR buf) {
  if (0 != TrueGetFullPathName(lpFileName, BUFSIZE, buf, lppPart)) {
    if (toupper(buf[0]) == srcDriver) {
      buf[0] = dstDriver;
      return true;
    }
  }
  return false;
}

bool mapDriverToPathA(LPCTSTR lpFileName) {
  return mapDriverToPath(lpFileName, (LPTSTR)pathA);
}

bool mapDriverToPathB(LPCTSTR lpFileName) {
  return mapDriverToPath(lpFileName, (LPTSTR)pathB);
}

bool mapDriverToPathC(LPCTSTR lpFileName) {
  return mapDriverToPath(lpFileName, (LPTSTR)pathC);
}

bool mapDriverToPathWA(LPCWSTR lpFileName) {
  return mapDriverToPathW(lpFileName, (LPWSTR)pathWA);
}

bool mapDriverToPathWB(LPCWSTR lpFileName) {
  return mapDriverToPathW(lpFileName, (LPWSTR)pathWB);
}

bool mapDriverToPathWC(LPCWSTR lpFileName) {
  return mapDriverToPathW(lpFileName, (LPWSTR)pathWC);
}

static DWORD (WINAPI * TrueSleepEx)(DWORD dwMilliseconds, BOOL bAlertable) = SleepEx;
DWORD WINAPI injectedSleepEx(DWORD dwMilliseconds, BOOL bAlertable) {
    return TrueSleepEx(dwMilliseconds, bAlertable);
}

#define map1Arg(apiName, apiArg1)                            \
  LPCTSTR arg1 = NULL;                                       \
  mapDriverToPathA(apiArg1) ? arg1 = pathA : arg1 = apiArg1; \
  //printf("access " #apiName  " --------> %s \n", arg1); 

#define map2Args(apiName, apiArg1, apiArg2)                           \
  LPCTSTR arg1 = NULL; LPCTSTR arg2 = NULL;                           \
  mapDriverToPathA(apiArg1) ? arg1 = pathA : arg1 = apiArg1;          \
  mapDriverToPathB(apiArg2) ? arg2 = pathB : arg2 = apiArg2;          \
  //printf("access " #apiName  " --------> %s _and_ %s\n", arg1, arg2); 

#define map3Args(apiName, apiArg1, apiArg2, apiArg3)                  \
  LPCTSTR arg1 = NULL; LPCTSTR arg2 = NULL; LPCTSTR arg3 = NULL;      \
  mapDriverToPathA(apiArg1) ? arg1 = pathA : arg1 = apiArg1;          \
  mapDriverToPathB(apiArg2) ? arg2 = pathB : arg2 = apiArg2;          \
  mapDriverToPathC(apiArg3) ? arg3 = pathC : arg2 = apiArg3;          \
  //printf("access " #apiName  " --------> %s _and_ %s _and_ %s\n", arg1, arg2, arg3); 

#define map1wArg(apiName, wapiArg1)                                \
  LPCWSTR warg1 = NULL;                                            \
  mapDriverToPathWA(wapiArg1) ? warg1 = pathWA : warg1 = wapiArg1; \
  //wprintf(L"access " L#apiName  L" --------> %s \n", warg1); 

#define map2wArgs(apiName, wapiArg1, wapiArg2)                             \
  LPCWSTR warg1 = NULL; LPCWSTR warg2 = NULL;                              \
  mapDriverToPathWA(wapiArg1) ? warg1 = pathWA : warg1 = wapiArg1;         \
  mapDriverToPathWB(wapiArg2) ? warg2 = pathWB : warg2 = wapiArg2;         \
  //wprintf(L"access " L#apiName  L" --------> %s _and_ %s\n", warg1, warg2);

#define map3wArgs(apiName, wapiArg1, wapiArg2, wapiArg3)            \
  LPCTSTR warg1 = NULL; LPCTSTR warg2 = NULL; LPCTSTR warg3 = NULL; \
  mapDriverToPathWA(wapiArg1) ? warg1 = pathWA : warg1 = wapiArg1;  \
  mapDriverToPathWB(wapiArg2) ? warg2 = pathWB : warg2 = wapiArg2;  \
  mapDriverToPathWC(wapiArg3) ? warg3 = pathWC : warg2 = wapiArg3;  \
  //wprintf(L"access " L#apiName  L" --------> %s _and_ %s _and_ %s \n", warg1, warg2, warg3); 

////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI injectedGetFullPathName( _In_  LPCTSTR lpFileName,_In_ DWORD nBufferLength
                                    , _Out_ LPTSTR lpBuffer,_Out_ LPTSTR *lpFilePart) {
  map1Arg(GetFullPathName, lpFileName);
  return TrueGetFullPathName(arg1, nBufferLength, lpBuffer, lpFilePart);
}

static HANDLE (WINAPI * TrueCreateFile)( LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode
                                       , LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition
                                       , DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFile;

HANDLE WINAPI injectedCreateFile( LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode
                                , LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition
                                , DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
  map1Arg(CreateFile, lpFileName);
  return TrueCreateFile( arg1, dwDesiredAccess, dwShareMode, lpSecurityAttributes
                       , dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

////////////////////////////////////////////////////////////////////////////////
DWORD (WINAPI * TrueAddUsersToEncryptedFile)
   (LPCWSTR lpFileName, PENCRYPTION_CERTIFICATE_LIST pUsers) = AddUsersToEncryptedFile;
DWORD WINAPI injectedAddUsersToEncryptedFile(LPCWSTR lpFileName, PENCRYPTION_CERTIFICATE_LIST pUsers) {
  map1wArg(AddUsersToEncryptedFile, lpFileName);
  return TrueAddUsersToEncryptedFile(warg1, pUsers);
}

////////////////////////////////////////////////////////////////////////////////
BOOL (WINAPI * TrueCopyFile)(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists) = CopyFile;
BOOL WINAPI injectedCopyFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists) {
  map2Args(CopyFile, lpExistingFileName, lpNewFileName);
  return TrueCopyFile(arg1, arg2, bFailIfExists);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
BOOL (WINAPI * TrueCreateHardLink)( LPCTSTR lpFileName, LPCTSTR lpExistingFileName
                                  , LPSECURITY_ATTRIBUTES lpSecurityAttributes) = CreateHardLink;
BOOL WINAPI injectedCreateHardLink( LPCTSTR lpFileName, LPCTSTR lpExistingFileName
                          , LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
  map2Args(CreateHardLink, lpFileName, lpExistingFileName);
  return TrueCreateHardLink(arg1, arg2, lpSecurityAttributes);
}

////////////////////////////////////////////////////////////////////////////////
BOOL (WINAPI * TrueDecryptFile)( LPCTSTR lpFileName, DWORD dwReserved) = DecryptFile;
BOOL WINAPI injectedDecryptFile( LPCTSTR lpFileName, DWORD dwReserved) {
  map1Arg(DecryptFile, lpFileName);
  return TrueDecryptFile(arg1, dwReserved);
}

////////////////////////////////////////////////////////////////////////////////
DWORD (WINAPI * TrueDuplicateEncryptionInfoFile)( LPCWSTR SrcFileName, LPCWSTR DstFileName, DWORD dwCreationDistribution
                , DWORD dwAttributes, const LPSECURITY_ATTRIBUTES lpSecurityAttributes) = DuplicateEncryptionInfoFile;
DWORD WINAPI injectedDuplicateEncryptionInfoFile( LPCWSTR SrcFileName, LPCWSTR DstFileName, DWORD dwCreationDistribution
                                                , DWORD dwAttributes, const LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
  map2wArgs(DuplicateEncryptionInfoFile, SrcFileName, DstFileName);
  return TrueDuplicateEncryptionInfoFile(warg1, warg2, dwCreationDistribution, dwAttributes, lpSecurityAttributes);
}

////////////////////////////////////////////////////////////////////////////////
BOOL (WINAPI * TrueEncryptFile)(LPCTSTR lpFileName) = EncryptFile;
BOOL WINAPI injectedEncryptFile(LPCTSTR lpFileName) {
  map1Arg(EncryptFile, lpFileName);
  return TrueEncryptFile(arg1);
}

BOOL (WINAPI * TrueEncryptionDisable)(LPCWSTR DirPath,BOOL Disabl) = EncryptionDisable;
BOOL WINAPI injectedEncryptionDisable(LPCWSTR DirPath,BOOL Disabl) {
  map1wArg(EncryptionDisable, DirPath);
  return TrueEncryptionDisable(warg1, Disabl);
}

BOOL (WINAPI * TrueFileEncryptionStatus)(LPCTSTR lpFileName,LPDWORD lpStatu) = FileEncryptionStatus;
BOOL WINAPI injectedFileEncryptionStatus(LPCTSTR lpFileName,LPDWORD lpStatu) {
  map1Arg(FileEncryptionStatus, lpFileName);
  return TrueFileEncryptionStatus(arg1, lpStatu);
}

HANDLE (WINAPI * TrueFindFirstFile)(LPCTSTR lpFileName,LPWIN32_FIND_DATA lpFindFileDat) = FindFirstFile;
HANDLE WINAPI injectedFindFirstFile(LPCTSTR lpFileName,LPWIN32_FIND_DATA lpFindFileDat) {
   map1Arg(FindFirstFile, lpFileName);
   return TrueFindFirstFile(arg1, lpFindFileDat);
}

HANDLE (WINAPI * TrueFindFirstFileEx)(LPCTSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags) = FindFirstFileEx;
HANDLE WINAPI injectedFindFirstFileEx(LPCTSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags) {
  map1Arg(FindFirstFileEx, lpFileName);
  return TrueFindFirstFileEx(arg1, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
}

BOOL (WINAPI * TrueGetBinaryType)(_In_ LPCTSTR lpApplicationName, _Out_ LPDWORD lpBinaryType) = GetBinaryType;
BOOL WINAPI injectedGetBinaryType(_In_ LPCTSTR lpApplicationName, _Out_ LPDWORD lpBinaryType) {
  map1Arg(GetBinaryType, lpApplicationName);
  return TrueGetBinaryType(arg1, lpBinaryType);
}

DWORD (WINAPI * TrueGetCompressedFileSize)(_In_ LPCTSTR lpFileName, _Out_opt_  LPDWORD lpFileSizeHigh) = GetCompressedFileSize;
DWORD WINAPI injectedGetCompressedFileSize(_In_ LPCTSTR lpFileName, _Out_opt_  LPDWORD lpFileSizeHigh) {
  map1Arg(GetCompressedFileSize, lpFileName);
  return TrueGetCompressedFileSize(arg1, lpFileSizeHigh);
}

INT (WINAPI * TrueGetExpandedName)(  _In_   LPTSTR lpszSource,  _Out_  LPTSTR lpszBuffer) = GetExpandedName;
INT WINAPI injectedGetExpandedName(  _In_   LPTSTR lpszSource,  _Out_  LPTSTR lpszBuffer) {
  map1Arg(GetExpandedName, lpszSource);
  return TrueGetExpandedName((LPTSTR)arg1, lpszBuffer);
}

DWORD (WINAPI * TrueGetFileAttributes)(  _In_  LPCTSTR lpFileName) = GetFileAttributes;
DWORD WINAPI injectedGetFileAttributes(  _In_  LPCTSTR lpFileName) {
  map1Arg(GetFileAttributes, lpFileName);
  return TrueGetFileAttributes(arg1);
}

BOOL (WINAPI * TrueGetFileAttributesEx)(  _In_   LPCTSTR lpFileName,  _In_   GET_FILEEX_INFO_LEVELS fInfoLevelId,  _Out_  LPVOID lpFileInformation) = GetFileAttributesEx;
BOOL WINAPI injectedGetFileAttributesEx(  _In_   LPCTSTR lpFileName,  _In_   GET_FILEEX_INFO_LEVELS fInfoLevelId,  _Out_  LPVOID lpFileInformation) {
  map1Arg(GetFileAttributesEx, lpFileName);
  return TrueGetFileAttributesEx(arg1, fInfoLevelId, lpFileInformation);
}

DWORD (WINAPI * TrueGetLongPathName)(  _In_   LPCTSTR lpszShortPath,  _Out_  LPTSTR lpszLongPath,  _In_   DWORD cchBuffer) = GetLongPathName;
DWORD WINAPI injectedGetLongPathName(  _In_   LPCTSTR lpszShortPath,  _Out_  LPTSTR lpszLongPath,  _In_   DWORD cchBuffer) {
  map1Arg(GetLongPathName, lpszShortPath);
  return TrueGetLongPathName(arg1, lpszLongPath, cchBuffer);
}

DWORD (WINAPI * TrueGetShortPathName)(  _In_   LPCTSTR lpszLongPath,  _Out_  LPTSTR lpszShortPath,  _In_   DWORD cchBuffer) = GetShortPathName;
DWORD WINAPI injectedGetShortPathName(  _In_   LPCTSTR lpszLongPath,  _Out_  LPTSTR lpszShortPath,  _In_   DWORD cchBuffer) {
  map1Arg(GetShortPathName, lpszLongPath);
  return TrueGetShortPathName(arg1, lpszShortPath, cchBuffer);
}

UINT (WINAPI * TrueGetTempFileName)(  _In_   LPCTSTR lpPathName,  _In_   LPCTSTR lpPrefixString,  _In_   UINT uUnique,  _Out_  LPTSTR lpTempFileName) = GetTempFileName;
UINT WINAPI injectedGetTempFileName(  _In_   LPCTSTR lpPathName,  _In_   LPCTSTR lpPrefixString,  _In_   UINT uUnique,  _Out_  LPTSTR lpTempFileName) {
  map1Arg(GetTempFileName, lpPathName);
  return TrueGetTempFileName(arg1, lpPrefixString, uUnique, lpTempFileName);
}

INT (WINAPI * TrueLZOpenFile)(  _In_   LPTSTR lpFileName,  _Out_  LPOFSTRUCT lpReOpenBuf,  _In_   WORD wStyle) = LZOpenFile;
INT WINAPI injectedLZOpenFile(  _In_   LPTSTR lpFileName,  _Out_  LPOFSTRUCT lpReOpenBuf,  _In_   WORD wStyle) {
  map1Arg(LZOpenFile, lpFileName);
  return TrueLZOpenFile((LPTSTR)arg1, lpReOpenBuf, wStyle);
}

BOOL (WINAPI * TrueMoveFile)(  _In_  LPCTSTR lpExistingFileName,  _In_  LPCTSTR lpNewFileName) = MoveFile;
BOOL WINAPI injectedMoveFile(  _In_  LPCTSTR lpExistingFileName,  _In_  LPCTSTR lpNewFileName) {
  map2Args(MoveFile, lpExistingFileName, lpNewFileName);
  return TrueMoveFile(arg1, arg2);
}

BOOL (WINAPI * TrueMoveFileEx)(  _In_      LPCTSTR lpExistingFileName,  _In_opt_  LPCTSTR lpNewFileName,  _In_      DWORD dwFlags) = MoveFileEx;
BOOL WINAPI injectedMoveFileEx(  _In_      LPCTSTR lpExistingFileName,  _In_opt_  LPCTSTR lpNewFileName,  _In_      DWORD dwFlags) {
  map2Args(MoveFileEx, lpExistingFileName, lpNewFileName);
  return TrueMoveFileEx(arg1, arg2, dwFlags);
}

BOOL (WINAPI * TrueMoveFileWithProgress)(  _In_      LPCTSTR lpExistingFileName,  _In_opt_  LPCTSTR lpNewFileName,  _In_opt_  LPPROGRESS_ROUTINE lpProgressRoutine,  _In_opt_  LPVOID lpData,  _In_      DWORD dwFlags) = MoveFileWithProgress;
BOOL WINAPI injectedMoveFileWithProgress(  _In_      LPCTSTR lpExistingFileName,  _In_opt_  LPCTSTR lpNewFileName,  _In_opt_  LPPROGRESS_ROUTINE lpProgressRoutine,  _In_opt_  LPVOID lpData,  _In_      DWORD dwFlags) {
  map2Args(MoveFileWithProgress, lpExistingFileName, lpNewFileName);
  return TrueMoveFileWithProgress(arg1, arg2, lpProgressRoutine, lpData, dwFlags);
}

DWORD (WINAPI * TrueOpenEncryptedFileRaw)(  _In_   LPCTSTR lpFileName,  _In_   ULONG ulFlags,  _Out_  PVOID *pvContext) = OpenEncryptedFileRaw;
DWORD WINAPI injectedOpenEncryptedFileRaw(  _In_   LPCTSTR lpFileName,  _In_   ULONG ulFlags,  _Out_  PVOID *pvContext) {
  map1Arg(OpenEncryptedFileRaw, lpFileName);
  return TrueOpenEncryptedFileRaw(arg1, ulFlags, pvContext);
}

HFILE (WINAPI * TrueOpenFile)(  _In_   LPCSTR lpFileName,  _Out_  LPOFSTRUCT lpReOpenBuff,  _In_   UINT uStyle) = OpenFile;
HFILE WINAPI injectedOpenFile(  _In_   LPCSTR lpFileName,  _Out_  LPOFSTRUCT lpReOpenBuff,  _In_   UINT uStyle) {
  map1Arg(OpenFile, lpFileName);
  return TrueOpenFile(arg1, lpReOpenBuff, uStyle);
}

DWORD (WINAPI * TrueQueryRecoveryAgentsOnEncryptedFile)(  _In_   LPCWSTR lpFileName,  _Out_  PENCRYPTION_CERTIFICATE_HASH_LIST *pRecoveryAgents) = QueryRecoveryAgentsOnEncryptedFile;
DWORD WINAPI injectedQueryRecoveryAgentsOnEncryptedFile(  _In_   LPCWSTR lpFileName,  _Out_  PENCRYPTION_CERTIFICATE_HASH_LIST *pRecoveryAgents) {
  map1wArg(QueryRecoveryAgentsOnEncryptedFile, lpFileName);
  return TrueQueryRecoveryAgentsOnEncryptedFile(warg1, pRecoveryAgents);
}

DWORD (WINAPI * TrueQueryUsersOnEncryptedFile)(  _In_   LPCWSTR lpFileName,  _Out_  PENCRYPTION_CERTIFICATE_HASH_LIST *pUsers) = QueryUsersOnEncryptedFile;
DWORD WINAPI injectedQueryUsersOnEncryptedFile(  _In_   LPCWSTR lpFileName,  _Out_  PENCRYPTION_CERTIFICATE_HASH_LIST *pUsers) {
  map1wArg(QueryUsersOnEncryptedFile, lpFileName);
  return TrueQueryUsersOnEncryptedFile(warg1, pUsers);
}

DWORD (WINAPI * TrueRemoveUsersFromEncryptedFile)(  _In_  LPCWSTR lpFileName,  _In_  PENCRYPTION_CERTIFICATE_HASH_LIST pHashes) = RemoveUsersFromEncryptedFile;
DWORD WINAPI injectedRemoveUsersFromEncryptedFile(  _In_  LPCWSTR lpFileName,  _In_  PENCRYPTION_CERTIFICATE_HASH_LIST pHashes) {
  map1wArg(RemoveUsersFromEncryptedFile, lpFileName);
  return TrueRemoveUsersFromEncryptedFile(warg1, pHashes);
}

BOOL (WINAPI * TrueReplaceFile)(  _In_        LPCTSTR lpReplacedFileName,  _In_        LPCTSTR lpReplacementFileName,  _In_opt_    LPCTSTR lpBackupFileName,  _In_        DWORD dwReplaceFlags, LPVOID lpExclude,  LPVOID lpReserved) = ReplaceFile;
BOOL WINAPI injectedReplaceFile(  _In_        LPCTSTR lpReplacedFileName,  _In_        LPCTSTR lpReplacementFileName,  _In_opt_    LPCTSTR lpBackupFileName,  _In_        DWORD dwReplaceFlags,  LPVOID lpExclude, LPVOID lpReserved) {
  map3Args(ReplaceFile, lpReplacedFileName, lpReplacementFileName, lpBackupFileName);
  return TrueReplaceFile(arg1, arg2, arg3, dwReplaceFlags, lpExclude, lpReserved);
}

DWORD (WINAPI * TrueSearchPath)(  _In_opt_   LPCTSTR lpPath,  _In_       LPCTSTR lpFileName,  _In_opt_   LPCTSTR lpExtension,  _In_       DWORD nBufferLength,  _Out_      LPTSTR lpBuffer,  _Out_opt_  LPTSTR *lpFilePart) = SearchPath;
DWORD WINAPI injectedSearchPath(  _In_opt_   LPCTSTR lpPath,  _In_       LPCTSTR lpFileName,  _In_opt_   LPCTSTR lpExtension,  _In_       DWORD nBufferLength,  _Out_      LPTSTR lpBuffer,  _Out_opt_  LPTSTR *lpFilePart) {
  map1Arg(SearchPath, lpPath);
  return TrueSearchPath(arg1, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);
}

BOOL (WINAPI * TrueSetFileAttributes)(  _In_  LPCTSTR lpFileName,  _In_  DWORD dwFileAttributes) = SetFileAttributes;
BOOL WINAPI injectedSetFileAttributes(  _In_  LPCTSTR lpFileName,  _In_  DWORD dwFileAttributes) {
  map1Arg(SetFileAttributes, lpFileName);
  return TrueSetFileAttributes(arg1, dwFileAttributes);
}

////////////////////////////////////////////////////////////////////////////////
BOOL (WINAPI * TrueDeleteFile)(_In_ LPCTSTR lpFileName) = DeleteFile;
BOOL WINAPI injectedDeleteFile(_In_ LPCTSTR lpFileName) {
  map1Arg(DeleteFile, lpFileName);
  return TrueDeleteFile(arg1);
}

////////////////////////////////////////////////////////////////////////////////
void extractDriverMapConfig() {
  TCHAR buf[4096];
  if (0 != GetEnvironmentVariable("map_driver", (LPTSTR)buf, BUFSIZE)) {
    if (strlen("C_to_D") != strlen(buf)) {
       printf("map_driver should in format X_to_Y. and X should not equal Y. exiting now.\n", buf);
       exit(-1);
    }
    srcDriver = buf[0];
    dstDriver = buf[5];
    if (  toupper(srcDriver) < 'A' || toupper(srcDriver) > 'Z'
       || toupper(dstDriver) < 'A' || toupper(dstDriver) > 'Z'
       || toupper(srcDriver) == toupper(dstDriver)) {
       printf("map_driver should in format X_to_Y. and X should not equal Y. exiting now.\n", buf);
       exit(-2);
    }
    
    //printf("--------->>>|%s|%c|%c\n", buf, srcDriver, dstDriver);
  }
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
    LONG error;
    (void)hinst;
    (void)reserved;

    if (DetourIsHelperProcess()) {
        return TRUE;
    }

    if (dwReason == DLL_PROCESS_ATTACH) {
        extractDriverMapConfig();

        DetourRestoreAfterWith();

        printf("simple" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"" Starting.\n");
        fflush(stdout);

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        //attach
        DetourAttach(&(PVOID&)TrueGetFullPathName, injectedGetFullPathName);
        DetourAttach(&(PVOID&)TrueSleepEx, injectedSleepEx);
        DetourAttach(&(PVOID&)TrueCreateFile, injectedCreateFile);
        DetourAttach(&(PVOID&)TrueAddUsersToEncryptedFile, injectedAddUsersToEncryptedFile);
        DetourAttach(&(PVOID&)TrueCopyFile, injectedCopyFile);
        DetourAttach(&(PVOID&)TrueCreateHardLink, injectedCreateHardLink);
        DetourAttach(&(PVOID&)TrueDecryptFile, injectedDecryptFile);
        DetourAttach(&(PVOID&)TrueDuplicateEncryptionInfoFile, injectedDuplicateEncryptionInfoFile);
        DetourAttach(&(PVOID&)TrueEncryptFile, injectedEncryptFile);
        DetourAttach(&(PVOID&)TrueEncryptionDisable, injectedEncryptionDisable);
        DetourAttach(&(PVOID&)TrueFileEncryptionStatus, injectedFileEncryptionStatus);
        DetourAttach(&(PVOID&)TrueFindFirstFile, injectedFindFirstFile);
        DetourAttach(&(PVOID&)TrueFindFirstFileEx, injectedFindFirstFileEx);
        DetourAttach(&(PVOID&)TrueGetBinaryType, injectedGetBinaryType);
        DetourAttach(&(PVOID&)TrueGetCompressedFileSize, injectedGetCompressedFileSize);
        DetourAttach(&(PVOID&)TrueGetFileAttributes, injectedGetFileAttributes);
        DetourAttach(&(PVOID&)TrueGetFileAttributesEx, injectedGetFileAttributesEx);
        DetourAttach(&(PVOID&)TrueGetLongPathName, injectedGetLongPathName);
        DetourAttach(&(PVOID&)TrueLZOpenFile, injectedLZOpenFile);
        DetourAttach(&(PVOID&)TrueMoveFileEx, injectedMoveFileEx);
        DetourAttach(&(PVOID&)TrueOpenEncryptedFileRaw, injectedOpenEncryptedFileRaw);
        DetourAttach(&(PVOID&)TrueQueryRecoveryAgentsOnEncryptedFile, injectedQueryRecoveryAgentsOnEncryptedFile);
        DetourAttach(&(PVOID&)TrueQueryUsersOnEncryptedFile, injectedQueryUsersOnEncryptedFile);
        DetourAttach(&(PVOID&)TrueRemoveUsersFromEncryptedFile, injectedRemoveUsersFromEncryptedFile);
        DetourAttach(&(PVOID&)TrueSetFileAttributes, injectedSetFileAttributes);
        DetourAttach(&(PVOID&)TrueDeleteFile, injectedDeleteFile);
        
        error = DetourTransactionCommit();

        if (error == NO_ERROR) {
            printf(DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"" Detoured.\n");
        } else {
            printf(DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"" Error detouring: %d\n", error);
        }
    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        //detach
        DetourDetach(&(PVOID&)TrueGetFullPathName, injectedGetFullPathName);
        DetourDetach(&(PVOID&)TrueSleepEx, injectedSleepEx);
        DetourDetach(&(PVOID&)TrueCreateFile, injectedCreateFile);
        DetourDetach(&(PVOID&)TrueAddUsersToEncryptedFile, injectedAddUsersToEncryptedFile);
        DetourDetach(&(PVOID&)TrueCopyFile, injectedCopyFile);
        DetourDetach(&(PVOID&)TrueCreateHardLink, injectedCreateHardLink);
        DetourDetach(&(PVOID&)TrueDecryptFile, injectedDecryptFile);
        DetourDetach(&(PVOID&)TrueDuplicateEncryptionInfoFile, injectedDuplicateEncryptionInfoFile);
        DetourDetach(&(PVOID&)TrueEncryptFile, injectedEncryptFile);
        DetourDetach(&(PVOID&)TrueEncryptionDisable, injectedEncryptionDisable);
        DetourDetach(&(PVOID&)TrueFileEncryptionStatus, injectedFileEncryptionStatus);
        DetourDetach(&(PVOID&)TrueFindFirstFile, injectedFindFirstFile);
        DetourDetach(&(PVOID&)TrueFindFirstFileEx, injectedFindFirstFileEx);
        DetourDetach(&(PVOID&)TrueGetBinaryType, injectedGetBinaryType);
        DetourDetach(&(PVOID&)TrueGetCompressedFileSize, injectedGetCompressedFileSize);
        DetourDetach(&(PVOID&)TrueGetFileAttributes, injectedGetFileAttributes);
        DetourDetach(&(PVOID&)TrueGetFileAttributesEx, injectedGetFileAttributesEx);
        DetourDetach(&(PVOID&)TrueGetLongPathName, injectedGetLongPathName);
        DetourDetach(&(PVOID&)TrueLZOpenFile, injectedLZOpenFile);
        DetourDetach(&(PVOID&)TrueMoveFileEx, injectedMoveFileEx);
        DetourDetach(&(PVOID&)TrueOpenEncryptedFileRaw, injectedOpenEncryptedFileRaw);
        DetourDetach(&(PVOID&)TrueQueryRecoveryAgentsOnEncryptedFile, injectedQueryRecoveryAgentsOnEncryptedFile);
        DetourDetach(&(PVOID&)TrueQueryUsersOnEncryptedFile, injectedQueryUsersOnEncryptedFile);
        DetourDetach(&(PVOID&)TrueRemoveUsersFromEncryptedFile, injectedRemoveUsersFromEncryptedFile);
        DetourDetach(&(PVOID&)TrueSetFileAttributes, injectedSetFileAttributes);
        DetourDetach(&(PVOID&)TrueDeleteFile, injectedDeleteFile);
                
        error = DetourTransactionCommit();

        printf( DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"" Removed (result=%d).\n", error);
        fflush(stdout);
    }
    return TRUE;
}

