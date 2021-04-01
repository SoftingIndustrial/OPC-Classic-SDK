# Microsoft Developer Studio Project File - Name="SOClient" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SOClient - Win32 Debug DA and XMLDA
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SOClient_vc6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SOClient_vc6.mak" CFG="SOClient - Win32 Debug DA and XMLDA"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SOClient - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Release DA" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Release AE" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Debug DA" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Debug AE" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Demo" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Demo DA" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Demo AE" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Debug Softing" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Release Softing" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Debug DCOM" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Release DCOM" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Debug XMLDA" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Release XMLDA" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Debug DA and XMLDA" (based on "Win32 (x86) Application")
!MESSAGE "SOClient - Win32 Release DA and XMLDA" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SOClient - Win32 Release"

# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\release"
# PROP Intermediate_Dir "vc6\release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "SODMC_SOAP" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "SODMC_AE" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 mpr.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\release/SOClient.exe"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\releaseDA"
# PROP Intermediate_Dir "vc6\releaseDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 mpr.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\releaseDA/SOClient.exe"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\releaseAE"
# PROP Intermediate_Dir "vc6\releaseAE"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_AE" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 mpr.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\releaseAE/SOClient.exe"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debug"
# PROP Intermediate_Dir "vc6\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "SODMC_SOAP" /D "_DEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "SODMC_AE" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 mpr.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debug/SOClient.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debugDA"
# PROP Intermediate_Dir "vc6\debugDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "_DEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 mpr.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debugDA/SOClient.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debugAE"
# PROP Intermediate_Dir "vc6\debugAE"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "_DEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_AE" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 mpr.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debugAE/SOClient.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\demo"
# PROP Intermediate_Dir "vc6\demo"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "SODMC_SOAP" /D "SOFLAG_DEMO_VERSION" /D "_DEBUG" /D "SODMC_DA" /D "SODMC_AE" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /d "_DEBUG" /d "SOFLAG_DEMO_VERSION"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 mpr.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\demo/SOClient.exe" /pdbtype:sept /libpath:"..\..\lib\vc6"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\demoDA"
# PROP Intermediate_Dir "vc6\demoDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "SOFLAG_DEMO_VERSION" /D "_DEBUG" /D "SODMC_DA" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /d "_DEBUG" /d "SOFLAG_DEMO_VERSION"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 mpr.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\demoDA/SOClient.exe" /pdbtype:sept /libpath:"..\..\lib\vc6"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\demoAE"
# PROP Intermediate_Dir "vc6\demoAE"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "SOFLAG_DEMO_VERSION" /D "_DEBUG" /D "SODMC_AE" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\..\common" /d "_DEBUG" /d "SOFLAG_DEMO_VERSION"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 mpr.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\demoAE/SOClient.exe" /pdbtype:sept /libpath:"..\..\lib\vc6"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debugSofting"
# PROP Intermediate_Dir "vc6\debugSofting"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SOFEATURE_TUNNEL" /D "SODMC_DCOM" /D "SODMC_SOAP" /D "_DEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "SODMC_AE" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "..\..\common" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 mpr.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debugSofting/SOClient.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SOClient___Win32_Release_Softing"
# PROP BASE Intermediate_Dir "SOClient___Win32_Release_Softing"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\releaseSofting"
# PROP Intermediate_Dir "vc6\releaseSofting"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "SODMC_SOAP" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "SODMC_AE" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SOFEATURE_TUNNEL" /D "SODMC_DCOM" /D "SODMC_SOAP" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "SODMC_AE" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\common" /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\common" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 mpr.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\release/SOClient.exe"
# ADD LINK32 mpr.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\releaseSofting/SOClient.exe"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SOClient___Win32_Debug_DCOM"
# PROP BASE Intermediate_Dir "SOClient___Win32_Debug_DCOM"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debugDCOM"
# PROP Intermediate_Dir "vc6\debugDCOM"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "SODMC_SOAP" /D "_DEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "SODMC_AE" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "_DEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "SODMC_AE" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\common" /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\common" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 mpr.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debug/SOClient.exe" /pdbtype:sept
# ADD LINK32 mpr.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debugDCOM/SOClient.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SOClient___Win32_Release_DCOM"
# PROP BASE Intermediate_Dir "SOClient___Win32_Release_DCOM"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\releaseDCOM"
# PROP Intermediate_Dir "vc6\releaseDCOM"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "SODMC_SOAP" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "SODMC_AE" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "SODMC_AE" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\common" /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\common" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 mpr.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\release/SOClient.exe"
# ADD LINK32 mpr.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\releaseDCOM/SOClient.exe"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SOClient___Win32_Debug_XMLDA"
# PROP BASE Intermediate_Dir "SOClient___Win32_Debug_XMLDA"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debugXMLDA"
# PROP Intermediate_Dir "vc6\debugXMLDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "_DEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_SOAP" /D "_DEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\common" /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\common" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 mpr.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debugDA/SOClient.exe" /pdbtype:sept
# ADD LINK32 mpr.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debugXMLDA/SOClient.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SOClient___Win32_Release_XMLDA"
# PROP BASE Intermediate_Dir "SOClient___Win32_Release_XMLDA"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\releaseXMLDA"
# PROP Intermediate_Dir "vc6\releaseXMLDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_SOAP" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\common" /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\common" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 mpr.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\releaseDA/SOClient.exe"
# ADD LINK32 mpr.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\releaseXMLDA/SOClient.exe"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SOClient___Win32_Debug_DA_and_XMLDA"
# PROP BASE Intermediate_Dir "SOClient___Win32_Debug_DA_and_XMLDA"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc6\debugDAXMLDA"
# PROP Intermediate_Dir "vc6\debugDAXMLDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "_DEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "SODMC_SOAP" /D "_DEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\common" /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\common" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 mpr.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debugDA/SOClient.exe" /pdbtype:sept
# ADD LINK32 mpr.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc6\debugDAXMLDA/SOClient.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SOClient___Win32_Release_DA_and_XMLDA"
# PROP BASE Intermediate_Dir "SOClient___Win32_Release_DA_and_XMLDA"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc6\releaseDAXMLDA"
# PROP Intermediate_Dir "vc6\releaseDAXMLDA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\include" /I "..\..\opc" /I "..\..\common" /D "SODMC_DCOM" /D "SODMC_SOAP" /D "NDEBUG" /D "SOFLAG_NO_DLLS" /D "SOFLAG_NO_FORCE_LIBS" /D "SODMC_DA" /D "_WIN32_DCOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\..\common" /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\common" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 mpr.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\releaseDA/SOClient.exe"
# ADD LINK32 mpr.lib /nologo /subsystem:windows /machine:I386 /out:"vc6\releaseDAXMLDA/SOClient.exe"

