using System;
using System.Collections;
using System.Text;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace TestServer
{
	public class MyRequest : DaRequest
	{
		#region Constructor
		//-----------------

		public MyRequest(
			EnumTransactionType transactionType,
            uint sessionHandle,
			DaAddressSpaceElement aDaAddressSpaceElement,
			int propertyID,
            uint requestHandle)
			:
			base(
				transactionType,
				sessionHandle,
				aDaAddressSpaceElement,
				propertyID,
				requestHandle)
		{

		}	//	end constructor

		//--
		#endregion

	}	//	end MyRequest class
}	//	ens namespace
