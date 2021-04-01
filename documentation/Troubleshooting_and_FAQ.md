## **Troubleshooting and FAQ**

### "No content was found on your computer" when using F1 within VS2010


To solve this problem follow the following steps:

-   Select **Help** menu item from VS2010 and select **Manage Help Settings**
    
-   Press OK button for using the default Library Location.
    
-   Run **Integrate Documentation into VS2010 help** batch file from Start menu/Softing OPC/Toolkit V44x

### Recompile the toolkit binary components if your Visual Studio version is different by the ones listed below.

The delivered toolkit binaries are compiled using the Visual Studio versions below:

**Development Environment**|**Version**
:-----:|:-----:
|Microsoft Visual Studio 2003|7.1.6030
|Microsoft Visual Studio 2005|8.0.50727.42
|Microsoft Visual Studio 2008|9.0.30729.1
|Microsoft Visual Studio 2010|10.0.40219.1
|Microsoft Visual Studio 2012|11.0.50727.1
|Microsoft Visual Studio 2013|12.0.21005.1
|Microsoft Visual Studio 2015|14.0.23107.0
|Microsoft Visual Studio 2017|15.4.1

If you have another Visual Studio version installed on your computer we suggest to recompile the toolkit binaries and use the resulted libraries instead of the installed ones. Together with the Toolkit binaries you should use the appropriate runtime binary files in your application installation.

### Note
```
For VS 2005, VS 2008, VS 2010, VS 2012, VS 2013, VS 2015 and VS 2017 the debug version delivered binaries 
(TBCVS2005D.dll, TBCVS2005uD.dll, TBCVS2008D.dll, TBCVS2008uD.dll, TBCVS2008Dx64.dll, 
TBCVS2008uDx64.dll,TBCVS2010D.dll, TBCVS2010uD.dll, TBCVS2010Dx64.dll, TBCVS2010uDx64.dll, 
TBCVS2012D.dll, TBCVS2012uD.dll, TBCVS2012Dx64.dll, TBCVS2012uDx64.dll, TBCVS2013D.dll, 
TBCVS2013uD.dll, TBCVS2013Dx64.dll, TBCVS2013uDx64.dll, TBCVS2015D.dll, TBCVS2015uD.dll, 
TBCVS2015Dx64.dll, TBCVS2015uDx64.dll, TBCVS2017D.dll, TBCVS2017uD.dll, TBCVS2017Dx64.dll, 
TBCVS2017uDx64.dll) are compiled without the manifest embedded in order to prevent the application startup 
failure when different debug runtime binaries are used on your PC. We strongly recommend to recompile above 
mentioned binaries (both debug and release) on your own environment and enable the manifest embedding.
```

### Note
```
The delivered OTB.dll respective OTBu.dll (delivered in ..\windows\bin\Legacy) are compiled with Visual Studio 
2003 in release configuration and can be used without any modifications, except the case when the source code 
license is used. These libraries are compiled on x86 target platform and can be used only for 32 bit applications.
```

### After finishing the compilation of an application with Microsoft Visual C++ 2005 an initialization error appears when loading the TBCVS2005.dll

It might look like this: 'LDR: LdrpWalkImportDescriptor() failed to probe c:\program files\softing\OpcToolkit\V44x\windows\c++\samples\client\daconsole\bin\vs2005\win32\release\TBCVS2005.dll for its manifest, ntstatus 0xc0150002'

To solve this problem try to recompile the toolkit on your machine and then use the resulted 'TBCVS2005.dll' for your application.


### Debugging OPC Toolkit core libraries in .NET based applications

In order to debug the core binaries you have to switch ON the **Enable unmanaged code debugging** option.

**The OPC server starts twice: first time started from the Windows Explorer or Visual Studio and second time started by the client application.**

This is a common DCOM problem caused by starting the server by different users (one from the user and one from the system). In order to prevent this, please configure the DCOM settings according to the [DCOM Configuration article](6674f8ee-0d1c-484c-afdb-26d7231e089e.htm)


### Registering an OPC Server fails.

Registration of an OPC server demands 'Administrator' rights.

###  Registering a Windows Service fails.

Registration of a Windows service demands 'Administrator' rights.

### Connecting to a server that runs as service fails. The received error message is "Access denied".

To solve the problem check your COM security settings. Read about setting the right DCOM configuration in the [DCOM Configuration article](6674f8ee-0d1c-484c-afdb-26d7231e089e.htm).


### Browsing the INAT server returns incorrect ItemIds.