!ENDIF 

# Begin Target

# Name "SOClient - Win32 Release"
# Name "SOClient - Win32 Release DA"
# Name "SOClient - Win32 Release AE"
# Name "SOClient - Win32 Debug"
# Name "SOClient - Win32 Debug DA"
# Name "SOClient - Win32 Debug AE"
# Name "SOClient - Win32 Demo"
# Name "SOClient - Win32 Demo DA"
# Name "SOClient - Win32 Demo AE"
# Name "SOClient - Win32 Debug Softing"
# Name "SOClient - Win32 Release Softing"
# Name "SOClient - Win32 Debug DCOM"
# Name "SOClient - Win32 Release DCOM"
# Name "SOClient - Win32 Debug XMLDA"
# Name "SOClient - Win32 Release XMLDA"
# Name "SOClient - Win32 Debug DA and XMLDA"
# Name "SOClient - Win32 Release DA and XMLDA"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\extimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\SOClient.cpp
# End Source File
# Begin Source File

SOURCE=.\SOClient.rc
# End Source File
# Begin Source File

SOURCE=.\SODmCEvent.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SODmCGroup.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SODmCItem.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SODmCMainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\SODmCMainView.cpp
# End Source File
# Begin Source File

SOURCE=.\SODmCMultiWriteDlg.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SODmCNameSpaceDA.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SODmCObjectView.cpp
# End Source File
# Begin Source File

