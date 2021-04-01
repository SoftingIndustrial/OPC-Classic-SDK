using System;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace UpdateMode
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
					MyDaAddressSpaceElement element = request.AddressSpaceElement as MyDaAddressSpaceElement;

					if (element == null)
					{
						request.Result = EnumResultCode.E_FAIL;
					}
					else
					{
						if (request.PropertyId == 0)
						{
							if (element.Type == MyDaAddressSpaceElement.TYPE_POLL_SEC)
							{
								// get address space element value
								DateTime now = DateTime.Now;
								ValueQT cacheValue = new ValueQT(now.Second, EnumQuality.GOOD, now);
								request.Value = cacheValue;
								request.Result = EnumResultCode.S_OK;
							}
							else if (element.Type == MyDaAddressSpaceElement.TYPE_POLL_MIN)
							{
								// get address space element value
								DateTime now = DateTime.Now;
								ValueQT cacheValue = new ValueQT(now.Minute, EnumQuality.GOOD, now);
								request.Value = cacheValue;
								request.Result = EnumResultCode.S_OK;
							}
							else if (element.Type == MyDaAddressSpaceElement.TYPE_REPORT_MIN)
							{
								//	this is a direct device read request
								DateTime now = DateTime.Now;
								ValueQT cacheValue = new ValueQT(now.Minute, EnumQuality.GOOD, now);
								request.Value = cacheValue;
								request.Result = EnumResultCode.S_OK;
							}
							else if (element.Type == MyDaAddressSpaceElement.TYPE_REPORT_SEC)
							{
								//	this is a direct device read request
								DateTime now = DateTime.Now;
								ValueQT cacheValue = new ValueQT(now.Second, EnumQuality.GOOD, now);
								request.Value = cacheValue;
								request.Result = EnumResultCode.S_OK;
							}   //	end if ... else
						}
						else
						{
							// get property value
							// get the value from the address space element
							element.GetPropertyValue(request);
						}   //	end if ... else
					}   //	end if ... else
				}   //	end if
			}   //	end for
			return CompleteRequests();
		}   //	end HandleReadRequests


		//--
		#endregion
	}   //	end class MyTransaction
}   //	end namespace
