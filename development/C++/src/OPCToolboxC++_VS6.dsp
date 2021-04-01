# Microsoft Developer Studio Project File - Name="OPCToolboxC++" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=OPCToolboxC++ - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OPCToolboxC++_VS6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OPCToolboxC++_VS6.mak" CFG="OPCToolboxC++ - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OPCToolboxC++ - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OPCToolboxC++ - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OPCToolboxC++ - Win32 DebugU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OPCToolboxC++ - Win32 ReleaseU" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OPCToolboxC++ - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\bin\VS6\Win32\release"
# PROP BASE Intermediate_Dir "..\bin\VS6\Win32\release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\VS6\Win32\release"
# PROP Intermediate_Dir "..\bin\VS6\Win32\release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /Od /Gy /I "..\..\core\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /c
# ADD CPP /nologo /Zp4 /MD /W3 /GR /GX /Od /Gy /I "." /I "..\..\core\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x417 /i "." /i "..\common" /i "..\SOSrv" /d "NDEBUG"
# ADD RSC /l 0x417 /i "." /i "..\common" /i "..\SOSrv" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib OTB.lib /nologo /subsystem:windows /dll /incremental:yes /machine:IX86 /out:"..\bin\VS6\Win32\release\TBCVS6.dll" /pdbtype:sept /libpath:"..\..\core\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib OTB.lib /nologo /subsystem:windows /dll /incremental:yes /machine:IX86 /out:"..\bin\VS6\Win32\release\TBCVS6.dll" /pdbtype:sept /libpath:"..\..\core\lib"
# Begin Special Build Tool
TargetDir=\OPCToolbox\OPCToolboxV4.2\dev\C++\bin\VS6\Win32\release
TargetPath=\OPCToolbox\OPCToolboxV4.2\dev\C++\bin\VS6\Win32\release\TBCVS6.dll
TargetName=TBCVS6
SOURCE="$(InputPath)"
PostBuild_Cmds=copy "$(TargetPath)" ..\..\bin	copy "$(TargetDir)\$(TargetName).lib" ..\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "OPCToolboxC++ - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\bin\VS6\Win32\debug"
# PROP BASE Intermediate_Dir "..\bin\VS6\Win32\debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin\VS6\Win32\debug"
# PROP Intermediate_Dir "..\bin\VS6\Win32\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "..\..\core\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /TP /c
# ADD CPP /nologo /Zp4 /MDd /W3 /GR /GX /ZI /Od /I "." /I "..\..\core\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /TP /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x417 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib OTBD.lib /nologo /subsystem:windows /dll /pdb:"..\bin\VS6\Win32\debug\TBCVS6D.pdb" /debug /machine:IX86 /out:"..\bin\VS6\Win32\debug\TBCVS6D.dll" /pdbtype:sept /libpath:"..\..\core\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib OTB.lib /nologo /subsystem:windows /dll /pdb:"..\bin\VS6\Win32\debug\TBCVS6D.pdb" /debug /machine:IX86 /out:"..\bin\VS6\Win32\debug\TBCVS6D.dll" /pdbtype:sept /libpath:"..\..\core\lib"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetDir=\OPCToolbox\OPCToolboxV4.2\dev\C++\bin\VS6\Win32\debug
TargetPath=\OPCToolbox\OPCToolboxV4.2\dev\C++\bin\VS6\Win32\debug\TBCVS6D.dll
TargetName=TBCVS6D
SOURCE="$(InputPath)"
PostBuild_Cmds=copy "$(TargetPath)" ..\..\bin	copy "$(TargetDir)\$(TargetName).lib" ..\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "OPCToolboxC++ - Win32 DebugU"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\bin\VS6\Win32\debugU"
# PROP BASE Intermediate_Dir "..\bin\VS6\Win32\debugU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin\VS6\Win32\debugU"
# PROP Intermediate_Dir "..\bin\VS6\Win32\debugU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "..\..\core\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /FR /TP /c
# ADD CPP /nologo /Zp4 /MDd /W3 /GR /GX /ZI /Od /I "." /I "..\..\core\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /FR /TP /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x417 /i "." /i "..\common" /i "..\SOSrv" /d "_DEBUG"
# ADD RSC /l 0x417 /i "." /i "..\common" /i "..\SOSrv" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib OTBuD.lib /nologo /subsystem:windows /dll /pdb:"..\bin\VS6\Win32\debugU\TBCVS6uD.pdb" /debug /machine:IX86 /out:"..\bin\VS6\Win32\debugU\TBCVS6uD.dll" /pdbtype:sept /libpath:"..\..\core\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib OTBu.lib /nologo /subsystem:windows /dll /pdb:"..\bin\VS6\Win32\debugU\TBCVS6uD.pdb" /debug /machine:IX86 /out:"..\bin\VS6\Win32\debugU\TBCVS6uD.dll" /pdbtype:sept /libpath:"..\..\core\lib"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetDir=\OPCToolbox\OPCToolboxV4.2\dev\C++\bin\VS6\Win32\debugU
TargetPath=\OPCToolbox\OPCToolboxV4.2\dev\C++\bin\VS6\Win32\debugU\TBCVS6uD.dll
TargetName=TBCVS6uD
SOURCE="$(InputPath)"
PostBuild_Cmds=copy "$(TargetPath)" ..\..\bin	copy "$(TargetDir)\$(TargetName).lib" ..\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "OPCToolboxC++ - Win32 ReleaseU"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\bin\VS6\releaseU"
# PROP BASE Intermediate_Dir "..\bin\VS6\Win32\releaseU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\VS6\Win32\releaseU"
# PROP Intermediate_Dir "..\bin\VS6\Win32\releaseU"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /Od /Gy /I "..\..\core\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /FR /c
# ADD CPP /nologo /Zp4 /MD /W3 /GR /GX /Zi /Od /Gy /I "." /I "..\..\core\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /FR /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x417 /i "." /i "..\common" /i "..\SOSrv" /d "NDEBUG"
# ADD RSC /l 0x417 /i "." /i "..\common" /i "..\SOSrv" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib OTBu.lib /nologo /subsystem:windows /dll /pdb:"..\bin\VS6\Win32\releaseU\TBCVS6u.pdb" /debug /incremental:yes /machine:IX86 /out:"..\bin\VS6\Win32\releaseU\TBCVS6u.dll" /implib:"..\bin\VS6\Win32\releaseU/TBCVS6u.lib" /pdbtype:sept /libpath:"..\..\core\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib OTBu.lib /nologo /subsystem:windows /dll /pdb:"..\bin\VS6\Win32\releaseU\TBCVS6u.pdb" /debug /incremental:yes /machine:IX86 /out:"..\bin\VS6\Win32\releaseU\TBCVS6u.dll" /pdbtype:sept /libpath:"..\..\core\lib"
# Begin Special Build Tool
TargetDir=\OPCToolbox\OPCToolboxV4.2\dev\C++\bin\VS6\Win32\releaseU
TargetPath=\OPCToolbox\OPCToolboxV4.2\dev\C++\bin\VS6\Win32\releaseU\TBCVS6u.dll
TargetName=TBCVS6u
SOURCE="$(InputPath)"
PostBuild_Cmds=copy "$(TargetPath)" ..\..\bin	copy "$(TargetDir)\$(TargetName).lib" ..\lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "OPCToolboxC++ - Win32 Release"
# Name "OPCToolboxC++ - Win32 Debug"
# Name "OPCToolboxC++ - Win32 DebugU"
# Name "OPCToolboxC++ - Win32 ReleaseU"
# Begin Group "OPCToolboxC++"

