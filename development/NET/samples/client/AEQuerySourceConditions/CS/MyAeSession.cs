using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace AEQuerySourceConditions
{
	public class MyAeSession : AeSession
	{

		#region Constructor
		//-----------------

		public MyAeSession(string url) : base(url)
		{
			QueryAeSourceConditionsCompleted += new QueryAeSourceConditonsEventHandler(HandleQueryAeSourceConditionsCompleted);
		}

		//--
		#endregion

		#region Private Members
		//---------------------

		#region Private Attributes
		//------------------------


		//--
		#endregion

		//--
		#endregion

		#region Public Methods
		//---------------------

		//--
		#endregion

		#region Public Properties
		//-----------------------


		//--
		#endregion

		#region Handles
		//---------------------

		public static void HandleQueryAeSourceConditionsCompleted(AeSession anAeSession, uint executionOptions, string sourcePath, string[] conditionNames, int result)
		{

			if (ResultCode.SUCCEEDED(result))
			{

				System.Console.WriteLine("\n Source conditions of " + sourcePath + " :");
				for (int i = 0; i < conditionNames.Length; i++)
				{
					System.Console.WriteLine("	[" + i + "] " + conditionNames[i]);
				}
			} //end if
			else
			{

				System.Console.WriteLine("Failed to asynchronously query the conditions of source: " + sourcePath);
			}

		}// end HandleQueryAeSourceConditionsCompleted


		//--
		#endregion

	}   //	end class MyAeSession

}   //	end namespace
