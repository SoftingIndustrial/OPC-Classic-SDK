using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;
using Softing.OPCToolbox;
using System.Collections.Generic;

namespace TestClientGui.Elements
{
	public class DaSubscriptionRefresh : Element
	{
		#region Public Properties
		public string Subscription { get; set; }

		public uint MaxAge { get; set; }
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
				int result = subscription.Refresh( MaxAge, new ExecutionOptions() );

				if( ResultCode.SUCCEEDED( result ) )
					return TestResult.Success();
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
	}
}
