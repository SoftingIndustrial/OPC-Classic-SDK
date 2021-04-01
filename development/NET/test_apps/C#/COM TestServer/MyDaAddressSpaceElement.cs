using System;
using System.Collections;
using System.Text;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;


namespace TestServer
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
		}	//	end constructor

		public MyDaAddressSpaceElement()
		{
		}	//	end constructor

		public MyDaAddressSpaceElement(String name, Type dataType)
		{
			Name = name;
			Datatype = dataType;
			AccessRights = EnumAccessRights.READWRITEABLE;
			IoMode = EnumIoMode.POLL;

		} //	end constructor

		//--
		#endregion


		#region Public Methods
		//---------------------

		/// <summary>
		/// Get elements property value data
		/// </summary>
		public void GetPropertyValue(DaRequest aRequest)
		{
			if (aRequest.PropertyId == (int)EnumPropertyId.ITEM_DESCRIPTION)
			{
				aRequest.Value = new ValueQT(
					"Description",
					EnumQuality.GOOD,
					DateTime.Now);

				aRequest.Result = EnumResultCode.S_OK;
			}
			else if (aRequest.PropertyId == (int)EnumPropertyId.HIGH_EU)
			{
				// EU High property
				aRequest.Value = new ValueQT(
					100.0,
					EnumQuality.GOOD,
					DateTime.Now);
				aRequest.Result = EnumResultCode.S_OK;
			}
			else if (aRequest.PropertyId == (int)EnumPropertyId.LOW_EU)
			{
				// EU Low property
				aRequest.Value = new ValueQT(
					0.0,
					EnumQuality.GOOD,
					DateTime.Now);
				aRequest.Result = EnumResultCode.S_OK;
			}
			else if (aRequest.PropertyId == (int)EnumPropertyId.HIGH_INSTRUMENT_RANGE)
			{
				// InstrumentRange High property
				aRequest.Value = new ValueQT(
					120.0,
					EnumQuality.GOOD,
					DateTime.Now);
				aRequest.Result = EnumResultCode.S_OK;
			}
			else if (aRequest.PropertyId == (int)EnumPropertyId.LOW_INSTRUMENT_RANGE)
			{
				// InstrumentRange Low property
				aRequest.Value = new ValueQT(
					0.0,
					EnumQuality.GOOD,
					DateTime.Now);
				aRequest.Result = EnumResultCode.S_OK;
			}
			else if (aRequest.PropertyId == (int)EnumPropertyId.EU_UNITS)
			{
				// EngineeringUnits property
				aRequest.Value = new ValueQT(
					0,
					EnumQuality.GOOD,
					DateTime.Now);
				aRequest.Result = EnumResultCode.S_OK;
			}
			else if (aRequest.PropertyId == 111)
			{
				// ValuePrecision property
				aRequest.Value = new ValueQT(
					2.0,
					EnumQuality.GOOD,
					DateTime.Now);
				aRequest.Result = EnumResultCode.S_OK;
			}
			else if (m_properties.ContainsKey(aRequest.PropertyId))
			{
				DaConstantProperty property = m_properties[aRequest.PropertyId] as DaConstantProperty;
				if (property != null)
				{
					aRequest.Value = property.Value;
					aRequest.Result = EnumResultCode.S_OK;
				}	//	end if
			}
			else
			{
				aRequest.Result = EnumResultCode.E_NOTFOUND;
			}	//	end if ... else
		}	//	end GetPropertyValue

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
			}	//	end if ... else
			return (int)(EnumResultCode.S_OK);
		}	//	end QueryProperties

		public int AddProperty(DaProperty aProperty)
		{
			if ( aProperty != null)
			{
				m_properties.Add(
					aProperty.Id,
					aProperty);
				return (int)EnumResultCode.S_OK;
			}	//	end if
			else
			{
				return (int)EnumResultCode.S_FALSE;
			}	//	end if...else
		}	//	end AddProperty

		//--
		#endregion

		#region Private Members
		//-------------------------------

		private Hashtable m_properties = new Hashtable();

		//--
		#endregion

	}	//	end class MyAddressSpaceElement
}	//	end namespace
