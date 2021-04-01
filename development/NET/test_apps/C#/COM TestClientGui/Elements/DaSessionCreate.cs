using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace TestClientGui.Elements
{
	public class DaSessionCreate : Element
	{
		#region Public Properties
		public string Url { get; set; }

		[DefaultValue( true )]
		public bool Connect { get; set; }

		[DefaultValue( true )]
		public bool Deep { get; set; }

		[DefaultValue( true )]
		public bool Active { get; set; }

		[DefaultValue( EnumOPCSpecification.DEFAULT )]
		public EnumOPCSpecification ForcedOpcSpecification { get; set; }

		public string Property { get; set; }

		[DefaultValue(true)]
		public bool ExpectedSuccess { get; set; }
		#endregion

		public DaSessionCreate()
		{
			ExpectedSuccess = true;
			Connect = true;
			Deep = true;
			Active = true;
		}

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( Url ) )
				return TestResult.ParameterMissing( "Url" );

			try
			{
				ExecutionOptions executionOptions = new ExecutionOptions( EnumExecutionType.SYNCHRONOUS, 0 );
				DaSession session = new DaSession( ExpandProperty( Url ) );

				if( !session.Valid )
					return TestResult.Failed( "DaSession not Valid" );

				// set the global prioperty no matter what
				SetProperty(Property, session);

				if (ForcedOpcSpecification != EnumOPCSpecification.DEFAULT)
				{
					session.ForcedOpcSpecification = ForcedOpcSpecification;
				}

				if( Connect )
				{
					int result = session.Connect( Deep, Active, executionOptions );

					if( ResultCode.FAILED( result ) )
					{
						EnumResultCode resCode = (EnumResultCode) Enum.Parse( typeof( EnumResultCode ), result.ToString() );

						return TestResult.Failed( string.Format( "Call returned: {0}  ({1})", resCode.ToString(), result ) );
					}
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
