# Microsoft Developer Studio Project File - Name="SOServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SOServer - Win32 Demo XML DA
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SOServer_vc6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SOServer_vc6.mak" CFG="SOServer - Win32 Demo XML DA"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SOServer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SOServer - Win32 Release DA" (based on "Win32 (x86) Application")
!MESSAGE "SOServer - Win32 Release AE" (based on "Win32 (x86) Application")
!MESSAGE "SOServer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "SOServer - Win32 Debug DA" (based on "Win32 (x86) Application")
!MESSAGE "SOServer - Win32 Debug AE" (based on "Win32 (x86) Application")
!MESSAGE "SOServer - Win32 Demo" (based on "Win32 (x86) Application")
!MESSAGE "SOServer - Win32 Demo DA" (based on "Win32 (x86) Application")
!MESSAGE "SOServer - Win32 Demo AE" (based on "Win32 (x86) Application")
!MESSAGE "SOServer - Win32 Debug XML DA" (based on "Win32 (x86) Application")
!MESSAGE "SOServer - Win32 Release XML DA" (based on "Win32 (x86) Application")
!MESSAGE "SOServer - Win32 Demo XML DA" (based on "Win32 (x86) Application")
!MESSAGE "SOServer - Win32 Debug Softing" (based on "Win32 (x86) Application")
!MESSAGE "SOServer - Win32 Release Softing" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SOServer - Win32 Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\release"
# PROP Intermediate_Dir "vc6\release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\common" /I "..\..\SOCmn" /I "..\..\SOSrv" /I "..\..\SODaS" /I "..\..\SOAeS" /I "..\..\include" /I "..\..\opc" /D "SODMS_AE" /D "SODMS_DA" /D "SODMS_ALL" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFEATURE_COMBINED_SERVER" /D "SOFLAG_NO_FORCE_LIBS" /D "SOFEATURE_CMN_OPCSERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /D "STRICT" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\release/SOServer.exe"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\releaseDA"
# PROP Intermediate_Dir "vc6\releaseDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\common" /I "..\..\SOCmn" /I "..\..\SOSrv" /I "..\..\SODaS" /I "..\..\SOAeS" /I "..\..\include" /I "..\..\opc" /D "SODMS_DA" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SOFEATURE_CMN_OPCSERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /D "STRICT" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\releaseDA/SOServer.exe"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\releaseAE"
# PROP Intermediate_Dir "vc6\releaseAE"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\common" /I "..\..\SOCmn" /I "..\..\SOSrv" /I "..\..\SODaS" /I "..\..\SOAeS" /I "..\..\include" /I "..\..\opc" /D "SOFLAG_NO_DEFAULT_FEATURES" /D "SOFEATURE_DCOM" /D "SODMS_AE" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SOFEATURE_CMN_OPCSERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /D "STRICT" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\releaseAE/SOServer.exe"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debug"
# PROP Intermediate_Dir "vc6\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\common" /I "..\..\SOCmn" /I "..\..\SOSrv" /I "..\..\SODaS" /I "..\..\SOAeS" /I "..\..\include" /I "..\..\opc" /D "SODMS_AE" /D "SODMS_DA" /D "SODMS_ALL" /D "_DEBUG" /D "SOFLAG_NO_DLLS" /D "SOFEATURE_COMBINED_SERVER" /D "SOFLAG_NO_FORCE_LIBS" /D "SOFEATURE_CMN_OPCSERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /D "STRICT" /Yu"stdafx.h" /FD /GZ /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debug/SOServer.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debugDA"
# PROP Intermediate_Dir "vc6\debugDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\common" /I "..\..\SOCmn" /I "..\..\SOSrv" /I "..\..\SODaS" /I "..\..\SOAeS" /I "..\..\include" /I "..\..\opc" /D "SODMS_DA" /D "_DEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SOFEATURE_CMN_OPCSERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /D "STRICT" /Yu"stdafx.h" /FD /GZ /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debugDA/SOServer.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debugAE"
# PROP Intermediate_Dir "vc6\debugAE"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\common" /I "..\..\SOCmn" /I "..\..\SOSrv" /I "..\..\SODaS" /I "..\..\SOAeS" /I "..\..\include" /I "..\..\opc" /D "SOFLAG_NO_DEFAULT_FEATURES" /D "SOFEATURE_DCOM" /D "SODMS_AE" /D "_DEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SOFEATURE_CMN_OPCSERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /D "STRICT" /Yu"stdafx.h" /FD /GZ /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debugAE/SOServer.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\demo"
# PROP Intermediate_Dir "vc6\demo"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\common" /I "..\..\SOCmn" /I "..\..\SOSrv" /I "..\..\SODaS" /I "..\..\SOAeS" /I "..\..\include" /I "..\..\opc" /D "SODMS_AE" /D "SODMS_DA" /D "SODMS_ALL" /D "SOFLAG_DEMO_VERSION" /D "_DEBUG" /D "SOFEATURE_COMBINED_SERVER" /D "SOFEATURE_CMN_OPCSERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /D "STRICT" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "_DEBUG" /d "SOFLAG_DEMO_VERSION"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\demo/SOServer.exe" /pdbtype:sept /libpath:"..\..\lib\vc6"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\demoDA"
# PROP Intermediate_Dir "vc6\demoDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\common" /I "..\..\SOCmn" /I "..\..\SOSrv" /I "..\..\SODaS" /I "..\..\SOAeS" /I "..\..\include" /I "..\..\opc" /D "SODMS_DA" /D "SOFLAG_DEMO_VERSION" /D "_DEBUG" /D "SOFEATURE_CMN_OPCSERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /D "STRICT" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "_DEBUG" /d "SOFLAG_DEMO_VERSION"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\demoDA/SOServer.exe" /pdbtype:sept /libpath:"..\..\lib\vc6"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\demoAE"
# PROP Intermediate_Dir "vc6\demoAE"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\common" /I "..\..\SOCmn" /I "..\..\SOSrv" /I "..\..\SODaS" /I "..\..\SOAeS" /I "..\..\include" /I "..\..\opc" /D "SODMS_AE" /D "SOFLAG_DEMO_VERSION" /D "_DEBUG" /D "SOFEATURE_CMN_OPCSERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /D "STRICT" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "_DEBUG" /d "SOFLAG_DEMO_VERSION"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\demoAE/SOServer.exe" /pdbtype:sept /libpath:"..\..\lib\vc6"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debugXMLDA"
# PROP Intermediate_Dir "vc6\debugXMLDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\common" /I "..\..\SOCmn" /I "..\..\SOSrv" /I "..\..\SODaS" /I "..\..\SOAeS" /I "..\..\include" /I "..\..\opc" /D "SODMS_DA" /D "_DEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SOFEATURE_CMN_OPCSERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /D "STRICT" /Yu"stdafx.h" /FD /GZ /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debugDA/SOServer.exe" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debugXMLDA/SOServer.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\releaseXMLDA"
# PROP Intermediate_Dir "vc6\releaseXMLDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\common" /I "..\..\SOCmn" /I "..\..\SOSrv" /I "..\..\SODaS" /I "..\..\SOAeS" /I "..\..\include" /I "..\..\opc" /D "SODMS_DA" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SOFEATURE_CMN_OPCSERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /D "STRICT" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\releaseXMLDA/SOServer.exe"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\demoXMLDA"
# PROP Intermediate_Dir "vc6\demoXMLDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\common" /I "..\..\SOCmn" /I "..\..\SOSrv" /I "..\..\SODaS" /I "..\..\SOAeS" /I "..\..\include" /I "..\..\opc" /D "SODMS_DA" /D "SOFLAG_DEMO_VERSION" /D "_DEBUG" /D "SOFEATURE_CMN_OPCSERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /D "STRICT" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "_DEBUG" /d "SOFLAG_DEMO_VERSION"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\demoXMLDA/SOServer.exe" /pdbtype:sept /libpath:"..\..\lib\vc6"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debugSofting"
# PROP Intermediate_Dir "vc6\debugSofting"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\common" /I "..\..\SOCmn" /I "..\..\SOSrv" /I "..\..\SODaS" /I "..\..\SOAeS" /I "..\..\include" /I "..\..\opc" /D "SOFEATURE_TUNNEL" /D "SODMS_AE" /D "SODMS_DA" /D "SODMS_ALL" /D "_DEBUG" /D "SOFLAG_NO_DLLS" /D "SOFEATURE_COMBINED_SERVER" /D "SOFLAG_NO_FORCE_LIBS" /D "SOFEATURE_CMN_OPCSERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /D "STRICT" /Yu"stdafx.h" /FD /GZ /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debugSofting/SOServer.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SOServer___Win32_Release_Softing"
# PROP BASE Intermediate_Dir "SOServer___Win32_Release_Softing"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\releaseSofting"
# PROP Intermediate_Dir "vc6\releaseSofting"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\..\common" /I "..\..\SOCmn" /I "..\..\SOSrv" /I "..\..\SODaS" /I "..\..\SOAeS" /I "..\..\include" /I "..\..\opc" /D "SODMS_AE" /D "SODMS_DA" /D "SODMS_ALL" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFEATURE_COMBINED_SERVER" /D "SOFLAG_NO_FORCE_LIBS" /D "SOFEATURE_CMN_OPCSERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /D "STRICT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\common" /I "..\..\SOCmn" /I "..\..\SOSrv" /I "..\..\SODaS" /I "..\..\SOAeS" /I "..\..\include" /I "..\..\opc" /D "SOFEATURE_TUNNEL" /D "SODMS_AE" /D "SODMS_DA" /D "SODMS_ALL" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFEATURE_COMBINED_SERVER" /D "SOFLAG_NO_FORCE_LIBS" /D "SOFEATURE_CMN_OPCSERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32_DCOM" /D "STRICT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\common" /i "..\..\SOSrv" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\release/SOServer.exe"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\releaseSofting/SOServer.exe"

