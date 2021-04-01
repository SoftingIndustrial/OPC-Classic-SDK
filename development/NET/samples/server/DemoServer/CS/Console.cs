using System;
using System.Runtime.InteropServices;
using System.Threading;
using Softing.OPCToolbox;

namespace DemoServer
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

		#region Public Constant Attributes

		public const uint CATEGORY_ID_SIMULATION = 1;
		public const uint CATEGORY_ID_TIME_TICK = 2;
		public const uint CATEGORY_ID_KEY_HIT = 3;
		public const uint CATEGORY_ID_DA_WRITE_SIMPLE = 4;
		public const uint CATEGORY_ID_DA_WRITE_TRACKING = 5;
		public const uint CATEGORY_ID_TIME_SLOT = 6;
		public const String COND_NAME_BETWEEN_SINGLE = "between [10,50]";
		public const String COND_NAME_BETWEEN_MULTIPLE = "between";

		public static UInt32 DA_SLEEP_INTERVAL = 1000;
		public static bool DO_SIMULATION = true;

		#endregion

		#region Private Attributes
		//-------------------------
		//DA
		private bool m_simulationDAEnd = false;
		private static Mutex m_mutexDA = new Mutex();
		//DA
		private bool m_simulationAEEnd = false;
		private static Mutex m_mutexAE = new Mutex();
		//OPCServer
		private static OpcServer m_opcServer = null;
		//--
		#endregion

		#region Public Properties
		//------------------------
		public bool SimulationDAEnd
		{
			get
			{
				return m_simulationDAEnd;
			}   //	end get
			set
			{
				m_simulationDAEnd = value;
			}   //	end set
		}   //	end SimulationDAEnd
		public Mutex MutexDA
		{
			get
			{
				return m_mutexDA;
			}   //	end get
		}   //	end MutexDA
		public bool SimulationAEEnd
		{
			get
			{
				return m_simulationAEEnd;
			}   //	end get
			set
			{
				m_simulationAEEnd = value;
			}   //	end set
		}   //	end SimulationAEEnd
		public Mutex MutexAE
		{
			get
			{
				return m_mutexAE;
			}   //	end get
		}   //	end MutexAE

		public OpcServer OpcServer
		{
			get
			{
				return m_opcServer;
			}   //	end get
		}   //	end OpcServer
			//--
		#endregion

		#region Public Methods
		//--------------------------
		public void CreateOpcServer()
		{
			if (m_opcServer == null)
			{
				m_opcServer = new OpcServer();
			}   //	end if
		}   //	end CreateOpcClient

		public void SimulationDAThread()
		{
			// Wait until it is safe to enter.
			m_mutexDA.WaitOne();

			while (!m_simulationDAEnd)
			{
				if (DO_SIMULATION)
				{
					//	Simulate the DA Server data changes generation
					m_opcServer.ChangeSimulationValues();
				}
				Thread.Sleep((int)DA_SLEEP_INTERVAL);
			}   //	end while
				// Release the Mutex
			m_mutexDA.ReleaseMutex();

		}   //	end SimulationDAThread

		public void StartDASimulationThread()
		{
			Thread simulationDAThread = new Thread(new ThreadStart(SimulationDAThread));
			simulationDAThread.Start();
		}   //	end StartDASimulationThread

		public void SimulationAEThread()
		{
			m_mutexAE.WaitOne();
			while (!m_simulationAEEnd)
			{
				//Simulate the AE Server events generation
				m_opcServer.FireSimulationEvents();
				Thread.Sleep(500);
			}   // end while
			m_mutexAE.ReleaseMutex();
		}   //  end SimulationAEThread

		public void StartAESimulationThread()
		{
			Thread simulationAEThread = new Thread(new ThreadStart(SimulationAEThread));
			simulationAEThread.Start();
		}   //  end StartAESimulationThread

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


				//	create and initialize the OpcServer instance
				console.CreateOpcServer();
				OpcServer server = console.OpcServer;
				server.Initialize();

				DemoCreator creator = new DemoCreator();
				if (!ResultCode.SUCCEEDED(server.Prepare(creator)))
				{
					server.Terminate();
					MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT);
					server = null;
					return;
				}   //	end if

				//	handle the command line arguments (register/unregister, etc)
				string commandline = Environment.CommandLine;
				result = server.ProcessCommandLine(commandline);

				if (result != (uint)EnumResultCode.S_OK)
				{
					if (result == (uint)EnumResultCode.S_FALSE)
					{
						//registration operation succesful
						server.Trace(
							EnumTraceLevel.INF,
							EnumTraceGroup.USER1,
							"Console::Main",
							"Registration succeeded");
					}
					else
					{
						server.Trace(
							EnumTraceLevel.INF,
							EnumTraceGroup.USER1,
							"Console::Main",
							"Registration failed");
					}   //	end if...else

					//	no matter what close the application if
					//processCommandLine returned something different of S_OK
					server.Terminate();
					server = null;
					return;
				}   //	end if

				//	start the OPC server's I/O internal mechanism 
				if (ResultCode.SUCCEEDED(server.Start()))
				{
					//	build the namespace
					server.BuildAddressSpace();
					//	declare the namespaces built and the server ready for clients to connect
					server.Ready();
				}   //	end if
				else
				{
					server.Trace(
						EnumTraceLevel.WRN,
						EnumTraceGroup.USER1,
						"Console::Main",
						"Serrver start failed");
					return;
				}
				//	start the simulation thread 
				if (ResultCode.SUCCEEDED(result))
				{
					console.StartDASimulationThread();
					console.StartAESimulationThread();
				}   //	end if
				System.Console.WriteLine("Press Ctrl-C to exit");

				while (!Console.End)
				{
					//	TODO: place your cyclic code here						
					Thread.Sleep(1000);
				}   //	end while

				//	terminate the DA simulation
				console.SimulationDAEnd = true;
				console.MutexDA.WaitOne();
				console.MutexDA.ReleaseMutex();

				//	terminate the AE simulation
				console.SimulationAEEnd = true;
				console.MutexAE.WaitOne();
				console.MutexAE.ReleaseMutex();

				server.Stop();
				server.Terminate();
				server = null;
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
