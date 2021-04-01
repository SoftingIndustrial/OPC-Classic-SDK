using System;
using System.Collections;
using System.Runtime.InteropServices;
using Softing.OPCToolbox.OTB;

namespace Softing.OPCToolbox.Client
{

	#region // Public Delegates
	//-----------------------------

	/// <summary>
	///<para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	///<para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.AeSubscription.AeEventsReceived"/> event of a <see cref="AeSubscription"/>.</para>
	/// </summary>
	///<param name="anAeSubscription"> The <see cref="DaSubscription"/> object to whom the <see cref="Softing.OPCToolbox.Client.AeSubscription.AeEventsReceived"/> event belongs.</param>
	///<param name="isRefresh"> Specifies whether the events are received as the result of a refresh.</param>
	///<param name="lastRefresh"> Specifies whether the event is the last one from the list of events caused by a refresh.</param>
	///<param name="events"> A list with all the events received.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="AeEventsReceivedEventHandler"]/doc/*'
	/// />
	public delegate void AeEventsReceivedEventHandler(
		AeSubscription anAeSubscription, bool isRefresh, bool lastRefresh, AeEvent[] events);

	/// <summary>
	///<para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	///<para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.AeSubscription.RefreshAeConditionsCompleted"/> event of a <see cref="AeSubscription"/>.</para>
	/// </summary>
	///<param name="anAeSubscription"> The <see cref="DaSubscription"/> object to whom the <see cref="Softing.OPCToolbox.Client.AeSubscription.RefreshAeConditionsCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="result">The result of refreshing the conditions.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="RefreshAeConditionsEventHandler"]/doc/*'
	/// />
	public delegate void RefreshAeConditionsEventHandler(AeSubscription anAeSubscription, uint executionContext, int result
		);

	/// <summary>
	///<para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	///<para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.AeSubscription.CancelRefreshAeConditionsCompleted"/> event of a <see cref="AeSubscription"/>.</para>
	/// </summary>
	///<param name="anAeSubscription"> The <see cref="DaSubscription"/> object to whom the <see cref="Softing.OPCToolbox.Client.AeSubscription.CancelRefreshAeConditionsCompleted"/> event belongs.</param>
	/// <param name="executionContext">An identifier for an asynchronous call.</param>
	/// <param name="result">The result of cancelling the conditions' refresh.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="CancelRefreshAeConditionsEventHandler"]/doc/*'
	/// />
	public delegate void CancelRefreshAeConditionsEventHandler(
		AeSubscription anAeSubscription, uint executionContext, int result);

	/// <summary>
	///<para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	///<para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.AeSubscription.AeConditionsChanged"/> event of a <see cref="AeSubscription"/>.</para>
	/// </summary>
	///<param name="anAeSubscription"> The <see cref="DaSubscription"/> object to whom the <see cref="Softing.OPCToolbox.Client.AeSubscription.CancelRefreshAeConditionsCompleted"/> event belongs.</param>
	/// <param name="conditions">A list with the new values of the condition-related events.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="AeConditionsChangedEventHandler"]/doc/*'
	/// />
	public delegate void AeConditionsChangedEventHandler(AeSubscription anAeSubscription, AeCondition[] conditions);

	//-

	#endregion

	/// <summary>
	/// The <see cref="AeSubscription"/> objects are used to monitor the event sources in the server and inform the client
	/// of the events which have occured. They notify the clients about the events occurence based on a set of defined filters.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="AeSubscription"]/doc/*'
	/// />
	public class AeSubscription : ObjectSpaceElement
	{
		#region //Public Constructors
		//---------------------------

