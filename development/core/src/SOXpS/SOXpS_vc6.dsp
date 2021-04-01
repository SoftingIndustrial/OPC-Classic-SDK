# Microsoft Developer Studio Project File - Name="SOXpS_vc6" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SOXpS_vc6 - Win32 ServiceDA Release UNICODE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SOXpS_vc6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SOXpS_vc6.mak" CFG="SOXpS_vc6 - Win32 ServiceDA Release UNICODE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SOXpS_vc6 - Win32 Server Debug" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 Server Release" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 Server Demo" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 Service Debug" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 Service Release" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 Server Debug UNICODE" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 Server Release UNICODE" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 Server Demo UNICODE" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 Service Debug UNICODE" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 Service Release UNICODE" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 ServerDA Debug" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 ServerDA Release" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 ServerDA Demo" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 ServiceDA Debug" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 ServiceDA Release" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 ServerDA Debug UNICODE" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 ServerDA Release UNICODE" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 ServerDA Demo UNICODE" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 ServiceDA Debug UNICODE" (based on "Win32 (x86) Application")
!MESSAGE "SOXpS_vc6 - Win32 ServiceDA Release UNICODE" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debug\server"
# PROP Intermediate_Dir "vc6\debug\server"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "_DEBUG" /D "SOXPS_SERVER" /D "_WINDOWS" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /D "SOXPS_AE" /D "SOXPS_ALL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x400 /i "..\..\common" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debug\server/SOXpSSrvDebug.exe" /pdbtype:sept /libpath:"..\lib\vc6"
# Begin Custom Build
IntDir=.\vc6\debug\server
InputPath=.\vc6\debug\server\SOXpSSrvDebug.exe
InputName=SOXpSSrvDebug
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\release\server"
# PROP Intermediate_Dir "vc6\release\server"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\common" /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "NDEBUG" /D "SOXPS_SERVER" /D "_WINDOWS" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /D "SOXPS_AE" /D "SOXPS_ALL" /FR /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x400 /i "..\..\common" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\release\server/SOXpSSrv.exe" /libpath:"..\lib\vc6"
# Begin Custom Build
IntDir=.\vc6\release\server
InputPath=.\vc6\release\server\SOXpSSrv.exe
InputName=SOXpSSrv
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Demo"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\demo\server"
# PROP Intermediate_Dir "vc6\demo\server"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "SOFLAG_DEMO_VERSION" /D "NDEBUG" /D "SOXPS_SERVER" /D "_WINDOWS" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /D "SOXPS_AE" /D "SOXPS_ALL" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "NDEBUG" /d "SOFLAG_DEMO_VERSION"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\demo\server/SOXpSSrvDemo.exe"
# Begin Custom Build
IntDir=.\vc6\demo\server
InputPath=.\vc6\demo\server\SOXpSSrvDemo.exe
InputName=SOXpSSrvDemo
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Service Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debug\service"
# PROP Intermediate_Dir "vc6\debug\service"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "_CONSOLE" /D "_DEBUG" /D "SOXPS_SERVICE" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /D "SOXPS_AE" /D "SOXPS_ALL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"vc6\debug\service/SOXpSSvcDebug.exe" /pdbtype:sept /libpath:"..\lib\vc6"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
IntDir=.\vc6\debug\service
InputPath=.\vc6\debug\service\SOXpSSvcDebug.exe
InputName=SOXpSSvcDebug
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Service Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\release\service"
# PROP Intermediate_Dir "vc6\release\service"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "_CONSOLE" /D "NDEBUG" /D "SOXPS_SERVICE" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /D "SOXPS_AE" /D "SOXPS_ALL" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /machine:I386 /out:"vc6\release\service/SOXpSSvc.exe" /libpath:"..\lib\vc6"
# SUBTRACT LINK32 /pdb:none /debug
# Begin Custom Build
IntDir=.\vc6\release\service
InputPath=.\vc6\release\service\SOXpSSvc.exe
InputName=SOXpSSvc
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Debug UNICODE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SOXpS_vc6___Win32_Server_Debug_UNICODE"
# PROP BASE Intermediate_Dir "SOXpS_vc6___Win32_Server_Debug_UNICODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debug\Userver"
# PROP Intermediate_Dir "vc6\debug\Userver"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "_DEBUG" /D "SOXPS" /D "SOXPS_SERVER" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "SOXPS_SERVER" /D "_WINDOWS" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /D "SOXPS_AE" /D "SOXPS_ALL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /i "..\common" /d "_DEBUG"
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "_DEBUG" /d "UNICODE" /d "_UNICODE"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debug\server/SOXpSSrvDebug.exe" /pdbtype:sept /libpath:"..\lib\vc6"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debug\Userver/SOXpSSrvDebugU.exe" /pdbtype:sept /libpath:"..\lib\vc6"
# Begin Custom Build
IntDir=.\vc6\debug\Userver
InputPath=.\vc6\debug\Userver\SOXpSSrvDebugU.exe
InputName=SOXpSSrvDebugU
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Release UNICODE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SOXpS_vc6___Win32_Server_Release_UNICODE"
# PROP BASE Intermediate_Dir "SOXpS_vc6___Win32_Server_Release_UNICODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\release\Userver"
# PROP Intermediate_Dir "vc6\release\Userver"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "NDEBUG" /D "SOXPS" /D "SOXPS_SERVER" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "SOXPS_SERVER" /D "_WINDOWS" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /D "SOXPS_AE" /D "SOXPS_ALL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /i "..\common" /d "NDEBUG"
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "NDEBUG" /d "UNICODE" /d "_UNICODE"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\release\server/SOXpSSrv.exe" /libpath:"..\lib\vc6"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\release\Userver/SOXpSSrvU.exe" /libpath:"..\lib\vc6"
# Begin Custom Build
IntDir=.\vc6\release\Userver
InputPath=.\vc6\release\Userver\SOXpSSrvU.exe
InputName=SOXpSSrvU
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Demo UNICODE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SOXpS_vc6___Win32_Server_Demo_UNICODE"
# PROP BASE Intermediate_Dir "SOXpS_vc6___Win32_Server_Demo_UNICODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\demo\Userver"
# PROP Intermediate_Dir "vc6\demo\Userver"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "SOFLAG_DEMO_VERSION" /D "NDEBUG" /D "SOXPS" /D "SOXPS_SERVER" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "UNICODE" /D "_UNICODE" /D "SOFLAG_DEMO_VERSION" /D "NDEBUG" /D "SOXPS_SERVER" /D "_WINDOWS" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /D "SOXPS_AE" /D "SOXPS_ALL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /i "..\common" /d "NDEBUG" /d "SOFLAG_DEMO_VERSION"
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "NDEBUG" /d "UNICODE" /d "_UNICODE" /d "SOFLAG_DEMO_VERSION"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\demo\server/SOXpSSrvDemo.exe"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\demo\Userver/SOXpSSrvUDemo.exe"
# Begin Custom Build
IntDir=.\vc6\demo\Userver
InputPath=.\vc6\demo\Userver\SOXpSSrvUDemo.exe
InputName=SOXpSSrvUDemo
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Service Debug UNICODE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SOXpS_vc6___Win32_Service_Debug_UNICODE"
# PROP BASE Intermediate_Dir "SOXpS_vc6___Win32_Service_Debug_UNICODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debug\Uservice"
# PROP Intermediate_Dir "vc6\debug\Uservice"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "_CONSOLE" /D "_DEBUG" /D "SOXPS" /D "SOXPS_SERVICE" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "_CONSOLE" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "SOXPS_SERVICE" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /D "SOXPS_AE" /D "SOXPS_ALL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /i "..\common" /d "_DEBUG"
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "_DEBUG" /d "UNICODE" /d "_UNICODE"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"vc6\debug\service/SOXpSSvcDebug.exe" /pdbtype:sept /libpath:"..\lib\vc6"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"vc6\debug\Uservice/SOXpSSvcUDebug.exe" /pdbtype:sept /libpath:"..\lib\vc6"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
IntDir=.\vc6\debug\Uservice
InputPath=.\vc6\debug\Uservice\SOXpSSvcUDebug.exe
InputName=SOXpSSvcUDebug
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Service Release UNICODE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SOXpS_vc6___Win32_Service_Release_UNICODE"
# PROP BASE Intermediate_Dir "SOXpS_vc6___Win32_Service_Release_UNICODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\release\Uservice"
# PROP Intermediate_Dir "vc6\release\Uservice"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "_CONSOLE" /D "NDEBUG" /D "SOXPS" /D "SOXPS_SERVICE" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "_CONSOLE" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "SOXPS_SERVICE" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /D "SOXPS_AE" /D "SOXPS_ALL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /i "..\common" /d "NDEBUG"
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "NDEBUG" /d "UNICODE" /d "_UNICODE"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /machine:I386 /out:"vc6\release\service/SOXpSSvc.exe" /libpath:"..\lib\vc6"
# SUBTRACT BASE LINK32 /pdb:none /debug
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /machine:I386 /out:"vc6\release\Uservice/SOXpSSvcU.exe" /libpath:"..\lib\vc6"
# SUBTRACT LINK32 /pdb:none /debug
# Begin Custom Build
IntDir=.\vc6\release\Uservice
InputPath=.\vc6\release\Uservice\SOXpSSvcU.exe
InputName=SOXpSSvcU
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SOXpS_vc6___Win32_ServerDA_Debug"
# PROP BASE Intermediate_Dir "SOXpS_vc6___Win32_ServerDA_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debug\serverDA"
# PROP Intermediate_Dir "vc6\debug\serverDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "_DEBUG" /D "SOXPS" /D "SOXPS_SERVER" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "_DEBUG" /D "SOXPS_SERVER" /D "_WINDOWS" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /i "..\common" /d "_DEBUG"
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debug\server/SOXpSSrvDebug.exe" /pdbtype:sept /libpath:"..\lib\vc6"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debug\serverDA/SOXpSSrvDADebug.exe" /pdbtype:sept /libpath:"..\lib\vc6"
# Begin Custom Build
IntDir=.\vc6\debug\serverDA
InputPath=.\vc6\debug\serverDA\SOXpSSrvDADebug.exe
InputName=SOXpSSrvDADebug
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SOXpS_vc6___Win32_ServerDA_Release"
# PROP BASE Intermediate_Dir "SOXpS_vc6___Win32_ServerDA_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\release\serverDA"
# PROP Intermediate_Dir "vc6\release\serverDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "NDEBUG" /D "SOXPS" /D "SOXPS_SERVER" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "NDEBUG" /D "SOXPS_SERVER" /D "_WINDOWS" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /i "..\common" /d "NDEBUG"
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\release\server/SOXpSSrv.exe" /libpath:"..\lib\vc6"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\release\serverDA/SOXpSSrvDA.exe" /libpath:"..\lib\vc6"
# Begin Custom Build
IntDir=.\vc6\release\serverDA
InputPath=.\vc6\release\serverDA\SOXpSSrvDA.exe
InputName=SOXpSSrvDA
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Demo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SOXpS_vc6___Win32_ServerDA_Demo"
# PROP BASE Intermediate_Dir "SOXpS_vc6___Win32_ServerDA_Demo"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\demo\serverDA"
# PROP Intermediate_Dir "vc6\demo\serverDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "SOFLAG_DEMO_VERSION" /D "NDEBUG" /D "SOXPS" /D "SOXPS_SERVER" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "SOFLAG_DEMO_VERSION" /D "NDEBUG" /D "SOXPS_SERVER" /D "_WINDOWS" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /i "..\common" /d "NDEBUG" /d "SOFLAG_DEMO_VERSION"
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "NDEBUG" /d "SOFLAG_DEMO_VERSION"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\demo\server/SOXpSSrvDemo.exe"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\demo\serverDA/SOXpSSrvDADemo.exe"
# Begin Custom Build
IntDir=.\vc6\demo\serverDA
InputPath=.\vc6\demo\serverDA\SOXpSSrvDADemo.exe
InputName=SOXpSSrvDADemo
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServiceDA Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SOXpS_vc6___Win32_ServiceDA_Debug"
# PROP BASE Intermediate_Dir "SOXpS_vc6___Win32_ServiceDA_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debug\serviceDA"
# PROP Intermediate_Dir "vc6\debug\serviceDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "_CONSOLE" /D "_DEBUG" /D "SOXPS" /D "SOXPS_SERVICE" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "_CONSOLE" /D "_DEBUG" /D "SOXPS_SERVICE" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /i "..\common" /d "_DEBUG"
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"vc6\debug\service/SOXpSSvcDebug.exe" /pdbtype:sept /libpath:"..\lib\vc6"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"vc6\debug\serviceDA/SOXpSSvcDADebug.exe" /pdbtype:sept /libpath:"..\lib\vc6"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
IntDir=.\vc6\debug\serviceDA
InputPath=.\vc6\debug\serviceDA\SOXpSSvcDADebug.exe
InputName=SOXpSSvcDADebug
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServiceDA Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SOXpS_vc6___Win32_ServiceDA_Release"
# PROP BASE Intermediate_Dir "SOXpS_vc6___Win32_ServiceDA_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\release\serviceDA"
# PROP Intermediate_Dir "vc6\release\serviceDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "_CONSOLE" /D "NDEBUG" /D "SOXPS" /D "SOXPS_SERVICE" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "_CONSOLE" /D "NDEBUG" /D "SOXPS_SERVICE" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /i "..\common" /d "NDEBUG"
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /machine:I386 /out:"vc6\release\service/SOXpSSvc.exe" /libpath:"..\lib\vc6"
# SUBTRACT BASE LINK32 /pdb:none /debug
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /machine:I386 /out:"vc6\release\serviceDA/SOXpSSvcDA.exe" /libpath:"..\lib\vc6"
# SUBTRACT LINK32 /pdb:none /debug
# Begin Custom Build
IntDir=.\vc6\release\serviceDA
InputPath=.\vc6\release\serviceDA\SOXpSSvcDA.exe
InputName=SOXpSSvcDA
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Debug UNICODE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SOXpS_vc6___Win32_ServerDA_Debug_UNICODE"
# PROP BASE Intermediate_Dir "SOXpS_vc6___Win32_ServerDA_Debug_UNICODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debug\UserverDA"
# PROP Intermediate_Dir "vc6\debug\UserverDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "SOXPS" /D "SOXPS_SERVER" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "SOXPS_SERVER" /D "_WINDOWS" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /i "..\common" /d "_DEBUG" /d "UNICODE" /d "_UNICODE"
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "_DEBUG" /d "UNICODE" /d "_UNICODE"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debug\serverUNICODE/SOXpSSrvDebugU.exe" /pdbtype:sept /libpath:"..\lib\vc6"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debug\UserverDA/SOXpSSrvUDADebug.exe" /pdbtype:sept /libpath:"..\lib\vc6"
# Begin Custom Build
IntDir=.\vc6\debug\UserverDA
InputPath=.\vc6\debug\UserverDA\SOXpSSrvUDADebug.exe
InputName=SOXpSSrvUDADebug
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Release UNICODE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SOXpS_vc6___Win32_ServerDA_Release_UNICODE"
# PROP BASE Intermediate_Dir "SOXpS_vc6___Win32_ServerDA_Release_UNICODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\release\UserverDA"
# PROP Intermediate_Dir "vc6\release\UserverDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "SOXPS" /D "SOXPS_SERVER" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "SOXPS_SERVER" /D "_WINDOWS" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /i "..\common" /d "NDEBUG" /d "UNICODE" /d "_UNICODE"
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "NDEBUG" /d "UNICODE" /d "_UNICODE"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\release\serverUNICODE/SOXpSSrvU.exe" /libpath:"..\lib\vc6"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\release\UserverDA/SOXpSSrvUDA.exe" /libpath:"..\lib\vc6"
# Begin Custom Build
IntDir=.\vc6\release\UserverDA
InputPath=.\vc6\release\UserverDA\SOXpSSrvUDA.exe
InputName=SOXpSSrvUDA
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Demo UNICODE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SOXpS_vc6___Win32_ServerDA_Demo_UNICODE"
# PROP BASE Intermediate_Dir "SOXpS_vc6___Win32_ServerDA_Demo_UNICODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\demo\UserverDA"
# PROP Intermediate_Dir "vc6\demo\UserverDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "UNICODE" /D "_UNICODE" /D "SOFLAG_DEMO_VERSION" /D "NDEBUG" /D "SOXPS" /D "SOXPS_SERVER" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "UNICODE" /D "_UNICODE" /D "SOFLAG_DEMO_VERSION" /D "NDEBUG" /D "SOXPS_SERVER" /D "_WINDOWS" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /i "..\common" /d "NDEBUG" /d "UNICODE" /d "_UNICODE" /d "SOFLAG_DEMO_VERSION"
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "NDEBUG" /d "UNICODE" /d "_UNICODE" /d "SOFLAG_DEMO_VERSION"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\demo\serverUNICODE/SOXpSSrvDemoU.exe"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\demo\UserverDA/SOXpSSrvUDADemo.exe"
# Begin Custom Build
IntDir=.\vc6\demo\UserverDA
InputPath=.\vc6\demo\UserverDA\SOXpSSrvUDADemo.exe
InputName=SOXpSSrvUDADemo
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServiceDA Debug UNICODE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SOXpS_vc6___Win32_ServiceDA_Debug_UNICODE"
# PROP BASE Intermediate_Dir "SOXpS_vc6___Win32_ServiceDA_Debug_UNICODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debug\UserviceDA"
# PROP Intermediate_Dir "vc6\debug\UserviceDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "_CONSOLE" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "SOXPS" /D "SOXPS_SERVICE" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "_CONSOLE" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "SOXPS_SERVICE" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /i "..\common" /d "_DEBUG" /d "UNICODE" /d "_UNICODE"
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "_DEBUG" /d "UNICODE" /d "_UNICODE"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"vc6\debug\serviceUNICODE/SOXpSSvcDebugU.exe" /pdbtype:sept /libpath:"..\lib\vc6"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"vc6\debug\UserviceDA/SOXpSSvcUDADebug.exe" /pdbtype:sept /libpath:"..\lib\vc6"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
IntDir=.\vc6\debug\UserviceDA
InputPath=.\vc6\debug\UserviceDA\SOXpSSvcUDADebug.exe
InputName=SOXpSSvcUDADebug
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServiceDA Release UNICODE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SOXpS_vc6___Win32_ServiceDA_Release_UNICODE"
# PROP BASE Intermediate_Dir "SOXpS_vc6___Win32_ServiceDA_Release_UNICODE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\release\UserviceDA"
# PROP Intermediate_Dir "vc6\release\UserviceDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /D "_CONSOLE" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "SOXPS" /D "SOXPS_SERVICE" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\SOXpS" /I "..\SOCmn" /I "..\SOSrv" /I "..\SODaS" /I "..\SOAeS" /I "..\include" /I "..\opc" /I "..\common" /I "..\..\common" /D "_CONSOLE" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "SOXPS_SERVICE" /D "SOXPS" /D "SOFLAG_NO_FORCE_LIBS" /D "STRICT" /D "WIN32" /D "_MBCS" /D "_WIN32_DCOM" /D "SOXPS_DA" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /i "..\common" /d "NDEBUG" /d "UNICODE" /d "_UNICODE"
# ADD RSC /l 0x400 /i "..\common" /i "..\..\common" /d "NDEBUG" /d "UNICODE" /d "_UNICODE"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /machine:I386 /out:"vc6\release\serviceUNICODE/SOXpSSvcU.exe" /libpath:"..\lib\vc6"
# SUBTRACT BASE LINK32 /pdb:none /debug
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /machine:I386 /out:"vc6\release\UserviceDA/SOXpSSvcUDA.exe" /libpath:"..\lib\vc6"
# SUBTRACT LINK32 /pdb:none /debug
# Begin Custom Build
IntDir=.\vc6\release\UserviceDA
InputPath=.\vc6\release\UserviceDA\SOXpSSvcUDA.exe
InputName=SOXpSSvcUDA
SOURCE="$(InputPath)"

