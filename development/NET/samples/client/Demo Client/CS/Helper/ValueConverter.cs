using System;
using System.Collections.Generic;
using System.Text;
using Softing.OPCToolbox;

namespace DemoClient.Helper
{
    public static class ValueConverter 
    {
        const string DATE_TIME = "System.DateTime";

        /// <summary>
        /// COnvert array data to strign
        /// </summary>
        /// <param name="itemValue"></param>
        /// <returns></returns>
        public static string ArrayToString(ValueQT itemValue)
        {
            try
            {
                DataTypes dataTypeHelper = new DataTypes();
                if (itemValue == null) return "";
                StringBuilder strBuilderData = new StringBuilder();
                //add value Type if needed
                if (Options.VTItemValue)
                {
                    if (!itemValue.Data.GetType().IsArray)
                    {
                        dataTypeHelper.Type = itemValue.Data.GetType();
                        strBuilderData.Append(dataTypeHelper.NativeDataType);
                        strBuilderData.Append(" (");
                    }
                    else 
                    {
                        Array dataArray = itemValue.Data as Array;
                        if (dataArray.GetLowerBound(0) == 0)
                        {
                            dataTypeHelper.Type = itemValue.Data.GetType();                            
                            strBuilderData.Append(dataTypeHelper.NativeDataType.Trim().Replace("ARRAY",""));
                        }
                        else
                        {
                            string dataType = dataArray.GetType().ToString();
                            dataType = dataType.Remove(dataType.IndexOf('*'));
                            dataTypeHelper.Type = System.Type.GetType(dataType);
                            strBuilderData.Append(dataTypeHelper.NativeDataType.Trim().Replace("ARRAY", ""));
                        }
                    }
                }
                if (itemValue.Data == null)
                {
                    return "";
                }
                if (itemValue.Data.GetType().IsArray)
                {
                    Array dataArray = itemValue.Data as Array;
                    strBuilderData.Append("[");
                    strBuilderData.Append(dataArray.GetLowerBound(0));
                    strBuilderData.Append(", ");
                    strBuilderData.Append(dataArray.GetUpperBound(0));
                    strBuilderData.Append("](");
                    for (int index = dataArray.GetLowerBound(0);
                                    index < dataArray.GetUpperBound(0);
                                    index++)
                    {
                        if (dataArray.GetValue(index).GetType().FullName.Equals(DATE_TIME))
                        {
                            DateTime dateTime = (DateTime)dataArray.GetValue(index);
                            strBuilderData.Append(dateTime.ToString("hh:mm:ss"));
                        }
                        else
                        {
                            strBuilderData.Append(ConvertToType(dataArray.GetValue(index).ToString(), dataArray.GetValue(index).GetType().FullName));
                        }
                        if (index != dataArray.GetUpperBound(0) - 1)
                        {
                            strBuilderData.Append(", ");
                        }
                    }
                    strBuilderData.Append(")");

                }
                else
                {
                    if (itemValue.Data.GetType().FullName.Equals(DATE_TIME))
                    {
                        DateTime dateTime = (DateTime)itemValue.Data;
                        strBuilderData.Append(dateTime.ToString("hh:mm:ss"));
                    }
                    else
                    {
                        strBuilderData.Append(ConvertToType(itemValue.Data.ToString(), itemValue.Data.GetType().FullName));
                    }
                    if (Options.VTItemValue)
                    {
                        strBuilderData.Append(")");
                    }
                }
                return strBuilderData.ToString();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
                return "";
            }
            
        }

