using System;
using System.Runtime.InteropServices;
using System.Threading;
using Softing.OPCToolbox;

namespace TestDll
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

		//--
		#endregion

		#region Public Properties
		//------------------------


		//--
		#endregion

		#region Public Methods
		//--------------------------


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

				//create an instance of the generated DLL
				Dll usedDll = new Dll();

				// call the DLL's Start method; server will be initialized, 
				// namespaces will be created and simulation thread will be started
				result = usedDll.Start();

				if (result != (uint)EnumResultCode.S_OK)
				{
					return;
				}

				System.Console.WriteLine("The results will be written in the ClientDll.txt file");
				System.Console.WriteLine("Press \'e\' or \' q\' and 'ENTER' to exit");

				bool end = false;

				while (!Console.End && !end)
				{
					String read = System.Console.ReadLine();
					switch (read)
					{
						case "E":
						case "e":
						case "Q":
						case "q":
							end = true;
							break;

						default:
							break;
					}
				}   //	end while


				//calls the DLL's Stop method which terminates the server application and closes the simulation thread
				usedDll.Stop();

				MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT);
			}
			catch (Exception exc)
			{
				System.Console.WriteLine(exc.ToString());
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
					message = "Stop execution, since CTRL command!";
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