"..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(IntDir)\$(InputName).exe ..\bin

# End Custom Build

!ENDIF 

# Begin Target

# Name "SOXpS_vc6 - Win32 Server Debug"
# Name "SOXpS_vc6 - Win32 Server Release"
# Name "SOXpS_vc6 - Win32 Server Demo"
# Name "SOXpS_vc6 - Win32 Service Debug"
# Name "SOXpS_vc6 - Win32 Service Release"
# Name "SOXpS_vc6 - Win32 Server Debug UNICODE"
# Name "SOXpS_vc6 - Win32 Server Release UNICODE"
# Name "SOXpS_vc6 - Win32 Server Demo UNICODE"
# Name "SOXpS_vc6 - Win32 Service Debug UNICODE"
# Name "SOXpS_vc6 - Win32 Service Release UNICODE"
# Name "SOXpS_vc6 - Win32 ServerDA Debug"
# Name "SOXpS_vc6 - Win32 ServerDA Release"
# Name "SOXpS_vc6 - Win32 ServerDA Demo"
# Name "SOXpS_vc6 - Win32 ServiceDA Debug"
# Name "SOXpS_vc6 - Win32 ServiceDA Release"
# Name "SOXpS_vc6 - Win32 ServerDA Debug UNICODE"
# Name "SOXpS_vc6 - Win32 ServerDA Release UNICODE"
# Name "SOXpS_vc6 - Win32 ServerDA Demo UNICODE"
# Name "SOXpS_vc6 - Win32 ServiceDA Debug UNICODE"
# Name "SOXpS_vc6 - Win32 ServiceDA Release UNICODE"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\extimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\SOXpSAEObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\SOXpSCallbacks.cpp
# End Source File
# Begin Source File

