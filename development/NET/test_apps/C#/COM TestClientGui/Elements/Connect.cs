using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;
using Softing.OPCToolbox;

namespace TestClientGui.Elements
{
	public class Connect : Element
	{
		#region Public Properties
		[XmlElement( ElementName = "Object" )]
		public string ObjectProperty { get; set; }

		[DefaultValue( true )]
		public bool Deep { get; set; }

		[DefaultValue( true )]
		public bool Active { get; set; }
		#endregion

		public Connect()
		{
			Deep = true;
			Active = true;
		}

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( ObjectProperty ) )
				return TestResult.ParameterMissing( "Object" );

			ObjectSpaceElement element = GetProperty( ObjectProperty ) as ObjectSpaceElement;
			ObjectSpaceElement[] elements = null;

			if( element == null )
			{
				elements = GetProperty( ObjectProperty ) as DaItem[];

				if( elements == null )
					return TestResult.ParameterInvalid( "Object" );
			}

			try
			{
				int result = (int) EnumResultCode.E_FAIL;

				if( element != null )
				{
					result = element.Connect( Deep, Active, new ExecutionOptions() );
				}
				else
				{
					foreach( ObjectSpaceElement el in elements )
					{
						result = el.Connect( Deep, Active, new ExecutionOptions() );

						if( !ResultCode.SUCCEEDED( result ) )
							break;
					}
				}

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
