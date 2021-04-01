using Softing.OPCToolbox.Server;

namespace DemoServer
{
	public class Request : DaRequest
	{
		#region Constructor
		//-----------------

		public Request(
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

		}   //	end constructor

		//--
		#endregion

	}   //	end MyRequest class
}   //	ens namespace
