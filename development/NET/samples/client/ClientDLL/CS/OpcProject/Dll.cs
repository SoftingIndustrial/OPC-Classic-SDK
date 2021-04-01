using System;
using System.Threading;
using System.IO;
using Softing.OPCToolbox;

namespace TestDll
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	public class Dll
	{
		#region Private Attributes
		//-------------------------
		private bool m_simulationEnd = false;
		private static Mutex m_mutex = new Mutex();
		private static OpcClient m_opcClient = null;
		
		//--
		#endregion

		#region Private Methods
		//----------------------

		private void Simulate()
		{
			FileStream fileStream =  new FileStream("ClientDll.txt", FileMode.Create, FileAccess.Write);
			StreamWriter streamWriter = new StreamWriter(fileStream);
			streamWriter.BaseStream.Seek(0, SeekOrigin.End);
			// Wait until it is safe to enter.
			m_mutex.WaitOne();

			while (!m_simulationEnd)
			{
				//Simulate the DA Client read operation
				string readValue = m_opcClient.ReadItem();
				
				//Log the read value to the file
				streamWriter.WriteLine(readValue);
				//Simulate the AE Client condition state request
				string state = m_opcClient.GetConditionState();
				
				//Log the condition state to the file
				streamWriter.WriteLine(state);
				Thread.Sleep(1000);
			}	//	end while
			streamWriter.Flush();
			streamWriter.Close();
			// Release the Mutex
			m_mutex.ReleaseMutex();

		}	//	end Simulate

		private void StartSimulationThread()
		{
			Thread simulationThread = new Thread(new ThreadStart(Simulate));
			simulationThread.Start();			
		}	//	end StartSimulationThread		
		//--
		#endregion

		#region Public Methods
		//--------------------

		public int Start()
		{
			try{
				int result = (int)EnumResultCode.S_OK;
				int registrationResult = (int)EnumResultCode.S_OK;
				// create the OpcClient instance
				m_opcClient = new OpcClient();
				//	TODO - binary license activation
				//	Fill in your binary license activation keys here
				//
				//	NOTE: you can activate one or all of the features at the same time
				//	firstly activate the COM-DA Client feature
				// result = m_opcClient.GetApplication().Activate(EnumFeature.DA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX");
				if (!ResultCode.SUCCEEDED(result))
				{
					return result;
				}

				//	activate the COM-AE Client Feature
				// result = m_opcClient.GetApplication().Activate(EnumFeature.AE_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX");
				if (!ResultCode.SUCCEEDED(result))
				{
					return result;
				}

				//	activate the XML-DA Client Feature
				//	result = m_opcClient.GetApplication().Activate(EnumFeature.XMLDA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX");
				if (!ResultCode.SUCCEEDED(result))
				{
					return result;
				}
				//	END TODO - binary license activation

				//	initialize the client instance
				if (!ResultCode.SUCCEEDED(m_opcClient.Initialize()))
				{			
					m_opcClient = null;	
					return (int)EnumResultCode.E_FAIL;
				}	//	end if
				//	initialize the DA client simulation
				registrationResult |= m_opcClient.InitializeDaObjects();
				//	initialize the AE client simulation
				registrationResult |= m_opcClient.InitializeAeObjects();
				//	start the simulation thread 
				if (ResultCode.SUCCEEDED(registrationResult))
				{
					StartSimulationThread();
				}	//	end if	
				return registrationResult;
			}
			catch(Exception exc)
			{				
				Console.WriteLine(exc.ToString());
                return (int)EnumResultCode.E_FAIL;
			}	//	end try...catch
		}	//	end Start

		public void Stop()
		{
			//	terminate the simulation
			m_simulationEnd = true;
			m_mutex.WaitOne();
			m_mutex.ReleaseMutex();
			m_opcClient.Terminate();						
			m_opcClient = null;
		}	//	end Stop

		//--
		#endregion

	}	//	end class Dll
}	//	end namespace
