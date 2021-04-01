using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.ComponentModel.Design.Serialization;
using System.ComponentModel;
using System.Runtime.Serialization;
using System.Security.Permissions;
using System.Globalization;
using Softing.OPCToolbox.OTB;

namespace Softing.OPCToolbox.Client
{

	#region //	Public Delegates
	//------------------------

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents a method that will handle the <see cref="Softing.OPCToolbox.Client.DaSubscription.DataChanged">DataChanged</see>
	/// event of a <see cref="DaSubscription"/>.</para>
	/// </summary>
	/// <param name="aDaSubscription">The <see cref="DaSubscription"/> object to whom the <see cref="Softing.OPCToolbox.Client.DaSubscription.DataChanged">DataChanged</see> event belongs.</param>
	/// <param name="items">A list of items belonging to the subscription.</param>
	/// <param name="values">A list with the new item values.</param>
	/// <param name="results">A list with the result of value change for each item. Each result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="DataChangedEventHandler"]/doc/*'
	/// />
	public delegate void DataChangedEventHandler(
		DaSubscription aDaSubscription,
		DaItem[] items,
		ValueQT[] values,
		int[] results);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents a method that will handle the <see cref="Softing.OPCToolbox.Client.DaSubscription.DataChanged">DataChanged</see>
	/// event of a <see cref="DaSubscription"/>.</para>
	/// </summary>
	/// <param name="executionContext">The direct refresh call execution context</param>
	/// <param name="aDaSubscription">The <see cref="DaSubscription"/> object to whom the <see cref="Softing.OPCToolbox.Client.DaSubscription.DataChanged">DataChanged</see> event belongs.</param>
	/// <param name="items">A list of items belonging to the subscription.</param>
	/// <param name="values">A list with the new item values.</param>
	/// <param name="results">A list with the result of value change for each item. Each result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="DataChangedExEventHandler"]/doc/*'
	/// />
	public delegate void DataChangedExEventHandler(
		uint executionContext,
		DaSubscription aDaSubscription,
		DaItem[] items,
		ValueQT[] values,
		int[] results);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents a method that will handle the <see cref="Softing.OPCToolbox.Client.DaSubscription.ValidateDaItemsCompleted">ValidateDaItemsCompleted</see>
	/// event of a <see cref="DaSubscription"/>.</para>
	/// </summary>
	/// <param name="aDaSubscription">The <see cref="DaSubscription"/> object to whom the <see cref="Softing.OPCToolbox.Client.DaSubscription.ValidateDaItemsCompleted">ValidateDaItemsCompleted</see> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="items">A list with items to be validated.</param>
	/// <param name="results">A list with the result of the validate for each item. Each result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <param name="result">The result of validating items. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="ValidateDaItemsEventHandler"]/doc/*'
	/// />
	public delegate void ValidateDaItemsEventHandler(
		DaSubscription aDaSubscription,
		uint executionContext,
		DaItem[] items,
		int[] results,
		int result);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents a method that will handle the <see cref="Softing.OPCToolbox.Client.DaSubscription.GetDaItemAttributesFromServerCompleted">GetItemAttributesFromServerCompleted</see>
	/// event of a <see cref="DaSubscription"/>.</para>
	/// </summary>
	/// <param name="aDaSubscription">The <see cref="DaSubscription"/> object to whom the <see cref="Softing.OPCToolbox.Client.DaSubscription.GetDaItemAttributesFromServerCompleted">GetDaItemAttributesFromServerCompleted</see> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="items">A list with the items whose attributes are to be updated.</param>
	/// <param name="whatAttributes">A list with all the attributes to be updated.</param>
	/// <param name="results">A list with the result of the update for each item attribute. Each result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <param name="result">The result of updating the item attributes from a server. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="GetDaItemAttributesFromServerEventHandler"]/doc/*'
	/// />
	///
	public delegate void GetDaItemAttributesFromServerEventHandler(
		DaSubscription aDaSubscription,
		uint executionContext,
		DaItem[] items,
		EnumObjectAttribute[] whatAttributes,
		int[] results,
		int result);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents a method that will handle the <see cref="Softing.OPCToolbox.Client.DaSubscription.SetDaItemAttributesToServerCompleted">SetItemAttributesToServerCompleted</see>
	/// event of a <see cref="DaSubscription"/>.</para>
	/// </summary>
	/// <param name="aDaSubscription">The <see cref="DaSubscription"/> object to whom the <see cref="Softing.OPCToolbox.Client.DaSubscription.SetDaItemAttributesToServerCompleted">SetItemAttributesToServerCompleted</see> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="items">A list with the items whose attributes are to be updated.</param>
	/// <param name="whatAttributes">A list with all the attributes to be updated.</param>
	/// <param name="results">A list with the result of the update for each item attribute. Each result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <param name="result">The result of updating the item attributes to a server. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="SetDaItemAttributesToServerEventHandler"]/doc/*'
	/// />
	public delegate void SetDaItemAttributesToServerEventHandler(
		DaSubscription aDaSubscription,
		uint executionContext,
		DaItem[] items,
		EnumObjectAttribute[] whatAttributes,
		int[] results,
		int result);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents a method that will handle the <see cref="Softing.OPCToolbox.Client.DaSubscription.ReadCompleted">ReadCompleted</see>
	/// event of a <see cref="DaSubscription"/>.</para>
	/// </summary>
	/// <param name="aDaSubscription">The <see cref="DaSubscription"/> object to whom the <see cref="Softing.OPCToolbox.Client.DaSubscription.ReadCompleted">ReadCompleted</see> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="items">A list with the items to be read.</param>
	/// <param name="values">A list with the values that have been read.</param>
	/// <param name="results">A list with the result of reading for each item. Each result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <param name="result">The result of reading the items. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="SubscriptionReadEventHandler"]/doc/*'
	/// />
	public delegate void SubscriptionReadEventHandler(
		DaSubscription aDaSubscription,
		uint executionContext,
		DaItem[] items,
		ValueQT[] values,
		int[] results,
		int result);

	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents a method that will handle the <see cref="Softing.OPCToolbox.Client.DaSubscription.WriteCompleted">WriteCompleted</see>
	/// event of a <see cref="DaSubscription"/>.</para>
	/// </summary>
	/// <param name="aDaSubscription">The <see cref="DaSubscription"/> object to whom the <see cref="Softing.OPCToolbox.Client.DaSubscription.WriteCompleted">WriteCompleted</see> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="items">A list with the items to be written.</param>
	/// <param name="values">A list with the values that need to be written.</param>
	/// <param name="results">A list with the result of writing for each item. Each result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <param name="result">The result of writing the items. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="SubscriptionWriteEventHandler"]/doc/*'
	/// />
	public delegate void SubscriptionWriteEventHandler(
		DaSubscription aDaSubscription,
		uint executionContext,
		DaItem[] items,
		ValueQT[] values,
		int[] results,
		int result);

