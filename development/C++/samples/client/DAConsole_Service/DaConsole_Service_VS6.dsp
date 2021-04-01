# Microsoft Developer Studio Project File - Name="DaConsole_Service" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DaConsole_Service - Win32 Debug Service
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DaConsole_Service.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DaConsole_Service.mak" CFG="DaConsole_Service - Win32 Debug Service"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DaConsole_Service - Win32 Debug Service" (based on "Win32 (x86) Application")
!MESSAGE "DaConsole_Service - Win32 Release Service" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DaConsole_Service - Win32 Debug Service"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "bin\VS6\Service\debug"
# PROP BASE Intermediate_Dir "bin\VS6\Service\debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin\VS6\Service\debug"
# PROP Intermediate_Dir "bin\VS6\Service\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\..\C++\src" /I "..\..\..\..\C++\src\client" /I "..\..\..\..\C++\src\client\Da" /D "WIN32" /D "_WINDOWS" /D "_WIN32_DCOM" /D "STRICT" /D "_DEBUG" /D "_MBCS" /Yu"stdafx.h" /GZ /c
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 TBCVS6D.lib kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:IX86 /out:"bin\VS6\Service\debug\DaConsole_ServiceD.exe" /pdbtype:sept /libpath:"..\..\..\..\C++\lib"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing Custom Build Step
IntDir=.\bin\VS6\Service\debug
OutDir=.\bin\VS6\Service\debug
TargetPath=.\bin\VS6\Service\debug\DaConsole_ServiceD.exe
InputPath=.\bin\VS6\Service\debug\DaConsole_ServiceD.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DaConsole_Service - Win32 Release Service"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "bin\VS6\Service\release"
# PROP BASE Intermediate_Dir "bin\VS6\Service\release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin\VS6\Service\release"
# PROP Intermediate_Dir "bin\VS6\Service\release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GX /Zi /I "..\..\..\..\C++\src" /I "..\..\..\..\C++\src\client" /I "..\..\..\..\C++\src\client\Da" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WIN32_DCOM" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 TBCVS6.lib kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:IX86 /pdbtype:sept /libpath:"..\..\..\..\C++\lib" /opt:ref /opt:icf
# Begin Custom Build - Performing Custom Build Step
IntDir=.\bin\VS6\Service\release
OutDir=.\bin\VS6\Service\release
TargetPath=.\bin\VS6\Service\release\DaConsole_Service.exe
InputPath=.\bin\VS6\Service\release\DaConsole_Service.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "DaConsole_Service - Win32 Debug Service"
# Name "DaConsole_Service - Win32 Release Service"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;def;odl;idl;hpj;bat;asm;asmx"
# Begin Source File

SOURCE=.\OpcClient.cpp
DEP_CPP_OPCCL=\
	"..\..\..\src\client\Ae\ClientAeSession.h"\
	"..\..\..\src\client\ClientAddressSpaceElement.h"\
	"..\..\..\src\client\ClientApplication.h"\
	"..\..\..\src\client\ClientCommon.h"\
	"..\..\..\src\client\ClientEnums.h"\
	"..\..\..\src\client\ClientObjectSpaceElement.h"\
	"..\..\..\src\client\Da\ClientDaAddressSpaceElement.h"\
	"..\..\..\src\client\Da\ClientDaItem.h"\
	"..\..\..\src\client\Da\ClientDaSession.h"\
	"..\..\..\src\client\Da\ClientDaSubscription.h"\
	"..\..\..\src\Enums.h"\
	"..\..\..\src\OSCompat.h"\
	"..\..\..\src\Trace.h"\
	"..\..\..\src\ValueQT.h"\
	".\MyDaItem.h"\
	".\MyDaSession.h"\
	".\MyDaSubscription.h"\
	".\OpcClient.h"\
	".\stdafx.h"\
	
NODEP_CPP_OPCCL=\
	"..\..\..\src\OTcommon.h"\
	
# End Source File
# Begin Source File

SOURCE=.\Service.cpp
DEP_CPP_SERVI=\
	"..\..\..\src\client\Ae\ClientAeSession.h"\
	"..\..\..\src\client\ClientAddressSpaceElement.h"\
	"..\..\..\src\client\ClientApplication.h"\
	"..\..\..\src\client\ClientEnums.h"\
	"..\..\..\src\client\ClientObjectSpaceElement.h"\
	"..\..\..\src\client\Da\ClientDaAddressSpaceElement.h"\
	"..\..\..\src\client\Da\ClientDaItem.h"\
	"..\..\..\src\client\Da\ClientDaSession.h"\
	"..\..\..\src\client\Da\ClientDaSubscription.h"\
	"..\..\..\src\Enums.h"\
	"..\..\..\src\Mutex.h"\
	"..\..\..\src\OSCompat.h"\
	"..\..\..\src\Trace.h"\
	"..\..\..\src\ValueQT.h"\
	".\MyDaItem.h"\
	".\MyDaSession.h"\
	".\MyDaSubscription.h"\
	".\OpcClient.h"\
	".\stdafx.h"\
	
NODEP_CPP_SERVI=\
	"..\..\..\src\OTcommon.h"\
	"..\..\..\src\pthread.h"\
	
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	"..\..\..\src\OSCompat.h"\
	".\stdafx.h"\
	
NODEP_CPP_STDAF=\
	"..\..\..\src\OTcommon.h"\
	

!IF  "$(CFG)" == "DaConsole_Service - Win32 Debug Service"

# ADD CPP /nologo /GX /Yc"stdafx.h" /GZ

!ELSEIF  "$(CFG)" == "DaConsole_Service - Win32 Release Service"

# ADD CPP /nologo /GX /Yc"stdafx.h"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc;xsd"
# Begin Source File

SOURCE=.\MyDaItem.h
# End Source File
# Begin Source File

SOURCE=.\MyDaSession.h
# End Source File
# Begin Source File

SOURCE=.\MyDaSubscription.h
# End Source File
# Begin Source File

SOURCE=.\OpcClient.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx"
# Begin Source File

SOURCE=.\DaConsole_Service.rc
# End Source File
# Begin Source File

SOURCE=.\icon.ico
# End Source File
# End Group
# End Target
# End Project
