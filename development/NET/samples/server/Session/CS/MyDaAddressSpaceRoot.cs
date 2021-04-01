using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace Session
{
	public class MyDaAddressSpaceRoot : DaAddressSpaceRoot
	{
		#region Public Methods
		//--------------------

		public override int QueryAddressSpaceElementData(
			string elementId,
			out AddressSpaceElement anAddressSpaceElement)
		{
			//	TODO: add string based address space validations
			anAddressSpaceElement = null;
			return (int)EnumResultCode.E_NOTIMPL;
		}   //	end QueryAddressSpaceElementData

		//--
		#endregion

	}   //	end MyDaAddressSpaceRoot
}   //	end namespace
