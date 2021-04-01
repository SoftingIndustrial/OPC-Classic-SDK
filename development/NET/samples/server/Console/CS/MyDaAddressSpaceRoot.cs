using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace Console
{
	public class MyDaAddressSpaceRoot : DaAddressSpaceRoot
	{
		#region Public Methods
		//--------------------

		public override int QueryAddressSpaceElementData(string anElementId, out AddressSpaceElement anElement)
		{

			MyDaAddressSpaceElement element = new MyDaAddressSpaceElement();
			element.ItemId = anElementId;
			anElement = element;

			return (int)EnumResultCode.S_OK;

		}   //	end QueryAddressSpaceElementData

		//--
		#endregion

	}   //	end MyDaAddressSpaceRoot
}   //	end namespace
