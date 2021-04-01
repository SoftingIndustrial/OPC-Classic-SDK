using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;
using Softing.OPCToolbox;
using System.Collections.Generic;

namespace TestClientGui.Elements
{
	public class DaItemsWaitForNotif : Element
	{
		#region Public Properties
		public string ItemsProperty { get; set; }

		[DefaultValue( true )]
		public bool ExpectValues { get; set; }

		public string ValuesProperty { get; set; }

		public string ResultsProperty { get; set; }
		#endregion

		public DaItemsWaitForNotif()
		{
			ExpectValues = true;
		}

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( ItemsProperty ) )
				return TestResult.ParameterMissing( "ItemsProperty" );

			m_items = GetProperty( ItemsProperty ) as DaItem[];
			if( m_items == null )
				return TestResult.ParameterInvalid( "ItemsProperty" );

			m_valueChanges = new DaItemValueChange[m_items.Length];
			for( int i = 0; i < m_items.Length; i++ )
				m_valueChanges[i] = new DaItemValueChange( m_items[i], new List<Value>() );

			ValueChangedEventHandler handler = new ValueChangedEventHandler( item_ValueChanged );
			foreach( DaItem item in m_items )
			{
				item.ValueChanged += handler;
			}

			int waitTime = GeSubscriptionSleepTime( m_items[0].DaSubscription );

			System.Threading.Thread.Sleep( waitTime );

			foreach( DaItem item in m_items )
			{
				item.ValueChanged -= handler;
			}

			if( m_bReceived )
			{
				if( !ExpectValues )
					return TestResult.Failed( "Received unexpected data changes" );

				return new DaItemsWaitForNotifTestResult( new List<DaItemValueChange>( m_valueChanges ) );
			}
			else
			{
				if( ExpectValues )
					return TestResult.Failed( "Didn't receive expected data changes" );

				return TestResult.Success();
			}
		}

		void item_ValueChanged( DaItem aDaItem, ValueQT aValue )
		{
			m_bReceived = true;

			//find the right item
			for( int i = 0; i < m_items.Length; i++ )
			{
				if( m_items[i] == aDaItem )
				{
					m_valueChanges[i].Values.Add( new Value( aValue ) );
				}
			}
		}

		public static int GeSubscriptionSleepTime( DaSubscription subscription )
		{
			uint subscriptionSleep = subscription.RevisedUpdateRate;

			return 2 * (int) subscriptionSleep;
		}

		#region Private Fields
		bool m_bReceived;
		DaItemValueChange[] m_valueChanges;
		DaItem[] m_items;
		#endregion
	}

	//support class
	[TypeConverter( typeof( ExpandableObjectConverter ) )]
	public class DaItemValueChange
	{
		public string ItemID { get; set; }
		public List<Value> Values { get; set; }

		public DaItemValueChange()
		{
		}

		public DaItemValueChange( DaItem item, List<Value> values )
		{
			ItemID = item.Id;

			Values = values;
		}
	}

	public class DaItemsWaitForNotifTestResult : TestResult
	{
		public List<DaItemValueChange> ValueChanges
		{
			get;
			set;
		}

		public DaItemsWaitForNotifTestResult()
		{
		}

		public DaItemsWaitForNotifTestResult( List<DaItemValueChange> valueChanges )
			: base( TestResults.Success )
		{
			ValueChanges = valueChanges;
		}
	}
}
