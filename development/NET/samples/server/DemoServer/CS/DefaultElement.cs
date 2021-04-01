using System;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace DemoServer
{
	public class BasicElement : DemoDaAddressSpaceElement
	{
		public BasicElement(String name)
			: base()
		{
			Name = name;
		}   //	enc constructor
		public BasicElement(System.Type type)
			: base()
		{
			Datatype = type;
			AccessRights = EnumAccessRights.READWRITEABLE;
			IoMode = Softing.OPCToolbox.Server.EnumIoMode.POLL;
			switch (type.ToString())
			{
				#region Type interpretation for setting proper name for the variable
				case "System.SByte":
					{
						Name = "I1";
					}
					break;
				case "System.Byte":
					{
						Name = "UI1";
					}
					break;
				case "System.Int16":
					{
						Name = "I2";
					}
					break;
				case "System.UInt16":
					{
						Name = "UI2";
					}
					break;
				case "System.Int32":
					{
						Name = "I4";
					}
					break;
				case "System.UInt32":
					{
						Name = "UI4";
					}
					break;
				case "System.Int64":
					{
						Name = "I8";
					}
					break;
				case "System.UInt64":
					{
						Name = "UI8";
					}
					break;
				case "System.Double":
					{
						Name = "R4";
					}
					break;
				case "System.Boolean":
					{
						Name = "Bool";
					}
					break;
				case "System.String":
					{
						Name = "String";
					}
					break;
				case "System.DateTime":
					{
						Name = "DateTime";
					}
					break;
				case "System.SByte[]":
					{
						Name = "I1 Array";
					}
					break;
				case "System.Byte[]":
					{
						Name = "UI1 Array";
					}
					break;
				case "System.Int16[]":
					{
						Name = "I2 Array";
					}
					break;
				case "System.UInt16[]":
					{
						Name = "UI2 Array";
					}
					break;
				case "System.Int32[]":
					{
						Name = "I4 Array";
					}
					break;
				case "System.UInt32[]":
					{
						Name = "UI4 Array";
					}
					break;
				case "System.Double[]":
					{
						Name = "R4 Array";
					}
					break;
				case "System.Boolean[]":
					{
						Name = "Bool Array";
					}
					break;
				case "System.String[]":
					{
						Name = "String Array";
					}
					break;
				case "System.DateTime[]":
					{
						Name = "DateTime Array";
					}
					break;
					#endregion
			}
		}   //	end constructor

		public override void Simulation()
		{
		}   //  end Simulation

		public override void Init()
		{
			Byte arraySize = 5;

			#region Type interpretation for initializing the variable

			if (
				Datatype.Equals(typeof(System.SByte)) ||
				Datatype.Equals(typeof(System.Byte)) ||
				Datatype.Equals(typeof(System.Int16)) ||
				Datatype.Equals(typeof(System.UInt16)) ||
				Datatype.Equals(typeof(System.Int32)) ||
				Datatype.Equals(typeof(System.UInt32)) ||
				Datatype.Equals(typeof(System.Int64)) ||
				Datatype.Equals(typeof(System.UInt64))
				)
			{
				ValueQT val = new ValueQT(0, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(val);
			}
			else if (Datatype.Equals(typeof(System.Double)))
			{
				ValueQT val = new ValueQT(0.0, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(val);
			}
			else if (Datatype.Equals(typeof(System.Boolean)))
			{
				ValueQT val = new ValueQT(true, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(val);
			}
			else if (Datatype.Equals(typeof(System.String)))
			{
				ValueQT val = new ValueQT("DEMO", EnumQuality.GOOD, DateTime.Now);
				ValueChanged(val);
			}
			else if (Datatype.Equals(typeof(System.DateTime)))
			{
				ValueQT val = new ValueQT(DateTime.Now, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(val);
			}
			else if (Datatype.Equals(typeof(System.SByte[])))
			{
				SByte[] temp = new SByte[arraySize];
				for (SByte i = 0; i < arraySize; i++)
				{
					temp[i] = (SByte)(i + 1);
				} // end for
				ValueQT val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(val);
			}
			else if (Datatype.Equals(typeof(System.Byte[])))
			{
				Byte[] temp = new Byte[arraySize];
				for (Byte i = 0; i < arraySize; i++)
				{
					temp[i] = (Byte)(i + 1);
				} // end for
				ValueQT val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(val);
			}
			else if (Datatype.Equals(typeof(System.Int16[])))
			{
				Int16[] temp = new Int16[arraySize];
				for (Int16 i = 0; i < arraySize; i++)
				{
					temp[i] = (Int16)(i + 1);
				} // end for
				ValueQT val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(val);
			}
			else if (Datatype.Equals(typeof(System.UInt16[])))
			{
				UInt16[] temp = new UInt16[arraySize];
				for (UInt16 i = 0; i < arraySize; i++)
				{
					temp[i] = (UInt16)(i + 1);
				} // end for
				ValueQT val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(val);
			}
			else if (Datatype.Equals(typeof(System.Int32[])))
			{
				Int32[] temp = new Int32[arraySize];
				for (Int32 i = 0; i < arraySize; i++)
				{
					temp[i] = (Int32)(i + 1);
				} // end for
				ValueQT val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(val);
			}
			else if (Datatype.Equals(typeof(System.UInt32[])))
			{
				UInt32[] temp = new UInt32[arraySize];
				for (UInt32 i = 0; i < arraySize; i++)
				{
					temp[i] = (UInt32)(i + 1);
				} // end for
				ValueQT val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(val);
			}
			else if (Datatype.Equals(typeof(System.Double[])))
			{
				Double[] temp = new Double[arraySize];
				for (Byte i = 0; i < arraySize; i++)
				{
					temp[i] = (Double)(i + 1);
				} // end for
				ValueQT val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(val);
			}
			else if (Datatype.Equals(typeof(System.Boolean[])))
			{
				Boolean[] temp = new Boolean[arraySize];
				for (Byte i = 0; i < arraySize; i++)
				{
					if (i % 2 == 0)
					{
						temp[i] = true;
					}
					else
					{
						temp[i] = false;
					}
				} // end for
				ValueQT val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(val);
			}
			else if (Datatype.Equals(typeof(System.String[])))
			{
				String[] temp = new String[arraySize];
				for (Byte i = 0; i < arraySize; i++)
				{
					temp[i] = ((char)(i + 65 + 0)).ToString();
					temp[i] += ((char)(i + 65 + 1)).ToString();
					temp[i] += ((char)(i + 65 + 2)).ToString();
				} // end for
				ValueQT val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(val);
			}
			else if (Datatype.Equals(typeof(System.DateTime[])))
			{
				DateTime[] temp = new DateTime[arraySize];
				for (Byte i = 0; i < arraySize; i++)
				{
					temp[i] = DateTime.Now;
				} // end for
				ValueQT val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(val);
			}

			#endregion

		} // end init

		public override void HandleReadRequest(DaRequest request)
		{
			ValueQT cacheValue = null;
			request.AddressSpaceElement.GetCacheValue(ref cacheValue);
			request.Value = cacheValue;
			request.Result = EnumResultCode.S_OK;
		}
		public override void HandleWriteRequest(DaRequest request)
		{
			request.Result = (EnumResultCode)request.AddressSpaceElement.ValueChanged(request.Value);
		}
	}

	public class BasicStaticElement : BasicElement
	{
		public BasicStaticElement(String name)
			: base(name)
		{
		}

		public BasicStaticElement(System.Type varType)
			: base(varType)
		{
		}
	}

	public class BasicDynamicElement : BasicElement
	{
		public BasicDynamicElement(String name)
			: base(name)
		{
		}

		public BasicDynamicElement(System.Type varType)
			: base(varType)
		{
		}

		private void IncrementWString(ref String str)
		{
			char[] myArray = str.ToCharArray();
			for (Byte i = 0; i < myArray.Length; i++)
			{
				myArray[i] += (char)1;
			}
			str = new String(myArray);
		}

		private void IncrementDate(ref DateTime dateTime)
		{
			dateTime = dateTime.AddSeconds(1);
		}

		public override void Simulation()
		{
			ValueQT cacheValue = null;
			GetCacheValue(ref cacheValue);

			#region Type simulation for the dynamic variables

			if (Datatype.Equals(typeof(System.SByte)))
			{
				SByte value = (SByte)cacheValue.Data;
				if (value == 127)
				{
					value = -128;
				}
				else
				{
					value++;
				}
				cacheValue.SetData(value, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.Byte)))
			{
				Byte value = (Byte)cacheValue.Data;
				if (value == 255)
				{
					value = 0;
				}
				else
				{
					value++;
				}
				cacheValue.SetData(value, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.Int16)))
			{
				cacheValue.SetData((Int16)cacheValue.Data + 1, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.UInt16)))
			{
				cacheValue.SetData((UInt16)cacheValue.Data + 1, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.Int32)))
			{
				cacheValue.SetData((Int32)cacheValue.Data + 1, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.UInt32)))
			{
				cacheValue.SetData((UInt32)cacheValue.Data + 1, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.Int64)))
			{
				cacheValue.SetData((Int64)cacheValue.Data + 1, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.UInt64)))
			{
				cacheValue.SetData((UInt64)cacheValue.Data + 1, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.Double)))
			{
				cacheValue.SetData((Double)cacheValue.Data + 1, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.Boolean)))
			{
				Boolean isTrue = (Boolean)cacheValue.Data;
				if (isTrue)
				{
					cacheValue.SetData(false, EnumQuality.GOOD, DateTime.Now);
				}
				else
				{
					cacheValue.SetData(true, EnumQuality.GOOD, DateTime.Now);
				}
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.String)))
			{
				String strData = (String)cacheValue.Data;
				IncrementWString(ref strData);
				cacheValue.SetData(strData, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.DateTime)))
			{
				DateTime dateTimeData = (DateTime)cacheValue.Data;
				IncrementDate(ref dateTimeData);
				cacheValue.SetData(dateTimeData, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.SByte[])))
			{
				SByte[] sByteArray = (SByte[])cacheValue.Data;
				for (Byte i = 0; i < sByteArray.Length; i++)
				{
					if (sByteArray[i] == 127)
					{
						sByteArray[i] = -128;
					}
					else
					{
						sByteArray[i]++;
					}
				}
				cacheValue.SetData(sByteArray, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.Byte[])))
			{
				Byte[] byteArray = (Byte[])cacheValue.Data;
				for (Byte i = 0; i < byteArray.Length; i++)
				{
					if (byteArray[i] == 255)
					{
						byteArray[i] = 0;
					}
					else
					{
						byteArray[i]++;
					}
				}
				cacheValue.SetData(byteArray, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.Int16[])))
			{
				Int16[] int16Array = (Int16[])cacheValue.Data;
				for (Byte i = 0; i < int16Array.Length; i++)
				{
					int16Array[i]++;
				}
				cacheValue.SetData(int16Array, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.UInt16[])))
			{
				UInt16[] uint16Array = (UInt16[])cacheValue.Data;
				for (Byte i = 0; i < uint16Array.Length; i++)
				{
					uint16Array[i]++;
				}
				cacheValue.SetData(uint16Array, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.Int32[])))
			{
				Int32[] int32Array = (Int32[])cacheValue.Data;
				for (Byte i = 0; i < int32Array.Length; i++)
				{
					int32Array[i]++;
				}
				cacheValue.SetData(int32Array, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.UInt32[])))
			{
				UInt32[] uint32Array = (UInt32[])cacheValue.Data;
				for (Byte i = 0; i < uint32Array.Length; i++)
				{
					uint32Array[i]++;
				}
				cacheValue.SetData(uint32Array, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.Double[])))
			{
				Double[] doubleArray = (Double[])cacheValue.Data;
				for (Byte i = 0; i < doubleArray.Length; i++)
				{
					doubleArray[i]++;
				}
				cacheValue.SetData(doubleArray, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.Boolean[])))
			{
				Boolean[] booleanArray = (Boolean[])cacheValue.Data;
				for (Byte i = 0; i < booleanArray.Length; i++)
				{
					if (booleanArray[i])
					{
						booleanArray[i] = false;
					}
					else
					{
						booleanArray[i] = true;
					}
				}
			}
			else if (Datatype.Equals(typeof(System.String[])))
			{
				String[] stringArray = (String[])cacheValue.Data;
				for (Byte i = 0; i < stringArray.Length; i++)
				{
					IncrementWString(ref stringArray[i]);
				}
				cacheValue.SetData(stringArray, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.DateTime[])))
			{
				DateTime[] dateTimeArray = (DateTime[])cacheValue.Data;
				for (Byte i = 0; i < dateTimeArray.Length; i++)
				{
					IncrementDate(ref dateTimeArray[i]);
				}
				cacheValue.SetData(dateTimeArray, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}

			#endregion

		}   //  end Simulation
	}
}
