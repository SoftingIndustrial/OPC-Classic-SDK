using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;

namespace TestClientGui.Elements
{
	public class DaSubscriptionRemove : Element
	{
		#region Public Properties
		public string Subscription { get; set; }
		#endregion

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( Subscription ) )
				return TestResult.ParameterMissing( "Subscription" );

			DaSubscription subscription = GetProperty( Subscription ) as DaSubscription;
			if( subscription == null )
				return TestResult.ParameterInvalid( "Subscription" );

			try
			{
				int result = subscription.Disconnect( new ExecutionOptions() );

				subscription.DaSession.RemoveDaSubscription( subscription );
			}
			catch( Exception ex )
			{
				return new TestResult( ex );
			}

			UnsetProperty( Subscription );

			return TestResult.Success();
		}
	}
}