SOURCE=.\SOXpSDAObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\SOXpSEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\SOXpSMain.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\common\DllMain.h
# End Source File
# Begin Source File

SOURCE=.\SOModule.h
# End Source File
# Begin Source File

SOURCE=..\common\SOVersion.h
# End Source File
# Begin Source File

SOURCE=..\include\SOXpS.h
# End Source File
# Begin Source File

SOURCE=.\SOXpSAEObjects.h
# End Source File
# Begin Source File

SOURCE=.\SOXpSCallbacks.h
# End Source File
# Begin Source File

SOURCE=.\SOXpSDAObjects.h
# End Source File
# Begin Source File

SOURCE=.\SOXpSEntry.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;rc"
# Begin Source File

SOURCE=..\SOSrv\SOSrvmc.mc

!IF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Debug"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Release"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Demo"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Service Debug"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Service Release"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Debug UNICODE"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Release UNICODE"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Demo UNICODE"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Service Debug UNICODE"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Service Release UNICODE"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Debug"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Release"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Demo"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServiceDA Debug"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServiceDA Release"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Debug UNICODE"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Release UNICODE"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Demo UNICODE"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServiceDA Debug UNICODE"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServiceDA Release UNICODE"

# Begin Custom Build
InputPath=..\SOSrv\SOSrvmc.mc
InputName=SOSrvmc