# PROP Default_Filter ""
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Enums.h
# End Source File
# Begin Source File

SOURCE=.\Mutex.cpp
DEP_CPP_MUTEX=\
	"..\..\core\include\OTcommon.h"\
	".\Mutex.h"\
	".\OSCompat.h"\
	
NODEP_CPP_MUTEX=\
	".\pthread.h"\
	
# End Source File
# Begin Source File

SOURCE=.\Mutex.h
# End Source File
# Begin Source File

SOURCE=.\OSCompat.h
# End Source File
# Begin Source File

SOURCE=.\SOModule.h
# End Source File
# Begin Source File

SOURCE=..\..\core\common\SOVersion.h
# End Source File
# Begin Source File

SOURCE=.\Trace.cpp
DEP_CPP_TRACE=\
	"..\..\core\include\OTcommon.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\Trace.h"\
	
# End Source File
# Begin Source File

SOURCE=.\Trace.h
# End Source File
# Begin Source File

SOURCE=.\ValueQT.cpp
DEP_CPP_VALUE=\
	"..\..\core\include\OTcommon.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\ValueQT.h
# End Source File
# Begin Source File

SOURCE=..\..\core\common\Version.rc
# End Source File
# End Group
# Begin Group "Client"

# PROP Default_Filter ""
# Begin Group "DaC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\client\DA\ClientDaAddressSpaceElement.cpp
DEP_CPP_CLIEN=\
	"..\..\core\include\OTClient.h"\
	"..\..\core\include\OTcommon.h"\
	".\client\AE\ClientAeSession.h"\
	".\client\ClientAddressSpaceElement.h"\
	".\client\ClientAddressSpaceElementBrowseOptions.h"\
	".\client\ClientApplication.h"\
	".\Client\ClientEnums.h"\
	".\Client\ClientObjectSpaceElement.h"\
	".\client\DA\ClientDaAddressSpaceElement.h"\
	".\client\DA\ClientDaGetPropertiesOptions.h"\
	".\Client\DA\ClientDaSession.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\client\DA\ClientDaAddressSpaceElement.h
