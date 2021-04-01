# Microsoft Developer Studio Project File - Name="OpcEnumerate" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=OpcEnumerate - Win32 DebugU
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OpcEnumerate_VS6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OpcEnumerate_VS6.mak" CFG="OpcEnumerate - Win32 DebugU"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OpcEnumerate - Win32 DebugU" (based on "Win32 (x86) Application")
!MESSAGE "OpcEnumerate - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "OpcEnumerate - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "OpcEnumerate - Win32 ReleaseU" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OpcEnumerate - Win32 DebugU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "bin\VS6\Win32\debugU"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\debugU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin\VS6\Win32\debugU"
# PROP Intermediate_Dir "bin\VS6\Win32\debugU"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\src\client\da" /I "..\..\..\src\client" /I "..\..\..\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "_AFXDLL" /D "_UNICODE" /FR /Fp"bin\VS6\Win32\debugU/OpcEnumerate.pch" /Yu"stdafx.h" /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\src\client\da" /I "..\..\..\src\client" /I "..\..\..\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "_AFXDLL" /D "_UNICODE" /FR /Fp"bin\VS6\Win32\debugU/OpcEnumerate.pch" /Yu"stdafx.h" /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_AFXDLL" /d "_DEBUG"
# ADD RSC /l 0x409 /d "_AFXDLL" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6uD.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib" /out:"bin\VS6\Win32\debugU\OpcEnumerate.exe" /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"..\..\..\..\core\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6uD.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib" /out:"bin\VS6\Win32\debugU\OpcEnumerate.exe" /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"..\..\..\..\core\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "OpcEnumerate - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "bin\VS6\Win32\debug"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin\VS6\Win32\debug"
# PROP Intermediate_Dir "bin\VS6\Win32\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\src\client\da" /I "..\..\..\src\client" /I "..\..\..\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Fp"bin\VS6\Win32\debug/OpcEnumerate.pch" /Yu"stdafx.h" /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\src\client\da" /I "..\..\..\src\client" /I "..\..\..\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Fp"bin\VS6\Win32\debug/OpcEnumerate.pch" /Yu"stdafx.h" /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_AFXDLL" /d "_DEBUG"
# ADD RSC /l 0x409 /d "_AFXDLL" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6D.lib /subsystem:windows /debug /machine:IX86 /nodefaultlib:"kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib" /out:"bin\VS6\Win32\debug\OpcEnumerate.exe" /pdbtype:sept /libpath:"..\..\..\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6D.lib /subsystem:windows /debug /machine:IX86 /nodefaultlib:"kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib" /out:"bin\VS6\Win32\debug\OpcEnumerate.exe" /pdbtype:sept /libpath:"..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "OpcEnumerate - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "bin\VS6\Win32\release"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin\VS6\Win32\release"
# PROP Intermediate_Dir "bin\VS6\Win32\release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Ob1 /Gy /I "..\..\..\src\client\da" /I "..\..\..\src\client" /I "..\..\..\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"bin\VS6\Win32\release/OpcEnumerate.pch" /Yu"stdafx.h" /GF /c
# ADD CPP /nologo /MD /W3 /GX /Ob1 /Gy /I "..\..\..\src\client\da" /I "..\..\..\src\client" /I "..\..\..\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"bin\VS6\Win32\release/OpcEnumerate.pch" /Yu"stdafx.h" /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_AFXDLL" /d "NDEBUG"
# ADD RSC /l 0x409 /d "_AFXDLL" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib" /out:"bin\VS6\Win32\release\OpcEnumerate.exe" /pdbtype:sept /libpath:"..\..\..\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib" /out:"bin\VS6\Win32\release\OpcEnumerate.exe" /pdbtype:sept /libpath:"..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "OpcEnumerate - Win32 ReleaseU"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "bin\VS6\Win32\releaseU"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\releaseU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin\VS6\Win32\releaseU"
# PROP Intermediate_Dir "bin\VS6\Win32\releaseU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Ob1 /Gy /I "..\..\..\src\client\da" /I "..\..\..\src\client" /I "..\..\..\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "_AFXDLL" /D "_UNICODE" /Fp"bin\VS6\Win32\releaseU/OpcEnumerate.pch" /Yu"stdafx.h" /GF /c
# ADD CPP /nologo /MD /W3 /GX /Ob1 /Gy /I "..\..\..\src\client\da" /I "..\..\..\src\client" /I "..\..\..\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "_AFXDLL" /D "_UNICODE" /Fp"bin\VS6\Win32\releaseU/OpcEnumerate.pch" /Yu"stdafx.h" /GF /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_AFXDLL" /d "NDEBUG"
# ADD RSC /l 0x409 /d "_AFXDLL" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6u.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /nodefaultlib:"kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib" /out:"bin\VS6\Win32\releaseU\OpcEnumerate.exe" /pdbtype:sept /libpath:"..\..\..\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6u.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /nodefaultlib:"kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib" /out:"bin\VS6\Win32\releaseU\OpcEnumerate.exe" /pdbtype:sept /libpath:"..\..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "OpcEnumerate - Win32 DebugU"
# Name "OpcEnumerate - Win32 Debug"
# Name "OpcEnumerate - Win32 Release"
# Name "OpcEnumerate - Win32 ReleaseU"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\OpcEnumerate.cpp
DEP_CPP_OPCEN=\
	"..\..\..\src\client\Ae\ClientAeSession.h"\
	"..\..\..\src\client\ClientApplication.h"\
	"..\..\..\src\client\ClientEnums.h"\
	"..\..\..\src\client\ClientObjectSpaceElement.h"\
	"..\..\..\src\client\ClientServerBrowser.h"\
	"..\..\..\src\client\Da\ClientDaSession.h"\
	"..\..\..\src\Enums.h"\
	"..\..\..\src\OSCompat.h"\
	"..\..\..\src\Trace.h"\
	"..\..\..\src\ValueQT.h"\
	".\OpcEnumerate.h"\
	".\OpcEnumerateDlg.h"\
	".\StdAfx.h"\
	
