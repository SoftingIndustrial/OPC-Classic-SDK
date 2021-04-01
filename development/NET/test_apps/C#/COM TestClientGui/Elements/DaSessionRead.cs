using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;
using Softing.OPCToolbox;
using System.Collections.Generic;
using System.Collections;

namespace TestClientGui.Elements
{
	public class DaSessionRead : Element
	{
		#region Public Properties
		public string Session { get; set; }

		public List<string> ItemIds { get; set; }

		public List<string> ItemPaths { get; set; }

		public uint MaxAge { get; set; }

		public string ValuesProperty { get; set; }

		public string ResultsProperty { get; set; }

		public List<Value> ExpectedValues { get; set; }

		public string ExpectedServiceResult { get; set; }

		[DefaultValue( EnumExecutionType.SYNCHRONOUS )]
		public EnumExecutionType ExecutionType { get; set; }
		#endregion

		public DaSessionRead()
		{
			ExecutionType = EnumExecutionType.SYNCHRONOUS;
		}

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( Session ) )
				return TestResult.ParameterMissing( "Session" );

			DaSession session = GetProperty( Session ) as DaSession;
			if( session == null )
				return TestResult.ParameterInvalid( "Session" );

			try
			{
				int result = (int) EnumResultCode.S_FALSE;
				ValueQT[] values;
				int[] results;

				string[] itemIDs = ItemIds.ToArray();
				string[] itemPaths = ItemPaths.Count > 0 ? ItemPaths.ToArray() : null;

				if( ExecutionType == EnumExecutionType.SYNCHRONOUS )
				{
					result = session.Read( MaxAge, itemIDs, itemPaths, out values, out results, null );
				}
				else
				{
					m_asyncExecutionContext = DaExecutionContext.Next;
					ExecutionOptions executionOptions = new ExecutionOptions( EnumExecutionType.ASYNCHRONOUS, m_asyncExecutionContext );

					m_waitHandle = new System.Threading.AutoResetEvent( false );

					SessionReadEventHandler asyncHandler = new SessionReadEventHandler( session_ReadCompleted );
					session.ReadCompleted += asyncHandler;

					result = session.Read( MaxAge, itemIDs, itemPaths, out values, out results, executionOptions );

					if( m_waitHandle.WaitOne( 30000 ) )
					{
						result = m_asyncResult;
						values = m_asyncValues;
						results = m_asyncResults;
					}

					session.ReadCompleted -= asyncHandler;
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

							if( expectedValue.TimeStamp != null && expectedValue.TimeStamp != receivedValue.TimeStamp )
								return new DaReadTestResult( result, values, results, string.Format( "Unexpected result Timestamp for item #{0}", i ) );
						}
					}

					return new DaReadTestResult( result, values, results );
				}
				else
				{
					EnumResultCode resCode = (EnumResultCode)Enum.Parse(typeof(EnumResultCode), result.ToString());
					if (!string.IsNullOrEmpty(ExpectedServiceResult))
					{
						EnumResultCode expectedResCode = (EnumResultCode)Enum.Parse(typeof(EnumResultCode), ExpectedServiceResult.ToString());
						if (expectedResCode != resCode)
						{
							return TestResult.Failed(string.Format("Unexpected service result: expected {0}, actual {1}", resCode, expectedResCode));
						}

						return TestResult.Success();
					}

					return TestResult.Failed(string.Format("Call returned: {0}  ({1})", resCode.ToString(), result));
				}
			}
			catch( Exception ex )
			{
				return new TestResult( ex );
			}
		}

		void session_ReadCompleted( DaSession aDaSession, uint executionContext, string[] itemIDs, string[] itemPaths, ValueQT[] values, int[] results, int result )
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

	public class DaReadTestResult : TestResult
	{
		[XmlIgnore]
		public ValueQT[] Values
		{
			get
			{
				if( m_values == null )
					return Value.CreateSDKCollection( m_valuesWrapper );
				else
					return m_values;
			}

			protected set
			{
				m_values = value;
			}
		}

		[System.ComponentModel.Browsable( false )]
		[XmlArray( ElementName = "Values" )]
		public List<Value> ValuesWrapper
		{
			get
			{
				if( m_values == null )
					return m_valuesWrapper;
				else
					return Value.CreateList( m_values );
			}

			set
			{
				m_valuesWrapper = value;
			}
		}

		public int Result
		{
			get;
			set;
		}

		public int[] Results
		{
			get;
			set;
		}

		public DaReadTestResult()
		{
		}

		public DaReadTestResult( int result, ValueQT[] values, int[] results )
			: base( TestResults.Success )
		{
			Result = result;
			m_values = values;
			Results = results;
		}

		public DaReadTestResult( int result, ValueQT[] values, int[] results, string details )
			: base( TestResults.Failed )
		{
			Result = result;
			m_values = values;
			Results = results;
			Details = details;
		}

		private ValueQT[] m_values;
		private List<Value> m_valuesWrapper;
	}
}
