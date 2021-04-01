using System;
using System.Runtime.InteropServices;
using Softing.OPCToolbox.OTB;
using System.Text;

namespace Softing.OPCToolbox
{
	/// <summary>
	/// Represents the value of a process variable supplied by an OPC Server.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//class[@name="ValueData"]/doc/*'
	/// />
	public class ValueData
	{
		#region //	Constants
		//-------------------

		// the number of characters that the variant string representation contains.
		private const uint MAX_LENGTH_OF_VARIANT_TO_STRING = 256u;

		//-
		#endregion

		#region //	Protected Attributes
		//------------------------------

		/// <summary>
		/// The value of a process variable supplied by an OPC Server.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ValueData"]/field[@name="m_data"]/doc/*'
		/// />
		protected object m_data = null;

		//-
		#endregion

		#region	//	Public Constructors
		//-----------------------------

		/// <summary>
		/// Initializes a new instance of the <see cref="ValueData"/> class with the value given by a process variable value.
		/// </summary>
		/// <param name="aData">The value of a process variable.</param>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ValueData"]/constructor[@name="ValueData"]/doc/*'
		/// />
		public ValueData(object aData)
		{
			m_data = aData;
		} //	end ctr

		/// <summary>
		/// Initializes a new instance of the <see cref="ValueData"/> class.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ValueData"]/method[@name="ValueData"]/doc/*'
		/// />
		public ValueData()
		{
		} //	end ctr

		internal ValueData(IntPtr aDataPtr)
		{
			if (aDataPtr != IntPtr.Zero)
			{
				this.m_data = Marshal.GetObjectForNativeVariant(aDataPtr);
			} //	end if
		} //	end ctr

		//-
		#endregion

		#region //	Public Properties
		//---------------------------

		/// <summary>
		/// Gets the value of The process variable.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ValueData"]/property[@name="Data"]/doc/*'
		/// />
		public object Data
		{
			get { return m_data; } //	end get
		}//	Data

		//-
		#endregion

		#region //	Public Methods
		//----------------------

		/// <summary>
		/// Returns the string representation of a <see cref="ValueData"/> object.
		/// </summary>
		/// <returns>A string that represents the textual representation of a <see cref="ValueData"/> object.</returns>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ValueData"]/method[@name="ToString"]/doc/*'
		/// />
		public override string ToString()
		{
			if (Data == null)
			{
				return string.Empty;
			} //	end if

			IntPtr pData = Marshal.AllocCoTaskMem(ValueQT.VARIANT_SIZE);
			Marshal.GetNativeVariantForObject(Data, pData);

			IntPtr pInput = Marshal.AllocCoTaskMem(Marshal.SystemDefaultCharSize*(int) MAX_LENGTH_OF_VARIANT_TO_STRING);

			IntPtr pOutput = OTBFunctions.OTVariantToString(
				pData,
				MAX_LENGTH_OF_VARIANT_TO_STRING,
				pInput);

			string output = Marshal.PtrToStringUni(pOutput);

			Marshal.FreeCoTaskMem(pInput);
			OTBFunctions.OTVariantClear(pData);
			Marshal.FreeCoTaskMem(pData);

			return output;
		} //	end ToString()

		//-
		#endregion

		#region //	Internal Methods
		//-------------------------

