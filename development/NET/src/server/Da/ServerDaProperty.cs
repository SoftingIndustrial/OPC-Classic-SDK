using System;
using System.Collections;
using System.Runtime.InteropServices;

namespace Softing.OPCToolbox.Server
{
	/// <summary>
	/// Represents one <b>non</b> standard property of an AddressSpace Element
	/// </summary>
	/// <include
	///		file='TBNS.doc.xml'
	///		path='//class[@name="DaProperty"]/doc/*'
	///	/>
	public class DaProperty
	{
		#region //	Public Constructors
		//-----------------------------

		/// <summary>
		/// Default Property construcotr
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaProperty"]/
		///		ctor[@name="DaProperty"]/doc/*'
		///	/>
		public DaProperty()
		{
		} //	end default constructor


		//-
		#endregion

		#region // Public Properties
		//--------------------------

		/// <summary>
		/// The access rights (of OPC clients) for this property (read and/or write)
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaProperty"]/
		///		property[@name="AccessRights"]/doc/*'
		///	/>
		public EnumAccessRights AccessRights
		{
			get { return m_accessRights; }
			set { m_accessRights = value; }
		} //	endnAccessRights


		/// <summary>
		/// The datatype of the property item e.g. System.String
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaProperty"]/
		///		property[@name="Datatype"]/doc/*'
		///	/>
		public Type Datatype
		{
			get { return m_datatype; }
			set { m_datatype = value; }
		} //	end DataType


		/// <summary>
		/// The description what the property is about
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaProperty"]/
		///		property[@name="Description"]/doc/*'
		///	/>
		public string Description
		{
			get { return m_description; }
			set { m_description = value; }
		} //	end description


		/// <summary>
		/// Numerical property id as described in the OPC specification
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaProperty"]/
		///		property[@name="Id"]/doc/*'
		///	/>
		public int Id
		{
			get { return m_id; }
			set { m_id = value; }
		} //	end Id


		/// <summary>
		///  Human readable  string describing the property item ID
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaProperty"]/
		///		property[@name="ItemId"]/doc/*'
		///	/>
		public string ItemId
		{
			get { return m_itemID; }
			set { m_itemID = value; }
		} //	end ItemId


		/// <summary>
		/// Name of this property
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaProperty"]/
		///		property[@name="Datatype"]/doc/*'
		///	/>
		public string Name
		{
			get { return m_name; }
			set { m_name = value; }
		} //	end Name


		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		/// Handles a read request for this property. This method is ment to be overriden. The default behavior
		/// marks the request as failed.
		/// </summary>
		/// <param name="aRequest">
		/// </param>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaProperty"]/
		///		method[@name="HandleReadRequest"]/doc/*'
		///	/>
		public virtual void HandleReadRequest(DaRequest aRequest)
		{
			//	Ment to be obverriden, just return failed
			aRequest.Result = EnumResultCode.E_FAIL;
			aRequest.Value = null;
			aRequest.Complete();
		} //	end HandleReadRequest


		/// <summary>
		/// Handles a write request for this property. This method is ment to be overriden. The default behavior
		/// marks the request as failed
		/// </summary>
		/// <param name="aRequest"></param>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaProperty"]/
		///		method[@name="HandleWriteRequest"]/doc/*'
		///	/>
		public virtual void HandleWriteRequest(DaRequest aRequest)
		{
			//	Ment to be obverriden, just return failed
			aRequest.Result = EnumResultCode.E_FAIL;
			aRequest.Value = null;
			aRequest.Complete();
		} //	end HandleWriteRequest


		//-
		#endregion

		#region	//	Protected Attributes
		//------------------------------

		/// <summary>
		/// The numeric identifier
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaProperty"]/
		///		attribute[@name="m_id"]/doc/*'
		///	/>
		protected int m_id = 0;

		/// <summary>
		/// Name for the property
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaProperty"]/
		///		attribute[@name="m_name"]/doc/*'
		///	/>
		protected string m_name = string.Empty;

		/// <summary>
		/// Readable string identifier
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaProperty"]/
		///		attribute[@name="m_itemID"]/doc/*'
		///	/>
		protected string m_itemID = string.Empty;

		/// <summary>
		/// Short description for property
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaProperty"]/
		///		attribute[@name="m_description"]/doc/*'
		///	/>
		protected string m_description = string.Empty;

		/// <summary>
		/// Datatype of the property based item
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaProperty"]/
		///		attribute[@name="m_datatype"]/doc/*'
		///	/>
		protected Type m_datatype = null;

		/// <summary>
		/// Access rights for this property.
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaProperty"]/
		///		attribute[@name="m_accessRights"]/doc/*'
		///	/>
		protected EnumAccessRights m_accessRights = EnumAccessRights.READABLE;

		//-
		#endregion
	} //	end DaProperty

	/// <summary>
	/// constant value property
	/// </summary>
	/// <include
	///		file='TBNS.doc.xml'
	///		path='//class[@name="DaConstantProperty"]/doc/*'
	///	/>
	public class DaConstantProperty : DaProperty
	{
		#region //	Public Constructors
		//-----------------------------

		/// <summary>
		/// Default constructor
		/// </summary>
		/// <param name="anId">Property ID </param>
		/// <param name="aName">Property Name</param>
		/// <param name="anItemId">Property Id </param>
		/// <param name="aValue">Property value</param>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaConstantProperty"]/
		///		ctor[@name="DaConstantProperty"]/doc/*'
		///	/>
		public DaConstantProperty(
			int anId,
			string aName,
			string anItemId,
			ValueQT aValue)
		{
			this.Id = anId;
			this.Name = aName;
			this.ItemId = anItemId;
			this.AccessRights = EnumAccessRights.READABLE;
			this.m_value = new ValueQT(aValue.Data, aValue.Quality, aValue.TimeStamp);
			this.Description = aName;
			this.Datatype = aValue.Data.GetType();
		} //	end default constructor


		//-
		#endregion

		#region // Public Properties
		//--------------------------

		/// <summary>
		/// The current value for this property instance
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaConstantProperty"]/
		///		property[@name="Value"]/doc/*'
		///	/>
		public ValueQT Value
		{
			get { return m_value; } //	end get
		} //	end Value


		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		/// Completes the request with the Value sorted within this object
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaConstantProperty"]/
		///		method[@name="HandleReadRequest"]/doc/*'
		///	/>
		public override void HandleReadRequest(DaRequest aRequest)
		{
			System.Diagnostics.Debug.Assert(aRequest.PropertyId == this.Id,
			                                "HandleRequest called with illegal request");

			aRequest.Result = EnumResultCode.S_OK;
			aRequest.Value = m_value;
			aRequest.Complete();
		} //	end HandleReadRequest


		//-
		#endregion

		#region	//	Protected Attributes
		//------------------------------

		/// <summary>
		/// Current value for this property
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///		path='//class[@name="DaConstantProperty"]/
		///		attribute[@name="m_value"]/doc/*'
		///	/>
		protected ValueQT m_value = null;

		//-
		#endregion
	} //	end DaConstantProperty
} //	end namespace Softing.OPCToolbox.Server