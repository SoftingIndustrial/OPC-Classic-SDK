using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
using System.Xml.Serialization;

namespace TestClientGui.Elements
{
	public class DaSubscriptionCreate : Element
	{
		#region Public Properties
		public string Session { get; set; }

		[DefaultValue( 1000 )]
		public uint UpdateRate { get; set; }

		[DefaultValue( true )]
		public bool Connect { get; set; }

		[DefaultValue( true )]
		public bool Deep { get; set; }

		[DefaultValue( true )]
		public bool Active { get; set; }

		public uint? RequestedKeepAliveTime { get; set; }

		public float? Deadband { get; set; }

		public string Property { get; set; }

		[DefaultValue( true )]
		public bool ExpectedSuccess { get; set; }
		#endregion

		public DaSubscriptionCreate()
		{
			UpdateRate = 1000;
			ExpectedSuccess = true;
			Connect = true;
			Deep = true;
			Active = true;
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
				DaSubscription subscription = new DaSubscription( UpdateRate, session );

				if (!subscription.Valid)
				{
					return TestResult.Failed("DaSubscription not Valid");
				}

				if( RequestedKeepAliveTime.HasValue )
					subscription.RequestedKeepAliveTime = RequestedKeepAliveTime.Value;

				// set the global property anyhow.
				SetProperty(Property, subscription);

				if( Connect )
				{
					int result = subscription.Connect( Deep, Active, null );

					if( ResultCode.FAILED( result ) )
					{
						EnumResultCode resCode = (EnumResultCode) Enum.Parse( typeof( EnumResultCode ), result.ToString() );

						return TestResult.Failed( string.Format( "Call returned: {0}  ({1})", resCode.ToString(), result ) );
					}
				}

				if (Deadband.HasValue)
				{
					subscription.Deadband = Deadband.Value;
				}
			}
			catch( Exception ex )
			{
				if (ExpectedSuccess)
				{
					return new TestResult(ex);
				}
			}

			return TestResult.Success();
		}
	}
}
