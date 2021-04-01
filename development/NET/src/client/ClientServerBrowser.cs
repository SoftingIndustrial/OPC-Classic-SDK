using System;
using System.Runtime.InteropServices;
using System.Collections;
using Softing.OPCToolbox.OTB;

namespace Softing.OPCToolbox.Client
{

	#region //Public Delegates
	//------------------------

	///<summary>
	///<para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	///<para><b>C#, Visual Basic, C++</b>Represents the method that will handle the
	///<see cref="Softing.OPCToolbox.Client.ServerBrowser.BrowseServersCompleted">BrowseServersCompleted </see> event of a
	///<see cref="Softing.OPCToolbox.Client.ServerBrowser">ServerBrowser</see> object.</para>
	///</summary>
	///<param name="aServerBrowser">The <see cref="ServerBrowser"/> object to whom the <see cref="Softing.OPCToolbox.Client.ServerBrowser.BrowseServersCompleted"/> event belongs.</param>
	///<param name="executionContext">An identifier for an asynchronous call. It has the same value with the code of the <see cref="ServerBrowser"/> object given as first parameter.</param>
	///<param name="ipAddress">An Internet Protocol (IP) address of the computer on which the servers are to be browsed.</param>
	///<param name="serverData">A list with data of available servers on the computer.</param>
	///<param name="result">The result of browsing the servers on a computer.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="BrowseServersEventHandler"]/doc/*'
	/// />
	public delegate void BrowseServersEventHandler(
		ServerBrowser aServerBrowser,
		uint executionContext,
		string ipAddress,
		ServerBrowserData[] serverData,
		int result);

	//-

	#endregion

	///<summary>
	///<para>Browses the OPC servers installed on the local or a remote computer.</para>
	///</summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="ServerBrowser"]/doc/*'
	/// />
	public class ServerBrowser
	{
		#region //Constructors
		//--------------------

		/// <summary>
		/// Initializes a new instance of <see cref="ServerBrowser"/> class with the value indicated by an IP address.
		/// </summary>
		/// <param name="ipAddress">
		///		An Internet Protocol (IP) address of the computer on which the servers are to be browsed.
		/// </param>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowser"]/constructor[@name="ServerBrowser"]/doc/*'
		/// />
		public ServerBrowser(string ipAddress)
		{
			m_ipAddress = ipAddress;
			m_index = ++m_objectIndex;
			Hashtable synchronizedServerBrowserList = Hashtable.Synchronized(Application.Instance.m_serverBrowserList);
			synchronizedServerBrowserList.Add(m_index, this);
		}

		//-
		#endregion

		#region //Public Events
		//---------------------

		///<summary>
		///Occurs when browsing the OPC servers installed on a computer completes.
		///<para><b>C#, Visual Basic, C++</b>The event is raised by asynchronously executing the <see cref="Browse"/> method of a <see cref="ServerBrowser"/> object.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="Browse"/> of a <see cref="ServerBrowser"/> object has been asynchronously executed.</para>
		///</summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowser"]/event[@name="BrowseServersCompleted"]/doc/*'
		/// />
		public event BrowseServersEventHandler BrowseServersCompleted;

		//-
		#endregion

		#region //Private Attributes
		//--------------------------

		private int m_index = 0;

		//-
		#endregion

		#region //Protected Attributes
		//--------------------------

		/// <summary>
		/// An Internet Protocol (IP) address of a computer on which the servers are to be browsed.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowser"]/field[@name="m_ipAddress"]/doc/*'
		/// />
		protected string m_ipAddress;

		//-
		#endregion

		#region Internal Static Attributes
		//--------------------------------

		internal static int m_objectIndex = 0;

		//-
		#endregion

		#region //Public Properties
		//------------------------

		/// <summary>
		/// Gets or sets the IP address of the computer on which the servers are to be browsed.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowser"]/property[@name="IpAddress"]/doc/*'
		/// />
		public string IpAddress
		{
			get { return m_ipAddress; }
			set { m_ipAddress = value; }
		}

		//-
		#endregion

		#region //Public Methods
		//----------------------

		#region //Overriden Methods
		//-------------------

		/// <summary>
		///		Returns an unique code for a <see cref="ServerBrowser"/> object, suitable for use in data structures that associate keys with values.
		/// </summary>
		/// <returns>
		///		An unique code for the object.
		///	</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowser"]/method[@name="GetHashCode"]/doc/*'
		/// />
		public override int GetHashCode()
		{
			return this.m_index;
		}