		/// <summary>
		/// Converts the system type to a VARTYPE.
		/// </summary>
		internal static ushort GetVartype(System.Type input)
		{
			if (input == null) return (ushort) VarEnum.VT_EMPTY;
			if (input == typeof (sbyte)) return (ushort) VarEnum.VT_I1;
			if (input == typeof (byte)) return (ushort) VarEnum.VT_UI1;
			if (input == typeof (short)) return (ushort) VarEnum.VT_I2;
			if (input == typeof (ushort)) return (ushort) VarEnum.VT_UI2;
			if (input == typeof (EnumEUType)) return (ushort)VarEnum.VT_I4;
			if (input == typeof (int)) return (ushort) VarEnum.VT_I4;
			if (input == typeof (uint)) return (ushort) VarEnum.VT_UI4;
			if (input == typeof (long)) return (ushort) VarEnum.VT_I8;
			if (input == typeof (ulong)) return (ushort) VarEnum.VT_UI8;
			if (input == typeof (float)) return (ushort) VarEnum.VT_R4;
			if (input == typeof (double)) return (ushort) VarEnum.VT_R8;
			if (input == typeof (decimal)) return (ushort) VarEnum.VT_CY;
			if (input == typeof (bool)) return (ushort) VarEnum.VT_BOOL;
			if (input == typeof (DateTime)) return (ushort) VarEnum.VT_DATE;
			if (input == typeof (string)) return (ushort) VarEnum.VT_BSTR;
			if (input == typeof (object)) return (ushort) VarEnum.VT_EMPTY;
			if (input == typeof (sbyte[])) return (ushort) (VarEnum.VT_ARRAY | VarEnum.VT_I1);
			if (input == typeof (byte[])) return (ushort) (VarEnum.VT_ARRAY | VarEnum.VT_UI1);
			if (input == typeof (short[])) return (ushort) (VarEnum.VT_ARRAY | VarEnum.VT_I2);
			if (input == typeof (ushort[])) return (ushort) (VarEnum.VT_ARRAY | VarEnum.VT_UI2);
			if (input == typeof (int[])) return (ushort) (VarEnum.VT_ARRAY | VarEnum.VT_I4);
			if (input == typeof (uint[])) return (ushort) (VarEnum.VT_ARRAY | VarEnum.VT_UI4);
			if (input == typeof (long[])) return (ushort) (VarEnum.VT_ARRAY | VarEnum.VT_I8);
			if (input == typeof (ulong[])) return (ushort) (VarEnum.VT_ARRAY | VarEnum.VT_UI8);
			if (input == typeof (float[])) return (ushort) (VarEnum.VT_ARRAY | VarEnum.VT_R4);
			if (input == typeof (double[])) return (ushort) (VarEnum.VT_ARRAY | VarEnum.VT_R8);
			if (input == typeof (decimal[])) return (ushort) (VarEnum.VT_ARRAY | VarEnum.VT_CY);
			if (input == typeof (bool[])) return (ushort) (VarEnum.VT_ARRAY | VarEnum.VT_BOOL);
			if (input == typeof (DateTime[])) return (ushort) (VarEnum.VT_ARRAY | VarEnum.VT_DATE);
			if (input == typeof (string[])) return (ushort) (VarEnum.VT_ARRAY | VarEnum.VT_BSTR);
			if (input == typeof (object[])) return (ushort) (VarEnum.VT_ARRAY | VarEnum.VT_VARIANT);

			// check for special types.
			if (input == typeof (System.Type)) return (ushort) VarEnum.VT_I2;

			return (ushort) VarEnum.VT_EMPTY;
		}

		/// <summary>
		/// Converts the VARTYPE to a system type.
		/// </summary>
		internal static System.Type GetSysType(ushort anInput)
		{
			VarEnum input = (VarEnum) anInput;

			if (input == VarEnum.VT_I1) return typeof (sbyte);
			if (input == VarEnum.VT_UI1) return typeof (byte);
			if (input == VarEnum.VT_I2) return typeof (short);
			if (input == VarEnum.VT_UI2) return typeof (ushort);
			if (input == VarEnum.VT_I4) return typeof (int);
			if (input == VarEnum.VT_INT) return typeof (int);
			if (input == VarEnum.VT_UI4) return typeof (uint);
			if (input == VarEnum.VT_UINT) return typeof (uint);
			if (input == VarEnum.VT_I8) return typeof (long);
			if (input == VarEnum.VT_UI8) return typeof (ulong);
			if (input == VarEnum.VT_R4) return typeof (float);
			if (input == VarEnum.VT_R8) return typeof (double);
			if (input == VarEnum.VT_CY) return typeof (decimal);
			if (input == VarEnum.VT_BOOL) return typeof (bool);
			if (input == VarEnum.VT_DATE) return typeof (DateTime);
			if (input == VarEnum.VT_BSTR) return typeof (string);
			if (input == VarEnum.VT_EMPTY) return typeof (object);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_I1)) return typeof (sbyte[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_UI1)) return typeof (byte[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_I2)) return typeof (short[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_UI2)) return typeof (ushort[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_I4)) return typeof (int[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_INT)) return typeof (int[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_UI4)) return typeof (uint[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_UINT)) return typeof (uint[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_I8)) return typeof (long[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_UI8)) return typeof (ulong[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_R4)) return typeof (float[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_R8)) return typeof (double[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_CY)) return typeof (decimal[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_BOOL)) return typeof (bool[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_DATE)) return typeof (DateTime[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_BSTR)) return typeof (string[]);
			if (input == (VarEnum.VT_ARRAY | VarEnum.VT_VARIANT)) return typeof (object[]);

