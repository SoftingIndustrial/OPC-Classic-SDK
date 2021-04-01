using System;
using System.Collections;
using System.Runtime.InteropServices;
using Softing.OPCToolbox.OTB;
using Softing.OPCToolbox.Server;

namespace Softing.OPCToolbox.Server
{
	/// <summary>
	/// The AeCategory class implements an event category in the OPC AE Server.
	/// A category contains events or conditions of the same kind.
	/// They are also characterized by the fact that they can supply the same attributes to the OPC Client.
	/// </summary>
	/// <include
	///		file='TBNS.doc.xml'
	///		path='//class[@name="AeCategory"]/doc/*'
	///	/>
	public class AeCategory
	{
		#region //	Constructors
		//-----------------------------

		/// <summary>
		/// Category constructor
		/// </summary>
		/// <param name="anEventType"> the type of event</param>
		/// <param name="anId">unique integer identifier of a category</param>
		/// <param name="aName"> the category name</param>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			ctor[@name="AeCategory"]/doc/*'
		///	/>
		public AeCategory(
			EnumEventType anEventType,
			uint anId,
			string aName)
		{
			this.m_eventType = anEventType;
			this.m_id = anId;
			this.m_name = aName;
		} //	end ctr


		/// <summary>
		/// Internal constructor. may be only called from witin the TBN asembly
		/// </summary>
		/// <param name="anEventType"></param>
		/// <param name="anId"></param>
		/// <param name="aName"></param>
		/// <param name="aHandle"></param>
		internal AeCategory(
			EnumEventType anEventType,
			uint anId,
			string aName,
			uint aHandle) :
				this(anEventType, anId, aName)
		{
			this.m_handle = aHandle;
		} //	end ctr


		//-
		#endregion

		#region	//	Public Properties
		//--------------------------