When browsing the INAT server a special property must be set (.NET) / a method must be called (C++) in order to have a correct browsing. It is about the ForceBrowseUp property (.NET) / setForceBrowseUp() method (C++). They must be set for an instance of the AddressSpaceElementBrowseOptions class before the browsing is started.

### Compiling a Microsoft Visual C++ Express Editions 2005, 2008, 2010, 2012, 2013, 2015 and 2017.

To solve this, follow the steps:

-   Install Microsoft Platform SDK on your computer
    
-   Configure your Visual C++ Express to include files from the "Include" directory of your newly installed Platform SDK. (Go to Tools->Options...->Project and Solutions->VC++ Directories. In the "Show Directories" comboBox select the "Include files" option. Create a new line that contains the path to the "Include" directory of your newly installed Platform SDK.)
    
-   Configure your Visual C++ Express to include files from the "Lib" directory of your newly installed Platform SDK. (Go to Tools->Options...->Project and Solutions->VC++ Directories. In the "Show Directories" comboBox select the "Library files" option. Create a new line that contains the path to the "Lib" directory of your newly installed Platfom SDK.)
    
-   Add to your project the "ole32.lib", "user32.lib" and "advapi32.lib" as additional dependencies. (Go to Project Properties->Linker->Input. On the "Additional Dependencies" line add the "ole32.lib","user32.lib" and "advapi32.lib" libraries.)



### Custom build step in server projects fails using VS2003 due to missing DLLs.

The server registration during the custom build step requires the Softing Toolkit DLLs and if these DLLs are not available the start of the server application will fail. These DLLs are available through the $(PATH) variable of the Windows installation and are required during the server registration. To solve this problem check the settings in your Microsoft Visual Studio 2003 installation (Menu Tools - Options, select Folder "Projects" and item "VC++ Directories" / "Directories for executable files") and add $(Path) to the list of variables. Later Visual Studio versions (2005, 2008 and 2010) add $(Path) by default during the installation to that list.


### Custom build step in client projects fails due to linking errors.

It might look like this:

'OpcClient.obj : error LNK2005: "private: static class SoftingOPCToolboxClient::Application * SoftingOPCToolboxClient::Application::m_instance" (?m_instance@Application@SoftingOPCToolboxClient@@0PAV12@A) already defined in Console.obj'

The problem can be caused by the include of the header file "ClientCommon.h" in another header file. "ClientCommon.h" is only meant to be included in .cpp files where necessary. To solve this problem, "ClientCommon.h" should be moved from the .h file to .cpp file.


### Windows Vista / Windows 7 with UAC (User Access Control) enabled.

The registration and unregistration processes of an DCOM based OPC server affects the registry database therefore someone may experience difficulties on Windows Vista / Windows 7. When UAC is enabled OPC server's registration respective unregistration demands to be **Run As Administrator** also by users with administrator rights.

The registration respective unregistration of an OPC server will be done by running the application with /regserver respective /unregserver argument. Usually in OPC generated projects registration is performed via the project's post build event. In Vista / 7 with UAC enabled this will not work. There are two options to do the registration:

-   **A cmd file** - containing registration command _yourOPCServer.exe /regserver_ started by means of Run As Administrator context menu.
    
    For a .net OPC project generated with the wizard, a _PostBuildEvent.bat_ file is automatically generated at compilation. In this case this file can be used for Run As Administrator and no new batch file is necessary.
    
-   **runas batch command** - start the _yourOPCServer.exe /regserver_ by means of the **runas** shell command
    
    E.g.: _runas /user:administrator "C:\OPC\yourOPCServer.exe /regserver"_


### Initializing the Toolkit with signed DLLs (TBN, TBC, OTB and other DLL/EXE files) takes longer than with unsigned binaries

