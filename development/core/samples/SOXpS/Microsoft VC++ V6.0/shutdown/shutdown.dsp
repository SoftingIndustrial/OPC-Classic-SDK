# Microsoft Developer Studio Project File - Name="shutdown" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=shutdown - Win32 Debug Service
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "shutdown.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "shutdown.mak" CFG="shutdown - Win32 Debug Service"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "shutdown - Win32 Release OutProc" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "shutdown - Win32 Debug OutProc" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "shutdown - Win32 Release Service" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "shutdown - Win32 Debug Service" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "shutdown - Win32 Release OutProc"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseOutProc"
# PROP BASE Intermediate_Dir "ReleaseOutProc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseOutProc"
# PROP Intermediate_Dir "ReleaseOutProc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "f:\OPCToolbox\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\..\src\include" /I "..\..\..\..\src\SOXpS" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib advapi32.lib ole32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib advapi32.lib ole32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Copy XPress OPC Server Tool and Register OPC Server
IntDir=.\ReleaseOutProc
TargetDir=.\ReleaseOutProc
InputPath=.\ReleaseOutProc\shutdown.dll
SOURCE="$(InputPath)"

"$(IntDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	set SO_XPS_SERVER="..\..\..\..\bin\SOXpSSrvDemo.exe" 
	if exist "..\..\..\..\bin\SOXpSSrv.exe" set SO_XPS_SERVER="..\..\..\..\bin\SOXpSSrv.exe" 
	if not exist "$(TargetDir)\shutdown.exe" copy %SO_XPS_SERVER% "$(TargetDir)\shutdown.exe" 
	"$(TargetDir)\shutdown.exe" /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "shutdown - Win32 Debug OutProc"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugOutProc"
# PROP BASE Intermediate_Dir "DebugOutProc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugOutProc"
# PROP Intermediate_Dir "DebugOutProc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "f:\OPCToolbox\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\src\include" /I "..\..\..\..\src\SOXpS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib advapi32.lib ole32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib advapi32.lib ole32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# Begin Custom Build - Copy XPress OPC Server Tool and Register OPC Server
IntDir=.\DebugOutProc
TargetDir=.\DebugOutProc
InputPath=.\DebugOutProc\shutdown.dll
SOURCE="$(InputPath)"

"$(IntDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	set SO_XPS_SERVER="..\..\..\..\bin\SOXpSSrvDemo.exe" 
	if exist "..\..\..\..\bin\SOXpSSrv.exe" set SO_XPS_SERVER="..\..\..\..\bin\SOXpSSrv.exe" 
	if not exist "$(TargetDir)\shutdown.exe" copy %SO_XPS_SERVER% "$(TargetDir)\shutdown.exe" 
	"$(TargetDir)\shutdown.exe" /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "shutdown - Win32 Release Service"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ReleaseService"
# PROP BASE Intermediate_Dir "ReleaseService"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ReleaseService"
# PROP Intermediate_Dir "ReleaseService"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "f:\OPCToolbox\include" /D "WIN32" /D "_WINDOWS" /D "_WIN32_DCOM" /D "NDEBUG" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\..\src\include" /I "..\..\..\..\src\SOXpS" /D "WIN32" /D "_WINDOWS" /D "_WIN32_DCOM" /D "NDEBUG" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib advapi32.lib ole32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib advapi32.lib ole32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Copy XPress OPC Server Tool and Register OPC Server
IntDir=.\ReleaseService
TargetDir=.\ReleaseService
InputPath=.\ReleaseService\shutdown.dll
SOURCE="$(InputPath)"

"$(IntDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	set SO_XPS_SERVER="..\..\..\..\bin\SOXpSSrvDemo.exe" 
	if exist "..\..\..\..\bin\SOXpSSvc.exe" set SO_XPS_SERVER="..\..\..\..\bin\SOXpSSvc.exe" 
	if not exist "$(TargetDir)\shutdown.exe" copy %SO_XPS_SERVER% "$(TargetDir)\shutdown.exe" 
	"$(TargetDir)\shutdown.exe" /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "shutdown - Win32 Debug Service"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugService"
# PROP BASE Intermediate_Dir "DebugService"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugService"
# PROP Intermediate_Dir "DebugService"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "f:\OPCToolbox\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WIN32_DCOM" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\src\include" /I "..\..\..\..\src\SOXpS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WIN32_DCOM" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib advapi32.lib ole32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib advapi32.lib ole32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# Begin Custom Build - Copy XPress OPC Server Tool and Register OPC Server
IntDir=.\DebugService
TargetDir=.\DebugService
InputPath=.\DebugService\shutdown.dll
SOURCE="$(InputPath)"

"$(IntDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	set SO_XPS_SERVER="..\..\..\..\bin\SOXpSSrvDemo.exe" 
	if exist "..\..\..\..\bin\SOXpSSvc.exe" set SO_XPS_SERVER="..\..\..\..\bin\SOXpSSvc.exe" 
	if not exist "$(TargetDir)\shutdown.exe" copy %SO_XPS_SERVER% "$(TargetDir)\shutdown.exe" 
	"$(TargetDir)\shutdown.exe" /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "shutdown - Win32 Release OutProc"
# Name "shutdown - Win32 Debug OutProc"
# Name "shutdown - Win32 Release Service"
# Name "shutdown - Win32 Debug Service"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Shutdown.cpp
# End Source File
# Begin Source File

SOURCE=.\Shutdown.def
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon.ico
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Shutdown.rc
# End Source File
# End Group
# End Target
# End Project
