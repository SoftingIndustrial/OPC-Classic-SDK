using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace AEObjectAttributes
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
		private MyAeSubscription m_aeSubscription = null;
		private ExecutionOptions m_executionOptions = null;
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
			if (m_aeSubscription.CurrentState != EnumObjectState.DISCONNECTED)
			{
				m_aeSubscription.Disconnect(m_executionOptions);
			}   // end if

			if (m_aeSession.CurrentState != EnumObjectState.DISCONNECTED)
			{
				m_aeSession.Disconnect(m_executionOptions);
			}   // end if

			m_aeSession.RemoveAeSubscription(m_aeSubscription);
			GetApplication().RemoveAeSession(m_aeSession);

			GetApplication().Terminate();
			m_aeSession = null;
			m_aeSubscription = null;
			m_executionOptions = null;
		}   //	end Terminate


		public int InitializeAeObjects()
		{
			int connectResult = (int)EnumResultCode.E_FAIL;
			m_executionOptions = new ExecutionOptions();
			m_executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS;
			m_executionOptions.ExecutionContext = 0;

			try
			{

				m_aeSession = new MyAeSession("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}");
				m_aeSubscription = new MyAeSubscription(m_aeSession);

				connectResult = m_aeSession.Connect(true, true, new ExecutionOptions());
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

		public void PrintAeSession()
		{

			System.Console.WriteLine("Session Attributes:");
			System.Console.WriteLine("	URL: " + m_aeSession.Url);
			System.Console.WriteLine("	ClientName: " + m_aeSession.ClientName);
			System.Console.WriteLine("	LocaleID: " + m_aeSession.LocaleId);
		} // end PrintAeSession

		public void PrintAeSubscription()
		{

			System.Console.WriteLine("Subscription Attributes:");
			System.Console.WriteLine("	RequestedBufferTime: " + m_aeSubscription.RequestedBufferTime);
			System.Console.WriteLine("	RevisedBufferTime: " + m_aeSubscription.RevisedBufferTime);

			System.Console.WriteLine("	RequestedMaxSize: " + m_aeSubscription.RequestedMaxSize + " events");
			System.Console.WriteLine("	RevisedMaxSize: " + m_aeSubscription.RevisedMaxSize + " events");

			System.Console.WriteLine("	Filter: ");
			System.Console.WriteLine("		LowSeverity: " + m_aeSubscription.FilterSeverityLow);
			System.Console.WriteLine("		HighSeverity: " + m_aeSubscription.FilterSeverityHigh);
			System.Console.WriteLine("		EventTypes: " + m_aeSubscription.FilterEventTypes);

			uint[] categories = m_aeSubscription.FilterCategories;
			System.Console.WriteLine("	Categories: count: " + categories.Length);
			for (int i = 0; i < categories.Length; i++)
			{
				System.Console.WriteLine("	 CategoryID: " + categories[i]);
			}

			string[] areas = m_aeSubscription.FilterAreas;
			System.Console.WriteLine("	Areas: count: " + areas.Length);
			for (int i = 0; i < areas.Length; i++)
			{
				System.Console.WriteLine("	 Area: " + areas[i]);
			}

			string[] sources = m_aeSubscription.FilterSources;
			System.Console.WriteLine("	Sources: count: " + sources.Length);
			for (int i = 0; i < sources.Length; i++)
			{
				System.Console.WriteLine("	 Area: " + sources[i]);
			}

			AeReturnedAttributes[] attributes = m_aeSubscription.ReturnedAttributes;

			System.Console.WriteLine("        Returned Attributes: count: " + attributes.Length);
			for (int i = 0; i < attributes.Length; i++)
			{
				if (attributes[i].AttributeIds != null)
				{
					System.Console.WriteLine("	    Attributes IDs: count: " + attributes[i].AttributeIds.Length);
					for (int j = 0; j < attributes[i].AttributeIds.Length; j++)
					{
						System.Console.WriteLine("	    Attribute ID: " + attributes[i].AttributeIds[j]);
					}//end for
				}//end if
			}//end for
		} //end PrintAeSubscription

		public int ChangeSession()
		{
			int[] updateResults = new int[1];
			int operationResult;
			//GetApplication().UpdateAttributesBehavior = EnumUpdateAttributesBehavior.EXPLICIT_UPDATE;																																				

			m_aeSession.ClientName = "AE ObjectAttributes Sample";
			m_aeSession.LocaleId = "de-DE";

			// write changed data to the server 
			operationResult = m_aeSession.SetAttributesToServer(
				new EnumObjectAttribute[] { EnumObjectAttribute.AESESSION_CLIENTNAME, EnumObjectAttribute.AESESSION_LCID },
				out updateResults,
				new ExecutionOptions());

			return operationResult;
		}// end ChangeSession

		public int CheckSession()
		{
			int[] updateResults = new int[1];
			int operationResult;

			// get it from the server to check 
			operationResult = m_aeSession.GetAttributesFromServer(
				new EnumObjectAttribute[] { EnumObjectAttribute.AESESSION_ALL },
				out updateResults,
				new ExecutionOptions());

			return operationResult;
		}// end CheckSession

		public int ChangeSubscription()
		{
			int[] updateResults = new int[1];
			int operationResult;

			m_aeSubscription.RequestedBufferTime = 2000;
			m_aeSubscription.RequestedMaxSize = 5;
			m_aeSubscription.FilterSeverityLow = 100;
			m_aeSubscription.FilterSeverityHigh = 700;
			m_aeSubscription.FilterEventTypes = (uint)(EnumEventType.SIMPLE | EnumEventType.TRACKING);
			m_aeSubscription.FilterCategories = new uint[] { 1, 2, 3 };
			m_aeSubscription.FilterAreas = new string[] { "computer.mouse", "computer.clock" };
			m_aeSubscription.FilterSources = new string[] { "computer.clock.time slot 2", "computer.clock.time slot 1" };

			// write changed data to the server 		
			operationResult = m_aeSubscription.SetAttributesToServer(
				new EnumObjectAttribute[] { EnumObjectAttribute.AESUBSCRIPTION_FILTER, EnumObjectAttribute.AESUBSCRIPTION_MAXSIZE, EnumObjectAttribute.AESUBSCRIPTION_BUFFERTIME },
				out updateResults,
				new ExecutionOptions());

			return operationResult;
		}// end ChangeSubscription

		public int CheckSubscription()
		{
			int[] updateResults = new int[1];
			int operationResult;

			// get it from the server to check 
			operationResult = m_aeSubscription.GetAttributesFromServer(
				new EnumObjectAttribute[] { EnumObjectAttribute.AESUBSCRIPTION_FILTER, EnumObjectAttribute.AESUBSCRIPTION_MAXSIZE, EnumObjectAttribute.AESUBSCRIPTION_BUFFERTIME },
				out updateResults,
				new ExecutionOptions());

			return operationResult;
		}// end CheckSubscription

		public int ChangeSubscriptionAttributes()
		{
			int[] updateResults = new int[1];
			int operationResult;

			AeReturnedAttributes attrs1 = new AeReturnedAttributes();
			attrs1.CategoryId = 2;
			attrs1.AttributeIds = new uint[] { 3, 4 };
			AeReturnedAttributes attrs2 = new AeReturnedAttributes();
			attrs2.CategoryId = 3;
			attrs2.AttributeIds = new uint[] { 6 };
			m_aeSubscription.ReturnedAttributes = new AeReturnedAttributes[] { attrs1, attrs2 };

			// write changed data to the server 			
			operationResult = m_aeSubscription.SetAttributesToServer(
				new EnumObjectAttribute[] { EnumObjectAttribute.AESUBSCRIPTION_RETURNED_ATTRIBUTES },
				out updateResults,
				new ExecutionOptions());

			return operationResult;
		}// end ChangeSubscriptionAttributes


		public int CheckSubscriptionAttributes()
		{
			int[] updateResults = new int[1];
			int operationResult;

			operationResult = m_aeSubscription.GetAttributesFromServer(
				new EnumObjectAttribute[] { EnumObjectAttribute.AESUBSCRIPTION_RETURNED_ATTRIBUTES },
				out updateResults,
				new ExecutionOptions());

			return operationResult;
		} // end CheckSubscriptionAttributes


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