		//-
		#endregion

		/// <summary>
		/// Browses all the servers that support a specified interface specification.
		///	The server information to be returned after browsing is specified using the <see cref="EnumServerBrowserData"/> object.
		/// After browsing, a list of <see cref="ServerBrowserData"/> objects that contain the requested information will be returned.
		/// </summary>
		/// <param name="whatOPCspecification">An OPC specification.</param>
		/// <param name="whatServerData">Indicates what information about the server to be returned.</param>
		/// <param name="serverData">A list with requested information about the available servers on the computer.</param>
		/// <param name="someExecutionOptions">Specifies the modality of execution for browsing servers on the computer.</param>
		/// <returns>The result of browsing servers on the computer.
		/// </returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowser"]/method[@name="Browse"]/doc/*'
		/// />
		public virtual int Browse(
			EnumOPCSpecification whatOPCspecification,
			EnumServerBrowserData whatServerData,
			out ServerBrowserData[] serverData,
			ExecutionOptions someExecutionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			serverData = null;
			try
			{
				byte whatSpecification = (byte) whatOPCspecification;
				byte whatData = (byte) whatServerData;
				uint serverDataCount = 0;
				IntPtr pServerData = new IntPtr();

				OTCExecutionOptions options = new OTCExecutionOptions();

				if (someExecutionOptions != null)
				{
					options.m_executionType = (byte) someExecutionOptions.ExecutionType;
					options.m_executionContext = (uint) someExecutionOptions.ExecutionContext;
				}
				else
				{
					options.m_executionType = (byte) EnumExecutionType.SYNCHRONOUS;
					options.m_executionContext = 0;
				}

				res = OTBFunctions.OTCBrowseServer(
					m_ipAddress,
					whatSpecification,
					whatData,
					ref serverDataCount,
					out pServerData,
					ref options);
				if (someExecutionOptions.ExecutionType == EnumExecutionType.SYNCHRONOUS)
				{
					if (ResultCode.SUCCEEDED(res))
					{
						uint dataCount = serverDataCount;
						serverData = new ServerBrowserData[dataCount];
						OTCServerData[] serverDataOTC = new OTCServerData[dataCount];
						OTCServerData typeOfServerData = new OTCServerData();
						for (int i = 0; i < dataCount; i++)
						{
							int structSize = Marshal.SizeOf(typeOfServerData);
							OTCServerData myData =
								(OTCServerData)
								Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(pServerData, structSize*i), typeof (OTCServerData));
							serverData[i] = new ServerBrowserData(
								Marshal.PtrToStringUni(myData.m_clsid),
								Marshal.PtrToStringUni(myData.m_progId),
								Marshal.PtrToStringUni(myData.m_description),
								Marshal.PtrToStringUni(myData.m_vProgId),
								(EnumOPCSpecification) myData.m_opcSpecification,
								Marshal.PtrToStringUni(myData.m_url));

							OTBFunctions.OTFreeMemory(myData.m_clsid);

							OTBFunctions.OTFreeMemory(myData.m_progId);

							OTBFunctions.OTFreeMemory(myData.m_description);

							OTBFunctions.OTFreeMemory(myData.m_vProgId);

							OTBFunctions.OTFreeMemory(myData.m_url);
						} //	end for

						OTBFunctions.OTFreeMemory(pServerData);
					} //	end if
				} //	end if
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"ServerBrowser.Browse",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Called when the servers from a computer are asynchronously browsed.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		///<param name="ipAddress">An Internet Protocol (IP) address of the computer on which the servers are to be browsed.</param>
		/// <param name="serverData">A list with data of available servers on the computer.</param>
		///<param name="result">The result of browsing the servers on a computer.</param>
		///<remarks>
		/// <note>
		///	The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///	</note>
		///</remarks>
		internal void OnBrowseServers(
			uint executionContext,
			string ipAddress,
			ServerBrowserData[] serverData,
			int result)
		{
			try
			{
				if (BrowseServersCompleted != null)
				{
					BrowseServersCompleted(this, executionContext, ipAddress, serverData, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"ServerBrowser.OnBrowseServers",
					exc.ToString());
			}
		}

		//-
		#endregion
	}

	/// <summary>
	/// Represents the information given when browsing the servers installed on a computer.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="ServerBrowserData"]/doc/*'
	/// />
	public class ServerBrowserData
	{
		#region //Protected Attributes
		//--------------------------

		/// <summary>
		/// A string that represents the server's CLSID.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowserData"]/field[@name="m_clsId"]/doc/*'
		/// />
		protected readonly string m_clsId;

		/// <summary>
		/// A string that represents the server's ProgID.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowserData"]/field[@name="m_progId"]/doc/*'
		/// />
		protected readonly string m_progId;

		/// <summary>
		/// A string that represents the server's description.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowserData"]/field[@name="m_description"]/doc/*'
		/// />
		protected readonly string m_description;

		/// <summary>
		/// A string that represents the server's version independent program ID.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowserData"]/field[@name="m_progIdVersionIndependent"]/doc/*'
		/// />
		protected readonly string m_progIdVersionIndependent;

		/// <summary>
		/// One of the <b>EnumOPCSpecification</b> values that represent the OPC specification supported by the server.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowserData"]/field[@name="m_opcSpecification"]/doc/*'
		/// />
		protected readonly EnumOPCSpecification m_opcSpecification;

		/// <summary>
		/// A string that represents the server's url.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowserData"]/field[@name="m_url"]/doc/*'
		/// />
		protected readonly string m_url = null;

		//-
		#endregion

		#region //Constructor
		//--------------------

		/// <summary>
		/// Initializes a new instance of <see cref="Softing.OPCToolbox.Client.ServerBrowserData"> ServerBrowserData </see> class.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowserData"]/constructor[@name="ServerBrowserData"]/doc/*'
		/// />
		public ServerBrowserData()
		{
			m_clsId = null;
			m_progId = null;
			m_description = null;
			m_progIdVersionIndependent = null;
			m_opcSpecification = 0;
			m_url = null;
		}

		/// <summary>
		/// Initializes a new instance of <see cref="Softing.OPCToolbox.Client.ServerBrowserData"> ServerBrowserData </see> class
		/// with the value indicated by a class identifier,
		/// a program identifier of the server, a description of the server,a version independent
		/// program identifier, an OPC specification and a server url.
		/// </summary>
		/// <param name="clsId">A server's CLSID. <para>The CLSID is a 128-bit value which uniquely identifies the server.</para></param>
		/// <param name="progId">A server's ProgID.<para>The ProgID contains a readable string that describes the component.The structure
		/// of the string is predifined: [Manufacturer.Server_name].</para> </param>
		/// <param name="description">A description for the server.</param>
		/// <param name="progIdVersionIndependent">A server's version independent program ID.</param>
		/// <param name="opcSpecification">An OPC Specification.</param>
		/// <param name="url">An url.</param>
		internal ServerBrowserData(string clsId, string progId, string description, string progIdVersionIndependent,
		                           EnumOPCSpecification opcSpecification, string url)
		{
			m_clsId = clsId;
			m_progId = progId;
			m_description = description;
			m_progIdVersionIndependent = progIdVersionIndependent;
			m_opcSpecification = opcSpecification;
			m_url = url;
		}

		//-
		#endregion

		#region //Public Properties
		//-------------------------

		/// <summary>
		/// Gets the server's CLSID. The CLSID is a 128-bit value which uniquely identifies a server.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowserData"]/property[@name="ClsId"]/doc/*'
		/// />
		public string ClsId
		{
			get { return m_clsId; }
		}

		/// <summary>
		/// Gets a description for the server.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowserData"]/property[@name="Description"]/doc/*'
		/// />
		public string Description
		{
			get { return m_description; }
		}

		/// <summary>
		/// Gets the OPC Specification which the server supports.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowserData"]/property[@name="OPCSpecification"]/doc/*'
		/// />
		public EnumOPCSpecification OPCSpecification
		{
			get { return m_opcSpecification; }
		}

		/// <summary>
		/// Gets the server's ProgID.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowserData"]/property[@name="ProgId"]/doc/*'
		/// />
		public string ProgId
		{
			get { return m_progId; }
		}

		/// <summary>
		/// Gets the server's version independent program ID.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowserData"]/property[@name="ProgIdVersionIndependent"]/doc/*'
		/// />
		public string ProgIdVersionIndependent
		{
			get { return m_progIdVersionIndependent; }
		}

		/// <summary>
		/// Gets the server's url.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="ServerBrowserData"]/property[@name="Url"]/doc/*'
		/// />
		public string Url
		{
			get { return m_url; }
		}

		//-
		#endregion
	}
}