        /// <summary>
        /// Convert string to type
        /// </summary>
        /// <param name="strData"></param>
        /// <param name="dataType"></param>
        /// <returns></returns>
        private static string ConvertToType(string strData, string dataType)
        {
            switch (dataType)
            {
                case "System.SByte":
                    {
                        SByte sByteValue = new sbyte();
                        SByte.TryParse(strData, out sByteValue);
                        if (Options.HexItemValue)
                        {
                            return string.Format("0x{0:X8}", sByteValue.ToString("X"));
                        }
                        else
                        {
                            return sByteValue.ToString();
                        }
                    }
                case "System.Byte":
                    {
                        Byte byteValue = new byte();
                        Byte.TryParse(strData, out byteValue);
                        if (Options.HexItemValue)
                        {
                            return string.Format("0x{0:X8}", byteValue.ToString("X"));
                        }
                        else
                        {
                            return byteValue.ToString();
                        }
                    }
                case "System.Int16":
                    {
                        Int16 int16Value = new Int16();
                        Int16.TryParse(strData, out int16Value);
                        if (Options.HexItemValue)
                        {
                            return string.Format("0x{0:X8}", int16Value.ToString("X"));
                        }
                        else
                        {
                            return int16Value.ToString();
                        }
                    }
                case "System.UInt16":
                    {
                        UInt16 uInt16Value = new UInt16();
                        UInt16.TryParse(strData, out uInt16Value);
                        if (Options.HexItemValue)
                        {
                            return string.Format("0x{0:X8}", uInt16Value.ToString("X"));
                        }
                        else
                        {
                            return uInt16Value.ToString();
                        }
                    }
                case "System.Int32":
                    {
                        Int32 int32Value = new Int32();
                        Int32.TryParse(strData, out int32Value);
                        if (Options.HexItemValue)
                        {
                            return string.Format("0x{0:X8}", int32Value.ToString("X"));
                        }
                        else
                        {
                            return int32Value.ToString();
                        }

                    }
                case "System.UInt32":
                    {
                        UInt32 uInt32Value = new UInt32();
                        UInt32.TryParse(strData, out uInt32Value);
                        if (Options.HexItemValue)
                        {
                            return string.Format("0x{0:X8}",uInt32Value.ToString("X"));
                        }
                        else
                        {
                            return uInt32Value.ToString();
                        }
                    }
                case "System.Int64":
                    {
                        Int64 int64Value = new Int64();
                        Int64.TryParse(strData, out int64Value);
                        if (Options.HexItemValue)
                        {
                            return string.Format("0x{0:X8}", int64Value.ToString("X"));
                        }
                        else
                        {
                            return int64Value.ToString();
                        }    

                    }
                case "System.UInt64":
                    {
                        UInt64 uInt64Value = new UInt64();
                        UInt64.TryParse(strData, out uInt64Value);
                        if (Options.HexItemValue)
                        {
                            return string.Format("0x{0:X8}",uInt64Value.ToString("X"));
                        }
                        else
                        {
                            return uInt64Value.ToString();
                        }
                    }

                case "System.Single":
                    {
                        Single singleValue = new Single();                        
                        Single.TryParse(strData, out singleValue);
                            return singleValue.ToString();
                    }
                  case "System.Double":
                    {
                        Double doubleValue = new Double();                        
                        Double.TryParse(strData, out doubleValue);
                        return doubleValue.ToString();
                    }
                case "System.Decimal":
                    {
                        Decimal decimalValue = new Decimal();
                        try
                        {
                            Decimal.TryParse(strData, out decimalValue);
                            return decimalValue.ToString();
                        }
                        catch
                        {
                            return strData;
                        }
                    }
                           
                default:
                    return strData;

            }
 
        }

        /// <summary>
        /// Date to string
        /// </summary>
        public static string DataToString(ValueQT itemValue)
        {
            StringBuilder strBuilderData = new StringBuilder();
            if (itemValue.Data.GetType().IsArray)
            {
                Array dataArray = itemValue.Data as Array;                
                for (int index = dataArray.GetLowerBound(0);
                                index < dataArray.GetUpperBound(0);
                                index++)
                {
                    strBuilderData.Append(ConvertToType(dataArray.GetValue(index).ToString(), dataArray.GetValue(index).GetType().FullName));
                    if (index != dataArray.GetUpperBound(0) - 1)
                    {
                        strBuilderData.Append("; ");
                    }
                }
            }
            return strBuilderData.ToString();
 
        }

        /// <summary>
        /// DisplayTime
        /// </summary>
        /// <param name="dateTime"></param>
        /// <returns></returns>
        public static String DateTimeOptions(DateTime dateTime)
        {
            if (Options.WithDateTime)
            {
                return dateTime.ToString();
            }
            else if(Options.PureTime)
            {
                return dateTime.ToString("dd:mm:ss.fff");
            }
            return dateTime.ToString("dd:mm:ss.fff"); 
        }
    }
}
