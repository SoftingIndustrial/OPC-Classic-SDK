using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;


namespace TestServer
{
	/// <summary>
	/// A class that generates data.
	/// </summary>
	public class DataGenerator
	{
		#region Constructors
		/// <summary>
		/// Initializes the data generator.
		/// </summary>
		public DataGenerator()
		{
			m_maxStringLength = 100;
			m_minDateTimeValue = new DateTime(1900, 1, 1, 0, 0, 0, DateTimeKind.Utc);
			m_maxDateTimeValue = new DateTime(2100, 1, 1, 0, 0, 0, DateTimeKind.Utc);
			m_random = new RandomSource();
			m_boundaryValueFrequency = 2;

			// load the boundary values.
			m_boundaryValues = new SortedDictionary<string, object[]>();

			for (int ii = 0; ii < s_AvailableBoundaryValues.Length; ii++)
			{
				m_boundaryValues[s_AvailableBoundaryValues[ii].SystemType.Name] = s_AvailableBoundaryValues[ii].Values.ToArray();
			}
		}
		#endregion

		#region Public Methods

		/// <summary>
		/// Gets or Sets the maximum length for generated strings.
		/// </summary>
		public int MaxStringLength
		{
			get { return m_maxStringLength; }
			set { m_maxStringLength = value; }
		}

		/// <summary>
		/// Gets or Sets the minimum value for generated date time values.
		/// </summary>
		public DateTime MinDateTimeValue
		{
			get { return m_minDateTimeValue; }
			set { m_minDateTimeValue = value; }
		}

		/// <summary>
		/// Gets or Sets the maximum value for generated date time values.
		/// </summary>
		public DateTime MaxDateTimeValue
		{
			get { return m_maxDateTimeValue; }
			set { m_maxDateTimeValue = value; }
		}

		/// <summary>
		/// Gets or Sets how frequently boundary values should be used expressed as percentage between 0 and 100.
		/// </summary>
		public int BoundaryValueFrequency
		{
			get { return m_boundaryValueFrequency; }
			set { m_boundaryValueFrequency = value; }
		}

		/// <summary>
		/// Gets a random data value.
		/// </summary>
		/// <returns></returns>
		public ValueQT GetRandomValueQT(Type expectedType)
		{
			return new ValueQT(GetRandom(expectedType), EnumQuality.GOOD, DateTime.Now);
		}

		/// <summary>
		/// Gets a random array data value.
		/// </summary>
		/// <returns></returns>
		public ValueQT GetRandomArrayValueQT(Type expectedType, int length, bool fixedLength)
		{
			return new ValueQT(GetRandomArray(expectedType, false, length, fixedLength), EnumQuality.GOOD, DateTime.Now);
		}

		/// <summary>
		/// Returns a random value of the specified type.
		/// </summary>
		public object GetRandom(Type expectedType)
		{
			if (expectedType == typeof(bool))	 return GetRandomBoolean();
			if (expectedType == typeof(byte)) return GetRandomByte();
			if (expectedType == typeof(sbyte))	return GetRandomSByte();
			if (expectedType == typeof(short))	return GetRandomInt16();
			if (expectedType == typeof(ushort))   return GetRandomUInt16();
			if (expectedType == typeof(int))	  return GetRandomInt32();
			if (expectedType == typeof(uint))	 return GetRandomUInt32();
			if (expectedType == typeof(long))	 return GetRandomInt64();
			if (expectedType == typeof(ulong))	return GetRandomUInt64();
			if (expectedType == typeof(float))	return GetRandomFloat();
			if (expectedType == typeof(double))   return GetRandomDouble();
			if (expectedType == typeof(string))   return GetRandomString();
			if (expectedType == typeof(DateTime)) return GetRandomDateTime();
			if (expectedType == typeof(Guid))	 return GetRandomGuid();
			if (expectedType == typeof(byte[]))   return GetRandomByteString();

			return null;
		}