!ENDIF 

# Begin Target

# Name "SOServer - Win32 Release"
# Name "SOServer - Win32 Release DA"
# Name "SOServer - Win32 Release AE"
# Name "SOServer - Win32 Debug"
# Name "SOServer - Win32 Debug DA"
# Name "SOServer - Win32 Debug AE"
# Name "SOServer - Win32 Demo"
# Name "SOServer - Win32 Demo DA"
# Name "SOServer - Win32 Demo AE"
# Name "SOServer - Win32 Debug XML DA"
# Name "SOServer - Win32 Release XML DA"
# Name "SOServer - Win32 Demo XML DA"
# Name "SOServer - Win32 Debug Softing"
# Name "SOServer - Win32 Release Softing"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\extimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\SOServer.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\SOModule.h
# End Source File
# Begin Source File

SOURCE=.\SOServer.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\SOServer.ico
# End Source File
# Begin Source File

SOURCE=.\SOServer.rc
# End Source File
# Begin Source File

SOURCE=..\..\SOSrv\SOSrvmc.mc

!IF  "$(CFG)" == "SOServer - Win32 Release"

# Begin Custom Build
InputPath=..\..\SOSrv\SOSrvmc.mc
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

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# Begin Custom Build
InputPath=..\..\SOSrv\SOSrvmc.mc
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

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# Begin Custom Build
InputPath=..\..\SOSrv\SOSrvmc.mc
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

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