# End Source File
# Begin Source File

SOURCE=.\client\DA\ClientDaGetPropertiesOptions.cpp
DEP_CPP_CLIENT=\
	"..\..\core\include\OTcommon.h"\
	".\Client\ClientEnums.h"\
	".\client\DA\ClientDaGetPropertiesOptions.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	
# End Source File
# Begin Source File

SOURCE=.\client\DA\ClientDaGetPropertiesOptions.h
# End Source File
# Begin Source File

SOURCE=.\client\DA\ClientDaItem.cpp
DEP_CPP_CLIENTD=\
	"..\..\core\include\OTClient.h"\
	"..\..\core\include\OTcommon.h"\
	".\client\AE\ClientAeSession.h"\
	".\client\ClientAddressSpaceElement.h"\
	".\client\ClientApplication.h"\
	".\Client\ClientEnums.h"\
	".\Client\ClientObjectSpaceElement.h"\
	".\client\DA\ClientDaAddressSpaceElement.h"\
	".\client\DA\ClientDaItem.h"\
	".\Client\DA\ClientDaSession.h"\
	".\client\DA\ClientDaSubscription.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\client\DA\ClientDaItem.h
# End Source File
# Begin Source File

SOURCE=.\client\DA\ClientDaProperty.cpp
DEP_CPP_CLIENTDA=\
	"..\..\core\include\OTcommon.h"\
	".\Client\ClientEnums.h"\
	".\client\DA\ClientDaProperty.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\client\DA\ClientDaProperty.h
# End Source File
# Begin Source File

