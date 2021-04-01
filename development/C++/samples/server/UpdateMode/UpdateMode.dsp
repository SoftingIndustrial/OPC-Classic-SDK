# Microsoft Developer Studio Project File - Name="UpdateMode" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=UpdateMode - Win32 Debug Console
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UpdateMode.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UpdateMode.mak" CFG="UpdateMode - Win32 Debug Console"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UpdateMode - Win32 Debug Console" (based on "Win32 (x86) Application")
!MESSAGE "UpdateMode - Win32 Release Console" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UpdateMode - Win32 Debug Console"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "bin\VS6\Console\debug"
# PROP BASE Intermediate_Dir "bin\VS6\Console\debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin\VS6\Console\debug"
# PROP Intermediate_Dir "bin\VS6\Console\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\C++\src\\" /I "..\..\..\..\C++\src\server\\" /D "WIN32" /D "_WINDOWS" /D "_WIN32_DCOM" /D "STRICT" /D "_DEBUG" /D "_MBCS" /Yu"stdafx.h" /GZ /c
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
# ADD LINK32 TBCVS6D.lib kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:IX86 /out:"bin\VS6\Console\debug\UpdateModeD.exe" /pdbtype:sept /libpath:"..\..\..\..\C++\lib"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Performing Custom Build Step
IntDir=.\bin\VS6\Console\debug
OutDir=.\bin\VS6\Console\debug
TargetPath=.\bin\VS6\Console\debug\UpdateModeD.exe
InputPath=.\bin\VS6\Console\debug\UpdateModeD.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "UpdateMode - Win32 Release Console"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "bin\VS6\Console\release"
# PROP BASE Intermediate_Dir "bin\VS6\Console\release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin\VS6\Console\release"
# PROP Intermediate_Dir "bin\VS6\Console\release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GX /Zi /I "..\..\..\..\C++\src\\" /I "..\..\..\..\C++\src\server\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WIN32_DCOM" /D "CONSOLE" /D "_MBCS" /Yu"stdafx.h" /c
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 TBCVS6.lib kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:IX86 /pdbtype:sept /libpath:"..\..\..\..\C++\lib" /opt:ref /opt:icf
# Begin Custom Build - Performing Custom Build Step
IntDir=.\bin\VS6\Console\release
OutDir=.\bin\VS6\Console\release
TargetPath=.\bin\VS6\Console\release\UpdateMode.exe
InputPath=.\bin\VS6\Console\release\UpdateMode.exe
SOURCE="$(InputPath)"

"$(OutDir)\autoRegister.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(TargetPath)" /RegServer 
	echo OPC Server registered > "$(IntDir)\autoRegister.log" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "UpdateMode - Win32 Debug Console"
# Name "UpdateMode - Win32 Release Console"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;def;odl;idl;hpj;bat;asm;asmx"
# Begin Source File

SOURCE=.\Console.cpp
DEP_CPP_CONSO=\
	".\MyCreator.h"\
	".\MyDaAddressSpaceElement.h"\
	".\MyDaAddressSpaceRoot.h"\
	".\MyRequest.h"\
	".\MyTransaction.h"\
	".\OpcServer.h"\
	".\stdafx.h"\
	
NODEP_CPP_CONSO=\
	".\Da\ServerDaAddressSpaceElement.h"\
	".\Da\ServerDaProperty.h"\
	".\Da\ServerDaRequest.h"\
	".\Da\ServerDaTransaction.h"\
	".\ServerApplication.h"\
	".\ServerCreator.h"\
	
# End Source File
# Begin Source File

SOURCE=.\OpcServer.cpp
DEP_CPP_OPCSE=\
	".\MyCreator.h"\
	".\MyDaAddressSpaceElement.h"\
	".\MyDaAddressSpaceRoot.h"\
	".\MyRequest.h"\
	".\MyTransaction.h"\
	".\OpcServer.h"\
	".\stdafx.h"\
	
NODEP_CPP_OPCSE=\
	".\Da\ServerDaAddressSpaceElement.h"\
	".\Da\ServerDaProperty.h"\
	".\Da\ServerDaRequest.h"\
	".\Da\ServerDaTransaction.h"\
	".\ServerApplication.h"\
	".\ServerCommon.h"\
	".\ServerCreator.h"\
	
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "UpdateMode - Win32 Debug Console"

# ADD CPP /nologo /GX /Yc"stdafx.h" /GZ

!ELSEIF  "$(CFG)" == "UpdateMode - Win32 Release Console"

# ADD CPP /nologo /GX /Yc"stdafx.h"

!ENDIF 

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

SOURCE=.\MyRequest.h
# End Source File
# Begin Source File

SOURCE=.\MyTransaction.h
# End Source File
# Begin Source File

SOURCE=.\OpcServer.h
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

SOURCE=.\icon.ico
# End Source File
# Begin Source File

SOURCE=.\UpdateMode.rc
# End Source File
# End Group
# End Target
# End Project
