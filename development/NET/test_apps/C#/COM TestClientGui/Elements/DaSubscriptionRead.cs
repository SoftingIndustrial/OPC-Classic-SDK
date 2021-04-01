using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;
using Softing.OPCToolbox;
using System.Collections.Generic;
using System.Collections;

namespace TestClientGui.Elements
{
	public class DaSubscriptionRead : Element
	{
		#region Public Properties
		public string ItemsProperty { get; set; }

		public uint MaxAge { get; set; }

		public string ValuesProperty { get; set; }

		public string ResultsProperty { get; set; }

		public List<Value> ExpectedValues { get; set; }

		[DefaultValue( EnumExecutionType.SYNCHRONOUS )]
		public EnumExecutionType ExecutionType { get; set; }
		#endregion

		public DaSubscriptionRead()
		{
			ExecutionType = EnumExecutionType.SYNCHRONOUS;
		}

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( ItemsProperty ) )
				return TestResult.ParameterMissing( "ItemsProperty" );

			DaItem[] items = GetProperty( ItemsProperty ) as DaItem[];
			if( items == null || items.Length == 0 )
				return TestResult.ParameterInvalid( "ItemsProperty" );

			DaSubscription subscription = items[0].DaSubscription;

			try
			{
				int result = (int) EnumResultCode.S_FALSE;
				ValueQT[] values;
				int[] results;

				if( ExecutionType == EnumExecutionType.SYNCHRONOUS )
				{
					result = subscription.Read( MaxAge, items, out values, out results, null );
				}
				else
				{
					m_asyncExecutionContext = DaExecutionContext.Next;
					ExecutionOptions executionOptions = new ExecutionOptions( EnumExecutionType.ASYNCHRONOUS, m_asyncExecutionContext );

					m_waitHandle = new System.Threading.AutoResetEvent( false );

					SubscriptionReadEventHandler asyncHandler = new SubscriptionReadEventHandler( subscription_ReadCompleted );
					subscription.ReadCompleted += asyncHandler;

					result = subscription.Read( MaxAge, items, out values, out results, executionOptions );

					if( m_waitHandle.WaitOne( 30000 ) )
					{
						result = m_asyncResult;
						values = m_asyncValues;
						results = m_asyncResults;
					}

					subscription.ReadCompleted -= asyncHandler;
				}

				if( ResultCode.SUCCEEDED( result ) )
				{
					if( !string.IsNullOrEmpty( ValuesProperty ) )
						SetProperty( ValuesProperty, values );

					if( !string.IsNullOrEmpty( ResultsProperty ) )
						SetProperty( ResultsProperty, results );


					//validate with expected values
					if( ExpectedValues.Count > 0 )
					{
						if( ExpectedValues.Count != values.Length )
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
								return new DaReadTestResult( result, values, results, string.Format( "Unexpected result Quality for item #{0}", i ) );

							//BUG in TB 4.0
							//if( expectedValue.TimeStamp != null && expectedValue.TimeStamp != receivedValue.TimeStamp )
							//	return new DaReadTestResult( result, values, results, string.Format( "Unexpected result Timestamp for item #{0}", i ) );
						}
					}

					return new DaReadTestResult( result, values, results );
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

		void subscription_ReadCompleted( DaSubscription aDaSubscription, uint executionContext, DaItem[] items, ValueQT[] values, int[] results, int result )
		{
			if( executionContext == m_asyncExecutionContext )
			{
				m_asyncValues = values;
				m_asyncResults = results;
				m_asyncResult = result;

				m_waitHandle.Set();
			}
		}

		private System.Threading.AutoResetEvent m_waitHandle;
		private uint m_asyncExecutionContext;
		private int[] m_asyncResults;
		private int m_asyncResult;
		private ValueQT[] m_asyncValues;
	}
}