		/// <summary>
		/// Returns a random value of the specified built-in type.
		/// </summary>
		public Array GetRandomArray(Type expectedType, bool useBoundaryValues, int length, bool fixedLength)
		{
			if (expectedType == typeof(bool))	 return GetRandomArray<bool>(useBoundaryValues, length, fixedLength);
			if (expectedType == typeof(byte))	 return GetRandomArray<byte>(useBoundaryValues, length, fixedLength);
			if (expectedType == typeof(sbyte))	return GetRandomArray<sbyte>(useBoundaryValues, length, fixedLength);
			if (expectedType == typeof(short))	return GetRandomArray<short>(useBoundaryValues, length, fixedLength);
			if (expectedType == typeof(ushort))   return GetRandomArray<ushort>(useBoundaryValues, length, fixedLength);
			if (expectedType == typeof(int))	  return GetRandomArray<int>(useBoundaryValues, length, fixedLength);
			if (expectedType == typeof(uint))	 return GetRandomArray<uint>(useBoundaryValues, length, fixedLength);
			if (expectedType == typeof(long))	 return GetRandomArray<long>(useBoundaryValues, length, fixedLength);
			if (expectedType == typeof(ulong))	return GetRandomArray<ulong>(useBoundaryValues, length, fixedLength);
			if (expectedType == typeof(float))	return  GetRandomArray<float>(useBoundaryValues, length, fixedLength);
			if (expectedType == typeof(double))   return GetRandomArray<double>(useBoundaryValues, length, fixedLength);
			if (expectedType == typeof(string))   return GetRandomArray<string>(useBoundaryValues, length, fixedLength);
			if (expectedType == typeof(DateTime)) return GetRandomArray<DateTime>(useBoundaryValues, length, fixedLength);
			if (expectedType == typeof(Guid))	 return GetRandomArray<Guid>(useBoundaryValues, length, fixedLength);
			if (expectedType == typeof(byte[]))   return GetRandomArray<byte[]>(useBoundaryValues, length, fixedLength);

			return null;
		}

		/// <summary>
		/// This method returns a random value of values for the type.
		/// </summary>
		public T GetRandom<T>(bool useBoundaryValues)
		{
			if (useBoundaryValues && UseBoundaryValue())
			{
				object boundaryValue = GetBoundaryValue(typeof(T));

				if (boundaryValue != null || !typeof(T).IsValueType)
				{
					return (T)boundaryValue;
				}
			}

			return (T)GetRandom(typeof(T));
		}

		/// <summary>
		/// This method returns a random array of values for the type.
		/// </summary>
		public T[] GetRandomArray<T>(bool useBoundaryValues, int length, bool fixedLength)
		{
			if (length < 0)
			{
				return null;
			}

			if (!fixedLength)
			{
				length = m_random.NextInt32(length);
			}

			T[] value = new T[length];

			for (int ii = 0; ii < value.Length; ii++)
			{
				object element = null;

				if (useBoundaryValues && UseBoundaryValue())
				{
					element = GetBoundaryValue(typeof(T));
				}
				else
				{
					element = GetRandom(typeof(T));
				}

				if (element == null)
				{
					element = default(T);
				}

				value[ii] = (T)element;
			}

			return value;
		}

		#region Boolean

		/// <summary cref="GetRandom(Type)" />
		public bool GetRandomBoolean()
		{
			return m_random.NextInt32(1) != 0;
		}
		#endregion

		#region SByte

		/// <summary cref="GetRandom(Type)" />
		public sbyte GetRandomSByte()
		{
			int buffer = m_random.NextInt32(Byte.MaxValue);

			if (buffer > SByte.MaxValue)
			{
				return (sbyte)(SByte.MinValue + (buffer - SByte.MaxValue) - 1);
			}

			return (sbyte)buffer;
		}
		#endregion

		#region Byte

		/// <summary cref="GetRandom(Type)" />
		public byte GetRandomByte()
		{
			return (byte)m_random.NextInt32(Byte.MaxValue);
		}
		#endregion