SOURCE=.\Client\DA\ClientDaSession.cpp
DEP_CPP_CLIENTDAS=\
	"..\..\core\include\OTClient.h"\
	"..\..\core\include\OTcommon.h"\
	".\client\AE\ClientAeSession.h"\
	".\client\ClientAddressSpaceElement.h"\
	".\client\ClientAddressSpaceElementBrowseOptions.h"\
	".\client\ClientApplication.h"\
	".\Client\ClientEnums.h"\
	".\Client\ClientObjectSpaceElement.h"\
	".\client\ClientServerStatus.h"\
	".\client\DA\ClientDaAddressSpaceElement.h"\
	".\client\DA\ClientDaGetPropertiesOptions.h"\
	".\client\DA\ClientDaProperty.h"\
	".\Client\DA\ClientDaSession.h"\
	".\client\DA\ClientDaSubscription.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\Client\DA\ClientDaSession.h
# End Source File
# Begin Source File

SOURCE=.\client\DA\ClientDaSubscription.cpp
DEP_CPP_CLIENTDASU=\
	"..\..\core\include\OTClient.h"\
	"..\..\core\include\OTcommon.h"\
	".\client\AE\ClientAeSession.h"\
	".\client\ClientAddressSpaceElement.h"\
	".\client\ClientApplication.h"\
	".\Client\ClientEnums.h"\
	".\Client\ClientObjectSpaceElement.h"\
	".\client\DA\ClientDaAddressSpaceElement.h"\
	".\client\DA\ClientDaItem.h"\
	".\Client\DA\ClientDaSession.h"\
	".\client\DA\ClientDaSubscription.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\client\DA\ClientDaSubscription.h
# End Source File
# End Group
# Begin Group "AeC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\client\AE\ClientAeAttribute.h
# End Source File
# Begin Source File

SOURCE=.\client\AE\ClientAeCategory.cpp
DEP_CPP_CLIENTA=\
	"..\..\core\include\OTClient.h"\
	"..\..\core\include\OTcommon.h"\
	".\client\AE\ClientAeAttribute.h"\
	".\client\AE\ClientAeCategory.h"\
	".\client\AE\ClientAeSession.h"\
	".\client\ClientAddressSpaceElement.h"\
	".\client\ClientApplication.h"\
	".\Client\ClientEnums.h"\
	".\Client\ClientObjectSpaceElement.h"\
	".\client\DA\ClientDaAddressSpaceElement.h"\
	".\Client\DA\ClientDaSession.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\client\AE\ClientAeCategory.h
# End Source File
# Begin Source File

SOURCE=.\client\AE\ClientAeCondition.cpp
DEP_CPP_CLIENTAE=\
	"..\..\core\include\OTcommon.h"\
	".\client\AE\ClientAeAttribute.h"\
	".\client\AE\ClientAeCondition.h"\
	".\client\AE\ClientAeEvent.h"\
	".\client\AE\ClientAeSession.h"\
	".\client\AE\ClientAeSubscription.h"\
	".\client\ClientAddressSpaceElement.h"\
	".\client\ClientApplication.h"\
	".\Client\ClientEnums.h"\
	".\Client\ClientObjectSpaceElement.h"\
	".\client\DA\ClientDaAddressSpaceElement.h"\
	".\Client\DA\ClientDaSession.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\client\AE\ClientAeCondition.h
# End Source File
# Begin Source File

SOURCE=.\client\AE\ClientAeEvent.h
# End Source File
# Begin Source File

SOURCE=.\client\AE\ClientAeSession.cpp
DEP_CPP_CLIENTAES=\
	"..\..\core\include\OTClient.h"\
	"..\..\core\include\OTcommon.h"\
	".\client\AE\ClientAeAttribute.h"\
	".\client\AE\ClientAeCategory.h"\
	".\client\AE\ClientAeCondition.h"\
	".\client\AE\ClientAeEvent.h"\
	".\client\AE\ClientAeSession.h"\
	".\client\AE\ClientAeSubscription.h"\
	".\client\ClientAddressSpaceElement.h"\
	".\client\ClientAddressSpaceElementBrowseOptions.h"\
	".\client\ClientApplication.h"\
	".\Client\ClientEnums.h"\
	".\Client\ClientObjectSpaceElement.h"\
	".\client\ClientServerStatus.h"\
	".\client\DA\ClientDaAddressSpaceElement.h"\
	".\Client\DA\ClientDaSession.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\client\AE\ClientAeSession.h
