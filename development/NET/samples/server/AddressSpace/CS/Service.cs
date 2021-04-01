using System;
using System.Threading;
using System.IO;
using Softing.OPCToolbox;

namespace AddressSpace
{
	public class OpcService : System.ServiceProcess.ServiceBase
	{
		#region Private Attributes
		//-------------------------

		private bool m_simulationEnd = false;
		private static Mutex m_mutex = new Mutex();
		private int m_count = 0;
		private static OpcServer m_opcServer = null;
		// The following constant holds the name and the description of the Windows NT service
		// the OPC application runs under.
		//    TODO : change your service name and description here
		private const string serviceName = "Softing.OPCToolbox.NET.CS.AddressSpace";
		private const string serviceDescription = "Softing OPC Toolkit - OPC DA Server. \r\n\r\nSample name: \"C# AddressSpace_Service\".";

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

		public Mutex Mutex
		{
			get
			{
				return m_mutex;
			}   //	end get
		}   //	end Mutex

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
		//--------------------

		public void CreateOpcServer()
		{
			if (m_opcServer == null)
			{
				m_opcServer = new OpcServer();
			}   //	end if
		}   //	end CreateOpcServer

		public void Simulate()
		{
			// Wait until it is safe to enter.
			m_mutex.WaitOne();

			while (!m_simulationEnd)
			{
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
				}   //	end if ... else

				Thread.Sleep(1000);
			}   //	end while
				// Release the Mutex
			m_mutex.ReleaseMutex();
		}   //	end Simulate

		public void StartSimulationThread()
		{
			Thread simulationThread = new Thread(new ThreadStart(Simulate));
			simulationThread.Start();
		}   //	end StartSimulationThread
			//--
		#endregion

		public OpcService()
		{
		}

		// The main entry point for the process
		static void Main()
		{
			int result = (int)EnumResultCode.S_OK;
			string commandline = Environment.CommandLine;

			OpcServer.AddressSpaceChanged = new System.Threading.AutoResetEvent(true);

			System.ServiceProcess.ServiceBase[] ServicesToRun;
			OpcService opcService = new OpcService();
			ServicesToRun = new System.ServiceProcess.ServiceBase[] { opcService };

			//	create and initialize the OpcServer instance
			opcService.CreateOpcServer();
			m_opcServer.Initialize();
			m_opcServer.ServiceName = serviceName;
			m_opcServer.ServiceDescription = serviceDescription;

			m_opcServer.ReconfigHandles = new MyDaAddressSpaceElement[3];
			MyCreator creator = new MyCreator();
			if (!ResultCode.SUCCEEDED(m_opcServer.Prepare(creator)))
			{
				m_opcServer.Terminate();
				m_opcServer = null;
				return;
			}   //	end if

			//	handle the command line arguments (register/unregister, etc)			
			result = m_opcServer.ProcessCommandLine(commandline);
			if (result != (int)EnumResultCode.S_OK)
			{
				if (result == (int)EnumResultCode.S_FALSE)
				{
					//registration operation succesful
					m_opcServer.Trace(
						EnumTraceLevel.INF,
						EnumTraceGroup.USER1,
						"Service::Main",
						"Registration succeeded");
				}
				else
				{
					m_opcServer.Trace(
						EnumTraceLevel.INF,
						EnumTraceGroup.USER1,
						"Service::Main",
						"Registration failed");
				}   //	end if...else

				// no matter what close the application if
				// processCommandLine returned something different of S_OK				
				m_opcServer.Terminate();
				m_opcServer = null;
				return;
			}   //	end if
			System.ServiceProcess.ServiceBase.Run(ServicesToRun);
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			base.Dispose(disposing);
		}

		/// <summary>
		/// Set things in motion so your service can do its work.
		/// </summary>
		protected override void OnStart(string[] args)
		{
			//	start the OPC server's I/O internal mechanism 
			if (ResultCode.SUCCEEDED(m_opcServer.Start()))
			{
				//	build the namespace
				m_opcServer.BuildAddressSpace();

				//	declare the namespaces built and the server ready for clients to connect
				m_opcServer.Ready();

				StartSimulationThread();
			}   //	end if
		}   //	end Start

		/// <summary>
		/// Stop this service.
		/// </summary>
		protected override void OnStop()
		{

			try
			{
				//	terminate the simulation
				SimulationEnd = true;
				Mutex.WaitOne();
				Mutex.ReleaseMutex();
				m_opcServer.Stop();
				m_opcServer.Terminate();
				m_opcServer = null;
			}
			catch (Exception exc)
			{
				FileStream fs = new FileStream("C:\\Service_TraceData.txt", FileMode.OpenOrCreate, FileAccess.Write);
				StreamWriter streamWriter = new StreamWriter(fs);
				streamWriter.BaseStream.Seek(0, SeekOrigin.End);
				streamWriter.WriteLine();
				streamWriter.WriteLine(exc.Message + exc.Source + exc.StackTrace + exc.TargetSite);
				streamWriter.Flush();
				streamWriter.Close();
			}   //	end try...catch
		}   //	end Stop
	}
}
