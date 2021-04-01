using System;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace DemoServer
{
	public class TimeVariable : DemoDaAddressSpaceElement
	{
		public enum TimeType
		{
			type_second,
			type_minute,
			type_hour,
			type_string,
			type_array,
			type_date,
			type_limitSecond
		}

		public enum TimeZone
		{
			zone_local,
			zone_GMT,
			zone_none
		}

		public TimeVariable(String name)
			: base()
		{
			Name = name;
			IoMode = EnumIoMode.POLL;
			AccessRights = EnumAccessRights.READABLE;
		}

		public TimeVariable(String name, TimeType type, TimeZone zone)
			: base()
		{
			Name = name;
			IoMode = EnumIoMode.POLL;
			AccessRights = EnumAccessRights.READABLE;
			m_timeType = type;
			m_timeZone = zone;
			switch (type)
			{
				case TimeType.type_hour:
				case TimeType.type_second:
				case TimeType.type_minute:
					Datatype = typeof(System.Byte);
					break;
				case TimeType.type_string:
					Datatype = typeof(System.String);
					break;
				case TimeType.type_array:
					Datatype = typeof(System.Byte[]);
					break;
				case TimeType.type_date:
					Datatype = typeof(System.DateTime);
					break;
				case TimeType.type_limitSecond:
					AccessRights = EnumAccessRights.READWRITEABLE;
					Datatype = typeof(System.Byte);
					break;
			}
		}

		public override void HandleReadRequest(DaRequest request)
		{
			DemoDaAddressSpaceElement daElement = request.AddressSpaceElement as DemoDaAddressSpaceElement;
			if (daElement != null)
			{
				ValueQT value = new ValueQT();
				DateTime now = DateTime.Now;
				if (m_timeType != TimeType.type_date)
				{
					switch (m_timeZone)
					{
						case TimeZone.zone_local:
							now = DateTime.Now;
							break;
						case TimeZone.zone_GMT:
							now = DateTime.UtcNow;
							break;
					}
					switch (m_timeType)
					{
						case TimeType.type_hour:
							value.SetData((Byte)now.Hour, EnumQuality.GOOD, DateTime.Now);
							break;
						case TimeType.type_second:
							value.SetData((Byte)now.Second, EnumQuality.GOOD, DateTime.Now);
							break;
						case TimeType.type_minute:
							value.SetData((Byte)now.Minute, EnumQuality.GOOD, DateTime.Now);
							break;
						case TimeType.type_date:
							value.SetData(now.Date, EnumQuality.GOOD, DateTime.Now);
							break;
						case TimeType.type_array:
							Byte[] arrayByte = new Byte[3];
							arrayByte[0] = (Byte)now.Hour;
							arrayByte[1] = (Byte)now.Minute;
							arrayByte[2] = (Byte)now.Second;
							value.SetData(arrayByte, EnumQuality.GOOD, DateTime.Now);
							break;
						case TimeType.type_limitSecond:
							ValueQT cacheValue = null;
							request.AddressSpaceElement.GetCacheValue(ref cacheValue);
							value.SetData(cacheValue.Data, EnumQuality.GOOD, DateTime.Now);
							break;
						case TimeType.type_string:
							value.SetData(string.Format("{0:HH:mm:ss}", now), EnumQuality.GOOD, DateTime.Now);
							break;
					}
				}
				else
				{
					value.SetData(now, EnumQuality.GOOD, DateTime.Now);
				}
				ValueChanged(value);
				request.Value = value;
				request.Result = EnumResultCode.S_OK;
			}
			else
			{
				request.Result = EnumResultCode.S_FALSE;
			}
		}
		public override void HandleWriteRequest(DaRequest request)
		{
			DemoDaAddressSpaceElement daElement = request.AddressSpaceElement as DemoDaAddressSpaceElement;
			if (daElement != null)
			{
				if (m_timeType == TimeType.type_limitSecond)
				{
					ValueQT writeValue = request.Value;
					if ((Byte)writeValue.Data <= 59)
					{
						daElement.ValueChanged(writeValue);
						request.Result = EnumResultCode.S_OK;
					}
					else
					{
						request.Result = EnumResultCode.S_FALSE;
					}
				}
			}
			else
			{
				request.Result = EnumResultCode.S_FALSE;
			}
		}

		public override void Init()
		{
			ValueQT value = new ValueQT(0, EnumQuality.GOOD, DateTime.Now);
			ValueChanged(value);
		} // end init

		public override void Simulation()
		{
		}

		protected TimeType m_timeType;
		protected TimeZone m_timeZone;
	}
}
