# Microsoft Developer Studio Project File - Name="CTT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=CTT - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ctt_VS6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ctt_VS6.mak" CFG="CTT - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CTT - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "CTT - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "CTT - Win32 DebugU" (based on "Win32 (x86) Console Application")
!MESSAGE "CTT - Win32 ReleaseU" (based on "Win32 (x86) Console Application")
!MESSAGE "CTT - Win32 DebugStatic" (based on "Win32 (x86) Console Application")
!MESSAGE "CTT - Win32 DebugStaticU" (based on "Win32 (x86) Console Application")
!MESSAGE "CTT - Win32 ReleaseStatic" (based on "Win32 (x86) Console Application")
!MESSAGE "CTT - Win32 ReleaseStaticU" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CTT - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "bin\VS6\Win32\debug"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin\VS6\Win32\debug"
# PROP Intermediate_Dir "bin\VS6\Win32\debug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_MBCS" /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_MBCS" /GZ /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6D.lib /nologo /subsystem:console /debug /machine:I386 /out:"bin\VS6\Win32\debug\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib TBCVS6D.lib /nologo /subsystem:console /debug /machine:I386 /out:"bin\VS6\Win32\debug\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
IntDir=.\bin\VS6\Win32\debug
OutDir=.\bin\VS6\Win32\debug
TargetPath=.\bin\VS6\Win32\debug\CTT.exe
InputPath=.\bin\VS6\Win32\debug\CTT.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(TargetPath) /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "CTT - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "bin\VS6\Win32\release"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin\VS6\Win32\release"
# PROP Intermediate_Dir "bin\VS6\Win32\release"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_MBCS" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_MBCS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6.lib /nologo /subsystem:console /debug /machine:IX86 /out:"bin\VS6\Win32\release\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib" /opt:ref /opt:icf
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib TBCVS6.lib /nologo /subsystem:console /debug /machine:IX86 /out:"bin\VS6\Win32\release\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib" /opt:ref /opt:icf
# Begin Custom Build
IntDir=.\bin\VS6\Win32\release
OutDir=.\bin\VS6\Win32\release
TargetPath=.\bin\VS6\Win32\release\CTT.exe
InputPath=.\bin\VS6\Win32\release\CTT.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(TargetPath) /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "CTT - Win32 DebugU"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "bin\VS6\Win32\debugU"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\debugU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin\VS6\Win32\debugU"
# PROP Intermediate_Dir "bin\VS6\Win32\debugU"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /GZ /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6uD.lib /nologo /subsystem:console /debug /machine:I386 /out:"bin\VS6\Win32\debugU\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"..\..\..\..\core\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib TBCVS6uD.lib /nologo /subsystem:console /debug /machine:I386 /out:"bin\VS6\Win32\debugU\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"..\..\..\..\core\lib"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
IntDir=.\bin\VS6\Win32\debugU
OutDir=.\bin\VS6\Win32\debugU
TargetPath=.\bin\VS6\Win32\debugU\CTT.exe
InputPath=.\bin\VS6\Win32\debugU\CTT.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(TargetPath) /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "CTT - Win32 ReleaseU"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "bin\VS6\Win32\releaseU"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\releaseU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin\VS6\Win32\releaseU"
# PROP Intermediate_Dir "bin\VS6\Win32\releaseU"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_UNICODE" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6u.lib /nologo /subsystem:console /debug /machine:IX86 /out:"bin\VS6\Win32\releaseU\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib" /opt:ref /opt:icf
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib TBCVS6u.lib /nologo /subsystem:console /debug /machine:IX86 /out:"bin\VS6\Win32\releaseU\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib" /opt:ref /opt:icf
# Begin Custom Build
IntDir=.\bin\VS6\Win32\releaseU
OutDir=.\bin\VS6\Win32\releaseU
TargetPath=.\bin\VS6\Win32\releaseU\CTT.exe
InputPath=.\bin\VS6\Win32\releaseU\CTT.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(TargetPath) /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "CTT - Win32 DebugStatic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "bin\VS6\Win32\debugStatic"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\debugStatic"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin\VS6\Win32\debugStatic"
# PROP Intermediate_Dir "bin\VS6\Win32\debugStatic"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MTd /W3 /GX /ZI /Od /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "TBC_USING_LIB" /D "_MBCS" /GZ /c
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_MBCS" /GZ /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6libD.lib OTBlibD.lib /nologo /subsystem:console /debug /machine:I386 /out:"bin\VS6\Win32\debugStatic\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"..\..\..\..\core\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib TBCVS6libD.lib OTBlibD.lib /nologo /subsystem:console /debug /machine:I386 /out:"bin\VS6\Win32\debugStatic\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"..\..\..\..\core\lib"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
IntDir=.\bin\VS6\Win32\debugStatic
OutDir=.\bin\VS6\Win32\debugStatic
TargetPath=.\bin\VS6\Win32\debugStatic\CTT.exe
InputPath=.\bin\VS6\Win32\debugStatic\CTT.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(TargetPath) /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "CTT - Win32 DebugStaticU"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "bin\VS6\Win32\debugStaticU"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\debugStaticU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin\VS6\Win32\debugStaticU"
# PROP Intermediate_Dir "bin\VS6\Win32\debugStaticU"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MTd /W3 /GX /ZI /Od /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "TBC_USING_LIB" /D "_UNICODE" /GZ /c
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /GZ /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6libuD.lib OTBuD.lib /nologo /subsystem:console /debug /machine:I386 /out:"bin\VS6\Win32\debugStaticU\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"..\..\..\..\core\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib TBCVS6libuD.lib OTBlibuD.lib /nologo /subsystem:console /debug /machine:I386 /out:"bin\VS6\Win32\debugStaticU\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"..\..\..\..\core\lib"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
IntDir=.\bin\VS6\Win32\debugStaticU
OutDir=.\bin\VS6\Win32\debugStaticU
TargetPath=.\bin\VS6\Win32\debugStaticU\CTT.exe
InputPath=.\bin\VS6\Win32\debugStaticU\CTT.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(TargetPath) /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "CTT - Win32 ReleaseStatic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "bin\VS6\Win32\releaseStatic"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\releaseStatic"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin\VS6\Win32\releaseStatic"
# PROP Intermediate_Dir "bin\VS6\Win32\releaseStatic"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "TBC_USING_LIB" /D "_MBCS" /c
# ADD CPP /nologo /MT /W3 /GX /Zi /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_MBCS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6lib.lib OTBlib.lib /nologo /subsystem:console /debug /machine:IX86 /out:"bin\VS6\Win32\releaseStatic\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"..\..\..\..\core\lib" /opt:ref /opt:icf
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib TBCVS6lib.lib OTBlib.lib /nologo /subsystem:console /debug /machine:IX86 /out:"bin\VS6\Win32\releaseStatic\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"..\..\..\..\core\lib" /opt:ref /opt:icf
# Begin Custom Build
IntDir=.\bin\VS6\Win32\releaseStatic
OutDir=.\bin\VS6\Win32\releaseStatic
TargetPath=.\bin\VS6\Win32\releaseStatic\CTT.exe
InputPath=.\bin\VS6\Win32\releaseStatic\CTT.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(TargetPath) /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "CTT - Win32 ReleaseStaticU"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "bin\VS6\Win32\releaseStaticU"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\releaseStaticU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin\VS6\Win32\releaseStaticU"
# PROP Intermediate_Dir "bin\VS6\Win32\releaseStaticU"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "TBC_USING_LIB" /D "_UNICODE" /c
# ADD CPP /nologo /MT /W3 /GX /Zi /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6libu.lib OTBu.lib /nologo /subsystem:console /debug /machine:IX86 /nodefaultlib:"kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib" /out:"bin\VS6\Win32\releaseStaticU\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"..\..\..\..\core\lib" /opt:ref /opt:icf
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib TBCVS6libu.lib OTBlibu.lib /nologo /subsystem:console /debug /machine:IX86 /out:"bin\VS6\Win32\releaseStaticU\CTT.exe" /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"..\..\..\..\core\lib" /opt:ref /opt:icf
# Begin Custom Build
IntDir=.\bin\VS6\Win32\releaseStaticU
OutDir=.\bin\VS6\Win32\releaseStaticU
TargetPath=.\bin\VS6\Win32\releaseStaticU\CTT.exe
InputPath=.\bin\VS6\Win32\releaseStaticU\CTT.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(TargetPath) /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "CTT - Win32 Debug"
# Name "CTT - Win32 Release"
# Name "CTT - Win32 DebugU"
# Name "CTT - Win32 ReleaseU"
# Name "CTT - Win32 DebugStatic"
# Name "CTT - Win32 DebugStaticU"
# Name "CTT - Win32 ReleaseStatic"
# Name "CTT - Win32 ReleaseStaticU"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;def;odl;idl;hpj;bat;asm;asmx"
# Begin Source File