		/// <summary>
		/// Initializes a new instance of the <see cref="AeSubscription"/> class with
		/// the value indicated by an <see cref="AeSession"/> object to whom to be added.
		/// After creating it, the <b>AeSubscription</b> object is added to the session it is meant to belong.
		/// </summary>
		/// <param name="parentSession">The session to whom the subscription is to be added.</param>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/constructor[@name="AeSubscription"]/doc/*'
		/// />
		public AeSubscription(AeSession parentSession)
		{
			m_conditionList = new ArrayList();

			//sets the session to whom the subscription belongs
			m_session = parentSession;

			try
			{
				int res = (int) EnumResultCode.E_FAIL;

				res = parentSession.AddAeSubscription(this);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.Constructor",
						"The subscription could not be added to the session!");
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSubscription.Constructor",
					exc.ToString());
			}
		}

		//-
		#endregion

		#region //Public Events
		//-----------------------

		/// <summary>
		/// Occurs when the client is notified about the occurence of some events.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/event[@name="AeEventsReceived"]/doc/*'
		/// />
		public event AeEventsReceivedEventHandler AeEventsReceived;

		/// <summary>
		/// Occurs when refreshing the conditions of some condition-related events completes.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="RefreshAeConditions"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="RefreshAeConditions"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/event[@name="RefreshAeConditionsCompleted"]/doc/*'
		/// />
		public event RefreshAeConditionsEventHandler RefreshAeConditionsCompleted;

		/// <summary>
		/// Occurs when aborting the refreshing of a condition-related event's conditions completes.
		///<para><b>C#, Visual Basic, C++</b>This event is raised by asynchronously executing the <see cref="CancelRefreshAeConditions"/> method.</para>
		///<para><b>Unmanaged C++</b>The method is called after the method <see cref="CancelRefreshAeConditions"/> has been asynchronously executed.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/event[@name="CancelRefreshAeConditionsCompleted"]/doc/*'
		/// />
		public event CancelRefreshAeConditionsEventHandler CancelRefreshAeConditionsCompleted;

		/// <summary>
		/// Occurs when the client is notified about the changes of the condition state for some condition-related events.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/event[@name="AeConditionsChanged"]/doc/*'
		/// />
		public event AeConditionsChangedEventHandler AeConditionsChanged;

		//-
		#endregion

		#region //Private Attributes
		//-----------------------

		private readonly OTCObjectAttributes m_objectAttributes = new OTCObjectAttributes();
		private readonly object m_attributesJanitor = new object();
		//-
		#endregion

		#region //Protected Attributes
		//--------------------------

		/// <summary>
		/// The session to which the subscription belongs.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/field[@name="m_session"]/doc/*'
		/// />
		protected AeSession m_session = null;

		/// <summary>
		///  A list with all the active and/or not acknowledged condition-related events belonging to the subscription.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/field[@name="m_conditionList"]/doc/*'
		/// />
		internal ArrayList m_conditionList = new ArrayList();

		//-
		#endregion

		#region //Public Properties
		//--------------------------

		/// <summary>
		/// Gets or sets the <see cref="AeSession"/> object to whom the subscription belongs.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/property[@name="AeSession"]/doc/*'
		/// />
		public AeSession AeSession
		{
			get { return m_session; }
			set { m_session = value; }
		}

		/// <summary>
		/// Gets or sets the requested time interval(in ms) at which the events to be sent to the client. All events that occur before
		/// this time elapses, are buffered until the moment they are sent.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/property[@name="RequestedBufferTime"]/doc/*'
		/// />
		public uint RequestedBufferTime
		{
			get
			{
				uint requestedBufferTime = 0;
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
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_BUFFERTIME},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.RequestedBufferTime.Get",
										"Update AESUBSCRIPTION_BUFFERTIME attribute failed! Result: " + updateResult);
								} //	end if
							} //	end if
							int getAttrResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_BUFFERTIME,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrResult))
							{
								requestedBufferTime = objectAttributes.m_aeSubscriptionRequestedBufferTime;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSubscription.RequestedBufferTime.Get",
									"Get attribute AESUBSCRIPTION_BUFFERTIME from server failed! Result: " + getAttrResult);
							}
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.RequestedBufferTime.Get",
						exc.ToString());
				}
				return requestedBufferTime;
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
							m_objectAttributes.m_aeSubscriptionRequestedBufferTime = value;
							int setAttrResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_BUFFERTIME,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSubscription.RequestedBufferTime.Set",
									"Update attribute AESUBSCRIPTION_BUFFERTIME to server failed!Result: " + setAttrResult);
							}
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_BUFFERTIME},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.RequestedBufferTime.Set",
										"Set attribute AESUBSCRIPTION_BUFFERTIME to server failed!Result: " + updateResult);
								}
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSubscription.RequestedBufferTime.Set",
							"Set RequestedBufferTime failed! Subscription is not added to a session");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.RequestedBufferTime.Set",
						exc.ToString());
				} //	end catch
			} //	end set
		} //	end RequestedBufferTime

		/// <summary>
		/// Gets the actual time interval(in ms) at which the events are sent to the client. All events that occur before
		/// this time elapses, are buffered until the moment they are sent.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/property[@name="RevisedBufferTime"]/doc/*'
		/// />
		public uint RevisedBufferTime
		{
			get
			{
				uint revisedBufferTime = 0;
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
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_BUFFERTIME},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.RevisedBufferTime.Get",
										"Update attribute AESUBSCRIPTION_BUFFERTIME from server failed!Result: " + updateResult);
								} //	end if
							} //	end if
							int getAttrResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_BUFFERTIME,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrResult))
							{
								revisedBufferTime = objectAttributes.m_aeSubscriptionRevisedBufferTime;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSubscription.RevisedBufferTime.Get",
									"Get attribute AESUBSCRIPTION_BUFFERTIME from server failed! Result: " + getAttrResult);
							}
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.RevisedBufferTime.Get",
						exc.ToString());
				}
				return revisedBufferTime;
			}
		}

		/// <summary>
		/// Gets or sets the requested length of the event queue in the server. When the event queue is filled up with events arrived, a notification is
		/// sent to the client. This is done independently of whether the <see cref="RevisedBufferTime"/> has run out.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/property[@name="RequestedMaxSize"]/doc/*'
		/// />
		public uint RequestedMaxSize
		{
			get
			{
				uint requestedMaxSize = 0;
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
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_MAXSIZE},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.RequestedMaxSize.Get",
										"Update attribute AESUBSCRIPTION_MAXSIZE from server failed! Result: " + updateResult);
								}
							}
							int getAttrResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_MAXSIZE,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrResult))
							{
								requestedMaxSize = objectAttributes.m_aeSubscriptionRequestedMaxSize;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSubscription.RequestedMaxSize.Get",
									"Get attribute AESUBSCRIPTION_MAXSIZE from server failed! Result: " + getAttrResult);
							}
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.RequestedMaxSize.Get",
						exc.ToString());
				}
				return requestedMaxSize;
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
							m_objectAttributes.m_aeSubscriptionRequestedMaxSize = value;
							int setAttrResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_MAXSIZE,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSubscription.RequestedMaxSize.Set",
									"Set attribute AESUBSCRIPTION_MAXSIZE to server failed! Result: " + setAttrResult);
							}
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_MAXSIZE},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.RequestedMaxSize.Set",
										"Update attribute AESUBSCRIPTION_MAXSIZE to server failed! Result: " + updateResult);
								}
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSubscription.RequestedMaxSize.Set",
							"Set MaxSize failed! Subscription is not added to a session ");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.RequestedMaxSize.Set",
						exc.ToString());
				} //	end catch
			} //	end set
		} //	end RequestedMaxSize

		/// <summary>
		/// Gets the actual length of the event queue in the server. When the event queue is filled up with events arrived, a notification is
		/// sent to the client. This is done independently of whether the <see cref="RevisedBufferTime"/> has run out.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/property[@name="RevisedMaxSize"]/doc/*'
		/// />
		public uint RevisedMaxSize
		{
			get
			{
				uint revisedMaxSize = 0;
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
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_MAXSIZE},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.RevisedMaxSize.Get",
										"Update attribute AESUBSCRIPTION_MAXSIZE from server failed! Result:" + updateResult);
								}
							}
							int getAttrResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_MAXSIZE,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrResult))
							{
								revisedMaxSize = objectAttributes.m_aeSubscriptionRevisedMaxSize;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSubscription.RevisedMaxSize.Get",
									"Get attribute AESUBSCRIPTION_MAXSIZE from server failed! Result:" + getAttrResult);
							}
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.RevisedMaxSize.Get",
						exc.ToString());
				}
				return revisedMaxSize;
			} //	end get
		} //	end RevisedMaxSize

		/// <summary>
		/// Gets or sets the criteria of filtering events with low severity.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/property[@name="FilterSeverityLow"]/doc/*'
		/// />
		public uint FilterSeverityLow
		{
			get
			{
				uint filterSeverityLow = 1;
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
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.FilterSeverityLow.Get",
										"Update attribute AESUBSCRIPTION_FILTER failed!Result: " + updateResult);
								}
							}
							int getAttrResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrResult))
							{
								filterSeverityLow = objectAttributes.m_aeSubscriptionFilterSeverityLow;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSubscription.FilterSeverityLow.Get",
									"Get attribute AESUBSCRIPTION_FILTER from server failed!Result: " + getAttrResult);
							}
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.FilterSeverityLow.Get",
						exc.ToString());
				}
				return filterSeverityLow;
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
							m_objectAttributes.m_aeSubscriptionFilterSeverityLow = value;
							int setAttrResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AE.Subscription.FilterSeverityLow.Set",
									"Set attribute AESUBSCRIPTION_FILTER to server failed! Result: " + setAttrResult);
							}
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.FilterSeverityLow.Set",
										"Update attribute AESUBSCRIPTION_FILTER to server failed! Result: " + updateResult);
								}
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSubscription.FilterSeverityLow.Set",
							"Set FilterSeverityLow failed! Subscription is not added to a session ");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.FilterSeverityLow.Set",
						exc.ToString());
				} //	end catch
			}
		}

		/// <summary>
		/// Gets or sets the criteria of filtering events with high severity.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/property[@name="FilterSeverityHigh"]/doc/*'
		/// />
		public uint FilterSeverityHigh
		{
			get
			{
				uint filterSeverityHigh = 1000;
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
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.FilterSeverityHigh.Get",
										"Update attribute AESUBSCRIPTION_FILTER from server failed! Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								filterSeverityHigh = objectAttributes.m_aeSubscriptionFilterSeverityHigh;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSubscription.FilterSeverityHigh.Get",
									"Get attribute AESUBSCRIPTION_FILTER from server failed! Result: " + getAttrsResult);
							}
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.FilterSeverityHigh.Get",
						exc.ToString());
				}
				return filterSeverityHigh;
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
							m_objectAttributes.m_aeSubscriptionFilterSeverityHigh = value;
							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSubscription.FilterSeverityHigh.Set",
									"Set attribute AESUBSCRIPTION_FILTER to server failed! Result: " + setAttrsResult);
							}
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.FilterSeverityHigh.Set",
										"Update attribute AESUBSCRIPTION_FILTER to server failed! Result: " + updateResult);
								} //	end if
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSubscription.FilterSeverityHigh.Set",
							"Set FilterSeverityHigh failed! Subscription is not added to a session ");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.FilterSeverityHigh.Set",
						exc.ToString());
				} //	end catch
			}
		}

		/// <summary>
		/// Gets or sets the criteria of filtering events based on the values of their types(simple,condition related,tracking related).
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/property[@name="FilterEventTypes"]/doc/*'
		/// />
		public uint FilterEventTypes
		{
			get
			{
				uint eventType = 7;
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
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.FilterEventTypes.Get",
										"Update attribute AESUBSCRIPTION_FILTER from server failed! Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								eventType = objectAttributes.m_aeSubscriptionFilterEventTypes;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSubscription.FilterEventTypes.Get",
									"Get attribute AESUBSCRIPTION_FILTER from server failed! Result: " + getAttrsResult);
							}
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.FilterEventTypes.Get",
						exc.ToString());
				}
				return eventType;
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
							m_objectAttributes.m_aeSubscriptionFilterEventTypes = (uint) value;
							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSubscription.FilterEventTypes.Set",
									"Set attribute AESUBSCRIPTION_FILTER from server failed! Result: " + setAttrsResult);
							}
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.FilterEventTypes.Set",
										"Update attribute AESUBSCRIPTION_FILTER from server failed! Result: " + updateResult);
								} //	end if
							} //	end if
						} //	end lock
					} //	end if
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSubscription.FilterEventTypes.Set",
							"Set FilterEventTypes failed! Subscription is not added to a session");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.FilterEventTypes.Set",
						exc.ToString());
				} //	end catch
			}
		}

		/// <summary>
		/// Gets or sets the criteria of filtering events based on the value of the category they belong to.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/property[@name="FilterCategories"]/doc/*'
		/// />
		public uint[] FilterCategories
		{
			get
			{
				uint[] categories = new uint[0];
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
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.FilterCategories.Get",
										"Update attribute AESUBSCRIPTION_FILTER from server failed! Error message: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								int uintSize = Marshal.SizeOf(typeof (uint));
								categories = new uint[objectAttributes.m_aeSubscriptionFilterCountCategrories];
								if (objectAttributes.m_aeSubscriptionFilterCategrories != IntPtr.Zero)
								{
									for (int i = 0; i < objectAttributes.m_aeSubscriptionFilterCountCategrories; i++)
									{
										uint myData =
											(uint)
											Marshal.PtrToStructure(
												OTBFunctions.GetIntPtrOffset(objectAttributes.m_aeSubscriptionFilterCategrories, uintSize*i), typeof (uint));
										categories[i] = myData;
									}
									OTBFunctions.OTFreeMemory(objectAttributes.m_aeSubscriptionFilterCategrories);
								}
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSubscription.FilterCategories.Get",
									"Get attribute AESUBSCRIPTION_FILTER from server failed! Result: " + getAttrsResult);
							}
						} //	end lock
					}
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AE.Subscription.FilterCategories.Get",
						exc.ToString());
				}
				return categories;
			}
			set
			{
				try
				{
					int[] categories = new int[value.Length];
					OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
					for (int j = 0; j < value.Length; j++)
					{
						categories[j] = (int) value[j];
					}
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						lock (m_attributesJanitor)
						{
							objectAttributes.m_aeSubscriptionFilterCountCategrories = (uint) categories.Length;
							if (categories != null)
							{
								int memorySize = categories.Length*ValueQT.VARIANT_SIZE;
								objectAttributes.m_aeSubscriptionFilterCategrories = Marshal.AllocCoTaskMem(memorySize);
								Marshal.GetNativeVariantForObject(null, objectAttributes.m_aeSubscriptionFilterCategrories);
								Marshal.Copy(categories, 0, objectAttributes.m_aeSubscriptionFilterCategrories, categories.Length);
							}
							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER,
								objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"AeSubscription.FilterCategories.Set",
									"Set attribute AESUBSCRIPTION_FILTER to server failed! Result: " + setAttrsResult);
							}
							OTBFunctions.OTVariantClear(objectAttributes.m_aeSubscriptionFilterCategrories);
							Marshal.FreeCoTaskMem(objectAttributes.m_aeSubscriptionFilterCategrories);
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.FilterCategories.Set",
										"Update attribute AESUBSCRIPTION_FILTER to server failed! Result: " + updateResult);
								}
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSubscription.FilterCategories.Set",
							"Set FilterCategories failed! Subscription is not added to a session ");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AE.Subscription.FilterCategories.Set",
						exc.ToString());
				} //	end catch
			} //	end set
		} //	end FilterCategories

		/// <summary>
		/// Gets or sets the criteria of filtering events based on the values of the areas in which the events occur.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/property[@name="FilterAreas"]/doc/*'
		/// />
		public string[] FilterAreas
		{
			get
			{
				string[] areas = new string[0];
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
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.FilterAreas.Get",
										"Update attribute AESUBSCRIPTION_FILTER from server failed! Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								int count = (int) objectAttributes.m_aeSubscriptionFilterCountAreas;
								areas = new string[count];
								if (objectAttributes.m_aeSubscriptionFilterAreas != IntPtr.Zero)
								{
									for (int j = 0; j < count; j++)
									{
										IntPtr myData =
											(IntPtr)
											Marshal.PtrToStructure(
												OTBFunctions.GetIntPtrOffset(objectAttributes.m_aeSubscriptionFilterAreas, Marshal.SizeOf(typeof (IntPtr))*j),
												typeof (IntPtr));
										areas[j] = Marshal.PtrToStringUni(myData);
										OTBFunctions.OTFreeMemory(myData);
									} //	end for

									OTBFunctions.OTFreeMemory(objectAttributes.m_aeSubscriptionFilterAreas);
								}
								else
								{
									if (ResultCode.FAILED(getAttrsResult))
									{
										Application.Instance.Trace(
											EnumTraceLevel.ERR,
											EnumTraceGroup.CLIENT,
											"AeSubscription.FilterAreas.Get",
											"Get attribute AESUBSCRIPTION_FILTER from server failed! Result: " + getAttrsResult);
									} //	end if
								} //	end if...else
							} //	end if
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.FilterAreas.Get",
						exc.ToString());
				} //	end catch
				return areas;
			} //	end get
			set
			{
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						int result = -1;
						lock (m_attributesJanitor)
						{
							string[] areas = value;
							if (areas != null)
							{
								objectAttributes.m_aeSubscriptionFilterCountAreas = (uint) areas.Length;

								IntPtr[] stringPointers = new IntPtr[areas.Length];
								int size = areas.Length*Marshal.SizeOf(typeof (IntPtr));
								objectAttributes.m_aeSubscriptionFilterAreas = Marshal.AllocCoTaskMem(size);

								for (int index = 0; index < areas.Length; index++)
								{
									stringPointers[index] = Marshal.StringToCoTaskMemAuto(areas[index]);
									Marshal.WriteIntPtr(objectAttributes.m_aeSubscriptionFilterAreas, index*Marshal.SizeOf(typeof (IntPtr)),
									                    stringPointers[index]);
								} //	end for

								int setAttrsResult = OTBFunctions.OTCSetAttributes(
									this.Handle,
									(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER,
									objectAttributes);

								if (ResultCode.FAILED(setAttrsResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.FilterAreas.Set",
										"Set attribute AESUBSCRIPTION_FILTER to server failed! Result: " + setAttrsResult);
								}

								for (int i = 0; i < areas.Length; i++)
								{
									Marshal.FreeCoTaskMem(stringPointers[i]);
								}

								Marshal.FreeCoTaskMem(objectAttributes.m_aeSubscriptionFilterAreas);

								if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
								{
									int updateResult = OTBFunctions.OTCUpdateAttributes(
										this.Handle,
										0,
										1,
										new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER},
										new int[] {result},
										ref options);

									if (ResultCode.FAILED(updateResult))
									{
										Application.Instance.Trace(
											EnumTraceLevel.ERR,
											EnumTraceGroup.CLIENT,
											"AeSubscription.FilterAreas.Set",
											"Update attribute AESUBSCRIPTION_FILTER to server failed! Result: " + updateResult);
									} //	end if
								} //	end if
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSubscription.FilterAreas.Set",
							"Set FilterAreas failed! Subscription is not added to a session ");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.FilterAreas.Set",
						exc.ToString());
				} //	end catch
			} //	end set
		} //	end FilterAreas

		/// <summary>
		/// Gets or sets the criteria of filtering events based on the values of the sources that produce the events.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/property[@name="FilterSources"]/doc/*'
		/// />
		public string[] FilterSources
		{
			get
			{
				string[] sources = new string[0];
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
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.FilterSources.Get",
										"Get attribute AESUBSCRIPTION_FILTER from server failed! Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								int count = (int) objectAttributes.m_aeSubscriptionFilterCountSources;
								sources = new string[count];
								if (objectAttributes.m_aeSubscriptionFilterSources != IntPtr.Zero)
								{
									for (int j = 0; j < count; j++)
									{
										IntPtr myData =
											(IntPtr)
											Marshal.PtrToStructure(
												OTBFunctions.GetIntPtrOffset(objectAttributes.m_aeSubscriptionFilterSources,
												                             (Marshal.SizeOf(typeof (IntPtr))*j)), typeof (IntPtr));
										sources[j] = Marshal.PtrToStringUni(myData);
										OTBFunctions.OTFreeMemory(myData);
									}
								}
								else
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.FilterSources.Get",
										"Get attribute AESUBSCRIPTION_FILTER from server failed! Result: " + getAttrsResult);
								}

								OTBFunctions.OTFreeMemory(objectAttributes.m_aeSubscriptionFilterSources);
							} //	end if
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.FilterSources.Get",
						exc.ToString());
				} //	end catch
				return sources;
			} //	end get
			set
			{
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						int result = -1;
						lock (m_attributesJanitor)
						{
							string[] sources = value;
							if (sources != null)
							{
								objectAttributes.m_aeSubscriptionFilterCountSources = (uint) sources.Length;

								IntPtr[] stringPointers = new IntPtr[sources.Length];
								int size = sources.Length*Marshal.SizeOf(typeof (IntPtr));
								objectAttributes.m_aeSubscriptionFilterSources = Marshal.AllocCoTaskMem(size);

								for (int index = 0; index < sources.Length; index++)
								{
									stringPointers[index] = Marshal.StringToCoTaskMemAuto(sources[index]);
									Marshal.WriteIntPtr(objectAttributes.m_aeSubscriptionFilterSources, index*Marshal.SizeOf(typeof (IntPtr)),
									                    stringPointers[index]);
								} //	end for

								int setAttrsResult = OTBFunctions.OTCSetAttributes(
									this.Handle,
									(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER,
									objectAttributes);

								if (ResultCode.FAILED(setAttrsResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.FilterSources.Set",
										"Set attribute AESUBSCRIPTION_FILTER to server failed! Result: " + setAttrsResult);
								} //	end if
								for (int i = 0; i < sources.Length; i++)
								{
									Marshal.FreeCoTaskMem(stringPointers[i]);
								} //	end for
								Marshal.FreeCoTaskMem(objectAttributes.m_aeSubscriptionFilterSources);
								if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
								{
									int updateResult = OTBFunctions.OTCUpdateAttributes(
										this.Handle,
										0,
										1,
										new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_FILTER},
										new int[] {result},
										ref options);
									if (ResultCode.FAILED(updateResult))
									{
										Application.Instance.Trace(
											EnumTraceLevel.ERR,
											EnumTraceGroup.CLIENT,
											"AeSubscription.FilterSources.Set",
											"Update attribute AESUBSCRIPTION_FILTER to server failed! Result: " + updateResult);
									} //	end if
								} //	end if
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSubscription.FilterSources.Set",
							"Set FilterSources failed! Subscription is not added to a session ");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.FilterSources.Set",
						exc.ToString());
				} //	end catch
			} //	end set
		} //	end FilterSources

		/// <summary>
		/// Gets or sets a list of optional attributes to be sent with the event notification.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/property[@name="ReturnedAttributes"]/doc/*'
		/// />
		public AeReturnedAttributes[] ReturnedAttributes
		{
			get
			{
				AeReturnedAttributes[] returnedAttributes = new AeReturnedAttributes[0];
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				OTCExecutionOptions options = new OTCExecutionOptions();
				try
				{
					if (this.Handle != 0)
					{
						lock (m_attributesJanitor)
						{
							int result = -1;
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateAttributes(
									this.Handle,
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_RETURNED_ATTRIBUTES},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.ReturnedAttributes.Get",
										"Update attribute AESUBSCRIPTION_RETURNED_ATTRIBUTES from server failed! Result: " + updateResult);
								} //	end if
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.AESUBSCRIPTION_RETURNED_ATTRIBUTES,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								int count = (int) objectAttributes.m_aeSubscriptionCountReturnedAttributes;
								returnedAttributes = new AeReturnedAttributes[count];
								if (objectAttributes.m_aeSubscriptionReturnedAttributes != IntPtr.Zero)
								{
									for (int j = 0; j < count; j++)
									{
										OTCAEReturnedAttributesData myData =
											(OTCAEReturnedAttributesData)
											Marshal.PtrToStructure(
												OTBFunctions.GetIntPtrOffset(objectAttributes.m_aeSubscriptionReturnedAttributes,
												                             Marshal.SizeOf(typeof (OTCAEReturnedAttributesData))*j),
												typeof (OTCAEReturnedAttributesData));
										if (myData != null)
										{
											returnedAttributes[j] = new AeReturnedAttributes();
											returnedAttributes[j].CategoryId = myData.m_categoryID;
											returnedAttributes[j].AttributeIds = new uint[myData.m_countAttributeIDs];

											for (int jj = 0; jj < myData.m_countAttributeIDs; jj++)
											{
												uint anAttribut =
													(uint)
													Marshal.PtrToStructure(
														OTBFunctions.GetIntPtrOffset(myData.m_attributeIDs, Marshal.SizeOf(typeof (uint))*jj), typeof (uint));
												returnedAttributes[j].AttributeIds[jj] = anAttribut;
											}

											OTBFunctions.OTFreeMemory(myData.m_attributeIDs);
										}
									} //	end for

									OTBFunctions.OTFreeMemory(objectAttributes.m_aeSubscriptionReturnedAttributes);
								} //	end if
								else
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.ReturnedAttributes.Get",
										"Get attribute AESUBSCRIPTION_RETURNED_ATTRIBUTES from server failed! Result: " + getAttrsResult);
								}
							} //	end if
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.ReturnedAttributes.Get",
						exc.ToString());
				} //	end catch
				return returnedAttributes;
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
							AeReturnedAttributes[] returnedAttributes = value;
							if (returnedAttributes != null)
							{
								m_objectAttributes.m_aeSubscriptionCountReturnedAttributes = (uint) returnedAttributes.Length;

								OTCAEReturnedAttributesData returnedAttrs = new OTCAEReturnedAttributesData();
								int memory = returnedAttributes.Length*Marshal.SizeOf(returnedAttrs);
								m_objectAttributes.m_aeSubscriptionReturnedAttributes = Marshal.AllocCoTaskMem(memory);

								IntPtr currentPtr = m_objectAttributes.m_aeSubscriptionReturnedAttributes;
								OTCAEReturnedAttributesData attributes = new OTCAEReturnedAttributesData();
								for (int i = 0; i < returnedAttributes.Length; i++)
								{
									attributes.m_categoryID = returnedAttributes[i].CategoryId;
									attributes.m_countAttributeIDs = (uint) returnedAttributes[i].AttributeIds.Length;
									int memorySize = returnedAttributes[i].AttributeIds.Length*Marshal.SizeOf(typeof (uint));
									attributes.m_attributeIDs = Marshal.AllocCoTaskMem(memorySize);
									int[] intAttributesReturned = new int[returnedAttributes[i].AttributeIds.Length];
									for (int ii = 0; ii < returnedAttributes[i].AttributeIds.Length; ii++)
									{
										intAttributesReturned[ii] = (int) returnedAttributes[i].AttributeIds[ii];
									}
									Marshal.Copy(intAttributesReturned, 0, attributes.m_attributeIDs, (int) attributes.m_countAttributeIDs);
									Marshal.StructureToPtr(attributes, currentPtr, false);
									currentPtr = new IntPtr(currentPtr.ToInt32() + Marshal.SizeOf(typeof (OTCAEReturnedAttributesData)));
								}
								int setAttrsResult = OTBFunctions.OTCSetAttributes(
									this.Handle,
									(uint) EnumObjectAttribute.AESUBSCRIPTION_RETURNED_ATTRIBUTES,
									m_objectAttributes);
								if (ResultCode.FAILED(setAttrsResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"AeSubscription.ReturnedAttributes.Set",
										"Set attribute AESUBSCRIPTION_RETURNED_ATTRIBUTES to server failed! Result: " + setAttrsResult);
								} //	end if

								Marshal.FreeCoTaskMem(attributes.m_attributeIDs);
								Marshal.FreeCoTaskMem(m_objectAttributes.m_aeSubscriptionReturnedAttributes);

								if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
								{
									int updateResult = OTBFunctions.OTCUpdateAttributes(
										this.Handle,
										0,
										1,
										new uint[] {(uint) EnumObjectAttribute.AESUBSCRIPTION_RETURNED_ATTRIBUTES},
										new int[] {result},
										ref options);
									if (ResultCode.FAILED(updateResult))
									{
										Application.Instance.Trace(
											EnumTraceLevel.ERR,
											EnumTraceGroup.CLIENT,
											"AeSubscription.ReturnedAttributes.Set",
											"Update attribute AESUBSCRIPTION_RETURNED_ATTRIBUTES to server failed! Result: " + updateResult);
									} //	end if
								} //	end if
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"AeSubscription.ReturnedAttributes.Set",
							"Set ReturnedAttributes failed! Subscription is not added to a session");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.ReturnedAttributes.Set",
						exc.ToString());
				} //	end catch
			} //	end set
		} //	end ReturnedAttributes

		/// <summary>
		/// Gets the list with all active and/or not acknowledged condition-related events. This list is maintained by interpreting
		/// the events received from the server.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/property[@name="AeConditionList"]/doc/*'
		/// />
		public AeCondition[] AeConditionList
		{
			get
			{
				lock (m_conditionList.SyncRoot) // enumerating
				{
					AeCondition[] conditions = new AeCondition[m_conditionList.Count];
					for (int i = 0; i < m_conditionList.Count; i++)
					{
						conditions[i] = (AeCondition) m_conditionList[i];
					}
					return conditions;
				}
			}
		}

		//-
		#endregion

		#region //Public Methods
		//----------------------

		/// <summary>
		/// Forces a refresh for all active conditions and inactive,unacknowledged conditions whose event notifications match
		/// the filter of the event subscription.
		/// </summary>
		/// <param name="executionOptions">Specifies the modality of execution for refreshing conditions.</param>
		/// <returns>The result of refreshing conditions.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/method[@name="RefreshAeConditions"]/doc/*'
		/// />
		public virtual int RefreshAeConditions(ExecutionOptions executionOptions)
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

				res = OTBFunctions.OTCRefreshAEConditions(this.Handle, 0, ref options);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.RefreshAeConditions",
						"Refreshing Ae conditions failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSubscription.RefreshAeConditions",
					exc.ToString());
			}
			return res;
		}

		/// <summary>
		/// Cancels a refresh in progress for the event subscription.
		/// </summary>
		/// <param name="executionOptions">Specifies the modality of execution for cancelling the conditions refresh.</param>
		/// <returns>The result of cancelling the refresh.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeSubscription"]/method[@name="CancelRefreshAeConditions"]/doc/*'
		/// />
		public virtual int CancelRefreshAeConditions(ExecutionOptions executionOptions)
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

				res = OTBFunctions.OTCRefreshAEConditions(this.Handle, 1, ref options);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"AeSubscription.CancelRefreshAeConditions",
						"Cancelling refreshing Ae conditions failed! Result: " + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSubscription.CancelRefreshAeConditions",
					exc.ToString());
			}
			return res;
		}

		//-
		#endregion

		#region //Internal Methods
		//----------------------

		/// <summary>
		/// Occurs when receiving event notifications from the server.
		/// </summary>
		/// <param name="isRefresh">Specifies whether the events are received as the result of a refresh.</param>
		/// <param name="lastRefresh">Specifies whether the event is the last one from the list of events caused by a refresh.</param>
		/// <param name="events">A list with all the events received.</param>
		internal void OnAeEventsReceived(bool isRefresh, bool lastRefresh, AeEvent[] events)
		{
			try
			{
				if (AeEventsReceived != null)
				{
					AeEventsReceived(this, isRefresh, lastRefresh, events);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSubscription.OnEventsReceived",
					exc.ToString());
			}
		} // end OnEventsReceived

		/// <summary>
		/// Occurs when the <see cref="AeConditionList"/> changes.
		/// </summary>
		/// <param name="conditions">A list with the new values of the condition-related events.</param>
		internal void OnAeConditionsChanged(AeCondition[] conditions)
		{
			try
			{
				if (AeConditionsChanged != null)
				{
					AeConditionsChanged(this, conditions);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSubscription.OnAeConditionsChanged",
					exc.ToString());
			}
		} //	end OnConditionsChanged

		/// <summary>
		/// Occurs when the conditions are asynchronously refreshed.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="result">The result of refreshing the conditions.</param>
		/// <remarks><note>
		/// The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		/// </note></remarks>remarks>
		internal void OnRefreshAeConditions(uint executionContext, int result)
		{
			try
			{
				if (RefreshAeConditionsCompleted != null)
				{
					RefreshAeConditionsCompleted(this, executionContext, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSubscription.OnRefreshAeConditions",
					exc.ToString());
			}
		}

		/// <summary>
		/// Occurs when refreshing conditions is asynchronously cancelled.
		/// </summary>
		/// <param name="executionContext">An identifier for an asynchronous call.</param>
		/// <param name="result">The result of cancelling </param>
		/// ///<remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		/// </note></remarks>remarks>
		internal void OnCancelRefreshAeConditions(uint executionContext, int result)
		{
			try
			{
				if (CancelRefreshAeConditionsCompleted != null)
				{
					CancelRefreshAeConditionsCompleted(this, executionContext, result);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"AeSubscription.OnCancelRefreshAeConditions",
					exc.ToString());
			}
		}

		//-
		#endregion

		#region //Internal Properties
		//----------------------

		internal object AeConditionListSyncRoot
		{
			get { return m_conditionList.SyncRoot; }
		}

		//-
		#endregion
	}

	/// <summary>
	/// Represents the attributes to be supplied along with the event,when an event notification is sent from a server.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="AeReturnedAttributes"]/doc/*'
	/// />
	public class AeReturnedAttributes
	{
		#region //Public Constructors
		//---------------------------

		/// <summary>
		/// Initializes a new instance of the <see cref="AeReturnedAttributes"/> class.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeReturnedAttributes"]/constructor[@name="AeReturnedAttributes"]/doc/*'
		/// />
		public AeReturnedAttributes()
		{
		}

		//-
		#endregion

		#region //Protected Members
		//-----------------------

		/// <summary>
		/// An identifier for a category of events.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeReturnedAttributes"]/field[@name="m_categoryId"]/doc/*'
		/// />
		protected uint m_categoryId = 0;

		/// <summary>
		/// A list of identifiers for the attributes supplied along with the event, when an event notification is sent from a server.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeReturnedAttributes"]/field[@name="m_attributesIds"]/doc/*'
		/// />
		protected uint[] m_attributesIds = null;

		//-
		#endregion

		#region //Public Properties
		//-------------------------

		/// <summary>
		/// Gets or sets the identifier for a category of events.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeReturnedAttributes"]/property[@name="CategoryId"]/doc/*'
		/// />
		public uint CategoryId
		{
			get { return m_categoryId; }
			set { m_categoryId = value; }
		}

		/// <summary>
		/// Gets or sets a list of identifiers for the attributes supplied along with the event, when an event notification is sent from a server.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeReturnedAttributes"]/property[@name="AttributeIds"]/doc/*'
		/// />
		public uint[] AttributeIds
		{
			get { return m_attributesIds; }
			set { m_attributesIds = value; }
		}

		//-
		#endregion
	}
}