		/// <summary>
		/// EventType associated to this category
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			property[@name="EventType"]/doc/*'
		///	/>
		public EnumEventType EventType
		{
			get { return this.m_eventType; } //	end get
			set { this.m_eventType = value; } //	end set
		} //	end EventType


		/// <summary>
		/// the name of the category
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			property[@name="Name"]/doc/*'
		///	/>
		public string Name
		{
			get { return this.m_name; } //	end get
			set { this.m_name = value; } //	end set
		} //	end Name


		/// <summary>
		/// unique Id
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			property[@name="Id"]/doc/*'
		///	/>
		public uint Id
		{
			get { return this.m_id; } //	end get
			set { this.m_id = value; } //	end set
		} //	end Name


		/// <summary>
		/// List of attributes associated to the Category
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			property[@name="AeAttributeList"]/doc/*'
		///	/>
		public Hashtable AeAttributeList
		{
			get
			{
				Hashtable syncAttributes = Hashtable.Synchronized(this.m_attributes);
				return syncAttributes;
			} //	end get
		} //	end AeAttributeList


		//-
		#endregion

		#region //	Internal Properties
		//-----------------------------

		/// <summary>
		/// The Catogury OTB internal handle identifier
		/// </summary>
		internal uint Handle
		{
			get { return m_handle; } //	end get
			set { m_handle = value; } //	end set
		} //	end Handle


		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		/// Creates and adds a new attribute to the category hashtable.
		/// </summary>
		/// <param name="anId"> new attribute ID</param>
		/// <param name="aName"> new attribute </param>
		/// <param name="aDataType"></param>
		/// <returns>
		/// S_OK - if succeeded
		/// E_FAIL - if failure
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			method[@name="AddAttribute.create"]/doc/*'
		///	/>
		public virtual int AddAttribute(uint anId, string aName, Type aDataType)
		{
			AeAttribute attribute = new AeAttribute(anId, aName, aDataType);
			return this.AddAttribute(attribute);
		} //	end AddAttribute


		/// <summary>
		/// adds a new condition definition object with the name provided as aConditionDefinition
		/// </summary>
		/// <param name="aConditionName">the condition name</param>
		/// <returns>
		/// S_OK - if succeeded
		/// E_FAIL - if failure
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			method[@name="AddConditionDefinition"]/doc/*'
		///	/>
		public virtual int AddConditionDefinition(string aConditionName)
		{
			try
			{
				if (aConditionName.Length == 0)
				{
					return (int) EnumResultCode.E_FAIL;
				} //	end if

				if (Handle == 0)
				{
					return (int) EnumResultCode.E_FAIL;
				} //	end if

				int result = OTBFunctions.OTSAddCondition(this.Handle, aConditionName);

				if (ResultCode.SUCCEEDED(result))
				{
					Hashtable syncConditions = Hashtable.Synchronized(this.m_conditions);

					//	Check for a duplicate condition
					AeConditionDefinition condition = syncConditions[aConditionName] as AeConditionDefinition;
					if (condition == null)
					{
						condition = new AeConditionDefinition(aConditionName);
						syncConditions.Add(aConditionName, condition);
					} //	end if
				} //	end if

				return result;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Category.AddCondition",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	enc try ... catch
		} //	end AddConditionDefinition


		/// <summary>
		/// retrieves the AeConditionDefinition associated to the aConditionName
		/// </summary>
		/// <param name="aConditionName">
		/// the name of the condition definition to be found
		/// </param>
		/// <returns>
		/// a reference to the AeCionditionDefinition associated
		/// null if no match found
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			method[@name="GetConditionDefinition"]/doc/*'
		///	/>
		public virtual AeConditionDefinition GetConditionDefinition(string aConditionName)
		{
			Hashtable syncConditions = Hashtable.Synchronized(this.m_conditions);
			return syncConditions[aConditionName] as AeConditionDefinition;
		} //	end GetConditionDefinition


		/// <summary>
		/// Adds a subcondition to a condition in the AeCategory.
		/// </summary>
		/// <param name="aConditionName">The existing condition.</param>
		/// <param name="aSubconditionName">New subcondition name.</param>
		/// <returns>
		/// S_OK - if succeeded
		/// E_FAIL - if failure
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			method[@name="AddSubConditionDefinition"]/doc/*'
		///	/>
		public virtual int AddSubConditionDefinition(
			string aConditionName,
			string aSubconditionName)
		{
			try
			{
				if (aConditionName.Length == 0 || aSubconditionName.Length == 0)
				{
					return (int) EnumResultCode.E_FAIL;
				} //	end if

				if (Handle == 0)
				{
					return (int) EnumResultCode.E_FAIL;
				} //	end if

				System.Diagnostics.Debug.Assert(Handle != 0, "Called for Null handle");

				Hashtable syncConditions = Hashtable.Synchronized(this.m_conditions);

				int result = (int) EnumResultCode.S_OK;

				AeConditionDefinition condition = syncConditions[aConditionName] as AeConditionDefinition;
				if (condition == null)
				{
					result = AddConditionDefinition(aConditionName);
					syncConditions = Hashtable.Synchronized(this.m_conditions);
					condition = syncConditions[aConditionName] as AeConditionDefinition;
				} //	end if

				if (ResultCode.SUCCEEDED(result))
				{
					//	at this time the condition should have been be registered
					result = OTBFunctions.OTSAddSubCondition(this.Handle, aConditionName, aSubconditionName);
					if (ResultCode.SUCCEEDED(result))
					{
						result = condition.AddSubConditonData(aSubconditionName);
					} //	end if
				} //	end if

				return result;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Category.AddCondition",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	end AddSubConditionDefinition


		/// <summary>
		/// returns the sub condition definition object for a specified condition and subcondition identifier
		/// </summary>
		/// <param name="aConditionName">
		/// The condition identifier where the needed subcondition lays
		/// </param>
		/// <param name="aSubconditionName">
		/// the id of the subcondition needed
		/// </param>
		/// <returns>
		/// the instance of subcondition if found;
		/// null if not found
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			method[@name="GetSubConditionDefinition"]/doc/*'
		///	/>
		public AeSubConditionDefinition GetSubConditionDefinition(
			string aConditionName,
			string aSubconditionName)
		{
			Hashtable syncConditions = Hashtable.Synchronized(this.m_conditions);
			AeConditionDefinition conditionDefinition = syncConditions[aConditionName] as AeConditionDefinition;

			if (conditionDefinition != null)
			{
				return conditionDefinition.GetSubConditionDefinition(aSubconditionName);
			} //	end if

			return null;
		} //	end GetConditionDefinition


		//-
		#endregion

		#region //	Protected Methods
		//---------------------------

		/// <summary>
		///	Adds an existing attribute object to the category internal hashtable
		/// </summary>
		/// <param name="anAttribute"> an already created instance of the attribute</param>
		/// <returns>
		/// S_OK - if succeeded
		/// E_FAIL - if failure
		/// </returns>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			method[@name="AddAttribute.existing"]/doc/*'
		///	/>
		protected virtual int AddAttribute(AeAttribute anAttribute)
		{
			try
			{
				if (anAttribute == null)
				{
					return (int) EnumResultCode.E_FAIL;
				} //	end if

				if (Handle == 0)
				{
					return (int) EnumResultCode.E_FAIL;
				} //	end if

				int result = OTBFunctions.OTSAddEventAttribute(
					this.Handle,
					anAttribute.Id,
					anAttribute.Name,
					ValueQT.GetVartype(anAttribute.DataType));

				if (ResultCode.SUCCEEDED(result))
				{
					Hashtable syncAttributeList = Hashtable.Synchronized(this.m_attributes);
					syncAttributeList.Add(anAttribute.Id, anAttribute);
				} //	end if

				return result;
			}
			catch (Exception e)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR,
				                           EnumTraceGroup.OPCSERVER,
				                           "Category.AddAttribute",
				                           "Exception caught:" + e.ToString());
				return (int) EnumResultCode.E_FAIL;
			} //	end try ... catch
		} //	end AddAttribute


		//-
		#endregion

		#region	//	Protected Attributes
		//------------------------------

		/// <summary>
		/// The type of the events that will be generated by this AeCategory
		/// </summary>
		/// <value>
		/// The default value is EnumEventType.SIMPLE
		/// The Category type must be specified when object created.
		/// </value>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			attribute[@name="m_eventType"]/doc/*'
		///	/>
		protected EnumEventType m_eventType = EnumEventType.SIMPLE;

		/// <summary>
		/// The condition's name
		/// </summary>
		/// <value>
		/// The default value is the empty string.
		/// The Category name must be specified when object created.
		/// </value>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			attribute[@name="m_name"]/doc/*'
		///	/>
		protected string m_name = string.Empty;

		/// <summary>
		/// Unique identifier for the instantiated Category.
		/// The Category Id must be specified when object created.
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			attribute[@name="m_id"]/doc/*'
		///	/>
		protected uint m_id = 0;

		/// <summary>
		/// internal OTB identifier
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			attribute[@name="m_handle"]/doc/*'
		///	/>
		private uint m_handle = 0;

		/// <summary>
		/// List of attributes types
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			attribute[@name="m_attributes"]/doc/*'
		///	/>
		protected Hashtable m_attributes = new Hashtable();

		/// <summary>
		/// list of conditions associated to this category
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeCategory"]/
		///			attribute[@name="m_conditions"]/doc/*'
		///	/>
		protected Hashtable m_conditions = new Hashtable();

		//-
		#endregion
	} //	end class AeCategory
} //	end Softing.OPCToolbox.Server