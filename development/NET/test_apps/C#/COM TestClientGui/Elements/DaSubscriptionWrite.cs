using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;
using Softing.OPCToolbox;
using System.Collections.Generic;

namespace TestClientGui.Elements
{
	public class DaSubscriptionWrite : Element
	{
		#region Public Properties
		public string ItemsProperty { get; set; }

		public List<Value> Values { get; set; }

		public string ValuesProperty { get; set; }

		public string ResultsProperty { get; set; }

		[DefaultValue( true )]
		public bool ValidateAllResults { get; set; }

		public List<int> ExpectedResults { get; set; }

		[DefaultValue( EnumExecutionType.SYNCHRONOUS )]
		public EnumExecutionType ExecutionType { get; set; }
		#endregion

		public DaSubscriptionWrite()
		{
			ValidateAllResults = true;
			ExecutionType = EnumExecutionType.SYNCHRONOUS;
		}

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( ItemsProperty ) )
				return TestResult.ParameterMissing( "ItemsProperty" );

			DaItem[] items = GetProperty( ItemsProperty ) as DaItem[];
			if( items == null )
				return TestResult.ParameterInvalid( "ItemsProperty" );

			DaSubscription subscription = items[0].DaSubscription;

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
				int result = (int) EnumResultCode.S_FALSE;
				int[] results = null;

				if( ExecutionType == EnumExecutionType.SYNCHRONOUS )
				{
					result = subscription.Write( items, values, out results, null );
				}
				else
				{
					m_asyncExecutionContext = DaExecutionContext.Next;
					ExecutionOptions executionOptions = new ExecutionOptions( EnumExecutionType.ASYNCHRONOUS, m_asyncExecutionContext );

					m_waitHandle = new System.Threading.AutoResetEvent( false );

					SubscriptionWriteEventHandler asyncHandler = new SubscriptionWriteEventHandler( subscription_WriteCompleted );
					subscription.WriteCompleted += asyncHandler;

					result = subscription.Write( items, values, out results, executionOptions );

					if( m_waitHandle.WaitOne( 30000 ) )
					{
						result = m_asyncResult;
						results = m_asyncResults;
					}

					subscription.WriteCompleted -= asyncHandler;
				}

				if( ResultCode.SUCCEEDED( result ) )
				{
					if( !string.IsNullOrEmpty( ResultsProperty ) )
						SetProperty( ResultsProperty, results );

					if( ValidateAllResults )
					{
						for( int i = 0; i < results.Length; i++ )
						{
							if( ResultCode.FAILED( results[i] ) )
							{
								EnumResultCode resCode = (EnumResultCode) Enum.Parse( typeof( EnumResultCode ), results[i].ToString() );

								return TestResult.Failed( string.Format( "Result {0} failed with the code: {1}  ({2})", i, resCode.ToString(), results[i] ) );
							}
						}
					}
					else if( ExpectedResults.Count > 0 )
					{
						//validate with expected values
						if( ExpectedResults.Count != results.Length )
							return new DaWriteTestResult( result, results, "Unexpected number of results" );

						for( int i = 0; i < results.Length; i++ )
						{
							if( results[i] != ExpectedResults[i] )
							{
								EnumResultCode resCode = (EnumResultCode) Enum.Parse( typeof( EnumResultCode ), results[i].ToString() );
								EnumResultCode resExpectedCode = (EnumResultCode) Enum.Parse( typeof( EnumResultCode ), ExpectedResults[i].ToString() );

								return new DaWriteTestResult( result, results, string.Format( "Unexpected result code for item {0}: received {1}  ({2}) instead of {3}  ({4})", i, resCode.ToString(), results[i], resExpectedCode.ToString(), ExpectedResults[i] ) );
							}
						}
					}

					return new DaWriteTestResult( result, results );
				}
				else
				{
					EnumResultCode resCode = (EnumResultCode) Enum.Parse( typeof( EnumResultCode ), result.ToString() );

					return TestResult.Failed( string.Format( "Call returned: {0}  ({1})", resCode.ToString(), result ) );
				}
			}
			catch( Exception ex )
			{
				return new TestResult( ex );
			}
		}

		void subscription_WriteCompleted( DaSubscription aDaSubscription, uint executionContext, DaItem[] items, ValueQT[] values, int[] results, int result )
		{
			if( executionContext == m_asyncExecutionContext )
			{
				m_asyncResults = results;
				m_asyncResult = result;

				m_waitHandle.Set();
			}
		}

		private System.Threading.AutoResetEvent m_waitHandle;
		private uint m_asyncExecutionContext;
		private int[] m_asyncResults;
		private int m_asyncResult;
	}
}
