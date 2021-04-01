using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace SerialIO
{
	public class MyTransaction : DaTransaction
	{
		#region Public Methods
		//--------------------

		public MyTransaction(
			EnumTransactionType aTransactionType,
			DaRequest[] requestList,
			uint aSessionKey) :
			base(
			aTransactionType,
			requestList,
			aSessionKey)
		{
		}   //	end constructor

		public override int HandleReadRequests()
		{
			//  Handle reads asynchrousouly
			Console.OpcServer.AddRequsts(this.m_requestList);
			return (int)EnumResultCode.S_OK;

		}   //	end HandleReadRequests

		public override int HandleWriteRequests()
		{
			//  Handle writesasynchrousouly

			Console.OpcServer.AddRequsts(this.m_requestList);
			return (int)EnumResultCode.S_OK;

		}   //	end HandleWriteRequests

		//--
		#endregion

	}   //	end class MyTransaction

}   //	end namespace