SOURCE=.\SODmCPropFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\SODmCServerAE.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SODmCServerDA.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SODmCSubscription.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SOClient.h
# End Source File
# Begin Source File

SOURCE=.\SODmCEvent.h
# End Source File
# Begin Source File

SOURCE=.\SODmCGroup.h
# End Source File
# Begin Source File

SOURCE=.\SODmCItem.h
# End Source File
# Begin Source File

SOURCE=.\SODmCMainFrm.h
# End Source File
# Begin Source File

SOURCE=.\SODmCMainView.h
# End Source File
# Begin Source File

SOURCE=.\SODmCMultiWriteDlg.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SODmCNameSpaceDA.h
# End Source File
# Begin Source File

SOURCE=.\SODmCObjectView.h
# End Source File
# Begin Source File

SOURCE=.\SODmCPropFrm.h
# End Source File
# Begin Source File

SOURCE=.\SODmCServerAE.h
# End Source File
# Begin Source File

SOURCE=.\SODmCServerDA.h
# End Source File
# Begin Source File

SOURCE=.\SODmCSubscription.h
# End Source File
# Begin Source File

SOURCE=.\SOModule.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\mainImg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\objectImg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\propfram.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SOClient.ico
# End Source File
# Begin Source File

SOURCE=.\res\SOClient.rc2
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "SODaC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\SODaC.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCBrowse.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaCBrowse.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCCallback.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaCCallback.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCComBrowse.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaCComBrowse.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCComGroup.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaCComGroup.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCComServer.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaCComServer.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCEntry.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaCEntry.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCGroup.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaCGroup.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCItem.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaCItem.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCServer.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaCServer.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCTPBrowse.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaCTPBrowse.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCTPCall.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCTPCall.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCTPGroup.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaCTPGroup.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCTPServer.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaCTPServer.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCXmlBrowse.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaCXmlBrowse.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCXmlCall.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCXmlCall.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCXmlGroup.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaCXmlGroup.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SODaC\SODaCXmlServer.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SODaCXmlServer.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# End Group
# Begin Group "SOCmn"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\SOCmn\opcae_er.mc

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\opcerror.mc

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmn.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnData.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnData.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnElement.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnElement.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnEvent.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnEvent.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnEventPool.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnEventPool.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnException.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnException.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnExpat.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnFunctions.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnFunctions.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnHttp.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnHttp.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnList.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnList.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnObject.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnObject.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnSocket.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnSocket.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnSocketClt.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnString.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnString.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnSync.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnSync.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnThread.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnThread.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnTime.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnTime.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnTP.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnTP.h
# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnTrace.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnTrace.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnVariant.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnVariant.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOCmn\SOCmnXML.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCmnXML.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# End Group
# Begin Group "SOClt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\SOClt.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOClt\SOCltBrowse.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCltBrowse.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOClt\SOCltComServer.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCltComServer.h
# End Source File
# Begin Source File

SOURCE=..\..\SOClt\SOCltElement.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCltElement.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOClt\SOCltEntry.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCltEntry.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCltError.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOClt\SOCltError.mc

!IF  "$(CFG)" == "SOClient - Win32 Release"

# Begin Custom Build
InputPath=..\..\SOClt\SOCltError.mc
InputName=SOCltError

BuildCmds= \
	mc -u -U -c $(InputPath) -h ..\..\include

"MSG00409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG00407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# Begin Custom Build
InputPath=..\..\SOClt\SOCltError.mc
InputName=SOCltError

BuildCmds= \
	mc -u -U -c $(InputPath) -h ..\..\include

"MSG00409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG00407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# Begin Custom Build
InputPath=..\..\SOClt\SOCltError.mc
InputName=SOCltError

BuildCmds= \
	mc -u -U -c $(InputPath) -h ..\..\include

"MSG00409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG00407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

# Begin Custom Build
InputPath=..\..\SOClt\SOCltError.mc
InputName=SOCltError

