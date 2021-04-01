using Softing.OPCToolbox.Server;

namespace Session
{
	public class MyCreator : Creator
	{
		#region Public Methods
		//---------------------

		public override DaAddressSpaceElement CreateInternalDaAddressSpaceElement(
			string anItemId,
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle)
		{
			return
				(DaAddressSpaceElement)new MyDaAddressSpaceElement(
					anItemId,
					aName,
					anUserData,
					anObjectHandle,
					aParentHandle);
		}   //	end DaAddressSpaceElement


		public override DaAddressSpaceRoot CreateDaAddressSpaceRoot()
		{
			return (DaAddressSpaceRoot)new MyDaAddressSpaceRoot();
		}   //	end CreateDaAddressSpaceRoot

		public override DaTransaction CreateTransaction(
			EnumTransactionType transactionType,
			DaRequest[] requestList,
			uint sessionKey)
		{
			return (DaTransaction)new MyTransaction(
									transactionType,
									requestList,
									sessionKey);
		}   //	end CreateTransaction

		public virtual DaAddressSpaceElement CreateMyDaAddressSpaceElement()
		{
			return (DaAddressSpaceElement)new MyDaAddressSpaceElement();
		}   //	end CreateDaAddressSpaceRoot

		public override DaRequest CreateRequest(
			EnumTransactionType aTransactionType,
			uint aSessionHandle,
			DaAddressSpaceElement anElement,
			int aPropertyId,
			uint aRequestHandle)
		{
			return new MyRequest(
				aTransactionType,
				aSessionHandle,
				anElement,
				aPropertyId,
				aRequestHandle);
		}   //	end CreateRequest

		public override DaSession CreateSession(EnumSessionType aType, uint aHandle)
		{
			return new MySession(aType, aHandle);
		}// end CreateSession

		//--
		#endregion

	}   //	end class MyCreator
}