# End Source File
# Begin Source File

SOURCE=.\client\AE\ClientAeSubscription.cpp
DEP_CPP_CLIENTAESU=\
	"..\..\core\include\OTClient.h"\
	"..\..\core\include\OTcommon.h"\
	".\client\AE\ClientAeCondition.h"\
	".\client\AE\ClientAeEvent.h"\
	".\client\AE\ClientAeSession.h"\
	".\client\AE\ClientAeSubscription.h"\
	".\client\ClientAddressSpaceElement.h"\
	".\client\ClientApplication.h"\
	".\Client\ClientEnums.h"\
	".\Client\ClientObjectSpaceElement.h"\
	".\client\DA\ClientDaAddressSpaceElement.h"\
	".\Client\DA\ClientDaSession.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\client\AE\ClientAeSubscription.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\client\ClientAddressSpaceElement.cpp
DEP_CPP_CLIENTAD=\
	"..\..\core\include\OTClient.h"\
	"..\..\core\include\OTcommon.h"\
	".\client\AE\ClientAeSession.h"\
	".\client\ClientAddressSpaceElement.h"\
	".\client\ClientAddressSpaceElementBrowseOptions.h"\
	".\client\ClientApplication.h"\
	".\Client\ClientEnums.h"\
	".\Client\ClientObjectSpaceElement.h"\
	".\client\DA\ClientDaAddressSpaceElement.h"\
	".\Client\DA\ClientDaSession.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\client\ClientAddressSpaceElement.h
# End Source File
# Begin Source File

SOURCE=.\client\ClientAddressSpaceElementBrowseOptions.cpp
DEP_CPP_CLIENTADD=\
	"..\..\core\include\OTcommon.h"\
	".\client\ClientAddressSpaceElementBrowseOptions.h"\
	".\Client\ClientEnums.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	
# End Source File
# Begin Source File

SOURCE=.\client\ClientAddressSpaceElementBrowseOptions.h
# End Source File
# Begin Source File

SOURCE=.\client\ClientApplication.cpp
DEP_CPP_CLIENTAP=\
	"..\..\core\include\OTClient.h"\
	"..\..\core\include\OTcommon.h"\
	".\client\AE\ClientAeAttribute.h"\
	".\client\AE\ClientAeCategory.h"\
	".\client\AE\ClientAeCondition.h"\
	".\client\AE\ClientAeEvent.h"\
	".\client\AE\ClientAeSession.h"\
	".\client\AE\ClientAeSubscription.h"\
	".\client\ClientAddressSpaceElement.h"\
	".\client\ClientApplication.h"\
	".\client\ClientCommon.h"\
	".\Client\ClientEnums.h"\
	".\Client\ClientObjectSpaceElement.h"\
	".\client\ClientServerBrowser.h"\
	".\client\ClientServerStatus.h"\
	".\client\DA\ClientDaAddressSpaceElement.h"\
	".\client\DA\ClientDaItem.h"\
	".\client\DA\ClientDaProperty.h"\
	".\Client\DA\ClientDaSession.h"\
	".\client\DA\ClientDaSubscription.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\client\ClientApplication.h
# End Source File
# Begin Source File

SOURCE=.\client\ClientCommon.h
# End Source File
# Begin Source File

SOURCE=.\Client\ClientEnums.h
# End Source File
# Begin Source File

SOURCE=.\Client\ClientObjectSpaceElement.cpp
DEP_CPP_CLIENTO=\
	"..\..\core\include\OTClient.h"\
	"..\..\core\include\OTcommon.h"\
	".\client\AE\ClientAeSession.h"\
	".\client\ClientAddressSpaceElement.h"\
	".\client\ClientApplication.h"\
	".\Client\ClientEnums.h"\
	".\Client\ClientObjectSpaceElement.h"\
	".\client\DA\ClientDaAddressSpaceElement.h"\
	".\Client\DA\ClientDaSession.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\Client\ClientObjectSpaceElement.h
