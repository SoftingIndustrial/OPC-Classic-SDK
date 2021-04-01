using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
using System.Xml.Serialization;

namespace TestClientGui.Elements
{
	public class DaSubscriptionUpdate : Element
	{
		#region Public Properties
		public string Subscription { get; set; }

		public uint? UpdateRate { get; set; }

		public uint? RequestedKeepAliveTime { get; set; }

		public float? Deadband { get; set; }
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
				if( !subscription.Valid )
					return TestResult.Failed( "DaSubscription not Valid" );

				if( UpdateRate.HasValue )
					subscription.RequestedUpdateRate = UpdateRate.Value;

				if( RequestedKeepAliveTime.HasValue )
					subscription.RequestedKeepAliveTime = RequestedKeepAliveTime.Value;

				if( Deadband.HasValue )
					subscription.Deadband = Deadband.Value;
			}
			catch( Exception ex )
			{
				return new TestResult( ex );
			}

			return TestResult.Success();
		}
	}
}
