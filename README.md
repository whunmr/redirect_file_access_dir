redirect_file_access_dir
========================

redirect accessed files' dir from one root dir to another.   such as redirect all file access to E:/ to C:/

usage:
------------
..\..\bin.X86\withdll.exe -d:..\..\bin.X86\simple32.dll -e:map_driver=E_to_C ..\..\bin.X86\sleep5.exe

withdll.exe will load simple32.dll into sleep5.exe, and set environment variable map_driver=E_to_C.

the ENV var map_driver=E_to_C, direct all file access of sleep5.exe from E:/xxx to C:/xxx


for example sleep5.exe code, running from E:\lab\detours\samples\simple\ :
------------
        err = fopen_s(&stream, "xxx.txt", "w");
        if( err == 0 ) {
           err = fclose( stream );
        } 
        DeleteFile("ddd.txt");
        AddUsersToEncryptedFile(L"xxx.txt", NULL);
        CopyFile("xxx.txt", "yyy.txt", true);
        CopyFile("E:/xxx.txt", "yyy.txt", true);
        CopyFile("xxx.txt", "E:/yyy.txt", true);
        printf("sleep5.exe: Done sleeping.\n");

log:
------------
access CreateFile --------> C:\lab\detours\samples\simple\xxx.txt
The file 'data2' was opened
access DeleteFile --------> C:\lab\detours\samples\simple\ddd.txt
access AddUsersToEncryptedFile --------> C:\lab\detours\samples\simple\xxx.txt
access CopyFile --------> C:\lab\detours\samples\simple\xxx.txt _and_ C:\lab\det
ours\samples\simple\yyy.txt
access CopyFile --------> C:\xxx.txt _and_ C:\lab\detours\samples\simple\yyy.txt
access CopyFile --------> C:\lab\detours\samples\simple\xxx.txt _and_ C:\yyy.txt

