using System;
using System.Collections.Generic;
using System.Text;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace TestClientGui
{
	class TestClient
	{
		internal static int Initialize()
		{
			// activating license


			int result = ApplicationInstance.Initialize();



			if( !ResultCode.SUCCEEDED( result ) )
			{
				return result;
			}



			if( ResultCode.SUCCEEDED( result ) )
			{
				//	enable toolkit internal initialization
				ApplicationInstance.EnableTracing(
					EnumTraceGroup.ALL,
					EnumTraceGroup.ALL,
					EnumTraceGroup.CLIENT,
					EnumTraceGroup.CLIENT,
					"Trace.txt",
					1000000,
					0 );
			}	//	end if

            result = ApplicationInstance.Activate(EnumFeature.DA_CLIENT, "0ed0-0000-0000-a180-2a47");
            result = ApplicationInstance.Activate(EnumFeature.XMLDA_CLIENT, "0ef0-0000-0000-4ada-4936");

			return (int) EnumResultCode.S_OK;
		}

		internal static Application ApplicationInstance
		{
			get
			{
				return Application.Instance;
			}
		}
	}
}
