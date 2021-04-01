using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace DAGetProperties
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

		private MyDaSession m_daSession = null;
		ExecutionOptions m_executionOptions;
		string m_addressSpaceElementId;
		string m_addressSpaceElementPath;
		int m_operationResult;

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

			//	activate the COM-DA Client feature
			//	result = Application.Instance.Activate(EnumFeature.DA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX");
			if (!ResultCode.SUCCEEDED(result))
			{
				return result;
			}

			//	activate the XML-DA Client Feature
			//	result = Application.Instance.Activate(EnumFeature.XMLDA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX");
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
			GetApplication().Terminate();
			m_daSession = null;
		}   //	end Terminate


		public int InitializeDaObjects()
		{
			int connectResult = (int)EnumResultCode.E_FAIL;

			m_addressSpaceElementId = "increment.I1";
			m_addressSpaceElementPath = string.Empty;
			m_executionOptions = new ExecutionOptions();
			m_executionOptions.ExecutionContext = 0;


			try
			{

				//	TODO add your server URL here
				//	this is the server url for Softing OPCToolbox Demo Server
				//	first choice is COM-DA 
				string url = "opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}";

				//uncomment if you need an XML-DA access
				//	string url = "http://localhost:8079/OPC/DA";

				m_daSession = new MyDaSession(url);

				connectResult = m_daSession.Connect(true, false, null);
			}
			catch (Exception exc)
			{
				GetApplication().Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER,
					"OpcClient::InitializaDaObjects",
					exc.ToString());
			}   //	end try...catch

			return connectResult;
		}   //	end InitializeDaObjects


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

		public void GetPropertiesAsync()
		{
			m_executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;

			DaProperty[] properties = null;
			DaGetPropertiesOptions getAllPropertiesOptions = new DaGetPropertiesOptions();
			getAllPropertiesOptions.WhatPropertyData = EnumPropertyData.ALL;

			System.Console.WriteLine("Get all properties of address space element: " + m_addressSpaceElementId);

			m_operationResult = m_daSession.GetDaProperties(
				m_addressSpaceElementId,
				m_addressSpaceElementPath,
				getAllPropertiesOptions,
				out properties,
				m_executionOptions);

			m_executionOptions.ExecutionContext++;

			System.Console.WriteLine("Get properties 1 and 5 of address space element: " + m_addressSpaceElementId);

			DaProperty[] someProperties = null;
			DaGetPropertiesOptions getPropertiesOptions = new DaGetPropertiesOptions();
			getPropertiesOptions.WhatPropertyData = EnumPropertyData.ALL;
			getPropertiesOptions.PropertyIds = new int[] { 1, 5 };

			m_operationResult = m_daSession.GetDaProperties(
				m_addressSpaceElementId,
				m_addressSpaceElementPath,
				getPropertiesOptions,
				out someProperties,
				m_executionOptions);

			m_executionOptions.ExecutionContext++;
		}// end GetPropertiesAsync

		public void GetPropertiesSync()
		{
			m_executionOptions.ExecutionType = EnumExecutionType.SYNCHRONOUS;
			DaProperty[] properties = null;
			DaGetPropertiesOptions getAllPropertiesOptions = new DaGetPropertiesOptions();
			getAllPropertiesOptions.WhatPropertyData = EnumPropertyData.ALL;

			System.Console.WriteLine("Get all properties of address space element: " + m_addressSpaceElementId);
			m_operationResult = m_daSession.GetDaProperties(
				m_addressSpaceElementId,
				m_addressSpaceElementPath,
				getAllPropertiesOptions,
				out properties,
				m_executionOptions);

			if (ResultCode.SUCCEEDED(m_operationResult))
			{

				System.Console.WriteLine("Get properties of address space element:  " + m_addressSpaceElementId);
				for (int i = 0; i < properties.Length; i++)
				{

					System.Console.WriteLine("	Property Name: " + properties[i].Name);
					System.Console.WriteLine("	Property Id: " + properties[i].Id);
					System.Console.WriteLine("	Property Item Id: " + properties[i].ItemId);
					System.Console.WriteLine("	Property DataType: " + properties[i].DataType);
					System.Console.WriteLine("	Property description: " + properties[i].Description);
					System.Console.WriteLine("	Property value: " + properties[i].ValueQT.Data.ToString() + "	");
					System.Console.WriteLine(" ");

				}
			} //end if
			else
			{

				System.Console.WriteLine("Failed to synchronously get properties of address space element: " + m_addressSpaceElementId + " - 0x" + m_operationResult.ToString("X8"));
			}

			System.Console.WriteLine("Get properties 1 and 5 of address space element: " + m_addressSpaceElementId);
			DaProperty[] someProperties = null;
			DaGetPropertiesOptions getPropertiesOptions = new DaGetPropertiesOptions();
			getPropertiesOptions.PropertyIds = new int[] { 1, 5 };
			getPropertiesOptions.WhatPropertyData = EnumPropertyData.ALL;

			m_operationResult = m_daSession.GetDaProperties(
				m_addressSpaceElementId,
				m_addressSpaceElementPath,
				getPropertiesOptions,
				out someProperties,
				m_executionOptions);

			if (ResultCode.SUCCEEDED(m_operationResult))
			{

				for (int i = 0; i < someProperties.Length; i++)
				{

					System.Console.WriteLine("	Property Name: " + someProperties[i].Name);
					System.Console.WriteLine("	Property Id: " + someProperties[i].Id);
					System.Console.WriteLine("	Property Item Id: " + someProperties[i].ItemId);
					System.Console.WriteLine("	Property DataType: " + someProperties[i].DataType);
					System.Console.WriteLine("	Property description: " + someProperties[i].Description);
					System.Console.WriteLine("	Property value: " + someProperties[i].ValueQT.Data.ToString() + "	");
					System.Console.WriteLine(" ");

				}
			}
			else
			{
				System.Console.WriteLine("Failed to synchronously get 1 and 5 properties of address space element: " + m_addressSpaceElementId + " - 0x" + m_operationResult.ToString("X8"));
			}
		}// end GetPropertiesSync

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
