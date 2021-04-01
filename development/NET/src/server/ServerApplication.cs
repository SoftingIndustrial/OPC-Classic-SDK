using System;
using System.Collections;
using System.Runtime.InteropServices;
using Softing.OPCToolbox;
using Softing.OPCToolbox.OTB;
using System.IO;
using System.Reflection;

namespace Softing.OPCToolbox.Server
{

	#region // Public Delegates
	//-------------------------

	/// <summary>
	/// <para>
	/// This will be called when the server life is ended. This is only happening when the server has been started by
	/// a client and the last client has closed the connection to the server.
	/// </para>
	/// <para><b>Unmanaged C++</b><br/>
	/// Delegate is not available for Unmanaged C++<br/>
	/// A function pointer definition is used for the callback receive purpose in C++<br/>
	/// typedef long (API_CALL *ShutdownRequestHandler)(void);
	/// </para>
	/// <para>Callback triggered from within the OPC Toolkit internals when the OPC server needs to shutdown
	/// </para>
	/// </summary>
	public delegate int ShutdownHandler();

	//-

	#endregion

	/// <summary>
	/// The Application class singleton provides the main functionality of the OPC server interface
	/// </summary>
	/// <include
	///  file='TBNS.doc.xml'
	///  path='//class[@name="Application"]/doc/*'
	/// />
	public sealed class Application
	{
		#region //	Public Singleton Accessor
		//-----------------------------------

