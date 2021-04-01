using System;
using System.Runtime.InteropServices;

namespace Softing.OPCToolbox.Client
{
	/// <summary>
	/// Represents a vendor-specific attribute the server can provide as part of an event notification for an event within a specified event category.
	/// </summary>
	/// <include
	///  file='TBNC.doc.xml'
	///  path='//class[@name="AeAttribute"]/doc/*'
	/// />
	public class AeAttribute
	{
		#region //Constructors
		//--------------------

		/// <summary>
		/// Initializes a new instance of the <see cref="AeAttribute"/> class.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeAttribute"]/constructor[@name="AeAttribute1"]/doc/*'
		/// />
		public AeAttribute()
		{
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="AeAttribute"/> class with the value given by an attribute identifier,
		/// an attribute description and an attribute type.
		/// </summary>
		/// <param name="anId">An identifier for the current attribute.</param>
		/// <param name="aDescription">A description for the current attribute.</param>
		/// <param name="aType">The data type of the current attribute.</param>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeAttribute"]/constructor[@name="AeAttribute2"]/doc/*'
		/// />
		public AeAttribute(
			uint anId,
			string aDescription,
			System.Type aType)
		{
			m_Id = anId;
			m_description = aDescription;
			m_dataType = aType;
		}

		//-
		#endregion

		#region //Protected Attributes
		//-------------------------

		/// <summary>
		/// An identifier for the current attribute.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeAttribute"]/field[@name="m_Id"]/doc/*'
		/// />
		protected uint m_Id = 0;

		/// <summary>
		/// A description for the current attribute.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeAttribute"]/field[@name="m_description"]/doc/*'
		/// />
		protected string m_description = String.Empty;

		/// <summary>
		/// The data type of the current attribute.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeAttribute"]/field[@name="m_dataType"]/doc/*'
		/// />
		protected System.Type m_dataType = null;

		//-
		#endregion

		#region //Public Properties
		//-------------------------

		/// <summary>
		/// Gets or sets a numeric identifier for the current attribute.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeAttribute"]/property[@name="Id"]/doc/*'
		/// />
		public uint Id
		{
			get { return m_Id; }
			set { m_Id = value; }
		}

		/// <summary>
		/// Gets or sets a description for the current attribute.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeAttribute"]/property[@name="Description"]/doc/*'
		/// />
		public string Description
		{
			get { return m_description; }
			set { m_description = value; }
		}

		/// <summary>
		/// Gets or sets the data type of the current attribute.
		/// </summary>
		/// <include
		///  file='TBNC.doc.xml'
		///  path='//class[@name="AeAttribute"]/property[@name="DataType"]/doc/*'
		/// />
		public System.Type DataType
		{
			get { return m_dataType; }
			set { m_dataType = value; }
		}

		//-
		#endregion
	}
}