# End Source File
# Begin Source File

SOURCE=.\client\ClientServerBrowser.cpp
DEP_CPP_CLIENTS=\
	"..\..\core\include\OTClient.h"\
	"..\..\core\include\OTcommon.h"\
	".\client\AE\ClientAeSession.h"\
	".\client\ClientAddressSpaceElement.h"\
	".\client\ClientApplication.h"\
	".\Client\ClientEnums.h"\
	".\Client\ClientObjectSpaceElement.h"\
	".\client\ClientServerBrowser.h"\
	".\client\DA\ClientDaAddressSpaceElement.h"\
	".\Client\DA\ClientDaSession.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\client\ClientServerBrowser.h
# End Source File
# Begin Source File

SOURCE=.\client\ClientServerStatus.cpp
DEP_CPP_CLIENTSE=\
	"..\..\core\include\OTcommon.h"\
	".\Client\ClientEnums.h"\
	".\client\ClientServerStatus.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\client\ClientServerStatus.h
# End Source File
# End Group
# Begin Group "Server"

# PROP Default_Filter ""
# Begin Group "DaS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\server\DA\ServerDaAddressSpaceElement.cpp
DEP_CPP_SERVE=\
	"..\..\core\include\OTcommon.h"\
	"..\..\core\include\OTServer.h"\
	".\Enums.h"\
	".\Mutex.h"\
	".\OSCompat.h"\
	".\server\DA\ServerDaAddressSpaceElement.h"\
	".\server\DA\ServerDaProperty.h"\
	".\server\DA\ServerDaRequest.h"\
	".\server\ServerAddressSpaceElement.h"\
	".\server\ServerApplication.h"\
	".\server\ServerCreator.h"\
	".\server\ServerEnums.h"\
	".\server\ServerWebTemplate.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
NODEP_CPP_SERVE=\
	".\pthread.h"\
	
# End Source File
# Begin Source File

SOURCE=.\server\DA\ServerDaAddressSpaceElement.h
# End Source File
# Begin Source File

SOURCE=.\server\DA\ServerDaProperty.cpp
DEP_CPP_SERVER=\
	"..\..\core\include\OTcommon.h"\
	".\Enums.h"\
	".\Mutex.h"\
	".\OSCompat.h"\
	".\server\DA\ServerDaProperty.h"\
	".\server\DA\ServerDaRequest.h"\
	".\server\ServerApplication.h"\
	".\server\ServerCreator.h"\
	".\server\ServerEnums.h"\
	".\server\ServerWebTemplate.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
NODEP_CPP_SERVER=\
	".\pthread.h"\
	
# End Source File
# Begin Source File

SOURCE=.\server\DA\ServerDaProperty.h
# End Source File
# Begin Source File

SOURCE=.\server\DA\ServerDaRequest.cpp
DEP_CPP_SERVERD=\
	"..\..\core\include\OTcommon.h"\
	"..\..\core\include\OTServer.h"\
	".\Enums.h"\
	".\Mutex.h"\
	".\OSCompat.h"\
	".\server\DA\ServerDaAddressSpaceElement.h"\
	".\server\DA\ServerDaRequest.h"\
	".\server\DA\ServerDaTransaction.h"\
	".\server\ServerAddressSpaceElement.h"\
	".\server\ServerApplication.h"\
	".\server\ServerCreator.h"\
	".\server\ServerEnums.h"\
	".\server\ServerWebTemplate.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
NODEP_CPP_SERVERD=\
	".\pthread.h"\
	
# End Source File
# Begin Source File

SOURCE=.\server\DA\ServerDaRequest.h
# End Source File
# Begin Source File

