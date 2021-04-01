using System;
using System.Collections;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace PerfClient
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
			//IsChanging = false;
		}   //	end constructor

		public MyDaAddressSpaceElement()
		{
		}   //	end constructor

		//--
		#endregion

		#region Private Attributes
		//-------------------------------

		private Hashtable m_properties = new Hashtable();
		private bool m_isChanging = false;

		//--
		#endregion

		#region Public Properties
		//-----------------------

		public bool IsChanging
		{
			get
			{
				return m_isChanging;
			}
			set
			{
				m_isChanging = value;
			}
		}   //	end SetIsChanging

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
					"description",
					EnumQuality.GOOD,
					DateTime.Now);

				aRequest.Result = EnumResultCode.S_OK;
			}
			else
			{
				aRequest.Result = EnumResultCode.E_NOTFOUND;
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


		public int SetInitialValue(string initialValue, string arraySizeString)
		{
			ValueQT val = null;
			sbyte tempSByte;
			byte tempByte;
			UInt16 tempUInt16;
			Int16 tempInt16;
			UInt32 tempUInt32;
			Int32 tempInt32;
			Decimal tempDecimal;
			Boolean tempBoolean;
			DateTime tempDateTime;
			float tempFloat;
			int arraySize;
			string tempString;

			//determine array size if given
			try
			{
				arraySize = int.Parse(arraySizeString);
			}
			catch (FormatException)
			{
				arraySize = 5;
			}

			//////////////////
			// Simple Types //
			//////////////////
			if (Datatype.Equals(typeof(System.SByte)))
			{
				try
				{
					tempSByte = SByte.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempSByte = 0;
				}
				val = new ValueQT(tempSByte, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.Byte)))
			{
				try
				{
					tempByte = byte.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempByte = 0;
				}
				val = new ValueQT(tempByte, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.UInt16)))
			{
				try
				{
					tempUInt16 = UInt16.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempUInt16 = 0;
				}
				val = new ValueQT(tempUInt16, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.Int16)))
			{
				try
				{
					tempInt16 = Int16.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempInt16 = 0;
				}
				val = new ValueQT(tempInt16, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.Int32)))
			{
				try
				{
					tempInt32 = Int32.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempInt32 = 0;
				}
				val = new ValueQT(tempInt32, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.UInt32)))
			{
				try
				{
					tempUInt32 = UInt32.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempUInt32 = 0;
				}
				val = new ValueQT(tempUInt32, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(float)))
			{
				try
				{
					tempFloat = float.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempFloat = 0;
				}
				val = new ValueQT(tempFloat, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.Boolean)))
			{
				//try
				//{
				//    tempBoolean = Boolean.Parse(initialValue);
				//}
				//catch (System.FormatException)
				//{
				tempBoolean = false;
				//}
				val = new ValueQT(tempBoolean, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.DateTime)))
			{
				//try
				//{
				//    tempDateTime = DateTime.Parse(initialValue);
				//}
				//catch (FormatException)
				//{
				tempDateTime = DateTime.Now;
				//}
				val = new ValueQT(tempDateTime, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.String)))
			{
				if (initialValue != "")
					val = new ValueQT(initialValue, EnumQuality.GOOD, DateTime.Now);
				else
					val = new ValueQT("Softing", EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.Decimal)))
			{
				try
				{
					tempDecimal = Decimal.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempDecimal = 0;
				}
				val = new ValueQT(tempDecimal, EnumQuality.GOOD, DateTime.Now);
			}


			/////////////////
			// Array Types //
			/////////////////
			else if (Datatype.Equals(typeof(System.SByte[])))
			{
				try
				{
					tempSByte = SByte.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempSByte = 0;
				}
				SByte[] temp = new SByte[arraySize];
				for (int i = 0; i < arraySize; i++)
				{
					temp[i] = tempSByte;
				} // end for
				val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.Byte[])))
			{
				try
				{
					tempByte = byte.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempByte = 0;
				}
				Byte[] temp = new Byte[arraySize];
				for (int i = 0; i < arraySize; i++)
				{
					temp[i] = tempByte;
				} // end for
				val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.UInt16[])))
			{
				try
				{
					tempUInt16 = UInt16.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempUInt16 = 0;
				}
				UInt16[] temp = new UInt16[arraySize];
				for (int i = 0; i < arraySize; i++)
				{
					temp[i] = tempUInt16;
				} // end for
				val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.Int16[])))
			{
				try
				{
					tempInt16 = Int16.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempInt16 = 0;
				}
				Int16[] temp = new Int16[arraySize];
				for (int i = 0; i < arraySize; i++)
				{
					temp[i] = tempInt16;
				} // end for
				val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.Int32[])))
			{
				try
				{
					tempInt32 = Int32.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempInt32 = 0;
				}
				Int32[] temp = new Int32[arraySize];
				for (int i = 0; i < arraySize; i++)
				{
					temp[i] = tempInt32;
				} // end for
				val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.UInt32[])))
			{
				try
				{
					tempUInt32 = UInt32.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempUInt32 = 0;
				}
				UInt32[] temp = new UInt32[arraySize];
				for (int i = 0; i < arraySize; i++)
				{
					temp[i] = tempUInt32;
				} // end for
				val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(float[])))
			{
				try
				{
					tempFloat = float.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempFloat = 0;
				}
				float[] temp = new float[arraySize];
				for (int i = 0; i < arraySize; i++)
				{
					temp[i] = tempFloat;
				} // end for
				val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.Boolean[])))
			{
				//try
				//{
				//    tempBoolean = Boolean.Parse(initialValue);
				//}
				//catch (FormatException)
				//{
				tempBoolean = false;
				//}
				Boolean[] temp = new Boolean[arraySize];
				for (int i = 0; i < arraySize; i++)
				{
					temp[i] = tempBoolean;
				} // end for
				val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.DateTime[])))
			{
				//try
				//{
				//    tempDateTime = DateTime.Parse(initialValue);
				//}
				//catch (FormatException)
				//{
				tempDateTime = DateTime.Now;
				//}
				DateTime[] temp = new DateTime[arraySize];
				for (int i = 0; i < arraySize; i++)
				{
					temp[i] = tempDateTime;
				} // end for
				val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.String[])))
			{
				if (initialValue == "")
					tempString = "Softing";
				else
					tempString = initialValue;

				String[] temp = new String[arraySize];
				for (int i = 0; i < arraySize; i++)
				{
					temp[i] = tempString;
				} // end for
				val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
			}
			else if (Datatype.Equals(typeof(System.Decimal[])))
			{
				try
				{
					tempDecimal = Decimal.Parse(initialValue);
				}
				catch (FormatException)
				{
					tempDecimal = 0;
				}
				Decimal[] temp = new Decimal[arraySize];
				for (int i = 0; i < arraySize; i++)
				{
					temp[i] = tempDecimal;
				} // end for
				val = new ValueQT(temp, EnumQuality.GOOD, DateTime.Now);
			}
			else
			{
				return (int)EnumResultCode.E_FAIL;
			}

			ValueChanged(val);
			return (int)EnumResultCode.S_OK;
		}// end setInitialValue;

		public int changeValue()
		{
			ValueQT cacheValue = null;
			GetCacheValue(ref cacheValue);

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
			else if (Datatype.Equals(typeof(float)))
			{
				cacheValue.SetData((float)cacheValue.Data + 1, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.Decimal)))
			{
				cacheValue.SetData((Decimal)cacheValue.Data + 1, EnumQuality.GOOD, DateTime.Now);
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
				char[] myArray = strData.ToCharArray();
				for (int i = 0; i < myArray.Length; i++)
				{
					myArray[i] += (char)1;
				}
				strData = new String(myArray);
				cacheValue.SetData(strData, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.DateTime)))
			{
				DateTime dateTimeData = (DateTime)cacheValue.Data;
				dateTimeData = dateTimeData.AddSeconds(1);
				cacheValue.SetData(dateTimeData, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.SByte[])))
			{
				SByte[] sByteArray = (SByte[])cacheValue.Data;
				for (int i = 0; i < sByteArray.Length; i++)
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
				for (int i = 0; i < byteArray.Length; i++)
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
				for (int i = 0; i < int16Array.Length; i++)
				{
					int16Array[i]++;
				}
				cacheValue.SetData(int16Array, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.UInt16[])))
			{
				UInt16[] uint16Array = (UInt16[])cacheValue.Data;
				for (int i = 0; i < uint16Array.Length; i++)
				{
					uint16Array[i]++;
				}
				cacheValue.SetData(uint16Array, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.Int32[])))
			{
				Int32[] int32Array = (Int32[])cacheValue.Data;
				for (int i = 0; i < int32Array.Length; i++)
				{
					int32Array[i]++;
				}
				cacheValue.SetData(int32Array, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.UInt32[])))
			{
				UInt32[] uint32Array = (UInt32[])cacheValue.Data;
				for (int i = 0; i < uint32Array.Length; i++)
				{
					uint32Array[i]++;
				}
				cacheValue.SetData(uint32Array, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(float[])))
			{
				float[] floatArray = (float[])cacheValue.Data;
				for (int i = 0; i < floatArray.Length; i++)
				{
					floatArray[i]++;
				}
				cacheValue.SetData(floatArray, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.Decimal[])))
			{
				Decimal[] decimalArray = (Decimal[])cacheValue.Data;
				for (int i = 0; i < decimalArray.Length; i++)
				{
					decimalArray[i]++;
				}
				cacheValue.SetData(decimalArray, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.Boolean[])))
			{
				Boolean[] booleanArray = (Boolean[])cacheValue.Data;
				for (int i = 0; i < booleanArray.Length; i++)
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
				cacheValue.SetData(booleanArray, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.String[])))
			{
				String[] stringArray = (String[])cacheValue.Data;
				for (int i = 0; i < stringArray.Length; i++)
				{
					char[] myArray = stringArray[i].ToCharArray();
					for (int ii = 0; ii < myArray.Length; ii++)
					{
						myArray[ii] += (char)1;
					}
					stringArray[i] = new String(myArray);
				}
				cacheValue.SetData(stringArray, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else if (Datatype.Equals(typeof(System.DateTime[])))
			{
				DateTime[] dateTimeArray = (DateTime[])cacheValue.Data;
				for (int i = 0; i < dateTimeArray.Length; i++)
				{
					dateTimeArray[i] = dateTimeArray[i].AddSeconds(1);
				}
				cacheValue.SetData(dateTimeArray, EnumQuality.GOOD, DateTime.Now);
				ValueChanged(cacheValue);
			}
			else
			{
				return (int)EnumResultCode.E_FAIL;
			}


			return (int)EnumResultCode.S_OK;
		}


		//--
		#endregion

	}   //	end class MyAddressSpaceElement
}   //	end namespace
