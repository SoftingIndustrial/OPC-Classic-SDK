using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace AECondition
{
	public class MyAeSession : AeSession
	{

		#region Constructor
		//-----------------

		public MyAeSession(string url) : base(url)
		{
			EnableAeConditionsBySourceCompleted += new EnableAeConditionsBySourceEventHandler(HandleEnableAeConditionsBySourceCompleted);

		}

		//--
		#endregion

		#region Private Members
		//---------------------

		#region Private Attributes
		//------------------------

		private OpcForm m_opcForm = null;

		//--
		#endregion

		//--
		#endregion

		#region Public Methods
		//---------------------

		public void SetForm(OpcForm form)
		{
			m_opcForm = form;
		}

		//--
		#endregion

		#region Public Properties
		//-----------------------


		//--
		#endregion

		#region Handles
		//---------------------

		private void HandleEnableAeConditionsBySourceCompleted(
			AeSession anAeSession,
			uint executionContext,
			bool enable,
			string path,
			int result)
		{
			if (ResultCode.SUCCEEDED(result))
			{
				Console.WriteLine("Enabling conditions succeeded!");
			}
		}

		//--
		#endregion

	}   //	end class MyAeSession

}   //	end namespace