BuildCmds= \
	mc -u -U $(InputPath)

"SOSrv409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOSrv41D.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SOXpS.rc
# End Source File
# Begin Source File

SOURCE=.\SOXpSmc.mc

!IF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Debug"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Release"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Demo"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Service Debug"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Service Release"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Debug UNICODE"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Release UNICODE"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Server Demo UNICODE"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Service Debug UNICODE"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 Service Release UNICODE"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Debug"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Release"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Demo"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServiceDA Debug"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServiceDA Release"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Debug UNICODE"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Release UNICODE"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServerDA Demo UNICODE"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServiceDA Debug UNICODE"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOXpS_vc6 - Win32 ServiceDA Release UNICODE"

# Begin Custom Build
InputPath=.\SOXpSmc.mc

BuildCmds= \
	mc -U -u $(InputPath)

"SOXpS0.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"SOXpSmc.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "SOCmn Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\SOCmn.h
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnCfg.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnData.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnData.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnElement.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnElement.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnEvent.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnEventPool.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnEventPool.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnException.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnException.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnExpat.cpp
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnFunctions.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnFunctions.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnHttp.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnHttp.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnList.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnList.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnObject.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnObject.h
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnOsCompat.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnSocket.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnString.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnString.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnSync.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnSync.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnThread.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnThread.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnTime.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnTime.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnTrace.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnTrace.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnVariant.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnVariant.h
# End Source File
# Begin Source File

