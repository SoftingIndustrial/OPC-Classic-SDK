using Softing.OPCToolbox.Server;

namespace AddressSpace
{
	public class MyCreator : Creator
	{

		public override DaAddressSpaceRoot CreateDaAddressSpaceRoot()
		{
			return (DaAddressSpaceRoot)new MyDaAddressSpaceRoot();
		}   //	end CreateDaAddressSpaceRoot

		public override DaTransaction CreateTransaction(
			EnumTransactionType aTransactionType,
			DaRequest[] aRequests,
			uint aSessionKey)
		{

			return (DaTransaction)new MyTransaction(aTransactionType, aRequests, aSessionKey);
		}   //	end ConsoleTransaction

		public override DaAddressSpaceElement CreateInternalDaAddressSpaceElement(
			string anItemId,
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle)
		{

			return (DaAddressSpaceElement)new MyDaAddressSpaceElement(anItemId, aName, anUserData, anObjectHandle, aParentHandle);

		}   //	end DaAddressSpaceElement

	}   //	end class MyCreator

}   //	end namespace 
