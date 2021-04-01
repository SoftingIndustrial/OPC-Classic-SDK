using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace AEConsole
{
	public class MyAeSession : AeSession
	{

		#region Constructor
		//-----------------

		public MyAeSession(string url) : base(url)
		{
			StateChangeCompleted += new StateChangeEventHandler(HandleStateChangeCompleted);
			PerformStateTransitionCompleted += new PerformStateTransitionEventHandler(HandlePerformObjectStateTransitionCompleted);
			ShutdownRequest += new ShutdownEventHandler(HandleShutdown);
			GetServerStatusCompleted += new GetStatusEventHandler(HandleGetStatusCompleted);
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

		// method that handles the completion of performing the ObjectSpaceElement state transition
		public static void HandlePerformObjectStateTransitionCompleted(ObjectSpaceElement obj, uint executionContext, int result)
		{
			if (ResultCode.SUCCEEDED(result))
			{
				System.Console.WriteLine(obj.ToString() + " Performed state transition " + executionContext);
			}
			else
			{
				System.Console.WriteLine(obj.ToString() + " Performed state transition failed");
			}
		}

		//method that handles the session shutdown
		public static bool HandleShutdown(ObjectSpaceElement obj, string reason)
		{
			System.Console.WriteLine();
			System.Console.WriteLine(obj.ToString() + " Shutdown - " + reason);
			System.Console.WriteLine();
			return true; // connect automatically after shutdown
		}

		// method that handles the completion of state changing of an ObjectSpaceElement
		public static void HandleStateChangeCompleted(ObjectSpaceElement obj, EnumObjectState state)
		{
			System.Console.WriteLine(obj + " State Changed - " + state);
		}

		// method that handles the completion of getting the server status
		public static void HandleGetStatusCompleted(ObjectSpaceElement obj, uint executionContext, ServerStatus serverStatus, int result)
		{
			if (ResultCode.FAILED(result))
			{
				System.Console.WriteLine(obj.ToString() + "Asynchronous GetServerStatus Failed");
			}
			if (ResultCode.SUCCEEDED(result))
			{
				System.Console.WriteLine("Asynchronous GetServerStatus Succeeded");
				System.Console.WriteLine("Server Status");
				System.Console.WriteLine("	Vendor info: " + serverStatus.VendorInfo);
				System.Console.WriteLine("	Product version: " + serverStatus.ProductVersion);
				System.Console.WriteLine("	State: " + serverStatus.State);
				System.Console.WriteLine("	Start time: " + serverStatus.StartTime);
				System.Console.WriteLine("	Last update time: " + serverStatus.LastUpdateTime);
				System.Console.WriteLine("	Current time: " + serverStatus.CurrentTime);
				System.Console.WriteLine("	GroupCount: " + serverStatus.GroupCount);
				System.Console.WriteLine("	Bandwidth: " + serverStatus.Bandwidth);
				for (int i = 0; i < serverStatus.SupportedLcIds.Length; i++)
					System.Console.WriteLine("	Supported LCID: " + serverStatus.SupportedLcIds[i]);
				System.Console.WriteLine("	Status info: " + serverStatus.StatusInfo);
			}
		}

		//--
		#endregion

	}   //	end class MyAeSession

}   //	end namespace