			return null;
		}

		//-
		#endregion
	} //	end ValueData

	/// <summary>
	/// Represents the value of a process variable with its quality and the time it was obtained by the device or by the server's cache.
	/// </summary>
	/// <include
	///  file='TBN.doc.xml'
	///  path='//class[@name="ValueQT"]/doc/*'
	/// />
	public class ValueQT : ValueData
	{
		#region //	Protected Attributes
		//----------------------------

		/// <summary>
		/// The quality for the value of a process variable supplied by an OPC Server.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ValueQT"]/field[@name="m_quality"]/doc/*'
		/// />
		protected EnumQuality m_quality = EnumQuality.BAD_WAITING_FOR_INITIAL_DATA;

		/// <summary>
		/// The time the value of a process variable was obtained by the device or by the server's cache.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ValueQT"]/field[@name="m_timeStamp"]/doc/*'
		/// />
		protected DateTime m_timeStamp = new DateTime(1601, 01, 01, 0, 0, 0, 0);

		//-
		#endregion

		#region //	Internal Constants
		//--------------------------

		/// <summary>
		/// The size of an OTCVariant.
		/// </summary>
		internal static readonly int VARIANT_SIZE = 8 + 2*IntPtr.Size;

		//-
		#endregion

		#region //	Public Properties
		//---------------------------

		/// <summary>
		/// Gets the quality of a process variable value.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ValueQT"]/property[@name="Quality"]/doc/*'
		/// />
		///<value>One of the <see cref="EnumQuality"/> values.</value>
		public EnumQuality Quality
		{
			get { return m_quality; } //	end get
		} //	end Quality


		/// <summary>
		/// Gets the time that the value and its quality were obtained by the device (if this is available) or the time the server
		/// updated or validated the value and quality in its CACHE.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ValueQT"]/property[@name="TimeStamp"]/doc/*'
		/// />
		public DateTime TimeStamp
		{
			get { return m_timeStamp; } //	end get
		} //	end TimeStamp


		//-
		#endregion

		#region // Public Constructors
		//----------------------------

		///<summary>
		///Initializes a new instance of the <see cref="ValueQT"/> class with the value indicated by a process variable value,
		///the value quality and the time the value was obtained by the device or by the server's cache.
		///</summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ValueQT"]/constructor[@name="ValueQT"]/doc/*'
		/// />
		public ValueQT(object aData, EnumQuality aQuality, DateTime aTimeStamp) : base(aData)
		{
			this.m_quality = aQuality;
			this.m_timeStamp = aTimeStamp;
		} //	end constructor


		internal ValueQT(ref OTValueData anOtValueData) : base(anOtValueData.m_value)
		{
			this.m_quality = (EnumQuality) anOtValueData.m_quality;
			this.m_timeStamp = anOtValueData.m_timestamp.ToDateTime();
		} //	end constructor


		///<summary>
		/// Initializes a new instance of the <see cref="ValueQT"/> class.
		/// The data inside it is null, the quality is EnumQuality.BAD_WAITING_FOR_INITIAL_DATA
		/// and the timestamp is a new instance of the DateTime object.
		///</summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ValueQT"]/method[@name="ValueQT_1"]/doc/*'
		/// />
		public ValueQT()
		{
		} //	end constructor

		//-
		#endregion

		#region //	Public Methods
		//------------------------

		/// <summary>
		/// Sets the value, quality and time stamp of a <see cref="ValueQT"/>.
		/// </summary>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ValueQT"]/method[@name="SetData"]/doc/*'
		/// />
		public void SetData(object aData, EnumQuality aQuality, DateTime aTimeStamp)
		{
			this.m_data = aData;
			this.m_quality = aQuality;
			this.m_timeStamp = aTimeStamp;
		} //	end SetData


		/// <summary>
		/// Specifies whether the <see cref="Quality"/> of a <see cref="ValueQT"/> object is set.
		/// </summary>
		/// <returns><i>true</i>,if quality is set;<i>false</i> otherwise.</returns>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ValueQT"]/method[@name="IsQualitySet"]/doc/*'
		/// />
		public bool IsQualitySet()
		{
			return m_quality != EnumQuality.BAD_WAITING_FOR_INITIAL_DATA;
		} //	end IsQualitySet

		/// <summary>
		/// Specifies whether the <see cref="TimeStamp"/> of a <see cref="ValueQT"/> object is set.
		/// </summary>
		/// <returns><i>true</i>,if timestamp is set;<i>false</i> otherwise.</returns>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ValueQT"]/method[@name="IsTimeStampSet"]/doc/*'
		/// />
		public bool IsTimeStampSet()
		{
			return ((m_timeStamp.Hour != 0) && (m_timeStamp.Minute != 0) && (m_timeStamp.Second != 0) &&
			        (m_timeStamp.Millisecond != 0));
		} //	end IsTimeStampSet

		/// <summary>
		///  Returns the string representation of a <see cref="ValueQT"/> object.
		/// </summary>
		/// <returns> The string representation of the ValueQT object.</returns>
		/// <include
		///  file='TBN.doc.xml'
		///  path='//class[@name="ValueQT"]/method[@name="ToString"]/doc/*'
		/// />
		public new string ToString()
		{
			return String.Format("{0} (Q:{1} T:{2})", base.ToString(), m_quality, m_timeStamp.ToString("dd.MM.yyyy HH:mm:ss.fff"));
		} //	end ToString

		//-
		#endregion
	} //	end class ValueQT
} //	end namespace Softing.OPCToolbox