		#region Int16
		/// <summary cref="GetRandom(Type)" />
		public short GetRandomInt16()
		{
			int buffer = m_random.NextInt32(UInt16.MaxValue);

			if (buffer > Int16.MaxValue)
			{
				return (short)(Int16.MinValue + (buffer - Int16.MaxValue) - 1);
			}

			return (short)buffer;
		}
		#endregion

		#region UInt16
		/// <summary cref="GetRandom(Type)" />
		public ushort GetRandomUInt16()
		{
			return (ushort)m_random.NextInt32(UInt16.MaxValue);
		}
		#endregion

		#region Int32
		/// <summary cref="GetRandom(Type)" />
		public int GetRandomInt32()
		{
			return (int)m_random.NextInt32(Int32.MaxValue);
		}
		#endregion

		#region UInt32
		/// <summary cref="GetRandom(Type)" />
		public uint GetRandomUInt32()
		{
			byte[] bytes = new byte[4];
			m_random.NextBytes(bytes, 0, bytes.Length);
			return BitConverter.ToUInt32(bytes, 0);
		}
		#endregion

		#region Int64
		/// <summary cref="GetRandom(Type)" />
		public long GetRandomInt64()
		{
			byte[] bytes = new byte[8];
			m_random.NextBytes(bytes, 0, bytes.Length);
			return BitConverter.ToInt64(bytes, 0);
		}
		#endregion

		#region UInt64
		/// <summary cref="GetRandom(Type)" />
		public ulong GetRandomUInt64()
		{
			byte[] bytes = new byte[8];
			m_random.NextBytes(bytes, 0, bytes.Length);
			return BitConverter.ToUInt64(bytes, 0);
		}
		#endregion

		#region Float
		/// <summary cref="GetRandom(Type)" />
		public float GetRandomFloat()
		{
			byte[] bytes = new byte[4];
			m_random.NextBytes(bytes, 0, bytes.Length);
			return BitConverter.ToSingle(bytes, 0);
		}
		#endregion

		#region Double
		/// <summary cref="GetRandom(Type)" />
		public double GetRandomDouble()
		{
			byte[] bytes = new byte[8];
			m_random.NextBytes(bytes, 0, bytes.Length);
			return BitConverter.ToSingle(bytes, 0);
		}
		#endregion

		#region String
		/// <summary cref="GetRandom(Type)" />
		public string GetRandomString()
		{
			string allowedChars = "abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNOPQRSTUVWXYZ0123456789";
			int maxLength = 50;
			int strLength = m_random.NextInt32(maxLength);
			char[] chars = new char[strLength];

			for (int i = 0; i < strLength; i++)
			{
				chars[i] = allowedChars[m_random.NextInt32(allowedChars.Length-1)];
			}

			return new string(chars);
		}

		#endregion

		#region DateTime
		/// <summary cref="GetRandom(Type)" />
		public DateTime GetRandomDateTime()
		{
			int minTicks = (int)(m_minDateTimeValue.Ticks >> 32);
			int maxTicks = (int)(m_maxDateTimeValue.Ticks >> 32);

			long delta = GetRandomRange(minTicks, maxTicks);

			long higherTicks = (delta << 32);

			uint lowerTicks = GetRandomUInt32();

			return new DateTime(higherTicks + lowerTicks, DateTimeKind.Utc);
		}
		#endregion

		#region Guid
		/// <summary cref="GetRandom(Type)" />
		public Guid GetRandomGuid()
		{
			byte[] bytes = new byte[16];
			m_random.NextBytes(bytes, 0, bytes.Length);
			return new Guid(bytes);
		}

		#endregion

		#region ByteString
		/// <summary cref="GetRandom(Type)" />
		public byte[] GetRandomByteString()
		{
			int length = m_random.NextInt32(m_maxStringLength);

			byte[] bytes = new byte[length];
			m_random.NextBytes(bytes, 0, bytes.Length);

			return bytes;
		}
		#endregion

		#endregion

		#region Private Methods

