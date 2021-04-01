using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;
using Softing.OPCToolbox;
using System.Collections.Generic;

namespace TestClientGui.Elements
{
	public class DaItemWrite : Element
	{
		#region Public Properties
		public string ItemsProperty { get; set; }

		public List<Value> Values { get; set; }

		public string ValuesProperty { get; set; }

		public string ResultsProperty { get; set; }

		[DefaultValue( true )]
		public bool ValidateAllResults { get; set; }

		public List<int> ExpectedResults { get; set; }
		#endregion

		public DaItemWrite()
		{
			ValidateAllResults = true;
		}

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( ItemsProperty ) )
				return TestResult.ParameterMissing( "ItemsProperty" );

			DaItem[] items = GetProperty( ItemsProperty ) as DaItem[];
			if( items == null )
				return TestResult.ParameterInvalid( "ItemsProperty" );

			ValueQT[] values;

			if( Values.Count == 0 )
			{
				if( !string.IsNullOrEmpty( ValuesProperty ) )
				{
					values = GetProperty( ValuesProperty ) as ValueQT[];
					if( values == null )
						return TestResult.ParameterInvalid( "ValuesProperty" );
				}
				else
					return TestResult.ParameterMissing( "Values" );
			}
			else
			{
				values = Value.CreateSDKCollection( Values );
			}

			try
			{
				int[] results = new int[items.Length];

				for( int i = 0; i < items.Length; i++ )
				{
					DaItem item = items[i];
					ValueQT value = values[i];
					int writeResult;

					int result = item.Write( value, out writeResult, new ExecutionOptions() );

					if( ResultCode.SUCCEEDED( result ) )
					{
						results[i] = writeResult;

						if( ValidateAllResults )
						{
							if( ResultCode.FAILED( results[i] ) )
							{
								EnumResultCode resCode = (EnumResultCode) Enum.Parse( typeof( EnumResultCode ), results[i].ToString() );

								return TestResult.Failed( string.Format( "Result {0} failed with the code: {1}  ({2})", i, resCode.ToString(), results[i] ) );
							}
						}
						else if( ExpectedResults.Count > i )
						{
							//validate with expected values

							if( results[i] != ExpectedResults[i] )
							{
								EnumResultCode resCode = (EnumResultCode) Enum.Parse( typeof( EnumResultCode ), results[i].ToString() );
								EnumResultCode resExpectedCode = (EnumResultCode) Enum.Parse( typeof( EnumResultCode ), ExpectedResults[i].ToString() );

								return new DaWriteTestResult( result, results, string.Format( "Unexpected result code for item {0}: received {1}  ({2}) instead of {3}  ({4})", i, resCode.ToString(), results[i], resExpectedCode.ToString(), ExpectedResults[i] ) );
							}
						}
					}
					else
					{
						EnumResultCode resCode = (EnumResultCode) Enum.Parse( typeof( EnumResultCode ), result.ToString() );

						return TestResult.Failed( string.Format( "Call returned: {0}  ({1})", resCode.ToString(), result ) );
					}
				}

				if( !string.IsNullOrEmpty( ResultsProperty ) )
					SetProperty( ResultsProperty, results );

				return TestResult.Success();
			}
			catch( Exception ex )
			{
				return new TestResult( ex );
			}
		}
	}
}
