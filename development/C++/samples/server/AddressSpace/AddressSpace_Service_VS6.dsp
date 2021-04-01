# Microsoft Developer Studio Project File - Name="AddressSpace_Service" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=AddressSpace_Service - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AddressSpace_Service_VS6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AddressSpace_Service_VS6.mak" CFG="AddressSpace_Service - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AddressSpace_Service - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "AddressSpace_Service - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "AddressSpace_Service - Win32 DebugU" (based on "Win32 (x86) Console Application")
!MESSAGE "AddressSpace_Service - Win32 ReleaseU" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AddressSpace_Service - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "bin\VS6\Win32\debug"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin\VS6\Win32\debug_Service"
# PROP Intermediate_Dir "bin\VS6\Win32\debug_Service"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_MBCS" /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /GZ /c
# SUBTRACT CPP /YX /Yc
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6D.lib /nologo /subsystem:console /debug /machine:I386 /out:"bin\VS6\Win32\debug\AddressSpace_Service.exe" /pdbtype:sept /libpath:"..\..\..\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib TBCVS6D.lib /nologo /subsystem:windows /debug /machine:I386 /out:"bin\VS6\Win32\debug\AddressSpace_Service.exe" /pdbtype:sept /libpath:"..\..\..\lib"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
IntDir=.\bin\VS6\Win32\debug_Service
OutDir=.\bin\VS6\Win32\debug_Service
TargetPath=.\bin\VS6\Win32\debug\AddressSpace_Service.exe
InputPath=.\bin\VS6\Win32\debug\AddressSpace_Service.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AddressSpace_Service - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "bin\VS6\Win32\release"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin\VS6\Win32\release_Service"
# PROP Intermediate_Dir "bin\VS6\Win32\release_Service"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_MBCS" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /c
# SUBTRACT CPP /YX /Yc
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6.lib /nologo /subsystem:console /debug /machine:IX86 /out:"bin\VS6\Win32\release\AddressSpace_Service.exe" /pdbtype:sept /libpath:"..\..\..\lib" /opt:ref /opt:icf
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib TBCVS6.lib /nologo /subsystem:windows /debug /machine:IX86 /out:"bin\VS6\Win32\release\AddressSpace_Service.exe" /pdbtype:sept /libpath:"..\..\..\lib" /opt:ref /opt:icf
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
IntDir=.\bin\VS6\Win32\release_Service
OutDir=.\bin\VS6\Win32\release_Service
TargetPath=.\bin\VS6\Win32\release\AddressSpace_Service.exe
InputPath=.\bin\VS6\Win32\release\AddressSpace_Service.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AddressSpace_Service - Win32 DebugU"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "bin\VS6\Win32\debugU"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\debugU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin\VS6\Win32\debugU_Service"
# PROP Intermediate_Dir "bin\VS6\Win32\debugU_Service"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_WIN32_DCOM" /GZ /c
# SUBTRACT CPP /YX /Yc
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6uD.lib /nologo /subsystem:console /debug /machine:I386 /out:"bin\VS6\Win32\debugU\AddressSpace_Service.exe" /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"..\..\..\..\core\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib TBCVS6uD.lib /nologo /subsystem:windows /debug /machine:I386 /out:"bin\VS6\Win32\debugU\AddressSpace_Service.exe" /pdbtype:sept /libpath:"..\..\..\lib" /libpath:"..\..\..\..\core\lib"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
IntDir=.\bin\VS6\Win32\debugU_Service
OutDir=.\bin\VS6\Win32\debugU_Service
TargetPath=.\bin\VS6\Win32\debugU\AddressSpace_Service.exe
InputPath=.\bin\VS6\Win32\debugU\AddressSpace_Service.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AddressSpace_Service - Win32 ReleaseU"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "bin\VS6\Win32\releaseU"
# PROP BASE Intermediate_Dir "bin\VS6\Win32\releaseU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin\VS6\Win32\releaseU_Service"
# PROP Intermediate_Dir "bin\VS6\Win32\releaseU_Service"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_WINDOWS" /D "_UNICODE" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /I "..\..\..\src" /I "..\..\..\src\server" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_WIN32_DCOM" /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib TBCVS6u.lib /nologo /subsystem:console /debug /machine:IX86 /out:"bin\VS6\Win32\releaseU\AddressSpace_Service.exe" /pdbtype:sept /libpath:"..\..\..\lib" /opt:ref /opt:icf
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib TBCVS6u.lib /nologo /subsystem:windows /debug /machine:IX86 /out:"bin\VS6\Win32\releaseU\AddressSpace_Service.exe" /pdbtype:sept /libpath:"..\..\..\lib" /opt:ref /opt:icf
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
IntDir=.\bin\VS6\Win32\releaseU_Service
OutDir=.\bin\VS6\Win32\releaseU_Service
TargetPath=.\bin\VS6\Win32\releaseU\AddressSpace_Service.exe
InputPath=.\bin\VS6\Win32\releaseU\AddressSpace_Service.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "AddressSpace_Service - Win32 Debug"
# Name "AddressSpace_Service - Win32 Release"
# Name "AddressSpace_Service - Win32 DebugU"
# Name "AddressSpace_Service - Win32 ReleaseU"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;def;odl;idl;hpj;bat;asm;asmx"
# Begin Source File

SOURCE=.\OpcServer.cpp
DEP_CPP_OPCSE=\
	"..\..\..\src\Enums.h"\
	"..\..\..\src\Mutex.h"\
	"..\..\..\src\OSCompat.h"\
	"..\..\..\src\PosixEvents.h"\
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
	".\MyDaAddressSpaceRoot.h"\
	".\MyTransaction.h"\
	".\OpcServer.h"\
	".\stdafx.h"\
	
NODEP_CPP_OPCSE=\
	"..\..\..\src\OTcommon.h"\
	"..\..\..\src\pthread.h"\
	
# End Source File
# Begin Source File

SOURCE=.\Service.cpp
DEP_CPP_SERVI=\
	"..\..\..\src\Enums.h"\
	"..\..\..\src\Mutex.h"\
	"..\..\..\src\OSCompat.h"\
	"..\..\..\src\PosixEvents.h"\
	"..\..\..\src\server\Da\ServerDaAddressSpaceElement.h"\
	"..\..\..\src\server\Da\ServerDaProperty.h"\
	"..\..\..\src\server\Da\ServerDaRequest.h"\
	"..\..\..\src\server\Da\ServerDaTransaction.h"\
	"..\..\..\src\server\ServerAddressSpaceElement.h"\
	"..\..\..\src\server\ServerApplication.h"\
	"..\..\..\src\server\ServerCreator.h"\
	"..\..\..\src\server\ServerEnums.h"\
	"..\..\..\src\server\ServerWebTemplate.h"\
	"..\..\..\src\ValueQT.h"\
	".\MyCreator.h"\
	".\MyDaAddressSpaceElement.h"\
	".\MyDaAddressSpaceRoot.h"\
	".\MyTransaction.h"\
	".\OpcServer.h"\
	".\stdafx.h"\
	
NODEP_CPP_SERVI=\
	"..\..\..\src\OTcommon.h"\
	"..\..\..\src\pthread.h"\
	
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	"..\..\..\src\OSCompat.h"\
	"..\..\..\src\PosixEvents.h"\
	".\stdafx.h"\
	
NODEP_CPP_STDAF=\
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

SOURCE=.\MyDaAddressSpaceRoot.h
# End Source File
# Begin Source File

SOURCE=.\MyTransaction.h
# End Source File
# Begin Source File

SOURCE=.\MyWebTemplate.h
# End Source File
# End Group
# End Target
# End Project
