using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;

namespace TestClientGui.Elements
{
	public class DaItemsRemove : Element
	{
		#region Public Properties
		public string ItemsProperty { get; set; }
		#endregion

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( ItemsProperty ) )
				return TestResult.ParameterMissing( "ItemsProperty" );

			DaItem[] items = GetProperty( ItemsProperty ) as DaItem[];
			if( items == null )
				return TestResult.ParameterInvalid( "ItemsProperty" );

			try
			{
				foreach( DaItem item in items )
				{
					int result = item.Disconnect( new ExecutionOptions() );

					item.DaSubscription.RemoveDaItem( item );
				}
			}
			catch( Exception ex )
			{
				return new TestResult( ex );
			}

			UnsetProperty( ItemsProperty );

			return TestResult.Success();
		}
	}
}