SOURCE=..\SOCmn\SOCmnXML.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOCmnXML.h
# End Source File
# End Group
# Begin Group "SOSrv Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\SOSrv.h
# End Source File
# Begin Source File

SOURCE=..\SOSrv\SOSrvBrowser.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOSrvBrowser.h
# End Source File
# Begin Source File

SOURCE=..\SOSrv\SOSrvComEnum.cpp
# End Source File
# Begin Source File

SOURCE=..\SOSrv\SOSrvComEnum.h
# End Source File
# Begin Source File

SOURCE=..\SOSrv\SOSrvComObject.cpp
# End Source File
# Begin Source File

SOURCE=..\SOSrv\SOSrvComObject.h
# End Source File
# Begin Source File

SOURCE=..\SOSrv\SOSrvEntry.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOSrvEntry.h
# End Source File
# Begin Source File

SOURCE=..\SOSrv\SOSrvServer.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOSrvServer.h
# End Source File
# Begin Source File

SOURCE=..\SOSrv\SOSrvTrace.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOSrvTrace.h
# End Source File
# Begin Source File

SOURCE=..\SOSrv\SOSrvWatch.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOSrvWatch.h
# End Source File
# Begin Source File

SOURCE=..\SOSrv\SOSrvXML.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOSrvXML.h
# End Source File
# End Group
# Begin Group "SODaS Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SODaS\globals.h
# End Source File
# Begin Source File

