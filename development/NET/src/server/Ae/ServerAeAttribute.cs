using System;
using System.Collections;
using System.Runtime.InteropServices;
using Softing.OPCToolbox.Server;

namespace Softing.OPCToolbox.Server
{
	/// <summary>
	/// An instance of the AeAttribute class describes a variant value which an
	/// OPC Client may request to be transmitted together with an event notification.
	/// </summary>
	/// <include
	///		file='TBNS.doc.xml'
	///		path='//class[@name="AeAttribute"]/doc/*'
	///	/>
	public class AeAttribute
	{
		#region // Public Constructors
		//----------------------------

		/// <summary>
		/// default Constructor
		/// </summary>
		/// <param name="anId"></param>
		/// <param name="aName"></param>
		/// <param name="aDataType"></param>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAttribute"]/
		///			ctor[@name="AeAttribute"]/doc/*'
		///	/>
		public AeAttribute(uint anId, string aName, Type aDataType)
		{
			this.m_id = anId;
			this.m_name = aName;
			this.m_dataType = aDataType;
		} //	end ctr


		//-
		#endregion

		#region //	Public Properties
		//------------------------------

		/// <summary>
		/// Attribute's identifier
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAttribute"]/
		///			property[@name="Id"]/doc/*'
		///	/>
		public uint Id
		{
			get { return this.m_id; }
			set { this.m_id = value; }
		} //	end Id


		/// <summary>
		/// Attribute's name
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAttribute"]/
		///			property[@name="Name"]/doc/*'
		///	/>
		public string Name
		{
			get { return this.m_name; }
			set { this.m_name = value; }
		} //	end Name


		/// <summary>
		/// Attribute's type
		/// </summary>
		/// <include
		///		file='TBNS.doc.xml'
		///			path='//class[@name="AeAttribute"]/
		///			property[@name="DataType"]/doc/*'
		///	/>
		public Type DataType
		{
			get { return this.m_dataType; }
			set { this.m_dataType = value; }
		} //	end DataType


		//-
		#endregion

		#region //	Private Attributes
		//------------------------------

		/// <summary>
		/// Attribute's integer id
		/// </summary>
		private uint m_id = 0;

		/// <summary>
		/// the attribute's name
		/// </summary>
		private string m_name = string.Empty;

		/// <summary>
		/// attribute's type
		/// </summary>
		private Type m_dataType;

		//-
		#endregion
	} //	end class AeAttribute
} //	end namsepace Softing.OPCToolbox.Server