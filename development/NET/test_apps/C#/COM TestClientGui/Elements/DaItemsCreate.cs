using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
using System.Xml.Serialization;
using System.Collections.Generic;

namespace TestClientGui.Elements
{
	public class DaItemsCreate : Element
	{
		#region Public Properties
		public string Subscription { get; set; }

		public List<string> ItemIds { get; set; }

		[DefaultValue( true )]
		public bool Connect { get; set; }

		[DefaultValue( true )]
		public bool Active { get; set; }

		public float? Deadband { get; set; }

		public string Property { get; set; }

		[DefaultValue( true )]
		public bool ExpectedSuccess { get; set; }
		#endregion

		public DaItemsCreate()
		{
			ExpectedSuccess = true;
			Connect = true;
			Active = true;
		}

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( Subscription ) )
				return TestResult.ParameterMissing( "Subscription" );

			DaSubscription subscription = GetProperty( Subscription ) as DaSubscription;
			if( subscription == null )
				return TestResult.ParameterInvalid( "Subscription" );

			try
			{
				ExecutionOptions executionOptions = new ExecutionOptions( EnumExecutionType.SYNCHRONOUS, 0 );
				int result;
				List<DaItem> items = new List<DaItem>( ItemIds.Count );

				foreach( string itemId in ItemIds )
				{
					DaItem item = new DaItem( itemId, subscription );

					if( !item.Valid )
						return TestResult.Failed( "Item not Valid" );

					if( Deadband.HasValue )
						item.Deadband = Deadband.Value;

					if( Connect )
					{
						result = item.Connect( false, Active, executionOptions );

						if( ResultCode.FAILED( result ) )
						{
							EnumResultCode resCode = (EnumResultCode) Enum.Parse( typeof( EnumResultCode ), result.ToString() );

							return TestResult.Failed( string.Format( "Call returned: {0}  ({1})", resCode.ToString(), result ) );
						}
					}

					items.Add( item );
				}

				SetProperty( Property, items.ToArray() );
			}
			catch( Exception ex )
			{
				if( ExpectedSuccess )
					return new TestResult( ex );
			}

			return TestResult.Success();
		}
	}
}
