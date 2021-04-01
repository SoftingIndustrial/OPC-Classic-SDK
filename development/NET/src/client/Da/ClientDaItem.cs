using System;
using System.Runtime.InteropServices;
using System.ComponentModel.Design.Serialization;
using System.ComponentModel;
using System.Runtime.Serialization;
using System.Security.Permissions;
using System.Collections;
using Softing.OPCToolbox.OTB;

namespace Softing.OPCToolbox.Client
{

	#region //	Public Delegates
	/// <summary>
	/// <para><b>Unmanaged C++</b>This is not available for Unmanaged C++.</para>
	/// <para><b>C#, Visual Basic, C++</b>Represents the method that will handle the <see cref="Softing.OPCToolbox.Client.DaItem.ValueChanged"/>
	/// event of an <see cref="DaItem"/>.</para>
	/// </summary>
	/// <param name="aDaItem">The <see cref="DaItem"/> object to whom the <see cref="Softing.OPCToolbox.Client.DaItem.ValueChanged"/> event belongs.</param>
	/// <param name="aValue">The new item value.</param>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//delegate[@name="ValueChangedEventHandler"]/doc/*'
	/// />
	public delegate void ValueChangedEventHandler(DaItem aDaItem, ValueQT aValue);

	#endregion

	/// <summary>
	/// <para>Represents an OPC Item.</para>
	/// <para>An OPC Server must create an item for each process variable or property it wants to supply.</para>
	///<para>The status of an item can be either active or inactive, which means that the item can be taken in account or not
	/// when automatically reading the process values.</para>
	/// <para>Item objects represent leaves and nodes of the address space.</para>
	///</summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="DaItem"]/doc/*'
	/// />
	[Serializable]
	public class DaItem : ObjectSpaceElement, ISerializable
	{
		#region //	Public Constructors
		//--------------------------

		/// <summary>
		/// Initializes a new instance of the <see cref="DaItem">DaItem</see> class  class with
		/// the value indicated by an item identifier and a <see cref="DaSubscription"/> object to whom to be added.
		/// After creating it, the <b>DaItem</b> object is added to the subscription it is meant to belong.
		/// </summary>
		/// <param name="itemId">An unique identifier for the item in the server's address space.</param>
		/// <param name="parentSubscription">The subscription to whom the item is to be added.</param>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/constructor[@name="DaItem"]/doc/*'
		/// />
		public DaItem(
			string itemId,
			DaSubscription parentSubscription)
		{
			this.m_valueQT = new ValueQT(
				null,
				EnumQuality.BAD_WAITING_FOR_INITIAL_DATA,
				new DateTime(1601, 01, 01, 0, 0, 0, 0));

			m_subscription = parentSubscription;

			try
			{
				int res = (int) EnumResultCode.E_FAIL;

				res = parentSubscription.AddDaItem(itemId, this);

				if (ResultCode.FAILED(res))
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaItem.Constructor",
						"The item could not be added to the subscription! Result:" + res);
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaItem.Constructor",
					exc.ToString());
			}
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="DaItem">DaItem</see> class.
		/// </summary>
		internal DaItem()
		{
		}

		//-
		#endregion

		#region //	Public Events
		//---------------------

		/// <summary>
		/// Occurs when the <b>ValueQT</b> of an <see cref="DaItem"/> added to the <see cref="Softing.OPCToolbox.Client.Control.DataControl"/> changes.
		/// </summary>
		public event System.EventHandler ValueQTChanged;

		/// <summary>
		/// Occurs when the string representation of a <see cref="DaItem"/>'s value changes. The event occurs only in case the <b>DaItem</b> was chosen using the <b>DataControl</b>.
		/// </summary>
		public event System.EventHandler ValueToStringChanged;

		/// <summary>
		/// Occurs when the value of an <see cref="DaItem"/> changes.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/event[@name="ValueChanged"]/doc/*'
		/// />
		public event ValueChangedEventHandler ValueChanged;

		//-
		#endregion

		#region //	Private Members
		//-----------------------

