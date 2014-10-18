//////////////////////////////////////////////////////////////////////////////
//
//  Detours Test Program (sleep5.cpp of sleep5.exe)
//
//  Microsoft Research Detours Package, Version 3.0.
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include <windows.h>
#include <stdio.h>

#pragma comment(lib,"Advapi32.lib")

int __cdecl main(int argc, char ** argv)
{
    errno_t err;
    FILE *stream;
    
    if (argc == 2) {
        Sleep(atoi(argv[1]) * 1000);
    }
    else {
        printf("sleep5.exe: Starting.\n");

        err = fopen_s(&stream, "xxx.txt", "w");
        if( err == 0 ) {
           printf( "The file 'data2' was opened\n" );
           err = fclose( stream );
        } else {
           printf( "The file 'data2' was not opened\n" );
        }
        
        Sleep(1000);

        DeleteFile("ddd.txt");
        AddUsersToEncryptedFile(L"xxx.txt", NULL);
        CopyFile("xxx.txt", "yyy.txt", true);
        CopyFile("E:/xxx.txt", "yyy.txt", true);
        CopyFile("xxx.txt", "E:/yyy.txt", true);
        printf("sleep5.exe: Done sleeping.\n");
    }
    return 0;
}
//
///////////////////////////////////////////////////////////////// End of File.
