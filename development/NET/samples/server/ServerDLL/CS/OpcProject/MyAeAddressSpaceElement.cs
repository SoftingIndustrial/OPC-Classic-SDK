using Softing.OPCToolbox.Server;

namespace DllSample
{
	public class MyAeAddressSpaceElement : AeAddressSpaceElement
	{
		#region Constructors
		//-------------------

		public MyAeAddressSpaceElement(
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

		public MyAeAddressSpaceElement()
		{
		}   //	end constructor

		//--
		#endregion
	}   //	end class MyAeAddressSpaceElemen
}