SOURCE=.\CTT.cpp
DEP_CPP_CTT_C=\
	"..\..\..\src\Enums.h"\
	"..\..\..\src\OSCompat.h"\
	"..\..\..\src\server\Da\ServerDaAddressSpaceElement.h"\
	"..\..\..\src\server\Da\ServerDaProperty.h"\
	"..\..\..\src\server\Da\ServerDaRequest.h"\
	"..\..\..\src\server\Da\ServerDaTransaction.h"\
	"..\..\..\src\server\ServerAddressSpaceElement.h"\
	"..\..\..\src\server\ServerApplication.h"\
	"..\..\..\src\server\ServerCommon.h"\
	"..\..\..\src\server\ServerCreator.h"\
	"..\..\..\src\server\ServerEnums.h"\
	"..\..\..\src\server\ServerWebTemplate.h"\
	"..\..\..\src\ValueQT.h"\
	".\MyCreator.h"\
	".\MyDaAddressSpaceElement.h"\
	".\MyTransaction.h"\
	
NODEP_CPP_CTT_C=\
	"..\..\..\src\OTcommon.h"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc;xsd"
# Begin Source File

SOURCE=.\MyCreator.h
# End Source File
# Begin Source File

SOURCE=.\MyDaAddressSpaceElement.h
# End Source File
# Begin Source File

SOURCE=.\MyTransaction.h
# End Source File
# End Group
# End Target
# End Project
