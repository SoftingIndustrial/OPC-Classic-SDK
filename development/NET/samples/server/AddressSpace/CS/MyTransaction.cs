using System;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace AddressSpace
{
	public class MyTransaction : DaTransaction
	{

		public MyTransaction(
			EnumTransactionType aTransactionType,
			DaRequest[] aRequests,
			uint aSessionKey) :
			base(aTransactionType, aRequests, aSessionKey)
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
					if (request.PropertyId == 0)
					{
						// get address space element value
						// take the toolkit cache value
						ValueQT valueQT = null;
						request.AddressSpaceElement.GetCacheValue(ref valueQT);
						request.Value = valueQT;
						request.Result = EnumResultCode.S_OK;
					}
					else
					{
						// get property value
						// get the value from the address space element
						MyDaAddressSpaceElement element = request.AddressSpaceElement as MyDaAddressSpaceElement;
						request.Value = new ValueQT(element.ObjectType, EnumQuality.GOOD, DateTime.Now);
						request.Result = EnumResultCode.S_OK;
					}   //	end if ... else
				}   //	end if
			}   //	end for

			return CompleteRequests();

		}   //	end HandleReadRequests

		public override int HandleWriteRequests()
		{

			int result = ValuesChanged();
			if (ResultCode.FAILED(result))
			{
				return result;
			}   //	end if

			return CompleteRequests();

		}   //	end HandleWriteRequests
	}   //	end class MyTransaction
}   //	end namespace
