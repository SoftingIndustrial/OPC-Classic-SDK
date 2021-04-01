using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace DaConsole
{
	public class MyDaSession : DaSession
	{

		#region Constructor
		//-----------------

		public MyDaSession(string url) : base(url)
		{
			StateChangeCompleted += new StateChangeEventHandler(HandleStateChanged);
			PerformStateTransitionCompleted += new PerformStateTransitionEventHandler(HandlePerformStateTransition);
			ShutdownRequest += new ShutdownEventHandler(HandleServerShutdown);
			ReadCompleted += new SessionReadEventHandler(HandleSessionReadCompleted);
			WriteCompleted += new SessionWriteEventHandler(HandleSessionWriteCompleted);
			GetStatusCompleted += new GetStatusEventHandler(HandleGetServerStatus);
			LogonCompleted += new LogonEventHandler(HandleLogonCompleted);
			LogoffCompleted += new LogoffEventHandler(HandleLogoffCompleted);
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

		public static void HandleStateChanged(ObjectSpaceElement sender, EnumObjectState state)
		{
			System.Console.WriteLine(sender + " State Changed - " + state);
		} // end HandleStateChanged

		public static void HandlePerformStateTransition(
			ObjectSpaceElement sender,
			uint executionContext,
			int result)
		{
			if (ResultCode.SUCCEEDED(result))
			{
				System.Console.WriteLine(sender + " Performed state transition - " + executionContext);
			}
			else
			{
				System.Console.WriteLine(sender + "  Performed state transition failed! Result: " + result);
			}
		} // end HandlePerformStateTransition


		public static bool HandleServerShutdown(ObjectSpaceElement sender, string reason)
		{
			System.Console.WriteLine();
			System.Console.WriteLine(sender + " Shutdown - " + reason);
			System.Console.WriteLine();
			return true; // reconnection will be performed
		} // end HandleServerShutdown


		public static void HandleSessionReadCompleted(
			DaSession daSession,
			uint executionContext,
			string[] itemIds,
			string[] itemPaths,
			ValueQT[] values,
			int[] results,
			int result)
		{
			if (ResultCode.SUCCEEDED(result))
			{
				System.Console.WriteLine(daSession + " asynchronous read succeeded! ");
				for (int i = 0; i < itemIds.Length; i++)
				{
					if (ResultCode.SUCCEEDED(results[i]))
					{
						System.Console.WriteLine(String.Format("{0,-19} {1} {2,-50} ", itemIds[i], "-", values[i].ToString()));
					}
					else
					{
						System.Console.WriteLine(" Session asynchronous read failed for item " + " Item: " + itemIds[i] + " [ResultID: " + results[i] + " ]");
					}
				}
			}
			else
			{
				System.Console.WriteLine(" Session asynchronous read failed! Result: " + result);
			}
		} // end HandleSessionReadCompleted


		public static void HandleSessionWriteCompleted(
			DaSession daSession,
			uint executionContext,
			string[] itemIds,
			string[] itemPaths,
			ValueQT[] values,
			int[] results,
			int result)
		{
			if (ResultCode.SUCCEEDED(result))
			{
				System.Console.WriteLine(daSession + " asynchronous write succeeded! ");
				for (int i = 0; i < itemIds.Length; i++)
				{
					if (ResultCode.SUCCEEDED(results[i]))
					{
						System.Console.WriteLine(String.Format("{0,-19} {1} {2,-50} ", itemIds[i], "-", values[i].ToString()));
					}
					else
					{
						System.Console.WriteLine(" Session asynchronous write for item " + itemIds[i] + " [ResultID: " + results[i] + " ]");
					}
				}
			}
			else
			{
				System.Console.WriteLine("Session asynchronous write failed! Result:" + result);
			}
		}// end HandleSessionWriteCompleted


		public static void HandleGetServerStatus(
			ObjectSpaceElement sender,
			uint executionContext,
			ServerStatus serverStatus,
			int result)
		{
			if (ResultCode.SUCCEEDED(result))
			{
				System.Console.WriteLine(sender);
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
				{
					System.Console.WriteLine("	Supported LCID: " + serverStatus.SupportedLcIds[i]);
				}
				System.Console.WriteLine("	Status info: " + serverStatus.StatusInfo);
			}
			else
			{
				System.Console.WriteLine("Asynchronous get server status failed! Result: " + result);
			}
		} // end HandleGetServerStatus


		public static void HandleLogonCompleted(
			ObjectSpaceElement sender,
			uint executionContext,
			String UserName,
			String Password,
			int result)
		{
			System.Console.Write("\n Logon for secure communication");
			if (ResultCode.SUCCEEDED(result))
			{
				System.Console.WriteLine("- successfully logon for user:" + UserName);
			}
			else
			{
				System.Console.WriteLine("- failed logon for user:" + UserName + "  " + result);
			}//end if ... else
		}//end handleLogonCompleted

		public static void HandleLogoffCompleted(
			ObjectSpaceElement sender,
			uint executionContext,
			int result)
		{
			System.Console.Write("\n Logon for secure communication");
			if (ResultCode.SUCCEEDED(result))
			{
				System.Console.WriteLine("- successfully logoff");
			}
			else
			{
				System.Console.WriteLine("- failed logoff" + "  " + result);
			}//end if ... else
		}//end handleLogoffCompleted
		 //--
		#endregion

	}   //	end class MyDaSession

}   //	end namespace
