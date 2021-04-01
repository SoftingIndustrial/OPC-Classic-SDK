using Softing.OPCToolbox.Server;

namespace Console
{
	public class MyCreator : Creator
	{
		#region Public Methods
		//---------------------

		public override AeAddressSpaceElement CreateInternalAeAddressSpaceElement(
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle)
		{
			return new MyAeAddressSpaceElement(
				aName,
				anUserData,
				anObjectHandle,
				aParentHandle);
		}   //	end CreateAeAddressSpaceElement

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

		public override WebTemplate CreateWebTemplate()
		{
			return new MyWebTemplate();
		}   //	end CreateWebTemplate

		//--
		#endregion

	}   //	end class MyCreator
}
