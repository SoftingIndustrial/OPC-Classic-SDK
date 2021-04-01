using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;
using System.IO;

namespace DaConsole_Service
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
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			streamWriter.WriteLine(sender + " State Changed - " + state);
			streamWriter.Flush();
			streamWriter.Close();
		} // end HandleStateChanged


		public static void HandlePerformStateTransition(
			ObjectSpaceElement sender,
			uint executionContext,
			int result)
		{
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			if (ResultCode.SUCCEEDED(result))
			{
				streamWriter.WriteLine(sender + " Performed state transition - " + executionContext);
			}
			else
			{
				streamWriter.WriteLine(sender + "  Performed state transition failed! Result: " + result);
			}
			streamWriter.Flush();
			streamWriter.Close();
		} // end HandlePerformStateTransition


		public static bool HandleServerShutdown(ObjectSpaceElement sender, string reason)
		{
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			streamWriter.WriteLine();
			streamWriter.WriteLine(sender + " Shutdown - " + reason);
			streamWriter.WriteLine();
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
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			if (ResultCode.SUCCEEDED(result))
			{
				streamWriter.WriteLine(daSession + " asynchronous read succeeded! ");
				for (int i = 0; i < itemIds.Length; i++)
				{
					if (ResultCode.SUCCEEDED(results[i]))
					{
						streamWriter.WriteLine(String.Format("{0,-19} {1} {2,-50} ", itemIds[i], "-", values[i].ToString()));
					}
					else
					{
						streamWriter.WriteLine(" Session asynchronous read failed for item " + " Item: " + itemIds[i] + " [ResultID: " + results[i] + " ]");
					}
				}
			}
			else
			{
				streamWriter.WriteLine(" Session asynchronous read failed! Result: " + result);
			}
			streamWriter.Flush();
			streamWriter.Close();
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
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			if (ResultCode.SUCCEEDED(result))
			{
				streamWriter.WriteLine(daSession + " asynchronous write succeeded! ");
				for (int i = 0; i < itemIds.Length; i++)
				{
					if (ResultCode.SUCCEEDED(results[i]))
					{
						streamWriter.WriteLine(String.Format("{0,-19} {1} {2,-50} ", itemIds[i], "-", values[i].ToString()));
					}
					else
					{
						streamWriter.WriteLine(" Session asynchronous write for item " + itemIds[i] + " [ResultID: " + results[i] + " ]");
					}
				}
			}
			else
			{
				streamWriter.WriteLine("Session asynchronous write failed! Result:" + result);
			}
			streamWriter.Flush();
			streamWriter.Close();
		}// end HandleSessionWriteCompleted


		public static void HandleGetServerStatus(
			ObjectSpaceElement sender,
			uint executionContext,
			ServerStatus serverStatus,
			int result)
		{
			FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fs);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			if (ResultCode.SUCCEEDED(result))
			{
				streamWriter.WriteLine(sender);
				streamWriter.WriteLine("Server Status");
				streamWriter.WriteLine("	Vendor info: " + serverStatus.VendorInfo);
				streamWriter.WriteLine("	Product version: " + serverStatus.ProductVersion);
				streamWriter.WriteLine("	State: " + serverStatus.State);
				streamWriter.WriteLine("	Start time: " + serverStatus.StartTime);
				streamWriter.WriteLine("	Last update time: " + serverStatus.LastUpdateTime);
				streamWriter.WriteLine("	Current time: " + serverStatus.CurrentTime);
				streamWriter.WriteLine("	GroupCount: " + serverStatus.GroupCount);
				streamWriter.WriteLine("	Bandwidth: " + serverStatus.Bandwidth);
				for (int i = 0; i < serverStatus.SupportedLcIds.Length; i++)
				{
					streamWriter.WriteLine("	Supported LCID: " + serverStatus.SupportedLcIds[i]);
				}
				streamWriter.WriteLine("	Status info: " + serverStatus.StatusInfo);
			}
			else
			{
				streamWriter.WriteLine("Asynchronous get server status failed! Result: " + result);
			}
			streamWriter.Flush();
			streamWriter.Close();
		} // end HandleGetServerStatus

		//--
		#endregion

	}   //	end class MyDaSession

}   //	end namespace