		/// <summary>
		/// Singleton OPC server's instance public accessor
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/method[@name="Instance"]/doc/*'
		/// />
		public static Application Instance = new Application();

		///<summary>
		///<para><b>Unmanaged C++</b>Releases all the resources allocated by the application.</para>
		///<para><b>C#, Visual Basic, C++</b><br/> The method is not available.</para>
		///</summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/method[@name="Release"]/doc/*'
		/// />
		public void Release()
		{
			Instance.Terminate();
			Instance = null;
		} //	end Release


		//-
		#endregion

		#region //	Public Events
		//-----------------------

		/// <summary>
		/// Triggered by the toolkit internal shutdown sequence. E.g. when the last session was closed by the last OPC client.
		/// Normally this will terminate the server application
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  event[@name="ShutdownRequest"]/doc/*'
		/// />
		public event ShutdownHandler ShutdownRequest = null;

		/// <summary>
		/// Triggered for each trace output line made by the OPC server application.
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  event[@name="TraceOutput"]/doc/*'
		/// />
		public event TraceEventHandler TraceOutput = null;

		//-
		#endregion

		#region //	Public Properties
		//---------------------------

		/// <summary>
		/// The version of the Softing OPC Toolkit core library (OTB.dll)
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="VersionOtb"]/doc/*'
		/// />
		public short VersionOtb
		{
			get { return m_initData.m_versionOTS; }
			set { m_initData.m_versionOTS = value; }
		} //	end VersionOts


		/// <summary>
		/// The OPC Server Application type: library or executable
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="AppType"]/doc/*'
		/// />
		public EnumApplicationType AppType
		{
			get { return (EnumApplicationType) m_initData.m_appType; }
			set { m_initData.m_appType = (byte) value; }
		} //	end AppType


		/// <summary>
		/// Windows Service name for the OPC Server application. Leave empty for non-Windows Service OPC Servers.
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="ServiceName"]/doc/*'
		/// />
		public string ServiceName
		{
			get { return m_initData.m_serviceName; }
			set { m_initData.m_serviceName = value; }
		} //	end ServiceName


		/// <summary>
		/// Windows Service description for the OPC Server application. Can be empty.
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="ServiceDescription"]/doc/*'
		/// />
		public string ServiceDescription
		{
			get { return m_initData.m_serviceDescription; }
			set { m_initData.m_serviceDescription = value; }
		} //	end ServiceName


		/// <summary>
		/// Class Id of COM-DA OPC server
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="ClsidDa"]/doc/*'
		/// />
		public string ClsidDa
		{
			get { return m_initData.m_clsidDA; }
			set { m_initData.m_clsidDA = value; }
		} //	end ClsidDa


		/// <summary>
		/// ProgID of COM-DA OPC server
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="ProgIdDa"]/doc/*'
		/// />
		public string ProgIdDa
		{
			get { return m_initData.m_progIdDA; }
			set { m_initData.m_progIdDA = value; }
		} //	end ProgIdDa


		/// <summary>
		/// Version independent ProgID COM-DA OPC server
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="VerIndProgIdDa"]/doc/*'
		/// />
		public string VerIndProgIdDa
		{
			get { return m_initData.m_verIndProgIdDA; }
			set { m_initData.m_verIndProgIdDA = value; }
		} //	end VerIndProgIdDa


		/// <summary>
		/// Description of DA server
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="Description"]/doc/*'
		/// />
		public string Description
		{
			get { return m_initData.m_description; }
			set { m_initData.m_description = value; }
		} //	end Description


		/// <summary>
		/// Class ID of the COM-AE OPC server
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="ClsidAe"]/doc/*'
		/// />
		public string ClsidAe
		{
			get { return m_initData.m_clsidAE; }
			set { m_initData.m_clsidAE = value; }
		} //	end ClsidAe


		/// <summary>
		/// ProgID of the COM-AE OPC server
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="ProgIdAe"]/doc/*'
		/// />
		public string ProgIdAe
		{
			get { return m_initData.m_progIdAE; }
			set { m_initData.m_progIdAE = value; }
		} //	end ProgIdAE


		/// <summary>
		/// Version independent ProgID of the COM-AE OPC server
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="VerIndProgIdAe"]/doc/*'
		/// />
		public string VerIndProgIdAe
		{
			get { return m_initData.m_verIndProgIdAE; }
			set { m_initData.m_verIndProgIdAE = value; }
		} //	end VerIndProgIdAe


		/// <summary>
		/// IP port number of HTTP server. The HTTP server will be used for the XML-DA
		/// communication or for the Softing's enabled Web server of the OPC Toolkit application.
		/// Leave 0 when not in use.
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="IpPortHTTP"]/doc/*'
		/// />
		public ushort IpPortHTTP
		{
			get { return m_initData.m_ipPortHTTP; }
			set { m_initData.m_ipPortHTTP = value; }
		} //	end IpPortHTTP


		/// <summary>
		/// URL for the XML-DA server
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="UrlDa"]/doc/*'
		/// />
		public string UrlDa
		{
			get { return m_initData.m_urlDA; }
			set { m_initData.m_urlDA = value; }
		} //	end UrlDa


		/// <summary>
		/// Major version number of the server
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="MajorVersion"]/doc/*'
		/// />
		public short MajorVersion
		{
			get { return m_initData.m_majorVersion; }
			set { m_initData.m_majorVersion = value; }
		} //	end MajorVersion


		/// <summary>
		/// Minor version number of the server
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="MinorVersion"]/doc/*'
		/// />
		public short MinorVersion
		{
			get { return m_initData.m_minorVersion; }
			set { m_initData.m_minorVersion = value; }
		} //	end MinorVersion


		/// <summary>
		/// Patch version number of the server
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="PatchVersion"]/doc/*'
		/// />
		public short PatchVersion
		{
			get { return m_initData.m_patchVersion; }
			set { m_initData.m_patchVersion = value; }
		} //	end MinorVersion


		/// <summary>
		/// Build number of the server
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="BuildNumber"]/doc/*'
		/// />
		public short BuildNumber
		{
			get { return m_initData.m_buildNumber; }
			set { m_initData.m_buildNumber = value; }
		} //	end BuildNumber


		/// <summary>
		/// Vendor info of the server
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="VendorInfo"]/doc/*'
		/// />
		public string VendorInfo
		{
			get { return m_initData.m_vendorInfo; }
			set { m_initData.m_vendorInfo = value; }
		} //	end VendorInfo


		/// <summary>
		/// Delimiter sign for address space levels in the string based ItemId
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="AddressSpaceDelimiter"]/doc/*'
		/// />
		public char AddressSpaceDelimiter
		{
			get { return m_initData.m_addressSpaceDelimiter; }
			set { m_initData.m_addressSpaceDelimiter = value; }
		} //	end AddressSpaceDelimiter


		/// <summary>
		/// Period in ms to monitor the connections to the OPC clients
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="ClientCheckPeriod"]/doc/*'
		/// />
		public int ClientCheckPeriod
		{
			get { return m_initData.m_clientCheckPeriod; }
			set { m_initData.m_clientCheckPeriod = value; }
		} //	end ClientCheckPeriod


		/// <summary>
		/// Timeout in milliseconds after which the cached ObjectData will expire (for String Based address spaces)
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="StringBasedObjectDataExpiry"]/doc/*'
		/// />
		public uint StringBasedObjectDataExpiry
		{
			get { return m_initData.m_stringBasedObjectDataExpiry; }
			set { m_initData.m_stringBasedObjectDataExpiry = value; }
		} //	end StringBasedObjectDataExpiry


		/// <summary>
		/// Delimiter sign for DA properties in the Item ID
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="PropertyDelimiter"]/doc/*'
		/// />
		public char PropertyDelimiter
		{
			get { return m_initData.m_propertyDelimiter; }
			set { m_initData.m_propertyDelimiter = value; }
		} //	end PropertyDelimiter


		/// <summary>
		/// Minimal update rate for a group (ms)
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="MinUpdateRateDa"]/doc/*'
		/// />
		public int MinUpdateRateDa
		{
			get { return m_initData.m_minUpdateRateDA; }
			set { m_initData.m_minUpdateRateDA = value; }
		} //	end MinUpdateRateDa


		/// <summary>
		/// Enables or disables the AE conditions support
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="SupportDisableConditions"]/doc/*'
		/// />
		public bool SupportDisableConditions
		{
			get { return System.Convert.ToBoolean(m_initData.m_supportDisableConditions); }
			set { m_initData.m_supportDisableConditions = System.Convert.ToByte(value); }
		} //	end SupportDisableConditions


		/// <summary>
		/// Optimize the Toolkit CORE for speed in detriment of sorted items (if speed
		/// optimisation is disabled, browsed items will appear in the order they were added).
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="OptimizeForSpeed"]/doc/*'
		/// />
		public bool OptimizeForSpeed
		{
			get { return System.Convert.ToBoolean(m_initData.m_optimizeForSpeed); }
			set { m_initData.m_optimizeForSpeed = System.Convert.ToByte(value); }
		} //	end OptimizeForSpeed


		/// <summary>
		/// DCOM security settings: direct call authentication level (default value: 0x2 RPC_C_AUTHN_LEVEL_CONNECT)
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="DcomAuthLevel"]/doc/*'
		/// />
		public uint DcomAuthLevel
		{
			get { return m_initData.m_authLevel; }
			set { m_initData.m_authLevel = value; }
		} //	end DcomAuthLevel

		/// <summary>
		/// DCOM security settings: direct call impersonation level (default value: 0x2 RPC_C_IMP_LEVEL_IDENTIFY)
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="DcomImpLevel"]/doc/*'
		/// />
		public uint DcomImpLevel
		{
			get { return m_initData.m_impLevel; }
			set { m_initData.m_impLevel = value; }
		} //	end DcomImpLevel

		/// <summary>
		/// DCOM security settings: callback authentication level (default value: 0x2 RPC_C_AUTHN_LEVEL_CONNECT)
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="DcomCbAuthLevel"]/doc/*'
		/// />
		public uint DcomCbAuthLevel
		{
			get { return m_initData.m_cbAuthLevel; }
			set { m_initData.m_cbAuthLevel = value; }
		} //	end DcomCbAuthLevel

		/// <summary>
		/// DCOM security settings: callback impersonation level (default value: 0x2 RPC_C_IMP_LEVEL_IDENTIFY)
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="DcomCbImpLevel"]/doc/*'
		/// />
		public uint DcomCbImpLevel
		{
			get { return m_initData.m_cbImpLevel; }
			set { m_initData.m_cbImpLevel = value; }
		} //	end DcomCbImpLevel


		/// <summary>
		/// Used to provide the web interface folder
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="WebRootDirectory"]/doc/*'
		/// />
		public string WebRootDirectory
		{
			get { return m_initData.m_webRootDirectory; }
			set { m_initData.m_webRootDirectory = value; }
		} //	end WebRootDirectory


		/// <summary>
		/// Provide the web interface root file
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="WebRootFile"]/doc/*'
		/// />
		public string WebRootFile
		{
			get { return m_initData.m_webRootFile; }
			set { m_initData.m_webRootFile = value; }
		} //	end WebRootFile


		/// <summary>
		/// Used to handle the operator password of the web interface
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="WebAdministratorPassword"]/doc/*'
		/// />
		public string WebAdministratorPassword
		{
			get { return m_initData.m_webAdministratorPassword; }
			set { m_initData.m_webAdministratorPassword = value; }
		} //	end WebAdministratorPassword


		/// <summary>
		/// Used to handle the operator password of the web interface
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="WebOperatorPassword"]/doc/*'
		/// />
		public string WebOperatorPassword
		{
			get { return m_initData.m_webOperatorPassword; }
			set { m_initData.m_webOperatorPassword = value; }
		} //	end WebOperatorPassword


		/// <summary>
		/// The port number the application should listen on the Softing Tunnel protocol.<br/>
		/// When 0 is provided, the the Softing Tunnel protocol is disabled.<br/>
		/// The Softing Tunnel functionality is not active in the commercial version of the OPC Toolkit.
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="TpPort"]/doc/*'
		/// />
		public ushort TpPort
		{
			get { return m_initData.m_tpPort; }
			set { m_initData.m_tpPort = value; }
		} //	end TpPort


		/// <summary>
		/// Gets the address space root object entry
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="DaAddressSpaceRoot"]/doc/*'
		/// />
		public DaAddressSpaceRoot DaAddressSpaceRoot
		{
			get { return m_DaAddressSpaceRoot; }
		} //	end DaAddressSpaceRoot


		/// <summary>
		/// Gets the namespace root
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="AeAddressSpaceRoot"]/doc/*'
		/// />
		public AeAddressSpaceRoot AeAddressSpaceRoot
		{
			get { return m_AeAddressSpaceRoot; }
		} //	AeAddressSpaceRoot


		/// <summary>
		/// The Application`s creator for namespace elements, namespace root, requests, transactions
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="Creator"]/doc/*'
		/// />
		public Creator Creator
		{
			get { return m_creator; }
		} //	end Creator


		/// <summary>
		/// Returns the Web server handling class associated to the OPC server
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="WebTemplate"]/doc/*'
		/// />
		public WebTemplate WebTemplate
		{
			get { return m_webTemplate; }
		} //	end WebTemplate


		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		/// Activates the specified Softing OPC functionality according to the provided feature
		/// </summary>
		/// <param name="aProduct">A features the licensed product. <see cref="EnumFeature"/> </param>
		/// <param name="aKey">Binary License Key in string format : "XXXX-XXXX-XXXX-XXXX-XXXX".</param>
		/// <returns>
		/// S_OK Everything was OK<br/>
		/// E_FAIL Actrivation has failed. Please note the provided key must match the feature<br/>
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="Activate"]/doc/*'
		/// />
		public int Activate(EnumFeature aProduct, string aKey)
		{
			int res = (int)EnumResultCode.E_FAIL;
			string licenseName = string.Empty;

			try
			{
				res = OTBFunctions.OTActivate((byte)aProduct, aKey, 0);
				EnumTraceLevel level = EnumTraceLevel.INF;

				if (ResultCode.FAILED(res))
				{
					level = EnumTraceLevel.ERR;
				}

				switch (aProduct)
				{
				case EnumFeature.DA_SERVER:
					licenseName = "OPC DA Server";
					break;
				case EnumFeature.XMLDA_SERVER:
					licenseName = "XMLDA Server";
					break;
				case EnumFeature.TP_SERVER:
					licenseName = "Tunnel Server";
					break;
				case EnumFeature.AE_SERVER:
					licenseName = "OPC AE Server";
					break;
				default:
					licenseName = "unknown server feature - " + aProduct;
					break;
				}

				Application.Instance.Trace(
					level,
					EnumTraceGroup.SERVER,
					"Application.Activate",
					"Result of activating " + licenseName + " license: " + res);
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.SERVER,
					"Application.Activate",
					exc.ToString());
			}
			return res;
		} //	end Activate


		/// <summary>
		/// Forces the target application to run as demo even if a proper binary or runtime license was activated
		/// </summary>
		/// <returns>
		/// S_OK Everything was OK<br/>
		/// E_FAIL Actrivation has failed<br/>
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="ForceDemoVersion"]/doc/*'
		/// />
		public int ForceDemoVersion()
		{
			return OTBFunctions.OTActivate(0, string.Empty, 1);
		} //	end ForceDemoVersion


		/// <summary>
		/// Initializes the OPC server core functionality
		/// </summary>
		/// <returns>
		/// S_OK - Everything was OK<br/>
		/// E_FAIL - Failure returned<br/>
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="Initialize"]/doc/*'
		/// />
		public int Initialize(Creator aCreator)
		{
			try
			{
				m_creator = aCreator;
				m_DaAddressSpaceRoot = m_creator.CreateDaAddressSpaceRoot();
				m_AeAddressSpaceRoot = m_creator.CreateAeAddressSpaceRoot();
				m_webTemplate = m_creator.CreateWebTemplate();
				return OTBFunctions.OTSInitialize(m_initData);
			}
			catch (Exception e)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.OPCSERVER,
					"Application.Initialize",
					e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	end Initialize


		/// <summary>
		/// Terminates the OPC sever application's core functionality
		/// </summary>
		/// <returns>
		/// S_OK - Everything was OK<br/>
		/// S_FALSE - Everything was OK<br/>
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="Terminate"]/doc/*'
		/// />
		public int Terminate()
		{
			m_initData.Release();
			return OTBFunctions.OTSTerminate();
		} //	end Terminate


		/// <summary>
		/// Processes the command-line parameters given to the OPC Server application and performs any Windows Service
		/// or DCOM (un)registration operation, if requested.<br/>
		/// If the method call returns S_FALSE, the server application should be stopped; a special (un)registration
		/// command was identified in the command line string and performed.<br/><br/>
		/// The following commands are interpreted by the Softing OPC Toolkit CORE:<br/>
		///     /regserver<br/>
		///     -regserver<br/>
		///     /unregserver<br/>
		///     -unregserver<br/><br/>
		/// </summary>
		/// <param name="aCommandLine">the entire commandline to be parsed</param>
		/// <returns>
		/// S_OK - No parameter found in the provided command line.<br/>
		/// S_FALSE - Special register/unregister request was found in the provided command line and the requested operation was successfully performed.<br/>
		/// other error codes - Special register/unregister request was found in the provided command line and the requested operation could not be conducted.<br/>
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="ProcessCommandLine"]/doc/*'
		/// />
		public int ProcessCommandLine(string aCommandLine)
		{
			return OTBFunctions.OTSProcessCommandLine(aCommandLine);
		} //	end ProcessCommandLine


		/// <summary>
		/// Allows the vendor application to explicitly set the OPC DA Server state.<br/>
		/// If this method is used by the vendor application, automatic transition from 
		/// NOCONFIG to READY server states is no longer performed, vendor application must
		/// explicitly change the internal OPC Server state.
		/// </summary>
		/// <param name="newState">the new OPC DA Server state</param>
		/// <returns>
		/// S_OK - New OPC DA Server state successfully set<br/>
		/// E_FAIL - Unable to set new OPC DA Server state<br/>
		/// E_INVALIDARG - Newly provided OPC DA Server state is invalid<br/>
		/// E_EXCEPTION - Exception occurred while setting the new OPC DA Server state
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="SetDaServerState"]/doc/*'
		/// />
		public int SetDaServerState(EnumServerState newState)
		{
			return OTBFunctions.OTSSetServerState((uint)newState, 0);
		} //	end ProcessCommandLine


		/// <summary>
		/// Allows the vendor application to explicitly set the OPC AE Server state.<br/>
		/// If this method is used by the vendor application, automatic transition from 
		/// NOCONFIG to READY server states is no longer performed, vendor application must
		/// explicitly change the internal OPC Server state.
		/// </summary>
		/// <param name="newState">the new OPC AE Server state</param>
		/// <returns>
		/// S_OK - New OPC AE Server state successfully set<br/>
		/// E_FAIL - Unable to set new OPC AE Server state<br/>
		/// E_INVALIDARG - Newly provided OPC AE Server state is invalid<br/>
		/// E_EXCEPTION - Exception occurred while setting the new OPC AE Server state
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="SetDaServerState"]/doc/*'
		/// />
		public int SetAeServerState(EnumServerState newState)
		{
			return OTBFunctions.OTSSetServerState(0, (uint)newState);
		} //	end ProcessCommandLine


		/// <summary>
		/// Advises the OTS callbacks<br/>
		/// This method should only be called once internally within TBC or TBN
		/// </summary>
		/// <returns>
		/// S_OK - Everything was OK
		/// </returns>
		private int Advise()
		{
			m_OTNSCallBacks.m_OTOnTrace = new OTOnTrace(HandleTrace);
			m_OTNSCallBacks.m_OTSShutdown = new OTSShutdown(HandleShutdown);
			m_OTNSCallBacks.m_OTSHandleReadRequests = new OTSHandleReadRequests(HandleReadRequests);
			m_OTNSCallBacks.m_OTSHandleWriteRequests = new OTSHandleWriteRequests(HandleWriteRequests);
			m_OTNSCallBacks.m_OTSRequestCompletedInternally = new OTSRequestCompletedInternally(HandleRequestCompletedInternally);
			m_OTNSCallBacks.m_OTSQueryProperties = new OTSQueryProperties(HandleQueryProperties);
			m_OTNSCallBacks.m_OTSQueryAddressSpaceElementChildren =
				new OTSQueryAddressSpaceElementChildren(HandleQueryAddressSpaceElementChildren);
			m_OTNSCallBacks.m_OTSQueryAddressSpaceElementData =
				new OTSQueryAddressSpaceElementData(HandleQueryAddressSpaceElementData);
			m_OTNSCallBacks.m_OTSQueryAddressSpaceElementDataEx =
				new OTSQueryAddressSpaceElementDataEx(HandleQueryAddressSpaceElementDataEx);

			m_OTNSCallBacks.m_OTSCreateAddressSpaceElement = new OTSCreateAddressSpaceElement(HandleCreateAddressSpaceElement);
			m_OTNSCallBacks.m_OTSCreateAddressSpaceElementEx = new OTSCreateAddressSpaceElementEx(HandleCreateAddressSpaceElementEx);
			m_OTNSCallBacks.m_OTSDestroyAddressSpaceElement = new OTSDestroyAddressSpaceElement(HandleDestroyAddressSpaceElement);

			m_OTNSCallBacks.m_OTSChangeSessionState = new OTSChangeSessionState(HandleChangeSessionState);
			m_OTNSCallBacks.m_OTSChangeItems = new OTSChangeItems(HandleChangeItems);
			m_OTNSCallBacks.m_OTSQueryCacheValue = new OTSQueryCacheValue(HandleQueryCacheValue);

			m_OTNSCallBacks.m_OTSQueryConditions = new OTSQueryConditions(HandleQueryConditions);
			m_OTNSCallBacks.m_OTSAcknowledgeCondition = new OTSAcknowledgeCondition(HandleAcknowledgeCondition);
			m_OTNSCallBacks.m_OTSQueryConditionDefinition = new OTSQueryConditionDefinition(HandleQueryConditionDefinition);
			m_OTNSCallBacks.m_OTSEnableConditions = new OTSEnableConditions(HandleEnableConditions);
			m_OTNSCallBacks.m_OTSWebHandleTemplate = new OTSWebHandleTemplate(HandleWebTemplate);

			return OTBFunctions.OTSAdvise(m_OTNSCallBacks);
		} //	end Advise


		/// <summary>
		/// Starts the OPC server's Toolkit internals. The namespace should be created after this method is called.
		/// </summary>
		/// <returns>
		/// S_OK - Everything was OK<br/>
		/// S_FALSE - Everything was OK<br/>
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="Start"]/doc/*'
		/// />
		public int Start()
		{
			return OTBFunctions.OTSStart();
		} //	end Start


		/// <summary>
		/// Should be called after address space(s) initialization. This prepares the internal toolkit components for the IO startup
		/// </summary>
		/// <returns>
		/// S_OK - Everything was OK, server state was automatically set to RUNNING<br/>
		/// S_FALSE - Everything was OK but server state was not set to RUNNING (previous server state was set by vendor application)<br/>
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="Ready"]/doc/*'
		/// />
		public int Ready()
		{
			return OTBFunctions.OTSReady();
		} //	end Ready


		/// <summary>
		/// Stops the OPC server's IO and prepares the Toolkit internals for server's shut down.
		/// </summary>
		/// <param name="reason">The reason (string) why OPC Server is shutting down</param>
		/// <returns>
		/// S_OK - Everything was OK<br/>
		/// S_FALSE - Everything was OK<br/>
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="Stop"]/doc/*'
		/// />
		public int Stop(string reason)
		{
			return OTBFunctions.OTSStop(reason);
		} //	end Stop


		/// <summary>
		/// Stops the OPC server's IO and prepares the Toolkit internals for server's shut down (parameterless).
		/// </summary>
		/// <returns>
		/// S_OK - Everything was OK<br/>
		/// S_FALSE - Everything was OK<br/>
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="Stop"]/doc/*'
		/// />
		public int Stop()
		{
			return OTBFunctions.OTSStop("");
		} //	end Stop

		/// <summary>
		/// Adds a new AE category to the AE component of the OPC server application instance
		/// </summary>
		/// <param name="aCategory"></param>
		/// <returns>
		/// S_OK - Everything was OK<br/>
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="AddAeCategory"]/doc/*'
		/// />
		public int AddAeCategory(AeCategory aCategory)
		{
			try
			{
				uint categoryHandle = 0;
				int result = OTBFunctions.OTSAddEventCategory(
					aCategory.Id,
					aCategory.Name,
					(uint) aCategory.EventType,
					out categoryHandle);

				if (ResultCode.SUCCEEDED(result))
				{
					aCategory.Handle = categoryHandle;
					Hashtable syncHashtable = Hashtable.Synchronized(this.m_categoryList);
					syncHashtable.Add(categoryHandle, aCategory);
				} //	end if

				return result;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.AddAeCategory",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	end AddAeCategory

		/// <summary>
		/// Retrieves the AE Category list stored in the OPC server application
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="AeCategoryList"]/doc/*'
		/// />
		public ICollection AeCategoryList
		{
			get
			{
				Hashtable syncHashtable = Hashtable.Synchronized(this.m_categoryList);
				return syncHashtable.Values;
			} //	end get
		} //	end AeCategoryList


		/// <summary>
		/// Trigger the change of a single AE condition provided as a parameter
		/// </summary>
		/// <param name="aCondition"></param>
		/// <returns>
		/// S_OK - Everything was OK<br/>
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="ChangeCondition"]/doc/*'
		/// />
		public int ChangeCondition(AeCondition aCondition)
		{
			try
			{
				return this.ChangeConditions(new AeCondition[] {aCondition});
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.AddCondition",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	try ... catch
		} //	end AddCondition


		/// <summary>
		/// Triggers the change of several AE conditions provided in the parameters list
		/// </summary>
		/// <param name="aConditionList"></param>
		/// <returns>
		/// S_OK - Everything was OK<br/>
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="ChangeConditions"]/doc/*'
		/// />
		public int ChangeConditions(ICollection aConditionList)
		{
			try
			{
				uint count = (uint) aConditionList.Count;
				if (count == 0)
				{
					return (int) EnumResultCode.S_FALSE;
				} //	end if

				uint[] inHandles = new uint[count];
				OTSConditionData[] conditionData = new OTSConditionData[count];
				uint[] outHandles = new uint[count];

				int index = 0;
				IntPtr conditionsData = Marshal.AllocCoTaskMem((int) count*Marshal.SizeOf(typeof (OTSConditionData)));
				IntPtr indexPointer = conditionsData;
				foreach (AeCondition condition in aConditionList)
				{
					conditionData[index] = new OTSConditionData();
					condition.FillOTSConditionData(ref conditionData[index]);
					Marshal.StructureToPtr(conditionData[index], indexPointer, false);

					inHandles[index] = condition.Handle;
					outHandles[index] = 0;

					indexPointer = new IntPtr(indexPointer.ToInt64() + Marshal.SizeOf(typeof (OTSConditionData)));
					index++;
				} //	end for

				int result = OTBFunctions.OTSConditionsChanged(
					count,
					inHandles,
					conditionsData,
					outHandles);

				for (index = 0; index < count; index++)
				{
					conditionData[index].Release();
				} //	end for

				Marshal.FreeCoTaskMem(conditionsData);

				if (ResultCode.SUCCEEDED(result))
				{
					Hashtable syncConditions = Hashtable.Synchronized(this.m_conditionList);
					index = 0;
					foreach (AeCondition condition in aConditionList)
					{
						if (inHandles[index] == 0)
						{
							if (outHandles[index] == 0)
							{
								//	strange situation, new condition to add, but not accepted
								result = (int) EnumResultCode.E_FAIL;
							}
							else
							{
								condition.Handle = outHandles[index];
								syncConditions.Add(outHandles[index], condition);
							} //	end if ... else
						}
						else
						{
							if (outHandles[index] == 0)
							{
								//	remove the conditon
								condition.Handle = 0;
								syncConditions.Remove(inHandles[index]);
							}
							else
							{
								//	normal situation for a update only action
							} //	end if ... else
						} //	end if ... else
						index++;
					} //	end for
				} //	end if

				return result;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.OPCSERVER,
					"Application.ConditionsChanged",
					"Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	try ... catch
		} //	end ConditionsChanged


		/// <summary>
		/// releases a condition from the OPC server's internal conditions list
		/// </summary>
		/// <param name="aConditionHandle"></param>
		/// <returns></returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="ReleaseCondition"]/doc/*'
		/// />
		public int ReleaseCondition(uint aConditionHandle)
		{
			try
			{
				Hashtable syncConditions = Hashtable.Synchronized(this.m_conditionList);
				syncConditions.Remove(aConditionHandle);
				return (int) EnumResultCode.S_OK;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.OPCSERVER,
					"Application.ReleaseCondition",
					"Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end try catch
		} //	end ReleaseCondition


		/// <summary>
		/// Fires the AE events provided by the parameter
		/// </summary>
		/// <param name="anEventList"></param>
		/// <returns></returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="FireEvents"]/doc/*'
		/// />
		public int FireEvents(ArrayList anEventList)
		{
			try
			{
				uint count = (uint) anEventList.Count;
				if (count == 0)
				{
					return (int) EnumResultCode.S_OK;
				} //	end if

				IntPtr pEventData = Marshal.AllocCoTaskMem((int) count*Marshal.SizeOf(typeof (OTSEventData)));
				IntPtr pIndex = pEventData;

				OTSEventData[] eventData = new OTSEventData[count];

				int index = 0;
				foreach (AeEvent eventObject in anEventList)
				{
					eventData[index] = eventObject.CreateOTSEventData();
					Marshal.StructureToPtr(eventData[index], pIndex, false);
					pIndex = new IntPtr(pIndex.ToInt64() + Marshal.SizeOf(typeof (OTSEventData)));
					index++;
				} //	end foreach

				int result = OTBFunctions.OTSFireEvents(count, pEventData);
				for (index = 0; index < count; index++)
				{
					eventData[index].Release();
				} //	end for
				Marshal.FreeCoTaskMem(pEventData);
				return result;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.FireEvents",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	end Fire


		/// <summary>
		/// Retrieves the current AE conditions list stored in the OPC server's application
		/// instance
		/// </summary>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  property[@name="AeConditionList"]/doc/*'
		/// />
		public ICollection AeConditionList
		{
			get
			{
				Hashtable syncHashtable = Hashtable.Synchronized(this.m_conditionList);
				return syncHashtable.Values;
			} //	end get
		} //	end AeConditionList


		/// <summary>
		/// Message trace interface
		/// </summary>
		/// <param name="aLevel"></param>
		/// <param name="aMask"></param>
		/// <param name="anObjectID"></param>
		/// <param name="aMessage"></param>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="Trace"]/doc/*'
		/// />
		public void Trace(EnumTraceLevel aLevel, EnumTraceGroup aMask, string anObjectID, string aMessage)
		{
			OPCToolbox.Trace.WriteLine((byte) aLevel, (uint) aMask, anObjectID, aMessage);
		} //	end Trace


		/// <summary>
		/// Enables tracing for the entire application.
		/// </summary>
		/// <param name="errorLevelMask">
		/// A trace filter at error level.
		/// </param>
		/// <param name="warningLevelMask">
		/// A trace filter at warning level.
		/// </param>
		/// <param name="infoLevelMask">
		/// A trace filter at informative level.
		/// </param>
		/// <param name="debugLevelMask">
		/// A trace filter at debug level.
		/// </param>
		/// <param name="fileName">
		/// A name of a file in which the tracing messages are to be written. When this parameter is empty
		/// the file tracing will be disabled
		/// </param>
		/// <param name="fileMaxSize">
		/// Maximum file size.Specifying -1 for maximum file size will determine tracing in a single file. The file in which
		/// the tracing messages will be written is the file specified by the first parameter.
		/// </param>
		///<param name="maximumBackups">
		/// The maximum number of backups Trace files created. Every new application restart will stored the existing trace file
		/// if any in backup. When this max umber is reached the older will be removed so that the new one is created.
		///</param>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="EnableTracing"]/doc/*'
		/// />
		public void EnableTracing(
			EnumTraceGroup errorLevelMask,
			EnumTraceGroup warningLevelMask,
			EnumTraceGroup infoLevelMask,
			EnumTraceGroup debugLevelMask,
			string fileName,
			uint fileMaxSize,
			uint maximumBackups)
		{
			Softing.OPCToolbox.Trace.DebugLevelMask = (uint) debugLevelMask;
			Softing.OPCToolbox.Trace.ErrorLevelMask = (uint) errorLevelMask;
			Softing.OPCToolbox.Trace.InfoLevelMask = (uint) infoLevelMask;
			Softing.OPCToolbox.Trace.WarningLevelMask = (uint) warningLevelMask;

			if (fileName != string.Empty)
			{
				Softing.OPCToolbox.Trace.MaximumBackups = maximumBackups;
				Softing.OPCToolbox.Trace.FileMaxSize = fileMaxSize;
				Softing.OPCToolbox.Trace.FileName = fileName;
				Softing.OPCToolbox.Trace.EnableTraceToFile = true;
			}
			else
			{
				Softing.OPCToolbox.Trace.EnableTraceToFile = false;
			}
		} //	end EnableTracing


		/// <summary>
		/// Adds the Softing Tunnel Protocol credential.
		/// The Softing Tunnel functionality is not active in the commercial version of the OPC Toolkit.
		/// </summary>
		/// <param name="users"></param>
		/// <param name="passwords"></param>
		/// <returns></returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="AddTpCredentials"]/doc/*'
		/// />
		public int AddTpCredentials(
			string[] users,
			string[] passwords)
		{
			if (users.Length != passwords.Length)
			{
				return (int) EnumResultCode.E_INVALIDARG;
			} //	end if

			m_initData.m_tpCredentialsNumber = users.Length;

			if (m_initData.m_tpCredentialsNumber > 0)
			{
				m_initData.m_tpUsers = OTBFunctions.OTAllocateMemory(users.Length*Marshal.SizeOf(typeof (IntPtr)));
				m_initData.m_tpPasswords = OTBFunctions.OTAllocateMemory(passwords.Length*Marshal.SizeOf(typeof (IntPtr)));

				for (int index = 0; index < m_initData.m_tpCredentialsNumber; index++)
				{
					IntPtr userPointer = OTBFunctions.AllocateOTBString(users[index]);
					Marshal.WriteIntPtr(m_initData.m_tpUsers,
					                    index*Marshal.SizeOf(typeof (IntPtr)),
					                    userPointer);

					IntPtr passwordPointer = OTBFunctions.AllocateOTBString(passwords[index]);
					Marshal.WriteIntPtr(m_initData.m_tpPasswords,
					                    index*Marshal.SizeOf(typeof (IntPtr)),
					                    passwordPointer);
				} //	end for
			} //	end if

			return (int) EnumResultCode.S_OK;
		} //	end AddTpCredentials


		/// <summary>
		/// Marks the request identified by "requestHandle" completed, as it was internally completed (~cancelled) inside Toolkit Core
		/// </summary>
		/// <param name="requestHandle"></param>
		/// <returns></returns>
		/// <include
		///  file='TBNS.doc.xml'
		///  path='//class[@name="Application"]/
		///  method[@name="MarkRequestCompletedInternally"]/doc/*'
		/// />
		private int MarkRequestCompletedInternally(uint requestHandle)
		{
			int res = (int) EnumResultCode.E_NOTFOUND;
			DaRequest request = null;
			DaTransaction transaction;
			lock (m_transactionList.SyncRoot) // enumerating
			{
				int count = m_transactionList.Count;
				for (int i = 0; i < count; ++i)
				{
					transaction = m_transactionList[i] as DaTransaction;
					if (transaction != null)
					{
						res = transaction.findRequestByHandle(requestHandle, out request);
						if (ResultCode.SUCCEEDED(res))
						{
							break;
						}
					}
				}
			}
			if (ResultCode.SUCCEEDED(res))
			{
				request.CompletedInternally();
				return res;
			}
			return (int) EnumResultCode.E_NOTFOUND;
		} //	end MarkRequestCompletedInternally


		//-
		#endregion

		#region //	Internal Members
		//--------------------------

		/// <summary>
		/// Get Client by the handle
		/// </summary>
		/// <param name="aSessionHandle"></param>
		/// <returns></returns>
		internal DaSession GetSession(uint aSessionHandle)
		{
			try
			{
				Hashtable synchronizedSessionList = Hashtable.Synchronized(m_sessionList);
				return synchronizedSessionList[aSessionHandle] as DaSession;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.GetSession",
				                           "Exception caught:" + e.ToString());
				return null;
			} //	end try ... catch
		} //	end GetSession


		internal void AddSession(DaSession aSession, uint aSessionHandle)
		{
			lock (m_sessionList.SyncRoot) // modifying content
			{
				m_sessionList.Add(aSessionHandle, aSession);
			}
		} //	end AddSession


		internal void RemoveSession(uint aSessionHandle)
		{
			lock (m_sessionList.SyncRoot) // modifying content
			{
				m_sessionList.Remove(aSessionHandle);
			}
		} //	end AddSession


		internal void AddTransaction(uint aKey, DaTransaction aTransaction)
		{
			lock (m_transactionList.SyncRoot) // modifying content
			{
				m_transactionList.Add(aKey, aTransaction);
			}
		} //	end AddTransaction


		internal DaTransaction FindTransaction(uint aKey)
		{
			DaTransaction result = null;
			lock (m_transactionList.SyncRoot) // modifying content
			{
				result = m_transactionList[aKey] as DaTransaction;
			}
			return result;
		} //	end FindTransaction


		internal void ReleaseTransaction(uint aKey)
		{
			Hashtable synchronizedTransactionList = Hashtable.Synchronized(m_transactionList);
			DaTransaction transaction = synchronizedTransactionList[aKey] as DaTransaction;

			if (transaction != null)
			{
				if (transaction.IsEmpty)
				{
					m_transactionList.Remove(aKey);
				}
				else
				{
					Trace(EnumTraceLevel.WRN,
						    EnumTraceGroup.OPCSERVER,
						    "Application.ReleaseTransaction",
						    "List is not emppty");
				}
			}
			else
			{
				Trace(EnumTraceLevel.ERR,
					    EnumTraceGroup.OPCSERVER,
					    "Application.ReleaseTransaction",
					    "transaction is empty");
			}
		} //	end  ReleaseTransaction


		//-
		#endregion

		#region //	Private Constructor
		//-----------------------------

		/// <summary>
		/// private constructor in order to assure the singleton design
		/// </summary>
		private Application()
		{
			m_initData = new OTSInitData();

			m_initData.m_versionOTS = 431;
			m_initData.m_appType = 1;
			m_initData.m_clsidDA = string.Empty;
			m_initData.m_progIdDA = string.Empty;
			m_initData.m_verIndProgIdDA = string.Empty;
			m_initData.m_clsidAE = string.Empty;
			m_initData.m_progIdAE = string.Empty;
			m_initData.m_verIndProgIdAE = string.Empty;
			m_initData.m_description = string.Empty;
			m_initData.m_ipPortHTTP = 0;
			m_initData.m_urlDA = string.Empty;
			m_initData.m_majorVersion = 0;
			m_initData.m_minorVersion = 0;
			m_initData.m_patchVersion = 0;
			m_initData.m_buildNumber = 0;
			m_initData.m_vendorInfo = string.Empty;
			m_initData.m_minUpdateRateDA = 100;
			m_initData.m_serviceName = string.Empty;
			m_initData.m_addressSpaceDelimiter = '.';
			m_initData.m_clientCheckPeriod = 1000;
			m_initData.m_propertyDelimiter = '#';
			m_initData.m_supportDisableConditions = 0;
			m_initData.m_stringBasedObjectDataExpiry = 60000;

			m_initData.m_webRootDirectory = string.Empty;
			m_initData.m_webRootFile = string.Empty;
			m_initData.m_webAdministratorPassword = string.Empty;
			m_initData.m_webOperatorPassword = string.Empty;

			m_initData.m_tpPort = 0;
			m_initData.m_tpCredentialsNumber = 0;
			m_initData.m_tpUsers = IntPtr.Zero;
			m_initData.m_tpPasswords = IntPtr.Zero;

			m_initData.m_optimizeForSpeed = 0; // disable speed optimization, preserve item order

			m_initData.m_authLevel = 2;    // RPC_C_AUTHN_LEVEL_CONNECT
			m_initData.m_impLevel = 2;     // RPC_C_IMP_LEVEL_IDENTIFY
			m_initData.m_cbAuthLevel = 2;  // RPC_C_AUTHN_LEVEL_CONNECT
			m_initData.m_cbImpLevel = 2;   // RPC_C_IMP_LEVEL_IDENTIFY

			int result = Advise();

			if (ResultCode.FAILED(result))
			{
				Trace(EnumTraceLevel.ERR,
				      EnumTraceGroup.OPCSERVER,
				      "Application.ctr",
				      "Callback advise failure");

				throw new ApplicationException("Callback advise failure");
			} //	end if
		} //	end constructor


		//-
		#endregion

		#region //	Private Members
		//---------------------------

		/// <summary>
		/// Terminates the application
		/// </summary>
		/// <returns>
		/// S_OK Everything was OK
		/// S_FALSE Everything was OK
		/// The Result should be checked with ResultCode.SUCCEEDED
		/// or with ResultCode.FAILED
		/// </returns>
		private int HandleShutdown()
		{
			try
			{
				if (ShutdownRequest != null)
				{
					return ShutdownRequest();
				} //	end if
				return (int) EnumResultCode.S_OK;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.OPCSERVER,
					"Application.HandleShutdown",
					e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	end HandleShutdown


		/// <summary>
		/// HandleTrace
		/// </summary>
		private void HandleTrace(
			string aString,
			ushort aLevel,
			uint aMask,
			string anObjId,
			string aMessage)
		{
			try
			{
				if (TraceOutput != null)
				{
					TraceOutput(
						aString,
						(EnumTraceLevel) aLevel,
						(EnumTraceGroup) aMask,
						anObjId,
						aMessage);
				} //	end if
			}
			catch (Exception e)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.OPCSERVER,
					"Application.HandleTrace",
					e.ToString());
			} //	end try ... catch
		} //	end HandleTrace


		/// <summary>
		/// HandleReadRequests
		/// </summary>
		/// <param name="aCount"></param>
		/// <param name="aRequestDataList"></param>
		/// <returns></returns>
		private int HandleReadRequests(
			int aCount,
			OTSRequestData[] aRequestDataList)
		{
			try
			{
				if (aCount == 0)
				{
					return (int) EnumResultCode.E_INVALIDARG;
				} //	end if

				DaRequest[] requests = new DaRequest[aCount];

				uint sessionHandle = aRequestDataList[0].m_sessionHandle;

				lock (DaAddressSpaceRoot.ElementSyncRoot) // enumerating
				{
					for (int i = 0; i < aCount; i++)
					{
						DaAddressSpaceElement element =
							DaAddressSpaceRoot.GetElementFromArray(aRequestDataList[i].m_object.m_userData) as DaAddressSpaceElement;

						requests[i] = m_creator.CreateRequest(
							EnumTransactionType.READ,
							sessionHandle,
							element,
							aRequestDataList[i].m_propertyID,
							aRequestDataList[i].m_requestHandle);

						if (aRequestDataList[i].m_object.m_objectHandle != requests[i].AddressSpaceElement.ObjectHandle)
						{
							//	this should not happen, it is an error and must be logged
							Application.Instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.OPCSERVER,
							                           "Application.HandleReadRequests",
							                           "Request object handle is different from the AddressSpaceElement object handle");
						} //	end if
					} //	end for
				}

				DaTransaction transaction = m_creator.CreateTransaction(EnumTransactionType.READ, requests, sessionHandle);
				if (transaction == null)
				{
					return (int) EnumResultCode.E_NOTIMPL;
				} //	end if

				Instance.AddTransaction(transaction.Key, transaction);

				return transaction.HandleReadRequests();
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.OPCSERVER,
				                           "Application.HandleReadRequests", e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	end HandleReadRequests


		/// <summary>
		/// HandleWriteRequests
		/// </summary>
		/// <param name="aCount"></param>
		/// <param name="aRequestDataList"></param>
		/// <param name="aValues"></param>
		/// <returns></returns>
		private int HandleWriteRequests(
			int aCount,
			OTSRequestData[] aRequestDataList,
			OTValueData[] aValues)
		{
			try
			{
				if (aCount == 0)
				{
					return (int) EnumResultCode.E_INVALIDARG;
				} //	end if

				DaRequest[] requests = new DaRequest[aCount];
				uint sessionHandle = aRequestDataList[0].m_sessionHandle;

				lock (DaAddressSpaceRoot.ElementSyncRoot) // enumerating
				{
					for (int i = 0; i < aCount; i++)
					{
						DaAddressSpaceElement element =
							DaAddressSpaceRoot.GetElementFromArray(aRequestDataList[i].m_object.m_userData) as DaAddressSpaceElement;

						requests[i] = m_creator.CreateRequest(
							EnumTransactionType.WRITE,
							sessionHandle,
							element,
							aRequestDataList[i].m_propertyID,
							aRequestDataList[i].m_requestHandle);

						requests[i].Value = new ValueQT(ref aValues[i]);

						if (aRequestDataList[i].m_object.m_objectHandle != requests[i].AddressSpaceElement.ObjectHandle)
						{
							//	this should not happen, it is an error and must be logged
							Application.Instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.OPCSERVER,
							                           "Application.HandleWriteRequests",
							                           "Request object handle is different from the AddressSpaceElement object handle");
						} //	end if
					} //	end for
				}

				DaTransaction transaction = m_creator.CreateTransaction(
					EnumTransactionType.WRITE,
					requests,
					sessionHandle);

				if (transaction == null)
				{
					return (int) EnumResultCode.E_NOTIMPL;
				} //	end if

				Instance.AddTransaction(transaction.Key, transaction);
				return transaction.HandleWriteRequests();
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.OPCSERVER,
				                           "Application.HandleWriteRequests", e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	end HandleWriteRequests


		private int HandleRequestCompletedInternally(
			uint requestHandle)
		{
			return Application.Instance.MarkRequestCompletedInternally(requestHandle);
		}

		/// <summary>
		/// HandleQueryProperties
		/// </summary>
		private int HandleQueryProperties(
			IntPtr pObjectData,
			string anItemId,
			int aPropertyId,
			out uint aPropertiesCount,
			out IntPtr pPropertiesList)
		{
			aPropertiesCount = 0;
			pPropertiesList = IntPtr.Zero;

			try
			{
				int result = (int) EnumResultCode.E_NOTIMPL;
				ArrayList propertyList = null;

				if (pObjectData != IntPtr.Zero)
				{
					OTObjectData objectData = (OTObjectData) Marshal.PtrToStructure(pObjectData, typeof (OTObjectData));

					if (pObjectData != IntPtr.Zero)
					{
						DaAddressSpaceElement addressElement =
							DaAddressSpaceRoot.GetElementFromArray(objectData.m_userData) as DaAddressSpaceElement;

						if (addressElement != null)
						{
							result = addressElement.QueryProperties(out propertyList);
						} //	end if
					} //	end if
				} //	end if

				if (propertyList == null)
				{
					//	try to get the property list using the item ID string
					result = DaAddressSpaceRoot.QueryAddressSpaceElementProperties(anItemId, out propertyList);
				} //	end if

				if (aPropertyId > 0 && aPropertyId < 7)
				{
					return (int) EnumResultCode.S_FALSE;
				} //	end if

				if (propertyList != null && propertyList.Count > 0)
				{
					aPropertiesCount = (uint) propertyList.Count;

					OTSPropertyData data = new OTSPropertyData();
					pPropertiesList = OTBFunctions.OTAllocateMemory((int) aPropertiesCount*Marshal.SizeOf(typeof (OTSPropertyData)));

					IntPtr currentPtr = pPropertiesList;
					foreach (DaProperty property in propertyList)
					{
						data.m_pid = property.Id;
						data.m_name = OTBFunctions.AllocateOTBString(property.Name);
						data.m_itemID = OTBFunctions.AllocateOTBString(property.ItemId);
						data.m_descr = OTBFunctions.AllocateOTBString(property.Description);
						data.m_datatype = ValueQT.GetVartype(property.Datatype);

						data.m_accessRights = (byte) property.AccessRights;

						Marshal.StructureToPtr(data, currentPtr, false);
						currentPtr = new IntPtr(currentPtr.ToInt64() + Marshal.SizeOf(typeof (OTSPropertyData)));
					} //	end foreach

					result = (int) EnumResultCode.S_OK;
				} //	end if

				return result;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.HandleQueryProperties",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end catch
		} //	end HandleQueryProperties


		/// <summary>
		/// HandleQueryAddressSpaceElementData
		/// </summary>
		private int HandleQueryAddressSpaceElementData(
			string aPath,
			byte anElementType,
			IntPtr aData)
		{
			try
			{
				if (aData == IntPtr.Zero || aPath == string.Empty)
				{
					return (int) EnumResultCode.E_INVALIDARG;
				} //	end if

				EnumAddressSpaceElementType elementType = (EnumAddressSpaceElementType) anElementType;
				AddressSpaceElement element = null;
				int result = (int) EnumResultCode.E_FAIL;

				switch (elementType)
				{
					case EnumAddressSpaceElementType.DA:

						result = DaAddressSpaceRoot.QueryAddressSpaceElementData(aPath, out element);

						if (ResultCode.SUCCEEDED(result))
						{
							DaAddressSpaceElement daElement = element as DaAddressSpaceElement;

							if (daElement != null)
							{
								OTSAddressSpaceElementData aDataRet = new OTSAddressSpaceElementData();
								aDataRet.m_accessRights = (byte) daElement.AccessRights;
								aDataRet.m_datatype = ValueQT.GetVartype(daElement.Datatype);
								aDataRet.m_hasChildren = Convert.ToByte(daElement.HasChildren);
								aDataRet.m_isBrowsable = Convert.ToByte(daElement.IsBrowsable);
								aDataRet.m_ioMode = (byte) daElement.IoMode;
								aDataRet.m_userData = daElement.UserData;
								aDataRet.m_itemID = OTBFunctions.AllocateOTBString(daElement.ItemId);
								aDataRet.m_name = OTBFunctions.AllocateOTBString(daElement.Name);

								Marshal.StructureToPtr(aDataRet, aData, false);
							} //	end if
						} //	end if
						break;

					case EnumAddressSpaceElementType.AE:
						result = AeAddressSpaceRoot.QueryAddressSpaceElementData(aPath, out element);

						if (ResultCode.SUCCEEDED(result))
						{
							AeAddressSpaceElement aeElement = element as AeAddressSpaceElement;

							if (aeElement != null)
							{
								OTSAddressSpaceElementData aDataRet = new OTSAddressSpaceElementData();
								aDataRet.m_name = OTBFunctions.AllocateOTBString(aeElement.Name);
								aDataRet.m_hasChildren = Convert.ToByte(aeElement.HasChildren);
								aDataRet.m_isBrowsable = Convert.ToByte(aeElement.IsBrowsable);
								aDataRet.m_userData = aeElement.UserData;
								Marshal.StructureToPtr(aDataRet, aData, false);
							} //	end if
						} //	end if
						break;

					default:
						Application.Instance.Trace(EnumTraceLevel.ERR,
						                           EnumTraceGroup.OPCSERVER,
						                           "Application.QueryAddressSpaceElementData",
						                           "Invalid element type received" + anElementType.ToString());
						break;
				} //	end switch

				return result;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.HandleQueryAddressSpaceElementData",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end catch
		} //	end HandleQueryAddressSpaceElementData


		/// <summary>
		/// HandleQueryAddressSpaceElementDataEx
		/// </summary>
		private int HandleQueryAddressSpaceElementDataEx(
			string aPath,
			byte anElementType,
			ref ushort elementDataFilter,
			IntPtr aData)
		{
			try
			{
				if (aData == IntPtr.Zero || aPath == string.Empty)
				{
					return (int) EnumResultCode.E_INVALIDARG;
				} //	end if

				EnumAddressSpaceElementType elementType = (EnumAddressSpaceElementType) anElementType;
				AddressSpaceElement element = null;
				int result = (int) EnumResultCode.E_FAIL;
				EnumAddressSpaceElementData elementDataFilterEx = (EnumAddressSpaceElementData)elementDataFilter;

				switch (elementType)
				{
					case EnumAddressSpaceElementType.DA:
						result = DaAddressSpaceRoot.QueryAddressSpaceElementDataEx(aPath, ref elementDataFilterEx, out element);

						if (ResultCode.SUCCEEDED(result))
						{
							elementDataFilter = (ushort) elementDataFilterEx;
							DaAddressSpaceElement daElement = element as DaAddressSpaceElement;

							if (daElement != null)
							{
								OTSAddressSpaceElementData aDataRet = new OTSAddressSpaceElementData();
								aDataRet.m_accessRights = (byte) daElement.AccessRights;
								aDataRet.m_datatype = ValueQT.GetVartype(daElement.Datatype);
								aDataRet.m_hasChildren = Convert.ToByte(daElement.HasChildren);
								aDataRet.m_isBrowsable = Convert.ToByte(daElement.IsBrowsable);
								aDataRet.m_ioMode = (byte) daElement.IoMode;
								aDataRet.m_userData = daElement.UserData;
								if (((EnumAddressSpaceElementData) elementDataFilter & EnumAddressSpaceElementData.ITEMID) ==
								    EnumAddressSpaceElementData.ITEMID)
								{
									aDataRet.m_itemID = OTBFunctions.AllocateOTBString(daElement.ItemId);
								}
								else
								{
									aDataRet.m_itemID = IntPtr.Zero;
								}
								if (((EnumAddressSpaceElementData) elementDataFilter & EnumAddressSpaceElementData.NAME) ==
								    EnumAddressSpaceElementData.NAME)
								{
									aDataRet.m_name = OTBFunctions.AllocateOTBString(daElement.Name);
								}
								else
								{
									aDataRet.m_name = IntPtr.Zero;
								}
								Marshal.StructureToPtr(aDataRet, aData, false);
							} //	end if
						} //	end if
						break;

					case EnumAddressSpaceElementType.AE:
						result = AeAddressSpaceRoot.QueryAddressSpaceElementDataEx(aPath, ref elementDataFilterEx, out element);

						if (ResultCode.SUCCEEDED(result))
						{
							elementDataFilter = (ushort) elementDataFilterEx;
							AeAddressSpaceElement aeElement = element as AeAddressSpaceElement;

							if (aeElement != null)
							{
								OTSAddressSpaceElementData aDataRet = new OTSAddressSpaceElementData();
								if (((EnumAddressSpaceElementData) elementDataFilter & EnumAddressSpaceElementData.NAME) ==
								    EnumAddressSpaceElementData.NAME)
								{
									aDataRet.m_name = OTBFunctions.AllocateOTBString(aeElement.Name);
								}
								else
								{
									aDataRet.m_name = IntPtr.Zero;
								}
								aDataRet.m_hasChildren = Convert.ToByte(aeElement.HasChildren);
								aDataRet.m_isBrowsable = Convert.ToByte(aeElement.IsBrowsable);
								aDataRet.m_userData = aeElement.UserData;
								Marshal.StructureToPtr(aDataRet, aData, false);
							} //	end if
						} //	end if
						break;

					default:
						Application.Instance.Trace(EnumTraceLevel.ERR,
						                           EnumTraceGroup.OPCSERVER,
						                           "Application.QueryAddressSpaceElementDataEx",
						                           "Invalid element type received" + anElementType.ToString());
						break;
				} //	end switch

				return result;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.HandleQueryAddressSpaceElementDataEx",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end catch
		} //	end HandleQueryAddressSpaceElementDataEx


		/// <summary>
		/// HandleQueryAddressSpaceElementChildren
		/// </summary>
		private int HandleQueryAddressSpaceElementChildren(
			string aPath,
			byte anElementType,
			out uint pCount,
			out IntPtr aChildrenList)
		{
			try
			{
				pCount = 0;
				aChildrenList = IntPtr.Zero;
				int res = (int) EnumResultCode.E_FAIL;
				EnumAddressSpaceElementType elementType = (EnumAddressSpaceElementType) anElementType;
				switch (elementType)
				{
					case EnumAddressSpaceElementType.DA:

						ArrayList childrenDa = new ArrayList();
						res = (int) DaAddressSpaceRoot.QueryAddressSpaceElementChildren(aPath, childrenDa);

						if (ResultCode.SUCCEEDED(res))
						{
							pCount = (uint) childrenDa.Count;
							if (pCount > 0)
							{
								OTSAddressSpaceElementData data = new OTSAddressSpaceElementData();
								aChildrenList = OTBFunctions.OTAllocateMemory((int) pCount*Marshal.SizeOf(typeof (OTSAddressSpaceElementData)));

								IntPtr currentPtr = aChildrenList;
								for (int i = 0; i < pCount; i++)
								{
									DaAddressSpaceElement elementDA = childrenDa[i] as DaAddressSpaceElement;
									data.m_elementType = (byte) EnumAddressSpaceElementType.DA;
									data.m_accessRights = (byte) elementDA.AccessRights;
									data.m_datatype = ValueQT.GetVartype(elementDA.Datatype);
									data.m_hasChildren = Convert.ToByte(elementDA.HasChildren);
									data.m_isBrowsable = Convert.ToByte(elementDA.IsBrowsable);
									data.m_ioMode = (byte) elementDA.IoMode;
									data.m_itemID = OTBFunctions.AllocateOTBString(elementDA.ItemId);
									data.m_name = OTBFunctions.AllocateOTBString(elementDA.Name);
									data.m_userData = elementDA.UserData;

									Marshal.StructureToPtr(data, currentPtr, false);
									currentPtr = new IntPtr(currentPtr.ToInt64() + Marshal.SizeOf(typeof (OTSAddressSpaceElementData)));
								} //	end for
							} //	end if
						} //	end if
						break;

					case EnumAddressSpaceElementType.AE:

						ArrayList childrenAe = new ArrayList();
						res = (int) AeAddressSpaceRoot.QueryAddressSpaceElementChildren(aPath, childrenAe);

						if (ResultCode.SUCCEEDED(res))
						{
							pCount = (uint) childrenAe.Count;
							if (pCount > 0)
							{
								OTSAddressSpaceElementData data = new OTSAddressSpaceElementData();
								aChildrenList = OTBFunctions.OTAllocateMemory((int) pCount*Marshal.SizeOf(typeof (OTSAddressSpaceElementData)));

								IntPtr currentPtr = aChildrenList;
								for (int i = 0; i < pCount; i++)
								{
									AeAddressSpaceElement elementAe = childrenAe[i] as AeAddressSpaceElement;
									data.m_elementType = (byte) EnumAddressSpaceElementType.AE;
									data.m_name = OTBFunctions.AllocateOTBString(elementAe.Name);
									data.m_hasChildren = Convert.ToByte(elementAe.HasChildren);
									data.m_isBrowsable = Convert.ToByte(elementAe.IsBrowsable);
									data.m_userData = elementAe.UserData;
									Marshal.StructureToPtr(data, currentPtr, false);
									currentPtr = new IntPtr(currentPtr.ToInt64() + Marshal.SizeOf(typeof (OTSAddressSpaceElementData)));
								} //	end for
							} //	end if
						} //	end if
						break;
				} //	end switch

				return res;
			} //	end try
			catch (Exception e)
			{
				pCount = 0;
				aChildrenList = IntPtr.Zero;
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.HandleQueryAddressSpaceElementChildren",
				                           "Exception caught:" + e.ToString());

				return (int) EnumResultCode.E_FAIL;
			} //	end catch
		} //	end HandleQueryAddressSpaceElementChildren


		/// <summary>
		/// HandleCreateAddressSpaceElement
		/// </summary>
		private int HandleCreateAddressSpaceElement(
			uint hParent,
			uint hObject,
			OTSAddressSpaceElementData aDataIn,
			IntPtr aDataOut)
		{
			try
			{
				if (aDataIn == null || aDataOut == IntPtr.Zero)
				{
					return (int) EnumResultCode.E_INVALIDARG;
				} //	end if

				EnumAddressSpaceElementType elementType = (EnumAddressSpaceElementType) aDataIn.m_elementType;
				string name = Marshal.PtrToStringUni(aDataIn.m_name);

				switch (elementType)
				{
					case EnumAddressSpaceElementType.DA:

						string itemId = Marshal.PtrToStringUni(aDataIn.m_itemID);
						DaAddressSpaceElement daElem = null;

						//	Check if the user data was already added
						daElem = DaAddressSpaceRoot.GetElementFromArray(aDataIn.m_userData) as DaAddressSpaceElement;
						if (daElem != null)
						{
							daElem.ItemId = itemId;
							aDataIn.m_itemID = IntPtr.Zero;
							aDataIn.m_name = IntPtr.Zero;
							aDataIn.m_userData = daElem.UserData;
							aDataIn.m_ioMode = (byte) daElem.IoMode;

							Marshal.StructureToPtr(aDataIn, aDataOut, false);

							return (int) EnumResultCode.S_OK;
						} //	end if

						daElem = Creator.CreateInternalDaAddressSpaceElement(itemId, name, aDataIn.m_userData, hObject, hParent);

						if (daElem == null)
						{
							return (int)EnumResultCode.E_NOTIMPL;
						}

						daElem.HasChildren = (aDataIn.m_hasChildren != 0);
						daElem.IsBrowsable = (aDataIn.m_isBrowsable != 0);
						daElem.AccessRights = (EnumAccessRights) aDataIn.m_accessRights;
						daElem.Datatype = ValueQT.GetSysType(aDataIn.m_datatype);
						daElem.IoMode = (EnumIoMode) aDataIn.m_ioMode;

						DaAddressSpaceElement daParent = DaAddressSpaceRoot.GetParent(hObject) as DaAddressSpaceElement;
						if (daParent != null)
						{
							daParent.AddChild(daElem);
							aDataIn.m_itemID = IntPtr.Zero;
							aDataIn.m_name = IntPtr.Zero;
							aDataIn.m_userData = daElem.UserData;

							Marshal.StructureToPtr(aDataIn, aDataOut, false);
							return (int) EnumResultCode.S_OK;
						}
						else
						{
							return (int) EnumResultCode.E_FAIL;
						} //	end if ... else

					case EnumAddressSpaceElementType.AE:

						AeAddressSpaceElement aeElem = null;

						//	Check if the user data was already added
						aeElem = AeAddressSpaceRoot.GetElementFromArray(aDataIn.m_userData) as AeAddressSpaceElement;

						if (aeElem != null)
						{
							aDataIn.m_itemID = IntPtr.Zero;
							aDataIn.m_name = IntPtr.Zero;
							aDataIn.m_userData = aeElem.UserData;
							Marshal.StructureToPtr(aDataIn, aDataOut, false);
							return (int) EnumResultCode.S_OK;
						} //	end if

						aeElem = Creator.CreateInternalAeAddressSpaceElement(name, aDataIn.m_userData, hObject, hParent);

						if (aeElem == null)
						{
							return (int)EnumResultCode.E_NOTIMPL;
						}

						aeElem.HasChildren = (aDataIn.m_hasChildren != 0);
						aeElem.IsBrowsable = (aDataIn.m_isBrowsable != 0);

						AeAddressSpaceElement aeParent = AeAddressSpaceRoot.GetParent(hObject) as AeAddressSpaceElement;
						if (aeParent != null)
						{
							aeParent.AddChild(aeElem);
							aDataIn.m_itemID = IntPtr.Zero;
							aDataIn.m_name = IntPtr.Zero;
							aDataIn.m_userData = aeElem.UserData;

							Marshal.StructureToPtr(aDataIn, aDataOut, false);
							return (int) EnumResultCode.S_OK;
						}
						else
						{
							return (int) EnumResultCode.E_FAIL;
						} //	end if ... else

					default:

						return (int) EnumResultCode.E_FAIL;
				} //	end switch
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.HandleCreateAddressSpaceElement",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	enc try ... catch
		} //	end HandleCreateAddressSpaceElement


		/// <summary>
		/// HandleCreateAddressSpaceElementEx
		/// </summary>
		private int HandleCreateAddressSpaceElementEx(
			uint hParent,
			uint hObject,
			OTSAddressSpaceElementData aDataIn,
			ushort elementDataFilter,
			IntPtr aDataOut)
		{
			try
			{
				if (aDataIn == null || aDataOut == IntPtr.Zero)
				{
					return (int)EnumResultCode.E_INVALIDARG;
				} //	end if

				EnumAddressSpaceElementType elementType = (EnumAddressSpaceElementType) aDataIn.m_elementType;

				if (elementType == EnumAddressSpaceElementType.AE)
				{
					return (int)EnumResultCode.E_UNEXPECTED;
				}

				string name = Marshal.PtrToStringUni(aDataIn.m_name);

				string itemId = Marshal.PtrToStringUni(aDataIn.m_itemID);
				DaAddressSpaceElement daElem = null;

				//	Check if the user data was already added
				daElem = DaAddressSpaceRoot.GetElementFromArray(aDataIn.m_userData) as DaAddressSpaceElement;
				if (daElem != null)
				{
					daElem.ItemId = itemId;
					aDataIn.m_itemID = IntPtr.Zero;
					aDataIn.m_name = IntPtr.Zero;
					aDataIn.m_userData = daElem.UserData;
					aDataIn.m_ioMode = (byte)daElem.IoMode;
					Marshal.StructureToPtr(aDataIn, aDataOut, false);
					return (int)EnumResultCode.S_OK;
				} //	end if

				daElem = Creator.CreateInternalDaAddressSpaceElementEx(itemId, name, aDataIn.m_userData, hObject, hParent, (EnumAddressSpaceElementData)elementDataFilter,
					(EnumAccessRights)aDataIn.m_accessRights, (EnumIoMode)aDataIn.m_ioMode, aDataIn.m_hasChildren != 0, aDataIn.m_isBrowsable != 0, ValueQT.GetSysType(aDataIn.m_datatype));

				if (daElem == null)
				{
					return (int)EnumResultCode.E_NOTIMPL;
				}

				daElem.HasChildren = (aDataIn.m_hasChildren != 0);
				daElem.IsBrowsable = (aDataIn.m_isBrowsable != 0);
				daElem.AccessRights = (EnumAccessRights)aDataIn.m_accessRights;
				daElem.Datatype = ValueQT.GetSysType(aDataIn.m_datatype);
				daElem.IoMode = (EnumIoMode)aDataIn.m_ioMode;

				DaAddressSpaceElement daParent = DaAddressSpaceRoot.GetParent(hObject) as DaAddressSpaceElement;
				if (daParent != null)
				{
					daParent.AddChild(daElem);
					aDataIn.m_itemID = IntPtr.Zero;
					aDataIn.m_name = IntPtr.Zero;
					aDataIn.m_userData = daElem.UserData;
					Marshal.StructureToPtr(aDataIn, aDataOut, false);
					return (int)EnumResultCode.S_OK;
				}
				else
				{
					return (int)EnumResultCode.E_FAIL;
				} //	end if ... else
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
										   EnumTraceGroup.OPCSERVER,
										   "Application.HandleCreateAddressSpaceElementEx",
										   "Exception caught:" + e.ToString());
				return (int)EnumResultCode.E_FAIL;
			} //	enc try ... catch
		} //	end HandleCreateAddressSpaceElement


		/// <summary>
		/// HandleDestroyAddressSpaceElement
		/// </summary>
		private int HandleDestroyAddressSpaceElement(OTObjectData anObjectData)
		{
			try
			{
				AddressSpaceElement element =
					DaAddressSpaceRoot.GetElementFromArray(anObjectData.m_userData) as AddressSpaceElement;

				if (element != null)
				{
					DaAddressSpaceRoot.RemoveElementFromArray(element);
					element.TriggerRemovedFromAddressSpace();
					return (int) EnumResultCode.S_OK;
				} //	end if

				element = AeAddressSpaceRoot.GetElementFromArray(anObjectData.m_userData) as AddressSpaceElement;
				if (element != null)
				{
					AeAddressSpaceRoot.RemoveElementFromArray(element);
					element.TriggerRemovedFromAddressSpace();
					return (int) EnumResultCode.S_OK;
				} //	end if
				return (int) EnumResultCode.S_FALSE;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.DestroyAddressSpaceElement",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	end HandleDestroyAddressSpaceElement


		/// <summary>
		/// HandleChangeSessionState
		/// </summary>
		private int HandleChangeSessionState(
			uint aSessionHandle,
			OTSSessionData aSessionStateData)
		{
			try
			{
				if (aSessionHandle == 0 || aSessionStateData == null)
				{
					return (int) EnumResultCode.E_INVALIDARG;
				} //	end if

				DaSession session = null;

				switch ((EnumSessionState) aSessionStateData.m_state)
				{
					case EnumSessionState.CREATE:
						session = Creator.CreateSession((EnumSessionType) aSessionStateData.m_type, aSessionHandle);
						if (session != null)
						{
							session.SetClientName(aSessionStateData.m_clientName);

							AddSession(session, aSessionHandle);
							session.ConnectSession();
							return (int) EnumResultCode.S_OK;
						}
						break;

					case EnumSessionState.MODIFY:
						session = GetSession(aSessionHandle);
						if (session != null)
						{
							session.SetClientName(aSessionStateData.m_clientName);
						}
						return (int) EnumResultCode.S_OK;

					case EnumSessionState.DESTROY:
						session = GetSession(aSessionHandle);
						if (session != null)
						{
							session.DisconnectSession();
							RemoveSession(aSessionHandle);
							return (int) EnumResultCode.S_OK;
						}
						break;

					case EnumSessionState.LOGOFF:
						session = GetSession(aSessionHandle);
						if (session != null)
						{
							return session.Logoff();
						}
						break;

					case EnumSessionState.LOGON:
						session = GetSession(aSessionHandle);
						if (session != null)
						{
							return session.Logon(aSessionStateData.m_userName, aSessionStateData.m_password);
						}
						break;
				} //	end switch

				return (int) EnumResultCode.E_FAIL;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.HandleChangeSessionState",
				                           "Exception caught:" + e.ToString());

				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	end HandleChangeSessionState


		/// <summary>
		/// HandleQueryCacheValue
		/// </summary>
		private int HandleQueryCacheValue(
			uint aSessionHandle,
			OTObjectData anObjectData,
			IntPtr pOutValue)
		{
			try
			{
				DaAddressSpaceElement element =
					DaAddressSpaceRoot.GetElementFromArray(anObjectData.m_userData) as DaAddressSpaceElement;
				DaSession session = this.GetSession(aSessionHandle);

				ValueQT cacheValue = null;
				int result = element.QueryCacheValue(session, ref cacheValue);

				if (!ResultCode.SUCCEEDED(result))
				{
					return result;
				} //	end if

				if (cacheValue == null)
				{
					return (int) EnumResultCode.E_FAIL;
				} //	end if
				OTValueData valueData = (OTValueData) Marshal.PtrToStructure(pOutValue, typeof (OTValueData));
				valueData.m_quality = (ushort) cacheValue.Quality;
				valueData.m_timestamp = new OTDateTime(cacheValue.TimeStamp);
				Marshal.GetNativeVariantForObject(cacheValue.Data, valueData.m_value);
				Marshal.StructureToPtr(valueData, pOutValue, false);

				return (int) EnumResultCode.S_OK;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.HandleQueryCacheValue",
				                           "Exception caught:" + e.ToString());

				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	end HandleQueryCacheValue


		/// <summary>
		/// HandleChangeItems
		/// </summary>
		private int HandleChangeItems(uint aCount, IntPtr anItemDataPointer)
		{
			try
			{
				if ((anItemDataPointer == IntPtr.Zero) || (aCount == 0))
				{
					return (int) EnumResultCode.S_FALSE;
				} //	end if

				bool errorFound = false;
				DaAddressSpaceRoot root = Application.Instance.DaAddressSpaceRoot;
				DaAddressSpaceElement[] elements = new DaAddressSpaceElement[aCount];

				OTSItemData itemData = new OTSItemData();
				int structSize = Marshal.SizeOf(itemData);

				lock (root.ElementSyncRoot) // enumerating
				{
					for (int i = 0; i < aCount; i++)
					{
						itemData =
							(OTSItemData)
							Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(anItemDataPointer, structSize*i), typeof (OTSItemData));
						DaAddressSpaceElement element = (DaAddressSpaceElement) root.GetElementFromArray(itemData.m_object.m_userData);

						if (element != null)
						{
							element.Change(itemData.m_active != 0, itemData.m_sampleRate);
							elements[i] = element;
						}
						else
						{
							errorFound = true;
						} //	end if...else
					} //	end for

					root.ChangeItems(aCount, anItemDataPointer);
				}
				return (errorFound ? (int) EnumResultCode.E_FAIL : (int) EnumResultCode.S_OK);
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.HandleChangeItems",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end catch
		} //	end HandleChangeItems


		/// <summary>
		/// HandleQueryConditions
		/// </summary>
		private int HandleQueryConditions(
			IntPtr pObjectData,
			string aSourcePath,
			out uint aConditionCount,
			out IntPtr pConditionNames)
		{
			aConditionCount = 0;
			pConditionNames = IntPtr.Zero;

			try
			{
				int result = (int) EnumResultCode.E_FAIL;

				if (pObjectData != IntPtr.Zero)
				{
					OTObjectData objectData = (OTObjectData) Marshal.PtrToStructure(pObjectData, typeof (OTObjectData));

					string[] conditionNames = null;

					result = AeAddressSpaceRoot.QueryConditions(
						objectData.m_userData,
						aSourcePath,
						out conditionNames);

					if (conditionNames != null)
					{
						aConditionCount = (uint) conditionNames.Length;
					} //	end if

					if (aConditionCount != 0)
					{
						pConditionNames = OTBFunctions.OTAllocateMemory((int) aConditionCount*Marshal.SizeOf(typeof (IntPtr)));
						for (int index = 0; index < aConditionCount; index++)
						{
							//	Allocate the string into the unmanaged memory
							IntPtr conditionName = OTBFunctions.AllocateOTBString(conditionNames[index]);
							Marshal.WriteIntPtr(pConditionNames, index*Marshal.SizeOf(typeof (IntPtr)), conditionName);
						} //	end for
					} //	end if
				}
				return result;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.HandleQueryConditions",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	end HandleQueryConditions


		/// <summary>
		/// HandleAcknowledgeCondition
		/// </summary>
		private int HandleAcknowledgeCondition(
			OTObjectData pConditionData,
			string anAckId,
			string anAckComment)
		{
			try
			{
				Hashtable syncConditions = Hashtable.Synchronized(this.m_conditionList);
				AeCondition condition = syncConditions[(uint) pConditionData.m_objectHandle] as AeCondition;
				if (condition != null)
				{
					condition.Acknowledge(anAckId, anAckComment, DateTime.Now);
					return ChangeConditions(new AeCondition[] {condition});
				} //	end if

				return (int) EnumResultCode.E_FAIL;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.AcknowledgeCondition",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	AcknowledgeCondition


		/// <summary>
		/// HandleQueryConditionDefinition
		/// </summary>
		private int HandleQueryConditionDefinition(
			OTObjectData pConditionData,
			IntPtr aConditionDefinitionData)
		{
			try
			{
				Hashtable syncConditions = Hashtable.Synchronized(this.m_conditionList);
				AeCondition condition = syncConditions[(uint) pConditionData.m_objectHandle] as AeCondition;
				if (condition == null)
				{
					return (int) EnumResultCode.E_FAIL;
				} //	end if

				AeConditionDefinition conditionDefinition = condition.Definition;

				//	check if condition definition found
				if (conditionDefinition != null)
				{
					OTSConditionDefinitionData data = conditionDefinition.CreateOTSConditionDefinitionData();

					//	copy the structure's to the the in/out parameter
					//	NOTE: the caller is responsible for releasing the memory allocated within this method
					Marshal.StructureToPtr(data, aConditionDefinitionData, false);
					return (int) EnumResultCode.S_OK;
				} //	end if
				return (int) EnumResultCode.S_FALSE;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.HandleQueryConditionDefinition",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	end HandleQueryConditionDefinition


		/// <summary>
		/// HandleEnableConditions
		/// </summary>
		private int HandleEnableConditions(
			byte isEnable,
			string anAddressSpaceElementPath)
		{
			try
			{
				int result = (int) EnumResultCode.S_OK;
				lock (m_conditionList.SyncRoot) // enumerating
				{
					foreach (AeCondition condition in m_conditionList.Values)
					{
						if (condition.SourcePath == anAddressSpaceElementPath)
						{
							if (isEnable != 0)
							{
								result |= condition.Enable(anAddressSpaceElementPath);
							}
							else
							{
								result |= condition.Disable(anAddressSpaceElementPath);
							} //	end if ... else
						} //	end if
					} //	end foreach
				}
				return result;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.HandleEnableConditions",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	end HandleEnableConditions


		/// <summary>
		/// HandleWebTemplate
		/// </summary>
		private int HandleWebTemplate(
			string aTemplateName,
			uint aNumArgs,
			string[] pArgs,
			out IntPtr pResult)
		{
			pResult = IntPtr.Zero;

			try
			{
				string resultString;
				int result = m_webTemplate.HandleWebTemplate(aTemplateName, aNumArgs, pArgs, out resultString);

				if (ResultCode.SUCCEEDED(result) && resultString != string.Empty)
				{
					pResult = OTB.OTBFunctions.AllocateOTBString(resultString);
				} //	end if

				return (int) result;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Application.HandleWebTemplate",
				                           "Exception caught:" + e.ToString());
				pResult = IntPtr.Zero;
				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	end HandleWebTemplate


		//-
		#endregion

		#region //	Private Attributes
		//----------------------------

		private OTSCallbackFunctions m_OTNSCallBacks = new OTSCallbackFunctions();

		private OTSInitData m_initData = new OTSInitData();

		private DaAddressSpaceRoot m_DaAddressSpaceRoot = null;

		private AeAddressSpaceRoot m_AeAddressSpaceRoot = null;

		private Creator m_creator = null;

		private Hashtable m_sessionList = new Hashtable();

		private Hashtable m_transactionList = new Hashtable();

		private Hashtable m_categoryList = new Hashtable();

		private Hashtable m_conditionList = new Hashtable();

		private WebTemplate m_webTemplate = null;

		//-
		#endregion
	} //	end class Application
} //	end namespace Softing.OPCToolbox.Server 