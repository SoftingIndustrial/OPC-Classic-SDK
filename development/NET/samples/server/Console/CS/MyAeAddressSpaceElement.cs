using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace Console
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

		#region Public Methods
		//-------------------

		public override int QueryConditions(
			string aSourcePath,
			out string[] aConditionNames)
		{

			aConditionNames = new string[2] { OpcServer.COND_NAME_BETWEEN_MULTIPLE, OpcServer.COND_NAME_BETWEEN_SINGLE };
			return (int)EnumResultCode.S_OK;
		}   //	end QueryConditions

		//--
		#endregion

	}   //	end class MyAeAddressSpaceElemen
}
