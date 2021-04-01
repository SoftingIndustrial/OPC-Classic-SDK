using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;


namespace TestClientGui.Elements.StressTests
{
	public class StressServer:Element
	{
#region Public Properties

		public string Url { get; set; }

		[DefaultValue(0)]
		public uint SessionsCount { get; set; }

		[DefaultValue(0)]
		public uint SubscriptionsCount { get; set; }

		[DefaultValue(1000)]
		public uint SubscriptionRate { get; set; }

		[DefaultValue(200)]
		public uint SubscriptionRateDeviation { get; set; }

		[DefaultValue(0)]
		public uint ItemsCount { get; set; }

		public bool ExpectedSuccess;
#endregion

		public StressServer()
		{
			ExpectedSuccess = true;
		}

		public override TestResult Execute()
		{
			if (string.IsNullOrEmpty(Url))
				return TestResult.ParameterMissing("Url");
		try
			{
				ExecutionOptions executionOptions = new ExecutionOptions( EnumExecutionType.SYNCHRONOUS, 0 );
				for (uint sessionCnt = 0; sessionCnt < SessionsCount; sessionCnt++)
				{
					DaSession session = new DaSession( ExpandProperty( Url ) );

					for (uint subscriptionCnt = 0; subscriptionCnt < SubscriptionsCount; subscriptionCnt++)
					{
						DaSubscription subscription = new DaSubscription(SubscriptionRate , session);

						for (uint itemCnt = 0; itemCnt < ItemsCount; itemCnt++)
						{
							string itemID = "Test Folder.TestItems_0.TV_" + itemCnt.ToString();
							DaItem item = new DaItem("item", subscription);
						}
					}
					session.Connect(true, true, executionOptions);
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