SOURCE=.\server\DA\ServerDaSession.cpp
DEP_CPP_SERVERDA=\
	"..\..\core\include\OTcommon.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\server\DA\ServerDaSession.h"\
	".\server\ServerEnums.h"\
	
# End Source File
# Begin Source File

SOURCE=.\server\DA\ServerDaSession.h
# End Source File
# Begin Source File

SOURCE=.\server\DA\ServerDaTransaction.cpp
DEP_CPP_SERVERDAT=\
	"..\..\core\include\OTcommon.h"\
	"..\..\core\include\OTServer.h"\
	".\Enums.h"\
	".\Mutex.h"\
	".\OSCompat.h"\
	".\server\DA\ServerDaAddressSpaceElement.h"\
	".\server\DA\ServerDaRequest.h"\
	".\server\DA\ServerDaSession.h"\
	".\server\DA\ServerDaTransaction.h"\
	".\server\ServerAddressSpaceElement.h"\
	".\server\ServerApplication.h"\
	".\server\ServerCreator.h"\
	".\server\ServerEnums.h"\
	".\server\ServerWebTemplate.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
NODEP_CPP_SERVERDAT=\
	".\pthread.h"\
	
# End Source File
# Begin Source File

SOURCE=.\server\DA\ServerDaTransaction.h
# End Source File
# End Group
# Begin Group "AeS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\server\AE\ServerAeAddressSpaceElement.cpp
DEP_CPP_SERVERA=\
	"..\..\core\include\OTcommon.h"\
	"..\..\core\include\OTServer.h"\
	".\Enums.h"\
	".\Mutex.h"\
	".\OSCompat.h"\
	".\server\AE\ServerAeAddressSpaceElement.h"\
	".\server\ServerAddressSpaceElement.h"\
	".\server\ServerApplication.h"\
	".\server\ServerCreator.h"\
	".\server\ServerEnums.h"\
	".\server\ServerWebTemplate.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
NODEP_CPP_SERVERA=\
	".\pthread.h"\
	
# End Source File
# Begin Source File

SOURCE=.\server\AE\ServerAeAddressSpaceElement.h
# End Source File
# Begin Source File

SOURCE=.\server\AE\ServerAeAttribute.cpp
DEP_CPP_SERVERAE=\
	"..\..\core\include\OTcommon.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\server\AE\ServerAeAttribute.h"\
	
# End Source File
# Begin Source File

SOURCE=.\server\AE\ServerAeAttribute.h
# End Source File
# Begin Source File

SOURCE=.\server\AE\ServerAeCategory.cpp
DEP_CPP_SERVERAEC=\
	"..\..\core\include\OTcommon.h"\
	"..\..\core\include\OTServer.h"\
	".\Enums.h"\
	".\Mutex.h"\
	".\OSCompat.h"\
	".\server\AE\ServerAeAttribute.h"\
	".\server\AE\ServerAeCategory.h"\
	".\server\AE\ServerAeCondition.h"\
	".\server\ServerApplication.h"\
	".\server\ServerCreator.h"\
	".\server\ServerEnums.h"\
	".\server\ServerWebTemplate.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
NODEP_CPP_SERVERAEC=\
	".\pthread.h"\
	
# End Source File
# Begin Source File

SOURCE=.\server\AE\ServerAeCategory.h
# End Source File
# Begin Source File

SOURCE=.\server\AE\ServerAeCondition.cpp
DEP_CPP_SERVERAECO=\
	"..\..\core\include\OTcommon.h"\
	".\Enums.h"\
	".\Mutex.h"\
	".\OSCompat.h"\
	".\server\AE\ServerAeAttribute.h"\
	".\server\AE\ServerAeCategory.h"\
	".\server\AE\ServerAeCondition.h"\
	".\server\ServerApplication.h"\
	".\server\ServerCreator.h"\
	".\server\ServerEnums.h"\
	".\server\ServerWebTemplate.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
