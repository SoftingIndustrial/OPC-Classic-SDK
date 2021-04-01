using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;
using Softing.OPCToolbox;
using System.Collections.Generic;
using System.Collections;

namespace TestClientGui.Elements
{
	public class DaItemRead : Element
	{
		#region Public Properties
		public string ItemsProperty { get; set; }

		public uint MaxAge { get; set; }

		public string ValuesProperty { get; set; }

		public string ResultsProperty { get; set; }

		public List<Value> ExpectedValues { get; set; }
		#endregion

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( ItemsProperty ) )
				return TestResult.ParameterMissing( "ItemsProperty" );

			DaItem[] items = GetProperty( ItemsProperty ) as DaItem[];
			if( items == null )
				return TestResult.ParameterInvalid( "ItemsProperty" );

			try
			{
				ValueQT[] values = new ValueQT[items.Length];
				int[] results = new int[items.Length];

				for( int i = 0; i < items.Length; i++ )
				{
					DaItem item = items[i];
					ValueQT value;
					int readResult;

					int result = item.Read( MaxAge, out value, out readResult, new ExecutionOptions() );
					values[i] = value;
					results[i] = result;

					if( ResultCode.FAILED( result ) )
					{
						EnumResultCode resCode = (EnumResultCode) Enum.Parse( typeof( EnumResultCode ), result.ToString() );

						return TestResult.Failed( string.Format( "Call returned: {0}  ({1})", resCode.ToString(), result ) );
					}
				}

				if( !string.IsNullOrEmpty( ValuesProperty ) )
					SetProperty( ValuesProperty, values );

				if( !string.IsNullOrEmpty( ResultsProperty ) )
					SetProperty( ResultsProperty, results );

				//validate with expected values
				if( ExpectedValues.Count != 0 && ExpectedValues.Count != values.Length )
					return new DaReadTestResult( (int) EnumResultCode.S_OK, values, results, "Unexpected number of results" );

				for( int i = 0; i < ExpectedValues.Count; i++ )
				{
					Softing.OPCToolbox.ValueQT receivedValue = values[i];
					Value expectedValue = ExpectedValues[i];

					if( expectedValue.Data != null )
					{
						if( !expectedValue.Data.Equals( receivedValue.Data ) )
							return new DaReadTestResult( (int) EnumResultCode.S_OK, values, results, string.Format( "Unexpected result Value for item #{0}", i ) );
					}
					else if( expectedValue.DataArray != null )
					{
						IList receivedList = receivedValue.Data as IList;

						if( receivedList != null )
						{
							if( expectedValue.DataArray.Count != receivedList.Count )
								return new DaReadTestResult( (int) EnumResultCode.S_OK, values, results, string.Format( "Result Value array is of different size then expected for item #{0}", i ) );

							for( int j = 0; j < receivedList.Count; j++ )
							{
								if( !expectedValue.DataArray[j].Equals( receivedList[j] ) )
									return new DaReadTestResult( (int) EnumResultCode.S_OK, values, results, string.Format( "Unexpected result Array Item Value for item #{0}", i ) );
							}
						}
					}

					if( expectedValue.Quality != null && expectedValue.Quality != receivedValue.Quality )
						return new DaReadTestResult( (int) EnumResultCode.S_OK, values, results, string.Format( "Unexpected result Quality for item #{0}", i ) );

					//BUG in TB 4.0
					//if( expectedValue.TimeStamp != null && expectedValue.TimeStamp != receivedValue.TimeStamp )
					//	return new DaReadTestResult( (int) EnumResultCode.S_OK, values, results, string.Format( "Unexpected result Timestamp for item #{0}", i ) );
				}

				return new DaReadTestResult( (int)EnumResultCode.S_OK, values, results );
			}
			catch( Exception ex )
			{
				return new TestResult( ex );
			}
		}
	}
}