		/// <summary>
		/// Randomly selects a boundary value for type.
		/// </summary>
		private object GetBoundaryValue(Type type)
		{
			if (type == null)
			{
				return null;
			}

			object[] boundaryValues = null;

			if (!m_boundaryValues.TryGetValue(type.Name, out boundaryValues))
			{
				return null;
			}

			if (boundaryValues == null || boundaryValues.Length == 0)
			{
				return null;
			}

			int index = m_random.NextInt32(boundaryValues.Length - 1);

			if (type.IsInstanceOfType(boundaryValues[index]))
			{
				return boundaryValues[index];
			}

			return null;
		}

		/// <summary>
		/// Returns whether a boundary value should be used.
		/// </summary>
		private bool UseBoundaryValue()
		{
			return m_random.NextInt32(99) < m_boundaryValueFrequency;
		}

		/// <summary>
		/// Returns a positive integer between the specified values.
		/// </summary>
		private int GetRandomRange(int min, int max)
		{
			if (min < 0)
			{
				min = 0;
			}

			if (max < 0)
			{
				max = 0;
			}

			if (min >= max)
			{
				return min;
			}

			return m_random.NextInt32(max - min) + min;
		}

		#endregion

		#region BoundaryValues Class
		/// <summary>
		/// Stores the boundary values for a data type.
		/// </summary>
		private class BoundaryValues
		{
			public BoundaryValues(Type systemType, params object[] values)
			{
				SystemType = systemType;

				if (values != null)
				{
					Values = new List<object>(values);
				}
				else
				{
					Values = new List<object>();
				}
			}

			public Type SystemType;
			public List<object> Values;
		}

		/// <summary>
		/// Boundary values used or testing.
		/// </summary>
		private static readonly BoundaryValues[] s_AvailableBoundaryValues = new BoundaryValues[]
		{
			new BoundaryValues(typeof(sbyte), SByte.MinValue, (sbyte)0, SByte.MaxValue),
			new BoundaryValues(typeof(byte), Byte.MinValue, Byte.MaxValue),
			new BoundaryValues(typeof(short), Int16.MinValue, (short)0, Int16.MaxValue),
			new BoundaryValues(typeof(ushort), UInt16.MinValue, UInt16.MaxValue),
			new BoundaryValues(typeof(int), Int32.MinValue, (int)0, Int32.MaxValue),
			new BoundaryValues(typeof(uint), UInt32.MinValue, UInt32.MaxValue),
			new BoundaryValues(typeof(long), Int64.MinValue, (long)0, Int64.MaxValue),
			new BoundaryValues(typeof(ulong), UInt64.MinValue, UInt64.MaxValue),
			new BoundaryValues(typeof(float), Single.Epsilon, Single.MaxValue, Single.MinValue, Single.NaN, Single.NegativeInfinity, Single.PositiveInfinity, (float)0 ),
			new BoundaryValues(typeof(double), Double.Epsilon, Double.MaxValue, Double.MinValue, Double.NaN, Double.NegativeInfinity, Double.PositiveInfinity, (double)0 ),
			new BoundaryValues(typeof(string), null, String.Empty ),
			new BoundaryValues(typeof(DateTime), DateTime.MinValue, DateTime.MaxValue, new DateTime(1099, 1, 1), new DateTime(2039, 4, 4), new DateTime(2001, 9, 11, 9, 15, 0, DateTimeKind.Local)),
			new BoundaryValues(typeof(Guid), Guid.Empty),
			new BoundaryValues(typeof(byte[]), null, new byte[0]),
		};
		#endregion

		#region Private Fields
		private RandomSource m_random;
		private int m_maxStringLength;
		private DateTime m_minDateTimeValue;
		private DateTime m_maxDateTimeValue;
		private int m_boundaryValueFrequency;
		private SortedDictionary<string, object[]> m_boundaryValues;
		private const string s_Punctuation = "`~!@#$%^&*()_-+={}[]:\"';?><,./";
		#endregion
	}
}
