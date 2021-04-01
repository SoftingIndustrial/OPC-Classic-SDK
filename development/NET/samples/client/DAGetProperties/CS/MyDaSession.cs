using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace DAGetProperties
{
	public class MyDaSession : DaSession
	{

		#region Constructor
		//-----------------

		public MyDaSession(string url) : base(url)
		{
			GetDaPropertiesCompleted += new GetDaPropertiesEventHandler(HandleGetDaPropertiesCompleted);
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


		public static void HandleGetDaPropertiesCompleted(
			DaSession aDaSession,
			uint executionContext,
			uint addressSpaceElementHandle,
			string addressSpaceElementId,
			string addressSpaceElementPath,
			DaProperty[] properties,
			int result)
		{


			if (ResultCode.SUCCEEDED(result))
			{

				System.Console.WriteLine("Get properties of address space element:  " + addressSpaceElementId);
				for (int i = 0; i < properties.Length; i++)
				{

					System.Console.WriteLine("	Property Name: " + properties[i].Name);
					System.Console.WriteLine("	Property Id: " + properties[i].Id);
					System.Console.WriteLine("	Property Item Id: " + properties[i].ItemId);
					System.Console.WriteLine("	Property DataType: " + properties[i].DataType);
					System.Console.WriteLine("	Property description: " + properties[i].Description);
					System.Console.WriteLine("	Property value: " + properties[i].ValueQT.Data.ToString() + "	");
					System.Console.WriteLine(" ");

				}
			} //end if
			else
			{

				System.Console.WriteLine("Failed to asynchronously get properties of address space element: " + addressSpaceElementId + " - 0x" + result.ToString("X8"));
			}

		}// end HandleGetDaPropertiesCompleted

		//--
		#endregion

	}   //	end class MyDaSession

}   //	end namespace