The delay is related to the fact that the OS has to connect to the internet several times in order to download a list of revoked certificates. Related to [MSDN article](http://msdn.microsoft.com/en-us/library/cc656914.aspx)



### The program can't start because TB?VS20??.dll / OTB*.dll is missing from your computer

Copying an application which uses Softing OPC Classic Toolkit from a PC to another (or even from a folder to another on the same PC) requires the user to also copy the required libraries - namely the Softing OPC Classic Toolkit CORE (OTB*.dll) and the Softing OPC Classic Toolkit API (TB?VS20?.dll).

### The .NET application will not build any more after upgrading from Toolbox V4.3x to Toolkit V4.40

Starting with Softing OPC Toolkit version 4.40, the signature of many .NET API function has changed (the data type of a single parameter). Namely, the parameter corresponding to the object handle has changed its data type from IntPtr to uint. This modification was required due to the native 64-bit support added to the Softing OPC Toolkit.

Troubleshooting and FAQ

### Why don`t the .NET Server Samples have the AnyCPU application configuration although the .NET API and the .NET Client Samples do have it?

There seems to be an incompatibility problem between AnyCPU .NET OPC Servers and Microsoft Windows DCOM. The problem is caused by the way the AnyCPU applications start on x64 systems: when the AnyCPU OPC Server is started (on a 64-bit operating system), a 32-bit process is initially created; once the AnyCPU OPC Server detects that the OS supports 64-bit processes, it just-in-time-switches to a 64-bit process. Windows DCOM is not very happy with this fact (JIT 32-to-64-bit switching), as it loses the reference (handle) to the 32-bit process and no communication will take place between the OPC Client and the OPC Server (although the OPC Server will correctly register with DCOM).

This problem will not occur if the AnyCPU OPC Server is run under a 32-bit OS.

The solution to this problem is to have the OPC Server be compiled as native x64 application for 64-bit operating systems and as native x86 application for 32-bit operating systems.

The OPC Clients have no problem with the AnyCPU application configuration as Windows DCOM is only instantiated after the process has successfully started.

### DaClient: removing many DaItems from a subscription causes performance loss

If a large number of items is to be removed, it is recommended to first disconnect the subscription, remove the required items and then reconnect the subscription. Depending on the OPC DA protocol used for connecting to the OPC DA Server, if this recommendation is not followed, the OPC DA Client built with Softing OPC Toolkit will: issue as many individual "remove DA Item" calls to the OPC DA Server as items are removed (if the OPC Client connects over OPCDA to the OPC Server);disconnect the subscription, remove the respective DA Item and reconnect the subscription as many times as items are removed from the subscription (if the OPC Client connects over XMLDA to the OPC Server).

### "missing afxres.h" while compiling Wizard-generated C++ projects or provided C++ samples (Visual Studio Express)

The C++ projects generated by Project Wizard along with the provided client / server samples include the afxres.h header in their resource files. This header is a part of the MFC Library and unfortunately Microsoft Visual Studio Express editions do not contain MFC. If the project doesn't use MFC, afxres.h may be safely replaced with windows.h.

### "error C1083: Cannot open include file: 'winsdkver.h'" while compiling Wizard-generated C++ projects or provided C++ samples (Visual Studio 2013)

Visual Studio 2013 does not deploy the entire Win8.1 SDK, even if a full installation is performed, therefore it is required to manually download and install the Microsoft Visual Studio 2013 SDK from [Microsoft Visual Studio 2013 SDK](http://www.microsoft.com/en-us/download/details.aspx?id=40758) (published: 11th of October 2013, size: 13.1MB)

### This version of Visual Studio requires a computer with a newer version of Windows (error when installing VS2013 on Windows7 or earlier versions of Windows operating systems)

Visual Studio 2013 requires at least Microsoft Windows 7 operating system with Service Pack 1 installed. The officially supported operating systems are: Windows 8.1 (x86 and x64), Windows 8 (x86 and x64), Windows 7 SP1 (x86 and x64), Windows Server 2012 R2 (x64), Windows Server 2012 (x64) and Windows Server 2008 R2 SP1 (x64). For more details please see [Visual Studio 2013 Compatibility](http://www.visualstudio.com/en-us/products/visual-studio-2013-compatibility-vs.aspx)

Please note that in the OPC Toolkit version V4.47.0 (12th of December 2019), the support for operating systems Windows 8.1 (x86 and x64), Windows 8 (x86 and x64), Windows Server 2012 R2 (x64) and Windows Server 2012 (x64) is removed.

### "error MSB8031: Use of MBCS encoding in MFC projects require an additional library to be downloaded and installed" while compiling Wizard-generated C++ projects or provided C++ samples which use MFC under ANSI character sets (Visual Studio 2013)

Starting with Visual Studio 2013 the support for Multi-Byte Character Sets has been discontinued. See [Support for Multibyte Character Sets (MBCSs)](http://msdn.microsoft.com/en-us/library/5z097dxa.aspx) for more details. However, it is still possible to create MFC applications which use ANSI character sets, but it is required to manually download and install the Multibyte MFC Library for Visual Studio 2013 from [Multibyte MFC Library for Visual Studio 2013](http://www.microsoft.com/en-gb/download/details.aspx?id=40770) (published: 11th of October 2013, size: 64.3MB)

### "warning MSB8003: Could not find WindowsSDKDir variable from the registry. TargetFrameworkVersion or PlatformToolset may be set to an invalid version number." / "error MSB6006: 'cmd.exe' exited with code 9009." / "error C1083: Cannot open include file: 'windows.h': No such file or directory" while compiling Softing OPC Classic Toolkit CORE (Visual Studio 2013)

Visual Studio 2013 installation does not deploy Windows SDK for Windows v8.1. Download and install the Microsoft Windows 8.1 SDK from [Windows Software Development Kit (SDK) for Windows 8.1](http://msdn.microsoft.com/en-us/windows/desktop/bg162891.aspx)

Please note that in the OPC Toolkit version V4.47.0 (12th of December 2019), the support for operating systems Windows 8.1 (x86 and x64), Windows 8 (x86 and x64), Windows Server 2012 R2 (x64) and Windows Server 2012 (x64) is removed.

It is also possible that the TargetFrameworkVersion or PlatformToolset of the project to be set to an invalid version number (if you have modified the project). Please make sure that Visual Studio 2013 is set in the project Properties - Configuration Properties - General - Platform Toolset.

Another cause for this misleading error is that the Windows SDK Registry variable may not be found. You will have to manually add/modify the Windows SDK variable inside Windows Registry:

```
Windows Registry Editor Version 5.00

[HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Microsoft SDKs\Windows]
"CurrentInstallFolder"="C:\\Program Files (x86)\\Windows Kits\\8.1\\"
```

### OPC Toolkit based applications running as Windows Service

The applications built with the Softing OPC Toolkit which have a Windows Service architecture have the startup type implicitly set to manual.

### Note
```
A different startup mode may be selected for the Windows Service-registered OPC Client/Server applications by 
navigating to the Services Microsoft Management Console (Control Panel -> Administrative Tools -> Computer 
Management -> Services and Applications -> Services).
```
It is not possible to set a service description to the Softing OPC Toolkit based Client/Server applications which are designed to be Windows Services.


### Error MSB3073: The command "copy ..\..\..\..\bi[...] exited with code -1073741515.

The samples Softing OPC Classic Toolkit deploys copy the required libraries for each build configuration every time the project is built. Considering that Softing OPC Classic Toolkit can be installed in any folder and also that "Program Files" folder naming differs according to operating system ("Programme" on German Windows installations, "Program Files (x86)" on 64-bit Windows installations), the paths to the required libraries are relative.

### Note
```
If the samples are copied/moved from the original Program Files Installation folder, it is solely the end user's 
responsibility to adjust the project`s post-build events and manually copy the required binaries, if the case.
```

Softing OPC Classic Toolkit Project Wizard automatically adds full paths to the generated projects since the Softing OPC Classic Toolkit is already installed. Nevertheless, should a Softing OPC Classic Toolkit installation be upgraded and the subsequent version installed elsewhere than the previous version, all the previously Project Wizard-generated projects need to be manually updated.

### Note
```
The Softing OPC Classic Toolkit's "bin" folder is no longer added to System Environment Variables (under PATH) 
from version 4.40 since different Visual Studios required binaries stored in different folders ("bin" or 
"bin\Legacy").
```

### Refreshing a connected subscription or an active subscription with no active items fails with 0x80004005 (E_FAIL)

According to the OPC DA Specifications V2.05a, the OPC DA Server receiving an IOPCAsyncIO2::Refresh call for an inactive subscription (group) or an active subscription (group) with no active items should return the error E_FAIL to the caller OPC DA Client.

### Note
```
It is not possible to refresh an inactive subscription or an active subscription with no active items.
```
### Source code licenses are not upgraded when performing a Softing OPC Toolkit upgrade

When installing a newer version of Softing OPC Toolkit, the newer source code installers will no longer be deployed and installed since it may cause incompatibility problems with previous source code or it may be the case that customer-operated changes may be overwritten. It is therefore necessary for the customer to manually perform the source code license upgrade, by activating the purchased source code licenses again in the License Manager application.


### Access Denied when performing a Softing OPC Toolkit upgrade from 43x to 44x

When upgrading Softing OPC Toolkit from 43x to 44x, the new Toolkit will use different DCOM values. The old (43x) Authentication Level and Impersonation Level settings are None and respectively Impersonate, and the new settings are Connect and respectively Identify. It is therefore necessary for the customer to manually perform the DCOM values update, should this be required. Please configure the DCOM settings according to the [DCOM Configuration article](6674f8ee-0d1c-484c-afdb-26d7231e089e.htm). It is highly recommended to use the new, more secure DCOM settings.

### RPC_E_TOO_LATE (0x80010119 / -2147417831) error messages while initializing DCOM

Client/Server vendor should move the trace initialization BEFORE toolkit application initialization to inspect these problems. If the error code RPC_E_TOO_LATE appears each time CoInitializeSecurity is called, an additional component (usually from .NET) initializes DCOM before Softing OPC Toolkit is able to (as some simple, local COM object is required, yet DCOM must be initialized before being able to use the COM object, therefore DCOM is usually initialized by the respective component with default system settings). Changing System-wide DCOM settings to the required DCOM Settings (usually 43x settings - None / Impersonate - again: this is not very secure) will result in the same errors being traced but the Toolkit itself will work. Instead of doing that, vendor should import CoInitializeEx and CoInitializeSecurity methods from OLE32 system library and call them on the first line of their application with the required settings.

### C#
```
[System.Runtime.InteropServices.DllImport("Ole32.dll", ExactSpelling = true, EntryPoint - "CoInitializeSecurity", CallingConvention = System.Runtime.InteropServices.CallingConvention.StdCall, SetLastError = false, PreserveSig = false)]
public static extern Int32 CoInitializeSecurity(IntPtr pSecDesc, Int32 cAuthSvc, IntPtr as AuthSvc, IntPtr pReserved1, UInt32 dwAuthnLevel, UInt32 dwImpLevel, IntPtr pAuthList, UInt32 dwCapabilities, IntPtr pReserved3);

[System.Runtime.InteropServices.DllImport("Ole32.dll", ExactSpelling = true, EntryPoint - "CoInitializeEx", CallingConvention = System.Runtime.InteropServices.CallingConvention.StdCall, SetLastError = false, PreserveSig = false)]
public static extern Int32 CoInitializeEx(IntPtr pReserved, UInt32 dwCoInit);

// The main entry point for the process
static void Main()
{
    try
    {
        int dwResCoInitialize = CoInitializeEx(IntPtr.Zero, 0 /* COINIT_MULTITHREADED = 0 */);
        int dwResCoInitializeSecurity = CoInitializeSecurity(IntPtr.Zero, -1, IntPtr.Zero, IntPtr.Zero, 2 /* RPC_C_AUTHN_LEVEL_CONNECT = 2 */, 2 /* RPC_C_IMP_LEVEL_IDENTIFY = 2 */, IntPtr.Zero, 0 /* EOAC_NONE = 0 */, IntPtr.Zero);

        // for Legacy DCOM settings, please replace the above CoInitializeSecurity line with the following one:
        int dwResCoInitializeSecurity = CoInitializeSecurity(IntPtr.Zero, -1, IntPtr.Zero, IntPtr.Zero, 1 /* RPC_C_AUTHN_LEVEL_NONE = 1 */, 2 /* RPC_C_IMP_LEVEL_IDENTIFY = 2 */, IntPtr.Zero, 0 /* EOAC_NONE = 0 */, IntPtr.Zero);
```

### DCOM-related problems starting with Toolkit Classic V4.4x

Starting with version 4.40 of the Softing OPC Classic Toolkit, the DCOM settings have been upgraded to support very secure connections. By default secure connections are enforced instead of unsecured connections. The difference is in both OPCDA/OPCAE clients and servers, in the initial CoInitializeSecurity call and also in the IClientSecurity::QueryBlanket / SetBlanket security proxy blanket negotiation.

The new DCOM settings used in the Softing OPC Classic Toolkit are RPC_C_AUTHN_LEVEL_CONNECT / RPC_C_IMP_LEVEL_IDENTIFY. These settings are more secure and should be used instead of the pre-V4.40 legacy settings. Please also follow the instructions in the [DCOM Configuration](6674f8ee-0d1c-484c-afdb-26d7231e089e.htm) to maximize the security of the machine.

Softing OPC Classic Toolbox V4.3x and earlier employ legacy DCOM settings - the RPC_C_AUTHN_LEVEL_NONE / RPC_C_IMP_LEVEL_IDENTIFY are used for both OPCDA/OPCAE Client and Server Applications. Although no support is offered anymore for these settings, they are provided for convenience reasons and might repair DCOM security incompatibility between various remote OPCDA/AE Client and/or Server applications.

### Failed to advise IOPCDataCallback connection (0xE00004B3), access denied (0x8007005)

For more information, please see the DA Console sample in the [Client](17df0506-719e-4a6c-b542-c1a098d7cb9b.htm) Programming Samples.

### Unable to load DLL 'OTBu.dll': The specified module could not be found. (Exception from HRESULT: 0x8007007E)

For more information, please see the Data Control sample in the [Client](17df0506-719e-4a6c-b542-c1a098d7cb9b.htm) Programming Samples.