using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace AEQuerySourceConditions
{
	public class OpcClient
	{

		#region Constructor
		//-----------------

		public OpcClient() { }

		//--
		#endregion

		#region Private Members
		//---------------------

		#region Private Attributes
		//------------------------

		private MyAeSession m_aeSession = null;
		private ExecutionOptions m_executionOptions;
		private string m_sourcePath;
		private string[] m_conditionNames;
		//--
		#endregion

		#region Private Methods
		//---------------------


		//--
		#endregion

		//--
		#endregion

		#region Public Methods
		//---------------------

		public Application GetApplication()
		{
			return Application.Instance;
		}   //	end GetApplication

		public int Initialize()
		{

			int result = (int)EnumResultCode.S_OK;
			GetApplication().VersionOtb = 447;
			//	TODO - binary license activation
			//	Fill in your binary license activation keys here
			//
			//	NOTE: you can activate one or all of the features at the same time

			//	activate the COM-AE Client Feature
			//	result = Application.Instance.Activate(EnumFeature.AE_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX");
			if (!ResultCode.SUCCEEDED(result))
			{
				return result;
			}
			//	END TODO - binary license activation

			//	proceed with the OPC Toolkit core initialization
			result = GetApplication().Initialize();

			if (ResultCode.SUCCEEDED(result))
			{
				//	enable toolkit internal initialization
				GetApplication().EnableTracing(
					EnumTraceGroup.ALL,
					EnumTraceGroup.ALL,
					EnumTraceGroup.CLIENT,
					EnumTraceGroup.CLIENT,
					"Trace.txt",
					1000000,
					0);
			}   //	end if
			return result;
		}   //	end Initialize

		public int ProcessCommandLine(string commandLine)
		{
			//	forward the command line arguments to the OPC Toolkit core internals
			return Application.Instance.ProcessCommandLine(commandLine);
		}   //	end ProcessCommandLine


		public void Terminate()
		{
			if (m_aeSession.CurrentState != EnumObjectState.DISCONNECTED)
			{
				m_aeSession.Disconnect(m_executionOptions);
			}
			GetApplication().RemoveAeSession(m_aeSession);

			GetApplication().Terminate();
			m_aeSession = null;
			m_executionOptions = null;
		}   //	end Terminate


		public int InitializeAeObjects()
		{
			int connectResult = (int)EnumResultCode.E_FAIL;
			m_executionOptions = new ExecutionOptions();
			m_executionOptions.ExecutionContext = 0;
			m_sourcePath = "computer.clock.time slot 1";

			try
			{

				m_aeSession = new MyAeSession("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}");

				connectResult = m_aeSession.Connect(
					true,
					false,
					m_executionOptions);

			}
			catch (Exception exc)
			{
				GetApplication().Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER,
					"OpcClient::InitializeAeObjects",
					exc.ToString());
			}   //	end try...catch

			return connectResult;
		}   //	end InitializeAeObjects

		public void Trace(
			EnumTraceLevel traceLevel,
			EnumTraceGroup traceGroup,
			string objectID,
			string message)
		{
			Application.Instance.Trace(
				traceLevel,
				traceGroup,
				objectID,
				message);
		}   //	end Trace

		public void QueryConditionsAsync()
		{
			int operationResult;
			m_executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;

			operationResult = m_aeSession.QueryAeSourceConditions(
				m_sourcePath,
				out m_conditionNames,
				m_executionOptions);

			m_executionOptions.ExecutionContext++;
		}// end QueryConditionsAsync

		public void QueryConditionsSync()
		{
			int operationResult;
			m_executionOptions.ExecutionType = EnumExecutionType.SYNCHRONOUS;

			operationResult = m_aeSession.QueryAeSourceConditions(
				m_sourcePath,
				out m_conditionNames,
				m_executionOptions);

			if (ResultCode.SUCCEEDED(operationResult))
			{

				System.Console.WriteLine("\n Source conditions of " + m_sourcePath + " :");
				for (int i = 0; i < m_conditionNames.Length; i++)
				{
					System.Console.WriteLine("	[" + i + "] " + m_conditionNames[i]);
				}
			} //end if
			else
			{

				System.Console.WriteLine("Failed to synchronously query the conditions of source: " + m_sourcePath);
			}
		}// end QueryConditionsSync

		//--
		#endregion

		#region Public Properties
		//-----------------------

		public string ServiceName
		{
			get
			{
				return Application.Instance.ServiceName;
			}
			set
			{
				Application.Instance.ServiceName = value;
			}
		}   //	end SetServiceName

		//--
		#endregion

	}   //	end class OpcClient

}   //	end namespace
