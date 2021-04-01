using System;
using System.Collections;
using Softing.OPCToolbox.Client;
using System.Runtime.InteropServices;
using Softing.OPCToolbox;
using System.ComponentModel;
using System.ComponentModel.Design.Serialization;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Security.Permissions;
using System.Globalization;
using Softing.OPCToolbox.OTB;

//The Data Access Specification defines an interface between client and server programs to access process data.

namespace Softing.OPCToolbox.Client
{

	#region // Public Delegates
	//-------------------------

	/// <summary>
	///<para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	///<para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.DaSession.ReadCompleted"/>
	///event of a <see cref="DaSession"/>.</para>
	/// </summary>
	/// <param name="aDaSession">The <see cref="DaSession"/> object to whom the <see cref="Softing.OPCToolbox.Client.DaSession.ReadCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="itemIDs">A list with the identifiers for the read items.</param>
	/// <param name="itemPaths">A list with the access paths for the read items.</param>
	/// <param name="values">A list with the read values.</param>
	/// <param name="results">A list with the result of reading for each item. Each result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <param name="result">The result of reading the items. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="SessionReadEventHandler"]/doc/*'
	/// />
	public delegate void SessionReadEventHandler(
		DaSession aDaSession,
		uint executionContext,
		string[] itemIDs,
		string[] itemPaths,
		ValueQT[] values,
		int[] results,
		int result);

	/// <summary>
	///<para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	///<para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.DaSession.WriteCompleted"/>
	///event of a <see cref="DaSession"/>.</para>
	/// </summary>
	/// <param name="aDaSession">The <see cref="DaSession"/> object to whom the <see cref="Softing.OPCToolbox.Client.DaSession.WriteCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="itemIDs">A list with the identifiers for the written items.</param>
	/// <param name="itemPaths">A list with the access paths for the written items.</param>
	/// <param name="values">A list with the written values.</param>
	/// <param name="results">A list with the result of writing for each item. Each result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <param name="result">The result of writing the items. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="SessionWriteEventHandler"]/doc/*'
	/// />
	public delegate void SessionWriteEventHandler(
		DaSession aDaSession,
		uint executionContext,
		string[] itemIDs,
		string[] itemPaths,
		ValueQT[] values,
		int[] results,
		int result);

	/// <summary>
	///<para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	///<para><b>C#, Visual Basic, C++</b> Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.DaSession.GetStatusCompleted"/>
	/// event of a <see cref="DaSession"/> or of an <see cref="AeSession"/>.</para>
	/// </summary>
	/// <param name="obj">The object to whom the <b>GetStatusCompleted"</b> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="serverStatus">A <see cref="ServerStatus">ServerStatus</see> object which contains information on the server's parameters.</param>
	/// <param name="result">The result of getting the server status. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="GetStatusEventHandler"]/doc/*'
	/// />
	public delegate void GetStatusEventHandler(
		ObjectSpaceElement obj,
		uint executionContext,
		ServerStatus serverStatus,
		int result);

	/// <summary>
	///<para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	///<para><b>C#, Visual Basic, C++</b> Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.DaSession.BrowseCompleted"/>
	/// event of a <see cref="DaSession"/>.</para>
	///</summary>
	///<param name="aDaSession">The <see cref="DaSession"/> object to whom the <see cref="Softing.OPCToolbox.Client.DaSession.BrowseCompleted"/> event belongs.</param>
	///<param name="executionContext">An identifier for an asynchronous call.</param>
	///<param name="addressSpaceElementHandle">A handle for the element whose children are to be fetched.</param>
	///<param name="addressSpaceElementID">An identifier in the server's address space for the element whose children are to be fetched.</param>
	///<param name="addressSpaceElementPath">An access path for the element whose children are to be fetched.</param>
	///<param name="addressSpaceElements">A list with the children elements.</param>
	///<param name="result">The result of browsing the server's address space. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="BrowseDaServerEventHandler"]/doc/*'
	/// />
	public delegate void BrowseDaServerEventHandler(
		DaSession aDaSession,
		uint executionContext,
		uint addressSpaceElementHandle,
		string addressSpaceElementID,
		string addressSpaceElementPath,
		DaAddressSpaceElement[] addressSpaceElements,
		int result);

	/// <summary>
	///<para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.DaSession.GetDaPropertiesCompleted"/>
	/// event of a <see cref="DaSession"/>.</para>
	/// </summary>
	/// <param name="aDaSession">The <see cref="DaSession"/> object to whom the <see cref="Softing.OPCToolbox.Client.DaSession.GetDaPropertiesCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="addressSpaceElementHandle">A handle for the <see cref="AddressSpaceElement"/> whose properties are to be returned.</param>
	/// <param name="addressSpaceElementID"> An identifier for the <see cref="AddressSpaceElement"/> whose properties are to be returned.</param>
	/// <param name="addressSpaceElementPath">An access path for the <see cref="AddressSpaceElement"/> whose properties are to be returned.</param>
	/// <param name="aDaProperty">A list with the properties of an <see cref="AddressSpaceElement"/> that exists in the server's address space.</param>
	///<param name="result">The result of getting an <see cref="AddressSpaceElement"/>'s properties. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="GetDaPropertiesEventHandler"]/doc/*'
	/// />
	public delegate void GetDaPropertiesEventHandler(
		DaSession aDaSession,
		uint executionContext,
		uint addressSpaceElementHandle,
		string addressSpaceElementID,
		string addressSpaceElementPath,
		DaProperty[] aDaProperty,
		int result);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.DaSession.GetErrorStringCompleted"/>
	/// event of a <see cref="DaSession"/> or of an <see cref="AeSession"/>.</para>
	/// </summary>
	/// <param name="obj">The object to whom the <b>GetErrorStringCompleted</b> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="errorCode">An error code.</param>
	/// <param name="errorString">A textual description for the error that corresponds to the error code.</param>
	///<param name="result">The result of getting the description of an error. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="GetErrorStringEventHandler"]/doc/*'
	/// />
	public delegate void GetErrorStringEventHandler(
		ObjectSpaceElement obj,
		uint executionContext,
		int errorCode,
		string errorString,
		int result);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.DaSession.LogonCompleted"/>
	/// event of a <see cref="DaSession"/> or of an <see cref="AeSession"/>.</para>
	/// </summary>
	/// <param name="obj">The object to whom the <b>LogonCompleted</b> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="userName">A user name.</param>
	/// <param name="password">A password.</param>
	///<param name="result">The result of logging on to the server. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="LogonEventHandler"]/doc/*'
	/// />
	public delegate void LogonEventHandler(
		ObjectSpaceElement obj,
		uint executionContext,
		string userName,
		string password,
		int result);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.DaSession.LogoffCompleted"/>
	/// event of a <see cref="DaSession"/> or of an <see cref="AeSession"/>.</para>
	/// </summary>
	/// <param name="obj">The object to whom the <b>LogoffCompleted</b> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	///<param name="result">The result of logging off from the server. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="LogoffEventHandler"]/doc/*'
	/// />
	public delegate void LogoffEventHandler(
		ObjectSpaceElement obj,
		uint executionContext,
		int result);

	//-

	#endregion

