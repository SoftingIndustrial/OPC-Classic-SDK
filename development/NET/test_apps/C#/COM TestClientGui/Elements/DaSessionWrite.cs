using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;
using Softing.OPCToolbox;
using System.Collections.Generic;

namespace TestClientGui.Elements
{
	public class DaSessionWrite : Element
	{
		#region Public Properties
		public string Session { get; set; }

		public List<string> ItemIds { get; set; }

		public List<string> ItemPaths { get; set; }

		public List<Value> Values { get; set; }

		public string ValuesProperty { get; set; }

		public string ResultsProperty { get; set; }

		[DefaultValue(true)]
		public bool ValidateAllResults { get; set; }

		public List<int> ExpectedResults { get; set; }

		public string ExpectedServiceResult { get; set; }

		[DefaultValue( EnumExecutionType.SYNCHRONOUS )]
		public EnumExecutionType ExecutionType { get; set; }
		#endregion

		public DaSessionWrite()
		{
			ValidateAllResults = true;
			ExecutionType = EnumExecutionType.SYNCHRONOUS;
		}

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( Session ) )
				return TestResult.ParameterMissing( "Session" );

			DaSession session = GetProperty( Session ) as DaSession;
			if( session == null )
				return TestResult.ParameterInvalid( "Session" );

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
				//if the quality was not set ; set it to the value QUALITY_NOT_SET
				for (int i = 0; i < Values.Count; i++)
				{
					if (Values[i].Wrapped.Quality == Softing.OPCToolbox.EnumQuality.BAD_WAITING_FOR_INITIAL_DATA)
						Values[i].Wrapped.SetData(Values[i].Wrapped.Data, Softing.OPCToolbox.EnumQuality.QUALITY_NOT_SET, Values[i].Wrapped.TimeStamp);
				}
				values = Value.CreateSDKCollection( Values );
			}

			try
			{
				int result = (int) EnumResultCode.S_FALSE;
				int[] results;

				string[] itemIDs = ItemIds.ToArray();
				string[] itemPaths = ItemPaths.Count > 0 ? ItemPaths.ToArray() : null;

				if( ExecutionType == EnumExecutionType.SYNCHRONOUS )
				{
					result = session.Write( itemIDs, itemPaths, values, out results, null );
				}
				else
				{
					m_asyncExecutionContext = DaExecutionContext.Next;
					ExecutionOptions executionOptions = new ExecutionOptions( EnumExecutionType.ASYNCHRONOUS, m_asyncExecutionContext );

					m_waitHandle = new System.Threading.AutoResetEvent( false );

					SessionWriteEventHandler asyncHandler = new SessionWriteEventHandler( session_WriteCompleted );
					session.WriteCompleted += asyncHandler;

					result = session.Write( itemIDs, itemPaths, values, out results, executionOptions );

					if( m_waitHandle.WaitOne( 30000 ) )
					{
						result = m_asyncResult;
						results = m_asyncResults;
					}

					session.WriteCompleted -= asyncHandler;
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
					if (!string.IsNullOrEmpty(ExpectedServiceResult))
					{
						EnumResultCode expectedResCode = (EnumResultCode)Enum.Parse(typeof(EnumResultCode), ExpectedServiceResult.ToString());
						if (expectedResCode != resCode)
						{
							return TestResult.Failed(string.Format("Unexpected service result: expected {0}, actual {1}", resCode, expectedResCode));
						}
						return TestResult.Success();
					}

					return TestResult.Failed( string.Format( "Call returned: {0}  ({1})", resCode.ToString(), result ) );
				}
			}
			catch( Exception ex )
			{
				return new TestResult( ex );
			}
		}

		void session_WriteCompleted( DaSession aDaSession, uint executionContext, string[] itemIDs, string[] itemPaths, ValueQT[] values, int[] results, int result )
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

	public class DaWriteTestResult : TestResult
	{
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

		public DaWriteTestResult()
		{
		}

		public DaWriteTestResult( int result, int[] results )
			: base( TestResults.Success )
		{
			Result = result;
			Results = results;
		}

		public DaWriteTestResult( int result, int[] results, string details )
			: base( TestResults.Failed )
		{
			Result = result;
			Results = results;
			Details = details;
		}
	}
}