SOURCE=..\include\SODaS.h
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSCache.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SODaSCache.h
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSComEnum.cpp
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSComEnum.h
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSComGroup.cpp
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSComGroup.h
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSComServer.cpp
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSComServer.h
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSEntry.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SODaSEntry.h
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSGroup.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SODaSGroup.h
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSItem.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SODaSItem.h
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSItemTag.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SODaSItemTag.h
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSNode.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SODaSNode.h
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SODaSProperty.h
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSRequest.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SODaSRequest.h
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSServer.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SODaSServer.h
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSTag.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SODaSTag.h
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSTransaction.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SODaSTransaction.h
# End Source File
# Begin Source File

SOURCE=..\SODaS\SODaSWatch.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SODaSWatch.h
# End Source File
# End Group
# Begin Group "SOAeS Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\SOAeS.h
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSArea.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOAeSArea.h
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSAreaBrowser.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOAeSAreaBrowser.h
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSAttribute.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOAeSAttribute.h
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSCategory.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOAeSCategory.h
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSComAreaBrowser.cpp
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSComAreaBrowser.h
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSComServer.cpp
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSComServer.h
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSComSubscription.cpp
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSComSubscription.h
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSCondition.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOAeSCondition.h
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSEntry.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOAeSEntry.h
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOAeSEvent.h
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSServer.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOAeSServer.h
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSSource.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOAeSSource.h
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSSubCondition.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOAeSSubCondition.h
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSSubscription.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOAeSSubscription.h
# End Source File
# Begin Source File

SOURCE=..\SOAeS\SOAeSWatch.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOAeSWatch.h
# End Source File
# End Group
# Begin Group "SOAsS Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SOAsS\SOAsSEntry.cpp
# End Source File
# Begin Source File

SOURCE=..\include\SOAsSEntry.h
# End Source File
# End Group
# End Target
# End Project
