using System;
using System.Collections;
using System.Text;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace TestServer
{
	public class MyTransaction : DaTransaction
	{
		#region Public Methods
		//--------------------

		public MyTransaction(
			EnumTransactionType aTransactionType,
			DaRequest[]requestList,
			uint aSessionKey)
			:
			base(
			aTransactionType,
			requestList,
			aSessionKey)
		{
		}	//	end constructor

		public override int HandleReadRequests()
		{
			int count = Requests.Count;

			for (int i = 0; i < count; i++)
			{
				DaRequest request = (DaRequest)Requests[i];
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
					}	//	end if ... else
				}	//	end if ... else
			}	//	end for
			return CompleteRequests();
		}	//	end HandleReadRequests

		public override int HandleWriteRequests()
		{
			int count = Requests.Count;
			lock (OpcServer.LockTransactionForWrite)
			{
				for (int i = 0; i < count; i++)
				{
					DaRequest request = Requests[i] as DaRequest;
					if (request != null)
					{
						MyDaAddressSpaceElement element = request.AddressSpaceElement as MyDaAddressSpaceElement;
						if (element != null)
						{
							ValueQT valueQT = request.Value;
							request.Result = (EnumResultCode)element.ValueChanged(valueQT);
						}	//	end if

						DaCreateItemsCommand createCommand = request.AddressSpaceElement as DaCreateItemsCommand;
						if (createCommand != null)
						{
							ValueQT valueQT = request.Value;
							// Execute create items command
							EnumResultCode result = createCommand.CreateItems(valueQT);
							request.Result = (EnumResultCode)createCommand.ValueChanged(valueQT);

							if (result != EnumResultCode.S_OK)
							{
								request.Result = result;
							}
						}

						DaDeleteItemsCommand deleteCommand = request.AddressSpaceElement as DaDeleteItemsCommand;
						if (deleteCommand != null)
						{
							// Execute delete items command
							request.Result = deleteCommand.DeleteItems();
						}

						DaStartStopSimulationCommand simulationCommand = request.AddressSpaceElement as DaStartStopSimulationCommand;
						if (simulationCommand != null)
						{
							ValueQT valueQT = request.Value;
							request.Result = (EnumResultCode)simulationCommand.ValueChanged(valueQT);

							EnumResultCode result;
							bool startCommand = (bool)valueQT.Data;

							if (startCommand)
							{
								// Execute Start simulation command
								result = simulationCommand.StartSimulation();
							}
							else
							{
								// Execute Stop simulation command
								result = simulationCommand.StopSimulation();
							}

							if (result != EnumResultCode.S_OK)
							{
								request.Result = result;
							}
						}
					}	//	end if
				}	//	end for
			}
			return CompleteRequests();
		}	//	end HandleWriteRequests

		//--
		#endregion
	}	//	end class MyTransaction
}	//	end namespace
