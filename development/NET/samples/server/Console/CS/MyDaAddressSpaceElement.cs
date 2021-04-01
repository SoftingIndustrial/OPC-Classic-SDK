using System;
using System.Collections;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace Console
{
	public class MyDaAddressSpaceElement : DaAddressSpaceElement
	{

		#region Constructors
		//------------------------------

		public MyDaAddressSpaceElement(
			string anItemID,
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle)
			:
			base(
			anItemID,
			aName,
			anUserData,
			anObjectHandle,
			aParentHandle)
		{
		}   //	end constructor

		public MyDaAddressSpaceElement()
		{
		}   //	end constructor

		//--
		#endregion

		#region Private Attributes
		//-------------------------------

		private Hashtable m_properties = new Hashtable();
		private byte m_Type = TYPE_UNKNOWN;

		//--
		#endregion

		#region Public Constants
		//-------------------------------

		static public byte TYPE_UNKNOWN = 0x0;
		static public byte TYPE_NODEMATH = 0x80;
		static public byte TYPE_NODERANDOM = 0x40;
		static public byte TYPE_RANDOM_100MS = 0x01;
		static public byte TYPE_RANDOM_1S = 0x02;
		static public byte TYPE_SINE = 0x04;
		static public byte TYPE_ACCEPT = 0x08;

		//--
		#endregion

		#region Public Attributes
		//-------------------------------

		public virtual byte Type
		{
			get { return m_Type; }
			set { m_Type = value; }
		}   //	Attribute Type

		//--
		#endregion

		#region Public Methods
		//---------------------

		/// <summary>
		/// Get elements property value data
		/// </summary>

		public void GetPropertyValue(DaRequest aRequest)
		{
			if (aRequest.PropertyId == 101)
			{

				if (Type == MyDaAddressSpaceElement.TYPE_RANDOM_100MS)
				{
					aRequest.Value = new ValueQT("Random value changing every 100 ms", EnumQuality.GOOD, DateTime.Now);
					aRequest.Result = EnumResultCode.S_OK;
				}
				else if (Type == MyDaAddressSpaceElement.TYPE_RANDOM_1S)
				{
					aRequest.Value = new ValueQT("Random value changing every second", EnumQuality.GOOD, DateTime.Now);
					aRequest.Result = EnumResultCode.S_OK;
				}
				else if (Type == MyDaAddressSpaceElement.TYPE_NODERANDOM)
				{
					aRequest.Value = new ValueQT("Random values", EnumQuality.GOOD, DateTime.Now);
					aRequest.Result = EnumResultCode.S_OK;
				}   //	end if ... else
			}
			else if (aRequest.PropertyId == 6020)
			{

				aRequest.Value = new ValueQT(OpcServer.g_angle, EnumQuality.GOOD, DateTime.Now);
				aRequest.Result = EnumResultCode.S_OK;
			}
			else
			{

				if (m_properties.ContainsKey(aRequest.PropertyId))
				{

					DaConstantProperty property = m_properties[aRequest.PropertyId] as DaConstantProperty;
					if (property != null)
					{

						aRequest.Value = property.Value;
						aRequest.Result = EnumResultCode.S_OK;
					}   //	end if
				}   // end if

			}   //	end if ... else

		}   //	end GetPropertyValue

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
			return (int)(EnumResultCode.S_OK);
		}   //	end QueryProperties


		public int AddProperty(DaProperty aProperty)
		{
			if (aProperty != null)
			{
				m_properties.Add(
					aProperty.Id,
					aProperty);
				return (int)EnumResultCode.S_OK;
			}   //	end if
			else
			{
				return (int)EnumResultCode.S_FALSE;
			}   //	end if...else
		}   //	end AddProperty

		//--
		#endregion

	}   //	end class MyAddressSpaceElement
}   //	end namespace
