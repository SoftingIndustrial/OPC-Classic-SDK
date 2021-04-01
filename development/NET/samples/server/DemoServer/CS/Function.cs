using System;
using System.Collections;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace DemoServer
{
	class VariableProperty
	{
		public static void CreateAngle(DemoDaAddressSpaceElement daElement,
			String name,
			String description,
			String itemId,
			EnumAccessRights accessRights)
		{
			DaProperty angleProperty = new DaProperty();
			angleProperty.Id = 6020;
			angleProperty.Name = name;
			angleProperty.Description = description;
			angleProperty.ItemId = itemId;
			angleProperty.Datatype = typeof(System.Int16);
			angleProperty.AccessRights = accessRights;
			daElement.AddProperty(angleProperty);
		}

		public static void SetAnalogEngineeringUnits(DemoDaAddressSpaceElement daElement,
			Double lowLimit,
			Double highLimit,
			EnumAccessRights accessRights)
		{
			ValueQT val = new ValueQT(EnumEUType.ANALOG, EnumQuality.GOOD, DateTime.Now);
			DaConstantProperty euType = new DaConstantProperty(
				7,
				"EUType",
				"7",
				val);
			daElement.AddProperty(euType);

			DaProperty euHigh = new DaProperty();
			euHigh.Id = 102;
			euHigh.Name = "High EU";
			euHigh.Description = "High EU Description";
			euHigh.ItemId = "102";
			euHigh.Datatype = typeof(System.Double);
			euHigh.AccessRights = accessRights;
			daElement.AddProperty(euHigh);

			DaProperty euLow = new DaProperty();
			euLow.Id = 103;
			euLow.Name = "Low EU";
			euLow.Description = "Low EU Description";
			euLow.ItemId = "103";
			euLow.Datatype = typeof(System.Double);
			euLow.AccessRights = accessRights;
			daElement.AddProperty(euLow);
		}

		public static double ConvertToRadian(Double angle)
		{
			Double radianAngle = (2 * 3.1415926535) * ((double)angle / 360.0);
			return radianAngle;
		}
	}

	public abstract class Function : DemoDaAddressSpaceElement
	{
		public Function(String name)
		{
			Angle = 0;
			Name = name;
			IoMode = EnumIoMode.REPORT;
			AccessRights = EnumAccessRights.READABLE;
			Datatype = typeof(System.Double);

			//create angle
			VariableProperty.CreateAngle(this,
				"Angle",
				"Angle Description",
				"Angle",
				EnumAccessRights.READWRITEABLE);

			//create High Eu, Low EU, 
			VariableProperty.SetAnalogEngineeringUnits(this, -1.0, 1.0, EnumAccessRights.READABLE);
		}

		public override void Simulation()
		{
		}   //  end Simulation

		public abstract void SetValue(ValueQT value);

		public override void Init()
		{
		} // end init

		public override void HandleReadRequest(DaRequest request)
		{
			if (request.PropertyId == 0)
			{
				ValueQT cacheValue = null;
				request.AddressSpaceElement.GetCacheValue(ref cacheValue);
				request.Value = cacheValue;
				request.Result = EnumResultCode.S_OK;
			}
			else
			{
				DemoDaAddressSpaceElement daElement = request.AddressSpaceElement as DemoDaAddressSpaceElement;
				if (daElement != null)
				{
					daElement.GetPropertyValue(request);
				}
				else
				{
					request.Result = EnumResultCode.S_FALSE;
				}
			}
		}
		public override void HandleWriteRequest(DaRequest request)
		{
			if (request.PropertyId == 0)
			{
				ValueQT cacheValue = null;
				request.AddressSpaceElement.GetCacheValue(ref cacheValue);
				request.AddressSpaceElement.ValueChanged(cacheValue);
				request.Value = cacheValue;
				request.Result = EnumResultCode.S_OK;
			}
			else if (request.PropertyId == 6020)
			{
				Function daElement = request.AddressSpaceElement as Function;
				if (daElement != null)
				{
					ValueQT writeValue = request.Value;
					daElement.SetValue(writeValue);
					request.Result = EnumResultCode.S_OK;
				}
				else
				{
					request.Result = EnumResultCode.S_FALSE;
				}
			}
			else
			{
				request.Result = EnumResultCode.S_FALSE;
			}
		}

		public Int16 Angle;
	}

	public class SinFunction : Function
	{
		public SinFunction()
			: base("sin")
		{
		}

		public override void Simulation()
		{
			Angle++;
			if (Angle >= 360)
			{
				Angle = 0;
			}
			ValueQT val = new ValueQT(Math.Sin(Angle / 180.0 * 3.14), EnumQuality.GOOD, DateTime.Now);
			ValueChanged(val);
		}

		public override void SetValue(ValueQT value)
		{
			Angle = (short)value.Data;
			ValueQT val = new ValueQT(Math.Sin(Angle / 180.0 * 3.14), EnumQuality.GOOD, DateTime.Now);
			ValueChanged(val);
		}

		public override void Init()
		{
			ValueQT value = new ValueQT(0, EnumQuality.GOOD, DateTime.Now);
			ValueChanged(value);
		} // end init
	}

	public class CosFunction : Function
	{
		public CosFunction()
			: base("cos")
		{
		}

		public override void Simulation()
		{
			Angle++;
			if (Angle >= 360)
			{
				Angle = 0;
			}
			ValueQT val = new ValueQT(Math.Cos(Angle / 180.0 * 3.14), EnumQuality.GOOD, DateTime.Now);
			ValueChanged(val);
		}

		public override void SetValue(ValueQT value)
		{
			Angle = (short)value.Data;
			ValueQT val = new ValueQT(Math.Cos(Angle / 180.0 * 3.14), EnumQuality.GOOD, DateTime.Now);
			ValueChanged(val);
		}

		public override void Init()
		{
			ValueQT value = new ValueQT(0, EnumQuality.GOOD, DateTime.Now);
			ValueChanged(value);
		} // end init
	}

	public class TanFunction : Function
	{
		public TanFunction()
			: base("tan")
		{
		}

		public override void Simulation()
		{
			Angle++;
			if (Angle >= 360)
			{
				Angle = 0;
			}
			ValueQT val = new ValueQT(Math.Tan(Angle / 180.0 * 3.14), EnumQuality.GOOD, DateTime.Now);
			ValueChanged(val);
		}

		public override void SetValue(ValueQT value)
		{
			Angle = (short)value.Data;
			ValueQT val = new ValueQT(Math.Tan(Angle / 180.0 * 3.14), EnumQuality.GOOD, DateTime.Now);
			ValueChanged(val);
		}

		public override void Init()
		{
			ValueQT value = new ValueQT(0, EnumQuality.GOOD, DateTime.Now);
			ValueChanged(value);
		} // end init
	}

	public class SimulationVariable : DemoDaAddressSpaceElement
	{
		public SimulationVariable()
			: base()
		{
			Name = "automatic change";
			IoMode = EnumIoMode.REPORT;
			Datatype = typeof(System.Boolean);
			AccessRights = EnumAccessRights.READWRITEABLE;
		}

		public override void HandleReadRequest(DaRequest request)
		{
			DemoDaAddressSpaceElement daElement = request.AddressSpaceElement as DemoDaAddressSpaceElement;
			if (daElement != null)
			{
				ValueQT cacheValue = null;
				request.AddressSpaceElement.GetCacheValue(ref cacheValue);
				request.Value = cacheValue;
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
				ValueQT writeValue = request.Value;
				daElement.ValueChanged(writeValue);
				request.Result = EnumResultCode.S_OK;
				// ser the boolean flag for the DA simulation thread
				Boolean boolVal = (Boolean)writeValue.Data;
				Console.DO_SIMULATION = boolVal;
			}
			else
			{
				request.Result = EnumResultCode.S_FALSE;
			}
		}

		public override void Init()
		{
			ValueQT value = new ValueQT(true, EnumQuality.GOOD, DateTime.Now);
			ValueChanged(value);
		} // end init

		public override void Simulation()
		{
		}
	}

	public class SleepIntervalVariable : DemoDaAddressSpaceElement
	{
		public SleepIntervalVariable()
			: base()
		{
			Name = "automatic change period";
			IoMode = EnumIoMode.REPORT;
			Datatype = typeof(System.UInt32);
			AccessRights = EnumAccessRights.READWRITEABLE;
		}

		public override void Simulation()
		{
		}

		public override void HandleReadRequest(DaRequest request)
		{
			DemoDaAddressSpaceElement daElement = request.AddressSpaceElement as DemoDaAddressSpaceElement;
			if (daElement != null)
			{
				ValueQT cacheValue = null;
				request.AddressSpaceElement.GetCacheValue(ref cacheValue);
				request.Value = cacheValue;
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
				ValueQT writeValue = request.Value;
				daElement.ValueChanged(writeValue);
				request.Result = EnumResultCode.S_OK;
				// generate the interval period changing
				Console.DA_SLEEP_INTERVAL = (UInt32)(writeValue.Data);
			}
			else
			{
				request.Result = EnumResultCode.S_FALSE;
			}
		}

		public override void Init()
		{
			ValueQT value = new ValueQT(1000, EnumQuality.GOOD, DateTime.Now);
			ValueChanged(value);
		} // end init
	}

	public class AlarmSimulation : DemoDaAddressSpaceElement
	{
		public enum AlarmType
		{
			simple,
			tracking
		}

		public AlarmSimulation(String name, AlarmType alarmType)
			: base()
		{
			Name = name;
			AccessRights = EnumAccessRights.READWRITEABLE;
			m_alarmType = alarmType;
			Datatype = typeof(System.Boolean);
			IoMode = EnumIoMode.POLL;
		}

		public override void Simulation()
		{
		}

		public override void Init()
		{
			ValueQT value = new ValueQT(true, EnumQuality.GOOD, DateTime.Now);
			ValueChanged(value);
		}

		public override void HandleReadRequest(DaRequest request)
		{
			DemoDaAddressSpaceElement daElement = request.AddressSpaceElement as DemoDaAddressSpaceElement;
			if (daElement != null)
			{
				ValueQT cacheValue = null;
				request.AddressSpaceElement.GetCacheValue(ref cacheValue);
				request.Value = cacheValue;
				request.Result = EnumResultCode.S_OK;
			}
			else
			{
				request.Result = EnumResultCode.S_FALSE;
			}
		}

		public override void HandleWriteRequest(DaRequest request)
		{
			AlarmSimulation daElement = request.AddressSpaceElement as AlarmSimulation;
			if (daElement != null)
			{
				ValueQT value = request.Value;
				daElement.ValueChanged(value);
				request.Result = EnumResultCode.S_OK;

				// generate the event
				ArrayList attributeValues = new ArrayList(1);
				attributeValues.Add((Boolean)value.Data);
				String actorId = "alarm.simulation";
				String message = "Boolean value: ";
				Boolean boolVal = (Boolean)value.Data;
				if (boolVal)
				{
					message += "TRUE";
				}
				else
				{
					message += "FALSE";
				}
				switch (daElement.m_alarmType)
				{
					case AlarmType.simple:
						AeEvent fireSimpleEvent = new AeEvent(
							EnumEventType.SIMPLE, "alarm simulation.fire simple event", Console.CATEGORY_ID_DA_WRITE_SIMPLE);
						fireSimpleEvent.Severity = 700;
						fireSimpleEvent.ActorId = actorId;
						fireSimpleEvent.Message = message;
						fireSimpleEvent.AttributeValueList = attributeValues;
						fireSimpleEvent.Fire();
						break;
					case AlarmType.tracking:
						AeEvent fireTrackingEvent = new AeEvent(
							EnumEventType.SIMPLE, "alarm simulation.fire tracking event", Console.CATEGORY_ID_DA_WRITE_TRACKING);
						fireTrackingEvent.Severity = 750;
						fireTrackingEvent.ActorId = actorId;
						fireTrackingEvent.Message = message;
						fireTrackingEvent.AttributeValueList = attributeValues;
						fireTrackingEvent.Fire();
						break;
				}
			}
			else
			{
				request.Result = EnumResultCode.S_FALSE;
			}
		}

		protected AlarmType m_alarmType;
	}

	public class KeyVariable : DemoDaAddressSpaceElement
	{
		public KeyVariable()
			: base()
		{
			Name = "key";
			AccessRights = EnumAccessRights.READABLE;
			Datatype = typeof(System.Byte);
			IoMode = EnumIoMode.POLL;
		}

		public override void Init()
		{
			ValueQT value = new ValueQT(0, EnumQuality.GOOD, DateTime.Now);
			ValueChanged(value);
		}

		public override void Simulation()
		{
		}

		public override void HandleReadRequest(DaRequest request)
		{
			DemoDaAddressSpaceElement daElement = request.AddressSpaceElement as DemoDaAddressSpaceElement;
			if (daElement != null)
			{
				ValueQT cacheValue = null;
				request.AddressSpaceElement.GetCacheValue(ref cacheValue);
				request.Value = cacheValue;
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
				ValueQT value = request.Value;
				daElement.ValueChanged(value);
				request.Result = EnumResultCode.S_OK;

				//generate the event
				ArrayList attributeValues = new ArrayList(1);
				attributeValues.Add((Byte)value.Data);
				char charPressed = (char)((Byte)value.Data);
				AeEvent keyEvent = new AeEvent(EnumEventType.TRACKING, "keyboard.key", Console.CATEGORY_ID_KEY_HIT);
				keyEvent.Severity = 600;
				String message = "key hit : ";
				message += charPressed.ToString();
				keyEvent.Message = message;
				keyEvent.AttributeValueList = attributeValues;
				keyEvent.Fire();
			}
			else
			{
				request.Result = EnumResultCode.S_FALSE;
			}
		}
	}

}
