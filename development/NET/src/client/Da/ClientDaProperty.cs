using System;
using System.Runtime.Serialization;
using System.Security.Permissions;

namespace Softing.OPCToolbox.Client
{
	///<summary>
	///<para>Represents the property of an item.</para>
	///<para>A property represents an information on the measurable variables of a device or on the devices the items represent.
	///Copying this information on items would unnecessarily expand the address space. Therefore attributes, so called properties, are allocated
	///to nodes and leaves in the server's address space.</para>
	///<para>The name of the manufacturer of a device,for example,can be mapped to a property.</para>
	///</summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="DaProperty"]/doc/*'
	/// />
	[SerializableAttribute]
	public class DaProperty
	{
		#region //Public Constructors
		/// <summary>
		/// Initializes a new instance of the <see cref="DaProperty"/> class.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/constructor[@name="DaProperty"]/doc/*'
		/// />
		public DaProperty()
		{
			this.m_id = 0;
			this.m_name = null;
			this.m_itemId = null;
			this.m_itemPath = null;
			this.m_description = null;
			this.m_dataType = null;
			this.m_value = null;
			this.m_result = -1;
		}

		//-----------------------------

		/// <summary>
		/// Initializes a new instance of the <see cref="DaProperty">DaProperty</see> class with the value given by a property ID,a property name, an item ID,
		/// an item path, a property description, a data type for the property value,a property value and a result of getting the property value.
		/// </summary>
		/// <param name="anId">An unique identifier for the property in the server's namespace.</param>
		/// <param name="aName">A second identifier for the property in the server's namespace.</param>
		/// <param name="anItemId">An unique identifier for the item to which the property belongs.</param>
		/// <param name="anItemPath">A path for the item to which the property belongs. </param>
		/// <param name="aDescription">A description of the property.</param>
		/// <param name="aDataType">A System.Type for the value of the porperty.</param>
		/// <param name="aValue">A property value.</param>
		/// <param name="aResult">A result for getting the property value.</param>
		internal DaProperty(
			uint anId,
			string aName,
			string anItemId,
			string anItemPath,
			string aDescription,
			System.Type aDataType,
			ValueQT aValue,
			int aResult)
		{
			this.m_id = anId;
			this.m_name = aName;
			this.m_itemId = anItemId;
			this.m_itemPath = anItemPath;
			this.m_description = aDescription;
			this.m_dataType = aDataType;
			this.m_value = aValue;
			this.m_result = aResult;
		} //	end constructor


		//-
		#endregion

		#region //Protected Attributes
		//----------------------------

		/// <summary>
		/// An unique identifier for the property.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/field[@name="m_id"]/doc/*'
		/// />
		protected readonly uint m_id = 0;

		/// <summary>
		/// A second identifier for the property.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/field[@name="m_name"]/doc/*'
		/// />
		protected readonly string m_name = string.Empty;

		/// <summary>
		/// An unique identifier for the property. This identifier is used in operations such reading/writing a/to a property.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/field[@name="m_itemId"]/doc/*'
		/// />
		protected readonly string m_itemId = string.Empty;

		/// <summary>
		/// An access path for the property. The access path is used in operations such reading/writing a/to a property.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/field[@name="m_itemPath"]/doc/*'
		/// />
		protected readonly string m_itemPath = string.Empty;

		/// <summary>
		/// A textual description of the property.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/field[@name="m_description"]/doc/*'
		/// />
		protected readonly string m_description = string.Empty;

		/// <summary>
		///The property value type.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/field[@name="m_dataType"]/doc/*'
		/// />
		protected readonly System.Type m_dataType = null;

		/// <summary>
		/// The property value together with its quality and timestamp.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/field[@name="m_value"]/doc/*'
		/// />
		protected readonly ValueQT m_value = null;

		///<summary>
		///The result of getting the property value.
		///<note>
		///The result should be checked with ResultCode.SUCCEEDED or with ResultCode.FAILED.
		///</note>
		///</summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/field[@name="m_result"]/doc/*'
		/// />
		protected readonly int m_result = -1;

		//-
		#endregion

		#region //Private Attributes
		private object m_valueJanitor = new Object();

		#endregion

		#region //Public Properties
		//--------------------------

		/// <summary>
		/// Gets an unique identifier for the property in the server's address space.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/property[@name="Id"]/doc/*'
		/// />
		public uint Id
		{
			get { return this.m_id; } //	end get
		} //	end Id

		/// <summary>
		/// Gets a second identifier for the property in the server's address space.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/property[@name="Name"]/doc/*'
		/// />
		public string Name
		{
			get { return this.m_name; } //	end get
		} //	end Name

		/// <summary>
		/// Gets an unique identifier for the property. It is to be used when reading/writing a/to a property.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/property[@name="ItemId"]/doc/*'
		/// />
		public string ItemId
		{
			get { return this.m_itemId; } //	end get
		} //	end ItemId

		/// <summary>
		/// Gets an access path for the property. It is to be used when reading/writing a/to a property.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/property[@name="ItemPath"]/doc/*'
		/// />
		public string ItemPath
		{
			get { return this.m_itemPath; } //	end get
		} //	end ItemPath


		/// <summary>
		/// Gets a textual description of the property.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/property[@name="Description"]/doc/*'
		/// />
		public string Description
		{
			get { return this.m_description; } //	end get
		} //	end Description


		/// <summary>
		/// Gets the type of the property value.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/property[@name="DataType"]/doc/*'
		/// />
		public System.Type DataType
		{
			get { return this.m_dataType; } //	end get
		} //	end Datatype

		/// <summary>
		/// Gets the property value together with its quality and timestamp.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/property[@name="ValueQT"]/doc/*'
		/// />
		public ValueQT ValueQT
		{
			get
			{
				ValueQT res = null;
				lock (m_valueJanitor)
				{
					res = this.m_value;
				} //	end lock
				return res;
			} //	end get
		} //	end Value


		/// <summary>
		/// Gets the result of getting the property value. The result should be checked with <b>SUCCEEDED</b> or <b>FAILED</b>.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="DaProperty"]/property[@name="Result"]/doc/*'
		/// />
		public int Result
		{
			get { return m_result; }
		}

		//-
		#endregion
	}

	//	end class DaProperty
}

//	end namespace Softing.OPCToolbox.Client