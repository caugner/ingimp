[Setup]
AppName=ingimp-2.2.17
AppVerName=ingimp version 2.2.17 / 2007.09.08
DefaultDirName={pf}\ingimp-2.2.17
DefaultGroupName=ingimp (Instrumented Gimp)
UninstallDisplayIcon={app}\bin\ingimp.exe
Compression=lzma

[Files]
Source: "C:\ingimp-2.2.17\*"; DestDir: "{app}"; Flags: recursesubdirs

[Icons]
Name: "{group}\ingimp-2.2.17"; Filename: "{app}\bin\ingimp.exe"