BuildCmds= \
	mc -u -U -c $(InputPath) -h ..\..\include

"MSG00409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG00407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# Begin Custom Build
InputPath=..\..\SOClt\SOCltError.mc
InputName=SOCltError

BuildCmds= \
	mc -u -U -c $(InputPath) -h ..\..\include

"MSG00409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG00407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# Begin Custom Build
InputPath=..\..\SOClt\SOCltError.mc
InputName=SOCltError

BuildCmds= \
	mc -u -U -c $(InputPath) -h ..\..\include

"MSG00409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG00407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

# Begin Custom Build
InputPath=..\..\SOClt\SOCltError.mc
InputName=SOCltError

BuildCmds= \
	mc -u -U -c $(InputPath) -h ..\..\include

"MSG00409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG00407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

# Begin Custom Build
InputPath=..\..\SOClt\SOCltError.mc
InputName=SOCltError

BuildCmds= \
	mc -u -U -c $(InputPath) -h ..\..\include

"MSG00409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG00407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

# Begin Custom Build
InputPath=..\..\SOClt\SOCltError.mc
InputName=SOCltError

BuildCmds= \
	mc -u -U -c $(InputPath) -h ..\..\include

"MSG00409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG00407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

# Begin Custom Build
InputPath=..\..\SOClt\SOCltError.mc
InputName=SOCltError

BuildCmds= \
	mc -u -U -c $(InputPath) -h ..\..\include

"MSG00409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG00407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# Begin Custom Build
InputPath=..\..\SOClt\SOCltError.mc
InputName=SOCltError

BuildCmds= \
	mc -u -U -c $(InputPath) -h ..\..\include

"MSG00409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG00407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# Begin Custom Build
InputPath=..\..\SOClt\SOCltError.mc
InputName=SOCltError

BuildCmds= \
	mc -u -U -c $(InputPath) -h ..\..\include

"MSG00409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG00407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# Begin Custom Build
InputPath=..\..\SOClt\SOCltError.mc
InputName=SOCltError

BuildCmds= \
	mc -u -U -c $(InputPath) -h ..\..\include

"MSG00409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG00407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# Begin Custom Build
InputPath=..\..\SOClt\SOCltError.mc
InputName=SOCltError

BuildCmds= \
	mc -u -U -c $(InputPath) -h ..\..\include

"MSG00409.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG00407.bin" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOClt\SOCltHttp.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCltHttp.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOClt\SOCltServer.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCltServer.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOClt\SOCltServerBrowser.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCltServerBrowser.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOClt\SOCltShutdown.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCltShutdown.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOClt\SOCltStorage.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCltStorage.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOClt\SOCltTask.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCltTask.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOClt\SOCltThread.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCltThread.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOClt\SOCltTP.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOCltTP.h
# End Source File
# End Group
# Begin Group "SOAeC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\SOAeC.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeC\SOAeCBrowse.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeCBrowse.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeC\SOAeCCallback.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeCCallback.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeC\SOAeCEntry.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeCEntry.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeC\SOAeCEvent.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeCEvent.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeC\SOAeCServer.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeCServer.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SOAeC\SOAeCSubscription.cpp

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\SOAeCSubscription.h

!IF  "$(CFG)" == "SOClient - Win32 Release"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug AE"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo DA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Demo AE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release Softing"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DCOM"

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Debug DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SOClient - Win32 Release DA and XMLDA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "GridControl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GridCtrl_SRC\BtnDataBase.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\BtnDataBase.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\CellRange.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridBtnCell.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridBtnCell.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridBtnCellBase.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridBtnCellBase.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridCell.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridCell.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridCellBase.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridCellBase.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridCellCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridCellCheck.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridCellCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridCellCombo.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridCellNumeric.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridCellNumeric.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridCtrl.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\GridDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\InPlaceEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\InPlaceEdit.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\TitleTip.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl_SRC\TitleTip.h
# End Source File
# End Group
# End Target
# End Project