		private readonly OTCObjectAttributes m_objectAttributes = new OTCObjectAttributes();
		private static readonly object s_attributesJanitor = new object();

		//-
		#endregion

		#region //	Protected Members
		//-----------------------

		/// <summary>
		/// The <see cref="DaSubscription"/> object to which the item belongs.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/field[@name="m_subscription"]/doc/*'
		/// />
		protected DaSubscription m_subscription = null;

		/// <summary>
		/// The value and the quality of a <b>DaItem</b> at a moment in time.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/field[@name="m_valueQT"]/doc/*'
		/// />
		protected ValueQT m_valueQT = null;

		/// <summary>
		/// A janitor for the <b>DaItem</b>'s ValueQT.
		/// </summary>
		protected readonly object m_valueQTJanitor = new object();

		//-
		#endregion

		#region //	Public Properties
		//----------------------

		/// <summary>
		/// Gets or sets the item's <see cref="ValueQT"/>.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/property[@name="ValueQT"]/doc/*'
		/// />
		public ValueQT ValueQT
		{
			get { return this.m_valueQT; }
			set
			{
				try
				{
					lock (m_valueQTJanitor)
					{
						this.m_valueQT = value;
					}
					if (ValueQTChanged != null)
					{
						ValueQTChanged(this, EventArgs.Empty);
					}
					if (ValueToStringChanged != null)
					{
						ValueToStringChanged(this, EventArgs.Empty);
					}
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "Da.Item.ValueQT.Set", exc.ToString());
				}
			}
		}

		/// <summary>
		/// <para><b>C#, Visual Basic, C++</b>Gets the string representation for the item's value.</para>
		/// <para><b>Unmanaged C++</b> This property is not available for Unmanaged C++.</para>
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/property[@name="ValueToString"]/doc/*'
		/// />
		public string ValueToString
		{
			get
			{
				try
				{
					if ((this.ValueQT.Data != null) && (this.ValueQT.Quality == EnumQuality.GOOD))
					{
						ValueData vt = new ValueData(ValueQT.Data);
						return vt.ToString();
					}
					else
					{
						if (this.ValueQT.Data == null)
						{
							return "";
						}
						if (this.ValueQT.Quality != EnumQuality.GOOD)
						{
							return Application.Instance.BadQualityValueString;
						}
					}
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.CLIENT, "Da.Item.ValueToString", exc.ToString());
				}
				return "";
			}
		}

		/// <summary>
		/// Gets or sets the <see cref="DaSubscription"/> object to which the item belongs.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/property[@name="DaSubscription"]/doc/*'
		/// />
		public DaSubscription DaSubscription
		{
			get { return m_subscription; }
			set { m_subscription = value; }
		}

		/// <summary>
		/// Gets or sets a unique identifier for the item in the server's address space.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/property[@name="Id"]/doc/*'
		/// />
		[DesignerSerializationVisibility(System.ComponentModel.DesignerSerializationVisibility.Visible)]
		public string Id
		{
			get
			{
				string ID = string.Empty;
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						lock (s_attributesJanitor)
						{
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateDAItemAttributes(
									this.DaSubscription.Handle,
									1,
									new uint[] {this.Handle},
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.DAITEM_ID},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaItem.Id.Get",
										"Update attribute DAITEM_ID from server failed!Result: " + updateResult);
								} //	end if
							} //	end if
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DAITEM_ID,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								ID = Marshal.PtrToStringUni(objectAttributes.m_daItemID);
								OTBFunctions.OTFreeMemory(objectAttributes.m_daItemID);
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaItem.Id.Get",
									"Get attribute DAITEM_ID from server failed!Result: " + getAttrsResult);
							} //	end else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Item.ID.Get",
						exc.ToString());
				}
				return ID;
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
							lock (s_attributesJanitor)
							{
								m_objectAttributes.m_daItemID = Marshal.StringToCoTaskMemUni(value);
								int setAttrsResult = OTBFunctions.OTCSetAttributes(
									this.Handle,
									(uint) EnumObjectAttribute.DAITEM_ID,
									m_objectAttributes);
								if (ResultCode.FAILED(setAttrsResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"Da.Session.LocaleId.Set",
										"Set attribute DAITEM_ID failed! Result: " + setAttrsResult);
								} //	end if
								if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
								{
									int updateResult = OTBFunctions.OTCUpdateDAItemAttributes(
										this.DaSubscription.Handle,
										1,
										new uint[] {this.Handle},
										0,
										1,
										new uint[] {(uint) EnumObjectAttribute.DAITEM_ID},
										new int[] {result},
										ref options);
									if (ResultCode.FAILED(updateResult))
									{
										Application.Instance.Trace(
											EnumTraceLevel.ERR,
											EnumTraceGroup.CLIENT,
											"DaItem.Id.Set",
											"Set attribute DAITEM_ID failed! Result: " + updateResult);
									}
								} //	end if
								Marshal.FreeCoTaskMem(m_objectAttributes.m_daItemID);
							} //	end lock
						} //	end if
					} //	end if
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaItem.Id.Set",
							"Set Id failed! Item is not added to a subscription.");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Item.ID.Set",
						exc.ToString());
				} //	end try...catch
			} //	end set
		}

		/// <summary>
		/// Gets or sets an identifier for the communication path between the Data Acces Server and the item.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/property[@name="Path"]/doc/*'
		/// />
		public string Path
		{
			get
			{
				string path = string.Empty;
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						lock (s_attributesJanitor)
						{
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateDAItemAttributes(
									this.DaSubscription.Handle,
									1,
									new uint[] {this.Handle},
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.DAITEM_PATH},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaItem.Path.Get",
										"Update attribute DAITEM_PATH from server failed!Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DAITEM_PATH,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								path = Marshal.PtrToStringUni(objectAttributes.m_daItemPath);
								OTBFunctions.OTFreeMemory(objectAttributes.m_daItemPath);
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaItem.Path.Get",
									"Get attribute DAITEM_PATH from server failed!Result: " + getAttrsResult);
							} //	end else
						} //	en lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Item.Path.Get",
						exc.ToString());
				}
				return path;
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
							lock (s_attributesJanitor)
							{
								m_objectAttributes.m_daItemPath = Marshal.StringToCoTaskMemUni(value);
								int setAttrsResult = OTBFunctions.OTCSetAttributes(
									this.Handle,
									(uint) EnumObjectAttribute.DAITEM_PATH,
									m_objectAttributes);
								if (ResultCode.FAILED(setAttrsResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaItem.Path.Set",
										"Set attribute DAITEM_PATH failed! Result: " + setAttrsResult);
								} //	end if
								if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
								{
									int updateResult = OTBFunctions.OTCUpdateDAItemAttributes(
										this.DaSubscription.Handle,
										1,
										new uint[] {this.Handle},
										0,
										1,
										new uint[] {(uint) EnumObjectAttribute.DAITEM_PATH},
										new int[] {result},
										ref options);
									if (ResultCode.FAILED(updateResult))
									{
										Application.Instance.Trace(
											EnumTraceLevel.ERR,
											EnumTraceGroup.CLIENT,
											"DaItem.Path.Set",
											"Set attribute DAITEM_PATH failed! Result: " + updateResult);
									}
								} //	end if
								Marshal.FreeCoTaskMem(m_objectAttributes.m_daItemPath);
							} //	end lock
						} //	end if
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaItem.Path.Set",
							"Set Path failed! Item is not added to a subscription.");
					} //	end if..else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Item.Path.Set",
						exc.ToString());
				} //	end try...catch
			} //	end set
		}

		/// <summary>
		/// Gets the data type for the data the item contains.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/property[@name="NativeDatatype"]/doc/*'
		/// />
		public System.Type NativeDatatype
		{
			get
			{
				ushort nativeDataType = 0;
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						lock (s_attributesJanitor)
						{
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateDAItemAttributes(
									this.DaSubscription.Handle,
									1,
									new uint[] {this.Handle},
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.DAITEM_NATIVE_DATATYPE},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaItem.NativeDatatype.Get",
										"Update attribute DAITEM_NATIVE_DATATYPE from server failed!Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DAITEM_NATIVE_DATATYPE,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								nativeDataType = objectAttributes.m_daItemNativeDatatype;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaItem.NativeDatatype.Get",
									"Get attribute DAITEM_NATIVE_DATATYPE from server failed!Result: " + getAttrsResult);
							} //	end if...else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaItem.NativeDatatype.Get",
						exc.ToString());
				}
				return ValueQT.GetSysType(nativeDataType);
			} //	end get
		} //	end NativeDatatype

		/// <summary>
		/// Gets or sets the type a client wants the item's data to be converted to.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/property[@name="RequestedDatatype"]/doc/*'
		/// />
		public System.Type RequestedDatatype
		{
			get
			{
				ushort requestedDataType = 0;
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						lock (s_attributesJanitor)
						{
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateDAItemAttributes(
									this.DaSubscription.Handle,
									1,
									new uint[] {this.Handle},
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.DAITEM_REQUESTED_DATATYPE},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaItem.RequestedDatatype.Get",
										"Update attribute DAITEM_REQUESTED_DATATYPE from server failed!Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DAITEM_REQUESTED_DATATYPE,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								requestedDataType = objectAttributes.m_daItemRequestedDatatype;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaItem.RequestedDatatype.Get",
									"Get attribute DAITEM_REQUESTED_DATATYPE from server failed!Result: " + getAttrsResult);
							} //	end else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaItem.RequestedDatatype.Get",
						exc.ToString());
				}
				return ValueQT.GetSysType(requestedDataType);
			} //	end get
			set
			{
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						lock (s_attributesJanitor)
						{
							m_objectAttributes.m_daItemRequestedDatatype = (ushort) ValueQT.GetVartype(value);
							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DAITEM_REQUESTED_DATATYPE,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaItem.RequestedDatatype.Set",
									"Set attribute DAITEM_REQUESTED_DATATYPE failed! Result: " + setAttrsResult);
							} //	end if
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateDAItemAttributes(
									this.DaSubscription.Handle,
									1,
									new uint[] {this.Handle},
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.DAITEM_REQUESTED_DATATYPE},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaItem.RequestedDatatype.Set",
										"Set attribute DAITEM_REQUESTED_DATATYPE failed! Result: " + updateResult);
								} //	end if
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaItem.RequestedDatatype.Set",
							"Set RequestedDatatype failed! Item is not added to a subscription.");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Item.RequestedDatatype.Set",
						exc.ToString());
				}
			} //	end set
		}

		/// <summary>
		/// Gets the access rights to the item in the address space.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/property[@name="AccessRights"]/doc/*'
		/// />
		public EnumAccessRights AccessRights
		{
			get
			{
				EnumAccessRights accessRights = new EnumAccessRights();
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						lock (s_attributesJanitor)
						{
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateDAItemAttributes(
									this.DaSubscription.Handle,
									1,
									new uint[] {this.Handle},
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.DAITEM_ACCESSRIGHTS},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaItem.AccessRights.Get",
										"Update attribute DAITEM_ACCESSRIGHTS from server failed!Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DAITEM_ACCESSRIGHTS,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								accessRights = (EnumAccessRights) objectAttributes.m_daItemAccessRights;
							}
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaItem.AccessRights.Get",
									"Get attribute DAITEM_ACCESSRIGHTS from server failed!Result: " + getAttrsResult);
							} //	end if...else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaItem.AccessRights.Get",
						exc.ToString());
				}
				return accessRights;
			} //	end get
		} //	end AccessRights

		/// <summary>
		/// Gets or sets the deadband to use when acquiring data from a source.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/property[@name="Deadband"]/doc/*'
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
						int result = -1;
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						lock (s_attributesJanitor)
						{
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateDAItemAttributes(
									this.DaSubscription.Handle,
									1,
									new uint[] {this.Handle},
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.DAITEM_DEADBAND},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaItem.Deadband.Get",
										"Update attribute DAITEM_DEADBAND from server failed!Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DAITEM_DEADBAND,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								deadband = objectAttributes.m_daItemDeadband;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaItem.Deadband.Get",
									"Get attribute DAITEM_DEADBAND from server failed!Result: " + getAttrsResult);
							} //	end else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Item.Deadband.Get",
						exc.ToString());
				} //	end try...catch
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
						lock (s_attributesJanitor)
						{
							m_objectAttributes.m_daItemDeadband = value;
							int setAttrsResult = OTBFunctions.OTCSetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DAITEM_DEADBAND,
								m_objectAttributes);
							if (ResultCode.FAILED(setAttrsResult))
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"Da.Session.LocaleId.Set",
									"Set attribute DAITEM_DEADBAND failed! Result: " + setAttrsResult);
							} //	end if
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateDAItemAttributes(
									this.DaSubscription.Handle,
									1,
									new uint[] {this.Handle},
									0,
									1,
									new uint[] {(uint) EnumObjectAttribute.DAITEM_DEADBAND},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"Da.Session.LocaleId.Set",
										"Set attribute DAITEM_DEADBAND failed! Result: " + updateResult);
								}
							} //	end if
						} //	end lock
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaItem.Deadband.Set",
							"Set Deadband failed! Item is not added to a subscription ");
					} //	end if...else
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaItem.Deadband.Set",
						exc.ToString());
				}
			} //	end set
		} //	end Deadband

		/// <summary>
		/// Gets the Engineering Unit Type of the item.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/property[@name="EUType"]/doc/*'
		/// />
		public EnumEUType EUType
		{
			get
			{
				EnumEUType type = new EnumEUType();
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
						lock (s_attributesJanitor)
						{
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateDAItemAttributes(
									this.DaSubscription.Handle,
									1,
									new uint[] {this.Handle},
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.DAITEM_EUTYPE},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaItem.EUType.Get",
										"Update attribute DAITEM_EUTYPE from server failed!Result: " + updateResult);
								}
							}
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DAITEM_EUTYPE,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								type = (EnumEUType) objectAttributes.m_daItemEuType;
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaItem.EUType.Get",
									"Get attribute DAITEM_EUTYPE from server failed!Result: " + getAttrsResult);
							} //	end else
						} //	end lock
					} //	end if
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaItem.EUType.Get",
						exc.ToString());
				}
				return type;
			} //	end get
		} //	end EUType

		/// <summary>
		/// Gets the Engineering Unit Information of the item.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/property[@name="EUInfo"]/doc/*'
		/// />
		public object EUInfo
		{
			get
			{
				object info = new object();
				OTCObjectAttributes objectAttributes = new OTCObjectAttributes();
				try
				{
					if (this.Handle != 0)
					{
						OTCExecutionOptions options = new OTCExecutionOptions();
						int result = -1;
						lock (s_attributesJanitor)
						{
							if (Application.Instance.UpdateAttributesBehavior == EnumUpdateAttributesBehavior.IMPLICIT_UPDATE)
							{
								int updateResult = OTBFunctions.OTCUpdateDAItemAttributes(
									this.DaSubscription.Handle,
									1,
									new uint[] {this.Handle},
									1,
									1,
									new uint[] {(uint) EnumObjectAttribute.DAITEM_EUINFO},
									new int[] {result},
									ref options);
								if (ResultCode.FAILED(updateResult))
								{
									Application.Instance.Trace(
										EnumTraceLevel.ERR,
										EnumTraceGroup.CLIENT,
										"DaItem.EUInfo.Get",
										"Update attribute DAITEM_EUINFO from server failed!Result: " + updateResult);
								}
							}
							objectAttributes.m_daItemEuInfo = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
							int getAttrsResult = OTBFunctions.OTCGetAttributes(
								this.Handle,
								(uint) EnumObjectAttribute.DAITEM_EUINFO,
								objectAttributes);
							if (ResultCode.SUCCEEDED(getAttrsResult))
							{
								info = Marshal.GetObjectForNativeVariant(objectAttributes.m_daItemEuInfo);
							} //	end if
							else
							{
								Application.Instance.Trace(
									EnumTraceLevel.ERR,
									EnumTraceGroup.CLIENT,
									"DaItem.EUInfo.Get",
									"Get attribute DAITEM_EUINFO from server failed!Result: " + getAttrsResult);
							} //	end else
							OTBFunctions.OTVariantClear(objectAttributes.m_daItemEuInfo);
							Marshal.FreeCoTaskMem(objectAttributes.m_daItemEuInfo);
						} //	end lock
					} //	end if
				}

				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"Da.Item.EUInfo.Get",
						exc.ToString());
				}
				return info;
			} //	end get
		} //	end EUInfo


		//-
		#endregion

		#region //	Public methods
		//----------------------

		/// <summary>
		/// Reads the item's current value and the quality of the value  in a certain moment of time.
		/// </summary>
		/// <param name="maxAge">The oldest (in milliseconds) acceptable cached value when reading the item.</param>
		/// <param name="itemValue">The read value together with its quality and the time it was read.</param>
		/// <param name="result">The result of reading the item. </param>
		/// <param name="executionOptions">Specifies the modality of execution for reading the item.</param>
		/// <returns>The result of reading the item.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/method[@name="Read"]/doc/*'
		/// />
		public virtual int Read(
			uint maxAge,
			out ValueQT itemValue,
			out int result,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			int[] itemResult = new int[] {-1};
			result = -1;
			itemValue = null;

			if (m_subscription == null)
			{
				res = (int) EnumResultCode.E_INVALIDARG;
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaItem.Read",
					" The item must belong to a subscription");
			}
			else
			{
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

					OTValueData[] aValue = new OTValueData[1];
					aValue[0] = new OTValueData();
					aValue[0].m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
					Marshal.GetNativeVariantForObject(null, aValue[0].m_value);

					res = OTBFunctions.OTCRead(
						m_subscription.Handle,
						1,
						new uint[] {this.Handle},
						new string[] {this.Id},
						new string[] {this.Path},
						maxAge,
						aValue,
						itemResult,
						ref options);

					if (options.m_executionType == (byte) EnumExecutionType.SYNCHRONOUS)
					{
						if (ResultCode.SUCCEEDED(res))
						{
							result = itemResult[0];
							itemValue = new ValueQT(ref aValue[0]);
						}
						else
						{
							Application.Instance.Trace(
								EnumTraceLevel.ERR,
								EnumTraceGroup.CLIENT,
								"DaItem.Read",
								" Reading item failed! Result: " + res);
						}
					}

					OTBFunctions.OTVariantClear(aValue[0].m_value);
					Marshal.FreeCoTaskMem(aValue[0].m_value);
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaItem.Read",
						exc.ToString());
				}
			}
			return res;
		}

		/// <summary>
		/// Writes the value, quality and timestamp for a <see cref="DaItem"/> object.
		/// </summary>
		/// <param name="itemValue">A value with its quality to be written to the item at a moment of time.</param>
		/// <param name="result">The result of the write. </param>
		/// <param name="executionOptions">Specifies the modality of execution for writing a value.</param>
		/// <returns>The result of writing a value.</returns>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/method[@name="Write"]/doc/*'
		/// />
		public virtual int Write(
			ValueQT itemValue,
			out int result,
			ExecutionOptions executionOptions)
		{
			int res = (int) EnumResultCode.E_FAIL;
			result = -1;
			if (m_subscription == null)
			{
				res = (int) EnumResultCode.E_INVALIDARG;
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"DaItem.Write",
					" The item must belong to a subscription");
			}
			else
			{
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

					int[] intData = new int[1] {-1};
					OTValueData[] inValues = new OTValueData[1];

					inValues[0] = new OTValueData();
					inValues[0].m_quality = (ushort)itemValue.Quality;

					if (itemValue.TimeStamp == DateTime.MinValue)
					{
						inValues[0].m_timestamp = new OTDateTime();
					}
					else
					{
						inValues[0].m_timestamp = new OTDateTime(itemValue.TimeStamp);
					}

					inValues[0].m_value = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
					Marshal.GetNativeVariantForObject(itemValue.Data, inValues[0].m_value);

					res = OTBFunctions.OTCWrite(
						m_subscription.Handle,
						1,
						new uint[] {this.Handle},
						new string[] {this.Id},
						new string[] {this.Path},
						inValues,
						intData,
						ref options);

					// write succedeed
					if (ResultCode.SUCCEEDED(res))
					{
						result = intData[0];
					}
					else
					{
						Application.Instance.Trace(
							EnumTraceLevel.ERR,
							EnumTraceGroup.CLIENT,
							"DaItem.Write",
							"Writing item failed! Result: " + res);
					}

					OTBFunctions.OTVariantClear(inValues[0].m_value);
					Marshal.FreeCoTaskMem(inValues[0].m_value);
				}
				catch (Exception exc)
				{
					Application.Instance.Trace(
						EnumTraceLevel.ERR,
						EnumTraceGroup.CLIENT,
						"DaItem.Write",
						exc.ToString());
				}
			}
			return res;
		}

		/// <summary>
		/// Updates the item's attributes from the server.
		/// </summary>
		/// <param name="whatAttributes">A list with all the item attributes to be updated.</param>
		/// <param name="results">A list with the result of the update for each item attribute.</param>
		/// <param name="executionOptions">Specifies the modality of execution for updating item attributes from the server.</param>
		/// <returns>The result of updating attributes from the server.</returns>
		public override int GetAttributesFromServer(
			EnumObjectAttribute[] whatAttributes,
			out int[] results,
			ExecutionOptions executionOptions)
		{
			return m_subscription.GetDaItemAttributesFromServer(
				new DaItem[] {this},
				whatAttributes,
				out results,
				executionOptions);
		} //	end GetAttributesFromServer


		/// <summary>
		/// Updates the item's attributes to the server.
		/// </summary>
		/// <param name="whatAttributes">A list with all the item attributes to be updated.</param>
		/// <param name="results">A list with the result of the update for each item attribute.</param>
		/// <param name="executionOptions">Specifies the modality of execution for updating item attributes to the server.</param>
		/// <returns>The result of updating attributes from the server.</returns>
		public override int SetAttributesToServer(
			EnumObjectAttribute[] whatAttributes,
			out int[] results,
			ExecutionOptions executionOptions)
		{
			return m_subscription.SetDaItemAttributesToServer(
				new DaItem[] {this},
				whatAttributes,
				out results,
				executionOptions);
		} //	end SetAttributesToServer


		//-
		#endregion

		#region //	Internal methods
		//----------------------

		/// <summary>
		/// Called when the <see cref="DaItem">Item</see>'s value changes.
		/// </summary>
		/// <param name="aValue">The new item value.</param>
		/// <param name="result">The result of the value change.</param>
		/// <remarks><note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note></remarks>
		internal void OnValueChange(
			ValueQT aValue,
			int result)
		{
			ValueQT newValue = null;
			try
			{
				if (ResultCode.SUCCEEDED(result))
				{
					m_valueQT = new ValueQT(aValue.Data, aValue.Quality, aValue.TimeStamp);
					if (ValueChanged != null)
					{
						ValueChanged(this, aValue);
					}
				}
				else
				{
					//the value cannot be trusted
					newValue = new ValueQT(null, EnumQuality.BAD, aValue.TimeStamp);
					m_valueQT = newValue;
					if (ValueChanged != null)
					{
						ValueChanged(this, newValue);
					}
				}
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.CLIENT,
					"Da.Item.OnValueChange",
					exc.Message);
			}
		}

		//-
		#endregion

		#region //	ISerializable
		/// <summary>
		///  Contructs an item object by de-serializing it from the stream.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/method[@name="DaItem"]/doc/*'
		/// />
		public DaItem(SerializationInfo info, StreamingContext context)
		{
		}

		/// <summary>
		/// Serializes an item object into a stream.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaItem"]/method[@name="GetObjectData"]/doc/*'
		/// />
		/// <param name="info"></param>
		/// <param name="context"></param>
		[SecurityPermission(SecurityAction.Demand, SerializationFormatter = true)]
		public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
		{
		}

		#endregion
	}
}