NODEP_CPP_SERVERAECO=\
	".\pthread.h"\
	
# End Source File
# Begin Source File

SOURCE=.\server\AE\ServerAeCondition.h
# End Source File
# Begin Source File

SOURCE=.\server\AE\ServerAeEvent.cpp
DEP_CPP_SERVERAEE=\
	"..\..\core\include\OTcommon.h"\
	".\Enums.h"\
	".\Mutex.h"\
	".\OSCompat.h"\
	".\server\AE\ServerAeEvent.h"\
	".\server\ServerApplication.h"\
	".\server\ServerCreator.h"\
	".\server\ServerEnums.h"\
	".\server\ServerWebTemplate.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
NODEP_CPP_SERVERAEE=\
	".\pthread.h"\
	
# End Source File
# Begin Source File

SOURCE=.\server\AE\ServerAeEvent.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\server\ServerAddressSpaceElement.cpp
DEP_CPP_SERVERAD=\
	"..\..\core\include\OTcommon.h"\
	"..\..\core\include\OTServer.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\server\ServerAddressSpaceElement.h"\
	".\server\ServerEnums.h"\
	
# End Source File
# Begin Source File

SOURCE=.\server\ServerAddressSpaceElement.h
# End Source File
# Begin Source File

SOURCE=.\server\ServerApplication.cpp
DEP_CPP_SERVERAP=\
	"..\..\core\include\OTcommon.h"\
	"..\..\core\include\OTServer.h"\
	".\Enums.h"\
	".\Mutex.h"\
	".\OSCompat.h"\
	".\server\AE\ServerAeAddressSpaceElement.h"\
	".\server\AE\ServerAeAttribute.h"\
	".\server\AE\ServerAeCategory.h"\
	".\server\AE\ServerAeCondition.h"\
	".\server\AE\ServerAeEvent.h"\
	".\server\DA\ServerDaAddressSpaceElement.h"\
	".\server\DA\ServerDaProperty.h"\
	".\server\DA\ServerDaRequest.h"\
	".\server\DA\ServerDaSession.h"\
	".\server\DA\ServerDaTransaction.h"\
	".\server\ServerAddressSpaceElement.h"\
	".\server\ServerApplication.h"\
	".\server\ServerCommon.h"\
	".\server\ServerCreator.h"\
	".\server\ServerEnums.h"\
	".\server\ServerWebTemplate.h"\
	".\Trace.h"\
	".\ValueQT.h"\
	
NODEP_CPP_SERVERAP=\
	".\pthread.h"\
	
# End Source File
# Begin Source File

SOURCE=.\server\ServerApplication.h
# End Source File
# Begin Source File

SOURCE=.\server\ServerCommon.h
# End Source File
# Begin Source File

SOURCE=.\server\ServerCreator.cpp
DEP_CPP_SERVERC=\
	"..\..\core\include\OTcommon.h"\
	".\Enums.h"\
	".\OSCompat.h"\
	".\server\AE\ServerAeAddressSpaceElement.h"\
	".\server\AE\ServerAeAttribute.h"\
	".\server\DA\ServerDaAddressSpaceElement.h"\
	".\server\DA\ServerDaRequest.h"\
	".\server\DA\ServerDaSession.h"\
	".\server\DA\ServerDaTransaction.h"\
	".\server\ServerAddressSpaceElement.h"\
	".\server\ServerCreator.h"\
	".\server\ServerEnums.h"\
	".\server\ServerWebTemplate.h"\
	".\ValueQT.h"\
	
# End Source File
# Begin Source File

SOURCE=.\server\ServerCreator.h
# End Source File
# Begin Source File

SOURCE=.\server\ServerEnums.h
# End Source File
# Begin Source File

SOURCE=.\server\ServerWebTemplate.h
# End Source File
# End Group
# End Group
# End Target
# End Project
