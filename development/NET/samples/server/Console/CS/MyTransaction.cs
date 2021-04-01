using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace Console
{
	public class MyTransaction : DaTransaction
	{
		#region Public Methods
		//--------------------

		public MyTransaction(
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
				DaRequest request = (DaRequest)Requests[i];
				if ((request != null) && (request.ProgressRequestState(EnumRequestState.CREATED, EnumRequestState.INPROGRESS) == true))
				{
					if (request.PropertyId == 0)
					{
						// get address space element value
						// take the Toolkit cache value
						ValueQT cacheValue = null;
						request.AddressSpaceElement.GetCacheValue(ref cacheValue);
						request.Value = cacheValue;
						request.Result = EnumResultCode.S_OK;
					}
					else
					{
						// get property value
						// get the value from the address space element
						MyDaAddressSpaceElement element = request.AddressSpaceElement as MyDaAddressSpaceElement;
						if (element != null)
						{
							element.GetPropertyValue(request);
						}
						else
						{
							request.Result = EnumResultCode.E_FAIL;
						}   //	end if ... else
					}   //	end if ... else
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
				if (request != null)
				{
					MyDaAddressSpaceElement element = (MyDaAddressSpaceElement)request.AddressSpaceElement;
					if (element != null)
					{
						ValueQT valueQT = request.Value;
						request.Result = (EnumResultCode)element.ValueChanged(valueQT);
					}   //	end if
				}   //	end if
			}   //	end for
			return CompleteRequests();
		}   //	end HandleWriteRequests

		//--
		#endregion
	}   //	end class MyTransaction
}   //	end namespace
