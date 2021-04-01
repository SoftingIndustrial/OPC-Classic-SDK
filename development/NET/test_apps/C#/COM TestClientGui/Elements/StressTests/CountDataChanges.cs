using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
//using System.Timers;
using System.Threading;


namespace TestClientGui.Elements.StressTests
{

	public class MyDaSubscription:DaSubscription
	{
		public MyDaSubscription(uint subscriptionId, uint updateRate, DaSession parentSession):
			base(updateRate, parentSession)
		{
			m_subscriptionId = subscriptionId;
		}
		#region Public Properties

		public uint m_subscriptionId;

		#endregion
	}

	public class CountDataChanges:Element
	{
	#region Public Properties

		public string Url { get; set; }

		[DefaultValue(1000)]
		public uint SubscriptionRate  { get; set; }

		[DefaultValue(1)]
		public uint SubscriptionCount { get; set; }

		[DefaultValue(1)]
		public uint ItemsCountPerSubscription { get; set; }

		[DefaultValue("Test Folder.TestItems_0")]
		public string ParentNode { get; set; }

		public int TimeInterval { get; set; }

	#endregion

		#region Private Properties

		private static object s_locker = new object();

		private static long s_counts;
		private static long[] s_dataChangesPerSubscription;

		#endregion

		public override TestResult Execute()
		{
			CountDataChanges.s_counts = 0;

			ExecutionOptions executionOptions = new ExecutionOptions(EnumExecutionType.SYNCHRONOUS, 0);
			if (TimeInterval == 0)
			{
				return TestResult.ParameterMissing("TimeInterval");
			}

			if (string.IsNullOrEmpty(Url))
			{
				return TestResult.ParameterMissing("Session");
			}


			if (string.IsNullOrEmpty(ParentNode))
			{
				ParentNode = "Test Folder.TestItems_0";
			}
			try
			{
				int res;
				DaSession session = new DaSession(ExpandProperty(Url));
				DaSubscription[] subscription = new MyDaSubscription[SubscriptionCount];
				CountDataChanges.s_dataChangesPerSubscription = new long[SubscriptionCount];
				
				uint itemCount = 0;
				MyDaSubscription currentSubscription;
				for (uint subCount = 0; subCount < SubscriptionCount; ++subCount)
				{
					currentSubscription = new MyDaSubscription(subCount, SubscriptionRate, session);
					subscription[subCount] = currentSubscription;
					CountDataChanges.s_dataChangesPerSubscription[subCount] = 0;
					for (uint i = 0; i < ItemsCountPerSubscription; ++i)
					{
						string itemID = ParentNode + ".TV_" + itemCount.ToString();
						DaItem item = new DaItem(itemID, currentSubscription);
						++itemCount;
					}
				}

				for (uint i = 0; i < SubscriptionCount; ++i)
				{
					subscription[i].DataChanged += new DataChangedEventHandler(HandleDataChanged);
				}

				res = session.Connect(true, true, executionOptions);

				System.Threading.Thread.Sleep(TimeInterval);

				session.Disconnect(executionOptions);

				string datachangesPerSubscription = "";
				for (uint i = 0; i < SubscriptionCount; ++i)
				{
					datachangesPerSubscription += " | Sub#" + i.ToString() + ": " + CountDataChanges.s_dataChangesPerSubscription[i].ToString();
				}
				datachangesPerSubscription = "Total: " + CountDataChanges.s_counts.ToString() + datachangesPerSubscription;
				return TestResult.Failed(datachangesPerSubscription);

			}
			catch (Exception ex)
			{
				return new TestResult(ex);
			}
		}

		public static void HandleDataChanged(
			DaSubscription daSubscription,
			DaItem[] items,
			ValueQT[] values,
			int[] results)
		{
			MyDaSubscription mySub = daSubscription as MyDaSubscription;
			lock (CountDataChanges.s_locker)
			{
				CountDataChanges.s_dataChangesPerSubscription[mySub.m_subscriptionId] += items.Length;
				CountDataChanges.s_counts += items.Length;
			}
		}

	}
}
