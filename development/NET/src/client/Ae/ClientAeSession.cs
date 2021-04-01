using System;
using System.Collections;
using System.Security.Permissions;
using System.Globalization;
using Softing.OPCToolbox.Client;
using System.Runtime.InteropServices;
using Softing.OPCToolbox.OTB;

//Alarms and Events Specification defines an interface between client and server programs for monitoring and acknowledging events and alarms.
//This specification describes objects and interfaces which are implemented by OPC Event Servers,
//and which provide the mechanisms for OPC Clients to be notified of the occurrence of specified events and alarm conditions.
//Within this specification, an alarm is an abnormal condition and is thus a special case of a condition.
//hand, an event is a detectable occurrence which is of significance to the OPC Event Server, the device it represents, and its OPC Clients.
//An event may or may not be associated with a condition.

namespace Softing.OPCToolbox.Client
{

	#region // Public Delegates
	//-----------------------------

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b> Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.AeSession.BrowseCompleted"/> event of an <see cref="AeSession"/>.</para>
	/// </summary>
	/// <param name="anAeSession">The <see cref="AeSession"/> object to whom the <see cref="Softing.OPCToolbox.Client.AeSession.BrowseCompleted"/> event belongs.</param>
	///<param name="executionContext">An identifier for an asynchronous call.</param>
	///<param name="addressSpaceElementHandle"> A handle for the <see cref="AddressSpaceElement"/> whose children are to be fetched.</param>
	///<param name="addressSpaceElements">A list with the children elements.</param>
	///<param name="result">The result of browsing the server's eventarea space.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="BrowseAeServerEventHandler"]/doc/*'
	/// />
	public delegate void BrowseAeServerEventHandler(
		AeSession anAeSession,
		uint executionContext,
		uint addressSpaceElementHandle,
		AddressSpaceElement[] addressSpaceElements,
		int result);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.AeSession.QueryAeCategoriesCompleted"/> event of an <see cref="AeSession"/>.</para>
	/// </summary>
	/// <param name="anAeSession">The <see cref="AeSession"/> object to whom the <see cref="Softing.OPCToolbox.Client.AeSession.QueryAeCategoriesCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="categories">A list with the event categories.</param>
	/// <param name="result">The result of quering the event categories.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="QueryAeCategoriesEventHandler"]/doc/*'
	/// />
	public delegate void QueryAeCategoriesEventHandler(
		AeSession anAeSession,
		uint executionContext,
		AeCategory[] categories,
		int result);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.AeSession.QueryAvailableAeFiltersCompleted"/> event of an <see cref="AeSession"/>.</para>
	/// </summary>
	/// <param name="anAeSession">The <see cref="AeSession"/> object to whom the <see cref="Softing.OPCToolbox.Client.AeSession.QueryAvailableAeFiltersCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="availableFilters">An <see cref="EnumFilterBy"/> object that represents the available filters.</param>
	/// <param name="result">The result of quering for the available filters.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="QueryAvailableAeFiltersEventHandler"]/doc/*'
	/// />
	public delegate void QueryAvailableAeFiltersEventHandler(
		AeSession anAeSession,
		uint executionContext,
		EnumFilterBy availableFilters,
		int result);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.AeSession.EnableAeConditionsByAreaCompleted"/> event of an <see cref="AeSession"/>.</para>
	/// </summary>
	/// <param name="anAeSession">The <see cref="AeSession"/> object to whom the <see cref="Softing.OPCToolbox.Client.AeSession.EnableAeConditionsByAreaCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="enable">Specifies if condition evaluation is enabled or disabled. <br></br><i>true</i> means enabled; <i>false</i> means disabled.</param>
	/// <param name="area">The area for which the condition evaluation is enabled.</param>
	/// <param name="result">The result of enabling the condition evaluation.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="EnableAeConditionsByAreaEventHandler"]/doc/*'
	/// />
	public delegate void EnableAeConditionsByAreaEventHandler(
		AeSession anAeSession,
		uint executionContext,
		bool enable,
		string area,
		int result);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.AeSession.EnableAeConditionsBySourceCompleted"/> event of an <see cref="AeSession"/>.</para>
	/// </summary>
	/// <param name="anAeSession">The <see cref="AeSession"/> object to whom the <see cref="Softing.OPCToolbox.Client.AeSession.EnableAeConditionsBySourceCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="enable">Specifies if condition evaluation is enabled or disabled. <br></br><i>true</i> means enabled; <i>false</i> means disabled.</param>
	/// <param name="source">The source for which the condition evaluation is enabled.</param>
	/// <param name="result">The result of enabling the condition evaluation.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="EnableAeConditionsBySourceEventHandler"]/doc/*'
	/// />
	public delegate void EnableAeConditionsBySourceEventHandler(
		AeSession anAeSession,
		uint executionContext,
		bool enable,
		string source,
		int result);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.AeSession.AcknowledgeAeConditionsCompleted"/> event of an <see cref="AeSession"/>.</para>
	/// </summary>
	/// <param name="anAeSession">The <see cref="AeSession"/> object to whom the <see cref="Softing.OPCToolbox.Client.AeSession.AcknowledgeAeConditionsCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="ackID">An identifier for the origin of the acknowledgement. </param>
	/// <param name="ackComment">A note that accompanies the acknowledgement.</param>
	/// <param name="conditions">A list with the acknowledged condition-related events.</param>
	/// <param name="results">A list with the result of acknowledging for each condition-related event.</param>
	/// <param name="result">The result of acknowledging the condition-related events.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="AcknowledgeAeConditionsEventHandler"]/doc/*'
	/// />
	public delegate void AcknowledgeAeConditionsEventHandler(
		AeSession anAeSession,
		uint executionContext,
		string ackID,
		string ackComment,
		AeCondition[] conditions,
		int[] results,
		int result);

	/// <summary>
	///<para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	///<para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.AeSession.QueryAeSourceConditionsCompleted"/> event of an <see cref="AeSession"/>.</para>
	/// </summary>
	/// <param name="anAeSession">The <see cref="AeSession"/> object to whom the <see cref="Softing.OPCToolbox.Client.AeSession.QueryAeSourceConditionsCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="sourcePath">An identifier in the server's eventarea space for the source.</param>
	/// <param name="conditionsNames">A list with the conditions' names.</param>
	/// <param name="result">The result of quering the conditons' names.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="QueryAeSourceConditonsEventHandler"]/doc/*'
	/// />
	public delegate void QueryAeSourceConditonsEventHandler(
		AeSession anAeSession,
		uint executionContext,
		string sourcePath,
		string[] conditionsNames,
		int result);

