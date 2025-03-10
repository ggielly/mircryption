# Microsoft Developer Studio Project File - Name="mircryption" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mircryption - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mircryption.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mircryption.mak" CFG="mircryption - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mircryption - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mircryption - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mircryption - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MIRCRYPTION_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Oa /Og /Os /Oy /Ob2 /Gf /Gy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MIRCRYPTION_EXPORTS" /FR /YX /Gs32700 /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 MSVCRT.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:1.8 /dll /machine:I386 /nodefaultlib /OPT:NOWIN98
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "mircryption - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MIRCRYPTION_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MIRCRYPTION_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 MSVCRT.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:1.8 /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "mircryption - Win32 Release"
# Name "mircryption - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\b64stuff.cpp
# End Source File
# Begin Source File

SOURCE=..\..\BlowfishCbc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\mc_blowfish.cpp
# End Source File
# Begin Source File

SOURCE=..\..\md5c.cpp
# End Source File
# Begin Source File

SOURCE=..\..\md5class.cpp
# End Source File
# Begin Source File

SOURCE=..\..\mircryptionclass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\newblowfish.cpp
# End Source File
# Begin Source File

SOURCE=..\..\oldblowfish.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\mirc_dllshell.cpp
# End Source File
# Begin Source File

SOURCE=..\mircryption.cpp
# End Source File
# Begin Source File

SOURCE=..\simpleexports.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "coreh"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\b64stuff.h
# End Source File
# Begin Source File

SOURCE=..\..\BlowfishCbc.h
# End Source File
# Begin Source File

SOURCE=..\..\mc_blowfish.h
# End Source File
# Begin Source File

SOURCE=..\..\md5c.h
# End Source File
# Begin Source File

SOURCE=..\..\md5class.h
# End Source File
# Begin Source File

SOURCE=..\..\md5global.h
# End Source File
# Begin Source File

SOURCE=..\..\mircryptionclass.h
# End Source File
# Begin Source File

SOURCE=..\..\newblowfish.h
# End Source File
# Begin Source File

SOURCE=..\..\oldblowfish1.h
# End Source File
# Begin Source File

SOURCE=..\..\oldblowfish2.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\mirc_dllshell.h
# End Source File
# Begin Source File

SOURCE=..\mircryption.def
# End Source File
# Begin Source File

SOURCE=..\mircryption.h
# End Source File
# Begin Source File

SOURCE=..\mircryption_resource.h
# End Source File
# Begin Source File

SOURCE=..\simpleexports.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\DialogScript1.rc
# End Source File
# Begin Source File

SOURCE=..\passpic.bmp
# End Source File
# End Group
# End Target
# End Project