	/// <summary>
	/// <para> Represents a client - Data Access Server session.</para>
	/// <para></para>
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="DaSession"]/doc/*'
	/// />
	[Serializable]
	public class DaSession : ObjectSpaceElement, ISerializable
	{
		#region //	Public Constructors
		//-----------------------------

		/// <summary>
		/// Initializes a new instance of the <see cref="DaSession"/> class with
		/// the value indicated by a session url and adds it to the application.
		/// </summary>
		/// <param name="url">The session url.</param>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/constructor[@name="DaSession"]/doc/*'
		/// />
		public DaSession(string url)
		{
			m_subscriptionList = new Hashtable();
			try
			{
				int res = (int) EnumResultCode.E_FAIL;

				res = Application.Instance.AddDaSession(url, this);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.Constructor",
						"The session could not be added to the application! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.Constructor",
					exc.ToString());
			}
		} //	end ctr


		//-
		#endregion

		#region //	Public Events
		//-----------------------

		/// <summary>
		/// Occurs when a server shuts down.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/event[@name="ShutdownRequest"]/doc/*'
		/// />
		public event ShutdownEventHandler ShutdownRequest;

		/// <summary>
		/// Occurs when reading some item values completes.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="Read"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="Read"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/event[@name="ReadCompleted"]/doc/*'
		/// />
		public event SessionReadEventHandler ReadCompleted;

		/// <summary>
		/// Occurs when writing some item values completes.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="Write"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="Write"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/event[@name="WriteCompleted"]/doc/*'
		/// />
		public event SessionWriteEventHandler WriteCompleted;

		/// <summary>
		/// Occurs when getting server status completes.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="GetStatus"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="GetStatus"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/event[@name="GetStatusCompleted"]/doc/*'
		/// />
		public event GetStatusEventHandler GetStatusCompleted;

		/// <summary>
		/// Occurs when browsing the server namespace completes.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="Browse"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="Browse"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/event[@name="BrowseCompleted"]/doc/*'
		/// />
		public event BrowseDaServerEventHandler BrowseCompleted;

		/// <summary>
		/// Occurs when getting the properties of an <see cref="Softing.OPCToolbox.Client.AddressSpaceElement"/> that belongs to the server's address space completes.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="GetDaProperties"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="GetDaProperties"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/event[@name="GetDaPropertiesCompleted"]/doc/*'
		/// />
		public event GetDaPropertiesEventHandler GetDaPropertiesCompleted;

		/// <summary>
		/// Occurs when getting textual description of an error code completes.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="GetErrorString"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="GetErrorString"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/event[@name="GetErrorStringCompleted"]/doc/*'
		/// />
		public event GetErrorStringEventHandler GetErrorStringCompleted;

		/// <summary>
		/// Occurs when the logon to the server completes.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="Logon"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="Logon"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/event[@name="LogonCompleted"]/doc/*'
		/// />
		public event LogonEventHandler LogonCompleted;

		/// <summary>
		/// Occurs when the logoff from the server completes.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="Logoff"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="Logoff"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/event[@name="LogoffCompleted"]/doc/*'
		/// />
		public event LogoffEventHandler LogoffCompleted;

		//-
		#endregion

		#region //	Protected Attributes
		//---------------------------

		/// <summary>
		/// A list with all the subscriptions (<see cref="Softing.OPCToolbox.Client.DaSubscription"/> objects) that are currently added to the session.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/field[@name="m_subscriptionList"]/doc/*'
		/// />
		protected Hashtable m_subscriptionList = new Hashtable();

		//-
		#endregion

		#region //	Private Attributes
		//-------------------------

		private readonly OTCObjectAttributes m_objectAttributes = new OTCObjectAttributes();
		private readonly object m_attributesJanitor = new object();

		//-
		#endregion

		#region //	Public Properties
		//---------------------------

		/// <summary>
		/// Gets the list with the subscriptions belonging to the server.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/property[@name="SubscriptionList"]/doc/*'
		/// />
		public DaSubscription[] SubscriptionList
		{
			get
			{
				lock (m_subscriptionList.SyncRoot) // enumerating
				{
					DaSubscription[] subscriptionVector = new DaSubscription[m_subscriptionList.Count];
					int i = 0;

					foreach (DaSubscription daSubscription in m_subscriptionList.Values)
					{
						subscriptionVector[i] = daSubscription;
						i++;
					} //	end foreach

					return subscriptionVector;
				}
			} //	end get
		} //	end SubscriptionList


		/// <summary>
		/// Gets the server's url.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/property[@name="Url"]/doc/*'
		/// />
		public string Url
		{
			get
			{
				string url = string.Empty;
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				try
				{
					if (this.Handle != 0)
					{
						lock (m_attributesJanitor)
						{
							OTCExecutionOptions options = new OTCExecutionOptions();
							int result = -1;
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.DASESSION_URL},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSession.URL.Get",
										"Update attribute DASESSION_URL from server failed!Result: " + updateResult);
								} //	end if
							} //	end lock
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASESSION_URL,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								url = Marshal.PtrToStringUni(objectAttributes.m_daSessionURL);
								OTBFunctions.OTFreeMemory(objectAttributes.m_daSessionURL);
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSession.URL.Get",
									"Get attribute DASESSION_URL from server failed!Result: " + getAttrsResult);
							} //	end else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.URL.Get",
						exc.ToString());
				}
				return url;
			} //	end get
		} //	end URL
		/// <summary>
		/// Gets or sets the valid language identifier for the current server.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/property[@name="LocaleId"]/doc/*'
		/// />
		public string LocaleId
		{
			get
			{
				uint lcid = 0;
				string cultureName = String.Empty;
				CultureInfo culture;
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						int result = -1;
						lock (m_attributesJanitor)
						{
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.DASESSION_LCID},
									new int[] {result},
									ref options);

								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSession.LocaleId.Get",
										"Update attribute DASESSION_LCID failed! Result: " + updateResult);
								}
							} //	end if
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASESSION_LCID,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								lcid = objectAttributes.m_daSessionLCID;
								switch (lcid)
								{
									case 2048:

										cultureName = "System Default";

										break;
									case 1024:

										cultureName = "User Default";
										break;
									case 0:

										cultureName = "Neutral";
										break;
									default:

										culture = new CultureInfo((int) lcid);
										cultureName = culture.Name;
										break;
								}
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSession.LocaleId.Get",
									"Get attribute DASESSION_LCID failed! Error message: " + getAttrsResult);
							} //	end else
						} //	end lock
					} //	end if
				}
				catch (ArgumentException exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.INF,
						EnumTraceGroup.CLIENT,
						"Da.Session.LocaleID.Get",
						exc.Message);
					cultureName = lcid.ToString();
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Session.LocaleID.Get",
						exc.ToString());
				}
				return cultureName;
			}
			set
			{
				uint lcid;
				try
				{
					switch (value)
					{
						case "System Default":

							lcid = 2048;

							break;
						case "User Default":

							lcid = 1024;
							break;
						case "Neutral":

							lcid = 0;
							break;
						default:

							CultureInfo cultureInfo = new CultureInfo(value);
							lcid = (uint) cultureInfo.LCID;
							break;
					} //	end switch

					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						lock (m_attributesJanitor)
						{
							m_objectAttributes.m_daSessionLCID = lcid;

							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASESSION_LCID,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSession.LocaleId.Set",
									"Set attribute DASESSION_LCID failed! Result: " + setAttrsResult);
							} //	end if
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.DASESSION_LCID},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSession.LocaleId.Set",
										"Set attribute DASESSION_LCID failed! Result: " + updateResult);
								} //	end if
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSession.LocaleId.Set",
							"Set LocaleId failed! Session is not added to the application");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Session.LocaleID.Set",
						exc.ToString());
				} //	end try...catch
			} //	end set
		} //	end LocaleId


		/// <summary>
		/// Gets or sets information about a client.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/property[@name="ClientName"]/doc/*'
		/// />
		public string ClientName
		{
			get
			{
				string clientName = string.Empty;
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						lock (m_attributesJanitor)
						{
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.DASESSION_CLIENTNAME},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSession.ClientName.Get",
										"Get attribute DASESSION_CLIENTNAME failed! Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASESSION_CLIENTNAME,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								clientName = Marshal.PtrToStringUni(objectAttributes.m_daSessionClientName);
								OTBFunctions.OTFreeMemory(objectAttributes.m_daSessionClientName);
							}
							else
							{
								if (ResultCode.FAILED(getAttrsResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSession.ClientName.Get",
										"Get attribute DASESSION_CLIENTNAME failed! Result: " + getAttrsResult);
								} //	end if
							} //	end if...else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Session.ClientName.Get",
						exc.ToString());
				}
				return clientName;
			} //	end get
			set
			{
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						if (value != string.Empty)
						{
							lock (m_attributesJanitor)
							{
								m_objectAttributes.m_daSessionClientName = Marshal.StringToCoTaskMemUni(value);
								int setAttrsResult = OTBFunctions.OTCSetAttributes(
									this.Handle,
									(uint) EnumObjectAttribute.DASESSION_CLIENTNAME,
									m_objectAttributes);
								if (ResultCode.FAILED(setAttrsResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSession.ClientName.Set",
										"Set attribute DASESSION_CLIENTNAME failed! Result: " + setAttrsResult);
								}
								if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
								{
									int updateResult = OTBFunctions.OTCUpdateAttributes(
										this.Handle,
										0,
										1,
										new uint[] {(uint) EnumObjectAttribute.DASESSION_CLIENTNAME},
										new int[] {result},
										ref options);
									if (ResultCode.FAILED(updateResult))
									{
										Application.Instance.Trace(
											EnumTraceLevel.ERR,
											EnumTraceGroup.CLIENT,
											"DaSession.ClientName.Set",
											"Set attribute DASESSION_CLIENTNAME failed! Result: " + updateResult);
									}
								} //	end if
								Marshal.FreeCoTaskMem(m_objectAttributes.m_daSessionClientName);
							} //	end lock
						} //	end if
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSession.ClientName.Set",
							"Set ClientName failed! Session is not added to the application");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Session.ClientName.Set",
						exc.ToString());
				} //	end try...catch
			} //	end set
		} //	end ClientName


		/// <summary>
		/// Gets or sets the session`s XML HTTP request timeout.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/property[@name="RequestTimeout"]/doc/*'
		/// />
		public uint RequestTimeout
		{
			get
			{
				uint requestTimeout = 0;
				try
				{
					if (this.Handle != 0)
					{
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						lock (m_attributesJanitor)
						{
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASESSION_REQUESTTIMEOUT,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								requestTimeout = objectAttributes.m_daSessionRequestTimeout;
							}
							else
							{
								if (ResultCode.FAILED(getAttrsResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSession.RequestTimeout.Get",
										"Get attribute DASESSION_REQUESTTIMEOUT failed! Result: " + getAttrsResult);
								} //	end if
							} //	end if...else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Session.RequestTimeout.Get",
						exc.ToString());
				}
				return requestTimeout;
			} //	end get
			set
			{
				try
				{
					if (this.Handle != 0)
					{
						if (value != 0)
						{
							lock (m_attributesJanitor)
							{
								m_objectAttributes.m_daSessionRequestTimeout = value;
								int setAttrsResult = OTBFunctions.OTCSetAttributes(
									this.Handle,
									(uint) EnumObjectAttribute.DASESSION_REQUESTTIMEOUT,
									m_objectAttributes);
								if (ResultCode.FAILED(setAttrsResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSession.RequestTimeout.Set",
										"Set attribute DASESSION_REQUESTTIMEOUT failed! Result: " + setAttrsResult);
								}
							} //	end lock
						} //	end if
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSession.RequestTimeout.Set",
							"Set RequestTimeout failed! Session is not added to the application");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Session.RequestTimeout.Set",
						exc.ToString());
				} //	end try...catch
			} //	end set
		} //	end RequestTimeout


		/// <summary>
		/// Gets or sets whether to perform an initial read or not.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/property[@name="PerformInitialRead"]/doc/*'
		/// />
		public bool PerformInitialRead
		{
			get
			{
				byte performInitialRead = 0;
				try
				{
					if (this.Handle != 0)
					{
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						lock (m_attributesJanitor)
						{
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASESSION_PERFORMINITIALREAD,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								performInitialRead = objectAttributes.m_daSessionPerformInitialRead;
							}
							else
							{
								if (ResultCode.FAILED(getAttrsResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSession.PerformInitialRead.Get",
										"Get attribute DASESSION_PERFORMINITIALREAD failed! Result: " + getAttrsResult);
								} //	end if
							} //	end if...else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Session.PerformInitialRead.Get",
						exc.ToString());
				}
				if (performInitialRead == 1)
				{
					return true;
				}
				return false;
			} //	end get
			set
			{
				try
				{
					if (this.Handle != 0)
					{
						lock (m_attributesJanitor)
						{
							m_objectAttributes.m_daSessionPerformInitialRead = 0;
							if (value)
							{
								m_objectAttributes.m_daSessionPerformInitialRead = 1;
							}
							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASESSION_PERFORMINITIALREAD,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSession.RequestTimeout.Set",
									"Set attribute DASESSION_PERFORMINITIALREAD failed! Result: " + setAttrsResult);
							}
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSession.PerformInitialRead.Set",
							"Set PerformInitialRead failed! Session is not added to the application");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Session.PerformInitialRead.Set",
						exc.ToString());
				} //	end try...catch
			} //	end set
		} //	end RequestTimeout


		/// <summary>
		/// Gets the OPC Specification supported by the server.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/property[@name="SupportedOpcSpecification"]/doc/*'
		/// />
		public Softing.OPCToolbox.EnumOPCSpecification SupportedOpcSpecification
		{
			get
			{
				EnumOPCSpecification specification = EnumOPCSpecification.DEFAULT;
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						lock (m_attributesJanitor)
						{
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.DASESSION_SUPPORTEDOPCSPECIFICATION},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSession.SupportedOPCSpecification.Get",
										"Update attribute DASESSION_SUPPORTEDOPCSPECIFICATION failed! Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASESSION_SUPPORTEDOPCSPECIFICATION,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								specification = (EnumOPCSpecification) objectAttributes.m_daSessionSupportedOPCSpecification;
							}
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSession.SupportedOPCSpecification.Get",
									"Get attribute DASESSION_SUPPORTEDOPCSPECIFICATION failed! Result: " + getAttrsResult);
							} //	end if...else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Session.SupportedOPCSpecification.Get",
						exc.ToString());
				} //	end try...catch
				return specification;
			} //	end get
		} //	end SupportedOPCSpecification


		/// <summary>
		/// Gets or sets the forced OPC specification.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/property[@name="ForcedOpcSpecification"]/doc/*'
		/// />
		public Softing.OPCToolbox.EnumOPCSpecification ForcedOpcSpecification
		{
			get
			{
				EnumOPCSpecification specification = EnumOPCSpecification.DEFAULT;
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						lock (m_attributesJanitor)
						{
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.DASESSION_FORCEDOPCSPECIFICATION},
									new int[] {result},
									ref options);

								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSession.ForcedOPCSpecification.Get",
										"Update attribute DASESSION_FORCEDOPCSPECIFICATION failed! Result: " + updateResult);
								} //	end if
							} //	end if
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASESSION_FORCEDOPCSPECIFICATION,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								specification = (EnumOPCSpecification) objectAttributes.m_daSessionForcedOPCSpecification;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSession.SupportedOPCSpecification.Get",
									"Update attribute DASESSION_SUPPORTEDOPCSPECIFICATION failed! Result: " + getAttrsResult);
							}
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.ForcedOPCSpecification.Get",
						exc.ToString());
				}
				return specification;
			} //	end get
			set
			{
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						lock (m_attributesJanitor)
						{
							m_objectAttributes.m_daSessionForcedOPCSpecification = (byte) value;
							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASESSION_FORCEDOPCSPECIFICATION,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSession.ForcedOPCSpecification.Set",
									"Set attribute DASESSION_FORCEDOPCSPECIFICATION failed! Result: " + setAttrsResult);
							}
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.DASESSION_FORCEDOPCSPECIFICATION},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSession.ForcedOPCSpecification.Set",
										"Update attribute DASESSION_FORCEDOPCSPECIFICATION failed! Result: " + updateResult);
								}
							} //	end if
						} //	end lock
					} //	end if
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSession.ForcedOPCSpecification.Set",
							"Set ForcedOpcSpecification failed! Session is not added to the application");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Session.ForcedOPCSpecification.Set",
						exc.ToString());
				}
			} //	end set
		} //	end ForcedOPCSpecification


		/// <summary>
		/// Gets or sets the execution context in which the Data Access Server is to be run.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/property[@name="ClassContext"]/doc/*'
		/// />
		public EnumClassContext ClassContext
		{
			get
			{
				uint classContext = 0;
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						int result = -1;
						lock (m_attributesJanitor)
						{
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.DASESSION_CLASSCONTEXT},
									new int[] {result},
									ref options);

								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSession.ClassContext.Get",
										"Update attribute DASESSION_CLASSCONTEXT failed! Result: " + updateResult);
								}
							} //	end if
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASESSION_CLASSCONTEXT,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								classContext = objectAttributes.m_daSessionComClsctx;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSession.ClassContext.Get",
									"Get attribute DASESSION_CLASSCONTEXT failed! Error message: " + getAttrsResult);
							} //	end else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Session.LocaleID.Get",
						exc.ToString());
				} //	end try...catch
				return (EnumClassContext) classContext;
			} //	end get
			set
			{
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						lock (m_attributesJanitor)
						{
							m_objectAttributes.m_daSessionComClsctx = (uint) value;
							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASESSION_CLASSCONTEXT,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSession.ClassContext.Set",
									"Set attribute DASESSION_CLASSCONTEXT failed! Result: " + setAttrsResult);
							} //	end if
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.DASESSION_CLASSCONTEXT},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSession.ClassContext.Set",
										"Set attribute DASESSION_CLASSCONTEXT failed! Result: " + updateResult);
								} //	end if
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSession.ClassContext.Set",
							"Set ClassContext failed! Session is not added to the application");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.ClassContext.Set",
						exc.ToString());
				} //	end try...catch
			} //	end set
		}

		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		/// Adds a <see cref="DaSubscription"/> object to the current session.
		/// </summary>
		/// <param name="updateRate">The cyclic rate that the  active <see cref="DaItem"/> objects have to be read.</param>
		/// <param name="aSubscription">The <see cref="DaSubscription"/> object to be added to the current session.</param>
		/// <returns>The result of adding the <see cref="DaSubscription"/> object.</returns>
		internal virtual int AddDaSubscription(
			uint updateRate,
			DaSubscription aSubscription)
		{
			int res = (int) EnumResultCode.E_FAIL;
			try
			{
				if (aSubscription != null)
				{
					uint groupHandle = 0;
					res = OTBFunctions.OTCAddDASubscription(
						this.Handle,
						updateRate,
						(uint) aSubscription.GetHashCode(),
						ref groupHandle);

					if (ResultCode.SUCCEEDED(res))
					{
						aSubscription.DaSession = this;
						aSubscription.Handle = groupHandle;
						lock (m_subscriptionList.SyncRoot) // modifying content
						{
							m_subscriptionList.Add(aSubscription.GetHashCode(), aSubscription);
						}
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSession.AddDaSubscription",
							"Adding subscription to the session failed! Result: " + res);
					}
				}
				else
				{
					res = (int) EnumResultCode.E_INVALIDARG;
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.AddDaSubscription",
						" The subsription argument is invalid");
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.AddDaSubscription",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Removes a <see cref="DaSubscription"/> object from the current session.
		/// </summary>
		/// <param name="aSubscription">A <see cref="DaSubscription">Subscription</see> object to be removed from the server.</param>
		/// <returns>The result of removing the <see cref="DaSubscription">Subscription</see> object.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/method[@name="RemoveDaSubscription"]/doc/*'
		/// />
		public virtual int RemoveDaSubscription(DaSubscription aSubscription)
		{
			int res = (int) EnumResultCode.E_FAIL;
			try
			{
				if (aSubscription != null)
				{
					res = OTBFunctions.OTCRemoveDASubscription(aSubscription.Handle);

					if (ResultCode.SUCCEEDED(res))
					{
						lock (m_subscriptionList.SyncRoot) // modifying content
						{
							m_subscriptionList.Remove(aSubscription.GetHashCode());
						}
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSession.RemoveDaSubscription",
							" Removing subscription from the session failed! Result: " + res);
					}
				}
				else
				{
					res = (int) EnumResultCode.E_INVALIDARG;
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.RemoveDaSubscription",
						"The argument is invalid");
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.RemoveDaSubscription",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Reads the current values for a set of items. The items are identified by their ID and access path. The
		/// ID and access path of an item can be determined using the <see cref="Browse"/> method of a <see cref="DaSession"/>
		/// which returns a list with the elements in the server's namespace together with information about them (such as name, ID, etc.)
		/// </summary>
		/// <param name="maxAge">The oldest (in milliseconds) acceptable cached value when reading an item.
		/// </param>
		/// <param name="itemIDs">A list with the identifiers for the items to be read.</param>
		/// <param name="itemPaths">A list with the access paths for the items to be read.</param>
		/// <param name="values">A list with the read values.</param>
		/// <param name="results">A list with the result of the read for each item. </param>
		/// <param name="executionOptions">Specifies the modality of execution for reading items.</param>
		/// <returns>The result of reading items from a server.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/method[@name="Read"]/doc/*'
		/// />
		public virtual int Read(
			uint maxAge,
			string[] itemIDs,
			string[] itemPaths,
			out ValueQT[] values,
			out int[] results,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			results = new int[itemIDs.Length];
			values = null;
			try
			{
				OTCExecutionOptions options = new OTCExecutionOptions();

				if (executionOptions != null)
				{
					options.m_executionType = (byte) executionOptions.ExecutionType;
					options.m_executionContext = (uint) executionOptions.ExecutionContext;
				}
				else
				{
					options.m_executionType = (byte) EnumExecutionType.SYNCHRONOUS;
					options.m_executionContext = 0;
				}

				IntPtr[] ahObjects = new IntPtr[itemIDs.Length];
				OTValueData[] outValues = new OTValueData[itemIDs.Length];
				for (int i = 0; i < itemIDs.Length; i++)
				{
					outValues[i].m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
					Marshal.GetNativeVariantForObject(null, outValues[i].m_value);
				}
				res = OTBFunctions.OTCRead(
					this.Handle,
					itemIDs.Length,
					null,
					itemIDs,
					itemPaths,
					maxAge,
					outValues,
					results,
					ref options);

				if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
				{
					if (ResultCode.SUCCEEDED(res))
					{
						values = new ValueQT[itemIDs.Length];
						for (int i = 0; i < outValues.Length; i++)
						{
							values[i] = new ValueQT(ref outValues[i]);
						} //	end for
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSession.Read",
							" Reading items using session failed! Result: " + res);
					}
				} //	end if execution options
				for (int i = 0; i < itemIDs.Length; i++)
				{
					OTBFunctions.OTVariantClear(outValues[i].m_value);
					Marshal.FreeCoTaskMem(outValues[i].m_value);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.Read",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Writes the value, quality and timestamp for a set of items. The items are identified by their ID and access path.
		/// The ID and access path of an item can be determined using the <see cref="Browse"/> method of a <see cref="DaSession"/>
		/// which returns a list with the elements in the server's namespace together with information about them (such as name, ID, etc.)
		/// </summary>
		/// <param name="itemIDs">A list with the identifiers for the items to be written.</param>
		/// <param name="itemPaths">A list with the access paths for the items to be written.</param>
		/// <param name="values">A list with the values to be written.</param>
		/// <param name="results">A list with the result of the write for each item. </param>
		/// <param name="executionOptions">Specifies the modality of execution for writing items.</param>
		/// <returns>The result of writing items to a server.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/method[@name="Write"]/doc/*'
		/// />
		public virtual int Write(
			string[] itemIDs,
			string[] itemPaths,
			ValueQT[] values,
			out int[] results,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			results = new int[itemIDs.Length];

			if (values == null)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.Write",
					"Values cannot be null!");

				return res;
			} //	end if

			try
			{
				OTCExecutionOptions options = new OTCExecutionOptions();

				if (executionOptions != null)
				{
					options.m_executionType = (byte) executionOptions.ExecutionType;
					options.m_executionContext = (uint) executionOptions.ExecutionContext;
				}
				else
				{
					options.m_executionType = (byte) EnumExecutionType.SYNCHRONOUS;
					options.m_executionContext = 0;
				}

				IntPtr[] itemHandles = new IntPtr[itemIDs.Length];
				OTValueData[] inValues = new OTValueData[itemIDs.Length];

				for (int i = 0; i < itemIDs.Length; i++)
				{
					inValues[i].m_quality = (ushort) values[i].Quality;

					if (values[i].TimeStamp == DateTime.MinValue)
					{
						inValues[i].m_timestamp = new OTDateTime();
					}
					else
					{
						inValues[i].m_timestamp = new OTDateTime(values[i].TimeStamp);
					}

					inValues[i].m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
					Marshal.GetNativeVariantForObject(values[i].Data, inValues[i].m_value);
				}

				res = OTBFunctions.OTCWrite(
					this.Handle,
					itemIDs.Length,
					null,
					itemIDs,
					itemPaths,
					inValues,
					results,
					ref options);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.Write",
						"Writing items using subscription failed! Result: " + res);
				}

				for (int i = 0; i < itemIDs.Length; i++)
				{
					OTBFunctions.OTVariantClear(inValues[i].m_value);
					Marshal.FreeCoTaskMem(inValues[i].m_value);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.Write",
					exc.ToString());
			}
			return res;
		}

		///<summary>
		///Browses the server's address space at the specified branch.
		///</summary>
		///<param name="rootID">An identifier in server address space for the element whose children are to be fetched.</param>
		///<param name="rootPath">A path in server address space for the element whose children are to be fetched.</param>
		///<param name="browseOptions">An <see cref="AddressSpaceElementBrowseOptions"/> object that defines the browsing options.</param>
		///<param name="addressSpaceElements">A list with the children elements.</param>
		///<param name="executionOptions">Specifies the modality of execution for browsing the address space.</param>
		///<returns>The result of browsig the server's address space.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/method[@name="Browse"]/doc/*'
		/// />
		public virtual int Browse(
			string rootID,
			string rootPath,
			DaAddressSpaceElementBrowseOptions browseOptions,
			out DaAddressSpaceElement[] addressSpaceElements,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			addressSpaceElements = null;
			OTCAddressSpaceBrowseOptions OTCBrowseOptions = new OTCAddressSpaceBrowseOptions();
			OTCBrowseOptions.m_accessRightsFilter = 0;

			try
			{
				OTCExecutionOptions options = new OTCExecutionOptions();

				if (executionOptions != null)
				{
					options.m_executionType = (byte) executionOptions.ExecutionType;
					options.m_executionContext = (uint) executionOptions.ExecutionContext;
				}
				else
				{
					options.m_executionType = (byte) EnumExecutionType.SYNCHRONOUS;
					options.m_executionContext = 0;
				}

				if (browseOptions != null)
				{
					OTCBrowseOptions.m_type = (byte) browseOptions.ElementTypeFilter;
					OTCBrowseOptions.m_elementNameFilter = Marshal.StringToCoTaskMemUni(browseOptions.ElementNameFilter);
					OTCBrowseOptions.m_vendorFilter = Marshal.StringToCoTaskMemUni(browseOptions.VendorFilter);
					OTCBrowseOptions.m_continuationPoint = OTBFunctions.AllocateOTBString(browseOptions.ContinuationPoint);
					OTCBrowseOptions.m_accessRightsFilter = (uint) browseOptions.AccessRightsFilter;
					OTCBrowseOptions.m_dataTypeFilter = ValueQT.GetVartype(browseOptions.DataTypeFilter);
					OTCBrowseOptions.m_maxElements = browseOptions.MaxElements;
					OTCBrowseOptions.m_retrieveItemID = (byte) (browseOptions.RetrieveItemId ? 1 : 0);
					OTCBrowseOptions.m_retrieveProperties = (byte) (browseOptions.ReturnProperties ? 1 : 0);
					OTCBrowseOptions.m_retrievePropertyValues = (byte) (browseOptions.ReturnPropertyValues ? 1 : 0);
					OTCBrowseOptions.m_forceBrowseUp = (byte) (browseOptions.ForceBrowseUp ? 1 : 0);
				}

				IntPtr pOTCaddressSpaceElements = new IntPtr();
				uint addressSpaceElementDataCount = 0;

				res = OTBFunctions.OTCBrowseAddressSpace(
					this.Handle,
					0,
					rootID,
					rootPath,
					ref OTCBrowseOptions,
					ref addressSpaceElementDataCount,
					out pOTCaddressSpaceElements,
					ref options);

				addressSpaceElements = new DaAddressSpaceElement[addressSpaceElementDataCount];

				if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
				{
					if (ResultCode.SUCCEEDED(res))
					{
						OTCAddressSpaceElementData typeOfAddressSpaceElement = new OTCAddressSpaceElementData();
						int structSize = Marshal.SizeOf(typeOfAddressSpaceElement);

						for (int i = 0; i < addressSpaceElementDataCount; i++)
						{
							OTCAddressSpaceElementData myData =
								(OTCAddressSpaceElementData)
								Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(pOTCaddressSpaceElements, structSize*i),
								                       typeof (OTCAddressSpaceElementData));
							addressSpaceElements[i] = new DaAddressSpaceElement(
								(EnumAddressSpaceElementType) myData.m_type,
								Marshal.PtrToStringUni(myData.m_name),
								Marshal.PtrToStringUni(myData.m_itemID),
								Marshal.PtrToStringUni(myData.m_itemPath),
								myData.m_objectHandle,
								this);
							OTBFunctions.OTFreeMemory(myData.m_itemID);
							OTBFunctions.OTFreeMemory(myData.m_name);
							OTBFunctions.OTFreeMemory(myData.m_itemPath);
						}
						if (pOTCaddressSpaceElements != IntPtr.Zero)
						{
							OTBFunctions.OTFreeMemory(pOTCaddressSpaceElements);
						}
						browseOptions.ContinuationPoint = Marshal.PtrToStringUni(OTCBrowseOptions.m_continuationPoint);
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSession.Browse",
							"Browsing server's address space failed! Result: " + res);
					}
				}
				Marshal.FreeCoTaskMem(OTCBrowseOptions.m_elementNameFilter);
				Marshal.FreeCoTaskMem(OTCBrowseOptions.m_vendorFilter);
				OTBFunctions.OTFreeMemory(OTCBrowseOptions.m_continuationPoint);
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.Browse",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		///  Returns the properties for an <see cref="AddressSpaceElement"/>. The AddressSpaceElement is identified by its ID and path.
		/// </summary>
		/// <param name="anAddressSpaceElementId">An identifier for the AddressSpaceElement whose properties are to be returned.</param>
		/// <param name="anAddressSpaceElementPath">An access path for the AddressSpaceElement whose properties are to be returned.</param>
		/// <param name="aGetPropertyOptions">A <see cref="DaGetPropertiesOptions"/> object that defines the options for getting properties.</param>
		/// <param name="someDaProperties">A list with an AddressSpaceElement properties.</param>
		///<param name="executionOptions">Specifies the modality of execution for getting properties.</param>
		///<returns>The result of getting an AddressSpaceElement properties.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/method[@name="GetDaProperties"]/doc/*'
		/// />
		public virtual int GetDaProperties(
			string anAddressSpaceElementId,
			string anAddressSpaceElementPath,
			DaGetPropertiesOptions aGetPropertyOptions,
			out DaProperty[] someDaProperties,
			ExecutionOptions executionOptions)
		{
			return GetDaProperties(null, anAddressSpaceElementId, anAddressSpaceElementPath, aGetPropertyOptions,
			                       out someDaProperties, executionOptions);
		}

		internal virtual int GetDaProperties(
			DaAddressSpaceElement anAddressSpaceElement,
			string anAddressSpaceElementId,
			string anAddressSpaceElementPath,
			DaGetPropertiesOptions aGetPropertyOptions,
			out DaProperty[] someDaProperties,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;

			try
			{
				//	Setup the Execution Options:
				OTCExecutionOptions options = new OTCExecutionOptions();
				uint aeHandle = 0;

				if (anAddressSpaceElement != null)
				{
					aeHandle = anAddressSpaceElement.Handle;
				}

				if (executionOptions != null)
				{
					options.m_executionType = (byte) executionOptions.ExecutionType;
					options.m_executionContext = (uint) executionOptions.ExecutionContext;
				}
				else
				{
					options.m_executionType = (byte) EnumExecutionType.SYNCHRONOUS;
					options.m_executionContext = 0;
				}

				uint propertiesDataCount = 0;

				OTCGetDAPropertiesOptions otcPropertyOptions = new OTCGetDAPropertiesOptions();
				IntPtr[] stringPointers = null;

				if (aGetPropertyOptions != null)
				{
					if (aGetPropertyOptions.PropertyIds != null)
					{
						int memorySize = aGetPropertyOptions.PropertyIds.Length*Marshal.SizeOf(typeof (System.UInt32));
						otcPropertyOptions.m_pPropertyIDs = Marshal.AllocCoTaskMem(memorySize);
						Marshal.Copy(aGetPropertyOptions.PropertyIds,
						             0,
						             otcPropertyOptions.m_pPropertyIDs,
						             aGetPropertyOptions.PropertyIds.Length);
					} //	end if

					if (aGetPropertyOptions.PropertyNames != null)
					{
						stringPointers = new IntPtr[aGetPropertyOptions.PropertyNames.Length];
						int size = aGetPropertyOptions.PropertyNames.Length*Marshal.SizeOf(stringPointers[0]);
						otcPropertyOptions.m_pPropertyNames = Marshal.AllocCoTaskMem(size);
						for (int index = 0; index < aGetPropertyOptions.PropertyNames.Length; index++)
						{
							stringPointers[index] = Marshal.StringToHGlobalAuto(aGetPropertyOptions.PropertyNames[index]);
							Marshal.WriteIntPtr(otcPropertyOptions.m_pPropertyNames, index*Marshal.SizeOf(typeof (IntPtr)),
							                    stringPointers[index]);
						} //	end for
					} //	end if

					if (aGetPropertyOptions.PropertyIds != null)
					{
						otcPropertyOptions.m_propertyCount = (uint) aGetPropertyOptions.PropertyIds.Length;
					}
					else
					{
						if (aGetPropertyOptions.PropertyNames != null)
						{
							otcPropertyOptions.m_propertyCount = (uint) aGetPropertyOptions.PropertyNames.Length;
						}
					}
					otcPropertyOptions.m_whatPropertyData = (byte) aGetPropertyOptions.WhatPropertyData;
					otcPropertyOptions.m_getLocalFromAddressSpaceElement = 0;
				}
				else
				{
					otcPropertyOptions.m_getLocalFromAddressSpaceElement = 1;
				}

				IntPtr pPropertyData = IntPtr.Zero;

				someDaProperties = null;
				res = OTBFunctions.OTCGetDAProperties(
					this.Handle,
					aeHandle,
					anAddressSpaceElementId,
					anAddressSpaceElementPath,
					otcPropertyOptions,
					out propertiesDataCount,
					out pPropertyData,
					ref options);

				if (ResultCode.SUCCEEDED(res))
				{
					if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
					{
						if (propertiesDataCount > 0)
						{
							someDaProperties = new DaProperty[propertiesDataCount];

							for (int index = 0; index < propertiesDataCount; index++)
							{
								OTCDAPropertyData myData = new OTCDAPropertyData();
								int structSize = Marshal.SizeOf(myData);

								myData =
									(OTCDAPropertyData)
									Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(pPropertyData, structSize*index),
									                       typeof (OTCDAPropertyData));
								someDaProperties[index] = new DaProperty(
									myData.m_ID,
									Marshal.PtrToStringUni(myData.m_name),
									Marshal.PtrToStringUni(myData.m_itemID),
									Marshal.PtrToStringUni(myData.m_itemPath),
									Marshal.PtrToStringUni(myData.m_description),
									ValueQT.GetSysType(myData.m_datatype),
									new ValueQT(ref myData.m_value),
									myData.m_result);
								if (myData.m_name != IntPtr.Zero)
								{
									OTBFunctions.OTFreeMemory(myData.m_name);
								}
								if (myData.m_itemID != IntPtr.Zero)
								{
									OTBFunctions.OTFreeMemory(myData.m_itemID);
								}
								if (myData.m_itemPath != IntPtr.Zero)
								{
									OTBFunctions.OTFreeMemory(myData.m_itemPath);
								}
								if (myData.m_description != IntPtr.Zero)
								{
									OTBFunctions.OTFreeMemory(myData.m_description);
								}
								if (myData.m_value.m_value != IntPtr.Zero)
								{
									OTBFunctions.OTVariantClear(myData.m_value.m_value);
									OTBFunctions.OTFreeMemory(myData.m_value.m_value);
								}
							} //	end for

							if (pPropertyData != IntPtr.Zero)
							{
								OTBFunctions.OTFreeMemory(pPropertyData);
							}
						} //	end if
					} //	end if
				}
				else
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.GetDaProperties",
						"Getting properties failed! Result: " + res);
				} //	end if...else

				if (otcPropertyOptions.m_pPropertyIDs != IntPtr.Zero)
				{
					Marshal.FreeCoTaskMem(otcPropertyOptions.m_pPropertyIDs);
				} //	end if
				if (stringPointers != null)
				{
					foreach (IntPtr stringPointer in stringPointers)
					{
						if (stringPointer != IntPtr.Zero)
						{
							Marshal.FreeCoTaskMem(stringPointer);
						}
					}
				}
				if (otcPropertyOptions.m_pPropertyNames != IntPtr.Zero)
				{
					Marshal.FreeCoTaskMem(otcPropertyOptions.m_pPropertyNames);
				} //	end if
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.GetDaProperties",
					exc.ToString());

				someDaProperties = null;
			}
			return res;
		}

		/// <summary>
		/// Gives information about the server's parameters.
		/// </summary>
		/// <param name="serverStatus">A <see cref="ServerStatus"/> object that contains information on the server's parameters.</param>
		///<param name="executionOptions">Specifies the modality of execution for getting the server status.</param>
		///<returns>The result of getting the server status.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/method[@name="GetStatus"]/doc/*'
		/// />
		public virtual int GetStatus(
			out ServerStatus serverStatus,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			serverStatus = null;
			try
			{
				OTCExecutionOptions options = new OTCExecutionOptions();

				if (executionOptions != null)
				{
					options.m_executionType = (byte) executionOptions.ExecutionType;
					options.m_executionContext = (uint) executionOptions.ExecutionContext;
				}
				else
				{
					options.m_executionType = (byte) EnumExecutionType.SYNCHRONOUS;
					options.m_executionContext = 0;
				}

				OTCServerStatus currentStatus = new OTCServerStatus();
				res = OTBFunctions.OTCGetServerStatus(
					this.Handle,
					currentStatus,
					ref options);

				if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
				{
					if (ResultCode.SUCCEEDED(res))
					{
						uint state = currentStatus.m_state;
						DateTime startTime = currentStatus.m_startTime.ToDateTime();
						DateTime lastUpdateTime = currentStatus.m_lastUpdateTime.ToDateTime();
						DateTime currentTime = currentStatus.m_currentTime.ToDateTime();

						uint[] supportedLCIDs = new uint[currentStatus.m_countLCIDs];
						string[] LCIDs = new string[currentStatus.m_countLCIDs];
						int uintSize = Marshal.SizeOf(typeof (uint));

						CultureInfo culture;
						for (int i = 0; i < currentStatus.m_countLCIDs; i++)
						{
							uint myData =
								(uint)
								Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(currentStatus.m_supportedLCIDs, uintSize*i), typeof (uint));
							supportedLCIDs[i] = myData;
							try
							{
								switch (supportedLCIDs[i])
								{
									case 2048:

										LCIDs[i] = "System Default";

										break;
									case 1024:

										LCIDs[i] = "User Default";
										break;
									case 0:

										LCIDs[i] = "Neutral";
										break;
									default:
										culture = new CultureInfo((int) supportedLCIDs[i]);
										LCIDs[i] = culture.Name;
										break;
								}
							}
							catch (ArgumentException exc)
							{
								Application.Instance.Trace(EnumTraceLevel.INF, EnumTraceGroup.CLIENT, "DaSession.GetStatus", exc.Message);
							}
						}

						serverStatus = new ServerStatus(
							state,
							startTime,
							currentTime,
							Marshal.PtrToStringUni(currentStatus.m_vendorInfo),
							Marshal.PtrToStringUni(currentStatus.m_productVersion),
							LCIDs,
							lastUpdateTime,
							currentStatus.m_groupCount,
							currentStatus.m_bandwidth,
							Marshal.PtrToStringUni(currentStatus.m_statusInfo));
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSession.GetStatus",
							"Getting status failed! Result: " + res);
					}

					OTBFunctions.OTFreeMemory(currentStatus.m_vendorInfo);

					OTBFunctions.OTFreeMemory(currentStatus.m_statusInfo);

					OTBFunctions.OTFreeMemory(currentStatus.m_productVersion);

					OTBFunctions.OTFreeMemory(currentStatus.m_supportedLCIDs);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.GetStatus",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Monitors the connection with the server.
		/// </summary>
		/// <param name="activate">Indicates if monitoring the connection is activated or not.</param>
		/// <param name="checkCycle"></param>
		/// <param name="connect1Attempts">The maximum number of attempts to reconnect server in the 1st phase.</param>
		/// <param name="connect1Cycle">The cycle(ticks) for the 1st phase.</param>
		/// <param name="connect2Cycle">The cycle(ticks) for the 2nd phase.</param>
		///<returns>The result of monitoring the connection with the server.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/method[@name="ActivateConnectionMonitor"]/doc/*'
		/// />
		public virtual int ActivateConnectionMonitor(
			bool activate,
			uint checkCycle,
			uint connect1Attempts,
			uint connect1Cycle,
			uint connect2Cycle)
		{
			int res = (int) EnumResultCode.E_FAIL;
			try
			{
				byte active = (byte) (activate ? 1 : 0);
				res = OTBFunctions.OTCActivateConnectionMonitor(
					this.Handle,
					active,
					checkCycle,
					connect1Attempts,
					connect1Cycle,
					connect2Cycle);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.ActivateConnectionMonitor",
						"Activating connection monitor failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.ActivateConnectionMonitor",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Gives a textual description of an error based on the error's code.
		/// </summary>
		/// <param name="errorCode">An error code.</param>
		/// <param name="errorText">A textual description for the error that corresponds to the error code.</param>
		///<param name="executionOptions">Specifies the modality of execution for getting the error description.</param>
		///<returns>The result of getting the error description.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/method[@name="GetErrorString"]/doc/*'
		/// />
		public virtual int GetErrorString(
			int errorCode,
			out string errorText,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			errorText = String.Empty;
			IntPtr errorString = IntPtr.Zero;
			try
			{
				OTCExecutionOptions options = new OTCExecutionOptions();

				if (executionOptions != null)
				{
					options.m_executionType = (byte) executionOptions.ExecutionType;
					options.m_executionContext = (uint) executionOptions.ExecutionContext;
				}
				else
				{
					options.m_executionType = (byte) EnumExecutionType.SYNCHRONOUS;
					options.m_executionContext = 0;
				}

				res = OTBFunctions.OTCGetErrorString(
					this.Handle,
					errorCode,
					out errorString,
					ref options);

				if (ResultCode.SUCCEEDED(res))
				{
					errorText = Marshal.PtrToStringUni(errorString);
				}
				else
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.GetErrorString",
						"Getting error string failed! Result: " + res);
				}
				OTBFunctions.OTFreeMemory(errorString);
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.GetErrorString",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Connects to a server using an user name and a password.
		/// </summary>
		/// <param name="userName">An user name.</param>
		/// <param name="password">A password.</param>
		///<param name="executionOptions">Specifies the modality of execution for logging on.</param>
		///<returns>The result of logging on.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/method[@name="Logon"]/doc/*'
		/// />
		public virtual int Logon(
			string userName,
			string password,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			try
			{
				OTCExecutionOptions options = new OTCExecutionOptions();

				if (executionOptions != null)
				{
					options.m_executionType = (byte) executionOptions.ExecutionType;
					options.m_executionContext = (uint) executionOptions.ExecutionContext;
				}
				else
				{
					options.m_executionType = (byte) EnumExecutionType.SYNCHRONOUS;
					options.m_executionContext = 0;
				}
				res = OTBFunctions.OTCLogon(
					this.Handle,
					userName,
					password,
					ref options);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.Logon",
						"Logging on failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.Logon",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Ends the current logon session.
		/// </summary>
		///<param name="executionOptions">Specifies the modality of execution for logging off.</param>
		///<returns>The result of logging off.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/method[@name="Logoff"]/doc/*'
		/// />
		public virtual int Logoff(ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			try
			{
				OTCExecutionOptions options = new OTCExecutionOptions();

				if (executionOptions != null)
				{
					options.m_executionType = (byte) executionOptions.ExecutionType;
					options.m_executionContext = (uint) executionOptions.ExecutionContext;
				}
				else
				{
					options.m_executionType = (byte) EnumExecutionType.SYNCHRONOUS;
					options.m_executionContext = 0;
				}

				res = OTBFunctions.OTCLogoff(
					this.Handle,
					ref options);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSession.Logoff",
						"Logging off failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.Logoff",
					exc.ToString());
			}
			return res;
		}

		//-
		#endregion

		#region //	Internal Methods
		//------------------------

		internal DaSubscription Find(int position)
		{
			return (DaSubscription) m_subscriptionList[position];
		}

		/// <summary>
		///  Called when items are asynchronously read from the server.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="itemIDs">A list with the identifiers for the read items.</param>
		/// <param name="itemPaths">A list with the access paths for the read items.</param>
		/// <param name="values">A list with the read values.</param>
		/// <param name="results">A list with the result of reading for each item.</param>
		/// <param name="result">The result of reading the items.</param>
		/// <remarks><note>
		///All the results should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnReadComplete(
			uint executionContext,
			string[] itemIDs,
			string[] itemPaths,
			ValueQT[] values,
			int[] results,
			int result)
		{
			try
			{
				if (ReadCompleted != null)
				{
					ReadCompleted(this, executionContext, itemIDs, itemPaths, values, results, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.OnReadComplete",
					exc.ToString());
			}
		}

		/// <summary>
		///  Called when items are asynchronously written to the server.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="itemIDs">A list with the identifiers for the written items.</param>
		/// <param name="itemPaths">A list with the access paths for the written items.</param>
		/// <param name="values">A list with the written values.</param>
		/// <param name="results">A list with the result of writing for each item.</param>
		/// <param name="result">The result of writing the items.</param>
		/// <remarks><note>
		///All the results should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnWriteComplete(
			uint executionContext,
			string[] itemIDs,
			string[] itemPaths,
			ValueQT[] values,
			int[] results,
			int result)
		{
			try
			{
				if (WriteCompleted != null)
				{
					WriteCompleted(this, executionContext, itemIDs, itemPaths, values, results, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.OnWriteComplete",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when the server shuts down.
		/// </summary>
		/// <param name="reason">The reason of the server shut down.</param>
		/// <remarks> If server shuts down while it is being used by an OPC Client the reconnection to the server will be performed only if the method returns <b>true</b>.</remarks>
		internal bool OnShutdown(string reason)
		{
			bool shutdown = false;
			try
			{
				if (ShutdownRequest != null)
				{
					shutdown = this.ShutdownRequest(this, reason);
				} //	end if
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.OnShutdown",
					exc.ToString());
			} //	end try...catch
			return shutdown;
		} //	end OnShutdown


		/// <summary>
		/// Called when the server status is gotten asynchronously.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="serverStatus">A <see cref="ServerStatus">ServerStatus</see> object which contain information on the server's parameters.</param>
		/// <param name="result">The result of getting the server status.</param>
		/// <remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal virtual void OnGetServerStatus(
			uint executionContext,
			ServerStatus serverStatus,
			int result)
		{
			try
			{
				if (GetStatusCompleted != null)
				{
					GetStatusCompleted(this, executionContext, serverStatus, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.OnGetServerStatus",
					exc.ToString());
			}
		}

		///<summary>
		///Called when the server's namespace is asynchronously browsed.
		///</summary>
		///<param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="addressSpaceElementHandle">A handle for the <see cref="AddressSpaceElement"/> whose children are to be fetched.</param>
		///<param name="addressSpaceElementID">An identifier in server's namespace for the element whose children are to be fetched.</param>
		///<param name="addressSpaceElementPath">An access path for the element whose children are to be fetched.</param>
		///<param name="addressSpaceElement">A list with the children elements.</param>
		///<param name="result">The result of browsing the server's namespace.</param>
		///<remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnBrowse(
			uint executionContext,
			uint addressSpaceElementHandle,
			string addressSpaceElementID,
			string addressSpaceElementPath,
			DaAddressSpaceElement[] addressSpaceElement,
			int result)
		{
			try
			{
				if (BrowseCompleted != null)
				{
					BrowseCompleted(
						this,
						executionContext,
						addressSpaceElementHandle,
						addressSpaceElementID,
						addressSpaceElementPath,
						addressSpaceElement,
						result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.OnBrowse",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when an <see cref="AddressSpaceElement">AddressSpaceElement</see> properties are asynchronously gotten.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="addressSpaceElementHandle">A handle for the <see cref="AddressSpaceElement"/> whose children are to be fetched.</param>
		/// <param name="addressSpaceElementID"> An identifier for the address space element whose properties are to be returned.</param>
		/// <param name="addressSpaceElementPath">An access path for the address space element whose properties are to be returned.</param>
		/// <param name="aDaProperty">A list with teh AddressSpaceElement's properties.</param>
		///<param name="result">The result of getting an AddressSpaceElement's properties.</param>
		///<remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnGetDaProperties(
			uint executionContext,
			uint addressSpaceElementHandle,
			string addressSpaceElementID,
			string addressSpaceElementPath,
			DaProperty[] aDaProperty,
			int result)
		{
			try
			{
				if (GetDaPropertiesCompleted != null)
				{
					GetDaPropertiesCompleted(
						this,
						executionContext,
						addressSpaceElementHandle,
						addressSpaceElementID,
						addressSpaceElementPath,
						aDaProperty,
						result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.OnGetDaProperties",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when the description of an error is asynchronously gotten.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="errorCode">An error code.</param>
		/// <param name="errorString">A textual description for the error that corresponds to the error code.</param>
		///<param name="result">The result of getting the description of an error.</param>
		///<remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnGetErrorString(
			uint executionContext,
			int errorCode,
			string errorString,
			int result)
		{
			try
			{
				if (GetErrorStringCompleted != null)
				{
					GetErrorStringCompleted(this, executionContext, errorCode, errorString, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.OnGetErrorString",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when the connection to the server using a user name and a password is done  asynchronously.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="userName">A user name.</param>
		/// <param name="password">A password.</param>
		///<param name="result">The result of logging on to the server.</param>
		///<remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnLogon(
			uint executionContext,
			string userName,
			string password,
			int result)
		{
			try
			{
				if (LogonCompleted != null)
				{
					LogonCompleted(this, executionContext, userName, password, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.OnLogon",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when the current logon session is ended  asynchronously.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		///<param name="result">The result of logging off from the server.</param>
		///<remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnLogoff(uint executionContext, int result)
		{
			try
			{
				if (LogoffCompleted != null)
				{
					LogoffCompleted(this, executionContext, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSession.OnLogoff",
					exc.ToString());
			}
		}

		//-
		#endregion

		#region //	ISerializable
		/// <summary>
		/// Contructs a session by de-serializing it from the stream.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/method[@name="DaSession"]/doc/*'
		/// />
		protected DaSession(SerializationInfo info, StreamingContext context)
		{
			lock (m_subscriptionList.SyncRoot) // de-serializing (modifying content)
			{
				m_subscriptionList = (Hashtable)info.GetValue("m_subscriptionList", typeof(Hashtable));
			}
		}

		/// <summary>
		/// Serializes a session into a stream.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSession"]/method[@name="GetObjectData"]/doc/*'
		/// />
		[SecurityPermission(SecurityAction.Demand, SerializationFormatter = true)]
		public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			lock (m_subscriptionList.SyncRoot) // serializing (enumerating)
			{
				info.AddValue("m_subscriptionList", m_subscriptionList);
			}
		}

		#endregion

		#region //Internal Properties
		//-------------------------

		internal object DaSubscriptionListSyncRoot
		{
			get { return m_subscriptionList.SyncRoot; }
		}

		//-
		#endregion
	} //	end class Session
} //	end namespace Softing.OPCToolbox.Client.Da