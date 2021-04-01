using System;
using System.IO;
using Softing.OPCToolbox;

namespace DaConsole_Service
{
	public class OpcService : System.ServiceProcess.ServiceBase
	{

		#region Private Attributes
		//-------------------------

		private static OpcClient m_opcClient = null;
		// The following constant holds the name and the description of the Windows NT service
		// the OPC application runs under.
		//    TODO : change your service name and description here
		private const string defaultServiceName = "DaConsole_Service OpcService";
		private const string defaultServiceDescription = "Softing OPC Toolkit - OPC DA Client. \r\n\r\nSample name: \"C# DaConsole_Service\".";

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
		//--------------------

		public void CreateOpcClient()
		{
			if (m_opcClient == null)
			{
				m_opcClient = new OpcClient();
			}   //	end if
		}   //	end CreateOpcClient
			//--
		#endregion

		public OpcService()
		{
		}

		// The main entry point for the process
		static void Main()
		{
			int result = (int)EnumResultCode.S_OK;
			bool commandLineProcessed = false;
			string commandline = Environment.CommandLine;

			System.ServiceProcess.ServiceBase[] ServicesToRun;
			OpcService opcService = new OpcService();
			ServicesToRun = new System.ServiceProcess.ServiceBase[] { opcService };

			opcService.CreateOpcClient();
			m_opcClient.ServiceName = defaultServiceName;
			m_opcClient.ServiceDescription = defaultServiceDescription;
			//	initialize the client instance
			if (!ResultCode.SUCCEEDED(m_opcClient.Initialize()))
			{
				m_opcClient = null;
				return;
			}   //	end if

			if (!commandLineProcessed)
			{
				result = m_opcClient.ProcessCommandLine(commandline);
				commandLineProcessed = true;
				if (result != (int)EnumResultCode.S_OK)
				{
					if (result == (int)EnumResultCode.S_FALSE)
					{
						//registration operation succesful
						m_opcClient.Trace(
							EnumTraceLevel.INF,
							EnumTraceGroup.USER1,
							"Service::Main",
							"Registration succeeded");
					}
					else
					{
						m_opcClient.Trace(
							EnumTraceLevel.INF,
							EnumTraceGroup.USER1,
							"Service::Main",
							"Registration failed");
					}   //	end if...else

					//	no matter what close the application if
					//processCommandLine returned something different of S_OK				
					m_opcClient.Terminate();
					m_opcClient = null;

					return;
				}   //	end if
			}
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
			int result = (int)EnumResultCode.S_OK;
			//	initialize the DA client simulation
			result |= m_opcClient.InitializeDaObjects();

		}   //	end Start

		/// <summary>
		/// Stop this service.
		/// </summary>
		protected override void OnStop()
		{

			try
			{
				m_opcClient.Terminate();
				m_opcClient = null;
			}
			catch (Exception exc)
			{
				FileStream fs = new FileStream("C:\\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write);
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
