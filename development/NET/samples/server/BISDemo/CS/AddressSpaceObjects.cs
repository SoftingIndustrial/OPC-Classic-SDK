using System;
using System.Collections;
using System.Xml;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace BISDemo
{

	public class BISCreator : Creator
	{

		#region    //	Public Override Methods
		//---------------------------------

		public override DaTransaction CreateTransaction(
			EnumTransactionType aTransactionType,
			DaRequest[] aRequests,
			uint aSessionKey)
		{
			return (DaTransaction)new BISTransaction(aTransactionType, aRequests, aSessionKey);
		}   //	end ConsoleTransaction

		public override AeAddressSpaceRoot CreateAeAddressSpaceRoot()
		{
			return new BISAeRoot();
		}   //	end CreateAeAddressSpaceRoot

		public override DaAddressSpaceRoot CreateDaAddressSpaceRoot()
		{
			return new BISDaRoot();
		}   //	end CreateAeAddressSpaceRoot

		//-
		#endregion

	}   //	end class BISCreator

	public class BISDaRoot : DaAddressSpaceRoot
	{

		#region    //	Public Override Methods
		//---------------------------------

		public uint findAddressSpaceElementData(string anAddress, out AddressSpaceElement anElement)
		{
			anElement = null;
			Hashtable elements = Hashtable.Synchronized(this.m_elements);
			foreach (object element in elements.Values)
			{
				DaBaseElement daElement = element as DaBaseElement;
				if (daElement.ItemId == anAddress)
				{
					anElement = daElement;
					break;
				}   //	end if
			}   //	end foreach

			return (uint)EnumResultCode.S_OK;
		}

		public override int QueryAddressSpaceElementData(string anElementId, out AddressSpaceElement anElement)
		{

			anElement = null;
			Hashtable elements = Hashtable.Synchronized(this.m_elements);
			foreach (object element in elements.Values)
			{
				DaAddressSpaceElement daElement = element as DaAddressSpaceElement;
				if (daElement.ItemId == anElementId)
				{
					anElement = daElement;
					return (int)EnumResultCode.S_OK;
				}   //	end if
			}   //	end foreach

			return (int)EnumResultCode.E_NOTIMPL;
		}

		public override int QueryAddressSpaceElementChildren(string anElementId, ArrayList aChildrenList)
		{
			aChildrenList = null;
			Hashtable elements = Hashtable.Synchronized(this.m_elements);
			foreach (object element in elements.Values)
			{
				DaAddressSpaceElement daElement = element as DaAddressSpaceElement;
				if (daElement.ItemId == anElementId)
				{
					aChildrenList.InsertRange(0, daElement.GetChildren());
					return (int)EnumResultCode.S_OK;
				}   //	end if
			}   //	end foreach

			return (int)EnumResultCode.E_NOTIMPL;
		}   //	end QueryAddressSpaceElementChildren

		public override int QueryAddressSpaceElementProperties(string anElementID, out ArrayList aPropertyList)
		{
			aPropertyList = null;
			Hashtable elements = Hashtable.Synchronized(this.m_elements);
			foreach (object element in elements.Values)
			{
				DaAddressSpaceElement daElement = element as DaAddressSpaceElement;
				if (daElement.ItemId == anElementID)
				{
					daElement.QueryProperties(out aPropertyList);
					return (int)EnumResultCode.S_OK;
				}   //	end if
			}   //	end foreach

			return (int)EnumResultCode.E_NOTIMPL;
		} // end QueryAddressSpaceElementProperties

		//-
		#endregion

	}   //	end class BISDaRoot


	public class BISAeRoot : AeAddressSpaceRoot
	{

		#region    //	Public Override Methods
		//---------------------------------

		// TODO Implement this methods for string based address space using the DA address space elements

		/// <summary>
		/// Returns the corresponding namespace-element
		/// </summary>
		/// <param name="ElementID">Namespace path of the element</param>
		/// <param name="Element">returned element</param>
		/// <returns>
		/// EnumResultCode.E_NOTIMPL°Must be overridden
		/// </returns>
		public override int QueryAddressSpaceElementData(string anElementId, out AddressSpaceElement anElement)
		{
			anElement = null;
			BISDaRoot daRoot = Application.Instance.DaAddressSpaceRoot as BISDaRoot;
			AddressSpaceElement anAddressSpaceElement = null;
			daRoot.findAddressSpaceElementData(anElementId, out anAddressSpaceElement);
			if (anAddressSpaceElement != null)
			{
				DaBaseElement daElement = anAddressSpaceElement as DaBaseElement;
				if (daElement != null)
				{
					anElement = new AeAddressSpaceElement();
					anElement.Name = daElement.Name;
					anElement.HasChildren = daElement.HasChildren;

					return (int)EnumResultCode.S_OK;
				} // end if
			} // end if .. else

			return (int)EnumResultCode.S_FALSE;
		} // end QueryAddressSpaceElementData


		public override int QueryAddressSpaceElementChildren(string anElementId, ArrayList aChildrenList)
		{
			BISDaRoot daRoot = Application.Instance.DaAddressSpaceRoot as BISDaRoot;
			ArrayList elements = null;
			if (anElementId == "")
			{
				elements = ArrayList.Synchronized(daRoot.GetChildren());
			}
			else
			{
				AddressSpaceElement anElement = null;
				daRoot.findAddressSpaceElementData(anElementId, out anElement);
				if (anElement != null)
				{
					DaBaseElement daElement = anElement as DaBaseElement;
					if (daElement != null)
					{
						elements = ArrayList.Synchronized(daElement.GetChildren());
					} // end if
				} // end if
			} // end if .. else

			foreach (object element in elements)
			{
				DaBaseElement daElement = element as DaBaseElement;
				if (daElement != null && daElement.Name != "CMD")
				{
					AeAddressSpaceElement anElement = new AeAddressSpaceElement();
					anElement.Name = daElement.Name;
					anElement.HasChildren = daElement.HasChildren;
					aChildrenList.Add(anElement);
				}   //	end if
			} // end foreach

			return (int)EnumResultCode.S_OK;
		} // end QueryAddressSpaceElementChildren

		//-
		#endregion

	}   //	end BISAeRoot

	/// <summary>
	/// DataPointTypeProperty
	/// </summary>
	public class DataPointTypeProperty : DaProperty
	{

		#region //	Protected constants
		//-----------------------------

		/// <summary>
		///	the ID for the DataPoint type property
		/// </summary>		
		protected const int DATAPOINT_TYPE_PROPERTY_ID = 5001;

		protected const string DATAPOINT_TYPE_PROPERTY_NAME = "DataPointType";

		//-
		#endregion

		#region //	Protected Attributes
		//------------------------------

		protected uint m_dataPointTypeValue = 0;

		//-
		#endregion

		#region //	Public Constructors
		//-----------------------------

		public DataPointTypeProperty(uint aDataPointTypeId) :
			this(aDataPointTypeId, DATAPOINT_TYPE_PROPERTY_NAME)
		{
		}   //	end ctr

		public DataPointTypeProperty(
			uint aDataPointTypeId,
			string aName)
			: this(aDataPointTypeId, aName, aName)
		{
		}   //	end ctr

		public DataPointTypeProperty(
			uint aDataPointTypeId,
			string aName,
			string aDescription)
		{

			System.Diagnostics.Debug.Assert(aDataPointTypeId >= DATAPOINT_TYPE_PROPERTY_ID, "The aDataPointType is not valid");
			this.Id = DATAPOINT_TYPE_PROPERTY_ID;
			this.Name = aName;
			this.ItemId = DATAPOINT_TYPE_PROPERTY_ID.ToString();
			this.Description = aDescription;
			this.Datatype = typeof(uint);
			this.AccessRights = EnumAccessRights.READABLE;
			this.m_dataPointTypeValue = aDataPointTypeId;

		}   //	end ctr

		//- 
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		/// HandleRequest
		/// </summary>		
		public override void HandleReadRequest(DaRequest aRequest)
		{

			System.Diagnostics.Debug.Assert(aRequest.PropertyId == this.Id,
				"HandleRequest called with illegal request");

			//	Meant to be obverriden, just return failed
			aRequest.Result = EnumResultCode.S_OK;
			aRequest.Value = new ValueQT(this.m_dataPointTypeValue, EnumQuality.GOOD, DateTime.Now);
			aRequest.Complete();

		}   //	end HandleReadRequest

		//-
		#endregion

	}   //	end DataPointTypeProperty

	/// <summary>
	/// CommandProperty
	/// </summary>
	public class CommandProperty : DaProperty
	{

		#region //	ProtectedConstants
		//----------------------------

		protected string XML_CMD_NAMESPACE = "file:///S3K/Proxyverwalter";

		//-
		#endregion

		#region //	Protected Attributes
		//------------------------------

		protected readonly XmlDocument m_xmlDoc = new XmlDocument();
		protected readonly XmlNode m_xmlCommandNode = null;

		//-
		#endregion

		#region  //	Public Constructors
		//-----------------------------
		/// <summary>
		/// Constructor
		/// </summary>
		public CommandProperty(
			uint aDataPointId,
			string aName)
		{

			this.Id = (int)aDataPointId;
			this.ItemId = aDataPointId.ToString();
			this.Name = aName;
			this.Description = aName;
			this.Datatype = typeof(string);
			this.AccessRights = EnumAccessRights.READABLE;

			XmlDeclaration declaration = m_xmlDoc.CreateXmlDeclaration("1.0", System.Text.Encoding.UTF8.WebName, null);
			m_xmlCommandNode = m_xmlDoc.CreateNode(XmlNodeType.Element, "nsPV:Commands", XML_CMD_NAMESPACE);
			m_xmlDoc.AppendChild(m_xmlCommandNode);

		}   //	end ctr

		//- 
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		/// HandleReadRequest
		/// </summary>
		public override void HandleReadRequest(DaRequest aRequest)
		{

			System.Diagnostics.Debug.Assert(aRequest.PropertyId == this.Id,
				"HandleRequest called with illegal request");
			Application.Instance.Trace(EnumTraceLevel.INF, EnumTraceGroup.OPCSERVER, "ComandProperty::HandleReadRequest", (string)this.CommandValue);
			aRequest.Result = EnumResultCode.S_OK;
			aRequest.Value = new ValueQT(this.CommandValue, EnumQuality.GOOD, DateTime.Now);
			aRequest.Complete();

		}   //	end HandleReadRequest

		/// <summary>
		/// AddCommand
		/// </summary>
		public void AddCommand(string aName, string aDescription, string anAddress)
		{

			XmlNode innerCommand = this.m_xmlDoc.CreateNode(XmlNodeType.Element, "nsPV:Command", XML_CMD_NAMESPACE);

			XmlAttribute attribute = this.m_xmlDoc.CreateAttribute("Name");
			attribute.Value = aName;
			innerCommand.Attributes.Append(attribute);

			attribute = this.m_xmlDoc.CreateAttribute("Description");
			attribute.Value = aDescription;
			innerCommand.Attributes.Append(attribute);

			attribute = this.m_xmlDoc.CreateAttribute("Address");
			attribute.Value = anAddress;
			innerCommand.Attributes.Append(attribute);

			this.m_xmlCommandNode.AppendChild(innerCommand);

		}   //	end AddCommand

		/// <summary>
		/// AddCommand
		/// </summary>
		public void AddCommand(string aName, string aDescription)
		{
			this.AddCommand(aName, aDescription, string.Empty);
		}   //	end AddCommand

		//-
		#endregion

		#region //	Protected Properties
		//------------------------------

		/// <summary>
		/// CommandValue
		/// </summary>
		protected string CommandValue
		{
			get
			{
				return m_xmlDoc.InnerXml;
			}   //	end get
		}   //	end CommandValue

		//-
		#endregion

	}   //	end CommandProperty

	/// <summary>
	/// Base class for the events used in the application
	/// </summary>
	public class BaseEvent : AeEvent
	{

		#region //	Public Constructors
		//-----------------------------

		/// <summary>
		/// DeviceEvent
		/// </summary>
		public BaseEvent(
			string aSource,
			uint aCategoryId)
			: base(EnumEventType.SIMPLE, aSource, aCategoryId)
		{

		}   //	end ctr

		//-
		#endregion

	}   //	end BaseEvent

	/// <summary>
	/// DataEvent
	/// </summary>
	public class DataEvent : BaseEvent
	{

		#region  //	Protected Class Attributes
		//------------------------------------

		protected static AeCategory CATEGORY_DATA = new AeCategory(EnumEventType.SIMPLE, 1, "Data");

		//-
		#endregion

		#region //	Public Class Methods
		//------------------------------

		/// <summary>
		/// BuildCategory
		/// </summary>
		public static void BuildCategory()
		{

			Application.Instance.AddAeCategory(CATEGORY_DATA);
			CATEGORY_DATA.AddAttribute(5550, "CV", typeof(uint));
			CATEGORY_DATA.AddAttribute(5551, "DATA", typeof(int));
			CATEGORY_DATA.AddAttribute(5552, "ADDRESS", typeof(string));
			CATEGORY_DATA.AddAttribute(5553, "TEXT", typeof(string));

		}   //	end BuildEventCategory

		//-
		#endregion

		#region //	Public Constructors
		//-----------------------------

		/// <summary>
		/// DeviceEvent
		/// </summary>
		public DataEvent(
			string aSource,
			object aCvAttribute,
			int aDataAttribute,
			string anAddressAttribute,
			string aTextAttribute)
			: base(aSource, CATEGORY_DATA.Id)
		{

			this.AttributeValueList.Add(aCvAttribute);
			this.AttributeValueList.Add(aDataAttribute);
			this.AttributeValueList.Add(anAddressAttribute);
			this.AttributeValueList.Add(aTextAttribute);

		}   //	end ctr

		//-
		#endregion

	}   //	end class DeviceEvent


	/// <summary>
	/// DeviceEvent
	/// </summary>
	public class DeviceEvent : BaseEvent
	{

		#region    //	Protected Class Attributes
		//------------------------------------

		protected static AeCategory CATEGORY_DEVICE = new AeCategory(EnumEventType.SIMPLE, 2, "Device");

		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		/// BuildCategory
		/// </summary>
		public static void BuildCategory()
		{
			Application.Instance.AddAeCategory(CATEGORY_DEVICE);
			CATEGORY_DEVICE.AddAttribute(5550, "CV", typeof(uint));
		}   //	end BuildEventCategory

		//-
		#endregion

		#region //	Public Constructors
		//-----------------------------

		/// <summary>
		/// DeviceEvent
		/// </summary>
		public DeviceEvent(
			string aSource,
			object aCvAttribute)
			: base(aSource, CATEGORY_DEVICE.Id)
		{

			this.AttributeValueList.Add(aCvAttribute);

		}   //	end ctr

		//-
		#endregion

	}   //	end class DeviceEvent


	/// <summary>
	/// BaseAeElemenet
	/// </summary>
	public class BaseAeElemenet : AeAddressSpaceElement
	{

		#region // Public constructors
		//----------------------------

		public BaseAeElemenet(
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle) :
			base(aName, anUserData, anObjectHandle, aParentHandle)
		{

		}   //	end ctr

		public BaseAeElemenet(
			string aName,
			bool hasChildren)
		{

			this.m_name = aName;
			this.m_hasChildren = hasChildren;

		}   //	end ctr

		//-
		#endregion

		#region //	Public Methods
		//------------------------

		public override int QueryConditions(
			string aSourcePath,
			out string[] aConditionNames)
		{

			aConditionNames = null;
			return (int)EnumResultCode.S_OK;
		}   //	end QueryConditions

		//-
		#endregion

	}   //	end class BaseAeElemenet


	/// <summary>
	/// DaBaseElement
	/// </summary>
	public class DaBaseElement : DaAddressSpaceElement
	{

		#region //	Public Constructors
		//-----------------------------

		public DaBaseElement(
			string anItemID,
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle)
			: base(anItemID, aName, anUserData, anObjectHandle, aParentHandle)
		{
		}   //	end ctr

		public DaBaseElement()
		{

		}   //	end ctr

		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		/// Get elements property value data
		/// </summary>
		public virtual void HandleReadRequest(DaRequest aRequest)
		{

			if (aRequest.PropertyId != 0)
			{

				DaProperty daProperty = m_properties[aRequest.PropertyId] as DaProperty;
				if (daProperty != null)
				{
					daProperty.HandleReadRequest(aRequest);
				}
				else
				{
					aRequest.Result = EnumResultCode.E_FAIL;
					aRequest.Value = null;
					aRequest.Complete();
				}   //	end if ... else
			}
			else
			{
				HandleItemReadRequest(aRequest);
			}   //	end if ... else

		}   //	end HandleReadRequest

		/// <summary>
		/// Handles a write request
		/// </summary>
		public virtual void HandleWriteRequest(DaRequest aRequest)
		{

			if (aRequest.PropertyId != 0)
			{

				DaProperty daProperty = m_properties[aRequest.PropertyId] as DaProperty;
				if (daProperty != null)
				{
					daProperty.HandleWriteRequest(aRequest);
				}
				else
				{
					aRequest.Result = EnumResultCode.E_FAIL;
					aRequest.Complete();
				}   //	end if ... else
			}
			else
			{
				HandleItemWriteRequest(aRequest);
			}   //	end if ... else

		}   //	end HandleWriteRequest

		public override int QueryProperties(out ArrayList aPropertyList)
		{

			if (m_properties.Count > 0)
			{
				aPropertyList = new ArrayList();
				aPropertyList.AddRange(m_properties.Values);
			}
			else
			{
				aPropertyList = null;
			}   //	end if ... else

			return (int)EnumResultCode.S_OK;

		}   //	end QueryProperties

		public virtual uint AddProperty(DaProperty aProperty)
		{

			m_properties.Add(aProperty.Id, aProperty);
			return (uint)EnumResultCode.S_OK;

		}   //	end AddProperty

		public virtual void Simulate()
		{

			try
			{
				if (this.HasChildren)
				{

					ArrayList children = this.GetChildren();
					foreach (object element in children)
					{
						DaBaseElement daElement = element as DaBaseElement;
						if (daElement != null)
						{
							daElement.Simulate();
						}   //	end if
					}   //	end 
				}   //	end if

				m_simulationOn = !m_simulationOn;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.WRN, EnumTraceGroup.OPCSERVER, "DaBaseElement.Simulate", "Exception" + e.ToString());
			}   //	end try ... catch
		}   //	end if

		/// <summary>
		/// ExecuteCommand
		/// </summary>
		public virtual int ExecuteCommand(string anAddress, string aCommand, string aDescription)
		{
			return (int)EnumResultCode.E_FAIL;
		}   //	end ExecuteCommand

		//-
		#endregion

		#region //	Protected Methods
		//---------------------------

		/// <summary>
		/// HandleItemReadRequest
		/// </summary>
		protected virtual void HandleItemReadRequest(DaRequest aRequest)
		{
			ValueQT cacheValue = null;
			this.GetCacheValue(ref cacheValue);
			aRequest.Value = cacheValue;
			aRequest.Result = EnumResultCode.S_OK;
			aRequest.Complete();
		}   //	end HandleItemReadRequest

		/// <summary>
		/// HandleItemWriteRequest
		/// </summary>		
		protected virtual void HandleItemWriteRequest(DaRequest aRequest)
		{

			aRequest.Result = EnumResultCode.E_BADRIGHTS;
			aRequest.Complete();

		}   //	end HandleItemWriteRequest

		//-
		#endregion

		#region    //	Private Attributes
		//----------------------------

		/// <summary>
		/// the properties list - the hash should be (int)Id
		/// </summary>
		private Hashtable m_properties = new Hashtable();

		/// <summary>
		/// 
		/// </summary>
		protected static bool m_simulationOn = true;

		//-
		#endregion

	}   //	end class DaBaseElement

	/// <summary>
	/// DataPointElement
	/// </summary>
	public class DataPointElement : DaBaseElement
	{

		#region //	Public Constructors
		//-----------------------------


		/// <summary>
		/// DataPointElement
		/// </summary>
		public DataPointElement(
			string aName,
			uint aDataTypeId,
			string aDescription) :
			base()
		{

			this.Name = aName;
			this.HasChildren = false;
			this.IoMode = EnumIoMode.POLL;
			this.AccessRights = EnumAccessRights.READWRITEABLE;
			this.Datatype = typeof(uint);
			this.AddProperty(new DataPointTypeProperty(aDataTypeId, "DataPointType", "DataPointType"));
			ValueQT value = new ValueQT(aDescription, EnumQuality.GOOD, DateTime.Now);
			this.AddProperty(new DaConstantProperty(101, "Description", "101", value));
		}   //	end ctr

		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		/// ValueChanged
		/// </summary>
		public int ValueChanged(
			uint aDataCv,
			int anInternalData,
			string anAddress,
			string aTextDescription)
		{

			try
			{

				ValueQT cacheValue = null;
				this.GetCacheValue(ref cacheValue);
				if (cacheValue != null)
				{
					if (cacheValue.Quality == EnumQuality.GOOD &&
						Convert.ToUInt32(cacheValue.Data) == aDataCv)
					{
						return (int)EnumResultCode.S_OK;
					}   //	end if
				}   //	end if

				cacheValue = new ValueQT(aDataCv, EnumQuality.GOOD, DateTime.Now);

				//	Notify value changed to the Da Element
				base.ValueChanged(cacheValue);

				//	Create a Datas change event and fire it
				DataEvent dataEvent = new DataEvent(this.ItemId, cacheValue.Data, anInternalData, anAddress, aTextDescription);
				dataEvent.Message = aDataCv.ToString();
				dataEvent.Severity = 500;
				dataEvent.ActorId = "OPC User";
				return dataEvent.Fire();
			}
			catch
			{
				return (int)EnumResultCode.E_FAIL;
			}   //	end try ... catch
		}   //	end ValueChanged 


		/// <summary>
		/// ExecuteCommand
		/// </summary>
		public override int ExecuteCommand(string anAddress, string aCommand, string aDescription)
		{
			string message = "Address: " + anAddress + " aCommand: " + aCommand + " aDescription: " + aDescription;
			Application.Instance.Trace(EnumTraceLevel.INF, EnumTraceGroup.OPCSERVER, "DataPointElement::executeCommmand", message);
			return this.ValueChanged(Convert.ToUInt32(aCommand), 0, anAddress, aDescription);
		}   //	end ExecuteCommand


		/// <summary>
		/// Simulate
		/// </summary>
		public override void Simulate()
		{

			try
			{
				if (m_simulationOn)
				{

					uint simulationCV = 0;
					ValueQT cacheValue = null;
					this.GetCacheValue(ref cacheValue);
					if (cacheValue != null)
					{

						simulationCV = Convert.ToUInt32(cacheValue.Data) + 1;
						if (simulationCV > 5)
						{
							simulationCV = 0;
						}   //	end if

						this.ValueChanged(simulationCV, 0, "simulation Address", "simulation Text Description");

					}   //	end if
				}   //	end if
			}
			finally
			{
				base.Simulate();
			}   //	end try ... finally

		}   //	end Simulate


		//-
		#endregion

	}   //	end DataPointElement

	/// <summary>
	/// CommandElement
	/// </summary>
	public class CommandElement : DaBaseElement
	{

		#region //	Protected Constants
		//-----------------------------

		protected const string COMMAND_NAME = "CMD";

		//-
		#endregion

		#region  // Public Constructors

		//-----------------------------

		/// <summary>
		/// CommandElement
		/// </summary>
		public CommandElement()
			: this(COMMAND_NAME)
		{
		}   //	end ctr

		/// <summary>
		/// CommandElement
		/// </summary>
		public CommandElement(string aName)
			: base()
		{

			this.HasChildren = false;
			this.Name = aName;
			this.AccessRights = EnumAccessRights.READWRITEABLE;
			this.IoMode = EnumIoMode.POLL;
			this.Datatype = typeof(string);

		}   //	end ctr

		//-
		#endregion

		#region //	Protected Methods
		//---------------------------

		/// <summary>
		/// HandleItemWriteRequest
		/// </summary>		
		protected override void HandleItemWriteRequest(DaRequest aRequest)
		{


			try
			{
				//	start parsing the received command
				aRequest.Result = EnumResultCode.E_FAIL;

				XmlDocument doc = new XmlDocument();
				doc.LoadXml((string)aRequest.Value.Data);
				Application.Instance.Trace(EnumTraceLevel.INF, EnumTraceGroup.OPCSERVER, "CommandElement::handleWrite", "Received xml text: " + (string)aRequest.Value.Data);

				bool xmlWithCommands = true;
				// xml text without commands node
				foreach (XmlNode command in doc.ChildNodes)
				{
					//	locate the command 
					if (command.LocalName.ToLower() == "command")
					{
						xmlWithCommands = false;
						string commandAddress = command.Attributes["Adresse"].Value;
						string commandName = command.Attributes["Name"].Value;
						string commandDescription = command.Attributes["Description"].Value;
						aRequest.Result = (EnumResultCode)this.ExecuteCommands(commandAddress, commandName, commandDescription);
						if (ResultCode.SUCCEEDED(aRequest.Result))
						{
							Application.Instance.Trace(EnumTraceLevel.INF, EnumTraceGroup.OPCSERVER, "CommandElement::handleWrite", "Command executed on: " + commandAddress + " description: " + commandDescription + " name and value: " + commandName);
							aRequest.Result = (EnumResultCode)this.ValueChanged(aRequest.Value);
						}
						else
						{
							Application.Instance.Trace(EnumTraceLevel.WRN, EnumTraceGroup.OPCSERVER, "CommandElement::handleWrite", "Command could not be executed on: " + commandAddress + " description: " + commandDescription + " name and value: " + commandName);
						} // end if .. else
					}   //	end if
				} // end foreach

				if (xmlWithCommands == true)
				{
					foreach (XmlNode commands in doc.ChildNodes)
					{
						//	locate the command 
						if (commands.LocalName.ToLower() == "commands")
						{
							foreach (XmlNode command in commands.ChildNodes)
							{
								//	locate the command 
								if (command.LocalName.ToLower() == "command")
								{
									string commandAddress = command.Attributes["Address"].Value;
									string commandName = command.Attributes["Name"].Value;
									string commandDescription = command.Attributes["Description"].Value;
									aRequest.Result = (EnumResultCode)this.ExecuteCommands(commandAddress, commandName, commandDescription);
									if (ResultCode.SUCCEEDED(aRequest.Result))
									{
										Application.Instance.Trace(EnumTraceLevel.INF, EnumTraceGroup.OPCSERVER, "CommandElement::handleWrite", "Command executed on: " + commandAddress + " description: " + commandDescription + " name and value: " + commandName);
										aRequest.Result = (EnumResultCode)this.ValueChanged(aRequest.Value);
									}
									else
									{
										Application.Instance.Trace(EnumTraceLevel.WRN, EnumTraceGroup.OPCSERVER, "CommandElement::handleWrite", "Command could not be executed on: " + commandAddress + " description: " + commandDescription + " name and value: " + commandName);
									} // end if .. else
								}   //	end if
							} // end foreach
						} // end if
					} // end foreach
				} // end if
			}
			catch
			{
				aRequest.Result = EnumResultCode.E_FAIL;
			}
			finally
			{
				aRequest.Complete();
			}   //	end try ... catch ... finally

		}   //	end HandleItemWriteRequest


		/// <summary>
		/// Executes the command
		/// </summary>
		/// <param name="anAddress"></param>
		/// <param name="aCommand"></param>
		/// <returns></returns>
		protected int ExecuteCommands(
			string anAddress,
			string aCommand,
			string aDescription)
		{

			try
			{

				//	Delegate command execution to the application
				AddressSpaceElement targetElement;
				// TODO: replace the QueryAddressSpaceElementData call with BISDemo specific message
				BISDaRoot daRoot = Application.Instance.DaAddressSpaceRoot as BISDaRoot;
				daRoot.findAddressSpaceElementData(anAddress, out targetElement);

				DaBaseElement element = targetElement as DaBaseElement;

				if (element == null)
				{
					//	the targeted element was not found
					return (int)EnumResultCode.E_FAIL;
				}   //	end if

				return element.ExecuteCommand(anAddress, aCommand, aDescription);
			}
			catch
			{
				return (int)EnumResultCode.E_FAIL;
			}   //	end try ... catch			

		}   //	end ExecuteCommand

		//-
		#endregion

	}   //	end CommandElement

	/// <summary>
	/// CommandElement
	/// </summary>
	public class DeviceElement : DaBaseElement
	{

		#region  //	Public constructors
		//-----------------------------

		public DeviceElement(
			string aName,
			uint aDataTypeId,
			string aDescription)
			: base()
		{

			this.AccessRights = EnumAccessRights.READABLE;
			this.Datatype = typeof(uint);
			this.IoMode = EnumIoMode.REPORT;
			this.Name = aName;
			this.HasChildren = false;
			this.IoMode = EnumIoMode.POLL;
			this.AddProperty(new DataPointTypeProperty(aDataTypeId));
			ValueQT value = new ValueQT(aDescription, EnumQuality.GOOD, DateTime.Now);
			this.AddProperty(new DaConstantProperty(101, "Description", "101", value));
		}   //	end ctr

		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		/// Connected
		/// </summary>
		public bool Connected
		{
			get
			{
				ValueQT cacheValue = null;
				this.GetCacheValue(ref cacheValue);
				if (cacheValue != null && cacheValue.Quality == EnumQuality.GOOD)
				{
					return Convert.ToBoolean(cacheValue.Data);
				}
				else
				{
					return false;
				}   //	end if
			}   //	end get
			set
			{

				ValueQT cacheValue = null;
				this.GetCacheValue(ref cacheValue);
				if (cacheValue != null)
				{
					if (cacheValue.Quality == EnumQuality.GOOD &&
						Convert.ToBoolean(cacheValue.Data) == value)
					{
						return;
					}   //	end if
				}   //	end if

				cacheValue = new ValueQT(Convert.ToUInt32(value), EnumQuality.GOOD, DateTime.Now);

				//	Notify value changed to the Da Element
				this.ValueChanged(cacheValue);

				//	Create a State change event and fire it
				DeviceEvent deviceState = new DeviceEvent(this.ItemId, cacheValue.Data);
				deviceState.ActorId = "OPC User";
				if (value)
				{
					deviceState.Message = "Connected";
					deviceState.Severity = 1;
				}
				else
				{
					deviceState.Message = "Disconnected";
					deviceState.Severity = 1000;
				}   //	end if ... else
				deviceState.Fire();
			}   //	end set
		}   //	end Connected 

		/// <summary>
		/// Simulate
		/// </summary>
		public override void Simulate()
		{
			try
			{
				if (m_simulationOn)
				{
					ValueQT cacheValue = null;
					GetCacheValue(ref cacheValue);

					if (cacheValue != null)
					{
						bool simulationCV = Convert.ToBoolean(cacheValue.Data);
						simulationCV = simulationCV ? false : true;
						Connected = simulationCV;
					}   //	end if
				}   //	end if
			}
			finally
			{
				base.Simulate();
			}   //	end try ... finally
		}


		/// <summary>
		/// ValueChanged
		/// </summary>
		public int ValueChanged(
			uint aDataCv,
			int anInternalData,
			string anAddress,
			string aTextDescription)
		{

			try
			{

				ValueQT cacheValue = null;
				this.GetCacheValue(ref cacheValue);
				if (cacheValue != null)
				{
					if (cacheValue.Quality == EnumQuality.GOOD &&
						Convert.ToUInt32(cacheValue.Data) == aDataCv)
					{
						return (int)EnumResultCode.S_OK;
					}   //	end if
				}   //	end if

				cacheValue = new ValueQT(aDataCv, EnumQuality.GOOD, DateTime.Now);

				//	Notify value changed to the Da Element
				base.ValueChanged(cacheValue);

				//	Create a State change event and fire it
				DeviceEvent deviceState = new DeviceEvent(this.ItemId, cacheValue.Data);
				deviceState.ActorId = "OPC User";
				if (aDataCv > 0)
				{
					deviceState.Message = "Connected";
					deviceState.Severity = 1;
				}
				else
				{
					deviceState.Message = "Disconnected";
					deviceState.Severity = 1000;
				}   //	end if ... else
				return deviceState.Fire();
			}
			catch
			{
				return (int)EnumResultCode.E_FAIL;
			}   //	end try ... catch
		}   //	end ValueChanged 

		/// <summary>
		/// ExecuteCommand
		/// </summary>
		public override int ExecuteCommand(string anAddress, string aCommand, string aDescription)
		{
			string message = "Address: " + anAddress + " aCommand: " + aCommand + " aDescription: " + aDescription;
			Application.Instance.Trace(EnumTraceLevel.INF, EnumTraceGroup.OPCSERVER, "DataPointElement::executeCommmand", message);
			return this.ValueChanged(Convert.ToUInt32(aCommand), 0, anAddress, aDescription);
		}   //	end ExecuteCommand

		//-
		#endregion

	}   //	end DeviceElement


}   //	end namespace BISDemo

