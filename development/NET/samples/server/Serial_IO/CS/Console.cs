using System;
using System.Runtime.InteropServices;
using System.Threading;
using Softing.OPCToolbox;

namespace SerialIO
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

		private bool m_simulationEnd = false;
		private Mutex m_simulationMutex = new Mutex();

		private bool m_handleRequestsEnd = false;
		private Mutex m_handleRequestsMutex = new Mutex();

		private static OpcServer m_opcServer = null;
		//--
		#endregion

		#region Public Properties
		//------------------------

		public bool SimulationEnd
		{
			get
			{
				return m_simulationEnd;
			}   //	end get
			set
			{
				m_simulationEnd = value;
			}   //	end set
		}   //	end SimulationEnd


		public Mutex SimulationMutex
		{
			get
			{
				return m_simulationMutex;
			}   //	end get
		}   //	end Mutex


		public bool HandleRequestsEnd
		{
			get
			{
				return m_handleRequestsEnd;
			}   //	end get
			set
			{
				m_handleRequestsEnd = value;
			}   //	end set
		}   //	end SimulationEnd


		public Mutex HandleRequestsMutex
		{
			get
			{
				return m_handleRequestsMutex;
			}   //	end get
		}   //	end Mutex


		public static OpcServer OpcServer
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

		public void Simulate()
		{
			// Wait until it is safe to enter.
			m_simulationMutex.WaitOne();

			while (!m_simulationEnd)
			{
				//	Simulate the DA Server data changes generation
				m_opcServer.ChangeSimulationValues();
				Thread.Sleep(1000);
			}   //	end while
				// Release the Mutex
			m_simulationMutex.ReleaseMutex();

		}   //	end Simulate

		public void StartSimulationThread()
		{
			Thread simulationThread = new Thread(new ThreadStart(Simulate));
			simulationThread.Start();
		}   //	end StartSimulationThread

		public void HandleRequests()
		{
			// Wait until it is safe to enter.
			m_handleRequestsMutex.WaitOne();

			while (!m_simulationEnd)
			{
				//	Simulate the DA Server data changes generation
				m_opcServer.HandleRequests();
				Thread.Sleep(100);
			}   //	end while

			// Release the Mutex
			m_handleRequestsMutex.ReleaseMutex();

		}   //	end Simulate


		public void StartRequestHandlesThread()
		{
			Thread handleRequestsThread = new Thread(new ThreadStart(HandleRequests));
			handleRequestsThread.Start();
		}   //	end StartSimulationThread

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
				OpcServer server = Console.OpcServer;

				//	TODO - binary license activation
				//	Fill in your binary license activation keys here
				//
				//	NOTE: you can activate one or all of the features at the same time
				//	firstly activate the COM-DA Server feature
				//	result = Application.Instance.Activate(EnumFeature.DA_SERVER, "XXXX-XXXX-XXXX-XXXX-XXXX");
				if (!ResultCode.SUCCEEDED(result))
				{
					return;
				}

				//	activate the XML-DA Server Feature
				//	result = Application.Instance.Activate(EnumFeature.XMLDA_SERVER, "XXXX-XXXX-XXXX-XXXX-XXXX");
				if (!ResultCode.SUCCEEDED(result))
				{
					return;
				}

				//	END TODO - binary license activation

				server.Initialize();

				MyCreator creator = new MyCreator();
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
					//	start the simulation thread 
				if (ResultCode.SUCCEEDED(result))
				{
					console.StartSimulationThread();
					console.StartRequestHandlesThread();
				}   //	end if
				System.Console.WriteLine("Press Ctrl-C to exit\n");

				while (!Console.End)
				{
					//	TODO: place your cyclic code here						
					Thread.Sleep(1000);
				}   //	end while

				//	terminate the simulation
				console.SimulationEnd = true;
				console.SimulationMutex.WaitOne();
				console.SimulationMutex.ReleaseMutex();

				server.Stop();

				//  terminate request handling
				console.HandleRequestsEnd = true;
				console.HandleRequestsMutex.WaitOne();
				console.HandleRequestsMutex.ReleaseMutex();


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
