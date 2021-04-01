using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace DemoServer
{
	public class Transaction : DaTransaction
	{
		#region Public Methods
		//--------------------

		public Transaction(
			EnumTransactionType aTransactionType,
			DaRequest[] requestList,
			uint aSessionKey)
			:
			base(
			aTransactionType,
			requestList,
			aSessionKey)
		{
		}   //	end constructor

		public override int HandleReadRequests()
		{
			int count = Requests.Count;
			for (int i = 0; i < count; i++)
			{
				DaRequest request = Requests[i] as DaRequest;
				if ((request != null) && (request.ProgressRequestState(EnumRequestState.CREATED, EnumRequestState.INPROGRESS) == true))
				{
					DemoDaAddressSpaceElement element = (DemoDaAddressSpaceElement)request.AddressSpaceElement;
					if (element != null)
					{
						element.HandleReadRequest(request);
					}
					else
					{
						request.Result = EnumResultCode.E_FAIL;
					}
				}   //	end if
			}   //	end for
			return CompleteRequests();
		}   //	end HandleReadRequests

		public override int HandleWriteRequests()
		{
			int count = Requests.Count;
			for (int i = 0; i < count; i++)
			{
				DaRequest request = Requests[i] as DaRequest;
				if ((request != null) && (request.ProgressRequestState(EnumRequestState.CREATED, EnumRequestState.INPROGRESS) == true))
				{
					DemoDaAddressSpaceElement element = (DemoDaAddressSpaceElement)request.AddressSpaceElement;
					if (element != null)
					{
						element.HandleWriteRequest(request);
					}
					else
					{
						request.Result = EnumResultCode.E_FAIL;
					}
				}   //	end if
			}   //	end for
			return CompleteRequests();
		}   //	end HandleWriteRequests

		//--
		#endregion
	}   //	end class MyTransaction
}   //	end namespace
