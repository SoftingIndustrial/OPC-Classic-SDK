using System;
using System.ComponentModel;
using Softing.OPCToolbox.Client;
using System.Xml.Serialization;
using Softing.OPCToolbox;

namespace TestClientGui.Elements
{
	public class Disconnect : Element
	{
		#region Public Properties
		[XmlElement( ElementName = "Object" )]
		public string ObjectProperty { get; set; }
		#endregion

		public override TestResult Execute()
		{
			if( string.IsNullOrEmpty( ObjectProperty ) )
				return TestResult.ParameterMissing( "Object" );

			ObjectSpaceElement element = GetProperty( ObjectProperty ) as ObjectSpaceElement;
			if( element == null )
				return TestResult.ParameterInvalid( "Object" );

			try
			{
				int result = element.Disconnect( new ExecutionOptions() );

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
