using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;
using Softing.OPCToolbox;
using System.Collections.Generic;

namespace TestClientGui.Elements
{
	public class DaSubscriptionWaitForNotif : Element
	{
		#region Public Properties
		public string Subscription { get; set; }

		public int? WaitTime { get; set; }

		[DefaultValue( true )]
		public bool ExpectValues { get; set; }

		public string ValuesProperty { get; set; }

		public string ResultsProperty { get; set; }
		#endregion

		public DaSubscriptionWaitForNotif()
		{
			ExpectValues = true;
			m_dataChanges = new List<DaSubscriptionDataChange>();
		}

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( Subscription ) )
				return TestResult.ParameterMissing( "Subscription" );

			DaSubscription subscription = GetProperty( Subscription ) as DaSubscription;
			if( subscription == null )
				return TestResult.ParameterInvalid( "Subscription" );

			DataChangedEventHandler handler = new DataChangedEventHandler( subscription_DataChanged );
			subscription.DataChanged += handler;

			int waitTime = WaitTime.HasValue ? WaitTime.Value : GeSubscriptionSleepTime( subscription );
			bool bOverWait = false;

			System.Threading.Thread.Sleep( waitTime );

			if( ExpectValues && !m_bReceived )	//if expecting notifications and we don't have any, then wait some more
			{
				System.Threading.Thread.Sleep( waitTime );
				bOverWait = true;
			}

			subscription.DataChanged -= handler;

			if( m_bReceived )
			{
				if( !ExpectValues )
					return new DaSubscriptionWaitForNotifTestResult( TestResults.Failed, m_dataChanges, "Received unexpected data changes" );

                if (bOverWait)
                    return new DaSubscriptionWaitForNotifTestResult(TestResults.Warning, m_dataChanges, "Notifications received late");
                else
                {
                    string details = "Notifications per second ";
                    Int32 sum = 0;
                    for (UInt32 i = 0; i < m_dataChanges.Count; i++)
                    {
                        sum += m_dataChanges.ToArray()[i].ItemIDs.Length;
                    }

                    details += sum * 1000 / waitTime;
                    return new DaSubscriptionWaitForNotifTestResult(TestResults.Success, m_dataChanges, details);
                    
                }
			}
			else
			{
				if( ExpectValues )
					return TestResult.Failed( "Didn't receive expected data changes" );

				return TestResult.Success();
			}
		}

		void subscription_DataChanged( DaSubscription aDaSubscription, DaItem[] items, ValueQT[] values, int[] results )
		{
			//check for keep alive notifications
			if (items == null)
				return;
			if (items.Length == 0)
				return;
			if (items[0] == null)
				return;

			m_bReceived = true;
			m_dataChanges.Add( new DaSubscriptionDataChange( items, values, results ) );
		}

		public static int GeSubscriptionSleepTime( DaSubscription subscription )
		{
			int max = subscription.RevisedUpdateRate > subscription.RevisedKeepAliveTime ?
				(int) subscription.RevisedUpdateRate : (int) subscription.RevisedKeepAliveTime;

			int subscriptionSleep = max + (int) subscription.RevisedUpdateRate;

			return subscriptionSleep;
		}

		public override void ClearResults()
		{
			base.ClearResults();

			m_bReceived = false;
			m_dataChanges.Clear();
		}

		#region Private Fields
		bool m_bReceived;
		List<DaSubscriptionDataChange> m_dataChanges;
		#endregion
	}

	//support class
	[TypeConverter( typeof( ExpandableObjectConverter ) )]
	public class DaSubscriptionDataChange
	{
		public string[] ItemIDs { get; set; }
		public Value[] Values { get; set; }
		public int[] Results { get; set; }
		public DateTime ReceivingTime { get; set; }

		public DaSubscriptionDataChange()
		{
		}

		public DaSubscriptionDataChange( DaItem[] items, ValueQT[] values, int[] results )
		{
			ReceivingTime = DateTime.Now;

			ItemIDs = new string[items.Length];

			for( int i = 0; i < items.Length; i++ )
				ItemIDs[i] = items[i].Id;

			Values = new Value[values.Length];

			for( int i = 0; i < values.Length; i++ )
				Values[i] = new Value( values[i] );

			Results = results;
		}
	}

	public class DaSubscriptionWaitForNotifTestResult : TestResult
	{
		public List<DaSubscriptionDataChange> DataChanges
		{
			get;
			set;
		}

		public DaSubscriptionWaitForNotifTestResult()
		{
		}

		public DaSubscriptionWaitForNotifTestResult( List<DaSubscriptionDataChange> dataChanges )
			: base( TestResults.Success )
		{
			DataChanges = dataChanges;
		}

		public DaSubscriptionWaitForNotifTestResult( TestResults type, List<DaSubscriptionDataChange> dataChanges, string details )
			: base( type )
		{
			DataChanges = dataChanges;
			Details = details;
		}
	}
}
