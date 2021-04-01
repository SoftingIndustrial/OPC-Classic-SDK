using Softing.OPCToolbox.Server;

namespace DemoServer
{
	public class DemoAeAddressSpaceElement : AeAddressSpaceElement
	{
		#region Constructors
		//-------------------

		public DemoAeAddressSpaceElement(
			string aName,
			uint anUserData,
			uint anObjectHandle,
			uint aParentHandle)
			:
			base(
			aName,
			anUserData,
			anObjectHandle,
			aParentHandle)
		{
		}   //	end constructor

		public DemoAeAddressSpaceElement()
		{
		}   //	end constructor

		//--
		#endregion
	}   //	end class MyAeAddressSpaceElemen
}
