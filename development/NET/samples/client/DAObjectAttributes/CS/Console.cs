using System;
using System.Runtime.InteropServices;
using System.Threading;
using Softing.OPCToolbox;

namespace DAObjectAttributes
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
					//	initialize the DA client simulation
				result |= m_opcClient.InitializeDaObjects();

				client.PrintInitialState();

				client.ChangeSession();

				client.ChangeSubscription();

				client.ChangeItemDatatype();

				client.Terminate();
				client = null;
				MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT);
			}
			catch (Exception exc)
			{
				System.Console.WriteLine(exc.ToString());
			}   //	end try...catch
			finally
			{
				System.Console.WriteLine("\n Press any key to finish...");
				System.Console.ReadLine();
			}
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
