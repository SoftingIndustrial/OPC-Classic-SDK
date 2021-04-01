using System;
using System.Runtime.InteropServices;
using System.Threading;
using Softing.OPCToolbox;

namespace AEConsole
{
	public class Console
	{
		#region Public Attributes
		//-----------------------------------

		// signals application should terminate
		public static AutoResetEvent EndEvent;
		public static bool End = false;
		//--
		#endregion

		#region Private Attributes
		//-------------------------
		private static OpcClient m_opcClient = null;
		//--
		#endregion

		#region Public Properties
		//------------------------

		public OpcClient OpcClient
		{
			get
			{
				return m_opcClient;
			}   //	end get
		}   //	end OpcClient

		//--
		#endregion

		#region Public Methods
		//--------------------------
		public void CreateOpcClient()
		{
			if (m_opcClient == null)
			{
				m_opcClient = new OpcClient();
			}   //	end if
		}   //	end CreateOpcClient

		//--
		#endregion

		public static void Usage()
		{
			System.Console.WriteLine();
			System.Console.Write("\nUsage:\n");
			System.Console.Write("Use lower case letters for synchronous execution \n");
			System.Console.Write("Use upper case letters for asynchronous execution \n");
			System.Console.Write("Press \'a\' or \'A\' to activate the objects\n");
			System.Console.Write("Press \'c\' or \'C\' to connect the objects\n");
			System.Console.Write("Press \'d\' or \'D\' to disconnect the objects\n");
			System.Console.Write("Press \'s\' or \'S\' to get the server status\n");
			System.Console.Write("Press \'m\' to activate the connection monitor\n");
			System.Console.Write("Press \'n\' to deactivate the connection monitor\n");
			System.Console.Write("Press \'?\' or \"u\" to display this usage information\n");
			System.Console.Write("Press \'e\' or \'q\' or Ctrl-C to exit\n");
			System.Console.WriteLine();
		}

		public static void Main(String[] args)
		{
			try
			{
				int result = (int)EnumResultCode.S_OK;
				EndEvent = new AutoResetEvent(false);
				Console console = new Console();

				MyWin32.HandlerRoutine handlerRoutine = new MyWin32.HandlerRoutine(MyWin32.Handler);
				MyWin32.SetConsoleCtrlHandler(
					handlerRoutine,
					true);

				console.CreateOpcClient();
				// gets the OpcClient instance
				OpcClient client = console.OpcClient;


				//	initialize the client instance
				if (!ResultCode.SUCCEEDED(client.Initialize()))
				{
					client = null;
					return;
				}   //	end if
					//	initialize the AE client simulation
				result |= m_opcClient.InitializeAeObjects();

				Usage();
				bool end = false;
				while (!Console.End && !end)
				{
					String read = System.Console.ReadLine();
					switch (read)
					{
						case "A":
							client.activateObjectsAsync();
							break;
						case "a":
							client.activateObjectsSync();
							break;
						case "C":
							client.connectObjectsAsync();
							break;
						case "c":
							client.connectObjectsSync();
							break;
						case "D":
							client.disconnectObjectsAsync();
							break;
						case "d":
							client.disconnectObjectsSync();
							break;
						case "E":
						case "e":
						case "Q":
						case "q":
							{
								end = true;
							}
							break;
						default:
							{
								Usage();
							}
							break;
						case "S":
							client.getServerStatusAsync();
							break;
						case "s":
							client.getServerStatusSync();
							break;
						case "M":
						case "m":
							client.activateConnectionMonitor();
							break;
						case "N":
						case "n":
							client.deactivateConnectionMonitor();
							break;
					}
				}// end while

				client.Terminate();
				client = null;
				MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT);
			}
			catch (Exception exc)
			{
				System.Diagnostics.Debug.WriteLine(exc.Message + exc.Source + exc.StackTrace + exc.TargetSite);
			}   //	end try...catch
		}   //	end Main
	}   //	end class Console

	public class MyWin32
	{
		// Declare the SetConsoleCtrlHandler function 
		// as external and receiving a delegate.   
		[DllImport("Kernel32")]
		public static extern Boolean SetConsoleCtrlHandler(
			HandlerRoutine Handler,
			Boolean Add);

		// A delegate type to be used as the handler routine 
		// for SetConsoleCtrlHandler.
		public delegate Boolean HandlerRoutine(CtrlTypes CtrlType);

		// An enumerated type for the control messages 
		// sent to the handler routine.
		public enum CtrlTypes
		{
			CTRL_C_EVENT = 0,
			CTRL_BREAK_EVENT,
			CTRL_CLOSE_EVENT,
			CTRL_LOGOFF_EVENT = 5,
			CTRL_SHUTDOWN_EVENT
		}   //	end enum CtrlTypes

		// A private static handler function.
		static public Boolean Handler(MyWin32.CtrlTypes CtrlType)
		{
			string message = string.Empty;

			// A switch to handle the event type.
			switch (CtrlType)
			{
				case MyWin32.CtrlTypes.CTRL_C_EVENT:
				case MyWin32.CtrlTypes.CTRL_BREAK_EVENT:
				case MyWin32.CtrlTypes.CTRL_CLOSE_EVENT:
				case MyWin32.CtrlTypes.CTRL_LOGOFF_EVENT:
				case MyWin32.CtrlTypes.CTRL_SHUTDOWN_EVENT:
					//message = "Stop execution, since CTRL command!";
					Console.EndEvent.Set();
					Console.End = true;
					break;
			}   //	end switch
				// Use interop to display a message for the type of event.
			System.Console.WriteLine(message);
			return true;
		}   //	end Handler
	}   //	end class Console
}   //	end namespace
