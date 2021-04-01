using System;
using System.Runtime.InteropServices;
using System.Threading;
using Softing.OPCToolbox;

namespace AddressSpace
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
		private static Mutex m_mutex = new Mutex();
		private int m_count = 0;
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
			}	//	end get
			set
			{
				m_simulationEnd = value;
			}	//	end set
		}	//	end SimulationEnd
		public Mutex Mutex
		{
			get
			{
				return m_mutex;
			}	//	end get
		}	//	end Mutex

		public OpcServer OpcServer
		{
			get
			{				
				return m_opcServer;
			}	//	end get
		}	//	end OpcServer
		//--
		#endregion

		#region Public Methods
		//--------------------------
		public void CreateOpcServer()
		{
			if (m_opcServer == null)
			{
				m_opcServer = new OpcServer();
			}	//	em=nd if
		}	//	end CreateOpcClient

		public void Simulate()
		{	
			// Wait until it is safe to enter.
			m_mutex.WaitOne();
			
			int iRet = WaitHandle.WaitTimeout;

			while (!m_simulationEnd)
			{
				iRet = System.Threading.WaitHandle.WaitAny(
					new AutoResetEvent[] {OpcServer.AddressSpaceChanged}, 1000, true);
				if (iRet == WaitHandle.WaitTimeout) {

					if (m_count > 30)
					{

						OpcServer.ReconfigHandles[0].RemoveChild(OpcServer.ReconfigHandles[2]);
						OpcServer.ObjectBased.RemoveChild(OpcServer.ReconfigHandles[0]);
						OpcServer.ObjectBased.RemoveChild(OpcServer.ReconfigHandles[1]);
						m_opcServer.RebuildRandomAddressSpace(OpcServer.ObjectBased);
						OpcServer.AddressSpaceChanged.Set();
						m_count = 0;
					}
					else
					{
						m_count++;
					}	//	end if ... else
				}
				else if (iRet == 0) 
				{
					OpcServer.ShowObjectTree();
				}	//	end if ... else
							
			}	//	end while
			// Release the Mutex
			m_mutex.ReleaseMutex();

		}	//	end Simulate

		public void StartSimulationThread()
		{
			Thread simulationThread = new Thread(new ThreadStart(Simulate));
			simulationThread.Start();			
		}	//	end StartSimulationThread

		//--
		#endregion

		public static void Main(String []args)
		{
			try{

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
				
				MyCreator creator = new MyCreator();
				if (!ResultCode.SUCCEEDED(server.Prepare(creator)))
				{
					server.Terminate();
					MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT);
					server = null;
					return;
				}	//	end if

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
					}	//	end if...else

					//	no matter what close the application if
					//processCommandLine returned something different of S_OK
					server.Terminate();
					server = null;
					return;
				}	//	end if
				server.ReconfigHandles = new MyDaAddressSpaceElement[3];
				OpcServer.AddressSpaceChanged = new System.Threading.AutoResetEvent(true);

				//	start the OPC server's I/O internal mechanism 
				if (ResultCode.SUCCEEDED(server.Start()))
				{	
					//	build the namespace
					server.BuildAddressSpace();
					//	declare the namespaces built and the server ready for clients to connect
					server.Ready();
				}	//	end if
				
				server.ShowObjectTree();

				//	start the simulation thread 
				if (ResultCode.SUCCEEDED(result))
				{
					console.StartSimulationThread();
				}	//	end if

				System.Console.WriteLine("Press Ctrl-C to exit\n");
				
				while (!Console.End)
				{
					//	TODO: place your cyclic code here						
					Thread.Sleep(1000);
				}	//	end while

				//	terminate the simulation
				console.SimulationEnd = true;
				console.Mutex.WaitOne();
				console.Mutex.ReleaseMutex();

				server.Stop();	
				server.Terminate();
				server = null;	
				MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT);
			}
			catch(Exception exc)
			{				
				System.Console.WriteLine(exc.ToString());
			}	//	end try...catch
		}	//	end Main
	}	//	end class Console

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
		}	//	end enum CtrlTypes
		
		// A private static handler function.
		static public Boolean Handler(MyWin32.CtrlTypes CtrlType)
		{
			string message = string.Empty;

			// A switch to handle the event type.
			switch(CtrlType)
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
			}	//	end switch
			// Use interop to display a message for the type of event.
			System.Console.WriteLine(message);
			return true;
		}	//	end Handler
	}	//	end class Console
}	//	end namespace