	//-

	#endregion

	/// <summary>
	/// <para>Represents a logical grouping of <b>OPC Data Access Item</b> objects that can be accessed by a client.</para>
	/// <para>The items are grouped based on different aspects (for example, the dynamics of the changed value).
	/// The most important criterion for grouping the items is the update rate, common to all the items in a group.</para>
	/// <para>The values of active items in an active group are checked cyclically with a given update rate and the clients are
	/// notified about the value changes.</para>
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="DaSubscription"]/doc/*'
	/// />
	[Serializable]
	public class DaSubscription : ObjectSpaceElement, ISerializable
	{
		#region //	Public Constructors
		//---------------------------

		/// <summary>
		/// Initializes a new instance of the <see cref="DaSubscription"/> class with
		/// the value indicated by an update rate and a <see cref="DaSession"/> object to whom to be added.
		/// After creating it, the <b>DaSubscription</b> object is added to the session it is meant to belong.
		/// </summary>
		/// <param name="updateRate">The cyclic rate that the  active <see cref="DaItem"/> objects have to be read.</param>
		/// <param name="parentSession">The session to whom the subscription is to be added.</param>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/constructor[@name="DaSubscription"]/doc/*'
		/// />
		public DaSubscription(
			uint updateRate,
			DaSession parentSession)
		{
			m_itemList = new Hashtable();

			//sets the parentSession of the newly created subscription
			m_session = parentSession;

			try
			{
				int res = (int) EnumResultCode.E_FAIL;

				res = parentSession.AddDaSubscription(updateRate, this);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.Constructor",
						"The subscription could not be added to the session! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.Constructor",
					exc.ToString());
			}
		}

		//-
		#endregion

		#region //	Public Events
		//---------------------

		/// <summary>
		/// Occurs when the client is notified about the changes in the items values for the items that belong to the subscription.
		/// This event is raised at a time interval equal with the subscription's update rate, only if at least one of the items that belong to the subscription change its value.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/event[@name="DataChanged"]/doc/*'
		/// />
		public virtual event DataChangedEventHandler DataChanged;

		/// <summary>
		/// Occurs when the client is notified about the changes in the items values for the items that belong to the subscription.
		/// This event is raised at a time interval equal with the subscription's update rate, only if at least one of the items that belong to the subscription change its value.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/event[@name="DataChangedEx"]/doc/*'
		/// />
		public virtual event DataChangedExEventHandler DataChangedEx;

		/// <summary>
		///Occurs when validating items completes.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="ValidateDaItems"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="ValidateDaItems"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/event[@name="ValidateDaItemsCompleted"]/doc/*'
		/// />
		public event ValidateDaItemsEventHandler ValidateDaItemsCompleted;

		/// <summary>
		/// Occurs when updating some item attributes from the server completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="GetDaItemAttributesFromServer"/> method.</para>
		/// <para><b>Unmanaged C++</b>This method is called after the method <see cref="GetDaItemAttributesFromServer"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/event[@name="GetDaItemAttributesFromServerCompleted"]/doc/*'
		/// />
		public event GetDaItemAttributesFromServerEventHandler GetDaItemAttributesFromServerCompleted;

		/// <summary>
		/// Occurs when updating some item attributes to the server completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="SetDaItemAttributesToServer"/> method.</para>
		/// <para><b>Unmanaged C++</b>This method is called after the method <see cref="SetDaItemAttributesToServer"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/event[@name="SetDaItemAttributesToServerCompleted"]/doc/*'
		/// />
		public event SetDaItemAttributesToServerEventHandler SetDaItemAttributesToServerCompleted;

		/// <summary>
		/// Occurs when reading some item values completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="Read"/> method.</para>
		/// <para><b>Unmanaged C++</b>This method is called after the method <see cref="Read"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/event[@name="ReadCompleted"]/doc/*'
		/// />
		public event SubscriptionReadEventHandler ReadCompleted;

		/// <summary>
		/// Occurs when writing some item values completes.
		/// <para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="Write"/> method.</para>
		/// <para><b>Unmanaged C++</b>This method is called after the method <see cref="Write"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/event[@name="WriteCompleted"]/doc/*'
		/// />
		public event SubscriptionWriteEventHandler WriteCompleted;

		//-
		#endregion

		#region //	Private Attributes
		//-----------------------

		private readonly OTCObjectAttributes m_objectAttributes = new OTCObjectAttributes();
		private readonly object m_attributesJanitor = new object();

		//-
		#endregion

		#region //	Protected Attributes
		//--------------------------

		/// <summary>
		///  A list with all the items (<see cref="DaItem"/> objects) belonging to the subscription.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/field[@name="m_itemList"]/doc/*'
		/// />
		protected Hashtable m_itemList = new Hashtable();

		/// <summary>
		/// The session (<see cref="DaSession"/> object) to which the subscription belongs.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/field[@name="m_session"]/doc/*'
		/// />
		protected DaSession m_session = null;

		//-
		#endregion

		#region //	Public Properties
		//--------------------------

		/// <summary>
		/// Gets or sets the <see cref="Softing.OPCToolbox.Client.DaSession"/> object to which the subscription belongs.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/property[@name="DaSession"]/doc/*'
		/// />
		public DaSession DaSession
		{
			get { return m_session; }
			set { m_session = value; }
		}

		/// <summary>
		/// Gets the list with the items belonging to the subscription.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/property[@name="ItemList"]/doc/*'
		/// />
		public DaItem[] ItemList
		{
			get
			{
				lock (m_itemList.SyncRoot) // enumerating
				{
					DaItem[] itemVector = new DaItem[m_itemList.Count];
					int i = 0;

					foreach (DaItem daItem in m_itemList.Values)
					{
						itemVector[i] = daItem;
						i++;
					} //	end foreach

					return itemVector;
				}
			}
		}

		/// <summary>
		/// Gets or sets the speed (in milliseconds)requested for scanning the active item objects.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/property[@name="RequestedUpdateRate"]/doc/*'
		/// />
		public uint RequestedUpdateRate
		{
			get
			{
				uint updateRate = 0;
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
									new uint[] {(uint) EnumObjectAttribute.DASUBSCRIPTION_UPDATERATE},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSUbscription.RequestedUpdateRate.Get",
										"Update attribute DASUBSCRIPTION_UPDATERATE from server failed! Result: " + updateResult);
								} //	end if
							} //	end if

							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASUBSCRIPTION_UPDATERATE,
								objectAttributes);

							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								updateRate = objectAttributes.m_daSubscriptionRequestedUpdateRate;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSUbscription.RequestedUpdateRate.Get",
									"Get attribute DASUBSCRIPTION_UPDATERATE from server failed! Result: " + getAttrsResult);
							} //	end else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.RequestedUpdateRate.Get",
						exc.ToString());
				}
				return updateRate;
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
							m_objectAttributes.m_daSubscriptionRequestedUpdateRate = value;
							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASUBSCRIPTION_UPDATERATE,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSubscription.RequestedUpdateRate.Set",
									"Set attribute DASUBSCRIPTION_UPDATERATE failed! Result: " + setAttrsResult);
							} //	end if
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.DASUBSCRIPTION_UPDATERATE},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSubscription.RequestedUpdateRate.Set",
										"Set attribute DASUBSCRIPTION_UPDATERATE failed! Result: " + updateResult);
								}
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSubscription.RequestedUpdateRate.Set",
							"Set RequestedUpdateRate failed! Subscription is not added to a session");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.RequestedUpdateRate.Set",
						exc.ToString());
				}
			} //	end set
		} // end RequestedUpdateRate

		/// <summary>
		/// Gets the actual speed with which the values of the active item objects are scanned.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/property[@name="RevisedUpdateRate"]/doc/*'
		/// />
		/// <value>
		///
		/// </value>
		[DesignerSerializationVisibility(System.ComponentModel.DesignerSerializationVisibility.Visible)]
		public uint RevisedUpdateRate
		{
			get
			{
				uint revisedUpdateRate = 0;
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
									new uint[] {(uint) EnumObjectAttribute.DASUBSCRIPTION_UPDATERATE},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSubscription.RevisedUpdateRate.Get",
										"Update attribute DASUBSCRIPTION_UPDATERATE from server failed! Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASUBSCRIPTION_UPDATERATE,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								revisedUpdateRate = objectAttributes.m_daSubscriptionRevisedUpdateRate;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSubscription.RevisedUpdateRate.Get",
									"Get attribute DASUBSCRIPTION_UPDATERATE from server failed! Result: " + getAttrsResult);
							} //	end else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.RevisedUpdateRate.Get",
						exc.ToString());
				}
				return revisedUpdateRate;
			} //	end get
		} //	end RevisedUpdateRate

		/// <summary>
		/// Gets or sets the requested maximum period between updates sent to the client.
		/// Clients can set the keep-alive time for a subscription to cause the server to provide client callbacks on the subscription when
		/// there are no new events to report. Clients can then be assured of the health of the server and subscription without resorting to
		/// pinging the server with calls to GetStatus.
		/// Using this facility, a client can expect a callback (data or keep-alive) within the specified keep-alive.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/property[@name="RequestedKeepAliveTime"]/doc/*'
		/// />
		public uint RequestedKeepAliveTime
		{
			get
			{
				uint requestedKeepAliveTime = 0;
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
									new uint[] {(uint) EnumObjectAttribute.DASUBSCRIPTION_KEEPALIVETIME},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSubscription.RequestedKeepAliveTime.Get",
										"Update attribute DASUBSCRIPTION_KEEPALIVETIME from server failed! Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASUBSCRIPTION_KEEPALIVETIME,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								requestedKeepAliveTime = objectAttributes.m_daSubscriptionRequestedKeepAliveTime;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSubscription.RequestedKeepAliveTime.Get",
									"Get attribute DASUBSCRIPTION_KEEPALIVETIME from server failed! Result: " + getAttrsResult);
							} //	end else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.RequestedKeepAliveTime.Get",
						exc.ToString());
				}
				return requestedKeepAliveTime;
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
							m_objectAttributes.m_daSubscriptionRequestedKeepAliveTime = value;
							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASUBSCRIPTION_KEEPALIVETIME,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSession.LocaleId.Set",
									"Set attribute DASUBSCRIPTION_KEEPALIVETIME failed! Result: " + setAttrsResult);
							} //	end if
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.DASUBSCRIPTION_KEEPALIVETIME},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSubscription.RequestedKeepAliveTime.Get",
										"Set attribute DASUBSCRIPTION_KEEPALIVETIME failed! Result: " + updateResult);
								}
							}
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSubscription.RequestedKeepAliveTime.Get",
							"Set RequestedKEepAliveTime failed! Subscription is not added to a session");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.RequestedKeepAliveTime.Set",
						exc.ToString());
				}
			} //	end set
		} // end RequestedKeepAliveTime

		/// <summary>
		/// Gets the actual maximum period between updates sent to the client.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/property[@name="RevisedKeepAliveTime"]/doc/*'
		/// />
		public uint RevisedKeepAliveTime
		{
			get
			{
				uint revisedKeepAliveTime = 0;
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
									new uint[] {(uint) EnumObjectAttribute.DASUBSCRIPTION_KEEPALIVETIME},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSubscription.RevisedKeepAliveTime.Get",
										"Update attribute DASUBSCRIPTION_KEEPALIVETIME from server failed! Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASUBSCRIPTION_KEEPALIVETIME,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								revisedKeepAliveTime = objectAttributes.m_daSubscriptionRevisedKeepAliveTime;
							}
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSubscription.RevisedKeepAliveTime.Get",
									"Get attribute DASUBSCRIPTION_KEEPALIVETIME from server failed! Result: " + getAttrsResult);
							} //	end else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.RevisedKeepAliveTime.Get",
						exc.ToString());
				}
				return revisedKeepAliveTime;
			} //	end get
		} // end RevisedKeepAliveTime

		public EnumDataRetrievalMode DataRetreivalMode
		{
			get
			{
				EnumDataRetrievalMode dataRetrievalMode = (EnumDataRetrievalMode)0;

				try
				{
					OTCObjectAttributes objectAttributes = new OTCObjectAttributes();

					int getAttrResult = OTBFunctions.OTCGetAttributes(
						this.Handle,
						(uint)EnumObjectAttribute.DASUBSCRIPTION_DATARETRIEVAL,
						objectAttributes);

					if (ResultCode.SUCCEEDED(getAttrResult))
					{
						dataRetrievalMode = (EnumDataRetrievalMode)objectAttributes.m_daSubscriptionDataRetreivalMode;
					} // end if
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSubscription.DataRetreivalMode.Get",
							"Get DASUBSCRIPTION_DATARETRIEVAL failed! Result: " + getAttrResult);
					}
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.DataRetreivalMode.Get",
						exc.ToString());
				}

				return dataRetrievalMode;
			}
			set
			{
				try
				{
					OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
					objectAttributes.m_daSubscriptionDataRetreivalMode = (uint)value;

					int setAttrsResult = OTBFunctions.OTCSetAttributes(
						this.Handle,
						(uint)EnumObjectAttribute.DASUBSCRIPTION_DATARETRIEVAL,
						objectAttributes);

					if (ResultCode.FAILED(setAttrsResult))
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSubscription.DataRetreivalMode.Set",
							"Set DASUBSCRIPTION_DATARETRIEVAL to " + value + " failed! Result: " + setAttrsResult);
					}
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.DataRetreivalMode.Set",
						exc.ToString());
				}
			}
		}

		/// <summary>
		/// Gets or sets a symbolic name for the subscription object that describes its semantic significance (e.g "Temperatures_on_Floor_1")
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/property[@name="Name"]/doc/*'
		/// />
		[DesignerSerializationVisibility(System.ComponentModel.DesignerSerializationVisibility.Visible)]
		public string Name
		{
			get
			{
				string subscriptionName = string.Empty;
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
									new uint[] {(uint) EnumObjectAttribute.DASUBSCRIPTION_NAME},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSubscription.Name.Get",
										"Update attribute DASUBSCRIPTION_NAME from server failed! Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASUBSCRIPTION_NAME,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								subscriptionName = Marshal.PtrToStringUni(objectAttributes.m_daSubscriptionName);
								OTBFunctions.OTFreeMemory(objectAttributes.m_daSubscriptionName);
							}
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSubscription.Name.Get",
									"Get attribute DASUBSCRIPTION_NAME from server failed! Result: " + getAttrsResult);
							} //	end else
						} //	end lock
					}
					else
					{
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.Name.Get",
						exc.ToString());
				} //	end try...catch
				return subscriptionName;
			} //	end get
			set
			{
				try
				{
					if (this.Handle != 0)
					{
						if (value != string.Empty)
						{
							OTCExecutionOptions options = new OTCExecutionOptions();
							int result = -1;
							lock (m_attributesJanitor)
							{
								m_objectAttributes.m_daSubscriptionName = Marshal.StringToCoTaskMemUni(value);
								int setAttrsResult = OTBFunctions.OTCSetAttributes(
									this.Handle,
									(uint) EnumObjectAttribute.DASUBSCRIPTION_NAME,
									m_objectAttributes);
								if (ResultCode.FAILED(setAttrsResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSubscription.Name.Set",
										"Set attribute DASUBSCRIPTION_NAME failed! Result: " + setAttrsResult);
								} //	end if
								if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
								{
									int updateResult = OTBFunctions.OTCUpdateAttributes(
										this.Handle,
										0,
										1,
										new uint[] {(uint) EnumObjectAttribute.DASUBSCRIPTION_NAME},
										new int[] {result},
										ref options);
									if (ResultCode.FAILED(updateResult))
									{
										Application.Instance.Trace(
											EnumTraceLevel.ERR,
											EnumTraceGroup.CLIENT,
											"DaSubscription.Name.Set",
											"Set attribute DASUBSCRIPTION_NAME failed! Result: " + updateResult);
									} //	end if
								} //	end if
								Marshal.FreeCoTaskMem(m_objectAttributes.m_daSubscriptionName);
							} //	end lock
						} //	end if
					} //	end if
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSubscription.Name.Set",
							"Set Name failed! Subscription is not added to a session");
					} //	end if...else
				}
				catch (ArgumentNullException exc)
				{
					Application.Instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "DaSubscription.Name.Set", exc.ToString());
				}
				catch (OutOfMemoryException exc)
				{
					Application.Instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "DaSubscription.Name.Set", exc.ToString());
				}
				catch (Exception exc)
				{
					Marshal.FreeCoTaskMem(m_objectAttributes.m_daSubscriptionName);
					Application.Instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "DaSubscription.Name.Set", exc.ToString());
				}
			} //	end set
		} // end Name

		/// <summary>
		/// Gets or sets the minimum percentage change required to trigger a data update for an item.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/property[@name="Deadband"]/doc/*'
		/// />
		public float Deadband
		{
			get
			{
				float deadband = 0;
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
									new uint[] {(uint) EnumObjectAttribute.DASUBSCRIPTION_DEADBAND},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSubscription.Deadband.Get",
										"Update attribute DASUBSCRIPTION_DEADBAND from server failed! Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASUBSCRIPTION_DEADBAND,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								deadband = objectAttributes.m_daSubscriptionDeadband;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSubscription.Deadband.Get",
									"Get attribute DASUBSCRIPTION_DEADBAND from server failed! Result: " + getAttrsResult);
							} //	end else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.Deadband.Get",
						exc.ToString());
				}
				return deadband;
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
							m_objectAttributes.m_daSubscriptionDeadband = value;
							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASUBSCRIPTION_DEADBAND,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSubscription.Deadband.Set",
									"Set attribute DASUBSCRIPTION_DEADBAND failed! Result: " + setAttrsResult);
							} //	end if
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.DASUBSCRIPTION_DEADBAND},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSubscription.Deadband.Set",
										"Set attribute DASUBSCRIPTION_DEADBAND failed! Result: " + updateResult);
								} //	end if
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSubscription.Deadband.Set",
							"Set Deadband failed! Subscription is not added to a session ");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.Deadband.Set",
						exc.ToString());
				}
			} //	end set
		} // end Deadband

		/// <summary>
		/// Gets or sets the deviation from GMT(Greenwich Mean Time) in minutes.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/property[@name="TimeBias"]/doc/*'
		/// />
		public int TimeBias
		{
			get
			{
				int timeBias = 0;
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
									new uint[] {(uint) EnumObjectAttribute.DASUBSCRIPTION_TIMEBIAS},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSubscription.TimeBias.Get",
										"Update attribute DASUBSCRIPTION_TIMEBIAS from server failed! Result: " + updateResult);
								} //	end if
							} //	end lock
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASUBSCRIPTION_TIMEBIAS,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								timeBias = objectAttributes.m_daSubscriptionTimeBias;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSubscription.TimeBias.Get",
									"Get attribute DASUBSCRIPTION_TIMEBIAS from server failed! Result: " + getAttrsResult);
							} //	end else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.Timebias.Get",
						exc.Message);
				}
				return timeBias;
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
							m_objectAttributes.m_daSubscriptionTimeBias = value;
							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASUBSCRIPTION_TIMEBIAS,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSubscription.TimeBias.Set",
									"Set attribute DASUBSCRIPTION_TIMEBIAS failed! Result: " + setAttrsResult);
							} //	end if
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.DASUBSCRIPTION_TIMEBIAS},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSubscription.TimeBias.Set",
										"Set attribute DASUBSCRIPTION_TIMEBIAS failed! Result: " + updateResult);
								} //	end if
							} //	end if
						} //	end lock
					} //	end if
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSubscription.TimeBias.Set",
							"Set TimeBias failed! Subscription is not added to a session ");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Subscription.TimeBias.Set",
						exc.Message);
				} //	end try...catch
			} //	end set
		} // end TimeBias

		/// <summary>
		/// Gets or sets the valid language identifier for the current subscription.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/property[@name="LocaleId"]/doc/*'
		/// />
		public string LocaleId
		{
			get
			{
				uint localeID = 0;
				string cultureName = String.Empty;
				CultureInfo culture;
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
									new uint[] {(uint) EnumObjectAttribute.DASUBSCRIPTION_LCID},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSubscription.LocaleId.Get",
										"Update attribute DASUBSCRIPTION_LCID from server failed! Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASUBSCRIPTION_LCID,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								localeID = objectAttributes.m_daSubscriptionLCID;
								switch (localeID)
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

										culture = new CultureInfo((int) localeID);
										cultureName = culture.Name;
										break;
								}
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSubscription.LocaleId.Get",
									"Get attribute DASUBSCRIPTION_LCID from server failed! Result: " + getAttrsResult);
							} //	end else
						} //	end lock
					} //	end if
				}
				catch (ArgumentException exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.INF,
						EnumTraceGroup.CLIENT,
						"DaSubscription.LocaleId.Get",
						exc.Message);
					cultureName = localeID.ToString();
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Subscription.LocaleID.Get",
						exc.ToString());
				}
				return cultureName;
			} //	end get
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
							m_objectAttributes.m_daSubscriptionLCID = lcid;

							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DASUBSCRIPTION_LCID,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaSubscription.LocaleId.Set",
									"Set attribute DASUBSCRIPTION_LCID failed! Result: " + setAttrsResult);
							} //	end if
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.DASUBSCRIPTION_LCID},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaSubscription.LocaleId.Set",
										"Set attribute DASUBSCRIPTION_LCID failed! Result: " + updateResult);
								}
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSubscription.LocaleId.Set",
							"Set LocaleId failed! Subscription is not added to a session ");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.LocaleId.Set",
						exc.ToString());
				}
			} //	end set
		} //	end LocaleID

		//-
		#endregion

		#region //	Public Methods
		//----------------------

		/// <summary>
		/// Adds an item (<see cref="DaItem"/> object) to the subscription( <see cref="DaSubscription"/> object).
		/// </summary>
		/// <param name="itemId">An unique identifier for the item in the server's address space.</param>
		/// <param name="anItem">The item to be added.</param>
		/// <returns>The result of adding the <b>DaItem</b> object.</returns>
		internal virtual int AddDaItem(
			string itemId,
			DaItem anItem)
		{
			int res = (int) EnumResultCode.E_FAIL;
			try
			{
				if (anItem != null)
				{
					uint itemHandle = 0;
					res = OTBFunctions.OTCAddDAItem(
						this.Handle,
						itemId,
						(uint) anItem.GetHashCode(),
						ref itemHandle);

					if (ResultCode.SUCCEEDED(res))
					{
						anItem.DaSubscription = this;
						anItem.Handle = itemHandle;
						lock (m_itemList.SyncRoot) // modifying content
						{
							m_itemList.Add(anItem.GetHashCode(), anItem);
						}
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSubscription.AddDaItem",
							" Adding item failed! Result: " + res);
					}
				}
				else
				{
					res = (int) EnumResultCode.E_INVALIDARG;
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.AddDaItem",
						" The item argument is invalid");
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.AddDaItem",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Adds an item (<see cref="DaItem"/> object)to the subscription ( <see cref="DaSubscription"/> object).
		/// </summary>
		/// <param name="anItem">The item to be added.</param>
		/// <returns>The result of adding the item.</returns>
		internal virtual int AddDaItem(
			DaItem anItem)
		{
			int res = (int) EnumResultCode.E_FAIL;

			try
			{
				if (anItem != null)
				{
					uint itemHandle = 0;
					res = OTBFunctions.OTCAddDAItem(
						this.Handle,
						anItem.Id,
						(uint) anItem.GetHashCode(),
						ref itemHandle);

					if (ResultCode.SUCCEEDED(res))
					{
						anItem.DaSubscription = this;
						anItem.Handle = itemHandle;
						lock (m_itemList.SyncRoot) // modifying content
						{
							m_itemList.Add(anItem.GetHashCode(), anItem);
						}
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSubscription.AddDaItem",
							" Adding item failed! Result: " + res);
					}
				}
				else
				{
					res = (int) EnumResultCode.E_INVALIDARG;
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.AddDaItem",
						" The item argument is invalid");
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.AddDaItem",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Removes an item (<see cref="DaItem"/> object) from the subscription (<see cref="DaSubscription"/> object).
		/// </summary>
		/// <param name="anItem">The item to be removed.</param>
		/// <returns>The result of removing the item.</returns>
		/// <remarks><note>
		/// If a large number of DA Items is to be removed, it is recommended to first disconnect the DA Subscription,
		/// remove the required DA Items and then reconnect the DA Subscription. Not following this recommendation may
		/// lead to performance-related problems with removal of DA Items (for a large number of items). Please see
		/// FAQ for more details on this matter.
		/// </note></remarks>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/method[@name="RemoveDaItem"]/doc/*'
		/// />
		public virtual int RemoveDaItem(DaItem anItem)
		{
			int res = (int) EnumResultCode.E_FAIL;
			try
			{
				if (anItem != null)
				{
					res = OTBFunctions.OTCRemoveDAItem(((DaItem) m_itemList[anItem.GetHashCode()]).Handle);
					if (ResultCode.SUCCEEDED(res))
					{
						lock (m_itemList.SyncRoot) // modifying content
						{
							m_itemList.Remove(anItem.GetHashCode());
						}
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSubscription.RemoveDaItem",
							" Removing item failed! Result: " + res);
					}
				}
				else
				{
					res = (int) EnumResultCode.E_INVALIDARG;
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.RemoveDaItem",
						"The argument is invalid!");
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.RemoveDaItem",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Reads the current values for a set of items that belong to the subscription.
		/// </summary>
		/// <param name="maxAge">The oldest (in milliseconds) acceptable cached value when reading an item.</param>
		/// <param name="items">A list with the items whose values are to be read.</param>
		/// <param name="values">A list with the read values.</param>
		/// <param name="results">A list with the result of the read for each item. </param>
		/// <param name="executionOptions">Specifies the modality of execution for reading items.</param>
		/// <returns>The result of reading values for a set of items that belong to a subscription.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/method[@name="Read"]/doc/*'
		/// />
		public virtual int Read(
			uint maxAge,
			DaItem[] items,
			out ValueQT[] values,
			out int[] results,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			values = null;
			results = new int[items.Length];
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
				} //	all the subscription items are to be read
				uint[] itemHandles = new uint[items.Length];
				for (int i = 0; i < items.Length; i++)
				{
					itemHandles[i] = ((DaItem) items[i]).Handle;
				}
				OTValueData[] itemValues = new OTValueData[items.Length];
				for (int i = 0; i < items.Length; i++)
				{
					itemValues[i].m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
					Marshal.GetNativeVariantForObject(null, itemValues[i].m_value);
				}
				res = OTBFunctions.OTCRead(
					this.Handle,
					items.Length,
					itemHandles,
					null,
					null,
					maxAge,
					itemValues,
					results,
					ref options);

				if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
				{
					// the read succedeed
					if (ResultCode.SUCCEEDED(res))
					{
						values = new ValueQT[items.Length];
						for (int i = 0; i < itemValues.Length; i++)
						{
							values[i] = new ValueQT(ref itemValues[i]);
						}
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaSubscription.Read",
							" Reading items using subscription failed! Result: " + res);
					}
				}
				//deallocates memory
				for (int i = 0; i < itemValues.Length; i++)
				{
					OTBFunctions.OTVariantClear(itemValues[i].m_value);
					Marshal.FreeCoTaskMem(itemValues[i].m_value);
				}
			}
			catch (ArgumentNullException exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.Read",
					exc.ToString());
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.Read",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Writes the value, quality and timestamp for a set of items that belong to the subscription.
		/// </summary>
		/// <param name="items">A list with the items to be written.</param>
		/// <param name="values">A list with the values to be written.</param>
		/// <param name="results">A list with the result of the write for each item. </param>
		/// <param name="executionOptions">Specifies the modality of execution for writing items.</param>
		/// <returns>The result of writing values for the items that belong to the subscription.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/method[@name="Write"]/doc/*'
		/// />
		public virtual int Write(
			DaItem[] items,
			ValueQT[] values,
			out int[] results,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			results = new int[items.Length];

			if (values == null)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.Write",
					"Values cannot be null!");

				return res;
			} //	end if

			if (items == null)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.Write",
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

				uint[] itemHandles = new uint[items.Length];
				OTValueData[] inValues = new OTValueData[items.Length];

				for (int i = 0; i < items.Length; i++)
				{
					itemHandles[i] = ((DaItem) items[i]).Handle;
				}

				for (int i = 0; i < items.Length; i++)
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
					items.Length,
					itemHandles,
					null,
					null,
					inValues,
					results,
					ref options);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.Write",
						" Write items using subscription failed! Result: " + res);
				}

				for (int i = 0; i < items.Length; i++)
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
					"DaSubscription.Write",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Validates a set of items(checks if they could be added to the subscription without error).
		/// </summary>
		/// <param name="items">A list with items to be validated.</param>
		/// <param name="results">A list with the result of the validate for each item.</param>
		/// <param name="executionOptions">Specifies the modality of execution for validating items.</param>
		/// <returns>The result of validating items.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/method[@name="ValidateDaItems"]/doc/*'
		/// />
		public virtual int ValidateDaItems(
			DaItem[] items,
			out int[] results,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			results = new int[items.Length];
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

				uint[] itemHandles = new uint[items.Length];
				for (int i = 0; i < items.Length; i++)
				{
					itemHandles[i] = ((DaItem) items[i]).Handle;
				}
				res = OTBFunctions.OTCValidateDAItems(
					this.Handle,
					items.Length,
					itemHandles,
					results,
					ref options);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.ValidateDaItems",
						" Validating item failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.ValidateDaItems",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Updates the attributes of the items that belong to a subscription. The update is made from the server.
		/// </summary>
		/// <param name="items">A list with the items whose attributes are to be updated. </param>
		/// <param name="whatAttributes">A list with all the item attributes to be updated.</param>
		/// <param name="results">A list with the result of the update for each item attribute. </param>
		/// <param name="executionOptions">Specifies the modality of execution for updating attributes from a server.</param>
		/// <returns>The result of updating attributes from the server.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/method[@name="GetDaItemAttributesFromServer"]/doc/*'
		/// />
		public virtual int GetDaItemAttributesFromServer(
			DaItem[] items,
			EnumObjectAttribute[] whatAttributes,
			out int[] results,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			results = new int[items.Length];
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

				uint[] itemHandles = new uint[items.Length];
				for (int i = 0; i < itemHandles.Length; i++)
				{
					itemHandles[i] = items[i].Handle;
				}
				uint[] whatAttributesToChange = new uint[whatAttributes.Length];

				for (int i = 0; i < whatAttributes.Length; i++)
				{
					whatAttributesToChange[i] = (uint) whatAttributes[i];
				}
				res = OTBFunctions.OTCUpdateDAItemAttributes(
					this.Handle,
					items.Length,
					itemHandles,
					1,
					(uint) whatAttributes.Length,
					whatAttributesToChange,
					results,
					ref options);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.GetDaItemAttributesFromServer",
						" Getting attributes from server failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.GetDaItemAttributesFromServer",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Updates the attributes of the items that belong to a subscription. The update is made to the server.
		/// </summary>
		/// <param name="items">A list with the items whose attributes are to be updated. </param>
		/// <param name="whatAttributes">A list with all the item attributes to be updated.</param>
		/// <param name="results">A list with the result of the update for each item attribute. </param>
		/// <param name="executionOptions">Specifies the modality of execution for updating attributes to a server.</param>
		/// <returns>The result of updating attributes to the server.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/method[@name="SetDaItemAttributesToServer"]/doc/*'
		/// />
		public virtual int SetDaItemAttributesToServer(
			DaItem[] items,
			EnumObjectAttribute[] whatAttributes,
			out int[] results,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			results = new int[items.Length];
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

				uint[] itemHandles = new uint[items.Length];
				for (int i = 0; i < itemHandles.Length; i++)
				{
					itemHandles[i] = items[i].Handle;
				}
				uint[] whatAttributesToChange = new uint[whatAttributes.Length];
				for (int i = 0; i < whatAttributes.Length; i++)
				{
					whatAttributesToChange[i] = (uint) whatAttributes[i];
				}
				res = OTBFunctions.OTCUpdateDAItemAttributes(
					this.Handle,
					items.Length,
					itemHandles,
					0,
					(uint) whatAttributes.Length,
					whatAttributesToChange,
					results,
					ref options);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.SetDaItemAttributesToServer",
						" Setting attributes to server failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.SetDaItemAttributesToServer",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		///	Reads all the active items of the subscription if the subscription is active.
		/// </summary>
		/// <param name="maxAge">The oldest (in milliseconds) acceptable cached value when reading an item.</param>
		/// <param name="executionOptions">Specifies the modality of execution for refreshing the subscription.</param>
		/// <returns>The result of refreshing the subscription.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/method[@name="Refresh"]/doc/*'
		/// />
		public virtual int Refresh(uint maxAge, ExecutionOptions executionOptions)
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

				res = OTBFunctions.OTCRefreshDAGroup(
					this.Handle,
					maxAge,
					ref options);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaSubscription.Refresh",
						" Refreshing subscription failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaSubscription.Refresh",
					exc.ToString());
			}
			return res;
		}

		//-
		#endregion

		#region //	Internal Methods
		internal DaItem Find(int position)
		{
			return (DaItem) m_itemList[position];
		}

		//-------------------------------

		/// <summary>
		/// Called when the active items of the subscription change their values.
		/// </summary>
		/// <param name="executionContext">An user provided identifier for the async callback.</param>
		/// <param name="items">A list of items belonging to the subscription.</param>
		/// <param name="values">A list with the new item values.</param>
		/// <param name="results">A list with the result of value change for each item.</param>
		/// <remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal virtual void OnDataChange(
			uint executionContext,
			DaItem[] items,
			ValueQT[] values,
			int[] results)
		{
			try
			{
				if (DataChanged != null)
				{
					this.DataChanged(this, items, values, results);
				}

				if (DataChangedEx != null)
				{
					this.DataChangedEx(executionContext, this, items, values, results);
				}

				for (int i = 0; i < items.Length; i++)
				{
					if (items[i] != null)
					{
						items[i].OnValueChange(values[i], results[i]);
					}
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Da.Subscription.OnDataChange",
					exc.ToString());
			}
		}

		/// <summary>
		///  Called when items are asynchronously read using a subscription.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="items">A list with the read items.</param>
		/// <param name="values">A list with the read values.</param>
		/// <param name="results">A list with the result of reading for each item.</param>
		/// <param name="result">The result of reading the items.</param>
		/// <remarks><note>
		///All the results should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnReadComplete(
			uint executionContext,
			DaItem[] items,
			ValueQT[] values,
			int[] results,
			int result)
		{
			try
			{
				if (ReadCompleted != null)
				{
					ReadCompleted(this, executionContext, items, values, results, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Da.Subscription.OnReadComplete",
					exc.ToString());
			}
		}

		/// <summary>
		///  Called when items are asynchronously written using a subscription.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="items">A list with the written items.</param>
		/// <param name="values">A list with the written values.</param>
		/// <param name="results">A list with the result of writing for each item.</param>
		/// <param name="result">The result of writing the items.</param>
		/// <remarks><note>
		///All the results should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnWriteComplete(
			uint executionContext,
			DaItem[] items,
			ValueQT[] values,
			int[] results,
			int result)
		{
			try
			{
				if (WriteCompleted != null)
				{
					WriteCompleted(this, executionContext, items, values, results, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Da.Subscription.OnWriteComplete",
					exc.ToString());
			}
		}

		/// <summary>
		///  Called when items are asynchronously validated.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="items">A list with validated items.</param>
		/// <param name="results">A list with the result of the validate for each item.</param>
		///<param name="result">The result of validating items.</param>
		///<remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnValidateDaItems(
			uint executionContext,
			DaItem[] items,
			int[] results,
			int result)
		{
			try
			{
				if (ValidateDaItemsCompleted != null)
				{
					ValidateDaItemsCompleted(this, executionContext, items, results, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Da.Subscription.OnValidateItems",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when item attributes are asynchronously updated from the server.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="items">A list with the items whose attributes are to be updated.</param>
		/// <param name="whatAttributes">A list with all the item attributes to be updated.</param>
		/// <param name="results">A list with the result of the update for each item attribute.</param>
		/// <param name="result">The result of updating the item attributes from a server.</param>
		/// <remarks><note>
		///All the results should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnGetDaItemAttributesFromServer(
			uint executionContext,
			DaItem[] items,
			EnumObjectAttribute[] whatAttributes,
			int[] results,
			int result)
		{
			try
			{
				if (GetDaItemAttributesFromServerCompleted != null)
				{
					GetDaItemAttributesFromServerCompleted(this, executionContext, items, whatAttributes, results, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Da.Subscription.OnGetItemAttributesFromServer",
					exc.ToString());
			}
		}

		/// <summary>
		/// Called when item attributes are asynchronously updated to the server.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="items">A list with the items whose attributes are to be updated.</param>
		/// <param name="whatAttributes">A list with all the item attributes to be updated.</param>
		/// <param name="results">A list with the result of the update for each item attribute.</param>
		/// <param name="result">The result of updating the item attributes to a server.</param>
		/// <remarks><note>
		///All the results should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnSetDaItemAttributesToServer(
			uint executionContext,
			DaItem[] items,
			EnumObjectAttribute[] whatAttributes,
			int[] results,
			int result)
		{
			try
			{
				if (SetDaItemAttributesToServerCompleted != null)
				{
					SetDaItemAttributesToServerCompleted(this, executionContext, items, whatAttributes, results, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Da.Subscription.OnSetItemAttributesToServer",
					exc.ToString());
			}
		}

		//-
		#endregion

		#region // ISerializable
		/// <summary>
		///  Contructs a subscription object by de-serializing it from the stream.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/method[@name="DaSubscription"]/doc/*'
		/// />
		protected DaSubscription(SerializationInfo info, StreamingContext context)
		{
			lock (m_itemList.SyncRoot) // de-serializing (modifying content)
			{
				m_itemList = (Hashtable) info.GetValue("m_itemList", typeof (Hashtable));
			}
		}

		/// <summary>
		/// Serializes a subscription object into a stream.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaSubscription"]/method[@name="GetObjectData"]/doc/*'
		/// />
		[SecurityPermission(SecurityAction.Demand, SerializationFormatter = true)]
		public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			lock (m_itemList.SyncRoot) // serializing (enumerating)
			{
				info.AddValue("m_itemList", m_itemList);
			}
		}

		#endregion

		#region //Internal Properties
		//--------------------------

		internal object DaItemListSyncRoot
		{
			get { return m_attributesJanitor; }
		}

		//-
		#endregion
	}
}