	/// <summary>
	///<para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	///<para><b>C#, Visual Basic, C++</b>Represents a method that will handle the <see cref="Softing.OPCToolbox.Client.AeSession.GetAeConditionStateCompleted"/> event of an <see cref="AeSession"/>.</para>
	/// </summary>
	/// <param name="anAeSession">The <see cref="AeSession"/> object to whom the <see cref="Softing.OPCToolbox.Client.AeSession.GetAeConditionStateCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="sourcePath">An identifier in the server's eventarea space for the source.</param>
	/// <param name="conditionName">A name for the condition whose state is to be given.</param>
	/// <param name="attributesIds">A list with the attributes identifiers related to the condition.</param>
	/// <param name="conditionState">An <see cref="AeConditionState"/> object that contains all the information on the condition's  state.</param>
	/// <param name="result">The result of getting the conditon's state.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="GetAeConditionStateEventHandler"]/doc/*'
	/// />
	public delegate void GetAeConditionStateEventHandler(
		AeSession anAeSession,
		uint executionContext,
		string sourcePath,
		string conditionName,
		uint[] attributesIds,
		AeConditionState conditionState,
		int result);

	//-

	#endregion

	/// <summary>
	/// <para>Represents a client - OPC Alarms and Events Server session.</para>
	/// <para>The Alarms and Events Server can record and evaluate values from different data sources and decide whether and event has occured.</para>
	/// <para>Unlike the Data Access Server it sends to the client information that something has happened, not values.</para>
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="AeSession"]/doc/*'
	/// />
	public class AeSession : Softing.OPCToolbox.Client.ObjectSpaceElement
	{
		#region //Public Constructors
		//-----------------------------

		/// <summary>
		/// Initializes a new instance of the <see cref="AeSession"/> class with
		/// the value indicated by a session url.
		/// After creating it, the <b>AeSession</b> object is added to the application.
		/// </summary>
		/// <param name="url">The session url.</param>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/constructor[@name="AeSession"]/doc/*'
		/// />
		public AeSession(string url)
		{
			m_subscriptionList = new Hashtable();

			try
			{
				int res = (int) EnumResultCode.E_FAIL;

				res = Application.Instance.AddAeSession(url, this);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSession.Constructor",
						"The session could not be added to the application! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.Constructor",
					exc.ToString());
			}
		} //	end ctr


		//-
		#endregion

		#region //Private Attributes
		//-------------------------

		private readonly OTCObjectAttributes m_objectAttributes = new OTCObjectAttributes();
		private readonly object m_attributesJanitor = new object();

		//-
		#endregion

		#region //Protected Attributes
		//---------------------------

		/// <summary>
		/// A list with all the subscriptions (<see cref="Softing.OPCToolbox.Client.AeSubscription"/> objects) that are currently added to the session.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/field[@name="m_subscriptionList"]/doc/*'
		/// />
		protected Hashtable m_subscriptionList = new Hashtable();

		//-
		#endregion

		#region //Internal Attributes
		//---------------------------

		//the category list is filled up with information when the QueryAECategories method is called
		internal Hashtable m_categoryList = new Hashtable();

		//-
		#endregion

		#region //Public Events
		//-----------------------

		/// <summary>
		/// Occurs when the Alarms and Events Server shuts down.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/event[@name="ShutdownRequest"]/doc/*'
		/// />
		public event ShutdownEventHandler ShutdownRequest;

		/// <summary>
		/// Occurs when getting the server status completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="GetStatus"/> method.</para>
		/// <para><b>Unmanaged C++</b>The method is called after the method <see cref="GetStatus"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/event[@name="GetServerStatusCompleted"]/doc/*'
		/// />
		public event GetStatusEventHandler GetServerStatusCompleted;

		/// <summary>
		/// Occurs when browsing the server namespace completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="Browse"/> method.</para>
		/// <para><b>Unmanaged C++</b>The method is called after the method <see cref="Browse"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/event[@name="BrowseCompleted"]/doc/*'
		/// />
		public event BrowseAeServerEventHandler BrowseCompleted;

		/// <summary>
		/// Occurs when quering the categories of the events supported by the server completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="QueryAeCategories"/> method.</para>
		/// <para><b>Unmanaged C++</b>The method is called after the method <see cref="QueryAeCategories"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/event[@name="QueryAeCategoriesCompleted"]/doc/*'
		/// />
		public event QueryAeCategoriesEventHandler QueryAeCategoriesCompleted;

		/// <summary>
		/// Occurs when quering for the available filter criteria  completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="QueryAvailableAeFilters"/> method.</para>
		/// <para><b>Unmanaged C++</b>The method is called after the method <see cref="QueryAvailableAeFilters"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/event[@name="QueryAvailableAeFiltersCompleted"]/doc/*'
		/// />
		public event QueryAvailableAeFiltersEventHandler QueryAvailableAeFiltersCompleted;

		/// <summary>
		/// Occurs when enabling or disabling the monitoring of condition-related events for an entire area completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="EnableAeConditionsByArea"/> method.</para>
		/// <para><b>Unmanaged C++</b>The method is called after the method <see cref="EnableAeConditionsByArea"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/event[@name="EnableAeConditionsByAreaCompleted"]/doc/*'
		/// />
		public event EnableAeConditionsByAreaEventHandler EnableAeConditionsByAreaCompleted;

		/// <summary>
		/// Occurs when enabling or disabling the monitoring of condition-related events for a source of the eventarea completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="EnableAeConditionsBySource"/> method.</para>
		/// <para><b>Unmanaged C++</b>The method is called after the method <see cref="EnableAeConditionsBySource"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/event[@name="EnableAeConditionsBySourceCompleted"]/doc/*'
		/// />
		public event EnableAeConditionsBySourceEventHandler EnableAeConditionsBySourceCompleted;

		/// <summary>
		/// Occurs when acknowledging the condition-related events completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="AcknowledgeAeConditions"/> method.</para>
		/// <para><b>Unmanaged C++</b>The method is called after the method <see cref="AcknowledgeAeConditions"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/event[@name="AcknowledgeAeConditionsCompleted"]/doc/*'
		/// />
		public event AcknowledgeAeConditionsEventHandler AcknowledgeAeConditionsCompleted;

		/// <summary>
		/// Occurs when quering the condition-related events allocated to a source completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="QueryAeSourceConditions"/> method.</para>
		/// <para><b>Unmanaged C++</b>The method is called after the method <see cref="QueryAeSourceConditions"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/event[@name="QueryAeSourceConditionsCompleted"]/doc/*'
		/// />
		public event QueryAeSourceConditonsEventHandler QueryAeSourceConditionsCompleted;

		///<summary>
		/// Occurs when getting a condition state completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="GetAeConditionState"/> method.</para>
		/// <para><b>Unmanaged C++</b>The method is called after the method <see cref="GetAeConditionState"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/event[@name="GetAeConditionStateCompleted"]/doc/*'
		/// />
		public event GetAeConditionStateEventHandler GetAeConditionStateCompleted;

		/// <summary>
		/// Occurs when getting textual description of an error completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="GetErrorString"/> method.</para>
		/// <para><b>Unmanaged C++</b>The method is called after the method <see cref="GetErrorString"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/event[@name="GetErrorStringCompleted"]/doc/*'
		/// />
		public event GetErrorStringEventHandler GetErrorStringCompleted;

		/// <summary>
		/// Occurs when the logon to the server completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="Logon"/> method.</para>
		/// <para><b>Unmanaged C++</b>The method is called after the method <see cref="Logon"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/event[@name="LogonCompleted"]/doc/*'
		/// />
		public event LogonEventHandler LogonCompleted;

		/// <summary>
		/// Occurs when the logoff from the server completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="Logoff"/> method.</para>
		/// <para><b>Unmanaged C++</b>The method is called after the method <see cref="Logoff"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/event[@name="LogoffCompleted"]/doc/*'
		/// />
		public event LogoffEventHandler LogoffCompleted;

		//-
		#endregion

		#region //Public Properties
		//---------------------------

		/// <summary>
		/// Gets the list with the subscriptions belonging to the session.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/property[@name="SubscriptionList"]/doc/*'
		/// />
		public AeSubscription[] SubscriptionList
		{
			get
			{
				lock (m_subscriptionList.SyncRoot) // enumerating
				{
					AeSubscription[] subscriptionVector = new AeSubscription[m_subscriptionList.Count];
					int i = 0;

					foreach (AeSubscription aeSubscription in m_subscriptionList.Values)
					{
						subscriptionVector[i] = aeSubscription;
						i++;
					} //	end foreach

					return subscriptionVector;
				}
			}
		} //	end SubscriptionList


		/// <summary>
		/// Gets the server's url.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/property[@name="Url"]/doc/*'
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
									new uint[] {(uint) EnumObjectAttribute.AESESSION_URL},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSession.Url.Get",
										"Update attribute AESESSION_URL from server failed!Result: " + updateResult);
								} //	end if
							} //	end if
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESESSION_URL,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								url = Marshal.PtrToStringUni(objectAttributes.m_aeSessionURL);
								OTBFunctions.OTFreeMemory(objectAttributes.m_aeSessionURL);
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSession.Url.Get",
									"Get attribute AESESSION_URL from server failed!Result: " + getAttrsResult);
							}
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSession.Url.Get",
						exc.ToString());
				} //	end try...catch
				return url;
			} //	end get
		} //	end URL

		/// <summary>
		/// Gets or sets the valid language identifier for the current server.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/property[@name="LocaleId"]/doc/*'
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
									new uint[] {(uint) EnumObjectAttribute.AESESSION_LCID},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSession.LocaleId.Get",
										"Update attribute AESESSION_LCID from server failed!Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESESSION_LCID, objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								lcid = objectAttributes.m_aeSessionLCID;
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
									"AeSession.LocaleId.Get",
									"Get attribute AESESSION_LCID from server failed!Result: " + getAttrsResult);
							}
						} //	end lock
					} //	end if
				}
				catch (ArgumentException exc)
				{
					Application.Instance.Trace(EnumTraceLevel.INF,
					                           EnumTraceGroup.CLIENT,
					                           "AE.Session.LocaleID.Get",
					                           exc.Message);
					cultureName = lcid.ToString();
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AE.Session.LocaleID.Get",
						exc.ToString());
				}
				return cultureName;
			}
			set
			{
				try
				{
					uint lcid;

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
							m_objectAttributes.m_aeSessionLCID = lcid;

							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESESSION_LCID,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSession.LocaleId.Set",
									"Set attribute AESESSION_LCID to server failed!Result: " + setAttrsResult);
							}
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.AESESSION_LCID},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSession.LocaleId.Set",
										"Set attribute AESESSION_LCID to server failed!Result: " + updateResult);
								}
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSession.LocaleId.Set",
							"Set LocaleId failed! Session is not added to the application ");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AE.Session.LocaleID.Set",
						exc.ToString());
				}
			} //	end set
		} //	end LocaleID


		/// <summary>
		/// Gets or sets information about a client.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/property[@name="ClientName"]/doc/*'
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
								int getAttrsResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.AESESSION_CLIENTNAME},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(getAttrsResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSession.ClientName.Get",
										"Get attribute AESESSION_CLIENTNAME from server failed!Result: " + getAttrsResult);
								}
							}
							int updateResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESESSION_CLIENTNAME,
								objectAttributes);
							if (ResultCode.SUCCEEDED(updateResult))
							{
								clientName = Marshal.PtrToStringUni(objectAttributes.m_aeSessionClientName);
								OTBFunctions.OTFreeMemory(objectAttributes.m_aeSessionClientName);
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSession.ClientName.Get",
									"Update attribute AESESSION_CLIENTNAME from server failed!Result: " + updateResult);
							}
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AE.Session.ClientName.Get",
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
								m_objectAttributes.m_aeSessionClientName = Marshal.StringToCoTaskMemUni(value);
								int setAttrsResult = OTBFunctions.OTCSetAttributes(
									this.Handle,
									(uint) EnumObjectAttribute.AESESSION_CLIENTNAME,
									m_objectAttributes);
								if (ResultCode.FAILED(setAttrsResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSession.ClientName.Set",
										"Set attribute AESESSION_CLIENTNAME to server failed!Result: " + setAttrsResult);
								}
								if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
								{
									int updateResult = OTBFunctions.OTCUpdateAttributes(
										this.Handle,
										0,
										1,
										new uint[] {(uint) EnumObjectAttribute.AESESSION_CLIENTNAME},
										new int[] {result},
										ref options);
									if (ResultCode.FAILED(updateResult))
									{
										Application.Instance.Trace(
											EnumTraceLevel.ERR,
											EnumTraceGroup.CLIENT,
											"AeSession.ClientName.Set",
											"Update attribute AESESSION_CLIENTNAME to server failed!Result: " + updateResult);
									} //	end if
								} //	end if
								Marshal.FreeCoTaskMem(m_objectAttributes.m_aeSessionClientName);
							} //	end lock
						} //	end if
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSession.ClientName.Set",
							"Set ClientName failed! Session is not added to the application");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AE.Session.ClientName.Set",
						exc.ToString());
				}
			} //	end set
		} //	end ClientName


		/// <summary>
		/// Gets or sets the execution context in which the Alarms and Events Server is to be run.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/property[@name="ClassContext"]/doc/*'
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
									new uint[] {(uint) EnumObjectAttribute.AESESSION_CLASSCONTEXT},
									new int[] {result},
									ref options);

								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSession.ClassContext.Get",
										"Update attribute AESESSION_CLASSCONTEXT failed! Result: " + updateResult);
								}
							} //	end if
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESESSION_CLASSCONTEXT,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								classContext = objectAttributes.m_aeSessionComClsctx;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSession.ClassContext.Get",
									"Get attribute AESESSION_CLASSCONTEXT failed! Error message: " + getAttrsResult);
							} //	end else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSession.ClassContext.Get",
						exc.ToString());
				}
				return (EnumClassContext) classContext;
			}
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
							m_objectAttributes.m_aeSessionComClsctx = (uint) value;
							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESESSION_CLASSCONTEXT,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSession.ClassContext.Set",
									"Set attribute AESESSION_CLASSCONTEXT failed! Result: " + setAttrsResult);
							} //	end if
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.AESESSION_CLASSCONTEXT},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSession.ClassContext.Set",
										"Set attribute AESESSION_CLASSCONTEXT failed! Result: " + updateResult);
								} //	end if
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSession.ClassContext.Set",
							"Set ClassContext failed! Session is not added to the application ");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSession.ClassContext.Set",
						exc.ToString());
				}
			} //	end set
		}

		#endregion

		#region //Public Methods
		//------------------------

		/// <summary>
		/// Adds an <see cref="AeSubscription"/> object to the session.
		/// </summary>
		/// <param name="aSubscription">The <see cref="AeSubscription"/> object to be added to the server.</param>
		/// <returns>The result of adding the <see cref="AeSubscription"/> object.</returns>
		internal virtual int AddAeSubscription(AeSubscription aSubscription)
		{
			int res = (int) EnumResultCode.E_FAIL;
			try
			{
				if (aSubscription != null)
				{
					uint groupHandle = 0;

					res = OTBFunctions.OTCAddAESubscription(
						this.Handle,
						(uint)aSubscription.GetHashCode(),
						ref groupHandle);
					if (ResultCode.SUCCEEDED(res))
					{
						aSubscription.AeSession = this;
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
							"AeSession.AddAeSubscription",
							" Adding subscription to session failed! Result: " + res);
					}
				}
				else
				{
					res = (int) EnumResultCode.E_INVALIDARG;
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSession.AddAeSubscription",
						" The argument is invalid");
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.AddAeSubscription",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Removes an <see cref="AeSubscription"/> object from the server.
		/// </summary>
		/// <param name="aSubscription">The <see cref="AeSubscription"/> object to be removed from the server.</param>
		/// <returns>The result of removing the <see cref="AeSubscription"/> object.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/method[@name="RemoveAeSubscription"]/doc/*'
		/// />
		public virtual int RemoveAeSubscription(AeSubscription aSubscription)
		{
			int res = (int) EnumResultCode.E_FAIL;
			try
			{
				if (aSubscription != null)
				{
					res = OTBFunctions.OTCRemoveAESubscription(aSubscription.Handle);
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
							"AeSession.RemoveAeSubscription",
							" Removing subscription from session failed! Result: " + res);
					}
				}
				else
				{
					res = (int) EnumResultCode.E_INVALIDARG;
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSession.RemoveAeSubscription",
						"The argument is invalid");
				}
			}
			catch (Exception exc)
			{
				res = (int) EnumResultCode.E_FAIL;
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.RemoveAeSubscription",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Gives information on the server's parameters.
		/// </summary>
		/// <param name="serverStatus">A <see cref="ServerStatus"/> object that contains information on the server's parameters.</param>
		///<param name="executionOptions">Specifies the modality of execution for getting the server status.</param>
		///<returns>The result of getting the server status.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/method[@name="GetStatus"]/doc/*'
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

				res = OTBFunctions.OTCGetServerStatus(this.Handle, currentStatus, ref options);

				if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
				{
					if (ResultCode.SUCCEEDED(res))
					{
						string vendorInfo = Marshal.PtrToStringUni(currentStatus.m_vendorInfo);
						string statusInfo = Marshal.PtrToStringUni(currentStatus.m_statusInfo);
						string productVersion = Marshal.PtrToStringUni(currentStatus.m_productVersion);
						uint state = currentStatus.m_state;
						uint bandwidth = currentStatus.m_bandwidth;
						uint groupCount = currentStatus.m_groupCount;

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
								Application.Instance.Trace(EnumTraceLevel.INF, EnumTraceGroup.CLIENT, "AE.Session.GetStatus", exc.Message);
							}
						}

						serverStatus = new ServerStatus(state, startTime, currentTime, vendorInfo, productVersion, LCIDs, lastUpdateTime,
						                                groupCount, bandwidth, statusInfo);
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSession.GetStatus",
							"Getting session status failed! Result: " + res);
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
					"AeSession.GetStatus",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// <para>Browses the server's eventarea space at the specified branch.</para>
		/// </summary>
		/// <param name="addressSpaceElement">The <see cref="AddressSpaceElement"/> whose children are to be fetched.</param>
		/// <param name="browseOptions">the <see cref="AddressSpaceElementBrowseOptions"/> object that defines the browsing options.</param>
		/// <param name="addressSpaceElements">A list with the children elements.</param>
		/// <param name="executionOptions">Specifies the modality of execution for browsing the eventarea.</param>
		///<returns>The result of browsig the server's eventarea.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/method[@name="Browse"]/doc/*'
		/// />
		public virtual int Browse(
			AddressSpaceElement addressSpaceElement,
			AddressSpaceElementBrowseOptions browseOptions,
			out AddressSpaceElement[] addressSpaceElements,
			ExecutionOptions executionOptions)
		{
			addressSpaceElements = null;
			OTCAddressSpaceBrowseOptions otcBrowseOptions = new OTCAddressSpaceBrowseOptions();

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

				otcBrowseOptions.m_elementNameFilter = Marshal.StringToCoTaskMemUni(browseOptions.ElementNameFilter);
				otcBrowseOptions.m_type = (byte) browseOptions.ElementTypeFilter;
				otcBrowseOptions.m_forceBrowseUp = (byte) (browseOptions.ForceBrowseUp ? 1 : 0);

				IntPtr pOTCaddressSpaceElements = new IntPtr();
				uint addressSpaceElementDataCount = 0;

				if (addressSpaceElement != null)
				{
					res = OTBFunctions.OTCBrowseAddressSpace(
						this.Handle,
						addressSpaceElement.Handle,
						null,
						null,
						ref otcBrowseOptions,
						ref addressSpaceElementDataCount,
						out pOTCaddressSpaceElements,
						ref options);
				}
				else
				{
					res = OTBFunctions.OTCBrowseAddressSpace(
						this.Handle,
						0,
						null,
						null,
						ref otcBrowseOptions,
						ref addressSpaceElementDataCount,
						out pOTCaddressSpaceElements,
						ref options);
				}

				addressSpaceElements = new AddressSpaceElement[addressSpaceElementDataCount];

				if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
				{
					if (ResultCode.SUCCEEDED(res))
					{
						OTCAddressSpaceElementData typeOfAddressSpaceElement = new OTCAddressSpaceElementData();
						for (int i = 0; i < addressSpaceElementDataCount; i++)
						{
							int structSize = Marshal.SizeOf(typeOfAddressSpaceElement);
							OTCAddressSpaceElementData myData =
								(OTCAddressSpaceElementData)
								Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(pOTCaddressSpaceElements, structSize*i),
								                       typeof (OTCAddressSpaceElementData));
							addressSpaceElements[i] = new AddressSpaceElement(
								(EnumAddressSpaceElementType) myData.m_type,
								Marshal.PtrToStringUni(myData.m_name),
								Marshal.PtrToStringUni(myData.m_itemID),
								myData.m_objectHandle);

							OTBFunctions.OTFreeMemory(myData.m_name);
						}

						if (pOTCaddressSpaceElements != IntPtr.Zero)
						{
							OTBFunctions.OTFreeMemory(pOTCaddressSpaceElements);
						}
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSession.Browse",
							"Browsing Ae server's address space failed! Result: " + res);
					}
				} //	if synchronous
				if (otcBrowseOptions.m_elementNameFilter != IntPtr.Zero)
				{
					Marshal.FreeCoTaskMem(otcBrowseOptions.m_elementNameFilter);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.Browse",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Returns the specific categories of events supported by the server.
		/// </summary>
		/// <param name="categories">A list with the events categories.</param>
		/// <param name="executionOptions">Specifies the modality of execution for quering the categories of events.</param>
		///<returns>The result of quering the categories of events.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/method[@name="QueryAeCategories"]/doc/*'
		/// />
		public virtual int QueryAeCategories(
			out AeCategory[] categories,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			categories = new AeCategory[0];
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

				uint count = 0;
				IntPtr inEventTypes = IntPtr.Zero;
				IntPtr inCategoryIDs = IntPtr.Zero;
				IntPtr inCategoryDescriptions = IntPtr.Zero;

				res = OTBFunctions.OTCQueryAECategories(
					this.Handle,
					out count,
					out inEventTypes,
					out inCategoryIDs,
					out inCategoryDescriptions,
					ref options);

				if (ResultCode.SUCCEEDED(res))
				{
					if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
					{
						lock (m_categoryList.SyncRoot) // enumerating
						{
							categories = new AeCategory[count];
							int uintSize = Marshal.SizeOf(typeof (uint));
							int ptrSize = Marshal.SizeOf(typeof (IntPtr));

							for (int i = 0; i < count; i++)
							{
								uint eventType =
									(uint) Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(inEventTypes, i*uintSize), typeof (uint));
								uint categoryId =
									(uint) Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(inCategoryIDs, i*uintSize), typeof (uint));
								IntPtr categoryDescription =
									(IntPtr)
									Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(inCategoryDescriptions, i*ptrSize), typeof (IntPtr));
								categories[i] = new AeCategory(
									(EnumEventType) eventType,
									categoryId,
									Marshal.PtrToStringUni(categoryDescription),
									this);

								bool exists = false;
								foreach (AeCategory category in m_categoryList.Values)
								{
									if (categories[i].Id == category.Id)
										exists = true;
								}
								if (!exists)
								{
									m_categoryList.Add(categories[i].GetHashCode(), categories[i]);
								}

								OTBFunctions.OTFreeMemory(categoryDescription);
							}
						}
						OTBFunctions.OTFreeMemory(inEventTypes);
						OTBFunctions.OTFreeMemory(inCategoryIDs);
						OTBFunctions.OTFreeMemory(inCategoryDescriptions);
					} //	end if
				} //	end if
				else
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSession.QueryAeCategories",
						"Quering Ae categories failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.QueryAeCategories",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Queries for the filter criteria available in the server's filter space. The filter space is used for the logical structuring of the events.
		/// A server can support filtering based on the following values:
		/// <br></br>- values for the event type(condition related,simple,tracking related)
		/// <br></br>- values for the event category
		/// <br></br>- values for the event severity
		/// <br></br>- values for the area (nodes in the eventarea)
		/// <br></br>- values for the source (leaves in the eventarea)
		/// </summary>
		/// <param name="availableFilters">An <see cref="EnumFilterBy"/> object that represents the available filters.</param>
		/// <param name="executionOptions">Specifies the modality of execution for quering for available filters.</param>
		/// <returns>The result of quering for the available filters.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/method[@name="QueryAvailableAeFilters"]/doc/*'
		/// />
		public virtual int QueryAvailableAeFilters(
			out EnumFilterBy availableFilters,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			availableFilters = 0;
			uint OtcAvailableFilters = 0;
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

				res = OTBFunctions.OTCQueryAvailableAEFilters(
					this.Handle,
					out OtcAvailableFilters,
					ref options);

				if (ResultCode.SUCCEEDED(res))
				{
					if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
					{
						availableFilters = (EnumFilterBy) OtcAvailableFilters;
					} //	end if
				}
				else
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSession.QueryAvailableAeFilters",
						" Quering Ae available filters failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.QueryAvailableAeFilters",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Enables or disables the condition evaluation for an entire area.
		/// </summary>
		/// <param name="areas">A list with the areas for which the conditions are to be enabled or disabled.</param>
		/// <param name="enable">Specifies if condition evaluation is enabled or disabled. <br></br><i>true</i> means enabled; <i>false</i> means disabled.</param>
		/// <param name="executionOptions">Specifies the modality of execution for enabling condition evaluation.</param>
		/// <returns>The result of enabling the condition evaluation.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/method[@name="EnableAeConditionsByArea"]/doc/*'
		/// />
		public virtual int EnableAeConditionsByArea(
			string[] areas,
			bool enable,
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

				byte enabled = (byte) ((enable == true) ? 1 : 0);

				for (int i = 0; i < areas.Length; i++)
				{
					res = OTBFunctions.OTCEnableAEConditions(
						this.Handle,
						enabled,
						1, //for true the area is taking into account
						areas[i],
						ref options);

					if (ResultCode.FAILED(res))
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSession.EnableAeConditionsByArea",
							"Enabling Ae Conditions by area failed! Result: " + res);
					}
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.EnableAeConditionsByArea",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Enables or disables the condition evaluation for a source.
		/// </summary>
		/// <param name="sources">A list with the sources for which the conditions are to be enabled or disabled.</param>
		/// <param name="enable">Specifies if condition evaluation is enabled or disabled. <br></br><i>true</i> means enabled; <i>false</i> means disabled.</param>
		/// <param name="executionOptions">Specifies the modality of execution for enabling condition evaluation.</param>
		/// <returns>The result of enabling the condition evaluation.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/method[@name="EnableAeConditionsBySource"]/doc/*'
		/// />
		public virtual int EnableAeConditionsBySource(
			string[] sources,
			bool enable,
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

				byte enabled = (byte) ((enable == true) ? 1 : 0);

				for (int i = 0; i < sources.Length; i++)
				{
					res = OTBFunctions.OTCEnableAEConditions(
						this.Handle,
						enabled,
						0, //for false the source is taking into account
						sources[i],
						ref options);

					if (ResultCode.FAILED(res))
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSession.EnableAeConditionsBySource",
							"Enabling Ae Conditions by source failed! Result: " + res);
					}
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.EnableAeConditionsBySource",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Acknowledges condition-related events that have occured.
		/// </summary>
		/// <param name="ackId">An identifier for the origin of the acknowledgement. </param>
		/// <param name="ackComment">A note that accompanies the acknowledgement.</param>
		/// <param name="conditions">A list with the condition-related events to be acknowledged.</param>
		/// <param name="results">A list with the result of acknowledging for each condition-related event.</param>
		/// <param name="executionOptions">Specifies the modality of execution for acknowledging events.</param>
		/// <returns>The result of acknowledging events.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/method[@name="AcknowledgeAeConditions"]/doc/*'
		/// />
		public virtual int AcknowledgeAeConditions(
			string ackId,
			string ackComment,
			AeCondition[] conditions,
			out int[] results,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			results = new int[0];

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

				if (conditions != null)
				{
					OTCEventData[] newConditions = new OTCEventData[conditions.Length];
					for (int i = 0; i < conditions.Length; i++)
					{
						newConditions[i].m_sourcePath = Marshal.StringToCoTaskMemUni(conditions[i].SourcePath);
						newConditions[i].m_conditionName = Marshal.StringToCoTaskMemUni(conditions[i].ConditionName);
						newConditions[i].m_activeTime = new OTDateTime(conditions[i].ActiveTime);
						newConditions[i].m_cookie = conditions[i].Cookie;
					}
					results = new int[conditions.Length];
					res = OTBFunctions.OTCAcknowledgeAEConditions(
						this.Handle,
						ackId,
						ackComment,
						(uint) conditions.Length,
						newConditions,
						results,
						ref options);

					if (ResultCode.FAILED(res))
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSession.AcknowledgeAeConditions",
							" Acknowledging Ae conditions failed! Result: " + res);
					}

					for (int i = 0; i < conditions.Length; i++)
					{
						Marshal.FreeCoTaskMem(newConditions[i].m_sourcePath);
						Marshal.FreeCoTaskMem(newConditions[i].m_conditionName);
					} //	end for
				} //	end if
			} //	try
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.AcknowledgeAeConditions",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Gives the specific condition names associated with a specified source.
		/// </summary>
		/// <param name="sourceName">A name for the source.</param>
		/// <param name="conditionNames">A list with the condition names for the specified source.</param>
		/// <param name="executionOptions">Specifies the modality of execution for quering the condition names.</param>
		/// <returns>The result of quering the condition names.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/method[@name="QueryAeSourceConditions"]/doc/*'
		/// />
		public virtual int QueryAeSourceConditions(
			string sourceName,
			out string[] conditionNames,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;

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

			conditionNames = new string[0];
			uint count = 0;
			IntPtr pConditionNames = IntPtr.Zero;

			try
			{
				res = OTBFunctions.OTCQueryAESourceConditions(
					this.Handle,
					sourceName,
					out count,
					out pConditionNames,
					ref options);

				if (ResultCode.SUCCEEDED(res))
				{
					if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
					{
						conditionNames = new string[count];
						if ((pConditionNames != IntPtr.Zero) && (executionOptions.ExecutionType == EnumExecutionType.SYNCHRONOUS))
						{
							int ptrSize = Marshal.SizeOf(typeof (IntPtr));
							for (int i = 0; i < count; i++)
							{
								IntPtr conditionName =
									(IntPtr) Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(pConditionNames, i*ptrSize), typeof (IntPtr));
								conditionNames[i] = Marshal.PtrToStringUni(conditionName);
								OTBFunctions.OTFreeMemory(conditionName);
							}
							OTBFunctions.OTFreeMemory(pConditionNames);
						}
					}
				}
				else
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSession.QueryAeSourceConditions",
						"Quering Ae Source conditions failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.QueryAeSourceConditions",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		///  Gives specific information on conditions available in a source of the eventarea. This method could be invoked by a client
		///  when starting a server in order to get information on the state of the varios conditions.
		/// </summary>
		/// <param name="sourcePath">An identifier in the server's eventarea space for the source.</param>
		/// <param name="conditionName">A name for the condition whose state is to be given.</param>
		/// <param name="attributes">A list with the attributes related to the condition.</param>
		/// <param name="conditionState">An <see cref="AeConditionState"/> object that contains all the information on the condition's  state.</param>
		/// <param name="executionOptions">Specifies the modality of execution for getting the condition's state.</param>
		/// <returns>The result of getting the conditon's state.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/method[@name="GetAeConditionState"]/doc/*'
		/// />
		public virtual int GetAeConditionState(
			string sourcePath,
			string conditionName,
			AeAttribute[] attributes,
			out AeConditionState conditionState,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			OTCAEConditionStateData inConditionState = new OTCAEConditionStateData();
			conditionState = null;

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

				uint count = 0;
				uint[] attributesIds = new uint[count];
				if (attributes != null)
				{
					count = (uint) attributes.Length;
					attributesIds = new uint[count];
					for (int i = 0; i < count; i++)
					{
						attributesIds[i] = attributes[i].Id;
					}
				}
				res = OTBFunctions.OTCGetAEConditionState(
					this.Handle,
					sourcePath,
					conditionName,
					count,
					attributesIds,
					inConditionState,
					ref options);

				if (ResultCode.SUCCEEDED(res))
				{
					if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
					{
						conditionState = new AeConditionState();
						conditionState.AcknowledgerComment = Marshal.PtrToStringUni(inConditionState.m_ackComment);
						conditionState.AcknowledgerId = Marshal.PtrToStringUni(inConditionState.m_ackID);
						conditionState.ConditionAckTime = inConditionState.m_conditionAckTime.ToDateTime();
						conditionState.ConditionActiveTime = inConditionState.m_conditionActiveTime.ToDateTime();
						conditionState.ConditionInactiveTime = inConditionState.m_conditionInactiveTime.ToDateTime();
						conditionState.Quality = (EnumQuality) inConditionState.m_quality;
						conditionState.State = (EnumConditionState) inConditionState.m_state;
						conditionState.SubConditionActiveTime = inConditionState.m_subConditionActiveTime.ToDateTime();
						conditionState.ActiveSubConditionDefinition = Marshal.PtrToStringUni(inConditionState.m_actSubConditionDefinition);
						conditionState.ActiveSubConditionDescription =
							Marshal.PtrToStringUni(inConditionState.m_actSubConditionDescription);
						conditionState.ActiveSubConditionName = Marshal.PtrToStringUni(inConditionState.m_actSubConditionName);
						conditionState.ActiveSubConditionSeverity = inConditionState.m_actSubConditionSeverity;
						conditionState.SubConditionActiveTime = inConditionState.m_subConditionActiveTime.ToDateTime();

						uint countSubconditions = inConditionState.m_subConditionCount;
						conditionState.SubConditionsDefinitions = new string[countSubconditions];
						conditionState.SubConditionsDescriptions = new string[countSubconditions];
						conditionState.SubConditionsNames = new string[countSubconditions];
						conditionState.SubConditionsSeverities = new uint[countSubconditions];

						int ptrSize = Marshal.SizeOf(typeof (IntPtr));
						int uintSize = Marshal.SizeOf(typeof (uint));

						for (int i = 0; i < countSubconditions; i++)
						{
							IntPtr subConditionName =
								(IntPtr)
								Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(inConditionState.m_pSubConditionName, i*ptrSize),
								                       typeof (IntPtr));
							IntPtr subConditionDefinition =
								(IntPtr)
								Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(inConditionState.m_pSubConditionDefinition, i*ptrSize),
								                       typeof (IntPtr));
							IntPtr subConditionDescription =
								(IntPtr)
								Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(inConditionState.m_pSubConditionDescription, i*ptrSize),
								                       typeof (IntPtr));

							conditionState.SubConditionsSeverities[i] =
								(uint)
								Marshal.PtrToStructure(OTBFunctions.GetIntPtrOffset(inConditionState.m_pSubConditionSeverity, i*uintSize),
								                       typeof (uint));
							conditionState.SubConditionsDefinitions[i] = Marshal.PtrToStringUni(subConditionDefinition);
							conditionState.SubConditionsDescriptions[i] = Marshal.PtrToStringUni(subConditionDescription);
							conditionState.SubConditionsNames[i] = Marshal.PtrToStringUni(subConditionName);

							OTBFunctions.OTFreeMemory(subConditionName);
							OTBFunctions.OTFreeMemory(subConditionDefinition);
							OTBFunctions.OTFreeMemory(subConditionDescription);
						} //	end for

						uint countAttributes = inConditionState.m_eventAttrCount;
						conditionState.EventAttributes = new ArrayList();
						conditionState.AttributesErrors = new int[countAttributes];

						for (int i = 0; i < countAttributes; i++)
						{
							IntPtr attribute = OTBFunctions.GetIntPtrOffset(inConditionState.m_pEventAttrs, ValueQT.VARIANT_SIZE*i);
							object eventAttributte = (object) Marshal.GetObjectForNativeVariant(attribute);
							conditionState.EventAttributes.Add(eventAttributte);
							conditionState.AttributesErrors[i] =
								(int)
								Marshal.PtrToStructure(
									OTBFunctions.GetIntPtrOffset(inConditionState.m_pAttrErrors, i*Marshal.SizeOf(typeof (int))), typeof (int));
						} //	end for
						OTBFunctions.OTFreeMemory(inConditionState.m_ackComment);
						OTBFunctions.OTFreeMemory(inConditionState.m_ackID);
						OTBFunctions.OTFreeMemory(inConditionState.m_actSubConditionDefinition);
						OTBFunctions.OTFreeMemory(inConditionState.m_actSubConditionDescription);
						OTBFunctions.OTFreeMemory(inConditionState.m_actSubConditionName);

						OTBFunctions.OTFreeMemory(inConditionState.m_pSubConditionDefinition);
						OTBFunctions.OTFreeMemory(inConditionState.m_pSubConditionDescription);
						OTBFunctions.OTFreeMemory(inConditionState.m_pSubConditionName);
						OTBFunctions.OTFreeMemory(inConditionState.m_pSubConditionSeverity);

						if (inConditionState.m_eventAttrCount != 0 &&
						    inConditionState.m_pEventAttrs != IntPtr.Zero)
						{
							IntPtr currentPointer = inConditionState.m_pEventAttrs;
							for (int index = 0; index < inConditionState.m_eventAttrCount; index++)
							{
								OTBFunctions.OTVariantClear(currentPointer);
								currentPointer = new IntPtr(currentPointer.ToInt32() + ValueQT.VARIANT_SIZE);
							} //	end for
						} //	end if
						OTBFunctions.OTFreeMemory(inConditionState.m_pEventAttrs);
						OTBFunctions.OTFreeMemory(inConditionState.m_pAttrErrors);
					} //	end if execution is asynchronous
				}
				else
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSession.GetAeConditionState",
						" Getting Ae condition state failed! Result: " + res);
				}
			}  //	try
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.GetAeConditionState",
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
		///  path='//class[@name="AeSession"]/method[@name="ActivateConnectionMonitor"]/doc/*'
		/// />
		///<remarks><note>
		///The returned value should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
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
						"AeSession.ActivateConnectionMonitor",
						"Activating connection monitor failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.ActivateConnectionMonitor",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Gives a textual description of an error.
		/// </summary>
		/// <param name="errorCode">An error code.</param>
		/// <param name="errorText">A textual description for the error that corresponds to the error code.</param>
		///<param name="executionOptions">Specifies the modality of execution for getting the error description.</param>
		///<returns>The result of getting the error description.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSession"]/method[@name="GetErrorString"]/doc/*'
		/// />
		///<remarks><note>
		///The returned value should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		public virtual int GetErrorString(
			int errorCode,
			out string errorText,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			errorText = String.Empty;
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

				IntPtr errorString = IntPtr.Zero;
				res = OTBFunctions.OTCGetErrorString(
					this.Handle,
					errorCode,
					out errorString,
					ref options);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSession.GetErrorString",
						"Getting error string failed! Result: " + res);
				}

				if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
				{
					if (errorString != IntPtr.Zero)
					{
						errorText = Marshal.PtrToStringUni(errorString);
						OTBFunctions.OTFreeMemory(errorString);
					} //	end if
				} //	end if
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.GetErrorString",
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
		///  path='//class[@name="AeSession"]/method[@name="Logon"]/doc/*'
		/// />
		///<remarks><note>
		///The returned value should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
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
						"AeSession.Logon",
						"Logging on failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.Logon",
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
		///  path='//class[@name="AeSession"]/method[@name="Logoff"]/doc/*'
		/// />
		///<remarks><note>
		///The returned value should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
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
						"AeSession.Logoff",
						"Logging off failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.Logoff",
					exc.ToString());
			}
			return res;
		}

		//-
		#endregion

		#region //Internal Methods
		//-----------------------

		internal AeSubscription Find(int position)
		{
			return (AeSubscription) m_subscriptionList[position];
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
					shutdown = ShutdownRequest(this, reason);
				} //	end if
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.OnShutdown",
					exc.ToString());
			} //	end try...catch
			return shutdown;
		} //	end OnShutdown


		/// <summary>
		/// Called when the server status is given asynchronously.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="serverStatus">A <see cref="ServerStatus">ServerStatus</see> object which contain information on the server's parameters.</param>
		/// <param name="result">The result of getting the server status.</param>
		/// <remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnGetServerStatus(
			uint executionContext,
			ServerStatus serverStatus,
			int result)
		{
			try
			{
				if (GetServerStatusCompleted != null)
				{
					GetServerStatusCompleted(this, executionContext, serverStatus, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.OnGetServerStatus",
					exc.ToString());
			}
		}

		///<summary>
		///Called when the server's eventarea space is asynchronously browsed.
		///</summary>
		///<param name="executionContext">An identifier for an asynchronous call.</param>
		///<param name="addressSpaceElementHandle"> A handle for the <see cref="AddressSpaceElement"/> whose children are to be fetched.</param>
		///<param name="addressSpaceElements">A list with the children elements.</param>
		///<param name="result">The result of browsing the server's eventarea space.</param>
		///<remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnBrowse(
			uint executionContext,
			uint addressSpaceElementHandle,
			AddressSpaceElement[] addressSpaceElements,
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
						addressSpaceElements,
						result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.OnBrowse",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when the specific categories of events supported by a server are asynchronously given.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="categories">A list with the event categories.</param>
		/// <param name="result">The result of quering the event categories.</param>
		///<remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnQueryAeCategories(
			uint executionContext,
			AeCategory[] categories,
			int result)
		{
			try
			{
				if (QueryAeCategoriesCompleted != null)
					QueryAeCategoriesCompleted(this, executionContext, categories, result);
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.OnQueryAECategories",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when the available filters are asynchronously given.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="availableFilters">A <see cref="EnumFilterBy"/> object that represents the available filters.</param>
		/// <param name="result">The result of quering for the available filters.</param>
		/// ///<remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnQueryAvailableAeFilters(
			uint executionContext,
			EnumFilterBy availableFilters,
			int result)
		{
			try
			{
				if (QueryAvailableAeFiltersCompleted != null)
					QueryAvailableAeFiltersCompleted(this, executionContext, availableFilters, result);
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.OnQueryAECategories",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when the conditions evaluation for an entire area is asynchronously enabled.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="enable">Specifies if condition evaluation is enabled or disabled. <br></br><i>true</i> means enabled; <i>false</i> means disabled.</param>
		/// <param name="area">The area for which the condition evaluation is enabled.</param>
		/// <param name="result">The result of enabling the condition evaluation.</param>
		/// <remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnEnableAeConditionsByArea(
			uint executionContext,
			bool enable,
			string area,
			int result)
		{
			try
			{
				if (EnableAeConditionsByAreaCompleted != null)
					EnableAeConditionsByAreaCompleted(this, executionContext, enable, area, result);
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.OnEnableConditionsByArea",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when the conditions evaluation for a source is asynchronously enabled.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="enable">Specifies if condition evaluation is enabled or disabled. <br></br><i>true</i> means enabled; <i>false</i> means disabled.</param>
		/// <param name="source">The source for which the condition evaluation is enabled.</param>
		/// <param name="result">The result of enabling the condition evaluation.</param>
		/// <remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnEnableAeConditionsBySource(
			uint executionContext,
			bool enable,
			string source,
			int result)
		{
			try
			{
				if (EnableAeConditionsBySourceCompleted != null)
					EnableAeConditionsBySourceCompleted(this, executionContext, enable, source, result);
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.OnEnableConditionsBySource",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when condition-related events are asynchronously acknowledged.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="ackID">An identifier for the origin of the acknowledgement. </param>
		/// <param name="ackComment">A note that accompanies the acknowledgement.</param>
		/// <param name="conditions">A list with the acknowledged condition-related events.</param>
		/// <param name="results">A list with the result of acknowledging for each condition-related event.</param>
		/// <param name="result">The result of acknowledging the condition-related events.</param>
		/// <remarks><note>
		///All the results should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnAcknowledgeAeConditions(
			uint executionContext,
			string ackID,
			string ackComment,
			AeCondition[] conditions,
			int[] results,
			int result)
		{
			try
			{
				if (AcknowledgeAeConditionsCompleted != null)
					AcknowledgeAeConditionsCompleted(this, executionContext, ackID, ackComment, conditions, results, result);
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.OnAcknowledgeConditions",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when the names of the conditions allocated to a source are asynchronously given.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="source">An identifier in the server's eventarea space for the source.</param>
		/// <param name="conditionsNames">A list with the conditions' names.</param>
		/// <param name="result">The result of quering the conditons' names.</param>
		///<remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnQueryAeSourceConditions(
			uint executionContext,
			string source,
			string[] conditionsNames,
			int result)
		{
			try
			{
				if (QueryAeSourceConditionsCompleted != null)
					QueryAeSourceConditionsCompleted(this, executionContext, source, conditionsNames, result);
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.OnEnableConditionsBySource",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when information on the state of various conditions is asynchronously given.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="sourcePath">An identifier in the server's eventarea space for the source.</param>
		/// <param name="conditionName">A name for the condition whose state is to be given.</param>
		/// <param name="attributtesIDs">A list with the attributes identifiers related to the condition.</param>
		/// <param name="conditionState">A <see cref="AeConditionState">AeConditionState</see>object that contains all the information on the condition's  state.</param>
		/// <param name="result">The result of getting the conditon's state.</param>
		/// ///<remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnGetAeConditionState(
			uint executionContext,
			string sourcePath,
			string conditionName,
			uint[] attributtesIDs,
			AeConditionState conditionState,
			int result)
		{
			try
			{
				if (GetAeConditionStateCompleted != null)
				{
					GetAeConditionStateCompleted(this, executionContext, sourcePath, conditionName, attributtesIDs, conditionState,
					                             result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSession.OnGetConditionState",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when the description of an error is asynchronously given.
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
					"AeSession.OnGetErrorString",
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
					"AeSession.OnLogon",
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
		internal void OnLogoff(
			uint executionContext,
			int result)
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
					"AeSession.OnLogoff",
					exc.ToString());
			}
		}

		//-
		#endregion
	}
}