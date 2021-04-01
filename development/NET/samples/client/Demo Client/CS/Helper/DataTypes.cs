using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Design;
using System.ComponentModel;
using System.Windows.Forms.Design;
using System.Collections;
using System.Runtime.InteropServices;

namespace DemoClient.Helper
{
    public class DataTypes
    {

        #region Private Members

        private  Dictionary<System.Type, string> dataTypes = new Dictionary<Type, string>();

        #endregion

        #region Properties

        private static Type type = null;
        public  Type Type
        {
            get { return type; }
            set { type = value; }
        }

        /// <summary>
        /// The DaItem Native data type
        /// </summary>
        public  string NativeDataType
        {
            get
            {
                if (type == null) return null;
                if (dataTypes.ContainsKey(type))
                {
                    return dataTypes[type];
                }
                else return null;
            }
        }
       
        /// <summary>
        /// Data types required dictionary
        /// </summary>
        public  Dictionary<System.Type, string> RequiredDataTypes
        {
            get
            {              
                return dataTypes;
            }
        }

        #endregion

        #region Constructor

        public DataTypes()
        {
            dataTypes.Add(typeof(System.SByte), "I1");
            dataTypes.Add(typeof(System.Byte), "UI1");
            dataTypes.Add(typeof(System.Int16), "I2");
            dataTypes.Add(typeof(System.UInt16), "UI2");
            dataTypes.Add(typeof(System.Int32), "I4");            
            dataTypes.Add(typeof(System.UInt32), "UI4");
            dataTypes.Add(typeof(System.Int64), "I8");
            dataTypes.Add(typeof(System.UInt64), "UI8");
            dataTypes.Add(typeof(System.Single), "R4");
            dataTypes.Add(typeof(System.Double), "R8");
            dataTypes.Add(typeof(System.Boolean), "BOOL");
            dataTypes.Add(typeof(System.String), "BSTR");
            dataTypes.Add(typeof(System.DateTime), "DATE");
            dataTypes.Add(typeof(System.Decimal), "CY");
            dataTypes.Add(typeof(System.Object), "EMPTY");
            dataTypes.Add(typeof(System.SByte[]), "I1 ARRAY");
            dataTypes.Add(typeof(System.Byte[]), "UI1 ARRAY");
            dataTypes.Add(typeof(System.Int16[]), "I2 ARRAY");
            dataTypes.Add(typeof(System.UInt16[]), "UI2 ARRAY");
            dataTypes.Add(typeof(System.Int32[]), "I4 ARRAY");
            dataTypes.Add(typeof(System.UInt32[]), "UI4 ARRAY");
            dataTypes.Add(typeof(System.Int64[]), "I8 ARRAY");
            dataTypes.Add(typeof(System.UInt64[]), "UI8 ARRAY");
            dataTypes.Add(typeof(System.Single[]), "R4 ARRAY");
            dataTypes.Add(typeof(System.Double[]), "R8 ARRAY");
            dataTypes.Add(typeof(System.Boolean[]), "BOOL ARRAY");
            dataTypes.Add(typeof(System.String[]), "BSTR ARRAY");
            dataTypes.Add(typeof(System.DateTime[]), "DATE ARRAY");
            dataTypes.Add(typeof(System.Decimal[]), "CY ARRAY");
            dataTypes.Add(typeof(System.Object[]), "VARIANT ARRAY");
        }

        #endregion
    }
       

    public class DataTypeConverter
    {

        /// <summary>
        /// Converts the VARTYPE to a system type.
        /// </summary>	
        /// <include 
        ///  file='TBN.doc.xml' 
        ///  path='//class[@name="ValueData"]/method[@name="GetSysType"]/doc/*'
        /// />	
        internal static System.Type GetSysType(short anInput)
        {
            VarEnum input = (VarEnum)anInput;
            if (input == VarEnum.VT_I1) return typeof(SByte);
            if (input == VarEnum.VT_UI1) return typeof(Byte);
            if (input == VarEnum.VT_I2) return typeof(Int16);
            if (input == VarEnum.VT_UI2) return typeof(UInt32);
            if (input == VarEnum.VT_I4) return typeof(Int32);
            if (input == VarEnum.VT_INT) return typeof(Int32);
            if (input == VarEnum.VT_UI4) return typeof(UInt32);
            if (input == VarEnum.VT_UINT) return typeof(UInt32);
            if (input == VarEnum.VT_I8) return typeof(Int64);
            if (input == VarEnum.VT_UI8) return typeof(UInt64);
            if (input == VarEnum.VT_R4) return typeof(Single);
            if (input == VarEnum.VT_R8) return typeof(Double);
            if (input == VarEnum.VT_CY) return typeof(Decimal);
            if (input == VarEnum.VT_BOOL) return typeof(Boolean);
            if (input == VarEnum.VT_DATE) return typeof(DateTime);
            if (input == VarEnum.VT_BSTR) return typeof(String);
            if (input == VarEnum.VT_EMPTY) return typeof(Object);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_I1)) return typeof(SByte[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_UI1)) return typeof(Byte[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_I2)) return typeof(Int16[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_UI2)) return typeof(UInt16[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_I4)) return typeof(Int32[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_INT)) return typeof(Int32[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_UI4)) return typeof(UInt32[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_UINT)) return typeof(UInt32[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_I8)) return typeof(Int64[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_UI8)) return typeof(UInt64[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_R4)) return typeof(Single[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_R8)) return typeof(Double[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_CY)) return typeof(Decimal[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_BOOL)) return typeof(Boolean[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_DATE)) return typeof(DateTime[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_BSTR)) return typeof(String[]);
            if (input == (VarEnum.VT_ARRAY | VarEnum.VT_VARIANT)) return typeof(Object[]);

            return null;
        }		

    }
}