# Begin Custom Build
InputPath=..\..\SOSrv\SOSrvmc.mc
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

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# Begin Custom Build
InputPath=..\..\SOSrv\SOSrvmc.mc
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

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# Begin Custom Build
InputPath=..\..\SOSrv\SOSrvmc.mc
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

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# Begin Custom Build
InputPath=..\..\SOSrv\SOSrvmc.mc
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

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# Begin Custom Build
InputPath=..\..\SOSrv\SOSrvmc.mc
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

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

# Begin Custom Build
InputPath=..\..\SOSrv\SOSrvmc.mc
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

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

# Begin Custom Build
InputPath=..\..\SOSrv\SOSrvmc.mc
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
# End Group
# Begin Group "SOCmn"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\SOCmn.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnCfg.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnData.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnData.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnElement.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnElement.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnEvent.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnEvent.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnEventPool.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnEventPool.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnException.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnException.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnExpat.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnFunctions.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnFunctions.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnHttp.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnHttp.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnList.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnList.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnObject.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnObject.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnOsCompat.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnSocket.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnSocket.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnSocketSrv.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnString.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnString.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnSync.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnSync.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnThread.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnThread.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnTime.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnTime.h
# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnTP.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnTP.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnTrace.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnTrace.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnVariant.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnVariant.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnXML.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnXML.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# End Group
# Begin Group "SOSrv"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\SOSrv.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOSrv\SOSrvBrowser.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOSrvBrowser.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOSrv\SOSrvComEnum.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOSrv\SOSrvComEnum.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOSrv\SOSrvComObject.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOSrv\SOSrvComObject.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOSrv\SOSrvEntry.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOSrvEntry.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOSrv\SOSrvHttp.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOSrvHttp.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOSrv\SOSrvServer.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOSrvServer.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOSrv\SOSrvTP.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOSrvTP.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOSrv\SOSrvTrace.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOSrvTrace.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOSrv\SOSrvWatch.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOSrvWatch.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOSrv\SOSrvXML.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOSrvXML.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# End Group
# Begin Group "SODaS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\SODaS.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSCache.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaSCache.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSComEnum.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSComEnum.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSComGroup.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSComGroup.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSComServer.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSComServer.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSEntry.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaSEntry.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSGroup.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaSGroup.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSItem.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaSItem.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSItemTag.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaSItemTag.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSNode.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaSNode.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSProperty.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaSProperty.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSRequest.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaSRequest.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSServer.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaSServer.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSTag.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaSTag.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSTPGroup.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSTPGroup.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSTPServer.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSTPServer.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSTransaction.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaSTransaction.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSWatch.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaSWatch.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSXmlServer.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaS\SODaSXmlServer.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# End Group
# Begin Group "SOAeS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\SOAeS.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSArea.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeSArea.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSAreaBrowser.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeSAreaBrowser.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSAttribute.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeSAttribute.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSCategory.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeSCategory.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSComAreaBrowser.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSComAreaBrowser.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSComServer.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSComServer.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSComSubscription.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSComSubscription.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSCondition.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeSCondition.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSEntry.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeSEntry.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSEvent.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeSEvent.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSServer.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeSServer.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSSource.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeSSource.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSSubCondition.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeSSubCondition.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSSubscription.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeSSubscription.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeS\SOAeSWatch.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeSWatch.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# End Group
# Begin Group "SOAsS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\SOAsS.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAsS\SOAsSAttribute.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAsSAttribute.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAsS\SOAsSCondition.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAsSCondition.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAsS\SOAsSEntry.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAsSEntry.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAsS\SOAsSItemTag.cpp

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAsSItemTag.h

!IF  "$(CFG)" == "SOServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Demo XML DA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOServer - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOServer - Win32 Release Softing"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
