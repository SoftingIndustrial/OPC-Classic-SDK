using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;

namespace TestClientGui.Elements
{
	public class DaSessionRemove : Element
	{
		#region Public Properties
		public string Session { get; set; }
		#endregion

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( Session ) )
				return TestResult.ParameterMissing( "Session" );

			DaSession session = GetProperty( Session ) as DaSession;
			if( session == null )
				return TestResult.ParameterInvalid( "Session" );

			try
			{
				int result = session.Disconnect( new ExecutionOptions() );

				TestClient.ApplicationInstance.RemoveDaSession( session );
			}
			catch( Exception ex )
			{
				return new TestResult( ex );
			}

			UnsetProperty( Session );

			return TestResult.Success();
		}
	}
}