NODEP_CPP_OPCEN=\
	"..\..\..\src\OTcommon.h"\
	
# End Source File
# Begin Source File

SOURCE=.\OpcEnumerate.rc
# End Source File
# Begin Source File

SOURCE=.\OpcEnumerateDlg.cpp
DEP_CPP_OPCENU=\
	"..\..\..\src\client\Ae\ClientAeSession.h"\
	"..\..\..\src\client\ClientApplication.h"\
	"..\..\..\src\client\ClientCommon.h"\
	"..\..\..\src\client\ClientEnums.h"\
	"..\..\..\src\client\ClientObjectSpaceElement.h"\
	"..\..\..\src\client\ClientServerBrowser.h"\
	"..\..\..\src\client\Da\ClientDaSession.h"\
	"..\..\..\src\Enums.h"\
	"..\..\..\src\OSCompat.h"\
	"..\..\..\src\Trace.h"\
	"..\..\..\src\ValueQT.h"\
	".\OpcEnumerate.h"\
	".\OpcEnumerateDlg.h"\
	".\StdAfx.h"\
	
NODEP_CPP_OPCENU=\
	"..\..\..\src\OTcommon.h"\
	
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "OpcEnumerate - Win32 DebugU"

# ADD CPP /nologo /GX /Yc"stdafx.h" /GZ

!ELSEIF  "$(CFG)" == "OpcEnumerate - Win32 Debug"

# ADD CPP /nologo /GX /Yc"stdafx.h" /GZ

!ELSEIF  "$(CFG)" == "OpcEnumerate - Win32 Release"

# ADD CPP /nologo /GX /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "OpcEnumerate - Win32 ReleaseU"

# ADD CPP /nologo /GX /Yc"stdafx.h"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\OpcEnumerate.h
# End Source File
# Begin Source File

SOURCE=.\OpcEnumerateDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\OpcEnumerate.ico
# End Source File
# Begin Source File

SOURCE=.\res\OpcEnumerate.rc2
# End Source File
# Begin Source File

SOURCE=.\res\tree.bmp
# End Source File
# End Group
# End Target
# End Project
