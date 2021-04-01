using System;
using System.Xml.Serialization;
using System.Collections.Generic;
using System.ComponentModel;
using System.Collections;

namespace TestClientGui.Elements
{
	[TypeConverter( typeof( ExpandableObjectConverter ) )]
	public class Value
	{
		public Softing.OPCToolbox.EnumQuality? Quality
		{
			get
			{
				if( m_wrapped.IsQualitySet() )
					return m_wrapped.Quality;
				else
					return null;
			}

			set
			{
				m_wrapped.SetData( m_wrapped.Data, value ?? Softing.OPCToolbox.EnumQuality.QUALITY_NOT_SET, m_wrapped.TimeStamp );
			}
		}

		public DateTime? TimeStamp
		{
			get
			{
				if( m_wrapped.IsTimeStampSet() )
					return m_wrapped.TimeStamp;
				else
					return null;
			}

			set
			{
				m_wrapped.SetData( m_wrapped.Data, m_wrapped.Quality, value ?? DateTime.MinValue );
			}
		}

		public object Data
		{
			get
			{
				if( m_wrapped.Data is IList && GetTypedArray( (IList)m_wrapped.Data ) != null )
				{
					return null;
				}

				return m_wrapped.Data;
			}

			set
			{
				m_wrapped.SetData( value, m_wrapped.Quality, m_wrapped.TimeStamp );
			}
		}

		public List<object> DataArray
		{
			get
			{
				if( m_dirtyValue )
				{
					return m_dataArray;
				}

				if( m_wrapped.Data is IList )
				{
					Array typedArray = GetTypedArray( (IList) m_wrapped.Data ) as Array;

					if( typedArray != null )
					{
						m_dataArray = new List<object>( typedArray.Length );

						foreach( object item in typedArray )
						{
							m_dataArray.Add( item );
						}

						m_dirtyValue = false;
					}
				}
				else if( m_wrapped.Data != null && m_dataArray != null )
				{
					m_dataArray = null;
					m_dirtyValue = false;
				}

				return m_dataArray;
			}

			set
			{
				m_dataArray = value;
				m_dirtyValue = true;
			}
		}

		[XmlIgnore]
		public Softing.OPCToolbox.ValueQT Wrapped
		{
			get
			{
				if( m_dirtyValue && m_dataArray.Count > 0 )
				{
					m_wrapped.SetData( GetTypedArray( m_dataArray ), m_wrapped.Quality, m_wrapped.TimeStamp );

					m_dirtyValue = false;
				}

				return m_wrapped;
			}
		}

		public static Softing.OPCToolbox.ValueQT[] CreateSDKCollection( List<Value> list )
		{
			Softing.OPCToolbox.ValueQT[] l = new Softing.OPCToolbox.ValueQT[list.Count];

			for( int i = 0; i < list.Count; i++ )
			{
				l[i] = list[i].Wrapped;
			}

			return l;
		}

		public static List<Value> CreateList( Softing.OPCToolbox.ValueQT[] collection )
		{
			List<Value> l = new List<Value>( collection.Length );

			for( int i = 0; i < collection.Length; i++ )
			{
				l.Add(  new Value( collection[i] ) );
			}

			return l;
		}

		public Value()
		{
			m_wrapped = new Softing.OPCToolbox.ValueQT();
		}

		public Value( Softing.OPCToolbox.ValueQT value )
		{
			m_wrapped = value;
		}

		private static object GetTypedArray( IList valueArray )
		{
			if( valueArray[0] is bool )
			{
				bool[] array = new bool[valueArray.Count];
				for( int i = 0; i < valueArray.Count; i++ )
				{
					array[i] = (bool) valueArray[i];
				}
				return array;
			}
			if( valueArray[0] is byte )
			{
				byte[] array = new byte[valueArray.Count];
				for( int i = 0; i < valueArray.Count; i++ )
				{
					array[i] = (byte) valueArray[i];
				}

				return array;
			}
			if( valueArray[0] is sbyte )
			{
				sbyte[] array = new sbyte[valueArray.Count];
				for( int i = 0; i < valueArray.Count; i++ )
				{
					array[i] = (sbyte) valueArray[i];
				}

				return array;
			}

			if( valueArray[0] is DateTime )
			{
				DateTime[] array = new DateTime[valueArray.Count];
				for( int i = 0; i < valueArray.Count; i++ )
				{
					array[i] = (DateTime) valueArray[i];
				}

				return array;
			}

			if( valueArray[0] is decimal )
			{
				decimal[] array = new decimal[valueArray.Count];
				for( int i = 0; i < valueArray.Count; i++ )
				{
					array[i] = (decimal) valueArray[i];
				}

				return array;
			}

			if( valueArray[0] is float )
			{
				float[] array = new float[valueArray.Count];
				for( int i = 0; i < valueArray.Count; i++ )
				{
					array[i] = (float) valueArray[i];
				}

				return array;
			}
			if( valueArray[0] is double )
			{
				double[] array = new double[valueArray.Count];
				for( int i = 0; i < valueArray.Count; i++ )
				{
					array[i] = (double) valueArray[i];
				}

				return array;
			}

			if( valueArray[0] is short )
			{
				short[] array = new short[valueArray.Count];
				for( int i = 0; i < valueArray.Count; i++ )
				{
					array[i] = (short) valueArray[i];
				}

				return array;
			}
			if( valueArray[0] is ushort )
			{
				ushort[] array = new ushort[valueArray.Count];
				for( int i = 0; i < valueArray.Count; i++ )
				{
					array[i] = (ushort) valueArray[i];
				}

				return array;
			}

			if( valueArray[0] is int )
			{
				int[] array = new int[valueArray.Count];
				for( int i = 0; i < valueArray.Count; i++ )
				{
					array[i] = (int) valueArray[i];
				}

				return array;
			}
			if( valueArray[0] is uint )
			{
				uint[] array = new uint[valueArray.Count];
				for( int i = 0; i < valueArray.Count; i++ )
				{
					array[i] = (uint) valueArray[i];
				}

				return array;
			}

			if( valueArray[0] is long )
			{
				long[] array = new long[valueArray.Count];
				for( int i = 0; i < valueArray.Count; i++ )
				{
					array[i] = (long) valueArray[i];
				}

				return array;
			}
			if( valueArray[0] is ulong )
			{
				ulong[] array = new ulong[valueArray.Count];
				for( int i = 0; i < valueArray.Count; i++ )
				{
					array[i] = (ulong) valueArray[i];
				}

				return array;
			}

			if( valueArray[0] is string )
			{
				string[] array = new string[valueArray.Count];
				for( int i = 0; i < valueArray.Count; i++ )
				{
					array[i] = (string) valueArray[i];
				}

				return array;
			}

			if( valueArray[0] is object )
			{
				object[] array = new object[valueArray.Count];
				for( int i = 0; i < valueArray.Count; i++ )
				{
					array[i] = (object) valueArray[i];
				}

				return array;
			}

			return null;
		}

		#region Private fields
		Softing.OPCToolbox.ValueQT m_wrapped;
		List<object> m_dataArray;
		bool m_dirtyValue;
		#endregion
	}
}
